#ifndef EXPERIMENT_BACK_EXPERIMENT_T_H_
#define EXPERIMENT_BACK_EXPERIMENT_T_H_

#include <stdbool.h>

typedef struct ExperimentData {
    int *array;
    int *sorted_array;
    int size;

    int num_attempts;
} experiment_t;

experiment_t *init_experiment (int *array, int *sorted_array, int size, int num_attempts);
void free_experiment(experiment_t *experiment);

#endif  // EXPERIMENT_BACK_EXPERIMENT_T_H_