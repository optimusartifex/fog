// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_REGION_H
#define _FOG_G2D_TOOLS_REGION_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::RegionData]
// ============================================================================

//! @brief Region data.
struct FOG_API RegionData
{
  // [Ref / Deref]

  FOG_INLINE RegionData* refAlways() const { refCount.inc(); return const_cast<RegionData*>(this); }
  FOG_INLINE void derefInline() { if (refCount.deref() && (flags & IsDynamic) != 0) Memory::free(this); }

  RegionData* ref() const;
  void deref();

  static RegionData* adopt(void* address, sysuint_t capacity);
  static RegionData* adopt(void* address, sysuint_t capacity, const BoxI& r);
  static RegionData* adopt(void* address, sysuint_t capacity, const BoxI* extents, const BoxI* rects, sysuint_t count);

  static RegionData* create(sysuint_t capacity);
  static RegionData* create(sysuint_t capacity, const BoxI* extents, const BoxI* rects, sysuint_t count);

  static RegionData* copy(const RegionData* other);

  static FOG_INLINE sysuint_t sizeFor(sysuint_t capacity)
  { return sizeof(RegionData) - sizeof(BoxI) + sizeof(BoxI) * capacity; }

  // [Flags]

  //! @brief String data flags.
  enum Flags
  {
    //! @brief String data are created on the heap.
    //!
    //! Object is created by function like @c Fog::Memory::alloc() or by
    //! @c new operator. It this flag is not set, you can't delete object from
    //! the heap and object is probabbly only temporary (short life object).
    IsDynamic = (1U << 0),

    //! @brief String data are shareable.
    //!
    //! Object can be directly referenced by internal method @c ref().
    //! Sharable data are usually created on the heap and together
    //! with this flag is set also @c IsDynamic, but it isn't prerequisite.
    IsSharable = (1U << 1),

    //! @brief String data are strong to weak assignments.
    //!
    //! This flag means:
    //!   "Don't assign other data to me, instead, copy them to me!".
    IsStrong = (1U << 2)
  };

  // [Members]

  //! @brief Reference count.
  mutable Atomic<sysuint_t> refCount;
  //! @brief Region flags.
  uint32_t flags;

  //! @brief Count of preallocated rectangles.
  sysuint_t capacity;
  //! @brief Count of used rectangles.
  sysuint_t length;
  //! @brief Region extents.
  BoxI extents;
  //! @brief Region rectangles, always YX sorted.
  BoxI rects[1];
};

// ============================================================================
// [Fog::Region]
// ============================================================================

//! @brief Region defined by rectangles.
struct FOG_API Region
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Region();
  Region(const Region& other);
  explicit Region(const BoxI& rect);
  explicit Region(const RectI& rect);

  explicit FOG_INLINE Region(RegionData* d) : _d(d) {}
  FOG_INLINE Region(_Uninitialized) {}

  ~Region();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getRefCount().
  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getRefCount() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();
  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @brief Get type of region, see @c REGION_TYPE enum for possible values.
  uint32_t getType() const;

  FOG_INLINE bool isInfinite() const { return _d == _dinfinite.instancep(); }
  FOG_INLINE bool isNone() const { return _d->length == 0 && !isInfinite(); }
  FOG_INLINE bool isSimple() const { return _d->length == 1; }
  FOG_INLINE bool isComplex() const { return _d->length > 1; }

  //! @copydoc Doxygen::Implicit::getFlags().
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }

  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }
  //! @copydoc Doxygen::Implicit::isDynamic().
  FOG_INLINE bool isDynamic() const { return (_d->flags & RegionData::IsDynamic) != 0; }
  //! @copydoc Doxygen::Implicit::isSharable().
  FOG_INLINE bool isSharable() const { return (_d->flags & RegionData::IsSharable) != 0; }
  //! @copydoc Doxygen::Implicit::isStrong().
  FOG_INLINE bool isStrong() const { return (_d->flags & RegionData::IsStrong) != 0; }

  err_t setSharable(bool val);
  err_t setStrong(bool val);

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Get const pointer to the region data.
  FOG_INLINE const BoxI* getData() const  { return _d->rects; }

  //! @brief Get mutable pointer to the region data.
  FOG_INLINE BoxI* getDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Region::getDataX() - Called on non-detached object");
    return _d->rects;
  }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  //! @brief Returns capacity of region in rectangles.
  FOG_INLINE sysuint_t getCapacity() const  { return _d->capacity; }

  //! @brief Returns count of rectangles in region.
  FOG_INLINE sysuint_t getLength() const  { return _d->length; }

  //! @brief Returns @c true if region is empty.
  FOG_INLINE bool isEmpty() const  { return _d->length == 0; }

  //! @brief Returns region extents.
  FOG_INLINE const BoxI& getExtents() const  { return _d->extents; }

  //! @brief Reserve @a n rectangles in region and detach it. If retion is
  //! infinite then ERR_RT_INVALID_CONTEXT is returned.
  err_t reserve(sysuint_t n);
  //! @brief Prepare @a n rectangles in region and clear it.
  err_t prepare(sysuint_t n);
  //! @brief Squeeze region (allocating memory exactly needed for this object).
  void squeeze();

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  //! @brief Tests if a given point is in region, see @c REGION_HIT_TEST enum.
  uint32_t hitTest(const PointI& pt) const;
  uint32_t hitTest(const RectI& r) const;
  uint32_t hitTest(const BoxI& r) const;

  FOG_INLINE uint32_t hitTest(int x, int y) const { return hitTest(PointI(x, y)); }

  // --------------------------------------------------------------------------
  // [Clear / Reset]
  // --------------------------------------------------------------------------

  //! @brief Removes all rectagnels from region.
  void clear();
  //! @copydoc Doxygen::Implicit::reset().
  void reset();

  // --------------------------------------------------------------------------
  // [Operations]
  // --------------------------------------------------------------------------

  err_t set(const Region& r);
  err_t set(const RectI& r);
  err_t set(const BoxI& r);

  //! @brief Creates a deep copy instead of reference.
  //!
  //! This function is used internally to optimize computing where we know
  //! that we need to copy region and do some ops with them (Because if we
  //! create a reference, it will free existing allocated block of memory
  //! and it's expensive to alloc it back.
  //!
  //! @param r Region to copy from;
  //! @return @c ERR_OK on success, error code on failure.
  err_t setDeep(const Region& r);

  err_t set(const RectI* rects, sysuint_t count);
  err_t set(const BoxI* rects, sysuint_t count);

  err_t combine(const Region& r, uint32_t combineOp);
  err_t combine(const RectI& r, uint32_t combineOp);
  err_t combine(const BoxI& r, uint32_t combineOp);

  err_t translate(const PointI& pt);
  err_t shrink(const PointI& pt);
  err_t frame(const PointI& pt);

  FOG_INLINE err_t translate(int x, int y) { return translate(PointI(x, y)); }
  FOG_INLINE err_t shrink(int x, int y) { return shrink(PointI(x, y)); }
  FOG_INLINE err_t frame(int x, int y) { return frame(PointI(x, y)); }

  bool eq(const Region& other) const;

  // --------------------------------------------------------------------------
  // [Windows Specific]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  HRGN toHRGN() const;
  err_t fromHRGN(HRGN hrgn);
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Region& operator=(const Region& r) { set(r); return *this; }
  FOG_INLINE Region& operator=(const RectI& r) { set(r); return *this; }
  FOG_INLINE Region& operator=(const BoxI& r) { set(r); return *this; }

  FOG_INLINE Region& operator+=(const Region& r) { combine(r, REGION_OP_UNION); return *this; }
  FOG_INLINE Region& operator+=(const RectI& r) { combine(r, REGION_OP_UNION); return *this; }
  FOG_INLINE Region& operator+=(const BoxI& r) { combine(r, REGION_OP_UNION); return *this; }

  FOG_INLINE Region& operator|=(const Region& r) { combine(r, REGION_OP_UNION); return *this; }
  FOG_INLINE Region& operator|=(const RectI& r) { combine(r, REGION_OP_UNION); return *this; }
  FOG_INLINE Region& operator|=(const BoxI& r) { combine(r, REGION_OP_UNION); return *this; }

  FOG_INLINE Region& operator&=(const Region& r) { combine(r, REGION_OP_INTERSECT); return *this; }
  FOG_INLINE Region& operator&=(const RectI& r) { combine(r, REGION_OP_INTERSECT); return *this; }
  FOG_INLINE Region& operator&=(const BoxI& r) { combine(r, REGION_OP_INTERSECT); return *this; }

  FOG_INLINE Region& operator^=(const Region& r) { combine(r, REGION_OP_XOR); return *this; }
  FOG_INLINE Region& operator^=(const RectI& r) { combine(r, REGION_OP_XOR); return *this; }
  FOG_INLINE Region& operator^=(const BoxI& r) { combine(r, REGION_OP_XOR); return *this; }

  FOG_INLINE Region& operator-=(const Region& r) { combine(r, REGION_OP_SUBTRACT); return *this; }
  FOG_INLINE Region& operator-=(const RectI& r) { combine(r, REGION_OP_SUBTRACT); return *this; }
  FOG_INLINE Region& operator-=(const BoxI& r) { combine(r, REGION_OP_SUBTRACT); return *this; }

  FOG_INLINE bool operator==(const Region& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Region& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Static<RegionData> _dnull;
  static Static<RegionData> _dinfinite;

  static Region* _oempty;
  static Region* _oinfinite;

  //! @brief Empty region instance.
  static const Region& empty() { return *_oempty; }
  //! @brief Infinite region instance.
  static const Region& infinite() { return *_oinfinite; }

  static err_t combine(Region& dst, const Region& src1, const Region& src2, uint32_t combineOp);
  static err_t combine(Region& dst, const Region& src1, const BoxI& src2, uint32_t combineOp);
  static err_t combine(Region& dst, const BoxI& src1, const Region& src2, uint32_t combineOp);
  static err_t combine(Region& dst, const BoxI& src1, const BoxI& src2, uint32_t combineOp);

  static err_t translate(Region& dst, const Region& src, const PointI& pt);
  static err_t shrink(Region& dst, const Region& src, const PointI& pt);
  static err_t frame(Region& dst, const Region& src, const PointI& pt);

  //! @brief Special method that will intersect two regions and clip them.
  //!
  //! @note Calling this method is faster than doing these operations individually.
  static err_t intersectAndClip(Region& dst, const Region& src1Region, const Region& src2Region, const BoxI& clip);
  //! @overload
  static err_t translateAndClip(Region& dst, const Region& src1Region, const PointI& pt, const BoxI& clip);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(RegionData)
};

//! @}

} // Fog namespace

FOG_INLINE const Fog::Region operator+(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::combine(r, src1, src2, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::combine(r, src1, src2, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::combine(r, src1, src2, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::combine(r, src1, src2, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::combine(r, src1, src2, Fog::REGION_OP_SUBTRACT ); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::Region& src1, const Fog::RectI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& src1, const Fog::RectI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& src1, const Fog::RectI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& src1, const Fog::RectI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& src1, const Fog::RectI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_SUBTRACT ); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::Region& src1, const Fog::BoxI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& src1, const Fog::BoxI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_UNION    ); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& src1, const Fog::BoxI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_INTERSECT); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& src1, const Fog::BoxI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_XOR      ); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& src1, const Fog::BoxI& src2) { Fog::Region r(src1); r.combine(src2, Fog::REGION_OP_SUBTRACT ); return r; }

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Region, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_TOOLS_REGION_H
