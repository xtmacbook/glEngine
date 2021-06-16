#include "window_w32.h"
#include "log.h"
#include "gls.h"
#include "windowManager.h"

#ifdef  EN_OS_WIN32
#include <Windowsx.h>
#include <Dbt.h>
#endif

#if !defined(_WNDCLASSNAME)
#define _WNDCLASSNAME "GL330"
#endif

static const GUID _GUID_DEVINTERFACE_HID =
{ 0x4d1e55b2,0xf16f,0x11cf,{ 0x88,0xcb,0x00,0x11,0x11,0x00,0x00,0x30 } };

#define GUID_DEVINTERFACE_HID _GUID_DEVINTERFACE_HID

LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void  mouse_curse_pos_callback(GWindow *window, double xpos, double ypos);
extern void  mouse_button_callback(GWindow* window, int button, int action, int mods);
extern void  key_callback(GWindow*window, int key, int st, int action, int mods);
extern void  windowSize(GWindow* window, int width, int height);

HINSTANCE hInstance;

static int getKeyMods(void)
{
	int mods = 0;

	if (GetKeyState(VK_SHIFT) & 0x8000)
		mods |= GLU_MOD_SHIFT;
	if (GetKeyState(VK_CONTROL) & 0x8000)
		mods |= GLU_MOD_CONTROL;
	if (GetKeyState(VK_MENU) & 0x8000)
		mods |= GLU_MOD_ALT;
	if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
		mods |= GLU_MOD_SUPER;
	if (GetKeyState(VK_CAPITAL) & 1)
		mods |= GLU_MOD_CAPS_LOCK;
	if (GetKeyState(VK_NUMLOCK) & 1)
		mods |= GLU_MOD_NUM_LOCK;

	return mods;
}

extern void InputErrorWin32(int error, const char* description)
{
	WCHAR buffer[516] = L"";
	char message[516] = "";

	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		GetLastError() & 0xffff,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer,
		sizeof(buffer) / sizeof(WCHAR),
		NULL);
	WideCharToMultiByte(CP_UTF8, 0, buffer, -1, message, sizeof(message), NULL, NULL);

	char buf[1024];
	snprintf(buf,1024,"%s: %s",description,message);
	PRINT_ERROR(buf);
}

extern bool registerWindowClassWin32()
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = windowProc;
	wc.hInstance = hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = _WNDCLASSNAME;

	if (!RegisterClassEx(&wc))
	{
		PRINT_ERROR("Win32: Failed to register window class! \n");
		return false;
	}

	return true;
}

void unregisterWindowClassWin32()
{
	UnregisterClass(_WNDCLASSNAME, GetModuleHandleW(NULL));
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GWindow* window = (GWindow*)GetProp(hwnd, "GLU");

	switch (msg) {
	
	case WM_MOUSEACTIVATE:
	{
		if (HIWORD(lParam) == WM_LBUTTONDOWN)
		{
			if (LOWORD(lParam) != HTCLIENT)
				window->win_.frameAction_ = true;
		}
		break;
	}

	case WM_CAPTURECHANGED:
	{
		if (lParam == 0 && window->win_.frameAction_)
		{
			window->win_.frameAction_ = false;
		}
		break;
	}
	case WM_CLOSE:
	{
		_inputWindowCloseRequest(window);
		return 0;
	}
	case WM_SIZE:
		windowSize(window, LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		ValidateRect(hwnd, 0);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	{
		int i, button, action;

		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
			button = GLU_MOUSE_BUTTON_LEFT;
		else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
			button = GLU_MOUSE_BUTTON_RIGHT;
		else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
			button = GLU_MOUSE_BUTTON_MIDDLE;
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			button = GLU_MOUSE_BUTTON_4;
		else
			button = GLU_MOUSE_BUTTON_5;

		if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
			msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
		{
			action = GLU_PRESS;
		}
		else
			action = GLU_RELEASE;

		for (i = 0; i <= GLU_MOUSE_BUTTON_LAST; i++)
		{
			if (window->mouseButtons_[i] == GLU_PRESS)
				break;
		}

		if (i > GLU_MOUSE_BUTTON_LAST)
			SetCapture(hwnd);

		mouse_button_callback(window, button, action, getKeyMods());

		for (i = 0; i <= GLU_MOUSE_BUTTON_LAST; i++)
		{
			if (window->mouseButtons_[i] == GLU_PRESS)
				break;
		}

		if (i > GLU_MOUSE_BUTTON_LAST)
			ReleaseCapture();

		if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP)
			return true;

		return 0;
	}
	case WM_MOUSEMOVE:
	{
		const int x = GET_X_LPARAM(lParam);
		const int y = GET_Y_LPARAM(lParam);
		mouse_curse_pos_callback(window, x, y);
		window->win_.lastCursorPosX_ = x;
		window->win_.lastCursorPosY_ = y;
		return 0;
	}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		int key, scancode;
		const int action = (HIWORD(lParam) & KF_UP) ? GLU_RELEASE : GLU_PRESS;
		const int mods = getKeyMods();

		scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
		if (!scancode)
		{
			scancode = MapVirtualKey((UINT)wParam, MAPVK_VK_TO_VSC);
		}

		key = opg.win32_.keycodes[scancode];

		// The Ctrl keys require special handling
		if (wParam == VK_CONTROL)
		{
			if (HIWORD(lParam) & KF_EXTENDED)
			{
				key = GLU_KEY_RIGHT_CONTROL;
			}
			else
			{
				// NOTE: Alt Gr sends Left Ctrl followed by Right Alt
				// HACK: We only want one event for Alt Gr, so if we detect
				//       this sequence we discard this Left Ctrl message now
				//       and later report Right Alt normally
				MSG next;
				const DWORD time = GetMessageTime();

				if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
				{
					if (next.message == WM_KEYDOWN ||
						next.message == WM_SYSKEYDOWN ||
						next.message == WM_KEYUP ||
						next.message == WM_SYSKEYUP)
					{
						if (next.wParam == VK_MENU &&
							(HIWORD(next.lParam) & KF_EXTENDED) &&
							next.time == time)
						{
							// Next message is Right Alt down so discard this
							break;
						}
					}
				}

				// This is a regular Left Ctrl message
				key = GLU_KEY_LEFT_CONTROL;
			}
		}
		else if (wParam == VK_PROCESSKEY)
		{
			// IME notifies that keys have been filtered by setting the
			// virtual key-code to VK_PROCESSKEY
			break;
		}

		if (action == GLU_RELEASE && wParam == VK_SHIFT)
		{
			inputKey(window, GLU_KEY_LEFT_SHIFT, scancode, action, mods);
			inputKey(window, GLU_KEY_RIGHT_SHIFT, scancode, action, mods);
		}
		else if (wParam == VK_SNAPSHOT)
		{
			inputKey(window, key, scancode, GLU_PRESS, mods);
			inputKey(window, key, scancode, GLU_RELEASE, mods);
		}
		else
			inputKey(window, key, scancode, action, mods);

		break;
	}

	case WM_ACTIVATEAPP:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
void inputKey(GWindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key <= GLU_KEY_LAST)
	{
		bool repeated = false;
		if (action == GLU_RELEASE && window->keys_[key] == GLU_RELEASE)
			return;

		if (action == GLU_PRESS && window->keys_[key] == GLU_PRESS)
			repeated = true;
		
		window->keys_[key] = (char)action;

		if (repeated)
			action = GLU_REPEAT;
	}

	key_callback(window, key, scancode, action, mods);
}
void defaultWindowHints(void)
{
	opg.hints.framebuffer.redBits = 8;
	opg.hints.framebuffer.greenBits = 8;
	opg.hints.framebuffer.blueBits = 8;
	opg.hints.framebuffer.alphaBits = 8;
	opg.hints.framebuffer.depthBits = 24;
	opg.hints.framebuffer.stencilBits = 8;
	opg.hints.framebuffer.doublebuffer = true;
}

extern bool  createHelperWindow(void)
{
	MSG msg;

	opg.win32_.helperWindowHandle_ =
		CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
			_WNDCLASSNAME,
			"message window",
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0, 1, 1,
			NULL, NULL,
			GetModuleHandle(NULL),
			NULL);

	if (!opg.win32_.helperWindowHandle_)
	{
		PRINT_ERROR("Win32: Failed to create helper window and code");
		//DWORD errorCode = GetLastError();
		return false;
	}

	// HACK: The command to the first ShowWindow call is ignored if the parent
	//       process passed along a STARTUPINFO, so clear that with a no-op call
	ShowWindow(opg.win32_.helperWindowHandle_, SW_HIDE);

	// Register for HID device notifications
	{
		DEV_BROADCAST_DEVICEINTERFACE dbi;
		ZeroMemory(&dbi, sizeof(dbi));
		dbi.dbcc_size = sizeof(dbi);
		dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		dbi.dbcc_classguid = GUID_DEVINTERFACE_HID;

		opg.win32_.deviceNotificationHandle_ =
			RegisterDeviceNotification(opg.win32_.helperWindowHandle_,
			(DEV_BROADCAST_HDR*)&dbi,
				DEVICE_NOTIFY_WINDOW_HANDLE);
	}

	while (PeekMessage(&msg, opg.win32_.helperWindowHandle_, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

bool createNativeWindow( GWindow * win, 
	const char *window_name,
	int width, int height)
{

	int xpos, ypos;
	xpos = 50;
	ypos = 50;

	win->win_.handle_ = CreateWindowEx(
		0,                        // Optional window styles.
		_WNDCLASSNAME,                     // Window class
		window_name,					// Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		xpos,							//// Size and position			
		ypos, 
		width, 
		height,
		NULL,       // Parent window    
		NULL,       // Menu
		GetModuleHandle(NULL),  // Instance handle
		NULL        // Additional application data
	);

	if (win->win_.handle_ == 0)
	{
		PRINT_ERROR("CreateWindow failed!");
		return false;
	}

	SetProp(win->win_.handle_, "GLU", win);

	return true;
}

GWindow * gluCreateWindow(const DeviceConfig*dc, const WindowConfig*wc)
{
	GWindow* window = new GWindow;
	opg.windowListHead_ = window;

	if (!createNativeWindow(window, wc->title_, wc->width_, wc->height_))
	{
		delete window;
		return NULL;
	}

	if (!_initWGL())
	{
		delete window;
		return NULL;
	}

	defaultWindowHints();

	Bconfig* fbconfig = &opg.hints.framebuffer;

	if (_createContextWGL(window, dc, fbconfig))
	{
		if (wc->shouldCreateSharedContext_)
		{
			window->sharedContext_ = new Context;
			window->sharedContext_->wgl_ct_.dc_ = window->context_.wgl_ct_.dc_;

			if (!__createContextWGL(&window->win_.handle_, window->sharedContext_, &window->context_.wgl_ct_.handle_,  dc, fbconfig,true))
			{
				gluDestoryWindow(window);
				return NULL;
			}
		}
	}
	else
	{
		gluDestoryWindow(window);
		return NULL;
	}
	if (!platformRefreshContextAttribs(window, dc))
	{
		gluDestoryWindow(window);
		return NULL;
	}

	platformShowWindow(window);
	platformFocusWindow(window);

	return window;
}

void platformGetCursorPos(GWindow* window, double* xpos, double* ypos)
{
	POINT pos;
	if (GetCursorPos(&pos))
	{
		ScreenToClient(window->win_.handle_, &pos);
		if (xpos)
			*xpos = pos.x;
		if (ypos)
			*ypos = pos.y;
	}
}

bool platformRefreshContextAttribs(GWindow* window, const DeviceConfig* ctxconfig)
{
	return true;
}

void platformShowWindow(GWindow* window)
{
	ShowWindow(window->win_.handle_, SW_SHOWNA);
}

void PlatformDestroyWindow(GWindow*window)
{

	if (window->context_.destroy_)
		window->context_.destroy_(&window->context_);

	if(window->sharedContext_)
		if(window->sharedContext_->destroy_)
			window->sharedContext_->destroy_(window->sharedContext_);

	if (window->win_.handle_)
	{
		RemoveProp(window->win_.handle_, "GLU");
		DestroyWindow(window->win_.handle_);
		window->win_.handle_ = NULL;
	}

	if (window->win_.bigIcon_)
		DestroyIcon(window->win_.bigIcon_);

	if (window->win_.smallIcon_)
		DestroyIcon(window->win_.smallIcon_);
}

void platformPollEvents()
{
	MSG msg;
	HWND handle;
	GWindow* window;

	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			// NOTE: While GLFW does not itself post WM_QUIT, other processes
			//       may post it to this one, for example Task Manager
			// HACK: Treat WM_QUIT as a close on all windows

			window = opg.windowListHead_;
			while (window)
			{
				_inputWindowCloseRequest(window);
				window = window->next_;
			}
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

}

void platformFocusWindow(GWindow* window)
{
	BringWindowToTop(window->win_.handle_);
	SetForegroundWindow(window->win_.handle_);
	SetFocus(window->win_.handle_);
}





GWindow::~GWindow()
{
	if (sharedContext_) {
		delete sharedContext_;
		sharedContext_ = nullptr;
	}
}
