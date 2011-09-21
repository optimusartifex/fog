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
//! The color matrix is a 5x5 matrix that can be used to do a linear color
//! transformation.
struct FOG_NO_EXPORT ColorMatrix
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorMatrix()
  {
    _api.colormatrix.ctor(this);
  }

  FOG_INLINE ColorMatrix(const float data[25])
  {
    _api.colormatrix.copy(m, data);
  }

  FOG_INLINE ColorMatrix(const ColorMatrix& other)
  {
    _api.colormatrix.copy(m, other.m);
  }

  FOG_INLINE ColorMatrix(
    float m00, float m01, float m02, float m03, float m04,
    float m10, float m11, float m12, float m13, float m14,
    float m20, float m21, float m22, float m23, float m24,
    float m30, float m31, float m32, float m33, float m34,
    float m40, float m41, float m42, float m43, float m44)
  {
    m[ 0] = m00; m[ 1] = m01; m[ 2] = m02; m[ 3] = m03; m[ 4] = m04;
    m[ 5] = m10; m[ 6] = m11; m[ 7] = m12; m[ 8] = m13; m[ 9] = m14;
    m[10] = m20; m[11] = m21; m[12] = m22; m[13] = m23; m[14] = m24;
    m[15] = m30; m[16] = m31; m[17] = m32; m[18] = m33; m[19] = m34;
    m[20] = m40; m[21] = m41; m[22] = m42; m[23] = m43; m[24] = m44;
  }

  explicit FOG_INLINE ColorMatrix(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float* getData()
  {
    return reinterpret_cast<float*>(m);
  }

  FOG_INLINE const float* getData() const
  {
    return reinterpret_cast<const float*>(m);
  }

  //! @brief Get the type of the color matrix.
  //!
  //! Type of color matrix is important part of optimization that can be done
  //! by blitters. The main advantage of color matrix class is that many color
  //! operations can be defined by it. But these operations usually do not use
  //! all matrix cells, so the getType() method checks for common values and
  //! returns type which can be optimized by image-filter.
  //!
  //! @see @c Type for type possibilities and its descriptions.
  FOG_INLINE uint32_t getType() const
  {
    return _api.colormatrix.getType(this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorMatrix& setMatrix(const float data[25])
  {
    _api.colormatrix.copy(m, data);
    return *this;
  }

  FOG_INLINE ColorMatrix& setMatrix(const ColorMatrix& other)
  {
    _api.colormatrix.copy(m, other.m);
    return *this;
  }

  FOG_INLINE ColorMatrix& setMatrix(
    float m00, float m01, float m02, float m03, float m04,
    float m10, float m11, float m12, float m13, float m14,
    float m20, float m21, float m22, float m23, float m24,
    float m30, float m31, float m32, float m33, float m34,
    float m40, float m41, float m42, float m43, float m44)
  {
    m[ 0] = m00; m[ 1] = m01; m[ 2] = m02; m[ 3] = m03; m[ 4] = m04;
    m[ 5] = m10; m[ 6] = m11; m[ 7] = m12; m[ 8] = m13; m[ 9] = m14;
    m[10] = m20; m[11] = m21; m[12] = m22; m[13] = m23; m[14] = m24;
    m[15] = m30; m[16] = m31; m[17] = m32; m[18] = m33; m[19] = m34;
    m[20] = m40; m[21] = m41; m[22] = m42; m[23] = m43; m[24] = m44;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Add]
  // --------------------------------------------------------------------------

  //! @brief Add other matrix into this matrix.
  FOG_INLINE ColorMatrix& add(const ColorMatrix& other)
  {
    _api.colormatrix.addMatrix(this, this, &other);
    return *this;
  }

  FOG_INLINE ColorMatrix& add(float scalar)
  {
    _api.colormatrix.addScalar(this, this, NULL, scalar);
    return *this;
  }

  FOG_INLINE ColorMatrix& add(const RectI& rect, float scalar)
  {
    _api.colormatrix.addScalar(this, this, &rect, scalar);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Subtract]
  // --------------------------------------------------------------------------

  //! @brief Subtract other matrix from this matrix.
  FOG_INLINE ColorMatrix& subtract(const ColorMatrix& other)
  {
    _api.colormatrix.subtractMatrix(this, this, &other);
    return *this;
  }
  
  FOG_INLINE ColorMatrix& subtract(float scalar)
  {
    _api.colormatrix.subtractScalar(this, this, NULL, scalar);
    return *this;
  }

  FOG_INLINE ColorMatrix& subtract(const RectI& rect, float scalar)
  {
    _api.colormatrix.subtractScalar(this, this, &rect, scalar);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Multiply]
  // --------------------------------------------------------------------------

  //! @brief Multiply this matrix with @a other matrix.
  FOG_INLINE ColorMatrix& multiply(const ColorMatrix& other)
  {
    _api.colormatrix.multiplyMatrix(this, &other, this);
    return *this;
  }

  //! @brief Multiply this matrix with @a other matrix.
  FOG_INLINE ColorMatrix& multiply(const ColorMatrix& other, uint32_t order)
  {
    _api.colormatrix.multiplyOther(this, &other, order);
    return *this;
  }

  //! @brief Multiply this matrix with scalar value.
  FOG_INLINE ColorMatrix& multiply(float scalar)
  {
    _api.colormatrix.multiplyScalar(this, this, NULL, scalar);
    return *this;
  }

  //! @overload
  FOG_INLINE ColorMatrix& multiply(const RectI& rect, float scalar)
  {
    _api.colormatrix.multiplyScalar(this, this, &rect, scalar);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  //! @brief Translate the color components of the matrix.
  FOG_INLINE ColorMatrix& translateArgb(float a, float r, float g, float b, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.translateArgb(this, a, r, g, b, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& translateRgb(float c, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.translateArgb(this, 1.0f, c, c, c, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& translateAlpha(float a, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.translateArgb(this, a, 1.0f, 1.0f, 1.0f, order);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Scale]
  // --------------------------------------------------------------------------

  //! @brief Scale the color components of the matrix.
  FOG_INLINE ColorMatrix& scaleArgb(float a, float r, float g, float b, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.scaleArgb(this, a, r, g, b, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& scaleRgb(float c, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.scaleArgb(this, 1.0f, c, c, c, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& scaleAlpha(float a, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.scaleArgb(this, a, 1.0f, 1.0f, 1.0f, order);
    return *this;
  }

  //! @brief Scale the tint.
  //!
  //! @param phi [-PI to PI, 0 = blue]
  //! @param amount [-1.0 to 1.0, 0 = neutral]
  FOG_INLINE ColorMatrix& scaleTint(float phi, float amount)
  {
    _api.colormatrix.scaleTint(this, phi, amount);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Rotate]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorMatrix& rotateRed(float phi, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.rotateColor(this, 2, 1, phi, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& rotateGreen(float phi, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.rotateColor(this, 0, 2, phi, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& rotateBlue(float phi, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.rotateColor(this, 1, 0, phi, order);
    return *this;
  }

  //! @brief Rotate the hue about the gray axis.
  //!
  //! Luminance is fixed, other colors are rotated.
  FOG_INLINE ColorMatrix& rotateHue(float phi)
  {
    _api.colormatrix.rotateHue(this, phi);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Shear]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorMatrix& shearRed(float g, float b, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.shearColor(this, 0, 1, g, 2, b, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& shearGreen( float r, float b, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.shearColor(this, 1, 0, r, 2, b, order);
    return *this;
  }

  FOG_INLINE ColorMatrix& shearBlue(float r, float g, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.shearColor(this, 2, 0, r, 1, g, order);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Saturate]
  // --------------------------------------------------------------------------

  //! @brief Saturate.
  //!
  //! Saturation of 0.0 yields black & white, 1.0 is neutral.
  FOG_INLINE ColorMatrix& saturate(float s, uint32_t order = MATRIX_ORDER_PREPEND)
  {
    _api.colormatrix.saturate(this, s, order);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ColorMatrix& other) const
  {
    return _api.colormatrix.eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  //! @brief Map @a argb color.
  FOG_INLINE void mapArgb(Argb32& argb) const
  {
    return _api.colormatrix.mapArgb32(this, &argb, &argb);
  }

  //! @overload.
  FOG_INLINE void mapArgb(Argb32& dst, const Argb32& src) const
  {
    return _api.colormatrix.mapArgb32(this, &dst, &src);
  }

  //! @brief Map @a argb color.
  FOG_INLINE void mapArgb(Argb64& argb) const
  {
    return _api.colormatrix.mapArgb64(this, &argb, &argb);
  }

  //! @overload.
  FOG_INLINE void mapArgb(Argb64& dst, const Argb64& src) const
  {
    return _api.colormatrix.mapArgb64(this, &dst, &src);
  }

  //! @brief Map @a argb color.
  FOG_INLINE void mapArgb(ArgbF& argb) const
  {
    return _api.colormatrix.mapArgbF(this, &argb, &argb);
  }

  //! @overload.
  FOG_INLINE void mapArgb(ArgbF& dst, const ArgbF& src) const
  {
    return _api.colormatrix.mapArgbF(this, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE ColorMatrix& operator=(const ColorMatrix& other)
  {
    _api.colormatrix.copy(m, other.m);
    return *this;
  }

  FOG_INLINE ColorMatrix& operator+=(const ColorMatrix& other)
  {
    _api.colormatrix.addMatrix(this, this, &other);
    return *this;
  }

  FOG_INLINE ColorMatrix& operator+=(float scalar)
  {
    _api.colormatrix.addScalar(this, this, NULL, scalar);
    return *this;
  }

  FOG_INLINE ColorMatrix& operator-=(const ColorMatrix& other)
  {
    _api.colormatrix.subtractMatrix(this, this, &other);
    return *this;
  }

  FOG_INLINE ColorMatrix& operator-=(float scalar)
  {
    _api.colormatrix.subtractScalar(this, this, NULL, scalar);
    return *this;
  }

  FOG_INLINE ColorMatrix& operator*=(const ColorMatrix& other)
  {
    _api.colormatrix.multiplyMatrix(this, this, &other);
    return *this;
  }

  FOG_INLINE ColorMatrix& operator*=(float scalar)
  {
    _api.colormatrix.multiplyScalar(this, this, NULL, scalar);
    return *this;
  }

  FOG_INLINE ColorMatrix operator+(const ColorMatrix& other) const
  {
    ColorMatrix t(UNINITIALIZED);
    _api.colormatrix.addMatrix(&t, this, &other);
    return t;
  }

  FOG_INLINE ColorMatrix operator-(const ColorMatrix& other) const
  {
    ColorMatrix t(UNINITIALIZED);
    _api.colormatrix.subtractMatrix(&t, this, &other);
    return t;
  }
  
  FOG_INLINE ColorMatrix operator*(const ColorMatrix& other) const
  {
    ColorMatrix t(UNINITIALIZED);
    _api.colormatrix.multiplyMatrix(&t, this, &other);
    return t;
  }

  FOG_INLINE bool operator==(const ColorMatrix& other) { return eq(other); }
  FOG_INLINE bool operator!=(const ColorMatrix& other) { return !eq(other); }

  //! @brief Overload the [] operator for access to a row. This will enable
  //! access to the elements by using [y][x].
  FOG_INLINE float* operator[](size_t y)
  {
    FOG_ASSERT_X(y < 5, "Fog::ColorMatrix::operator[] - Index out of bounds.");
    return &m[y * 5];
  }

  //! @overload.
  FOG_INLINE const float* const operator[](size_t y) const
  {
    FOG_ASSERT_X(y < 5, "Fog::ColorMatrix::operator[] - Index out of bounds.");
    return &m[y * 5];
  }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  //! @brief Get global identity matrix.
  static FOG_INLINE const ColorMatrix& identity() { return *_api.colormatrix.oIdentity; }
  //! @brief Get global zero matrix.
  static FOG_INLINE const ColorMatrix& zero() { return *_api.colormatrix.oZero; }
  //! @brief Get global grayscale matrix.
  static FOG_INLINE const ColorMatrix& greyscale() { return *_api.colormatrix.oGreyscale; }
  //! @brief Get global pre-hue matrix (used by rotate-hue).
  static FOG_INLINE const ColorMatrix& preHue() { return *_api.colormatrix.oPreHue; }
  //! @brief Get global post-hue matrix (used by rotate-hue).
  static FOG_INLINE const ColorMatrix& postHue() { return *_api.colormatrix.oPostHue; }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ColorMatrix* a, const ColorMatrix* b)
  {
    return _api.colormatrix.eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.colormatrix.eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float m[25];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_EFFECTS_COLORMATRIX_H
