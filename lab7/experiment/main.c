#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <mpi.h>

double clock_function(int start, int end);
int find_primes(int start, int end);
int check_prime(int N);

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Usage: %s <start> <end>\n", argv[0]);
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);

    double time = clock_function(start, end);

    printf("%d %lf\n", rank, time);

    MPI_Finalize();

    return 0;
}

double clock_function(int start, int end) {
    int primes = 0;

    double start_time = MPI_Wtime();
    primes = find_primes(start, end);
    double end_time = MPI_Wtime();
/*
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* results = NULL;
    if (rank == 0) {
        results = (int *) malloc(size * sizeof(int));
    }

    MPI_Gather(&number_of_local_primes, 1, MPI_INT, results, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            number_of_primes += results[i];
        }

        free(results);
    }
*/
    return end_time - start_time;
}

int find_primes(int start, int end) {
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number_of_local_primes = 0;

    int chunk = (end - start + 1) / size;
    int remain = (end - start + 1) % size;

    int local_start = start + rank * chunk;
    int local_end = local_start + chunk - 1;

    if (rank < remain) {
        local_start += rank;
        local_end += 1;
    } else {
        local_start += remain;
    }

    int max_threads = omp_get_max_threads();
    int threads = (size > max_threads) ? max_threads / size : 2;
    omp_set_num_threads(threads);

#pragma omp parallel for reduction (+:number_of_local_primes)
    for (int n = local_start; n <= local_end; n++) {
        number_of_local_primes += check_prime(n);
    }

    return number_of_local_primes;
}

int check_prime(int N) {
    int is_prime = 1;
    int max = (int) sqrt(N);

    if (N < 2) {
        return 0;
    }

#pragma omp parallel shared(is_prime, max)
    {
#pragma omp for
        for (int i = 2; i <= max; ++i) {
            if (N % i == 0) is_prime = 0;
            if (is_prime == 0) i = max + 1;
        }
    }

    return is_prime;
}