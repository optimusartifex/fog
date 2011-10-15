// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/FaceSSE.h>
#include <Fog/Core/Face/FaceSSE2.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::Color - SetData]
// ============================================================================

static err_t FOG_CDECL Color_setData_SSE2(Color* self, uint32_t modelExtended, const void* modelData)
{
  Face::m128f xmm0;
  Face::m128f xmm1;
  Face::m128i xmmI;
  Face::m128i xmmT;

  Face::m128fZero(xmm0);
  Face::m128fZero(xmm1);

  switch (modelExtended)
  {
    case COLOR_MODEL_NONE:
    {
      Face::m128fStore8Lo(reinterpret_cast<uint8_t*>(self) +  0, xmm0);
      Face::m128fStore8Lo(reinterpret_cast<uint8_t*>(self) +  8, xmm0);
      Face::m128fStore8Lo(reinterpret_cast<uint8_t*>(self) + 16, xmm1);
      Face::m128fStore4  (reinterpret_cast<uint8_t*>(self) + 24, xmm1);
      return ERR_OK;
    }

    case COLOR_MODEL_ACMYK:
      Face::m128fLoad4(xmm1, reinterpret_cast<const uint8_t*>(modelData) + 16);
      // ... Fall through ...

    case COLOR_MODEL_AHSV:
    case COLOR_MODEL_AHSL:
      Face::m128fLoad16uLoHi(xmm0, modelData);

      Face::m128fStore16uLoHi(&self->_data[0], xmm0);
      Face::m128fStore4(&self->_data[4], xmm1);

      self->_model = modelExtended;
      self->_hints = NO_FLAGS;

      fog_api.color_convert[_COLOR_MODEL_ARGB32][modelExtended](&self->_argb32, self->_data);
      return ERR_OK;

    case COLOR_MODEL_ARGB:
      Face::m128fLoad16uLoHi(xmm0, modelData);
      Face::m128fStore4(&self->_data[4], xmm1);
      Face::m128fStore16uLoHi(&self->_data[0], xmm0);

      Face::m128fMulPS(xmm0, xmm0, FOG_XMM_GET_CONST_PS(m128f_4x_255));
      Face::m128iCvtPI32FromPS(xmmI, xmm0);

      Face::m128iSwapPI32(xmmI, xmmI);
      Face::m128iPackPU8FromPI32(xmmI, xmmI);

      self->_model = COLOR_MODEL_ARGB;
      self->_hints = NO_FLAGS;
      Face::m128iStore4(&self->_argb32, xmmI);
      return ERR_OK;

    case _COLOR_MODEL_ARGB32:
      Face::m128iLoad4(xmmI, modelData);
      Face::m128iUnpackPI32FromPI8Lo(xmmT, xmmI);
      Face::m128fCvtPSFromPI32(xmm0, xmmT);
      Face::m128fMulPS(xmm0, xmm0, FOG_XMM_GET_CONST_PS(m128f_4x_1_div_255));
      goto _SetARGB;

    case _COLOR_MODEL_ARGB64:
      Face::m128iLoad8(xmmI, modelData);
      Face::m128iUnpackPI32FromPI16Lo(xmmT, xmmI);
      Face::m128iRShiftPU32<8>(xmmI, xmmI);
      Face::m128fCvtPSFromPI32(xmm0, xmmT);
      Face::m128iPackPU8FromPU16(xmmI, xmmI);
      Face::m128fMulPS(xmm0, xmm0, FOG_XMM_GET_CONST_PS(m128f_4x_1_div_65535));
_SetARGB:
      Face::m128fShuffle<0, 1, 2, 3>(xmm0, xmm0);
      Face::m128fStore16uLoHi(&self->_data[0], xmm0);
      Face::m128fStore4(&self->_data[4], xmm1);
      self->_model = COLOR_MODEL_ARGB;
      self->_hints = NO_FLAGS;
      Face::m128iStore4(&self->_argb32, xmmI);
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Color_init_SSE2(void)
{
  fog_api.color_setData = Color_setData_SSE2;
}

} // Fog namespace
