#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <gtk/gtk.h>
#include <stddef.h>

#define FREE_(pointer) do { \
    free(pointer);          \
    pointer = NULL;         \
} while (0)

enum gtkError {
    GTK_NO_ERROR       = 0,
    CALLOC_ERROR       = 1,
    NO_GTK_APP         = 2,
    NO_GTK_WINDOW      = 3,
    NO_GTK_WIDGET      = 4,
    APPLICATION_ERROR  = 5,
    NO_GTK_BACKGROUND  = 6,
    NO_GTK_BUTTON      = 7,
    NO_GTK_OVERLAY     = 8,
    NO_GTK_ENTRY_FIELD = 9,
    NO_GTK_BOX         = 10,
    NO_GTK_LABEL       = 11,
    NO_COM_FILE        = 12
};

static const size_t BUFFER_SIZE = 256;

// FUNCTION PROTOTYPES //
gtkError    initializeGtkApplication      (const char *applicationID, GApplicationFlags flags, int width, int height, int argc, char **argv);
gtkError    destroyGtkApplication         ();
gtkError    initializeApplicationInterface();
static void activateGtkApplication        (GApplication *application);

void       *crackFile(void *arg);

static void checkButton(GtkButton *button);
// FUNCTION PROTOTYPES //

#endif // APPLICATION_H_
