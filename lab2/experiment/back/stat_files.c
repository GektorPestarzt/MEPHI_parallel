#include <stdio.h>
#include <string.h>

#include "stat_files.h"

FILE *open_file_by_name(const char *file_path, const char *file_name);

bool is_files_open(const files_t *files);

bool read_files_path(files_t *files, int argc, char *argv[]) {
    if (argc != 2) {
        return true;
    }

    strcpy(files->files_path, argv[1]);
    return false;
}

bool open_files(files_t *files, int mode) {
    files->avg = open_file_by_name(files->files_path, STAT_FILE_NAME[mode + numberOfModes * avg]);
    files->spu = open_file_by_name(files->files_path, STAT_FILE_NAME[mode + numberOfModes * spu]);
    files->eff = open_file_by_name(files->files_path, STAT_FILE_NAME[mode + numberOfModes * eff]);

    if (!is_files_open(files)) {
        close_files(files);
        return true;
    }

    return false;
}

FILE *open_file_by_name(const char *file_path, const char *file_name) {
    char file[512] = {'\0'};
    strcat(file, file_path);

    if (file[strlen(file) - 1] != '/') {
        strcat(file, "/");
    }
    strcat(file, file_name);

    FILE *fd = fopen(file, "w");
    return fd;
}

bool is_files_open(const files_t *files) {
    if (files->avg == NULL || files->eff == NULL || files->spu == NULL) {
        return false;
    }
    return true;
}

void close_files(files_t *files) {
    if (files->avg != NULL) {
        fclose(files->avg);
        files->avg = NULL;
    }
    if (files->spu != NULL) {
        fclose(files->spu);
        files->spu = NULL;
    }
    if (files->eff != NULL) {
        fclose(files->eff);
        files->eff = NULL;
    }
}