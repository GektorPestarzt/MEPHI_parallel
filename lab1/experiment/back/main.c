#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MAX_THREADS 10
#define NUMBER_OF_ATTEMPTS 50
#define ARRAY_SIZE 10000000

#define RANDOM_SEED 920215

#ifdef _OPENMP
double find_max_parallel(int *array, long array_size, int number_of_threads);
#endif  // _OPENMP

double find_max_linear(int *array, long array_size, int);

double *clocking_threads(int max_threads, int number_of_attempts, long array_size);

void output(FILE *fd, double *avarage_time_result, int thread_max, int number_of_attempts, long array_size);

int main(int argc, char *argv[]) {
    int max_threads = 1;
#ifdef _OPENMP
    max_threads = MAX_THREADS;
#endif // _OPENMP


    if (argc != 2) {
        printf("%s: error. Format: %s <log file name>\n", argv[0], argv[0]);
        printf("Done.\n");
        return 1;
    }

    double *avarage_time_result;
    avarage_time_result = clocking_threads(max_threads, NUMBER_OF_ATTEMPTS, ARRAY_SIZE);

    FILE *fd = fopen(argv[1], "w");
    if (fd == NULL) {
        printf("%s: error. Can`t open file \"%s\"\n", argv[0], argv[1]);
        printf("Done.\n");
        return 1;
    }

    output(fd, avarage_time_result, max_threads, NUMBER_OF_ATTEMPTS, ARRAY_SIZE);
    free(avarage_time_result);
    fclose(fd);

    printf("Done.\n");
    return 0;
}

double *check_different_threads(int max_threads, long array_size, double (*find_max)(int *, long, int));

double *clocking_threads(int max_threads, int number_of_attempts, long array_size) {
    double *avarage_time_result = (double *) calloc(max_threads, sizeof(double));

    for (int i = 0; i < number_of_attempts; ++i) {
        double *attempt;

#ifdef _OPENMP
        attempt = check_different_threads(max_threads, array_size, find_max_parallel);
#else 
        attempt = check_different_threads(max_threads, array_size, find_max_linear);
#endif  // _OPENMP

        for (int i = 0; i < max_threads; ++i) {
            avarage_time_result[i] += attempt[i];
        }

        free(attempt);
    }

    for (int i = 0; i < max_threads; ++i) {
        avarage_time_result[i] /= number_of_attempts;
    }

    return avarage_time_result;
}

void output(FILE *fd, double *avarage_time_result, int thread_number, int number_of_attempts, long array_size) {
    fprintf(fd, "=============================\n\n");
#ifdef _OPENMP
    /* Determine the OpenMP support */
    fprintf(fd, "OpenMP: %d\n\n", _OPENMP);
#else
    fprintf(fd, "OpenMP: off\n\n");
#endif  // _OPENMP

    fprintf(fd, "=============================\n");
    fprintf(fd, "\n");
    fprintf(fd, "Parametrs:\n");
#ifdef _OPENMP
    fprintf(fd, "Threads number: %d\n", thread_number);
#else
    thread_number += thread_number; // unused parametr
    fprintf(fd, "Threads number: NAN\n");
#endif  // _OPENMP
    fprintf(fd, "Number of attempts: %d\n", number_of_attempts);
    fprintf(fd, "Arrays size: %ld\n", array_size);
    fprintf(fd, "\n");

    fprintf(fd, "=============================\n");
    fprintf(fd, "\n");

#ifdef _OPENMP
    for (int i = 1; i <= thread_number; ++i) {
        fprintf(fd, "Thread #%2d: %lf sec\n", i, avarage_time_result[i - 1]);
    }
#else
    fprintf(fd, "First try: %lf sec\n", avarage_time_result[0]);
#endif  // _OPENMP
    fprintf(fd, "\n=============================\n");
}

double *check_different_threads(int max_threads, long array_size, double (*find_max)(int *, long, int)) {
    /* Initialize the RNG */
    srand(RANDOM_SEED);

    int *array = (int *) malloc(sizeof(int) * array_size);
    for (long i = 0; i < array_size; ++i) {
        array[i] = rand();
    }

    double *time_result = (double *) calloc(max_threads, sizeof(double));
    for (int threads = 1; threads <= max_threads; ++threads) {
        double time = find_max(array, array_size, threads);
        time_result[threads - 1] += time;
    }

    free(array);
    return time_result;
}

#ifdef _OPENMP
double find_max_parallel(int *array, long array_size, int number_of_threads) {
    int  max   = -1;

    double start = omp_get_wtime();
    #pragma omp parallel num_threads(number_of_threads) shared(array, array_size) reduction(max: max) default(none)
    {
        #pragma omp for
        for (long i = 0; i < array_size; ++i)
        {
            if (array[i] > max) { max = array[i]; };
        }
    }

    #pragma omp barrier
    double end = omp_get_wtime();

    return end - start;
}
#endif  // _OPENMP

double find_max_linear(int *array, long array_size, int number_of_threads) {
    number_of_threads += number_of_threads; // unused parametr

    int  max   = -1;
    clock_t start, end;

    start = clock();
    for (long i = 0; i < array_size; ++i)
    {
        if (array[i] > max) { max = array[i]; };
    }
    end = clock();

    double execution_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    return execution_time;
}