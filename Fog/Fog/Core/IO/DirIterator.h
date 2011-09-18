// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_IO_DIRITERATOR_H
#define _FOG_CORE_IO_DIRITERATOR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/IO/DirEntry.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_IO
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
//! Fog::DirEntry entry;
//!
//! // We can check if directory is successfully opened, bud @c read() return
//! // false if not, so this check isn't necessary.
//! while (dir.read(entry))
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

  FOG_NO_EXPORT DirIterator()
  {
    _api.diriterator.ctor(this);
  }

  FOG_NO_EXPORT DirIterator(const StringW& path)
  {
    _api.diriterator.ctorString(this, &path);
  }

  FOG_NO_EXPORT ~DirIterator()
  {
    _api.diriterator.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isOpen() const { return _d->handle != NULL; }

  FOG_INLINE void* getHandle() const { return _d->handle; }
  FOG_INLINE const StringW& getPath() const { return _d->pathAbs; }

  FOG_INLINE bool getSkipDots() const { return _skipDots; }
  FOG_INLINE void setSkipDots(bool val) { _skipDots = val; }

  // --------------------------------------------------------------------------
  // [Open / Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t open(const StringW& path)
  {
    return _api.diriterator.open(this, &path);
  }

  FOG_INLINE void close()
  {
    _api.diriterator.close(this);
  }

  // --------------------------------------------------------------------------
  // [Read]
  // --------------------------------------------------------------------------

  FOG_INLINE bool read(DirEntry& dirEntry)
  {
    return _api.diriterator.readDirEntry(this, &dirEntry);
  }

  FOG_INLINE bool read(StringW& fileName)
  {
    return _api.diriterator.readString(this, &fileName);
  }

  // --------------------------------------------------------------------------
  // [Rewind / Tell]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t rewind()
  {
    return _api.diriterator.rewind(this);
  }

  FOG_INLINE int64_t tell()
  {
    return _api.diriterator.tell(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(DirIteratorData)
  bool _skipDots;

private:
  _FOG_NO_COPY(DirIterator)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_IO_DIRITERATOR_H
