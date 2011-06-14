// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Memory/Ops.h>

namespace Fog {

// ============================================================================
// [Fog::Memory - Copy]
// ============================================================================

static void* FOG_CDECL _Memory_zero(void* dst, size_t size)
{
  return memset(dst, 0, size);
}

// ============================================================================
// [Fog::Memory - Xchg]
// ============================================================================

static void _Memory_xchg(uint8_t* addr1, uint8_t* addr2, size_t count)
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
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_memory_init_ops(void)
{
  _core.memory.copy = (CoreApi::Memory_Copy)::memcpy;
  _core.memory.move = (CoreApi::Memory_Move)::memmove;
  _core.memory.zero = (CoreApi::Memory_Zero)_Memory_zero;
  _core.memory.set = (CoreApi::Memory_Set)::memset;

  _core.memory.copy_nt = _core.memory.copy;
  _core.memory.zero_nt = _core.memory.zero;
  _core.memory.set_nt = _core.memory.set;

  _core.memory.xchg = (CoreApi::Memory_Xchg)_Memory_xchg;
}

} // Fog namespace
