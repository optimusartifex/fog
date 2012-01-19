// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERPAINTSERIALIZER_P_H
#define _FOG_G2D_PAINTING_RASTERPAINTSERIALIZER_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterApi_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterPaintSerializer]
// ============================================================================

//! @internal
//!
//! @brief Raster paint-engine serializer.
//!
//! This class contains function pointers to low-level painter operations. When
//! single-threaded mode is used, the serializer contains function pointers to
//! render functions, otherwise serialize functions are used.
struct FOG_NO_EXPORT RasterPaintSerializer
{
  // --------------------------------------------------------------------------
  // [Types - Paint]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_FASTCALL *FillAll)(RasterPaintEngine* engine);
  typedef err_t (FOG_FASTCALL *FillPathF)(RasterPaintEngine* engine, const PathF* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FillPathD)(RasterPaintEngine* engine, const PathD* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *StrokeAndFillPathF)(RasterPaintEngine* engine, const PathF* path);
  typedef err_t (FOG_FASTCALL *StrokeAndFillPathD)(RasterPaintEngine* engine, const PathD* path);

  typedef err_t (FOG_FASTCALL *FillNormalizedBoxI)(RasterPaintEngine* engine, const BoxI* box);
  typedef err_t (FOG_FASTCALL *FillNormalizedBoxF)(RasterPaintEngine* engine, const BoxF* box);
  typedef err_t (FOG_FASTCALL *FillNormalizedBoxD)(RasterPaintEngine* engine, const BoxD* box);
  typedef err_t (FOG_FASTCALL *FillNormalizedPathF)(RasterPaintEngine* engine, const PathF* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FillNormalizedPathD)(RasterPaintEngine* engine, const PathD* path, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Funcs - Paint]
  // --------------------------------------------------------------------------

  FillAll fillAll;
  FillPathF fillPathF;
  FillPathD fillPathD;
  StrokeAndFillPathF strokeAndFillPathF;
  StrokeAndFillPathD strokeAndFillPathD;

  FillNormalizedBoxI fillNormalizedBoxI;
  FillNormalizedBoxF fillNormalizedBoxF;
  FillNormalizedBoxD fillNormalizedBoxD;
  FillNormalizedPathF fillNormalizedPathF;
  FillNormalizedPathD fillNormalizedPathD;

  // --------------------------------------------------------------------------
  // [Types - Blit]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_FASTCALL *BlitImageD)(RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform);
  typedef err_t (FOG_FASTCALL *BlitNormalizedImageA)(RasterPaintEngine* engine, const PointI* pt, const Image* srcImage, const RectI* srcFragment);
  typedef err_t (FOG_FASTCALL *BlitNormalizedImageI)(RasterPaintEngine* engine, const BoxI* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform);
  typedef err_t (FOG_FASTCALL *BlitNormalizedImageD)(RasterPaintEngine* engine, const BoxD* box, const Image* srcImage, const RectI* srcFragment, const TransformD* srcTransform);

  // --------------------------------------------------------------------------
  // [Funcs - Blit]
  // --------------------------------------------------------------------------

  BlitImageD blitImageD;
  BlitNormalizedImageA blitNormalizedImageA;
  BlitNormalizedImageI blitNormalizedImageI;
  BlitNormalizedImageD blitNormalizedImageD;

  // --------------------------------------------------------------------------
  // [Types - Filter]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_FASTCALL *FilterPathF)(RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FilterPathD)(RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule);

  typedef err_t (FOG_FASTCALL *FilterNormalizedBoxI)(RasterPaintEngine* engine, const FeBase* feBase, const BoxI* box);
  typedef err_t (FOG_FASTCALL *FilterNormalizedBoxF)(RasterPaintEngine* engine, const FeBase* feBase, const BoxF* box);
  typedef err_t (FOG_FASTCALL *FilterNormalizedBoxD)(RasterPaintEngine* engine, const FeBase* feBase, const BoxD* box);

  typedef err_t (FOG_FASTCALL *FilterNormalizedPathF)(RasterPaintEngine* engine, const FeBase* feBase, const PathF* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *FilterNormalizedPathD)(RasterPaintEngine* engine, const FeBase* feBase, const PathD* path, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Funcs - Filter]
  // --------------------------------------------------------------------------

  FilterPathF filterPathF;
  FilterPathD filterPathD;

  FilterNormalizedBoxI filterNormalizedBoxI;
  FilterNormalizedBoxF filterNormalizedBoxF;
  FilterNormalizedBoxD filterNormalizedBoxD;

  FilterNormalizedPathF filterNormalizedPathF;
  FilterNormalizedPathD filterNormalizedPathD;

  // --------------------------------------------------------------------------
  // [Types - Clip]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_FASTCALL *ClipAll)(RasterPaintEngine* engine);
  typedef err_t (FOG_FASTCALL *ClipPathF)(RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *ClipPathD)(RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *StrokeAndClipPathF)(RasterPaintEngine* engine, uint32_t clipOp, const PathF* path);
  typedef err_t (FOG_FASTCALL *StrokeAndClipPathD)(RasterPaintEngine* engine, uint32_t clipOp, const PathD* path);

  typedef err_t (FOG_FASTCALL *ClipNormalizedBoxI)(RasterPaintEngine* engine, uint32_t clipOp, const BoxI* box);
  typedef err_t (FOG_FASTCALL *ClipNormalizedBoxF)(RasterPaintEngine* engine, uint32_t clipOp, const BoxF* box);
  typedef err_t (FOG_FASTCALL *ClipNormalizedBoxD)(RasterPaintEngine* engine, uint32_t clipOp, const BoxD* box);
  typedef err_t (FOG_FASTCALL *ClipNormalizedPathF)(RasterPaintEngine* engine, uint32_t clipOp, const PathF* path, uint32_t fillRule);
  typedef err_t (FOG_FASTCALL *ClipNormalizedPathD)(RasterPaintEngine* engine, uint32_t clipOp, const PathD* path, uint32_t fillRule);

  // --------------------------------------------------------------------------
  // [Funcs - Clip]
  // --------------------------------------------------------------------------

  ClipAll clipAll;
  ClipPathF clipPathF;
  ClipPathD clipPathD;
  StrokeAndClipPathF strokeAndClipPathF;
  StrokeAndClipPathD strokeAndClipPathD;

  ClipNormalizedBoxI clipNormalizedBoxI;
  ClipNormalizedBoxF clipNormalizedBoxF;
  ClipNormalizedBoxD clipNormalizedBoxD;

  ClipNormalizedPathF clipNormalizedPathF;
  ClipNormalizedPathD clipNormalizedPathD;
};

// ============================================================================
// [Fog::RasterPaintSerializer - VTable]
// ============================================================================

extern FOG_NO_EXPORT RasterPaintSerializer RasterPaintSerializer_vtable[RASTER_MODE_COUNT];

void FOG_NO_EXPORT RasterPaintSerializer_init_st(void);

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERPAINTSERIALIZER_P_H
