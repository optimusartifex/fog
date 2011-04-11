// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGELEMENT_H
#define _FOG_SVG_DOM_SVGELEMENT_H

// [Dependencies]
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/Xml/Dom/XmlElement.h>
#include <Fog/Svg/Dom/SvgObject.h>
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct SvgRenderContext;

// ============================================================================
// [Fog::SvgElement]
// ============================================================================

struct FOG_API SvgElement :
  public XmlElement,
  public SvgObject
{
  typedef XmlElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgElement(const ManagedString& tagName, uint32_t svgType);
  virtual ~SvgElement();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual SvgElement* clone() const;

  // --------------------------------------------------------------------------
  // [SVG Type]
  // --------------------------------------------------------------------------

  FOG_INLINE int getSvgType() const { return _svgType; }

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRender(SvgRenderContext* context) const;
  virtual err_t onRenderShape(SvgRenderContext* context) const;
  virtual err_t onApplyPattern(SvgRenderContext* context, SvgElement* obj, int paintType) const;
  virtual err_t onCalcBoundingBox(RectF* box) const;

  static err_t _walkAndRender(const XmlElement* root, SvgRenderContext* context);

  // --------------------------------------------------------------------------
  // [SVG Styles]
  // --------------------------------------------------------------------------

  virtual String getStyle(const String& name) const;
  virtual err_t setStyle(const String& name, const String& value);

  // --------------------------------------------------------------------------
  // [SVG Implementation]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t isBoundingRectDirty() const { return _boundingRectDirty; }

  const RectF& getBoundingRect() const;

  // --------------------------------------------------------------------------
  // [Visible]
  // --------------------------------------------------------------------------

  FOG_INLINE bool getVisible() const { return _visible; }
  FOG_INLINE void setVisible(bool value) { _visible = value; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t _svgType;
  mutable uint8_t _boundingRectDirty;

  uint8_t _visible;
  uint8_t _unused;

protected:
  mutable RectF _boundingRect;

private:
  FOG_DISABLE_COPY(SvgElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGELEMENT_H
