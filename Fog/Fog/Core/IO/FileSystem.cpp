// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/OS/System.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
// windows.h is already included in Fog/Core/C++/StdHeaders.h
# include <io.h>
# ifndef IO_REPARSE_TAG_SYMLINK
#  define IO_REPARSE_TAG_SYMLINK 0xA000000C
# endif // IO_REPARSE_TAG_SYMLINK
#endif

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <dirent.h>
# include <errno.h>
# include <sys/stat.h>
# if defined(FOG_HAVE_UNISTD_H)
#  include <unistd.h>
# endif
#endif

namespace Fog {

// ============================================================================
// [Helpers]
// ============================================================================

// Copy src to dst and convert slashes to posix form
static CharW* copyAndNorm(CharW* dst, const CharW* src, size_t length)
{
  for (size_t i = length; i; i--)
  {
#if defined(FOG_OS_WINDOWS)
    CharW ch = *src++;
    if (FOG_UNLIKELY(ch == CharW('\\')))
      *dst++ = '/';
    else
      *dst++ = ch;
#else
    *dst++ = *src++;
#endif
  }

  return dst;
}

static bool isDirSeparator(CharW ch)
{
  return ch == CharW('/') || ch == CharW('\\');
}

// ============================================================================
// [Fog::FileSystem]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
uint32_t FileSystem::testFile(const StringW& fileName, uint32_t flags)
{
  if (flags == 0)
    return 0;

  StringTmpW<TEMPORARY_LENGTH> fileNameW;
  FOG_RETURN_ON_ERROR(System::makeWindowsPath(fileNameW, fileName));

  WIN32_FILE_ATTRIBUTE_DATA fi;
  if (::GetFileAttributesExW(reinterpret_cast<const wchar_t*>(fileNameW.getData()), GetFileExInfoStandard, &fi))
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

    // IS_EXECUTABLE and CAN_EXECUTE
    if ((flags & (FileSystem::IS_EXECUTABLE | FileSystem::CAN_EXECUTE)) != 0)
    {
      StringTmpW<16> ext;
      extractExtension(ext, fileName);

      const CharW* extStr = ext.getData();
      size_t extLength = ext.getLength();

      // Executable extension has usually 3 characters.
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

bool FileSystem::findFile(const List<StringW>& paths, const StringW& fileName, StringW& dst)
{
  List<StringW> _paths(paths);
  ListIterator<StringW> it(_paths);

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
    if (path.appendAndNormalizeSlashes(fileName, SLASH_FORM_WINDOWS) != ERR_OK)
      continue;

    // Test.
    if (GetFileAttributesExW(reinterpret_cast<const wchar_t*>(path.getData()), GetFileExInfoStandard, &fi) &&
      !(fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
      return joinPath(dst, it.getItem(), fileName) == ERR_OK;
    }

    it.next();
  }

  return false;
}

static uint createDirectoryHelper(const CharW* path, size_t len)
{
  if (len == 3 && path[0].isAsciiLetter() && path[1] == CharW(':') && path[2] == CharW('/'))
  {
    // TODO: Maybe we should return failure if disk is not mounted.
    return ERR_IO_DIR_ALREADY_EXISTS;
  }

  StringTmpW<TEMPORARY_LENGTH> pathW(StubW(path, len));
  FOG_RETURN_ON_ERROR(System::makeWindowsPath(pathW, pathW));

  if (!CreateDirectoryW(reinterpret_cast<const wchar_t*>(pathW.getData()), NULL))
    return System::errorFromOSLastError();

  return ERR_OK;
}

err_t FileSystem::createDirectory(const StringW& dir, bool recursive)
{
  if (dir.isEmpty()) return ERR_RT_INVALID_ARGUMENT;

  err_t err;
  StringTmpW<TEMPORARY_LENGTH> dirAbs;

  if ( (err = toAbsolutePath(dirAbs, StringW(), dir)) ) return err;
  if (!recursive) return createDirectoryHelper(dirAbs.getData(), dirAbs.getLength());

  // FileSystem::toAbsolutePath() always normalize dir to 'X:/', we can imagine
  // that dirAbs is absolute dir, so we need to find first two occurences
  // of '/'. Second occurece can be end of string.
  size_t i = dirAbs.indexOf(CharW('/'));
  size_t length = dirAbs.getLength();

  if (i == INVALID_INDEX) return ERR_RT_INVALID_ARGUMENT;
  if (dirAbs.getAt(length-1) == CharW('/')) length--;

  do {
    i++;
    i = dirAbs.indexOf(Range(i, i + length), CharW('/'), CASE_SENSITIVE);

    err = createDirectoryHelper(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    if (err != ERR_OK && err != ERR_IO_DIR_ALREADY_EXISTS) return err;
  } while (i != INVALID_INDEX);

  return ERR_OK;
}

err_t FileSystem::deleteDirectory(const StringW& path)
{
  StringTmpW<TEMPORARY_LENGTH> pathW;
  FOG_RETURN_ON_ERROR(System::makeWindowsPath(pathW, path));

  if (!::RemoveDirectoryW(reinterpret_cast<const wchar_t*>(pathW.getData())))
    return System::errorFromOSLastError();

  return ERR_OK;
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
int FileSystem::stat(const StringW& fileName, struct stat* s)
{
  StringTmpA<TEMPORARY_LENGTH> t;
  TextCodec::local8().encode(t, fileName);
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

uint32_t FileSystem::testFile(const StringW& fileName, uint32_t flags)
{
  if (flags == 0) return 0;
  struct stat s;
  return (stat(fileName, &s) == 0) ? test_stat(&s, flags) : 0;
}

bool FileSystem::findFile(const List<StringW>& paths, const StringW& fileName, StringW& dst)
{
  List<StringW> _paths(paths);
  ListIterator<StringW> it(_paths);

  struct stat s;

  StringTmpA<TEMPORARY_LENGTH> path8;
  StringTmpA<TEMPORARY_LENGTH> fileName8;

  // Encode fileName here to avoid encoding in loop.
  TextCodec::local8().encode(fileName8, fileName);

  while (it.isValid())
  {
    // Setup path.
    TextCodec::local8().encode(path8, it.getItem());

    // Append directory separator if needed
    if (path8.getLength() && !path8.endsWith(StubA("/", 1))) path8.append('/');

    // Append file
    path8.append(fileName8);

    // Test
    if (::stat(path8.getData(), &s) == 0 && S_ISREG(s.st_mode))
    {
      return joinPath(dst, it.getItem(), fileName) == ERR_OK;
    }

    it.next();
  }
  return false;
}

static err_t createDirectoryHelper(const CharW* path, size_t len)
{
  if (len == 1 && path[0] == CharW('/'))
    return ERR_IO_DIR_ALREADY_EXISTS;

  StringTmpA<TEMPORARY_LENGTH> path8;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(path8, StubW(path, len)));

  if (::mkdir(path8.getData(), S_IRWXU | S_IXGRP | S_IXOTH) == 0) return ERR_OK;

  if (errno == EEXIST)
    return ERR_IO_DIR_ALREADY_EXISTS;
  else
    return errno;
}

err_t FileSystem::createDirectory(const StringW& dir, bool recursive)
{
  if (dir.isEmpty()) return ERR_RT_INVALID_ARGUMENT;
  if (!recursive) return createDirectoryHelper(dir.getData(), dir.getLength());

  err_t err;
  StringTmpW<TEMPORARY_LENGTH> dirAbs;
  if ( (err = toAbsolutePath(dirAbs, StringW(), dir)) ) return err;

  // FileSystem::toAbsolutePath() always normalize dir to '/', we can imagine
  // that dirAbs is absolute dir, so we need to find first two occurences
  // of '/'. Second occurece can be end of string.
  if (dirAbs.getLength() == 1 && dirAbs.getAt(0) == '/') return ERR_IO_DIR_ALREADY_EXISTS;

  size_t i = dirAbs.indexOf(CharW('/'));
  size_t length = dirAbs.getLength();

  if (i == INVALID_INDEX) return ERR_RT_INVALID_ARGUMENT;
  if (dirAbs.getAt(length-1) == CharW('/')) length--;

  do {
    i++;
    i = dirAbs.indexOf(Range(i, i + length), CharW('/'), CASE_SENSITIVE);

    err = createDirectoryHelper(dirAbs.getData(), (i == INVALID_INDEX) ? length : i);
    if (err != ERR_OK && err != ERR_IO_DIR_ALREADY_EXISTS) return err;
  } while (i != INVALID_INDEX);

  return ERR_OK;
}

err_t FileSystem::deleteDirectory(const StringW& dir)
{
  err_t err;
  StringTmpA<TEMPORARY_LENGTH> dir8;

  if ((err = TextCodec::local8().encode(dir8, dir))) return err;

  if (::rmdir(dir8.getData()) == 0)
    return ERR_OK;
  else
    return errno;
}
#endif // FOG_OS_POSIX

err_t FileSystem::extractFile(StringW& dst, const StringW& path)
{
  size_t index = path.lastIndexOf(CharW('/')) + 1U;

  if (index)
  {
    StringW path_(path);
    return dst.set(path_.getData() + index, path_.getLength() - index);
  }
  else
  {
    return dst.set(path);
  }
}

err_t FileSystem::extractDirectory(StringW& dst, const StringW& _path)
{
  size_t index = _path.lastIndexOf(CharW('/'));

  // in some cases (for example path /root), index can be 0. So check for this
  // case
  if (index != INVALID_INDEX)
  {
    StringW path(_path);
    return dst.set(path.getData(), index != 0 ? index : 1);
  }
  else
  {
    dst.clear();
    return ERR_OK;
  }
}

err_t FileSystem::extractExtension(StringW& dst, const StringW& _path)
{
  StringW path(_path);
  dst.clear();

  size_t pathLength = path.getLength();
  if (!pathLength) { return ERR_RT_INVALID_ARGUMENT; }

  // Speed is important, so RAW manipulation is needed
  const CharW* pathBegin = path.getData();
  const CharW* pathEnd = pathBegin + pathLength;
  const CharW* pathCur = pathEnd;

  do {
    if (FOG_UNLIKELY(*--pathCur == CharW('.')))
    {
      if (++pathCur != pathEnd)
      {
        return dst.set(pathCur, size_t(pathEnd - pathCur));
      }
      break;
    }
    else if (*pathCur == CharW('/'))
    {
      break;
    }
  } while (pathCur != pathBegin);

  return ERR_OK;
}

err_t FileSystem::normalizePath(StringW& dst, const StringW& _path)
{
  if (isNormalizedPath(_path)) return dst.set(_path);

  StringW path(_path);
  // we need to convert:
  // - all "//" sequences to "/"
  // - all "/./" to "/" or "BEGIN./" to ""
  // - all "/../" eats previous directory

  size_t pathLength = path.getLength();
  if (pathLength == 0)
  {
    dst.clear();
    return ERR_OK;
  }

  FOG_RETURN_ON_ERROR(dst.resize(pathLength));

  CharW* dstBeg = dst.getDataX();
  CharW* dstCur = dstBeg;

  const CharW* pathCur = path.getData();
  const CharW* pathEnd = pathCur + pathLength;

  CharW c;
  bool prevSlash = false;

  // Handle Windows absolute path "X:\"
#if defined(FOG_OS_WINDOWS)
  if (pathLength > 2 &&
    pathCur[0].isAsciiLetter() &&
    pathCur[1] == CharW(':') &&
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
    if (isDirSeparator(c) && (size_t)(dstCur - dstBeg) == 1 && dstCur[-1] == CharW('.'))
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
      if (c == CharW('.'))
      {
        size_t remain = (size_t)(pathEnd - pathCur);

        // Catch "/./" -> "/"
        if (remain > 0 && isDirSeparator(pathCur[0]))
        {
          // We were already written "/", so increase by one and continue
          pathCur++; continue;
        }

        // Catch "/../" -> Eat previous if is
        if (remain > 1 && pathCur[0] == CharW('.') && isDirSeparator(pathCur[1]))
        {
          // We know that dstCur[-1] contains '/', so we need to
          // skip this separator and get back last directory. Only what
          // we need to check if "../" possibility. In this case we can't
          // go back and we will append "../" to the result.

          size_t resultLength = (size_t)(dstCur - dstBeg);

          // Bail if result is too small to hold any directory
          if (resultLength <= 1) goto __inc;

          // check for "../"
          if (resultLength > 2 &&
            (dstCur - 3 == dstBeg || isDirSeparator(dstCur[-4])) &&
            dstCur[-3] == CharW('.') &&
            dstCur[-2] == CharW('.')) goto __inc;

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
    if (c == CharW('\\')) c = CharW('/');
#endif // FOG_OS_WINDOWS
    *dstCur++ = c;
  }

  dst._modified(dstCur);
  return ERR_OK;
}

err_t FileSystem::toAbsolutePath(StringW& dst, const StringW& base, const StringW& path)
{
  if (!FileSystem::isAbsolutePath(path))
  {
    err_t err;
    if (base.isEmpty())
    {
      StringTmpW<TEMPORARY_LENGTH> working;
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
  {
    return FileSystem::normalizePath(dst, path);
  }
}

static err_t _joinPath(StringW& dst, const StringW& base, const StringW& part)
{
  const CharW* d_str = base.getData();
  const CharW* f_str = part.getData();
  size_t d_length = base.getLength();
  size_t f_length = part.getLength();

  if (d_length && isDirSeparator(d_str[d_length-1])) d_length--;

  // This is not so readable but fast
  err_t err;
  if ( (err = dst.resize(d_length + f_length + 1)) ) return err;

  CharW* dstCur = dst.getDataX();
  dstCur = copyAndNorm(dstCur, d_str, d_length); *dstCur++ = '/';
  dstCur = copyAndNorm(dstCur, f_str, f_length);
  dst._modified();

  return ERR_OK;
}

err_t FileSystem::joinPath(StringW& dst, const StringW& base, const StringW& part)
{
  // if base or path is shared with dst, we need to do copy
  if (dst._d == base._d || dst._d == part._d)
    return _joinPath(dst, StringW(base), StringW(part));
  else
    return _joinPath(dst, base, part);
}

bool FileSystem::isPathContainsFile(const StringW& path, const StringW& file, uint cs)
{
  size_t index = path.lastIndexOf(CharW('/'));
  size_t length = path.getLength() - index;

  return (length == file.getLength() &&
    StringUtil::eq(path.getData() + index, file.getData(), length, cs));
}

bool FileSystem::isPathContainsDirectory(const StringW& path, const StringW& directory, uint cs)
{
  const CharW* d_str = directory.getData();
  size_t d_length = directory.getLength();

  if (d_length == 0) return false;
  if (isDirSeparator(d_str[d_length-1])) d_length--;

  return (path.getLength() > d_length &&
    isDirSeparator(path.getAt(d_length)) &&
    path.startsWith(StubW(d_str, d_length), cs));
}

bool FileSystem::isPathContainsExtension(const StringW& path, const StringW& extension, uint cs)
{
  size_t pathLength = path.getLength();
  if (!pathLength) return false;

  // Speed is important, so RAW manipulation is needed
  const CharW* pathBegin = path.getData();
  const CharW* pathEnd = pathBegin + pathLength;
  const CharW* pathCur = pathEnd;

  do {
    if (FOG_UNLIKELY(*--pathCur == CharW('.')))
    {
      pathCur++;
      return ((size_t)(pathEnd - pathCur) == extension.getLength() &&
        StringUtil::eq(pathCur, extension.getData(), extension.getLength(), cs));
    }
    else if (FOG_UNLIKELY(isDirSeparator(*pathCur)))
    {
      return false;
    }
  } while (pathCur != pathBegin);
  return false;
}

bool FileSystem::isNormalizedPath(const StringW& path)
{
  if (!FileSystem::isAbsolutePath(path)) return false;

#if defined(FOG_OS_WINDOWS)
  const CharW* pathBegin = path.getData();
  const CharW* pathCur = pathBegin;
  const CharW* pathEnd = pathBegin + path.getLength();

  // normalize X:\ form
  if (pathCur[2] == CharW('\\')) return false;

  pathCur += 3;
  while (pathCur < pathEnd)
  {
    // All "\" will be replaced to "/"
    if (pathCur[0] == CharW('\\')) return false;

    if (pathCur[0] == CharW('/'))
    {
      // test for "//"
      if (pathCur[-1] == CharW('/')) return false;
      if (pathCur[-1] == CharW('.'))
      {
        // NOTE: Here is no problem with reading out of
        // buffer range, because buffer often starts with
        // "\" if we are in this loop.

        // test for "/./"
        if (pathCur[-2] == CharW('/')) return false;
        // test for "/../"
        if (pathCur[-2] == CharW('.') && pathCur[-3] == CharW('/')) return false;
      }
    }
    pathCur++;
  }
  return true;
#else
  const CharW* pathBegin = path.getData();
  const CharW* pathCur = pathBegin;
  const CharW* pathEnd = pathBegin + path.getLength();

  if (*pathCur++ == CharW('/'))
  {
    while (pathCur != pathEnd)
    {

      if (pathCur[0] == CharW('/'))
      {
        // test for "//"
        if (pathCur[-1] == CharW('/')) return false;
        if (pathCur[-1] == CharW('.'))
        {
          // NOTE: Here is no problem reading out of buffer
          // range, because buffer often starts with "/" if
          // we are in this loop.

          // test for "/./"
          if (pathCur[-2] == CharW('/')) return false;
          // test for "/../"
          if (pathCur[-2] == CharW('.') && pathCur[-3] == CharW('/')) return false;
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

bool FileSystem::isAbsolutePath(const StringW& path)
{
#if defined(FOG_OS_WINDOWS)
  // We can accept that "[A-Za-z]:/" as absolute path
  if (path.getLength() > 2)
  {
    const CharW* pathStr = path.getData();
    return (pathStr[0].isAsciiLetter() &&
      pathStr[1] == CharW(':') &&
      isDirSeparator(pathStr[2]));
  }
  else
    return false;
#else
  return (path.getLength() != 0 && path.getAt(0) == CharW('/'));
#endif
}

bool FileSystem::testLocalName(const StringW& path)
{
#if defined(FOG_OS_WINDOWS)
  return true;
#else
  if (TextCodec::local8().isUnicode()) return true;

  StringTmpA<TEMPORARY_LENGTH> path8;
  return TextCodec::local8().encode(path8, path) == ERR_OK;
#endif
}

} // Fog namespace
