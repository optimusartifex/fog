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

union RasterHints;
struct RasterPaintGroup;
struct RasterPaintContext;
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
// [Fog::RasterPaintSource]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintSource
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
// [Fog::RasterPaintTarget]
// ============================================================================

//! @internal
//!
//! @brief Raster paint engine group.
struct FOG_NO_EXPORT RasterPaintTarget
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    memset(this, 0, sizeof(*this));
  }

  // --------------------------------------------------------------------------
  // [Members - Buffer]
  // --------------------------------------------------------------------------

  //! @brief The raster-layer size, NEEDED FOR _setupLayer().
  SizeI size;
  //! @brief The raster-layer stride, NEEDED FOR _setupLayer().
  ssize_t stride;
  //! @brief Pointer to the first scanline, NEEDED FOR _setupLayer().
  uint8_t* pixels;

  //! @brief The raster-layer format, NEEDED FOR _setupLayer().
  uint32_t format;
  //! @brief The raster-layer bytes-per-pixel.
  uint32_t bpp;
  //! @brief The raster-layer bytes-per-line.
  uint32_t bpl;
  //! @brief The raster-layer pixel precision.
  uint32_t precision;

  // --------------------------------------------------------------------------
  // [Members - Image Data]
  // --------------------------------------------------------------------------

  //! @brief Image data instance (optional, for lock/unlock).
  ImageData* imageData;
};

// ============================================================================
// [Fog::RasterPaintGroup]
// ============================================================================

struct FOG_NO_EXPORT RasterPaintGroup
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    top = NULL;
    flags = NO_FLAGS;

    numGroups = 0;
    opacityF = 1.0f;

    boundingBox.reset();

    groupRecord = NULL;
    cmdRecord = NULL;
    cmdStart = NULL;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Link to the top group.
  RasterPaintGroup* top;

  //! @brief Group flags (types of commands used in this group).
  uint32_t flags;
  //! @brief How many groups are inside of this group.
  uint32_t numGroups;

#if FOG_ARCH_BITS >= 64
  //! @brief The group bounding box (it can only grow).
  BoxI boundingBox;
  //! @brief The group opacity;
  float opacityF;
#else
  //! @brief The group opacity;
  float opacityF;
  //! @brief The group bounding box (it can only grow).
  BoxI boundingBox;
#endif

  //! @brief Group record (recorded position in groupAllocator).
  MemZoneRecord* groupRecord;
  //! @brief Commands record (recorded position in cmdAllocator).
  MemZoneRecord* cmdRecord;
  //! @brief Commands start pointer.
  uint8_t* cmdStart;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H
