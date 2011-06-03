// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMORYBUFFER_H
#define _FOG_CORE_MEMORY_MEMORYBUFFER_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryManager.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemoryBuffer]
// ============================================================================

//! @brief Memory buffer is used to hold one block of allocated memory used
//! mainly as a temporary storage.
//!
//! Purpose of this class is to define interface that can be used to work with
//! temporary memory. The simplest usage scenario is to use the @c MemoryBuffer
//! to work with temporary array of some elements. Complicated scenario is to
//! use @c MemoryBuffer as an input parameter to a function that needs always
//! temporary memory, but the size can vary.
//!
//! The @c MemoryBuffer class allows to embed statically allocated buffer that
//! can be used instead of dynamically allocated buffer. The embedded buffer
//! can be allocated on the stack (using @c TemporaryMemoryBuffer template) or
//! pre-allocated by you. Using embedded buffer should be benefical in heavy
//! multithreaded environment when processing small chunks of data.
//!
//! All members of memory buffer are inlined for maximum performance.
struct FOG_API MemoryBuffer
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a @c MemoryManager instance, using default memory manager
  //! and no embedded buffer.
  FOG_INLINE MemoryBuffer() :
    _memoryManager(Memory::getDefaultManager()),
    _memoryBuffer(NULL),
    _memorySize(0),
    _embeddedBuffer(NULL),
    _embeddedSize(0)
  {
  }

  //! @brief Create a @c MemoryManager instance, using a given memory manager
  //! and no embedded buffer.
  FOG_INLINE MemoryBuffer(MemoryManager* memoryManager) :
    _memoryManager(memoryManager),
    _memoryBuffer(NULL),
    _memorySize(0),
    _embeddedBuffer(NULL),
    _embeddedSize(0)
  {
  }

  //! @brief Create a @c MemoryManager instance, using a given memory manager
  //! and a given embedded buffer.
  FOG_INLINE MemoryBuffer(MemoryManager* memoryManager, uint8_t* embeddedBuffer, size_t embeddedSize) :
    _memoryManager(memoryManager),
    _memoryBuffer(embeddedBuffer),
    _memorySize(embeddedSize),
    _embeddedBuffer(embeddedBuffer),
    _embeddedSize(embeddedSize)
  {
  }

  //! @brief Destroy the @c MemoryManager instance, freeing all memory
  //! allocated by @c MemoryManager.
  FOG_INLINE ~MemoryBuffer()
  {
    if (_memoryBuffer != _embeddedBuffer)
    {
      _memoryManager->free(_memoryBuffer, _memorySize);
    }
  }

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  //! @brief Try to allocate memory buffer at @a size and return it.
  FOG_INLINE uint8_t* alloc(size_t size)
  {
    return (size > _memorySize) ? _alloc(size) : _memoryBuffer;
  }

protected:
  //! @brief Private allocator used if there is no memory. This method is not
  //! inlined, because it is called only if embedded buffer is too small.
  uint8_t* _alloc(size_t size);

public:
  FOG_INLINE uint8_t* getMemoryBuffer() const { return _memoryBuffer; }
  FOG_INLINE size_t getMemorySize() const { return _memorySize; }

  // --------------------------------------------------------------------------
  // [Embedded memory]
  // --------------------------------------------------------------------------

  //! @brief Get embedded memory block which can be used instead of memory
  //! allocated by @c MemoryManager (can be @c NULL if no embedded block
  //! is used).
  FOG_INLINE uint8_t* getEmbeddedBuffer() const { return _embeddedBuffer; }
  //! @brief Get size of embedded memory block (can be zero if no embedded
  //! block is used).
  FOG_INLINE size_t getEmbeddedSize() const { return _embeddedSize; }

protected:
  //! @brief Memory manager used to allocate / free the memory block.
  MemoryManager* _memoryManager;

  //! @brief Memory buffer.
  uint8_t* _memoryBuffer;
  //! @brief Memory buffer size.
  size_t _memorySize;

  //! @brief Embedded memory block (will be never freed, can be stack based
  //! memory).
  uint8_t* _embeddedBuffer;
  //! @brief Embedded memory block size.
  size_t _embeddedSize;

private:
  _FOG_CLASS_NO_COPY(MemoryBuffer)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMORYBUFFER_H
