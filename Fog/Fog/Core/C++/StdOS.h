// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CPP_STDOS_H
#define _FOG_CORE_CPP_STDOS_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CPP_BASE_H)
#error "Fog/Core/C++/StdOS.h can be only included by Fog/Core/C++/Base.h"
#endif // _FOG_CORE_CPP_BASE_H

// ============================================================================
// [Fog::Core::C++ - OS]
// ============================================================================

//! @addtogroup Fog_Core_Cpp
//! @{

//! @def FOG_OS_WINDOWS
//! @brief Operating system is windows.

#if !defined(FOG_OS_DEFINED) && \
    (defined(__WIN32) || defined(_WIN32) || defined(_WINDOWS))
# define FOG_OS_DEFINED
# define FOG_OS_WINDOWS
#endif // __WIN32 || _WIN32 || _WINDOWS

//! @def FOG_OS_POSIX
//! @brief Portable check if operating system is like unix (linux, freebsd, ...)
//!
//! @note Posix is always defined if Fog is not compiled for Windows. We treat
//! all other operating systems as posix and we use posix API a lot. Differences
//! are usually handled in places where needed (Linux/BSD/MacOSX specific code)

//! @def FOG_OS_LINUX
//! @brief Operating system is linux.

#if !defined(FOG_OS_DEFINED) && \
    (defined(linux) || defined(__linux) || defined(__linux__))
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_LINUX
#endif // linux || __linux || __linux__

//! @def FOG_OS_FREEBSD
//! @brief Operating system is freebsd.

#if !defined(FOG_OS_DEFINED) && defined(__FreeBSD__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_BSD
# define FOG_OS_FREEBSD
#endif // __FreeBSD__

//! @def FOG_OS_OPENBSD
//! @brief Operating system is openbsd.

#if !defined(FOG_OS_DEFINED) && defined(__OpenBSD__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_BSD
# define FOG_OS_OPENBSD
#endif // __OpenBSD__

//! @def FOG_OS_NETBSD
//! @brief Operating system is netbsd.

#if !defined(FOG_OS_DEFINED) && defined(__NetBSD__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_BSD
# define FOG_OS_NETBSD
#endif // __NetBSD__

//! @def FOG_OS_HPUX
//! @brief Operating system is netbsd.

#if !defined(FOG_OS_DEFINED) && defined(__HPUX__)
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_HPUX
#endif // __HPUX__

#if !defined(FOG_OS_DEFINED) && (defined(__DARWIN__) || defined(__APPLE__))
# define FOG_OS_DEFINED
# define FOG_OS_POSIX
# define FOG_OS_MAC

# include <TargetConditionals.h>
# if TARGET_OS_IPHONE
#  define FOG_OS_IOS
# endif // TARGET_OS_IPHONE

#endif // __DARWIN__ || __APPLE__

//! @}

#endif // _FOG_CORE_CPP_STDOS_H
