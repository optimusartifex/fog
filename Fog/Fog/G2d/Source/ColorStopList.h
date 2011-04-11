// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SOURCE_COLORSTOPLIST_H
#define _FOG_G2D_SOURCE_COLORSTOPLIST_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>

namespace Fog {

//! @addtogroup Fog_G2d_Source
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ColorStopCache;

// ============================================================================
// [Fog::ColorStopListData]
// ============================================================================

struct FOG_NO_EXPORT ColorStopListData
{
  // --------------------------------------------------------------------------
  // [Ref / Deref]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStopListData* ref() const
  {
    refCount.inc();
    return const_cast<ColorStopListData*>(this);
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref())
    {
      if (stopCachePrgb32) stopCachePrgb32->deref();
      Memory::free(this);
    }
  }

  // --------------------------------------------------------------------------
  // [Cache]
  // --------------------------------------------------------------------------

  FOG_INLINE void destroyCache()
  {
    if (stopCachePrgb32) stopCachePrgb32->deref();
    stopCachePrgb32 = NULL;
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE sysuint_t sizeFor(sysuint_t capacity)
  {
    return sizeof(ColorStopListData) - sizeof(ColorStop) + 
      capacity * sizeof(ColorStop);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable Atomic<sysuint_t> refCount;

  //! @brief The color-stop list capacity.
  sysuint_t capacity;
  //! @brief The color-stop list length.
  sysuint_t length;

  //! @brief The color-stop cache for PRGB32 or XRGB32 formats (the most used).
  ColorStopCache* stopCachePrgb32;

  //! @brief The color-stops.
  ColorStop data[1];
};

// ============================================================================
// [Fog::ColorStopList]
// ============================================================================

struct FOG_API ColorStopList
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ColorStopList();
  ColorStopList(const ColorStopList& other);
  ~ColorStopList();

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  FOG_INLINE bool isDetached() const { return getRefCount() == 1; }

  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }
  FOG_INLINE sysuint_t getLength() const { return _d->length; }
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  err_t reserve(sysuint_t n);
  void squeeze();

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  void clear();
  void reset();

  // --------------------------------------------------------------------------
  // [IsOpaque]
  // --------------------------------------------------------------------------

  bool isOpaque() const;
  bool isOpaque_ARGB32() const;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const ColorStop* getList() const
  {
    return _d->data;
  }

  FOG_INLINE const ColorStop& getAt(sysuint_t index) const
  {
    FOG_ASSERT_X(index < _d->length, "Fog::ColorStopList::getAt() - Index out of range");
    return _d->data[index];
  }

  err_t setList(const ColorStopList& other);
  err_t setList(const List<ColorStop>& stops);
  err_t setList(const ColorStop* stops, sysuint_t length);

  // --------------------------------------------------------------------------
  // [Manipulation]
  // --------------------------------------------------------------------------

  err_t add(const ColorStop& stop);

  err_t remove(float offset);
  err_t remove(const ColorStop& stop);

  err_t removeAt(sysuint_t index);
  err_t removeAt(const Range& range);
  err_t removeAt(const RangeF& range);

  sysuint_t indexOf(float offset) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ColorStopList& operator=(const ColorStopList& other) { setList(other); return *this; }
  FOG_INLINE ColorStopList& operator=(const List<ColorStop>& list) { setList(list); return *this; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<ColorStopListData> _dnull;
  static ColorStopListData* _dalloc(sysuint_t capacity);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(ColorStopListData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ColorStopList, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_SOURCE_COLORSTOPLIST_H
