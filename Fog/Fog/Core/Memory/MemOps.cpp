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
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

// ============================================================================
// [Fog::Memory - Copy]
// ============================================================================

static void* FOG_CDECL MemMgr_zero(void* dst, size_t size)
{
  return memset(dst, 0, size);
}

// ============================================================================
// [Fog::Memory - Xchg]
// ============================================================================

static void MemMgr_xchg(uint8_t* addr1, uint8_t* addr2, size_t count)
{
  size_t i;

  for (i = count / (sizeof(size_t)); i; i--)
  {
    MemOps::xchg_t<size_t>((size_t*)addr1, (size_t*)addr2);

    addr1 += sizeof(size_t);
    addr2 += sizeof(size_t);
  }

  for (i = count & (sizeof(size_t) - 1); i; i--)
  {
    MemOps::xchg_1((void*)addr1, (void*)addr2);

    addr1++;
    addr2++;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemOps_init(void)
{
  _api.memops.copy = (Api::MemOps_Copy)::memcpy;
  _api.memops.move = (Api::MemOps_Move)::memmove;
  _api.memops.zero = (Api::MemOps_Zero)MemMgr_zero;
  _api.memops.set = (Api::MemOps_Set)::memset;

  _api.memops.copy_nt = _api.memops.copy;
  _api.memops.zero_nt = _api.memops.zero;
  _api.memops.set_nt = _api.memops.set;

  _api.memops.xchg = (Api::MemOps_Xchg)MemMgr_xchg;
}

} // Fog namespace
