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

// [Guard]
#ifndef _FOG_GRAPHICS_MATRIX_H
#define _FOG_GRAPHICS_MATRIX_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/HashUtil.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/TypeInfo.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct FloatMatrix;
struct FloatPoint;
struct DoubleMatrix;
struct DoublePoint;

// ============================================================================
// [Fog::FloatMatrix]
// ============================================================================

//! @brief Matrix (32-bit float based).
//!
//! Affine transformation are linear transformations in Cartesian coordinates
//! (strictly speaking not only in Cartesian, but for the beginning we will
//! think so). They are rotation, scaling, translation and skewing.
//! After any affine transformation a line segment remains a line segment
//! and it will never become a curve.
//!
//! There will be no math about matrix calculations, since it has been
//! described many times. Ask yourself a very simple question:
//! "why do we need to understand and use some matrix stuff instead of just
//! rotating, scaling and so on". The answers are:
//!
//! 1. Any combination of transformations can be done by only 4 multiplications
//!    and 4 additions in floating point.
//! 2. One matrix transformation is equivalent to the number of consecutive
//!    discrete transformations, i.e. the matrix "accumulates" all transformations
//!    in the order of their settings. Suppose we have 4 transformations:
//!       * rotate by 30 degrees,
//!       * scale X to 2.0,
//!       * scale Y to 1.5,
//!       * move to (100, 100).
//!    The result will depend on the order of these transformations,
//!    and the advantage of matrix is that the sequence of discret calls:
//!    rotate(30), scaleX(2.0), scaleY(1.5), move(100,100)
//!    will have exactly the same result as the following matrix transformations:
//!
//!    Fog::Matrix m;
//!    m.rotate(30);
//!    m.scale(2.0, 1.5);
//!    m.translate(100, 100);
//!
//!    m.transform(&x, &y);
//!
//! What is the good of it? In real life we will set-up the matrix only once
//! and then transform many points, let alone the convenience to set any
//! combination of transformations.
//!
//! The affine matrix is all you need to perform any linear transformation,
//! but all transformations have origin point (0,0). It means that we need to
//! use 2 translations if we want to rotate someting around (100, 100):
//!
//! @code
//! m.translate(-100.0, -100.0);        // Move to [0, 0].
//! m.rotate(Fog::Math::deg2rad(30.0)); // Rotate.
//! m.translate(100.0, 100.0);          // Move back to [100, 100].
//! @endcode
struct FOG_API FloatMatrix
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create identity matrix.
  FOG_INLINE FloatMatrix()
  {
    sx  = 1.0; shy = 0.0;
    shx = 0.0; sy  = 1.0;
    tx  = 0.0; ty  = 0.0;
  }

  FOG_INLINE FloatMatrix(const FloatMatrix& other)
  {
    sx  = other.sx ; shy = other.shy;
    shx = other.shx; sy  = other.sy ;
    tx  = other.tx ; ty  = other.ty ;
  }

  FOG_INLINE FloatMatrix(const _DONT_INITIALIZE &) {}

  //! @brief Create custom matrix.
  FOG_INLINE FloatMatrix(
    float m0, float m1,
    float m2, float m3,
    float m4, float m5)
  {
    sx  = m0; shy = m1;
    shx = m2; sy  = m3;
    tx  = m4; ty  = m5;
  }

  //! @brief Create custom matrix from m[6].
  FOG_INLINE explicit FloatMatrix(const float* m)
  {
    sx  = m[0]; shy = m[1];
    shx = m[2]; sy  = m[3];
    tx  = m[4]; ty  = m[5];
  }

  //! @brief Rectangle to a parallelogram.
  FOG_INLINE FloatMatrix(float x1, float y1, float x2, float y2, const float* parl)
  {
    rectToParl(x1, y1, x2, y2, parl);
  }

  //! @brief Parallelogram to a rectangle.
  FOG_INLINE FloatMatrix(const float* parl, float x1, float y1, float x2, float y2)
  {
    parlToRect(parl, x1, y1, x2, y2);
  }

  //! @brief Arbitrary parallelogram transformation.
  FOG_INLINE FloatMatrix(const float* src, const float* dst)
  {
    parlToParl(src, dst);
  }

  //! @brief Create rotation matrix.
  static FloatMatrix fromRotation(float a);

  //! @brief Create scaling matrix.
  static FloatMatrix fromScaling(float s);

  //! @brief Create scaling matrix.
  static FloatMatrix fromScaling(float x, float y);

  //! @brief Create translation matrix.
  static FloatMatrix fromTranslation(float x, float y);

  //! @brief Create skewing (shear) matrix.
  static FloatMatrix fromSkewing(float x, float y);

  //! @brief Create line segment matrix - rotate, scale and translate,
  //! associating 0...dist with line segment x1, y1, x2, y2.
  static FloatMatrix fromLineSegment(float x1, float y1, float x2, float y2, float dist);

  //! Reflection matrix. Reflect coordinates across the line through
  //! the origin containing the unit vector (ux, uy).
  //!
  //! Contributed by John Horigan
  static FloatMatrix fromReflectionUnit(float ux, float uy);

  static FloatMatrix fromReflection(float a);
  static FloatMatrix fromReflection(float x, float y);

  // --------------------------------------------------------------------------
  // [Parellelogram transformations]
  //
  // Transform a parallelogram to another one. Src and dst are
  // pointers to arrays of three points (float[6], x1,y1,...) that
  // identify three corners of the parallelograms assuming implicit
  // fourth point. The arguments are arrays of float[6] mapped
  // to x1,y1, x2,y2, x3,y3  where the coordinates are:
  //        *-----------------*
  //       /          (x3,y3)/
  //      /                 /
  //     /(x1,y1)   (x2,y2)/
  //    *-----------------*
  // --------------------------------------------------------------------------

  FloatMatrix& parlToParl(const float* src, const float* dst);
  FloatMatrix& rectToParl(float x1, float y1, float x2, float y2, const float* parl);
  FloatMatrix& parlToRect(const float* parl, float x1, float y1, float x2, float y2);

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FloatMatrix& set(const FloatMatrix& other)
  {
    sx  = other.sx ; shy = other.shy;
    shx = other.shx; sy  = other.sy ;
    tx  = other.tx ; ty  = other.ty ;
    return *this;
  }

  FloatMatrix& set(float m0, float m1, float m2, float m3, float m4, float m5)
  {
    sx  = m0; shy = m1;
    shx = m2; sy  = m3;
    tx  = m4; ty  = m5;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Operations]
  // --------------------------------------------------------------------------

  //! @brief Reset matrix (to identity).
  FloatMatrix& reset();

  // Direct transformations operations
  FloatMatrix& translate(float x, float y, uint32_t order = MATRIX_PREPEND);
  FloatMatrix& rotate(float a, uint32_t order = MATRIX_PREPEND);
  FloatMatrix& scale(float x, float y, uint32_t order = MATRIX_PREPEND);
  FloatMatrix& skew(float x, float y, uint32_t order = MATRIX_PREPEND);

  //! @brief Multiply matrix by @a m.
  FloatMatrix& multiply(const FloatMatrix& m, uint32_t order = MATRIX_APPEND);

  //! @brief Return matrix multiplied by @a m.
  FloatMatrix multiplied(const FloatMatrix& m, uint32_t order = MATRIX_APPEND) const;

  //! @brief Multiply matrix by inverted @a m.
  FloatMatrix& multiplyInv(const FloatMatrix& m, uint32_t order = MATRIX_APPEND);

  //! @brief Invert matrix. Do not try to invert degenerate matrices,
  //! there's no check for validity. If you set scale to 0 and
  //! then try to invert matrix, expect unpredictable result.
  FloatMatrix& invert();

  //! @brief Return inverted matrix.
  FloatMatrix inverted() const;

  //! @brief Mirroring around X.
  FloatMatrix& flipX();

  //! @brief Mirroring around Y.
  FloatMatrix& flipY();

  //! @brief Get type of matrix.
  //!
  //! You can use type of matrix to optimize matrix operations.
  int getType() const;

  // --------------------------------------------------------------------------
  // [Load / Save]
  // --------------------------------------------------------------------------

  //! @brief Load matrix from an array of [6] floats.
  FOG_INLINE FloatMatrix& loadFrom(const float* m)
  {
    sx  = m[0]; shy = m[1];
    shx = m[2]; sy  = m[3];
    tx  = m[4]; ty  = m[5];

    return *this;
  }

  //! @brief Save matrix to an array of [6] floats.
  FOG_INLINE void saveTo(float* m) const
  {
    m[0] = sx ; m[1] = shy;
    m[2] = shx; m[3] = sy ;
    m[4] = tx ; m[5] = ty ;
  }

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  //! @brief Direct transformation of x and y.
  void transformPoint(float* x, float* y) const;

  //! @brief Direct transformation of x and y without translation.
  void transformVector(float* x, float* y) const;

  //! @overload
  FOG_INLINE void transformPoint(FloatPoint* p) { return transformPoint(&p->x, &p->y); }
  //! @overload
  FOG_INLINE void transformVector(FloatPoint* v) { return transformVector(&v->x, &v->y); }

  //! @brief Transform an array of points.
  void transformPoints(FloatPoint* dst, const FloatPoint* src, sysuint_t count) const;

  // --------------------------------------------------------------------------
  // [Auxiliary]
  // --------------------------------------------------------------------------

  //! @brief Check to see if the matrix is not degenerate.
  bool isValid(float epsilon = Math::DEFAULT_FLOAT_EPSILON) const;

  //! @brief Check to see if it's an identity matrix.
  bool isIdentity(float epsilon = Math::DEFAULT_FLOAT_EPSILON) const;

  //! @brief Check to see if two matrices are equal.
  bool isEqual(const FloatMatrix& m, float epsilon = Math::DEFAULT_FLOAT_EPSILON) const;

  //! @brief Calculate the determinant of matrix.
  FOG_INLINE float getDeterminant() const { return sx * sy - shy * shx; }

  //! @brief Calculate the reciprocal of the determinant.
  FOG_INLINE float getDeterminantReciprocal() const { return 1.0f / (sx * sy - shy * shx); }

  //! @brief Get translation part of matrix.
  FloatPoint getTranslation() const;

  //! @brief Get rotation. Use with caution considering possible degenerate cases.
  float getRotation() const;

  //! @brief Get scaling.
  FloatPoint getScaling() const;

  //! @brief Get the average scale (by X and Y).
  //!
  //! Basically used to calculate the approximation scale when decomposinting
  //! curves into line segments.
  float getAverageScaling() const;

  FloatPoint getAbsoluteScaling() const;

  // --------------------------------------------------------------------------
  // [Covert]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleMatrix toDoubleMatrix() const;

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::combineHash(
      HashUtil::getHashCode(sx),
      HashUtil::getHashCode(shy),
      HashUtil::getHashCode(shx),
      HashUtil::getHashCode(sy),
      HashUtil::getHashCode(tx),
      HashUtil::getHashCode(ty));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatMatrix& operator=(const FloatMatrix& other) { return set(other); }

  //! @brief Multiply the matrix by another one.
  FOG_INLINE FloatMatrix& operator*=(const FloatMatrix& m) { return multiply(m, MATRIX_APPEND); }

  //! @brief Multiply the matrix by inverse of another one.
  FOG_INLINE FloatMatrix& operator/=(const FloatMatrix& m) { return multiplyInv(m, MATRIX_APPEND); }

  //! @brief Multiply the matrix by another one and return
  //! the result in a separete matrix.
  FOG_INLINE FloatMatrix operator*(const FloatMatrix& m) { return FloatMatrix(*this).multiply(m, MATRIX_APPEND); }

  //! @brief Multiply the matrix by inverse of another one
  //! and return the result in a separete matrix.
  FOG_INLINE FloatMatrix operator/(const FloatMatrix& m) { return FloatMatrix(*this).multiplyInv(m, MATRIX_APPEND); }

  //! @brief Calculate and return the inverse matrix.
  FOG_INLINE FloatMatrix operator~() const { return inverted(); }

  //! @brief Equal operator with default epsilon.
  FOG_INLINE bool operator==(const FloatMatrix& m) const { return isEqual(m, Math::DEFAULT_FLOAT_EPSILON); }

  //! @brief Not Equal operator with default epsilon.
  FOG_INLINE bool operator!=(const FloatMatrix& m) const { return !isEqual(m, Math::DEFAULT_FLOAT_EPSILON); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union {
    float m[6];
    struct {
      // Never change order of these variables, they are accessed through SSE2
      // fast paths without referencing (directly from m[] pointer).
      float sx, shy;
      float shx, sy;
      float tx, ty;
    };
  };
};

// ============================================================================
// [Fog::DoubleMatrix]
// ============================================================================

//! @brief Matrix (64-bit float based).
//!
//! Affine transformation are linear transformations in Cartesian coordinates
//! (strictly speaking not only in Cartesian, but for the beginning we will
//! think so). They are rotation, scaling, translation and skewing.
//! After any affine transformation a line segment remains a line segment
//! and it will never become a curve.
//!
//! There will be no math about matrix calculations, since it has been
//! described many times. Ask yourself a very simple question:
//! "why do we need to understand and use some matrix stuff instead of just
//! rotating, scaling and so on". The answers are:
//!
//! 1. Any combination of transformations can be done by only 4 multiplications
//!    and 4 additions in floating point.
//! 2. One matrix transformation is equivalent to the number of consecutive
//!    discrete transformations, i.e. the matrix "accumulates" all transformations
//!    in the order of their settings. Suppose we have 4 transformations:
//!       * rotate by 30 degrees,
//!       * scale X to 2.0,
//!       * scale Y to 1.5,
//!       * move to (100, 100).
//!    The result will depend on the order of these transformations,
//!    and the advantage of matrix is that the sequence of discret calls:
//!    rotate(30), scaleX(2.0), scaleY(1.5), move(100,100)
//!    will have exactly the same result as the following matrix transformations:
//!
//!    Fog::Matrix m;
//!    m.rotate(30);
//!    m.scale(2.0, 1.5);
//!    m.translate(100, 100);
//!
//!    m.transform(&x, &y);
//!
//! What is the good of it? In real life we will set-up the matrix only once
//! and then transform many points, let alone the convenience to set any
//! combination of transformations.
//!
//! The affine matrix is all you need to perform any linear transformation,
//! but all transformations have origin point (0,0). It means that we need to
//! use 2 translations if we want to rotate someting around (100, 100):
//!
//! @code
//! m.translate(-100.0, -100.0);        // Move to [0, 0].
//! m.rotate(Fog::Math::deg2rad(30.0)); // Rotate.
//! m.translate(100.0, 100.0);          // Move back to [100, 100].
//! @endcode
struct FOG_API DoubleMatrix
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Create identity matrix.
  FOG_INLINE DoubleMatrix()
  {
    sx  = 1.0; shy = 0.0;
    shx = 0.0; sy  = 1.0;
    tx  = 0.0; ty  = 0.0;
  }

  FOG_INLINE DoubleMatrix(const FloatMatrix& other)
  {
    sx  = (double)other.sx ; shy = (double)other.shy;
    shx = (double)other.shx; sy  = (double)other.sy ;
    tx  = (double)other.tx ; ty  = (double)other.ty ;
  }

  FOG_INLINE DoubleMatrix(const DoubleMatrix& other)
  {
    sx  = other.sx ; shy = other.shy;
    shx = other.shx; sy  = other.sy ;
    tx  = other.tx ; ty  = other.ty ;
  }

  FOG_INLINE DoubleMatrix(const _DONT_INITIALIZE &) {}

  //! @brief Create custom matrix.
  FOG_INLINE DoubleMatrix(
    float m0, float m1,
    float m2, float m3,
    float m4, float m5)
  {
    sx  = (double)m0; shy = (double)m1;
    shx = (double)m2; sy  = (double)m3;
    tx  = (double)m4; ty  = (double)m5;
  }

  //! @brief Create custom matrix.
  FOG_INLINE DoubleMatrix(
    double m0, double m1,
    double m2, double m3,
    double m4, double m5)
  {
    sx  = m0; shy = m1;
    shx = m2; sy  = m3;
    tx  = m4; ty  = m5;
  }

  //! @brief Create custom matrix from m[6].
  FOG_INLINE explicit DoubleMatrix(const float* m)
  {
    sx  = (double)m[0]; shy = (double)m[1];
    shx = (double)m[2]; sy  = (double)m[3];
    tx  = (double)m[4]; ty  = (double)m[5];
  }

  //! @brief Create custom matrix from m[6].
  FOG_INLINE explicit DoubleMatrix(const double* m)
  {
    sx  = m[0]; shy = m[1];
    shx = m[2]; sy  = m[3];
    tx  = m[4]; ty  = m[5];
  }

  //! @brief Rectangle to a parallelogram.
  FOG_INLINE DoubleMatrix(double x1, double y1, double x2, double y2, const double* parl)
  {
    rectToParl(x1, y1, x2, y2, parl);
  }

  //! @brief Parallelogram to a rectangle.
  FOG_INLINE DoubleMatrix(const double* parl, double x1, double y1, double x2, double y2)
  {
    parlToRect(parl, x1, y1, x2, y2);
  }

  //! @brief Arbitrary parallelogram transformation.
  FOG_INLINE DoubleMatrix(const double* src, const double* dst)
  {
    parlToParl(src, dst);
  }

  //! @brief Create rotation matrix.
  static DoubleMatrix fromRotation(double a);

  //! @brief Create scaling matrix.
  static DoubleMatrix fromScaling(double s);

  //! @brief Create scaling matrix.
  static DoubleMatrix fromScaling(double x, double y);

  //! @brief Create translation matrix.
  static DoubleMatrix fromTranslation(double x, double y);

  //! @brief Create skewing (shear) matrix.
  static DoubleMatrix fromSkewing(double x, double y);

  //! @brief Create line segment matrix - rotate, scale and translate, 
  //! associating 0...dist with line segment x1, y1, x2, y2.
  static DoubleMatrix fromLineSegment(double x1, double y1, double x2, double y2, double dist);

  //! Reflection matrix. Reflect coordinates across the line through 
  //! the origin containing the unit vector (ux, uy).
  //!
  //! Contributed by John Horigan
  static DoubleMatrix fromReflectionUnit(double ux, double uy);

  static DoubleMatrix fromReflection(double a);
  static DoubleMatrix fromReflection(double x, double y);

  // --------------------------------------------------------------------------
  // [Parellelogram transformations]
  //
  // Transform a parallelogram to another one. Src and dst are 
  // pointers to arrays of three points (double[6], x1,y1,...) that 
  // identify three corners of the parallelograms assuming implicit 
  // fourth point. The arguments are arrays of double[6] mapped 
  // to x1,y1, x2,y2, x3,y3  where the coordinates are:
  //        *-----------------*
  //       /          (x3,y3)/
  //      /                 /
  //     /(x1,y1)   (x2,y2)/
  //    *-----------------*
  // --------------------------------------------------------------------------

  DoubleMatrix& parlToParl(const double* src, const double* dst);
  DoubleMatrix& rectToParl(double x1, double y1, double x2, double y2, const double* parl);
  DoubleMatrix& parlToRect(const double* parl, double x1, double y1, double x2, double y2);

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  DoubleMatrix& set(const FloatMatrix& other)
  {
    sx  = (double)other.sx ; shy = (double)other.shy;
    shx = (double)other.shx; sy  = (double)other.sy ;
    tx  = (double)other.tx ; ty  = (double)other.ty ;
    return *this;
  }

  DoubleMatrix& set(
    float m0, float m1, float m2,
    float m3, float m4, float m5)
  {
    sx  = (double)m0; shy = (double)m1;
    shx = (double)m2; sy  = (double)m3;
    tx  = (double)m4; ty  = (double)m5;
    return *this;
  }

  DoubleMatrix& set(const DoubleMatrix& other)
  {
    sx  = other.sx ; shy = other.shy;
    shx = other.shx; sy  = other.sy ;
    tx  = other.tx ; ty  = other.ty ;
    return *this;
  }

  DoubleMatrix& set(
    double m0, double m1, double m2,
    double m3, double m4, double m5)
  {
    sx  = m0; shy = m1;
    shx = m2; sy  = m3;
    tx  = m4; ty  = m5;
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Operations]
  // --------------------------------------------------------------------------

  //! @brief Reset matrix (to identity).
  DoubleMatrix& reset();

  // Direct transformations operations
  DoubleMatrix& translate(double x, double y, uint32_t order = MATRIX_PREPEND);
  DoubleMatrix& rotate(double a, uint32_t order = MATRIX_PREPEND);
  DoubleMatrix& scale(double x, double y, uint32_t order = MATRIX_PREPEND);
  DoubleMatrix& skew(double x, double y, uint32_t order = MATRIX_PREPEND);

  //! @brief Multiply matrix by @a m.
  DoubleMatrix& multiply(const DoubleMatrix& m, uint32_t order = MATRIX_APPEND);

  //! @brief Return matrix multiplied by @a m.
  DoubleMatrix multiplied(const DoubleMatrix& m, uint32_t order = MATRIX_APPEND) const;

  //! @brief Multiply matrix by inverted @a m.
  DoubleMatrix& multiplyInv(const DoubleMatrix& m, uint32_t order = MATRIX_APPEND);

  //! @brief Invert matrix. Do not try to invert degenerate matrices,
  //! there's no check for validity. If you set scale to 0 and 
  //! then try to invert matrix, expect unpredictable result.
  DoubleMatrix& invert();

  //! @brief Return inverted matrix.
  DoubleMatrix inverted() const;

  //! @brief Mirroring around X.
  DoubleMatrix& flipX();

  //! @brief Mirroring around Y.
  DoubleMatrix& flipY();

  //! @brief Get type of matrix.
  //!
  //! You can use type of matrix to optimize matrix operations.
  int getType() const;

  // --------------------------------------------------------------------------
  // [Load / Save]
  // --------------------------------------------------------------------------

  //! @brief Load matrix from an array of [6] doubles.
  FOG_INLINE DoubleMatrix& loadFrom(const double* m)
  {
    sx  = m[0]; shy = m[1];
    shx = m[2]; sy  = m[3];
    tx  = m[4]; ty  = m[5];

    return *this;
  }

  //! @brief Save matrix to an array of [6] doubles.
  FOG_INLINE void saveTo(double* m) const
  {
    m[0] = sx ; m[1] = shy;
    m[2] = shx; m[3] = sy ;
    m[4] = tx ; m[5] = ty ;
  }

  // --------------------------------------------------------------------------
  // [Transformations]
  // --------------------------------------------------------------------------

  //! @brief Direct transformation of x and y.
  void transformPoint(double* x, double* y) const;

  //! @brief Direct transformation of x and y without translation.
  void transformVector(double* x, double* y) const;

  //! @overload
  FOG_INLINE void transformPoint(DoublePoint* p) { return transformPoint(&p->x, &p->y); }
  //! @overload
  FOG_INLINE void transformVector(DoublePoint* v) { return transformVector(&v->x, &v->y); }

  //! @brief Transform an array of points.
  void transformPoints(DoublePoint* dst, const DoublePoint* src, sysuint_t count) const;

  // --------------------------------------------------------------------------
  // [Auxiliary]
  // --------------------------------------------------------------------------

  //! @brief Check to see if the matrix is not degenerate.
  bool isValid(double epsilon = Math::DEFAULT_DOUBLE_EPSILON) const;

  //! @brief Check to see if it's an identity matrix.
  bool isIdentity(double epsilon = Math::DEFAULT_DOUBLE_EPSILON) const;

  //! @brief Check to see if two matrices are equal.
  bool isEqual(const DoubleMatrix& m, double epsilon = Math::DEFAULT_DOUBLE_EPSILON) const;

  //! @brief Calculate the determinant of matrix.
  FOG_INLINE double getDeterminant() const { return sx * sy - shy * shx; }

  //! @brief Calculate the reciprocal of the determinant.
  FOG_INLINE double getDeterminantReciprocal() const { return 1.0 / (sx * sy - shy * shx); }

  //! @brief Get translation part of matrix.
  DoublePoint getTranslation() const;

  //! @brief Get rotation. Use with caution considering possible degenerate cases.
  double getRotation() const;

  //! @brief Get scaling.
  DoublePoint getScaling() const;

  //! @brief Get the average scale (by X and Y).
  //!
  //! Basically used to calculate the approximation scale when decomposinting
  //! curves into line segments.
  double getAverageScaling() const;

  DoublePoint getAbsoluteScaling() const;

  // --------------------------------------------------------------------------
  // [Covert]
  // --------------------------------------------------------------------------

  FOG_INLINE FloatMatrix toFloatMatrix() const
  { return FloatMatrix((float)sx, (float)shy, (float)shx, (float)sy, (float)tx, (float)ty); }

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getHashCode() const
  {
    return HashUtil::combineHash(
      HashUtil::getHashCode(sx),
      HashUtil::getHashCode(shy),
      HashUtil::getHashCode(shx),
      HashUtil::getHashCode(sy),
      HashUtil::getHashCode(tx),
      HashUtil::getHashCode(ty));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE DoubleMatrix& operator=(const FloatMatrix& other) { return set(other); }

  FOG_INLINE DoubleMatrix& operator=(const DoubleMatrix& other) { return set(other); }

  //! @brief Multiply the matrix by another one.
  FOG_INLINE DoubleMatrix& operator*=(const DoubleMatrix& m) { return multiply(m, MATRIX_APPEND); }

  //! @brief Multiply the matrix by inverse of another one.
  FOG_INLINE DoubleMatrix& operator/=(const DoubleMatrix& m) { return multiplyInv(m, MATRIX_APPEND); }

  //! @brief Multiply the matrix by another one and return
  //! the result in a separete matrix.
  FOG_INLINE DoubleMatrix operator*(const DoubleMatrix& m) { return DoubleMatrix(*this).multiply(m, MATRIX_APPEND); }

  //! @brief Multiply the matrix by inverse of another one 
  //! and return the result in a separete matrix.
  FOG_INLINE DoubleMatrix operator/(const DoubleMatrix& m) { return DoubleMatrix(*this).multiplyInv(m, MATRIX_APPEND); }

  //! @brief Calculate and return the inverse matrix.
  FOG_INLINE DoubleMatrix operator~() const { return inverted(); }

  //! @brief Equal operator with default epsilon.
  FOG_INLINE bool operator==(const DoubleMatrix& m) const { return isEqual(m, Math::DEFAULT_DOUBLE_EPSILON); }

  //! @brief Not Equal operator with default epsilon.
  FOG_INLINE bool operator!=(const DoubleMatrix& m) const { return !isEqual(m, Math::DEFAULT_DOUBLE_EPSILON); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  union {
    double m[6];
    struct {
      // Never change order of these variables, they are accessed through SSE2
      // fast paths without referencing (directly from m[] pointer).
      double sx, shy;
      double shx, sy;
      double tx, ty;
    };
  };
};

// ============================================================================
// [Defined Later]
// ============================================================================

DoubleMatrix FloatMatrix::toDoubleMatrix() const
{
  return DoubleMatrix((double)sx, (double)shy, (double)shx, (double)sy, (double)tx, (double)ty);
}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<T>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::FloatMatrix , Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::DoubleMatrix, Fog::TYPEINFO_PRIMITIVE)

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_MATRIX_H
