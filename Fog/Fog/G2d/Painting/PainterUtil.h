// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_PAINTERUTIL_H
#define _FOG_G2D_PAINTING_PAINTERUTIL_H

// [Dependencies]
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {
namespace PainterUtil {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::PaintUtil]
// ============================================================================

//! @brief Paint stroked 3d-looking rectangle using two colors.
FOG_API void draw3dRect(Painter* p, const RectI& r, Argb32 color1, Argb32 color2, bool checked = false);

//! @}

} // PainterUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_PAINTERUTIL_H
