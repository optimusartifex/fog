// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GEOMETRY_STROKER_H
#define _FOG_G2D_GEOMETRY_STROKER_H

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathEffect.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_G2d_Geometry
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct PathStrokerParamsF;
struct PathStrokerParamsD;

// ============================================================================
// [Helpers]
// ============================================================================

FOG_API err_t _ListFloatFromListDouble(List<float>& dst, const List<double>& src);
FOG_API err_t _ListDoubleFromListFloat(List<double>& dst, const List<float>& src);

// ============================================================================
// [Fog::PathStrokerHints]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
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
#include <Fog/Core/Pack/PackRestore.h>

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
  FOG_INLINE void setDashList(const float* dashList, size_t length) { _dashList.assign(dashList, length); }

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
  FOG_INLINE void setDashList(const double* dashList, size_t length) { _dashList.assign(dashList, length); }

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

struct FOG_API PathStrokerF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PathStrokerF();
  PathStrokerF(const PathStrokerParamsF& params);
  ~PathStrokerF();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathStrokerParamsF& getParams() const { return _params; }
  void setParams(const PathStrokerParamsF& params);

  FOG_INLINE const BoxF& getClipBox() const { return _clipBox; }
  FOG_INLINE bool isClippingEnabled() const { return _isClippingEnabled; }

  FOG_INLINE void setClipBox(const BoxF& clipBox)
  {
    _clipBox = clipBox;
    _isClippingEnabled = true;
    _isDirty = true;
  }

  FOG_INLINE void resetClipBox()
  {
    _clipBox.reset();
    _isClippingEnabled = false;
    _isDirty = true;
  }

  FOG_INLINE void setTransform(const TransformF& transform)
  {
    _transform = transform;
    _isDirty = true;
  }

  FOG_INLINE void resetTransform()
  {
    _transform.reset();
    _isDirty = true;
  }

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  void _update();
  FOG_INLINE void update() const { if (_isDirty) const_cast<PathStrokerF*>(this)->_update(); }

  // --------------------------------------------------------------------------
  // [Process]
  // --------------------------------------------------------------------------

  err_t strokeShape(PathF& dst, uint32_t shapeType, const void* shapeData,
    const TransformF* tr = NULL, const BoxF* clipBox = NULL) const;

  err_t strokePath(PathF& dst, const PathF& src,
    const TransformF* tr = NULL, const BoxF* clipBox = NULL) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PathStrokerParamsF _params;
  TransformF _transform;

  BoxF _clipBox;
  BoxF _transformedClipBox;

  bool _isDirty;
  bool _isClippingEnabled;
  bool _isComplexTransform;

  //! @brief Width / 2 after the simple transformation (if used).
  float _w;
  //! @brief Absolute value of _w.
  float _wAbs;
  float _wEps;
  float _da;
  float _flatness;
  int _wSign;

private:
  _FOG_CLASS_NO_COPY(PathStrokerF)
};

// ============================================================================
// [Fog::PathStrokerD]
// ============================================================================

struct FOG_API PathStrokerD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  PathStrokerD();
  PathStrokerD(const PathStrokerParamsD& params);
  ~PathStrokerD();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathStrokerParamsD& getParams() const { return _params; }
  void setParams(const PathStrokerParamsD& params);

  FOG_INLINE const BoxD& getClipBox() const { return _clipBox; }
  FOG_INLINE bool isClippingEnabled() const { return _isClippingEnabled; }

  FOG_INLINE void setClipBox(const BoxD& clipBox)
  {
    _clipBox = clipBox;
    _isClippingEnabled = true;
    _isDirty = true;
  }

  FOG_INLINE void resetClipBox()
  {
    _clipBox.reset();
    _isClippingEnabled = false;
    _isDirty = true;
  }

  FOG_INLINE void setTransform(const TransformD& transform)
  {
    _transform = transform;
    _isDirty = true;
  }

  FOG_INLINE void resetTransform()
  {
    _transform.reset();
    _isDirty = true;
  }

  // --------------------------------------------------------------------------
  // [Update]
  // --------------------------------------------------------------------------

  void _update();
  FOG_INLINE void update() const { if (_isDirty) const_cast<PathStrokerD*>(this)->_update(); }

  // --------------------------------------------------------------------------
  // [Process]
  // --------------------------------------------------------------------------

  err_t strokeShape(PathD& dst, uint32_t shapeType, const void* shapeData,
    const TransformD* tr = NULL, const BoxD* clipBox = NULL) const;

  err_t strokePath(PathD& dst, const PathD& src,
    const TransformD* tr = NULL, const BoxD* clipBox = NULL) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PathStrokerParamsD _params;
  TransformD _transform;

  BoxD _clipBox;
  BoxD _transformedClipBox;

  bool _isDirty;
  bool _isClippingEnabled;
  bool _isComplexTransform;

  //! @brief Width / 2 after the simple transformation (if used).
  double _w;
  //! @brief Absolute value of _w.
  double _wAbs;
  double _wEps;
  double _da;
  double _flatness;
  int _wSign;

private:
  _FOG_CLASS_NO_COPY(PathStrokerD)
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

  _ListFloatFromListDouble(_dashList, other._dashList);
  _hints.packed = other._hints.packed;

  return *this;
}

FOG_INLINE PathStrokerParamsD& PathStrokerParamsD::operator=(const PathStrokerParamsF& other)
{
  _lineWidth = other._lineWidth;
  _miterLimit = other._miterLimit;
  _dashOffset = other._dashOffset;

  _ListDoubleFromListFloat(_dashList, other._dashList);
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
// [Fog::PathStrokerParamsT]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(PathStrokerParams)

// ============================================================================
// [Fog::PathStrokerT<>]
// ============================================================================

FOG_TYPEVARIANT_DECLARE_F_D(PathStroker)

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::PathStrokerHints, Fog::TYPEINFO_PRIMITIVE)

_FOG_TYPEINFO_DECLARE(Fog::PathStrokerParamsF, Fog::TYPEINFO_MOVABLE)
_FOG_TYPEINFO_DECLARE(Fog::PathStrokerParamsD, Fog::TYPEINFO_MOVABLE)

_FOG_TYPEINFO_DECLARE(Fog::PathStrokerF, Fog::TYPEINFO_MOVABLE)
_FOG_TYPEINFO_DECLARE(Fog::PathStrokerD, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_GEOMETRY_STROKER_H
