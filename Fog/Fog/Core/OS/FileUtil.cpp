// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/FileUtil.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/OS/UserUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/OS/WinUtil.h>
# include <io.h>
# ifndef IO_REPARSE_TAG_SYMLINK
#  define IO_REPARSE_TAG_SYMLINK 0xA000000C
# endif // IO_REPARSE_TAG_SYMLINK
#endif

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <dirent.h>
# include <sys/stat.h>
# if defined(FOG_HAVE_UNISTD_H)
#  include <unistd.h>
# endif
#endif

namespace Fog {

// ============================================================================
// [Fog::FileUtil - Test (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static uint32_t FOG_CDECL FileUtil_testRaw(const CharW* pathData, uint32_t flags)
{
  WIN32_FILE_ATTRIBUTE_DATA fi;

  if (::GetFileAttributesExW(reinterpret_cast<const wchar_t*>(pathData), GetFileExInfoStandard, &fi))
  {
    uint result = FILE_INFO_EXISTS;
    if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      result |= FILE_INFO_DIRECTORY;
    else
      result |= FILE_INFO_REGULAR_FILE;

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      result |= FILE_INFO_CAN_READ;
    else
      result |= FILE_INFO_CAN_READ | FILE_INFO_CAN_WRITE;

    if ((flags & FILE_INFO_SYMLINK) && (fi.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
    {
      WIN32_FIND_DATAW fd;
      HANDLE r;

      r = FindFirstFileW(reinterpret_cast<const wchar_t*>(pathData), &fd);
      if (r != INVALID_HANDLE_VALUE)
      {
        if (fd.dwReserved0 & IO_REPARSE_TAG_SYMLINK)
        {
          result |= FILE_INFO_SYMLINK;
        }
        FindClose(r);
      }
    }

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
      result |= FILE_INFO_HIDDEN;

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
      result |= FILE_INFO_ARCHIVE;

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
      result |= FILE_INFO_COMPRESSED;

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
      result |= FILE_INFO_SPARSE;

    if (fi.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
      result |= FILE_INFO_SYSTEM;

    // IS_EXECUTABLE and CAN_EXECUTE
    if ((flags & (FILE_INFO_EXECUTABLE | FILE_INFO_CAN_EXECUTE)) != 0)
    {
      StringTmpW<16> ext;
      FilePath::extractExtension(ext, StubW(pathData));

      const CharW* extStr = ext.getData();
      size_t extLength = ext.getLength();

      // Executable extension has usually 3 characters.
      if (extLength == 3)
      {
        if (StringUtil::eq(extStr, "exe", 3, CASE_INSENSITIVE) ||
            StringUtil::eq(extStr, "com", 3, CASE_INSENSITIVE) ||
            StringUtil::eq(extStr, "bat", 3, CASE_INSENSITIVE))
        {
          result |= FILE_INFO_EXECUTABLE | FILE_INFO_CAN_EXECUTE;
        }
      }
    }

    return result & flags;
  }

  return 0;
}

static uint32_t FOG_CDECL FileUtil_testStubW(const StubW* path, uint32_t flags)
{
  if (flags == 0)
    return 0;

  StringTmpW<TEMPORARY_LENGTH> pathW;
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(pathW, *path));

  return FileUtil_testRaw(pathW.getData(), flags);
}

static uint32_t FOG_CDECL FileUtil_testStringW(const StringW* path, uint32_t flags)
{
  if (flags == 0)
    return 0;

  StringTmpW<TEMPORARY_LENGTH> pathW;
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(pathW, *path));

  return FileUtil_testRaw(pathW.getData(), flags);
}

static bool FOG_CDECL FileUtil_testLocalNameStubW(const StubW* path)
{
  return true;
}

static bool FOG_CDECL FileUtil_testLocalNameStringW(const StringW* path)
{
  return true;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::FileUtil - Test (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static uint32_t FileUtil_testStat(struct stat *s, uint32_t flags)
{
  uint32_t result = FILE_INFO_EXISTS;

  if (S_ISREG(s->st_mode)) result |= FILE_INFO_REGULAR_FILE;
  if (S_ISDIR(s->st_mode)) result |= FILE_INFO_DIRECTORY;
  if (S_ISLNK(s->st_mode)) result |= FILE_INFO_SYMLINK;

  if ((s->st_mode & S_IXUSR) ||
      (s->st_mode & S_IXGRP) ||
      (s->st_mode & S_IXOTH) )
  {
    result |= FILE_INFO_EXECUTABLE;
  }

  if ((flags & (FILE_INFO_CAN_READ   |
                FILE_INFO_CAN_WRITE  |
                FILE_INFO_CAN_EXECUTE)) != 0)
  {
    uid_t uid = UserUtil::getUid();
    gid_t gid = UserUtil::getGid();

    if (s->st_uid == uid && (s->st_mode & S_IRWXU))
    {
      if (s->st_mode & S_IRUSR) result |= FILE_INFO_CAN_READ;
      if (s->st_mode & S_IWUSR) result |= FILE_INFO_CAN_WRITE;
      if (s->st_mode & S_IXUSR) result |= FILE_INFO_CAN_EXECUTE;
    }
    else if (s->st_gid == gid && (s->st_mode & S_IRWXG))
    {
      if (s->st_mode & S_IRGRP) result |= FILE_INFO_CAN_READ;
      if (s->st_mode & S_IWGRP) result |= FILE_INFO_CAN_WRITE;
      if (s->st_mode & S_IXGRP) result |= FILE_INFO_CAN_EXECUTE;
    }
    else if (s->st_mode & S_IRWXO)
    {
      if (s->st_mode & S_IROTH) result |= FILE_INFO_CAN_READ;
      if (s->st_mode & S_IWOTH) result |= FILE_INFO_CAN_WRITE;
      if (s->st_mode & S_IXOTH) result |= FILE_INFO_CAN_EXECUTE;
    }
    // TODO: How to handle this...?
    else
    {
      result |= FILE_INFO_CAN_READ;
      result |= FILE_INFO_CAN_WRITE;
      result |= FILE_INFO_CAN_EXECUTE;
    }
  }

  // Return and clear up flags that wasn't specified to return.
  return result & flags;
}

static uint32_t FOG_CDECL FileUtil_testStubW(const StubW* path, uint32_t flags)
{
  if (flags == NO_FLAGS)
    return NO_FLAGS;

  struct stat s;
  if (FileUtil::stat(&s, *path) == ERR_OK)
    return FileUtil_testStat(&s, flags);

  return 0;
}

static uint32_t FOG_CDECL FileUtil_testStringW(const StringW* path, uint32_t flags)
{
  if (flags == NO_FLAGS)
    return NO_FLAGS;

  struct stat s;
  if (FileUtil::stat(&s, *path) == ERR_OK)
    return FileUtil_testStat(&s, flags);

  return 0;
}

static bool FOG_CDECL FileUtil_testLocalNameStubW(const StubW* path)
{
  if (TextCodec::local8().isUnicode())
    return true;

  StringTmpA<TEMPORARY_LENGTH> path8;
  return TextCodec::local8().encode(path8, *path) == ERR_OK;
}

static bool FOG_CDECL FileUtil_testLocalNameStringW(const StringW* path)
{
  if (TextCodec::local8().isUnicode())
    return true;

  StringTmpA<TEMPORARY_LENGTH> path8;
  return TextCodec::local8().encode(path8, *path) == ERR_OK;
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Fog::FileUtil - FindFile (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static bool FOG_CDECL FileUtil_findFile(StringW* dst, const StringW* fileName, const List<StringW>* paths)
{
  ListIterator<StringW> it(*paths);

  WIN32_FILE_ATTRIBUTE_DATA fi;
  StringTmpW<TEMPORARY_LENGTH> path;

  while (it.isValid())
  {
    // Set path.
    if (path.setAndNormalizeSlashes(it.getItem(), SLASH_FORM_WINDOWS) != ERR_OK)
      continue;

    // Append directory separator if needed.
    if (!path.endsWith(CharW('\\')))
    {
      if (path.append(CharW('\\')) != ERR_OK)
        continue;
    }

    // Append file. If something fail here we try to find file in next path.
    if (path.appendAndNormalizeSlashes(*fileName, SLASH_FORM_WINDOWS) != ERR_OK)
      continue;

    // Test.
    if (GetFileAttributesExW(reinterpret_cast<const wchar_t*>(path.getData()), GetFileExInfoStandard, &fi) &&
      !(fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      return FilePath::join(*dst, it.getItem(), *fileName) == ERR_OK;
    }

    it.next();
  }

  return false;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::FileUtil - FindFile (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static bool FOG_CDECL FileUtil_findFile(StringW* dst, const StringW* fileName, const List<StringW>* paths)
{
  ListIterator<StringW> it(*paths);

  struct stat s;

  StringTmpA<TEMPORARY_LENGTH> path8;
  StringTmpA<TEMPORARY_LENGTH> fileName8;

  // Encode fileName here to avoid encoding in loop.
  TextCodec::local8().encode(fileName8, *fileName);

  while (it.isValid())
  {
    // Setup path.
    TextCodec::local8().encode(path8, it.getItem());

    // Append directory separator if needed
    if (path8.getLength() && !path8.endsWith(StubA("/", 1)))
      path8.append('/');

    // Append file
    path8.append(fileName8);

    // Test
    if (::stat(path8.getData(), &s) == 0 && S_ISREG(s.st_mode))
      return FilePath::join(*dst, it.getItem(), *fileName) == ERR_OK;

    it.next();
  }
  return false;
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Fog::FileUtil - Directory (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static uint FOG_CDECL FileUtil_createDirectoryHelper(const CharW* path, size_t len)
{
  if (len == 3 && path[0].isAsciiLetter() && path[1] == CharW(':') && path[2] == CharW('/'))
  {
    // TODO: Maybe we should return failure if disk is not mounted.
    return ERR_PATH_EXISTS;
  }

  StringTmpW<TEMPORARY_LENGTH> pathW(StubW(path, len));
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(pathW, pathW));

  if (!CreateDirectoryW(reinterpret_cast<const wchar_t*>(pathW.getData()), NULL))
    return OSUtil::getErrFromOSLastError();

  return ERR_OK;
}

static err_t FOG_CDECL FileUtil_createDirectory(const StringW* dir, bool recursive)
{
  if (dir->isEmpty())
    return ERR_RT_INVALID_ARGUMENT;

  StringTmpW<TEMPORARY_LENGTH> dirAbs;
  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(dirAbs, *dir));

  if (!recursive)
    return FileUtil_createDirectoryHelper(dirAbs.getData(), dirAbs.getLength());

  // FilePath::toAbsolute() always normalize directory to 'X:/'; we can imagine
  // that dirAbs is absolute dir, thus it's needed to find first two occurences
  // of '/' - second occurece can be at the end of string.
  size_t i = dirAbs.indexOf(CharW('/'));
  size_t length = dirAbs.getLength();

  if (i == INVALID_INDEX)
    return ERR_RT_INVALID_ARGUMENT;

  if (dirAbs.getAt(length - 1) == CharW('/'))
    length--;

  do {
    i++;
    i = dirAbs.indexOf(Range(i, i + length), CharW('/'), CASE_SENSITIVE);

    StubW curDir(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    err_t err = FileUtil_createDirectoryHelper(curDir.getData(), curDir.getLength());

    if (err == ERR_PATH_EXISTS)
    {
      if (FileUtil::isDirectory(curDir))
        continue;
      else
        return ERR_PATH_EXISTS;
    }

    if (err != ERR_OK)
      return err;

  } while (i != INVALID_INDEX);

  return ERR_OK;
}

static err_t FOG_CDECL FileUtil_deleteDirectory(const StringW* path)
{
  StringTmpW<TEMPORARY_LENGTH> pathW;
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(pathW, *path));

  if (!::RemoveDirectoryW(reinterpret_cast<const wchar_t*>(pathW.getData())))
    return OSUtil::getErrFromOSLastError();

  return ERR_OK;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::FileUtil - Directory (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static err_t FOG_CDECL FileUtil_createDirectoryHelper(const CharW* path, size_t len)
{
  if (len == 1 && path[0] == CharW('/'))
    return ERR_PATH_EXISTS;

  StringTmpA<TEMPORARY_LENGTH> path8;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(path8, StubW(path, len)));

  if (::mkdir(path8.getData(), S_IRWXU | S_IXGRP | S_IXOTH) == 0)
    return ERR_OK;
  else
    return OSUtil::getErrFromLibCErrno();
}

static err_t FOG_CDECL FileUtil_createDirectory(const StringW* dir, bool recursive)
{
  if (dir->isEmpty())
    return ERR_RT_INVALID_ARGUMENT;

  if (!recursive)
    return FileUtil_createDirectoryHelper(dir->getData(), dir->getLength());

  StringTmpW<TEMPORARY_LENGTH> dirAbs;
  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(dirAbs, *dir));

  // FilePath::toAbsolute() always normalize directory to '/'; we can imagine
  // that dirAbs is absolute dir, thus it's needed to find first two occurences
  // of '/' - second occurece can be at the end of string.
  if (dirAbs.getLength() == 1 && dirAbs.getAt(0) == '/')
    return ERR_PATH_EXISTS;

  size_t i = dirAbs.indexOf(CharW('/'));
  size_t length = dirAbs.getLength();

  if (i == INVALID_INDEX) return ERR_RT_INVALID_ARGUMENT;
  if (dirAbs.getAt(length-1) == CharW('/')) length--;

  do {
    i++;
    i = dirAbs.indexOf(Range(i, i + length), CharW('/'), CASE_SENSITIVE);

    StubW curDir(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    err_t err = FileUtil_createDirectoryHelper(curDir.getData(), curDir.getLength());

    if (err == ERR_PATH_EXISTS)
    {
      if (FileUtil::isDirectory(curDir))
        continue;
      else
        return ERR_PATH_EXISTS;
    }

    if (err != ERR_OK)
      return err;

  } while (i != INVALID_INDEX);

  return ERR_OK;
}

static err_t FOG_CDECL FileUtil_deleteDirectory(const StringW* dir)
{
  StringTmpA<TEMPORARY_LENGTH> dir8;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(dir8, *dir));

  if (::rmdir(dir8.getData()) == 0)
    return ERR_OK;
  else
    return OSUtil::getErrFromLibCErrno();
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Fog::FileUtil - Posix Support]
// ============================================================================

#if defined(FOG_OS_POSIX)
static int FOG_CDECL FileUtil_statStubW(void* dst, const StubW* fileName)
{
  StringTmpA<TEMPORARY_LENGTH> t;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(t, *fileName));

  if (stat(t.getData(), (struct stat*)dst) == 0)
    return ERR_OK;
  else
    return OSUtil::getErrFromLibCErrno();
}

static int FOG_CDECL FileUtil_statStringW(void* dst, const StringW* fileName)
{
  StringTmpA<TEMPORARY_LENGTH> t;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(t, *fileName));

  if (::stat(t.getData(), (struct stat*)dst) == 0)
    return ERR_OK;
  else
    return OSUtil::getErrFromLibCErrno();
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FileUtil_init(void)
{
  fog_api.fileutil_testStubW = FileUtil_testStubW;
  fog_api.fileutil_testStringW = FileUtil_testStringW;

  fog_api.fileutil_testLocalNameStubW = FileUtil_testLocalNameStubW;
  fog_api.fileutil_testLocalNameStringW = FileUtil_testLocalNameStringW;

  fog_api.fileutil_findFile = FileUtil_findFile;

  fog_api.fileutil_createDirectory = FileUtil_createDirectory;
  fog_api.fileutil_deleteDirectory = FileUtil_deleteDirectory;

#if defined(FOG_OS_POSIX)
  fog_api.fileutil_statStubW = FileUtil_statStubW;
  fog_api.fileutil_statStringW = FileUtil_statStringW;
#endif // FOG_OS_POSIX
}

} // Fog namespace
