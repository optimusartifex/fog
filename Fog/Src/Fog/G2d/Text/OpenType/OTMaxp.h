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
// [Fog::OTMaxpHeader]
// ============================================================================

struct FOG_NO_EXPORT OTMaxpHeader
{
  //! @brief Version number in fixed point (0x00010000 for 1.0).
  OTFixed16x16 version;
};

// ============================================================================
// [Fog::OTMaxp]
// ============================================================================

struct FOG_NO_EXPORT OTMaxp : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const OTMaxpHeader* getHeader() const { return reinterpret_cast<OTMaxpHeader*>(_data); }
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTMAXP_H
