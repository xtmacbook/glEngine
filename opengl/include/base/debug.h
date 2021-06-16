#pragma once

#ifndef __Debug_H_
#define __Debug_H_

#include "decl.h"
#include <stdarg.h> // va_list
#include "sys.h"

#if defined EN_OS_WIN32 || defined EN_OS_WIN64
#include "defsVcWin32.h"
#endif // EN_OS_WIN32

#undef assert

#define EN_ABORT_DEBUG      1
#define EN_ABORT_IGNORE     2
#define EN_ABORT_EXIT       3

#define enNoAssert(exp) \
    EN_MULTI_LINE_MACRO_BEGIN \
    (void)sizeof(exp); \
    EN_MULTI_LINE_MACRO_END


#if defined EN_OS_WIN32 
#       define enDebugBreak()       __debugbreak()
#   else
#       error "No kaDebugBreak()!"
#   endif

#define enAssertMacro(exp) \
    EN_MULTI_LINE_MACRO_BEGIN \
    if (!(exp)) { \
        if (enAbort(#exp, __FILE__, __LINE__, __FUNC__) == EN_ABORT_DEBUG) { \
            enDebugBreak(); \
        } \
    } \
    EN_MULTI_LINE_MACRO_END

#define enCheckMacro(exp) \
    (\
        (exp) ? true : ( \
            (enAbort(#exp, __FILE__, __LINE__, __FUNC__) == EN_ABORT_DEBUG) ? (enDebugBreak(), true) : ( false ) \
        ) \
    )

#define enAssert(exp)    enAssertMacro(exp)
#define enCheck(exp)     enAssertMacro(exp)

#if defined(_DEBUG)
#   define enDebugAssert(exp)   enAssertMacro(exp)
#   define enDebugCheck(exp)    enAssertMacro(exp)
#else // _DEBUG
#   define enDebugAssert(exp)   enNoAssert(exp)
#   define enDebugCheck(exp)    enNoAssert(exp)
#endif // _DEBUG


#define enError(x)      enAbort(x, __FILE__, __LINE__, __FUNC__)
#define enWarning(x)    enDebugPrint("*** Warning %s/%d: %s\n", __FILE__, __LINE__, (x))

#ifndef EN_DEBUG_PRINT
#define EN_DEBUG_PRINT 1 //defined(_DEBUG)
#endif

#if EN_DEBUG_PRINT
#define enDebug(...)    enDebugPrint(__VA_ARGS__)
#else
#if _MSC_VER
#define enDebug(...)    __noop(__VA_ARGS__)
#else
#define enDebug(...)    ((void)0) // Non-msvc platforms do not evaluate arguments?
#endif
#endif

LIBENIGHT_EXPORT int enAbort(const char *exp, const char *file, int line, const char * func = NULL, const char * msg = NULL, ...) __attribute__((format(printf, 5, 6)));
LIBENIGHT_EXPORT void EN_CDECL enDebugPrint(const char *msg, ...) __attribute__((format(printf, 1, 2)));

namespace base
{
	struct MessageHandler {
		virtual void log(const char * str, va_list arg) = 0;
		virtual ~MessageHandler() {}
	};

	// Assert handler interface.
	struct AssertHandler {
		virtual int assertion(const char *exp, const char *file, int line, const char *func, const char *msg, va_list arg) = 0;
		virtual ~AssertHandler() {}
	};

	namespace debug
	{
		LIBENIGHT_EXPORT void dumpInfo();
		LIBENIGHT_EXPORT void dumpCallstack(MessageHandler *messageHandler, int callstackLevelsToSkip = 0);

		LIBENIGHT_EXPORT void setMessageHandler(MessageHandler * messageHandler);
		LIBENIGHT_EXPORT void resetMessageHandler();

		LIBENIGHT_EXPORT void setAssertHandler(AssertHandler * assertHanlder);
		LIBENIGHT_EXPORT void resetAssertHandler();

		LIBENIGHT_EXPORT void enableSigHandler(bool interactive);
		LIBENIGHT_EXPORT void disableSigHandler();

		LIBENIGHT_EXPORT bool isDebuggerPresent();
		LIBENIGHT_EXPORT bool attachToDebugger();

		LIBENIGHT_EXPORT void terminate(int code);
	}
}

#endif