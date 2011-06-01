// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_PAINTPARAMS_H
#define _FOG_G2D_PAINTING_PAINTPARAMS_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::PaintHints]
// ============================================================================

#include <Fog/Core/Pack/PackByte.h>
union FOG_NO_EXPORT PaintHints
{
  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    compositingOperator = COMPOSITE_SRC_OVER;
    renderQuality = RENDER_QUALITY_DEFAULT;
    imageQuality = IMAGE_QUALITY_DEFAULT;
    gradientQuality = GRADIENT_QUALITY_NORMAL;
    outlinedText = 0;
    geometricPrecision = GEOMETRIC_PRECISION_NORMAL;
    fillRule = FILL_RULE_DEFAULT;
    reserved = 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct
  {
    uint32_t compositingOperator : 8;
    uint32_t renderQuality : 4;
    uint32_t imageQuality : 4;
    uint32_t gradientQuality : 4;
    uint32_t outlinedText : 1;
    uint32_t fastLine : 1;
    uint32_t geometricPrecision : 1;
    uint32_t fillRule : 1;
    uint32_t reserved : 8;
  };

  uint32_t packed;
};
#include <Fog/Core/Pack/PackRestore.h>

// ============================================================================
// [Fog::PaintParamsF]
// ============================================================================

// TODO: What to do?
struct FOG_NO_EXPORT PaintParamsF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PaintParamsF()
  {
    _hints.reset();
    _opacity = 1.0f;
  }

  FOG_INLINE PaintParamsF(const PaintParamsF& other) :
    _opacity(other._opacity),
    _strokeParams(other._strokeParams)
  {
    _hints.packed = other._hints.packed;
  }

  // --------------------------------------------------------------------------
  // [Accessors - Global]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCompositingOperator() const { return _hints.compositingOperator; }
  FOG_INLINE void setCompositingOperator(uint32_t op) { _hints.compositingOperator = op; }

  FOG_INLINE uint32_t getRenderQuality() const { return _hints.renderQuality; }
  FOG_INLINE void setRenderQuality(uint32_t hint) { _hints.renderQuality = hint; }

  FOG_INLINE uint32_t getImageQuality() const { return _hints.imageQuality; }
  FOG_INLINE void setImageQuality(uint32_t hint) { _hints.imageQuality = hint; }

  FOG_INLINE uint32_t getGradientQuality() const { return _hints.gradientQuality; }
  FOG_INLINE void setGradientQuality(uint32_t hint) { _hints.gradientQuality = hint; }

  FOG_INLINE uint32_t getOutlinedText() const { return _hints.outlinedText; }
  FOG_INLINE void setOutlinedText(uint32_t hint) { _hints.outlinedText = hint; }

  FOG_INLINE uint32_t getGeometricPrecision() const { return _hints.geometricPrecision; }
  FOG_INLINE void setGeometricPrecision(uint32_t hint) { _hints.geometricPrecision = hint; }

  FOG_INLINE uint32_t getFillRule() const { return _hints.fillRule; }
  FOG_INLINE void setFillRule(uint32_t fillRule) { _hints.fillRule = fillRule; }

  FOG_INLINE float getOpacity() const { return _opacity; }
  FOG_INLINE void setOpacity(float opacity) { _opacity = opacity; }

  FOG_INLINE const PathStrokerParamsF& getStrokeParams() const { return _strokeParams; }
  FOG_INLINE void setStrokeParams(const PathStrokerParamsF& params) { _strokeParams = params; }

  // --------------------------------------------------------------------------
  // [Accessors - StrokeParams]
  // --------------------------------------------------------------------------

  FOG_INLINE float getLineWidth() const { return _strokeParams.getLineWidth(); }
  FOG_INLINE float getMiterLimit() const { return _strokeParams.getMiterLimit(); }
  FOG_INLINE float getDashOffset() const { return _strokeParams.getDashOffset(); }
  FOG_INLINE const List<float>& getDashList() const { return _strokeParams.getDashList(); }

  FOG_INLINE uint32_t getStartCap() const { return _strokeParams.getStartCap(); }
  FOG_INLINE uint32_t getEndCap() const { return _strokeParams.getEndCap(); }
  FOG_INLINE uint32_t getLineJoin() const { return _strokeParams.getLineJoin(); }

  FOG_INLINE void setLineWidth(float lineWidth) { _strokeParams.setLineWidth(lineWidth); }
  FOG_INLINE void setMiterLimit(float miterLimit) { _strokeParams.setMiterLimit(miterLimit); }
  FOG_INLINE void setDashOffset(float dashOffset) { _strokeParams.setDashOffset(dashOffset); }
  FOG_INLINE void setDashList(const List<float>& dashList) { _strokeParams.setDashList(dashList); }
  FOG_INLINE void setDashList(const float* dashList, sysuint_t length) { _strokeParams.setDashList(dashList, length); }

  FOG_INLINE void setStartCap(uint32_t startCap) { _strokeParams.setStartCap(startCap); }
  FOG_INLINE void setEndCap(uint32_t endCap) { _strokeParams.setEndCap(endCap); }
  FOG_INLINE void setLineCaps(uint32_t lineCaps) { _strokeParams.setLineCaps(lineCaps); }
  FOG_INLINE void setLineJoin(uint32_t lineJoin) { _strokeParams.setLineJoin(lineJoin); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _hints.reset();
    _opacity = 1.0f;
    _strokeParams.reset();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  PaintParamsF& operator=(const PaintParamsF& other)
  {
    _hints.packed = other._hints.packed;
    _opacity = other._opacity;
    _strokeParams = other._strokeParams;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PaintHints _hints;
  float _opacity;
  PathStrokerParamsF _strokeParams;
};

// ============================================================================
// [Fog::PaintParamsD]
// ============================================================================

struct FOG_NO_EXPORT PaintParamsD
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PaintParamsD()
  {
    _hints.reset();
    _opacity = 1.0;
  }

  FOG_INLINE PaintParamsD(const PaintParamsD& other) :
    _opacity(other._opacity),
    _strokeParams(other._strokeParams)
  {
    _hints.packed = other._hints.packed;
  }

  // --------------------------------------------------------------------------
  // [Accessors - Global]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCompositingOperator() const { return _hints.compositingOperator; }
  FOG_INLINE void setCompositingOperator(uint32_t op) { _hints.compositingOperator = op; }

  FOG_INLINE uint32_t getRenderQuality() const { return _hints.renderQuality; }
  FOG_INLINE void setRenderQuality(uint32_t hint) { _hints.renderQuality = hint; }

  FOG_INLINE uint32_t getImageQuality() const { return _hints.imageQuality; }
  FOG_INLINE void setImageQuality(uint32_t hint) { _hints.imageQuality = hint; }

  FOG_INLINE uint32_t getGradientQuality() const { return _hints.gradientQuality; }
  FOG_INLINE void setGradientQuality(uint32_t hint) { _hints.gradientQuality = hint; }

  FOG_INLINE uint32_t getOutlinedText() const { return _hints.outlinedText; }
  FOG_INLINE void setOutlinedText(uint32_t hint) { _hints.outlinedText = hint; }

  FOG_INLINE uint32_t getGeometricPrecision() const { return _hints.geometricPrecision; }
  FOG_INLINE void setGeometricPrecision(uint32_t hint) { _hints.geometricPrecision = hint; }

  FOG_INLINE uint32_t getFillRule() const { return _hints.fillRule; }
  FOG_INLINE void setFillRule(uint32_t fillRule) { _hints.fillRule = fillRule; }

  FOG_INLINE double getOpacity() const { return _opacity; }
  FOG_INLINE void setOpacity(double opacity) { _opacity = opacity; }

  FOG_INLINE const PathStrokerParamsD& getStrokeParams() const { return _strokeParams; }
  FOG_INLINE void setStrokeParams(const PathStrokerParamsD& params) { _strokeParams = params; }

  // --------------------------------------------------------------------------
  // [Accessors - StrokeParams]
  // --------------------------------------------------------------------------

  FOG_INLINE double getLineWidth() const { return _strokeParams.getLineWidth(); }
  FOG_INLINE double getMiterLimit() const { return _strokeParams.getMiterLimit(); }
  FOG_INLINE double getDashOffset() const { return _strokeParams.getDashOffset(); }
  FOG_INLINE const List<double>& getDashList() const { return _strokeParams.getDashList(); }

  FOG_INLINE uint32_t getStartCap() const { return _strokeParams.getStartCap(); }
  FOG_INLINE uint32_t getEndCap() const { return _strokeParams.getEndCap(); }
  FOG_INLINE uint32_t getLineJoin() const { return _strokeParams.getLineJoin(); }

  FOG_INLINE void setLineWidth(double lineWidth) { _strokeParams.setLineWidth(lineWidth); }
  FOG_INLINE void setMiterLimit(double miterLimit) { _strokeParams.setMiterLimit(miterLimit); }
  FOG_INLINE void setDashOffset(double dashOffset) { _strokeParams.setDashOffset(dashOffset); }
  FOG_INLINE void setDashList(const List<double>& dashList) { _strokeParams.setDashList(dashList); }
  FOG_INLINE void setDashList(const double* dashList, sysuint_t length) { _strokeParams.setDashList(dashList, length); }

  FOG_INLINE void setStartCap(uint32_t startCap) { _strokeParams.setStartCap(startCap); }
  FOG_INLINE void setEndCap(uint32_t endCap) { _strokeParams.setEndCap(endCap); }
  FOG_INLINE void setLineCaps(uint32_t lineCaps) { _strokeParams.setLineCaps(lineCaps); }
  FOG_INLINE void setLineJoin(uint32_t lineJoin) { _strokeParams.setLineJoin(lineJoin); }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    _hints.reset();
    _opacity = 1.0;
    _strokeParams.reset();
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  PaintParamsD& operator=(const PaintParamsD& other)
  {
    _hints.packed = other._hints.packed;
    _opacity = other._opacity;
    _strokeParams = other._strokeParams;

    return *this;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  PaintHints _hints;
  double _opacity;
  PathStrokerParamsD _strokeParams;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::PaintParamsF, Fog::TYPEINFO_MOVABLE)
_FOG_TYPEINFO_DECLARE(Fog::PaintParamsD, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_G2D_PAINTING_PAINTPARAMS_H
