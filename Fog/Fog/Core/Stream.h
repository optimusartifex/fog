// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_STREAM_H
#define _FOG_CORE_STREAM_H

// [Dependencies]
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Streaming
//! @{

// ============================================================================
// [Fog::Constants]
// ============================================================================

//! @brief Flags describing @c StreamDevice.
enum STREAM_DEVICE_FLAGS
{
  STREAM_IS_OPEN     = (1 << 0),
  STREAM_IS_SEEKABLE = (1 << 1),
  STREAM_IS_READABLE = (1 << 2),
  STREAM_IS_WRITABLE = (1 << 3),
  STREAM_IS_CLOSABLE = (1 << 4),

  STREAM_IS_HFILE    = (1 << 16),
  STREAM_IS_FD       = (1 << 17),
  STREAM_IS_MEMORY   = (1 << 18),
  STREAM_IS_GROWABLE = (1 << 19)
};

//! @brief Stream open flags.
enum STREAM_OPEN_FLAGS
{
  STREAM_OPEN_READ = (1 << 0),
  STREAM_OPEN_WRITE = (1 << 1),
  STREAM_OPEN_RW = STREAM_OPEN_READ | STREAM_OPEN_WRITE,
  STREAM_OPEN_TRUNCATE = (1 << 2),
  STREAM_OPEN_APPEND = (1 << 3),
  STREAM_OPEN_CREATE = (1 << 4),
  STREAM_OPEN_CREATE_PATH = (1 << 5),
  STREAM_OPEN_CREATE_ONLY = (1 << 6)
};

//! @brief Stream seek mode.
enum STREAM_SEEK_MODE
{
  STREAM_SEEK_SET = 0,
  STREAM_SEEK_CUR = 1,
  STREAM_SEEK_END = 2
};

// ============================================================================
// [Fog::StreamDevice]
// ============================================================================

//! @brief Stream device is interface used inside the Fog::Stream.
//!
//! It contains abstract virtual methods that can be reimplemented to make a
//! new stream device that can be used in all functions that needs Fog::Stream.
struct FOG_API StreamDevice
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  StreamDevice();
  virtual ~StreamDevice();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  virtual StreamDevice* ref() const;
  virtual void deref();

  // --------------------------------------------------------------------------
  // [Abstract]
  // --------------------------------------------------------------------------

  virtual int64_t seek(int64_t offset, int whence) = 0;
  virtual int64_t tell() const = 0;

  virtual sysuint_t read(void* buffer, sysuint_t size) = 0;
  virtual sysuint_t write(const void* buffer, sysuint_t size) = 0;

  virtual err_t getSize(int64_t* size) = 0;
  virtual err_t setSize(int64_t size) = 0;
  virtual err_t truncate(int64_t offset) = 0;

  virtual void close() = 0;

  virtual ByteArray getBuffer() const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable Atomic<sysuint_t> refCount;
  uint32_t flags;
};

// ============================================================================
// [Fog::Stream]
// ============================================================================

//! @brief Stream is IO stream implementation used in by Fog library.
struct FOG_API Stream
{
  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  static StreamDevice* _dnull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Stream();
  Stream(const Stream& stream);
  explicit Stream(StreamDevice* d);
  ~Stream();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFlags() const { return _d->flags; }

  FOG_INLINE bool isNull()     const { return _d == _dnull; }

  FOG_INLINE bool isOpen()     const { return (_d->flags & STREAM_IS_OPEN    ) != 0; }
  FOG_INLINE bool isSeekable() const { return (_d->flags & STREAM_IS_SEEKABLE) != 0; }
  FOG_INLINE bool isReadable() const { return (_d->flags & STREAM_IS_READABLE) != 0; }
  FOG_INLINE bool isWritable() const { return (_d->flags & STREAM_IS_WRITABLE) != 0; }
  FOG_INLINE bool isClosable() const { return (_d->flags & STREAM_IS_CLOSABLE) != 0; }

  FOG_INLINE bool isFile()     const { return (_d->flags & (STREAM_IS_FD | STREAM_IS_HFILE)) != 0; }
  FOG_INLINE bool isHFILE()    const { return (_d->flags & STREAM_IS_HFILE   ) != 0; }
  FOG_INLINE bool isFD()       const { return (_d->flags & STREAM_IS_FD      ) != 0; }
  FOG_INLINE bool isMemory()   const { return (_d->flags & STREAM_IS_MEMORY  ) != 0; }
  FOG_INLINE bool isGrowable() const { return (_d->flags & STREAM_IS_GROWABLE) != 0; }

  void setSeekable(bool seekable);

  // --------------------------------------------------------------------------
  // [Open]
  // --------------------------------------------------------------------------

  err_t openFile(const String& fileName, uint32_t openFlags);
  err_t openMMap(const String& fileName, bool loadOnFail = true);

#if defined(FOG_OS_WINDOWS)
  err_t openHandle(HANDLE hFile, uint32_t openFlags, bool canClose);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  err_t openFd(int fd, uint32_t openFlags, bool canClose);
#endif // FOG_OS_POSIX

  err_t openBuffer();
  err_t openBuffer(const ByteArray& buffer);
  err_t openBuffer(void* buffer, sysuint_t size, uint32_t openFlags);

  // --------------------------------------------------------------------------
  // [Seek / Tell]
  // --------------------------------------------------------------------------

  int64_t seek(int64_t offset, int whence);
  int64_t tell() const;

  // --------------------------------------------------------------------------
  // [Read / Write]
  // --------------------------------------------------------------------------

  sysuint_t read(void* buffer, sysuint_t size);
  sysuint_t read(ByteArray& dst, sysuint_t size);
  sysuint_t readAll(ByteArray& dst, sysuint_t maxBytes = 0);

  sysuint_t write(const void* buffer, sysuint_t size);
  sysuint_t write(const ByteArray& data);

  // --------------------------------------------------------------------------
  // [GetSize, SetSize, Truncate]
  // --------------------------------------------------------------------------

  err_t getSize(int64_t* size);
  err_t setSize(int64_t size);
  err_t truncate(int64_t offset);

  // --------------------------------------------------------------------------
  // [Close]
  // --------------------------------------------------------------------------

  void close();

  // --------------------------------------------------------------------------
  // [Stream Specific]
  // --------------------------------------------------------------------------

  //! @brief Get stream memory buffer. This method works only on memory streams.
  //!
  //! If stream was open by:
  //!   <core>openBuffer(void* buffer, sysuint_t size, uint32_t openFlags);</core>
  //! buffer will be created for it and data will be copied to this buffer.
  //!
  //! If stream was open by @c ByteArray instance, this method will return it.
  ByteArray getBuffer() const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  Stream& operator=(const Stream& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(StreamDevice)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Stream, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_CORE_STREAM_H
