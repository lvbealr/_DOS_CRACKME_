#include <gtk/gtk.h>

#include "patcher.h"

#define FREE_(pointer) do { \
    free(pointer);          \
    pointer = NULL;         \
} while (0)


// Make static or move in header
const size_t PASSWORD_START_ADDRESS = 0xD7;

// TODO what is arg?
void *patchFile(void *arg) {
    FILE *comFile = fopen(pathBuffer, "r+b");

    fseek(comFile, PASSWORD_START_ADDRESS, SEEK_SET);
    fwrite(INSERT_USER_PASSWORD, sizeof(char), PASSWORD_SIZE, comFile);

    fclose(comFile);

    FREE_(pathBuffer);

    // TODO do you really need this??
    return NULL;
}
