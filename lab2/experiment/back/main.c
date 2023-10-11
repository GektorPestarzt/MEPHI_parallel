#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <time.h>

#include "experiment_t.h"
#include "init_array.h"
#include "stat_files.h"
#include "modes.h"

#define NUM_ATTEMPTS 10
#define ARRAY_SIZE 10000000

#define RANDOM_SEED 2315342

#ifdef _OPENMP
    #define MAX_THREADS 10
#else
    #define MAX_THREADS 1
#endif // _OPENMP

void output(FILE *fd, float *avarage_time_result);
experiment_t *menu_mode(int size, int num_attempts, int mode);

double clock_experiment_average(const experiment_t *experiment);
double clock_function(const experiment_t *expreriment);
int find_array_target(const int *array, int size, int target, int num_threads);

int main(int argc, char *argv[]) {
#ifdef _OPENMP
    printf("OpenMP: %d\n\n", _OPENMP);
#else
    printf("OpenMP: OFF\n\n");
#endif  // _OPENMP

    bool error = false;
    srand(RANDOM_SEED);

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

        fprintf(files.avg, "%s,%s%d\n", "threads", "mode ", mode);
        fprintf(files.spu, "%s,%s%d\n", "threads", "mode ", mode);
        fprintf(files.eff, "%s,%s%d\n", "threads", "mode ", mode);

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

    case modeMiddle:
        array = init_sorted_array(size);
        target = size / 2;
        break;

    case modeLast:
        array = init_sorted_array(size);
        target = size - 1;
        break;

    case modeRandom:
        array = init_sorted_array(size);
        target = -1; // random target
        break;

    case modeAll:
        array = init_identical_array(size, 0);
        target = 0;
        break;

    case modeNone:
        array = init_sorted_array(size);
        target = -2; // no target
        break;
    }

    experiment = init_experiment(array, size, target, -1, num_attempts);
    return experiment;
}

double clock_experiment_average(const experiment_t *experiment) {
    double time = 0;
    for (int i = 0; i < experiment->num_attempts; ++i) {
        if (experiment)
        time += clock_function(experiment);
    }

    time /= experiment->num_attempts;
    return time;
}

double clock_function(const experiment_t *experiment) {
    int target = experiment->target;
    if (target == -1) {
        target = rand() % experiment->size;
    }

#ifdef _OPENMP
    double start = omp_get_wtime();
#else
    double start = clock();
#endif  //_OPENMP

    find_array_target(experiment->array, experiment->size, target, experiment->num_threads);

#ifdef _OPENMP
    double end = omp_get_wtime();
    float time = end - start;
#else
    double end = clock();
    double time = ((double)(end - start)) / CLOCKS_PER_SEC;
#endif  //_OPENMP

    return time;
}

int find_array_target(const int *array, int size, int target, int num_threads) {
    int index = -1;
    bool flag = false;

    #pragma omp parallel num_threads(num_threads) shared(array, target, index)
    {
        #pragma omp for
        for(int i = 0; i < size; ++i)
        {
#ifdef _OPENMP
            if (flag) continue;
#else
            if (flag) break;
#endif  // _OPENMP

            if (array[i] == target) {
                #pragma omp critical
                {
                    index = i;
                    flag = true;
                }
            }
        }
    }

    return index;
}