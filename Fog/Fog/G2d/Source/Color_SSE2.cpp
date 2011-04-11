// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Guard]
#include <Fog/Core/Config/Config.h>
#if defined(FOG_OPTIMIZE_SSE2)

// [Dependencies]
#include <Fog/Core/Face/Face_SSE.h>
#include <Fog/Core/Face/Face_SSE2.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Tools/Byte.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Global/Api.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::Color - SetData]
// ============================================================================

static err_t FOG_CDECL _G2d_Color_setData_SSE2(Color& self, uint32_t modelExtended, const void* modelData)
{
  Face::m128f xmm0;
  Face::m128f xmm1;

  Face::m128fZero(xmm0);
  Face::m128fZero(xmm1);

  switch (modelExtended)
  {
    case COLOR_MODEL_NONE:
    {
      Face::m128fStore8Lo(reinterpret_cast<uint8_t*>(&self) +  0, xmm0);
      Face::m128fStore8Lo(reinterpret_cast<uint8_t*>(&self) +  8, xmm0);
      Face::m128fStore8Lo(reinterpret_cast<uint8_t*>(&self) + 16, xmm1);
      Face::m128fStore4  (reinterpret_cast<uint8_t*>(&self) + 24, xmm1);
      return ERR_OK;
    }

    case COLOR_MODEL_ACMYK:
      Face::m128fLoad4(xmm1, reinterpret_cast<const uint8_t*>(modelData) + 16);
      // ... Fall through ...

    case COLOR_MODEL_AHSV:
    case COLOR_MODEL_AHSL:
    {
      Face::m128fLoad16uLoHi(xmm0, reinterpret_cast<const uint8_t*>(modelData) +  0);

      Face::m128fStore16uLoHi(&self._data[0], xmm0);
      Face::m128fStore4(&self._data[4], xmm1);

      self._model = modelExtended;
      self._hints = NO_FLAGS;

      _g2d.color.convert[_COLOR_MODEL_ARGB32][modelExtended](&self._argb32, self._data);
      return ERR_OK;
    }

    case COLOR_MODEL_ARGB:
    {
      Face::m128i xmmI;

      Face::m128fLoad16uLoHi(xmm0, reinterpret_cast<const uint8_t*>(modelData) +  0);

      Face::m128fStore16uLoHi(&self._data[0], xmm0);
      Face::m128fStore4(&self._data[4], xmm1);

      Face::m128fMulPS(xmm0, xmm0, FOG_SSE_GET_CONST_PS(m128f_to_byte));
      Face::m128iCvtPI32FromPS(xmmI, xmm0);

      Face::m128iSwapPI32(xmmI, xmmI);
      Face::m128iPackPU8FromPI32(xmmI, xmmI);

      self._model = COLOR_MODEL_ARGB;
      self._hints = NO_FLAGS;
      Face::m128iStore4(&self._argb32, xmmI);

      return ERR_OK;
    }

    case _COLOR_MODEL_ARGB32:
    {
      Face::m128i xmmI;

      Face::m128iLoad4(xmmI, modelData);
      Face::m128iUnpackPI16FromPI8Lo(xmmI, xmmI);

      Face::m128iUnpackPI32FromPI16Lo(xmmI, xmmI);
      Face::m128fCvtPSFromPI32(xmm0, xmmI);
      Face::m128fShuffle<0, 1, 2, 3>(xmm0, xmm0);
      Face::m128fMulPS(xmm0, xmm0, FOG_SSE_GET_CONST_PS(m128f_from_byte));
      Face::m128iPackPU8FromPI32(xmmI, xmmI);

      Face::m128fStore16uLoHi(&self._data[0], xmm0);
      Face::m128fStore4(&self._data[4], xmm1);
      self._model = COLOR_MODEL_ARGB;
      self._hints = NO_FLAGS;
      Face::m128iStore4(&self._argb32, xmmI);

      return ERR_OK;
    }

    case _COLOR_MODEL_ARGB64:
    {
      Face::m128i xmmI;

      Face::m128iLoad8(xmmI, modelData);
      Face::m128iUnpackPI32FromPI16Lo(xmmI, xmmI);
      Face::m128fCvtPSFromPI32(xmm0, xmmI);

      Face::m128iSwapPI32(xmmI, xmmI);
      Face::m128iRShiftLogPI32<8>(xmmI, xmmI);
      Face::m128fMulPS(xmm0, xmm0, FOG_SSE_GET_CONST_PS(m128f_from_word));
      Face::m128iPackPU8FromPI32(xmmI, xmmI);

      Face::m128fStore16uLoHi(&self._data[0], xmm0);
      Face::m128fStore4(&self._data[4], xmm1);
      self._model = COLOR_MODEL_ARGB;
      self._hints = NO_FLAGS;
      Face::m128iStore4(&self._argb32, xmmI);

      return ERR_OK;
    }

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_color_init_sse2(void)
{
  _g2d.color.setData = _G2d_Color_setData_SSE2;
}

} // Fog namespace

// [Guard]
#endif // FOG_OPTIMIZE_SSE2
