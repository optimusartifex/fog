// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTCMAP_H
#define _FOG_G2D_TEXT_OPENTYPE_OTCMAP_H

// [Dependencies]
#include <Fog/G2d/Text/OpenType/OTApi.h>
#include <Fog/G2d/Text/OpenType/OTTypes.h>

namespace Fog {

// [Byte-Pack]
#include <Fog/Core/C++/PackByte.h>

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Fog::OTCMapHeader]
// ============================================================================

//! @brief TrueType/OpenType 'cmap' - Character to glyph mapping header.
struct FOG_NO_EXPORT OTCMapHeader
{
  //! @brief Version of 'cmap' header, must be 0.
  OTUInt16 version;
  //! @brief Count of 'cmap' (sub)tables.
  OTUInt16 count;
};

// ============================================================================
// [Fog::OTCMapEncoding]
// ============================================================================

//! @brief TrueType/OpenType 'cmap' - Character to glyph mapping encoding.
struct FOG_NO_EXPORT OTCMapEncoding
{
  //! @brief Platform identifier.
  OTUInt16 platformId;
  //! @brief Plaform-specific encoding identifier.
  OTUInt16 specificId;
  //! @brief Offset of the mapping table.
  OTUInt32 offset;
};

// ============================================================================
// [Fog::OTCMapItem]
// ============================================================================

//! @brief TrueType/OpenType 'cmap' - Character to glyph mapping parsed item,
//! internally by Fog-Framework.
struct FOG_NO_EXPORT OTCMapItem
{
  uint32_t encodingId;
  uint32_t priority;
  err_t status;
};

// ============================================================================
// [Fog::OTCMap]
// ============================================================================

//! @brief TrueType/OpenType 'cmap' - Character to glyph mapping table.
struct FOG_NO_EXPORT OTCMap : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const OTCMapHeader* getHeader() const { return reinterpret_cast<OTCMapHeader*>(_data); }

  FOG_INLINE uint32_t getUnicodeEncodingIndex() const { return _unicodeEncodingIndex; }

  FOG_INLINE uint32_t getCount() const { return _count; }
  FOG_INLINE OTCMapItem* getItems() const { return _items; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _unicodeEncodingIndex;
  uint32_t _count;
  OTCMapItem* _items;

  //! @brief Get character placement.
  OTCMapInitContextFunc _initContext;
};

// ============================================================================
// [Fog::OTCMapContext]
// ============================================================================

struct FOG_NO_EXPORT OTCMapContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE OTCMapContext()
  {
    _getGlyphPlacementFunc = NULL;
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t init(OTCMap* cmap, uint32_t encodingId)
  {
    FOG_ASSERT_X(cmap != NULL,
      "Fog::OTCMapContext::init() - Called on NULL cmap.");
    FOG_ASSERT_X(cmap->_initContext != NULL,
      "Fog::OTCMapContext::init() - Called on unsupported cmap.");

    return cmap->_initContext(this, cmap, encodingId);
  }

  FOG_INLINE size_t getGlyphPlacement(uint32_t* glyphId, size_t glyphAdvance, const uint16_t* sData, size_t sLength)
  {
    FOG_ASSERT_X(_getGlyphPlacementFunc != NULL,
      "Fog::OTCmapContext::getGlyphPlacement() - Called on uninitialized context");

    return _getGlyphPlacementFunc(this, glyphId, glyphAdvance, sData, sLength);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Pointer to 'getGlyphPlacement' implementation.
  OTCMapGetGlyphPlacementFunc _getGlyphPlacementFunc;
  //! @brief Pointer to data used by 'getGlyphPlacement' implementation.
  void* _data;
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTCMAP_H
