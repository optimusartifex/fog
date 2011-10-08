// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_FILTERS_COLORLUTARRAY_H
#define _FOG_G2D_IMAGING_FILTERS_COLORLUTARRAY_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ColorLutArrayData]
// ============================================================================

struct FOG_NO_EXPORT ColorLutArrayData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorLutArrayData* addRef() const
  {
    reference.inc();
    return const_cast<ColorLutArrayData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.colorlutarray_dFree(this);
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
// [Fog::ColorLutArray]
// ============================================================================

struct FOG_NO_EXPORT ColorLutArray
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorLutArray()
  {
    _api.colorlutarray_ctor(this);
  }

  FOG_INLINE ColorLutArray(const ColorLutArray& other)
  {
    _api.colorlutarray_ctorCopy(this, &other);
  }

  explicit FOG_INLINE ColorLutArray(ColorLutArrayData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~ColorLutArray()
  {
    _api.colorlutarray_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return _d->reference.get() == 1; }

  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  FOG_INLINE err_t _detach() { return _api.colorlutarray_detach(this); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint8_t getAt(size_t index) const
  {
    FOG_ASSERT_X(index < 256,
      "Fog::ColorLutArray::getAt() - Index out of range.");

    return _d->data[index];
  }

  FOG_INLINE err_t setAt(size_t index, uint8_t value)
  {
    FOG_ASSERT_X(index < 256,
      "Fog::ColorLutArray::setAt() - Index out of range.");

    return _api.colorlutarray_setAt(this, index, value);
  }

  FOG_INLINE const uint8_t* getData() const
  {
    return _d->data;
  }

  FOG_INLINE uint8_t* getDataX() const
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::ColorLutArray::getDataX() - Not detached.");

    return _d->data;
  }

  FOG_INLINE err_t setFromFunction(ColorLutArray* self, const ComponentTransferFunction& func)
  {
    return _api.colorlutarray_setFromFunction(self, &func);
  }

  FOG_INLINE bool isIdentity() const
  {
    return _api.colorlutarray_isIdentity(_d->data);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _api.colorlutarray_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ColorLutArray& other) const
  {
    return _api.colorlutarray_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Assignment operator.
  FOG_INLINE ColorLutArray& operator=(const ColorLutArray& other)
  {
    _api.colorlutarray_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ColorLutArray& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ColorLutArray& other) const { return !eq(other); }

  FOG_INLINE uint8_t operator[](size_t index) const
  {
    FOG_ASSERT_X(index < 256,
      "Fog::ColorLutArray::operator[] - Index out of range.");

    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const ColorLutArray& identity()
  {
    return *_api.colorlutarray_oIdentity;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ColorLutArray* a, const ColorLutArray* b)
  {
    return _api.colorlutarray_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.colorlutarray_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE void setIdentity(uint8_t* data)
  {
    _api.colorlutarray_setIdentity(data);
  }

  static FOG_INLINE bool isIdentity(const uint8_t* data)
  {
    return _api.colorlutarray_isIdentity(data);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ColorLutArrayData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_FILTERS_COLORLUTARRAY_H
