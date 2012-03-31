// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/G2d/Painting/Painter.h>

namespace Fog {

FOG_NO_EXPORT void NullPaintEngine_init(void);
FOG_NO_EXPORT void RasterPaintEngine_init(void);

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Painter_init(void)
{
  NullPaintEngine_init();
  RasterPaintEngine_init();
}

} // Fog namespace
