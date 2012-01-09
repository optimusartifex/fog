// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_FECOLORLUTARRAY_H
#define _FOG_G2D_IMAGING_FILTERS_FECOLORLUTARRAY_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::FeColorLutArrayData]
// ============================================================================

struct FOG_NO_EXPORT FeColorLutArrayData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE FeColorLutArrayData* addRef() const
  {
    reference.inc();
    return const_cast<FeColorLutArrayData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.fecolorlutarray_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Data.
  uint8_t data[256];
};

// ============================================================================
// [Fog::FeColorLutArray]
// ============================================================================

struct FOG_NO_EXPORT FeColorLutArray
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE FeColorLutArray()
  {
    fog_api.fecolorlutarray_ctor(this);
  }

  FOG_INLINE FeColorLutArray(const FeColorLutArray& other)
  {
    fog_api.fecolorlutarray_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE FeColorLutArray(FeColorLutArray&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE FeColorLutArray(FeColorLutArrayData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~FeColorLutArray()
  {
    fog_api.fecolorlutarray_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  FOG_INLINE err_t _detach() { return fog_api.fecolorlutarray_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t getAt(size_t index) const
  {
    FOG_ASSERT_X(index < 256,
      "Fog::FeColorLutArray::getAt() - Index out of range.");

    return _d->data[index];
  }

  FOG_INLINE err_t setAt(size_t index, uint8_t value)
  {
    FOG_ASSERT_X(index < 256,
      "Fog::FeColorLutArray::setAt() - Index out of range.");

    return fog_api.fecolorlutarray_setAt(this, index, value);
  }

  FOG_INLINE const uint8_t* getData() const
  {
    return _d->data;
  }

  FOG_INLINE uint8_t* getDataX() const
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::FeColorLutArray::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE err_t setFromFunction(FeColorLutArray* self, const FeComponentFunction& func)
  {
    return fog_api.fecolorlutarray_setFromComponentFunction(self, &func);
  }

  FOG_INLINE bool isIdentity() const
  {
    return fog_api.fecolorlutarray_isIdentity(_d->data);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.fecolorlutarray_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const FeColorLutArray& other) const
  {
    return fog_api.fecolorlutarray_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE FeColorLutArray& operator=(const FeColorLutArray& other)
  {
    fog_api.fecolorlutarray_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const FeColorLutArray& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const FeColorLutArray& other) const { return !eq(other); }

  FOG_INLINE uint8_t operator[](size_t index) const
  {
    FOG_ASSERT_X(index < 256,
      "Fog::FeColorLutArray::operator[] - Index out of range.");

    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const FeColorLutArray& identity()
  {
    return *fog_api.fecolorlutarray_oIdentity;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const FeColorLutArray* a, const FeColorLutArray* b)
  {
    return fog_api.fecolorlutarray_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.fecolorlutarray_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE void setIdentity(uint8_t* data)
  {
    fog_api.fecolorlutarray_setIdentity(data);
  }

  static FOG_INLINE bool isIdentity(const uint8_t* data)
  {
    return fog_api.fecolorlutarray_isIdentity(data);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(FeColorLutArrayData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_FECOLORLUTARRAY_H
