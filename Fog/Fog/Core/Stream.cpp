// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/MapFile.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/TextCodec.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(FOG_OS_POSIX)
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#endif // FOG_OS_POSIX

#if !defined(FOG_OS_WINDOWS)
# define _fseeki64 fseeko
# define _ftelli64 ftello
#elif defined(__MINGW32__)
# warning "MinGW or Borland portability issue: large files support for FILE* stream is turned off"
# define _fseeki64 fseek
# define _ftelli64 ftell
#elif defined(FOG_CC_BORLAND)
# define _fseeki64 fseek
# define _ftelli64 ftell
#endif

// Under BSD anc MAC the functions with "64" suffix are not defined. Standard
// functions are using 64-bit offset by default.
#if defined(FOG_OS_BSD) || defined(FOG_OS_MAC)
#define open64 open
#define lseek64 lseek
#define ftruncate64 ftruncate
#define O_LARGEFILE 0
#endif

namespace Fog {

// ============================================================================
// [Fog::StreamDevice]
// ============================================================================

StreamDevice::StreamDevice()
{
  refCount.init(1);
  flags = 0;
}

StreamDevice::~StreamDevice()
{
}

StreamDevice* StreamDevice::ref() const
{
  refCount.inc();
  return const_cast<StreamDevice*>(this);
}

void StreamDevice::deref()
{
  if (refCount.deref()) delete this;
}

ByteArray StreamDevice::getBuffer() const
{
  return ByteArray();
}

// ============================================================================
// [Fog::NullStreamDevice]
// ============================================================================

struct FOG_HIDDEN NullStreamDevice : public StreamDevice
{
  NullStreamDevice();

  virtual int64_t seek(int64_t offset, int whence);
  virtual int64_t tell() const;

  virtual sysuint_t read(void* buffer, sysuint_t size);
  virtual sysuint_t write(const void* buffer, sysuint_t size);

  virtual err_t getSize(int64_t* size);
  virtual err_t setSize(int64_t size);

  virtual err_t truncate(int64_t offset);

  virtual void close();
};

NullStreamDevice::NullStreamDevice()
{
  flags = STREAM_IS_WRITABLE;
}

int64_t NullStreamDevice::seek(int64_t offset, int whence)
{
  return -1;
}

int64_t NullStreamDevice::tell() const
{
  return -1;
}

sysuint_t NullStreamDevice::read(void* buffer, sysuint_t size)
{
  return 0;
}

sysuint_t NullStreamDevice::write(const void* buffer, sysuint_t size)
{
  return 0;
}

err_t NullStreamDevice::getSize(int64_t* size)
{
  *size = 0;
  return ERR_OK;
}

err_t NullStreamDevice::setSize(int64_t size)
{
  if (size == 0)
    return ERR_OK;
  else
    return ERR_IO_CANT_RESIZE;
}

err_t NullStreamDevice::truncate(int64_t offset)
{
  return ERR_IO_CANT_TRUNCATE;
}

void NullStreamDevice::close()
{
}

static Static<NullStreamDevice> streamdevice_null;

// ============================================================================
// [Fog::HANDLEStreamDevice]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
struct FOG_HIDDEN HANDLEStreamDevice : public StreamDevice
{
  HANDLEStreamDevice(HANDLE hFile, uint32_t fflags);
  ~HANDLEStreamDevice();

  static err_t openFile(const String& fileName, uint32_t openFlags, StreamDevice** dst);

  virtual int64_t seek(int64_t offset, int whence);
  virtual int64_t tell() const;

  virtual sysuint_t read(void* buffer, sysuint_t size);
  virtual sysuint_t write(const void* buffer, sysuint_t size);

  virtual err_t getSize(int64_t* size);
  virtual err_t setSize(int64_t size);
  virtual err_t truncate(int64_t offset);

  virtual void close();

  HANDLE hFile;
};

HANDLEStreamDevice::HANDLEStreamDevice(HANDLE hFile, uint32_t fflags) :
  hFile(hFile)
{
  flags |= fflags | STREAM_IS_HFILE | STREAM_IS_OPEN;
}

HANDLEStreamDevice::~HANDLEStreamDevice()
{
}

err_t HANDLEStreamDevice::openFile(const String& _fileName, uint32_t openFlags, StreamDevice** dst)
{
  FOG_ASSERT((openFlags & (STREAM_OPEN_READ | STREAM_OPEN_WRITE)) != 0);

  HANDLE hFile;

  // Convert path to local file system string
  err_t err;
  String fileName = _fileName;

  if ((err = fileName.slashesToWin())) return err;

  DWORD dwDesiredAccess = 0;
  DWORD dwCreationDisposition = 0;
  DWORD dwFlags = 0;

  uint32_t createPathFlags = 
    STREAM_OPEN_CREATE     |
    STREAM_OPEN_CREATE_PATH|
    STREAM_OPEN_WRITE      ;

  uint32_t fflags = STREAM_IS_OPEN | STREAM_IS_SEEKABLE | STREAM_IS_CLOSABLE;
  if (openFlags & STREAM_OPEN_READ ) fflags |= STREAM_IS_READABLE;
  if (openFlags & STREAM_OPEN_WRITE) fflags |= STREAM_IS_WRITABLE;

  if (openFlags & STREAM_OPEN_READ ) dwDesiredAccess |= GENERIC_READ;
  if (openFlags & STREAM_OPEN_WRITE) dwDesiredAccess |= GENERIC_WRITE;

  if ((openFlags & (STREAM_OPEN_WRITE | STREAM_OPEN_TRUNCATE)) ==
                   (STREAM_OPEN_WRITE | STREAM_OPEN_TRUNCATE))
  {
    dwCreationDisposition = TRUNCATE_EXISTING;
  }
  else if (openFlags & STREAM_OPEN_CREATE)
  {
    if (openFlags & STREAM_OPEN_CREATE_ONLY)
      // Only create file, fail if file exist
      dwCreationDisposition = CREATE_NEW;
    else
      // Create or open
      dwCreationDisposition = OPEN_ALWAYS;
  }
  else
  {
    dwCreationDisposition = OPEN_EXISTING;
  }

  hFile = CreateFileW(
    reinterpret_cast<const wchar_t*>(fileName.getData()),
    dwDesiredAccess,           // open mode (read / write)
    FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode
    NULL,                      // security
    dwCreationDisposition,     // creation disposition
    dwFlags,                   // attributes (defaults)
    NULL);                     // no template

  if (hFile == INVALID_HANDLE_VALUE)
  {
    DWORD dwError = GetLastError();

    if ((openFlags & STREAM_OPEN_CREATE) &&
      dwCreationDisposition == TRUNCATE_EXISTING &&
      dwError == ERROR_FILE_NOT_FOUND)
    {
      hFile = CreateFileW(
        reinterpret_cast<const wchar_t*>(fileName.getData()),
        dwDesiredAccess,           // open mode (read / write)
        FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode
        NULL,                      // security
        CREATE_NEW,                // creation disposition
        dwFlags,                   // attributes (defaults)
        NULL);                     // no template

      if (hFile == INVALID_HANDLE_VALUE)
      {
        return GetLastError();
      }
      else
      {
        *dst = new(std::nothrow) HANDLEStreamDevice(hFile, fflags);
        return ERR_OK;
      }
    }
    else
    {
      return (err_t)dwError;
    }
  }
  else
  {
    // Sucess, but there is 'STREAM_OpenAppend' possibility.
    if ((openFlags & (STREAM_OPEN_WRITE | STREAM_OPEN_APPEND)) ==
                     (STREAM_OPEN_WRITE | STREAM_OPEN_APPEND))
    {
      SetFilePointer(hFile, 0, 0, FILE_END);
    }

    *dst = new(std::nothrow) HANDLEStreamDevice(hFile, fflags);
    return ERR_OK;
  }
}

int64_t HANDLEStreamDevice::seek(int64_t offset, int whence)
{
  UInt64Union i;
  DWORD dwError;

  i.u64 = offset;
  i.u32low = SetFilePointer(hFile, i.u32low, (PLONG)&i.u32high, whence);

  if (i.u32low == INVALID_SET_FILE_POINTER && (dwError = GetLastError()) != ERROR_SUCCESS)
  {
    return -1;
  }
  else
  {
    return (int64_t)i.u64;
  }
}

int64_t HANDLEStreamDevice::tell() const
{
  UInt64Union i;
  DWORD dwError;

  i.u32high = 0;
  i.u32low = SetFilePointer(hFile, 0, (PLONG)&i.u32high, FILE_CURRENT);

  if (i.u32low == INVALID_SET_FILE_POINTER && (dwError = GetLastError()) != ERROR_SUCCESS)
  {
    return -1;
  }
  else
  {
    return (int64_t)i.u64;
  }
}

sysuint_t HANDLEStreamDevice::read(void* buffer, sysuint_t size)
{
  DWORD bytesRead = 0;

#if FOG_ARCH_BITS == 64
  while (size > 0)
  {
    DWORD size32 = (DWORD)Math::min<sysuint_t>(size, UINT32_MAX);
    DWORD rd = 0;

    BOOL result = ReadFile(hFile, buffer, size32, &rd, NULL);

    bytesRead += rd;
    if (rd != size32) break;

    buffer = (char*)buffer + rd;
    size -= size32;
  }
#else
  ReadFile(hFile, buffer, (DWORD)size, &bytesRead, NULL);
#endif
  return bytesRead;
}

sysuint_t HANDLEStreamDevice::write(const void* buffer, sysuint_t size)
{
  DWORD bytesWritten = 0;

#if FOG_ARCH_BITS == 64
  while (size > 0)
  {
    DWORD size32 = (DWORD)Math::min<sysuint_t>(size, UINT32_MAX);
    DWORD wr = 0;

    BOOL result = WriteFile(hFile, buffer, size32, &wr, NULL);

    bytesWritten += wr;
    if (wr != size32) break;

    buffer = (char*)buffer + wr;
    size -= size32;
  }
#else
  WriteFile(hFile, buffer, (DWORD)size, &bytesWritten, NULL);
#endif
  return bytesWritten;
}

err_t HANDLEStreamDevice::getSize(int64_t* size)
{
  FOG_ASSERT(size != NULL);

  if (GetFileSizeEx(hFile, reinterpret_cast<LARGE_INTEGER*>(size)) == 0)
    return GetLastError();
  else
    return ERR_OK;
}

err_t HANDLEStreamDevice::setSize(int64_t size)
{
  // TODO
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t HANDLEStreamDevice::truncate(int64_t offset)
{
  if (seek(offset, STREAM_SEEK_SET) != -1)
  {
    if (!SetEndOfFile(hFile)) return ERR_IO_CANT_TRUNCATE;
    return ERR_OK;
  }
  else
  {
    return ERR_IO_CANT_TRUNCATE;
  }
}

void HANDLEStreamDevice::close()
{
  if ((flags & STREAM_IS_CLOSABLE) != 0)
  {
    CloseHandle(hFile);
  }
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::FdStreamDevice]
// ============================================================================

#if defined FOG_OS_POSIX
struct FOG_HIDDEN FdStreamDevice : public StreamDevice
{
  FdStreamDevice(int fd, uint32_t fflags);
  ~FdStreamDevice();

  static err_t openFile(const String& fileName, uint32_t openFlags, StreamDevice** dst);

  virtual int64_t seek(int64_t offset, int whence);
  virtual int64_t tell() const;

  virtual sysuint_t read(void* buffer, sysuint_t size);
  virtual sysuint_t write(const void* buffer, sysuint_t size);

  virtual err_t getSize(int64_t* size);
  virtual err_t setSize(int64_t size);
  virtual err_t truncate(int64_t offset);

  virtual void close();

  int fd;
};

FdStreamDevice::FdStreamDevice(int fd, uint32_t fflags) :
  fd(fd)
{
  flags |= fflags | STREAM_IS_FD | STREAM_IS_OPEN;
}

FdStreamDevice::~FdStreamDevice()
{
  close();
}

err_t FdStreamDevice::openFile(const String& fileName, uint32_t openFlags, StreamDevice** dst)
{
  FOG_ASSERT((openFlags & (STREAM_OPEN_READ | STREAM_OPEN_WRITE)) != 0);

  int fd;
  int fdFlags = 0; // Be quite.

  uint32_t fflags = STREAM_IS_OPEN | STREAM_IS_SEEKABLE | STREAM_IS_CLOSABLE;
  if (openFlags & STREAM_OPEN_READ ) fflags |= STREAM_IS_READABLE;
  if (openFlags & STREAM_OPEN_WRITE) fflags |= STREAM_IS_WRITABLE;

  // Convert path to local file system string.
  err_t err;
  TemporaryByteArray<TEMPORARY_LENGTH> fileName8;

  if ((err = TextCodec::local8().appendFromUnicode(fileName8, fileName))) return err;

  // Read / Write.
  if ((openFlags & STREAM_OPEN_RW) == STREAM_OPEN_RW)
    fdFlags = O_RDWR;
  else if ((openFlags & STREAM_OPEN_READ) != 0)
    fdFlags = O_RDONLY;
  else if ((openFlags & STREAM_OPEN_WRITE) != 0)
    fdFlags = O_WRONLY;

  // Truncate / Append.
  if (openFlags & STREAM_OPEN_WRITE)
  {
    if (openFlags & STREAM_OPEN_TRUNCATE) fdFlags |= O_TRUNC;
    if (openFlags & STREAM_OPEN_APPEND  ) fdFlags |= O_APPEND;
  }

  // Only create file, fail if file exist.
  if (openFlags & (STREAM_OPEN_CREATE | STREAM_OPEN_CREATE_ONLY) ==
                  (STREAM_OPEN_CREATE | STREAM_OPEN_CREATE_ONLY))
  {
    fdFlags |= O_EXCL;
  }

  // Open file.
  fd = ::open64(fileName8.getData(), fdFlags | O_LARGEFILE);

  // Try to create file if open failed (or create it if OpenCreate flag was set).
  if (fd < 0 && (errno == ENOENT) && (openFlags & STREAM_OPEN_CREATE) != 0)
  {
    fd = ::open64(fileName8.getData(), fdFlags | O_CREAT | O_LARGEFILE, 0644);
  }

  if (fd < 0)
  {
    // Error (Invalid file descriptor).
    return errno;
  }
  else
  {
    // Success.
    *dst = new(std::nothrow) FdStreamDevice(fd, fflags);
    return ERR_OK;
  }
}

int64_t FdStreamDevice::seek(int64_t offset, int whence)
{
  int64_t result = ::lseek64(fd, offset, whence);

  if (result < 0)
    return -1;
  else
    return result;
}

int64_t FdStreamDevice::tell() const
{
  int64_t result = ::lseek64(fd, 0, SEEK_SET);

  if (result < FOG_INT64_C(0))
    return -1;
  else
    return result;
}

sysuint_t FdStreamDevice::read(void* buffer, sysuint_t size)
{
  sysint_t n = ::read(fd, buffer, size);

  if (n < 0)
    return (sysuint_t)-1;
  else
    return (sysuint_t)n;
}

sysuint_t FdStreamDevice::write(const void* buffer, sysuint_t size)
{
  sysint_t n = ::write(fd, buffer, size);

  if (n < 0)
    return (sysuint_t)-1;
  else
    return (sysuint_t)n;
}

err_t FdStreamDevice::getSize(int64_t* size)
{
  FOG_ASSERT(size != NULL);

#if defined(FOG_OS_WINDOWS)
  _stati64 s;
  if (_fstati64(fd, &s) == 0)
#else
  struct stat s;
  if (fstat(fd, &s) == 0)
#endif
  {
    *size = (int64_t)s.st_size;
    return ERR_OK;
  }
  else
  {
    *size = 0;
    return errno;
  }
}

err_t FdStreamDevice::setSize(int64_t size)
{
  // TODO
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FdStreamDevice::truncate(int64_t offset)
{
  int result = ::ftruncate64(fd, offset);
  return result != 0 ? (err_t)ERR_IO_CANT_TRUNCATE : (err_t)ERR_OK;
}

void FdStreamDevice::close()
{
  if ((flags & STREAM_IS_CLOSABLE) != 0) ::close(fd);
}
#endif // FOR_OS_POSIX

// ============================================================================
// [Fog::MemoryStreamDevice]
// ============================================================================

struct FOG_HIDDEN MemoryStreamDevice : public StreamDevice
{
  MemoryStreamDevice();
  MemoryStreamDevice(const void* memory, sysuint_t size, uint32_t fflags);

  virtual int64_t seek(int64_t offset, int whence);
  virtual int64_t tell() const;

  virtual sysuint_t read(void* buffer, sysuint_t size);
  virtual sysuint_t write(const void* buffer, sysuint_t size);

  virtual err_t getSize(int64_t* size);
  virtual err_t setSize(int64_t size);
  virtual err_t truncate(int64_t offset);

  virtual void close();

  virtual ByteArray getBuffer() const;

  uint8_t* data;
  sysuint_t size;

  uint8_t* cur;
  uint8_t* end;
};

MemoryStreamDevice::MemoryStreamDevice() :
  data(NULL),
  size(0),
  cur(NULL),
  end(NULL)
{
}

MemoryStreamDevice::MemoryStreamDevice(
  const void* memory, sysuint_t size, uint32_t fflags) :
    data((uint8_t*)memory), size(size)
{
  cur = data;
  end = data + size;

  flags |= 
    STREAM_IS_OPEN     |
    STREAM_IS_SEEKABLE |
    STREAM_IS_MEMORY   |
    fflags;
}

int64_t MemoryStreamDevice::seek(int64_t offset, int whence)
{
  int64_t i;

  switch (whence)
  {
    case STREAM_SEEK_SET:
      i = offset;
      break;
    case STREAM_SEEK_CUR:
      i = (int64_t)(cur - data) + offset;
      break;
    case STREAM_SEEK_END:
      i = (int64_t)(end - data) + offset;
      break;
  }

  if (i < FOG_UINT64_C(0))
  {
    cur = data;
    return -1;
  }
  else if (i > (int64_t)size)
  {
    cur = end;
    return -1;
  }
  else
  {
    cur = data + (sysuint_t)i;
    return i;
  }
}

int64_t MemoryStreamDevice::tell() const
{
  return (int64_t)(cur - data);
}

sysuint_t MemoryStreamDevice::read(void* buffer, sysuint_t size)
{
  sysuint_t remain = (sysuint_t)(end - cur);
  if (size > remain) size = remain;

  Memory::copy(buffer, cur, size);
  cur += size;

  return size;
}

sysuint_t MemoryStreamDevice::write(const void* buffer, sysuint_t size)
{
  if ((flags & STREAM_IS_WRITABLE) == 0) return 0;

  sysuint_t remain = (sysuint_t)(end - cur);
  if (size > remain) size = remain;

  Memory::copy(cur, buffer, size);
  cur += size;

  return size;
}

err_t MemoryStreamDevice::getSize(int64_t* size)
{
  FOG_ASSERT(size != NULL);
  *size = (int64_t)this->size;
  return ERR_OK;
}

err_t MemoryStreamDevice::setSize(int64_t size)
{
  if (size == (int64_t)this->size) return ERR_OK;
  return ERR_IO_CANT_RESIZE;
}

err_t MemoryStreamDevice::truncate(int64_t offset)
{
  return ERR_IO_CANT_TRUNCATE;
}

void MemoryStreamDevice::close()
{
}

ByteArray MemoryStreamDevice::getBuffer() const
{
  ByteArray buffer;
  if (buffer.reserve(size) == ERR_OK)
    Memory::copy((void*)buffer.getData(), (const void*)data, size);
  return buffer;
}

// ============================================================================
// [Fog::ByteArrayStreamDevice]
// ============================================================================

struct FOG_HIDDEN ByteArrayStreamDevice : public StreamDevice
{
  ByteArrayStreamDevice(ByteArray data, uint32_t fflags);
  virtual ~ByteArrayStreamDevice();

  virtual int64_t seek(int64_t offset, int whence);
  virtual int64_t tell() const;

  virtual sysuint_t read(void* buffer, sysuint_t size);
  virtual sysuint_t write(const void* buffer, sysuint_t size);

  virtual err_t getSize(int64_t* size);
  virtual err_t setSize(int64_t size);
  virtual err_t truncate(int64_t offset);

  virtual void close();

  virtual ByteArray getBuffer() const;

  ByteArray data;
  sysuint_t pos;
};

ByteArrayStreamDevice::ByteArrayStreamDevice(ByteArray buffer, uint32_t fflags) :
  data(buffer), pos(FOG_UINT64_C(0))
{
  flags |= fflags | STREAM_IS_OPEN | STREAM_IS_SEEKABLE | STREAM_IS_MEMORY | STREAM_IS_GROWABLE;
}

ByteArrayStreamDevice::~ByteArrayStreamDevice()
{
}

int64_t ByteArrayStreamDevice::seek(int64_t offset, int whence)
{
  sysuint_t length = data.getLength();
  int64_t i;

  switch (whence)
  {
    case STREAM_SEEK_SET:
      i = offset;
      break;
    case STREAM_SEEK_CUR:
      i = (int64_t)pos + offset;
      break;
    case STREAM_SEEK_END:
      i = (int64_t)length + offset;
      break;
  }

  if (i < FOG_UINT64_C(0))
  {
    pos = 0;
    return -1;
  }
  else if (i > (int64_t)length)
  {
    pos = length;
    return -1;
  }
  else
  {
    pos = (sysuint_t)i;
    return i;
  }
}

int64_t ByteArrayStreamDevice::tell() const
{
  return (int64_t)pos;
}

sysuint_t ByteArrayStreamDevice::read(void* buffer, sysuint_t size)
{
  sysuint_t length = data.getLength();
  sysuint_t remain = (sysuint_t)(length - pos);
  if (size > remain) size = remain;

  Memory::copy(buffer, const_cast<char*>(data.getData() + pos), size);
  pos += size;

  return size;
}

sysuint_t ByteArrayStreamDevice::write(const void* buffer, sysuint_t size)
{
  sysuint_t length = data.getLength();
  sysuint_t overwriteSize = Math::min(length - pos, size);
  sysuint_t appendSize = size - overwriteSize;

  if (data.detach() != ERR_OK) return 0;

  const uint8_t* src = reinterpret_cast<const uint8_t*>(buffer);

  if (overwriteSize)
  {
    Memory::copy(const_cast<char*>(data.getXData() + pos), src, overwriteSize);
    src += overwriteSize;
    pos += overwriteSize;
  }

  if (appendSize)
  {
    if (data.append(reinterpret_cast<const char*>(src), appendSize) != ERR_OK)
      return overwriteSize;
    pos += appendSize;
  }

  return size;
}

err_t ByteArrayStreamDevice::getSize(int64_t* size)
{
  FOG_ASSERT(size != NULL);
  *size = (int64_t)data.getLength();
  return ERR_OK;
}

err_t ByteArrayStreamDevice::setSize(int64_t size)
{
  if (size < 0) return ERR_RT_INVALID_ARGUMENT;
  if (size >= SYSINT_MAX) return ERR_RT_OUT_OF_MEMORY;

  err_t err = data.resize((sysuint_t)size);
  if (err) return err;

  return ERR_OK;
}

err_t ByteArrayStreamDevice::truncate(int64_t offset)
{
  if (offset >= (int64_t)data.getLength()) return ERR_OK;

  data.truncate((sysuint_t)offset);
  if (pos > data.getLength()) pos = data.getLength();

  return ERR_OK;
}

void ByteArrayStreamDevice::close()
{
  data.free();
}

ByteArray ByteArrayStreamDevice::getBuffer() const
{
  return data;
}

// ============================================================================
// [Fog::NullStreamDevice]
// ============================================================================

struct FOG_HIDDEN MMapStreamDevice : public MemoryStreamDevice
{
  MMapStreamDevice();
  virtual ~MMapStreamDevice();

  virtual void close();

  err_t map(const String& fileName, bool loadOnFail);

  MapFile mapFile;
};

MMapStreamDevice::MMapStreamDevice()
{
}

MMapStreamDevice::~MMapStreamDevice()
{
  close();
}

err_t MMapStreamDevice::map(const String& fileName, bool loadOnFail)
{
  err_t err = mapFile.map(fileName, loadOnFail);
  if (err != ERR_OK) return err;

  data = reinterpret_cast<uint8_t*>(const_cast<void*>(mapFile.getData()));
  size = mapFile.getLength();
  cur = data;
  end = data + size;

  flags |=
    STREAM_IS_OPEN     |
    STREAM_IS_SEEKABLE |
    STREAM_IS_READABLE |
    STREAM_IS_MEMORY   |
    STREAM_IS_CLOSABLE   ;

  return ERR_OK;
}

void MMapStreamDevice::close()
{
  mapFile.unmap();
}

// ============================================================================
// [Fog::Stream]
// ============================================================================

StreamDevice* Stream::_dnull;

Stream::Stream() :
  _d(_dnull->ref())
{
}

Stream::Stream(const Stream& other) :
  _d(other._d->ref())
{
  if (!_d) _d = _dnull->ref();
}

Stream::Stream(StreamDevice* d) :
  _d(d)
{
}

Stream::~Stream()
{
  _d->deref();
}

void Stream::setSeekable(bool seekable)
{
  if (_d == _dnull) return;

  if (seekable)
    _d->flags |= STREAM_IS_SEEKABLE;
  else
    _d->flags &= ~STREAM_IS_SEEKABLE;
}

err_t Stream::openFile(const String& fileName, uint32_t openFlags)
{
  static uint32_t CREATE_PATH_FLAGS =
    STREAM_OPEN_CREATE | STREAM_OPEN_CREATE_PATH | STREAM_OPEN_WRITE;

  // If we are opening new file, we will close previous. This means that if 
  // open fail, we end at neutral state (null stream).
  close();

  err_t err;

  // Validate openFlags.
  if ((openFlags & STREAM_OPEN_RW) == 0) return ERR_RT_INVALID_ARGUMENT;

  // Create path if asked for.
  if ((openFlags & CREATE_PATH_FLAGS) == CREATE_PATH_FLAGS)
  {
    TemporaryString<TEMPORARY_LENGTH> dirName;
    if ((err = FileSystem::extractDirectory(dirName, fileName))) return err;

    if (!dirName.isEmpty() && dirName != Ascii8("."))
    {
      if ((err = FileSystem::createDirectory(dirName, true))) return err;
    }
  }

  // Try to open file using OS specific calls.
  StreamDevice* newd;

#if defined(FOG_OS_WINDOWS)
  err = HANDLEStreamDevice::openFile(fileName, openFlags, &newd);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  err = FdStreamDevice::openFile(fileName, openFlags, &newd);
#endif // FOG_OS_POSIX

  if (err) return err;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t Stream::openMMap(const String& fileName, bool loadOnFail)
{
  close();

  MMapStreamDevice* newd = new(std::nothrow) MMapStreamDevice();
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  err_t err = newd->map(fileName, loadOnFail);
  if (err != ERR_OK)
  {
    delete newd;
    return err;
  }

  atomicPtrXchg<StreamDevice>(&_d, newd)->deref();
  return ERR_OK;
}

#if defined(FOG_OS_WINDOWS)
err_t Stream::openHandle(HANDLE hFile, uint32_t openFlags, bool canClose)
{
  close();

  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
  {
    return ERR_RT_INVALID_ARGUMENT;
  }

  uint32_t fflags = STREAM_IS_SEEKABLE;
  if (openFlags & STREAM_OPEN_READ ) fflags |= STREAM_IS_READABLE;
  if (openFlags & STREAM_OPEN_WRITE) fflags |= STREAM_IS_WRITABLE;
  if (canClose) fflags |= STREAM_IS_CLOSABLE;

  StreamDevice* newd = new(std::nothrow) HANDLEStreamDevice(hFile, fflags);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
err_t Stream::openFd(int fd, uint32_t openFlags, bool canClose)
{
  close();
  if (fd < 0) return ERR_RT_INVALID_ARGUMENT;

  uint32_t fflags = STREAM_IS_SEEKABLE | STREAM_IS_GROWABLE;
  if (openFlags & STREAM_OPEN_READ ) fflags |= STREAM_IS_READABLE;
  if (openFlags & STREAM_OPEN_WRITE) fflags |= STREAM_IS_WRITABLE;
  if (canClose) fflags |= STREAM_IS_CLOSABLE;

  StreamDevice* newd = new(std::nothrow) FdStreamDevice(fd, fflags);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}
#endif // FOG_OS_POSIX

err_t Stream::openBuffer()
{
  ByteArray buffer;
  buffer.reserve(8192);
  return openBuffer(buffer);
}

err_t Stream::openBuffer(const ByteArray& buffer)
{
  close();

  StreamDevice* newd = new(std::nothrow) ByteArrayStreamDevice(buffer, 
    STREAM_IS_READABLE | STREAM_IS_WRITABLE);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

err_t Stream::openBuffer(void* buffer, sysuint_t size, uint32_t openFlags)
{
  close();
  if (buffer == NULL) return ERR_RT_INVALID_ARGUMENT;

  uint32_t fflags = 0;

  if (openFlags & STREAM_OPEN_READ ) fflags |= STREAM_IS_READABLE;
  if (openFlags & STREAM_OPEN_WRITE) fflags |= STREAM_IS_WRITABLE;

  StreamDevice* newd = new(std::nothrow) MemoryStreamDevice(buffer, size, fflags);
  if (!newd) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

int64_t Stream::seek(int64_t offset, int whence)
{
  return _d->seek(offset, whence);
}

int64_t Stream::tell() const
{
  return _d->tell();
}

sysuint_t Stream::read(void* buffer, sysuint_t size)
{
  return _d->read(buffer, size);
}

sysuint_t Stream::read(ByteArray& dst, sysuint_t size)
{
  err_t err = dst.prepare(size);
  if (err) return 0;

  sysuint_t n = _d->read((void*)dst.getXData(), size);
  dst.resize(n);
  return n;
}

sysuint_t Stream::readAll(ByteArray& dst, sysuint_t maxBytes)
{
  dst.clear();

  int64_t curPosition = -1;
  int64_t endPosition = -1;

  // If stream is not seekable we shouldn't seek.
  if (!isSeekable()) goto nonSeekable;

  curPosition = tell();
  endPosition = seek(0, STREAM_SEEK_END);

  if (curPosition == 0 && endPosition == 0)
  {
nonSeekable:
    // This happen for example in /proc/ or in virtual files. We will try to
    // read everything we can.
    uint64_t remain = (maxBytes == 0) ? UINT64_MAX : (uint64_t)maxBytes;

    for (;;)
    {
      sysuint_t count = 1024 * 1024; // 1 MB.
      sysuint_t done;

      if ((uint64_t)count > remain) count = (sysuint_t)remain;

      sysuint_t len = dst.getLength();
      err_t err = dst.reserve(len + (sysuint_t)count);
      if (err) break;

      done = read(dst.getMData() + len, count);
      dst.resize(len + done);

      if (done != count) break;
    }

    return dst.getLength();
  }
  else
  {
    if (curPosition == -1 || endPosition == -1) return 0;
    int64_t remain = endPosition - curPosition;

    if (maxBytes > 0 && remain > maxBytes)
      remain = maxBytes;

    if (seek(curPosition, SEEK_SET) == -1) return 0;

#if FOG_ARCH_BITS > 32
    if (remain > (int64_t)(SYSINT_MAX)) return 0;
#endif // FOG_ARCH_BITS > 32
    if (dst.reserve((sysuint_t)remain) != ERR_OK) return 0;

    sysuint_t n = read(dst.getMData(), (sysuint_t)remain);
    dst.resize(n);
    return n;
  }
}

sysuint_t Stream::write(const void* buffer, sysuint_t size)
{
  return _d->write(buffer, size);
}

sysuint_t Stream::write(const ByteArray& data)
{
  return _d->write((const void*)data.getData(), data.getLength());
}

err_t Stream::getSize(int64_t* size)
{
  return _d->getSize(size);
}

err_t Stream::setSize(int64_t size)
{
  return _d->setSize(size);
}

err_t Stream::truncate(int64_t offset)
{
  return _d->truncate(offset);
}

void Stream::close()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

ByteArray Stream::getBuffer() const
{
  return _d->getBuffer();
}

Stream& Stream::operator=(const Stream& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_stream_init(void)
{
  using namespace Fog;

  streamdevice_null.init();
  Stream::_dnull = streamdevice_null.instancep();

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_stream_shutdown(void)
{
  using namespace Fog;

  streamdevice_null.destroy();
}
