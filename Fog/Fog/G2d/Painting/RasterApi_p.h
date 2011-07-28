// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERAPI_P_H
#define _FOG_G2D_PAINTING_RASTERAPI_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct MemoryBuffer;

struct BoxRasterizer8;
struct BoxRasterizer16;

struct PathRasterizer8;
struct PathRasterizer16;

struct RasterFiller;

struct RasterScanline8;
struct RasterScanline16;

struct RasterSpan;
struct RasterSpan8;
struct RasterSpan16;

// ============================================================================
// [Fog::RasterApi]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterApi
{
  // --------------------------------------------------------------------------
  // [BoxRasterizer8]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *BoxRasterizer8_InitAligned)(BoxRasterizer8* self, const BoxI* box);
  typedef void (FOG_CDECL *BoxRasterizer8_Init24x8)(BoxRasterizer8* self, const BoxI* box24x8);

  struct FOG_NO_EXPORT _FuncsBoxRasterizer8
  {
    BoxRasterizer8_InitAligned initAligned;
    BoxRasterizer8_Init24x8 init24x8;
  } boxrasterizer8;
};

extern FOG_API RasterApi _rasterApi;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERAPI_P_H
