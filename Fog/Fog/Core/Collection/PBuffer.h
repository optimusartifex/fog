// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COLLECTION_UBUFFER_H
#define _FOG_CORE_COLLECTION_UBUFFER_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Memory/Memory.h>

namespace Fog {

//! @addtogroup Fog_Core_Collection
//! @{

// ============================================================================
// Fog::PBuffer<>
// ============================================================================

//! @brief This template is for fast routines that need to use memory
//! preferably allocated on the stack. Maximum size of memory allocated
//! on the stack is @a N.
//!
//! If N is zero, the heap memory is used.
//!
//! This class is used internaly in Fog/Core and all upper libraries to simplify
//! some code, because there are situations where we need to alloc memory by
//! alloca(), but if the amount of requested memory is too big, it's better
//! to use memory on the heap.
//!
//! @c alloc() member can be called only once. Memory allocated on the heap is
//! freed by destructor or explicit @c reset() call.
//!
//! If you use @c reset() to free allocated bytes, you can use @c alloc() again.
//!
//! This template simulates the @c alloca() function, improving portability
//! and using heap memory allocation if requested buffer is too large.
template<sysuint_t N = 0>
struct PBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PBuffer() :
    _mem(NULL)
  {
  }

  FOG_INLINE ~PBuffer()
  {
    _reset();
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE void* getMem()
  {
    return _mem;
  }

  FOG_INLINE void* getPrivateStorage()
  {
    return _storage;
  }

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  FOG_INLINE void* alloc(sysuint_t size)
  {
    FOG_ASSERT(_mem == NULL);

    if (N == 0)
      return (_mem = Memory::alloc(size));
    else if (size > N)
      return (_mem = Memory::alloc(size));
    else
      return (_mem = (void*)_storage);
  }

  FOG_INLINE void reset()
  {
    _reset();
    _mem = NULL;
  }

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

protected:
  FOG_INLINE void _reset()
  {
    if (_mem != NULL && (N == 0 || (_mem != (void*)_storage))) Memory::free(_mem);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  void *_mem;
  uint8_t _storage[N];

private:
  FOG_DISABLE_COPY(PBuffer)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_COLLECTION_UBUFFER_H
