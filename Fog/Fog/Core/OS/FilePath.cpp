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
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ============================================================================
// [Fog::FilePath - Helpers]
// ============================================================================

// Copy src to dst and convert slashes to posix form
static CharW* FilePath_copyNorm(CharW* dst, const CharW* src, size_t length)
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

static err_t _joinPath(StringW* dst, const StringW* base, const StringW* part)
{
  const CharW* bData = base->getData();
  const CharW* pData = part->getData();
  size_t bLength = base->getLength();
  size_t pLength = part->getLength();

  if (bLength && FilePath::isAnyDirSeparator(bData[bLength-1]))
    bLength--;

  CharW* dData = dst->_prepare(CONTAINER_OP_REPLACE, bLength + pLength + 1);

  if (FOG_IS_NULL(dData))
    return ERR_RT_OUT_OF_MEMORY;

  dData = FilePath_copyNorm(dData, bData, bLength);
  *dData++ = '/';
  dData = FilePath_copyNorm(dData, pData, pLength);

  return ERR_OK;
}

// ============================================================================
// [Fog::FilePath - Join]
// ============================================================================

static err_t FOG_CDECL FilePath_join(StringW* dst, const StringW* base_, const StringW* part_)
{
  // if base or path is shared with dst, we need to do a copy first.
  if (dst->_d == base_->_d || dst->_d == part_->_d)
  {
    StringW base(*base_);
    StringW part(*part_);
    return _joinPath(dst, &base, &part);
  }
  else
  {
    return _joinPath(dst, base_, part_);
  }
}

// ============================================================================
// [Fog::FilePath - Extract]
// ============================================================================

static err_t FOG_CDECL FilePath_extractFile(StringW* dst, const StringW* path)
{
  size_t index = path->lastIndexOf(CharW('/')) + 1U;
  return dst->set(*path, Range(index, path->getLength()));
}

static err_t FOG_CDECL FilePath_extractExtension(StringW* dst, const StringW* path)
{
  size_t pathLength = path->getLength();

  if (pathLength != 0)
  {
    // Speed is important, so RAW manipulation is needed
    const CharW* pData = path->getData();
    const CharW* pEnd  = pData + pathLength;
    const CharW* p     = pEnd;

    do {
      if (*--p == CharW('.'))
      {
        if (++p != pEnd)
          return dst->set(*path, Range(size_t(p - pData), pathLength));
        break;
      }

      if (*p == CharW('/'))
        break;
    } while (p != pData);
  }

  dst->clear();
  return ERR_OK;
}

static err_t FOG_CDECL FilePath_extractDirectory(StringW* dst, const StringW* path)
{
  size_t index = path->lastIndexOf(CharW('/'));

  // In some cases (for example "/root"), index can be 0. So check for this case.
  if (index != INVALID_INDEX)
    return dst->set(*path, Range(0, index != 0 ? index : 1));

  dst->clear();
  return ERR_OK;
}

static bool FOG_CDECL FilePath_containsFile(const StringW* path, const StringW* file, uint cs)
{
  size_t index = path->lastIndexOf(CharW('/'));
  size_t length = path->getLength() - index;

  return (length == file->getLength() &&
    StringUtil::eq(path->getData() + index, file->getData(), length, cs));
}

static bool FOG_CDECL FilePath_containsExtension(const StringW* path, const StringW* extension, uint cs)
{
  size_t pathLength = path->getLength();
  if (!pathLength) return false;

  // Speed is important, so RAW manipulation is needed
  const CharW* pathBegin = path->getData();
  const CharW* pathEnd = pathBegin + pathLength;
  const CharW* pathCur = pathEnd;

  do {
    if (FOG_UNLIKELY(*--pathCur == CharW('.')))
    {
      pathCur++;
      return ((size_t)(pathEnd - pathCur) == extension->getLength() &&
        StringUtil::eq(pathCur, extension->getData(), extension->getLength(), cs));
    }
    else if (FOG_UNLIKELY(FilePath::isAnyDirSeparator(*pathCur)))
    {
      return false;
    }
  } while (pathCur != pathBegin);
  return false;
}

static bool FOG_CDECL FilePath_containsDirectory(const StringW* path, const StringW* directory, uint cs)
{
  const CharW* dData = directory->getData();
  size_t dLength = directory->getLength();

  if (dLength == 0)
    return false;

  if (FilePath::isAnyDirSeparator(dData[dLength-1]))
    dLength--;

  return (path->getLength() > dLength &&
    FilePath::isAnyDirSeparator(path->getAt(dLength)) &&
    path->startsWith(StubW(dData, dLength), cs));
}

// ============================================================================
// [Fog::FilePath - Normalize]
// ============================================================================

static err_t FOG_CDECL FilePath_normalize(StringW* dst, const StringW* _path)
{
  if (FilePath::isNormalized(*_path))
    return dst->set(*_path);

  StringW path(*_path);
  // we need to convert:
  // - all "//" sequences to "/"
  // - all "/./" to "/" or "BEGIN./" to ""
  // - all "/../" eats previous directory

  size_t pathLength = path.getLength();
  if (pathLength == 0)
  {
    dst->clear();
    return ERR_OK;
  }

  CharW* pBegin = dst->_prepare(CONTAINER_OP_REPLACE, pathLength);
  CharW* p = pBegin;

  if (pBegin == NULL)
    return ERR_RT_OUT_OF_MEMORY;

  const CharW* sPtr = path.getData();
  const CharW* sEnd = sPtr + pathLength;

  CharW c;
  bool prevSlash = false;

  // Handle Windows absolute path "X:\".
#if defined(FOG_OS_WINDOWS)
  if (pathLength > 2 &&
    sPtr[0].isAsciiLetter() &&
    sPtr[1] == CharW(':') &&
    FilePath::isAnyDirSeparator(sPtr[2]))
  {
    p[0] = sPtr[0];
    p[1] = sPtr[1];

    // We will make path like absolute, next slash will be handled in loop and 
    // corrected.
    pBegin += 2; 
    p      += 2;
    sPtr   += 2;
  }
#endif // FOG_OS_WINDOWS

  while (sPtr < sEnd)
  {
    c = *sPtr++;

    // Skip "./" from result.
    if (FilePath::isAnyDirSeparator(c) && (size_t)(p - pBegin) == 1 && p[-1] == CharW('.'))
    {
      prevSlash = true;
      p--;
      continue;
    }

    if (prevSlash)
    {
      // Catch "//" -> "/" sequence.
      if (FilePath::isAnyDirSeparator(c))
        // We were already written "/", so continue is ok.
        continue;

      // Catch "/."
      if (c == CharW('.'))
      {
        size_t remain = (size_t)(sEnd - sPtr);

        // Catch "/./" -> "/".
        if (remain > 0 && FilePath::isAnyDirSeparator(sPtr[0]))
        {
          // We were already written "/", so increase by one and continue.
          sPtr++; continue;
        }

        // Catch "/../" -> Eat previous if is.
        if (remain > 1 && sPtr[0] == CharW('.') && FilePath::isAnyDirSeparator(sPtr[1]))
        {
          // We know that p[-1] contains '/', so we need to skip this separator
          // and get back last directory. Only what we need to check if "../"
          // possibility. In this case we can't go back and we will append "../"
          // to the result.

          size_t resultLength = (size_t)(p - pBegin);

          // Bail if result is too small to hold any directory.
          if (resultLength <= 1) goto _Inc;

          // check for "../".
          if (resultLength > 2 &&
            (p - 3 == pBegin || FilePath::isAnyDirSeparator(p[-4])) &&
            p[-3] == CharW('.') &&
            p[-2] == CharW('.')) goto _Inc;

          // Now we can continue.
          p -= 2;
          sPtr += 2;

          for (;;)
          {
            c = *p;
            if (FilePath::isAnyDirSeparator(c)) goto _Inc;
            if (p == pBegin)
            {
              prevSlash = false;
              break;
            }
            p--;
          }
          continue;
        }
      }
    }

_Inc:
    prevSlash = FilePath::isAnyDirSeparator(c);
#if defined(FOG_OS_WINDOWS)
    if (c == CharW('\\')) c = CharW('/');
#endif // FOG_OS_WINDOWS
    *p++ = c;
  }

  dst->_modified(p);
  return ERR_OK;
}

static bool FOG_CDECL FilePath_isNormalized(const StringW* path)
{
  if (!FilePath::isAbsolute(*path))
    return false;

  const CharW* p    = path->getData();
  const CharW* pEnd = p + path->getLength();

#if defined(FOG_OS_WINDOWS)
  // Normalize X:\ form.
  if (p[2] == CharW('\\'))
    return false;

  p += 3;
  while (p < pEnd)
  {
    // All "\" will be replaced to "/".
    if (p[0] == CharW('\\'))
      return false;

    if (p[0] == CharW('/'))
    {
      // Test for "//".
      if (p[-1] == CharW('/'))
        return false;

      if (p[-1] == CharW('.'))
      {
        // NOTE: Here is no problem with reading out of buffer range, because
        // buffer often starts with "\" if we are in this loop.

        // Test for "/./".
        if (p[-2] == CharW('/'))
          return false;

        // Test for "/../".
        if (p[-2] == CharW('.') && p[-3] == CharW('/'))
          return false;
      }
    }
    p++;
  }
  return true;
#else
  if (*p++ == CharW('/'))
  {
    while (p != pEnd)
    {

      if (p[0] == CharW('/'))
      {
        // Test for "//".
        if (p[-1] == CharW('/'))
          return false;

        if (p[-1] == CharW('.'))
        {
          // NOTE: Here is no problem reading out of buffer range, because 
          // buffer often starts with "/" if we are in this loop.

          // Test for "/./".
          if (p[-2] == CharW('/'))
            return false;

          // Test for "/../".
          if (p[-2] == CharW('.') && p[-3] == CharW('/'))
            return false;
        }
      }
      p++;
    }
    return true;
  }
  return false;
#endif
}

// ============================================================================
// [Fog::FilePath - Root]
// ============================================================================

static bool FOG_CDECL FilePath_isRoot(const StringW* path)
{
  StringTmpW<TEMPORARY_LENGTH> norm;
  if (FilePath::normalize(norm, *path) != ERR_OK)
    return false;

  const CharW* p = norm.getData();
  size_t length = norm.getLength();

#if defined(FOG_OS_WINDOWS)
  if (length < 2 || !p[0].isAsciiLetter() || p[1] != CharW(':'))
    return false;

  if (length == 2)
    return true;
  if (length == 3 && p[2] == CharW('/'))
    return true;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  if (length == 1 && p[0] == CharW('/'))
    return true;
#endif // FOG_OS_POSIX

  return false;
}

// ============================================================================
// [Fog::FilePath - Relative / Absolute]
// ============================================================================

static err_t FOG_CDECL FilePath_toAbsolute(StringW* dst, const StringW* path, const StringW* base)
{
  if (FilePath::isAbsolute(*path))
    return FilePath::normalize(*dst, *path);

  if (base == NULL || base->isEmpty())
  {
    StringTmpW<TEMPORARY_LENGTH> working;
    FOG_RETURN_ON_ERROR(Application::getWorkingDirectory(working));
    FOG_RETURN_ON_ERROR(FilePath::join(*dst, working, *path));
  }
  else
  {
    FOG_RETURN_ON_ERROR(FilePath::join(*dst, *base, *path));
  }

  return FilePath::normalize(*dst, *dst);
}

static bool FOG_CDECL FilePath_isAbsolute(const StringW* path)
{
#if defined(FOG_OS_WINDOWS)
  // We can accept that "[A-Za-z]:/" as an absolute path.
  if (path->getLength() > 2)
  {
    const CharW* s = path->getData();
    return s[0].isAsciiLetter() && s[1] == CharW(':') && FilePath::isAnyDirSeparator(s[2]);
  }
  else
    return false;
#else
  return (path->getLength() != 0 && path->getAt(0) == CharW('/'));
#endif
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FilePath_init(void)
{
  _api.filepath_join = FilePath_join;

  _api.filepath_extractFile = FilePath_extractFile;
  _api.filepath_extractExtension = FilePath_extractExtension;
  _api.filepath_extractDirectory = FilePath_extractDirectory;

  _api.filepath_containsFile = FilePath_containsFile;
  _api.filepath_containsExtension = FilePath_containsExtension;
  _api.filepath_containsDirectory = FilePath_containsDirectory;

  _api.filepath_normalize = FilePath_normalize;
  _api.filepath_isNormalized = FilePath_isNormalized;

  _api.filepath_isRoot = FilePath_isRoot;

  _api.filepath_toAbsolute = FilePath_toAbsolute;
  _api.filepath_isAbsolute = FilePath_isAbsolute;
}

} // Fog namespace
