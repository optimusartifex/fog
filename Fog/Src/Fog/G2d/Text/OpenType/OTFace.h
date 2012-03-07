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

  //! @brief Get whether the table of @a tag name was loaded and it's accessible.
  FOG_INLINE bool hasTable(uint32_t tag) const
  {
    return getTable(tag) != NULL;
  }

  //! @brief Get whether the table @a param belongs o this @ref OTFace.
  FOG_INLINE bool hasTable(OTTable* param) const
  {
    return fog_ot_api.otface_hasTable(this, param);
  }

  //! @brief Get previously loaded table of @a tag name.
  FOG_INLINE OTTable* getTable(uint32_t tag) const
  {
    return fog_ot_api.otface_getTable(this, tag);
  }

  //! @brief Try to load table of @a tag name.
  //!
  //! In case that this function succeeds the table is returned, in case that
  //! the requested table dosn't exist in font file, value of 0x1 is returned,
  //! and usually stored to a particular object. Please use FOG_OT_LOADED()
  //! macro to get whether the table object is valid.
  FOG_INLINE OTTable* tryLoadTable(uint32_t tag)
  {
    return fog_ot_api.otface_tryLoadTable(this, tag);
  }

  FOG_INLINE OTTable* addTable(uint32_t tag, uint8_t* data, uint32_t length)
  {
    return fog_ot_api.otface_addTable(this, tag, data, length);
  }

  // --------------------------------------------------------------------------
  // [Core Tables]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hasCMap()
  {
    return FOG_OT_LOADED(getCMap());
  }

  FOG_INLINE OTCMapTable* getCMap()
  {
    if (_cmap == NULL)
      _cmap = reinterpret_cast<OTCMapTable*>(tryLoadTable(FOG_OT_TAG('c', 'm', 'a', 'p')));
    return _cmap;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Face which owns this @ref OTFace instance.
  Face* _face;

  //! @brief Loaded tables.
  OTTable* _tableData;
  //! @brief Table-data free callback.
  OTFaceFreeTableDataFunc _freeTableDataFunc;

  //! @brief Fast access to 'cmap' table.
  OTCMapTable* _cmap;

  //! @brief allocaor.
  Static<MemZoneAllocator> _allocator;

private:
  FOG_NO_COPY(OTFace)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OTFACE_H
