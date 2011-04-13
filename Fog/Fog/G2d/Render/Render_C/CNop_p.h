// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_CNOP_P_H
#define _FOG_G2D_RENDER_RENDER_C_CNOP_P_H

// [Dependencies]
#include <Fog/G2d/Render/Render_C/Defs_p.h>

namespace Fog {
namespace Render_C {

// ============================================================================
// [Fog::Render_C - CompositeNop]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT CompositeNop
{
  // ==========================================================================
  // [CBlit - Line]
  // ==========================================================================

  static void FOG_FASTCALL nop_cblit_line(
    uint8_t* dst, const RenderSolid* src, int w, const RenderClosure* closure)
  {
  }

  // ==========================================================================
  // [CBlit - Span]
  // ==========================================================================

  static void FOG_FASTCALL nop_cblit_span(
    uint8_t* dst, const RenderSolid* src, const Span* span, const RenderClosure* closure)
  {
  }

  // ==========================================================================
  // [VBlit - Line]
  // ==========================================================================

  static void FOG_FASTCALL nop_vblit_line(
    uint8_t* dst, const uint8_t* src, int w, const RenderClosure* closure)
  {
  }

  // ==========================================================================
  // [VBlit - Span]
  // ==========================================================================

  static void FOG_FASTCALL nop_vblit_span(
    uint8_t* dst, const Span* span, const RenderClosure* closure)
  {
  }
};

} // Render namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_CNOP_P_H
