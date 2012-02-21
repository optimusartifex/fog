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

union RasterHints;
struct RasterPaintCmd;
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

// ============================================================================
// [Fog::RasterPaintDoCmd]
// ============================================================================

//! @internal
//!
//! @brief Raster paint engine command handler (renderer, serializer, etc...).
//!
//! This class contains function pointers (vtable) to the lowest-level painter
//! operations used by raster paint engine. The vtable is different for ST/MT
//! modes, and for using groups - @refPainter::newGroup() and @ref Painter::endGroup().
struct FOG_NO_EXPORT RasterPaintDoCmd
{
  // --------------------------------------------------------------------------
  // [Funcs - Paint]
  // --------------------------------------------------------------------------

  err_t (FOG_FASTCALL *fillAll)(RasterPaintEngine* engine);
  err_t (FOG_FASTCALL *fillPathF)(RasterPaintEngine* engine, const PathF* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *fillPathD)(RasterPaintEngine* engine, const PathD* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *fillStrokedPathF)(RasterPaintEngine* engine, const PathF* path);
  err_t (FOG_FASTCALL *fillStrokedPathD)(RasterPaintEngine* engine, const PathD* path);
  err_t (FOG_FASTCALL *fillNormalizedBoxI)(RasterPaintEngine* engine, const BoxI* box);
  err_t (FOG_FASTCALL *fillNormalizedBoxF)(RasterPaintEngine* engine, const BoxF* box);
  err_t (FOG_FASTCALL *fillNormalizedBoxD)(RasterPaintEngine* engine, const BoxD* box);
  err_t (FOG_FASTCALL *fillNormalizedPathF)(RasterPaintEngine* engine, const PathF* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *fillNormalizedPathD)(RasterPaintEngine* engine, const PathD* path, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Funcs - Blit]
  // --------------------------------------------------------------------------

  err_t (FOG_FASTCALL *blitImageD)(RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality);
  err_t (FOG_FASTCALL *blitNormalizedImageA)(RasterPaintEngine* engine, const PointI* pt, const Image* srcImage, const RectI* srcFragment);
  err_t (FOG_FASTCALL *blitNormalizedImageI)(RasterPaintEngine* engine, const BoxI* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality);
  err_t (FOG_FASTCALL *blitNormalizedImageD)(RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform, uint32_t imageQuality);

  // --------------------------------------------------------------------------
  // [Funcs - Filter]
  // --------------------------------------------------------------------------

  err_t (FOG_FASTCALL *filterAll)(RasterPaintEngine* engine, const FeBase* feBase);
  err_t (FOG_FASTCALL *filterPathF)(RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *filterPathD)(RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *filterStrokedPathF)(RasterPaintEngine* engine, const FeBase* feBase, const PathF* path);
  err_t (FOG_FASTCALL *filterStrokedPathD)(RasterPaintEngine* engine, const FeBase* feBase, const PathD* path);
  err_t (FOG_FASTCALL *filterNormalizedBoxI)(RasterPaintEngine* engine, const FeBase* feBase, const BoxI* box);
  err_t (FOG_FASTCALL *filterNormalizedBoxF)(RasterPaintEngine* engine, const FeBase* feBase, const BoxF* box);
  err_t (FOG_FASTCALL *filterNormalizedBoxD)(RasterPaintEngine* engine, const FeBase* feBase, const BoxD* box);
  err_t (FOG_FASTCALL *filterNormalizedPathF)(RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *filterNormalizedPathD)(RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Funcs - Mask]
  // --------------------------------------------------------------------------

  err_t (FOG_FASTCALL *switchToMask)(RasterPaintEngine* engine);
  err_t (FOG_FASTCALL *discardMask)(RasterPaintEngine* engine);

  err_t (FOG_FASTCALL *saveMask)(RasterPaintEngine* engine);
  err_t (FOG_FASTCALL *restoreMask)(RasterPaintEngine* engine);

  err_t (FOG_FASTCALL *maskFromNormalizedBoxI)(RasterPaintEngine* engine, const BoxI* box);
  err_t (FOG_FASTCALL *maskFromNormalizedBoxF)(RasterPaintEngine* engine, const BoxF* box);
  err_t (FOG_FASTCALL *maskFromNormalizedBoxD)(RasterPaintEngine* engine, const BoxD* box);
  err_t (FOG_FASTCALL *maskFromNormalizedPathF)(RasterPaintEngine* engine, const PathF* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *maskFromNormalizedPathD)(RasterPaintEngine* engine, const PathD* path, uint32_t fillRule);

  err_t (FOG_FASTCALL *maskIntersectNormalizedBoxI)(RasterPaintEngine* engine, const BoxI* box);
  err_t (FOG_FASTCALL *maskIntersectNormalizedBoxF)(RasterPaintEngine* engine, const BoxF* box);
  err_t (FOG_FASTCALL *maskIntersectNormalizedBoxD)(RasterPaintEngine* engine, const BoxD* box);
  err_t (FOG_FASTCALL *maskIntersectNormalizedPathF)(RasterPaintEngine* engine, const PathF* path, uint32_t fillRule);
  err_t (FOG_FASTCALL *maskIntersectNormalizedPathD)(RasterPaintEngine* engine, const PathD* path, uint32_t fillRule);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTSTRUCTS_P_H
