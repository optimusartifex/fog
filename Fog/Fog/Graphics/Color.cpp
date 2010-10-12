// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/Color.h>

namespace Fog {

// ============================================================================
// [Fog::Color]
// ============================================================================

ArgbF Color::getArgbF() const
{
  ArgbF result(DONT_INITIALIZE);

  switch (_colorModel)
  {
    default:
    case COLOR_MODEL_NONE:
      result.reset();
      break;
    case COLOR_MODEL_ARGB:
      result.setArgb(_data[0], _data[1], _data[2], _data[3]);
      break;
    case COLOR_MODEL_AHSV:
      _convertArgbfFromAhsvf(result.getData(), _data);
      break;
    case COLOR_MODEL_ACMYK:
      _convertArgbfFromAcmykf(result.getData(), _data);
      break;
  }

  return result;
}

AhsvF Color::getAhsvF() const
{
  AhsvF result(DONT_INITIALIZE);

  switch (_colorModel)
  {
    default:
    case COLOR_MODEL_NONE:
      result.reset();
      break;
    case COLOR_MODEL_ARGB:
      _convertAhsvfFromArgbf(result.getData(), _data);
      break;
    case COLOR_MODEL_AHSV:
      result._data[0] = _data[0];
      result._data[1] = _data[1];
      result._data[2] = _data[2];
      result._data[3] = _data[3];
      break;
    case COLOR_MODEL_ACMYK:
    {
      float tmp[4];
      _convertArgbfFromAcmykf(tmp, _data);
      _convertAhsvfFromArgbf(result.getData(), tmp);
      break;
    }
  }

  return result;
}

AcmykF Color::getAcmykF() const
{
  AcmykF result(DONT_INITIALIZE);

  switch (_colorModel)
  {
    default:
    case COLOR_MODEL_NONE:
      result.reset();
      break;
    case COLOR_MODEL_ARGB:
      _convertAcmykfFromArgbf(result.getData(), _data);
      break;
    case COLOR_MODEL_AHSV:
    {
      float tmp[4];
      _convertArgbfFromAhsvf(tmp, _data);
      _convertAcmykfFromArgbf(result.getData(), tmp);
      break;
    }
    case COLOR_MODEL_ACMYK:
      result._data[0] = _data[0];
      result._data[1] = _data[1];
      result._data[2] = _data[2];
      result._data[3] = _data[3];
      result._data[4] = _data[4];
      break;
  }

  return result;
}

void Color::setColorModel(uint32_t colorModel)
{
  if (FOG_UNLIKELY(colorModel >= COLOR_MODEL_COUNT)) return;
  if (colorModel == _colorModel) return;

  ArgbF tmp = getArgbF();
  switch (colorModel)
  {
    case COLOR_MODEL_NONE:
      reset();
      break;
    case COLOR_MODEL_ARGB:
      _colorModel = COLOR_MODEL_ARGB;
      _data[0] = tmp._data[0];
      _data[1] = tmp._data[1];
      _data[2] = tmp._data[2];
      _data[3] = tmp._data[3];
      _data[4] = 0.0f;
      break;
    case COLOR_MODEL_AHSV:
      _colorModel = COLOR_MODEL_AHSV;
      _convertAhsvfFromArgbf(_data, tmp.getData());
      _data[4] = 0.0f;
      break;
    case COLOR_MODEL_ACMYK:
      _colorModel = COLOR_MODEL_ACMYK;
      _convertAcmykfFromArgbf(_data, tmp.getData());
      break;
  }
}

void Color::setArgb(const ArgbI& argb)
{
  _colorModel = COLOR_MODEL_ARGB;
  _argb = argb;

  _data[0] = (float)argb.getAlpha() * 255.0f;
  _data[1] = (float)argb.getRed()   * 255.0f;
  _data[2] = (float)argb.getGreen() * 255.0f;
  _data[3] = (float)argb.getBlue()  * 255.0f;
  _data[4] = 0.0f;
}

void Color::setArgb(const ArgbF& argb)
{
  _colorModel = COLOR_MODEL_ARGB;
  _argb.setArgb(
    (int)(argb.getAlpha() * 255.0f),
    (int)(argb.getRed()   * 255.0f),
    (int)(argb.getGreen() * 255.0f),
    (int)(argb.getBlue()  * 255.0f));

  _data[0] = argb.getAlpha();
  _data[1] = argb.getRed();
  _data[2] = argb.getGreen();
  _data[3] = argb.getBlue();
  _data[4] = 0.0f;
}

void Color::setAhsv(const AhsvF& ahsv)
{
  _colorModel = COLOR_MODEL_AHSV;
  _argb.setPacked(_convertArgbiFromAhsvf(ahsv.getData()));

  _data[0] = ahsv.getAlpha();
  _data[1] = ahsv.getHue();
  _data[2] = ahsv.getSaturation();
  _data[3] = ahsv.getValue();
  _data[4] = 0.0f;
}

void Color::setAcmyk(const AcmykF& acmyk)
{
  _colorModel = COLOR_MODEL_ACMYK;
  _argb.setPacked(_convertArgbiFromAcmykf(acmyk.getData()));

  _data[0] = acmyk.getAlpha();
  _data[1] = acmyk.getCyan();
  _data[2] = acmyk.getMagenta();
  _data[3] = acmyk.getYellow();
  _data[4] = acmyk.getBlack();
}

void Color::setColor(const Color& other)
{
  _colorModel = other._colorModel;
  _argb = other._argb;

  _data[0] = other._data[0];
  _data[1] = other._data[1];
  _data[2] = other._data[2];
  _data[3] = other._data[3];
  _data[4] = other._data[4];
}

void Color::reset()
{
  _colorModel = COLOR_MODEL_NONE;
  _argb = 0x00000000;

  _data[0] = 0.0f;
  _data[1] = 0.0f;
  _data[2] = 0.0f;
  _data[3] = 0.0f;
  _data[4] = 0.0f;
}

err_t Color::op(uint32_t opType, const Color& other, float factor)
{
  if (FOG_UNLIKELY(opType >= COLOR_OP_COUNT)) return ERR_RT_INVALID_ARGUMENT;

  uint32_t cm = getColorModel();

  switch (opType)
  {
    case COLOR_OP_NONE:
      break;

    case COLOR_OP_LERP:
    {
      if (factor < 0.0f) return ERR_RT_INVALID_ARGUMENT;
      if (factor > 1.0f) return ERR_RT_INVALID_ARGUMENT;

      if (cm != COLOR_MODEL_ARGB) setColorModel(COLOR_MODEL_ARGB);
      ArgbF other_argbf = other.getArgbF();

      _data[0] += (other_argbf._data[0] - _data[0]) * factor;
      _data[1] += (other_argbf._data[1] - _data[1]) * factor;
      _data[2] += (other_argbf._data[2] - _data[2]) * factor;
      _data[3] += (other_argbf._data[3] - _data[3]) * factor;

      if (cm != COLOR_MODEL_ARGB) setColorModel(cm);
      break;
    }
  }

  return ERR_OK;
}

err_t Color::lighten(float factor)
{
  if (factor < 0.0f) return ERR_RT_INVALID_ARGUMENT;
  if (factor < 1.0f) return darken(1.0f - factor);

  uint32_t cm = getColorModel();
  if (cm != COLOR_MODEL_AHSV) setColorModel(COLOR_MODEL_AHSV);

  float s = _data[2];
  float v = _data[3] * factor;

  if (v > 1.0f)
  {
    s = Math::max(1.0f + s - v, 0.0f);
    v = 1.0f;
  }

  _data[2] = s;
  _data[3] = v;

  if (cm != COLOR_MODEL_AHSV) setColorModel(cm);
  return ERR_OK;
}

err_t Color::darken(float factor)
{
  if (factor < 0.0f) return ERR_RT_INVALID_ARGUMENT;
  if (factor < 1.0f) return lighten(1.0f - factor);

  uint32_t cm = getColorModel();
  if (cm != COLOR_MODEL_AHSV) setColorModel(COLOR_MODEL_AHSV);

  _data[3] /= factor;

  if (cm != COLOR_MODEL_AHSV) setColorModel(cm);
  return ERR_OK;
}

// ============================================================================
// [Fog::Color - Statics]
// ============================================================================

uint32_t Color::_convertArgbiFromAhsvf(const float* src)
{
  int ai, hi, si, vi;
  float f;

  float a = src[0];
  float h = src[1];
  float s = src[2];
  float v = src[3];

  a *= 255.0f;
  v *= 255.0f;

  ai = (int)a;
  vi = (int)v;

  if (Math::flt(s, 0.0f))
  {
    return ArgbI::pack(ai, vi, vi, vi);
  }

  if (FOG_UNLIKELY(h >= 360.0f || h < 0.0f))
  {
    h = fmodf(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
  }

  h *= (1.0f / 60.0f);
  hi = (int)h;
  f = h - (float)hi;

  s *= v;
  f *= s;
  s = v - s;

  si = (int)s;

  switch (hi)
  {
    default:
    case 0: return ArgbI::pack(ai, vi, (int)(s + f), si);
    case 1: return ArgbI::pack(ai, (int)(v - f), vi, si);
    case 2: return ArgbI::pack(ai, si, vi, (int)(s + f));
    case 3: return ArgbI::pack(ai, si, (int)(v - f), vi);
    case 4: return ArgbI::pack(ai, (int)(s + f), si, vi);
    case 5: return ArgbI::pack(ai, vi, si, (int)(v - f));
  }
}

uint32_t Color::_convertArgbiFromAcmykf(const float* src)
{
  float a = src[0];
  float kinv = 1.0f - src[4];

  return ArgbI::pack(
    (int)(a * 255.0f),
    (int)((kinv - src[1] * kinv) * 255.0f),
    (int)((kinv - src[2] * kinv) * 255.0f),
    (int)((kinv - src[3] * kinv) * 255.0f));
}

void Color::_convertArgbfFromAhsvf(float* dst, const float* src)
{
  int hi;
  float f;

  float a = src[0];
  float h = src[1];
  float s = src[2];
  float v = src[3];

  dst[0] = a;

  if (Math::flt(s, 0.0f))
  {
    dst[1] = v;
    dst[2] = v;
    dst[3] = v;
    return;
  }

  if (FOG_UNLIKELY(h >= 360.0f || h < 0.0f))
  {
    h = fmodf(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
  }

  h *= (1.0f / 60.0f);
  hi = (int)h;
  f = h - (float)hi;

  s *= v;
  f *= s;
  s = v - s;

  switch (hi)
  {
    default:
    case 0: dst[1] = v    ; dst[2] = s + f; dst[3] = s    ; break;
    case 1: dst[1] = v - f; dst[2] = v    ; dst[3] = s    ; break;
    case 2: dst[1] = s    ; dst[2] = v    ; dst[3] = s + f; break;
    case 3: dst[1] = s    ; dst[2] = v - f; dst[3] = v    ; break;
    case 4: dst[1] = s + f; dst[2] = s    ; dst[3] = v    ; break;
    case 5: dst[1] = v    ; dst[2] = s    ; dst[3] = v - f; break;
  }
}

void Color::_convertArgbfFromAcmykf(float* dst, const float* src)
{
  float a = src[0];
  float kinv = 1.0f - src[4];

  dst[0] = a;
  dst[1] = kinv - src[1] * kinv;
  dst[2] = kinv - src[2] * kinv;
  dst[3] = kinv - src[3] * kinv;
}

void Color::_convertAhsvfFromArgbi(float* dst, uint32_t src)
{
  int r = (int)ArgbI::getRed(src);
  int g = (int)ArgbI::getGreen(src);
  int b = (int)ArgbI::getBlue(src);

  int minimum;
  int maximum;
  int delta;

  minimum = Math::min(r, g, b);
  maximum = Math::max(r, g, b);
  delta = maximum - minimum;

  float a = (1.0f / 255.0f) * (float)(int)ArgbI::getAlpha(src);

  float h = 0.0f;
  float s = 0.0f;
  float v = (100.0f / 255.0f) * maximum;
  float recip = (delta) ? 100.0f / (6.0f * delta) : 0.0f;

  if (maximum != 0)
  {
    s = (float)(100 * delta) / (float)maximum;
  }

  if (maximum == r)
    h = (float)(g - b);
  else if (maximum == g)
    h = 2.0f * delta + (float)(b - r);
  else
    h = 4.0f * delta + (float)(r - g);

  h *= recip;

  if (h < 0.0f)
    h += 100.0f;
  else if (h > 100.0f)
    h -= 100.0f;

  dst[0] = a;
  dst[1] = h;
  dst[2] = s;
  dst[3] = v;
}

void Color::_convertAhsvfFromArgbf(float* dst, const float* src)
{
  float r = src[1];
  float g = src[2];
  float b = src[3];

  float minimum;
  float maximum;
  float delta;

  minimum = Math::min(r, g, b);
  maximum = Math::max(r, g, b);
  delta = maximum - minimum;

  float h = 0.0f;
  float s = 0.0f;
  float v = 100.0f * maximum;
  float recip = (delta > 0.0f) ? 100.0f / (6.0f * delta) : 0.0f;

  if (maximum > 0.0f)
  {
    s = (float)(100 * delta) / (float)maximum;
  }

  if (maximum == r)
    h = g - b;
  else if (maximum == g)
    h = 2.0f * delta + b - r;
  else
    h = 4.0f * delta + r - g;

  h *= recip;

  if (h < 0.0f)
    h += 100.0f;
  else if (h > 100.0f)
    h -= 100.0f;

  dst[0] = src[0];
  dst[1] = h;
  dst[2] = s;
  dst[3] = v;
}

void Color::_convertAcmykfFromArgbi(float* dst, uint32_t src)
{
  float srcf[4] = { 
    (float)(int)ArgbI::getAlpha(src),
    (float)(int)ArgbI::getRed(src),
    (float)(int)ArgbI::getGreen(src),
    (float)(int)ArgbI::getBlue(src)
  };

  _convertAcmykfFromArgbf(dst, srcf);
}

void Color::_convertAcmykfFromArgbf(float* dst, const float* src)
{
  float c = 1.0f - src[1];
  float m = 1.0f - src[2];
  float y = 1.0f - src[3];
  float k = Math::min(c, m, y);

  if (Math::flt(k, 1.0f))
  {
    float recip = 1.0f / (1.0f - k);

    c -= k; c *= recip;
    m -= k; m *= recip;
    y -= k; y *= recip;
  }

  dst[0] = src[0];
  dst[1] = c;
  dst[2] = m;
  dst[3] = y;
  dst[4] = k;
}

} // Fog namespace