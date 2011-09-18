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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/System.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

#if defined(FOG_OS_WINDOWS)
# include <io.h>
#else
# include <dirent.h>
# include <errno.h>
# include <sys/stat.h>
#if defined(FOG_HAVE_UNISTD_H)
# include <unistd.h>
#endif
#endif

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

static void FOG_CDECL DirIterator_ctorString(DirIterator* self, const StringW* path)
{
  self->_d = &DirIterator_dEmpty;
  _api.diriterator.open(self, path);
}

static void FOG_CDECL DirIterator_dtor(DirIterator* self)
{
  if (self->_d == &DirIterator_dEmpty)
    return;
  _api.diriterator.close(self);
}

// ============================================================================
// [Fog::DirIterator - Windows]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

struct FOG_NO_EXPORT WinDirIteratorData : public DirIteratorData
{
  Static<StringW> pathOS;
  int64_t position;
  WIN32_FIND_DATAW findData;
};

static WinDirIteratorData* WinDirIteratorData_create(void)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(
    MemMgr::alloc(sizeof(WinDirIteratorData))
  );

  if (FOG_IS_NULL(d))
    return NULL;

  d->handle = NULL;
  d->pathAbs.init();
  d->pathOS.init();
  d->position = 0;

  return d;
}

static void WinDirIteratorData_destroy(WinDirIteratorData* d)
{
  d->pathAbs.destroy();
  d->pathOS.destroy();
  MemMgr::free(d);
}

// ============================================================================
// [Fog::DirIterator - Windows - Open / Close]
// ============================================================================

static err_t FOG_CDECL DirIterator_open(DirIterator* self, const StringW* path)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);

  if (d != &DirIterator_dEmpty)
  {
    ::FindClose((HANDLE)d->handle);

    d->pathAbs->clear();
    d->pathOS->clear();
    d->position = 0;
  }
  else
  {
    d = WinDirIteratorData_create();
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  FOG_RETURN_ON_ERROR(FileSystem::toAbsolutePath(d->pathAbs, StringW(), *path));
  FOG_RETURN_ON_ERROR(System::makeWindowsPath(d->pathOS, d->pathAbs));
  FOG_RETURN_ON_ERROR(d->pathOS->append(Ascii8("\\*")));

  d->handle = (void*)FindFirstFileW(reinterpret_cast<const wchar_t*>(d->pathOS->getData()), &d->findData);

  // Try to open with long file name support if path is too long
  if (d->handle == (void*)INVALID_HANDLE_VALUE)
  {
    WinDirIteratorData_destroy(d);
    self->_d = &DirIterator_dEmpty;
    return System::errorFromOSLastError();
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
  WinDirIteratorData_destroy(d);

  self->_d = &DirIterator_dEmpty;
}

// ============================================================================
// [Fog::DirIterator - Windows - Read]
// ============================================================================

static bool DirIterator_readWIN32FINDDATA(DirIterator* self, WIN32_FIND_DATAW* wfd)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);
  HANDLE handle = (HANDLE)d->handle;

  if (!handle)
    return false;

  if (d->position == 0)
  {
    memcpy(wfd, &d->findData, sizeof(WIN32_FIND_DATAW));
  }
  else
  {
_Next:
    if (::FindNextFileW(handle, wfd) == 0)
      return false;
  }

  d->position++;

  // Skip "." and ".."
  if ((wfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && self->_skipDots)
  {
    const WCHAR* fn = wfd->cFileName;

    if (fn[0] == L'.' && ((fn[1] == 0) || (fn[1] == L'.' && fn[2] == 0)))
      goto _Next;
  }

  return true;
}

static bool FOG_CDECL DirIterator_readDirEntry(DirIterator* self, DirEntry* dirEntry)
{
  WIN32_FIND_DATAW& wfd = dirEntry->_winFindData;

  if (!DirIterator_readWIN32FINDDATA(self, &wfd))
    return false;

  if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
    dirEntry->_type = DIR_ENTRY_DIRECTORY;
  else
    dirEntry->_type = DIR_ENTRY_FILE;

  dirEntry->_size = ((uint64_t)(wfd.nFileSizeHigh) << 32) | ((uint64_t)(wfd.nFileSizeLow));
  dirEntry->_name->setWChar(wfd.cFileName);

  return true;
}

static bool FOG_CDECL DirIterator_readString(DirIterator* self, StringW* fileName)
{
  WIN32_FIND_DATAW wfd;

  if (!DirIterator_readWIN32FINDDATA(self, &wfd))
    return false;

  fileName->setWChar(wfd.cFileName);
  return true;
}

// ============================================================================
// [Fog::DirIterator - Windows - Rewind / Tell]
// ============================================================================

static err_t FOG_CDECL DirIterator_rewind(DirIterator* self)
{
  WinDirIteratorData* d = reinterpret_cast<WinDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return ERR_RT_INVALID_STATE;

  if (d->position == 0)
    return ERR_OK;

  HANDLE handle = (void*)FindFirstFileW(reinterpret_cast<const wchar_t*>(d->pathOS->getData()), &d->findData);
  if (handle == (void*)INVALID_HANDLE_VALUE)
    return System::errorFromOSLastError();

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
// [Fog::DirIterator - Posix]
// ============================================================================

#if defined(FOG_OS_POSIX)

// Glibc's readdir is reentrant. POSIX guarantees only readdir_r to be reentrant
// __linux__ is defined on Linux, __GNU__ on HURD and __GLIBC__ on Debian GNU/k*BSD
#if defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
# define FOG_HAVE_REENTRANT_READDIR
#endif

struct FOG_NO_EXPORT PosixDirIteratorData : public DirIteratorData
{
  Static<StringA> pathCache;
  size_t pathCacheBaseLength;
  struct dirent* dent;
};

static PosixDirIteratorData* PosixDirIteratorData_create(void)
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

static void PosixDirIteratorData_destroy(PosixDirIteratorData* d)
{
  d->pathAbs.destroy();
  d->pathCache.destroy();

  if (d->dent)
    MemMgr::free(d->dent);

  MemMgr::free(d);
}

// ============================================================================
// [Fog::DirIterator - Posix - Open / Close]
// ============================================================================

static err_t FOG_CDECL DirIterator_open(DirIterator* self, const StringW* path)
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
    d = PosixDirIteratorData_create();
    if (FOG_IS_NULL(d))
      return ERR_RT_OUT_OF_MEMORY;
  }

  FOG_RETURN_ON_ERROR(FileSystem::toAbsolutePath(d->pathAbs, StringW(), *path));
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
      PosixDirIteratorData_destroy(d);

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
  PosixDirIteratorData_destroy(d);

  self->_d = &DirIterator_dEmpty;
}

// ============================================================================
// [Fog::DirIterator - Posix - Read]
// ============================================================================

static bool FOG_CDECL DirIterator_readDirEntry(DirIterator* self, DirEntry* dirEntry)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return false;

  bool skipDots = self->_skipDots;
  struct dirent *dent;

  while (::readdir_r((DIR*)d->handle, d->dent, &dent) == 0 && (dent != NULL))
  {
    const char* name = dent->d_name;

    // Skip "." and ".."
    if (name[0] == '.' && skipDots)
    {
      if (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')) continue;
    }

    // Get entry name length.
    size_t nameLength = strlen(name);

    // Translate entry name to unicode.
    TextCodec::local8().decode(dirEntry->_name, StubA(name, nameLength));

    d->pathCache->resize(d->pathCacheBaseLength);
    d->pathCache->append('/');
    d->pathCache->append(name, nameLength);

    uint type = 0;

    if (::stat(d->pathCache->getData(), &dirEntry->_posixStatData) != 0)
    {
      // This is situation that's bad symbolic link (I was experienced
      // this) and there is no reason to write an error message...
    }
    else
    {
      // S_ISXXX are POSIX macros to get a file type.
      if (S_ISREG(dirEntry->_posixStatData.st_mode)) type |= DIR_ENTRY_FILE;
      if (S_ISDIR(dirEntry->_posixStatData.st_mode)) type |= DIR_ENTRY_DIRECTORY;
#if defined(S_ISCHR)
      if (S_ISCHR(dirEntry->_posixStatData.st_mode)) type |= DIR_ENTRY_CHAR_DEVICE;
#endif
#if defined(S_ISBLK)
      if (S_ISBLK(dirEntry->_posixStatData.st_mode)) type |= DIR_ENTRY_BLOCK_DEVICE;
#endif
#if defined(S_ISFIFO)
      if (S_ISFIFO(dirEntry->_posixStatData.st_mode)) type |= DIR_ENTRY_FIFO;
#endif
#if defined(S_ISLNK)
      if (S_ISLNK(dirEntry->_posixStatData.st_mode)) type |= DIR_ENTRY_LINK;
#endif
#if defined(S_ISSOCK)
      if (S_ISSOCK(dirEntry->_posixStatData.st_mode)) type |= DIR_ENTRY_SOCKET;
#endif
    }

    dirEntry->_type = type;
    if (type == DIR_ENTRY_FILE)
      dirEntry->_size = dirEntry->_posixStatData.st_size;
    else
      dirEntry->_size = 0;

    return true;
  }

  return false;
}

static bool FOG_CDECL DirIterator_readString(DirIterator* self, StringW* fileName)
{
  PosixDirIteratorData* d = reinterpret_cast<PosixDirIteratorData*>(self->_d);

  if (d == &DirIterator_dEmpty)
    return false;

  struct dirent *dent;
  bool skipDots = self->_skipDots;

  while (::readdir_r((DIR*)d->handle, d->dent, &dent) == 0 && (dent != NULL))
  {
    const char* name = dent->d_name;

    // Skip "." and ".."
    if (name[0] == '.' && skipDots)
    {
      if (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'))
        continue;
    }

    // Translate entry name to unicode.
    return TextCodec::local8().decode(*fileName, StubA(name, DETECT_LENGTH)) == ERR_OK;
  }

  return false;
}

// ============================================================================
// [Fog::DirIterator - Rewind / Tell]
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

  _api.diriterator.ctor = DirIterator_ctor;
  _api.diriterator.ctorString = DirIterator_ctorString;
  _api.diriterator.dtor = DirIterator_dtor;

  _api.diriterator.open = DirIterator_open;
  _api.diriterator.close = DirIterator_close;

  _api.diriterator.readDirEntry = DirIterator_readDirEntry;
  _api.diriterator.readString = DirIterator_readString;

  _api.diriterator.rewind = DirIterator_rewind;
  _api.diriterator.tell = DirIterator_tell;
}

} // Fog namespace
