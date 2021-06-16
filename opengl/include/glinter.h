
#ifndef  _GL_INTER_H_
#define  _GL_INTER_H_

#include "decl.h"
#include "thread.h"
#include "win32_platform.h"

struct LIBENIGHT_EXPORT Bconfig
{
	int         redBits;
	int         greenBits;
	int         blueBits;
	int         alphaBits;
	int         depthBits;
	int         stencilBits;

	int         accumRedBits;
	int         accumGreenBits;
	int         accumBlueBits;
	int         accumAlphaBits;

	int         auxBuffers;
	bool	    stereo;
	int         samples;
	bool	    sRGB;
	bool		doublebuffer;
	bool	    transparent;
	uintptr_t   handle;
};
struct LIBENIGHT_EXPORT Version_Config
{

	enum PROFILE
	{
		OPENGL_CORE_PROFILE,
		OPENGL_COMPAT_PROFILE
	};
	Version_Config();

	int glMaxJor_ = 3;
	int glMinJor_ = 3;
	int glforward_compat_;
	PROFILE glProfile_;
};
struct LIBENIGHT_EXPORT Context_Config
{
	enum ROBUSTNESS
	{
		NO_RESET_NOTIFICATION,
		LOSE_CONTEXT_ON_RESET
	};
	enum RELEASE
	{
		ANY_RELEASE_BEHAVIOR,
		RELEASE_BEHAVIOR_FLUSH,
		RELEASE_BEHAVIOR_NONE
	};

	bool				   debug_context_ = false;
	bool				   debug_sync_ = false;
	ROBUSTNESS			   robustness_;
	RELEASE				   release_;
	bool				   noerror_ = false;

};
struct LIBENIGHT_EXPORT DeviceConfig
{
public:
	Version_Config  glVersion_;
	Context_Config  glContext_;
};
struct LIBENIGHT_EXPORT WindowConfig
{
	int width_;
	int height_;
	int pos_x_;
	int pos_y_;

	const char * title_;
	bool shouldCreateSharedContext_ = false;
};
struct LIBENIGHT_EXPORT DLLLibrary
{
	bool            initialized_ = false;
	struct
	{
		Bconfig   framebuffer;
	} hints;

	LibraryWGL				wgl;
	LibraryWin32			win32_;
	base::TlsWin32			contextSlot_;
	GWindow *				windowListHead_;

};

extern struct DLLLibrary opg;
struct GWindow;

typedef void(*GLFWglproc)(void);
typedef void(*Makecontextcurrentfun)(Context*);
typedef void(*Swapbuffersfun)(Context*);
typedef void(*Swapintervalfun)(int);
typedef int(*Extensionsupportedfun)(const char*);
typedef GLFWglproc(*Getprocaddressfun)(const char*);
typedef void(*Destroycontextfun)(Context*);



typedef struct Vidmode
{
	int width_;
	int height_;
	int redBits_;
	int greenBits_;
	int refreshRate_;
} Vidmode;

typedef struct Gammaramp
{
	unsigned short* red;
	unsigned short* green;
	unsigned short* blue;
	unsigned int size;
} Gammaramp;

struct Monitor
{
	char*           name;
	void*           userPointer;

	// Physical dimensions in millimeters.
	int             widthMM, heightMM;

	// The window whose video mode is current on this monitor
	GWindow*    window;

	Vidmode*    modes;
	int             modeCount;
	Vidmode     currentMode;

	Gammaramp   originalRamp;
	Gammaramp   currentRamp;

	// This is defined in the window API's platform.h
	PLATFORM_WINDOW_STATE;
};

void _inputWindowFocus(GWindow* window, bool focused);
void _inputWindowDamage(GWindow* window);
void _inputWindowCloseRequest(GWindow*);

void platformGetCursorPos(GWindow* window, double* xpos, double* ypos);
bool platformRefreshContextAttribs(GWindow* window,const DeviceConfig* ctxconfig);
void platformPollEvents();
void platformFocusWindow(GWindow* window);
void platformShowWindow(GWindow* window);
void PlatformDestroyWindow(GWindow*);
#endif
