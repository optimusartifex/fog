// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDER_C_DEFS_P_H
#define _FOG_G2D_RENDER_RENDER_C_DEFS_P_H

// [Dependencies]
#include <Fog/Core/Collection/PBuffer.h>
#include <Fog/Core/Face/Face_C.h>
#include <Fog/Core/Face/Face_C_G2d.h>
#include <Fog/Core/Math/Fixed.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryOps.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Rasterizer/Span_p.h>
#include <Fog/G2d/Render/RenderApi_p.h>
#include <Fog/G2d/Render/RenderConstants_p.h>
#include <Fog/G2d/Render/RenderFuncs_p.h>
#include <Fog/G2d/Render/RenderStructs_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/ColorStopCache.h>
#include <Fog/G2d/Source/ColorStopList.h>
#include <Fog/G2d/Source/ColorUtil.h>
#include <Fog/G2d/Source/ConicalGradient.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/LinearGradient.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Source/RadialGradient.h>
#include <Fog/G2d/Source/RectangularGradient.h>
#include <Fog/G2d/Source/Texture.h>

// [Dependencies - Fog::Render_C]
#include <Fog/G2d/Render/Render_C/Access_p.h>

// ============================================================================
// [C_BLIT_8]
// ============================================================================

// Macros to help creating blit functions.
//
// Usage:
//
//    C_BLIT_SPAN8_BEGIN(bytes_per_pixel)
//
//    -------------------------------------------------------------------------
//
//    C_BLIT_SPAN8_C_ANY()
//    {
//      'dst'  - The destination pointer.
//      'msk0' - The const-mask value.
//      'w'    - The number of pixels to process (width).
//    }
//
//      or
//
//    C_BLIT_SPAN8_C_OPAQUE()
//    {
//      'dst'  - The destination pointer.
//      'w'    - The number of pixels to process (width).
//    }
//    C_BLIT_SPAN8_C_MASK()
//    {
//      'dst'  - The destination pointer.
//      'msk0' - The const-mask value.
//      'w'    - The number of pixels to process (width).
//    }
//
//    -------------------------------------------------------------------------
//
//    C_BLIT_SPAN8_A8_GLYPH()
//    {
//      'dst'  - The destination pointer.
//      'msk'  - The a8-mask pointer.
//      'w'    - The number of pixels to process (width).
//    }
//
//    -------------------------------------------------------------------------
//
//    C_BLIT_SPAN8_A8_EXTRA()
//    {
//      'dst'  - The destination pointer.
//      'msk'  - The a8-extended-mask pointer. Omit checks for fully-transparent
//               and fully-opaque pixels, it happens rarely.
//      'w'    - The number of pixels to process (width).
//    }
//
//    -------------------------------------------------------------------------
//
//    C_BLIT_SPAN8_ARGB32_GLYPH()
//    {
//      'dst'  - The destination pointer.
//      'msk'  - The argb32-mask pointer.
//      'w'    - The number of pixels to process (width).
//    }
//
//    -------------------------------------------------------------------------
//
//    C_BLIT_SPAN8_END()
//
// The V_BLIT_SPAN8_XXX functions are used when there is variable-source. The
// 'src' variable should be used inside of 'CASE' sections.

//! @brief CBlit - Span8 - Begin.
#define C_BLIT_SPAN8_BEGIN(BPP) \
  uint8_t* dstBase = dst; \
  \
  do { \
    int x = (uint)span->getX0(); \
    int w = (int)(span->getX1() - x); \
    FOG_ASSUME(w > 0); \
    \
    dst = dstBase + (sysuint_t)(uint)x * BPP; \
    const uint8_t* _msk = (const uint8_t*)reinterpret_cast<const Span8*>(span)->getGenericMask(); \
    \
    switch (span->getType()) \
    {

// ----------------------------------------------------------------------------

//! @brief CBlit - Span8 - C-Any.
#define C_BLIT_SPAN8_C_ANY() \
      case SPAN_C: \
      { \
        uint32_t msk0 = Span8::getConstMaskFromPointer(_msk); \
        FOG_ASSUME(msk0 <= 0x100); \
        {

//! @brief CBlit - Span8 - C-Opaque.
#define C_BLIT_SPAN8_C_OPAQUE() \
      case SPAN_C: \
      { \
        uint32_t msk0 = Span8::getConstMaskFromPointer(_msk); \
        FOG_ASSUME(msk0 <= 0x100); \
        \
        if (msk0 == 0x100) \
        {

//! @brief CBlit - Span8 - C-Mask.
#define C_BLIT_SPAN8_C_MASK() \
        } \
        else \
        {

// ----------------------------------------------------------------------------

//! @brief CBlit - Span8 - A8-Glyph.
#define C_BLIT_SPAN8_A8_GLYPH() \
        } \
        break; \
      } \
      \
      case SPAN_A8_GLYPH: \
      case SPAN_AX_GLYPH: \
      { \
        const uint8_t* msk = _msk;

// ----------------------------------------------------------------------------

//! @brief CBlit - Span8 - A8-Extra.
#define C_BLIT_SPAN8_A8_EXTRA() \
        break; \
      } \
      \
      case SPAN_AX_EXTRA: \
      { \
        const uint8_t* msk = _msk;

// ----------------------------------------------------------------------------

//! @brief CBlit - Span8 - ARGB32-Glyph.
#define C_BLIT_SPAN8_ARGB32_GLYPH() \
        break; \
      } \
      \
      case SPAN_ARGB32_GLYPH: \
      case SPAN_ARGBXX_GLYPH: \
      { \
        const uint8_t* msk = _msk;

// ----------------------------------------------------------------------------

//! @brief CBlit - End.
#define C_BLIT_SPAN8_END() \
        break; \
      } \
    } \
  } while ((span = span->getNext()) != NULL);



// ============================================================================
// [V_BLIT_8]
// ============================================================================


//! @brief VBlit - Span8 - Begin.
#define V_BLIT_SPAN8_BEGIN(BPP) \
  uint8_t* dstBase = dst; \
  \
  do { \
    uint x = (uint)span->getX0(); \
    int w = (int)((uint)span->getX1() - x); \
    FOG_ASSUME(w > 0); \
    \
    dst = dstBase + x * BPP; \
    const uint8_t* _msk = (const uint8_t*)reinterpret_cast<const Span8*>(span)->getGenericMask(); \
    const uint8_t* src = (const uint8_t*)reinterpret_cast<const SpanExt8*>(span)->getData(); \
    \
    switch (span->getType()) \
    {

// ----------------------------------------------------------------------------

//! @brief VBlit - Span8 - C-Any.
#define V_BLIT_SPAN8_C_ANY() \
      case SPAN_C: \
      { \
        uint32_t msk0 = Span8::getConstMaskFromPointer(_msk); \
        FOG_ASSERT(msk0 <= 0x100); \
        {

//! @brief VBlit - Span8 - C-Opaque.
#define V_BLIT_SPAN8_C_OPAQUE() \
      case SPAN_C: \
      { \
        uint32_t msk0 = Span8::getConstMaskFromPointer(_msk); \
        FOG_ASSERT(msk0 <= 0x100); \
        \
        if (msk0 == 0x100) \
        {

//! @brief VBlit - Span8 - C-Mask.
#define V_BLIT_SPAN8_C_MASK() \
        } \
        else \
        {

// ----------------------------------------------------------------------------

//! @brief VBlit - Span8 - A8-Mask - Glyph.
#define V_BLIT_SPAN8_A8_GLYPH() \
        } \
        break; \
      } \
      \
      case SPAN_A8_GLYPH: \
      case SPAN_AX_GLYPH: \
      { \
        const uint8_t* msk = _msk;

// ----------------------------------------------------------------------------

//! @brief VBlit - Span8 - A8-Mask - Extended.
#define V_BLIT_SPAN8_A8_EXTRA() \
        break; \
      } \
      \
      case SPAN_AX_EXTRA: \
      { \
        const uint8_t* msk = _msk;

// ----------------------------------------------------------------------------

//! @brief VBlit - Span8 - ARGB32-Glyph.
#define V_BLIT_SPAN8_ARGB32_GLYPH() \
        break; \
      } \
      \
      case SPAN_ARGB32_GLYPH: \
      case SPAN_ARGBXX_GLYPH: \
      { \
        const uint8_t* msk = _msk;

// ----------------------------------------------------------------------------

//! @brief VBlit - End.
#define V_BLIT_SPAN8_END() \
        break; \
      } \
    } \
  } while ((span = span->getNext()) != NULL);



// ============================================================================
// [BLIT_LOOP]
// ============================================================================

// These macros were designed to simplify blit functions. The idea is very simple.
// There are usually three loops per highly optimized blitter. The first loop
// aligns the destination buffer, usually to 16 bytes (for SSE2 processing).
// The second loop (main loop) processes more pixels per iteration (for example
// 4 ARGB pixels, 8 ARGB pixels, 16 A8 pixels, ...). The third loop is similar
// to first loop and its purpose is to process remaining pixels.
//
// There is an idea to merge align and tail loop. This is likely to save some
// binary space (about 1/4 of blitting functions size) and simplify development.
// So do not repeat dirty stuff and use BLIT_LOOP_... macros.
//
// The two loops are named SMALL and LARGE.
//
// 8-bit entities:
//   - 1 pixel at time:
//     - BLIT_LOOP_8x1_BEGIN(dst)        - Loop begin.
//     - BLIT_LOOP_8x1_END(dst)          - Loop end.
//
//   - 4 pixels at time:
//     - BLIT_LOOP_8x16_SMALL_BEGIN(dst) - Small loop begin.
//     - BLIT_LOOP_8x16_SMALL_END(dst)   - Small loop end.
//
//   - 16 pixels at time:
//     - BLIT_LOOP_8x16_MAIN_BEGIN(dst)  - Main loop begin.
//     - BLIT_LOOP_8x16_MAIN_END(dst)    - Main loop end.
//
// 32-bit entities:
//   - 1 pixel at time:
//     - BLIT_LOOP_32x4_SMALL_BEGIN(dst) - Small loop begin.
//     - BLIT_LOOP_32x4_SMALL_END(dst)   - Small loop end.
//
//   - 4 pixels at time:
//     - BLIT_LOOP_32x4_MAIN_BEGIN(dst)  - Main loop begin.
//     - BLIT_LOOP_32x4_MAIN_END(dst)    - Main loop end.
//
// Because compilers can be quite missed from our machinery, it's needed
// to follow some rules to help them to optimize this code:
// - declare temporary variables (mainly sse2 registers) in local loop scope.
// - do not add anything between BLIT_LOOP_32x4_SMALL_END and BLIT_LOOP_32x4_MAIN_BEGIN.

// ============================================================================
// [BLIT_LOOP - DstFx1 - DstF per pixel, 1 pixel in a main loop]
// ============================================================================

#define BLIT_LOOP_DstFx1_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_DstFx1_BEGIN(_Group_) \
  for (;;) {

#define BLIT_LOOP_DstFx1_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    continue;

#define BLIT_LOOP_DstFx1_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  } \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 8x1 - 8-bits per pixel, 1 pixel in a main loop]
// ============================================================================

#define BLIT_LOOP_8x1_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_8x1_BEGIN(_Group_) \
  for (;;) {

#define BLIT_LOOP_8x1_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    continue;

#define BLIT_LOOP_8x1_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  }; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 8x4 - 8-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_8x4_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_8x4_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 3) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_8x4_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 3) != 0) continue; \
    break;

#define BLIT_LOOP_8x4_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 3) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_8x4_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_8x4_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_8x4_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 8x8 - 8-bits per pixel, 8 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_8x8_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_8x8_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 7) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_8x8_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) != 0) continue; \
    break;

#define BLIT_LOOP_8x8_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_8x8_MAIN_BEGIN(_Group_) \
  w -= 8; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_8x8_MAIN_CONTINUE(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break;

#define BLIT_LOOP_8x8_MAIN_END(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 8; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 8x16 - 8-bits per pixel, 16 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_8x16_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_8x16_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 15) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_8x16_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) != 0) continue; \
    break;

#define BLIT_LOOP_8x16_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_8x16_MAIN_BEGIN(_Group_) \
  w -= 16; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_8x16_MAIN_CONTINUE(_Group_) \
    if ((w -= 16) >= 0) continue; \
    break;

#define BLIT_LOOP_8x16_MAIN_END(_Group_) \
    if ((w -= 16) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 16; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 16x1 - 16-bits per pixel, 1 pixel in a main loop]
// ============================================================================

#define BLIT_LOOP_16x1_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_16x1_BEGIN(_Group_) \
  for (;;) {

#define BLIT_LOOP_16x1_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    continue;

#define BLIT_LOOP_16x1_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  }; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 16x2 - 16-bits per pixel, 2 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_16x2_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_16x2_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 3) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_16x2_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    break;

#define BLIT_LOOP_16x2_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_16x2_MAIN_BEGIN(_Group_) \
  w -= 2; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_16x2_MAIN_CONTINUE(_Group_) \
    if ((w -= 2) >= 0) continue; \
    break;

#define BLIT_LOOP_16x2_MAIN_END(_Group_) \
    if ((w -= 2) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 2; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 16x4 - 16-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_16x4_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_16x4_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 7) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_16x4_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) != 0) continue; \
    break;

#define BLIT_LOOP_16x4_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_16x4_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_16x4_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_16x4_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 16x8 - 16-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_16x8_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_16x8_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 15) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_16x8_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) != 0) continue; \
    break;

#define BLIT_LOOP_16x8_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_16x8_MAIN_BEGIN(_Group_) \
  w -= 8; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_16x8_MAIN_CONTINUE(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break;

#define BLIT_LOOP_16x8_MAIN_END(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 8; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 24x1 - 24-bits per pixel, 1 pixel in a main loop]
// ============================================================================

#define BLIT_LOOP_24x1_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_24x1_BEGIN(_Group_) \
  for (;;) {

#define BLIT_LOOP_24x1_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    continue;

#define BLIT_LOOP_24x1_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  }; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 24x4 - 24-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_24x4_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_24x4_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 3) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_24x4_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 3) != 0) continue; \
    break;

#define BLIT_LOOP_24x4_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 3) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_24x4_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_24x4_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_24x4_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 24x8 - 24-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_24x8_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_24x8_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 7) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_24x8_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) != 0) continue; \
    break;

#define BLIT_LOOP_24x8_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_24x8_MAIN_BEGIN(_Group_) \
  w -= 8; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_24x8_MAIN_CONTINUE(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break;

#define BLIT_LOOP_24x8_MAIN_END(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 8; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 32xX - Generic]
// ============================================================================

#define BLIT_LOOP_32xX_INIT() \
  FOG_ASSUME(w > 0);

// ============================================================================
// [BLIT_LOOP - 32x1 - 32-bits per pixel, 1 pixel in a main loop]
// ============================================================================

#define BLIT_LOOP_32x1_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_32x1_BEGIN(_Group_) \
  for (;;) {

#define BLIT_LOOP_32x1_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    continue;

#define BLIT_LOOP_32x1_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  }; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 32x4 - 32-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_32x4_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_32x4_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 15) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_32x4_SMALL_BEGIN_ALT(_Group_, _PrepareCode_) \
  if (((sysuint_t)dst & 15) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  _PrepareCode_ \
  for (;;) {

#define BLIT_LOOP_32x4_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) != 0) continue; \
    break;

#define BLIT_LOOP_32x4_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \

#define BLIT_LOOP_32x4_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_32x4_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_32x4_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 32x4_VS_16 - 32-bits per pixel vs 16-bits per pixel]
// ============================================================================

#define BLIT_LOOP_32x4_VS_16_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_32x4_VS_16_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)src & 3) == 0 && w >= 4) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_32x4_VS_16_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (w < 4) continue; \
    break;

#define BLIT_LOOP_32x4_VS_16_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (w < 4) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \

#define BLIT_LOOP_32x4_VS_16_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_32x4_VS_16_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_32x4_VS_16_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 32x4_VS_24 - 32-bits per pixel vs 24-bits per pixel]
// ============================================================================

#define BLIT_LOOP_32x4_VS_24_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_32x4_VS_24_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)src & 3) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_32x4_VS_24_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)src & 3) != 0) continue; \
    break;

#define BLIT_LOOP_32x4_VS_24_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)src & 3) != 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \

#define BLIT_LOOP_32x4_VS_24_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_32x4_VS_24_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_32x4_VS_24_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 32x16 - 32-bits per pixel, 16 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_32x16_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_32x16_ALIGN_BEGIN(_Group_) \
  if (((sysuint_t)dst & 15) == 0) goto _##_Group_##_SmallSkip; \
  \
  do {

#define BLIT_LOOP_32x16_ALIGN_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  } while (((sysuint_t)dst & 15) != 0); \
  \
_##_Group_##_SmallSkip: \
  ;

#define BLIT_LOOP_32x16_MAIN_BEGIN(_Group_) \
  w -= 16; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  do {

#define BLIT_LOOP_32x16_MAIN_END(_Group_) \
  } while ((w -= 16) >= 0); \
  \
_##_Group_##_MainSkip: \
  w += 16;

#define BLIT_LOOP_32x16_TAIL_4(_Group_, _ProcessCode_) \
  switch (w >> 2) \
  { \
    case 3: _ProcessCode_ \
    case 2: _ProcessCode_ \
    case 1: _ProcessCode_ \
  }

#define BLIT_LOOP_32x16_TAIL_1(_Group_, _ProcessCode_) \
  switch (w & 3) \
  { \
    case 3: _ProcessCode_ \
    case 2: _ProcessCode_ \
    case 1: _ProcessCode_ \
  } \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 48x1 - 48-bits per pixel, 1 pixel in a main loop]
// ============================================================================

#define BLIT_LOOP_48x1_BEGIN(_Group_) \
  for (;;) {

#define BLIT_LOOP_48x1_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    continue;

#define BLIT_LOOP_48x1_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  }; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 48x4 - 48-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_48x4_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_48x4_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 7) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_48x4_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) == 0) continue; \
    break;

#define BLIT_LOOP_48x4_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 7) == 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \

#define BLIT_LOOP_48x4_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
_##_Group_##_MainLoop: \
  for (;;) {

#define BLIT_LOOP_48x4_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_48x4_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 48x8 - 48-bits per pixel, 8 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_48x8_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_48x8_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 15) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_48x8_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) == 0) continue; \
    break;

#define BLIT_LOOP_48x8_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (((sysuint_t)dst & 15) == 0) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \

#define BLIT_LOOP_48x8_MAIN_BEGIN(_Group_) \
  w -= 8; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
_##_Group_##_MainLoop: \
  for (;;) {

#define BLIT_LOOP_48x8_MAIN_CONTINUE(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break;

#define BLIT_LOOP_48x8_MAIN_END(_Group_) \
    if ((w -= 8) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 8; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 64x1 - 64-bits per pixel, 1 pixel in a main loop]
// ============================================================================

#define BLIT_LOOP_64x1_BEGIN(_Group_) \
  for (;;) {

#define BLIT_LOOP_64x1_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    continue;

#define BLIT_LOOP_64x1_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  }; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 64x2 - 64-bits per pixel, 2 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_64x2_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_64x2_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 15) != 0) \
  { \
_##_Group_##_SmallBegin: \

#define BLIT_LOOP_64x2_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
  } \

#define BLIT_LOOP_64x2_MAIN_BEGIN(_Group_) \
  w -= 2; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
_##_Group_##_MainLoop: \
  for (;;) {

#define BLIT_LOOP_64x2_MAIN_CONTINUE(_Group_) \
    if ((w -= 2) >= 0) continue; \
    break;

#define BLIT_LOOP_64x2_MAIN_END(_Group_) \
    if ((w -= 2) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 2; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [BLIT_LOOP - 64x4 - 64-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define BLIT_LOOP_64x4_INIT() \
  FOG_ASSUME(w > 0);

#define BLIT_LOOP_64x4_SMALL_BEGIN(_Group_) \
  if (((sysuint_t)dst & 15) == 0) goto _##_Group_##_SmallSkip; \
  \
_##_Group_##_SmallBegin: \
  for (;;) {

#define BLIT_LOOP_64x4_SMALL_CONTINUE(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (w < 4) continue; \
    break;

#define BLIT_LOOP_64x4_SMALL_END(_Group_) \
    if (--w == 0) goto _##_Group_##_End; \
    if (w < 4) continue; \
    break; \
  } \
  \
_##_Group_##_SmallSkip: \

#define BLIT_LOOP_64x4_MAIN_BEGIN(_Group_) \
  w -= 4; \
  if (w < 0) goto _##_Group_##_MainSkip; \
  \
  for (;;) {

#define BLIT_LOOP_64x4_MAIN_CONTINUE(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break;

#define BLIT_LOOP_64x4_MAIN_END(_Group_) \
    if ((w -= 4) >= 0) continue; \
    break; \
  } \
  \
_##_Group_##_MainSkip: \
  w += 4; \
  if (w != 0) goto _##_Group_##_SmallBegin; \
  \
_##_Group_##_End: \
  ;

// ============================================================================
// [Fog::RenderC - P_FETCH]
// ============================================================================

// Example of fetch function:
//
// static void FOG_FASTCALL fetch(
//   RenderPatternContext* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
// {
//   // Initialize fetch function (some variables needed for span loop).
//   P_FETCH_SPAN8_INIT()
//
//   // Here place code that you need to initialize the fetcher. You can access
//   // these variables from here:
//   // 'x' - X position in target raster buffer
//   // 'y' - Y position in target raster buffer (you need inverted matrix to
//   //       X/Y to position in our pattern).
//   // 'w' - Width (number of pixels to fetch).
//   // 'dst' - Buffer
//   //
//   // This place should be used only for initialization, transforming X/Y
//   // to destination (texture) buffer, etc...
//
//   // Begin of span-loop.
//   P_FETCH_SPAN8_BEGIN()
//
//     // Here you must decide if you pass your own buffer to span (only
//     // possible if mode is @c RENDER_FETCH_REFERENCE). To set current
//     // buffer, use:
//
//     P_FETCH_SPAN8_SET_CURRENT()
//
//     // To set custom buffer (owned by pattern), use:
//
//     P_FETCH_SPAN8_SET_CUSTOM(yourCustomBuffer)
//
//     // If you used custom buffer you finished with this span, otherwise use
//     // 'dst' pointer (and increment it after each fetcher pixel). For
//     // example:
//
//     do {
//       ((uint32_t*)dst)[0] = 0x00000000;
//     } while (--w);
//
//     // Here is place to tell fetcher how is the preferred way of handling
//     // "holes". Hole is space between two spans. If spans are neighbours,
//     // then hole is zero pixels width (this is common). But hole can be
//     // also one or more pixels width (this happens if this span->x1 is less
//     // than next span->x1).
//     //
//     // If you are not interested about holes, use simply:
//
//     P_FETCH_SPAN8_NEXT()
//
//     // Otherwise use:
//
//     P_FETCH_SPAN8_HOLE(
//     {
//       // Your code, you can use goto to skip to specific part in loop.
//       // If you are using 'x' then you must update it.
//       //
//       // Use 'hole' variable that contains number of pixels to skip.
//       x += hole;
//     })
//
//   // End of span-loop.
//   P_FETCH_SPAN8_END()
// }
//

#define P_FETCH_SPAN8_INIT() \
  uint8_t* dst = buffer; \
  \
  int x = (int)span->getX0(); \
  int w = (int)((uint)span->getX1() - (uint)x); \
  FOG_ASSERT(w > 0);

#define P_FETCH_SPAN8_BEGIN() \
  for (;;) \
  {

#define P_FETCH_SPAN8_SET_CURRENT() \
    reinterpret_cast<SpanExt8*>(span)->setData(dst);

#define P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBORS(_BPP_) \
    reinterpret_cast<SpanExt8*>(span)->setData(dst); \
    \
    { \
      Span* next = span->getNext(); \
      if (next && next->getX0() == span->getX1()) \
      { \
        do { \
          reinterpret_cast<SpanExt8*>(next)->setData(dst + (uint)w * _BPP_); \
          span = next; \
          w = (int)((uint)next->getX1() - (uint)x); \
          next = span->getNext(); \
        } while (next && next->getX0() == span->getX1()); \
      } \
    }

#define P_FETCH_SPAN8_SET_CUSTOM(_Dst_) \
    reinterpret_cast<SpanExt8*>(span)->setData((uint8_t*)(_Dst_));

#define P_FETCH_SPAN8_NEXT() \
    if ((span = span->getNext()) == NULL) break; \
    \
    x = (int)span->getX0(); \
    w = (int)((uint)span->getX1() - (uint)x); \
    FOG_ASSERT(w > 0);

#define P_FETCH_SPAN8_HOLE(_Code_) \
    { \
      int endX = (int)span->getX1(); \
      if ((span = span->getNext()) == NULL) break; \
      \
      int startX = (int)span->getX0(); \
      FOG_ASSERT(endX <= startX); \
      \
      w = (int)((uint)span->getX1() - (uint)startX); \
      FOG_ASSERT(w > 0); \
      \
      if (endX != startX) \
      { \
        int hole = (startX - endX); \
        _Code_ \
      } \
      \
    }

#define P_FETCH_SPAN8_END() \
  }

// ============================================================================
// [Fog::RenderC - P_INTERPOLATE_C]
// ============================================================================

#if FOG_ARCH_BITS >= 64

#define P_INTERPOLATE_C_32_2(_Dst_, _Src0_, _Weight0_, _Src1_, _Weight1_) \
{ \
  Face::p64 _PixT0; \
  Face::p64 _PixT1; \
  \
  Face::p64UnpackPBWFromPBB2031(_PixT0, (_Src0_)); \
  Face::p64UnpackPBWFromPBB2031(_PixT1, (_Src1_)); \
  \
  Face::p64Mul(_PixT0, _PixT0, _Weight0_); \
  Face::p64Mul(_PixT1, _PixT1, _Weight1_); \
  Face::p64Add(_PixT0, _PixT0, _PixT1); \
  Face::p64And(_PixT0, _PixT0, Face::p64FromU64(FOG_UINT64_C(0xFF00FF00FF00FF00))); \
  \
  Face::p64Pack2031_RShift8(_Dst_, _PixT0); \
}

#define P_INTERPOLATE_C_32_2_WITH_ZERO(_Dst_, _Src0_, _Weight0_) \
{ \
  Face::p64 _PixT0; \
  \
  Face::u64_1x4bUnpack0213(_PixT0, (_Src0_)); \
  Face::p64Mul(_PixT0, _PixT0, _Weight0_); \
  Face::p64And(_PixT0, _PixT0, Face::p64FromU64(FOG_UINT64_C(0xFF00FF00FF00FF00))); \
  \
  Face::p64Pack2031_RShift8(_Dst_, _PixT0); \
}

#define P_INTERPOLATE_C_32_4(_Dst_, _Src0_, _Weight0_, _Src1_, _Weight1_, _Src2_, _Weight2_, _Src3_, _Weight3_) \
{ \
  Face::p64 _PixT0; \
  Face::p64 _PixT1; \
  Face::p64 _PixT2; \
  \
  Face::p64UnpackPBWFromPBB2031(_PixT0, (_Src0_)); \
  Face::p64UnpackPBWFromPBB2031(_PixT1, (_Src1_)); \
  \
  Face::p64Mul(_PixT0, _PixT0, Face::p64FromU64(_Weight0_)); \
  Face::p64Mul(_PixT1, _PixT1, Face::p64FromU64(_Weight1_)); \
  Face::p64Add(_PixT0, _PixT0, _PixT1); \
  \
  Face::p64UnpackPBWFromPBB2031(_PixT1, (_Src2_)); \
  Face::p64UnpackPBWFromPBB2031(_PixT2, (_Src3_)); \
  \
  Face::p64Mul(_PixT1, _PixT1, Face::p64FromU64(_Weight2_)); \
  Face::p64Mul(_PixT2, _PixT2, Face::p64FromU64(_Weight3_)); \
  Face::p64Add(_PixT0, _PixT0, _PixT1); \
  Face::p64Add(_PixT0, _PixT0, _PixT2); \
  \
  Face::p64And(_PixT0, _PixT0, Face::p64FromU64(FOG_UINT64_C(0xFF00FF00FF00FF00))); \
  \
  Face::p64Pack2031_RShift8(_Dst_, _PixT0); \
}

#else

#define P_INTERPOLATE_C_32_2(_Dst_, _Src0_, _Weight0_, _Src1_, _Weight1_) \
{ \
  Face::p32 _PixT0_0, _PixT0_1; \
  Face::p32 _PixT1_0, _PixT1_1; \
  Face::p32 _Weight; \
  \
  Face::p32UnpackPBWFromPBB_2031(_PixT0_0, _PixT0_1, (_Src0_)); \
  Face::p32UnpackPBWFromPBB_2031(_PixT1_0, _PixT1_1, (_Src1_)); \
  \
  _Weight = _Weight0_; \
  _PixT0_0 *= _Weight; \
  _PixT0_1 *= _Weight; \
  \
  _Weight = _Weight1_; \
  _PixT1_0 *= _Weight; \
  _PixT1_1 *= _Weight; \
  \
  _PixT0_0 += _PixT1_0; \
  _PixT0_1 += _PixT1_1; \
  \
  _PixT0_0 = (_PixT0_0 >> 8) & 0x00FF00FF; \
  _PixT0_1 = (_PixT0_1     ) & 0xFF00FF00; \
  \
  _Dst_ = _FOG_FACE_COMBINE_2(_PixT0_0, _PixT0_1); \
}

#define P_INTERPOLATE_C_32_2_WITH_ZERO(_Dst_, _Src0_, _Weight0_) \
{ \
  Face::p32 _PixT0_0, _PixT0_1; \
  Face::p32 _Weight; \
  \
  Face::p32UnpackPBWFromPBB_2031(_PixT0_0, _PixT0_1, (_Src0_)); \
  \
  _Weight = _Weight0_; \
  _PixT0_0 *= _Weight; \
  _PixT0_1 *= _Weight; \
  \
  _PixT0_0 = (_PixT0_0 >> 8) & 0x00FF00FF; \
  _PixT0_1 = (_PixT0_1     ) & 0xFF00FF00; \
  \
  _Dst_ = _FOG_FACE_COMBINE_2(_PixT0_0, _PixT0_1); \
}

#define P_INTERPOLATE_C_32_4(_Dst_, _Src0_, _Weight0_, _Src1_, _Weight1_, _Src2_, _Weight2_, _Src3_, _Weight3_) \
{ \
  Face::p32 _PixT0_0, _PixT0_1; \
  Face::p32 _PixT1_0, _PixT1_1; \
  Face::p32 _Weight; \
  \
  Face::p32UnpackPBWFromPBB_2031(_PixT0_0, _PixT0_1, (_Src0_)); \
  Face::p32UnpackPBWFromPBB_2031(_PixT1_0, _PixT1_1, (_Src1_)); \
  \
  _Weight = (_Weight0_); \
  _PixT0_0 *= _Weight; \
  _PixT0_1 *= _Weight; \
  \
  _Weight = (_Weight1_); \
  _PixT1_0 *= _Weight; \
  _PixT1_1 *= _Weight; \
  \
  _PixT0_0 += _PixT1_0; \
  _PixT0_1 += _PixT1_1; \
  \
  Face::p32UnpackPBWFromPBB_2031(_PixT1_0, _PixT1_1, (_Src2_)); \
  \
  _Weight = (_Weight2_); \
  _PixT1_0 *= _Weight; \
  _PixT1_1 *= _Weight; \
  \
  _PixT0_0 += _PixT1_0; \
  _PixT0_1 += _PixT1_1; \
  \
  Face::p32UnpackPBWFromPBB_2031(_PixT1_0, _PixT1_1, (_Src3_)); \
  \
  _Weight = (_Weight3_); \
  _PixT1_0 *= _Weight; \
  _PixT1_1 *= _Weight; \
  \
  _PixT0_0 += _PixT1_0; \
  _PixT0_1 += _PixT1_1; \
  \
  _PixT0_0 = (_PixT0_0 >> 8) & 0x00FF00FF; \
  _PixT0_1 = (_PixT0_1     ) & 0xFF00FF00; \
  \
  _Dst_ = _FOG_FACE_COMBINE_2(_PixT0_0, _PixT0_1); \
}

#endif // FOG_ARCH_BITS

// [Guard]
#endif // _FOG_G2D_RENDER_RENDER_C_DEFS_P_H
