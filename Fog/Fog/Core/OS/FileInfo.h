// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_FILEINFO_H
#define _FOG_CORE_OS_FILEINFO_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>

#if defined(FOG_OS_POSIX)
# include <sys/stat.h>
#endif // FOG_OS_POSIX

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::FileInfoData]
// ============================================================================

struct FOG_NO_EXPORT FileInfoData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FileInfoData* addRef() const
  {
    reference.inc();
    return const_cast<FileInfoData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.fileinfo_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;
  //! @brief Variable type and flags.
  uint32_t vType;
  //! @brief File entry flags.
  uint32_t fileFlags;

  //! @brief File entry path (the directory where it's located).
  Static<StringW> filePath;
  //! @brief File entry name.
  Static<StringW> fileName;

  //! @brief File size.
  uint64_t size;

  //! @brief The time when the file was created.
  //!
  //! @note Can be zero if filesystem doesn't support such attribute.
  Time creationTime;

  //! @brief The last time when the file was modified.
  Time modifiedTime;

  //! @brief The last time when the file was accessed.
  //!
  //! @note Can be zero due to performance reasons. Access time is often turned
  //! off.
  Time accessTime;
};

// ============================================================================
// [Fog::FileInfo]
// ============================================================================

//! @brief File information.
struct FOG_NO_EXPORT FileInfo
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FileInfo()
  {
    fog_api.fileinfo_ctor(this);
  }

  FOG_INLINE FileInfo(const FileInfo& other)
  {
    fog_api.fileinfo_ctorCopy(this, &other);
  }

  explicit FOG_INLINE FileInfo(FileInfoData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FileInfo()
  {
    fog_api.fileinfo_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach()
  {
    return isDetached() ? (err_t)ERR_OK : fog_api.fileinfo_detach(this);
  }

  FOG_INLINE err_t _detach()
  {
    return fog_api.fileinfo_detach(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getFileName() const { return _d->fileName; }
  FOG_INLINE uint32_t getFileFlags() const { return _d->fileFlags; }
  FOG_INLINE uint64_t getSize() const { return _d->size; }
  FOG_INLINE Time getCreationTime() const { return _d->creationTime; }
  FOG_INLINE Time getModifiedTime() const { return _d->modifiedTime; }

  // --------------------------------------------------------------------------
  // [Read]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t fromFile(const StringW& path)
  {
    return fog_api.fileinfo_fromFile(this, &path);
  }

  FOG_INLINE err_t fromFile(const StringW& filePath, const StringW& fileName)
  {
    return fog_api.fileinfo_fromFileEx(this, &filePath, &fileName);
  }

#if defined(FOG_OS_WINDOWS)
  FOG_INLINE err_t fromWinFileAttributeData(const StringW& filePath, const StringW& fileName, const WIN32_FILE_ATTRIBUTE_DATA* wfad)
  {
    return fog_api.fileinfo_fromWinFileAttributeData(this, &filePath, &fileName, wfad);
  }

  FOG_INLINE err_t fromWinFindData(const StringW& filePath, const WIN32_FIND_DATAW* wfd)
  {
    return fog_api.fileinfo_fromWinFindData(this, &filePath, wfd);
  }
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  FOG_INLINE err_t fromStat(const StringW& filePath, const StringW& fileName, const struct stat* s)
  {
    return fog_api.fileinfo_fromStat(this, &filePath, &fileName, s);
  }
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE const FileInfo& operator=(const FileInfo& other)
  {
    fog_api.fileinfo_copy(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FileInfoData)
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_OS_FILEINFO_H
