#include "log.h"
#include "window_w32.h"
#include "windowManager.h"
#include "thread.h"
#include "debug.h"

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <winerror.h>
#define  DONT_CARE -1

DLLLibrary opg;

extern void InputErrorWin32(int error, const char* description);

bool stringInExtensionString(const char* string, const char* extensions)
{
	const char* start = extensions;

	for (;;)
	{
		const char* where;
		const char* terminator;

		where = strstr(start, string);
		if (!where)
			return false;

		terminator = where + strlen(string);
		if (where == start || *(where - 1) == ' ')
		{
			if (*terminator == ' ' || *terminator == '\0')
				break;
		}

		start = terminator;
	}

	return true;
}

static int findPixelFormatAttribValue(const int* attribs,
	int attribCount,
	const int* values,
	int attrib)
{
	int i;

	for (i = 0; i < attribCount; i++)
	{
		if (attribs[i] == attrib)
			return values[i];
	}

	PRINT_ERROR("WGL: Unknown pixel format attribute requested");
	return 0;
}
#define addAttrib(a) \
{ \
    assert((size_t) attribCount < sizeof(attribs) / sizeof(attribs[0])); \
    attribs[attribCount++] = a; \
}

#define setAttrib(a, v) \
{ \
    assert(((size_t) index + 1) < sizeof(attribs) / sizeof(attribs[0])); \
    attribs[index++] = a; \
    attribs[index++] = v; \
}

#define findAttribValue(a) \
    findPixelFormatAttribValue(attribs, attribCount, values, a)

static int extensionSupportedWGL(const char* extension)
{
	const char* extensions = NULL;

	if (opg.wgl.GetExtensionsStringARB)
		extensions = wglGetExtensionsStringARB(wglGetCurrentDC());
	else if (opg.wgl.GetExtensionsStringEXT)
		extensions = wglGetExtensionsStringEXT();

	if (!extensions)
		return false;

	return stringInExtensionString(extension, extensions);
}

static void makeContextCurrentWGL(Context* context)
{
	if (context)
	{
		if (wglMakeCurrent(context->wgl_ct_.dc_, context->wgl_ct_.handle_))
			platformSetTls(&opg.contextSlot_, context);
		else
		{
			PRINT_ERROR("WGL: Failed to make context current");
			platformSetTls(&opg.contextSlot_, NULL);
		}
	}
	else
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			PRINT_ERROR("WGL: Failed to clear current context");
		}

		platformSetTls(&opg.contextSlot_, NULL);
	}
}

static void swapBuffersWGL(Context* context)
{
	//if (!window->monitor_)
	{
		if (IsWindowsVistaOrGreater())
		{
			// DWM Composition is always enabled on Win8+
			BOOL enabled = IsWindows8OrGreater();

			// HACK: Use DwmFlush when desktop composition is enabled
			if (enabled ||
				(SUCCEEDED(DwmIsCompositionEnabled(&enabled)) && enabled))
			{
				int count = abs(context->wgl_ct_.interval_);
				while (count--)
					DwmFlush();
			}
		}
	}

	SwapBuffers(context->wgl_ct_.dc_);
}

static void swapIntervalWGL(int interval)
{
	Context* context = (Context*)platformGetTls(&opg.contextSlot_);

	context->wgl_ct_.interval_ = interval;

	//if (!window->monitor_)
	{
		if (IsWindowsVistaOrGreater())
		{
			// DWM Composition is always enabled on Win8+
			BOOL enabled = IsWindows8OrGreater();

			// HACK: Disable WGL swap interval when desktop composition is enabled to
			//       avoid interfering with DWM vsync
			if (enabled ||
				(SUCCEEDED(DwmIsCompositionEnabled(&enabled)) && enabled))
				interval = 0;
		}
	}

	if (opg.wgl.EXT_swap_control)
		wglSwapIntervalEXT(interval);
}

static GLFWglproc getProcAddressWGL(const char* procname)
{
	const GLFWglproc proc = (GLFWglproc)wglGetProcAddress(procname);
	if (proc)
		return proc;

	return (GLFWglproc)GetProcAddress(opg.wgl.instance, procname);
}

static void destroyContextWGL(Context* context)
{
	if (context->wgl_ct_.handle_)
	{
		wglDeleteContext(context->wgl_ct_.handle_);
		context->wgl_ct_.handle_ = NULL;
	}
}

// Initialize WGL
//
bool _initWGL(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	HGLRC prc, rc;
	HDC pdc, dc;

	if (opg.wgl.instance)
		return true;

	opg.wgl.instance = LoadLibrary("opengl32.dll");
	if (!opg.wgl.instance)
	{
		PRINT_ERROR("WGL: Failed to load opengl32.dll");
		return false;
	}

	opg.wgl.CreateContext = (PFN_wglCreateContext)
		GetProcAddress(opg.wgl.instance, "wglCreateContext");
	opg.wgl.DeleteContext = (PFN_wglDeleteContext)
		GetProcAddress(opg.wgl.instance, "wglDeleteContext");
	opg.wgl.GetProcAddress = (PFN_wglGetProcAddress)
		GetProcAddress(opg.wgl.instance, "wglGetProcAddress");
	opg.wgl.GetCurrentDC = (PFN_wglGetCurrentDC)
		GetProcAddress(opg.wgl.instance, "wglGetCurrentDC");
	opg.wgl.GetCurrentContext = (PFN_wglGetCurrentContext)
		GetProcAddress(opg.wgl.instance, "wglGetCurrentContext");
	opg.wgl.MakeCurrent = (PFN_wglMakeCurrent)
		GetProcAddress(opg.wgl.instance, "wglMakeCurrent");
	opg.wgl.ShareLists = (PFN_wglShareLists)
		GetProcAddress(opg.wgl.instance, "wglShareLists");

	// NOTE: A dummy context has to be created for opengl32.dll to load the
	//       OpenGL ICD, from which we can then query WGL extensions
	// NOTE: This code will accept the Microsoft GDI ICD; accelerated context
	//       creation failure occurs during manual pixel format enumeration

	dc = GetDC(opg.win32_.helperWindowHandle_);

	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;

	if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd))
	{
		PRINT_ERROR("WGL: Failed to set pixel format for dummy context");
		return false;
	}

	rc = wglCreateContext(dc);
	if (!rc)
	{
		PRINT_ERROR("WGL: Failed to create dummy context");
		return false;
	}

	pdc = wglGetCurrentDC();
	prc = wglGetCurrentContext();

	if (!wglMakeCurrent(dc, rc))
	{
		PRINT_ERROR("WGL: Failed to make dummy context current");
		wglMakeCurrent(pdc, prc);
		wglDeleteContext(rc);
		return false;
	}

	// NOTE: Functions must be loaded first as they're needed to retrieve the
	//       extension string that tells us whether the functions are supported
	opg.wgl.GetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)
		wglGetProcAddress("wglGetExtensionsStringEXT");
	opg.wgl.GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
		wglGetProcAddress("wglGetExtensionsStringARB");
	opg.wgl.CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
		wglGetProcAddress("wglCreateContextAttribsARB");
	opg.wgl.SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
		wglGetProcAddress("wglSwapIntervalEXT");
	opg.wgl.GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
		wglGetProcAddress("wglGetPixelFormatAttribivARB");

	// NOTE: WGL_ARB_extensions_string and WGL_EXT_extensions_string are not
	//       checked below as we are already using them
	opg.wgl.ARB_multisample =
		extensionSupportedWGL("WGL_ARB_multisample");
	opg.wgl.ARB_framebuffer_sRGB =
		extensionSupportedWGL("WGL_ARB_framebuffer_sRGB");
	opg.wgl.EXT_framebuffer_sRGB =
		extensionSupportedWGL("WGL_EXT_framebuffer_sRGB");
	opg.wgl.ARB_create_context =
		extensionSupportedWGL("WGL_ARB_create_context");
	opg.wgl.ARB_create_context_profile =
		extensionSupportedWGL("WGL_ARB_create_context_profile");
	opg.wgl.EXT_create_context_es2_profile =
		extensionSupportedWGL("WGL_EXT_create_context_es2_profile");
	opg.wgl.ARB_create_context_robustness =
		extensionSupportedWGL("WGL_ARB_create_context_robustness");
	opg.wgl.ARB_create_context_no_error =
		extensionSupportedWGL("WGL_ARB_create_context_no_error");
	opg.wgl.EXT_swap_control =
		extensionSupportedWGL("WGL_EXT_swap_control");
	opg.wgl.EXT_colorspace =
		extensionSupportedWGL("WGL_EXT_colorspace");
	opg.wgl.ARB_pixel_format =
		extensionSupportedWGL("WGL_ARB_pixel_format");
	opg.wgl.ARB_context_flush_control =
		extensionSupportedWGL("WGL_ARB_context_flush_control");

	wglMakeCurrent(pdc, prc);
	wglDeleteContext(rc);
	return true;
}

static const Bconfig * chooseFBConfig(const Bconfig* desired,
	const Bconfig* alternatives, unsigned int count)
{
	unsigned int i;
	unsigned int missing, leastMissing = UINT_MAX;
	unsigned int colorDiff, leastColorDiff = UINT_MAX;
	unsigned int extraDiff, leastExtraDiff = UINT_MAX;
	const Bconfig* current;
	const Bconfig* closest = NULL;

	for (i = 0; i < count; i++)
	{
		current = alternatives + i;

		if (desired->stereo > 0 && current->stereo == 0)
		{
			// Stereo is a hard constraint
			continue;
		}

		if (desired->doublebuffer != current->doublebuffer)
		{
			// Double buffering is a hard constraint
			continue;
		}

		// Count number of missing buffers
		{
			missing = 0;

			if (desired->alphaBits > 0 && current->alphaBits == 0)
				missing++;

			if (desired->depthBits > 0 && current->depthBits == 0)
				missing++;

			if (desired->stencilBits > 0 && current->stencilBits == 0)
				missing++;

			if (desired->auxBuffers > 0 &&
				current->auxBuffers < desired->auxBuffers)
			{
				missing += desired->auxBuffers - current->auxBuffers;
			}

			if (desired->samples > 0 && current->samples == 0)
			{
				// Technically, several multisampling buffers could be
				// involved, but that's a lower level implementation detail and
				// not important to us here, so we count them as one
				missing++;
			}

			if (desired->transparent != current->transparent)
				missing++;
		}

		// These polynomials make many small channel size differences matter
		// less than one large channel size difference

		// Calculate color channel size difference value
		{
			colorDiff = 0;

			if (desired->redBits != DONT_CARE)
			{
				colorDiff += (desired->redBits - current->redBits) *
					(desired->redBits - current->redBits);
			}

			if (desired->greenBits != DONT_CARE)
			{
				colorDiff += (desired->greenBits - current->greenBits) *
					(desired->greenBits - current->greenBits);
			}

			if (desired->blueBits != DONT_CARE)
			{
				colorDiff += (desired->blueBits - current->blueBits) *
					(desired->blueBits - current->blueBits);
			}
		}

		// Calculate non-color channel size difference value
		{
			extraDiff = 0;

			if (desired->alphaBits != DONT_CARE)
			{
				extraDiff += (desired->alphaBits - current->alphaBits) *
					(desired->alphaBits - current->alphaBits);
			}

			if (desired->depthBits != DONT_CARE)
			{
				extraDiff += (desired->depthBits - current->depthBits) *
					(desired->depthBits - current->depthBits);
			}

			if (desired->stencilBits != DONT_CARE)
			{
				extraDiff += (desired->stencilBits - current->stencilBits) *
					(desired->stencilBits - current->stencilBits);
			}

			if (desired->accumRedBits != DONT_CARE)
			{
				extraDiff += (desired->accumRedBits - current->accumRedBits) *
					(desired->accumRedBits - current->accumRedBits);
			}

			if (desired->accumGreenBits != DONT_CARE)
			{
				extraDiff += (desired->accumGreenBits - current->accumGreenBits) *
					(desired->accumGreenBits - current->accumGreenBits);
			}

			if (desired->accumBlueBits != DONT_CARE)
			{
				extraDiff += (desired->accumBlueBits - current->accumBlueBits) *
					(desired->accumBlueBits - current->accumBlueBits);
			}

			if (desired->accumAlphaBits != DONT_CARE)
			{
				extraDiff += (desired->accumAlphaBits - current->accumAlphaBits) *
					(desired->accumAlphaBits - current->accumAlphaBits);
			}

			if (desired->samples != DONT_CARE)
			{
				extraDiff += (desired->samples - current->samples) *
					(desired->samples - current->samples);
			}

			if (desired->sRGB && !current->sRGB)
				extraDiff++;
		}

		// Figure out if the current one is better than the best one found so far
		// Least number of missing buffers is the most important heuristic,
		// then color buffer size match and lastly size match for other buffers

		if (missing < leastMissing)
			closest = current;
		else if (missing == leastMissing)
		{
			if ((colorDiff < leastColorDiff) ||
				(colorDiff == leastColorDiff && extraDiff < leastExtraDiff))
			{
				closest = current;
			}
		}

		if (current == closest)
		{
			leastMissing = missing;
			leastColorDiff = colorDiff;
			leastExtraDiff = extraDiff;
		}
	}

	return closest;
}

static int _choosePixelFormat(HDC* hdc, const DeviceConfig*dc, const Bconfig * fbconfig)

{
	Bconfig* usableConfigs;
	const Bconfig* closest;
	int i, pixelFormat, nativeCount, usableCount = 0, attribCount = 0;
	int attribs[40];
	int values[sizeof(attribs) / sizeof(attribs[0])];

	if (opg.wgl.ARB_pixel_format)
	{
		const int attrib = WGL_NUMBER_PIXEL_FORMATS_ARB;

		if (!wglGetPixelFormatAttribivARB(*hdc,
			1, 0, 1, &attrib, &nativeCount))
		{
			PRINT_ERROR("WGL: Failed to retrieve pixel format attribute");
			return 0;
		}
		addAttrib(WGL_SUPPORT_OPENGL_ARB);
		addAttrib(WGL_DRAW_TO_WINDOW_ARB);
		addAttrib(WGL_PIXEL_TYPE_ARB);
		addAttrib(WGL_ACCELERATION_ARB);
		addAttrib(WGL_RED_BITS_ARB);
		addAttrib(WGL_RED_SHIFT_ARB);
		addAttrib(WGL_GREEN_BITS_ARB);
		addAttrib(WGL_GREEN_SHIFT_ARB);
		addAttrib(WGL_BLUE_BITS_ARB);
		addAttrib(WGL_BLUE_SHIFT_ARB);
		addAttrib(WGL_ALPHA_BITS_ARB);
		addAttrib(WGL_ALPHA_SHIFT_ARB);
		addAttrib(WGL_DEPTH_BITS_ARB);
		addAttrib(WGL_STENCIL_BITS_ARB);
		addAttrib(WGL_ACCUM_BITS_ARB);
		addAttrib(WGL_ACCUM_RED_BITS_ARB);
		addAttrib(WGL_ACCUM_GREEN_BITS_ARB);
		addAttrib(WGL_ACCUM_BLUE_BITS_ARB);
		addAttrib(WGL_ACCUM_ALPHA_BITS_ARB);
		addAttrib(WGL_AUX_BUFFERS_ARB);
		addAttrib(WGL_STEREO_ARB);
		addAttrib(WGL_DOUBLE_BUFFER_ARB);


		if (opg.wgl.ARB_multisample)
			addAttrib(WGL_SAMPLES_ARB);

		if (opg.wgl.ARB_framebuffer_sRGB || opg.wgl.EXT_framebuffer_sRGB)
			addAttrib(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
	}
	else
	{
		nativeCount = DescribePixelFormat(*hdc,
			1,
			sizeof(PIXELFORMATDESCRIPTOR),
			NULL);
	}

	usableConfigs = (Bconfig*)calloc(nativeCount, sizeof(Bconfig));
	for (i = 0; i < nativeCount; i++)
	{
		Bconfig* u = usableConfigs + usableCount;
		pixelFormat = i + 1;
		if (opg.wgl.ARB_pixel_format)
		{
			if (!wglGetPixelFormatAttribivARB(*hdc,
				pixelFormat, 0,
				attribCount,
				attribs, values))
			{

				InputErrorWin32(0x00010008,
					"WGL: Failed to retrieve pixel format attributes");
				free(usableConfigs);
				return 0;
			}

			if (!findAttribValue(WGL_SUPPORT_OPENGL_ARB) ||
				!findAttribValue(WGL_DRAW_TO_WINDOW_ARB))
			{
				continue;
			}

			if (findAttribValue(WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB)
				continue;

			if (findAttribValue(WGL_ACCELERATION_ARB) == WGL_NO_ACCELERATION_ARB)
				continue;

			u->redBits = findAttribValue(WGL_RED_BITS_ARB);
			u->greenBits = findAttribValue(WGL_GREEN_BITS_ARB);
			u->blueBits = findAttribValue(WGL_BLUE_BITS_ARB);
			u->alphaBits = findAttribValue(WGL_ALPHA_BITS_ARB);

			u->depthBits = findAttribValue(WGL_DEPTH_BITS_ARB);
			u->stencilBits = findAttribValue(WGL_STENCIL_BITS_ARB);

			u->accumRedBits = findAttribValue(WGL_ACCUM_RED_BITS_ARB);
			u->accumGreenBits = findAttribValue(WGL_ACCUM_GREEN_BITS_ARB);
			u->accumBlueBits = findAttribValue(WGL_ACCUM_BLUE_BITS_ARB);
			u->accumAlphaBits = findAttribValue(WGL_ACCUM_ALPHA_BITS_ARB);

			u->auxBuffers = findAttribValue(WGL_AUX_BUFFERS_ARB);

			if (findAttribValue(WGL_STEREO_ARB))
				u->stereo = true;
			if (findAttribValue(WGL_DOUBLE_BUFFER_ARB))
				u->doublebuffer = true;

			if (opg.wgl.ARB_multisample)
				u->samples = findAttribValue(WGL_SAMPLES_ARB);

			if (opg.wgl.ARB_framebuffer_sRGB ||
				opg.wgl.EXT_framebuffer_sRGB)
			{
				if (findAttribValue(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB))
					u->sRGB = true;
			}
		}
		else
		{
			// Get pixel format attributes through legacy PFDs

			PIXELFORMATDESCRIPTOR pfd;

			if (!DescribePixelFormat(*hdc,
				pixelFormat,
				sizeof(PIXELFORMATDESCRIPTOR),
				&pfd))
			{
				PRINT_ERROR("WGL: Failed to describe pixel format");

				free(usableConfigs);
				return 0;
			}

			if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW) ||
				!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
			{
				continue;
			}

			if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) &&
				(pfd.dwFlags & PFD_GENERIC_FORMAT))
			{
				continue;
			}

			if (pfd.iPixelType != PFD_TYPE_RGBA)
				continue;

			u->redBits = pfd.cRedBits;
			u->greenBits = pfd.cGreenBits;
			u->blueBits = pfd.cBlueBits;
			u->alphaBits = pfd.cAlphaBits;

			u->depthBits = pfd.cDepthBits;
			u->stencilBits = pfd.cStencilBits;

			u->accumRedBits = pfd.cAccumRedBits;
			u->accumGreenBits = pfd.cAccumGreenBits;
			u->accumBlueBits = pfd.cAccumBlueBits;
			u->accumAlphaBits = pfd.cAccumAlphaBits;

			u->auxBuffers = pfd.cAuxBuffers;

			if (pfd.dwFlags & PFD_STEREO)
				u->stereo = true;
			if (pfd.dwFlags & PFD_DOUBLEBUFFER)
				u->doublebuffer = true;
		}

		u->handle = pixelFormat;
		usableCount++;
	}
	if (!usableCount)
	{
		PRINT_ERROR("WGL: The driver does not appear to support OpenGL");

		free(usableConfigs);
		return 0;
	}

	closest = chooseFBConfig(fbconfig, usableConfigs, usableCount);
	if (!closest)
	{
		PRINT_ERROR("WGL: Failed to find a suitable pixel format");

		free(usableConfigs);
		return 0;
	}

	pixelFormat = (int)closest->handle;
	free(usableConfigs);

	return pixelFormat;
}

 

bool ___createContextWGL(HWND* hwnd, HDC* hdc, HGLRC* hglrc,HGLRC*share, const DeviceConfig *dc, const Bconfig* fbconfig, bool shared)
{
	if (opg.wgl.ARB_create_context)
	{
		if (!shared)
		{
			int attribs[40];
			int pixelFormat;
			PIXELFORMATDESCRIPTOR pfd;
			*hdc = GetDC(*hwnd);
			if (!*hdc)
			{
				PRINT_ERROR("WGL: Failed to retrieve DC for window");
				return false;
			}

			pixelFormat = _choosePixelFormat(hdc, dc, fbconfig);

			if (!pixelFormat)
				return false;

			if (!DescribePixelFormat(*hdc,
				pixelFormat, sizeof(pfd), &pfd))
			{
				PRINT_ERROR("WGL: Failed to retrieve PFD for selected pixel format");
				return false;
			}

			if (!SetPixelFormat(*hdc, pixelFormat, &pfd))
			{
				PRINT_ERROR("WGL: Failed to set selected pixel format");

				DWORD errorCode = GetLastError();

				return false;
			}

			if (dc->glVersion_.glforward_compat_)
			{
				if (!opg.wgl.ARB_create_context)
				{
					PRINT_ERROR("WGL: A forward compatible OpenGL context requested but WGL_ARB_create_context is unavailable");
					return false;
				}
			}

			if (dc->glVersion_.glProfile_)
			{
				if (!opg.wgl.ARB_create_context_profile)
				{
					PRINT_ERROR("WGL: OpenGL profile requested but WGL_ARB_create_context_profile is unavailable");
					return false;
				}
			}


			if (opg.wgl.ARB_create_context)
			{
				int index = 0, mask = 0, flags = 0;

				if (dc->glVersion_.glforward_compat_)
					flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
				if (dc->glVersion_.glProfile_ == Version_Config::OPENGL_CORE_PROFILE)
					mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
				else if (dc->glVersion_.glProfile_ == Version_Config::OPENGL_COMPAT_PROFILE)
					mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

				if (dc->glContext_.debug_context_)
				{
					flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
				}

				if (dc->glContext_.robustness_)
				{
					if (opg.wgl.ARB_create_context_robustness)
					{
						if (dc->glContext_.robustness_ == Context_Config::NO_RESET_NOTIFICATION)
						{
							setAttrib(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
								WGL_NO_RESET_NOTIFICATION_ARB);
						}
						else if (dc->glContext_.robustness_ == Context_Config::LOSE_CONTEXT_ON_RESET)
						{
							setAttrib(WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
								WGL_LOSE_CONTEXT_ON_RESET_ARB);
						}

						flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
					}
				}

				if (dc->glContext_.robustness_)
				{
					if (opg.wgl.ARB_context_flush_control)
					{
						if (dc->glContext_.release_ == Context_Config::RELEASE_BEHAVIOR_NONE)
						{
							setAttrib(WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
								WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB);
						}
						else if (dc->glContext_.release_ == Context_Config::RELEASE_BEHAVIOR_FLUSH)
						{
							setAttrib(WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
								WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB);
						}
					}
				}

				if (dc->glContext_.noerror_)
				{
					if (opg.wgl.ARB_create_context_no_error)
						setAttrib(WGL_CONTEXT_OPENGL_NO_ERROR_ARB, true);
				}

				setAttrib(WGL_CONTEXT_MAJOR_VERSION_ARB, dc->glVersion_.glMaxJor_);
				setAttrib(WGL_CONTEXT_MINOR_VERSION_ARB, dc->glVersion_.glMinJor_);

				if (flags)
					setAttrib(WGL_CONTEXT_FLAGS_ARB, flags);

				if (mask)
					setAttrib(WGL_CONTEXT_PROFILE_MASK_ARB, mask);

				setAttrib(0, 0);

			}
			*hglrc =
				wglCreateContextAttribsARB(*hdc, *share, attribs);
		}
		else
		{
			*hglrc = wglCreateContextAttribsARB(*hdc, *share, NULL);
		}

		if (!*hglrc)
		{
			const DWORD error = GetLastError();
			if (error == (0xc0070000 | ERROR_INVALID_VERSION_ARB))
			{
				char err[56];
				snprintf(err, 56, "WGL: Driver does not support OpenGL version %i.%i", dc->glVersion_.glMaxJor_, dc->glVersion_.glMinJor_);
				PRINT_ERROR(err);
			}
			else if (error == (0xc0070000 | ERROR_INVALID_PROFILE_ARB))
			{
				PRINT_ERROR("WGL: Driver does not support the requested OpenGL profile");
			}
			else if (error == (0xc0070000 | ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB))
			{
				PRINT_ERROR("WGL: The share context is not compatible with the requested context");
			}
			else
			{
				PRINT_ERROR("WGL: Failed to create OpenGL context");

			}

			return false;
		}
	}
	else
	{
		*hglrc = wglCreateContext(*hdc);
		if (!*hglrc)
		{
			PRINT_ERROR("WGL: Failed to create OpenGL context");
			return false;
		}

		if (*share)
		{
			if (!wglShareLists(*share, *hglrc))
			{
				PRINT_ERROR("WGL: Failed to enable sharing with specified OpenGL context");
				return false;
			}
		}
	}
}

bool __createContextWGL(HWND* hand, Context*wgl, HGLRC*share, const DeviceConfig *dc, const Bconfig* fbconfig, bool shared)
{
	if (fbconfig == NULL)
	{
		Bconfig* bcf = &opg.hints.framebuffer;
		if (!___createContextWGL(hand, &(wgl->wgl_ct_.dc_), &wgl->wgl_ct_.handle_, share, dc, bcf,shared)) return false;
	}
	else
	{
		if (!___createContextWGL(hand, &(wgl->wgl_ct_.dc_), &wgl->wgl_ct_.handle_, share, dc, fbconfig,shared)) return false;
	}

	wgl->makeCurrent_ = makeContextCurrentWGL;
	wgl->swapBuffers_ = swapBuffersWGL;
	wgl->swapInterval_ = swapIntervalWGL;
	wgl->extensionSupported_ = extensionSupportedWGL;
	wgl->getProcAddress_ = getProcAddressWGL;
	wgl->destroy_ = destroyContextWGL;

	return true;
}

bool _createContextWGL(GWindow * win, const DeviceConfig* dc, const Bconfig* fbconfig)
{
	HGLRC share = 0;
	return __createContextWGL(&win->win_.handle_,&win->context_, &share, dc, fbconfig);
}

void _terminateWgl()
{
	if (opg.wgl.instance)
		FreeLibrary(opg.wgl.instance);
}
