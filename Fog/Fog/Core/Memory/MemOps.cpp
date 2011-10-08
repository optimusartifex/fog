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
// [Fog::MemOps - Equality]
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

typedef void* (FOG_CDECL *MemOps_Copy)(void* dst, const void* src, size_t size);
typedef void* (FOG_CDECL *MemOps_Move)(void* dst, const void* src, size_t size);
typedef void* (FOG_CDECL *MemOps_Zero)(void* dst, size_t size);
typedef void* (FOG_CDECL *MemOps_Set)(void* dst, uint val, size_t size);

typedef void (FOG_CDECL *MemOps_Xchg)(void* a, void* b, size_t size);
typedef void* (FOG_CDECL *MemOps_Eq)(const void* a, const void* b, size_t size);

FOG_NO_EXPORT void MemOps_init(void)
{
  _api.memops_copy = (MemOps_Copy)::memcpy;
  _api.memops_move = (MemOps_Move)::memmove;
  _api.memops_zero = (MemOps_Zero)MemOps_zero;
  _api.memops_set = (MemOps_Set)::memset;

  _api.memops_copynt = _api.memops_copy;
  _api.memops_zeront = _api.memops_zero;
  _api.memops_setnt = _api.memops_set;

  _api.memops_xchg = (MemOps_Xchg)MemOps_xchg;
  _api.memops_eq = (MemOps_Eq)MemOps_eq;
}

} // Fog namespace
