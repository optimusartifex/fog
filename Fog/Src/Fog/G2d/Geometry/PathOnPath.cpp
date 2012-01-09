// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Geometry/PathOnPath.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::PathOnPath]
// ============================================================================

PathOnPath::PathOnPath() :
  _calcLength(0.0),
  _baseLength(0.0)
{
}

PathOnPath::~PathOnPath()
{
}

err_t PathOnPath::process(PathF& dst, const PathF& src) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t PathOnPath::process(PathD& dst, const PathD& src) const
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t PathOnPath::setPath(const PathD& path, const TransformD* matrix)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
#if 0
  // Use larger scaling factor to increase path-on-path precision.
  double scalingFactor = 3.0;
  if (matrix) scalingFactor *= matrix->getAverageScaling();

  err_t err = path.flattenSubPathTo(_path, 0, matrix, scalingFactor);
  if (FOG_IS_ERROR(err)) return err;

  if (_path.isEmpty()) return ERR_OK;
  _dist.resize(_path.getLength());

  return ERR_OK;
#endif // 0
}

err_t PathOnPath::setBaseLength(double baseLength)
{
  _baseLength = baseLength;
  return ERR_OK;
}

} // Fog namespace
