// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccSse2.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/ContainerUtil.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ===========================================================================
// [Fog::List - Untyped - IndexOf]
// ===========================================================================

template<int Direction>
static size_t FOG_CDECL List_indexOf_4B_SSE2(const ListUntyped* self, const Range* range, const void* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  const uint8_t* p = reinterpret_cast<const uint8_t*>(d->data);
  p += (Direction == 1) ? rStart * 4 : rEnd * 4 - 4;

  uint32_t item0 = reinterpret_cast<const uint32_t*>(item)[0];
  size_t i = rEnd - rStart;

  int msk;

  if (i >= 20 && ((size_t)p & 0x3) == 0)
  {
    const ssize_t alignConst = (Direction == 1) ? 0 :  16 - 4;
    const ssize_t loadOffset = (Direction == 1) ? 0 : -16 + 4;

    __m128i xmmItem;
    Acc::m128iCvtSI128FromSI(xmmItem, item0);
    Acc::m128iExpandPI32FromSI32(xmmItem, xmmItem);

    // Align.
    while (((ssize_t)p & 0x15) != alignConst)
    {
      if (reinterpret_cast<const uint32_t*>(p)[0] == item0)
        goto _Match;

      p += Direction * 4;
      i--;
    }

    while (i >= 16)
    {
      __m128i xmm0, xmm1;
      __m128i xmm2, xmm3;

      Acc::m128iLoad16a(xmm0, p + Direction *  0 + loadOffset);
      Acc::m128iLoad16a(xmm1, p + Direction * 16 + loadOffset);

      Acc::m128iCmpEqPI32(xmm0, xmm0, xmmItem);
      Acc::m128iCmpEqPI32(xmm1, xmm1, xmmItem);

      Acc::m128iMoveMaskPI32(msk, xmm0);
      Acc::m128iLoad16a(xmm2, p + Direction * 32 + loadOffset);
      if (msk != 0) goto _Near;

      Acc::m128iLoad16a(xmm3, p + Direction * 48 + loadOffset);
      Acc::m128iMoveMaskPI32(msk, xmm1);

      p += Direction * 16;
      if (msk != 0) goto _Near;

      Acc::m128iCmpEqPI32(xmm2, xmm2, xmmItem);
      Acc::m128iCmpEqPI32(xmm3, xmm3, xmmItem);

      Acc::m128iMoveMaskPI32(msk, xmm2);

      p += Direction * 16;
      if (msk != 0) goto _Near;

      Acc::m128iMoveMaskPI32(msk, xmm3);

      p += Direction * 16;
      if (msk != 0) goto _Near;

      p += Direction * 16;
      i -= 16;
    }

    if (i == 0)
      return INVALID_INDEX;
  }

  do {
    if (reinterpret_cast<const uint32_t*>(p)[0] == item0)
      goto _Match;
    p += Direction * 4;
  } while (--i);

  return INVALID_INDEX;

_Near:
  if (Direction == 1)
  {
    if ((msk & 0x1) == 0x1) goto _Match;
    p += Direction * 4;

    if ((msk & 0x2) == 0x2) goto _Match;
    p += Direction * 4;

    if ((msk & 0x4) == 0x4) goto _Match;
    p += Direction * 4;
  }
  else
  {
    if ((msk & 0x8) == 0x8) goto _Match;
    p += Direction * 4;

    if ((msk & 0x4) == 0x4) goto _Match;
    p += Direction * 4;

    if ((msk & 0x2) == 0x2) goto _Match;
    p += Direction * 4;
  }

_Match:
  return (size_t)(p - reinterpret_cast<const uint8_t*>(d->data)) / 4;
}

template<int Direction>
static size_t FOG_CDECL List_indexOf_16B_SSE2(const ListUntyped* self, const Range* range, const void* item)
{
  ListUntypedData* d = self->_d;

  size_t length = d->length;
  size_t rStart, rEnd;

  if (!Range::fit(rStart, rEnd, length, range))
    return INVALID_INDEX;

  const uint8_t* p = reinterpret_cast<const uint8_t*>(d->data);
  p += (Direction == 1) ? rStart * 16 : rEnd * 16 - 16;

  __m128i xmmItem;
  Acc::m128iLoad16u(xmmItem, item);

  size_t i = rEnd - rStart;

  if (((size_t)p & 0x15) == 0)
  {
    while (i >= 4)
    {
      __m128i xmm0, xmm1;
      int msk0, msk1;

      Acc::m128iLoad16a(xmm0, p + Direction *  0);
      Acc::m128iLoad16a(xmm1, p + Direction * 16);

      Acc::m128iCmpEqPI8(xmm0, xmm0, xmmItem);
      Acc::m128iCmpEqPI8(xmm1, xmm1, xmmItem);

      Acc::m128iMoveMaskPI8(msk0, xmm0);
      Acc::m128iMoveMaskPI8(msk1, xmm1);

      Acc::m128iLoad16a(xmm0, p + Direction * 32);
      if (msk0 == 0xFFFF) goto _Match0;

      Acc::m128iLoad16a(xmm1, p + Direction * 48);
      if (msk1 == 0xFFFF) goto _Match1;

      Acc::m128iCmpEqPI8(xmm0, xmm0, xmmItem);
      Acc::m128iCmpEqPI8(xmm1, xmm1, xmmItem);

      p += Direction * 32;

      Acc::m128iMoveMaskPI8(msk0, xmm0);
      Acc::m128iMoveMaskPI8(msk1, xmm1);

      if (msk0 == 0xFFFF) goto _Match0;
      if (msk1 == 0xFFFF) goto _Match1;

      p += Direction * 32;
      i -= 4;
    }

    while (i)
    {
      __m128i xmm0;
      int msk0;

      Acc::m128iLoad16a(xmm0, p + Direction *  0);
      Acc::m128iCmpEqPI8(xmm0, xmm0, xmmItem);
      Acc::m128iMoveMaskPI8(msk0, xmm0);
      if (msk0 == 0xFFFF) goto _Match0;

      p += Direction * 16;
      i--;
    }
  }
  else
  {
    while (i >= 4)
    {
      __m128i xmm0, xmm1, xmm2, xmm3;
      int msk0, msk1;

      Acc::m128iLoad16u(xmm0, p + Direction *  0);
      Acc::m128iLoad16u(xmm1, p + Direction * 16);

      Acc::m128iCmpEqPI8(xmm0, xmm0, xmmItem);
      Acc::m128iCmpEqPI8(xmm1, xmm1, xmmItem);

      Acc::m128iLoad16u(xmm2, p + Direction * 32);
      Acc::m128iLoad16u(xmm3, p + Direction * 48);

      Acc::m128iMoveMaskPI8(msk0, xmm0);
      Acc::m128iMoveMaskPI8(msk1, xmm1);

      if (msk0 == 0xFFFF) goto _Match0;
      if (msk1 == 0xFFFF) goto _Match1;

      Acc::m128iCmpEqPI8(xmm2, xmm2, xmmItem);
      Acc::m128iCmpEqPI8(xmm3, xmm3, xmmItem);

      p += Direction * 32;

      Acc::m128iMoveMaskPI8(msk0, xmm2);
      Acc::m128iMoveMaskPI8(msk1, xmm3);

      if (msk0 == 0xFFFF) goto _Match0;
      if (msk1 == 0xFFFF) goto _Match1;

      p += Direction * 32;
      i -= 4;
    }

    while (i)
    {
      __m128i xmm0;
      int msk0;

      Acc::m128iLoad16u(xmm0, p);
      Acc::m128iCmpEqPI8(xmm0, xmm0, xmmItem);
      Acc::m128iMoveMaskPI8(msk0, xmm0);
      if (msk0 == 0xFFFF) goto _Match0;

      p += Direction * 16;
      i--;
    }
  }

  return INVALID_INDEX;

_Match1:
  p += Direction * 16;

_Match0:
  return (size_t)(p - reinterpret_cast<const uint8_t*>(d->data)) / 16;
}

// ===========================================================================
// [Init / Fini]
// ===========================================================================

FOG_NO_EXPORT void List_init_SSE2(void)
{
  // -------------------------------------------------------------------------
  // [Funcs]
  // -------------------------------------------------------------------------

  fog_api.list_untyped_indexOf_4B = List_indexOf_4B_SSE2<1>;
  fog_api.list_untyped_indexOf_16B = List_indexOf_16B_SSE2<1>;

  fog_api.list_untyped_lastIndexOf_4B = List_indexOf_4B_SSE2<-1>;
  fog_api.list_untyped_lastIndexOf_16B = List_indexOf_16B_SSE2<-1>;
}

} // Fog namespace
