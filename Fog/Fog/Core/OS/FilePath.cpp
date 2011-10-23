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
#include <Fog/Core/OS/Environment.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/UserUtil.h>
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

static err_t _joinPath(StringW* dst,
  const CharW* bData, size_t bLength,
  const CharW* pData, size_t pLength)
{
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

static err_t FOG_CDECL FilePath_joinStubW(StringW* dst, const StubW* base, const StubW* part)
{
  return _joinPath(dst, base->getData(), base->getLength(),
                        part->getData(), part->getLength());
}

static err_t FOG_CDECL FilePath_joinStringW(StringW* dst, const StringW* base_, const StringW* part_)
{
  // if base or path is shared with dst, we need to do a copy first.
  if (dst->_d == base_->_d || dst->_d == part_->_d)
  {
    StringW base(*base_);
    StringW part(*part_);

    return _joinPath(dst, base.getData(), base.getLength(),
                          part.getData(), part.getLength());
  }
  else
  {
    return _joinPath(dst, base_->getData(), base_->getLength(),
                          part_->getData(), part_->getLength());
  }
}

// ============================================================================
// [Fog::FilePath - Extract]
// ============================================================================

static err_t FOG_CDECL FilePath_extractFileStubW(StringW* dst, const StubW* path)
{
  size_t pathLength = path->getComputedLength();
  size_t index = StringUtil::lastIndexOf(path->getData(), pathLength, CharW('/')) + 1U;

  return dst->set(path->getData() + index, pathLength - index);
}

static err_t FOG_CDECL FilePath_extractFileStringW(StringW* dst, const StringW* path)
{
  size_t index = path->lastIndexOf(CharW('/')) + 1U;
  return dst->set(*path, Range(index, path->getLength()));
}

static err_t FOG_CDECL FilePath_extractExtensionStubW(StringW* dst, const StubW* path)
{
  size_t pathLength = path->getComputedLength();

  if (pathLength != 0)
  {
    const CharW* pData = path->getData();
    const CharW* pEnd  = pData + pathLength;
    const CharW* p     = pEnd;

    do {
      if (*--p == CharW('.'))
      {
        if (++p != pEnd)
          return dst->set(p, (size_t)(pEnd - p));
        break;
      }

      if (*p == CharW('/'))
        break;
    } while (p != pData);
  }

  dst->clear();
  return ERR_OK;
}

static err_t FOG_CDECL FilePath_extractExtensionStringW(StringW* dst, const StringW* path)
{
  size_t pathLength = path->getLength();

  if (pathLength != 0)
  {
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

static err_t FOG_CDECL FilePath_extractDirectoryStubW(StringW* dst, const StubW* path)
{
  size_t pathLength = path->getComputedLength();
  size_t index = StringUtil::lastIndexOf(path->getData(), pathLength, CharW('/'));

  // In some cases (for example "/root"), index can be 0. So check for this case.
  if (index != INVALID_INDEX)
    return dst->set(path->getData(), index != 0 ? index : 1);

  dst->clear();
  return ERR_OK;
}

static err_t FOG_CDECL FilePath_extractDirectoryStringW(StringW* dst, const StringW* path)
{
  size_t index = path->lastIndexOf(CharW('/'));

  // In some cases (for example "/root"), index can be 0. So check for this case.
  if (index != INVALID_INDEX)
    return dst->set(*path, Range(0, index != 0 ? index : 1));

  dst->clear();
  return ERR_OK;
}

static bool FOG_CDECL FilePath_containsFileStubW(const StringW* path, const StubW* file, uint cs)
{
  size_t fileLength = file->getComputedLength();

  size_t index = path->lastIndexOf(CharW('/'));
  size_t length = path->getLength() - index;

  return (length == fileLength &&
    StringUtil::eq(path->getData() + index, file->getData(), length, cs));
}

static bool FOG_CDECL FilePath_containsFileStringW(const StringW* path, const StringW* file, uint cs)
{
  size_t index = path->lastIndexOf(CharW('/'));
  size_t length = path->getLength() - index;

  return (length == file->getLength() &&
    StringUtil::eq(path->getData() + index, file->getData(), length, cs));
}

static FOG_INLINE bool FilePath_containsExtensionRaw(const CharW* pData, size_t pLength, const CharW* eData, size_t eLength, uint cs)
{
  const CharW* pBeg = pData;
  const CharW* pEnd = pData + pLength;
  const CharW* p = pEnd;

  do {
    if (FOG_UNLIKELY(*--p == CharW('.')))
    {
      p++;
      return ((size_t)(pEnd - p) == eLength &&
        StringUtil::eq(p, eData, eLength, cs));
    }
    else if (FOG_UNLIKELY(FilePath::isAnyDirSeparator(*p)))
    {
      return false;
    }
  } while (p != pBeg);
  return false;
}

static bool FOG_CDECL FilePath_containsExtensionStubW(const StringW* path, const StubW* extension, uint cs)
{
  size_t pathLength = path->getLength();
  if (!pathLength)
    return false;

  return FilePath_containsExtensionRaw(path->getData(), pathLength, extension->getData(), extension->getComputedLength(), cs);
}

static bool FOG_CDECL FilePath_containsExtensionStringW(const StringW* path, const StringW* extension, uint cs)
{
  size_t pathLength = path->getLength();
  if (!pathLength)
    return false;

  return FilePath_containsExtensionRaw(path->getData(), pathLength, extension->getData(), extension->getLength(), cs);
}

static FOG_INLINE bool FilePath_containsDirectoryRaw(const StringW* path, const CharW* dData, size_t dLength, uint cs)
{
  if (dLength == 0)
    return false;

  if (FilePath::isAnyDirSeparator(dData[dLength-1]))
    dLength--;

  return (path->getLength() > dLength &&
    FilePath::isAnyDirSeparator(path->getAt(dLength)) &&
    path->startsWith(StubW(dData, dLength), cs));
}

static bool FOG_CDECL FilePath_containsDirectoryStubW(const StringW* path, const StubW* directory, uint cs)
{
  return FilePath_containsDirectoryRaw(path, directory->getData(), directory->getComputedLength(), cs);
}

static bool FOG_CDECL FilePath_containsDirectoryStringW(const StringW* path, const StringW* directory, uint cs)
{
  return FilePath_containsDirectoryRaw(path, directory->getData(), directory->getLength(), cs);
}

// ============================================================================
// [Fog::FilePath - Normalize]
// ============================================================================

static err_t FilePath_normalizeRaw(StringW* dst, const CharW* s, size_t sLength)
{
  // we need to convert:
  // - all "//" sequences to "/"
  // - all "/./" to "/" or "BEGIN./" to ""
  // - all "/../" eats previous directory

  if (sLength == 0)
  {
    dst->clear();
    return ERR_OK;
  }

  CharW* pBegin = dst->_prepare(CONTAINER_OP_REPLACE, sLength);
  CharW* p = pBegin;

  if (pBegin == NULL)
    return ERR_RT_OUT_OF_MEMORY;

  const CharW* sPtr = s;
  const CharW* sEnd = sPtr + sLength;

  CharW c;
  bool prevSlash = false;

  // Handle Windows absolute path "X:\".
#if defined(FOG_OS_WINDOWS)
  if (sLength > 2 &&
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

static err_t FOG_CDECL FilePath_normalizeStubW(StringW* dst, const StubW* _path)
{
  if (FilePath::isNormalized(*_path))
    return dst->set(*_path);

  return FilePath_normalizeRaw(dst, _path->getData(), _path->getComputedLength());
}

static err_t FOG_CDECL FilePath_normalizeStringW(StringW* dst, const StringW* _path)
{
  if (FilePath::isNormalized(*_path))
    return dst->set(*_path);

  StringW path(*_path);
  return FilePath_normalizeRaw(dst, path.getData(), path.getLength());
}

static bool FilePath_isNormalizedRaw(const CharW* s, size_t sLength)
{
  const CharW* sEnd = s + sLength;

#if defined(FOG_OS_WINDOWS)
  // Normalize X:\ form.
  if (s[2] == CharW('\\'))
    return false;

  s += 3;
  while (s < sEnd)
  {
    // All "\" will be replaced to "/".
    if (s[0] == CharW('\\'))
      return false;

    if (s[0] == CharW('/'))
    {
      // Test for "//".
      if (s[-1] == CharW('/'))
        return false;

      if (s[-1] == CharW('.'))
      {
        // NOTE: Here is no problem with reading out of buffer range, because
        // buffer often starts with "\" if we are in this loop.

        // Test for "/./".
        if (s[-2] == CharW('/'))
          return false;

        // Test for "/../".
        if (s[-2] == CharW('.') && s[-3] == CharW('/'))
          return false;
      }
    }
    s++;
  }
  return true;
#else
  if (*s++ == CharW('/'))
  {
    while (s != sEnd)
    {

      if (s[0] == CharW('/'))
      {
        // Test for "//".
        if (s[-1] == CharW('/'))
          return false;

        if (s[-1] == CharW('.'))
        {
          // NOTE: Here is no problem reading out of buffer range, because
          // buffer often starts with "/" if we are in this loop.

          // Test for "/./".
          if (s[-2] == CharW('/'))
            return false;

          // Test for "/../".
          if (s[-2] == CharW('.') && s[-3] == CharW('/'))
            return false;
        }
      }
      s++;
    }
    return true;
  }
  return false;
#endif
}

static bool FOG_CDECL FilePath_isNormalizedStubW(const StubW* path)
{
  if (!FilePath::isAbsolute(*path))
    return false;
  else
    return FilePath_isNormalizedRaw(path->getData(), path->getComputedLength());
}

static bool FOG_CDECL FilePath_isNormalizedStringW(const StringW* path)
{
  if (!FilePath::isAbsolute(*path))
    return false;
  else
    return FilePath_isNormalizedRaw(path->getData(), path->getLength());
}

// ============================================================================
// [Fog::FilePath - Root]
// ============================================================================

static FOG_INLINE bool FilePath_isRootRaw(const CharW* s, size_t sLength)
{
#if defined(FOG_OS_WINDOWS)
  if (sLength < 2 || !s[0].isAsciiLetter() || s[1] != CharW(':'))
    return false;
  if (sLength == 2)
    return true;
  if (sLength == 3 && s[2] == CharW('/'))
    return true;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  if (sLength == 1 && s[0] == CharW('/'))
    return true;
#endif // FOG_OS_POSIX

  return false;
}

static bool FOG_CDECL FilePath_isRootStubW(const StubW* path)
{
  StringTmpW<TEMPORARY_LENGTH> norm;
  if (FilePath::normalize(norm, *path) != ERR_OK)
    return false;
  else
    return FilePath_isRootRaw(norm.getData(), norm.getLength());
}

static bool FOG_CDECL FilePath_isRootStringW(const StringW* path)
{
  StringTmpW<TEMPORARY_LENGTH> norm;
  if (FilePath::normalize(norm, *path) != ERR_OK)
    return false;
  else
    return FilePath_isRootRaw(norm.getData(), norm.getLength());
}

// ============================================================================
// [Fog::FilePath - Relative / Absolute]
// ============================================================================

static err_t FOG_CDECL FilePath_toAbsoluteStubW(StringW* dst, const StubW* path, const StubW* base)
{
  if (FilePath::isAbsolute(*path))
    return FilePath::normalize(*dst, *path);

  size_t baseLength;
  if (base == NULL || (baseLength = base->getComputedLength()) == 0)
  {
    StringTmpW<TEMPORARY_LENGTH> working;

    FOG_RETURN_ON_ERROR(Application::getWorkingDirectory(working));
    FOG_RETURN_ON_ERROR(FilePath::join(*dst, StubW(working.getData(), working.getLength()), *path));
  }
  else
  {
    FOG_RETURN_ON_ERROR(FilePath::join(*dst, *base, *path));
  }

  return FilePath::normalize(*dst, *dst);
}

static err_t FOG_CDECL FilePath_toAbsoluteStringW(StringW* dst, const StringW* path, const StringW* base)
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

static FOG_INLINE bool FilePath_isAbsoluteRaw(const CharW* s, size_t sLength)
{
#if defined(FOG_OS_WINDOWS)
  // We can accept that "[A-Za-z]:/" as an absolute path.
  if (sLength > 2)
  {
    return s[0].isAsciiLetter() && s[1] == CharW(':') && FilePath::isAnyDirSeparator(s[2]);
  }
  else
    return false;
#else
  return (sLength != 0 && s[0] == CharW('/'));
#endif
}

static bool FOG_CDECL FilePath_isAbsoluteStubW(const StubW* path)
{
  return FilePath_isAbsoluteRaw(path->getData(), path->getComputedLength());
}

static bool FOG_CDECL FilePath_isAbsoluteStringW(const StringW* path)
{
  return FilePath_isAbsoluteRaw(path->getData(), path->getLength());
}

// ============================================================================
// [Fog::FilePath - Environment Substitution]
// ============================================================================

static err_t FOG_CDECL FilePath_substituteFunc(StringW* dst, const StringW* key)
{
  StringTmpW<256> value;

  FOG_RETURN_ON_ERROR(Environment::getValue(*key, value));
  FOG_RETURN_ON_ERROR(dst->append(value));

  return ERR_OK;
}

static err_t FOG_CDECL FilePath_substituteEnvironmentVars(StringW* _dst, const StringW* path, uint32_t format)
{
  err_t err = ERR_OK;
  size_t length = path->getLength();

  const CharW* p = path->getData();
  const CharW* pEnd = p + length;
  const CharW* mark;

  StringW tmp;
  StringW* dst = _dst;

  StringTmpW<256> varName;

  if (dst == path)
    dst = &tmp;

  if (length == 0)
    return dst->set(*path);

  err = dst->reserve(128);
  if (FOG_IS_ERROR(err))
    goto _Fail;

  // --------------------------------------------------------------------------
  // [Detect HOME]
  // --------------------------------------------------------------------------

  {
    uint32_t homeLen = 0;

    if (p[0] == CharW('~'))
    {
      homeLen = 1;
    }
    else
    {
      if ((format & FILE_PATH_SUBSTITUTE_FORMAT_WINDOWS) != 0)
      {
        if (length >= 6 && StringUtil::eq(p, "%HOME%", 6))
          homeLen = 6;
        else if (length >= 14 && StringUtil::eq(p, "%USER_PROFILE%", 14))
          homeLen = 14;
      }

      if ((format & FILE_PATH_SUBSTITUTE_FORMAT_UNIX) != 0)
      {
        if (length >= 5 && StringUtil::eq(p, "$HOME", 5) &&
           (length == 5 || !(p[5].isNumlet() || p[5] == CharW('_'))))
          homeLen = 5;
        else if (length >= 7 && StringUtil::eq(p, "${HOME}", 7))
          homeLen = 7;
      }
    }

    if (homeLen)
    {
      FOG_RETURN_ON_ERROR(UserUtil::getUserDirectory(*dst, USER_DIRECTORY_HOME));
      p += homeLen;
    }
  }

  // --------------------------------------------------------------------------
  // [Detect Environment Variables]
  // --------------------------------------------------------------------------

  mark = p;

  if (format != FILE_PATH_SUBSTITUTE_FORMAT_NONE)
  {
    const CharW* variableName = NULL;
    size_t variableLen = 0;

    while (p != pEnd)
    {
      CharW c = p[0];

      // UNIX style.
      if (c == CharW('$') && (format & FILE_PATH_SUBSTITUTE_FORMAT_UNIX))
      {
        if (mark != p)
        {
          err = dst->append(StubW(mark, (size_t)(p - mark)));
          if (FOG_IS_ERROR(err))
            goto _Fail;
          mark = p;
        }

        if (++p == pEnd)
          goto _End;

        c = p[0];
        bool hasBracket = c == CharW('{');

        // Detect "{"
        if (hasBracket)
        {
          if (++p == pEnd)
            goto _End;

          variableName = p;

          // Environment name must start with a letter.
          c = p[0];
          if (!(c.isAsciiLetter() || c == CharW('_')))
            goto _BadSyntax;

          for (;;)
          {
            c = p[0];
            if (!(c.isAsciiNumlet() || c == CharW('_')))
            {
              if (c == CharW('}'))
              {
                variableLen = (size_t)(p - variableName);
                p++;
                break;
              }
              else
                goto _BadSyntax;
            }

            if (++p == pEnd)
              goto _BadSyntax;
          }
        }
        else
        {
          // Environment name must start with a letter.
          c = p[0];
          if (!(c.isAsciiLetter() || c == CharW('_')))
            goto _BadSyntax;

          variableName = p;

          for (;;)
          {
            c = p[0];
            if (!(c.isAsciiNumlet() || c == CharW('_')))
              break;

            if (++p == pEnd)
              break;
          }

          variableLen = (size_t)(p - variableName);
        }

        goto _Substitute;
      }
      // Windows style.
      else if (c == CharW('%') && (format & FILE_PATH_SUBSTITUTE_FORMAT_WINDOWS))
      {
        if (mark != p)
        {
          err = dst->append(StubW(mark, (size_t)(p - mark)));
          if (FOG_IS_ERROR(err))
            goto _Fail;
          mark = p;
        }

        if (++p == pEnd)
          goto _End;

        variableName = p;

        // Environment name must start with a letter.
        c = p[0];
        if (!(c.isAsciiLetter() || c == CharW('_')))
          goto _BadSyntax;

        for (;;)
        {
          c = p[0];
          if (!(c.isAsciiNumlet() || c == CharW('_')))
          {
            if (c == CharW('%'))
            {
              variableLen = (size_t)(p - variableName);
              p++;
              break;
            }
            else
              goto _BadSyntax;
          }

          if (++p == pEnd)
            goto _BadSyntax;
        }

_Substitute:
        err = varName.set(StubW(variableName, variableLen));
        if (FOG_IS_ERROR(err))
          goto _Fail;

        err = FilePath_substituteFunc(dst, &varName);
        if (FOG_IS_ERROR(err))
          goto _Fail;

        mark = p;
        if (p == pEnd)
          break;
      }

      p++;
    }
  }

_End:
  if (mark != pEnd)
    err = dst->append(StubW(mark, (size_t)(pEnd - mark)));

  if (FOG_IS_ERROR(err))
  {
_Fail:
    _dst->clear();
  }
  else if (dst == &tmp)
  {
    _dst->set(tmp);
  }

  return err;

_BadSyntax:
  err = ERR_PATH_BAD_SYNTAX;
  goto _Fail;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FilePath_init(void)
{
  fog_api.filepath_joinStubW = FilePath_joinStubW;
  fog_api.filepath_joinStringW = FilePath_joinStringW;

  fog_api.filepath_extractFileStubW = FilePath_extractFileStubW;
  fog_api.filepath_extractFileStringW = FilePath_extractFileStringW;

  fog_api.filepath_extractExtensionStubW = FilePath_extractExtensionStubW;
  fog_api.filepath_extractExtensionStringW = FilePath_extractExtensionStringW;

  fog_api.filepath_extractDirectoryStubW = FilePath_extractDirectoryStubW;
  fog_api.filepath_extractDirectoryStringW = FilePath_extractDirectoryStringW;

  fog_api.filepath_containsFileStubW = FilePath_containsFileStubW;
  fog_api.filepath_containsFileStringW = FilePath_containsFileStringW;

  fog_api.filepath_containsExtensionStubW = FilePath_containsExtensionStubW;
  fog_api.filepath_containsExtensionStringW = FilePath_containsExtensionStringW;

  fog_api.filepath_containsDirectoryStubW = FilePath_containsDirectoryStubW;
  fog_api.filepath_containsDirectoryStringW = FilePath_containsDirectoryStringW;

  fog_api.filepath_normalizeStubW = FilePath_normalizeStubW;
  fog_api.filepath_normalizeStringW = FilePath_normalizeStringW;

  fog_api.filepath_isNormalizedStubW = FilePath_isNormalizedStubW;
  fog_api.filepath_isNormalizedStringW = FilePath_isNormalizedStringW;

  fog_api.filepath_isRootStubW = FilePath_isRootStubW;
  fog_api.filepath_isRootStringW = FilePath_isRootStringW;

  fog_api.filepath_toAbsoluteStubW = FilePath_toAbsoluteStubW;
  fog_api.filepath_toAbsoluteStringW = FilePath_toAbsoluteStringW;

  fog_api.filepath_isAbsoluteStubW = FilePath_isAbsoluteStubW;
  fog_api.filepath_isAbsoluteStringW = FilePath_isAbsoluteStringW;

  fog_api.filepath_substituteEnvironmentVars = FilePath_substituteEnvironmentVars;
}

} // Fog namespace
