// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.4
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------

// [Guard]
#ifndef _FOG_GRAPHICS_STROKER_H
#define _FOG_GRAPHICS_STROKER_H

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Geometry.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct DoubleMatrix;
struct DoublePath;
struct StrokerPrivate;

// ============================================================================
// [Fog::StrokeParams]
// ============================================================================

struct FOG_HIDDEN StrokeParams
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE StrokeParams() :
    _lineWidth(LINE_WIDTH_DEFAULT),
    _miterLimit(MITER_LIMIT_DEFAULT),
    _innerLimit(INNER_LIMIT_DEFAULT),
    _startCap(LINE_CAP_DEFAULT),
    _endCap(LINE_CAP_DEFAULT),
    _lineJoin(LINE_JOIN_DEFAULT),
    _innerJoin(INNER_JOIN_DEFAULT),
    _dashOffset(DASH_OFFSET_DEFAULT)
  {

  }

  FOG_INLINE StrokeParams(const StrokeParams& other) :
    _lineWidth(other._lineWidth),
    _miterLimit(other._miterLimit),
    _innerLimit(other._innerLimit),
    _params(other._params),
    _dashes(other._dashes),
    _dashOffset(other._dashOffset)
  {
  }

  FOG_INLINE ~StrokeParams()
  {
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE StrokeParams& operator=(const StrokeParams& other)
  {
    _lineWidth = other._lineWidth;
    _miterLimit = other._miterLimit;
    _innerLimit = other._innerLimit;
    _params = other._params;
    _dashes = other._dashes;
    _dashOffset = other._dashOffset;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _lineWidth = LINE_WIDTH_DEFAULT;
    _miterLimit = MITER_LIMIT_DEFAULT;
    _innerLimit = INNER_LIMIT_DEFAULT;
    _startCap = LINE_CAP_DEFAULT;
    _endCap = LINE_CAP_DEFAULT;
    _lineJoin = LINE_JOIN_DEFAULT;
    _innerJoin = INNER_JOIN_DEFAULT;
    _dashes.free();
    _dashOffset = DASH_OFFSET_DEFAULT;
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE double getLineWidth() const { return _lineWidth; }
  FOG_INLINE double getMiterLimit() const { return _miterLimit; }

  FOG_INLINE uint32_t getStartCap() const { return _startCap; }
  FOG_INLINE uint32_t getEndCap() const { return _endCap; }

  FOG_INLINE uint32_t getLineJoin() const { return _lineJoin; }
  FOG_INLINE uint32_t getInnerJoin() const { return _innerJoin; }

  FOG_INLINE const List<double>& getDashes() const { return _dashes; }
  FOG_INLINE double getDashOffset() const { return _dashOffset; }

  FOG_INLINE void setLineWidth(double lineWidth) { _lineWidth = lineWidth; }
  FOG_INLINE void setMiterLimit(double miterLimit) { _miterLimit = miterLimit; }
  FOG_INLINE void setInnerLimit(double innerLimit) { _innerLimit = innerLimit; }

  FOG_INLINE void setStartCap(uint32_t startCap) { _startCap = (uint8_t)startCap; }
  FOG_INLINE void setEndCap(uint32_t endCap) { _endCap = (uint8_t)endCap; }
  FOG_INLINE void setLineCaps(uint32_t lineCaps) { _startCap = _endCap = (uint8_t)lineCaps; }

  FOG_INLINE void setLineJoin(uint32_t lineJoin) { _lineJoin = (uint8_t)lineJoin; }
  FOG_INLINE void setInnerJoin(uint32_t innerJoin) { _innerJoin = (uint8_t)innerJoin; }

  FOG_INLINE void setDashes(const List<double>& dashes) { _dashes = dashes; }
  FOG_INLINE void setDashes(const double* dashes, sysuint_t length)
  {
    _dashes.clear();
    _dashes.append(dashes, length);
  }
  FOG_INLINE void setDashOffset(double dashOffset) { _dashOffset = dashOffset; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  double _lineWidth;
  double _miterLimit;
  double _innerLimit;

  union
  {
    struct
    {
      uint8_t _startCap;
      uint8_t _endCap;

      uint8_t _lineJoin;
      uint8_t _innerJoin;
    };
    uint32_t _params;
  };

  List<double> _dashes;
  double _dashOffset;
};

// ============================================================================
// [Fog::Stroker]
// ============================================================================

struct FOG_API Stroker
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Stroker();
  Stroker(const StrokeParams& params, double approximationScale = 1.0);
  ~Stroker();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const StrokeParams& getParams() const { return _params; }
  FOG_INLINE double getApproximationScale() const { return _approximationScale; }

  void setParams(const StrokeParams& params);
  void setApproximationScale(double approximationScale);

  // --------------------------------------------------------------------------
  // [Stroking]
  // --------------------------------------------------------------------------

  err_t stroke(DoublePath& dst) const;
  err_t stroke(DoublePath& dst, const DoublePath& src) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  void _update();

  StrokeParams _params;
  double _approximationScale;

  double _w;
  double _wAbs;
  double _wEps;
  double _da;
  int _wSign;

private:
  friend struct StrokerPrivate;

  FOG_DISABLE_COPY(Stroker)
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_STROKER_H
