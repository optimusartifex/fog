// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MEMORY_MEMORY_H
#define _FOG_CORE_MEMORY_MEMORY_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Api.h>

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct MemoryManager;

// ============================================================================
// [Function Prototypes]
// ============================================================================

typedef void (FOG_CDECL *MemoryCleanupHandlerFn)(void* closure);

// ============================================================================
// [Fog::Uint64Union]
// ============================================================================

union UInt64Union
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  FOG_INLINE int32_t getI32Lo() const { return i32[0]; }
  FOG_INLINE int32_t getI32Hi() const { return i32[1]; }

  FOG_INLINE uint32_t getU32Lo() const { return u32[0]; }
  FOG_INLINE uint32_t getU32Hi() const { return u32[1]; }
#else
  FOG_INLINE int32_t getI32Lo() const { return i32[1]; }
  FOG_INLINE int32_t getI32Hi() const { return i32[0]; }

  FOG_INLINE uint32_t getU32Lo() const { return u32[1]; }
  FOG_INLINE uint32_t getU32Hi() const { return u32[0]; }
#endif

  FOG_INLINE int64_t getI64() const { return i64; }
  FOG_INLINE uint64_t getU64() const { return u64; }

  FOG_INLINE void setI64(int64_t x) { i64 = x; }
  FOG_INLINE void setU64(uint64_t x) { u64 = x; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  int8_t i8[8];
  uint8_t u8[8];

  int32_t i32[2];
  uint32_t u32[2];

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
  struct { uint32_t u32Lo, u32Hi; };
  struct { int32_t  i32Lo, i32Hi; };
#else
  struct { uint32_t u32Hi, u32Lo; };
  struct { int32_t  i32Hi, i32Lo; };
#endif

  int64_t i64;
  uint64_t u64;
};

// ============================================================================
// [Fog::Memory]
// ============================================================================

//! @brief Memory related functions.
//!
//! @c new() and @c delete() in @c Fog namespace are modified to call
//! @c Memory::alloc() and @c Memory::free() functions.
namespace Memory {

//! @addtogroup Fog_Core_Memory
//! @{

// ============================================================================
// [Fog::Memory - Memory Manager]
// ============================================================================

FOG_API MemoryManager* getDefaultManager();

// ============================================================================
// [Fog::Memory - Alloc / Free]
// ============================================================================

static FOG_INLINE void* alloc(sysuint_t size)
{
  return _core.memory.alloc(size);
}

static FOG_INLINE void* calloc(sysuint_t size)
{
  return _core.memory.calloc(size);
}

static FOG_INLINE void* realloc(void* addr, sysuint_t size)
{
  return _core.memory.realloc(addr, size);
}

static FOG_INLINE void* reallocOrFree(void* addr, sysuint_t size)
{
  return _core.memory.reallocOrFree(addr, size);
}

static FOG_INLINE void free(void* addr)
{
  return _core.memory.free(addr);
}

// ============================================================================
// [Fog::Memory - Cleanup]
// ============================================================================

static FOG_INLINE void cleanup()
{
  _core.memory.cleanup();
}

static FOG_INLINE err_t registerCleanupHandler(MemoryCleanupHandlerFn handler, void* closure)
{
  return _core.memory.registerCleanupHandler((void*)handler, closure);
}

static FOG_INLINE err_t unregisterCleanupHandler(MemoryCleanupHandlerFn handler, void* closure)
{
  return _core.memory.unregisterCleanupHandler((void*)handler, closure);
}

// ============================================================================
// [Fog::Memory - Ops]
// ============================================================================

static FOG_INLINE void* copy(void* dst, const void* src, sysuint_t size)
{
  return _core.memory.copy(dst, src, size);
}

static FOG_INLINE void* move(void* dst, const void* src, sysuint_t size)
{
  return _core.memory.move(dst, src, size);
}

static FOG_INLINE void* set(void* dst, int c, sysuint_t size)
{
  return _core.memory.set(dst, c, size);
}

static FOG_INLINE void* zero(void* dst, sysuint_t size)
{
  return _core.memory.zero(dst, size);
}

// ============================================================================
// [Fog::Memory - Ops (nt)]
// ============================================================================

static FOG_INLINE void* copy_nt(void* dst, const void* src, sysuint_t size)
{
  return _core.memory.copy_nt(dst, src, size);
}

static FOG_INLINE void* set_nt(void* dst, int c, sysuint_t size)
{
  return _core.memory.set_nt(dst, c, size);
}

static FOG_INLINE void* zero_nt(void* dst, sysuint_t size)
{
  return _core.memory.zero_nt(dst, size);
}

// ============================================================================
// [Fog::Memory - Xchg]
// ============================================================================

static FOG_INLINE void xchg(void* mem0, void* mem1, sysuint_t size)
{
  _core.memory.xchg(mem0, mem1, size);
}

//! @}

} // Memory namespace
} // Fog namespace

#if !defined(FOG_USE_STDNEW)
static FOG_INLINE void* _fog_memory_alloc(sysuint_t size) { return Fog::Memory::alloc(size); }
static FOG_INLINE void _fog_memory_free(void* addr) { return Fog::Memory::free(addr); }
#endif

// [Guard]
#endif // _FOG_CORE_MEMORY_MEMORY_H
