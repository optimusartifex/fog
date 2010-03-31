// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FILEUTIL_H
#define _FOG_CORE_FILEUTIL_H

// [Dependencies]
#include <Fog/Core/String.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {
namespace FileUtil {

// ============================================================================
// [Fog::FileUtil]
// ============================================================================

FOG_API err_t extractFile(String& dst, const String& path);
FOG_API err_t extractDirectory(String& dst, const String& path);
FOG_API err_t extractExtension(String& dst, const String& path);
FOG_API err_t normalizePath(String& dst, const String& path);
FOG_API err_t toAbsolutePath(String& dst, const String& base, const String& path);
FOG_API err_t joinPath(String& dst, const String& base, const String& part);

FOG_API bool isPathContainsFile(const String& path, const String& file, uint cs = CASE_SENSITIVE);
FOG_API bool isPathContainsDirectory(const String& path, const String& directory, uint cs = CASE_SENSITIVE);
FOG_API bool isPathContainsExtension(const String& path, const String& extension, uint cs = CASE_SENSITIVE);
FOG_API bool isNormalizedPath(const String& path);
FOG_API bool isAbsolutePath(const String& path);

FOG_API bool testLocalName(const String& path);

#if defined(FOG_OS_WINDOWS)
static const char directorySeparatorA = '\\';
static const Char directorySeparatorU = Char('\\');

static const char pathSeparatorA = ';';
static const Char pathSeparatorU = Char(';');
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
static const char directorySeparatorA = '/';
static const Char directorySeparatorU = Char('/');

static const char pathSeparatorA = ':';
static const Char pathSeparatorU = Char(':');
#endif // FOG_OS_POSIX

} // FileUtil namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_FILEUTIL_H
