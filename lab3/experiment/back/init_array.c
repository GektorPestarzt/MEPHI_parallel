#include <stdlib.h>

#include "init_array.h"

int *init_random_array(int size, int range) {
    int *array = (int *) malloc (size * sizeof(int));

    if (array == NULL) {
        return NULL;
    }

    srand(RANDOM_SEED_1);
    for (int i = 0; i < size; ++i) { array[i] = rand() % range; }

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

int *init_reversed_array(int size) {
    int *array = (int *) malloc (size * sizeof(int));

    if (array == NULL) {
        return NULL;
    }

    for (int i = 0; i < size; ++i) { array[i] = size - i - 1; }

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

int *init_mixed_array(int size) {
    int *dst = (int *) malloc (size * sizeof(int));

    if (dst == NULL) {
        return NULL;
    }

    int sh = size / 2;
    if (size % 2) ++sh;

    for (int i = 0; i < size / 2; ++i) { 
        dst[2*i] = sh - i - 1;
        dst[2*i + 1] = i;
    }
    dst[size - 1] = 0;

    return dst;
}

int *init_copy_array(const int *src, int size) {
    int *dst = (int *) malloc (size * sizeof(int));

    if (dst == NULL) {
        return NULL;
    }

    srand(RANDOM_SEED_1);
    for (int i = 0; i < size; ++i) { dst[i] = src[i]; }

    return dst;
}