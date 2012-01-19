// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_RECTARRAY_H
#define _FOG_G2D_GEOMETRY_RECTARRAY_H

// [Dependencies]
#include <Fog/G2d/Geometry/Rect.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::RectArrayI]
// ============================================================================

//! @brief Rect array (32-bit integer version).
struct FOG_NO_EXPORT RectArrayI
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectArrayI() :
    _length(0),
    _data(NULL)
  {
  }

  FOG_INLINE RectArrayI(const RectArrayI& other) :
    _length(other._length),
    _data(other._data)
  {
  }

  FOG_INLINE RectArrayI(const RectI* data, size_t length) :
    _length(length),
    _data(const_cast<RectI*>(data))
  {
  }

  explicit FOG_INLINE RectArrayI(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE const RectI* getData() const { return _data; }

  FOG_INLINE void setLength(size_t length) { _length = length; }
  FOG_INLINE void setData(RectI* data) { _data = data; }

  FOG_INLINE void setArray(const RectI* data, size_t length)
  {
    _length = length;
    _data = const_cast<RectI*>(data); 
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t _length;
  RectI* _data;
};

// ============================================================================
// [Fog::RectArrayF]
// ============================================================================

//! @brief Rect array (float).
struct FOG_NO_EXPORT RectArrayF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectArrayF() :
    _length(0),
    _data(NULL)
  {
  }

  FOG_INLINE RectArrayF(const RectArrayF& other) :
    _length(other._length),
    _data(other._data)
  {
  }

  FOG_INLINE RectArrayF(const RectF* data, size_t length) :
    _length(length),
    _data(const_cast<RectF*>(data))
  {
  }

  explicit FOG_INLINE RectArrayF(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE const RectF* getData() const { return _data; }

  FOG_INLINE void setLength(size_t length) { _length = length; }
  FOG_INLINE void setData(RectF* data) { _data = data; }

  FOG_INLINE void setArray(const RectF* data, size_t length)
  {
    _length = length;
    _data = const_cast<RectF*>(data); 
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t _length;
  RectF* _data;
};

// ============================================================================
// [Fog::RectArrayD]
// ============================================================================

//! @brief Rect array (double).
struct FOG_NO_EXPORT RectArrayD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE RectArrayD() :
    _length(0),
    _data(NULL)
  {
  }

  FOG_INLINE RectArrayD(const RectArrayD& other) :
    _length(other._length),
    _data(other._data)
  {
  }

  FOG_INLINE RectArrayD(const RectD* data, size_t length) :
    _length(length),
    _data(const_cast<RectD*>(data))
  {
  }

  explicit FOG_INLINE RectArrayD(_Uninitialized) {}

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getLength() const { return _length; }
  FOG_INLINE const RectD* getData() const { return _data; }

  FOG_INLINE void setLength(size_t length) { _length = length; }
  FOG_INLINE void setData(const RectD* data) { _data = const_cast<RectD*>(data); }

  FOG_INLINE void setArray(const RectD* data, size_t length)
  {
    _length = length;
    _data = const_cast<RectD*>(data); 
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t _length;
  RectD* _data;
};

// ============================================================================
// [Fog::RectT<>]
// ============================================================================

_FOG_NUM_T(RectArray)
_FOG_NUM_I(RectArray)
_FOG_NUM_F(RectArray)
_FOG_NUM_D(RectArray)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_RECTARRAY_H
