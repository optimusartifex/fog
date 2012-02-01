// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_MATRIX_H
#define _FOG_G2D_TOOLS_MATRIX_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::MatrixDataF]
// ============================================================================

struct FOG_NO_EXPORT MatrixDataF
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixDataF* addRef() const
  {
    reference.inc();
    return const_cast<MatrixDataF*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.matrixf_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const
  {
    return size.w == 0;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t area)
  {
    return sizeof(MatrixDataF) - sizeof(float) + area * sizeof(float);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Matrix size.
  SizeI size;
  //! @brief Elements.
  float data[1];
};

// ============================================================================
// [Fog::MatrixDataD]
// ============================================================================

struct FOG_NO_EXPORT MatrixDataD
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixDataD* addRef() const
  {
    reference.inc();
    return const_cast<MatrixDataD*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.matrixd_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const
  {
    return size.w == 0;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t area)
  {
    return sizeof(MatrixDataD) - sizeof(double) + area * sizeof(double);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Size.
  SizeI size;
  //! @brief Elements.
  double data[1];
};

// ============================================================================
// [Fog::MatrixF]
// ============================================================================

//! @brief Matrix (float).
struct FOG_NO_EXPORT MatrixF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixF()
  {
    fog_api.matrixf_ctor(this);
  }

  FOG_INLINE MatrixF(const MatrixF& other)
  {
    fog_api.matrixf_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE MatrixF(MatrixF&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE MatrixF(const SizeI& size, const float* data = NULL)
  {
    fog_api.matrixf_ctorCreate(this, &size, data);
  }

  explicit FOG_INLINE MatrixF(MatrixDataF* d)
    : _d(d)
  {
  }

  FOG_INLINE ~MatrixF()
  {
    fog_api.matrixf_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  FOG_INLINE err_t _detach() { return fog_api.matrixf_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const SizeI& getSize() const { return _d->size; }
  FOG_INLINE int getWidth() const { return _d->size.w; }
  FOG_INLINE int getHeight() const { return _d->size.h; }
  FOG_INLINE bool isEmpty() const { return _d->size.w == 0; }

  FOG_INLINE const float* getData() const
  {
    return _d->data;
  }

  FOG_INLINE float* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixF::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE const float* getRow(size_t index) const
  {
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixF::getRow() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  FOG_INLINE float* getRowX(size_t index)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixF::getRowX() - Not detached.");
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixF::getRowX() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setMatrix(const MatrixF& other)
  {
    fog_api.matrixf_copy(this, &other);
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const SizeI& size, const float* data = NULL)
  {
    return fog_api.matrixf_create(this, &size, data);
  }

  FOG_INLINE err_t resize(const SizeI& size, float value = 0.0f)
  {
    return fog_api.matrixf_resize(this, &size, value);
  }

  FOG_INLINE void reset()
  {
    fog_api.matrixf_reset(this);
  }

  FOG_INLINE float getCell(int x, int y) const
  {
    return fog_api.matrixf_getCell(this, x, y);
  }

  FOG_INLINE err_t setCell(int x, int y, float value)
  {
    return fog_api.matrixf_setCell(this, x, y, value);
  }

  FOG_INLINE err_t fill(const RectI& rect, float value)
  {
    return fog_api.matrixf_fill(this, &rect, value);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixF& operator=(const MatrixF& other)
  {
    fog_api.matrixf_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const MatrixF& other) const { return  fog_api.matrixf_eq(this, &other); }
  FOG_INLINE bool operator!=(const MatrixF& other) const { return !fog_api.matrixf_eq(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const MatrixF& getEmptyInstance()
  {
    return *fog_api.matrixf_oEmpty;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const MatrixF* a, const MatrixF* b)
  {
    return fog_api.matrixf_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.matrixf_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Data]
  // --------------------------------------------------------------------------

  static FOG_INLINE MatrixDataF* _dCreate(const SizeI& size, const float* data = NULL)
  {
    return fog_api.matrixf_dCreate(&size, data);
  }

  static FOG_INLINE void _dFree(MatrixDataF* d)
  {
    fog_api.matrixf_dFree(d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(MatrixDataF)
};

// ============================================================================
// [Fog::MatrixD]
// ============================================================================

//! @brief Matrix (double).
struct FOG_NO_EXPORT MatrixD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixD()
  {
    fog_api.matrixd_ctor(this);
  }

  FOG_INLINE MatrixD(const MatrixD& other)
  {
    fog_api.matrixd_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE MatrixD(MatrixD&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  FOG_INLINE MatrixD(const SizeI& size, const double* data = NULL)
  {
    fog_api.matrixd_ctorCreate(this, &size, data);
  }

  explicit FOG_INLINE MatrixD(MatrixDataD* d)
    : _d(d)
  {
  }

  FOG_INLINE ~MatrixD()
  {
    fog_api.matrixd_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  FOG_INLINE err_t _detach() { return fog_api.matrixd_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const SizeI& getSize() const { return _d->size; }
  FOG_INLINE int getWidth() const { return _d->size.w; }
  FOG_INLINE int getHeight() const { return _d->size.h; }
  FOG_INLINE bool isEmpty() const { return _d->size.w == 0; }

  FOG_INLINE const double* getData() const
  {
    return _d->data;
  }

  FOG_INLINE double* getDataX()
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixD::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE const double* getRow(size_t index) const
  {
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixD::getRow() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  FOG_INLINE double* getRowX(size_t index)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::MatrixD::getRowX() - Not detached.");
    FOG_ASSERT_X(index < (size_t)(uint)_d->size.h,
      "Fog::MatrixD::getRowX() - Index out of range.");

    return _d->data + (index * (size_t)(uint)_d->size.w);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setMatrix(const MatrixD& other)
  {
    fog_api.matrixd_copy(this, &other);
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t create(const SizeI& size, const double* data = NULL)
  {
    return fog_api.matrixd_create(this, &size, data);
  }

  FOG_INLINE err_t resize(const SizeI& size, double value = 0.0f)
  {
    return fog_api.matrixd_resize(this, &size, value);
  }

  FOG_INLINE void reset()
  {
    fog_api.matrixd_reset(this);
  }

  FOG_INLINE double getCell(int x, int y) const
  {
    return fog_api.matrixd_getCell(this, x, y);
  }

  FOG_INLINE err_t setCell(int x, int y, double value)
  {
    return fog_api.matrixd_setCell(this, x, y, value);
  }

  FOG_INLINE err_t fill(const RectI& rect, double value)
  {
    return fog_api.matrixd_fill(this, &rect, value);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixD& operator=(const MatrixD& other)
  {
    fog_api.matrixd_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const MatrixD& other) const { return  fog_api.matrixd_eq(this, &other); }
  FOG_INLINE bool operator!=(const MatrixD& other) const { return !fog_api.matrixd_eq(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const MatrixD& getEmptyInstance()
  {
    return *fog_api.matrixd_oEmpty;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const MatrixD* a, const MatrixD* b)
  {
    return fog_api.matrixd_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.matrixd_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Data]
  // --------------------------------------------------------------------------

  static FOG_INLINE MatrixDataD* _dCreate(const SizeI& size, const double* data = NULL)
  {
    return fog_api.matrixd_dCreate(&size, data);
  }

  static FOG_INLINE void _dFree(MatrixDataD* d)
  {
    fog_api.matrixd_dFree(d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(MatrixDataD)
};

// ============================================================================
// [Fog::MatrixT<>]
// ============================================================================

_FOG_NUM_T(Matrix)
_FOG_NUM_T(MatrixData)
_FOG_NUM_F(Matrix)
_FOG_NUM_F(MatrixData)
_FOG_NUM_D(Matrix)
_FOG_NUM_D(MatrixData)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_MATRIX_H
