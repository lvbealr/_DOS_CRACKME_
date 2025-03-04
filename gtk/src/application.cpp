#include <gst/gst.h>

#include "customWarning.h"
#include "application.h"
#include "objects.h"
#include "objectParameters.h"
#include "patcher.h"

#define SET_ALIGN(object, halign, valign) do {         \
    gtk_widget_set_halign(GTK_WIDGET(object), halign); \
    gtk_widget_set_valign(GTK_WIDGET(object), valign); \
} while (0)

typedef struct {
  GtkPicture *picture;
  GdkPixbufAnimation *animation;
  GdkPixbufAnimationIter *iter;
  guint timeout_id;
} animationData;

static gboolean updateAnimation(gpointer userData) {
    animationData *data = (animationData *) userData;

    gdk_pixbuf_animation_iter_advance(data->iter, NULL);

    GdkPixbuf* frame = gdk_pixbuf_animation_iter_get_pixbuf(data->iter);
    GdkPaintable* paintable = GDK_PAINTABLE(gdk_texture_new_for_pixbuf(frame));

    gtk_picture_set_paintable(data->picture, paintable);

    int delay = gdk_pixbuf_animation_iter_get_delay_time(data->iter);

    g_source_remove(data->timeout_id);
    data->timeout_id = g_timeout_add(delay, updateAnimation, data);

    return G_SOURCE_REMOVE;
}

static void playMusic(GtkWidget *widget, gpointer data) {

  	GError *error = NULL;

    char *cmdBuffer  = (char *)calloc(BUFFER_SIZE, sizeof(char));
    customAssert(cmdBuffer != NULL, 1);

    char *currDir   = (char *)calloc(BUFFER_SIZE / 2, sizeof(char));
    customAssert(currDir  != NULL, 1);
    getcwd(currDir, BUFFER_SIZE / 2);

    sprintf(cmdBuffer, "playbin uri=file://%s/%s", currDir, PATH_TO_SOUND);

    GstElement *pipeline = gst_parse_launch(cmdBuffer, &error);

    if (error) {
      g_printerr("Error: %gs\n", error->message);
      g_error_free(error);
      return;
    }

    FREE_(cmdBuffer);
    FREE_(currDir);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

 gtkError initializeGtkApplication(const char *applicationID, GApplicationFlags flags, int windowWidth, int windowHeight, int argc, char **argv) {
     gst_init(&argc, &argv);
     gtk_init();

     applicationID = applicationID;
     application   = gtk_application_new(applicationID, flags);
     customWarning(application != NULL, APPLICATION_ERROR);

     width  = windowWidth;
     height = windowHeight;

     msgBuffer   = (char *)calloc(BUFFER_SIZE, sizeof(char));
     customWarning(msgBuffer!= NULL, CALLOC_ERROR);

     g_signal_connect(application, "activate",  G_CALLBACK(activateGtkApplication), NULL);

     gtkError applicationStatus = (gtkError) g_application_run(G_APPLICATION(application), argc, argv);

     return GTK_NO_ERROR;
 }

 static void activateGtkApplication(GApplication *application) {
     customWarning(application != NULL, (void) NO_GTK_APP);

     window = gtk_application_window_new(GTK_APPLICATION(application));
     customWarning(window != NULL, (void) NO_GTK_WINDOW);

     g_signal_connect(window, "realize", G_CALLBACK(playMusic), NULL);

     gtk_window_set_title          (GTK_WINDOW(window), applicationID);
     gtk_window_set_default_size   (GTK_WINDOW(window), width, height);
     gtk_window_set_resizable      (GTK_WINDOW(window), FALSE);

     initializeApplicationInterface();

     gtk_window_present            (GTK_WINDOW(window));
 }

 gtkError initializeApplicationInterface() {
     customWarning(window != NULL, NO_GTK_WINDOW);

     overlay = gtk_overlay_new();
     customWarning(overlay != NULL, NO_GTK_OVERLAY);
     gtk_window_set_child(GTK_WINDOW(window), overlay);

     GtkWidget *picture = gtk_picture_new();
     gtk_widget_set_hexpand(picture, TRUE);

     // GIF TO BACKGROUND //////////////////////////////////////////////////////////////////////////
     GError *error = NULL;
     GdkPixbufAnimation *animation = gdk_pixbuf_animation_new_from_file("img/background.gif", &error);

     GdkPixbufAnimationIter *iter = gdk_pixbuf_animation_get_iter(animation, NULL);

     animationData *data = g_new0(animationData, 1);
     data->picture 	     = GTK_PICTURE(picture);
     data->animation     = animation;
     data->iter 		 = iter;

     data->timeout_id = g_timeout_add(100, updateAnimation, data);

     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), picture);
     gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_COVER);
	 // GIF TO BACKGROUND //////////////////////////////////////////////////////////////////////////

     labelBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, LABEL_BOX_INDENT);
     customWarning(labelBox != NULL, NO_GTK_BOX);

     SET_ALIGN(labelBox, GTK_ALIGN_CENTER, GTK_ALIGN_END);

     gtk_widget_set_margin_bottom(labelBox, LABEL_BOX_MARGIN_BOTTOM);
     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), labelBox);

     firstLabel = gtk_label_new(FIRST_LABEL_MESSAGE);
     customWarning(firstLabel != NULL, NO_GTK_LABEL);
     gtk_label_set_max_width_chars(GTK_LABEL(firstLabel), LABEL_MAX_WIDTH_CHARS);
     gtk_label_set_wrap(GTK_LABEL(firstLabel), TRUE);
     gtk_box_append(GTK_BOX(labelBox), firstLabel);

     secondLabel = gtk_label_new(SECOND_LABEL_MESSAGE);
     customWarning(secondLabel != NULL, NO_GTK_LABEL);
     gtk_label_set_max_width_chars(GTK_LABEL(secondLabel), LABEL_MAX_WIDTH_CHARS);
     gtk_label_set_wrap(GTK_LABEL(secondLabel), TRUE);
     gtk_box_append(GTK_BOX(labelBox), secondLabel);

     progressBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, PROGRESS_BOX_INDENT);
     customWarning(progressBox != NULL, NO_GTK_BOX);
     SET_ALIGN(progressBox, GTK_ALIGN_CENTER, GTK_ALIGN_END);
     gtk_widget_set_margin_bottom(progressBox, PROGRESS_BOX_MARGIN_BOTTOM);
     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), progressBox);

     progressBar  = gtk_progress_bar_new();
     gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), PROGRESS_BAR_DEFAULT_VALUE);
     gtk_box_append(GTK_BOX(progressBox), progressBar);

     managerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MANAGER_BOX_INDENT);
     customWarning(managerBox != NULL, NO_GTK_BOX);
     SET_ALIGN(managerBox, GTK_ALIGN_CENTER, GTK_ALIGN_END);
     gtk_widget_set_margin_bottom(managerBox, MANAGER_BOX_MARGIN_BOTTOM);
     gtk_overlay_add_overlay(GTK_OVERLAY(overlay), managerBox);

//     background = gtk_picture_new_for_filename(BACKGROUND_IMAGE_PATH);
//     customWarning(background != NULL, NO_GTK_BACKGROUND);
//     gtk_overlay_set_child(GTK_OVERLAY(overlay), background);

     entryField = gtk_entry_new();
     customWarning(entryField != NULL, NO_GTK_ENTRY_FIELD);
     gtk_entry_set_placeholder_text(GTK_ENTRY(entryField), ENTRYFIELD_PLACEHOLDER_TXT);
     SET_ALIGN(entryField, GTK_ALIGN_CENTER, GTK_ALIGN_END);
     gtk_widget_set_size_request(entryField, ENTRYFIELD_DEFAULT_WIDTH, ENTRYFIELD_DEFAULT_HEIGHT);
     gtk_entry_set_max_length(GTK_ENTRY(entryField), ENTRYFIELD_MAX_TEXT_SIZE);
     gtk_box_append(GTK_BOX(managerBox), entryField);

     button     = gtk_button_new_with_label(BUTTON_LABEL_TEXT);
     customWarning(button != NULL, NO_GTK_BUTTON);
     gtk_widget_set_size_request(button, BUTTON_DEFAULT_WIDTH, BUTTON_DEFAULT_HEIGHT);
     gtk_box_append(GTK_BOX(managerBox), button);

     g_signal_connect(button, "clicked", G_CALLBACK(checkButton), NULL);

     GtkCssProvider *cssProvider = gtk_css_provider_new();
     gtk_css_provider_load_from_path(cssProvider, STYLE_CSS_PATH);

     gtk_style_context_add_provider_for_display(
             gdk_display_get_default(),
             GTK_STYLE_PROVIDER(cssProvider),
             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
     );

     return GTK_NO_ERROR;
 }

 #include <unistd.h>

 void checkButton(GtkButton *button) {
     if (!(button)) {
         return;
     }

     gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0);

     crackedFile = gtk_editable_get_text(GTK_EDITABLE(entryField));
     FILE *comFile  = {};

	 pathBuffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
     customAssert(pathBuffer != NULL, 1);
     getcwd(pathBuffer, BUFFER_SIZE);

     strcat(pathBuffer, "/");
     strcat(pathBuffer, crackedFile);

     if (comFile = fopen(crackedFile, "r+b")) {
         fclose(comFile);

         crackFile(NULL);
        
         return;
     }

     sprintf(msgBuffer, COM_FILE_NOT_FOUND_MESSAGE);
     gtk_label_set_markup(GTK_LABEL(firstLabel), msgBuffer);
     sprintf(msgBuffer, PATH_TO_FILE_MESSAGE, crackedFile);
     gtk_label_set_markup(GTK_LABEL(secondLabel), msgBuffer);

     return;
 }

 static guint progressBarThread = 0;

 static gboolean updateProgress(gpointer userData) {
     gdouble fraction = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(progressBar));

     if (abs(fraction - 1) < 1e-6) {
         g_source_remove(progressBarThread);

         progressBarThread = 0;
         return false;
     }

     if (abs(fraction - 0.75) < 0.05) {
         g_usleep(80000);
     }

     gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressBar));
     gtk_progress_bar_set_pulse_step(GTK_PROGRESS_BAR(progressBar), 0.005);
     gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), fraction + 0.005);

     return true;
 }

 void *crackFile(void *arg) {
     FILE *comFile = fopen(crackedFile, "r+b");

     if (!comFile) {
         sprintf(msgBuffer, COM_FILE_NOT_FOUND_MESSAGE);
         gtk_label_set_markup(GTK_LABEL(firstLabel), msgBuffer);
         sprintf(msgBuffer, PATH_TO_FILE_MESSAGE, crackedFile);
         gtk_label_set_markup(GTK_LABEL(secondLabel), msgBuffer);

         return NULL;
     }

     if (progressBarThread != 0) {
         return NULL;
     }

     sprintf(msgBuffer, COM_FILE_FOUND_MESSAGE);
     gtk_label_set_markup(GTK_LABEL(firstLabel), msgBuffer);
     sprintf(msgBuffer, PATH_TO_FILE_MESSAGE, crackedFile);
     gtk_label_set_markup(GTK_LABEL(secondLabel), msgBuffer);

     fclose(comFile);

     sleep(1);

     patchFile(NULL);
     progressBarThread = g_timeout_add(25, updateProgress, NULL);

     sprintf(msgBuffer, PATCHED_SUCCESSFUL);
     gtk_label_set_markup(GTK_LABEL(firstLabel), msgBuffer);
     sprintf(msgBuffer, "Now correct password is: <span foreground='orange'>");
     strncat(msgBuffer, INSERT_USER_PASSWORD, PASSWORD_SIZE);
     strcat(msgBuffer, "</span>");
     gtk_label_set_markup(GTK_LABEL(secondLabel), msgBuffer);

     return NULL;
 }


 gtkError destroyGtkApplication() {
     FREE_(msgBuffer);

     g_application_quit(G_APPLICATION(application));

     return GTK_NO_ERROR;
 }