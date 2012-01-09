// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMBUFFERTMP_P_H
#define _FOG_CORE_MEMORY_MEMBUFFERTMP_P_H

// [Dependencies]
#include <Fog/Core/Memory/MemBuffer.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemBufferTmp<>]
// ============================================================================

//! @brief Memory buffer using static storage.
//!
//! This template is for fast routines that need to use memory preferably
//! allocated on the stack, but the memory requirement is not known at compile
//! time. The count of bytes allocated on the stack is the template parameter
//! @a N.
//!
//! This class is used internaly in Fog/Core and all upper libraries to simplify
//! memory management in cases that local memory is needed to complete an easier
//! task. There are situations where it's plausible to use the similar memory
//! allocation like alloca(), but using safe and portable way.
template<size_t N>
struct MemBufferTmp : public MemBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MemBufferTmp() :
    MemBuffer(UNINITIALIZED)
  {
    _mem = _storage;
    _buf = _storage;
    _capacity = N;
  }

  FOG_INLINE ~MemBufferTmp()
  {
  }

  // --------------------------------------------------------------------------
  // [Alloc / Reset]
  // --------------------------------------------------------------------------

  using MemBuffer::alloc;

  FOG_INLINE void reset()
  {
    _reset();

    _mem = _buf;
    _capacity = N;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _storage[N];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMBUFFERTMP_P_H
