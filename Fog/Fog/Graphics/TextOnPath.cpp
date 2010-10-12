// [Fog-Graphics]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Graphics/TextOnPath.h>
#include <Fog/Graphics/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::PathTransform]
// ============================================================================

PathTransform::PathTransform()
{
}

PathTransform::~PathTransform()
{
}

// ============================================================================
// [Fog::TextOnPath]
// ============================================================================

TextOnPath::TextOnPath() :
  _calcLength(0.0),
  _baseLength(0.0)
{
}

TextOnPath::~TextOnPath()
{
}

err_t TextOnPath::transform(PathD& dst, const PathD& src) const
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t TextOnPath::setPath(const PathD& path, const TransformD* matrix)
{
  // Use larger scaling factor to increase path on path precision.
  double scalingFactor = 3.0;
  if (matrix) scalingFactor *= matrix->getAverageScaling();

  err_t err = path.flattenSubPathTo(_path, 0, matrix, scalingFactor);
  if (err) return err;

  if (_path.isEmpty()) return ERR_OK;

  _dist.resize(_path.getLength());

  return ERR_OK;
}

err_t TextOnPath::setBaseLength(double baseLength)
{
  _baseLength = baseLength;
  return ERR_OK;
}

} // Fog namespace
