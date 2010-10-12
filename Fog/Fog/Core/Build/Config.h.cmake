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
// [Debug]
// ============================================================================

#cmakedefine FOG_DEBUG
#cmakedefine FOG_DEBUG_MEMORY

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
// [Libraries]
// ============================================================================

//! @brief Defined if jpeg library is available.
#cmakedefine FOG_HAVE_LIBJPEG
//! @brief Defined if png library is available.
#cmakedefine FOG_HAVE_LIBPNG

// ============================================================================
// [Fog-Font Backends]
// ============================================================================

//! @brief Whether to build Windows font support.
//!
//! This is default when using Windows.
#cmakedefine FOG_FONT_WINDOWS
//! @brief Whether to build Mac font support.
//!
//! This is default when using Mac.
#cmakedefine FOG_FONT_MAC
//! @brief Whether to build FreeType support
//!
//! This is default when using Linux/BSD. May be used together with Windows/Mac
//! font support.
#cmakedefine FOG_FONT_FREETYPE
//! @brief Whether to build FontConfig support.
//!
//! Can be enabled only if FOG_FONT_FREETYPE is defined.
#cmakedefine FOG_HAVE_FONTCONFIG

// ============================================================================
// [Fog-Gui Backends]
// ============================================================================

//! @brief Whether to build Windows GUI support.
#cmakedefine FOG_GUI_WINDOWS
//! @brief Defined to build Mac GUI support.
#cmakedefine FOG_GUI_MAC
//! @brief Whether to build X11 GUI support (plugin).
#cmakedefine FOG_GUI_X11

// [Guard]
#endif // _FOG_BUILD_CONFIG_H
