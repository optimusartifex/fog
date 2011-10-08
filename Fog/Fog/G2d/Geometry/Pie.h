// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PIE_H
#define _FOG_G2D_GEOMETRY_PIE_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Geometry/Arc.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PieF]
// ============================================================================

//! @brief Pie (float).
struct FOG_NO_EXPORT PieF : ArcF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PieF() : ArcF() {}
  FOG_INLINE PieF(const ArcF& other) : ArcF(other) {}
  FOG_INLINE PieF(const PointF& cp, float rad, float start_, float sweep_) : ArcF(cp, rad, start_, sweep_) {}
  FOG_INLINE PieF(const PointF& cp, const PointF& rad, float start_, float sweep_) : ArcF(cp, rad, start_, sweep_) {}
  FOG_INLINE PieF(const RectF& r, float start_, float sweep_) : ArcF(r, start_, sweep_) {}
  FOG_INLINE PieF(const BoxF& r, float start_, float sweep_) : ArcF(r, start_, sweep_) {}

  explicit FOG_INLINE PieF(_Uninitialized) : ArcF(UNINITIALIZED) {}
  explicit FOG_INLINE PieF(const ArcD& other) : ArcF(other) {}

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxF& dst) const
  {
    return PieF::_getBoundingBox(dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxF& dst, const TransformF& tr) const
  {
    return PieF::_getBoundingBox(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst) const
  {
    return PieF::_getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectF& dst, const TransformF& tr) const
  {
    return PieF::_getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t _getBoundingBox(BoxF& dst, const TransformF* tr) const
  {
    return _api.arcf_getBoundingBox(this, &dst, tr, true);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _api.arcf_getBoundingBox(this, reinterpret_cast<BoxF*>(&dst), tr, true);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return _api.pief_hitTest(this, &pt);
  }
};

// ============================================================================
// [Fog::PieD]
// ============================================================================

//! @brief Pie (double).
struct FOG_NO_EXPORT PieD : ArcD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PieD() : ArcD() {}
  FOG_INLINE PieD(const ArcD& other) : ArcD(other) {}
  FOG_INLINE PieD(const PointD& cp, double rad, double start_, double sweep_) : ArcD(cp, rad, start_, sweep_) {}
  FOG_INLINE PieD(const PointD& cp, const PointD& rad, double start_, double sweep_) : ArcD(cp, rad, start_, sweep_) {}
  FOG_INLINE PieD(const RectD& r, double start_, double sweep_) : ArcD(r, start_, sweep_) {}
  FOG_INLINE PieD(const BoxD& r, double start_, double sweep_) : ArcD(r, start_, sweep_) {}

  explicit FOG_INLINE PieD(_Uninitialized) : ArcD(UNINITIALIZED) {}
  explicit FOG_INLINE PieD(const ArcF& other) : ArcD(other) {}

  // --------------------------------------------------------------------------
  // [BoundingBox / BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t getBoundingBox(BoxD& dst) const
  {
    return PieD::_getBoundingBox(dst, NULL);
  }

  FOG_INLINE err_t getBoundingBox(BoxD& dst, const TransformD& tr) const
  {
    return PieD::_getBoundingBox(dst, &tr);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst) const
  {
    return PieD::_getBoundingRect(dst, NULL);
  }

  FOG_INLINE err_t getBoundingRect(RectD& dst, const TransformD& tr) const
  {
    return PieD::_getBoundingRect(dst, &tr);
  }

  FOG_INLINE err_t _getBoundingBox(BoxD& dst, const TransformD* tr) const
  {
    return _api.arcd_getBoundingBox(this, &dst, tr, true);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _api.arcd_getBoundingBox(this, reinterpret_cast<BoxD*>(&dst), tr, true);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return _api.pied_hitTest(this, &pt);
  }
};

// ============================================================================
// [Fog::PieT<>]
// ============================================================================

_FOG_NUM_T(Pie)
_FOG_NUM_F(Pie)
_FOG_NUM_D(Pie)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE_F_VEC(Fog::PieF, 6)
FOG_FUZZY_DECLARE_D_VEC(Fog::PieD, 6)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PIE_H
