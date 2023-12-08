#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mpi.h"

#define NUMBER_OF_ATTEMPTS 100
#define ARRAY_SIZE 10000000

#define RANDOM_SEED 920215

int find_max(int *array, long array_size);
double clocking(int *array, long array_size);
double clocking_avarage(long array_size, int number_of_attempts);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    double avarage_time_result;
    avarage_time_result = clocking_avarage(ARRAY_SIZE, NUMBER_OF_ATTEMPTS);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        printf("%lf - result\n", avarage_time_result);
    }

    MPI_Finalize();
    return 0;
}

double clocking_avarage(long array_size, int number_of_attempts) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Initialize the RNG */
    srand(RANDOM_SEED);

    double time_result = 0;
    for (int i = 0; i < number_of_attempts; ++i) {
        int *array = (int *) malloc(sizeof(int) * array_size);

        if (rank == 0) {
            for (long i = 0; i < array_size; ++i) {
                array[i] = rand();
            }
        }
        MPI_Bcast(array, ARRAY_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

        time_result += clocking(array, array_size);
        free(array);
    }

    return time_result / number_of_attempts;
}

double clocking(int *array, long array_size) {
    double start = MPI_Wtime();

    find_max(array, array_size);

    double end = MPI_Wtime();

    return end - start;
}

int find_max(int *array, long array_size) {
  int rank, size;
  int local_max;
  int global_max;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int ibeg = rank * (array_size / size);
  int iend = (rank == size - 1) ? array_size : (rank + 1) * (array_size / size);
  local_max = array[ibeg];

  for (int i = ibeg + 1; i < iend; ++i) {
    if (array[i] > local_max) {
        local_max = array[i];
    }
  }

  MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
  return global_max;
}