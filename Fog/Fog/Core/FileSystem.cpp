// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/FileUtil.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/UserInfo.h>

#if defined(FOG_OS_WINDOWS)
// windows.h is already included in Fog/Core/Build.h
#include <io.h>
#ifndef IO_REPARSE_TAG_SYMLINK
#define IO_REPARSE_TAG_SYMLINK 0xA000000C
#endif // IO_REPARSE_TAG_SYMLINK
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
// [Fog::FileSystem]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
uint32_t FileSystem::testFile(const String& fileName, uint32_t flags)
{
  if (flags == 0) return 0;

  TemporaryString<TEMP_LENGTH> fileNameW;
  err_t err;
  if ((err = fileNameW.set(fileName)) || (err = fileNameW.slashesToWin())) return 0;

  WIN32_FILE_ATTRIBUTE_DATA fi;

  if (GetFileAttributesExW(reinterpret_cast<const wchar_t*>(fileNameW.getData()), GetFileExInfoStandard, &fi))
  {
    uint result = FileSystem::FILE_EXISTS;
    if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      result |= FileSystem::IS_DIRECTORY;
    else
      result |= FileSystem::IS_FILE;

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      result |= FileSystem::CAN_READ;
    else
      result |= FileSystem::CAN_READ | FileSystem::CAN_WRITE;

    if ((flags & FileSystem::IS_LINK) && (fi.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
    {
      WIN32_FIND_DATAW fd;
      HANDLE r;
      
      r = FindFirstFileW(reinterpret_cast<const wchar_t*>(fileNameW.getData()), &fd);
      if (r != INVALID_HANDLE_VALUE)
      {
        if (fd.dwReserved0 & IO_REPARSE_TAG_SYMLINK)
        {
          result |= FileSystem::IS_LINK;
        }
        FindClose(r);
      }
    }

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) result |= FileSystem::IS_HIDDEN;
    if (fi.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) result |= FileSystem::IS_ARCHIVE;
    if (fi.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) result |= FileSystem::IS_COMPRESSED;
    if (fi.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) result |= FileSystem::IS_SPARSE;
    if (fi.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) result |= FileSystem::IS_SYSTEM;

    // IsExecutable and CanExecute
    if ((flags & (FileSystem::IS_EXECUTABLE | FileSystem::CAN_EXECUTE)) != 0)
    {
      TemporaryString<16> ext;
      FileUtil::extractExtension(ext, fileName);

      const Char* extStr = ext.getData();
      sysuint_t extLength = ext.getLength();

      // Executable extension has usually 3 characters
      if (extLength == 3)
      {
        if (StringUtil::eq(extStr, "exe", 3, CASE_INSENSITIVE) ||
            StringUtil::eq(extStr, "com", 3, CASE_INSENSITIVE) ||
            StringUtil::eq(extStr, "bat", 3, CASE_INSENSITIVE))
        {
          result |= FileSystem::IS_EXECUTABLE | FileSystem::CAN_EXECUTE;
        }
      }
    }

    return result & flags;
  }
  else
    return 0;
}

bool FileSystem::findFile(const List<String>& paths, const String& fileName, String& dst)
{
  List<String> _paths(paths);
  List<String>::ConstIterator it(_paths);

  WIN32_FILE_ATTRIBUTE_DATA fi;
  TemporaryString<TEMP_LENGTH> path;

  for (it.toStart(); it.isValid(); it.toNext())
  {
    // Set path.
    path.setDeep(it.value());

    // Append directory separator if needed.
    if (!it.value().endsWith(Ascii8("\\", 1)) || !it.value().endsWith(Ascii8("/", 1)))
    {
      path.append(Char('\\'));
    }

    // Append file. If something fail here we try to find file in next path.
    if (path.append(fileName) != ERR_OK) continue;
    if (path.slashesToWin() != ERR_OK) continue;

    // Test.
    if (GetFileAttributesExW(reinterpret_cast<const wchar_t*>(path.getData()), GetFileExInfoStandard, &fi) &&
      !(fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      return FileUtil::joinPath(dst, it.value(), fileName) == ERR_OK;
    }
  }

  return false;
}

static uint createDirectoryHelper(const Char* path, sysuint_t len)
{
  err_t err;

  if (len == 3 && path[0].isAsciiAlpha() && path[1] == Char(':') && path[2] == Char('/'))
  {
    // TODO: Maybe we should return failure if disk is not mounted.
    return ERR_IO_DIR_ALREADY_EXISTS;
  }

  TemporaryString<TEMP_LENGTH> pathW;
  if ((err = pathW.set(path, len)) || (err = pathW.slashesToWin())) return err;

  if (!CreateDirectoryW(reinterpret_cast<const wchar_t*>(pathW.getData()), NULL))
  {
    // TODO: Move this to some helper function that will translate windows error
    // codes into Fog error codes.
    DWORD lastError = GetLastError();
    if (lastError != ERROR_ALREADY_EXISTS)
      return lastError;
    else
      return ERR_IO_DIR_ALREADY_EXISTS;
  }

  return ERR_OK;
}

err_t FileSystem::createDirectory(const String& dir, bool recursive)
{
  if (dir.isEmpty()) return ERR_RT_INVALID_ARGUMENT;

  err_t err;
  TemporaryString<TEMP_LENGTH> dirAbs;

  if ( (err = FileUtil::toAbsolutePath(dirAbs, String(), dir)) ) return err;
  if (!recursive) return createDirectoryHelper(dirAbs.getData(), dirAbs.getLength());

  // FileSystem::toAbsolutePath() always normalize dir to 'X:/', we can imagine
  // that dirAbs is absolute dir, so we need to find first two occurences
  // of '/'. Second occurece can be end of string.
  sysuint_t i = dirAbs.indexOf(Char('/'));
  sysuint_t length = dirAbs.getLength();

  if (i == INVALID_INDEX) return ERR_RT_INVALID_ARGUMENT;
  if (dirAbs.at(length-1) == Char('/')) length--;

  do {
    i++;
    i = dirAbs.indexOf(Char('/'), CASE_SENSITIVE, Range(i, length - i));

    err = createDirectoryHelper(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    if (err != ERR_OK && err != ERR_IO_DIR_ALREADY_EXISTS) return err;
  } while (i != INVALID_INDEX);

  return ERR_OK;
}

err_t FileSystem::deleteDirectory(const String& path)
{
  err_t err;
  TemporaryString<TEMP_LENGTH> pathW;

  if ((err = pathW.set(path)) ||
      (err = pathW.slashesToWin()))
  {
    return err;
  }

  if (RemoveDirectoryW(reinterpret_cast<const wchar_t*>(pathW.getData())))
    return ERR_OK;
  else
    return GetLastError();
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
int FileSystem::stat(const String& fileName, struct stat* s)
{
  TemporaryByteArray<TEMP_LENGTH> t;
  TextCodec::local8().appendFromUnicode(t, fileName);
  return ::stat(t.getData(), s);
}

static uint32_t test_stat(struct stat *s, uint32_t flags)
{
  uint32_t result = FileSystem::FILE_EXISTS;

  if (S_ISREG(s->st_mode)) result |= FileSystem::IS_FILE;
  if (S_ISDIR(s->st_mode)) result |= FileSystem::IS_DIRECTORY;
  if (S_ISLNK(s->st_mode)) result |= FileSystem::IS_LINK;

  if ((s->st_mode & S_IXUSR) ||
      (s->st_mode & S_IXGRP) ||
      (s->st_mode & S_IXOTH) )
  {
    result |= FileSystem::IS_EXECUTABLE;
  }

  if ((flags & (FileSystem::CAN_READ   |
                FileSystem::CAN_WRITE  |
                FileSystem::CAN_EXECUTE)) != 0)
  {
    uid_t uid = UserInfo::uid();
    gid_t gid = UserInfo::gid();

    if (s->st_uid == uid && (s->st_mode & S_IRWXU)) {
      if (s->st_mode & S_IRUSR) result |= FileSystem::CAN_READ;
      if (s->st_mode & S_IWUSR) result |= FileSystem::CAN_WRITE;
      if (s->st_mode & S_IXUSR) result |= FileSystem::CAN_EXECUTE;
    }
    else if (s->st_gid == gid && (s->st_mode & S_IRWXG)) {
      if (s->st_mode & S_IRGRP) result |= FileSystem::CAN_READ;
      if (s->st_mode & S_IWGRP) result |= FileSystem::CAN_WRITE;
      if (s->st_mode & S_IXGRP) result |= FileSystem::CAN_EXECUTE;
    }
    else if (s->st_mode & S_IRWXO) {
      if (s->st_mode & S_IROTH) result |= FileSystem::CAN_READ;
      if (s->st_mode & S_IWOTH) result |= FileSystem::CAN_WRITE;
      if (s->st_mode & S_IXOTH) result |= FileSystem::CAN_EXECUTE;
    }
    // TODO: How to handle this...?
    else
    {
      result |= FileSystem::CAN_READ;
      result |= FileSystem::CAN_WRITE;
      result |= FileSystem::CAN_EXECUTE;
    }
  }

  // Return and clear up flags that wasn't specified to return.
  return result & flags;
}

uint32_t FileSystem::testFile(const String& fileName, uint32_t flags)
{
  if (flags == 0) return 0;
  struct stat s;
  return (stat(fileName, &s) == 0) ? test_stat(&s, flags) : 0;
}

bool FileSystem::findFile(const List<String>& paths, const String& fileName, String& dst)
{
  List<String> _paths(paths);
  List<String>::ConstIterator it(_paths);

  struct stat s;

  TemporaryByteArray<TEMP_LENGTH> path8;
  TemporaryByteArray<TEMP_LENGTH> fileName8;

  // Encode fileName here to avoid encoding in loop.
  TextCodec::local8().appendFromUnicode(fileName8, fileName);

  for (it.toStart(); it.isValid(); it.toNext())
  {
    // Append path.
    TextCodec::local8().fromUnicode(path8, it.value());

    // Append directory separator if needed
    if (path8.getLength() && !path8.endsWith(Str8("/", 1))) path8.append('/');

    // Append file
    path8.append(fileName8);

    // Test
    if (::stat(path8.getData(), &s) == 0 && S_ISREG(s.st_mode))
    {
      return FileUtil::joinPath(dst, it.value(), fileName) == ERR_OK;
    }
  }
  return false;
}

static err_t createDirectoryHelper(const Char* path, sysuint_t len)
{
  if (len == 1 && path[0] == '/') return ERR_IO_DIR_ALREADY_EXISTS;

  TemporaryByteArray<TEMP_LENGTH> path8;
  err_t err;

  if ((err = TextCodec::local8().appendFromUnicode(path8, path, len))) return err;
  if (::mkdir(path8.getData(), S_IRWXU | S_IXGRP | S_IXOTH) == 0) return ERR_OK;

  if (errno == EEXIST)
    return ERR_IO_DIR_ALREADY_EXISTS;
  else
    return errno;
}

err_t FileSystem::createDirectory(const String& dir, bool recursive)
{
  if (dir.isEmpty()) return ERR_RT_INVALID_ARGUMENT;
  if (!recursive) return createDirectoryHelper(dir.getData(), dir.getLength());

  err_t err;
  TemporaryString<TEMP_LENGTH> dirAbs;
  if ( (err = FileUtil::toAbsolutePath(dirAbs, String(), dir)) ) return err;

  // FileSystem::toAbsolutePath() always normalize dir to '/', we can imagine
  // that dirAbs is absolute dir, so we need to find first two occurences
  // of '/'. Second occurece can be end of string.
  if (dirAbs.getLength() == 1 && dirAbs.at(0) == '/') return ERR_IO_DIR_ALREADY_EXISTS;

  sysuint_t i = dirAbs.indexOf(Char('/'));
  sysuint_t length = dirAbs.getLength();

  if (i == INVALID_INDEX) return ERR_RT_INVALID_ARGUMENT;
  if (dirAbs.at(length-1) == Char('/')) length--;

  do {
    i++;
    i = dirAbs.indexOf(Char('/'), CASE_SENSITIVE, Range(i, length - i));

    err = createDirectoryHelper(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    if (err != ERR_OK && err != ERR_IO_DIR_ALREADY_EXISTS) return err;
  } while (i != INVALID_INDEX);

  return ERR_OK;
}

err_t FileSystem::deleteDirectory(const String& dir)
{
  err_t err;
  TemporaryByteArray<TEMP_LENGTH> dir8;

  if ((err = TextCodec::local8().appendFromUnicode(dir8, dir))) return err;

  if (::rmdir(dir8.getData()) == 0)
    return ERR_OK;
  else
    return errno;
}
#endif // FOG_OS_POSIX

} // Fog namespace
