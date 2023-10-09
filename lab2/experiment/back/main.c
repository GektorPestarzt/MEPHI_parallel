#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <time.h>

#include "experiment_t.h"
#include "init_array.h"
#include "stat_files.h"
#include "modes.h"

#define NUM_ATTEMPTS 1000
#define ARRAY_SIZE 10000000

#ifdef _OPENMP
    #define MAX_THREADS 10
#else
    #define MAX_THREADS 1
#endif // _OPENMP

void output(FILE *fd, float *avarage_time_result);
experiment_t *menu_mode(int size, int num_attempts, int mode);

double clock_experiment_average(const experiment_t *experiment);
double clock_function(const experiment_t *expreriment);
int find_array_target(const experiment_t *experiment);

int main(int argc, char *argv[]) {
#ifdef _OPENMP
    printf("OpenMP: %d\n\n", _OPENMP);
#else
    printf("OpenMP: OFF\n\n");
#endif  // _OPENMP

    bool error = false;

    files_t files;
    error = read_files_path(&files, argc, argv);
    if (error) {
        printf("%s: error. Format: %s <stat file path>\n", argv[0], argv[0]);
        printf("Done.\n");
        return 1;
    }


    for (int mode = 0; mode < numberOfModes; ++mode) {
        bool error = open_files(&files, mode);
        if (error) {
            printf("can`t open files by path: %s\n", files.files_path);
            printf("Done.\n");
            return 1;
        }

        experiment_t *experiment;
        experiment = menu_mode(ARRAY_SIZE, NUM_ATTEMPTS, mode);

        double *time_result = (double *) malloc (MAX_THREADS * sizeof(double));
        for (int threads = 1; threads <= MAX_THREADS; ++threads) {
            experiment->num_threads = threads;
            time_result[threads - 1] = clock_experiment_average(experiment);

            fprintf(files.avg, "%d,%lf\n", threads, time_result[threads - 1]);
        }

        free_experiment(experiment);

        for (int threads = 1; threads <= MAX_THREADS; ++threads) {
            fprintf(files.spu, "%d,%lf\n", threads, time_result[0] / time_result[threads - 1]);
            fprintf(files.eff, "%d,%lf\n", threads, (time_result[0] / time_result[threads - 1]) / threads);
        }
        free(time_result);

        printf("mode %d: completed\n", mode);
        close_files(&files);
    }

    printf("Done.\n");
    return 0;
}

experiment_t *menu_mode(int size, int num_attempts, int mode) {
    experiment_t *experiment = NULL;
    int *array = NULL, target = 0;

    switch (mode)
    {
    case modeFirst:
        array = init_sorted_array(size);
        target = 0;
        break;

    case modeLast:
        array = init_sorted_array(size);
        target = size - 1;
        break;

    case modeMiddle:
        array = init_sorted_array(size);
        target = size / 2;
        break;

    case modeRandom:
        array = init_random_array(size);
        target = rand() % size;
        break;

    case modeAll:
        array = init_identical_array(size, 0);
        target = 0;
        break;

    case modeNone:
        array = init_sorted_array(size);
        target = -1;
        break;
    }

    experiment = init_experiment(array, size, target, -1, num_attempts);
    return experiment;
}

double clock_experiment_average(const experiment_t *experiment) {
    double time = 0;
    for (int i = 0; i < experiment->num_attempts; ++i) {
        time += clock_function(experiment);
    }

    time /= experiment->num_attempts;
    return time;
}

double clock_function(const experiment_t *expreriment) {

#ifdef _OPENMP
    double start = omp_get_wtime();
#else
    double start = clock();
#endif  //_OPENMP

    find_array_target(expreriment);

#ifdef _OPENMP
    double end = omp_get_wtime();
    float time = end - start;
#else
    double end = clock();
    double time = ((double)(end - start)) / CLOCKS_PER_SEC;
#endif  //_OPENMP

    return time;
}

int find_array_target(const experiment_t *experiment) {
    int index = -1;

    #pragma omp parallel num_threads(experiment->num_threads) shared(experiment, index)
    {
        #pragma omp for
        for(int i = 0; i < experiment->size; ++i)
        {
            if (index != -1) continue;

            #pragma omp critical
            if (experiment->array[i] == experiment->target) {
                index = i;
            }
        }
    }

    return index;
}