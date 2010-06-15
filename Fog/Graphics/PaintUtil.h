// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTUTIL_H
#define _FOG_GRAPHICS_PAINTUTIL_H

// [Dependencies]
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/Painter.h>

namespace Fog {
namespace PaintUtil {

//! @addtogroup Fog_Graphics_Painting
//! @{

// ============================================================================
// [Fog::PaintUtil]
// ============================================================================

//! @brief Paint stroked 3d-looking rectangle using two colors.
FOG_API void draw3dRect(Painter* p, const IntRect& r, Argb color1, Argb color2, bool checked = false);

//! @}

} // PaintUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_GRAPHICS_PAINTUTIL_H
