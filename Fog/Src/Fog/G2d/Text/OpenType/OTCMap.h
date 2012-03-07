// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OTCMAP_H
#define _FOG_G2D_TEXT_OTCMAP_H

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

//! @brief 'cmap' index table.
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

struct FOG_NO_EXPORT OTCMapEncoding
{
  //! @brief Platform identifier.
  OTUInt16 platformID;
  //! @brief Plaform-specific encoding identifier.
  OTUInt16 specificID;
  //! @brief Offset of the mapping table.
  OTUInt32 offset;
};

// ============================================================================
// [Fog::OTCMap...]
// ============================================================================


// ============================================================================
// [Fog::OTCMapTable]
// ============================================================================

struct FOG_NO_EXPORT OTCMapTable : public OTTable
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Get character placement.
  OTCMapInitContextFunc _initContext;

  // size_t _recordCount;
  // OTCMapRecord* _recordList;
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

  FOG_INLINE err_t init(OTCMapTable* cMapTable, uint32_t language)
  {
    FOG_ASSERT_X(cMapTable != NULL,
      "Fog::OTCMapContext::init() - Called with NULL cMapTable.");
    FOG_ASSERT_X(cMapTable->_initContext != NULL,
      "Fog::OTCMapContext::init() - Called with unsupported cMapTable.");

    return cMapTable->_initContext(this, cMapTable, language);
  }

  FOG_INLINE size_t getGlyphPlacement(uint32_t* glyphId, const uint16_t* str, size_t length)
  {
    FOG_ASSERT_X(_getGlyphPlacementFunc != NULL,
      "Fog::OTCmapContext::getGlyphPlacement() - Called on uninitialized context");

    return _getGlyphPlacementFunc(this, glyphId, str, length);
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
#endif // _FOG_G2D_TEXT_OTCMAP_H
