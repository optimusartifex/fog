// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_TOOLS_REGIONTMP_P_H
#define _FOG_G2D_TOOLS_REGIONTMP_P_H

// [Dependencies]
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Tools
//! @{

// ============================================================================
// [Fog::RegionTmp<>]
// ============================================================================

template<size_t N>
struct RegionTmp : public Region
{
  // --------------------------------------------------------------------------
  // [Storage]
  // --------------------------------------------------------------------------

  struct Storage
  {
    RegionData d;
    // There will be 1 rect more, because some compilers gives me warnings
    // about this construct when <N == 1>
    BoxI rects[N];
  } _storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RegionTmp() :
    Region(_api.region.dAdopt((void*)&_storage, N))
  {
  }

  FOG_INLINE RegionTmp(const RegionTmp<N>& other) :
    Region(_api.region.dAdoptRegion((void*)&_storage, N, other._d->data, other._d->length, &other._d->boundingBox))
  {
  }

  FOG_INLINE RegionTmp(const Region& other) :
    Region(_api.region.dAdoptRegion((void*)&_storage, N, other._d->data, other._d->length, &other._d->boundingBox))
  {
  }

  explicit FOG_INLINE RegionTmp(const BoxI& box) :
    Region(_api.region.dAdoptBox((void*)&_storage, N, &box))
  {
  }

  explicit FOG_INLINE RegionTmp(const RectI& rect) :
    Region(_adoptRect((void*)&_storage, rect))
  {
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _d->release();

    if (_storage.d.reference.get() == 0)
      _d = _api.region.dAdopt((void*)&_storage, N);
    else
      _d = _api.region.oEmpty->_d->addRef();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  // These overloads are needed to successful use this template (or implicit conversion
  // will break template and new region will be allocated)
  FOG_INLINE RegionTmp& operator=(const RegionTmp<N>& r)
  {
    return reinterpret_cast<RegionTmp&>(setRegion(r));
  }

  FOG_INLINE RegionTmp& operator=(const Region& r)
  {
    return reinterpret_cast<RegionTmp&>(setRegion(r));
  }

  FOG_INLINE RegionTmp& operator=(const BoxI& r)
  {
    return reinterpret_cast<RegionTmp&>(setBox(r));
  }

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  static FOG_INLINE RegionData* _adoptRect(void* storage, const RectI& rect)
  {
    BoxI box(rect);
    return _api.region.dAdoptBox(storage, N, &box);
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_REGIONTMP_P_H
