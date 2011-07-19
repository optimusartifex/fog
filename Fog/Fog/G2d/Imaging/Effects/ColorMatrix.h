// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_EFFECTS_COLORMATRIX_H
#define _FOG_G2D_IMAGING_EFFECTS_COLORMATRIX_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ColorMatrix]
// ============================================================================

//! @brief Color matrix.
//!
//! The color matrix is a 5x5 matrix that can be used to do a color based
//! transformations. But unlike the affine transformations used by vector
//! graphics input and output to color matrix is an ARGB color entity.
//!
//! The ColorMatrix class was based on AggOO by Dratek Software. The original
//! code was relicensed to the MIT license used by the Fog-Framework. The
//! original author Chad M. Draper agreed with relicensing.
struct FOG_API ColorMatrix
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorMatrix()
  {
    _copyData(getData(), IDENTITY.getData());
  }

  FOG_INLINE ColorMatrix(const float src[25])
  {
    _copyData(getData(), src);
  }

  FOG_INLINE ColorMatrix(const ColorMatrix& other)
  {
    _copyData(getData(), other.getData());
  }

  FOG_INLINE ColorMatrix(
    float m00, float m01, float m02, float m03, float m04,
    float m10, float m11, float m12, float m13, float m14,
    float m20, float m21, float m22, float m23, float m24,
    float m30, float m31, float m32, float m33, float m34,
    float m40, float m41, float m42, float m43, float m44)
  {
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03; m[0][4] = m04;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13; m[1][4] = m14;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23; m[2][4] = m24;
    m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33; m[3][4] = m34;
    m[4][0] = m40; m[4][1] = m41; m[4][2] = m42; m[4][3] = m43; m[4][4] = m44;
  }

  explicit FOG_INLINE ColorMatrix(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  //! @brief Characteristics of color matrix.
  //!
  //! Characteristics is bit mask.
  enum TYPE
  {
    //! @brief Matrix contains RGB shear part.
    //!
    //! RGB shear part is illustrated here:
    //!   [n X X n n]
    //!   [X n X n n]
    //!   [X X n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    TYPE_SHEAR_RGB = 0x01,

    //! @brief Matrix contains alpha shear part.
    //!
    //! Alpha shear part is illustrated here:
    //!   [n n n X n]
    //!   [n n n X n]
    //!   [n n n X n]
    //!   [X X X n n]
    //!   [n n n n n]
    TYPE_SHEAR_ALPHA = 0x02,

    //! @brief Matrix contains ARGB shear part.
    //!
    //! ARGB shear part is illustrated here:
    //!   [n X X X n]
    //!   [X n X X n]
    //!   [X X n X n]
    //!   [X X X n n]
    //!   [n n n n n]
    //!
    //! @note ARGB shear is combination of RGB and Alpha shear parts.
    TYPE_SHEAR_ARGB = 0x03,

    //! @brief Matrix contains RGB lut part.
    //!
    //! RGB lut part is illustrated here:
    //!   [X n n n n]
    //!   [n X n n n]
    //!   [n n X n n]
    //!   [n n n n n]
    //!   [n n n n n]
    TYPE_LUT_RGB = 0x04,

    //! @brief Matrix contains RGB lut part.
    //!
    //! Alpha lut part is illustrated here:
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n X n]
    //!   [n n n n n]
    TYPE_LUT_ALPHA = 0x08,

    //! @brief Matrix contains ARGB lut part.
    //!
    //! ARGB lut part is illustrated here:
    //!   [X n n n n]
    //!   [n X n n n]
    //!   [n n X n n]
    //!   [n n n X n]
    //!   [n n n n n]
    //!
    //! @note ARGB lut is combination of RGB and Alpha lut parts.
    TYPE_LUT_ARGB = 0x0C,

    //! @brief Matrix contains const RGB lut part (all cells are set to 1.0).
    //!
    //! Const RGB lut part is illustrated here:
    //!   [1 n n n n]
    //!   [n 1 n n n]
    //!   [n n 1 n n]
    //!   [n n n n n]
    //!   [n n n n n]
    TYPE_CONST_RGB = 0x10,

    //! @brief Matrix contains const alpha lut part (cell set to 1.0).
    //!
    //! Const alpha lut part is illustrated here:
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n 1 n]
    //!   [n n n n n]
    TYPE_CONST_ALPHA = 0x20,

    //! @brief Matrix contains const ARGB lut part (all cells are set to 1.0).
    //!
    //! Const ARGB lut part is illustrated here:
    //!   [1 n n n n]
    //!   [n 1 n n n]
    //!   [n n 1 n n]
    //!   [n n n 1 n]
    //!   [n n n n n]
    //!
    //! @note ARGB const lut is combination of RGB a Alpha const lut.
    TYPE_CONST_ARGB = 0x30,

    //! @brief Matrix contains RGB translation part
    //!
    //! RGB translation part is illustrated here:
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [X X X n n]
    TYPE_TRANSLATE_RGB  = 0x40,

    //! @brief Matrix contains alpha translation part
    //!
    //! Alpha translation part is illustrated here:
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n X n]
    TYPE_TRANSLATE_ALPHA = 0x80,

    //! @brief Matrix contains ARGB translation part
    //!
    //! ARGB translation part is illustrated here:
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [n n n n n]
    //!   [X X X X n]
    //!
    //! @note ARGB translation is combination of RGB and Alpha translation parts.
    TYPE_TRANSLATE_ARGB = 0xC0
  };

  //! @brief Get the type of the color matrix.
  //!
  //! Type of color matrix is important part of optimization that can be done
  //! in blitters. The main advantage of color matrix class is that many color
  //! operations can be defined by it. But these operations usually not uses
  //! all matrix cells, so the getType() method checks for zero/one values and
  //! returns type which can be optimized by image-filter.
  //!
  //! @see @c Type for type possibilities and its descriptions.
  int getType() const;

  FOG_INLINE float* getData() { return reinterpret_cast<float*>(m); }
  FOG_INLINE const float* getData() const { return reinterpret_cast<const float*>(m); }

  // --------------------------------------------------------------------------
  // [Operations]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorMatrix& set(const float src[25])
  {
    _copyData(getData(), src);
    return *this;
  }

  FOG_INLINE ColorMatrix& set(const ColorMatrix& other)
  {
    _copyData(getData(), other.getData());
    return *this;
  }

  FOG_INLINE ColorMatrix& set(
    float m00, float m01, float m02, float m03, float m04,
    float m10, float m11, float m12, float m13, float m14,
    float m20, float m21, float m22, float m23, float m24,
    float m30, float m31, float m32, float m33, float m34,
    float m40, float m41, float m42, float m43, float m44)
  {
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03; m[0][4] = m04;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13; m[1][4] = m14;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23; m[2][4] = m24;
    m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33; m[3][4] = m34;
    m[4][0] = m40; m[4][1] = m41; m[4][2] = m42; m[4][3] = m43; m[4][4] = m44;
    return *this;
  }

  //! @brief Add other matrix into this matrix.
  ColorMatrix& add(const ColorMatrix& other);
  //! @brief Subtract other matrix from this matrix.
  ColorMatrix& subtract(const ColorMatrix& other);
  //! @brief Multiply this matrix with other matrix.
  ColorMatrix& multiply(const ColorMatrix& other, uint32_t order = MATRIX_ORDER_PREPEND);
  //! @brief Multiply this matrix with scalar.
  ColorMatrix& multiply(float scalar);

  //! @brief Transform a 1x4 vector by the matrix.
  void transformVector(float* v) const;

  //! @brief Transform a @a rgb color.
  void transformRgb(Argb32* rgb) const;

  //! @brief Transform a @a rgba color.
  void transformArgb(Argb32* rgba) const;

  //! @brief Transform alpha value @a a.
  void transformAlpha(uint8_t* a) const;

  //! @brief Scale the color components of the matrix.
  ColorMatrix& scale(float sa, float sr, float sg, float sb, uint32_t order = MATRIX_ORDER_PREPEND);

  FOG_INLINE ColorMatrix& scaleColors(float scalar, uint32_t order = MATRIX_ORDER_PREPEND)
  { return scale(scalar, scalar, scalar, 1.0f, order );}

  FOG_INLINE ColorMatrix& scaleOpacity(float scalar, uint32_t order = MATRIX_ORDER_PREPEND)
  { return scale(1.0f, 1.0f, 1.0f, scalar, order ); }

  //! @brief Translate the color components of the matrix.
  ColorMatrix& translate(float ta, float tr, float tg, float tb, uint32_t order = MATRIX_ORDER_PREPEND);

  FOG_INLINE ColorMatrix& translateColors(float transVal, uint32_t order = MATRIX_ORDER_PREPEND)
  { return translate(0.0, transVal, transVal, transVal, order); }

  FOG_INLINE ColorMatrix& translateOpacity(float transVal, uint32_t order = MATRIX_ORDER_PREPEND)
  { return translate(transVal, 1.0f, 1.0f, 1.0f, order); }

  FOG_INLINE ColorMatrix& rotateRed(float phi, uint32_t order = MATRIX_ORDER_PREPEND)
  { return _rotateColor(phi, 2, 1, order); }

  FOG_INLINE ColorMatrix& rotateGreen(float phi, uint32_t order = MATRIX_ORDER_PREPEND)
  { return _rotateColor(phi, 0, 2, order); }

  FOG_INLINE ColorMatrix& rotateBlue(float phi, uint32_t order = MATRIX_ORDER_PREPEND)
  { return _rotateColor(phi, 1, 0, order); }

  FOG_INLINE ColorMatrix& shearRed(float g, float b, uint32_t order = MATRIX_ORDER_PREPEND)
  { return _shearColor(0, 1, g, 2, b, order); }

  FOG_INLINE ColorMatrix& shearGreen( float r, float b, uint32_t order = MATRIX_ORDER_PREPEND)
  { return _shearColor(1, 0, r, 2, b, order); }

  FOG_INLINE ColorMatrix& shearBlue(float r, float g, uint32_t order = MATRIX_ORDER_PREPEND)
  { return _shearColor(2, 0, r, 1, g, order); }

  //! @brief Set the saturation of the matrix
  //! @remark Saturation of 0.0 yields black & white, 1.0 is neutral.
  ColorMatrix& setSaturation(float sat, uint32_t order = MATRIX_ORDER_PREPEND);

  //! @brief Scale the tint.
  //! @param phi [-PI to PI, 0 = blue]
  //! @param amount [-1.0 to 1.0, 0 = neutral]
  ColorMatrix& setTint(float phi, float amount);

  //! @brief Rotate the hue about the gray axis.
  //! @remark Luminance is fixed, other colors are rotated.
  ColorMatrix& rotateHue(float phi);

  bool eq(const ColorMatrix& other, float epsilon = MATH_EPSILON_D) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE ColorMatrix& operator=(const ColorMatrix& other)
  {
    _copyData(getData(), other.getData());
    return *this;
  }

  //! @brief Equality operator.
  FOG_INLINE bool operator==(const ColorMatrix& other) { return eq(other); }

  //! @brief Inequality operator.
  FOG_INLINE bool operator!=(const ColorMatrix& other) { return !eq(other); }

  FOG_INLINE ColorMatrix& operator+=(const ColorMatrix& other) { return add(other); }
  FOG_INLINE ColorMatrix& operator-=(const ColorMatrix& other) { return subtract(other); }
  FOG_INLINE ColorMatrix& operator*=(const ColorMatrix& other) { return multiply(other); }

  FOG_INLINE ColorMatrix operator+(const ColorMatrix& other) { ColorMatrix t(*this); t.add(other); return t; }
  FOG_INLINE ColorMatrix operator-(const ColorMatrix& other) { ColorMatrix t(*this); t.subtract(other); return t; }
  FOG_INLINE ColorMatrix operator*(const ColorMatrix& other) { ColorMatrix t(*this); t.multiply(other); return t; }

  //! @brief Overload the [] operator for access to a row. This will enable
  //! access to the elements by using [r][c].
  FOG_INLINE float* operator[](size_t index)
  {
    FOG_ASSERT_X(index < 5U, "Fog::ColorMatrix::operator[] - Index out of bounds.");
    return m[index];
  }

  //! @overload.
  FOG_INLINE const float* const operator[](size_t index) const
  {
    FOG_ASSERT_X(index < 5U, "Fog::ColorMatrix::operator[] - Index out of bounds.");
    return m[index];
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Grayscale color matrix is modified from the GDI+ FAQ (submitted
  //! by Gilles Khouzam) to use the NTSC color values. The version in the FAQ
  //! used 0.3, 0.59, and 0.11, so it was close.
  static const ColorMatrix GREYSCALE;

  //! @brief Identity matrix.
  static const ColorMatrix IDENTITY;
  static const ColorMatrix WHITE;
  static const ColorMatrix ZERO;

  static const ColorMatrix PRE_HUE;
  static const ColorMatrix POST_HUE;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float m[5][5];

  // --------------------------------------------------------------------------
  // [Private]
  // --------------------------------------------------------------------------

private:
  //! @brief Rotate the matrix about a color axis.
  //! @note The color of the axis is unchanged, while the others are rotated
  //! in color space.
  ColorMatrix& _rotateColor(float phi, int x, int y, uint32_t order = MATRIX_ORDER_PREPEND);

  //! @brief Shear the matrix in one of the color planes
  ColorMatrix& _shearColor(int x, int y1, float col1, int y2, float col2, uint32_t order = MATRIX_ORDER_PREPEND);

  //! @brief Copy matrix data from @a srt to @a dst.
  static void _copyData(float* dst, const float* src);
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::ColorMatrix, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_G2D_IMAGING_EFFECTS_COLORMATRIX_H
