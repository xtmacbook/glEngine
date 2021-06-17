//
//  sys.h
//  OpenGL
//
//  Created by xt on 16/1/30.
//  Copyright (c) 2016Äê xt. All rights reserved.
//


#include <posh/posh.h>

#if defined POSH_OS_LINUX
#   define EN_OS_LINUX 1
#   define EN_OS_UNIX 1
#elif defined POSH_OS_ORBIS
#   define EN_OS_ORBIS 1
#elif defined POSH_OS_FREEBSD
#   define EN_OS_FREEBSD 1
#   define EN_OS_UNIX 1
#elif defined POSH_OS_NETBSD
#   define EN_OS_NETBSD 1
#   define EN_OS_UNIX 1
#elif defined POSH_OS_OPENBSD
#   define EN_OS_OPENBSD 1
#   define EN_OS_UNIX 1
#elif defined POSH_OS_CYGWIN32
#   define EN_OS_CYGWIN 1
#elif defined POSH_OS_MINGW
#   define EN_OS_MINGW 1
#   define EN_OS_WIN32 1
#elif defined POSH_OS_OSX
#   define EN_OS_DARWIN 1
#   define EN_OS_UNIX 1
#elif defined POSH_OS_IOS
#   define EN_OS_DARWIN 1 //ACS should we keep this on IOS?
#   define EN_OS_UNIX 1
#   define EN_OS_IOS 1
#elif defined POSH_OS_UNIX
#   define EN_OS_UNIX 1
#elif defined POSH_OS_WIN64
#   define EN_OS_WIN32 1
#   define EN_OS_WIN64 1
#elif defined POSH_OS_WIN32
#   define EN_OS_WIN32 1
#elif defined POSH_OS_XBOX
#   define EN_OS_XBOX 1
#else
#   error "Unsupported OS"
#endif

#if defined POSH_CPU_X86_64
//#   define NV_CPU_X86 1
#   define EN_CPU_X86_64 1
#elif defined POSH_CPU_X86
#   define EN_CPU_X86 1
#elif defined POSH_CPU_PPC
#   define EN_CPU_PPC 1
#elif defined POSH_CPU_STRONGARM
#   define EN_CPU_ARM 1
#elif defined POSH_CPU_AARCH64
#   define EN_CPU_AARCH64 1
#elif defined POSH_CPU_E2K
#   define EN_CPU_E2K 1
#else
#   error "Unsupported CPU"
#endif

// Endiannes:
#define EN_LITTLE_ENDIAN    POSH_LITTLE_ENDIAN
#define EN_BIG_ENDIAN       POSH_BIG_ENDIAN
#define EN_ENDIAN_STRING    POSH_ENDIAN_STRING

// Compiler:
// EV_CC_GNUC
// EV_CC_MSVC
// EV_CC_CLANG
#if defined POSH_COMPILER_CLANG
#   define EN_CC_CLANG  1
#   define EN_CC_GNUC   1    // Clang is compatible with GCC.
#   define EN_CC_STRING "clang"
#elif defined POSH_COMPILER_GCC
#   define EN_CC_GNUC   1
#   define EN_CC_STRING "gcc"
#elif defined POSH_COMPILER_MSVC
#   define EN_CC_MSVC   1
#   define EN_CC_STRING "msvc"
#else
#   error "Unsupported compiler"
#endif
#if EN_CC_MSVC
#define EN_CC_CPP11 (__cplusplus > 199711L || _MSC_VER >= 1800) // Visual Studio 2013 has all the features we use, but doesn't advertise full C++11 support yet.
#else
// @@ IC: This works in CLANG, about GCC?
// @@ ES: Doesn't work in gcc. These 3 features are available in GCC >= 4.4.
#ifdef __clang__
#define EN_CC_CPP11 (__has_feature(cxx_deleted_functions) && __has_feature(cxx_rvalue_references) && __has_feature(cxx_static_assert))
#elif defined __GNUC__ 
#define EN_CC_CPP11 ( __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
#endif
#endif


// Disable copy constructor and assignment operator. 
#if EN_CC_CPP11
#define EN_FORBID_COPY(C) \
    C( const C & ) = delete; \
    C &operator=( const C & ) = delete
#else
#define EN_FORBID_COPY(C) \
    private: \
    C( const C & ); \
    C &operator=( const C & )
#endif

#if !defined(SWIG)
#define BIT(x)   (1u << (x))
#define BIT64(x) (1ull << (x))
#else
#define BIT(x)   (1 << (x))
#define BIT64(x)   (1 << (x))
#endif

#if defined POSH_COMPILER_CLANG
#include "compiler/clangspecific.h"
#elif defined POSH_COMPILER_GCC
#include "compiler/gCCspecific.h"
#elif defined POSH_COMPILER_MSVC
#include "compiler/mSVCspecific.h"
#else
#   error "Unsupported compiler"
#endif

#if defined POSH_COMPILER_MSVC
#include <windows.h>
#endif


