// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Build/Build.h>

#if (defined(FOG_CC_IDE)) || (defined(FOG_CC_GNU) && FOG_CC_GNU >= 4) || (defined(FOG_CC_MSVC) && FOG_CC_MSVC >= 1500)

#include <Fog/Core/Intrin_SSE2.h>
#include <Fog/Core/Intrin_SSE3.h>
#include <Fog/Core/Intrin_SSSE3.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Misc.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/ArgbUtil.h>
#include <Fog/Graphics/ByteUtil_p.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/ColorMatrix.h>
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

// [Raster_SSE2]
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Defs_SSE2_p.h>
#include <Fog/Graphics/RasterEngine/Defs_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Composite_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Interpolate_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Pattern_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Scale_SSSE3_p.h>
#include <Fog/Graphics/RasterEngine/Filters_SSSE3_p.h>

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE void fog_rasterengine_init_ssse3(void)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  // [Constants]

  FunctionMap* m = functionMap;

  // [Composite]

  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcOverSSSE3::prgb32_vspan_prgb32;
  m->composite[OPERATOR_SRC_OVER][PIXEL_FORMAT_XRGB32].vspan[PIXEL_FORMAT_PRGB32] = CompositeSrcOverSSSE3::prgb32_vspan_prgb32;
}

#else

FOG_INIT_DECLARE void fog_rasterengine_init_ssse3(void)
{
  // Nothing
}

#endif
