// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_COLORSTOPLIST_H
#define _FOG_G2D_SOURCE_COLORSTOPLIST_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Interval.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Fog::ColorStopListData]
// ============================================================================

struct FOG_NO_EXPORT ColorStopListData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStopListData* addRef() const
  {
    reference.inc();
    return const_cast<ColorStopListData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.colorstoplist.dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Cache]
  // --------------------------------------------------------------------------

  FOG_INLINE void destroyCache()
  {
    if (stopCachePrgb32)
      stopCachePrgb32->release();

    stopCachePrgb32 = NULL;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t capacity)
  {
    return sizeof(ColorStopListData) - sizeof(ColorStop) +
      capacity * sizeof(ColorStop);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible to the VarData header in the first
  // form called - "implicitly shared class". The members must be binary
  // compatible to the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  //! @brief Padding (0.32).
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief The color-stop list capacity.
  size_t capacity;
  //! @brief The color-stop list length.
  size_t length;

  //! @brief The color-stop cache for PRGB32 or XRGB32 formats (the most used).
  ColorStopCache* stopCachePrgb32;

  //! @brief The color-stops.
  ColorStop data[1];
};

// ============================================================================
// [Fog::ColorStopList]
// ============================================================================

struct FOG_NO_EXPORT ColorStopList
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStopList()
  {
    _api.colorstoplist.ctor(this);
  }

  FOG_INLINE ColorStopList(const ColorStopList& other)
  {
    _api.colorstoplist.ctorCopy(this, &other);
  }

  explicit FOG_INLINE ColorStopList(ColorStopListData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~ColorStopList()
  {
    _api.colorstoplist.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  FOG_INLINE size_t getLength() const { return _d->length; }
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  FOG_INLINE err_t reserve(size_t n)
  {
    return _api.colorstoplist.reserve(this, n);
  }

  FOG_INLINE void squeeze()
  {
    _api.colorstoplist.squeeze(this);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ColorStop* getList() const
  {
    return _d->data;
  }

  FOG_INLINE const ColorStop& getAt(size_t index) const
  {
    FOG_ASSERT_X(index < _d->length,
      "Fog::ColorStopList::getAt() - Index out of range");

    return _d->data[index];
  }

  FOG_INLINE err_t setList(const ColorStopList& other)
  {
    return _api.colorstoplist.copy(this, &other);
  }

  FOG_INLINE err_t setList(const List<ColorStop>& stops)
  {
    return _api.colorstoplist.setData(this, stops.getData(), stops.getLength());
  }

  FOG_INLINE err_t setList(const ColorStop* stops, size_t length)
  {
    return _api.colorstoplist.setData(this, stops, length);
  }

  // --------------------------------------------------------------------------
  // [IsOpaque]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isOpaque() const
  {
    return _api.colorstoplist.isOpaque(this);
  }

  FOG_INLINE bool isOpaque_ARGB32() const
  {
    return _api.colorstoplist.isOpaque_ARGB32(this);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    _api.colorstoplist.clear(this);
  }

  FOG_INLINE void reset()
  {
    _api.colorstoplist.reset(this);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t add(const ColorStop& stop)
  {
    return _api.colorstoplist.addStop(this, &stop);
  }

  FOG_INLINE err_t remove(float offset)
  {
    return _api.colorstoplist.removeOffset(this, offset);
  }

  FOG_INLINE err_t remove(const ColorStop& stop)
  {
    return _api.colorstoplist.removeStop(this, &stop);
  }

  FOG_INLINE err_t removeAt(size_t index)
  {
    return _api.colorstoplist.removeAt(this, index);
  }

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return _api.colorstoplist.removeRange(this, &range);
  }

  FOG_INLINE err_t removeRange(const IntervalF& interval)
  {
    return _api.colorstoplist.removeInterval(this, &interval);
  }

  FOG_INLINE size_t indexOf(float offset) const
  {
    return _api.colorstoplist.indexOfOffset(this, offset);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ColorStopList& other) const
  {
    return _api.colorstoplist.eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStopList& operator=(const ColorStopList& other) { setList(other); return *this; }
  FOG_INLINE ColorStopList& operator=(const List<ColorStop>& list) { setList(list); return *this; }

  FOG_INLINE bool operator==(const ColorStopList& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ColorStopList& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Eq]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ColorStopList* a, const ColorStopList* b)
  {
    return _api.colorstoplist.eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.colorstoplist.eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Data]
  // --------------------------------------------------------------------------

  static FOG_INLINE ColorStopListData* _dCreate(size_t capacity)
  {
    return _api.colorstoplist.dCreate(capacity);
  }

  static FOG_INLINE void _dFree(ColorStopListData* d)
  {
    return _api.colorstoplist.dFree(d);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ColorStopListData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SOURCE_COLORSTOPLIST_H
