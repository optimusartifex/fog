// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// AggOO - Version 0.1
// Copyright (c) 2006 Dratek Software
//
// Author of AggOO (Chad M. Draper) agreed relicencing code to the MIT license
// that is used for Fog (original code was 3 clause BSD license). Thanks.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Math.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/ColorMatrix.h>

namespace Fog {

// The luminance color weights are close to the values of the NTSC color
// weights, but these values are preferable.
static const float LUM_R = 0.3086f;
static const float LUM_G = 0.6094f;
static const float LUM_B = 0.0820f;

// ============================================================================
// [Fog::ColorMatrix]
// ============================================================================

int ColorMatrix::getType() const
{
  int parts = 0;

  // RGB shear part is illustrated here:
  //   [n X X n n]
  //   [X n X n n]
  //   [X X n n n]
  //   [n n n n n]
  //   [n n n n n]
  if (m[0][1] != 0.0f || m[0][2] != 0.0f ||
      m[1][0] != 0.0f || m[1][2] != 0.0f ||
      m[2][0] != 0.0f || m[2][1] != 0.0f) parts |= TYPE_SHEAR_RGB;

  // Alpha shear part is illustrated here:
  //   [n n n X n]
  //   [n n n X n]
  //   [n n n X n]
  //   [X X X n n]
  //   [n n n n n]
  if (m[0][3] != 0.0f ||
      m[1][3] != 0.0f ||
      m[2][3] != 0.0f ||
      m[2][0] != 0.0f || m[2][1] != 0.0f || m[2][2] != 0.0f) parts |= TYPE_SHEAR_ALPHA;

  // RGB lut part is illustrated here:
  //   [X n n n n]
  //   [n X n n n]
  //   [n n X n n]
  //   [n n n n n]
  //   [n n n n n]
  if (m[0][0] != 0.0f ||
      m[1][1] != 0.0f ||
      m[2][2] != 0.0f) parts |= TYPE_LUT_RGB;

  // Alpha lut part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n X n]
  //   [n n n n n]
  if (m[3][3] != 0.0f) parts |= TYPE_LUT_ALPHA;

  //! @brief Matrix contains const RGB lut part (all cells are set to 1.0).
  //!
  //! Const RGB lut part is illustrated here:
  //!   [1 n n n n]
  //!   [n 1 n n n]
  //!   [n n 1 n n]
  //!   [n n n n n]
  //!   [n n n n n]
  if (m[0][0] == 1.0f &&
      m[1][1] == 1.0f &&
      m[2][2] == 1.0f) parts |= TYPE_CONST_RGB;

  //! @brief Matrix contains const alpha lut part (cell set to 1.0).
  //!
  //! Const alpha lut part is illustrated here:
  //!   [n n n n n]
  //!   [n n n n n]
  //!   [n n n n n]
  //!   [n n n 1 n]
  //!   [n n n n n]
  if (m[3][3] == 1.0f) parts |= TYPE_CONST_ALPHA;

  // RGB translation part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [X X X n n]
  if (m[4][0] != 0.0f || m[4][1] != 0.0f || m[4][2] != 0.0f) parts |= TYPE_TRANSLATE_RGB;

  // Alpha translation part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n X n]
  if (m[4][3] != 0.0f) parts |= TYPE_TRANSLATE_ALPHA;

  return parts;
}

ColorMatrix& ColorMatrix::add(const ColorMatrix& other)
{
  for (sysuint_t i = 0; i < 25; i++)
    getData()[i] += other.getData()[i];
  return *this;
}

ColorMatrix& ColorMatrix::subtract(const ColorMatrix& other)
{
  for (sysuint_t i = 0; i < 25; i++)
    getData()[i] -= other.getData()[i];
  return *this;
}

ColorMatrix& ColorMatrix::multiply(const ColorMatrix& other, uint32_t order)
{
  ColorMatrix save(*this);

  const ColorMatrix* mat1 = &save;
  const ColorMatrix* mat2 = &other;

  if (order == MATRIX_APPEND)
  {
    mat1 = &other;
    mat2 = &save;
  }

  m[0][0] = mat1->m[0][0] * mat2->m[0][0] + mat1->m[0][1] * mat2->m[1][0] + mat1->m[0][2] * mat2->m[2][0] + mat1->m[0][3] * mat2->m[3][0] + mat1->m[0][4] * mat2->m[4][0];
  m[0][1] = mat1->m[0][0] * mat2->m[0][1] + mat1->m[0][1] * mat2->m[1][1] + mat1->m[0][2] * mat2->m[2][1] + mat1->m[0][3] * mat2->m[3][1] + mat1->m[0][4] * mat2->m[4][1];
  m[0][2] = mat1->m[0][0] * mat2->m[0][2] + mat1->m[0][1] * mat2->m[1][2] + mat1->m[0][2] * mat2->m[2][2] + mat1->m[0][3] * mat2->m[3][2] + mat1->m[0][4] * mat2->m[4][2];
  m[0][3] = mat1->m[0][0] * mat2->m[0][3] + mat1->m[0][1] * mat2->m[1][3] + mat1->m[0][2] * mat2->m[2][3] + mat1->m[0][3] * mat2->m[3][3] + mat1->m[0][4] * mat2->m[4][3];
  m[0][4] = mat1->m[0][0] * mat2->m[0][4] + mat1->m[0][1] * mat2->m[1][4] + mat1->m[0][2] * mat2->m[2][4] + mat1->m[0][3] * mat2->m[3][4] + mat1->m[0][4] * mat2->m[4][4];

  m[1][0] = mat1->m[1][0] * mat2->m[0][0] + mat1->m[1][1] * mat2->m[1][0] + mat1->m[1][2] * mat2->m[2][0] + mat1->m[1][3] * mat2->m[3][0] + mat1->m[1][4] * mat2->m[4][0];
  m[1][1] = mat1->m[1][0] * mat2->m[0][1] + mat1->m[1][1] * mat2->m[1][1] + mat1->m[1][2] * mat2->m[2][1] + mat1->m[1][3] * mat2->m[3][1] + mat1->m[1][4] * mat2->m[4][1];
  m[1][2] = mat1->m[1][0] * mat2->m[0][2] + mat1->m[1][1] * mat2->m[1][2] + mat1->m[1][2] * mat2->m[2][2] + mat1->m[1][3] * mat2->m[3][2] + mat1->m[1][4] * mat2->m[4][2];
  m[1][3] = mat1->m[1][0] * mat2->m[0][3] + mat1->m[1][1] * mat2->m[1][3] + mat1->m[1][2] * mat2->m[2][3] + mat1->m[1][3] * mat2->m[3][3] + mat1->m[1][4] * mat2->m[4][3];
  m[1][4] = mat1->m[1][0] * mat2->m[0][4] + mat1->m[1][1] * mat2->m[1][4] + mat1->m[1][2] * mat2->m[2][4] + mat1->m[1][3] * mat2->m[3][4] + mat1->m[1][4] * mat2->m[4][4];

  m[2][0] = mat1->m[2][0] * mat2->m[0][0] + mat1->m[2][1] * mat2->m[1][0] + mat1->m[2][2] * mat2->m[2][0] + mat1->m[2][3] * mat2->m[3][0] + mat1->m[2][4] * mat2->m[4][0];
  m[2][1] = mat1->m[2][0] * mat2->m[0][1] + mat1->m[2][1] * mat2->m[1][1] + mat1->m[2][2] * mat2->m[2][1] + mat1->m[2][3] * mat2->m[3][1] + mat1->m[2][4] * mat2->m[4][1];
  m[2][2] = mat1->m[2][0] * mat2->m[0][2] + mat1->m[2][1] * mat2->m[1][2] + mat1->m[2][2] * mat2->m[2][2] + mat1->m[2][3] * mat2->m[3][2] + mat1->m[2][4] * mat2->m[4][2];
  m[2][3] = mat1->m[2][0] * mat2->m[0][3] + mat1->m[2][1] * mat2->m[1][3] + mat1->m[2][2] * mat2->m[2][3] + mat1->m[2][3] * mat2->m[3][3] + mat1->m[2][4] * mat2->m[4][3];
  m[2][4] = mat1->m[2][0] * mat2->m[0][4] + mat1->m[2][1] * mat2->m[1][4] + mat1->m[2][2] * mat2->m[2][4] + mat1->m[2][3] * mat2->m[3][4] + mat1->m[2][4] * mat2->m[4][4];

  m[3][0] = mat1->m[3][0] * mat2->m[0][0] + mat1->m[3][1] * mat2->m[1][0] + mat1->m[3][2] * mat2->m[2][0] + mat1->m[3][3] * mat2->m[3][0] + mat1->m[3][4] * mat2->m[4][0];
  m[3][1] = mat1->m[3][0] * mat2->m[0][1] + mat1->m[3][1] * mat2->m[1][1] + mat1->m[3][2] * mat2->m[2][1] + mat1->m[3][3] * mat2->m[3][1] + mat1->m[3][4] * mat2->m[4][1];
  m[3][2] = mat1->m[3][0] * mat2->m[0][2] + mat1->m[3][1] * mat2->m[1][2] + mat1->m[3][2] * mat2->m[2][2] + mat1->m[3][3] * mat2->m[3][2] + mat1->m[3][4] * mat2->m[4][2];
  m[3][3] = mat1->m[3][0] * mat2->m[0][3] + mat1->m[3][1] * mat2->m[1][3] + mat1->m[3][2] * mat2->m[2][3] + mat1->m[3][3] * mat2->m[3][3] + mat1->m[3][4] * mat2->m[4][3];
  m[3][4] = mat1->m[3][0] * mat2->m[0][4] + mat1->m[3][1] * mat2->m[1][4] + mat1->m[3][2] * mat2->m[2][4] + mat1->m[3][3] * mat2->m[3][4] + mat1->m[3][4] * mat2->m[4][4];

  m[4][0] = mat1->m[4][0] * mat2->m[0][0] + mat1->m[4][1] * mat2->m[1][0] + mat1->m[4][2] * mat2->m[2][0] + mat1->m[4][3] * mat2->m[3][0] + mat1->m[4][4] * mat2->m[4][0];
  m[4][1] = mat1->m[4][0] * mat2->m[0][1] + mat1->m[4][1] * mat2->m[1][1] + mat1->m[4][2] * mat2->m[2][1] + mat1->m[4][3] * mat2->m[3][1] + mat1->m[4][4] * mat2->m[4][1];
  m[4][2] = mat1->m[4][0] * mat2->m[0][2] + mat1->m[4][1] * mat2->m[1][2] + mat1->m[4][2] * mat2->m[2][2] + mat1->m[4][3] * mat2->m[3][2] + mat1->m[4][4] * mat2->m[4][2];
  m[4][3] = mat1->m[4][0] * mat2->m[0][3] + mat1->m[4][1] * mat2->m[1][3] + mat1->m[4][2] * mat2->m[2][3] + mat1->m[4][3] * mat2->m[3][3] + mat1->m[4][4] * mat2->m[4][3];
  m[4][4] = mat1->m[4][0] * mat2->m[0][4] + mat1->m[4][1] * mat2->m[1][4] + mat1->m[4][2] * mat2->m[2][4] + mat1->m[4][3] * mat2->m[3][4] + mat1->m[4][4] * mat2->m[4][4];

  return *this;
}

ColorMatrix& ColorMatrix::multiply(float scalar)
{
  for (sysuint_t i = 0; i < 25; i++)
    getData()[i] *= scalar;
  return *this;
}

void ColorMatrix::transformVector(float* FOG_RESTRICT v) const
{
  float temp[4];

  temp[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0] + m[4][0];
  temp[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1] + m[4][1];
  temp[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2] + m[4][2];
  temp[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3] + m[4][3];

  v[0] = temp[0];
  v[1] = temp[1];
  v[2] = temp[2];
  v[3] = temp[3];
}

void ColorMatrix::transformRgb(Argb* clr) const
{
  float r = (float)clr->r;
  float g = (float)clr->g;
  float b = (float)clr->b;

  int tr = Math::iround(r * m[0][0] + g * m[1][0] + b * m[2][0] + 255.0f * m[3][0] + m[4][0] * 255.0f);
  int tg = Math::iround(r * m[0][1] + g * m[1][1] + b * m[2][1] + 255.0f * m[3][1] + m[4][1] * 255.0f);
  int tb = Math::iround(r * m[0][2] + g * m[1][2] + b * m[2][2] + 255.0f * m[3][2] + m[4][2] * 255.0f);

  tr = Math::bound<int>(tr, 0, 255);
  tg = Math::bound<int>(tg, 0, 255);
  tb = Math::bound<int>(tb, 0, 255);

  clr->r = (uint8_t)tr;
  clr->g = (uint8_t)tg;
  clr->b = (uint8_t)tb;
}

void ColorMatrix::transformArgb(Argb* clr) const
{
  float r = (float)clr->r;
  float g = (float)clr->g;
  float b = (float)clr->b;
  float a = (float)clr->a;

  int tr = Math::iround(r * m[0][0] + g * m[1][0] + b * m[2][0] + a * m[3][0] + m[4][0] * 255.0f);
  int tg = Math::iround(r * m[0][1] + g * m[1][1] + b * m[2][1] + a * m[3][1] + m[4][1] * 255.0f);
  int tb = Math::iround(r * m[0][2] + g * m[1][2] + b * m[2][2] + a * m[3][2] + m[4][2] * 255.0f);
  int ta = Math::iround(r * m[0][3] + g * m[1][3] + b * m[2][3] + a * m[3][3] + m[4][3] * 255.0f);

  tr = Math::bound<int>(tr, 0, 255);
  tg = Math::bound<int>(tg, 0, 255);
  tb = Math::bound<int>(tb, 0, 255);
  ta = Math::bound<int>(ta, 0, 255);

  clr->set(tr, tg, tb, ta);
}

void ColorMatrix::transformAlpha(uint8_t* a) const
{
  int ta = Math::iround((float)*a * m[3][3] + m[4][3] * 255.0f);

  ta = Math::bound<int>(ta, 0, 255);

  *a = (uint8_t)ta;
}

ColorMatrix& ColorMatrix::scale(float sa, float sr, float sg, float sb, uint32_t order)
{
  ColorMatrix mod(ColorMatrix::IDENTITY);
  mod[0][0] = sr;
  mod[1][1] = sg;
  mod[2][2] = sb;
  mod[3][3] = sa;
  return multiply(mod, order);
}

ColorMatrix& ColorMatrix::translate(float ta, float tr, float tg, float tb, uint32_t order)
{
  ColorMatrix mod(ColorMatrix::IDENTITY);
  mod[4][0] = tr;
  mod[4][1] = tg;
  mod[4][2] = tb;
  mod[4][3] = ta;
  return multiply(mod, order);
}

ColorMatrix& ColorMatrix::setSaturation(float sat, uint32_t order)
{
  // If the saturation is 1.0, then this matrix remains unchanged.
  // If the saturation is 0.0, each color is scaled by its luminance
  float satComplement  = 1.0f - sat;
  float satComplementR = LUM_R * satComplement;
  float satComplementG = LUM_G * satComplement;
  float satComplementB = LUM_B * satComplement;

  // Create the matrix:
  ColorMatrix mod(
      satComplementR + sat, satComplementR, satComplementR, 0.0f, 0.0f,
      satComplementG, satComplementG + sat, satComplementG, 0.0f, 0.0f,
      satComplementB, satComplementB, satComplementB + sat, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f, 1.0f );

  return multiply(mod, order);
}

ColorMatrix& ColorMatrix::setTint(float phi, float amount)
{
  // Rotate the hue, scale the blue and rotate back.
  return rotateHue(phi).scale(1.0f, 1.0f, 1.0f, 1.0f + amount).rotateHue(-phi);
}

ColorMatrix& ColorMatrix::rotateHue(float phi)
{
  // Rotate the gray vector to the blue axis and rotate around the blue axis.
  return multiply(PRE_HUE).rotateBlue(phi).multiply(POST_HUE);
}

bool ColorMatrix::eq(const ColorMatrix& other, float epsilon) const
{
  for (sysuint_t i = 0; i < 25; i++)
    if (!Math::feq(getData()[i], other.getData()[i], epsilon)) return false;

  return true;
}

ColorMatrix& ColorMatrix::_rotateColor(float phi, int x, int y, uint32_t order)
{
  float phiSin;
  float phiCos;
  Math::sincos(phi, &phiSin, &phiCos);

  ColorMatrix mod(ColorMatrix::IDENTITY);
  mod.m[x][x] =  phiCos;
  mod.m[x][y] = -phiSin;
  mod.m[y][x] =  phiSin;
  mod.m[y][y] =  phiCos;
  return multiply(mod, order);
}

ColorMatrix& ColorMatrix::_shearColor(int x, int y1, float col1, int y2, float col2, uint32_t order)
{
  ColorMatrix mod(ColorMatrix::IDENTITY);
  mod.m[y1][x] = col1;
  mod.m[y2][x] = col2;
  return multiply(mod, order);
}

void ColorMatrix::_copyData(float* _dst, const float* _src)
{
  memcpy(_dst, _src, sizeof(float) * 25);
}

// ============================================================================
// [Fog::ColorMatrix - Statics]
// ============================================================================

ColorMatrix ColorMatrix::GREYSCALE(
  LUM_R, LUM_R, LUM_R, 0.0f, 0.0f,
  LUM_G, LUM_G, LUM_G, 0.0f, 0.0f,
  LUM_B, LUM_B, LUM_B, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

ColorMatrix ColorMatrix::IDENTITY(
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

ColorMatrix ColorMatrix::WHITE(
  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

ColorMatrix ColorMatrix::ZERO(
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

ColorMatrix ColorMatrix::PRE_HUE(
  0.8164966106f, 0.0f, 0.5345109105f, 0.0f, 0.0f,
  -0.4082482755f, 0.7071067691f, 1.055511713f, 0.0f, 0.0f,
  -0.4082482755f, -0.7071067691f, 0.1420281678f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

ColorMatrix ColorMatrix::POST_HUE(
  0.8467885852f, -0.3779562712f, -0.3779562712f, 0.0f, 0.0f,
  -0.3729280829f, 0.3341786563f, -1.080034852f, 0.0f, 0.0f,
  0.5773502588f, 0.5773502588f, 0.5773502588f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

// Code to generate PRE_HUE and POST_HUE matrices.
#if 0
static void dumpColorMatrix(const ColorMatrix& cm)
{
  for (int i = 0; i < 5; i++)
  {
    printf("%.10gf, %.10gf, %.10gf, %.10gf, %.10gf,\n", 
      cm.m[i][0],
      cm.m[i][1],
      cm.m[i][2],
      cm.m[i][3],
      cm.m[i][4]);
  }
}

static void dumpPreHueAndPostHue()
{
  ColorMatrix preHue(ColorMatrix::IDENTITY);
  ColorMatrix postHue(ColorMatrix::IDENTITY);

  // [CMD 2006-02-22]
  //
  // The green rotation is the angle where the sin is 1 / sqrt( 3 )
  // and the cosine is sqrt( 2 / 3 ). According to the documentation
  // for QColorMatrix, this is 39.182655, but I came up with
  // 35.26439. Considering that Sjaak Priester explains that 35.0
  // gave better results than 39.182655, I wonder where the higher
  // value came from...  If you want to use 39.182655, simply
  // change the value of greenRotation below.
  const float greenRotation(Math::deg2rad(35.26439f));

  // Rotating the hue requires several matrix multiplications.  To
  // speed things up a bit, use two static matrices (pre and post
  // rotation.)

  // Prepare the preHue matrix
  // Rotate the gray vector in the red plane:
  preHue.rotateRed(Math::deg2rad(45.0f));

  // Rotate again in the green plane so it coinsides with the
  // blue axis:
  preHue.rotateGreen(-greenRotation);

  // Shear the blue plane, in order to keep the color luminance
  // constant:
  float lum[4] = { LUM_R, LUM_G, LUM_B, 1.0f };

  // Transform by the luminance vector:
  preHue.transformVector(lum);

  // Calculate the red and green factors:
  float shearRcp   = 1.0f / lum[2];
  float shearRed   = lum[0] * shearRcp;
  float shearGreen = lum[1] * shearRcp;

  // Shear the blue plane:
  preHue.shearBlue(shearRed, shearGreen);

  // Prepare the postHue matrix, which is actually the inverse of the
  // preHue matrix
  postHue.shearBlue(-shearRed, -shearGreen);
  postHue.rotateGreen(greenRotation);
  postHue.rotateRed(Math::deg2rad(-45.0f));

  dumpColorMatrix(preHue);
  dumpColorMatrix(postHue);
}
#endif

} // Fog namespace
