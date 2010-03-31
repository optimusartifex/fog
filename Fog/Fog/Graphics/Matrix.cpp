// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//
// Affine transformations
//
//----------------------------------------------------------------------------

#include <Fog/Core/Math.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Matrix.h>

namespace Fog {

// ============================================================================
// [Fog::FloatMatrix]
// ============================================================================

FloatMatrix FloatMatrix::fromRotation(float a)
{
  float aSin;
  float aCos;
  Math::sincos(a, &aSin, &aCos);

  return FloatMatrix(aCos, aSin, -aSin, aCos, 0.0f, 0.0f);
}

FloatMatrix FloatMatrix::fromScaling(float s)
{
  return FloatMatrix(s, 0.0f, 0.0f, s, 0.0f, 0.0f);
}

FloatMatrix FloatMatrix::fromScaling(float x, float y)
{
  return FloatMatrix(x, 0.0f, 0.0f, y, 0.0f, 0.0f);
}

FloatMatrix FloatMatrix::fromTranslation(float x, float y)
{
  return FloatMatrix(1.0f, 0.0f, 0.0f, 1.0f, x, y);
}

FloatMatrix FloatMatrix::fromSkewing(float x, float y)
{
  return FloatMatrix(1.0f, tan(y), tan(x), 1.0f, 0.0f, 0.0f);
}

FloatMatrix FloatMatrix::fromLineSegment(float x1, float y1, float x2, float y2, float dist)
{
  FloatMatrix m;

  float dx = x2 - x1;
  float dy = y2 - y1;

  if (dist > 0.0f)
  {
    float s = Math::sqrt(dx * dx + dy * dy) / dist;
    m.scale(s, s, MATRIX_APPEND);
  }
  m.rotate(atan2(dy, dx), MATRIX_APPEND);
  m.translate(x1, y1, MATRIX_APPEND);

  return m;
}

FloatMatrix FloatMatrix::fromReflectionUnit(float ux, float uy)
{
  float ux2 = ux * 2.0f;
  float uy2 = uy * 2.0f;

  return FloatMatrix(ux2 * ux - 1.0f, ux2 * uy, ux2 * uy, uy2 * uy - 1.0f, 0.0f, 0.0f);
}

FloatMatrix FloatMatrix::fromReflection(float a)
{
  float aSin;
  float aCos;

  Math::sincos(a, &aSin, &aCos);
  return fromReflectionUnit(aCos, aSin);
}

FloatMatrix FloatMatrix::fromReflection(float x, float y)
{
  float t = Math::sqrt(x * x + y * y);
  return fromReflectionUnit(x / t, y / t);
}

FloatMatrix& FloatMatrix::parlToParl(const float* src, const float* dst)
{
  sx  = src[2] - src[0];
  shy = src[3] - src[1];
  shx = src[4] - src[0];
  sy  = src[5] - src[1];
  tx  = src[0];
  ty  = src[1];

  invert();

  multiply(
    FloatMatrix(
      dst[2] - dst[0],
      dst[3] - dst[1],
      dst[4] - dst[0],
      dst[5] - dst[1],
      dst[0], dst[1]), MATRIX_APPEND);

  return *this;
}

FloatMatrix& FloatMatrix::rectToParl(float x1, float y1, float x2, float y2, const float* parl)
{
  float src[6];

  src[0] = x1; src[1] = y1;
  src[2] = x2; src[3] = y1;
  src[4] = x2; src[5] = y2;
  parlToParl(src, parl);

  return *this;
}

FloatMatrix& FloatMatrix::parlToRect(const float* parl, float x1, float y1, float x2, float y2)
{
  float dst[6];

  dst[0] = x1; dst[1] = y1;
  dst[2] = x2; dst[3] = y1;
  dst[4] = x2; dst[5] = y2;
  parlToParl(parl, dst);

  return *this;
}

FloatMatrix& FloatMatrix::reset()
{
  sx  = sy  = 1.0f;
  shy = shx = tx = ty = 0.0f;

  return *this;
}

FloatMatrix& FloatMatrix::translate(float x, float y, uint32_t order)
{
  if (order == MATRIX_APPEND)
  {
    tx += x;
    ty += y;
  }
  else
  {
    tx += x * sx  + y * shx;
    ty += x * shy + y * sy ;
  }

  return *this;
}

FloatMatrix& FloatMatrix::rotate(float a, uint32_t order)
{
  float aSin;
  float aCos;
  Math::sincos(a, &aSin, &aCos);

  if (order == MATRIX_APPEND)
  {
    float t0 = sx  * aCos + shy * -aSin;
    float t2 = shx * aCos + sy  * -aSin;
    float t4 = tx  * aCos + ty  * -aSin;

    shy = sx  * aSin + shy * aCos;
    sy  = shx * aSin + sy  * aCos;
    ty  = tx  * aSin + ty  * aCos;

    sx  = t0;
    shx = t2;
    tx  = t4;
  }
  else
  {
    float t0 = sx  * aCos  + shx * aSin;
    float t1 = shy * aCos  + sy  * aSin;

    shx = sx  * -aSin + shx * aCos;
    sy  = shy * -aSin + sy  * aCos;

    sx  = t0;
    shy = t1;
  }

  return *this;
}

FloatMatrix& FloatMatrix::scale(float x, float y, uint32_t order)
{
  sx  *= x;
  sy  *= y;

  shx *= x;
  shy *= y;

  if (order == MATRIX_APPEND)
  {
    tx  *= x;
    ty  *= y;
  }

  return *this;
}

FloatMatrix& FloatMatrix::skew(float x, float y, uint32_t order)
{
  float xTan = tan(x);
  float yTan = tan(y);

  if (order == MATRIX_APPEND)
  {
    float t0 = sx  + shy * xTan;
    float t2 = shx + sy  * xTan;
    float t4 = tx  + ty  * xTan;

    shy = sx  * yTan + shy;
    sy  = shx * yTan + sy ;
    ty  = tx  * yTan + ty ;

    sx  = t0;
    shx = t2;
    tx  = t4;
  }
  else
  {
    float t0 = sx  + shx * yTan;
    float t1 = shy + sy  * yTan;

    shx = sx  * xTan + shx;
    sy  = shy * xTan + sy ;

    sx  = t0;
    shy = t1;
  }

  return *this;
}

FloatMatrix& FloatMatrix::multiply(const FloatMatrix& m, uint32_t order)
{
  if (order == MATRIX_APPEND)
  {
    float t0 = sx  * m.sx + shy * m.shx;
    float t2 = shx * m.sx + sy  * m.shx;
    float t4 = tx  * m.sx + ty  * m.shx + m.tx;

    shy = sx  * m.shy + shy * m.sy;
    sy  = shx * m.shy + sy  * m.sy;
    ty  = tx  * m.shy + ty  * m.sy + m.ty;

    sx  = t0;
    shx = t2;
    tx  = t4;
  }
  else
  {
    tx = sx  * m.tx  + shx * m.ty + tx;
    ty = shy * m.tx  + sy  * m.ty + ty;

    float t0 = sx  * m.sx  + shx * m.shy;
    float t1 = shy * m.sx  + sy  * m.shy;

    shx = sx  * m.shx + shx * m.sy;
    sy  = shy * m.shx + sy  * m.sy;

    sx  = t0;
    shy = t1;
  }

  return *this;
}

FloatMatrix FloatMatrix::multiplied(const FloatMatrix& m, uint32_t order) const
{
  FloatMatrix result(DONT_INITIALIZE);

  const FloatMatrix* m0 = this;
  const FloatMatrix* m1 = &m;

  if (order == MATRIX_PREPEND)
  {
    m0 = &m;
    m1 = this;
  }

  result.sx  = m0->sx  * m1->sx  + m0->shy * m1->shx;
  result.shy = m0->sx  * m1->shy + m0->shy * m1->sy;

  result.shx = m0->shx * m1->sx  + m0->sy  * m1->shx;
  result.sy  = m0->shx * m1->shy + m0->sy  * m1->sy;

  result.tx  = m0->tx  * m1->sx  + m0->ty  * m1->shx + m1->tx;
  result.ty  = m0->tx  * m1->shy + m0->ty  * m1->sy  + m1->ty;

  return result;
}

FloatMatrix& FloatMatrix::multiplyInv(const FloatMatrix& m, uint32_t order)
{
  return multiply(m.inverted(), order);
}

FloatMatrix& FloatMatrix::invert()
{
  float d, t0, t4;

  d  = getDeterminantReciprocal();

  t0  =  sy  * d;
  sy  =  sx  * d;
  shy = -shy * d;
  shx = -shx * d;

  t4 = -tx * t0  - ty * shx;
  ty = -tx * shy - ty * sy;

  sx = t0;
  tx = t4;

  return *this;
}

FloatMatrix FloatMatrix::inverted() const
{
  FloatMatrix result(DONT_INITIALIZE);
  float d;

  d  = getDeterminantReciprocal();

  result.sx  =  sy  * d;
  result.sy  =  sx  * d;
  result.shy = -shy * d;
  result.shx = -shx * d;

  result.tx = -tx * result.sx  - ty * result.shx;
  result.ty = -tx * result.shy - ty * result.sy;

  return result;
}

FloatMatrix& FloatMatrix::flipX()
{
  sx  = -sx;
  shy = -shy;
  tx  = -tx;

  return *this;
}

FloatMatrix& FloatMatrix::flipY()
{
  shx = -shx;
  sy  = -sy;
  ty  = -ty;

  return *this;
}

int FloatMatrix::getType() const
{
  int type = 0;

  if (Math::feq(tx, 0.0f, Math::DEFAULT_FLOAT_EPSILON)) type |= MATRIX_TYPE_TRANSLATE;
  if (Math::feq(ty, 0.0f, Math::DEFAULT_FLOAT_EPSILON)) type |= MATRIX_TYPE_TRANSLATE;

  if (Math::feq(sx, 1.0f, Math::DEFAULT_FLOAT_EPSILON)) type |= MATRIX_TYPE_SCALE;
  if (Math::feq(sy, 1.0f, Math::DEFAULT_FLOAT_EPSILON)) type |= MATRIX_TYPE_SCALE;

  if (Math::feq(shx, 1.0f, Math::DEFAULT_FLOAT_EPSILON)) type |= MATRIX_TYPE_SHEAR;
  if (Math::feq(shy, 1.0f, Math::DEFAULT_FLOAT_EPSILON)) type |= MATRIX_TYPE_SHEAR;

  return type;
}

void FloatMatrix::transformPoint(float* x, float* y) const
{
  float _x = *x;
  float _y = *y;

  *x = _x * sx  + _y * shx + tx;
  *y = _x * shy + _y * sy  + ty;
}

void FloatMatrix::transformVector(float* x, float* y) const
{
  float _x = *x;
  float _y = *y;

  *x = _x * sx  + _y * shx;
  *y = _x * shy + _y * sy;
}

void FloatMatrix::transformPoints(FloatPoint* dst, const FloatPoint* src, sysuint_t count) const
{
  for (sysuint_t i = 0; i < count; i++)
  {
    float x = src[i].x;
    float y = src[i].y;

    dst[i].x = x * sx  + y * shx + tx;
    dst[i].y = x * shy + y * sy  + ty;
  }
}

bool FloatMatrix::isValid(float epsilon) const
{
  return Math::abs(sx) > epsilon && Math::abs(sy) > epsilon;
}

bool FloatMatrix::isIdentity(float epsilon) const
{
  return Math::feq(sx,  1.0f, epsilon) &&
         Math::feq(shy, 0.0f, epsilon) &&
         Math::feq(shx, 0.0f, epsilon) &&
         Math::feq(sy,  1.0f, epsilon) &&
         Math::feq(tx,  0.0f, epsilon) &&
         Math::feq(ty,  0.0f, epsilon);
}

bool FloatMatrix::isEqual(const FloatMatrix& m, float epsilon) const
{
  return Math::feq(sx,  m.sx,  epsilon) &&
         Math::feq(shy, m.shy, epsilon) &&
         Math::feq(shx, m.shx, epsilon) &&
         Math::feq(sy,  m.sy,  epsilon) &&
         Math::feq(tx,  m.tx,  epsilon) &&
         Math::feq(ty,  m.ty,  epsilon);
}

FloatPoint FloatMatrix::getTranslation() const
{
  return FloatPoint(tx, ty);
}

float FloatMatrix::getRotation() const
{
  return atan2(shy, sx);
}

FloatPoint FloatMatrix::getScaling() const
{
  FloatMatrix t(this->multiplied(fromRotation(-getRotation())));

  float x = t.sx + t.shx;
  float y = t.shy + t.sy;

  return FloatPoint(x, y);
}

float FloatMatrix::getAverageScaling() const
{
  float x = 0.707106781f * (sx + shx);
  float y = 0.707106781f * (shy + sy);

  return Math::sqrt(x * x + y * y);
}

FloatPoint FloatMatrix::getAbsoluteScaling() const
{
  // Used to calculate scaling coefficients in image resampling.
  // When there is considerable shear this method gives us much
  // better estimation than just sx, sy.
  float x = Math::sqrt(sx  * sx  + shx * shx);
  float y = Math::sqrt(shy * shy + sy  * sy);

  return FloatPoint(x, y);
}

// ============================================================================
// [Fog::DoubleMatrix]
// ============================================================================

DoubleMatrix DoubleMatrix::fromRotation(double a)
{
  double aSin;
  double aCos;
  Math::sincos(a, &aSin, &aCos);

  return DoubleMatrix(aCos, aSin, -aSin, aCos, 0.0, 0.0);
}

DoubleMatrix DoubleMatrix::fromScaling(double s)
{
  return DoubleMatrix(s, 0.0, 0.0, s, 0.0, 0.0);
}

DoubleMatrix DoubleMatrix::fromScaling(double x, double y)
{
  return DoubleMatrix(x, 0.0, 0.0, y, 0.0, 0.0);
}

DoubleMatrix DoubleMatrix::fromTranslation(double x, double y)
{
  return DoubleMatrix(1.0, 0.0, 0.0, 1.0, x, y);
}

DoubleMatrix DoubleMatrix::fromSkewing(double x, double y)
{
  return DoubleMatrix(1.0, tan(y), tan(x), 1.0, 0.0, 0.0);
}

DoubleMatrix DoubleMatrix::fromLineSegment(double x1, double y1, double x2, double y2, double dist)
{
  DoubleMatrix m;

  double dx = x2 - x1;
  double dy = y2 - y1;

  if (dist > 0.0)
  {
    double s = Math::sqrt(dx * dx + dy * dy) / dist;
    m.scale(s, s, MATRIX_APPEND);
  }
  m.rotate(atan2(dy, dx), MATRIX_APPEND);
  m.translate(x1, y1, MATRIX_APPEND);

  return m;
}

DoubleMatrix DoubleMatrix::fromReflectionUnit(double ux, double uy)
{
  double ux2 = ux * 2.0;
  double uy2 = uy * 2.0;

  return DoubleMatrix(ux2 * ux - 1.0, ux2 * uy, ux2 * uy, uy2 * uy - 1.0, 0.0, 0.0);
}

DoubleMatrix DoubleMatrix::fromReflection(double a)
{
  double aSin;
  double aCos;

  Math::sincos(a, &aSin, &aCos);
  return fromReflectionUnit(aCos, aSin);
}

DoubleMatrix DoubleMatrix::fromReflection(double x, double y)
{
  double t = Math::sqrt(x * x + y * y);
  return fromReflectionUnit(x / t, y / t);
}

DoubleMatrix& DoubleMatrix::parlToParl(const double* src, const double* dst)
{
  sx  = src[2] - src[0];
  shy = src[3] - src[1];
  shx = src[4] - src[0];
  sy  = src[5] - src[1];
  tx  = src[0];
  ty  = src[1];

  invert();

  multiply(
    DoubleMatrix(
      dst[2] - dst[0],
      dst[3] - dst[1],
      dst[4] - dst[0],
      dst[5] - dst[1],
      dst[0], dst[1]), MATRIX_APPEND);

  return *this;
}

DoubleMatrix& DoubleMatrix::rectToParl(double x1, double y1, double x2, double y2, const double* parl)
{
  double src[6];

  src[0] = x1; src[1] = y1;
  src[2] = x2; src[3] = y1;
  src[4] = x2; src[5] = y2;
  parlToParl(src, parl);

  return *this;
}

DoubleMatrix& DoubleMatrix::parlToRect(const double* parl, double x1, double y1, double x2, double y2)
{
  double dst[6];

  dst[0] = x1; dst[1] = y1;
  dst[2] = x2; dst[3] = y1;
  dst[4] = x2; dst[5] = y2;
  parlToParl(parl, dst);

  return *this;
}

DoubleMatrix& DoubleMatrix::reset()
{
  sx  = sy  = 1.0;
  shy = shx = tx = ty = 0.0;

  return *this;
}

DoubleMatrix& DoubleMatrix::translate(double x, double y, uint32_t order)
{
  if (order == MATRIX_APPEND)
  {
    tx += x;
    ty += y;
  }
  else
  {
    tx += x * sx  + y * shx;
    ty += x * shy + y * sy ;
  }

  return *this;
}

DoubleMatrix& DoubleMatrix::rotate(double a, uint32_t order)
{
  double aSin;
  double aCos;
  Math::sincos(a, &aSin, &aCos);

  if (order == MATRIX_APPEND)
  {
    double t0 = sx  * aCos + shy * -aSin;
    double t2 = shx * aCos + sy  * -aSin;
    double t4 = tx  * aCos + ty  * -aSin;

    shy = sx  * aSin + shy * aCos;
    sy  = shx * aSin + sy  * aCos;
    ty  = tx  * aSin + ty  * aCos;

    sx  = t0;
    shx = t2;
    tx  = t4;
  }
  else
  {
    double t0 = sx  * aCos  + shx * aSin;
    double t1 = shy * aCos  + sy  * aSin;

    shx = sx  * -aSin + shx * aCos;
    sy  = shy * -aSin + sy  * aCos;

    sx  = t0;
    shy = t1;
  }

  return *this;
}

DoubleMatrix& DoubleMatrix::scale(double x, double y, uint32_t order)
{
  sx  *= x;
  sy  *= y;

  shx *= x;
  shy *= y;

  if (order == MATRIX_APPEND)
  {
    tx  *= x;
    ty  *= y;
  }

  return *this;
}

DoubleMatrix& DoubleMatrix::skew(double x, double y, uint32_t order)
{
  double xTan = tan(x);
  double yTan = tan(y);

  if (order == MATRIX_APPEND)
  {
    double t0 = sx  + shy * xTan;
    double t2 = shx + sy  * xTan;
    double t4 = tx  + ty  * xTan;

    shy = sx  * yTan + shy;
    sy  = shx * yTan + sy ;
    ty  = tx  * yTan + ty ;

    sx  = t0;
    shx = t2;
    tx  = t4;
  }
  else
  {
    double t0 = sx  + shx * yTan;
    double t1 = shy + sy  * yTan;

    shx = sx  * xTan + shx;
    sy  = shy * xTan + sy ;

    sx  = t0;
    shy = t1;
  }

  return *this;
}

DoubleMatrix& DoubleMatrix::multiply(const DoubleMatrix& m, uint32_t order)
{
  if (order == MATRIX_APPEND)
  {
    double t0 = sx  * m.sx + shy * m.shx;
    double t2 = shx * m.sx + sy  * m.shx;
    double t4 = tx  * m.sx + ty  * m.shx + m.tx;

    shy = sx  * m.shy + shy * m.sy;
    sy  = shx * m.shy + sy  * m.sy;
    ty  = tx  * m.shy + ty  * m.sy + m.ty;

    sx  = t0;
    shx = t2;
    tx  = t4;
  }
  else
  {
    tx = sx  * m.tx  + shx * m.ty + tx;
    ty = shy * m.tx  + sy  * m.ty + ty;

    double t0 = sx  * m.sx  + shx * m.shy;
    double t1 = shy * m.sx  + sy  * m.shy;

    shx = sx  * m.shx + shx * m.sy;
    sy  = shy * m.shx + sy  * m.sy;

    sx  = t0;
    shy = t1;
  }

  return *this;
}

DoubleMatrix DoubleMatrix::multiplied(const DoubleMatrix& m, uint32_t order) const
{
  DoubleMatrix result(DONT_INITIALIZE);

  const DoubleMatrix* m0 = this;
  const DoubleMatrix* m1 = &m;

  if (order == MATRIX_PREPEND)
  {
    m0 = &m;
    m1 = this;
  }

  result.sx  = m0->sx  * m1->sx  + m0->shy * m1->shx;
  result.shy = m0->sx  * m1->shy + m0->shy * m1->sy;

  result.shx = m0->shx * m1->sx  + m0->sy  * m1->shx;
  result.sy  = m0->shx * m1->shy + m0->sy  * m1->sy;

  result.tx  = m0->tx  * m1->sx  + m0->ty  * m1->shx + m1->tx;
  result.ty  = m0->tx  * m1->shy + m0->ty  * m1->sy  + m1->ty;

  return result;
}

DoubleMatrix& DoubleMatrix::multiplyInv(const DoubleMatrix& m, uint32_t order)
{
  return multiply(m.inverted(), order);
}

DoubleMatrix& DoubleMatrix::invert()
{
  double d, t0, t4;

  d  = getDeterminantReciprocal();

  t0  =  sy  * d;
  sy  =  sx  * d;
  shy = -shy * d;
  shx = -shx * d;

  t4 = -tx * t0  - ty * shx;
  ty = -tx * shy - ty * sy;

  sx = t0;
  tx = t4;

  return *this;
}

DoubleMatrix DoubleMatrix::inverted() const
{
  DoubleMatrix result(DONT_INITIALIZE);
  double d;

  d  = getDeterminantReciprocal();

  result.sx  =  sy  * d;
  result.sy  =  sx  * d;
  result.shy = -shy * d;
  result.shx = -shx * d;

  result.tx = -tx * result.sx  - ty * result.shx;
  result.ty = -tx * result.shy - ty * result.sy;

  return result;
}

DoubleMatrix& DoubleMatrix::flipX()
{
  sx  = -sx;
  shy = -shy;
  tx  = -tx;

  return *this;
}

DoubleMatrix& DoubleMatrix::flipY()
{
  shx = -shx;
  sy  = -sy;
  ty  = -ty;

  return *this;
}

int DoubleMatrix::getType() const
{
  int type = 0;

  if (Math::feq(tx, 0.0, Math::DEFAULT_DOUBLE_EPSILON)) type |= MATRIX_TYPE_TRANSLATE;
  if (Math::feq(ty, 0.0, Math::DEFAULT_DOUBLE_EPSILON)) type |= MATRIX_TYPE_TRANSLATE;

  if (Math::feq(sx, 1.0, Math::DEFAULT_DOUBLE_EPSILON)) type |= MATRIX_TYPE_SCALE;
  if (Math::feq(sy, 1.0, Math::DEFAULT_DOUBLE_EPSILON)) type |= MATRIX_TYPE_SCALE;

  if (Math::feq(shx, 1.0, Math::DEFAULT_DOUBLE_EPSILON)) type |= MATRIX_TYPE_SHEAR;
  if (Math::feq(shy, 1.0, Math::DEFAULT_DOUBLE_EPSILON)) type |= MATRIX_TYPE_SHEAR;

  return type;
}

void DoubleMatrix::transformPoint(double* x, double* y) const
{
  double _x = *x;
  double _y = *y;

  *x = _x * sx  + _y * shx + tx;
  *y = _x * shy + _y * sy  + ty;
}

void DoubleMatrix::transformVector(double* x, double* y) const
{
  double _x = *x;
  double _y = *y;

  *x = _x * sx  + _y * shx;
  *y = _x * shy + _y * sy;
}

void DoubleMatrix::transformPoints(DoublePoint* dst, const DoublePoint* src, sysuint_t count) const
{
  for (sysuint_t i = 0; i < count; i++)
  {
    double x = src[i].x;
    double y = src[i].y;

    dst[i].x = x * sx  + y * shx + tx;
    dst[i].y = x * shy + y * sy  + ty;
  }
}

bool DoubleMatrix::isValid(double epsilon) const
{
  return Math::abs(sx) > epsilon && Math::abs(sy) > epsilon;
}

bool DoubleMatrix::isIdentity(double epsilon) const
{
  return Math::feq(sx,  1.0, epsilon) &&
         Math::feq(shy, 0.0, epsilon) &&
         Math::feq(shx, 0.0, epsilon) &&
         Math::feq(sy,  1.0, epsilon) &&
         Math::feq(tx,  0.0, epsilon) &&
         Math::feq(ty,  0.0, epsilon);
}

bool DoubleMatrix::isEqual(const DoubleMatrix& m, double epsilon) const
{
  return Math::feq(sx,  m.sx,  epsilon) &&
         Math::feq(shy, m.shy, epsilon) &&
         Math::feq(shx, m.shx, epsilon) && 
         Math::feq(sy,  m.sy,  epsilon) &&
         Math::feq(tx,  m.tx,  epsilon) &&
         Math::feq(ty,  m.ty,  epsilon);
}

DoublePoint DoubleMatrix::getTranslation() const
{
  return DoublePoint(tx, ty);
}

double DoubleMatrix::getRotation() const
{
  return atan2(shy, sx);
}

DoublePoint DoubleMatrix::getScaling() const
{
  DoubleMatrix t(this->multiplied(fromRotation(-getRotation())));

  double x = t.sx + t.shx;
  double y = t.shy + t.sy;

  return DoublePoint(x, y);
}

double DoubleMatrix::getAverageScaling() const
{
  double x = 0.707106781 * (sx + shx);
  double y = 0.707106781 * (shy + sy);

  return Math::sqrt(x * x + y * y);
}

DoublePoint DoubleMatrix::getAbsoluteScaling() const
{
  // Used to calculate scaling coefficients in image resampling. 
  // When there is considerable shear this method gives us much
  // better estimation than just sx, sy.
  double x = Math::sqrt(sx  * sx  + shx * shx);
  double y = Math::sqrt(shy * shy + sy  * sy);

  return DoublePoint(x, y);
}

} // Fog namespace
