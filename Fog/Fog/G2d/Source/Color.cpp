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
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/G2d/Face/Face_Raster_C.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::Color - Convert - Generic]
// ============================================================================

static void FOG_CDECL Color_zero4f(float* dst, const float* src)
{
  dst[0] = 0.0f;
  dst[1] = 0.0f;
  dst[2] = 0.0f;
  dst[3] = 0.0f;
}

static void FOG_CDECL Color_zero5f(float* dst, const float* src)
{
  dst[0] = 0.0f;
  dst[1] = 0.0f;
  dst[2] = 0.0f;
  dst[3] = 0.0f;
  dst[4] = 0.0f;
}

static void FOG_CDECL Color_copy4f(float* dst, const float* src)
{
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
}

static void FOG_CDECL Color_copy5f(float* dst, const float* src)
{
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  dst[3] = src[3];
  dst[4] = src[4];
}

// ============================================================================
// [Fog::Color - Convert - ArgbF]
// ============================================================================

static void FOG_CDECL Color_argbf_from_argb32(float* dst, const ArgbBase32* src)
{
  ColorUtil::argbfFromArgb32(dst, *src);
}

static void FOG_CDECL Color_argbf_from_argb64(float* dst, const ArgbBase64* src)
{
  ColorUtil::argbfFromArgb64(dst, *src);
}

static void FOG_CDECL Color_argbf_from_ahsvf(float* dst, const float* src)
{
  float a = src[0];
  float h = src[1];
  float s = src[2];
  float v = src[3];

  dst[0] = a;

  // The HUE should be already at range [0, 1], convert 1.0 to 0.0.
  if (h >= 1.0f) h -= 1.0f;

  h *= 6.0f;
  int i = (int)Math::ufloor(h);

  float f = h - (float)i;
  float p = v * (1.0f - s);
  float q = v * (1.0f - s * f);
  float t = v * (1.0f - s * (1.0f - f));

  switch (i)
  {
    case 0: dst[1] = v; dst[2] = t; dst[3] = p; break;
    case 1: dst[1] = q; dst[2] = v; dst[3] = p; break;
    case 2: dst[1] = p; dst[2] = v; dst[3] = t; break;
    case 3: dst[1] = p; dst[2] = q; dst[3] = v; break;
    case 4: dst[1] = t; dst[2] = p; dst[3] = v; break;
    case 5: dst[1] = v; dst[2] = p; dst[3] = q; break;
  }
}

static void FOG_CDECL Color_argbf_from_ahslf(float* dst, const float* src)
{
  float a = src[0];
  float h = src[1];
  float s = src[2];
  float l = src[3];

  dst[0] = a;

  // Handle the achromatic case and zero lightness.
  if (FOG_UNLIKELY(Math::isFuzzyZero(s)) ||
      FOG_UNLIKELY(Math::isFuzzyZero(l)))
  {
    dst[1] = l;
    dst[2] = l;
    dst[3] = l;
    return;
  }

  // The HUE should be already at range [0, 1], convert 1.0 to 0.0.
  if (h >= 1.0f) h -= 1.0f;

  float hArray[3];
  float p;
  float q;
  float t;

  p = h + (1.0f / 3.0f);
  q = h - (1.0f / 3.0f);

  if (p > 1.0f) p -= 1.0f;
  if (q < 0.0f) q += 1.0f;

  hArray[0] = p;
  hArray[1] = h;
  hArray[2] = q;

  q = (l < 0.5f) ? l * (1.0f + s) : l + s - l * s;
  p = 2.0f * l - q;

  // Cache this computation.
  t = (q - p) * 6.0f;

  // Adjust DST so it will start at RGB (to match hArray).
  dst++;

  for (int i = 0; i < 3; i++)
  {
    float c;
    h = hArray[i];

    // 0.00 -> 0.16
    if (h < (1.0f / 6.0f)) c = p + t * h;
    // 0.16 -> 0.50
    else if (h < (1.0f / 2.0f)) c = q;
    // 0.50 -> 0.66
    else if (h < (2.0f / 3.0f)) c = p + t * ((2.0f / 3.0f) - h);
    // 0.66 -> 1.00
    else c = p;

    dst[i] = c;
  }
}

static void FOG_CDECL Color_argbf_from_acmykf(float* dst, const float* src)
{
  float a = src[0];
  float kinv = 1.0f - src[4];

  dst[0] = a;
  dst[1] = kinv - src[1] * kinv;
  dst[2] = kinv - src[2] * kinv;
  dst[3] = kinv - src[3] * kinv;
}

// ============================================================================
// [Fog::Color - Convert - AhsvF]
// ============================================================================

static void FOG_CDECL Color_ahsvf_from_argbf(float* dst, const float* src)
{
  float a = src[0];
  float r = src[1];
  float g = src[2];
  float b = src[3];

  float minimum = Math::min(r, g, b);
  float maximum = Math::max(r, g, b);
  float delta = maximum - minimum;

  float h = 0.0f;
  float s = 0.0f;
  float v = maximum;

  if (Math::isFuzzyPositiveNumber(delta))
  {
    float recip = (1.0f / 6.0f) / delta;
    s = (float)(delta) / (float)maximum;

    if (maximum == r)
      h = 6.0f * delta + g - b;
    else if (maximum == g)
      h = 2.0f * delta + b - r;
    else
      h = 4.0f * delta + r - g;

    h *= recip;
    if (h > 1.0f) h -= 1.0f;
  }

  dst[0] = a;
  dst[1] = h;
  dst[2] = s;
  dst[3] = v;
}

static void FOG_CDECL Color_ahsvf_from_ahslf(float* dst, const float* src)
{
  float a = src[0];
  float h = src[1];
  float s = src[2];
  float l = src[3];

  dst[0] = a;
  dst[1] = h;

  if (l <= 0.5f)
  {
    float p = s + 1.0f;

    dst[2] = 2.0f * s / p;
    dst[3] = l * p;
  }
  else
  {
    s *= 1.0f - l;
    l += s;
    s *= 2.0f;

    dst[2] = s / l;
    dst[3] = l;
  }
}

static void FOG_CDECL Color_ahsvf_from_acmykf(float* dst, const float* src)
{
  float argb[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](argb, src);
  fog_api.color_convert[COLOR_MODEL_AHSV][COLOR_MODEL_ARGB ](dst, argb);
}

static void FOG_CDECL Color_ahsvf_from_argb32(float* dst, const ArgbBase32* src)
{
  float argbf[4];
  ColorUtil::argbfFromArgb32(argbf, *src);
  fog_api.color_convert[COLOR_MODEL_AHSV][COLOR_MODEL_ARGB](dst, argbf);
}

static void FOG_CDECL Color_ahsvf_from_argb64(float* dst, const ArgbBase64* src)
{
  float argbf[4];
  ColorUtil::argbfFromArgb64(argbf, *src);
  fog_api.color_convert[COLOR_MODEL_AHSV][COLOR_MODEL_ARGB](dst, argbf);
}

// ============================================================================
// [Fog::Color - Convert - AhslF]
// ============================================================================

static void FOG_CDECL Color_ahslf_from_argbf(float* dst, const float* src)
{
  float a = src[0];
  float r = src[1];
  float g = src[2];
  float b = src[3];

  float minimum = Math::min(r, g, b);
  float maximum = Math::max(r, g, b);
  float delta = maximum - minimum;

  float sum = maximum + minimum;

  float h = 0.0f;
  float s = 0.0f;
  float l = sum * 0.5f;

  if (Math::isFuzzyPositiveNumber(delta))
  {
    if (l > 0.5f) sum = 2.0f - sum;
    s = delta / sum;

    if (maximum == r)
      h = (g - b) / delta + (g < b) ? 6.0f : 0.0f;
    else if (maximum == g)
      h = (b - r) / delta + (2.0f);
    else
      h = (r - g) / delta + (4.0f);

    h *= (1.0f / 6.0f);
  }

  dst[0] = a;
  dst[1] = h;
  dst[2] = s;
  dst[3] = l;
}

static void FOG_CDECL Color_ahslf_from_ahsvf(float* dst, const float* src)
{
  float a = src[0];
  float h = src[1];
  float s = src[2];
  float v = src[3];

  dst[0] = a;
  dst[1] = h;

  float p = 2.0f - s;
  float q = p * v;

  if (q <= 1.0f)
  {
    dst[2] = s / p;
    dst[3] = 0.5f * q;
  }
  else
  {
    dst[2] = (s * v) / (2.0f - q);
    dst[3] = 0.5f * q;
  }
}

static void FOG_CDECL Color_ahslf_from_acmykf(float* dst, const float* src)
{
  float argb[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](argb, src);
  fog_api.color_convert[COLOR_MODEL_AHSL][COLOR_MODEL_ARGB ](dst, argb);
}

static void FOG_CDECL Color_ahslf_from_argb32(float* dst, const ArgbBase32* src)
{
  float argbf[4];
  ColorUtil::argbfFromArgb32(argbf, *src);
  fog_api.color_convert[COLOR_MODEL_AHSL][COLOR_MODEL_ARGB](dst, argbf);
}

static void FOG_CDECL Color_ahslf_from_argb64(float* dst, const ArgbBase64* src)
{
  float argbf[4];
  ColorUtil::argbfFromArgb64(argbf, *src);
  fog_api.color_convert[COLOR_MODEL_AHSL][COLOR_MODEL_ARGB](dst, argbf);
}

// ============================================================================
// [Fog::Color - Convert - AcmykF]
// ============================================================================

static void FOG_CDECL Color_acmykf_from_argbf(float* dst, const float* src)
{
  float a = src[0];
  float c = 1.0f - src[1];
  float m = 1.0f - src[2];
  float y = 1.0f - src[3];
  float k = Math::min(c, m, y);

  if (Math::isFuzzyLe(k, 1.0f))
  {
    float recip = 1.0f / (1.0f - k);

    c -= k; c *= recip;
    m -= k; m *= recip;
    y -= k; y *= recip;
  }

  dst[0] = a;
  dst[1] = c;
  dst[2] = m;
  dst[3] = y;
  dst[4] = k;
}

static void FOG_CDECL Color_acmykf_from_ahsvf(float* dst, const float* src)
{
  float argb[4];
  fog_api.color_convert[COLOR_MODEL_ARGB ][COLOR_MODEL_AHSV](argb, src);
  fog_api.color_convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](dst, argb);
}

static void FOG_CDECL Color_acmykf_from_ahslf(float* dst, const float* src)
{
  float argb[4];
  fog_api.color_convert[COLOR_MODEL_ARGB ][COLOR_MODEL_AHSL](argb, src);
  fog_api.color_convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](dst, argb);
}

static void FOG_CDECL Color_acmykf_from_argb32(float* dst, const ArgbBase32* src)
{
  float argbf[4];
  ColorUtil::argbfFromArgb32(argbf, *src);
  fog_api.color_convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](dst, argbf);
}

static void FOG_CDECL Color_acmykf_from_argb64(float* dst, const ArgbBase64* src)
{
  float argbf[4];
  ColorUtil::argbfFromArgb64(argbf, *src);
  fog_api.color_convert[COLOR_MODEL_ACMYK][COLOR_MODEL_ARGB](dst, argbf);
}

// ============================================================================
// [Fog::Color - Convert - Argb32]
// ============================================================================

static void FOG_CDECL Color_argb32_zero(ArgbBase32* dst, const ArgbBase32* src)
{
  FOG_UNUSED(src);
  dst->u32 = 0;
}

static void FOG_CDECL Color_argb32_from_argbf(ArgbBase32* dst, const ArgbBaseF* src)
{
  ColorUtil::argb32FromF(*dst, src->data);
}

static void FOG_CDECL Color_argb32_from_ahsvf(ArgbBase32* dst, const float* src)
{
  float argbf[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSV](argbf, src);
  ColorUtil::argb32FromF(*dst, argbf);
}

static void FOG_CDECL Color_argb32_from_ahslf(ArgbBase32* dst, const float* src)
{
  float argbf[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSL](argbf, src);
  ColorUtil::argb32FromF(*dst, argbf);
}


static void FOG_CDECL Color_argb32_from_acmykf(ArgbBase32* dst, const float* src)
{
  float argbf[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](argbf, src);
  ColorUtil::argb32FromF(*dst, argbf);
}

static void FOG_CDECL Color_argb32_from_argb32(ArgbBase32* dst, const ArgbBase32* src)
{
  dst->u32 = src->u32;
}

static void FOG_CDECL Color_argb32_from_argb64(ArgbBase32* dst, const ArgbBase64* src)
{
  ColorUtil::argb32From64(*dst, *src);
}

// ============================================================================
// [Fog::Color - Convert - Argb64]
// ============================================================================

static void FOG_CDECL Color_argb64_zero(ArgbBase64* dst, const ArgbBase32* src)
{
  FOG_UNUSED(src);
  dst->u64 = 0;
}

static void FOG_CDECL Color_argb64_from_argbf(ArgbBase64* dst, const ArgbBaseF* src)
{
  ColorUtil::argb64FromF(*dst, src->data);
}

static void FOG_CDECL Color_argb64_from_ahsvf(ArgbBase64* dst, const float* src)
{
  float argbf[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSV](argbf, src);
  ColorUtil::argb64FromF(*dst, argbf);
}

static void FOG_CDECL Color_argb64_from_ahslf(ArgbBase64* dst, const float* src)
{
  float argbf[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_AHSL](argbf, src);
  ColorUtil::argb64FromF(*dst, argbf);
}


static void FOG_CDECL Color_argb64_from_acmykf(ArgbBase64* dst, const float* src)
{
  float argbf[4];
  fog_api.color_convert[COLOR_MODEL_ARGB][COLOR_MODEL_ACMYK](argbf, src);
  ColorUtil::argb64FromF(*dst, argbf);
}

static void FOG_CDECL Color_argb64_from_argb32(ArgbBase64* dst, const ArgbBase32* src)
{
  ColorUtil::argb64From32(*dst, *src);
}

static void FOG_CDECL Color_argb64_from_argb64(ArgbBase64* dst, const ArgbBase64* src)
{
  dst->u64 = src->u64;
}

// ============================================================================
// [Fog::Color - SetModel]
// ============================================================================

static err_t FOG_CDECL Color_setModel(Color* self, uint32_t model)
{
  uint32_t oldModel = self->_model;

  if (model == oldModel) return ERR_OK;
  if (model == COLOR_MODEL_NONE) goto _None;

  if (FOG_UNLIKELY(model >= COLOR_MODEL_COUNT)) return ERR_RT_INVALID_ARGUMENT;

  fog_api.color_convert[model][oldModel](self->_data, self->_data);
  self->_model = model;
  return ERR_OK;

_None:
  self->reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::Color - SetData]
// ============================================================================

static err_t FOG_CDECL Color_setData(Color* self, uint32_t modelExtended, const void* modelData)
{
  self->_model = modelExtended;
  self->_hints = NO_FLAGS;
  self->_data[4] = 0.0f;

  switch (modelExtended)
  {
    case COLOR_MODEL_NONE:
      self->reset();
      return ERR_OK;

    case COLOR_MODEL_ACMYK:
      self->_data[4] = reinterpret_cast<const float*>(modelData)[4];

    case COLOR_MODEL_ARGB:
    case COLOR_MODEL_AHSV:
    case COLOR_MODEL_AHSL:
      self->_data[0] = reinterpret_cast<const float*>(modelData)[0];
      self->_data[1] = reinterpret_cast<const float*>(modelData)[1];
      self->_data[2] = reinterpret_cast<const float*>(modelData)[2];
      self->_data[3] = reinterpret_cast<const float*>(modelData)[3];

      fog_api.color_convert[_COLOR_MODEL_ARGB32][modelExtended](&self->_argb32, self->_data);
      return ERR_OK;

    case _COLOR_MODEL_ARGB32:
      self->_model = COLOR_MODEL_ARGB;
      self->_argb32 = *reinterpret_cast<const ArgbBase32*>(modelData);

      ColorUtil::argbfFromArgb32(self->_data, *reinterpret_cast<const ArgbBase32*>(modelData));
      return ERR_OK;

    case _COLOR_MODEL_ARGB64:
      self->_model = COLOR_MODEL_ARGB;
      ColorUtil::argbfFromArgb64(self->_data, *reinterpret_cast<const ArgbBase64*>(modelData));
      ColorUtil::argb32From64(self->_argb32, *reinterpret_cast<const ArgbBase64*>(modelData));
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::Color - Mix]
// ============================================================================

struct FOG_NO_EXPORT ColorMix_C
{
  static FOG_INLINE float blend     (float S0, float S1, float M, float MI) { return (S1 - S0)*M; }
  static FOG_INLINE float add       (float S0, float S1, float M, float MI) { return S0 + S1*M; }
  static FOG_INLINE float subtract  (float S0, float S1, float M, float MI) { return S0 - S1*M; }
  static FOG_INLINE float multiply  (float S0, float S1, float M, float MI) { return (S0 * (MI + S1*M)); }
  static FOG_INLINE float divide    (float S0, float S1, float M, float MI) { return Math::isFuzzyZero(S1) ? S0 : S0*MI + S0*M / S1; }
  static FOG_INLINE float screen    (float S0, float S1, float M, float MI) { return 1.0f - (MI + (1.0f - S1)*M) * (1.0f - S0); }
  static FOG_INLINE float overlay   (float S0, float S1, float M, float MI) { return S0 < 0.5f ? S0 * (MI + 2.0f * S1*M) : 1.0f - (MI + 2.0f * (1.0f - S1)*M) * (1.0f - S0); }
  static FOG_INLINE float darken    (float S0, float S1, float M, float MI) { return Math::min(S0, S1*M); }
  static FOG_INLINE float lighten   (float S0, float S1, float M, float MI) { return Math::max(S0, S1*M); }
  static FOG_INLINE float dodge     (float S0, float S1, float M, float MI) { float D = S1*M     ; return (D <= 0.0f) ? 1.0f : Math::min(S0 / D, 1.0f); }
  static FOG_INLINE float burn      (float S0, float S1, float M, float MI) { float D = S1*M + MI; return (D <= 0.0f) ? 0.0f : Math::bound(D, 0.0f, 1.0f); }
  static FOG_INLINE float difference(float S0, float S1, float M, float MI) { return MI * S0 + Math::abs(S0 - S1)*M; }
};

static err_t FOG_CDECL Color_mix(Color* self, uint32_t mixOp, uint32_t alphaOp, const Color* secondary, float mask)
{
  // Source models.
  uint32_t m0 = self->_model;
  uint32_t m1 = secondary->_model;

  // Source colors.
  const float* s0 = self->_data;
  const float* s1 = secondary->_data;

  // Destination color.
  float d[4];

  // Temporary.
  float tmp[8];

  // Convert the primary/secondary colors into the ARGB color-space.
  if (FOG_UNLIKELY(m0 != COLOR_MODEL_ARGB)) { fog_api.color_convert[COLOR_MODEL_ARGB][m0](tmp + 0, s0); s0 = tmp + 0; }
  if (FOG_UNLIKELY(m1 != COLOR_MODEL_ARGB)) { fog_api.color_convert[COLOR_MODEL_ARGB][m1](tmp + 4, s1); s1 = tmp + 4; }

  float M = mask;
  float MI = 1.0f;

  if (alphaOp == COLOR_MIX_ALPHA_MASK) M *= s1[0];
  MI -= M;

#define _MIX_FN(_Expression_) \
  { \
    float S0, S1; \
    S0 = s0[0]; S1 = s1[0]; d[0] = _Expression_; \
    S0 = s0[1]; S1 = s1[1]; d[1] = _Expression_; \
    S0 = s0[2]; S1 = s1[2]; d[2] = _Expression_; \
    S0 = s0[3]; S1 = s1[3]; d[3] = _Expression_; \
  }

  switch (mixOp)
  {
    case COLOR_MIX_OP_NONE:
      return ERR_OK;

    case COLOR_MIX_OP_BLEND     : _MIX_FN( ColorMix_C::blend     (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_ADD       : _MIX_FN( ColorMix_C::add       (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_SUBTRACT  : _MIX_FN( ColorMix_C::subtract  (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_MULTIPLY  : _MIX_FN( ColorMix_C::multiply  (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_DIVIDE    : _MIX_FN( ColorMix_C::divide    (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_SCREEN    : _MIX_FN( ColorMix_C::screen    (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_OVERLAY   : _MIX_FN( ColorMix_C::overlay   (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_DARKEN    : _MIX_FN( ColorMix_C::darken    (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_LIGHTEN   : _MIX_FN( ColorMix_C::lighten   (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_DODGE     : _MIX_FN( ColorMix_C::dodge     (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_BURN      : _MIX_FN( ColorMix_C::burn      (S0, S1, M, MI) ); break;
    case COLOR_MIX_OP_DIFFERENCE: _MIX_FN( ColorMix_C::difference(S0, S1, M, MI) ); break;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (alphaOp == COLOR_MIX_ALPHA_COMPONENT) self->_data[0] = d[0];
  self->_data[1] = d[1];
  self->_data[2] = d[2];
  self->_data[3] = d[3];

  ColorUtil::argb32FromF(self->_argb32, self->_data);
  if (FOG_UNLIKELY(m0 != COLOR_MODEL_ARGB)) { fog_api.color_convert[m0][COLOR_MODEL_ARGB](self->_data, self->_data); }

  return ERR_OK;
}

// ============================================================================
// [Fog::Color - Adjust]
// ============================================================================

static err_t FOG_CDECL Color_adjust(Color* self, uint32_t adjustOp, float param)
{
  const float* argb = NULL;       // ARGB components (if used).
  uint32_t sModel = self->_model; // Source color model.
  uint32_t dModel;                // Working color model.
  float d[4];                     // Working data.

  if (sModel == COLOR_MODEL_ARGB) argb = self->_data;

  switch (adjustOp)
  {
    case COLOR_ADJUST_OP_NONE:
      return ERR_OK;

    case COLOR_ADJUST_OP_LIGHTEN:
    {
      if (param < 0.0f) return ERR_RT_INVALID_ARGUMENT;

      dModel = COLOR_MODEL_AHSV;
      fog_api.color_convert[dModel][sModel](d, self->_data);

      float s = d[2];
      float v = d[3] / param;

      if (v > 1.0f)
      {
        s = Math::max(1.0f + s - v, 0.0f);
        v = 1.0f;
      }

      d[2] = s;
      d[3] = v;
      break;
    }

    case COLOR_ADJUST_OP_DARKEN:
    {
      if (param < 0.0f) return ERR_RT_INVALID_ARGUMENT;

      dModel = COLOR_MODEL_AHSV;
      fog_api.color_convert[dModel][sModel](d, self->_data);

      if (Math::isFuzzyZero(param))
      {
        d[2] = 0.0f;
        d[3] = 1.0f;
        break;
      }

      float s = d[2];
      float v = d[3] / param;

      if (v > 1.0f)
      {
        s = Math::max(1.0f + s - v, 0.0f);
        v = 1.0f;
      }

      d[2] = s;
      d[3] = v;
      break;
    }

    case COLOR_ADJUST_OP_HUE:
    {
      dModel = COLOR_MODEL_AHSV;
      fog_api.color_convert[dModel][sModel](d, self->_data);

      param = Math::positiveFraction(d[1] + param);
      break;
    }

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  // Convert color from the working color model into the source color model.
  fog_api.color_convert[sModel][dModel](self->_data, d);

  // Update the ARGB32 color, using ARGB color model (the fastest way) if possible.
  if (argb)
    ColorUtil::argb32FromF(self->_argb32, argb);
  else
    fog_api.color_convert[sModel][dModel](&self->_argb32, d);
  return ERR_OK;
}

// ============================================================================
// [Fog::Color - Parse / Serialize]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
struct ColorName
{
  char name[22];

  uint32_t r : 8;
  uint32_t g : 8;
  uint32_t b : 8;
  uint32_t flags : 8;
};
#include <Fog/Core/C++/PackRestore.h>

enum COLOR_ENTITY_FLAGS
{
  COLOR_ENTITY_CSS_BASIC = 0,
  COLOR_ENTITY_CSS_EXTENDED = 1,
  COLOR_ENTITY_CSS_TRANSPARENT = 2,
  COLOR_ENTITY_CSS_NONE = 3
};

#define _COLOR_ENTITY(_Name_, _R_, _G_, _B_, _Flags_) \
  { _Name_, _R_, _G_, _B_, _Flags_ }

static const ColorName _colorNames[] =
{
  _COLOR_ENTITY("aliceblue",           240, 248, 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("antiquewhite",        250, 235, 215, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("aqua",                0  , 255, 255, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("aquamarine",          127, 255, 212, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("azure",               240, 255, 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("beige",               245, 245, 220, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("bisque",              255, 228, 196, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("black",               0  , 0,   0  , COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("blanchedalmond",      255, 235, 205, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("blue",                0  , 0  , 255, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("blueviolet",          138, 43 , 226, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("brown",               165, 42 , 42 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("burlywood",           222, 184, 135, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("cadetblue",           95 , 158, 160, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("chartreuse",          127, 255, 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("chocolate",           210, 105, 30 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("coral",               255, 127, 80 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("cornflowerblue",      100, 149, 237, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("cornsilk",            255, 248, 220, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("crimson",             220, 20 , 60 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("cyan",                0  , 255, 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkblue",            0  , 0  , 139, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkcyan",            0  , 139, 139, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkgoldenrod",       184, 134, 11 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkgray",            169, 169, 169, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkgreen",           0  , 100, 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkgrey",            169, 169, 169, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkkhaki",           189, 183, 107, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkmagenta",         139, 0  , 139, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkolivegreen",      85 , 107, 47 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkorange",          255, 140, 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkorchid",          153, 50 , 204, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkred",             139, 0  , 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darksalmon",          233, 150, 122, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkseagreen",        143, 188, 143, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkslateblue",       72 , 61 , 139, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkslategray",       47 , 79 , 79 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkslategrey",       47 , 79 , 79 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkturquoise",       0  , 206, 209, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("darkviolet",          148, 0  , 211, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("deeppink",            255, 20 , 147, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("deepskyblue",         0  , 191, 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("dimgray",             105, 105, 105, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("dimgrey",             105, 105, 105, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("dodgerblue",          30 , 144, 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("firebrick",           178, 34 , 34 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("floralwhite",         255, 250, 240, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("forestgreen",         34 , 139, 34 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("fuchsia",             255, 0  , 255, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("gainsboro",           220, 220, 220, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("ghostwhite",          248, 248, 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("gold",                255, 215, 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("goldenrod",           218, 165, 32 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("gray",                128, 128, 128, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("green",               0  , 128, 0  , COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("greenyellow",         173, 255, 47 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("grey",                128, 128, 128, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("honeydew",            240, 255, 240, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("hotpink",             255, 105, 180, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("indianred",           205, 92 , 92 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("indigo",              75 , 0  , 130, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("ivory",               255, 255, 240, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("khaki",               240, 230, 140, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lavender",            230, 230, 250, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lavenderblush",       255, 240, 245, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lawngreen",           124, 252, 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lemonchiffon",        255, 250, 205, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightblue",           173, 216, 230, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightcoral",          240, 128, 128, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightcyan",           224, 255, 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightgoldenrodyellow",250, 250, 210, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightgray",           211, 211, 211, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightgreen",          144, 238, 144, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightgrey",           211, 211, 211, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightpink",           255, 182, 193, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightsalmon",         255, 160, 122, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightseagreen",       32 , 178, 170, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightskyblue",        135, 206, 250, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightslategray",      119, 136, 153, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightslategrey",      119, 136, 153, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightsteelblue",      176, 196, 222, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lightyellow",         255, 255, 224, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("lime",                0  , 255, 0  , COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("limegreen",           50 , 205, 50 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("linen",               250, 240, 230, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("magenta",             255, 0  , 255, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("maroon",              128, 0  , 0  , COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("mediumaquamarine",    102, 205, 170, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumblue",          0  , 0  , 205, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumorchid",        186, 85 , 211, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumpurple",        147, 112, 219, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumseagreen",      60 , 179, 113, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumslateblue",     123, 104, 238, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumspringgreen",   0,   250, 154, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumturquoise",     72 , 209, 204, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mediumvioletred",     199, 21 , 133, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("midnightblue",        25 , 25 , 112, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mintcream",           245, 255, 250, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("mistyrose",           255, 228, 225, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("moccasin",            255, 228, 181, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("navajowhite",         255, 222, 173, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("navy",                0  , 0  , 128, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("oldlace",             253, 245, 230, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("olive",               128, 128, 0  , COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("olivedrab",           107, 142, 35 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("orange",              255, 165, 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("orangered",           255, 69 , 0  , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("orchid",              218, 112, 214, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("palegoldenrod",       238, 232, 170, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("palegreen",           152, 251, 152, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("paleturquoise",       175, 238, 238, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("palevioletred",       219, 112, 147, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("papayawhip",          255, 239, 213, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("peachpuff",           255, 218, 185, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("peru",                205, 133, 63 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("pink",                255, 192, 203, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("plum",                221, 160, 221, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("powderblue",          176, 224, 230, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("purple",              128, 0  , 128, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("red",                 255, 0  , 0  , COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("rosybrown",           188, 143, 143, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("royalblue",           65 , 105, 225, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("saddlebrown",         139, 69 , 19 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("salmon",              250, 128, 114, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("sandybrown",          244, 164, 96 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("seagreen",            46 , 139, 87 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("seashell",            255, 245, 238, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("sienna",              160, 82 , 45 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("silver",              192, 192, 192, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("skyblue",             135, 206, 235, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("slateblue",           106, 90 , 205, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("slategray",           112, 128, 144, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("slategrey",           112, 128, 144, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("snow",                255, 250, 250, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("springgreen",         0  , 255, 127, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("steelblue",           70 , 130, 180, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("tan",                 210, 180, 140, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("teal",                0  , 128, 128, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("thistle",             216, 191, 216, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("tomato",              255, 99 , 71 , COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("transparent",         0  , 0  , 0  , COLOR_ENTITY_CSS_TRANSPARENT),
  _COLOR_ENTITY("turquoise",           64 , 224, 208, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("violet",              238, 130, 238, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("wheat",               245, 222, 179, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("white",               255, 255, 255, COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("whitesmoke",          245, 245, 245, COLOR_ENTITY_CSS_EXTENDED   ),
  _COLOR_ENTITY("yellow",              255, 255, 0  , COLOR_ENTITY_CSS_BASIC      ),
  _COLOR_ENTITY("yellowgreen",         154, 205, 50 , COLOR_ENTITY_CSS_EXTENDED   )
};
#undef _COLOR_ENTITY

static uint32_t Color_singleHexToComponent(uint32_t c0)
{
  uint32_t h0 = (c0 <= '9') ? c0 - '0' : 9 + (c0 & 0xF);
  FOG_ASSERT(h0 <= 0xF);

  return h0 * 0x11;
}

static uint32_t Color_doubleHexToComponent(uint32_t c0, uint32_t c1)
{
  uint32_t h0 = (c0 <= '9') ? c0 - '0' : 9 + (c0 & 0xF);
  uint32_t h1 = (c1 <= '9') ? c1 - '0' : 9 + (c1 & 0xF);
  FOG_ASSERT(h0 <= 0xF);
  FOG_ASSERT(h1 <= 0xF);

  return (h0 << 4) + h1;
}

static const ColorName* Color_find(const StubA& key)
{
  const ColorName* base = _colorNames;
  size_t i, lim;

  const char* keyName = key.getData();
  size_t keyLength = key.getLength();

  FOG_ASSERT(keyLength != DETECT_LENGTH);
  if (keyLength == 0) return NULL;

  for (lim = FOG_ARRAY_SIZE(_colorNames); lim != 0; lim >>= 1)
  {
    const ColorName* cur = base + (lim >> 1);
    uint8_t c0;
    uint8_t c1;

    for (i = 0;;)
    {
      c0 = (uint8_t)cur->name[i];
      c1 = (uint8_t)keyName[i];
      if (c0 != c1) break;

      if (++i < keyLength) continue;

      c0 = (uint8_t)cur->name[i];
      c1 = 0;

      // Match.
      if (c0 != c1) break;
      return cur;
    }

    if (c0 < c1)
    {
      base = cur + 1;
      lim--;
      continue;
    }
    else
    {
      continue;
    }
  }

  return NULL;
}

template<typename CharT>
static err_t FOG_CDECL Color_parse(Color* dst, const CharT_(Stub)* str, uint32_t flags)
{
  const CharT_Value* cur = reinterpret_cast<const CharT_Value*>(str->getData());
  const CharT_Value* end = cur + str->getComputedLength();

  if (cur == end)
    return ERR_RT_INVALID_ARGUMENT;

  uint8_t keyword[32];
  keyword[0] = 0;

  // --------------------------------------------------------------------------
  // [Spaces]
  // --------------------------------------------------------------------------

  if ((flags & COLOR_NAME_STRICT) == 0)
  {
    // Skip whitespaces.
    while (CharT_Func::isSpace(*cur))
    {
      if (++cur == end)
        goto _Fail;
    }
  }

  // --------------------------------------------------------------------------
  // [#RGB or #RRGGBB]
  // --------------------------------------------------------------------------

  size_t i, len;

  if ((flags & COLOR_NAME_CSS_HEX) != 0)
  {
    // Note: We fail if color starts with '#' and the components are not valid,
    // because there is no other definition that starts with '#'.
    if (cur[0] == '#')
    {
      Argb32 argb32(0xFF000000);

      cur++;
      len = (size_t)(end - cur);

      // Try to parse the #RRGGBB string.
      if (len >= 6)
      {
        for (i = 0; i < 6; i++)
        {
          if (!CharT_Func::isAsciiXDigit(cur[i]))
          {
            if (i == 3)
              goto _TryCssHex3;
            else
              goto _Fail;
          }
        }

        // The HEX letters were verified, parse the values.
        argb32.r = (uint8_t)Color_doubleHexToComponent(cur[0], cur[1]);
        argb32.g = (uint8_t)Color_doubleHexToComponent(cur[2], cur[3]);
        argb32.b = (uint8_t)Color_doubleHexToComponent(cur[4], cur[5]);
        cur += 6;
      }
      // Try to parse the #RGB string.
      else if (len >= 3)
      {
_TryCssHex3:
        for (i = 0; i < 3; i++)
        {
          if (!CharT_Func::isAsciiXDigit(cur[i]))
            goto _Fail;
        }

        // The HEX letters were verified, parse the values.
        argb32.r = (uint8_t)Color_singleHexToComponent(cur[0]);
        argb32.g = (uint8_t)Color_singleHexToComponent(cur[1]);
        argb32.b = (uint8_t)Color_singleHexToComponent(cur[2]);
        cur += 3;
      }
      else
      {
        goto _Fail;
      }

      dst->setArgb32(argb32);
      goto _Done;
    }
  }

  // --------------------------------------------------------------------------
  // [rgb[a] or hsl[a]]
  // --------------------------------------------------------------------------

  {
    uint32_t kPacked = 0;
    bool isAlpha = false;
    bool isHSL = false;
    bool percentUsed = false;

    float f[4];
    f[0] = 1.0f;

    if ((size_t)(end - cur) >= 5)
    {
      keyword[0] = CharT_Func::isAsciiLetter(cur[0]) ? (uint8_t)cur[0] : (uint8_t)0;
      keyword[1] = CharT_Func::isAsciiLetter(cur[1]) ? (uint8_t)cur[1] : (uint8_t)0;
      keyword[2] = CharT_Func::isAsciiLetter(cur[2]) ? (uint8_t)cur[2] : (uint8_t)0;
      keyword[3] = CharT_Func::isAsciiLetter(cur[3]) ? (uint8_t)cur[3] : (uint8_t)0;

      if (flags & COLOR_NAME_IGNORE_CASE)
      {
        keyword[0] = CharA::toLower(keyword[0]);
        keyword[1] = CharA::toLower(keyword[1]);
        keyword[2] = CharA::toLower(keyword[2]);
        keyword[3] = CharA::toLower(keyword[3]);
      }

      kPacked = reinterpret_cast<uint32_t*>(keyword)[0];
    }

    // rgb(...)
    if ((flags & COLOR_NAME_CSS_RGB ) != 0 && kPacked == FOG_MAKE_UINT32_SEQ('r', 'g', 'b', '\0'))
    {
      cur += 3;

      goto _ParseRrbOrHsv;
    }

    // hsl(...)
    if ((flags & COLOR_NAME_CSS_HSL ) != 0 && kPacked == FOG_MAKE_UINT32_SEQ('h', 's', 'l', '\0'))
    {
      cur += 3;
      isHSL = true;

      goto _ParseRrbOrHsv;
    }

    // rgba(...)
    if ((flags & COLOR_NAME_CSS_RGBA) != 0 && kPacked == FOG_MAKE_UINT32_SEQ('r', 'g', 'b', 'a'))
    {
      cur += 4;
      isAlpha = true;

      goto _ParseRrbOrHsv;
    }

    // hsla(...)
    if ((flags & COLOR_NAME_CSS_HSLA) != 0 && kPacked == FOG_MAKE_UINT32_SEQ('h', 's', 'l', 'a'))
    {
      cur += 4;
      isAlpha = true;
      isHSL = true;

      // Parse rgb[a] or hsl[a].
_ParseRrbOrHsv:

      // Skip whitespaces.
      while (CharT_Func::isSpace(*cur))
      {
        if (++cur == end)
          goto _Fail;
      }

      // Parse '('.
      if (*cur != CharT('(') || ++cur == end)
        goto _Fail;

      // Skip whitespaces.
      while (CharT_Func::isSpace(*cur))
      {
        if (++cur == end)
          goto _Fail;
      }

      {
        for (i = 1; i < 4; i++)
        {
          float val;
          size_t valEnd;
          uint32_t pf;

          // Skip whitespaces.
          while (CharT_Func::isSpace(*cur))
          {
            if (++cur == end)
              goto _Fail;
          }

          // Parse number. It's against standard, but some SVG generators emit
          // number in floating point, including 'dot' and some numbers behind.
          if ((StringUtil::parseReal(&val, (const CharT*)cur, (size_t)(end - cur), CharT('.'), &valEnd, &pf) != ERR_OK) ||
              (pf & STRING_PARSED_EXPONENT) != 0)
          {
            goto _Fail;
          }

          // StringW can't end here.
          if ((cur += valEnd) == end)
            goto _Fail;

          if (isHSL)
          {
            if (i == 1)
            {
              val = Math::mod(val, 360.0f);
              if (val < 0.0f) val += 360.0f;
              f[i] = val * (1.0f / 360.0f);
            }
            else
            {
              // Parse '%'.
              if (*cur != CharT('%') || ++cur == end)
                goto _Fail;
              f[i] = Math::bound<float>(val, 0.0f, 100.0f) * (1.0f / 100.0f);
            }
          }
          else
          {
            // RGB value can be at 0-255 or 0-100%.
            if (*cur == CharT('%'))
            {
              percentUsed = true;
              if (++cur == end)
                goto _Fail;

              f[i] = Math::bound<float>(val, 0.0f, 100.0f) * (1.0f / 100.0f);
            }
            else
            {
              // Invalid when strict mode is used, accepted when not.
              if (percentUsed && (flags & COLOR_NAME_STRICT))
                goto _Fail;

              f[i] = Math::bound<float>(val, 0.0f, 255.0f) * (1.0f / 255.0f);
            }
          }

          // Skip whitespaces.
          while (CharT_Func::isSpace(*cur))
          {
            if (++cur == end)
              goto _Fail;
          }

          // Expect ','.
          if (*cur == CharT(','))
          {
            if (++cur == end)
              goto _Fail;
            if (i == 3 && !isAlpha)
              goto _Fail;
          }
          else
          {
            if (i < 3 || isAlpha)
              goto _Fail;
          }
        }

        if (isAlpha)
        {
          size_t valEnd;
          uint32_t pf;

          // Parse float.
          if ((StringUtil::parseReal(&f[0], (const CharT*)cur, (size_t)(end - cur), CharT('.'), &valEnd, &pf) != ERR_OK) ||
              (pf & STRING_PARSED_EXPONENT) != 0)
          {
            goto _Fail;
          }

          // Can't stop here.
          if ((cur += valEnd) == end)
            goto _Fail;

          // Saturate to 0-1.
          f[0] = Math::bound(f[0], 0.0f, 1.0f);

          // Skip whitespaces.
          while (CharT_Func::isSpace(*cur))
          {
            if (++cur == end)
              goto _Fail;
          }
        }

        // Expect ')'.
        if (*cur != CharT(')'))
          goto _Fail;
        cur++;

        // Finalize.
        if (isHSL)
          dst->setAhslF(*reinterpret_cast<AhslF*>(f));
        else
          dst->setArgbF(*reinterpret_cast<ArgbF*>(f));
        goto _Done;
      }
    }
  }

  // --------------------------------------------------------------------------
  // [CSS - 'keyword' + 'transparent']
  // --------------------------------------------------------------------------

  if ((flags & (COLOR_NAME_CSS_KEYWORD | COLOR_NAME_CSS_TRANSPARENT)) != 0)
  {
    const ColorName* entity;
    StubA nameA(UNINITIALIZED);

    i = 0;
    for (;;)
    {
      CharT_Value c = cur[i];
      if (!(CharT_Func::isAsciiNumlet(c) || c == CharT('-'))) break;

      keyword[i] = (uint8_t)c;
      if (++i == 32)
        goto _Fail;

      nameA.setData(keyword);
      nameA.setLength(i);
    }

    if ((flags & COLOR_NAME_IGNORE_CASE) != 0)
    {
      for (i = 0; i < nameA.getLength(); i++)
        keyword[i] = CharA::toLower(keyword[i]);
    }

    cur += i;

    // CSS keywords (basic and extended color names).
    if ((flags & COLOR_NAME_CSS_KEYWORD) != 0 && (entity = Color_find(nameA)) != NULL)
    {
      dst->setArgb32(Argb32(0xFF, entity->r, entity->g, entity->b));
      goto _Done;
    }

    // CSS 'transparent'.
    if (nameA.getLength() == 11 && StringUtil::eq(nameA.getData(), "transparent", 11, CASE_SENSITIVE))
    {
      cur += 11;

      dst->setArgb32(Argb32(0x00000000));
      goto _Done;
    }
  }

_Fail:
  dst->reset();
  return ERR_RT_INVALID_ARGUMENT;

_Done:
  if ((flags & COLOR_NAME_STRICT) != 0 && cur != end) goto _Fail;
  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE2(Color_init_SSE2)

FOG_NO_EXPORT void Color_init(void)
{
  typedef Api::Color_Convert ConvFunc;

  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.color_convert[ COLOR_MODEL_ARGB  ][ COLOR_MODEL_NONE  ] = (ConvFunc)Color_zero4f;
  fog_api.color_convert[ COLOR_MODEL_ARGB  ][ COLOR_MODEL_ARGB  ] = (ConvFunc)Color_copy4f;
  fog_api.color_convert[ COLOR_MODEL_ARGB  ][ COLOR_MODEL_AHSV  ] = (ConvFunc)Color_argbf_from_ahsvf;
  fog_api.color_convert[ COLOR_MODEL_ARGB  ][ COLOR_MODEL_AHSL  ] = (ConvFunc)Color_argbf_from_ahslf;
  fog_api.color_convert[ COLOR_MODEL_ARGB  ][ COLOR_MODEL_ACMYK ] = (ConvFunc)Color_argbf_from_acmykf;
  fog_api.color_convert[ COLOR_MODEL_ARGB  ][_COLOR_MODEL_ARGB32] = (ConvFunc)Color_argbf_from_argb32;
  fog_api.color_convert[ COLOR_MODEL_ARGB  ][_COLOR_MODEL_ARGB64] = (ConvFunc)Color_argbf_from_argb64;

  fog_api.color_convert[ COLOR_MODEL_AHSV  ][ COLOR_MODEL_NONE  ] = (ConvFunc)Color_zero4f;
  fog_api.color_convert[ COLOR_MODEL_AHSV  ][ COLOR_MODEL_ARGB  ] = (ConvFunc)Color_ahsvf_from_argbf;
  fog_api.color_convert[ COLOR_MODEL_AHSV  ][ COLOR_MODEL_AHSV  ] = (ConvFunc)Color_copy4f;
  fog_api.color_convert[ COLOR_MODEL_AHSV  ][ COLOR_MODEL_AHSL  ] = (ConvFunc)Color_ahsvf_from_ahslf;
  fog_api.color_convert[ COLOR_MODEL_AHSV  ][ COLOR_MODEL_ACMYK ] = (ConvFunc)Color_ahsvf_from_acmykf;
  fog_api.color_convert[ COLOR_MODEL_AHSV  ][_COLOR_MODEL_ARGB32] = (ConvFunc)Color_ahsvf_from_argb32;
  fog_api.color_convert[ COLOR_MODEL_AHSV  ][_COLOR_MODEL_ARGB64] = (ConvFunc)Color_ahsvf_from_argb64;

  fog_api.color_convert[ COLOR_MODEL_AHSL  ][ COLOR_MODEL_NONE  ] = (ConvFunc)Color_zero4f;
  fog_api.color_convert[ COLOR_MODEL_AHSL  ][ COLOR_MODEL_ARGB  ] = (ConvFunc)Color_ahslf_from_argbf;
  fog_api.color_convert[ COLOR_MODEL_AHSL  ][ COLOR_MODEL_AHSV  ] = (ConvFunc)Color_ahslf_from_ahsvf;
  fog_api.color_convert[ COLOR_MODEL_AHSL  ][ COLOR_MODEL_AHSL  ] = (ConvFunc)Color_copy4f;
  fog_api.color_convert[ COLOR_MODEL_AHSL  ][ COLOR_MODEL_ACMYK ] = (ConvFunc)Color_ahslf_from_acmykf;
  fog_api.color_convert[ COLOR_MODEL_AHSL  ][_COLOR_MODEL_ARGB32] = (ConvFunc)Color_ahslf_from_argb32;
  fog_api.color_convert[ COLOR_MODEL_AHSL  ][_COLOR_MODEL_ARGB64] = (ConvFunc)Color_ahslf_from_argb64;

  fog_api.color_convert[ COLOR_MODEL_ACMYK ][ COLOR_MODEL_NONE  ] = (ConvFunc)Color_zero5f;
  fog_api.color_convert[ COLOR_MODEL_ACMYK ][ COLOR_MODEL_ARGB  ] = (ConvFunc)Color_acmykf_from_argbf;
  fog_api.color_convert[ COLOR_MODEL_ACMYK ][ COLOR_MODEL_AHSV  ] = (ConvFunc)Color_acmykf_from_ahsvf;
  fog_api.color_convert[ COLOR_MODEL_ACMYK ][ COLOR_MODEL_AHSL  ] = (ConvFunc)Color_acmykf_from_ahslf;
  fog_api.color_convert[ COLOR_MODEL_ACMYK ][ COLOR_MODEL_ACMYK ] = (ConvFunc)Color_copy5f;
  fog_api.color_convert[ COLOR_MODEL_ACMYK ][_COLOR_MODEL_ARGB32] = (ConvFunc)Color_acmykf_from_argb32;
  fog_api.color_convert[ COLOR_MODEL_ACMYK ][_COLOR_MODEL_ARGB64] = (ConvFunc)Color_acmykf_from_argb64;

  fog_api.color_convert[_COLOR_MODEL_ARGB32][ COLOR_MODEL_NONE  ] = (ConvFunc)Color_argb32_zero;
  fog_api.color_convert[_COLOR_MODEL_ARGB32][ COLOR_MODEL_ARGB  ] = (ConvFunc)Color_argb32_from_argbf;
  fog_api.color_convert[_COLOR_MODEL_ARGB32][ COLOR_MODEL_AHSV  ] = (ConvFunc)Color_argb32_from_ahsvf;
  fog_api.color_convert[_COLOR_MODEL_ARGB32][ COLOR_MODEL_AHSL  ] = (ConvFunc)Color_argb32_from_ahslf;
  fog_api.color_convert[_COLOR_MODEL_ARGB32][ COLOR_MODEL_ACMYK ] = (ConvFunc)Color_argb32_from_acmykf;
  fog_api.color_convert[_COLOR_MODEL_ARGB32][_COLOR_MODEL_ARGB32] = (ConvFunc)Color_argb32_from_argb32;
  fog_api.color_convert[_COLOR_MODEL_ARGB32][_COLOR_MODEL_ARGB64] = (ConvFunc)Color_argb32_from_argb64;

  fog_api.color_convert[_COLOR_MODEL_ARGB64][ COLOR_MODEL_NONE  ] = (ConvFunc)Color_argb64_zero;
  fog_api.color_convert[_COLOR_MODEL_ARGB64][ COLOR_MODEL_ARGB  ] = (ConvFunc)Color_argb64_from_argbf;
  fog_api.color_convert[_COLOR_MODEL_ARGB64][ COLOR_MODEL_AHSV  ] = (ConvFunc)Color_argb64_from_ahsvf;
  fog_api.color_convert[_COLOR_MODEL_ARGB64][ COLOR_MODEL_AHSL  ] = (ConvFunc)Color_argb64_from_ahslf;
  fog_api.color_convert[_COLOR_MODEL_ARGB64][ COLOR_MODEL_ACMYK ] = (ConvFunc)Color_argb64_from_acmykf;
  fog_api.color_convert[_COLOR_MODEL_ARGB64][_COLOR_MODEL_ARGB32] = (ConvFunc)Color_argb64_from_argb32;
  fog_api.color_convert[_COLOR_MODEL_ARGB64][_COLOR_MODEL_ARGB64] = (ConvFunc)Color_argb64_from_argb64;

  fog_api.color_setModel = Color_setModel;
  fog_api.color_setData = Color_setData;

  fog_api.color_mix = Color_mix;
  fog_api.color_adjust = Color_adjust;

  fog_api.color_parseA = Color_parse<char>;
  fog_api.color_parseU = Color_parse<CharW>;

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE2(Color_init_SSE2)
}

} // Fog namespace
