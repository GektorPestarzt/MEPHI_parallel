#include <stdlib.h>

#include "init_array.h"

int *init_random_array(int size) {
    int *array = (int *) malloc (size * sizeof(int));

    if (array == NULL) {
        return NULL;
    }

    srand(RANDOM_SEED_1);
    for (int i = 0; i < size; ++i) { array[i] = rand(); }

    return array;
}

int *init_sorted_array(int size) {
    int *array = (int *) malloc (size * sizeof(int));

    if (array == NULL) {
        return NULL;
    }

    for (int i = 0; i < size; ++i) { array[i] = i; }

    return array;
}

int *init_identical_array(int size, int element) {
    int *array = (int *) malloc (size * sizeof(int));

    if (array == NULL) {
        return NULL;
    }

    for (int i = 0; i < size; ++i) { array[i] = element; }

    return array;
}