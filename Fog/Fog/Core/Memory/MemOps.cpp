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
// [Fog::MemOps - Zero]
// ============================================================================

static void* FOG_CDECL MemOps_zero(void* dst, size_t size)
{
  return memset(dst, 0, size);
}

// ============================================================================
// [Fog::MemOps - Eq]
// ============================================================================

static bool FOG_CDECL MemOps_eq(const uint8_t* a, const uint8_t* b, size_t size)
{
  for (size_t i = 0; i < size; i++)
    if (a[i] != b[i])
      return false;

  return true;
}

// ============================================================================
// [Fog::MemOps - Xchg]
// ============================================================================

static void MemOps_xchg(uint8_t* a, uint8_t* b, size_t count)
{
  size_t i;

  for (i = count / (sizeof(size_t)); i; i--)
  {
    MemOps::xchg_t<size_t>((size_t*)a, (size_t*)b);

    a += sizeof(size_t);
    b += sizeof(size_t);
  }

  for (i = count & (sizeof(size_t) - 1); i; i--)
  {
    MemOps::xchg_1((void*)a, (void*)b);

    a++;
    b++;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemOps_init(void)
{
  _api.memops.copy = (Api::MemOps_Copy)::memcpy;
  _api.memops.move = (Api::MemOps_Move)::memmove;
  _api.memops.zero = (Api::MemOps_Zero)MemOps_zero;
  _api.memops.set = (Api::MemOps_Set)::memset;

  _api.memops.copy_nt = _api.memops.copy;
  _api.memops.zero_nt = _api.memops.zero;
  _api.memops.set_nt = _api.memops.set;

  _api.memops.xchg = (Api::MemOps_Xchg)MemOps_xchg;
  _api.memops.eq = (Api::MemOps_Eq)MemOps_eq;
}

} // Fog namespace
