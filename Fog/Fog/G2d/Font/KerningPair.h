// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_FONT_KERNINGPAIR_H
#define _FOG_G2D_FONT_KERNINGPAIR_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Font
//! @{

// ============================================================================
// [Fog::KerningPairI]
// ============================================================================

//! @brief Kerning pair used if font uses quantized metrics.
#include <Fog/Core/Pack/PackByte.h>
struct KerningPairI
{
  uint16_t first;
  uint16_t second;
  int amount;
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::KerningPairF]
// ============================================================================

//! @brief Kerning pair used if font uses non-quantized floating point metrics.
//!
//! Floating kerning pairs are shared for all font sizes. To get demanded
//! value use Font::getKerningScale() value.
#include <Fog/Core/Pack/PackByte.h>
struct KerningPairF
{
  uint16_t first;
  uint16_t second;
  float amount;
};
#include <Fog/Core/Pack/PackRestore.h>

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::KerningPairI, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::KerningPairF, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_FONT_KERNINGPAIR_H
