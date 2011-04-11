// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_IO_DIRITERATOR_H
#define _FOG_CORE_IO_DIRITERATOR_H

// [Dependencies]
#include <Fog/Core/IO/DirEntry.h>

namespace Fog {

//! @addtogroup Fog_Core_IO
//! @{

// ============================================================================
// [Fog::DirIterator]
// ============================================================================

//! @brief DirIterator browsing class.
//!
//! This class can be used to browse a directory.
//!
//! @code
//! // Code example to successfully parse current directory
//!
//! // Open a directory stream with a given path, path will be normalized,
//! // so we can use "" and "." for current directory or for example ".." for
//! // parent of current directory
//! Fog::DirIterator dir(".");
//! Fog::DirEntry entry;
//!
//! // We can check if directory is successfully opened, bud @c read() return
//! // false if not, so this check isn't necessary here.
//! while (dir.read(entry))
//! {
//!   // We are readed directory entry, check it by entry.type() and
//!   // use its name by entry.name(). See @c DirEntry for details.
//! }
//!
//! // Close directory stream if opened
//! dir.close();
//! @endcode
struct FOG_API DirIterator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DirIterator();
  DirIterator(const String& path);
  ~DirIterator();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  err_t open(const String& path);
  void close();
  bool read(DirEntry& to);

  err_t rewind();
  int64_t tell();

  FOG_INLINE bool isOpen() const  { return _handle != NULL; }

  FOG_INLINE bool getSkipDots() const { return _skipDots; }
  FOG_INLINE void setSkipDots(bool val) { _skipDots = val; }

  FOG_INLINE void* getHandle() const { return _handle; }
  FOG_INLINE const String& getPath() const { return _path; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  void* _handle;
  String _path;

#if defined(FOG_OS_WINDOWS)
  WIN32_FIND_DATAW _winFindData;
  int64_t _position;
  bool _fileInEntry;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  ByteArray _pathCache;
  sysuint_t _pathCacheBaseLength;
  union {
    struct dirent _dent;
    uint8_t _dentBuf[offsetof(struct dirent, d_name) + _POSIX_NAME_MAX + 1];
  };
#endif // FOG_OS_POSIX

  bool _skipDots;

private:
  FOG_DISABLE_COPY(DirIterator)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_IO_DIRITERATOR_H
