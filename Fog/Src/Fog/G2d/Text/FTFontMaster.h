// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TEXT_FTFONTMASTER_H
#define _FOG_G2D_TEXT_FTFONTMASTER_H

// [Dependencies]
#include <Fog/G2d/Text/FTFontFace.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::FTFontMaster]
// ============================================================================

struct FOG_API FTFontMaster
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FTFontMaster();
  virtual ~FTFontMaster();

private:
  _FOG_CLASS_NO_COPY(FTFontMaster)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TEXT_FTFONTMASTER_H
