// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

// ============================================================================
// [Fog::Region - HRGB]
// ============================================================================

static FOG_INLINE void RECTFromBoxI(RECT* dst, const BoxI* src)
{
  int left   = src->x0;
  int top    = src->y0;
  int right  = src->x1;
  int bottom = src->y1;

  dst->left   = left  ;
  dst->top    = top   ;
  dst->right  = right ;
  dst->bottom = bottom;
}

static FOG_INLINE void BoxIFromRECT(BoxI* dst, const RECT* src)
{
  int x0 = src->left;
  int y0 = src->top;
  int x1 = src->right;
  int y1 = src->bottom;

  dst->x0 = x0;
  dst->y0 = y0;
  dst->x1 = x1;
  dst->y1 = y1;
}

static err_t FOG_CDECL Region_toHRGN(const Region* self, HRGN* dst)
{
  RegionData* d = self->_d;
  size_t length = d->length;

  if (length >= (UINT_MAX - sizeof(RGNDATAHEADER)) / sizeof(RECT))
  {
    *dst = (HRGN)NULLREGION;
    return ERR_RT_OUT_OF_MEMORY;
  }

  DWORD hrgnDataSize = (DWORD)length * sizeof(RECT);

  MemBufferTmp<512> buffer;
  RGNDATAHEADER *hRegionHeader = reinterpret_cast<RGNDATAHEADER *>(buffer.alloc(sizeof(RGNDATAHEADER) + hrgnDataSize));

  if (FOG_IS_NULL(hRegionHeader))
  {
    *dst = (HRGN)NULLREGION;
    return ERR_RT_OUT_OF_MEMORY;
  }

  hRegionHeader->dwSize = sizeof(RGNDATAHEADER);
  hRegionHeader->iType = RDH_RECTANGLES;
  hRegionHeader->nCount = (DWORD)length;
  hRegionHeader->nRgnSize = hrgnDataSize;

  // Copy rectangles to the region.
  {
    RECT* dstData = reinterpret_cast<RECT *>((uint8_t *)hRegionHeader + sizeof(RGNDATAHEADER));
    const BoxI* srcData = d->data;

    for (size_t i = 0; i < length; i++)
    {
      RECTFromBoxI(&dstData[i], &srcData[i]);
    }
    RECTFromBoxI(&hRegionHeader->rcBound, &d->boundingBox);
  }

  HRGN hRegion = ExtCreateRegion(NULL, (DWORD)sizeof(RGNDATAHEADER) + hrgnDataSize, (RGNDATA *)hRegionHeader);
  if (FOG_IS_NULL(hRegion))
  {
    *dst = (HRGN)NULLREGION;
    return ERR_OK;
  }

  *dst = hRegion;
  return ERR_OK;
}

static err_t FOG_CDECL Region_fromHRGN(Region* self, HRGN src)
{
  self->clear();

  if (src == NULL || src == (HRGN)NULLREGION)
    return ERR_OK;

  DWORD hrgnDataSize = GetRegionData(src, 0, NULL);
  if (hrgnDataSize == 0)
    return ERR_OK;

  MemBufferTmp<512> buffer;
  RGNDATAHEADER* hRegionHeader = reinterpret_cast<RGNDATAHEADER *>(buffer.alloc(hrgnDataSize));

  if (FOG_IS_NULL(hRegionHeader))
    return ERR_RT_OUT_OF_MEMORY;

  if (!GetRegionData(src, hrgnDataSize, (RGNDATA*)hRegionHeader))
    return OSUtil::getErrFromOSLastError();

  FOG_ASSERT(sizeof(RECT) == sizeof(BoxI));

  RECT* srcRect = (RECT*)((uint8_t*)hRegionHeader + sizeof(RGNDATAHEADER));
  BoxI* dstBox = reinterpret_cast<BoxI*>(srcRect);

  size_t length = hRegionHeader->nCount;

  if (FOG_OFFSET_OF(RECT, left  ) != FOG_OFFSET_OF(BoxI, x0) ||
      FOG_OFFSET_OF(RECT, top   ) != FOG_OFFSET_OF(BoxI, y0) ||
      FOG_OFFSET_OF(RECT, right ) != FOG_OFFSET_OF(BoxI, x1) ||
      FOG_OFFSET_OF(RECT, bottom) != FOG_OFFSET_OF(BoxI, y1))
  {
    for (size_t i = 0; i < length; i++)
      BoxIFromRECT(&dstBox[i], &srcRect[i]);
  }

  return self->setBoxList(dstBox, length);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Region_init_win(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.region_toHRGN = Region_toHRGN;
  fog_api.region_fromHRGN = Region_fromHRGN;
}

} // Fog namespace
