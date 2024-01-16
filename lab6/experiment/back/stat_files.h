#ifndef EXPERIMENT_BACK_STAT_FILES_H_
#define EXPERIMENT_BACK_STAT_FILES_H_

#include <stdbool.h>

#include "modes.h"

#define MAX_FILE_NAME_LENGTH 40

enum type {
    avg,
    spu,
    eff,
    numberOfTypes
};

typedef struct files {
    FILE *avg;
    FILE *spu;
    FILE *eff;

    char files_path[512];
} files_t;

static const char STAT_FILE_NAME[numberOfModes * numberOfTypes]
                                      [MAX_FILE_NAME_LENGTH] = {
                                          {'a', 'v', 'g', '_', '0', '.', 'c', 's', 'v', '\0'},
                                          {'a', 'v', 'g', '_', '1', '.', 'c', 's', 'v', '\0'},
                                          {'a', 'v', 'g', '_', '2', '.', 'c', 's', 'v', '\0'},
                                          {'a', 'v', 'g', '_', '3', '.', 'c', 's', 'v', '\0'},
                                          {'a', 'v', 'g', '_', '4', '.', 'c', 's', 'v', '\0'},
                                          {'a', 'v', 'g', '_', '5', '.', 'c', 's', 'v', '\0'},
                                          {'a', 'v', 'g', '_', '6', '.', 'c', 's', 'v', '\0'},
                                          {'s', 'p', 'u', '_', '0', '.', 'c', 's', 'v', '\0'},
                                          {'s', 'p', 'u', '_', '1', '.', 'c', 's', 'v', '\0'},
                                          {'s', 'p', 'u', '_', '2', '.', 'c', 's', 'v', '\0'},
                                          {'s', 'p', 'u', '_', '3', '.', 'c', 's', 'v', '\0'},
                                          {'s', 'p', 'u', '_', '4', '.', 'c', 's', 'v', '\0'},
                                          {'s', 'p', 'u', '_', '5', '.', 'c', 's', 'v', '\0'},
                                          {'s', 'p', 'u', '_', '6', '.', 'c', 's', 'v', '\0'},
                                          {'e', 'f', 'f', '_', '0', '.', 'c', 's', 'v', '\0'},
                                          {'e', 'f', 'f', '_', '1', '.', 'c', 's', 'v', '\0'},
                                          {'e', 'f', 'f', '_', '2', '.', 'c', 's', 'v', '\0'},
                                          {'e', 'f', 'f', '_', '3', '.', 'c', 's', 'v', '\0'},
                                          {'e', 'f', 'f', '_', '4', '.', 'c', 's', 'v', '\0'},
                                          {'e', 'f', 'f', '_', '5', '.', 'c', 's', 'v', '\0'},
                                          {'e', 'f', 'f', '_', '6', '.', 'c', 's', 'v', '\0'}};

bool read_files_path (files_t *files, int argc, char *argv[]);
bool open_files (files_t *files, int mode);
void close_files (files_t *files);

#endif  // EXPERIMENT_BACK_STAT_FILES_H_