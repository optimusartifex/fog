// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PIE_H
#define _FOG_G2D_GEOMETRY_PIE_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Global/Api.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct PieF;
struct PieD;

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
  FOG_INLINE PieF(_Uninitialized) : ArcF(UNINITIALIZED) {}

  FOG_INLINE PieF(const ArcF& other) : ArcF(other) {}
  FOG_INLINE PieF(const PointF& cp, float rad, float start_, float sweep_) : ArcF(cp, rad, start_, sweep_) {}
  FOG_INLINE PieF(const PointF& cp, const PointF& rad, float start_, float sweep_) : ArcF(cp, rad, start_, sweep_) {}
  FOG_INLINE PieF(const RectF& r, float start_, float sweep_) : ArcF(r, start_, sweep_) {}
  FOG_INLINE PieF(const BoxF& r, float start_, float sweep_) : ArcF(r, start_, sweep_) {}

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
    return _g2d.arcf.getBoundingBox(this, &dst, tr, true);
  }

  FOG_INLINE err_t _getBoundingRect(RectF& dst, const TransformF* tr) const
  {
    err_t err = _g2d.arcf.getBoundingBox(this, reinterpret_cast<BoxF*>(&dst), tr, true);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointF& pt) const
  {
    return _g2d.pief.hitTest(this, &pt);
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
  FOG_INLINE PieD(_Uninitialized) : ArcD(UNINITIALIZED) {}

  FOG_INLINE PieD(const ArcD& other) : ArcD(other) {}
  FOG_INLINE PieD(const PointD& cp, double rad, double start_, double sweep_) : ArcD(cp, rad, start_, sweep_) {}
  FOG_INLINE PieD(const PointD& cp, const PointD& rad, double start_, double sweep_) : ArcD(cp, rad, start_, sweep_) {}
  FOG_INLINE PieD(const RectD& r, double start_, double sweep_) : ArcD(r, start_, sweep_) {}
  FOG_INLINE PieD(const BoxD& r, double start_, double sweep_) : ArcD(r, start_, sweep_) {}

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
    return _g2d.arcd.getBoundingBox(this, &dst, tr, true);
  }

  FOG_INLINE err_t _getBoundingRect(RectD& dst, const TransformD* tr) const
  {
    err_t err = _g2d.arcd.getBoundingBox(this, reinterpret_cast<BoxD*>(&dst), tr, true);
    dst.w -= dst.x;
    dst.h -= dst.y;
    return err;
  }

  // --------------------------------------------------------------------------
  // [HitTest]
  // --------------------------------------------------------------------------

  FOG_INLINE bool hitTest(const PointD& pt) const
  {
    return _g2d.pied.hitTest(this, &pt);
  }
};

// ============================================================================
// [Fog::PieT<>]
// ============================================================================

FOG_CLASS_PRECISION_F_D(Pie)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::PieF, Fog::TYPEINFO_PRIMITIVE)
_FOG_TYPEINFO_DECLARE(Fog::PieD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_FUZZY_DECLARE(Fog::PieF, Fuzzy<ArcF>::eq(a, b))
FOG_FUZZY_DECLARE(Fog::PieD, Fuzzy<ArcD>::eq(a, b))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PIE_H
