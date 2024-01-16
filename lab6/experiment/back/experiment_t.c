#include "experiment_t.h"

#include <stdlib.h>
#include <mpi.h>

experiment_t *init_experiment (int *array, int *sorted_array, int size, int num_attempts) {
    experiment_t *experiment = (experiment_t *)malloc (sizeof(experiment_t));

    if (experiment == NULL) {
        return NULL;
    }

    experiment->sorted_array = sorted_array;
    experiment->array = array;
    experiment->size = size;
    experiment->num_attempts = num_attempts;

    return experiment;
}

void free_experiment(experiment_t *experiment) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        free(experiment->array);
        free(experiment->sorted_array);
    }

    free(experiment);
}