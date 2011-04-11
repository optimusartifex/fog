// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_NULL_NULLENGINE_P_H
#define _FOG_G2D_PAINTING_NULL_NULLENGINE_P_H

// [Dependencies]
#include <Fog/G2d/Painting/PainterEngine.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting_Null
//! @{

// ============================================================================
// [Fog::NullPainterVTable]
// ============================================================================

struct FOG_NO_EXPORT NullPainterVTable : public PainterVTable
{
};

// ============================================================================
// [Fog::NullPainterEngine]
// ============================================================================

//! @internal
//!
//! @brief The null painter engine (does nothing).
//!
//! The nearly all methods result in @ref ERR_RT_INVALID_STATE error code.
struct FOG_NO_EXPORT NullPainterEngine : public PainterEngine {};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_NULL_NULLENGINE_P_H
