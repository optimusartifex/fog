// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITENOP_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITENOP_P_H

// [Dependencies]
#include <Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h>

namespace Fog {
namespace RasterOps_C {

// ============================================================================
// [Fog::RasterOps_C - CompositeNop]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeNop
{
  // ==========================================================================
  // [CBlit - Line]
  // ==========================================================================

  static void FOG_FASTCALL nop_cblit_line(
    uint8_t* dst, const RasterSolid* src, int w, const RasterClosure* closure)
  {
  }

  // ==========================================================================
  // [CBlit - Span]
  // ==========================================================================

  static void FOG_FASTCALL nop_cblit_span(
    uint8_t* dst, const RasterSolid* src, const RasterSpan* span, const RasterClosure* closure)
  {
  }

  // ==========================================================================
  // [VBlit - Line]
  // ==========================================================================

  static void FOG_FASTCALL nop_vblit_line(
    uint8_t* dst, const uint8_t* src, int w, const RasterClosure* closure)
  {
  }

  // ==========================================================================
  // [VBlit - Span]
  // ==========================================================================

  static void FOG_FASTCALL nop_vblit_span(
    uint8_t* dst, const RasterSpan* span, const RasterClosure* closure)
  {
  }
};

} // Render namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_C_COMPOSITENOP_P_H