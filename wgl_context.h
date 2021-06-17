#include "sys.h"
#include "gls.h"

#include "decl.h"

#ifdef  EN_OS_WIN32
#include <Windowsx.h>
#endif

#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB 0x31b3

#define ERROR_INVALID_VERSION_ARB 0x2095
#define ERROR_INVALID_PROFILE_ARB 0x2096
#define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054

#define wglSwapIntervalEXT opg.wgl.SwapIntervalEXT
#define wglGetPixelFormatAttribivARB opg.wgl.GetPixelFormatAttribivARB
#define wglGetExtensionsStringEXT opg.wgl.GetExtensionsStringEXT
#define wglGetExtensionsStringARB opg.wgl.GetExtensionsStringARB
#define wglCreateContextAttribsARB opg.wgl.CreateContextAttribsARB

// opengl32.dll function pointer typedefs
typedef HGLRC(WINAPI * PFN_wglCreateContext)(HDC);
typedef BOOL(WINAPI * PFN_wglDeleteContext)(HGLRC);
typedef PROC(WINAPI * PFN_wglGetProcAddress)(LPCSTR);
typedef HDC(WINAPI * PFN_wglGetCurrentDC)(void);
typedef HGLRC(WINAPI * PFN_wglGetCurrentContext)(void);
typedef BOOL(WINAPI * PFN_wglMakeCurrent)(HDC, HGLRC);
typedef BOOL(WINAPI * PFN_wglShareLists)(HGLRC, HGLRC);

#define wglCreateContext opg.wgl.CreateContext
#define wglDeleteContext opg.wgl.DeleteContext
#define wglGetProcAddress opg.wgl.GetProcAddress
#define wglGetCurrentDC opg.wgl.GetCurrentDC
#define wglGetCurrentContext opg.wgl.GetCurrentContext
#define wglMakeCurrent opg.wgl.MakeCurrent
#define wglShareLists opg.wgl.ShareLists

struct GWindow;
struct DeviceConfig;
struct Bconfig;

struct Context;
struct LIBENIGHT_EXPORT ContextWGL
{
	HDC       dc_;
	HGLRC     handle_;
	int       interval_;
};

struct LibraryWGL
{
	HINSTANCE                           instance;
	PFN_wglCreateContext                CreateContext;
	PFN_wglDeleteContext                DeleteContext;
	PFN_wglGetProcAddress               GetProcAddress;
	PFN_wglGetCurrentDC                 GetCurrentDC;
	PFN_wglGetCurrentContext            GetCurrentContext;
	PFN_wglMakeCurrent                  MakeCurrent;
	PFN_wglShareLists                   ShareLists;

	PFNWGLSWAPINTERVALEXTPROC           SwapIntervalEXT;
	PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;
	PFNWGLGETEXTENSIONSSTRINGEXTPROC    GetExtensionsStringEXT;
	PFNWGLGETEXTENSIONSSTRINGARBPROC    GetExtensionsStringARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC   CreateContextAttribsARB;
	bool                            EXT_swap_control;
	bool                            EXT_colorspace;
	bool                            ARB_multisample;
	bool                            ARB_framebuffer_sRGB;
	bool                            EXT_framebuffer_sRGB;
	bool                            ARB_pixel_format;
	bool                            ARB_create_context;
	bool                            ARB_create_context_profile;
	bool                            EXT_create_context_es2_profile;
	bool                            ARB_create_context_robustness;
	bool                            ARB_create_context_no_error;
	bool                            ARB_context_flush_control;
};

bool _initWGL(void);
bool _createContextWGL(GWindow*, const DeviceConfig *, const Bconfig* fbconfig);
bool __createContextWGL(HWND* hand, Context*, HGLRC*share, const DeviceConfig *, const Bconfig* fbconfig,bool shared = false);
bool ___createContextWGL(HWND* hwnd, HDC* hdc /*out*/ , HGLRC* hglrc /*out*/ , HGLRC*share /*in*/, const DeviceConfig *, const Bconfig* fbconfig,bool shared = false);
void _terminateWgl();

