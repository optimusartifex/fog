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
#include <Fog/G2d/Imaging/Filters/ColorMatrix.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {

// ============================================================================
// [Fog::ColorMatrix - Global]
// ============================================================================

struct FOG_NO_EXPORT ColorMatrixStatic
{
  uint32_t _filterType;
  float m[25];
};

// LinearRGB luminance weights.
static const float LumR = 0.213f;
static const float LumG = 0.715f;
static const float LumB = 0.072f;

#define _FOG_COLOR_MATRIX_STATIC(_Name_, M00, M01, M02, M03, M04, M10, M11, M12, M13, M14, M20, M21, M22, M23, M24, M30, M31, M32, M33, M34, M40, M41, M42, M43, M44) \
  static const ColorMatrixStatic _Name_ = \
  { \
    IMAGE_FILTER_TYPE_COLOR_MATRIX, \
    { \
      M00, M01, M02, M03, M04, \
      M10, M11, M12, M13, M14, \
      M20, M21, M22, M23, M24, \
      M30, M31, M32, M33, M34, \
      M40, M41, M42, M43, M44  \
    } \
  }

_FOG_COLOR_MATRIX_STATIC(ColorMatrix_oIdentity,
  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f
);

_FOG_COLOR_MATRIX_STATIC(ColorMatrix_oZero,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 0.0f
);

_FOG_COLOR_MATRIX_STATIC(ColorMatrix_oGreyscale,
  LumR, LumR, LumR, 0.0f, 0.0f,
  LumG, LumG, LumG, 0.0f, 0.0f,
  LumB, LumB, LumB, 0.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 0.0f, 1.0f
);

_FOG_COLOR_MATRIX_STATIC(ColorMatrix_oPreHue,
  0.8164966106f, 0.0f         , 0.5345109105f, 0.0f, 0.0f,
 -0.4082482755f, 0.7071067691f, 1.0555117130f, 0.0f, 0.0f,
 -0.4082482755f,-0.7071067691f, 0.1420281678f, 0.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 1.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 0.0f, 1.0f
);

_FOG_COLOR_MATRIX_STATIC(ColorMatrix_oPostHue,
  0.8467885852f,-0.3779562712f,-0.3779562712f, 0.0f, 0.0f,
 -0.3729280829f, 0.3341786563f,-1.0800348520f, 0.0f, 0.0f,
  0.5773502588f, 0.5773502588f, 0.5773502588f, 0.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 1.0f, 0.0f,
  0.0f         , 0.0f         , 0.0f         , 0.0f, 1.0f
);

// ============================================================================
// [Fog::ColorMatrix - Helpers]
// ============================================================================

static bool ColorMatrix_fitRect(uint& x0, uint& y0, uint& x1, uint& y1, const RectI* rect)
{
  FOG_ASSERT(rect != NULL);

  x0 = Math::max<int>(rect->x, 0);
  y0 = Math::max<int>(rect->y, 0);

  x1 = Math::min<int>(rect->x + rect->w, 5);
  y1 = Math::min<int>(rect->y + rect->h, 5);

  return x0 < x1 && y0 < y1;
}

// ============================================================================
// [Fog::ColorMatrix - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ColorMatrix_ctor(ColorMatrix* self)
{
  self->_filterType = IMAGE_FILTER_TYPE_COLOR_MATRIX;
  _api.colormatrix_copy(self->m, ColorMatrix_oIdentity.m);
}

// ============================================================================
// [Fog::ColorMatrix - GetType]
// ============================================================================

static uint32_t FOG_CDECL ColorMatrix_getType(const ColorMatrix* self)
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
    type |= COLOR_MATRIX_SHEAR_RGB;
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
    type |= COLOR_MATRIX_SHEAR_ALPHA;
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
    type |= COLOR_MATRIX_LUT_RGB;
  }

  // Alpha lut part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n X n]
  //   [n n n n n]
  if (m[18] != 0.0f)
  {
    type |= COLOR_MATRIX_LUT_ALPHA;
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
    type |= COLOR_MATRIX_CONST_RGB;
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
    type |= COLOR_MATRIX_CONST_ALPHA;
  }

  // RGB translation part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [X X X n n]
  if (m[20] != 0.0f || m[21] != 0.0f || m[22] != 0.0f)
  {
    type |= COLOR_MATRIX_TRANSLATE_RGB;
  }

  // Alpha translation part is illustrated here:
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n n n]
  //   [n n n X n]
  if (m[23] != 0.0f)
  {
    type |= COLOR_MATRIX_TRANSLATE_ALPHA;
  }

  return type;
}

// ============================================================================
// [Fog::ColorMatrix - Add]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_addMatrix(ColorMatrix* dst, const ColorMatrix* a, const ColorMatrix* b)
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

static err_t FOG_CDECL ColorMatrix_addScalar(ColorMatrix* dst, const ColorMatrix* a, const RectI* rect, float s)
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
  else if (ColorMatrix_fitRect(x0, y0, x1, y1, rect))
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
// [Fog::ColorMatrix - Subtract]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_subtractMatrix(ColorMatrix* dst, const ColorMatrix* a, const ColorMatrix* b)
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

static err_t FOG_CDECL ColorMatrix_subtractScalar(ColorMatrix* dst, const ColorMatrix* a, const RectI* rect, float s)
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
  else if (ColorMatrix_fitRect(x0, y0, x1, y1, rect))
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
// [Fog::ColorMatrix - Multiply]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_multiplyOther(ColorMatrix* dst, const ColorMatrix* other, uint32_t order)
{
  if (order == MATRIX_ORDER_PREPEND)
    return _api.colormatrix_multiplyMatrix(dst, other, dst);
  else
    return _api.colormatrix_multiplyMatrix(dst, dst, other);
}

static err_t FOG_CDECL ColorMatrix_multiplyMatrix(ColorMatrix* dst, const ColorMatrix* a, const ColorMatrix* b)
{
  float tmp[25];

  float *dm = dst->m;
  const float *am = a->m;
  const float *bm = b->m;

  if (a == dst || b == dst)
  {
    _api.colormatrix_copy(tmp, dst->m);
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

static err_t FOG_CDECL ColorMatrix_multiplyScalar(ColorMatrix* dst, const ColorMatrix* a, const RectI* rect, float s)
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
  else if (ColorMatrix_fitRect(x0, y0, x1, y1, rect))
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
static err_t FOG_CDECL ColorMatrix_simplifiedPremultiply(ColorMatrix* self, const ColorMatrix* other)
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
// [Fog::ColorMatrix - TranslateArgb]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_translateArgb(ColorMatrix* self, float a, float r, float g, float b, uint32_t order)
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
// [Fog::ColorMatrix - ScaleArgb]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_scaleArgb(ColorMatrix* self, float a, float r, float g, float b, uint32_t order)
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

static err_t FOG_CDECL ColorMatrix_scaleTint(ColorMatrix* self, float phi, float amount)
{
  // Rotate the hue, scale the blue and rotate back.
  self->rotateHue(phi);
  self->scaleArgb(1.0f, 1.0f, 1.0f, 1.0f + amount);
  self->rotateHue(-phi);

  return ERR_OK;
}

// ============================================================================
// [Fog::ColorMatrix - Saturate]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_saturate(ColorMatrix* self, float s, uint32_t order)
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
// [Fog::ColorMatrix - Rotate]
// ============================================================================

static err_t FOG_CDECL ColorMatrix_rotateHue(ColorMatrix* self, float phi)
{
  // Rotate the gray vector to the blue axis and rotate around the blue axis.
  _api.colormatrix_simplifiedPremultiply(self,
    reinterpret_cast<const ColorMatrix*>(&ColorMatrix_oPreHue));

  self->rotateBlue(phi);

  _api.colormatrix_simplifiedPremultiply(self,
    reinterpret_cast<const ColorMatrix*>(&ColorMatrix_oPostHue));

  return ERR_OK;
}

static err_t FOG_CDECL ColorMatrix_rotateColor(ColorMatrix* self, int x, int y, float phi, uint32_t order)
{
  if ((uint)x >= 5 || (uint)y >= 5)
    return ERR_RT_INVALID_ARGUMENT;

  float phiSin;
  float phiCos;
  Math::sincos(phi, &phiSin, &phiCos);

  ColorMatrix rot;

  rot[x][x] = phiCos;
  rot[x][y] =-phiSin;
  rot[y][x] = phiSin;
  rot[y][y] = phiCos;

  if (Math::max(x, y) < 3 && order == MATRIX_ORDER_PREPEND)
    return _api.colormatrix_simplifiedPremultiply(self, &rot);
  else
    return _api.colormatrix_multiplyOther(self, &rot, order);
}

static err_t FOG_CDECL ColorMatrix_shearColor(ColorMatrix* self, int x, int y0, float c0, int y1, float c1, uint32_t order)
{
  if ((uint)x >= 5 || (uint)y0 >= 5 || (uint)y1 >= 5)
    return ERR_RT_INVALID_ARGUMENT;

  ColorMatrix shear;

  shear[y0][x] = c0;
  shear[y1][x] = c1;

  if (Math::max(x, y0, y1) < 3 && order == MATRIX_ORDER_PREPEND)
    return _api.colormatrix_simplifiedPremultiply(self, &shear);
  else
    return _api.colormatrix_multiplyOther(self, &shear, order);
}

// ============================================================================
// [Fog::ColorMatrix - Map]
// ============================================================================

static void FOG_CDECL ColorMatrix_mapArgb32(const ColorMatrix* self, Argb32* dst, const Argb32* src)
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

static void FOG_CDECL ColorMatrix_mapArgb64(const ColorMatrix* self, Argb64* dst, const Argb64* src)
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

static void FOG_CDECL ColorMatrix_mapArgbF(const ColorMatrix* self, ArgbF* dst, const ArgbF* src)
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
// [Fog::ColorMatrix - Copy]
// ============================================================================

static void FOG_CDECL ColorMatrix_copy(float* dst, const float* src)
{
  MemOps::copy(dst, src, 25 * sizeof(float));
}

// ============================================================================
// [Fog::ColorMatrix - Equality]
// ============================================================================

static bool FOG_CDECL ColorMatrix_eq(const ColorMatrix* a, const ColorMatrix* b)
{
  return MemOps::eq(a->m, b->m, 25 * sizeof(float));
}

// ============================================================================
// [Fog::ColorMatrix - Dump]
// ============================================================================

// TODO: Rerun, update hue rotation to use the snippet defined by SVG 1.1 instead.
#if 0
// Dump code to generate PreHue and PostHue matrices. The original code to
// generate these matrices comes probably from QColorMatrix class by Sjaak
// Priester (sjaak@sjaakpriester.nl). I used some comments from the original
// code which should describe how pre-hue and post-hue matrices are generated.

// Dump ColorMatrix as C code to stdout.
static void ColorMatrix_dump(const ColorMatrix& m, const char* name)
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
static void ColorMatrix_dumpHelpers()
{
  ColorMatrix preHue;
  ColorMatrix postHue;

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

  ColorMatrix_dump(preHue, "ColorMatrix_oPreHue");
  ColorMatrix_dump(postHue, "ColorMatrix_oPostHue");
}
#endif

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE(ColorMatrix_init_SSE)

FOG_NO_EXPORT void ColorMatrix_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  _api.colormatrix_ctor = ColorMatrix_ctor;
  _api.colormatrix_getType = ColorMatrix_getType;

  _api.colormatrix_addMatrix = ColorMatrix_addMatrix;
  _api.colormatrix_addScalar = ColorMatrix_addScalar;
  _api.colormatrix_subtractMatrix = ColorMatrix_subtractMatrix;
  _api.colormatrix_subtractScalar = ColorMatrix_subtractScalar;
  _api.colormatrix_multiplyOther = ColorMatrix_multiplyOther;
  _api.colormatrix_multiplyMatrix = ColorMatrix_multiplyMatrix;
  _api.colormatrix_multiplyScalar = ColorMatrix_multiplyScalar;
  _api.colormatrix_simplifiedPremultiply = ColorMatrix_simplifiedPremultiply;

  _api.colormatrix_translateArgb = ColorMatrix_translateArgb;
  _api.colormatrix_scaleArgb = ColorMatrix_scaleArgb;
  _api.colormatrix_scaleTint = ColorMatrix_scaleTint;
  _api.colormatrix_saturate = ColorMatrix_saturate;
  _api.colormatrix_rotateHue = ColorMatrix_rotateHue;
  _api.colormatrix_rotateColor = ColorMatrix_rotateColor;
  _api.colormatrix_shearColor = ColorMatrix_shearColor;

  _api.colormatrix_mapArgb32 = ColorMatrix_mapArgb32;
  _api.colormatrix_mapArgb64 = ColorMatrix_mapArgb64;
  _api.colormatrix_mapArgbF = ColorMatrix_mapArgbF;

  _api.colormatrix_copy = ColorMatrix_copy;
  _api.colormatrix_eq = ColorMatrix_eq;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  _api.colormatrix_oIdentity = reinterpret_cast<const ColorMatrix*>(&ColorMatrix_oIdentity);
  _api.colormatrix_oZero = reinterpret_cast<const ColorMatrix*>(&ColorMatrix_oZero);
  _api.colormatrix_oGreyscale = reinterpret_cast<const ColorMatrix*>(&ColorMatrix_oGreyscale);
  _api.colormatrix_oPreHue = reinterpret_cast<const ColorMatrix*>(&ColorMatrix_oPreHue);
  _api.colormatrix_oPostHue = reinterpret_cast<const ColorMatrix*>(&ColorMatrix_oPostHue);

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE(ColorMatrix_init_SSE)
}

} // Fog namespace
