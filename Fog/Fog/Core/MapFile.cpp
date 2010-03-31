// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/MapFile.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/String.h>
#include <Fog/Core/TextCodec.h>

#if defined(FOG_OS_POSIX)
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace Fog {

// ============================================================================
// [Fog::MapFile]
// ============================================================================

MapFile::MapFile() :
  _data(NULL),
  _length(0),
#if defined(FOG_OS_WINDOWS)
  _hFileMapping(NULL),
  _hFile(NULL),
#endif // FOG_OS_WINDOWS
#if defined(FOG_OS_POSIX)
  _fd(-1),
#endif // FOG_OS_POSIX
  _state(STATE_NONE)
{
}

MapFile::~MapFile()
{
  unmap();
}

#if defined(FOG_OS_WINDOWS)
err_t MapFile::map(const String& fileName, bool loadOnFail)
{
  unmap();

  err_t err;
  String fileNameW(fileName);
  if ((err = fileNameW.slashesToPosix())) return err;

  HANDLE hFile;
  HANDLE hFileMapping;
  void* data;
  sysuint_t size;

  DWORD szLow = 0;
  DWORD szHigh = 0;

  // Try to open file.
  if ((hFile = CreateFileW(
    reinterpret_cast<const wchar_t*>(fileNameW.getData()), FILE_READ_DATA, FILE_SHARE_READ,
    NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
  {
    return GetLastError();
  }

  // Get size of file (if size is too large -> fail)
  szLow = GetFileSize(hFile, &szHigh);
#if FOG_ARCH_BITS == 32
  if (szHigh) 
  {
    CloseHandle(hFile);
    return ERR_IO_FILE_TOO_BIG;
  }
  size = szLow;
#else
  size = (sysuint_t)( ((uint64_t)szHigh << FOG_UINT64_C(32)) | (uint64_t)szLow );
#endif

  if (size == 0)
  {
    CloseHandle(hFile);
    return ERR_IO_FILE_IS_EMPTY;
  }

  // Create FileMapping object.
  hFileMapping = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  if (hFileMapping != INVALID_HANDLE_VALUE)
  {
    // Map it (if this will cause error, we try to load file manually).
    if ((data = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0)) != NULL)
    {
      _fileName = fileName;
      _data = data;
      _length = size;
      _hFileMapping = hFileMapping;
      _hFile = hFile;
      _state = STATE_MAPPED;
      return ERR_OK;
    }

    err = GetLastError();
    CloseHandle(hFileMapping);
  }
  else
  {
    err = GetLastError();
  }

  if (!loadOnFail)
  {
    CloseHandle(hFile);
    return err;
  }

  data = Memory::alloc(size);
  if (!data)
  {
    CloseHandle(hFile);
    return ERR_RT_OUT_OF_MEMORY;
  }

  static const sysuint_t chunkSize = 1024*1024*16;

  uint8_t* dataCur = (uint8_t*)data;
  sysuint_t i;
  DWORD bytesToRead;
  DWORD bytesRead;

  bytesToRead = chunkSize;
  for (i = size / chunkSize + 1; i; i--, dataCur += bytesToRead)
  {
    if (i == 1)
    {
      bytesToRead = (size % chunkSize);
      if (bytesToRead == 0) break;
    }

    if (!ReadFile(hFile, (LPVOID)dataCur, bytesToRead, &bytesRead, NULL) || bytesRead != bytesToRead)
    {
      err = GetLastError();
      Memory::free(data);
      CloseHandle(hFile);
      return err;
    }
  }

  CloseHandle(hFile);

  _fileName = fileName;
  _data = data;
  _length = size;
  _hFileMapping = NULL;
  _hFile = NULL;
  _state = STATE_LOADED;
  return ERR_OK;
}

void MapFile::unmap()
{
  if (!_hFileMapping && !_data) return;

  if (_hFileMapping)
  {
    UnmapViewOfFile(_data);
    CloseHandle(_hFileMapping);
    CloseHandle(_hFile);
  }
  else
  {
    Memory::free(_data);
  }

  _fileName.clear();
  _data = NULL;
  _length = 0UL;
  _hFileMapping = NULL;
  _hFile = NULL;
  _state = STATE_NONE;
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
err_t MapFile::map(const String& fileName, bool loadOnFail)
{
  unmap();

  err_t err;

  TemporaryByteArray<TEMP_LENGTH> fileName8;
  if ((err = TextCodec::local8().appendFromUnicode(fileName8, fileName))) return err;

  int fd = open(fileName8.getData(), O_RDONLY);
  if (fd < 0) return errno;

  struct stat s;
  if (fstat(fd, &s) == 0
#if FOG_ARCH_BITS == 32
    && s.st_size < (int64_t)ULONG_MAX
#endif // FOG_ARCH_BITS == 32
    )
  {
    uint mmap_flags = PROT_READ;
    void *mmap_data = mmap(NULL, s.st_size, mmap_flags, MAP_SHARED, fd, 0);

    // Success
    if (mmap_data != (void *)-1)
    {
      _fileName = fileName;
      _data = mmap_data;
      _length = (sysuint_t)s.st_size;
      _fd = fd;
      _state = STATE_MAPPED;
      return ERR_OK;
    }

    void* data = Memory::alloc((sysuint_t)s.st_size);
    if (!data)
    {
      close(fd);
      return ERR_RT_OUT_OF_MEMORY;
    }

    const sysuint_t chunkSize = 1024*1024*16;

    uint8_t* dataCur = (uint8_t*)data;
    sysuint_t i;
    sysuint_t bytesToRead;
    sysuint_t bytesRead;

    bytesToRead = chunkSize;
    for (i = (sysuint_t)s.st_size / chunkSize + 1; i; i--, dataCur += bytesToRead)
    {
      if (i == 1)
      {
        bytesToRead = ((sysuint_t)s.st_size % chunkSize);
        if (bytesToRead == 0) break;
      }

      bytesRead = read(fd, dataCur, bytesToRead);
      if (bytesRead != bytesToRead)
      {
        err = errno;
        Memory::free(data);
        close(fd);
        return err;
      }
    }

    close(fd);

    _fileName = fileName;
    _data = data;
    _length = (sysuint_t)s.st_size;
    _fd = -1;
    _state = STATE_LOADED;
    return ERR_OK;
  }
  else
  {
    err = errno;
    close(fd);
    return err;
  }
}

void MapFile::unmap()
{
  if (_fd == -1 && !_data) return;

  if (_fd != -1)
  {
    munmap((char *)_data, _length);
    close(_fd);
  }
  else
  {
    Memory::free(_data);
  }

  _fileName.clear();
  _data = NULL;
  _length = 0UL;
  _fd = -1;
  _state = STATE_NONE;
}
#endif // FOG_OS_POSIX

} // Fog namespace
