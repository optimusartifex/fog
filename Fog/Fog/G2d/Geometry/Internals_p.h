// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_GEOMETRY_P_H
#define _FOG_G2D_GEOMETRY_GEOMETRY_P_H

// [Dependencies]
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Path.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Quadratic Curve Macros]
// ============================================================================

// a = p0 - 2*p1 + p2
// b = -2*p0 + 2*p1
// c = p0
#define _FOG_QUAD_EXTRACT_PARAMETERS(_Number_, _AX_, _AY_, _BX_, _BY_, _CX_, _CY_, _Pts_) \
  FOG_MACRO_BEGIN \
    _CX_ = _Pts_[0].x; \
    _CY_ = _Pts_[0].y; \
    \
    _AX_ = _CX_ - _Number_(2.0) * _Pts_[1].x + _Pts_[2].x; \
    _AY_ = _CY_ - _Number_(2.0) * _Pts_[1].y + _Pts_[2].y; \
    \
    _BX_ = _Number_(2.0) * (_Pts_[1].x - _Pts_[0].x); \
    _BY_ = _Number_(2.0) * (_Pts_[1].y - _Pts_[0].y); \
  FOG_MACRO_END

// Calculate the quadratic bezier curve coefficients at (t).
//
// a = (1-t)^2
// b = 2 * (1-t) * t
// c = t^2
#define _FOG_QUAD_COEFF(_Number_, t, a, b, c) \
  FOG_MACRO_BEGIN \
    _Number_ inv_t = 1.0f - t; \
    \
    a = inv_t * inv_t; \
    b = 2.0f * inv_t * t; \
    c = t * t; \
  FOG_MACRO_END

#define _FOG_QUAD_MERGE(_Number_, t, _Pts_, _PMin_, _PMax_) \
  FOG_MACRO_BEGIN \
    if (t > _Number_(0.0) && t < _Number_(1.0)) \
    { \
      _FOG_QUAD_COEFF(_Number_, t, a, b, c); \
      \
      typename PointT<_Number_>::T _p( \
        a * _Pts_[0].x + b * _Pts_[1].x + c * _Pts_[2].x, \
        a * _Pts_[0].y + b * _Pts_[1].y + c * _Pts_[2].y); \
      \
      if (_p.x < _PMin_.x) _PMin_.x = _p.x; else if (_p.x > _PMax_.x) _PMax_.x = _p.x; \
      if (_p.y < _PMin_.y) _PMin_.y = _p.y; else if (_p.y > _PMax_.y) _PMax_.y = _p.y; \
    } \
  FOG_MACRO_END

// ============================================================================
// [Cubic Curve Macros]
// ============================================================================

#define _FOG_CUBIC_EXTRACT_PARAMETERS(_Number_, _AX_, _AY_, _BX_, _BY_, _CX_, _CY_, _DX_, _DY_, _Pts_) \
  FOG_MACRO_BEGIN \
    _DX_ = _Pts_[0].x; \
    _DY_ = _Pts_[0].y; \
    \
    _AX_ = -_DX_ + _Number_(3.0) * (_Pts_[1].x - _Pts_[2].x) + _Pts_[3].x; \
    _AY_ = -_DY_ + _Number_(3.0) * (_Pts_[1].y - _Pts_[2].y) + _Pts_[3].y; \
    \
    _BX_ = _Number_(3.0) * (_DX_ - _Number_(2.0) * _Pts_[1].x + _Pts_[2].x); \
    _BY_ = _Number_(3.0) * (_DY_ - _Number_(2.0) * _Pts_[1].y + _Pts_[2].y); \
    \
    _CX_ = _Number_(3.0) * (_Pts_[1].x - _Pts_[0].x); \
    _CY_ = _Number_(3.0) * (_Pts_[1].y - _Pts_[0].y); \
  FOG_MACRO_END

// Calculate the cubic bezier curve coefficients at (t).
//
// a = (1-t)^3
// b = 3 * (1-t)^2 * t
// c = 3 * (1-t) * t^2
// d = (t)^3
#define _FOG_CUBIC_COEFF(_Number_, t, a, b, c, d) \
  FOG_MACRO_BEGIN \
    _Number_ inv_t = _Number_(1.0) - t; \
    _Number_ t_2 = t * t; \
    _Number_ inv_t_2 = inv_t * inv_t; \
    \
    a = inv_t * inv_t_2; \
    b = _Number_(3.0) * inv_t_2 * t; \
    c = _Number_(3.0) * inv_t * t_2; \
    d = t * t_2; \
  FOG_MACRO_END

#define _FOG_CUBIC_MERGE(_Number_, t, _Pts_, _PMin_, _PMax_) \
  FOG_MACRO_BEGIN \
    if (t > _Number_(0.0) && t < _Number_(1.0)) \
    { \
      _FOG_CUBIC_COEFF(_Number_, t, a, b, c, d); \
      \
      typename PointT<_Number_>::T _p( \
        a * _Pts_[0].x + b * _Pts_[1].x + c * _Pts_[2].x + d * _Pts_[3].x, \
        a * _Pts_[0].y + b * _Pts_[1].y + c * _Pts_[2].y + d * _Pts_[3].y); \
      \
      if (_p.x < _PMin_.x) _PMin_.x = _p.x; else if (_p.x > _PMax_.x) _PMax_.x = _p.x; \
      if (_p.y < _PMin_.y) _PMin_.y = _p.y; else if (_p.y > _PMax_.y) _PMax_.y = _p.y; \
    } \
  FOG_MACRO_END

// ============================================================================
// [Cubic Curve Macros]
// ============================================================================

static FOG_INLINE void _G2d_PathT_verifyBoundingBox(const PathF& path)
{
  if ((path._d->flags & PATH_DATA_DIRTY_BBOX) == 0)
  {
    BoxF _bbOld, _bbNew;
    path._d->flags |= PATH_DATA_DIRTY_BBOX;

    _bbOld = path._d->boundingBox;
    path.getBoundingBox(_bbNew);

    // Track the inconsistency when new bounding box is greater than the old one.
    FOG_ASSERT(_bbOld.subsumes(_bbNew));
  }
}

static FOG_INLINE void _G2d_PathT_verifyBoundingBox(const PathD& path)
{
  if ((path._d->flags & PATH_DATA_DIRTY_BBOX) == 0)
  {
    BoxD _bbOld, _bbNew;
    path._d->flags |= PATH_DATA_DIRTY_BBOX;

    _bbOld = path._d->boundingBox;
    path.getBoundingBox(_bbNew);

    // Track the inconsistency when new bounding box is greater than the old one.
    FOG_ASSERT(_bbOld.subsumes(_bbNew));
  }
}

#if defined(FOG_DEBUG)
#define _FOG_PATH_VERIFY_BOUNDING_BOX(_Path_) _G2d_PathT_verifyBoundingBox(_Path_)
#else
#define _FOG_PATH_VERIFY_BOUNDING_BOX(_Path_) FOG_NOP
#endif

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_GEOMETRY_P_H
