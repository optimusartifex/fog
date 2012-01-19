// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_POINTARRAY_H
#define _FOG_G2D_GEOMETRY_POINTARRAY_H

// [Dependencies]
#include <Fog/G2d/Geometry/Point.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PointArrayI]
// ============================================================================

//! @brief Point array (32-bit integer version).
struct FOG_NO_EXPORT PointArrayI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointArrayI() :
    _length(0),
    _data(NULL)
  {
  }

  FOG_INLINE PointArrayI(const PointArrayI& other) :
    _length(other._length),
    _data(other._data)
  {
  }

  FOG_INLINE PointArrayI(const PointI* data, size_t length) :
    _length(length),
    _data(const_cast<PointI*>(data))
  {
  }

  explicit FOG_INLINE PointArrayI(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE const PointI* getData() const { return _data; }

  FOG_INLINE void setLength(size_t length) { _length = length; }
  FOG_INLINE void setData(PointI* data) { _data = data; }

  FOG_INLINE void setArray(const PointI* data, size_t length)
  {
    _length = length;
    _data = const_cast<PointI*>(data); 
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t _length;
  PointI* _data;
};

// ============================================================================
// [Fog::PointArrayF]
// ============================================================================

//! @brief Point array (float).
struct FOG_NO_EXPORT PointArrayF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointArrayF() :
    _length(0),
    _data(NULL)
  {
  }

  FOG_INLINE PointArrayF(const PointArrayF& other) :
    _length(other._length),
    _data(other._data)
  {
  }

  FOG_INLINE PointArrayF(const PointF* data, size_t length) :
    _length(length),
    _data(const_cast<PointF*>(data))
  {
  }

  explicit FOG_INLINE PointArrayF(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE const PointF* getData() const { return _data; }

  FOG_INLINE void setLength(size_t length) { _length = length; }
  FOG_INLINE void setData(PointF* data) { _data = data; }

  FOG_INLINE void setArray(const PointF* data, size_t length)
  {
    _length = length;
    _data = const_cast<PointF*>(data); 
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t _length;
  PointF* _data;
};

// ============================================================================
// [Fog::PointArrayD]
// ============================================================================

//! @brief Point array (double).
struct FOG_NO_EXPORT PointArrayD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PointArrayD() :
    _length(0),
    _data(NULL)
  {
  }

  FOG_INLINE PointArrayD(const PointArrayD& other) :
    _length(other._length),
    _data(other._data)
  {
  }

  FOG_INLINE PointArrayD(const PointD* data, size_t length) :
    _length(length),
    _data(const_cast<PointD*>(data))
  {
  }

  explicit FOG_INLINE PointArrayD(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE const PointD* getData() const { return _data; }

  FOG_INLINE void setLength(size_t length) { _length = length; }
  FOG_INLINE void setData(const PointD* data) { _data = const_cast<PointD*>(data); }

  FOG_INLINE void setArray(const PointD* data, size_t length)
  {
    _length = length;
    _data = const_cast<PointD*>(data); 
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t _length;
  PointD* _data;
};

// ============================================================================
// [Fog::PointT<>]
// ============================================================================

_FOG_NUM_T(PointArray)
_FOG_NUM_I(PointArray)
_FOG_NUM_F(PointArray)
_FOG_NUM_D(PointArray)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_POINTARRAY_H
