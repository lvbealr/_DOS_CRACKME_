#ifndef CONSTANT_H_
#define CONSTANT_H_

// ######################################### OBJECTS PARAMETERS ############################################# //

static const int    WINDOW_WIDTH               = 640;
static const int    WINDOW_HEIGHT              = 480;

static const int    LABEL_BOX_INDENT           = 5;
static const int    LABEL_BOX_MARGIN_BOTTOM    = 80;

static const int    LABEL_MAX_WIDTH_CHARS      = 64;

static const char  *FIRST_LABEL_MESSAGE        = "Welcome to Crack.ME!";
static const char  *SECOND_LABEL_MESSAGE       = "";

static const int    PROGRESS_BOX_INDENT        = 0;
static const int    PROGRESS_BOX_MARGIN_BOTTOM = 70;

static const double PROGRESS_BAR_DEFAULT_VALUE = 0.0;

static const int    MANAGER_BOX_INDENT         = 0;
static const int    MANAGER_BOX_MARGIN_BOTTOM  = 20;

static const char  *ENTRYFIELD_PLACEHOLDER_TXT = "Enter a path to file to crack...";
static const int    ENTRYFIELD_DEFAULT_WIDTH   = 400;
static const int    ENTRYFIELD_DEFAULT_HEIGHT  = 20;
static const int    ENTRYFIELD_MAX_TEXT_SIZE   = 64;

static const char  *BUTTON_LABEL_TEXT          = "☠ CRACK!";
static const int    BUTTON_DEFAULT_WIDTH       = 5;
static const int    BUTTON_DEFAULT_HEIGHT      = 5;

static const char  *STYLE_CSS_PATH             = "include/style.css";

static const char  *COM_FILE_NOT_FOUND_MESSAGE = "<span foreground='red'>[ERROR]:</span> couldn't open file";
static const char  *COM_FILE_FOUND_MESSAGE     = "<span foreground='orange'>[OK]: open file...</span>";
static const char  *PATH_TO_FILE_MESSAGE       = "<span foreground='gray'>%s</span>";
static const char  *PATCHED_SUCCESSFUL         = "<span foreground='green'>[SUCCESS]: file was cracked!</span>";

static const char  *PATH_TO_SOUND              = "sounds/bgSound3.mp3";

// ######################################### OBJECTS PARAMETERS ############################################# //

#endif // CONSTANT_H_