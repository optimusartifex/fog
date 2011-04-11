// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/IO/DirEntry.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

// ============================================================================
// [Fog::DirEntry]
// ============================================================================

DirEntry::DirEntry()
{
}

DirEntry::DirEntry(const DirEntry& other) :
  _name(other._name),
  _type(other._type),
  _size(other._size)
{
#if defined(FOG_OS_WINDOWS)
  memcpy(&_winFindData, &other._winFindData, sizeof(WIN32_FIND_DATAW));
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  memcpy(&_statInfo, &other._statInfo, sizeof(struct stat));
#endif // FOG_OS_POSIX
}

DirEntry::~DirEntry()
{
}

DirEntry& DirEntry::operator=(const DirEntry& other)
{
  _name = other._name;
  _type = other._type;
  _size = other._size;

#if defined(FOG_OS_WINDOWS)
  memcpy(&_winFindData, &other._winFindData, sizeof(WIN32_FIND_DATAW));
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  memcpy(&_statInfo, &other._statInfo, sizeof(struct stat));
#endif // FOG_OS_POSIX

  return *this;
}

} // Fog namespace
