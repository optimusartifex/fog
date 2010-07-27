// [Fog-Core Library - Public API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_BUILD_CONFIG_H
#define _FOG_BUILD_CONFIG_H

// ============================================================================
// [Library Directories]
// ============================================================================

//! @brief Install prefix specified by configure parameters.
#define FOG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}"

// ============================================================================
// [Debugging]
// ============================================================================

#cmakedefine FOG_DEBUG
#cmakedefine FOG_DEBUG_MEMORY

// ============================================================================
// [Byte Order]
// ============================================================================

//! @brief Target byteorder, can be: 
//!   - @c FOG_LITTLE_ENDIAN
//!   - @c FOG_BIG_ENDIAN. 
#define FOG_BYTE_ORDER ${FOG_BYTE_ORDER}

// ============================================================================
// [Size of Types]
// ============================================================================

#define FOG_SIZEOF_VOID ${FOG_SIZEOF_VOID}
#define FOG_SIZEOF_SHORT ${FOG_SIZEOF_SHORT}
#define FOG_SIZEOF_INT ${FOG_SIZEOF_INT}
#define FOG_SIZEOF_LONG ${FOG_SIZEOF_LONG}
#define FOG_SIZEOF_WCHAR_T ${FOG_SIZEOF_WCHAR_T}

// ============================================================================
// [Header Files]
// ============================================================================

//! @brief Defined if we have fenv.h
#cmakedefine FOG_HAVE_FENV_H
//! @brief Defined if we have float.h
#cmakedefine FOG_HAVE_FLOAT_H
//! @brief Defined if we have limits.h
#cmakedefine FOG_HAVE_LIMITS_H
//! @brief Defined if we have stdint.h
#cmakedefine FOG_HAVE_STDINT_H
//! @brief Defined if we have unistd.h
#cmakedefine FOG_HAVE_UNISTD_H

// ============================================================================
// [Dynamic Loadable Library Support]
// ============================================================================

//! @brief Defined if we are using Windows dynamic library loader.
#cmakedefine FOG_LIBRARY_WINDOWS
//! @brief Defined if we are using Linux @c dlopen() dynamic library loader.
#cmakedefine FOG_LIBRARY_DL

// ============================================================================
// [Libraries]
// ============================================================================

//! @brief Defined if jpeg library is available.
#cmakedefine FOG_HAVE_LIBJPEG
//! @brief Defined if png library is available.
#cmakedefine FOG_HAVE_LIBPNG

// ============================================================================
// [Fog/Font Backends]
// ============================================================================

//! @brief Used for windows font subsystem.
#cmakedefine FOG_FONT_WINDOWS

//! @brief Used for mac font subsystem.
#cmakedefine FOG_FONT_MAC

//! @brief Used for fontconfig support (Unix/Linux).
#cmakedefine FOG_HAVE_FONTCONFIG
//! @brief Used if freetype library is located
//! (can be used together with Windows font subsystem).
#cmakedefine FOG_FONT_FREETYPE

// ============================================================================
// [Fog/Gui Backends]
// ============================================================================

//! @brief Defined if we can use Windows GUI.
#cmakedefine FOG_GUI_WINDOWS
//! @brief Defined if we can use X Window System (this not means that we depend to it).
#cmakedefine FOG_GUI_X11

// ============================================================================
// [MMX/SSE support]
// ============================================================================

//! @brief Build Fog with MMX support
#cmakedefine FOG_BUILD_MMX
//! @brief Build Fog with MMXExt support
#cmakedefine FOG_BUILD_MMXEXT
//! @brief Build Fog with SSE support
#cmakedefine FOG_BUILD_SSE
//! @brief Build Fog with SSE2 support
#cmakedefine FOG_BUILD_SSE2

// [Guard]
#endif // _FOG_BUILD_CONFIG_H
