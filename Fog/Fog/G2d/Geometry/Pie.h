// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PIE_H
#define _FOG_G2D_GEOMETRY_PIE_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
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

  // --------------------------------------------------------------------------
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectF getBoundingRect() const
  {
    RectF result;
    _g2d.arcf.getBoundingRect(this, &result, true);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE PieD toPieD() const;
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

  // --------------------------------------------------------------------------
  // [BoundingRect]
  // --------------------------------------------------------------------------

  FOG_INLINE RectD getBoundingRect() const
  {
    RectD result;
    _g2d.arcd.getBoundingRect(this, &result, true);
    return result;
  }

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE PieF toPieF() const;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE PieD PieF::toPieD() const { return PieD(center.toPointD(), radius.toPointD(), (double)start, (double)sweep); }
FOG_INLINE PieF PieD::toPieF() const { return PieF(center.toPointF(), radius.toPointF(), (float)start, (float)sweep); }

// ============================================================================
// [Fog::PieT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(Pie)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::PieF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::PieD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::PieF, Fuzzy<ArcF>::eq(a, b))
FOG_DECLARE_FUZZY(Fog::PieD, Fuzzy<ArcD>::eq(a, b))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PIE_H
