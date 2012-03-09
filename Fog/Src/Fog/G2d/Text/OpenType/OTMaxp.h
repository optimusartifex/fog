// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTMAXP_H
#define _FOG_G2D_TEXT_OPENTYPE_OTMAXP_H

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
// [Fog::OTMaxpHeaderV0_5]
// ============================================================================

//! @brief TrueType/OpenType 'maxp' - Maximum Profile header, version 0.5.
struct FOG_NO_EXPORT OTMaxpHeaderV0_5
{
  //! @brief Version number in fixed point (0x00005000 for 0.5).
  OTFixed16x16 version;
  //! @brief The number of glyphs in the font.
  OTUInt16 numGlyphs;
};

// ============================================================================
// [Fog::OTMaxpHeaderV1_0]
// ============================================================================

//! @brief TrueType/OpenType 'maxp' - Maximum Profile header, version 1.0.
struct FOG_NO_EXPORT OTMaxpHeaderV1_0
{
  //! @brief Version number in fixed point (0x00010000 for 1.0).
  OTFixed16x16 version;
  //! @brief The number of glyphs in the font.
  OTUInt16 numGlyphs;
  //! @brief Maximum points in a non-composite glyph.
  OTUInt16 maxPoints;
  //! @brief Maximum contours in a non-composite glyph.
  OTUInt16 maxContours;
  //! @brief Maximum points in a composite glyph.
  OTUInt16 maxComponentPoints;
  //! @brief Maximum contours in a composite glyph.
  OTUInt16 maxComponentContours;
  //! @brief 1 if instructions do not use the twilight zone (Z0), or 2 if 
  //! instructions do use Z0; should be set to 2 in most cases.
  OTUInt16 maxZones;
  //! Maximum points used in Z0.
  OTUInt16 maxTwilightPoints;
  //! @brief Number of Storage Area locations.
  OTUInt16 maxStorage;
  //! @brief Number of FDEFs.
  OTUInt16 maxFunctionDefs;
  //! @brief Number of IDEFs.
  OTUInt16 maxInstructionDefs;
  //! @brief Maximum stack depth.
  //!
  //! @note This includes Font and CVT Programs, as well as the instructions
  //! for each glyph.
  OTUInt16 maxStackElements;
  //! @brief Maximum byte count for glyph instructions.
  OTUInt16 maxSizeOfInstructions;
  //! @brief Maximum number of components referenced at "top level" for any 
  //! composite glyph.
  OTUInt16 maxComponentElements;
  //! @brief Maximum levels of recursion; 1 for simple components.
  OTUInt16 maxComponentDepth;
};

// ============================================================================
// [Fog::OTMaxp]
// ============================================================================

//! @brief TrueType/OpenType 'maxp' - Maximum Profile table.
//!
//! Specification:
//!   - http://www.microsoft.com/typography/otspec/maxp.htm
//!   - https://developer.apple.com/fonts/ttrefman/RM06/Chap6maxp.html
struct FOG_NO_EXPORT OTMaxp : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get version of TrueType/OpenType 'maxp' table.
  FOG_INLINE uint32_t getVersion() const
  {
    return reinterpret_cast<const OTFixed16x16*>(getData())->getValueA();
  }

  //! @brief Get TrueType/OpenType 'maxp' table version 0.5.
  FOG_INLINE const OTMaxpHeaderV0_5* getHeaderV0_5() const
  {
    FOG_ASSERT(getVersion() >= 0x00005000);
    return reinterpret_cast<OTMaxpHeaderV0_5*>(_data);
  }

  //! @brief Get TrueType/OpenType 'maxp' table version 1.0.
  FOG_INLINE const OTMaxpHeaderV1_0* getHeaderV1_0() const
  {
    FOG_ASSERT(getVersion() >= 0x00010000);
    return reinterpret_cast<OTMaxpHeaderV1_0*>(_data);
  }
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTMAXP_H
