// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Build.h>

// TODO: Move this check somewhere else, maybe into Build.h?
#if (defined(FOG_CC_IDE)) || (defined(FOG_CC_GNU) && FOG_CC_GNU >= 4) || (defined(FOG_CC_MSVC) && FOG_CC_MSVC >= 1500)

#include <Fog/Core/Intrin/IntrinSSE2.h>
#include <Fog/Core/Intrin/IntrinSSE3.h>
#include <Fog/Core/Intrin/IntrinSSSE3.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Misc.h>
#include <Fog/Face/FaceByte.h>
#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/ColorMatrix.h>
#include <Fog/Graphics/ColorUtil.h>
#include <Fog/Graphics/Constants_p.h>
#include <Fog/Graphics/DitherMatrix_p.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/ImageFilterEngine.h>
#include <Fog/Graphics/ImageFilterParams.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/RasterUtil_p.h>
#include <Fog/Graphics/Span_p.h>

// [Fog::RasterEngine::SSSE3]
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Defs_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Mask_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Filters_SSSE3_p.h>

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE void fog_rasterengine_init_ssse3(void)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  RasterFuncs& f = rasterFuncs;

  // --------------------------------------------------------------------------
  // [Composite]
  // --------------------------------------------------------------------------

  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_PRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSSE3::prgb32_vblit_prgb32_full;
  f.composite[OPERATOR_SRC_OVER][IMAGE_FORMAT_XRGB32].vblit_full[IMAGE_FORMAT_PRGB32] = CompositeSrcOverSSSE3::prgb32_vblit_prgb32_full;
}

#else

FOG_INIT_DECLARE void fog_rasterengine_init_ssse3(void)
{
  // Nothing
}

#endif
