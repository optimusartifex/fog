// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTHMTX_H
#define _FOG_G2D_TEXT_OPENTYPE_OTHMTX_H

// [Dependencies]
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/G2d/Text/OpenType/OTApi.h>
#include <Fog/G2d/Text/OpenType/OTTypes.h>

namespace Fog {

// [Byte-Pack]
#include <Fog/Core/C++/PackByte.h>

//! @addtogroup Fog_G2d_Text_OpenType
//! @{

// ============================================================================
// [Fog::OTHmtxMetric]
// ============================================================================

//! @brief TrueType/OpenType 'hmtx' - Horizontal metrics record.
struct FOG_NO_EXPORT OTHmtxMetric
{
  //! @brief Glyph advance, in font design units.
  OTUInt16 advanceWidth;
  //! @brief Left side bearing, in font design units.
  OTInt16 leftSideBearing;
};

// ============================================================================
// [Fog::OTHmtxHeader]
// ============================================================================

//! @brief TrueType/OpenType 'hmtx' - Horizontal metrics header.
struct FOG_NO_EXPORT OTHmtxHeader
{
  //! @brief Array of @ref OTHmtxMetric records.
  //!
  //! @note Length of this array in specification is mentioned as 
  //! @c numberOfHMetrics. We declared it as [1] array, so we can use
  //! indexed access here.
  OTHmtxMetric hMetrics[1];
};

// ============================================================================
// [Fog::OTHmtx]
// ============================================================================

//! @brief TrueType/OpenType 'hmtx' - Horizontal metrics table.
//!
//! The 'hmtx' table contains metric information for the horizontal layout each
//! of the glyphs in the font. It begins with the hMetrics array. Each element
//! in this array has two parts: the advance width and left side bearing. The 
//! value numberOfHMetrics is taken from the 'hhea' (Horizontal Header) table.
//! In a monospaced font, only one entry is required but that entry may not be
//! omitted.
//! 
//! Optionally, an array of left side bearings follows. Generally, this array of
//! left side bearings is used for a run of monospaced glyphs. For example, it
//! might be used for a Kanji font or for Courier. Only one such run is allowed
//! per font. It must be at the end of the table.The corresponding glyphs are 
//! assumed to have the same advance width as that found in the last entry in 
//! the hMetrics array. Since there must be a left side bearing and an advance 
//! width associated with each glyph in the font, the number of entries in this
//! array is derived from the total number of glyphs in the font minus the value
//! numberOfHMetrics.
//!
//! Specification:
//!   - http://www.microsoft.com/typography/otspec/hmtx.htm
//!   - https://developer.apple.com/fonts/ttrefman/RM06/Chap6hmtx.html
struct FOG_NO_EXPORT OTHmtx : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const OTHmtxHeader* getHeader() const
  {
    return reinterpret_cast<const OTHmtxHeader*>(_data);
  }

  FOG_INLINE const OTHmtxMetric* getHMetrics() const
  {
    return reinterpret_cast<const OTHmtxMetric*>(_data);
  }

  FOG_INLINE const OTInt16* getLeftSideBearing() const
  {
    return reinterpret_cast<const OTInt16*>(_data + _numberOfHMetrics * sizeof(OTHmtxMetric));
  }

  FOG_INLINE uint32_t getNumberOfHMetrics() const { return _numberOfHMetrics; }
  FOG_INLINE uint32_t getNumberOfLeftSideBearing() const { return _numberOfLeftSideBearing; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Number of horizontal metrics which can be retrieved by @c getHMetrics().
  uint32_t _numberOfHMetrics;
  //! @brief Number of left side bearing which can be retrieved by @c getLeftSideBearing().
  uint32_t _numberOfLeftSideBearing;
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTHMTX_H
