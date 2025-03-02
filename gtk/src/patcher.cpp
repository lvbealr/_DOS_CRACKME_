#include <gtk/gtk.h>

#include "patcher.h"

#define FREE_(pointer) do { \
    free(pointer);          \
    pointer = NULL;         \
} while (0)


const size_t PASSWORD_START_ADDRESS = 0xD7;
const size_t PASSWORD_END_ADDRESS   = 0xDC;
const size_t PASSWORD_SIZE          = PASSWORD_END_ADDRESS - PASSWORD_START_ADDRESS + 1;

void *patchFile(void *arg) {
    // todo check file

    FILE *comFile = fopen(pathBuffer, "r+b");

    fseek(comFile, PASSWORD_START_ADDRESS, SEEK_SET);
    fwrite(INSERT_USER_PASSWORD, sizeof(char), PASSWORD_SIZE, comFile);

    fclose(comFile);

    FREE_(pathBuffer);

    return NULL;
}