// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_PAINTERUTIL_H
#define _FOG_GRAPHICS_PAINTERUTIL_H

// [Dependencies]
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Painter.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {
namespace PaintUtil {

// ============================================================================
// [Fog::PaintUtil]
// ============================================================================

//! @brief Paint stroked 3d-looking rectangle using two colors.
FOG_API void draw3dRect(Painter* p, const IntRect& r, Argb color1, Argb color2, bool checked = false);

} // PaintUtil namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_PAINTERUTIL_H
