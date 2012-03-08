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
// [Fog::OTHmtxHeader]
// ============================================================================

struct FOG_NO_EXPORT OTHmtxHeader
{
};

// ============================================================================
// [Fog::OTHmtx]
// ============================================================================

struct FOG_NO_EXPORT OTHmtx : public OTTable
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const OTHmtxHeader* getHeader() const { return reinterpret_cast<OTHmtxHeader*>(_data); }
};

//! @}

// [Byte-Pack]
#include <Fog/Core/C++/PackRestore.h>

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_OPENTYPE_OTHMTX_H
