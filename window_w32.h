
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "sys.h"
#include "decl.h"
#include "glinter.h"

struct Context
{
	ContextWGL		wgl_ct_;

	Makecontextcurrentfun  makeCurrent_;
	Swapbuffersfun         swapBuffers_;
	Swapintervalfun        swapInterval_;
	Extensionsupportedfun  extensionSupported_;
	Getprocaddressfun      getProcAddress_;
	Destroycontextfun      destroy_;
};

struct LIBENIGHT_EXPORT GWindow
{
	~GWindow();

	GWindow *		next_ = NULL;
	WindowWin32		win_;
	Context			context_;
	Monitor*        monitor_ = NULL;
	bool			shouldClose_ = false;
	
	int                 cursorMode_;
	char                mouseButtons_[GLU_MOUSE_BUTTON_LAST + 1];
	char                keys_[GLU_KEY_LAST + 1];
	
	double              virtualCursorPosX_;
	double				virtualCursorPosY_;

	Context*			sharedContext_ = nullptr;
};

#endif
