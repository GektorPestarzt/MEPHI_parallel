#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "mpi.h"

#include "experiment_t.h"
#include "init_array.h"
#include "stat_files.h"
#include "modes.h"

#define NUM_ATTEMPTS 100
#define ARRAY_SIZE 1000000

#define RANDOM_SEED 2315345

typedef struct chunk {
    MPI_Comm comm;
    int size;
    int *start;
    int num_extras;
    MPI_Comm comm_extra;
} Chunk;

experiment_t *menu_mode(int size, int num_attempts, int mode);

double clock_mode(int mode);
double clock_function(const experiment_t *expreriment);

void shell_sort_1(int *array, int size);
void shell_sort_2(int *array, int array_size);
void init_chunk(Chunk *chunk, int i, int queue_size, int array_size, int *array, int size, int *all_num_extras, int gap);
MPI_Comm init_comm(int chunk_size, int size);
void insertion_sort(int *array, int array_size);
void shell_sort_linear(int *array, int size);

int div_up(int x, int y);
int min(int x, int y);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    bool error = false;
    srand(RANDOM_SEED);

    for (int mode = 0; mode < 1 /*numberOfModes*/; ++mode) {

        double time_result = 0;
        for (int attemp = 0; attemp < NUM_ATTEMPTS; ++attemp) {
            double time = clock_mode(mode);
            time_result += clock_mode(mode) / NUM_ATTEMPTS;
        }

        if (rank == 0) {
            printf("%lf\n", time_result);
        }
    }

    MPI_Finalize();
    return 0;
}

experiment_t *menu_mode(int size, int num_attempts, int mode) {
    experiment_t *experiment = NULL;
    int *array = NULL;
    int *sorted_array = NULL;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        switch (mode)
        {
        case modeRandom:
            array = init_random_array(size, size);
            break;

        case modeSorted:
            array = init_sorted_array(size);
            break;

        case modeReversed:
            array = init_reversed_array(size);
            break;

        case modeRange1:
            array = init_random_array(size, 1);
            break;

        case modeRange100:
            array = init_random_array(size, 100);
            break;

        case modeRange10000:
            array = init_random_array(size, 10000);
            break;

        case modeMixed:
            array = init_mixed_array(size);
            break;
        }

        sorted_array = init_copy_array(array, size);
        // shell_sort_linear(sorted_array, size);
    }

    experiment = init_experiment(array, sorted_array, size, num_attempts);
    return experiment;
}

double clock_mode(int mode) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    experiment_t *experiment = menu_mode(ARRAY_SIZE, NUM_ATTEMPTS, mode);

    double time = clock_function(experiment);

    free_experiment(experiment);

    if (isnan(time)) {
        return NAN;
    }

    return time;
}

double clock_function(const experiment_t *experiment) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *array = NULL;
    if (rank == 0) {
        array = init_copy_array(experiment->array, experiment->size);
    }

    double start = MPI_Wtime();

    shell_sort_2(array, experiment->size);

    double end = MPI_Wtime();

    if (rank == 0) {/*
        for (int i = 0; i < experiment->size; ++i) {
            // printf("%d %d\n", array[i], experiment->sorted_array[i]);
            if (experiment->sorted_array[i] != array[i]) {
                free(array);
                return NAN;
            }
        }
        // printf("----------\n");
*/
        free(array);
    }

    return end - start;
}

void shell_sort_2(int *array, int array_size) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int chunk = array_size / size;
    int *larray = malloc(chunk * sizeof(int));

    MPI_Scatter(array, chunk, MPI_INT, larray, chunk, MPI_INT, 0, MPI_COMM_WORLD);

    shell_sort_linear(larray, chunk);
    
    MPI_Gather(larray, chunk, MPI_INT, array, chunk, MPI_INT, 0, MPI_COMM_WORLD);
    free(larray);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        shell_sort_linear(array, array_size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void shell_sort_1(int *array, int array_size) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
/*
    for (int j = 0; j < array_size; ++j) {
        printf("%d ", array[j]);
    }
    printf("\n-------------------\n");
*/
    for (int gap = array_size / 2; gap > 0; gap /= 2) {
        int queue_size = div_up(gap, size);
        int all_num_extras = array_size % gap;

        for (int i = 0; i < queue_size; ++i) {
            Chunk chunk;
            init_chunk(&chunk, i, queue_size, array_size, array, size, &all_num_extras, gap);

            // printf("\nchunk_size = %d\n", chunk.size);

            if (rank < chunk.size) {
                int receive_array_size = rank < chunk.num_extras ? (array_size / gap + 1) : (array_size / gap);
                int *receive_array = malloc(receive_array_size * sizeof(int));

                for (int k = 0; k < array_size / gap; ++k) {
                    MPI_Scatter(chunk.start + gap * k, 1, MPI_INT, receive_array + k, 1, MPI_INT, 0, chunk.comm);
                }
                if (rank < chunk.num_extras) {
                    MPI_Scatter(chunk.start + gap * (receive_array_size - 1), 1, MPI_INT, receive_array + receive_array_size - 1, 1, MPI_INT, 0, chunk.comm_extra);
                }
/*
                printf("r ");
                for (int j = 0; j < receive_array_size; ++j) {
                    printf("%d ", receive_array[j]);
                }
                printf("\n");
*/
                shell_sort_linear(receive_array, receive_array_size);
                // insertion_sort(receive_array, receive_array_size);
                
/*
                printf("b ");
                for (int j = 0; j < receive_array_size; ++j) {
                    printf("%d ", receive_array[j]);
                }
                printf("\n");
*/
                for (int k = 0; k < array_size / gap; ++k) {
                    MPI_Gather(receive_array + k, 1, MPI_INT, chunk.start + gap * k, 1, MPI_INT, 0, chunk.comm);
                }
                if (rank < chunk.num_extras) {
                    MPI_Gather(receive_array + receive_array_size - 1, 1, MPI_INT, chunk.start + gap * (receive_array_size - 1), 1, MPI_INT, 0, chunk.comm_extra);
                }

                free(receive_array);
                if (chunk.comm != MPI_COMM_WORLD && chunk.comm != MPI_COMM_NULL) {
                    MPI_Comm_free(&chunk.comm);
                }
                if (chunk.comm_extra != MPI_COMM_NULL && chunk.comm_extra != MPI_COMM_WORLD) {
                    MPI_Comm_free(&chunk.comm_extra);
                }
/*
                for (int j = 0; j < array_size; ++j) {
                    printf("%d ", array[j]);
                }
                printf("\n\n");*/
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
}

void init_chunk(Chunk *chunk, int i, int queue_size, int array_size, int *array, int size, int *all_num_extras, int gap) {
    if (i == queue_size - 1 && gap % size != 0) {
        chunk->size = gap % size;
    } else {
        chunk->size = min(size, gap);
    }

    chunk->comm = init_comm(chunk->size, size);
    chunk->start = array + chunk->size * i;

    chunk->num_extras = min(*all_num_extras, chunk->size);
    *all_num_extras -= chunk->num_extras;
    chunk->comm_extra = init_comm(chunk->num_extras, size);
}

MPI_Comm init_comm(int chunk_size, int size) {
    if (chunk_size == 0) {
        return MPI_COMM_NULL;
    } else if (chunk_size == size) {
        return MPI_COMM_WORLD;
    }

    MPI_Group  orig_group, new_group;
    MPI_Comm   new_comm;

    MPI_Comm_group(MPI_COMM_WORLD, &orig_group);

    int ranges[1][3] = {{0, chunk_size - 1, 1}};
    MPI_Group_range_incl(orig_group, 1, ranges, &new_group);
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);

    MPI_Group_free(&orig_group);
    MPI_Group_free(&new_group);
    return new_comm;
}

int div_up(int x, int y) {
    return (x - 1) / y + 1;
}

int min(int x, int y) {
    return x > y ? y : x;
}

void shell_sort_linear(int *array, int size) {
    for (int s = size / 2; s > 0; s /= 2) {
        for (int i = s; i < size; ++i) {
            for (int j = i - s; j >= 0 && array[j] > array[j + s]; j -= s) {
                int temp = array[j];
                array[j] = array[j + s];
                array[j + s] = temp;
            }
        }
    }
}


void insertion_sort(int *array, int array_size) {
    int key, j;

    for (int i = 1; i < array_size; ++i)  {
        key = array[i];
        j = i - 1;
 
        while (j >= 0 && array[j] > key) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = key;
    }
}