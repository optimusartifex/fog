// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_CHORD_H
#define _FOG_G2D_GEOMETRY_CHORD_H

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

struct ChordF;
struct ChordD;

// ============================================================================
// [Fog::ChordF]
// ============================================================================

//! @brief Chord (float).
struct FOG_NO_EXPORT ChordF : ArcF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ChordF() : ArcF() {}
  FOG_INLINE ChordF(_Uninitialized) : ArcF(UNINITIALIZED) {}

  FOG_INLINE ChordF(const ArcF& other) : ArcF(other) {}
  FOG_INLINE ChordF(const PointF& cp, float rad, float start_, float sweep_) : ArcF(cp, rad, start_, sweep_) {}
  FOG_INLINE ChordF(const PointF& cp, const PointF& rad, float start_, float sweep_) : ArcF(cp, rad, start_, sweep_) {}
  FOG_INLINE ChordF(const RectF& r, float start_, float sweep_) : ArcF(r, start_, sweep_) {}
  FOG_INLINE ChordF(const BoxF& r, float start_, float sweep_) : ArcF(r, start_, sweep_) {}

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE ChordD toChordD() const;
};

// ============================================================================
// [Fog::ChordD]
// ============================================================================

//! @brief Chord (double).
struct FOG_NO_EXPORT ChordD : ArcD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ChordD() : ArcD() {}
  FOG_INLINE ChordD(_Uninitialized) : ArcD(UNINITIALIZED) {}

  FOG_INLINE ChordD(const ArcD& other) : ArcD(other) {}
  FOG_INLINE ChordD(const PointD& cp, double rad, double start_, double sweep_) : ArcD(cp, rad, start_, sweep_) {}
  FOG_INLINE ChordD(const PointD& cp, const PointD& rad, double start_, double sweep_) : ArcD(cp, rad, start_, sweep_) {}
  FOG_INLINE ChordD(const RectD& r, double start_, double sweep_) : ArcD(r, start_, sweep_) {}
  FOG_INLINE ChordD(const BoxD& r, double start_, double sweep_) : ArcD(r, start_, sweep_) {}

  // --------------------------------------------------------------------------
  // [Convert]
  // --------------------------------------------------------------------------

  FOG_INLINE ChordF toChordF() const;
};

// ============================================================================
// [Implemented-Later]
// ============================================================================

FOG_INLINE ChordD ChordF::toChordD() const { return ChordD(center.toPointD(), radius.toPointD(), (double)start, (double)sweep); }
FOG_INLINE ChordF ChordD::toChordF() const { return ChordF(center.toPointF(), radius.toPointF(), (float)start, (float)sweep); }

// ============================================================================
// [Fog::ChordT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(Chord)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::ChordF, Fog::TYPEINFO_PRIMITIVE)
FOG_DECLARE_TYPEINFO(Fog::ChordD, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Fuzzy<>]
// ============================================================================

FOG_DECLARE_FUZZY(Fog::ChordF, Fuzzy<ArcF>::eq(a, b))
FOG_DECLARE_FUZZY(Fog::ChordD, Fuzzy<ArcD>::eq(a, b))

// [Guard]
#endif // _FOG_G2D_GEOMETRY_CHORD_H
