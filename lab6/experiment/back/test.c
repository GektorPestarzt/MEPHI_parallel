#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 14

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *array;
    int array_size = ARRAY_SIZE;

    if (rank == 0) {
        array = malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; ++i) {
            array[i] = i;
        }
    }

    // int *receive_array = calloc(size, sizeof(int));
    int *receive_array = calloc(4, sizeof(int));

    MPI_Scatter(array, 4, MPI_INT, receive_array, 4, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == size - 1) {
        printf("%d %d %d %d\n", receive_array[0], receive_array[1], receive_array[2], receive_array[3]);
    }

    MPI_Finalize();
}