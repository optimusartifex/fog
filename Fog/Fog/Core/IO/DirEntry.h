// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_IO_DIRENTRY_H
#define _FOG_CORE_IO_DIRENTRY_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/String.h>

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <sys/stat.h>
# include <dirent.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_IO
//! @{

// ============================================================================
// [Fog::DirEntry]
// ============================================================================

//! @brief Directory entry.
struct FOG_NO_EXPORT DirEntry
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE DirEntry()
  {
    _api.direntry.ctor(this);
  }

  FOG_INLINE DirEntry(const DirEntry& other)
  {
    _api.direntry.ctorCopy(this, &other);
  }

  FOG_INLINE ~DirEntry()
  {
    _api.direntry.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getName() const { return _name; }
  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE uint64_t getSize() const { return _size; }

#if defined(FOG_OS_WINDOWS)
  FOG_INLINE const WIN32_FIND_DATAW& getWinFindData() const { return _winFindData; }
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  FOG_INLINE const struct stat& getPosixStatData() const { return _posixStatData; }
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE DirEntry& operator=(const DirEntry& other)
  {
    _api.direntry.setDirEntry(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<StringW> _name;
  uint32_t _type;
  uint64_t _size;

#if defined(FOG_OS_WINDOWS)
  WIN32_FIND_DATAW _winFindData;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  struct stat _posixStatData;
#endif // FOG_OS_POSIX
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_IO_DIRENTRY_H
