#ifndef PATCHER_H_
#define PATCHER_H_

// TODO you're fucking crazy... НАСКОЛЬКО НАХУЙ НАДР БЫТЬ ОТБИТЫМ ЧТОБЫ ЕЩЕ И ДУБЛИРОВАТЬ ВСЕ ЭТО ГОВНИЩЕ
extern GtkApplication *application;
extern const char     *applicationID;

extern GtkWidget      *window;
extern GtkWidget      *overlay;
extern GtkWidget      *labelBox;
extern GtkWidget      *firstLabel;
extern GtkWidget      *secondLabel;
extern GtkWidget      *progressBox;
extern GtkWidget      *progressBar;
extern GtkWidget      *managerBox;
extern GtkWidget      *background;
extern GtkWidget      *entryField;
extern GtkWidget      *button;

extern int             width;
extern int             height;
extern const char     *crackedFile;
extern char           *pathBuffer;
extern char           *msgBuffer;

const char INSERT_USER_PASSWORD[] = {0x48, 0x41, 0x43, 0x4B, 0x45, 0x44};
const size_t PASSWORD_SIZE        = sizeof(INSERT_USER_PASSWORD);

// FUNCTIONS //
void *patchFile(void *arg);
// FUNCTIONS //

#endif // PATCHER_H
