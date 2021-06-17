
#ifndef  _WIN32_PLATORM_H_
#define  _WIN32_PLATORM_H_
#include "sys.h"

#ifdef  EN_OS_WIN32
#include <wctype.h>
#include <windows.h>
#include <dinput.h>
#include <xinput.h>
#include <dbt.h>
#endif

#if WINVER < 0x0600
#define DWM_BB_ENABLE 0x00000001
#define DWM_BB_BLURREGION 0x00000002
typedef struct
{
	DWORD dwFlags;
	BOOL fEnable;
	HRGN hRgnBlur;
	BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND;
#else
#include <dwmapi.h>
#endif /*Windows Vista*/

typedef LONG(WINAPI * PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);

#define RtlVerifyVersionInfo opg.win32_.ntdll.RtlVerifyVersionInfo_

// HACK: Define versionhelpers.h functions manually as MinGW lacks the header
#define IsWindowsXPOrGreater()                                 \
    WindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WINXP),   \
                                        LOBYTE(_WIN32_WINNT_WINXP), 0)
#define IsWindowsVistaOrGreater()                                     \
    WindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_VISTA),   \
                                        LOBYTE(_WIN32_WINNT_VISTA), 0)
#define IsWindows7OrGreater()                                         \
    WindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WIN7),    \
                                        LOBYTE(_WIN32_WINNT_WIN7), 0)
#define IsWindows8OrGreater()                                         \
    WindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WIN8),    \
                                        LOBYTE(_WIN32_WINNT_WIN8), 0)
#define IsWindows8Point1OrGreater()                                   \
    WindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WINBLUE), \
                                        LOBYTE(_WIN32_WINNT_WINBLUE), 0)

#define _glfwIsWindows10AnniversaryUpdateOrGreaterWin32() \
    WindowsVersionOrGreaterWin32(14393)
#define _glfwIsWindows10CreatorsUpdateOrGreaterWin32() \
    WindowsVersionOrGreaterWin32(15063)


// dwmapi.dll function pointer typedefs
typedef HRESULT(WINAPI * PFN_DwmIsCompositionEnabled)(BOOL*);
typedef HRESULT(WINAPI * PFN_DwmFlush)(VOID);
typedef HRESULT(WINAPI * PFN_DwmEnableBlurBehindWindow)(HWND, const DWM_BLURBEHIND*);
#define DwmIsCompositionEnabled opg.win32_.dwmapi.IsCompositionEnabled_
#define DwmFlush opg.win32_.dwmapi.Flush_
#define DwmEnableBlurBehindWindow opg.win32_.dwmapi.EnableBlurBehindWindow_

#define PLATFORM_WINDOW_STATE         WindowWin32  win32_


#include "wgl_context.h"
#include "glu.h"

struct LibraryWin32
{
	HWND                helperWindowHandle_;
	HDEVNOTIFY          deviceNotificationHandle_;

	short int           keycodes[512];
	short int           scancodes[GLU_KEY_LAST + 1];
	char                keynames[GLU_KEY_LAST + 1][5];

	struct {
		HINSTANCE                       instance_;
		PFN_DwmIsCompositionEnabled     IsCompositionEnabled_;
		PFN_DwmFlush                    Flush_;
		PFN_DwmEnableBlurBehindWindow   EnableBlurBehindWindow_;
	} dwmapi;

	struct {
		HINSTANCE                       instance_;
		PFN_RtlVerifyVersionInfo        RtlVerifyVersionInfo_;
	} ntdll;
};

typedef struct WindowWin32
{
	HWND                handle_;
	HICON               bigIcon_;
	HICON               smallIcon_;
	bool                frameAction_;
	
	int                 lastCursorPosX_;
	int		    lastCursorPosY_;


} WindowWin32;

typedef struct MonitorWin32
{
	HMONITOR            handle_;
	// This size matches the static size of DISPLAY_DEVICE.DeviceName
	WCHAR               adapterName_[32];
	WCHAR               displayName_[32];
	char                publicAdapterName_[32];
	char                publicDisplayName_[32];
	bool            modesPruned_;
	bool            modeChanged_;

} MonitorWin32;

bool WindowsVersionOrGreaterWin32(WORD major, WORD minor, WORD sp);
bool loadLibraries();
bool platformInit(void);
void platformTerminate();
void inputKey(GWindow* window,
	int key, int scancode, int action, int mods);
#endif
