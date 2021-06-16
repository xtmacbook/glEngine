
#ifndef _GLU_H_
#define  _GLU_H_

#define GLU_RELEASE                0
#define GLU_PRESS                  1
#define GLU_REPEAT				   2

#define GLU_KEY_UNKNOWN            -1

/* Printable keys */
#define GLU_KEY_SPACE              32
#define GLU_KEY_APOSTROPHE         39  /* ' */
#define GLU_KEY_COMMA              44  /* , */
#define GLU_KEY_MINUS              45  /* - */
#define GLU_KEY_PERIOD             46  /* . */
#define GLU_KEY_SLASH              47  /* / */
#define GLU_KEY_0                  48
#define GLU_KEY_1                  49
#define GLU_KEY_2                  50
#define GLU_KEY_3                  51
#define GLU_KEY_4                  52
#define GLU_KEY_5                  53
#define GLU_KEY_6                  54
#define GLU_KEY_7                  55
#define GLU_KEY_8                  56
#define GLU_KEY_9                  57
#define GLU_KEY_SEMICOLON          59  /* ; */
#define GLU_KEY_EQUAL              61  /* = */
#define GLU_KEY_A                  65
#define GLU_KEY_B                  66
#define GLU_KEY_C                  67
#define GLU_KEY_D                  68
#define GLU_KEY_E                  69
#define GLU_KEY_F                  70
#define GLU_KEY_G                  71
#define GLU_KEY_H                  72
#define GLU_KEY_I                  73
#define GLU_KEY_J                  74
#define GLU_KEY_K                  75
#define GLU_KEY_L                  76
#define GLU_KEY_M                  77
#define GLU_KEY_N                  78
#define GLU_KEY_O                  79
#define GLU_KEY_P                  80
#define GLU_KEY_Q                  81
#define GLU_KEY_R                  82
#define GLU_KEY_S                  83
#define GLU_KEY_T                  84
#define GLU_KEY_U                  85
#define GLU_KEY_V                  86
#define GLU_KEY_W                  87
#define GLU_KEY_X                  88
#define GLU_KEY_Y                  89
#define GLU_KEY_Z                  90
#define GLU_KEY_LEFT_BRACKET       91  /* [ */
#define GLU_KEY_BACKSLASH          92  /* \ */
#define GLU_KEY_RIGHT_BRACKET      93  /* ] */
#define GLU_KEY_GRAVE_ACCENT       96  /* ` */
#define GLU_KEY_WORLD_1            161 /* non-US #1 */
#define GLU_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define GLU_KEY_ESCAPE             256
#define GLU_KEY_ENTER              257
#define GLU_KEY_TAB                258
#define GLU_KEY_BACKSPACE          259
#define GLU_KEY_INSERT             260
#define GLU_KEY_DELETE             261
#define GLU_KEY_RIGHT              262
#define GLU_KEY_LEFT               263
#define GLU_KEY_DOWN               264
#define GLU_KEY_UP                 265
#define GLU_KEY_PAGE_UP            266
#define GLU_KEY_PAGE_DOWN          267
#define GLU_KEY_HOME               268
#define GLU_KEY_END                269
#define GLU_KEY_CAPS_LOCK          280
#define GLU_KEY_SCROLL_LOCK        281
#define GLU_KEY_NUM_LOCK           282
#define GLU_KEY_PRINT_SCREEN       283
#define GLU_KEY_PAUSE              284
#define GLU_KEY_F1                 290
#define GLU_KEY_F2                 291
#define GLU_KEY_F3                 292
#define GLU_KEY_F4                 293
#define GLU_KEY_F5                 294
#define GLU_KEY_F6                 295
#define GLU_KEY_F7                 296
#define GLU_KEY_F8                 297
#define GLU_KEY_F9                 298
#define GLU_KEY_F10                299
#define GLU_KEY_F11                300
#define GLU_KEY_F12                301
#define GLU_KEY_F13                302
#define GLU_KEY_F14                303
#define GLU_KEY_F15                304
#define GLU_KEY_F16                305
#define GLU_KEY_F17                306
#define GLU_KEY_F18                307
#define GLU_KEY_F19                308
#define GLU_KEY_F20                309
#define GLU_KEY_F21                310
#define GLU_KEY_F22                311
#define GLU_KEY_F23                312
#define GLU_KEY_F24                313
#define GLU_KEY_F25                314
#define GLU_KEY_KP_0               320
#define GLU_KEY_KP_1               321
#define GLU_KEY_KP_2               322
#define GLU_KEY_KP_3               323
#define GLU_KEY_KP_4               324
#define GLU_KEY_KP_5               325
#define GLU_KEY_KP_6               326
#define GLU_KEY_KP_7               327
#define GLU_KEY_KP_8               328
#define GLU_KEY_KP_9               329
#define GLU_KEY_KP_DECIMAL         330
#define GLU_KEY_KP_DIVIDE          331
#define GLU_KEY_KP_MULTIPLY        332
#define GLU_KEY_KP_SUBTRACT        333
#define GLU_KEY_KP_ADD             334
#define GLU_KEY_KP_ENTER           335
#define GLU_KEY_KP_EQUAL           336
#define GLU_KEY_LEFT_SHIFT         340
#define GLU_KEY_LEFT_CONTROL       341
#define GLU_KEY_LEFT_ALT           342
#define GLU_KEY_LEFT_SUPER         343
#define GLU_KEY_RIGHT_SHIFT        344
#define GLU_KEY_RIGHT_CONTROL      345
#define GLU_KEY_RIGHT_ALT          346
#define GLU_KEY_RIGHT_SUPER        347
#define GLU_KEY_MENU               348

#define GLU_KEY_LAST               GLU_KEY_MENU

#define GLU_MOUSE_BUTTON_1         0
#define GLU_MOUSE_BUTTON_2         1
#define GLU_MOUSE_BUTTON_3         2
#define GLU_MOUSE_BUTTON_4         3
#define GLU_MOUSE_BUTTON_5         4
#define GLU_MOUSE_BUTTON_6         5
#define GLU_MOUSE_BUTTON_7         6
#define GLU_MOUSE_BUTTON_8         7
#define GLU_MOUSE_BUTTON_LAST      GLU_MOUSE_BUTTON_8
#define GLU_MOUSE_BUTTON_LEFT      GLU_MOUSE_BUTTON_1
#define GLU_MOUSE_BUTTON_RIGHT     GLU_MOUSE_BUTTON_2
#define GLU_MOUSE_BUTTON_MIDDLE    GLU_MOUSE_BUTTON_3


#define GLU_MOD_SHIFT           0x0001
#define GLU_MOD_CONTROL         0x0002
#define GLU_MOD_ALT             0x0004
#define GLU_MOD_SUPER           0x0008
#define GLU_MOD_CAPS_LOCK       0x0010
#define GLU_MOD_NUM_LOCK        0x0020

struct GWindow;
struct DeviceConfig;
struct WindowConfig;
struct Context;
struct Bconfig;

#include "decl.h"
bool	    gluInitWindow();
GWindow *   gluCreateWindow(const DeviceConfig*, const WindowConfig*);
LIBENIGHT_EXPORT Context *   gluCreateContext(HWND*, HGLRC*share,const DeviceConfig*, const Bconfig*);
LIBENIGHT_EXPORT void		gluDestoryWindow(GWindow*);

LIBENIGHT_EXPORT void		gluMakeCurrentContext(GWindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContext(GWindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContext();


LIBENIGHT_EXPORT void		gluMakeCurrentContextNULL(GWindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContextNULL(GWindow*);
LIBENIGHT_EXPORT void		gluMakeCurrentShareContextNULL();


LIBENIGHT_EXPORT void		gluSwapBuffer(GWindow*);
LIBENIGHT_EXPORT void		gluTerminate();

void gluPollEvents();
void gluSwapInterval(int interval);
void gluGetCursorPos(GWindow* handle, double* xpos, double* ypos);
int  gluGetKey(GWindow*, int key);
#endif _GLU_H_
