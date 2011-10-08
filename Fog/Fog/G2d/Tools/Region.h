// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_REGION_H
#define _FOG_G2D_TOOLS_REGION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::RegionData]
// ============================================================================

//! @brief Region data.
struct FOG_NO_EXPORT RegionData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE RegionData* addRef() const
  {
    reference.inc();
    return const_cast<RegionData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      _api.region_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [GetSizeOf]
  // --------------------------------------------------------------------------

  static FOG_INLINE size_t getSizeOf(size_t capacity)
  {
    return sizeof(RegionData) - sizeof(BoxI) + capacity * sizeof(BoxI);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the second
  // form called - "implicitly shared container". The members must be binary
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
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | padding0_32| Not used by the Var. This member is only   |
  // |              |            | defined for 64-bit compilation to pad      |
  // |              |            | other members!                             |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | capacity   | Capacity of the container (items).         |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | length     | Length of the container (items).           |
  // +==============+============+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Count of allocated rectangles.
  size_t capacity;
  //! @brief Count of used rectangles.
  size_t length;

  //! @brief Bounding box.
  BoxI boundingBox;
  //! @brief List of YX sorted boxes.
  BoxI data[1];
};

// ============================================================================
// [Fog::Region]
// ============================================================================

//! @brief Region defined by set of YX sorted rectangle(s).
struct FOG_NO_EXPORT Region
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Region()
  {
    _api.region_ctor(this);
  }

  FOG_INLINE Region(const Region& other)
  {
    _api.region_ctorRegion(this, &other);
  }

  explicit FOG_INLINE Region(const BoxI& box)
  {
    _api.region_ctorBox(this, &box);
  }

  explicit FOG_INLINE Region(const RectI& rect)
  {
    _api.region_ctorRect(this, &rect);
  }

  explicit FOG_INLINE Region(_Uninitialized) {}
  explicit FOG_INLINE Region(RegionData* d) : _d(d) {}

  FOG_INLINE ~Region()
  {
    _api.region_dtor(this);
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
  FOG_INLINE err_t _detach() { return _api.region_detach(this); }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  //! @brief Get region capacity.
  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  //! @brief Get region length.
  FOG_INLINE size_t getLength() const { return _d->length; }

  //! @brief Reserve @a n rectangles in region and detach it. If retion is
  //! infinite then ERR_RT_INVALID_CONTEXT is returned.
  FOG_INLINE err_t reserve(size_t n)
  {
    return _api.region_reserve(this, n);
  }

  //! @brief Squeeze region (allocating memory exactly needed for this object).
  FOG_INLINE void squeeze()
  {
    return _api.region_squeeze(this);
  }

  //! @brief Prepare @a n rectangles in region and clear it.
  FOG_INLINE err_t prepare(size_t n)
  {
    return _api.region_prepare(this, n);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  //! @brief Get const pointer to the region data.
  FOG_INLINE const BoxI* getData() const
  {
    return _d->data;
  }

  //! @brief Get mutable pointer to the region data.
  FOG_INLINE BoxI* getDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Region::getDataX() - Not detached");
    return _d->data;
  }

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::isStatic().
  FOG_INLINE bool isStatic() const { return (_d->vType & VAR_FLAG_STATIC) != 0; }

  // --------------------------------------------------------------------------
  // [Type]
  // --------------------------------------------------------------------------

  //! @brief Get type of region, see @c REGION_TYPE enum for possible values.
  FOG_INLINE uint32_t getType() const { return _api.region_getType(this); }

  //! @brief Get whether the region is empty (zero length and not infinite).
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }
  //! @brief Get whether the region is one rectangle.
  FOG_INLINE bool isRect() const { return _d->length == 1; }
  //! @brief Get whether the region is complex.
  FOG_INLINE bool isComplex() const { return _d->length > 1; }
  //! @brief Get whether the region is infinite.
  FOG_INLINE bool isInfinite() const { return _d == _api.region_oInfinite->_d; }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  //! @brief Removes all rectagnels from region.
  FOG_INLINE void clear()
  {
    return _api.region_clear(this);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::reset().
  FOG_INLINE void reset()
  {
    return _api.region_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t setRegion(const Region& region)
  {
    return _api.region_setRegion(this, &region);
  }

  //! @brief Create a deep copy of region @a region.
  FOG_INLINE err_t setDeep(const Region& region)
  {
    return _api.region_setDeep(this, &region);
  }

  FOG_INLINE err_t setBox(const BoxI& box)
  {
    return _api.region_setBox(this, &box);
  }

  FOG_INLINE err_t setRect(const RectI& rect)
  {
    return _api.region_setRect(this, &rect);
  }

  FOG_INLINE err_t setBoxList(const BoxI* data, size_t length)
  {
    return _api.region_setBoxList(this, data, length);
  }

  FOG_INLINE err_t setRectList(const RectI* data, size_t length)
  {
    return _api.region_setRectList(this, data, length);
  }

  // --------------------------------------------------------------------------
  // [Combine]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t combine(const Region& region, uint32_t combineOp)
  {
    return _api.region_combineRegionRegion(this, this, &region, combineOp);
  }

  FOG_INLINE err_t combine(const BoxI& box, uint32_t combineOp)
  {
    return _api.region_combineRegionBox(this, this, &box, combineOp);
  }

  FOG_INLINE err_t combine(const RectI& rect, uint32_t combineOp)
  {
    BoxI box(rect);
    return _api.region_combineRegionBox(this, this, &box, combineOp);
  }

  FOG_INLINE err_t union_(const Region& region) { return combine(region, REGION_OP_UNION); }
  FOG_INLINE err_t union_(const BoxI& box) { return combine(box, REGION_OP_UNION); }
  FOG_INLINE err_t union_(const RectI& rect) { return combine(rect, REGION_OP_UNION); }

  FOG_INLINE err_t intersect(const Region& region) { return combine(region, REGION_OP_INTERSECT); }
  FOG_INLINE err_t intersect(const BoxI& box) { return combine(box, REGION_OP_INTERSECT); }
  FOG_INLINE err_t intersect(const RectI& rect) { return combine(rect, REGION_OP_INTERSECT); }

  FOG_INLINE err_t xor_(const Region& region) { return combine(region, REGION_OP_XOR); }
  FOG_INLINE err_t xor_(const BoxI& box) { return combine(box, REGION_OP_XOR); }
  FOG_INLINE err_t xor_(const RectI& rect) { return combine(rect, REGION_OP_XOR); }

  FOG_INLINE err_t subtract(const Region& region) { return combine(region, REGION_OP_SUBTRACT); }
  FOG_INLINE err_t subtract(const BoxI& box) { return combine(box, REGION_OP_SUBTRACT); }
  FOG_INLINE err_t subtract(const RectI& rect) { return combine(rect, REGION_OP_SUBTRACT); }

  FOG_INLINE err_t subtractReverse(const Region& region) { return combine(region, REGION_OP_SUBTRACT_REV); }
  FOG_INLINE err_t subtractReverse(const BoxI& box) { return combine(box, REGION_OP_SUBTRACT_REV); }
  FOG_INLINE err_t subtractReverse(const RectI& rect) { return combine(rect, REGION_OP_SUBTRACT_REV); }

  // --------------------------------------------------------------------------
  // [Translate]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t translate(const PointI& pt)
  {
    return _api.region_translate(this, this, &pt);
  }

  FOG_INLINE err_t translate(int x, int y)
  {
    PointI pt(x, y);
    return _api.region_translate(this, this, &pt);
  }

  // --------------------------------------------------------------------------
  // [Special]
  // --------------------------------------------------------------------------

  //! @brief Translate current region by @a pt and clip it into @a clipBox.
  //!
  //! This is special function designed for windowing systems where the region
  //! used by UI component needs to be translated and clipped to the view-box.
  //! The result of the operation is the same as translating the region by
  //! @a pt and then intersecting it with @a clipBox. The advantage of using
  //! this method is that everything is done withing a single pass so the
  //! performance is increased.
  FOG_INLINE err_t translateAndClip(const PointI& pt, const BoxI& clipBox)
  {
    return _api.region_translateAndClip(this, this, &pt, &clipBox);
  }

  //! @brief Intersect current region with the @a region and clip it into @a clipBox.
  //!
  //! This is special function designed for windowing systems. The result of
  //! this operation is the same as intersecting the current region with the
  //! @a region and then intersecting it again with the @a clipBox. The
  //! advantage of using this method is that both operations are done within
  //! a single pass so the performance is increased.
  FOG_INLINE err_t intersectAndClip(const Region& region, const BoxI& clipBox)
  {
    return _api.region_intersectAndClip(this, this, &region, &clipBox);
  }

  // --------------------------------------------------------------------------
  // [BoundingBox]
  // --------------------------------------------------------------------------

  //! @brief Returns region extents.
  FOG_INLINE const BoxI& getBoundingBox() const
  {
    return _d->boundingBox;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Tests if a given point is in region, see @c REGION_HIT_TEST enum.
  FOG_INLINE uint32_t hitTest(const PointI& pt) const
  {
    return _api.region_hitTestPoint(this, &pt);
  }

  FOG_INLINE uint32_t hitTest(int x, int y) const
  {
    PointI pt(x, y);
    return _api.region_hitTestPoint(this, &pt);
  }

  FOG_INLINE uint32_t hitTest(const BoxI& box) const
  {
    return _api.region_hitTestBox(this, &box);
  }

  FOG_INLINE uint32_t hitTest(const RectI& rect) const
  {
    return _api.region_hitTestRect(this, &rect);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Region& other) const
  {
    return _api.region_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Windows Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  FOG_INLINE err_t toHRGN(HRGN* hrgn) const
  {
    return _api.region_hrgnFromRegion(hrgn, this);
  }

  FOG_INLINE err_t fromHRGN(HRGN hrgn)
  {
    return _api.region_regionFromHRGN(this, hrgn);
  }
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Region& operator=(const Region& other) { _api.region_setRegion(this, &other); return *this; }
  FOG_INLINE Region& operator=(const BoxI& box) { _api.region_setBox(this, &box); return *this; }
  FOG_INLINE Region& operator=(const RectI& rect) { _api.region_setRect(this, &rect); return *this; }

  FOG_INLINE Region& operator+=(const Region& other) { union_   (other); return *this; }
  FOG_INLINE Region& operator|=(const Region& other) { union_   (other); return *this; }
  FOG_INLINE Region& operator&=(const Region& other) { intersect(other); return *this; }
  FOG_INLINE Region& operator^=(const Region& other) { xor_     (other); return *this; }
  FOG_INLINE Region& operator-=(const Region& other) { subtract (other); return *this; }

  FOG_INLINE Region& operator+=(const BoxI& box) { union_   (box); return *this; }
  FOG_INLINE Region& operator|=(const BoxI& box) { union_   (box); return *this; }
  FOG_INLINE Region& operator&=(const BoxI& box) { intersect(box); return *this; }
  FOG_INLINE Region& operator^=(const BoxI& box) { xor_     (box); return *this; }
  FOG_INLINE Region& operator-=(const BoxI& box) { subtract (box); return *this; }

  FOG_INLINE Region& operator+=(const RectI& rect) { union_   (rect); return *this; }
  FOG_INLINE Region& operator|=(const RectI& rect) { union_   (rect); return *this; }
  FOG_INLINE Region& operator&=(const RectI& rect) { intersect(rect); return *this; }
  FOG_INLINE Region& operator^=(const RectI& rect) { xor_     (rect); return *this; }
  FOG_INLINE Region& operator-=(const RectI& rect) { subtract (rect); return *this; }

  FOG_INLINE bool operator==(const Region& other) const { return  _api.region_eq(this, &other); }
  FOG_INLINE bool operator!=(const Region& other) const { return !_api.region_eq(this, &other); }

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  //! @brief Get empty region instance.
  static FOG_INLINE const Region& empty() { return *_api.region_oEmpty; }
  //! @brief Get infinite region instance.
  static FOG_INLINE const Region& infinite() { return *_api.region_oInfinite; }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool eq(const Region* a, const Region* b)
  {
    return _api.region_eq(a, b);
  }

  static FOG_INLINE EqFunc getEqFunc()
  {
    return (EqFunc)_api.region_eq;
  }

  // --------------------------------------------------------------------------
  // [Statics - Combine]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t combine(Region& dst, const Region& a, const Region& b, uint32_t combineOp)
  {
    return _api.region_combineRegionRegion(&dst, &a, &b, combineOp);
  }

  static FOG_INLINE err_t combine(Region& dst, const Region& a, const BoxI& b, uint32_t combineOp)
  {
    return _api.region_combineRegionBox(&dst, &a, &b, combineOp);
  }

  static FOG_INLINE err_t combine(Region& dst, const Region& a, const RectI& b, uint32_t combineOp)
  {
    BoxI bBox(b);
    return _api.region_combineRegionBox(&dst, &a, &bBox, combineOp);
  }

  static FOG_INLINE err_t combine(Region& dst, const BoxI& a, const Region& b, uint32_t combineOp)
  {
    return _api.region_combineBoxRegion(&dst, &a, &b, combineOp);
  }

  static FOG_INLINE err_t combine(Region& dst, const RectI& a, const Region& b, uint32_t combineOp)
  {
    BoxI aBox(a);
    return _api.region_combineBoxRegion(&dst, &aBox, &b, combineOp);
  }

  static FOG_INLINE err_t combine(Region& dst, const BoxI& a, const BoxI& b, uint32_t combineOp)
  {
    return _api.region_combineBoxBox(&dst, &a, &b, combineOp);
  }

  static FOG_INLINE err_t combine(Region& dst, const RectI& a, const RectI& b, uint32_t combineOp)
  {
    BoxI aBox(a);
    BoxI bBox(b);
    return _api.region_combineBoxBox(&dst, &aBox, &bBox, combineOp);
  }

  static FOG_INLINE err_t union_(Region& dst, const Region& a, const Region& b) { return combine(dst, a, b, REGION_OP_UNION); }
  static FOG_INLINE err_t union_(Region& dst, const Region& a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_UNION); }
  static FOG_INLINE err_t union_(Region& dst, const Region& a, const RectI&  b) { return combine(dst, a, b, REGION_OP_UNION); }
  static FOG_INLINE err_t union_(Region& dst, const BoxI&   a, const Region& b) { return combine(dst, a, b, REGION_OP_UNION); }
  static FOG_INLINE err_t union_(Region& dst, const RectI&  a, const Region& b) { return combine(dst, a, b, REGION_OP_UNION); }
  static FOG_INLINE err_t union_(Region& dst, const BoxI&   a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_UNION); }
  static FOG_INLINE err_t union_(Region& dst, const RectI&  a, const RectI&  b) { return combine(dst, a, b, REGION_OP_UNION); }

  static FOG_INLINE err_t intersect(Region& dst, const Region& a, const Region& b) { return combine(dst, a, b, REGION_OP_INTERSECT); }
  static FOG_INLINE err_t intersect(Region& dst, const Region& a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_INTERSECT); }
  static FOG_INLINE err_t intersect(Region& dst, const Region& a, const RectI&  b) { return combine(dst, a, b, REGION_OP_INTERSECT); }
  static FOG_INLINE err_t intersect(Region& dst, const BoxI&   a, const Region& b) { return combine(dst, a, b, REGION_OP_INTERSECT); }
  static FOG_INLINE err_t intersect(Region& dst, const RectI&  a, const Region& b) { return combine(dst, a, b, REGION_OP_INTERSECT); }
  static FOG_INLINE err_t intersect(Region& dst, const BoxI&   a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_INTERSECT); }
  static FOG_INLINE err_t intersect(Region& dst, const RectI&  a, const RectI&  b) { return combine(dst, a, b, REGION_OP_INTERSECT); }

  static FOG_INLINE err_t xor_(Region& dst, const Region& a, const Region& b) { return combine(dst, a, b, REGION_OP_XOR); }
  static FOG_INLINE err_t xor_(Region& dst, const Region& a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_XOR); }
  static FOG_INLINE err_t xor_(Region& dst, const Region& a, const RectI&  b) { return combine(dst, a, b, REGION_OP_XOR); }
  static FOG_INLINE err_t xor_(Region& dst, const BoxI&   a, const Region& b) { return combine(dst, a, b, REGION_OP_XOR); }
  static FOG_INLINE err_t xor_(Region& dst, const RectI&  a, const Region& b) { return combine(dst, a, b, REGION_OP_XOR); }
  static FOG_INLINE err_t xor_(Region& dst, const BoxI&   a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_XOR); }
  static FOG_INLINE err_t xor_(Region& dst, const RectI&  a, const RectI&  b) { return combine(dst, a, b, REGION_OP_XOR); }

  static FOG_INLINE err_t subtract(Region& dst, const Region& a, const Region& b) { return combine(dst, a, b, REGION_OP_SUBTRACT); }
  static FOG_INLINE err_t subtract(Region& dst, const Region& a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_SUBTRACT); }
  static FOG_INLINE err_t subtract(Region& dst, const Region& a, const RectI&  b) { return combine(dst, a, b, REGION_OP_SUBTRACT); }
  static FOG_INLINE err_t subtract(Region& dst, const BoxI&   a, const Region& b) { return combine(dst, a, b, REGION_OP_SUBTRACT); }
  static FOG_INLINE err_t subtract(Region& dst, const RectI&  a, const Region& b) { return combine(dst, a, b, REGION_OP_SUBTRACT); }
  static FOG_INLINE err_t subtract(Region& dst, const BoxI&   a, const BoxI&   b) { return combine(dst, a, b, REGION_OP_SUBTRACT); }
  static FOG_INLINE err_t subtract(Region& dst, const RectI&  a, const RectI&  b) { return combine(dst, a, b, REGION_OP_SUBTRACT); }

  // --------------------------------------------------------------------------
  // [Statics - Translate / TranslateAndClip]
  // --------------------------------------------------------------------------

  static FOG_INLINE err_t translate(Region& dst, const Region& src, const PointI& pt)
  {
    return _api.region_translate(&dst, &src, &pt);
  }

  static FOG_INLINE err_t translateAndClip(Region& dst, const Region& src, const PointI& pt, const BoxI& clipBox)
  {
    return _api.region_translateAndClip(&dst, &src, &pt, &clipBox);
  }

  // --------------------------------------------------------------------------
  // [Statics - IntersectAndClip]
  // --------------------------------------------------------------------------

  //! @brief Special method that will intersect two regions and clip them.
  //!
  //! @note Calling this method is faster than doing these operations individually.
  static FOG_INLINE err_t intersectAndClip(Region& dst, const Region& a, const Region& b, const BoxI& clipBox)
  {
    return _api.region_intersectAndClip(&dst, &a, &b, &clipBox);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(RegionData)
};

//! @}

} // Fog namespace

FOG_INLINE const Fog::Region operator+(const Fog::Region& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_SUBTRACT ); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::Region& a, const Fog::BoxI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& a, const Fog::BoxI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& a, const Fog::BoxI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& a, const Fog::BoxI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& a, const Fog::BoxI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_SUBTRACT ); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::BoxI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::BoxI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::BoxI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::BoxI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::BoxI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_SUBTRACT ); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::Region& a, const Fog::RectI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& a, const Fog::RectI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& a, const Fog::RectI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& a, const Fog::RectI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& a, const Fog::RectI& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_SUBTRACT ); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::RectI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::RectI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::RectI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::RectI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::RectI& a, const Fog::Region& b) { Fog::Region r; Fog::Region::combine(r, a, b, Fog::REGION_OP_SUBTRACT ); return r; }

// [Guard]
#endif // _FOG_G2D_TOOLS_REGION_H
