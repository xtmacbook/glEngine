
#include "sys.h"

// Function calling modes
#define EN_CDECL        __cdecl
#define EN_STDCALL      __stdcall
#define EN_FASTCALL     __fastcall
#define EN_DEPRECATED


// Set standard function names.
#if _MSC_VER < 1900
#   define snprintf _snprintf
#endif
#if _MSC_VER < 1500
#   define vsnprintf _vsnprintf
#endif
#if _MSC_VER < 1700
#   define strtoll _strtoi64
#   define strtoull _strtoui64
#endif


#if _MSC_VER < 1800 // Not sure what version introduced this.
#define va_copy(a, b) (a) = (b)
#endif

#if !defined restrict
#define restrict
#endif

// Ignore gcc attributes.
#define __attribute__(X)

#if !defined __FUNC__
#define __FUNC__ __FUNCTION__ 
#endif

#if EN_CC_MSVC
#define EN_MULTI_LINE_MACRO_BEGIN do {  
#define EN_MULTI_LINE_MACRO_END \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
    } while(false) \
    __pragma(warning(pop))  
#else
#define EN_MULTI_LINE_MACRO_BEGIN do {
#define EN_MULTI_LINE_MACRO_END } while(false)
#endif


#include <stdint.h>


// Unwanted VC++ warnings to disable.
/*
#pragma warning(disable : 4244)     // conversion to float, possible loss of data
#pragma warning(disable : 4245)     // conversion from 'enum ' to 'unsigned long', signed/unsigned mismatch
#pragma warning(disable : 4100)     // unreferenced formal parameter
#pragma warning(disable : 4514)     // unreferenced inline function has been removed
#pragma warning(disable : 4710)     // inline function not expanded
#pragma warning(disable : 4127)     // Conditional expression is constant
#pragma warning(disable : 4305)     // truncation from 'const double' to 'float'
#pragma warning(disable : 4505)     // unreferenced local function has been removed

#pragma warning(disable : 4702)     // unreachable code in inline expanded function
#pragma warning(disable : 4711)     // function selected for automatic inlining
#pragma warning(disable : 4725)     // Pentium fdiv bug

#pragma warning(disable : 4786)     // Identifier was truncated and cannot be debugged.

#pragma warning(disable : 4675)     // resolved overload was found by argument-dependent lookup
*/

#pragma warning(1 : 4705)     // Report unused local variables.
#pragma warning(1 : 4555)     // Expression has no effect.
