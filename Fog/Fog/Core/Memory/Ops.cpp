// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/Ops.h>

namespace Fog {

// ============================================================================
// [Fog::Memory - Copy]
// ============================================================================

static void* FOG_CDECL Memory_zero(void* dst, size_t size)
{
  return memset(dst, 0, size);
}

// ============================================================================
// [Fog::Memory - Xchg]
// ============================================================================

static void Memory_xchg(uint8_t* addr1, uint8_t* addr2, size_t count)
{
  size_t i;

  for (i = count / (sizeof(size_t)); i; i--)
  {
    Memory::xchg_t<size_t>((size_t*)addr1, (size_t*)addr2);

    addr1 += sizeof(size_t);
    addr2 += sizeof(size_t);
  }

  for (i = count & (sizeof(size_t) - 1); i; i--)
  {
    Memory::xchg_1((void*)addr1, (void*)addr2);

    addr1++;
    addr2++;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemoryOps_init(void)
{
  _api.memory.copy = (Api::Memory_Copy)::memcpy;
  _api.memory.move = (Api::Memory_Move)::memmove;
  _api.memory.zero = (Api::Memory_Zero)Memory_zero;
  _api.memory.set = (Api::Memory_Set)::memset;

  _api.memory.copy_nt = _api.memory.copy;
  _api.memory.zero_nt = _api.memory.zero;
  _api.memory.set_nt = _api.memory.set;

  _api.memory.xchg = (Api::Memory_Xchg)Memory_xchg;
}

} // Fog namespace
