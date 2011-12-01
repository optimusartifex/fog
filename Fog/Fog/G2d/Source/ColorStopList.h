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
      fog_api.colorstoplist_dFree(this);
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
  // This data-object is binary compatible with the VarData header in the first
  // form called - "implicitly shared class". The members must be binary
  // compatible with the header below:
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
    fog_api.colorstoplist_ctor(this);
  }

  FOG_INLINE ColorStopList(const ColorStopList& other)
  {
    fog_api.colorstoplist_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE ColorStopList(ColorStopList&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE ColorStopList(ColorStopListData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~ColorStopList()
  {
    fog_api.colorstoplist_dtor(this);
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
    return fog_api.colorstoplist_reserve(this, n);
  }

  FOG_INLINE void squeeze()
  {
    fog_api.colorstoplist_squeeze(this);
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
    return fog_api.colorstoplist_copy(this, &other);
  }

  FOG_INLINE err_t setList(const List<ColorStop>& stops)
  {
    return fog_api.colorstoplist_setData(this, stops.getData(), stops.getLength());
  }

  FOG_INLINE err_t setList(const ColorStop* stops, size_t length)
  {
    return fog_api.colorstoplist_setData(this, stops, length);
  }

  // --------------------------------------------------------------------------
  // [IsOpaque]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isOpaque() const
  {
    return fog_api.colorstoplist_isOpaque(this);
  }

  FOG_INLINE bool isOpaqueARGB32() const
  {
    return fog_api.colorstoplist_isOpaqueARGB32(this);
  }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void clear()
  {
    fog_api.colorstoplist_clear(this);
  }

  FOG_INLINE void reset()
  {
    fog_api.colorstoplist_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t add(float offset, const Argb32& argb32)
  {
    return fog_api.colorstoplist_addArgb32(this, offset, &argb32);
  }

  FOG_INLINE err_t add(float offset, const Color& color)
  {
    return fog_api.colorstoplist_addColor(this, offset, &color);
  }

  FOG_INLINE err_t add(const ColorStop& stop)
  {
    return fog_api.colorstoplist_addColorStop(this, &stop);
  }

  FOG_INLINE err_t remove(float offset)
  {
    return fog_api.colorstoplist_removeOffset(this, offset);
  }

  FOG_INLINE err_t remove(const ColorStop& stop)
  {
    return fog_api.colorstoplist_removeStop(this, &stop);
  }

  FOG_INLINE err_t removeAt(size_t index)
  {
    return fog_api.colorstoplist_removeAt(this, index);
  }

  FOG_INLINE err_t removeRange(const Range& range)
  {
    return fog_api.colorstoplist_removeRange(this, &range);
  }

  FOG_INLINE err_t removeRange(const IntervalF& interval)
  {
    return fog_api.colorstoplist_removeInterval(this, &interval);
  }

  FOG_INLINE size_t indexOf(float offset) const
  {
    return fog_api.colorstoplist_indexOfOffset(this, offset);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ColorStopList& other) const
  {
    return fog_api.colorstoplist_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStopList& operator=(const ColorStopList& other) { setList(other); return *this; }
  FOG_INLINE ColorStopList& operator=(const List<ColorStop>& list) { setList(list); return *this; }

  FOG_INLINE bool operator==(const ColorStopList& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ColorStopList& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE const ColorStopList& empty()
  {
    return *fog_api.colorstoplist_oEmpty;
  }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const ColorStopList* a, const ColorStopList* b)
  {
    return fog_api.colorstoplist_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)fog_api.colorstoplist_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Data]
  // --------------------------------------------------------------------------

  static FOG_INLINE ColorStopListData* _dCreate(size_t capacity)
  {
    return fog_api.colorstoplist_dCreate(capacity);
  }

  static FOG_INLINE void _dFree(ColorStopListData* d)
  {
    return fog_api.colorstoplist_dFree(d);
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
