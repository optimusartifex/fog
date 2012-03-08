// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_OPENTYPE_OTHEAD_H
#define _FOG_G2D_TEXT_OPENTYPE_OTHEAD_H

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
// [Fog::OTHeadHeader]
// ============================================================================

struct FOG_NO_EXPORT OTHeadHeader
{
  OTFixed16x16 version;
  OTFixed16x16 revision;

  OTUInt32 checkSumAdjustment;
  OTUInt32 magicNumber;
  OTUInt16 flags;
  OTUInt16 unitsPerEm;

  OTDateTime created;
  OTDateTime modified;

  OTInt16 xMin;
  OTInt16 yMin;
  OTInt16 xMax;
  OTInt16 yMax;

  OTUInt16 macStyle;
  OTUInt16 lowestRecPPEM;

  OTInt16 fontDirectionHint;
  OTInt16 indexToLocFormat;
  OTInt16 glyphDataFormat;
};

// ============================================================================
// [Fog::OTHead]
// ============================================================================

struct FOG_NO_EXPORT OTHead : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const OTHeadHeader* getHeader() const { return reinterpret_cast<OTHeadHeader*>(_data); }

  FOG_INLINE uint16_t getUnitsPerEM() const { return _unitsPerEM; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint16_t _unitsPerEM;
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTHEAD_H
