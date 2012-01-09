// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_NULLPAINTENGINE_P_H
#define _FOG_G2D_PAINTING_NULLPAINTENGINE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/PaintEngine.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::NullPaintEngine]
// ============================================================================

//! @internal
//!
//! @brief The null paint-engine (does nothing).
//!
//! The nearly all methods result in @ref ERR_RT_INVALID_STATE error code.
struct FOG_NO_EXPORT NullPaintEngine : public PaintEngine
{
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_NULLPAINTENGINE_P_H
