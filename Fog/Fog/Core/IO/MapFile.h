// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_IO_MAPFILE_H
#define _FOG_CORE_IO_MAPFILE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_IO
//! @{

// ============================================================================
// [Fog::MapFile]
// ============================================================================

//! @brief Class designed to provide mapping files into shared memory in a few
//! lines of code. Mapping files into shared memory increases performance. It
//! avoids mutex locking and unlocking in read / white functions and gives us
//! higher performance if file was loaded into shared memory before. System
//! will simply make data readable for current process.
//!
//! @note Class is designed only for read-only access.
struct FOG_API MapFile
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MapFile();
  ~MapFile();

  // --------------------------------------------------------------------------
  // [Map / Unmap]
  // --------------------------------------------------------------------------

  //! @brief Map a given file into memory.
  err_t map(const StringW& fileName, bool loadOnFail = true);
  //! @brief Unmap file if mapped.
  void unmap();

  //! @brief Get whether file is open.
  FOG_INLINE bool isOpen() const { return _data != NULL; }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get mapped file name.
  FOG_INLINE const StringW& getFileName() const { return _fileName; }

  //! @brief Get mapped file data.
  FOG_INLINE const void* getData() const { return _data; }

  //! @brief Get mapped file length.
  FOG_INLINE size_t getLength() const { return _length; }

  // --------------------------------------------------------------------------
  // [OS Specific]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  FOG_INLINE HANDLE getHFileMapping() const { return _hFileMapping; }

  FOG_INLINE HANDLE getHFile() const { return _hFile; }
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  FOG_INLINE int getFd() const { return _fd; }
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief States that can be returned by @c getState() method.
  enum STATE
  {
    //! @brief No mapped file.
    STATE_NONE = 0,
    //! @brief File is mapped in memory (mmap).
    STATE_MAPPED = 1,
    //! @brief File failed to map and it was loaded instead.
    STATE_LOADED = 2
  };

  //! @brief Get mapped file state.
  FOG_INLINE int getState() const { return _state; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

private:
  StringW _fileName;

  void* _data;
  size_t _length;

#if defined(FOG_OS_WINDOWS)
  HANDLE _hFileMapping;
  HANDLE _hFile;
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  int _fd;
#endif // FOG_OS_POSIX

  int _state;

private:
  _FOG_NO_COPY(MapFile)
};

//! @}

} // Fog namespace

#endif // _FOG_CORE_IO_MAPFILE_H
