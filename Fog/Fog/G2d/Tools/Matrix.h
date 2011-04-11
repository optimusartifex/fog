// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_MATRIX_H
#define _FOG_G2D_TOOLS_MATRIX_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::MatrixDataF]
// ============================================================================

struct FOG_NO_EXPORT MatrixDataF
{
  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixDataF* ref() const
  {
    refCount.inc();
    return const_cast<MatrixDataF*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) Memory::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The reference count.
  mutable Atomic<sysuint_t> refCount;
  //! @brief The matrix size.
  SizeI size;
  //! @brief The matrix elements.
  float data[1];
};

// ============================================================================
// [Fog::MatrixDataD]
// ============================================================================

struct FOG_NO_EXPORT MatrixDataD
{
  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE MatrixDataD* ref() const
  {
    refCount.inc();
    return const_cast<MatrixDataD*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref()) Memory::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The reference count.
  mutable Atomic<sysuint_t> refCount;
  //! @brief The matrix size.
  SizeI size;
  //! @brief The matrix elements.
  double data[1];
};

// ============================================================================
// [Fog::MatrixF]
// ============================================================================

//! @brief Convolution matrix.
struct FOG_API MatrixF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MatrixF();
  MatrixF(const MatrixF& other);
  MatrixF(const SizeI& size, const float* data = NULL);
  explicit FOG_INLINE MatrixF(MatrixDataF* d) : _d(d) {}
  ~MatrixF();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  FOG_INLINE bool isDetached() const { return getRefCount() == 1; }
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }

  err_t _detach();

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
    FOG_ASSERT_X(isDetached(), "Fog::MatrixF::getDataX() - Called on non-detached object.");
    return _d->data;
  }

  FOG_INLINE const float* getRow(sysuint_t index) const
  {
    FOG_ASSERT_X(index < (sysuint_t)(uint)_d->size.h, "Fog::MatrixF::getRow() - Index out of range.");
    return _d->data + (index * (sysuint_t)(uint)_d->size.w);
  }

  FOG_INLINE float* getRowX(sysuint_t index)
  {
    FOG_ASSERT_X(isDetached(), "Fog::MatrixF::getRowX() - Called on non-detached object.");
    FOG_ASSERT_X(index < (sysuint_t)(uint)_d->size.h, "Fog::MatrixF::getRow() - Index out of range.");
    return _d->data + (index * (sysuint_t)(uint)_d->size.w);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  err_t create(const SizeI& size, const float* data = NULL);
  err_t resize(const SizeI& size, float value = 0.0f);

  void reset();

  float getCell(int x, int y) const;
  err_t setCell(int x, int y, float val);
  err_t fill(const RectI& rect, float val);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  MatrixF& operator=(const MatrixF& other);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<MatrixDataF> _dnull;

  static MatrixDataF* _dalloc(const SizeI& size);
  static void _dcopy(
    MatrixDataF* dst, int dstX, int dstY, 
    MatrixDataF* src, int srcX, int srcY, int w, int h);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(MatrixDataF)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::MatrixF, Fog::TYPEINFO_MOVABLE)
//FOG_DECLARE_TYPEINFO(Fog::MatrixD, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_TOOLS_MATRIX_H
