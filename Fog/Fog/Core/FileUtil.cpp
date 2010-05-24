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
#include <Fog/Core/FileUtil.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/UserInfo.h>

namespace Fog {

// ============================================================================
// [Fog::FileUtil]
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

err_t FileUtil::extractFile(String& dst, const String& path)
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

err_t FileUtil::extractDirectory(String& dst, const String& _path)
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

err_t FileUtil::extractExtension(String& dst, const String& _path)
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

err_t FileUtil::normalizePath(String& dst, const String& _path)
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

err_t FileUtil::toAbsolutePath(String& dst, const String& base, const String& path)
{
  if (!FileUtil::isAbsolutePath(path))
  {
    err_t err;
    if (base.isEmpty())
    {
      TemporaryString<TEMPORARY_LENGTH> working;
      if ( (err = Application::getWorkingDirectory(working)) ) return err;
      if ( (err = FileUtil::joinPath(dst, working, path)) ) return err;
    }
    else
    {
      if ( (err = FileUtil::joinPath(dst, base, path)) ) return err;
    }
    return FileUtil::normalizePath(dst, dst);
  }
  else
    return FileUtil::normalizePath(dst, path);
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

err_t FileUtil::joinPath(String& dst, const String& base, const String& part)
{
  // if base or path is shared with dst, we need to do copy
  if (dst._d == base._d || dst._d == part._d)
    return _joinPath(dst, String(base), String(part));
  else
    return _joinPath(dst, base, part);
}

bool FileUtil::isPathContainsFile(const String& path, const String& file, uint cs)
{
  sysuint_t index = path.lastIndexOf(Char('/'));
  sysuint_t length = path.getLength() - index;

  return (length == file.getLength() &&
    StringUtil::eq(path.getData() + index, file.getData(), length, cs));
}

bool FileUtil::isPathContainsDirectory(const String& path, const String& directory, uint cs)
{
  const Char* d_str = directory.getData();
  sysuint_t d_length = directory.getLength();

  if (d_length == 0) return false;
  if (isDirSeparator(d_str[d_length-1])) d_length--;

  return (path.getLength() > d_length &&
    isDirSeparator(path.at(d_length)) &&
    path.startsWith(Utf16(d_str, d_length), cs));
}

bool FileUtil::isPathContainsExtension(const String& path, const String& extension, uint cs)
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

bool FileUtil::isNormalizedPath(const String& path)
{
  if (!FileUtil::isAbsolutePath(path)) return false;

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

bool FileUtil::isAbsolutePath(const String& path)
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

bool FileUtil::testLocalName(const String& path)
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
