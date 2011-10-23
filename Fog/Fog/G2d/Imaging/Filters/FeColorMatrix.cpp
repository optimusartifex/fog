// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Imaging/Filters/FeColorMatrix.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::FeColorMatrix - Global]
// ============================================================================

struct FOG_NO_EXPORT FeColorMatrixStatic
{
  uint32_t _feType;
  float m[25];
};

// LinearRGB luminance weights.
static const float LumR = 0.213f;
static const float LumG = 0.715f;
static const float LumB = 0.072f;

#define _FOG_FE_COLOR_MATRIX_STATIC(_Name_, M00, M01, M02, M03, M04, M10, M11, M12, M13, M14, M20, M21, M22, M23, M24, M30, M31, M32, M33, M34, M40, M41, M42, M43, M44) \
  static const FeColorMatrixStatic _Name_ = \
  { \
    FE_TYPE_COLOR_MATRIX, \
    { \
      M00, M01, M02, M03, M04, \
      M10, M11, M12, M13, M14, \
      M20, M21, M22, M23, M24, \
      M30, M31, M32, M33, M34, \
      M40, M41, M42, M43, M44  \
    } \
  }

_FOG_FE_COLOR_MATRIX_STATIC(FeColorMatrix_oIdentity,
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f
);

_FOG_FE_COLOR_MATRIX_STATIC(FeColorMatrix_oZero,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f
);

_FOG_FE_COLOR_MATRIX_STATIC(FeColorMatrix_oGreyscale,
  LumR, LumR, LumR, 0.0f, 0.0f,
  LumG, LumG, LumG, 0.0f, 0.0f,
  LumB, LumB, LumB, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f
);

_FOG_FE_COLOR_MATRIX_STATIC(FeColorMatrix_oPreHue,
  0.8164966106f, 0.0f         , 0.5345109105f, 0.0f, 0.0f,
 -0.4082482755f, 0.7071067691f, 1.0555117130f, 0.0f, 0.0f,
 -0.4082482755f,-0.7071067691f, 0.1420281678f, 0.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 1.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 0.0f, 1.0f
);

_FOG_FE_COLOR_MATRIX_STATIC(FeColorMatrix_oPostHue,
  0.8467885852f,-0.3779562712f,-0.3779562712f, 0.0f, 0.0f,
 -0.3729280829f, 0.3341786563f,-1.0800348520f, 0.0f, 0.0f,
  0.5773502588f, 0.5773502588f, 0.5773502588f, 0.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 1.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 0.0f, 1.0f
);

// ============================================================================
// [Fog::FeColorMatrix - Helpers]
// ============================================================================

static bool FeColorMatrix_fitRect(uint& x0, uint& y0, uint& x1, uint& y1, const RectI* rect)
{
  FOG_ASSERT(rect != NULL);

  x0 = Math::max<int>(rect->x, 0);
  y0 = Math::max<int>(rect->y, 0);

  x1 = Math::min<int>(rect->x + rect->w, 5);
  y1 = Math::min<int>(rect->y + rect->h, 5);

  return x0 < x1 && y0 < y1;
}

// ============================================================================
// [Fog::FeColorMatrix - Construction / Destruction]
// ============================================================================

static void FOG_CDECL FeColorMatrix_ctor(FeColorMatrix* self)
{
  self->_feType = FE_TYPE_COLOR_MATRIX;
  fog_api.fecolormatrix_copy(self->m, FeColorMatrix_oIdentity.m);
}

// ============================================================================
// [Fog::FeColorMatrix - GetType]
// ============================================================================

static uint32_t FOG_CDECL FeColorMatrix_getType(const FeColorMatrix* self)
{
  const float* m = self->m;
  uint32_t type = 0;

  // RGB shear part is illustrated here:
  //   [n X X n n]
  //   [X n X n n]
  //   [X X n n n]
  //   [n n n n n]
  //   [n n n n n]
  if (m[ 1] != 0.0f || m[ 2] != 0.0f ||
      m[ 5] != 0.0f || m[ 7] != 0.0f ||
      m[10] != 0.0f || m[11] != 0.0f)
  {
    type |= FE_COLOR_MATRIX_SHEAR_RGB;
  }

  // Alpha shear part is illustrated here:
  //   [n n n X n]
  //   [n n n X n]
  //   [n n n X n]
  //   [X X X n n]
  //   [n n n n n]
  if (m[ 3] != 0.0f ||
      m[ 8] != 0.0f ||
      m[13] != 0.0f ||
      m[10] != 0.0f || m[11] != 0.0f || m[12] != 0.0f)
  {
    type |= FE_COLOR_MATRIX_SHEAR_ALPHA;
  }

  // RGB lut part is illustrated here:
  //   [X n n n n]
  //   [n X n n n]
  //   [n n X n n]
  //   [n n n n n]
  //   [n n n n n]
  if (m[ 0] != 0.0f ||
      m[ 6] != 0.0f ||
      m[12] != 0.0f)
  {
    type |= FE_COLOR_MATRIX_LUT_RGB;
  }

  // Alpha lut part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n X n]
  //   [n n n n n]
  if (m[18] != 0.0f)
  {
    type |= FE_COLOR_MATRIX_LUT_ALPHA;
  }

  //! @brief Matrix contains const RGB lut part (all cells are set to 1.0).
  //!
  //! Const RGB lut part is illustrated here:
  //!   [1 n n n n]
  //!   [n 1 n n n]
  //!   [n n 1 n n]
  //!   [n n n n n]
  //!   [n n n n n]
  if (m[ 0] == 1.0f &&
      m[ 6] == 1.0f &&
      m[12] == 1.0f)
  {
    type |= FE_COLOR_MATRIX_CONST_RGB;
  }

  //! @brief Matrix contains const alpha lut part (cell set to 1.0).
  //!
  //! Const alpha lut part is illustrated here:
  //!   [n n n n n]
  //!   [n n n n n]
  //!   [n n n n n]
  //!   [n n n 1 n]
  //!   [n n n n n]
  if (m[18] == 1.0f)
  {
    type |= FE_COLOR_MATRIX_CONST_ALPHA;
  }

  // RGB translation part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [X X X n n]
  if (m[20] != 0.0f || m[21] != 0.0f || m[22] != 0.0f)
  {
    type |= FE_COLOR_MATRIX_TRANSLATE_RGB;
  }

  // Alpha translation part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n X n]
  if (m[23] != 0.0f)
  {
    type |= FE_COLOR_MATRIX_TRANSLATE_ALPHA;
  }

  return type;
}

// ============================================================================
// [Fog::FeColorMatrix - Add]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_addMatrix(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b)
{
  uint i;

  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  for (i = 0; i < 25; i++)
  {
    dm[i] = am[i] + bm[i];
  }

  return ERR_OK;
}

static err_t FOG_CDECL FeColorMatrix_addScalar(FeColorMatrix* dst, const FeColorMatrix* a, const RectI* rect, float s)
{
  uint i;
  uint x0, y0, x1, y1;

  float *dm = dst->m;
  const float *am = a->m;

  if (rect == NULL)
  {
    for (i = 0; i < 25; i++)
    {
      dm[i] = am[i] + s;
    }
  }
  else if (FeColorMatrix_fitRect(x0, y0, x1, y1, rect))
  {
    dm += y0 * 5;
    am += y0 * 5;
    do {
      for (i = x0; i < x1; i++)
      {
        dm[i] = am[i] + s;
      }

      dm += 5;
      am += 5;
    } while (++y0 < y1);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - Subtract]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_subtractMatrix(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b)
{
  uint i;

  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  for (i = 0; i < 25; i++)
  {
    dm[i] = am[i] - bm[i];
  }

  return ERR_OK;
}

static err_t FOG_CDECL FeColorMatrix_subtractScalar(FeColorMatrix* dst, const FeColorMatrix* a, const RectI* rect, float s)
{
  uint i;
  uint x0, y0, x1, y1;

  float *dm = dst->m;
  const float *am = a->m;

  if (rect == NULL)
  {
    for (i = 0; i < 25; i++)
    {
      dm[i] = am[i] - s;
    }
  }
  else if (FeColorMatrix_fitRect(x0, y0, x1, y1, rect))
  {
    dm += y0 * 5;
    am += y0 * 5;
    do {
      for (i = x0; i < x1; i++)
      {
        dm[i] = am[i] - s;
      }

      dm += 5;
      am += 5;
    } while (++y0 < y1);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - Multiply]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_multiplyOther(FeColorMatrix* dst, const FeColorMatrix* other, uint32_t order)
{
  if (order == MATRIX_ORDER_PREPEND)
    return fog_api.fecolormatrix_multiplyMatrix(dst, other, dst);
  else
    return fog_api.fecolormatrix_multiplyMatrix(dst, dst, other);
}

static err_t FOG_CDECL FeColorMatrix_multiplyMatrix(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b)
{
  float tmp[25];

  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  if (a == dst || b == dst)
  {
    fog_api.fecolormatrix_copy(tmp, dst->m);
    if (a == dst) am = tmp;
    if (b == dst) bm = tmp;
  }

  dm[ 0] = am[ 0] * bm[ 0] + am[ 1] * bm[ 5] + am[ 2] * bm[10] + am[ 3] * bm[15] + am[ 4] * bm[20];
  dm[ 1] = am[ 0] * bm[ 1] + am[ 1] * bm[ 6] + am[ 2] * bm[11] + am[ 3] * bm[16] + am[ 4] * bm[21];
  dm[ 2] = am[ 0] * bm[ 2] + am[ 1] * bm[ 7] + am[ 2] * bm[12] + am[ 3] * bm[17] + am[ 4] * bm[22];
  dm[ 3] = am[ 0] * bm[ 3] + am[ 1] * bm[ 8] + am[ 2] * bm[13] + am[ 3] * bm[18] + am[ 4] * bm[23];
  dm[ 4] = am[ 0] * bm[ 4] + am[ 1] * bm[ 9] + am[ 2] * bm[14] + am[ 3] * bm[19] + am[ 4] * bm[24];

  dm[ 5] = am[ 5] * bm[ 0] + am[ 6] * bm[ 5] + am[ 7] * bm[10] + am[ 8] * bm[15] + am[ 9] * bm[20];
  dm[ 6] = am[ 5] * bm[ 1] + am[ 6] * bm[ 6] + am[ 7] * bm[11] + am[ 8] * bm[16] + am[ 9] * bm[21];
  dm[ 7] = am[ 5] * bm[ 2] + am[ 6] * bm[ 7] + am[ 7] * bm[12] + am[ 8] * bm[17] + am[ 9] * bm[22];
  dm[ 8] = am[ 5] * bm[ 3] + am[ 6] * bm[ 8] + am[ 7] * bm[13] + am[ 8] * bm[18] + am[ 9] * bm[23];
  dm[ 9] = am[ 5] * bm[ 4] + am[ 6] * bm[ 9] + am[ 7] * bm[14] + am[ 8] * bm[19] + am[ 9] * bm[24];

  dm[10] = am[10] * bm[ 0] + am[11] * bm[ 5] + am[12] * bm[10] + am[13] * bm[15] + am[14] * bm[20];
  dm[11] = am[10] * bm[ 1] + am[11] * bm[ 6] + am[12] * bm[11] + am[13] * bm[16] + am[14] * bm[21];
  dm[12] = am[10] * bm[ 2] + am[11] * bm[ 7] + am[12] * bm[12] + am[13] * bm[17] + am[14] * bm[22];
  dm[13] = am[10] * bm[ 3] + am[11] * bm[ 8] + am[12] * bm[13] + am[13] * bm[18] + am[14] * bm[23];
  dm[14] = am[10] * bm[ 4] + am[11] * bm[ 9] + am[12] * bm[14] + am[13] * bm[19] + am[14] * bm[24];

  dm[15] = am[15] * bm[ 0] + am[16] * bm[ 5] + am[17] * bm[10] + am[18] * bm[15] + am[19] * bm[20];
  dm[16] = am[15] * bm[ 1] + am[16] * bm[ 6] + am[17] * bm[11] + am[18] * bm[16] + am[19] * bm[21];
  dm[17] = am[15] * bm[ 2] + am[16] * bm[ 7] + am[17] * bm[12] + am[18] * bm[17] + am[19] * bm[22];
  dm[18] = am[15] * bm[ 3] + am[16] * bm[ 8] + am[17] * bm[13] + am[18] * bm[18] + am[19] * bm[23];
  dm[19] = am[15] * bm[ 4] + am[16] * bm[ 9] + am[17] * bm[14] + am[18] * bm[19] + am[19] * bm[24];

  dm[20] = am[20] * bm[ 0] + am[21] * bm[ 5] + am[22] * bm[10] + am[23] * bm[15] + am[24] * bm[20];
  dm[21] = am[20] * bm[ 1] + am[21] * bm[ 6] + am[22] * bm[11] + am[23] * bm[16] + am[24] * bm[21];
  dm[22] = am[20] * bm[ 2] + am[21] * bm[ 7] + am[22] * bm[12] + am[23] * bm[17] + am[24] * bm[22];
  dm[23] = am[20] * bm[ 3] + am[21] * bm[ 8] + am[22] * bm[13] + am[23] * bm[18] + am[24] * bm[23];
  dm[24] = am[20] * bm[ 4] + am[21] * bm[ 9] + am[22] * bm[14] + am[23] * bm[19] + am[24] * bm[24];

  return ERR_OK;
}

static err_t FOG_CDECL FeColorMatrix_multiplyScalar(FeColorMatrix* dst, const FeColorMatrix* a, const RectI* rect, float s)
{
  uint i;
  uint x0, y0, x1, y1;

  float *dm = dst->m;
  const float *am = a->m;

  if (rect == NULL)
  {
    for (i = 0; i < 25; i++)
    {
      dm[i] = am[i] * s;
    }
  }
  else if (FeColorMatrix_fitRect(x0, y0, x1, y1, rect))
  {
    dm += y0 * 5;
    am += y0 * 5;

    do {
      for (i = x0; i < x1; i++)
      {
        dm[i] = am[i] * s;
      }

      dm += 5;
      am += 5;
    } while (++y0 < y1);
  }

  return ERR_OK;
}

// Simplified premultiply two matrices.
//
// 'self'  - not restricted.
// 'other' - restriced to use only 9x9 part as illustrated below:
//
//       |a b c 0 0|
//       |e f g 0 0|
//   M = |h i j 0 0| * M
//       |0 0 0 1 0|
//       |0 0 0 0 1|
static err_t FOG_CDECL FeColorMatrix_simplifiedPremultiply(FeColorMatrix* self, const FeColorMatrix* other)
{
  FOG_ASSERT(self != other);

  float* m = self->m;
  const float* o = other->m;

  float m0 = m[0];
  float m1 = m[1];
  float m2 = m[2];
  float m3 = m[3];
  float m4 = m[4];
  float m5 = m[5];
  float m6 = m[6];
  float m7 = m[7];
  float m8 = m[8];
  float m9 = m[9];

  m[ 0] = o[ 0] * m0 + o[ 1] * m5 + o[ 2] * m[10];
  m[ 1] = o[ 0] * m1 + o[ 1] * m6 + o[ 2] * m[11];
  m[ 2] = o[ 0] * m2 + o[ 1] * m7 + o[ 2] * m[12];
  m[ 3] = o[ 0] * m3 + o[ 1] * m8 + o[ 2] * m[13];
  m[ 4] = o[ 0] * m4 + o[ 1] * m9 + o[ 2] * m[14];

  m[ 5] = o[ 5] * m0 + o[ 6] * m5 + o[ 7] * m[10];
  m[ 6] = o[ 5] * m1 + o[ 6] * m6 + o[ 7] * m[11];
  m[ 7] = o[ 5] * m2 + o[ 6] * m7 + o[ 7] * m[12];
  m[ 8] = o[ 5] * m3 + o[ 6] * m8 + o[ 7] * m[13];
  m[ 9] = o[ 5] * m4 + o[ 6] * m9 + o[ 7] * m[14];

  m[10] = o[10] * m0 + o[11] * m5 + o[12] * m[10];
  m[11] = o[10] * m1 + o[11] * m6 + o[12] * m[11];
  m[12] = o[10] * m2 + o[11] * m7 + o[12] * m[12];
  m[13] = o[10] * m3 + o[11] * m8 + o[12] * m[13];
  m[14] = o[10] * m4 + o[11] * m9 + o[12] * m[14];

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - TranslateArgb]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_translateArgb(FeColorMatrix* self, float a, float r, float g, float b, uint32_t order)
{
  float* m = self->m;

  if (order == MATRIX_ORDER_PREPEND)
  {
    //     |1 0 0 0 0|
    //     |0 1 0 0 0|
    // M = |0 0 1 0 0| * M
    //     |0 0 0 1 0|
    //     |r g b a 1|

    m[20] += r * m[ 0] + g * m[ 5] + b * m[10] + a * m[15];
    m[21] += r * m[ 1] + g * m[ 6] + b * m[11] + a * m[16];
    m[22] += r * m[ 2] + g * m[ 7] + b * m[12] + a * m[17];
    m[23] += r * m[ 3] + g * m[ 8] + b * m[13] + a * m[18];
    m[24] += r * m[ 4] + g * m[ 9] + b * m[14] + a * m[19];
  }
  else
  {
    //         |1 0 0 0 0|
    //         |0 1 0 0 0|
    // M = M * |0 0 1 0 0|
    //         |0 0 0 1 0|
    //         |r g b a 1|

    m[ 0] += m[ 4] * r;
    m[ 1] += m[ 4] * g;
    m[ 2] += m[ 4] * b;
    m[ 3] += m[ 4] * a;

    m[ 5] += m[ 9] * r;
    m[ 6] += m[ 9] * g;
    m[ 7] += m[ 9] * b;
    m[ 8] += m[ 9] * a;

    m[10] += m[14] * r;
    m[11] += m[14] * g;
    m[12] += m[14] * b;
    m[13] += m[14] * a;

    m[15] += m[19] * r;
    m[16] += m[19] * g;
    m[17] += m[19] * b;
    m[18] += m[19] * a;

    m[20] += m[24] * r;
    m[21] += m[24] * g;
    m[22] += m[24] * b;
    m[23] += m[24] * a;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - ScaleArgb]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_scaleArgb(FeColorMatrix* self, float a, float r, float g, float b, uint32_t order)
{
  float* m = self->m;

  if (order == MATRIX_ORDER_PREPEND)
  {
    //     |r 0 0 0 0|
    //     |0 g 0 0 0|
    // M = |0 0 b 0 0| * M
    //     |0 0 0 a 0|
    //     |0 0 0 0 1|

    m[ 0] *= r;
    m[ 1] *= r;
    m[ 2] *= r;
    m[ 3] *= r;
    m[ 4] *= r;

    m[ 5] *= g;
    m[ 6] *= g;
    m[ 7] *= g;
    m[ 8] *= g;
    m[ 9] *= g;

    m[10] *= b;
    m[11] *= b;
    m[12] *= b;
    m[13] *= b;
    m[14] *= b;

    m[15] *= a;
    m[16] *= a;
    m[17] *= a;
    m[18] *= a;
    m[19] *= a;
  }
  else
  {
    //         |r 0 0 0 0|
    //         |0 g 0 0 0|
    // M = M * |0 0 b 0 0|
    //         |0 0 0 a 0|
    //         |0 0 0 0 1|

    m[ 0] *= r;
    m[ 1] *= g;
    m[ 2] *= b;
    m[ 3] *= a;

    m[ 5] *= r;
    m[ 6] *= g;
    m[ 7] *= b;
    m[ 8] *= a;

    m[10] *= r;
    m[11] *= g;
    m[12] *= b;
    m[13] *= a;

    m[15] *= r;
    m[16] *= g;
    m[17] *= b;
    m[18] *= a;

    m[20] *= r;
    m[21] *= g;
    m[22] *= b;
    m[23] *= a;
  }

  return ERR_OK;
}

static err_t FOG_CDECL FeColorMatrix_scaleTint(FeColorMatrix* self, float phi, float amount)
{
  // Rotate the hue, scale the blue and rotate back.
  self->rotateHue(phi);
  self->scaleArgb(1.0f, 1.0f, 1.0f, 1.0f + amount);
  self->rotateHue(-phi);

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - Saturate]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_saturate(FeColorMatrix* self, float s, uint32_t order)
{
  // If the saturation is 1.0, then this matrix remains unchanged.
  // If the saturation is 0.0, each color is scaled by its luminance.
  float* m = self->m;

  float i = 1.0f - s;
  float r = LumR * i;
  float g = LumG * i;
  float b = LumB * i;

  float r_plus_s = r + s;
  float g_plus_s = g + s;
  float b_plus_s = b + s;

  if (order == MATRIX_ORDER_PREPEND)
  {
    //     |r+s  r   r   0   0 |
    //     | g  g+s  g   0   0 |
    // M = | b   b  b+s  0   0 | * M
    //     | 0   0   0   1   0 |
    //     | 0   0   0   0   1 |

    float x0 = m[0] + m[5] + m[10];
    float x1 = m[1] + m[6] + m[11];
    float x2 = m[2] + m[7] + m[12];
    float x3 = m[3] + m[8] + m[13];
    float x4 = m[4] + m[9] + m[14];

    m[ 0] = r * x0 + s * m[ 0];
    m[ 1] = r * x1 + s * m[ 1];
    m[ 2] = r * x2 + s * m[ 2];
    m[ 3] = r * x3 + s * m[ 3];
    m[ 4] = r * x4 + s * m[ 4];

    m[ 5] = g * x0 + s * m[ 5];
    m[ 6] = g * x1 + s * m[ 6];
    m[ 7] = g * x2 + s * m[ 7];
    m[ 8] = g * x3 + s * m[ 8];
    m[ 9] = g * x4 + s * m[ 9];

    m[10] = b * x0 + s * m[10];
    m[11] = b * x1 + s * m[11];
    m[12] = b * x2 + s * m[12];
    m[13] = b * x3 + s * m[13];
    m[14] = b * x4 + s * m[14];
  }
  else
  {
    //         |r+s  r   r   0   0 |
    //         | g  g+s  g   0   0 |
    // M = M * | b   b  b+s  0   0 |
    //         | 0   0   0   1   0 |
    //         | 0   0   0   0   1 |

    float x0 = m[ 0] * r + m[ 1] * g + m[ 2] * b;
    float x1 = m[ 5] * r + m[ 6] * g + m[ 7] * b;
    float x2 = m[10] * r + m[11] * g + m[12] * b;
    float x3 = m[15] * r + m[16] * g + m[17] * b;
    float x4 = m[20] * r + m[21] * g + m[22] * b;

    m[ 0] = x0 + s * m[ 0];
    m[ 1] = x0 + s * m[ 1];
    m[ 2] = x0 + s * m[ 2];

    m[ 5] = x1 + s * m[ 5];
    m[ 6] = x1 + s * m[ 6];
    m[ 7] = x1 + s * m[ 7];

    m[10] = x2 + s * m[10];
    m[11] = x2 + s * m[11];
    m[12] = x2 + s * m[12];

    m[15] = x3 + s * m[15];
    m[16] = x3 + s * m[16];
    m[17] = x3 + s * m[17];

    m[20] = x4 + s * m[20];
    m[21] = x4 + s * m[21];
    m[22] = x4 + s * m[22];
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::FeColorMatrix - Rotate]
// ============================================================================

static err_t FOG_CDECL FeColorMatrix_rotateHue(FeColorMatrix* self, float phi)
{
  // Rotate the gray vector to the blue axis and rotate around the blue axis.
  fog_api.fecolormatrix_simplifiedPremultiply(self,
    reinterpret_cast<const FeColorMatrix*>(&FeColorMatrix_oPreHue));

  self->rotateBlue(phi);

  fog_api.fecolormatrix_simplifiedPremultiply(self,
    reinterpret_cast<const FeColorMatrix*>(&FeColorMatrix_oPostHue));

  return ERR_OK;
}

static err_t FOG_CDECL FeColorMatrix_rotateColor(FeColorMatrix* self, int x, int y, float phi, uint32_t order)
{
  if ((uint)x >= 5 || (uint)y >= 5)
    return ERR_RT_INVALID_ARGUMENT;

  float phiSin;
  float phiCos;
  Math::sincos(phi, &phiSin, &phiCos);

  FeColorMatrix rot;

  rot[x][x] = phiCos;
  rot[x][y] =-phiSin;
  rot[y][x] = phiSin;
  rot[y][y] = phiCos;

  if (Math::max(x, y) < 3 && order == MATRIX_ORDER_PREPEND)
    return fog_api.fecolormatrix_simplifiedPremultiply(self, &rot);
  else
    return fog_api.fecolormatrix_multiplyOther(self, &rot, order);
}

static err_t FOG_CDECL FeColorMatrix_shearColor(FeColorMatrix* self, int x, int y0, float c0, int y1, float c1, uint32_t order)
{
  if ((uint)x >= 5 || (uint)y0 >= 5 || (uint)y1 >= 5)
    return ERR_RT_INVALID_ARGUMENT;

  FeColorMatrix shear;

  shear[y0][x] = c0;
  shear[y1][x] = c1;

  if (Math::max(x, y0, y1) < 3 && order == MATRIX_ORDER_PREPEND)
    return fog_api.fecolormatrix_simplifiedPremultiply(self, &shear);
  else
    return fog_api.fecolormatrix_multiplyOther(self, &shear, order);
}

// ============================================================================
// [Fog::FeColorMatrix - Map]
// ============================================================================

static void FOG_CDECL FeColorMatrix_mapArgb32(const FeColorMatrix* self, Argb32* dst, const Argb32* src)
{
  const float* m = self->m;

  float r = float(src->r);
  float g = float(src->g);
  float b = float(src->b);
  float a = float(src->a);

  int tr = Math::iround(r * m[0] + g * m[5] + b * m[10] + a * m[15] + 255.0f * m[20]);
  int tg = Math::iround(r * m[1] + g * m[6] + b * m[11] + a * m[16] + 255.0f * m[21]);
  int tb = Math::iround(r * m[2] + g * m[7] + b * m[12] + a * m[17] + 255.0f * m[22]);
  int ta = Math::iround(r * m[3] + g * m[8] + b * m[13] + a * m[18] + 255.0f * m[23]);

  dst->r = Math::boundToByte(tr);
  dst->g = Math::boundToByte(tg);
  dst->b = Math::boundToByte(tb);
  dst->a = Math::boundToByte(ta);
}

static void FOG_CDECL FeColorMatrix_mapArgb64(const FeColorMatrix* self, Argb64* dst, const Argb64* src)
{
  const float* m = self->m;

  float r = float(src->r);
  float g = float(src->g);
  float b = float(src->b);
  float a = float(src->a);

  int tr = Math::iround(r * m[0] + g * m[5] + b * m[10] + a * m[15] + 65535.0f * m[20]);
  int tg = Math::iround(r * m[1] + g * m[6] + b * m[11] + a * m[16] + 65535.0f * m[21]);
  int tb = Math::iround(r * m[2] + g * m[7] + b * m[12] + a * m[17] + 65535.0f * m[22]);
  int ta = Math::iround(r * m[3] + g * m[8] + b * m[13] + a * m[18] + 65535.0f * m[23]);

  dst->r = Math::boundToWord(tr);
  dst->g = Math::boundToWord(tg);
  dst->b = Math::boundToWord(tb);
  dst->a = Math::boundToWord(ta);
}

static void FOG_CDECL FeColorMatrix_mapArgbF(const FeColorMatrix* self, ArgbF* dst, const ArgbF* src)
{
  const float* m = self->m;

  float r = src->r;
  float g = src->g;
  float b = src->b;
  float a = src->a;

  dst->r = r * m[0] + g * m[5] + b * m[10] + a * m[15] + m[20];
  dst->g = r * m[1] + g * m[6] + b * m[11] + a * m[16] + m[21];
  dst->b = r * m[2] + g * m[7] + b * m[12] + a * m[17] + m[22];
  dst->a = r * m[3] + g * m[8] + b * m[13] + a * m[18] + m[23];
}

// ============================================================================
// [Fog::FeColorMatrix - Copy]
// ============================================================================

static void FOG_CDECL FeColorMatrix_copy(float* dst, const float* src)
{
  MemOps::copy(dst, src, 25 * sizeof(float));
}

// ============================================================================
// [Fog::FeColorMatrix - Equality]
// ============================================================================

static bool FOG_CDECL FeColorMatrix_eq(const FeColorMatrix* a, const FeColorMatrix* b)
{
  return MemOps::eq(a->m, b->m, 25 * sizeof(float));
}

// ============================================================================
// [Fog::FeColorMatrix - Dump]
// ============================================================================

// TODO: Rerun, update hue rotation to use the snippet defined by SVG 1.1 instead.
#if 0
// Dump code to generate PreHue and PostHue matrices. The original code to
// generate these matrices comes probably from QFeColorMatrix class by Sjaak
// Priester (sjaak@sjaakpriester.nl). I used some comments from the original
// code which should describe how pre-hue and post-hue matrices are generated.

// Dump FeColorMatrix as C code to stdout.
static void FeColorMatrix_dump(const FeColorMatrix& m, const char* name)
{
  printf("static const float %s[25] =\n");
  printf("{\n");

  for (int i = 0; i < 5; i++)
  {
    printf("%.10gf, %.10gf, %.10gf, %.10gf, %.10gf",
      m[i][0], m[i][1], m[i][2], m[i][3], m[i][4]);
    printf(i != 4 ? ",\n" : "\n");
  }

  printf("};\n");
}

// Dump pre-hue and post-hue matrices.
static void FeColorMatrix_dumpHelpers()
{
  FeColorMatrix preHue;
  FeColorMatrix postHue;

  // NOTE: Theoretically, greenRotation should have the value of 39.182655
  // degrees, being the angle for which the sine is 1/(sqrt(3)), and the
  // cosine is sqrt(2/3). However, I found that using a slightly smaller angle
  // works better. In particular, the greys in the image are not visibly
  // affected with the smaller angle, while they deviate a little bit with the
  // theoretical value. An explanation escapes me for now. If you rather stick
  // with the theory, change the comments in the previous lines.
  float greenRotation = Math::deg2rad(35.0f);

  // Rotate the gray vector in the red plane:
  preHue.rotateRed(Math::deg2rad(45.0f));

  // Rotate again in the green plane so it coinsides with the blue axis.
  preHue.rotateGreen(-greenRotation, MATRIX_ORDER_APPEND);

  // Shear the blue plane, in order to keep the color luminance constant.
  ArgbF lum(1.0f, LumR, LumG, LumB);
  preHue.mapArgb(lum);
  float red = lum.r / lum.b;
  float green = lum.g / lum.b;
  preHue.shearBlue(red, green);

  // Prepare the PostHue matrix, which is actually the inverse of the PreHue.
  postHue.shearBlue(-red, -green);
  postHue.rotateGreen(greenRotation);
  postHue.rotateRed(Math::deg2rad(-45.0f));

  FeColorMatrix_dump(preHue, "FeColorMatrix_oPreHue");
  FeColorMatrix_dump(postHue, "FeColorMatrix_oPostHue");
}
#endif

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE(FeColorMatrix_init_SSE)

FOG_NO_EXPORT void FeColorMatrix_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.fecolormatrix_ctor = FeColorMatrix_ctor;
  fog_api.fecolormatrix_getType = FeColorMatrix_getType;

  fog_api.fecolormatrix_addMatrix = FeColorMatrix_addMatrix;
  fog_api.fecolormatrix_addScalar = FeColorMatrix_addScalar;
  fog_api.fecolormatrix_subtractMatrix = FeColorMatrix_subtractMatrix;
  fog_api.fecolormatrix_subtractScalar = FeColorMatrix_subtractScalar;
  fog_api.fecolormatrix_multiplyOther = FeColorMatrix_multiplyOther;
  fog_api.fecolormatrix_multiplyMatrix = FeColorMatrix_multiplyMatrix;
  fog_api.fecolormatrix_multiplyScalar = FeColorMatrix_multiplyScalar;
  fog_api.fecolormatrix_simplifiedPremultiply = FeColorMatrix_simplifiedPremultiply;

  fog_api.fecolormatrix_translateArgb = FeColorMatrix_translateArgb;
  fog_api.fecolormatrix_scaleArgb = FeColorMatrix_scaleArgb;
  fog_api.fecolormatrix_scaleTint = FeColorMatrix_scaleTint;
  fog_api.fecolormatrix_saturate = FeColorMatrix_saturate;
  fog_api.fecolormatrix_rotateHue = FeColorMatrix_rotateHue;
  fog_api.fecolormatrix_rotateColor = FeColorMatrix_rotateColor;
  fog_api.fecolormatrix_shearColor = FeColorMatrix_shearColor;

  fog_api.fecolormatrix_mapArgb32 = FeColorMatrix_mapArgb32;
  fog_api.fecolormatrix_mapArgb64 = FeColorMatrix_mapArgb64;
  fog_api.fecolormatrix_mapArgbF = FeColorMatrix_mapArgbF;

  fog_api.fecolormatrix_copy = FeColorMatrix_copy;
  fog_api.fecolormatrix_eq = FeColorMatrix_eq;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  fog_api.fecolormatrix_oIdentity = reinterpret_cast<const FeColorMatrix*>(&FeColorMatrix_oIdentity);
  fog_api.fecolormatrix_oZero = reinterpret_cast<const FeColorMatrix*>(&FeColorMatrix_oZero);
  fog_api.fecolormatrix_oGreyscale = reinterpret_cast<const FeColorMatrix*>(&FeColorMatrix_oGreyscale);
  fog_api.fecolormatrix_oPreHue = reinterpret_cast<const FeColorMatrix*>(&FeColorMatrix_oPreHue);
  fog_api.fecolormatrix_oPostHue = reinterpret_cast<const FeColorMatrix*>(&FeColorMatrix_oPostHue);

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE(FeColorMatrix_init_SSE)
}

} // Fog namespace
