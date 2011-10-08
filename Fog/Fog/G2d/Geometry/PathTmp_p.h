// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATH_TMP_P_H
#define _FOG_G2D_GEOMETRY_PATH_TMP_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
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
template<size_t N = 108>
struct FOG_NO_EXPORT PathTmpF : public PathF
{
  struct _Storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathTmpF() : PathF(_api.pathf_dAdopt(&_storage, N))
  {
  }

  FOG_INLINE PathTmpF(const PathTmpF<N>& other) :
    PathF(_api.pathf_dAdopt(&_storage, N))
  {
    setDeep(other);
  }

  FOG_INLINE PathTmpF(const PathF& other) :
    PathF(_api.pathf_dAdopt(_storage, N))
  {
    setDeep(other);
  }

  FOG_INLINE ~PathTmpF()
  {
    FOG_ASSERT( (_d == &_storage.d && _storage.d.reference.get() == 1) ||
                (_d != &_storage.d && _storage.d.reference.get() == 0) );
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
template<size_t N = 58>
struct FOG_NO_EXPORT PathTmpD : public PathD
{
  struct _Storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathTmpD() :
    PathD(_api.pathd_dAdopt(&_storage, N))
  {
  }

  FOG_INLINE PathTmpD(const PathTmpD<N>& other) :
    PathD(_api.pathd_dAdopt(&_storage, N))
  {
    setDeep(other);
  }

  FOG_INLINE PathTmpD(const PathD& other) :
    PathD(_api.pathd_dAdopt(&_storage, N))
  {
    setDeep(other);
  }

  FOG_INLINE ~PathTmpD()
  {
    FOG_ASSERT( (_d == &_storage.d && _storage.d.reference.get() == 1) ||
                (_d != &_storage.d && _storage.d.reference.get() == 0) );
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

_FOG_NUM_TEMPLATE1_T(PathTmp, size_t, N)
_FOG_NUM_TEMPLATE1_F(PathTmp, size_t, N)
_FOG_NUM_TEMPLATE1_D(PathTmp, size_t, N)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATH_TMP_P_H
