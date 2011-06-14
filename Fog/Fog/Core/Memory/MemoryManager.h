// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMORYMANAGER_H
#define _FOG_CORE_MEMORY_MEMORYMANAGER_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>

namespace Fog {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::MemoryManager]
// ============================================================================

//! @brief Memory manager is interface that is used by some classes to allocate
//! and free memory.
//!
//! The purpose of this class is to make memory allocation / free totally
//! customizable so performance important tasks can run faster. The default
//! implementation that uses Memory::alloc() and Memory::free() can be get
//! using MemoryManager::getDefault().
//!
//! @internal
//!
//! Unlike @c BlockAllocator or @c ZoneAllocator, @c MemoryManager
//! is designed to replace malloc()/free() calls. It not allows to automatically
//! free memory in destructor. It's error if you allocate memory using
//! @c MemoryManager that is not freed when the @c MemoryManager instance is
//! destroyed.
struct FOG_API MemoryManager
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create a @c MemoryManager instance.
  MemoryManager();

  //! @brief Destroy the @c MemoryManager instance.
  //!
  //! When destructor is called, all memory allocated by memory manager must
  //! be freed.
  virtual ~MemoryManager();

  // --------------------------------------------------------------------------
  // [Alloc / Free]
  // --------------------------------------------------------------------------

  //! @brief Alloc @a size bytes of memory.
  //!
  //! @param size The size of memory to be allocated.
  //! @param allocated If this parameter is non-null then memory allocated can
  //! be bigger than the @a size is and the actual size of the allocated memory
  //! is returned through it. If you use this parameter then you MUST pass
  //! the returned size to the @c free() when freeing the allocated block.
  //!
  //! This method should be thread-safe for memory manager which can be used
  //! by different threads. In some cases it's enabled to make this method
  //! reentrant.
  virtual void* alloc(size_t size, size_t* allocated) = 0;

  //! @brief Free memory previously allocated by @c alloc() method.
  //! @param ptr Pointer to memory location to free. It must be valid, NULL
  //! is not allowed!
  //! @param size Size of the memory block passed through @a ptr. It must be
  //! value passed/returned by @c alloc() method. Although some memory
  //! allocators allow this to be any value it's mistake!
  virtual void free(void* p, size_t size) = 0;

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static MemoryManager* getDefault();

private:
  _FOG_CLASS_NO_COPY(MemoryManager)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMORYMANAGER_H
