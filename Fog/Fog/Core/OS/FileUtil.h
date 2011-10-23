// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_FILEUTIL_H
#define _FOG_CORE_OS_FILEUTIL_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>

// [Dependencies - C]
#if defined(FOG_OS_POSIX)
# include <sys/stat.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::FileUtil]
// ============================================================================

struct FOG_NO_EXPORT FileUtil
{
  // --------------------------------------------------------------------------
  // [Test / Check]
  // --------------------------------------------------------------------------

  static FOG_INLINE uint32_t test(const StubW& path, uint32_t flags)
  {
    return fog_api.fileutil_testStubW(&path, flags);
  }

  static FOG_INLINE uint32_t test(const StringW& path, uint32_t flags)
  {
    return fog_api.fileutil_testStringW(&path, flags);
  }

  static FOG_INLINE bool testLocalName(const StubW& path)
  {
    return fog_api.fileutil_testLocalNameStubW(&path);
  }

  static FOG_INLINE bool testLocalName(const StringW& path)
  {
    return fog_api.fileutil_testLocalNameStringW(&path);
  }

  static FOG_INLINE bool exists(const StubW& fileName)
  {
    return fog_api.fileutil_testStubW(&fileName, FILE_INFO_EXISTS) == FILE_INFO_EXISTS;
  }

  static FOG_INLINE bool exists(const StringW& fileName)
  {
    return fog_api.fileutil_testStringW(&fileName, FILE_INFO_EXISTS) == FILE_INFO_EXISTS;
  }

  static FOG_INLINE bool isFile(const StubW& fileName)
  {
    return fog_api.fileutil_testStubW(&fileName, FILE_INFO_REGULAR_FILE) == FILE_INFO_REGULAR_FILE;
  }

  static FOG_INLINE bool isFile(const StringW& fileName)
  {
    return fog_api.fileutil_testStringW(&fileName, FILE_INFO_REGULAR_FILE) == FILE_INFO_REGULAR_FILE;
  }

  static FOG_INLINE bool isDirectory(const StubW& fileName)
  {
    return fog_api.fileutil_testStubW(&fileName, FILE_INFO_DIRECTORY) == FILE_INFO_DIRECTORY;
  }

  static FOG_INLINE bool isDirectory(const StringW& fileName)
  {
    return fog_api.fileutil_testStringW(&fileName, FILE_INFO_DIRECTORY) == FILE_INFO_DIRECTORY;
  }

  // --------------------------------------------------------------------------
  // [Find]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool findFile(StringW& dst, const StringW& fileName, const List<StringW>& paths)
  {
    return fog_api.fileutil_findFile(&dst, &fileName, &paths);
  }

  // --------------------------------------------------------------------------
  // [Directory]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t createDirectory(const StringW& dir, bool recursive = true)
  {
    return fog_api.fileutil_createDirectory(&dir, recursive);
  }

  static FOG_INLINE err_t deleteDirectory(const StringW& dir)
  {
    return fog_api.fileutil_deleteDirectory(&dir);
  }

  // --------------------------------------------------------------------------
  // [Posix Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_POSIX)
  static FOG_INLINE int stat(struct stat* dst, const StubW& fileName)
  {
    return fog_api.fileutil_statStubW(dst, &fileName);
  }

  static FOG_INLINE int stat(struct stat* dst, const StringW& fileName)
  {
    return fog_api.fileutil_statStringW(dst, &fileName);
  }
#endif // FOG_OS_POSIX
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_OS_FILEUTIL_H
