#include "experiment_t.h"

#include <stdlib.h>

experiment_t *init_experiment (int *array, int *sorted_array, int size, int num_threads, int num_attempts) {
    experiment_t *experiment = (experiment_t *)malloc (sizeof(experiment_t));

    if (experiment == NULL) {
        return NULL;
    }

    experiment->sorted_array = sorted_array;
    experiment->array = array;
    experiment->size = size;
    experiment->num_threads = num_threads;
    experiment->num_attempts = num_attempts;

    return experiment;
}

void free_experiment(experiment_t *experiment) {
    free(experiment->array);
    free(experiment->sorted_array);
    free(experiment);
}