// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERCONTEXT_P_H
#define _FOG_G2D_PAINTING_RASTERCONTEXT_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemBuffer.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageFilterScale.h>
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/RasterPaintStructs_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterStructs_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterContext]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT RasterContext
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RasterContext();
  ~RasterContext();

  // --------------------------------------------------------------------------
  // [Init / Reset]
  // --------------------------------------------------------------------------

  err_t _initByMaster(const RasterContext& master);
  err_t _initPrecision(uint32_t precision);

  void _reset();

  // --------------------------------------------------------------------------
  // [Multithreading]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isSingleThreaded() const { return scope.isSingleThreaded(); }
  FOG_INLINE bool isMultiThreaded() const { return scope.isMultiThreaded(); }

  // --------------------------------------------------------------------------
  // [Mask]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Members - Engine]
  // --------------------------------------------------------------------------

  //! @brief Owner of this context.
  RasterPaintEngine* engine;

  //! @brief Context scope (used by multithreaded paint-engine)
  RasterScope scope;

  // --------------------------------------------------------------------------
  // [Members - Precision]
  // --------------------------------------------------------------------------

  //! @brief Context precision (see @c IMAGE_PRECISION).
  uint32_t precision;

  struct FOG_NO_EXPORT _FullOpacity
  {
    //! @brief Full opacity (0x100 or 0x10000). Depends on @c IMAGE_PRECISION.
    uint32_t u;
    //! @brief Full opacity for float conversion. Depends on @c IMAGE_PRECISION.
    float f;
  } fullOpacity;

  // --------------------------------------------------------------------------
  // [Members - Filter]
  // --------------------------------------------------------------------------

  ImageFilterScaleD filterScale;

  // --------------------------------------------------------------------------
  // [Members - Layer]
  // --------------------------------------------------------------------------

  //! @brief Layer.
  RasterLayer layer;

  // --------------------------------------------------------------------------
  // [Members - Render]
  // --------------------------------------------------------------------------

  //! @brief Paint hints.
  PaintHints paintHints;
  //! @brief Raster hints.
  RasterHints rasterHints;

  //! @brief Solid source.
  RasterSolid solid;
  //! @brief Pattern context.
  //!
  //! Applicable if source type is @c PATTERN_TYPE_TEXTURE or @c PATTERN_TYPE_GRADIENT.
  RasterPattern* pc;
  //! @brief Render closure.
  RasterClosure closure;

  union
  {
    //! @brief The box analytic rasterizer (8-bit).
    Static<BoxRasterizer8> boxRasterizer8;

    // TODO: 16-bit image processing.
    // //! @brief The box analytic rasterizer (16-bit).
    // Static<BoxRasterizer16> boxRasterizer16;
  };

  union
  {
    //! @brief The path/polygon analytic rasterizer (8-bit).
    Static<PathRasterizer8> pathRasterizer8;

    // TODO: 16-bit image processing.
    // //! @brief The analytic rasterizer (16-bit).
    // Static<PathRasterizer16> pathRasterizer16;
  };

  union
  {
    //! @brief The scanline container (8-bit).
    Static<RasterScanline8> scanline8;

    // TODO: 16-bit image processing.
    // //! @brief The scanline container (16-bit).
    // Static<RasterScanline16> scanline16;
  };

  // --------------------------------------------------------------------------
  // [Members - Clip]
  // --------------------------------------------------------------------------

  //! @brief Type of clipping, see @c RASTER_CLIP.
  uint32_t clipType;
  //! @brief Clip region.
  Region clipRegion;
  //! @brief Clip box (integer).
  BoxI clipBoxI;

  // --------------------------------------------------------------------------
  // [Members - Temp]
  // --------------------------------------------------------------------------

  //! @brief Temporary memory buffer.
  MemBuffer buffer;

  //! @brief Temporary path per context, used by calculations (float).
  PathF tmpPathF[3];

  //! @brief Temporary path per context, used by calculations (double).
  PathD tmpPathD[3];

private:
  _FOG_NO_COPY(RasterContext)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERCONTEXT_P_H
