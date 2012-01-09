// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/FileInfo.h>
#include <Fog/Core/OS/DirIterator.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/OS/WinUtil.h>
# include <io.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <dirent.h>
# include <errno.h>
# include <sys/stat.h>
# if defined(FOG_HAVE_UNISTD_H)
#  include <unistd.h>
# endif
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::DirIterator - Global]
// ============================================================================

static Static<DirIteratorData> DirIterator_dEmpty;

// ============================================================================
// [Fog::DirIterator - Construction / Destruction]
// ============================================================================

static void FOG_CDECL DirIterator_ctor(DirIterator* self)
{
  self->_d = &DirIterator_dEmpty;
}

static void FOG_CDECL DirIterator_ctorString(DirIterator* self, const StringW* path, bool skipParent)
{
  self->_d = &DirIterator_dEmpty;
  fog_api.diriterator_open(self, path, skipParent);
}

static void FOG_CDECL DirIterator_dtor(DirIterator* self)
{
  if (self->_d == &DirIterator_dEmpty)
    return;
  fog_api.diriterator_close(self);
}

// ============================================================================
// [Fog::DirIterator - Helpers (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

struct FOG_NO_EXPORT WinDirIteratorData : public DirIteratorData
{
  Static<StringW> pathOS;
  int64_t position;
};

static WinDirIteratorData* WinDirIterator_dCreate(void)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(MemMgr::alloc(sizeof(WinDirIteratorData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->handle = NULL;
  d->skipParent = 0;

  d->pathAbs.init();
  d->pathOS.init();
  d->position = 0;

  return d;
}

static void WinDirIterator_dFree(WinDirIteratorData* d)
{
  d->pathAbs.destroy();
  d->pathOS.destroy();
  MemMgr::free(d);
}

// ============================================================================
// [Fog::DirIterator - Open / Close (Windows)]
// ============================================================================

static err_t FOG_CDECL DirIterator_open(DirIterator* self, const StringW* path, bool skipParent)
{
  // If we know that the FindFirstFile() first finds the '.' folder, then the
  // '..' folder, then the first item in the folder then we can simply ignore
  // the first file and we do not need to store WIN32_FIND_DATAW structure
  // in 'd'.
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);
  WIN32_FIND_DATAW wfd;

  if (d != &DirIterator_dEmpty)
  {
    ::FindClose((HANDLE)d->handle);

    d->pathAbs->clear();
    d->pathOS->clear();
    d->position = 0;
  }
  else
  {
    d = WinDirIterator_dCreate();
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  d->skipParent = skipParent;

  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(d->pathAbs, *path));
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(d->pathOS, d->pathAbs));
  FOG_RETURN_ON_ERROR(d->pathOS->append(Ascii8("\\*")));

  d->handle = (void*)FindFirstFileW(reinterpret_cast<const wchar_t*>(d->pathOS->getData()), &wfd);

  // Try to open with long file name support if path is too long.
  if (d->handle == (void*)INVALID_HANDLE_VALUE)
  {
    WinDirIterator_dFree(d);
    self->_d = &DirIterator_dEmpty;
    return OSUtil::getErrFromOSLastError();
  }

  self->_d = d;
  return ERR_OK;
}

static void FOG_CDECL DirIterator_close(DirIterator* self)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return;

  ::FindClose((HANDLE)d->handle);
  WinDirIterator_dFree(d);

  self->_d = &DirIterator_dEmpty;
}

// ============================================================================
// [Fog::DirIterator - Read (Windows)]
// ============================================================================

static bool DirIterator_readWIN32FINDDATA(DirIterator* self, WIN32_FIND_DATAW* wfd)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);
  HANDLE handle = (HANDLE)d->handle;

  if (!handle)
    return false;

  for (;;)
  {
    if (d->position != 0 && ::FindNextFileW(handle, wfd) == 0)
      return false;

    // Skip "." and "..".
    d->position++;
    if ((wfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && d->skipParent)
    {
      const WCHAR* fn = wfd->cFileName;
      if (fn[0] == L'.' && fn[1] == L'.' && fn[2] == 0)
        continue;
    }

    return true;
  }
}

static bool FOG_CDECL DirIterator_readFileInfo(DirIterator* self, FileInfo* fileInfo)
{
  WIN32_FIND_DATAW wfd;
  if (!DirIterator_readWIN32FINDDATA(self, &wfd))
    return false;

  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);
  return fileInfo->fromWinFindData(d->pathAbs, &wfd) == ERR_OK;
}

static bool FOG_CDECL DirIterator_readFileName(DirIterator* self, StringW* fileName)
{
  WIN32_FIND_DATAW wfd;
  if (!DirIterator_readWIN32FINDDATA(self, &wfd))
    return false;

  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);
  return fileName->setWChar(wfd.cFileName);
}

// ============================================================================
// [Fog::DirIterator - Rewind / Tell (Windows)]
// ============================================================================

static err_t FOG_CDECL DirIterator_rewind(DirIterator* self)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return ERR_RT_INVALID_STATE;

  if (d->position == 0)
    return ERR_OK;

  WIN32_FIND_DATAW wfd;
  HANDLE handle = (void*)FindFirstFileW(reinterpret_cast<const wchar_t*>(d->pathOS->getData()), &wfd);

  if (handle == (void*)INVALID_HANDLE_VALUE)
    return OSUtil::getErrFromOSLastError();

  ::FindClose((HANDLE)d->handle);
  d->handle = handle;
  d->position = 0;

  return ERR_OK;
}

static int64_t FOG_CDECL DirIterator_tell(DirIterator* self)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return FOG_UINT64_C(-1);
  else
    return d->position;
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::DirIterator - Helpers (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)

// Glibc's readdir is reentrant. POSIX guarantees only readdir_r to be reentrant
// __linux__ is defined on Linux, __GNU__ on HURD and __GLIBC__ on Debian GNU/k*BSD
#if defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
# define FOG_HAVE_REENTRANT_READDIR
#endif // __linux__ || __GNU__ || __GLIBC__

struct FOG_NO_EXPORT PosixDirIteratorData : public DirIteratorData
{
  Static<StringA> pathCache;
  size_t pathCacheBaseLength;
  struct dirent* dent;
};

static PosixDirIteratorData* PosixDirIterator_dCreate(void)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(
    MemMgr::alloc(sizeof(PosixDirIteratorData))
  );

  if (FOG_IS_NULL(d))
    return NULL;

  d->handle = NULL;
  d->pathAbs.init();
  d->pathCache.init();
  d->dent = NULL;

  return d;
}

static void PosixDirIterator_dFree(PosixDirIteratorData* d)
{
  d->pathAbs.destroy();
  d->pathCache.destroy();

  if (d->dent)
    MemMgr::free(d->dent);

  MemMgr::free(d);
}

// ============================================================================
// [Fog::DirIterator - Open / Close (Posix)]
// ============================================================================

static err_t FOG_CDECL DirIterator_open(DirIterator* self, const StringW* path, bool skipParent)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d != &DirIterator_dEmpty)
  {
    ::closedir((DIR*)d->handle);

    d->pathAbs->clear();
    d->pathCache->clear();

    MemMgr::free(d->dent);
    d->dent = NULL;
  }
  else
  {
    d = PosixDirIterator_dCreate();
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  d->skipParent = skipParent;

  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(d->pathAbs, *path));
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(d->pathCache, d->pathAbs));

  errno = 0;
  if ((d->handle = (void*)::opendir(d->pathCache->getData())) != NULL)
  {
    // Get max size of file name in this directory + fallback.
    long direntSize = pathconf(d->pathCache->getData(), _PC_NAME_MAX);
    if (direntSize == -1) direntSize = _POSIX_NAME_MAX;

    // Add offset of d_name field + 1 to get correct dirent size.
    direntSize += FOG_OFFSET_OF(struct dirent, d_name) + 1;

    // Ugly typecast from long, but there is no better way.
    d->dent = reinterpret_cast<struct dirent*>(MemMgr::alloc((size_t)direntSize));
    if (d->dent == NULL)
    {
      ::closedir((DIR*)d->handle);
      PosixDirIterator_dFree(d);

      self->_d = &DirIterator_dEmpty;
      return ERR_RT_OUT_OF_MEMORY;
    }

    d->pathCacheBaseLength = d->pathCache->getLength();
    return ERR_OK;
  }
  else
  {
    return errno;
  }
}

static void FOG_CDECL DirIterator_close(DirIterator* self)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return;

  ::closedir((DIR*)d->handle);
  PosixDirIterator_dFree(d);

  self->_d = &DirIterator_dEmpty;
}

// ============================================================================
// [Fog::DirIterator - Read (Posix)]
// ============================================================================

static bool FOG_CDECL DirIterator_readFileInfo(DirIterator* self, FileInfo* fileInfo)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return false;

  if (fileInfo->detach() != ERR_OK)
    return false;

  struct dirent *dent;
  while (::readdir_r((DIR*)d->handle, d->dent, &dent) == 0 && dent != NULL)
  {
    const char* name = dent->d_name;

    // Skip "." and "..".
    if (name[0] == '.')
    {
      if (name[1] == '\0')
        continue;

      if (name[1] == '.' && name[2] == '\0' && d->skipParent)
        continue;
    }

    // Get entry name length.
    size_t nameLength = strlen(name);

    // Translate entry name to unicode. We use the StringW instance associated
    // with the fileInfo to prevent dynamic memory allocation per read() request.
    FOG_ASSERT(fileInfo->isDetached());
    StringW& fileName = fileInfo->_d->fileName;

    if (TextCodec::local8().decode(fileName, StubA(name, nameLength)) != ERR_OK)
    {
      // TODO:
      // What to do here?
    }

    d->pathCache->resize(d->pathCacheBaseLength);
    d->pathCache->append('/');
    d->pathCache->append(name, nameLength);

    struct stat s;
    if (::stat(d->pathCache->getData(), &s) == 0)
    {
      if (fileInfo->fromStat(d->pathAbs(), fileName, &s) != ERR_OK)
        return false;
    }
    else
    {
      // TODO:
      // Bad symbolic link?
    }

    return true;
  }

  return false;
}

static bool FOG_CDECL DirIterator_readFileName(DirIterator* self, StringW* fileName)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return false;

  struct dirent *dent;
  while (::readdir_r((DIR*)d->handle, d->dent, &dent) == 0 && (dent != NULL))
  {
    const char* name = dent->d_name;

    // Skip "." and "..".
    if (name[0] == '.')
    {
      if (name[1] == '\0')
        continue;

      if (name[1] == '.' && name[2] == '\0' && d->skipParent)
        continue;
    }

    // Translate entry name to unicode.
    return TextCodec::local8().decode(*fileName, StubA(name, DETECT_LENGTH)) == ERR_OK;
  }

  return false;
}

// ============================================================================
// [Fog::DirIterator - Rewind / Tell (Posix)]
// ============================================================================

static err_t FOG_CDECL DirIterator_rewind(DirIterator* self)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return ERR_RT_INVALID_STATE;

  ::rewinddir((DIR*)(d->handle));
  return ERR_OK;
}

static int64_t FOG_CDECL DirIterator_tell(DirIterator* self)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return FOG_INT64_C(-1);
  else
    return (int64_t)::telldir((DIR*)(d->handle));
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void DirIterator_init(void)
{
  DirIterator_dEmpty->handle = NULL;
  DirIterator_dEmpty->pathAbs.init();

  fog_api.diriterator_ctor = DirIterator_ctor;
  fog_api.diriterator_ctorString = DirIterator_ctorString;
  fog_api.diriterator_dtor = DirIterator_dtor;

  fog_api.diriterator_open = DirIterator_open;
  fog_api.diriterator_close = DirIterator_close;

  fog_api.diriterator_readFileInfo = DirIterator_readFileInfo;
  fog_api.diriterator_readFileName = DirIterator_readFileName;

  fog_api.diriterator_rewind = DirIterator_rewind;
  fog_api.diriterator_tell = DirIterator_tell;
}

} // Fog namespace
