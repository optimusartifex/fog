// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/DirIterator.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/UserInfo.h>

#if defined(FOG_OS_WINDOWS)
// windows.h is already included in Core/Build.h
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

// ============================================================================
// [Fog::DirIterator]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

DirIterator::DirIterator() :
  _handle(NULL),
  _position(-1),
  _fileInEntry(false),
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

  TemporaryString<TEMPORARY_LENGTH> pathAbs;
  TemporaryString<TEMPORARY_LENGTH> t;

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
    if ((err == ERROR_PATH_NOT_FOUND) &&
        (err = t.prepend(Ascii8("\\\\?\\"))) == ERR_OK)
    {
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
  _fileInEntry = false;
  _path.clear();
  memset(&_winFindData, 0, sizeof(WIN32_FIND_DATAW));
}

bool DirIterator::read(String& fileName)
{
  // TODO: optimize using internal WIN32_FIND_DATA
  DirEntry e;
  if (read(e))
  {
    fileName = e.getName();
    return true;
  }
  return false;
}

bool DirIterator::read(DirEntry& to)
{
  if (!_handle) return false;

  if (_position != 0)
  {
    // try to read next file entry
__readNext:
    if (!::FindNextFileW(_handle, &to._winFindData)) return false;
  }
  else 
  {
    memcpy(&to._winFindData, &_winFindData, sizeof(WIN32_FIND_DATAW));
  }

  _position++;

  // we have valid file entry in to._winFindData
  if (to._winFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
  {
    to._type = DirEntry::TYPE_DIRECTORY;

    // Skip "." and ".."
    if (_skipDots)
    {
      const WCHAR* cfn = to._winFindData.cFileName;
      if (cfn[0] == L'.' && 
        ((cfn[1] == 0) || (cfn[1] == L'.' && cfn[2] == 0) ))
      {
        goto __readNext;
      }
    }
  }
  else
  {
    to._type = DirEntry::TYPE_FILE;
  }

  to._size = ((uint64_t)(to._winFindData.nFileSizeHigh) << 32) |
             ((uint64_t)(to._winFindData.nFileSizeLow));
  to._name.set(reinterpret_cast<const Char*>(to._winFindData.cFileName));

  return true;
}

err_t DirIterator::rewind()
{
  if (!_handle) return ERR_RT_INVALID_HANDLE;

  TemporaryString<TEMPORARY_LENGTH> t;

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
  _fileInEntry = true;

  return ERR_OK;
}

int64_t DirIterator::tell()
{
  return _position;
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)

DirIterator::DirIterator() :
  _handle(NULL),
  _skipDots(true),
  _pathCacheBaseLength(0)
{
}

DirIterator::DirIterator(const String& path) :
  _handle(NULL),
  _skipDots(true),
  _pathCacheBaseLength(0)
{
  open(path);
}

DirIterator::~DirIterator()
{
  if (_handle) close();
}

err_t DirIterator::open(const String& path)
{
  close();

  TemporaryString<TEMPORARY_LENGTH> pathAbs;
  TemporaryByteArray<TEMPORARY_LENGTH> t;

  err_t err;
  long direntSize;

  if ((err = FileSystem::toAbsolutePath(pathAbs, String(), path))) return err;
  if ((err = TextCodec::local8().appendFromUnicode(t, pathAbs))) return err;

  // Get max size of file name in this directory + fallback
  direntSize = pathconf(t.getData(), _PC_NAME_MAX);
  if (direntSize == -1) direntSize = _POSIX_NAME_MAX;
  // Add offset of d_name field + 1 to get correct dirent size
  direntSize += offsetof(struct dirent, d_name) + 1;

  // Ugly typecast from long, but there is no better way
  _dent = (struct dirent*)Memory::alloc((sysuint_t)direntSize);
  if (!_dent) return ERR_RT_OUT_OF_MEMORY;

  if ((_handle = (void*)::opendir(t.getData())) != NULL)
  {
    _path = pathAbs;
    _pathCache = t;
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

  ::closedir((DIR*)(_handle));
  Memory::free(_dent);
  _handle = NULL;
  _path.clear();
  _pathCache.clear();
  _pathCacheBaseLength = 0;
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

    // Translate entry name to unicode
    return (TextCodec::local8().toUnicode(fileName, name) == ERR_OK);
  }
  return false;
}

bool DirIterator::read(DirEntry& to)
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
    sysuint_t nameLength = strlen(name);

    // Translate entry name to unicode.
    TextCodec::local8().toUnicode(to._name, name, nameLength);

    _pathCache.resize(_pathCacheBaseLength);
    _pathCache.append('/');
    _pathCache.append(name, nameLength);

    uint type = 0;

    if (::stat(_pathCache.getData(), &to._statInfo) != 0)
    {
      // This is situation that's bad symbolic link (I was experienced
      // this) and there is no reason to write an error message...
    }
    else
    {
      // S_ISXXX are posix macros to get easy file type...
      if (S_ISREG(to._statInfo.st_mode)) type |= DirEntry::TYPE_FILE;
      if (S_ISDIR(to._statInfo.st_mode)) type |= DirEntry::TYPE_DIRECTORY;
#if defined(S_ISCHR)
      if (S_ISCHR(to._statInfo.st_mode)) type |= DirEntry::TYPE_CHAR_DEVICE;
#endif
#if defined(S_ISBLK)
      if (S_ISBLK(to._statInfo.st_mode)) type |= DirEntry::TYPE_BLOCK_DEVICE;
#endif
#if defined(S_ISFIFO)
      if (S_ISFIFO(to._statInfo.st_mode)) type |= DirEntry::TYPE_FIFO;
#endif
#if defined(S_ISLNK)
      if (S_ISLNK(to._statInfo.st_mode)) type |= DirEntry::TYPE_LINK;
#endif
#if defined(S_ISSOCK)
      if (S_ISSOCK(to._statInfo.st_mode)) type |= DirEntry::TYPE_SOCKET;
#endif
    }

    to._type = type;
    if (type == DirEntry::TYPE_FILE)
      to._size = to._statInfo.st_size;
    else
      to._size = 0;
    return true;
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
