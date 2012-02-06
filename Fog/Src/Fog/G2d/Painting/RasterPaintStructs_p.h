// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

// TODO: Remove?
struct RasterCalc;
struct RasterCmd;

struct RasterContext;
union RasterHints;
struct RasterLayer;
struct RasterPaintEngine;
struct RasterScope;

// ============================================================================
// [Fog::RasterAbstractLinkedList]
// ============================================================================

struct RasterAbstractLinkedList
{
  RasterAbstractLinkedList* next;
};

// ============================================================================
// [Fog::RasterHints]
// ============================================================================

//! @internal
//!
//! @brief Raster hints.
union FOG_NO_EXPORT RasterHints
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    packed = 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct
  {
    //! @brief Opacity.
    uint32_t opacity : 24;

    //! @brief Whether the final transformation matrix guarantees rect-to-rect
    //! output.
    //!
    //! This means that rectange remains rectangle after transformation. Rotation
    //! is possible, but only by 90, 128 or 270 degrees.
    //!
    //! @note To check whether the output will be in integral numbers, check the
    //! @c integralTransform flag, because @c rectToRectTransform not guarantees
    //! pixel-aligned output.
    uint32_t rectToRectTransform : 1;

    //! @brief Whether the final transform is also available in float format.
    uint32_t finalTransformF : 1;

    //! @brief Whether the line width and caps are ideal.
    //!
    //! Ideal line means one-pixel width and square caps. If fastLineHint is set
    //! to true then different algorithm can be used for line rendering.
    uint32_t idealLine : 1;

    //! @brief Reserved for future use.
    uint32_t unused : 5;
  };

  //! @brief All data packed in single integer.
  uint32_t packed;
};

// ============================================================================
// [Fog::RasterSource]
// ============================================================================

struct FOG_NO_EXPORT RasterSource
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union
  {
    //! @brief color.
    Static<Color> color;
    //! @brief Texture.
    Static<Texture> texture;
    //! @brief Gradient.
    Static<GradientD> gradient;
  };

  Static<TransformD> transform;
  Static<TransformD> adjusted;
};

// ============================================================================
// [Fog::RasterLayer]
// ============================================================================

//! @internal
//!
//! @brief The Raster layer.
struct FOG_NO_EXPORT RasterLayer
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    memset(this, 0, sizeof(*this));
  }

  // --------------------------------------------------------------------------
  // [Members - Primary Format]
  // --------------------------------------------------------------------------

  //! @brief The raster-layer size, NEEDED FOR _setupLayer().
  SizeI size;
  //! @brief The raster-layer stride, NEEDED FOR _setupLayer().
  ssize_t stride;
  //! @brief Pointer to the first scanline, NEEDED FOR _setupLayer().
  uint8_t* pixels;
  //! @brief The raster-layer format, NEEDED FOR _setupLayer().
  uint32_t primaryFormat;
  //! @brief The raster-layer bytes-per-pixel.
  uint32_t primaryBPP;
  //! @brief The raster-layer bytes-per-line.
  uint32_t primaryBPL;
  //! @brief The raster-layer pixel precision.
  uint32_t precision;

  // --------------------------------------------------------------------------
  // [Members - Secondary Format]
  // --------------------------------------------------------------------------

  //! @brief The secondary-layer format.
  uint32_t secondaryFormat;
  //! @brief The secondary-layer bytes-per-pixel.
  uint32_t secondaryBPP;
  //! @brief The secondary-layer bytes-per-line.
  uint32_t secondaryBPL;

  //! @brief The 'secondary-from-primary' format blitter.
  RasterVBlitLineFunc cvtSecondaryFromPrimary;
  //! @brief The 'primary-from-secondary' format blitter.
  RasterVBlitLineFunc cvtPrimaryFromSecondary;

  // --------------------------------------------------------------------------
  // [Members - Image Data]
  // --------------------------------------------------------------------------

  //! @brief Image data instance (optional, for lock/unlock).
  ImageData* imageData;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H
