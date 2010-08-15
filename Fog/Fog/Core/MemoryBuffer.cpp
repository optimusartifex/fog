// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/MemoryBuffer.h>
#include <Fog/Core/MemoryManager.h>

namespace Fog {

// ============================================================================
// [Fog::MemoryBuffer]
// ============================================================================

uint8_t* MemoryBuffer::_alloc(sysuint_t size)
{
  FOG_ASSERT(size > _memorySize);
  if (_memoryBuffer != _embeddedBuffer) _memoryManager->free(_memoryBuffer, _memorySize);

  // Align...
  size = (size + 1023) & ~1023;

  if (FOG_UNLIKELY((_memoryBuffer = (uint8_t*)_memoryManager->alloc(size, &_memorySize)) == NULL))
  {
    _memoryBuffer = _embeddedBuffer;
    _memorySize = _embeddedSize;
    return NULL;
  }

  return _memoryBuffer;
}

} // Fog namespace
