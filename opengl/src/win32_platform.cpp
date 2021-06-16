#include "glinter.h"
#include "win32_platform.h"

extern bool registerWindowClassWin32();
extern bool  createHelperWindow(void);
extern void unregisterWindowClassWin32();

bool WindowsVersionOrGreaterWin32(WORD major, WORD minor, WORD sp)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0,{ 0 }, sp };
	DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
	ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
	// HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
	//       latter lies unless the user knew to embed a non-default manifest
	//       announcing support for Windows 10 via supportedOS GUID
	return RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
}

bool loadLibraries()
{
	opg.win32_.ntdll.instance_ = LoadLibrary("ntdll.dll");
	if (opg.win32_.ntdll.instance_)
	{
		opg.win32_.ntdll.RtlVerifyVersionInfo_ = (PFN_RtlVerifyVersionInfo)
			GetProcAddress(opg.win32_.ntdll.instance_, "RtlVerifyVersionInfo");
	}

	opg.win32_.dwmapi.instance_ = LoadLibrary("dwmapi.dll");
	if (opg.win32_.dwmapi.instance_)
	{
		opg.win32_.dwmapi.IsCompositionEnabled_ = (PFN_DwmIsCompositionEnabled)
			GetProcAddress(opg.win32_.dwmapi.instance_, "DwmIsCompositionEnabled");
		opg.win32_.dwmapi.Flush_ = (PFN_DwmFlush)
			GetProcAddress(opg.win32_.dwmapi.instance_, "DwmFlush");
		opg.win32_.dwmapi.EnableBlurBehindWindow_ = (PFN_DwmEnableBlurBehindWindow)
			GetProcAddress(opg.win32_.dwmapi.instance_, "DwmEnableBlurBehindWindow");
	}

	return true;
}
static void createKeyTables(void)
{
	int scancode;

	memset(opg.win32_.keycodes, -1, sizeof(opg.win32_.keycodes));
	memset(opg.win32_.scancodes, -1, sizeof(opg.win32_.scancodes));

	opg.win32_.keycodes[0x00B] = GLU_KEY_0;
	opg.win32_.keycodes[0x002] = GLU_KEY_1;
	opg.win32_.keycodes[0x003] = GLU_KEY_2;
	opg.win32_.keycodes[0x004] = GLU_KEY_3;
	opg.win32_.keycodes[0x005] = GLU_KEY_4;
	opg.win32_.keycodes[0x006] = GLU_KEY_5;
	opg.win32_.keycodes[0x007] = GLU_KEY_6;
	opg.win32_.keycodes[0x008] = GLU_KEY_7;
	opg.win32_.keycodes[0x009] = GLU_KEY_8;
	opg.win32_.keycodes[0x00A] = GLU_KEY_9;
	opg.win32_.keycodes[0x01E] = GLU_KEY_A;
	opg.win32_.keycodes[0x030] = GLU_KEY_B;
	opg.win32_.keycodes[0x02E] = GLU_KEY_C;
	opg.win32_.keycodes[0x020] = GLU_KEY_D;
	opg.win32_.keycodes[0x012] = GLU_KEY_E;
	opg.win32_.keycodes[0x021] = GLU_KEY_F;
	opg.win32_.keycodes[0x022] = GLU_KEY_G;
	opg.win32_.keycodes[0x023] = GLU_KEY_H;
	opg.win32_.keycodes[0x017] = GLU_KEY_I;
	opg.win32_.keycodes[0x024] = GLU_KEY_J;
	opg.win32_.keycodes[0x025] = GLU_KEY_K;
	opg.win32_.keycodes[0x026] = GLU_KEY_L;
	opg.win32_.keycodes[0x032] = GLU_KEY_M;
	opg.win32_.keycodes[0x031] = GLU_KEY_N;
	opg.win32_.keycodes[0x018] = GLU_KEY_O;
	opg.win32_.keycodes[0x019] = GLU_KEY_P;
	opg.win32_.keycodes[0x010] = GLU_KEY_Q;
	opg.win32_.keycodes[0x013] = GLU_KEY_R;
	opg.win32_.keycodes[0x01F] = GLU_KEY_S;
	opg.win32_.keycodes[0x014] = GLU_KEY_T;
	opg.win32_.keycodes[0x016] = GLU_KEY_U;
	opg.win32_.keycodes[0x02F] = GLU_KEY_V;
	opg.win32_.keycodes[0x011] = GLU_KEY_W;
	opg.win32_.keycodes[0x02D] = GLU_KEY_X;
	opg.win32_.keycodes[0x015] = GLU_KEY_Y;
	opg.win32_.keycodes[0x02C] = GLU_KEY_Z;

	opg.win32_.keycodes[0x028] = GLU_KEY_APOSTROPHE;
	opg.win32_.keycodes[0x02B] = GLU_KEY_BACKSLASH;
	opg.win32_.keycodes[0x033] = GLU_KEY_COMMA;
	opg.win32_.keycodes[0x00D] = GLU_KEY_EQUAL;
	opg.win32_.keycodes[0x029] = GLU_KEY_GRAVE_ACCENT;
	opg.win32_.keycodes[0x01A] = GLU_KEY_LEFT_BRACKET;
	opg.win32_.keycodes[0x00C] = GLU_KEY_MINUS;
	opg.win32_.keycodes[0x034] = GLU_KEY_PERIOD;
	opg.win32_.keycodes[0x01B] = GLU_KEY_RIGHT_BRACKET;
	opg.win32_.keycodes[0x027] = GLU_KEY_SEMICOLON;
	opg.win32_.keycodes[0x035] = GLU_KEY_SLASH;
	opg.win32_.keycodes[0x056] = GLU_KEY_WORLD_2;

	opg.win32_.keycodes[0x00E] = GLU_KEY_BACKSPACE;
	opg.win32_.keycodes[0x153] = GLU_KEY_DELETE;
	opg.win32_.keycodes[0x14F] = GLU_KEY_END;
	opg.win32_.keycodes[0x01C] = GLU_KEY_ENTER;
	opg.win32_.keycodes[0x001] = GLU_KEY_ESCAPE;
	opg.win32_.keycodes[0x147] = GLU_KEY_HOME;
	opg.win32_.keycodes[0x152] = GLU_KEY_INSERT;
	opg.win32_.keycodes[0x15D] = GLU_KEY_MENU;
	opg.win32_.keycodes[0x151] = GLU_KEY_PAGE_DOWN;
	opg.win32_.keycodes[0x149] = GLU_KEY_PAGE_UP;
	opg.win32_.keycodes[0x045] = GLU_KEY_PAUSE;
	opg.win32_.keycodes[0x146] = GLU_KEY_PAUSE;
	opg.win32_.keycodes[0x039] = GLU_KEY_SPACE;
	opg.win32_.keycodes[0x00F] = GLU_KEY_TAB;
	opg.win32_.keycodes[0x03A] = GLU_KEY_CAPS_LOCK;
	opg.win32_.keycodes[0x145] = GLU_KEY_NUM_LOCK;
	opg.win32_.keycodes[0x046] = GLU_KEY_SCROLL_LOCK;
	opg.win32_.keycodes[0x03B] = GLU_KEY_F1;
	opg.win32_.keycodes[0x03C] = GLU_KEY_F2;
	opg.win32_.keycodes[0x03D] = GLU_KEY_F3;
	opg.win32_.keycodes[0x03E] = GLU_KEY_F4;
	opg.win32_.keycodes[0x03F] = GLU_KEY_F5;
	opg.win32_.keycodes[0x040] = GLU_KEY_F6;
	opg.win32_.keycodes[0x041] = GLU_KEY_F7;
	opg.win32_.keycodes[0x042] = GLU_KEY_F8;
	opg.win32_.keycodes[0x043] = GLU_KEY_F9;
	opg.win32_.keycodes[0x044] = GLU_KEY_F10;
	opg.win32_.keycodes[0x057] = GLU_KEY_F11;
	opg.win32_.keycodes[0x058] = GLU_KEY_F12;
	opg.win32_.keycodes[0x064] = GLU_KEY_F13;
	opg.win32_.keycodes[0x065] = GLU_KEY_F14;
	opg.win32_.keycodes[0x066] = GLU_KEY_F15;
	opg.win32_.keycodes[0x067] = GLU_KEY_F16;
	opg.win32_.keycodes[0x068] = GLU_KEY_F17;
	opg.win32_.keycodes[0x069] = GLU_KEY_F18;
	opg.win32_.keycodes[0x06A] = GLU_KEY_F19;
	opg.win32_.keycodes[0x06B] = GLU_KEY_F20;
	opg.win32_.keycodes[0x06C] = GLU_KEY_F21;
	opg.win32_.keycodes[0x06D] = GLU_KEY_F22;
	opg.win32_.keycodes[0x06E] = GLU_KEY_F23;
	opg.win32_.keycodes[0x076] = GLU_KEY_F24;
	opg.win32_.keycodes[0x038] = GLU_KEY_LEFT_ALT;
	opg.win32_.keycodes[0x01D] = GLU_KEY_LEFT_CONTROL;
	opg.win32_.keycodes[0x02A] = GLU_KEY_LEFT_SHIFT;
	opg.win32_.keycodes[0x15B] = GLU_KEY_LEFT_SUPER;
	opg.win32_.keycodes[0x137] = GLU_KEY_PRINT_SCREEN;
	opg.win32_.keycodes[0x138] = GLU_KEY_RIGHT_ALT;
	opg.win32_.keycodes[0x11D] = GLU_KEY_RIGHT_CONTROL;
	opg.win32_.keycodes[0x036] = GLU_KEY_RIGHT_SHIFT;
	opg.win32_.keycodes[0x15C] = GLU_KEY_RIGHT_SUPER;
	opg.win32_.keycodes[0x150] = GLU_KEY_DOWN;
	opg.win32_.keycodes[0x14B] = GLU_KEY_LEFT;
	opg.win32_.keycodes[0x14D] = GLU_KEY_RIGHT;
	opg.win32_.keycodes[0x148] = GLU_KEY_UP;

	opg.win32_.keycodes[0x052] = GLU_KEY_KP_0;
	opg.win32_.keycodes[0x04F] = GLU_KEY_KP_1;
	opg.win32_.keycodes[0x050] = GLU_KEY_KP_2;
	opg.win32_.keycodes[0x051] = GLU_KEY_KP_3;
	opg.win32_.keycodes[0x04B] = GLU_KEY_KP_4;
	opg.win32_.keycodes[0x04C] = GLU_KEY_KP_5;
	opg.win32_.keycodes[0x04D] = GLU_KEY_KP_6;
	opg.win32_.keycodes[0x047] = GLU_KEY_KP_7;
	opg.win32_.keycodes[0x048] = GLU_KEY_KP_8;
	opg.win32_.keycodes[0x049] = GLU_KEY_KP_9;
	opg.win32_.keycodes[0x04E] = GLU_KEY_KP_ADD;
	opg.win32_.keycodes[0x053] = GLU_KEY_KP_DECIMAL;
	opg.win32_.keycodes[0x135] = GLU_KEY_KP_DIVIDE;
	opg.win32_.keycodes[0x11C] = GLU_KEY_KP_ENTER;
	opg.win32_.keycodes[0x059] = GLU_KEY_KP_EQUAL;
	opg.win32_.keycodes[0x037] = GLU_KEY_KP_MULTIPLY;
	opg.win32_.keycodes[0x04A] = GLU_KEY_KP_SUBTRACT;

	for (scancode = 0; scancode < 512; scancode++)
	{
		if (opg.win32_.keycodes[scancode] > 0)
			opg.win32_.scancodes[opg.win32_.keycodes[scancode]] = scancode;
	}
}

static void freeLibraries(void)
{

	if (opg.win32_.dwmapi.instance_)
		FreeLibrary(opg.win32_.dwmapi.instance_);
	if (opg.win32_.ntdll.instance_)
		FreeLibrary(opg.win32_.ntdll.instance_);
}

bool platformInit(void)
{
	if (opg.initialized_) return true;
	memset(&opg, 0, sizeof(opg));

	if (!loadLibraries()) return false;

	if (!registerWindowClassWin32()) 
		return false;

	if (!createHelperWindow())
		return false;

	createKeyTables();

	if (!base::createTls(&opg.contextSlot_))
		return false;
	
	opg.initialized_ = true;

	return true;
}

void platformTerminate()
{
	if (opg.win32_.helperWindowHandle_)
		DestroyWindow(opg.win32_.helperWindowHandle_);
	unregisterWindowClassWin32();
	_terminateWgl();
	freeLibraries();
}

