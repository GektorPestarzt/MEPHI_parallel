#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#include "experiment_t.h"
#include "init_array.h"
#include "stat_files.h"
#include "modes.h"

#define NUM_ATTEMPTS 100
#define ARRAY_SIZE 1000000

#define RANDOM_SEED 2315345

#ifdef _OPENMP
    #define MAX_THREADS 10
#else
    #define MAX_THREADS 1
#endif // _OPENMP

void output(FILE *fd, float *avarage_time_result);
experiment_t *menu_mode(int size, int num_attempts, int mode);

double *clock_mode(int mode);
double clock_function(const experiment_t *expreriment);
void shell_sort(int *array, int size, int num_threads);

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

        double *time_result = (double *) calloc (MAX_THREADS, sizeof(double));
        for (int attemp = 0; attemp < NUM_ATTEMPTS; ++attemp) {
            double *time_attemp = clock_mode(mode);
            for (int i = 0; i < MAX_THREADS; ++i) {
                time_result[i] += time_attemp[i] / NUM_ATTEMPTS;
            }
            free(time_attemp);
        }

        for (int threads = 1; threads <= MAX_THREADS; ++threads) {
            fprintf(files.avg, "%d,%lf\n", threads, time_result[threads - 1]);
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
    int *array = NULL;
    int *sorted_array = NULL;

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
    shell_sort(sorted_array, size, 1);

    experiment = init_experiment(array, sorted_array, size, -1, num_attempts);
    return experiment;
}

double *clock_mode(int mode) {
    experiment_t *experiment;
    experiment = menu_mode(ARRAY_SIZE, NUM_ATTEMPTS, mode);

    double *time = (double *) calloc(MAX_THREADS, sizeof(double));
    for (int threads = 1; threads <= MAX_THREADS; ++threads) {
        experiment->num_threads = threads;
        time[threads - 1] = clock_function(experiment);

        if (isnan(time[threads - 1])) {
            free_experiment(experiment);
            return NULL;
        }
    }

    free_experiment(experiment);
    return time;
}

double clock_function(const experiment_t *experiment) {
    int *array = init_copy_array(experiment->array, experiment->size);

#ifdef _OPENMP
    double start = omp_get_wtime();
#else
    double start = clock();
#endif  //_OPENMP

    shell_sort(array, experiment->size, experiment->num_threads);

#ifdef _OPENMP
    double end = omp_get_wtime();
    float time = end - start;
#else
    double end = clock();
    double time = ((double)(end - start)) / CLOCKS_PER_SEC;
#endif  //_OPENMP

    for (int i = 0; i < experiment->size; ++i) {
        if (experiment->sorted_array[i] != array[i]) {
            free(array);
            return NAN;
        }
    }

    free(array);
    return time;
}

void shell_sort(int *array, int size, int num_threads) {
    num_threads = num_threads;

    for (int s = size / 2; s > 0; s /= 2) {
        #pragma omp parallel for num_threads(num_threads) shared(array, size, s)
        for (int i = 0; i < s; i++) {
            for (int j = i + s; j < size; j += s) {
                int key = array[j];
                int k = j - s;
                while (k >= 0 && array[k] > key) {
                    array[k + s] = array[k];
                    k -= s;
                }
                array[k + s] = key;
            }
        }
    }
}

/*
{
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
*/