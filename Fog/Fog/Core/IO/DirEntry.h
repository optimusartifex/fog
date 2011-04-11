// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_IO_DIRENTRY_H
#define _FOG_CORE_IO_DIRENTRY_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/String.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
#include <sys/stat.h>
#include <dirent.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_IO
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct DirIterator;

// ============================================================================
// [Fog::DirEntry]
// ============================================================================

//! @brief DirIterator entry.
struct FOG_API DirEntry
{
  DirEntry();
  DirEntry(const DirEntry& other);
  ~DirEntry();

  DirEntry& operator=(const DirEntry& other);

  FOG_INLINE const String& getName() const { return _name; }
  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE uint64_t getSize() const { return _size; }

#if defined(FOG_OS_WINDOWS)
  FOG_INLINE const WIN32_FIND_DATAW& getWinFindData() const { return _winFindData; }
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  FOG_INLINE const struct stat& getStatInfo() const { return _statInfo; }
#endif // FOG_OS_POSIX

  //! @brief  entry type.
  enum TYPE
  {
    TYPE_UNKNOWN = 0,
    TYPE_FILE = (1 << 0),
    TYPE_DIRECTORY = (1 << 1),
    TYPE_CHAR_DEVICE = (1 << 2),
    TYPE_BLOCK_DEVICE = (1 << 3),
    TYPE_FIFO = (1 << 4),
    TYPE_LINK = (1 << 5),
    TYPE_SOCKET = (1 << 6)
  };

private:
  String _name;
  uint32_t _type;
  uint64_t _size;

#if defined(FOG_OS_WINDOWS)
  WIN32_FIND_DATAW _winFindData;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  struct stat _statInfo;
#endif // FOG_OS_POSIX

  friend struct DirIterator;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::DirEntry, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_CORE_IO_DIRENTRY_H
