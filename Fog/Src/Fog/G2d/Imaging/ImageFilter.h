// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGEFILTER_H
#define _FOG_G2D_IMAGING_IMAGEFILTER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Fog::ImageFilterData]
// ============================================================================

//! @brief Image filter data.
struct FOG_NO_EXPORT ImageFilterData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageFilterData* addRef() const
  {
    reference.inc();
    return const_cast<ImageFilterData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.imagefilter_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFeType() const
  {
    return *reinterpret_cast<const uint32_t*>(
      reinterpret_cast<const uint8_t*>(this) + sizeof(ImageFilterData));
  }

  FOG_INLINE const FeBase* getFeData() const
  {
    return reinterpret_cast<const FeBase*>(
      reinterpret_cast<const uint8_t*>(this) + sizeof(ImageFilterData));
  }

  FOG_INLINE FeBase* getFeData()
  {
    return reinterpret_cast<FeBase*>(
      reinterpret_cast<uint8_t*>(this) + sizeof(ImageFilterData));
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
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64
};

// ============================================================================
// [Fog::ImageFilter]
// ============================================================================

//! @brief Image filter.
struct FOG_NO_EXPORT ImageFilter
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageFilter()
  {
    fog_api.imagefilter_ctor(this);
  }

  FOG_INLINE ImageFilter(const ImageFilter& other)
  {
    fog_api.imagefilter_ctorCopy(this, &other);
  }

#if defined(FOG_CC_HAS_RVALUE)
  FOG_INLINE ImageFilter(ImageFilter&& other) : _d(other._d) { other._d = NULL; }
#endif // FOG_CC_HAS_RVALUE

  explicit FOG_INLINE ImageFilter(const FeBase& fe)
  {
    fog_api.imagefilter_ctorData(this, &fe);
  }

  explicit FOG_INLINE ImageFilter(ImageFilterData* d) :
    _d(d)
  {
  }

  FOG_INLINE ~ImageFilter()
  {
    fog_api.imagefilter_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _d->reference.get(); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getFeType() const
  {
    return _d->getFeType();
  }
  
  FOG_INLINE const FeBase* getFeData() const
  {
    return _d->getFeData();
  }

  FOG_INLINE err_t getData(FeBase& fe) const
  {
    return fog_api.imagefilter_getData(this, &fe);
  }

  FOG_INLINE err_t setData(const FeBase& fe)
  {
    return fog_api.imagefilter_setData(this, &fe);
  }

  FOG_INLINE err_t setFilter(const ImageFilter& other)
  {
    return fog_api.imagefilter_copy(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.imagefilter_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const ImageFilter& other) const
  {
    return fog_api.imagefilter_eq(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageFilter& operator=(const ImageFilter& other)
  {
    fog_api.imagefilter_copy(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const ImageFilter& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const ImageFilter& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics - Equality]
  // --------------------------------------------------------------------------

  static bool FOG_INLINE eq(const ImageFilter* a, const ImageFilter* b)
  {
    return fog_api.imagefilter_eq(a, b);
  }

  static EqFunc FOG_INLINE getEqFunc()
  {
    return (EqFunc)fog_api.imagefilter_eq;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ImageFilterData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGEFILTER_H
