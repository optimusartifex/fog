// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTHHEA_H
#define _FOG_G2D_TEXT_OPENTYPE_OTHHEA_H

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
// [Fog::OTHHeaHeader]
// ============================================================================

//! @brief TrueType/OpenType 'hhea' - Horizontal header (header).
struct FOG_NO_EXPORT OTHHeaHeader
{
  //! @brief Version number in fixed point (0x00010000 for 1.0).
  OTFixed16x16 version;
  //! @brief Typographic ascent.
  OTInt16 ascender;
  //! @brief Typographic descent.
  OTInt16 descender;
  //! @brief Typographic line-gap.
  OTInt16 lineGap;
  //! @brief Maximum advance width value in 'htmx' table.
  OTUInt16 advanceWidthMax;
  //! @brief Minimum left sidebearing value in 'htmx' table.
  OTInt16 minLeftSideBearing;
  //! @brief Minimum right sidebearing value in 'htmx' table.
  //!
  //! @note Calculated as "min(aw - lsb - (xMax - xMin))".
  OTInt16 minRightSideBearing;
  //! @brief Maximum x extent.
  //!
  //! @note Calculated as "max(lsb + (xMax - xMin))".
  OTInt16 xMaxExtent;
  //! @brief Used to calculate the slope of the cursor (rise).
  OTInt16 caretSlopeRise;
  //! @brief Used to calculate the slope of the cursor (run).
  OTInt16 caretSlopeRun;
  //! @brief The amount by which a slanted highlight on a glyph needs to be 
  //! shifted to produce the best appearance. Set to 0 for non-slanted fonts.
  OTInt16 caretOffset;
  //! @brief Reserved, should be zero.
  OTInt16 reserved[4];
  //! @brief Metric data format (should be zero).
  OTInt16 metricDataFormat;
  //! @brief Number of hMetric entries in 'hmtx' table.
  OTUInt16 numberOfHMetrics;
};

// ============================================================================
// [Fog::OTHHea]
// ============================================================================

//! @brief TrueType/OpenType 'hhea' - Horizontal header table.
struct FOG_NO_EXPORT OTHHea : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const OTHHeaHeader* getHeader() const { return reinterpret_cast<OTHHeaHeader*>(_data); }
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTHHEA_H
