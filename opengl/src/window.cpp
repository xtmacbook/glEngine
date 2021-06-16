
#include "glinter.h"
#include "window_w32.h"
#include "log.h"
#include "debug.h"

GWindow * g_main_window;

void _inputWindowFocus(GWindow* window, bool focused)
{
	platformFocusWindow(window);
}

void _inputWindowDamage(GWindow* window)
{
	 
}

void _inputWindowCloseRequest(GWindow*window)
{
	window->shouldClose_ = true;
}

void gluGetCursorPos(GWindow* handle, double* xpos, double* ypos)
{
	platformGetCursorPos(handle, xpos, ypos);
}

int gluGetKey(GWindow*window, int key)
{
	if (key < GLU_KEY_SPACE || key > GLU_KEY_LAST)
	{
		return GLU_RELEASE;
	}
	return (int)window->keys_[key];
}
void gluMakeCurrentContext(GWindow*handle)
{
	enAssert(handle != NULL);

	GWindow* window = (GWindow*)handle;
	if (window)
		window->context_.makeCurrent_(&window->context_);
}


void gluMakeCurrentShareContext()
{
	gluMakeCurrentShareContext(g_main_window);
}

void gluMakeCurrentShareContext(GWindow*handle)
{
	enAssert(handle != NULL);

	GWindow* window = (GWindow*)handle;
	if (window)
	{
		enAssert(window->sharedContext_ != nullptr);
		window->sharedContext_->makeCurrent_(window->sharedContext_);
	}
}

void gluMakeCurrentShareContextNULL(GWindow*handle)
{
	enAssert(handle != NULL);
	enAssert(handle->sharedContext_ != nullptr);
	handle->sharedContext_->makeCurrent_(NULL);
}

void gluMakeCurrentContextNULL(GWindow*handle)
{
	enAssert(handle != NULL);
	handle->context_.makeCurrent_(NULL);
}

void gluMakeCurrentShareContextNULL()
{
	gluMakeCurrentShareContextNULL(g_main_window);
}

void gluSwapBuffer(GWindow*handle)
{
	enAssert(handle != NULL);

	GWindow* window = (GWindow*)handle;
	if (window)
		window->context_.swapBuffers_(&window->context_);
}

void gluTerminate()
{
	if (!opg.initialized_)
		return;
	while (opg.windowListHead_)
		gluDestoryWindow((GWindow*)opg.windowListHead_);

	platformTerminate();
	base::destoryTls(&opg.contextSlot_);
	memset(&opg, 0, sizeof(opg));
}

void gluSwapInterval(int interval)
{
	Context * context = (Context*)platformGetTls(&opg.contextSlot_);
	if (!context)
	{
		PRINT_ERROR("Cannot set swap interval without a current OpenGL or OpenGL ES context");
		return;
	}

	context->swapInterval_(interval);
}
void gluPollEvents()
{
	platformPollEvents();
}

void gluDestoryWindow(GWindow*handle)
{
	GWindow* window = (GWindow*)handle;

	if (window == NULL)
		return;

	// The window's context must not be current on another thread when the
	// window is destroyed
	if (&window->context_.wgl_ct_ == platformGetTls(&opg.contextSlot_))
	{
		if (window->sharedContext_) gluMakeCurrentShareContextNULL();
		gluMakeCurrentContextNULL(handle);
	}

	PlatformDestroyWindow(window);

	{
		GWindow** prev = &opg.windowListHead_;

		while (*prev != window)
			prev = &((*prev)->next_);

		*prev = window->next_;
	}
	delete window;

	window = NULL;
}

bool gluInitWindow()
{
	return platformInit();
}

Context*  gluCreateContext(HWND*hwd, HGLRC*share, const DeviceConfig*dc, const Bconfig*wc)
{
	Context* newcontext = new Context;
	if(__createContextWGL(hwd, newcontext, share,  dc, wc)) return newcontext;
	delete newcontext;
	newcontext = NULL;
}
