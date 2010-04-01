// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_REGION_H
#define _FOG_GRAPHICS_REGION_H

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Geometry.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct DoublePath;
struct DoubleMatrix;

// ============================================================================
// [Fog::RegionData]
// ============================================================================

struct FOG_HIDDEN RegionData
{
  // [Ref / Deref]

  FOG_INLINE RegionData* refAlways() const { refCount.inc(); return const_cast<RegionData*>(this); }
  FOG_INLINE void derefInline() { if (refCount.deref() && (flags & IsDynamic) != 0) Memory::free(this); }

  RegionData* ref() const;
  void deref();

  static RegionData* adopt(void* address, sysuint_t capacity);
  static RegionData* adopt(void* address, sysuint_t capacity, const IntBox& r);
  static RegionData* adopt(void* address, sysuint_t capacity, const IntBox* extents, const IntBox* rects, sysuint_t count);
  
  static RegionData* create(sysuint_t capacity);
  static RegionData* create(sysuint_t capacity, const IntBox* extents, const IntBox* rects, sysuint_t count);

  static RegionData* copy(const RegionData* other);

  static FOG_INLINE sysuint_t sizeFor(sysuint_t capacity)
  { return sizeof(RegionData) - sizeof(IntBox) + sizeof(IntBox) * capacity; }

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
  IntBox extents;
  //! @brief Region rectangles, always YX sorted.
  IntBox rects[1];
};

// ============================================================================
// [Fog::Region]
// ============================================================================

struct FOG_API Region
{
  // --------------------------------------------------------------------------
  // [RegionData]
  // --------------------------------------------------------------------------

  static Static<RegionData> sharedNull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Region();
  Region(const Region& other);
  explicit Region(const IntBox& rect);
  explicit Region(const IntRect& rect);
  FOG_INLINE explicit Region(RegionData* d) : _d(d) {}
  ~Region();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return refCount() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return !isDetached() ? _detach() : (err_t)ERR_OK; }
  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();
  //! @copydoc Doxygen::Implicit::free().
  void free();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getFlags().
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }
  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == sharedNull.instancep(); }
  //! @copydoc Doxygen::Implicit::isDynamic().
  FOG_INLINE bool isDynamic() const { return (_d->flags & RegionData::IsDynamic) != 0; }
  //! @copydoc Doxygen::Implicit::isSharable().
  FOG_INLINE bool isSharable() const { return (_d->flags & RegionData::IsSharable) != 0; }
  //! @copydoc Doxygen::Implicit::isStrong().
  FOG_INLINE bool isStrong() const { return (_d->flags & RegionData::IsStrong) != 0; }

  err_t setSharable(bool val);
  err_t setStrong(bool val);

  // --------------------------------------------------------------------------
  // [RegionData]
  // --------------------------------------------------------------------------

  //! @brief Get const pointer to the region data.
  FOG_INLINE const IntBox* getData() const  { return _d->rects; }

  //! @brief Get mutable pointer to the region data.
  FOG_INLINE IntBox* getMData() { return detach() == ERR_OK ? _d->rects : (IntBox*)NULL; }

  //! @brief Get mutable pointer to the region data.
  FOG_INLINE IntBox* getXData()
  {
    FOG_ASSERT_X(isDetached(), "Fog::Region::getXData() - Not detached");
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
  FOG_INLINE const IntBox& extents() const  { return _d->extents; }

  err_t reserve(sysuint_t to);
  err_t prepare(sysuint_t to);
  void squeeze();

  //! @brief Get type of region, see @c REGION_TYPE enum for possible values.
  uint32_t getType() const;

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  //! @brief Tests if a given point is in region, see @c REGION_HITTEST enum.
  int hitTest(const IntPoint& pt) const;
  int hitTest(const IntRect& r) const;
  int hitTest(const IntBox& r) const;

  FOG_INLINE int hitTest(int x, int y) const { return hitTest(IntPoint(x, y)); }

  // --------------------------------------------------------------------------
  // [Clear]
  // --------------------------------------------------------------------------

  //! @brief Removes all rectagnels from region.
  void clear();

  // --------------------------------------------------------------------------
  // [Operations]
  // --------------------------------------------------------------------------

  err_t set(const Region& r);
  err_t set(const IntRect& r);
  err_t set(const IntBox& r);
  
  //! @brief Creates a deep copy instead of reference.
  //!
  //! This function is used internally to optimize computing where we know
  //! that we need to copy region and do some ops with them (Because if we
  //! create a reference, it will free existing allocated block of memory
  //! and it's expensive to alloc it back.
  //!
  //! @param r Region to copy from;
  //! @return Reference to itself.
  err_t setDeep(const Region& r);

  err_t set(const IntRect* rects, sysuint_t count);
  err_t set(const IntBox* rects, sysuint_t count);

  err_t unite(const Region& r);
  err_t unite(const IntRect& r);
  err_t unite(const IntBox& r);

  err_t intersect(const Region& r);
  err_t intersect(const IntRect& r);
  err_t intersect(const IntBox& r);

  err_t eor(const Region& r);
  err_t eor(const IntRect& r);
  err_t eor(const IntBox& r);

  err_t subtract(const Region& r);
  err_t subtract(const IntRect& r);
  err_t subtract(const IntBox& r);

  err_t op(const Region& r, int _op);
  err_t op(const IntRect& r, int _op);
  err_t op(const IntBox& r, int _op);

  err_t translate(const IntPoint& pt);
  err_t shrink(const IntPoint& pt);
  err_t frame(const IntPoint& pt);

  FOG_INLINE err_t translate(int x, int y) { return translate(IntPoint(x, y)); }
  FOG_INLINE err_t shrink(int x, int y) { return shrink(IntPoint(x, y)); }
  FOG_INLINE err_t frame(int x, int y) { return frame(IntPoint(x, y)); }

  err_t fromPath(const DoublePath& path, const DoubleMatrix* matrix = NULL, uint32_t threshold = 0x7F);

  bool eq(const Region& other) const;

  static err_t set(Region& dst, const Region& src);
  static err_t unite(Region& dst, const Region& src1, const Region& src2);
  static err_t intersect(Region& dst, const Region& src1, const Region& src2);
  static err_t eor(Region& dst, const Region& src1, const Region& src2);
  static err_t subtract(Region& dst, const Region& src1, const Region& src2);
  static err_t op(Region& dst, const Region& src1, const Region& src2, int _op);
  static err_t translate(Region& dst, const Region& src, const IntPoint& pt);
  static err_t shrink(Region& dst, const Region& src, const IntPoint& pt);
  static err_t frame(Region& dst, const Region& src, const IntPoint& pt);

  //! @brief Special method that will intersect two regions and clip box.
  //!
  //! @note Calling this method is faster than doing these operations individually.
  static err_t intersectAndClip(Region& dst, const Region& src1Region, const Region& src2Region, const IntBox& clip);

  static err_t translateAndClip(Region& dst, const Region& src1Region, const IntPoint& pt, const IntBox& clip);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Region& operator=(const Region& r) { set(r); return *this; }
  FOG_INLINE Region& operator=(const IntRect& r) { set(r); return *this; }
  FOG_INLINE Region& operator=(const IntBox& r) { set(r); return *this; }

  FOG_INLINE Region& operator+=(const Region& r) { unite(r); return *this; }
  FOG_INLINE Region& operator+=(const IntRect& r) { unite(r); return *this; }
  FOG_INLINE Region& operator+=(const IntBox& r) { unite(r); return *this; }

  FOG_INLINE Region& operator|=(const Region& r) { unite(r); return *this; }
  FOG_INLINE Region& operator|=(const IntRect& r) { unite(r); return *this; }
  FOG_INLINE Region& operator|=(const IntBox& r) { unite(r); return *this; }

  FOG_INLINE Region& operator&=(const Region& r) { intersect(r); return *this; }
  FOG_INLINE Region& operator&=(const IntRect& r) { intersect(r); return *this; }
  FOG_INLINE Region& operator&=(const IntBox& r) { intersect(r); return *this; }

  FOG_INLINE Region& operator^=(const Region& r) { eor(r); return *this; }
  FOG_INLINE Region& operator^=(const IntRect& r) { eor(r); return *this; }
  FOG_INLINE Region& operator^=(const IntBox& r) { eor(r); return *this; }

  FOG_INLINE Region& operator-=(const Region& r) { subtract(r); return *this; }
  FOG_INLINE Region& operator-=(const IntRect& r) { subtract(r); return *this; }
  FOG_INLINE Region& operator-=(const IntBox& r) { subtract(r); return *this; }

  // --------------------------------------------------------------------------
  // [Windows Specific]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  HRGN toHRGN() const;
  err_t fromHRGN(HRGN hrgn);
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(RegionData)
};

// ============================================================================
// [Fog::TemporaryRegion<N>]
// ============================================================================

template<sysuint_t N>
struct TemporaryRegion : public Region
{
  struct Storage
  {
    RegionData d;
    // There will be 1 rect more, because some compilers gives me warnings
    // about this construct when <N == 1>
    IntBox rects[N];
  } _storage;

  FOG_INLINE TemporaryRegion() :
    Region(RegionData::adopt((void*)&_storage, N))
  {
  }

  FOG_INLINE TemporaryRegion(const TemporaryRegion<N>& other) :
    Region(RegionData::adopt((void*)&_storage, N, &other._d->extents, other._d->rects, other._d->length))
  {
  }
  
  FOG_INLINE TemporaryRegion(const Region& other) :
    Region(RegionData::adopt((void*)&_storage, N, &other._d->extents, other._d->rects, other._d->length))
  {
  }

  FOG_INLINE explicit TemporaryRegion(const IntBox& r) :
    Region(RegionData::adopt((void*)&_storage, N, r))
  {
  }

  FOG_INLINE explicit TemporaryRegion(const IntRect& r) :
    Region(RegionData::adopt((void*)&_storage, N, IntBox(r)))
  {
  }

  FOG_INLINE void free()
  {
    if ((void*)_d != (void*)&_storage)
    {
      _d->deref();
      _d = RegionData::adopt((void*)&_storage, N);
    }
  }

  // These overloads are needed to succesfull use this template (or implicit conversion
  // will break template and new region will be allocated)
  FOG_INLINE TemporaryRegion& operator=(const TemporaryRegion<N>& r)
  {
    return reinterpret_cast<TemporaryRegion&>(set(r));
  }

  FOG_INLINE TemporaryRegion& operator=(const Region& r)
  {
    return reinterpret_cast<TemporaryRegion&>(set(r));
  }

  FOG_INLINE TemporaryRegion& operator=(const IntBox& r)
  {
    return reinterpret_cast<TemporaryRegion&>(set(r));
  }
};

} // Fog namespace

FOG_INLINE const Fog::Region operator+(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::unite(r, src1, src2); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::unite(r, src1, src2); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::intersect(r, src1, src2); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::eor(r, src1, src2); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& src1, const Fog::Region& src2) { Fog::Region r; Fog::Region::subtract(r, src1, src2); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::Region& src1, const Fog::IntRect& src2) { Fog::Region r(src1); r.unite(src2); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& src1, const Fog::IntRect& src2) { Fog::Region r(src1); r.unite(src2); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& src1, const Fog::IntRect& src2) { Fog::Region r(src1); r.intersect(src2); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& src1, const Fog::IntRect& src2) { Fog::Region r(src1); r.eor(src2); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& src1, const Fog::IntRect& src2) { Fog::Region r(src1); r.subtract(src2); return r; }

FOG_INLINE const Fog::Region operator+(const Fog::Region& src1, const Fog::IntBox& src2) { Fog::Region r(src1); r.unite(src2); return r; }
FOG_INLINE const Fog::Region operator|(const Fog::Region& src1, const Fog::IntBox& src2) { Fog::Region r(src1); r.unite(src2); return r; }
FOG_INLINE const Fog::Region operator&(const Fog::Region& src1, const Fog::IntBox& src2) { Fog::Region r(src1); r.intersect(src2); return r; }
FOG_INLINE const Fog::Region operator^(const Fog::Region& src1, const Fog::IntBox& src2) { Fog::Region r(src1); r.eor(src2); return r; }
FOG_INLINE const Fog::Region operator-(const Fog::Region& src1, const Fog::IntBox& src2) { Fog::Region r(src1); r.subtract(src2); return r; }

FOG_INLINE bool operator==(const Fog::Region& src1, const Fog::Region& src2) { return  src1.eq(src2); }
FOG_INLINE bool operator!=(const Fog::Region& src1, const Fog::Region& src2) { return !src1.eq(src2); }

//! @}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Region, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_GRAPHICS_REGION_H
