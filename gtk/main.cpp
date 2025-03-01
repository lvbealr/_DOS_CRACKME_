#include <gtk/gtk.h>

#include "application.h"
#include "objectParameters.h"

int main(int argc, char **argv) {
    initializeGtkApplication("Crack.ME", G_APPLICATION_DEFAULT_FLAGS, WINDOW_WIDTH, WINDOW_HEIGHT, argc, argv);

    destroyGtkApplication();

    return 0;
}