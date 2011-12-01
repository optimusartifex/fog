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
#include <Fog/Core/OS/FileMapping.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/OS/WinUtil.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <errno.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

namespace Fog {

// ============================================================================
// [Fog::FileMapping - Global]
// ============================================================================

static Static<FileMappingData> FileMapping_dNull;

// ============================================================================
// [Fog::FileMapping - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FileMapping_ctor(FileMapping* self)
{
  self->_d = FileMapping_dNull->addRef();
}

static void FOG_CDECL FileMapping_ctorCopy(FileMapping* self, const FileMapping* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL FileMapping_dtor(FileMapping* self)
{
  FileMappingData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::FileMapping - Helpers (Shared)]
// ============================================================================

static FileMappingData* FOG_CDECL FileMapping_dCreate(const StringW& fileName)
{
  FileMappingData* d = reinterpret_cast<FileMappingData*>(
    MemMgr::alloc(sizeof(FileMappingData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);

  d->reserved = 0;
  d->flags = NO_FLAGS;

  d->fileName.initCustom1(fileName);
  d->data = NULL;
  d->length = 0;

  return d;
}

static void FOG_CDECL FileMapping_dClose(FileMappingData* d);

static void FOG_CDECL FileMapping_dRelease(FileMappingData* d)
{
  FileMapping_dClose(d);

  d->fileName.destroy();
  MemMgr::free(d);
}

// ============================================================================
// [Fog::FileMapping - Helpers (Windows)]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static void FOG_CDECL FileMapping_dClose(FileMappingData* d)
{
  if (d->hFileMapping)
  {
    UnmapViewOfFile(d->data);
    CloseHandle(d->hFileMapping);
    CloseHandle(d->hFile);
  }
  else
  {
    MemMgr::free(d->data);
  }
}

// ============================================================================
// [Fog::FileMapping - Open / Close (Windows)]
// ============================================================================

static err_t FOG_CDECL FileMapping_open(FileMapping* self, const StringW* fileName, uint32_t flags)
{
  FileMappingData* d = self->_d;
  err_t err = ERR_OK;

  StringW fileNameAbs;
  StringW fileNameW;

  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(fileNameAbs, *fileName));
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(fileNameW, fileNameAbs));

  if (d->data && d->reference.get() == 1)
  {
    FileMapping_dClose(d);
    d->fileName->set(fileNameAbs);
  }
  else
  {
    d->release();
    d = FileMapping_dCreate(fileNameAbs);
    self->_d = d;
  }

  HANDLE hFile = NULL;
  HANDLE hFileMapping = NULL;

  void* data = NULL;
  size_t size = 0;

  DWORD szLow = 0;
  DWORD szHigh = 0;

  // --------------------------------------------------------------------------
  // [Open HFILE]
  // --------------------------------------------------------------------------

  hFile = ::CreateFileW(
    reinterpret_cast<const wchar_t*>(fileNameW.getData()),
    FILE_READ_DATA,
    FILE_SHARE_READ,
    NULL, OPEN_EXISTING, 0, NULL);

  if (hFile == INVALID_HANDLE_VALUE)
  {
    err = OSUtil::getErrFromOSLastError();
    goto _Fail;
  }

  // Get size of file (if size is too large -> fail)
  szLow = ::GetFileSize(hFile, &szHigh);

#if FOG_ARCH_BITS == 32
  if (szHigh)
  {
    err = ERR_FILE_TOO_LARGE;
    goto _Fail;
  }
  size = szLow;
#else
  size = (size_t)( ((uint64_t)szHigh << FOG_UINT64_C(32)) | (uint64_t)szLow );
#endif

  if (size == 0)
  {
    err = ERR_FILE_IS_EMPTY;
    goto _Fail;
  }

  // --------------------------------------------------------------------------
  // [Create HFILEMAPPING]
  // --------------------------------------------------------------------------

  hFileMapping = ::CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  if (hFileMapping == INVALID_HANDLE_VALUE)
  {
    err = OSUtil::getErrFromOSLastError();
  }
  else
  {
    // Map it (if this will cause error, we try to load file manually).
    data = ::MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (data != NULL)
    {
      d->flags = NO_FLAGS;
      d->data = data;
      d->length = size;
      d->hFile = hFile;
      d->hFileMapping = hFileMapping;

      return ERR_OK;
    }

    err = OSUtil::getErrFromOSLastError();

    ::CloseHandle(hFileMapping);
    hFileMapping = NULL;
  }

  if ((flags & FILE_MAPPING_FLAG_LOAD_FALLBACK) == 0)
    goto _Fail;

  // --------------------------------------------------------------------------
  // [Fallback]
  // --------------------------------------------------------------------------

  {
    static const size_t CHUNK_SIZE = 1024*1024*16;

    data = MemMgr::alloc(size);
    if (FOG_IS_NULL(data))
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto _Fail;
    }

    uint8_t* p = (uint8_t*)data;
    size_t i;

    DWORD bytesToRead;
    DWORD bytesRead;

    bytesToRead = CHUNK_SIZE;
    for (i = size / CHUNK_SIZE + 1; i; i--, p += bytesToRead)
    {
      if (i == 1)
      {
        bytesToRead = size % CHUNK_SIZE;
        if (bytesToRead == 0) break;
      }

      if (!::ReadFile(hFile, (LPVOID)p, bytesToRead, &bytesRead, NULL) || bytesRead != bytesToRead)
      {
        err = OSUtil::getErrFromOSLastError();
        MemMgr::free(data);
        goto _Fail;
      }
    }

    CloseHandle(hFile);

    d->flags = FILE_MAPPING_FLAG_LOAD_FALLBACK;
    d->data = data;
    d->length = size;
    d->hFile = NULL;
    d->hFileMapping = NULL;

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Fail]
  // --------------------------------------------------------------------------

_Fail:
  if (hFile)
    ::CloseHandle(hFile);

  d->fileName.destroy();
  MemMgr::free(d);

  self->_d = FileMapping_dNull->addRef();
  return err;
}

static void FOG_CDECL FileMapping_close(FileMapping* self)
{
  FileMappingData* d = self->_d;

  if (d == &FileMapping_dNull)
    return;

  self->_d = FileMapping_dNull->addRef();
  d->release();
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::FileMapping - Helpers (Posix)]
// ============================================================================

#if defined(FOG_OS_POSIX)
static void FOG_CDECL FileMapping_dClose(FileMappingData* d)
{
  if (d->fd != -1)
  {
    ::munmap((char *)d->data, d->length);
    ::close(d->fd);
  }
  else
  {
    MemMgr::free(d->data);
  }
}

// ============================================================================
// [Fog::FileMapping - Open / Close (Posix)]
// ============================================================================

static err_t FOG_CDECL FileMapping_open(FileMapping* self, const StringW* fileName, uint32_t flags)
{
  FileMappingData* d = self->_d;
  err_t err = ERR_OK;

  StringW fileNameAbs;
  StringTmpA<TEMPORARY_LENGTH> fileName8;

  FOG_RETURN_ON_ERROR(FilePath::toAbsolute(fileNameAbs, *fileName));
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(fileName8, fileNameAbs));

  if (d->data && d->reference.get() == 1)
  {
    FileMapping_dClose(d);
    d->fileName->set(fileNameAbs);
  }
  else
  {
    d->release();
    d = FileMapping_dCreate(fileNameAbs);
    self->_d = d;
  }

  int fd;
  struct stat s;

  void* data = NULL;
  size_t size = 0;

  // --------------------------------------------------------------------------
  // [Open / Stat]
  // --------------------------------------------------------------------------

  fd = ::open(fileName8.getData(), O_RDONLY);
  if (fd < 0)
  {
    err = OSUtil::getErrFromOSLastError();
    goto _Fail;
  }

  if (::fstat(fd, &s) != 0)
  {
    err = OSUtil::getErrFromOSLastError();
    goto _Fail;
  }

#if FOG_ARCH_BITS == 32
  if ((uint64_t)s.st_size >= (uint64_t)SIZE_MAX)
  {
    err = ERR_FILE_TOO_LARGE;
    goto _Fail;
  }
#endif // FOG_ARCH_BITS == 32

  size = (size_t)s.st_size;
  if (size == 0)
  {
    err = ERR_FILE_IS_EMPTY;
    goto _Fail;
  }

  // --------------------------------------------------------------------------
  // [MMap]
  // --------------------------------------------------------------------------

  {
    uint mmapFlags = PROT_READ;
    data = ::mmap(NULL, size, mmapFlags, MAP_SHARED, fd, 0);

    // Success
    if (data != (void *)-1)
    {
      d->flags = NO_FLAGS;
      d->data = data;
      d->length = size;
      d->fd = fd;

      return ERR_OK;
    }
  }

  // --------------------------------------------------------------------------
  // [Fallback]
  // --------------------------------------------------------------------------

  {
    const size_t CHUNK_SIZE = 1024*1024*16;

    data = MemMgr::alloc(size);
    if (FOG_IS_NULL(data))
    {
      err = ERR_RT_OUT_OF_MEMORY;
      goto _Fail;
    }

    uint8_t* p = (uint8_t*)data;
    size_t i;

    size_t bytesToRead;
    size_t bytesRead;

    bytesToRead = CHUNK_SIZE;
    for (i = size / CHUNK_SIZE + 1; i; i--, p += bytesToRead)
    {
      if (i == 1)
      {
        bytesToRead = size % CHUNK_SIZE;
        if (bytesToRead == 0) break;
      }

      bytesRead = ::read(fd, p, bytesToRead);
      if (bytesRead != bytesToRead)
      {
        err = OSUtil::getErrFromOSLastError();
        MemMgr::free(data);
        goto _Fail;
      }
    }

    ::close(fd);

    d->flags = FILE_MAPPING_FLAG_LOAD_FALLBACK;
    d->data = data;
    d->length = (size_t)s.st_size;
    d->fd = -1;

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Fallback]
  // --------------------------------------------------------------------------

_Fail:
  if (fd)
    ::close(fd);

  d->fileName.destroy();
  MemMgr::free(d);

  self->_d = FileMapping_dNull->addRef();
  return err;

}

static void FOG_CDECL FileMapping_close(FileMapping* self)
{
  FileMappingData* d = self->_d;

  if (d == &FileMapping_dNull)
    return;

  self->_d = FileMapping_dNull->addRef();
  d->release();
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Fog::FileMapping - Copy]
// ============================================================================

static void FOG_CDECL FileMapping_copy(FileMapping* self, const FileMapping* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void FileMapping_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.filemapping_ctor = FileMapping_ctor;
  fog_api.filemapping_ctorCopy = FileMapping_ctorCopy;
  fog_api.filemapping_dtor = FileMapping_dtor;
  fog_api.filemapping_open = FileMapping_open;
  fog_api.filemapping_close = FileMapping_close;
  fog_api.filemapping_copy = FileMapping_copy;
  fog_api.filemapping_dRelease = FileMapping_dRelease;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FileMappingData* d = &FileMapping_dNull;
  d->reference.init(1);
  d->fileName.initCustom1(fog_api.stringw_oEmpty->_d);
}

} // Fog namespace
