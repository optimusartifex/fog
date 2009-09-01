// [Fog/Graphics Library - C++ API]
//
// [Licence] 
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
#include <Fog/Core/Math.h>
#include <Fog/Core/TypeInfo.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Fog::Matrix]
// ============================================================================

//! @brief Matrix that can be used to do affine transformations.
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
//!      Fog::Matrix m;
//!      m *= Fog::Matrix::fromRotation(30);
//!      m *= Fog::Matrix::fromScale(2.0, 1.5);
//!      m *= Fog::Matrix::fromTranslation(100, 100);
//!
//!      m.transform(&x, &y);
//!
//! What is the good of it? In real life we will set-up the matrix only once
//! and then transform many points, let alone the convenience to set any
//! combination of transformations.
//!
//! So, how to use it? Very easy - literally as it's shown above. Not quite,
//! let us write a correct example:
//!
//!   Fog::Matrix m;
//!   m.rotate(Fog::Math::deg2rad(30.0));
//!   m.scale(2.0, 1.5);
//!   m.translate(100.0, 100.0);
//!   m.transform(&x, &y);
//!
//! The affine matrix is all you need to perform any linear transformation,
//! but all transformations have origin point (0,0). It means that we need to
//! use 2 translations if we want to rotate someting around (100, 100):
//!
//!   m.translate(-100.0, -100.0);  // move to (0,0)
//!   m.rotate(Fog::Math::deg2rad(30.0)); // rotate
//!   m.translate(100.0, 100.0);    // move back to (100,100)
struct FOG_API Matrix
{
  // [Construction / Destruction]

  //! @brief Create identity matrix.
  FOG_INLINE Matrix()
  {
    sx  = 1.0; shy = 0.0;
    shx = 0.0; sy  = 1.0;
    tx  = 0.0; ty  = 0.0;
  }

  //! @brief Create custom matrix.
  FOG_INLINE Matrix(
    double m0, double m1, double m2, 
    double m3, double m4, double m5)
  {
    sx  = m0; shy = m1;
    shx = m2; sy  = m3;
    tx  = m4; ty  = m5;
  }

  //! @brief Create custom matrix from m[6].
  FOG_INLINE explicit Matrix(const double* m)
  {
    sx  = m[0]; shy = m[1];
    shx = m[2]; sy  = m[3];
    tx  = m[4]; ty  = m[5];
  }

  //! @brief Rectangle to a parallelogram..
  FOG_INLINE Matrix(double x1, double y1, double x2, double y2, const double* parl)
  {
    rectToParl(x1, y1, x2, y2, parl);
  }

  //! @brief Parallelogram to a rectangle.
  FOG_INLINE Matrix(const double* parl, double x1, double y1, double x2, double y2)
  {
    parlToRect(parl, x1, y1, x2, y2);
  }

  //! @brief Arbitrary parallelogram transformation.
  FOG_INLINE Matrix(const double* src, const double* dst)
  {
    parlToParl(src, dst);
  }

  //! @brief Create rotation matrix.
  static Matrix fromRotation(double a);

  //! @brief Create scaling matrix.
  static Matrix fromScale(double s);

  //! @brief Create scaling matrix.
  static Matrix fromScale(double x, double y);

  //! @brief Create translation matrix
  static Matrix fromTranslation(double x, double y);

  //! @brief Create skewing (shear) matrix
  static Matrix fromSkew(double x, double y);

  //! @brief Create line segment matrix - rotate, scale and translate, 
  //! associating 0...dist with line segment x1, y1, x2, y2.
  static Matrix fromLineSegment(double x1, double y1, double x2, double y2, double dist);

  //! Reflection matrix. Reflect coordinates across the line through 
  //! the origin containing the unit vector (ux, uy).
  //!
  //! Contributed by John Horigan
  static Matrix fromReflectionUnit(double ux, double uy);

  static Matrix fromReflection(double a);
  static Matrix fromReflection(double x, double y);

  // [Parellelogram transformations]

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
  Matrix& parlToParl(const double* src, const double* dst);
  Matrix& rectToParl(double x1, double y1, double x2, double y2, const double* parl);
  Matrix& parlToRect(const double* parl, double x1, double y1, double x2, double y2);

  // [Operations]

  //! @brief Reset - load an identity matrix
  Matrix& reset();

  // Direct transformations operations
  Matrix& translate(double x, double y);
  Matrix& rotate(double a);
  Matrix& scale(double s);
  Matrix& scale(double x, double y);
  Matrix& skew(double x, double y);

  //! @brief Multiply matrix by another one
  Matrix& multiply(const Matrix& m);

  //! @brief Multiply matrix to inverse of another one
  Matrix& multiplyInv(const Matrix& m);

  //! @brief Multiply "m" to "this" and assign the result to "this"
  Matrix& premultiply(const Matrix& m);

  //! @brief Multiply inverse of "m" to "this" and assign the result to "this"
  Matrix& premultiplyInv(const Matrix& m);

  //! @brief Invert matrix. Do not try to invert degenerate matrices,
  //! there's no check for validity. If you set scale to 0 and 
  //! then try to invert matrix, expect unpredictable result.
  Matrix& invert();

  //! @brief Mirroring around X
  Matrix& flipX();

  //! @brief Mirroring around Y
  Matrix& flipY();

  // [Load / Store]

  //! @brief Store matrix to an array [6] of double
  FOG_INLINE void storeTo(double* m) const
  {
    m[0] = sx; m[1] = shy; m[2] = shx;
    m[3] = sy; m[4] = tx ; m[5] = ty ;
  }

  //! @brief Load matrix from an array [6] of double
  FOG_INLINE Matrix& loadFrom(const double* m)
  {
    sx = m[0]; shy = m[1]; shx = m[2];
    sy = m[3]; tx  = m[4]; ty  = m[5];
    return *this;
  }

  // [Operators]
  
  //! @brief Multiply the matrix by another one
  FOG_INLINE Matrix& operator*=(const Matrix& m)
  { return multiply(m); }

  //! @brief Multiply the matrix by inverse of another one
  FOG_INLINE Matrix& operator/=(const Matrix& m)
  { return multiplyInv(m); }

  //! @brief Multiply the matrix by another one and return
  //! the result in a separete matrix.
  FOG_INLINE Matrix operator*(const Matrix& m)
  { return Matrix(*this).multiply(m); }

  //! @brief Multiply the matrix by inverse of another one 
  //! and return the result in a separete matrix.
  FOG_INLINE Matrix operator/(const Matrix& m)
  { return Matrix(*this).multiplyInv(m); }

  //! @brief Calculate and return the inverse matrix
  FOG_INLINE Matrix operator~() const
  { Matrix ret = *this; return ret.invert(); }

  //! @brief Equal operator with default epsilon
  FOG_INLINE bool operator==(const Matrix& m) const
  { return isEqual(m, Math::defaultEpsilon); }

  //! @brief Not Equal operator with default epsilon
  FOG_INLINE bool operator!=(const Matrix& m) const
  { return !isEqual(m, Math::defaultEpsilon); }

  // [Transformations]

  //! @brief Direct transformation of x and y
  void transform(double* x, double* y) const;

  //! @brief Direct transformation of x and y, 2x2 matrix only, no translation
  void transform2x2(double* x, double* y) const;

  //! @brief Inverse transformation of x and y. It works slower than
  //! the direct transformation. For massive operations it's better to 
  //! invert() the matrix and then use direct transformations. 
  void transformInv(double* x, double* y) const;

  // [Auxiliary]

  //! @brief Calculate the determinant of matrix.
  FOG_INLINE double determinant() const
  { return sx * sy - shy * shx; }

  //! @brief Calculate the reciprocal of the determinant.
  double determinantReciprocal() const;

  //! @brief Get the average scale (by X and Y). 
  //!
  //! Basically used to calculate the approximation_scale when
  //! decomposinting curves into line segments.
  double scale() const;

  //! @brief Check to see if the matrix is not degenerate.
  bool isValid(double epsilon = Math::defaultEpsilon) const;

  //! @brief Check to see if it's an identity matrix.
  bool isIdentity(double epsilon = Math::defaultEpsilon) const;

  //! @brief Check to see if two matrices are equal.
  bool isEqual(const Matrix& m, double epsilon = Math::defaultEpsilon) const;

  //! @brief Determine the major parameters. Use with caution considering 
  //! possible degenerate cases.
  double rotation() const;

  void translation(double* dx, double* dy) const;
  void scaling(double* x, double* y) const;
  void scalingAbs(double* x, double* y) const;

  // [Members]

  union {
    double m[6];
    struct {
      // Never change order of these variables, they are accessed through SSE2
      // fast paths without referencing (directly from m[] pointer).
      double sx, shy, shx, sy, tx, ty;
    };
  };
};

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<T>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Matrix, Fog::PrimitiveType)

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_MATRIX_H