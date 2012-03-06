// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OTFACE_H
#define _FOG_G2D_TEXT_OTFACE_H

// [Dependencies]
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/G2d/Text/OpenType/OTApi.h>
#include <Fog/G2d/Text/OpenType/OTTypes.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Fog::OTFace]
// ============================================================================

struct FOG_NO_EXPORT OTFace
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE OTFace()
  {
    fog_ot_api.otface_ctor(this);
  }

  FOG_INLINE ~OTFace()
  {
    fog_ot_api.otface_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Tables]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasTable(uint32_t tag) const
  {
    return getTable(tag) != NULL;
  }

  FOG_INLINE bool hasTable(OTTable* param) const
  {
    return fog_ot_api.otface_hasTable(this, param);
  }

  FOG_INLINE OTTable* getTable(uint32_t tag) const
  {
    return fog_ot_api.otface_getTable(this, tag);
  }

  FOG_INLINE OTTable* addTable(uint32_t tag, uint8_t* data, uint32_t length)
  {
    return fog_ot_api.otface_addTable(this, tag, data, length);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<MemZoneAllocator> _allocator;

  OTTable* _tableData;
  OTFaceFreeTableDataFunc _freeTableDataFunc;

private:
  FOG_NO_COPY(OTFace)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OTFACE_H
