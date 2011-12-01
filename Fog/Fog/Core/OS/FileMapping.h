// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_FILEMAPPING_H
#define _FOG_CORE_OS_FILEMAPPING_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::FileMappingData]
// ============================================================================

struct FOG_NO_EXPORT FileMappingData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FileMappingData* addRef() const
  {
    reference.inc();
    return const_cast<FileMappingData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.filemapping_dRelease(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Reserved.
  uint32_t reserved;
  //! @brief Flags.
  uint32_t flags;

  //! @brief MMapped file-name.
  Static<StringW> fileName;

  //! @brief Data pointer.
  void* data;
  //! @brief Data length (file length).
  size_t length;

#if defined(FOG_OS_WINDOWS)
  //! @brief Handle to the Windows FILE.
  HANDLE hFile;
  //! @brief Handle to the Windows FILEMAPPING.
  HANDLE hFileMapping;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  //! @brief Posix file descriptor.
  int fd;
#endif // FOG_OS_POSIX
};

// ============================================================================
// [Fog::FileMapping]
// ============================================================================

//! @brief FileMapping is designed to provide mapping files into shared memory
//! in a few lines of code. Mapping files into shared memory increases
//! performance. It avoids mutex locking and unlocking in read / white functions
//! and gives us the highest possible performance for read-only file access.
//!
//! @note FileMapping class is designed only for read-only access across platforms.
struct FOG_NO_EXPORT FileMapping
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FileMapping()
  {
    fog_api.filemapping_ctor(this);
  }

  FOG_INLINE FileMapping(const FileMapping& other)
  {
    fog_api.filemapping_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE FileMapping(FileMapping&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE ~FileMapping()
  {
    fog_api.filemapping_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get whether file is open.
  FOG_INLINE bool isOpen() const
  {
    return _d->data != NULL;
  }

  //! @brief Get whether the load-fallback was used to load the file in case
  //! that file-mapping failed.
  FOG_INLINE bool isLoadFallbackUsed() const
  {
    return (_d->flags & FILE_MAPPING_FLAG_LOAD_FALLBACK) != 0;
  }

  // --------------------------------------------------------------------------
  // [Open / Close]
  // --------------------------------------------------------------------------

  //! @brief Create a file-mapping.
  FOG_INLINE err_t open(const StringW& fileName, uint32_t flags)
  {
    return fog_api.filemapping_open(this, &fileName, flags);
  }

  //! @brief Close the file-mapping.
  FOG_INLINE void close()
  {
    fog_api.filemapping_close(this);
  }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get mapped file name.
  FOG_INLINE const StringW& getFileName() const
  {
    return _d->fileName;
  }

  //! @brief Get mapped file data.
  FOG_INLINE const void* getData() const
  {
    return _d->data;
  }

  //! @brief Get mapped file length.
  FOG_INLINE size_t getLength() const
  {
    return _d->length;
  }

  // --------------------------------------------------------------------------
  // [Windows Specific]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  FOG_INLINE HANDLE getHFile() const { return _d->hFile; }
  FOG_INLINE HANDLE getHFileMapping() const { return _d->hFileMapping; }
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Posix Specific]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_POSIX)
  FOG_INLINE int getFd() const { return _d->fd; }
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FileMapping& operator=(const FileMapping& other)
  {
    fog_api.filemapping_copy(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FileMappingData)
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_OS_FILEMAPPING_H
