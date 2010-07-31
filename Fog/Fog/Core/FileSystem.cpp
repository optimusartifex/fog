// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Application.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/FileSystem.h>
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
// [Helpers]
// ============================================================================

// Copy src to dst and convert slashes to posix form
static Char* copyAndNorm(Char* dst, const Char* src, sysuint_t length)
{
  for (sysuint_t i = length; i; i--)
  {
#if defined(FOG_OS_WINDOWS)
    Char ch = *src++;
    if (FOG_UNLIKELY(ch == Char('\\')))
      *dst++ = '/';
    else
      *dst++ = ch;
#else
    *dst++ = *src++;
#endif
  }

  return dst;
}

static bool isDirSeparator(Char ch)
{
  return ch == Char('/') || ch == Char('\\');
}

// ============================================================================
// [Fog::FileSystem]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
uint32_t FileSystem::testFile(const String& fileName, uint32_t flags)
{
  if (flags == 0) return 0;

  TemporaryString<TEMPORARY_LENGTH> fileNameW;
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
      extractExtension(ext, fileName);

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
  TemporaryString<TEMPORARY_LENGTH> path;

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
      return joinPath(dst, it.value(), fileName) == ERR_OK;
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

  TemporaryString<TEMPORARY_LENGTH> pathW;
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
  TemporaryString<TEMPORARY_LENGTH> dirAbs;

  if ( (err = toAbsolutePath(dirAbs, String(), dir)) ) return err;
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
    i = dirAbs.indexOf(Char('/'), CASE_SENSITIVE, Range(i, i + length));

    err = createDirectoryHelper(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    if (err != ERR_OK && err != ERR_IO_DIR_ALREADY_EXISTS) return err;
  } while (i != INVALID_INDEX);

  return ERR_OK;
}

err_t FileSystem::deleteDirectory(const String& path)
{
  err_t err;
  TemporaryString<TEMPORARY_LENGTH> pathW;

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
  TemporaryByteArray<TEMPORARY_LENGTH> t;
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

    if (s->st_uid == uid && (s->st_mode & S_IRWXU))
    {
      if (s->st_mode & S_IRUSR) result |= FileSystem::CAN_READ;
      if (s->st_mode & S_IWUSR) result |= FileSystem::CAN_WRITE;
      if (s->st_mode & S_IXUSR) result |= FileSystem::CAN_EXECUTE;
    }
    else if (s->st_gid == gid && (s->st_mode & S_IRWXG))
    {
      if (s->st_mode & S_IRGRP) result |= FileSystem::CAN_READ;
      if (s->st_mode & S_IWGRP) result |= FileSystem::CAN_WRITE;
      if (s->st_mode & S_IXGRP) result |= FileSystem::CAN_EXECUTE;
    }
    else if (s->st_mode & S_IRWXO)
    {
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

  TemporaryByteArray<TEMPORARY_LENGTH> path8;
  TemporaryByteArray<TEMPORARY_LENGTH> fileName8;

  // Encode fileName here to avoid encoding in loop.
  TextCodec::local8().appendFromUnicode(fileName8, fileName);

  for (it.toStart(); it.isValid(); it.toNext())
  {
    // Append path.
    TextCodec::local8().fromUnicode(path8, it.value());

    // Append directory separator if needed
    if (path8.getLength() && !path8.endsWith(Stub8("/", 1))) path8.append('/');

    // Append file
    path8.append(fileName8);

    // Test
    if (::stat(path8.getData(), &s) == 0 && S_ISREG(s.st_mode))
    {
      return joinPath(dst, it.value(), fileName) == ERR_OK;
    }
  }
  return false;
}

static err_t createDirectoryHelper(const Char* path, sysuint_t len)
{
  if (len == 1 && path[0] == '/') return ERR_IO_DIR_ALREADY_EXISTS;

  TemporaryByteArray<TEMPORARY_LENGTH> path8;
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
  TemporaryString<TEMPORARY_LENGTH> dirAbs;
  if ( (err = toAbsolutePath(dirAbs, String(), dir)) ) return err;

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
    i = dirAbs.indexOf(Char('/'), CASE_SENSITIVE, Range(i, i + length));

    err = createDirectoryHelper(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    if (err != ERR_OK && err != ERR_IO_DIR_ALREADY_EXISTS) return err;
  } while (i != INVALID_INDEX);

  return ERR_OK;
}

err_t FileSystem::deleteDirectory(const String& dir)
{
  err_t err;
  TemporaryByteArray<TEMPORARY_LENGTH> dir8;

  if ((err = TextCodec::local8().appendFromUnicode(dir8, dir))) return err;

  if (::rmdir(dir8.getData()) == 0)
    return ERR_OK;
  else
    return errno;
}
#endif // FOG_OS_POSIX

err_t FileSystem::extractFile(String& dst, const String& path)
{
  sysuint_t index = path.lastIndexOf(Char('/')) + 1U;

  if (index)
  {
    String path_(path);
    return dst.set(path_.getData() + index, path_.getLength() - index);
  }
  else
  {
    return dst.set(path);
  }
}

err_t FileSystem::extractDirectory(String& dst, const String& _path)
{
  sysuint_t index = _path.lastIndexOf(Char('/'));

  // in some cases (for example path /root), index can be 0. So check for this
  // case
  if (index != INVALID_INDEX)
  {
    String path(_path);
    return dst.set(path.getData(), index != 0 ? index : 1);
  }
  else
  {
    dst.clear();
    return ERR_OK;
  }
}

err_t FileSystem::extractExtension(String& dst, const String& _path)
{
  String path(_path);
  dst.clear();

  sysuint_t pathLength = path.getLength();
  if (!pathLength) { return ERR_RT_INVALID_ARGUMENT; }

  // Speed is important, so RAW manipulation is needed
  const Char* pathBegin = path.getData();
  const Char* pathEnd = pathBegin + pathLength;
  const Char* pathCur = pathEnd;

  do {
    if (FOG_UNLIKELY(*--pathCur == Char('.')))
    {
      if (++pathCur != pathEnd)
      {
        return dst.set(pathCur, sysuint_t(pathEnd - pathCur));
      }
      break;
    }
    else if (*pathCur == Char('/'))
    {
      break;
    }
  } while (pathCur != pathBegin);

  return ERR_OK;
}

err_t FileSystem::normalizePath(String& dst, const String& _path)
{
  if (isNormalizedPath(_path)) return dst.set(_path);

  String path(_path);
  // we need to convert:
  // - all "//" sequences to "/"
  // - all "/./" to "/" or "BEGIN./" to ""
  // - all "/../" eats previous directory

  sysuint_t pathLength = path.getLength();
  bool prevSlash = false;

  err_t err;
  if ( (err = dst.resize(pathLength)) ) return err;

  Char* dstBeg = dst.getXData();
  Char* dstCur = dstBeg;

  const Char* pathCur = path.getData();
  const Char* pathEnd = pathCur + pathLength;

  Char c;

  // Handle Windows absolute path "X:\"
#if defined(FOG_OS_WINDOWS)
  if (pathLength > 2 &&
    pathCur[0].isAsciiAlpha() &&
    pathCur[1] == Char(':') &&
    isDirSeparator(pathCur[2]))
  {
    dstCur[0] = pathCur[0];
    dstCur[1] = pathCur[1];

    // We will make path like absolute, next slash will be handled
    // in loop and corrected
    dstBeg += 2; // Increase begin pointer too, see code why...
    dstCur += 2;
    pathCur += 2;
  }
#endif // FOG_OS_WINDOWS

  while (pathCur < pathEnd)
  {
    c = *pathCur++;

    // skip "./" from result
    if (isDirSeparator(c) && (sysuint_t)(dstCur - dstBeg) == 1 && dstCur[-1] == Char('.'))
    {
      prevSlash = true;
      dstCur--;
      continue;
    }

    if (prevSlash)
    {
      // Catch "//" -> "/" sequence
      if (isDirSeparator(c))
        // We were already written "/", so continue is ok
        continue;

      // Catch "/."
      if (c == Char('.'))
      {
        sysuint_t remain = (sysuint_t)(pathEnd - pathCur);

        // Catch "/./" -> "/"
        if (remain > 0 && isDirSeparator(pathCur[0]))
        {
          // We were already written "/", so increase by one and continue
          pathCur++; continue;
        }

        // Catch "/../" -> Eat previous if is
        if (remain > 1 && pathCur[0] == Char('.') && isDirSeparator(pathCur[1]))
        {
          // We know that dstCur[-1] contains '/', so we need to
          // skip this separator and get back last directory. Only what
          // we need to check if "../" possibility. In this case we can't
          // go back and we will append "../" to the result.

          sysuint_t resultLength = (sysuint_t)(dstCur - dstBeg);

          // Bail if result is too small to hold any directory
          if (resultLength <= 1) goto __inc;

          // check for "../"
          if (resultLength > 2 &&
            (dstCur - 3 == dstBeg || isDirSeparator(dstCur[-4])) &&
            dstCur[-3] == Char('.') &&
            dstCur[-2] == Char('.')) goto __inc;

          // Now we can continue
          dstCur -= 2;
          pathCur += 2;

          for (;;)
          {
            c = *dstCur;
            if (isDirSeparator(c)) goto __inc;
            if (dstCur == dstBeg)
            {
              prevSlash = false;
              break;
            }
            dstCur--;
          }
          continue;
        }
      }
    }

__inc:
    prevSlash = isDirSeparator(c);
#if defined(FOG_OS_WINDOWS)
    if (c == Char('\\')) c = Char('/');
#endif // FOG_OS_WINDOWS
    *dstCur++ = c;
  }

  dst.xFinalize(dstCur);
  return ERR_OK;
}

err_t FileSystem::toAbsolutePath(String& dst, const String& base, const String& path)
{
  if (!FileSystem::isAbsolutePath(path))
  {
    err_t err;
    if (base.isEmpty())
    {
      TemporaryString<TEMPORARY_LENGTH> working;
      if ( (err = Application::getWorkingDirectory(working)) ) return err;
      if ( (err = FileSystem::joinPath(dst, working, path)) ) return err;
    }
    else
    {
      if ( (err = FileSystem::joinPath(dst, base, path)) ) return err;
    }
    return FileSystem::normalizePath(dst, dst);
  }
  else
    return FileSystem::normalizePath(dst, path);
}

static err_t _joinPath(String& dst, const String& base, const String& part)
{
  const Char* d_str = base.getData();
  const Char* f_str = part.getData();
  sysuint_t d_length = base.getLength();
  sysuint_t f_length = part.getLength();

  if (d_length && isDirSeparator(d_str[d_length-1])) d_length--;

  // This is not so readable but fast
  err_t err;
  if ( (err = dst.resize(d_length + f_length + 1)) ) return err;

  Char* dstCur = dst.getXData();
  dstCur = copyAndNorm(dstCur, d_str, d_length); *dstCur++ = '/';
  dstCur = copyAndNorm(dstCur, f_str, f_length);
  dst.xFinalize();

  return ERR_OK;
}

err_t FileSystem::joinPath(String& dst, const String& base, const String& part)
{
  // if base or path is shared with dst, we need to do copy
  if (dst._d == base._d || dst._d == part._d)
    return _joinPath(dst, String(base), String(part));
  else
    return _joinPath(dst, base, part);
}

bool FileSystem::isPathContainsFile(const String& path, const String& file, uint cs)
{
  sysuint_t index = path.lastIndexOf(Char('/'));
  sysuint_t length = path.getLength() - index;

  return (length == file.getLength() &&
    StringUtil::eq(path.getData() + index, file.getData(), length, cs));
}

bool FileSystem::isPathContainsDirectory(const String& path, const String& directory, uint cs)
{
  const Char* d_str = directory.getData();
  sysuint_t d_length = directory.getLength();

  if (d_length == 0) return false;
  if (isDirSeparator(d_str[d_length-1])) d_length--;

  return (path.getLength() > d_length &&
    isDirSeparator(path.at(d_length)) &&
    path.startsWith(Utf16(d_str, d_length), cs));
}

bool FileSystem::isPathContainsExtension(const String& path, const String& extension, uint cs)
{
  sysuint_t pathLength = path.getLength();
  if (!pathLength) return false;

  // Speed is important, so RAW manipulation is needed
  const Char* pathBegin = path.getData();
  const Char* pathEnd = pathBegin + pathLength;
  const Char* pathCur = pathEnd;

  do {
    if (FOG_UNLIKELY(*--pathCur == Char('.')))
    {
      pathCur++;
      return ((sysuint_t)(pathEnd - pathCur) == extension.getLength() &&
        StringUtil::eq(pathCur, extension.getData(), extension.getLength(), cs));
    }
    else if (FOG_UNLIKELY(isDirSeparator(*pathCur)))
    {
      return false;
    }
  } while (pathCur != pathBegin);
  return false;
}

bool FileSystem::isNormalizedPath(const String& path)
{
  if (!FileSystem::isAbsolutePath(path)) return false;

#if defined(FOG_OS_WINDOWS)
  const Char* pathBegin = path.getData();
  const Char* pathCur = pathBegin;
  const Char* pathEnd = pathBegin + path.getLength();

  // normalize X:\ form
  if (pathCur[2] == Char('\\')) return false;

  pathCur += 3;
  while (pathCur < pathEnd)
  {
    // All "\" will be replaced to "/"
    if (pathCur[0] == Char('\\')) return false;

    if (pathCur[0] == Char('/'))
    {
      // test for "//"
      if (pathCur[-1] == Char('/')) return false;
      if (pathCur[-1] == Char('.'))
      {
        // NOTE: Here is no problem with reading out of
        // buffer range, because buffer often starts with
        // "\" if we are in this loop.

        // test for "/./"
        if (pathCur[-2] == Char('/')) return false;
        // test for "/../"
        if (pathCur[-2] == Char('.') && pathCur[-3] == Char('/')) return false;
      }
    }
    pathCur++;
  }
  return true;
#else
  const Char* pathBegin = path.getData();
  const Char* pathCur = pathBegin;
  const Char* pathEnd = pathBegin + path.getLength();

  if (*pathCur++ == Char('/'))
  {
    while (pathCur != pathEnd)
    {

      if (pathCur[0] == Char('/'))
      {
        // test for "//"
        if (pathCur[-1] == Char('/')) return false;
        if (pathCur[-1] == Char('.'))
        {
          // NOTE: Here is no problem reading out of buffer
          // range, because buffer often starts with "/" if
          // we are in this loop.

          // test for "/./"
          if (pathCur[-2] == Char('/')) return false;
          // test for "/../"
          if (pathCur[-2] == Char('.') && pathCur[-3] == Char('/')) return false;
        }
      }
      pathCur++;
    }
    return true;
  }
  else
    return false;
#endif
}

bool FileSystem::isAbsolutePath(const String& path)
{
#if defined(FOG_OS_WINDOWS)
  // We can accept that "[A-Za-z]:/" as absolute path
  if (path.getLength() > 2)
  {
    const Char* pathStr = path.getData();
    return (pathStr[0].isAsciiAlpha() &&
      pathStr[1] == Char(':') &&
      isDirSeparator(pathStr[2]));
  }
  else
    return false;
#else
  return (path.getLength() != 0 && path.at(0) == Char('/'));
#endif
}

bool FileSystem::testLocalName(const String& path)
{
#if defined(FOG_OS_WINDOWS)
  return true;
#else
  if (TextCodec::local8().isUnicode()) return true;

  TemporaryByteArray<TEMPORARY_LENGTH> path8;
  return TextCodec::local8().appendFromUnicode(path8, path) == ERR_OK;
#endif
}

} // Fog namespace
