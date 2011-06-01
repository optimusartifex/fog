// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_TEXTEXTENTS_H
#define _FOG_G2D_TEXT_TEXTEXTENTS_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::TextExtentsI]
// ============================================================================

struct TextExtentsI
{
  int begin;
  int advance;
  int end;
};

// ============================================================================
// [Fog::TextExtentsF]
// ============================================================================

struct TextExtentsF
{
  float begin;
  float advance;
  float end;
};

// ============================================================================
// [Fog::TextExtentsD]
// ============================================================================

struct TextExtentsD
{
  double begin;
  double advance;
  double end;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::TextExtentsI, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::TextExtentsF, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_TEXT_TEXTEXTENTS_H
