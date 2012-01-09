// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_DIRITERATOR_H
#define _FOG_CORE_OS_DIRITERATOR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/OS/FileInfo.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::DirIteratorData]
// ============================================================================

struct FOG_NO_EXPORT DirIteratorData
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  void* handle;
  uint32_t skipParent;
  Static<StringW> pathAbs;
};

// ============================================================================
// [Fog::DirIterator]
// ============================================================================

//! @brief Directory iterator.
//!
//! This class can be used to iterate over a directory.
//!
//! @code
//! // Open a directory stream with a given path, path will be normalized,
//! // so the "" or "." can be used for current directory or for example ".."
//! // for a parent directory.
//! Fog::DirIterator dir(Fog::Ascii8("."));
//! Fog::FileInfo fi;
//!
//! // We can check if directory is successfully opened, bud @c read() return
//! // false if not, so this check isn't necessary.
//! while (dir.read(fi))
//! {
//!   // The entryt was read. Use entry instance to check whether the entry
//!   // is a file or a directory, to get file size, or name, etc...
//! }
//!
//! // Close directory if open.
//! dir.close();
//! @endcode
struct FOG_NO_EXPORT DirIterator
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE DirIterator()
  {
    fog_api.diriterator_ctor(this);
  }

  FOG_INLINE DirIterator(const StringW& path, bool skipParent = true)
  {
    fog_api.diriterator_ctorString(this, &path, skipParent);
  }

  FOG_INLINE ~DirIterator()
  {
    fog_api.diriterator_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isOpen() const { return _d->handle != NULL; }

  FOG_INLINE void* getHandle() const { return _d->handle; }
  FOG_INLINE const StringW& getPath() const { return _d->pathAbs; }

  // --------------------------------------------------------------------------
  // [Open / Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t open(const StringW& path, bool skipParent = true)
  {
    return fog_api.diriterator_open(this, &path, skipParent);
  }

  FOG_INLINE void close()
  {
    fog_api.diriterator_close(this);
  }

  // --------------------------------------------------------------------------
  // [Read]
  // --------------------------------------------------------------------------

  FOG_INLINE bool read(FileInfo& fileInfo)
  {
    return fog_api.diriterator_readFileInfo(this, &fileInfo);
  }

  FOG_INLINE bool read(StringW& fileName)
  {
    return fog_api.diriterator_readFileName(this, &fileName);
  }

  // --------------------------------------------------------------------------
  // [Rewind / Tell]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t rewind()
  {
    return fog_api.diriterator_rewind(this);
  }

  FOG_INLINE int64_t tell()
  {
    return fog_api.diriterator_tell(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(DirIteratorData)

private:
  _FOG_NO_COPY(DirIterator)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_DIRITERATOR_H
