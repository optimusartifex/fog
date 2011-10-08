// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMBUFFER_H
#define _FOG_CORE_MEMORY_MEMBUFFER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemBuffer]
// ============================================================================

//! @brief Memory buffer.
//!
//! Memory buffer is a helper class which holds pointer to allocated memory
//! which is freed at destruction time.
struct FOG_NO_EXPORT MemBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MemBuffer() :
    _mem(NULL),
    _buf(NULL),
    _capacity(0)
  {
  }

  explicit FOG_INLINE MemBuffer(_Uninitialized)
  {
  }

  FOG_INLINE ~MemBuffer()
  {
    _reset();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void* getMem() const { return _mem; }
  FOG_INLINE size_t getCapacity() const { return _capacity; }

  // --------------------------------------------------------------------------
  // [Alloc / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void* alloc(size_t size)
  {
    if (size <= _capacity)
      return _mem;

    if (_mem != _buf)
      MemMgr::free(_mem);

    _mem = MemMgr::alloc(size);
    _capacity = size;

    return _mem;
  }

  FOG_INLINE void reset()
  {
    _reset();

    _mem = NULL;
    _capacity = 0;
  }

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

  FOG_INLINE void _reset()
  {
    if (_mem != _buf)
      MemMgr::free(_mem);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  void *_mem;
  void *_buf;

  size_t _capacity;

private:
  _FOG_NO_COPY(MemBuffer)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMBUFFER_H
