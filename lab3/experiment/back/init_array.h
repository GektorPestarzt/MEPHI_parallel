#ifndef EXPERIMENT_BACK_INIT_ARRAY_H_
#define EXPERIMENT_BACK_INIT_ARRAY_H_

#define RANDOM_SEED_1 51234234

int *init_random_array(int size, int range);
int *init_sorted_array(int size);
int *init_reversed_array(int size);
int *init_mixed_array(int size);

int *init_identical_array(int size, int element);
int *init_copy_array(const int *src, int size);

#endif // EXPERIMENT_BACK_INIT_ARRAY_H_