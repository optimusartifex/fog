// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTERFILLER_P_H
#define _FOG_G2D_PAINTING_RASTERFILLER_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterApi_p.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::RasterFiller]
// ============================================================================

struct FOG_NO_EXPORT RasterFiller
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  //! @brief Called by @c rasterize() to prepare for filling 
  //!
  //! @param y The Y coordinate where the fill starts.
  typedef void (FOG_FASTCALL *PrepareFn)(RasterFiller* self, int y);
  //! @brief Called by @c rasterize() to process a scanline.
  //!
  //! After the scanline is rendered the callback must advance position by 1
  //! when running single-threaded or by delta when running multi-threaded.
  typedef void (FOG_FASTCALL *ProcessFn)(RasterFiller* self, RasterSpan* spans); 
  //! @brief Called by @c rasterize() to skip a scanline(s).
  //!
  //! If running single-threaded the @a step parameter is the total scanlines
  //! to skip. When running multi-threaded, the @a step parameter must be
  //! multiplied by delta to get the total count of scanlines to skip.
  typedef void (FOG_FASTCALL *SkipFn)(RasterFiller* self, int step);

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE void prepare(int y)
  {
    _prepare(this, y);
  }

  FOG_INLINE void process(RasterSpan* spans)
  {
    _process(this, spans);
  }

  FOG_INLINE void skip(int step)
  {
    _skip(this, step);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Prepare callback (set by user).
  PrepareFn _prepare;
  //! @brief Process callback (set by user).
  ProcessFn _process;
  //! @brief Skip callback (set by user).
  SkipFn _skip;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTERFILLER_P_H
