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
  struct Storage
  {
    RegionData d;
    // There will be 1 rect more, because some compilers gives me warnings
    // about this construct when <N == 1>
    BoxI rects[N];
  } _storage;

  FOG_INLINE RegionTmp() :
    Region(RegionData::adopt((void*)&_storage, N))
  {
  }

  FOG_INLINE RegionTmp(const RegionTmp<N>& other) :
    Region(RegionData::adopt((void*)&_storage, N, &other._d->extents, other._d->rects, other._d->length))
  {
  }

  FOG_INLINE RegionTmp(const Region& other) :
    Region(RegionData::adopt((void*)&_storage, N, &other._d->extents, other._d->rects, other._d->length))
  {
  }

  explicit FOG_INLINE RegionTmp(const BoxI& r) :
    Region(RegionData::adopt((void*)&_storage, N, r))
  {
  }

  explicit FOG_INLINE RegionTmp(const RectI& r) :
    Region(RegionData::adopt((void*)&_storage, N, BoxI(r)))
  {
  }

  FOG_INLINE void reset()
  {
    if ((void*)_d != (void*)&_storage)
    {
      _d->deref();
      _d = RegionData::adopt((void*)&_storage, N);
    }
  }

  // These overloads are needed to succesfull use this template (or implicit conversion
  // will break template and new region will be allocated)
  FOG_INLINE RegionTmp& operator=(const RegionTmp<N>& r)
  {
    return reinterpret_cast<RegionTmp&>(set(r));
  }

  FOG_INLINE RegionTmp& operator=(const Region& r)
  {
    return reinterpret_cast<RegionTmp&>(set(r));
  }

  FOG_INLINE RegionTmp& operator=(const BoxI& r)
  {
    return reinterpret_cast<RegionTmp&>(set(r));
  }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_TOOLS_REGIONTMP_P_H
