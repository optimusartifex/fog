// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATH_TMP_P_H
#define _FOG_G2D_GEOMETRY_PATH_TMP_P_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/G2d/Geometry/Path.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PathTmpF]
// ============================================================================

//! @internal
//!
//! @brief Path - Temporary (float).
//!
//! @note Default size of @c PathTmpF is 108 elements which matches approximately
//! 1kB of memory.
template<sysuint_t N = 108>
struct FOG_NO_EXPORT PathTmpF : public PathF
{
  struct _Storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathTmpF() : PathF(_dadopt(_storage)) {}
  FOG_INLINE PathTmpF(const PathTmpF<N>& other) : PathF(_dadopt(_storage)) { setDeep(other); }
  FOG_INLINE PathTmpF(const PathF& other) : PathF(_dadopt(_storage)) { setDeep(other); }

  FOG_INLINE ~PathTmpF() 
  {
    FOG_ASSERT( (_d == &_storage.d && _storage.d.refCount.get() == 1) ||
                (_d != &_storage.d && _storage.d.refCount.get() == 0) );
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathTmpF<N>& operator=(const PathTmpF<N>& other) { setPath(other); return *this; }
  FOG_INLINE PathTmpF<N>& operator=(const PathF& other) { setPath(other); return *this; }
  FOG_INLINE PathTmpF<N>& operator+=(const PathF& other) { append(other); return *this; }

  FOG_INLINE bool operator==(const PathF& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const PathF& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static PathDataF* _dadopt(_Storage& storage)
  {
    storage.d.refCount.init(1);
    storage.d.flags = PATH_DATA_STATIC;
#if FOG_ARCH_BITS >= 64
    storage.d.padding = 0;
#endif // FOG_ARCH_BITS >= 64
    storage.d.capacity = N;
    storage.d.length = 0;
    storage.d.boundingBox.reset();
    storage.d.vertices = (PointF*)( (sysuint_t)(storage.data + N + 15) & ~(sysuint_t)15 );
    return &storage.d;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct _Storage
  {
    PathDataF d;
    uint8_t data[N * (sizeof(PointF) + sizeof(uint8_t))];
  } _storage;
};

// ============================================================================
// [Fog::PathTmpD]
// ============================================================================

//! @internal
//!
//! @brief Path - temporary (double).
//!
//! @note Default size of @c PathTmpD is 58 elements which matches approximately
//! 1kB of memory.
template<sysuint_t N = 58>
struct FOG_NO_EXPORT PathTmpD : public PathD
{
  struct _Storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathTmpD() : PathD(_dadopt(_storage)) {}
  FOG_INLINE PathTmpD(const PathTmpD<N>& other) : PathF(_dadopt(_storage)) { setDeep(other); }
  FOG_INLINE PathTmpD(const PathD& other) : PathF(_dadopt(_storage)) { setDeep(other); }

  FOG_INLINE ~PathTmpD()
  {
    FOG_ASSERT( (_d == &_storage.d && _storage.d.refCount.get() == 1) ||
                (_d != &_storage.d && _storage.d.refCount.get() == 0) );
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static PathDataD* _dadopt(_Storage& storage)
  {
    storage.d.refCount.init(1);
    storage.d.flags = PATH_DATA_STATIC;
#if FOG_ARCH_BITS >= 64
    storage.d.padding = 0;
#endif // FOG_ARCH_BITS >= 64
    storage.d.capacity = N;
    storage.d.length = 0;
    storage.d.boundingBox.reset();
    storage.d.vertices = (PointD*)( (sysuint_t)(storage.data + N + 15) & ~(sysuint_t)15 );
    return &storage.d;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathTmpD<N>& operator=(const PathTmpD<N>& other) { setPath(other); return *this; }
  FOG_INLINE PathTmpD<N>& operator=(const PathD& other) { setPath(other); return *this; }
  FOG_INLINE PathTmpD<N>& operator+=(const PathD& other) { append(other); return *this; }

  FOG_INLINE bool operator==(const PathD& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const PathD& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct _Storage
  {
    PathDataD d;
    uint8_t data[N * (sizeof(PointD) + sizeof(uint8_t))];
  } _storage;
};

// ============================================================================
// [Fog::PathTmpT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_TEMPLATE1_F_D(PathTmp, sysuint_t, N)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATH_TMP_P_H
