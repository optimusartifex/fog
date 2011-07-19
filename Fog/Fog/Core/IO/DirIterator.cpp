// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/IO/DirEntry.h>
#include <Fog/Core/IO/DirIterator.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/ByteArrayTmp_p.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

#if defined(FOG_OS_WINDOWS)
#include <io.h>
#else
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#if defined(FOG_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#endif

namespace Fog {

// ============================================================================
// [Fog::DirIterator - Windows]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

DirIterator::DirIterator() :
  _handle(NULL),
  _position(-1),
  _skipDots(true)
{
  memset(&_winFindData, 0, sizeof(WIN32_FIND_DATAW));
}

DirIterator::DirIterator(const String& path) :
  _handle(NULL),
  _position(-1),
  _skipDots(true)
{
  memset(&_winFindData, 0, sizeof(WIN32_FIND_DATAW));
  open(path);
}

DirIterator::~DirIterator()
{
  if (_handle) close();
}

err_t DirIterator::open(const String& path)
{
  if (_handle) close();

  StringTmp<TEMPORARY_LENGTH> pathAbs;
  StringTmp<TEMPORARY_LENGTH> t;

  err_t err;

  if ((err = FileSystem::toAbsolutePath(pathAbs, String(), path)) ||
      (err = t.set(pathAbs)) ||
      (err = t.append(Ascii8("\\*"))) ||
      (err = t.slashesToWin()))
  {
    return err;
  }

  _handle = (void*)FindFirstFileW(reinterpret_cast<const wchar_t*>(t.getData()), &_winFindData);

  // Try to open with long file name support if path is too long
  if (_handle == (void*)INVALID_HANDLE_VALUE)
  {
    err = GetLastError();
    if (err == ERROR_PATH_NOT_FOUND)
    {
      // TODO: Make this generic.
      FOG_RETURN_ON_ERROR(t.prepend(Ascii8("\\\\?\\")));

      _handle = (void*)FindFirstFileW(reinterpret_cast<const wchar_t*>(t.getData()), &_winFindData);
      err = GetLastError();
    }

    if (_handle == (void*)INVALID_HANDLE_VALUE)
    {
      _handle = NULL;
      return err;
    }
  }

  _path = pathAbs;
  _position = 0;

  return ERR_OK;
}

void DirIterator::close()
{
  if (!_handle) return;

  ::FindClose((HANDLE)(_handle));
  _handle = NULL;
  _position = -1;
  _path.clear();
  memset(&_winFindData, 0, sizeof(WIN32_FIND_DATAW));
}

bool DirIterator::read(DirEntry& dirEntry)
{
  if (!_handle) return false;

  if (_position != 0)
  {
    // Read next file entry.
_Next:
    if (!::FindNextFileW(_handle, &dirEntry._winFindData)) return false;
  }
  else
  {
    memcpy(&dirEntry._winFindData, &_winFindData, sizeof(WIN32_FIND_DATAW));
  }

  _position++;

  // We have valid file entry in dirEntry._winFindData.
  if (dirEntry._winFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
  {
    dirEntry._type = DirEntry::TYPE_DIRECTORY;

    // Skip "." and ".."
    if (_skipDots)
    {
      const WCHAR* cfn = dirEntry._winFindData.cFileName;

      if (cfn[0] == TEXT('.') && ((cfn[1] == 0) || (cfn[1] == TEXT('.') && cfn[2] == 0) ))
        goto _Next;
    }
  }
  else
  {
    dirEntry._type = DirEntry::TYPE_FILE;
  }

  dirEntry._size = ((uint64_t)(dirEntry._winFindData.nFileSizeHigh) << 32) |
                   ((uint64_t)(dirEntry._winFindData.nFileSizeLow));
  dirEntry._name.set(reinterpret_cast<const Char*>(dirEntry._winFindData.cFileName));
  return true;
}

bool DirIterator::read(String& fileName)
{
  // TODO: optimize using internal WIN32_FIND_DATA.
  DirEntry e;
  if (!read(e)) return false;

  fileName = e.getName();
  return true;
}

err_t DirIterator::rewind()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  StringTmp<TEMPORARY_LENGTH> t;

  err_t err;

  if ((err = t.set(_path)) ||
      (err = t.append(Ascii8("\\*"))) ||
      (err = t.slashesToWin()))
  {
    return err;
  }

  HANDLE h = FindFirstFileW(reinterpret_cast<const wchar_t*>(t.getData()), &_winFindData);
  if (h == INVALID_HANDLE_VALUE)
  {
    // TODO: long filename support as in open()
    return GetLastError();
  }

  ::FindClose((HANDLE)_handle);
  _handle = (void*)h;
  _position = 0;

  return ERR_OK;
}

int64_t DirIterator::tell()
{
  return _position;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::DirIterator - Posix]
// ============================================================================

#if defined(FOG_OS_POSIX)

// Glibc's readdir is reentrant. POSIX guarantees only readdir_r to be reentrant
// __linux__ is defined on Linux, __GNU__ on HURD and __GLIBC__ on Debian GNU/k*BSD
#if defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
# define FOG_HAVE_REENTRANT_READDIR
#endif

DirIterator::DirIterator() :
  _handle(NULL),
  _pathCacheBaseLength(0),
  _dent(NULL),
  _skipDots(true)
{
}

DirIterator::DirIterator(const String& path) :
  _handle(NULL),
  _pathCacheBaseLength(0),
  _dent(NULL),
  _skipDots(true)
{
  open(path);
}

DirIterator::~DirIterator()
{
  if (_handle) close();
}

err_t DirIterator::open(const String& path)
{
  StringTmp<TEMPORARY_LENGTH> pathAbs;

  close();

  FOG_RETURN_ON_ERROR(FileSystem::toAbsolutePath(pathAbs, String(), path));
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(_pathCache, pathAbs));

  errno = 0;
  if ((_handle = (void*)::opendir(_pathCache.getData())) != NULL)
  {
    // Get max size of file name in this directory + fallback.
    long direntSize = pathconf(_pathCache.getData(), _PC_NAME_MAX);
    if (direntSize == -1) direntSize = _POSIX_NAME_MAX;

    // Add offset of d_name field + 1 to get correct dirent size.
    direntSize += FOG_OFFSET_OF(struct dirent, d_name) + 1;

    // Ugly typecast from long, but there is no better way.
    _dent = reinterpret_cast<struct dirent*>(Memory::alloc((size_t)direntSize));
    if (!_dent)
    {
      ::closedir((DIR*)_handle);
      _handle = NULL;

      return ERR_RT_OUT_OF_MEMORY;
    }

    _path = pathAbs;
    _pathCacheBaseLength = _pathCache.getLength();
    return ERR_OK;
  }
  else
  {
    Memory::free(_dent);
    return errno;
  }
}

void DirIterator::close()
{
  if (!_handle) return;

  ::closedir((DIR*)_handle);
  _handle = NULL;

  _path.clear();
  _pathCache.clear();
  _pathCacheBaseLength = 0;
}

bool DirIterator::read(DirEntry& dirEntry)
{
  if (!_handle) return false;

  struct dirent *de;
  bool skipDots = getSkipDots();

  while (::readdir_r((DIR*)_handle, _dent, &de) == 0 && (de != NULL))
  {
    const char* name = de->d_name;

    // Skip "." and ".."
    if (name[0] == '.' && skipDots)
    {
      if (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')) continue;
    }

    // Get entry name length.
    size_t nameLength = strlen(name);

    // Translate entry name to unicode.
    TextCodec::local8().decode(dirEntry._name, Stub8(name, nameLength));

    _pathCache.resize(_pathCacheBaseLength);
    _pathCache.append('/');
    _pathCache.append(name, nameLength);

    uint type = 0;

    if (::stat(_pathCache.getData(), &dirEntry._statInfo) != 0)
    {
      // This is situation that's bad symbolic link (I was experienced
      // this) and there is no reason to write an error message...
    }
    else
    {
      // S_ISXXX are posix macros to get easy file type...
      if (S_ISREG(dirEntry._statInfo.st_mode)) type |= DirEntry::TYPE_FILE;
      if (S_ISDIR(dirEntry._statInfo.st_mode)) type |= DirEntry::TYPE_DIRECTORY;
#if defined(S_ISCHR)
      if (S_ISCHR(dirEntry._statInfo.st_mode)) type |= DirEntry::TYPE_CHAR_DEVICE;
#endif
#if defined(S_ISBLK)
      if (S_ISBLK(dirEntry._statInfo.st_mode)) type |= DirEntry::TYPE_BLOCK_DEVICE;
#endif
#if defined(S_ISFIFO)
      if (S_ISFIFO(dirEntry._statInfo.st_mode)) type |= DirEntry::TYPE_FIFO;
#endif
#if defined(S_ISLNK)
      if (S_ISLNK(dirEntry._statInfo.st_mode)) type |= DirEntry::TYPE_LINK;
#endif
#if defined(S_ISSOCK)
      if (S_ISSOCK(dirEntry._statInfo.st_mode)) type |= DirEntry::TYPE_SOCKET;
#endif
    }

    dirEntry._type = type;
    if (type == DirEntry::TYPE_FILE)
      dirEntry._size = dirEntry._statInfo.st_size;
    else
      dirEntry._size = 0;
    return true;
  }

  return false;
}

bool DirIterator::read(String& fileName)
{
  if (!_handle) return false;

  struct dirent *de;
  bool skipDots = getSkipDots();

  while (::readdir_r((DIR*)_handle, _dent, &de) == 0 && (de != NULL))
  {
    const char* name = de->d_name;

    // Skip "." and ".."
    if (name[0] == '.' && skipDots)
    {
      if (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')) continue;
    }

    // Translate entry name to unicode.
    return (TextCodec::local8().decode(fileName, Stub8(name, DETECT_LENGTH)) == ERR_OK);
  }
  return false;
}

err_t DirIterator::rewind()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  ::rewinddir((DIR*)(_handle));
  return ERR_OK;
}

int64_t DirIterator::tell()
{
  if (_handle)
    return (int64_t)::telldir((DIR*)(_handle));
  else
    return -1;
}
#endif // FOG_OS_POSIX

} // Fog namespace
