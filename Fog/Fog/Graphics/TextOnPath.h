// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_TEXTONPATH_H
#define _FOG_GRAPHICS_TEXTONPATH_H

// [Dependencies]
#include <Fog/Core/List.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Path.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Fog::PathTransform]
// ============================================================================

struct FOG_API PathTransform
{
  PathTransform();
  virtual ~PathTransform();

  virtual err_t transform(DoublePath& dst, const DoublePath& src) const = 0;

private:
  FOG_DISABLE_COPY(PathTransform)
};

// ============================================================================
// [Fog::TextOnPath]
// ============================================================================

struct FOG_API TextOnPath : public PathTransform
{
  TextOnPath();
  virtual ~TextOnPath();

  virtual err_t transform(DoublePath& dst, const DoublePath& src) const;

  FOG_INLINE const DoublePath& getPath() const { return _path; }
  FOG_INLINE double getCalcLength() const { return _calcLength; }
  FOG_INLINE double getBaseLength() const { return _baseLength; }

  err_t setPath(const DoublePath& path, const DoubleMatrix* matrix = NULL);
  err_t setBaseLength(double baseLength);

private:
  DoublePath _path;
  List<double> _dist;

  double _calcLength;
  double _baseLength;

  FOG_DISABLE_COPY(TextOnPath)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_TEXTONPATH_H
