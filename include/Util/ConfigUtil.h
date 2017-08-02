// **********************************************************************
//
// Copyright (c) 2003-2013 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef UTIL_CONFIG_H
#define UTIL_CONFIG_H

//
// Endianness
//
// Most CPUs support only one endianness, with the notable exceptions
// of Itanium (IA64) and MIPS.
//
#ifdef __GLIBC__
# include <endian.h>
#endif

#if defined(__i386)     || defined(_M_IX86) || defined(__x86_64)  || \
    defined(_M_X64)     || defined(_M_IA64) || defined(__alpha__) || \
    defined(__ARMEL__) || defined(_M_ARM_FP) || \
    defined(__MIPSEL__) || (defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN))
#   define LITTLE_ENDIAN
#elif defined(__sparc) || defined(__sparc__) || defined(__hppa)      || \
      defined(__ppc__) || defined(__powerpc) || defined(_ARCH_COM) || \
      defined(__MIPSEB__) || (defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN))
#   define BIG_ENDIAN
#else
#   error "Unknown architecture"
#endif

//
// 32 or 64 bit mode?
//
#if defined(__sun) && (defined(__sparcv9) || defined(__x86_64))    || \
      defined(__linux) && defined(__x86_64)                        || \
      defined(__hppa) && defined(__LP64__)                         || \
      defined(_ARCH_COM) && defined(__64BIT__)                     || \
      defined(__alpha__)                                           || \
      defined(_WIN64)
#   define UTIL_64
#else
#   define UTIL_32
#endif

//
// Check for C++ 11 support
// 
// We cannot just check for C++11 mode as some features were not 
// implemented in first versions of the compilers.
//
// The require compiler version should be equal or greater than
// VC100, G++ 4.5, Clang Apple 4.2 or Clang 3.2 (Unsupported).
//
#if (defined(__GNUC__) && (((__GNUC__* 100) + __GNUC_MINOR__) >= 405) && defined(__GXX_EXPERIMENTAL_CXX0X__)) || \
    (defined(__clang__) && \
      ((defined(__apple_build_version__) && (((__clang_major__ * 100) + __clang_minor__) >= 402)) || \
       (!defined(__apple_build_version__) && (((__clang_major__ * 100) + __clang_minor__) >= 302))) && \
      __cplusplus >= 201103) || \
    (defined(_MSC_VER) && (_MSC_VER >= 1600))
#   define CPP11
#elif __cplusplus >= 201103 || defined(__GXX_EXPERIMENTAL_CXX0X__)
#   error Unsupported C++11 compiler
#endif

#if defined(CPP11) && !defined(_MSC_VER)

// Visual Studio does not support noexcept yet
#   define NOEXCEPT noexcept
#   define NOEXCEPT_FALSE noexcept(false)
#else
#   define NOEXCEPT throw()
#   define NOEXCEPT_FALSE /**/
#endif

//
// Visual Studio 2012 or later, without Windows XP/2003 support
//
#if defined(_MSC_VER) && (_MSC_VER >= 1700) && !defined(_USING_V110_SDK71_)

//
// Check if building for WinRT
//
#   include <winapifamily.h>
#   if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#      define OS_WINRT
#      define STATIC_LIBS
#   endif

//
// Windows provides native condition variables on Vista and later,
// and Visual Studio 2012 with the default Platform Toolset (vc100) no 
// longer supports Windows XP or Windows Server 2003.
//
// You can "switch-on" this macro to use native condition variables with
// other C++ compilers on Windows.
//
#   define HAS_WIN32_CONDVAR 
#endif

//
// Compiler extensions to export and import symbols: see the documentation 
// for Visual C++, Solaris Studio and HP aC++.
//
#if (defined(_MSC_VER) && !defined(STATIC_LIBS)) || \
    (defined(__HP_aCC) && defined(__HP_WINDLL))
#   define DECLSPEC_EXPORT __declspec(dllexport)
#   define DECLSPEC_IMPORT __declspec(dllimport)
#   define HAS_DECLSPEC_IMPORT_EXPORT
#elif defined(__SUNPRO_CC)
#   define DECLSPEC_EXPORT __global
#   define DECLSPEC_IMPORT /**/
#else
#   define DECLSPEC_EXPORT /**/
#   define DECLSPEC_IMPORT /**/
#endif

//
// Let's use these extensions with Util:
//
#ifdef UTIL_EXPORTS
#   define UTIL_API DECLSPEC_EXPORT
#else
#   define UTIL_API DECLSPEC_IMPORT
#endif


#if defined(_MSC_VER)
#   define DEPRECATED_API __declspec(deprecated)
#elif defined(__GNUC__)
#   define DEPRECATED_API __attribute__((deprecated))
#else
#   define DEPRECATED_API /**/
#endif

#ifdef _WIN32
#   if !defined(STATIC_LIBS) && defined(_MSC_VER) && (!defined(_DLL) || !defined(_MT))
#       error "Only multi-threaded DLL libraries can be used with Ice!"
#   endif

#   include <windows.h>
#endif

//
// Some include files we need almost everywhere.
//
#include <cassert>
#include <iostream>
#include <sstream>

#ifndef _WIN32
#   include <pthread.h>
#   include <errno.h>
#endif

#ifdef _MSC_VER
//
// Move some warnings to level 4
//
#   pragma warning( 4 : 4250 ) // ... : inherits ... via dominance
#   pragma warning( 4 : 4251 ) // class ... needs to have dll-interface to be used by clients of class ..
#endif

namespace Util
{

//
// By deriving from this class, other classes are made non-copyable.
//
class UTIL_API noncopyable
{
protected:

    noncopyable() { }
    ~noncopyable() { } // May not be virtual! Classes without virtual 
                       // operations also derive from noncopyable.

private:

    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};

//
// Int64 typedef
//
#ifdef _MSC_VER
//
// With Visual C++, long is always 32-bit
//
typedef __int64 Int64;
#elif defined(UTIL_64)
typedef long Int64;
#else
typedef long long Int64;
#endif

}

//
// INT64: macro for Int64 literal values
//
#if defined(_MSC_VER)
#   define INT64(n) n##i64
#elif defined(UTIL_64)
#   define INT64(n) n##L
#else
#   define INT64(n) n##LL
#endif

//
// The Ice version.
//
#define STRING_VERSION "1.0.0" // "A.B.C", with A=major, B=minor, C=patch
#define INT_VERSION 100000      // AABBCC, with AA=major, BB=minor, CC=patch

#endif
