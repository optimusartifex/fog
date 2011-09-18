// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_PATHSTROKER_H
#define _FOG_G2D_GEOMETRY_PATHSTROKER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathEffect.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Fog::PathStrokerHints]
// ============================================================================

#include <Fog/Core/C++/PackByte.h>
union FOG_NO_EXPORT PathStrokerHints
{
  struct
  {
    uint32_t startCap : 8;
    uint32_t endCap : 8;
    uint32_t lineJoin : 8;
    uint32_t reserved : 8;
  };

  uint32_t packed;
};
#include <Fog/Core/C++/PackRestore.h>

// ============================================================================
// [Fog::PathStrokerParamsF]
// ============================================================================

struct FOG_NO_EXPORT PathStrokerParamsF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerParamsF() :
    _lineWidth(1.0f),
    _miterLimit(4.0f),
    _dashOffset(0.0f)
  {
    _hints.startCap = LINE_CAP_DEFAULT;
    _hints.endCap = LINE_CAP_DEFAULT;
    _hints.lineJoin = LINE_JOIN_DEFAULT;
    _hints.reserved = 0;
  }

  FOG_INLINE PathStrokerParamsF(const PathStrokerParamsF& other) :
    _lineWidth(other._lineWidth),
    _miterLimit(other._miterLimit),
    _dashOffset(other._dashOffset),
    _dashList(other._dashList)
  {
    _hints.packed = other._hints.packed;
  }

  FOG_INLINE ~PathStrokerParamsF()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE float getLineWidth() const { return _lineWidth; }
  FOG_INLINE float getMiterLimit() const { return _miterLimit; }
  FOG_INLINE float getDashOffset() const { return _dashOffset; }
  FOG_INLINE const List<float>& getDashList() const { return _dashList; }

  FOG_INLINE uint32_t getHints() const { return _hints.packed; }
  FOG_INLINE uint32_t getStartCap() const { return _hints.startCap; }
  FOG_INLINE uint32_t getEndCap() const { return _hints.endCap; }
  FOG_INLINE uint32_t getLineJoin() const { return _hints.lineJoin; }

  FOG_INLINE void setLineWidth(float lineWidth) { _lineWidth = lineWidth; }
  FOG_INLINE void setMiterLimit(float miterLimit) { _miterLimit = miterLimit; }
  FOG_INLINE void setDashOffset(float dashOffset) { _dashOffset = dashOffset; }
  FOG_INLINE void setDashList(const List<float>& dashList) { _dashList = dashList; }
  FOG_INLINE err_t setDashList(const float* dashList, size_t length) { return _dashList.setList(dashList, length); }

  FOG_INLINE void setHints(uint32_t hints) { _hints.packed = hints; }
  FOG_INLINE void setStartCap(uint32_t startCap) { _hints.startCap = startCap; }
  FOG_INLINE void setEndCap(uint32_t endCap) { _hints.endCap = endCap; }
  FOG_INLINE void setLineCaps(uint32_t lineCaps) { _hints.startCap = _hints.endCap = lineCaps; }
  FOG_INLINE void setLineJoin(uint32_t lineJoin) { _hints.lineJoin = lineJoin; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _lineWidth = 1.0f;
    _miterLimit = 4.0f;
    _dashOffset = 0.0f;
    _dashList.reset();
    resetHints();
  }

  FOG_INLINE void resetHints()
  {
    _hints.startCap = LINE_CAP_DEFAULT;
    _hints.endCap = LINE_CAP_DEFAULT;
    _hints.lineJoin = LINE_JOIN_DEFAULT;
    _hints.reserved = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerParamsF& operator=(const PathStrokerParamsF& other);
  FOG_INLINE PathStrokerParamsF& operator=(const PathStrokerParamsD& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float _lineWidth;
  float _miterLimit;

  float _dashOffset;
  PathStrokerHints _hints;

  List<float> _dashList;
};

// ============================================================================
// [Fog::PathStrokerParamsD]
// ============================================================================

struct FOG_NO_EXPORT PathStrokerParamsD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerParamsD() :
    _lineWidth(1.0),
    _miterLimit(4.0),
    _dashOffset(0.0)
  {
    _hints.startCap = LINE_CAP_DEFAULT;
    _hints.endCap = LINE_CAP_DEFAULT;
    _hints.lineJoin = LINE_JOIN_DEFAULT;
    _hints.reserved = 0;
  }

  FOG_INLINE PathStrokerParamsD(const PathStrokerParamsD& other) :
    _lineWidth(other._lineWidth),
    _miterLimit(other._miterLimit),
    _dashOffset(other._dashOffset),
    _dashList(other._dashList)
  {
    _hints.packed = other._hints.packed;
  }

  FOG_INLINE ~PathStrokerParamsD()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getLineWidth() const { return _lineWidth; }
  FOG_INLINE double getMiterLimit() const { return _miterLimit; }
  FOG_INLINE double getDashOffset() const { return _dashOffset; }
  FOG_INLINE const List<double>& getDashList() const { return _dashList; }

  FOG_INLINE uint32_t getHints() const { return _hints.packed; }
  FOG_INLINE uint32_t getStartCap() const { return _hints.startCap; }
  FOG_INLINE uint32_t getEndCap() const { return _hints.endCap; }
  FOG_INLINE uint32_t getLineJoin() const { return _hints.lineJoin; }

  FOG_INLINE void setLineWidth(double lineWidth) { _lineWidth = lineWidth; }
  FOG_INLINE void setMiterLimit(double miterLimit) { _miterLimit = miterLimit; }
  FOG_INLINE void setDashOffset(double dashOffset) { _dashOffset = dashOffset; }
  FOG_INLINE void setDashList(const List<double>& dashList) { _dashList = dashList; }
  FOG_INLINE err_t setDashList(const double* dashList, size_t length) { return _dashList.setList(dashList, length); }

  FOG_INLINE void setHints(uint32_t hints) { _hints.packed = hints; }
  FOG_INLINE void setStartCap(uint32_t startCap) { _hints.startCap = startCap; }
  FOG_INLINE void setEndCap(uint32_t endCap) { _hints.endCap = endCap; }
  FOG_INLINE void setLineCaps(uint32_t lineCaps) { _hints.startCap = _hints.endCap = lineCaps; }
  FOG_INLINE void setLineJoin(uint32_t lineJoin) { _hints.lineJoin = lineJoin; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _lineWidth = 1.0;
    _miterLimit = 4.0;
    _dashOffset = 0.0;
    _dashList.reset();
    resetHints();
  }

  FOG_INLINE void resetHints()
  {
    _hints.startCap = LINE_CAP_DEFAULT;
    _hints.endCap = LINE_CAP_DEFAULT;
    _hints.lineJoin = LINE_JOIN_DEFAULT;
    _hints.reserved = 0;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerParamsD& operator=(const PathStrokerParamsF& other);
  FOG_INLINE PathStrokerParamsD& operator=(const PathStrokerParamsD& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double _lineWidth;
  double _miterLimit;

  double _dashOffset;
  List<double> _dashList;

  PathStrokerHints _hints;
};

// ============================================================================
// [Fog::PathStrokerF]
// ============================================================================

struct FOG_NO_EXPORT PathStrokerF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerF()
  {
    _api.pathstrokerf.ctor(this);
  }

  FOG_INLINE PathStrokerF(const PathStrokerParamsF& params)
  {
    _api.pathstrokerf.ctorParams(this, &params, NULL, NULL);
  }

  FOG_INLINE PathStrokerF(const PathStrokerParamsF& params, const TransformF& transform)
  {
    _api.pathstrokerf.ctorParams(this, &params, &transform, NULL);
  }

  FOG_INLINE PathStrokerF(const PathStrokerParamsF& params, const TransformF& transform, const BoxF& clipBox)
  {
    _api.pathstrokerf.ctorParams(this, &params, &transform, &clipBox);
  }

  FOG_INLINE ~PathStrokerF()
  {
    _api.pathstrokerf.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Stroke-Params]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathStrokerParamsF& getParams() const
  {
    return _params;
  }

  FOG_INLINE void setParams(const PathStrokerParamsF& params)
  {
    _api.pathstrokerf.setParams(this, &params);
  }

  // --------------------------------------------------------------------------
  // [Flatten-Params]
  // --------------------------------------------------------------------------

  FOG_INLINE float getFlatness() const
  {
    return _flatness;
  }

  FOG_INLINE void setFlatness(float flatness)
  {
    _flatness = flatness;
    _isDirty = true;
  }

  FOG_INLINE uint32_t getFlattenType() const
  {
    return _flattenType;
  }

  FOG_INLINE void setFlattenType(uint32_t flattenType)
  {
    FOG_ASSERT(flattenType < PATH_FLATTEN_COUNT);
    _flattenType = (uint8_t)flattenType;
  }

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxF& getClipBox() const
  {
    return _clipBox;
  }

  FOG_INLINE void setClipBox(const BoxF& clipBox)
  {
    _clipBox = clipBox;

    _isDirty = true;
    _isClippingEnabled = true;
  }

  FOG_INLINE void resetClipBox()
  {
    _clipBox.reset();

    _isDirty = true;
    _isClippingEnabled = false;
  }

  FOG_INLINE bool isClippingEnabled() const
  {
    return _isClippingEnabled;
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE const TransformF& getTransform() const
  {
    return _transform;
  }

  FOG_INLINE void setTransform(const TransformF& transform)
  {
    _transform() = transform;
    _isDirty = true;
  }

  FOG_INLINE void resetTransform()
  {
    _transform->reset();
    _isDirty = true;
  }

  // --------------------------------------------------------------------------
  // [Stroke]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t strokeShape(PathF& dst, uint32_t shapeType, const void* shapeData) const
  {
    return _api.pathstrokerf.strokeShape(this, &dst, shapeType, shapeData);
  }

  FOG_INLINE err_t strokeRect(PathF& dst, const RectF& rect) const
  {
    return _api.pathstrokerf.strokeShape(this, &dst, SHAPE_TYPE_RECT, &rect);
  }

  FOG_INLINE err_t strokePath(PathF& dst, const PathF& src) const
  {
    return _api.pathstrokerf.strokePath(this, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  FOG_INLINE void _update()
  {
    _api.pathstrokerf.update(this);
  }

  FOG_INLINE void update() const
  {
    if (_isDirty)
      _api.pathstrokerf.update(const_cast<PathStrokerF*>(this));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerF& operator=(const PathStrokerF& other)
  {
    _api.pathstrokerf.setOther(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathStrokerParamsF> _params;
  Static<TransformF> _transform;

  BoxF _clipBox;
  BoxF _transformedClipBox;

  //! @brief Width / 2 after the simple transformation (if used).
  float _w;
  //! @brief Absolute value of _w.
  float _wAbs;
  float _wEps;
  float _da;
  float _flatness;

  int _wSign;

  uint8_t _isDirty;
  uint8_t _isClippingEnabled;
  uint8_t _isTransformSimple;
  uint8_t _flattenType;
};

// ============================================================================
// [Fog::PathStrokerD]
// ============================================================================

struct FOG_NO_EXPORT PathStrokerD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerD()
  {
    _api.pathstrokerd.ctor(this);
  }

  FOG_INLINE PathStrokerD(const PathStrokerParamsD& params)
  {
    _api.pathstrokerd.ctorParams(this, &params, NULL, NULL);
  }

  FOG_INLINE PathStrokerD(const PathStrokerParamsD& params, const TransformD& transform)
  {
    _api.pathstrokerd.ctorParams(this, &params, &transform, NULL);
  }

  FOG_INLINE PathStrokerD(const PathStrokerParamsD& params, const TransformD& transform, const BoxD& clipBox)
  {
    _api.pathstrokerd.ctorParams(this, &params, &transform, &clipBox);
  }

  FOG_INLINE ~PathStrokerD()
  {
    _api.pathstrokerd.dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Stroke-Params]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathStrokerParamsD& getParams() const
  {
    return _params;
  }

  FOG_INLINE void setParams(const PathStrokerParamsD& params)
  {
    _api.pathstrokerd.setParams(this, &params);
  }

  // --------------------------------------------------------------------------
  // [Flatten-Params]
  // --------------------------------------------------------------------------

  FOG_INLINE double getFlatness() const
  {
    return _flatness;
  }

  FOG_INLINE void setFlatness(double flatness)
  {
    _flatness = flatness;
    _isDirty = true;
  }

  FOG_INLINE uint32_t getFlattenType() const
  {
    return _flattenType;
  }

  FOG_INLINE void setFlattenType(uint32_t flattenType)
  {
    FOG_ASSERT(flattenType < PATH_FLATTEN_COUNT);
    _flattenType = (uint8_t)flattenType;
  }

  // --------------------------------------------------------------------------
  // [Clipping]
  // --------------------------------------------------------------------------

  FOG_INLINE const BoxD& getClipBox() const
  {
    return _clipBox;
  }

  FOG_INLINE void setClipBox(const BoxD& clipBox)
  {
    _clipBox = clipBox;

    _isDirty = true;
    _isClippingEnabled = true;
  }

  FOG_INLINE void resetClipBox()
  {
    _clipBox.reset();

    _isDirty = true;
    _isClippingEnabled = false;
  }

  FOG_INLINE bool isClippingEnabled() const
  {
    return _isClippingEnabled;
  }

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  FOG_INLINE const TransformD& getTransform() const
  {
    return _transform;
  }

  FOG_INLINE void setTransform(const TransformD& transform)
  {
    _transform() = transform;
    _isDirty = true;
  }

  FOG_INLINE void resetTransform()
  {
    _transform->reset();
    _isDirty = true;
  }

  // --------------------------------------------------------------------------
  // [Stroke]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t strokeShape(PathD& dst, uint32_t shapeType, const void* shapeData) const
  {
    return _api.pathstrokerd.strokeShape(this, &dst, shapeType, shapeData);
  }

  FOG_INLINE err_t strokeRect(PathD& dst, const RectD& rect) const
  {
    return _api.pathstrokerd.strokeShape(this, &dst, SHAPE_TYPE_RECT, &rect);
  }

  FOG_INLINE err_t strokePath(PathD& dst, const PathD& src) const
  {
    return _api.pathstrokerd.strokePath(this, &dst, &src);
  }

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  FOG_INLINE void _update()
  {
    _api.pathstrokerd.update(this);
  }

  FOG_INLINE void update() const
  {
    if (_isDirty)
      _api.pathstrokerd.update(const_cast<PathStrokerD*>(this));
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE PathStrokerD& operator=(const PathStrokerD& other)
  {
    _api.pathstrokerd.setOther(this, &other);
    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Static<PathStrokerParamsD> _params;
  Static<TransformD> _transform;

  BoxD _clipBox;
  BoxD _transformedClipBox;

  //! @brief Width / 2 after the simple transformation (if used).
  double _w;
  //! @brief Absolute value of _w.
  double _wAbs;
  double _wEps;
  double _da;
  double _flatness;

  int _wSign;

  uint8_t _isDirty;
  uint8_t _isClippingEnabled;
  uint8_t _isTransformSimple;
  uint8_t _flattenType;
};

// ============================================================================
// [Implemented Later]
// ============================================================================

FOG_INLINE PathStrokerParamsF& PathStrokerParamsF::operator=(const PathStrokerParamsF& other)
{
  _lineWidth = other._lineWidth;
  _miterLimit = other._miterLimit;
  _dashOffset = other._dashOffset;
  _dashList = other._dashList;
  _hints.packed = other._hints.packed;

  return *this;
}

FOG_INLINE PathStrokerParamsF& PathStrokerParamsF::operator=(const PathStrokerParamsD& other)
{
  _lineWidth = (float)other._lineWidth;
  _miterLimit = (float)other._miterLimit;
  _dashOffset = (float)other._dashOffset;

  _dashList.setList(other._dashList);
  _hints.packed = other._hints.packed;

  return *this;
}

FOG_INLINE PathStrokerParamsD& PathStrokerParamsD::operator=(const PathStrokerParamsF& other)
{
  _lineWidth = other._lineWidth;
  _miterLimit = other._miterLimit;
  _dashOffset = other._dashOffset;

  _dashList.setList(other._dashList);
  _hints.packed = other._hints.packed;

  return *this;
}

FOG_INLINE PathStrokerParamsD& PathStrokerParamsD::operator=(const PathStrokerParamsD& other)
{
  _lineWidth = other._lineWidth;
  _miterLimit = other._miterLimit;
  _dashOffset = other._dashOffset;

  _dashList = other._dashList;
  _hints.packed = other._hints.packed;

  return *this;
}

// ============================================================================
// [Fog::PathStrokerT<> / Fog::PathStrokerParamsT<>]
// ============================================================================

_FOG_NUM_T(PathStroker)
_FOG_NUM_T(PathStrokerParams)
_FOG_NUM_F(PathStroker)
_FOG_NUM_F(PathStrokerParams)
_FOG_NUM_D(PathStroker)
_FOG_NUM_D(PathStrokerParams)

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GEOMETRY_PATHSTROKER_H
