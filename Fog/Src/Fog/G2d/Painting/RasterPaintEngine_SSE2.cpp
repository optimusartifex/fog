// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/FaceC.h>
#include <Fog/Core/Face/FaceSSE2.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterConstants_p.h>
#include <Fog/G2d/Painting/RasterPaintEngine_p.h>
#include <Fog/G2d/Painting/RasterPaintWork_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterState_p.h>
#include <Fog/G2d/Painting/RasterUtil_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Text/Font.h>

namespace Fog {

// ============================================================================
// [Fog::RasterPaintEngine - Source]
// ============================================================================

static err_t FOG_CDECL RasterPaintEngine_setSourceArgb32_byte_SSE2(Painter* self, uint32_t argb32)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  FOG_ASSERT(engine->ctx.precision == IMAGE_PRECISION_BYTE);

  if (engine->sourceType != RASTER_SOURCE_ARGB32)
  {
    engine->saveSourceAndDiscard();
    engine->sourceType = RASTER_SOURCE_ARGB32;
    engine->ctx.pc = (RasterPattern*)(size_t)0x1;
  }
  else
  {
    engine->saveSourceArgb32();
  }

  __m128i xmmARGB;
  __m128i xmmAAAA;

  Face::m128iCvtSI128FromSI(xmmARGB, argb32);
  Face::m128iUnpackPI16FromPI8Lo(xmmARGB, xmmARGB);
  Face::m128iShufflePI16Lo<3, 3, 3, 3>(xmmAAAA, xmmARGB);
  Face::m128iFillPBWi<3>(xmmARGB, xmmARGB);
  Face::m128iMulDiv255PI16(xmmARGB, xmmARGB, xmmAAAA);
  Face::m128iPackPU8FromPU16(xmmARGB, xmmARGB);

  engine->source.color->_argb32.p32 = argb32;
  Face::m128iStore4(&engine->ctx.solid.prgb32, xmmARGB);
  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setSourceArgb64_byte_SSE2(Painter* self, const Argb64* argb64)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  FOG_ASSERT(engine->ctx.precision == IMAGE_PRECISION_BYTE);

  if (engine->sourceType != RASTER_SOURCE_COLOR)
  {
    engine->saveSourceAndDiscard();
    engine->sourceType = RASTER_SOURCE_COLOR;
    engine->ctx.pc = (RasterPattern*)(size_t)0x1;
  }
  else
  {
    engine->saveSourceColor();
  }

  __m128i xmmARGB64;
  __m128i xmmARGB32;
  __m128i xmmAAAA32;
  __m128f xmmARGBF;

  Face::m128iLoad8(xmmARGB64, argb64);
  Face::m128iRShiftPU32<8>(xmmARGB32, xmmARGB64);

  Face::m128iUnpackPI32FromPI16Lo(xmmARGB64, xmmARGB64);
  Face::m128iPackPU8FromPU16(xmmARGB32, xmmARGB32);

  Face::m128fCvtPSFromPI32(xmmARGBF, xmmARGB64);
  // Store Argb32.
  Face::m128iStore4(&engine->source.color->_argb32, xmmARGB32);

  Face::m128iUnpackPI16FromPI8Lo(xmmARGB32, xmmARGB32);
  Face::m128fMulPS(xmmARGBF, xmmARGBF, FOG_XMM_GET_CONST_PS(m128f_4x_1_div_65535));

  Face::m128iShufflePI16Lo<3, 3, 3, 3>(xmmAAAA32, xmmARGB32);
  Face::m128fShuffle<0, 1, 2, 3>(xmmARGBF, xmmARGBF);

  Face::m128iFillPBWi<3>(xmmARGB32, xmmARGB32);
  // Store ArgbF.
  Face::m128fStore16uLoHi(&engine->source.color->_data[0], xmmARGBF);

  Face::m128iMulDiv255PI16(xmmARGB32, xmmARGB32, xmmAAAA32);
  Face::m128fZero(xmmARGBF);
  Face::m128iPackPU8FromPU16(xmmARGB32, xmmARGB32);

  Face::m128fStore4(&engine->source.color->_data[4], xmmARGBF);
  engine->source.color->_model = COLOR_MODEL_ARGB;
  engine->source.color->_hints = NO_FLAGS;

  // Store Prgb32 (raster-solid context).
  Face::m128iStore4(&engine->ctx.solid.prgb32, xmmARGB32);

  return ERR_OK;
}

static err_t FOG_CDECL RasterPaintEngine_setSourceColor_byte_SSE2(Painter* self, const Color* color)
{
  RasterPaintEngine* engine = reinterpret_cast<RasterPaintEngine*>(self->_engine);
  FOG_ASSERT(engine->ctx.precision == IMAGE_PRECISION_BYTE);

  if (engine->sourceType != RASTER_SOURCE_COLOR)
  {
    engine->saveSourceAndDiscard();
    engine->sourceType = RASTER_SOURCE_COLOR;
    engine->ctx.pc = (RasterPattern*)(size_t)0x1;
  }
  else
  {
    engine->saveSourceColor();
  }

  __m128i xmm0;
  __m128i xmm1;
  __m128i xmm2;
  __m128i xmmAAAA32;
  __m128i xmmARGB32;

  Face::m128iLoad4(xmmARGB32, reinterpret_cast<const char*>(color) + 24);
  Face::m128iLoad8(xmm2, reinterpret_cast<const char*>(color) + 16);
  Face::m128iStore4(reinterpret_cast<char*>(&engine->source.color) + 24, xmmARGB32);

  Face::m128iUnpackPI16FromPI8Lo(xmmARGB32, xmmARGB32);

  Face::m128iLoad8(xmm1, reinterpret_cast<const char*>(color) +  8);
  Face::m128iShufflePI16Lo<3, 3, 3, 3>(xmmAAAA32, xmmARGB32);

  Face::m128iLoad8(xmm0, reinterpret_cast<const char*>(color) +  0);
  Face::m128iFillPBWi<3>(xmmARGB32, xmmARGB32);

  Face::m128iStore8(reinterpret_cast<char*>(&engine->source.color) +  0, xmm0);
  Face::m128iMulDiv255PI16(xmmARGB32, xmmARGB32, xmmAAAA32);

  Face::m128iStore8(reinterpret_cast<char*>(&engine->source.color) +  8, xmm1);
  Face::m128iPackPU8FromPU16(xmmARGB32, xmmARGB32);

  Face::m128iStore8(reinterpret_cast<char*>(&engine->source.color) + 16, xmm2);
  Face::m128iStore4(&engine->ctx.solid.prgb32, xmmARGB32);

  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void RasterPaintEngine_init_SSE2(void)
{
  PaintEngineVTable* v_byte = &RasterPaintEngine_vtable[IMAGE_PRECISION_BYTE];
  PaintEngineVTable* v_word = &RasterPaintEngine_vtable[IMAGE_PRECISION_WORD];

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  v_byte->setSourceArgb32 = RasterPaintEngine_setSourceArgb32_byte_SSE2;
  v_byte->setSourceArgb64 = RasterPaintEngine_setSourceArgb64_byte_SSE2;
  v_byte->setSourceColor = RasterPaintEngine_setSourceColor_byte_SSE2;
}

} // Fog namespace
