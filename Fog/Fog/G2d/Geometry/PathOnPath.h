// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATHONPATH_H
#define _FOG_G2D_GEOMETRY_PATHONPATH_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathEffect.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PathOnPath]
// ============================================================================

struct FOG_API PathOnPath : public PathEffect
{
  PathOnPath();
  virtual ~PathOnPath();

  virtual err_t process(PathF& dst, const PathF& src) const;
  virtual err_t process(PathD& dst, const PathD& src) const;

  FOG_INLINE const PathD& getPath() const { return _path; }
  FOG_INLINE double getCalcLength() const { return _calcLength; }
  FOG_INLINE double getBaseLength() const { return _baseLength; }

  err_t setPath(const PathD& path, const TransformD* matrix = NULL);
  err_t setBaseLength(double baseLength);

protected:
  PathD _path;
  List<double> _dist;

  double _calcLength;
  double _baseLength;

private:
  FOG_DISABLE_COPY(PathOnPath)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::PathOnPath, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATHONPATH_H
