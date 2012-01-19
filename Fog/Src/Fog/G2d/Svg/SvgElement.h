// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SVG_SVGELEMENT_H
#define _FOG_G2D_SVG_SVGELEMENT_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

//! @addtogroup Fog_G2d_Svg
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct SvgGState;
struct SvgVisitor;

// ============================================================================
// [Fog::SvgElement]
// ============================================================================

struct FOG_API SvgElement : public XmlElement
{
  typedef XmlElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgElement(const ManagedStringW& tagName, uint32_t svgType);
  virtual ~SvgElement();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [SVG Attributes]
  // --------------------------------------------------------------------------

  virtual XmlAttribute* _createAttribute(const ManagedStringW& name) const;

  // --------------------------------------------------------------------------
  // [SVG Interface]
  // --------------------------------------------------------------------------

  virtual err_t onPrepare(SvgVisitor* visitor, SvgGState* state) const;
  virtual err_t onProcess(SvgVisitor* visitor) const;
  virtual err_t onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const;

  virtual err_t onGeometryBoundingBox(BoxF& box, const TransformF* tr) const;
  virtual err_t onStrokeBoundingBox(BoxF& box, const PathStrokerParamsF& stroke, const TransformF* tr) const;

  err_t _visitContainer(SvgVisitor* visitor) const;

  // --------------------------------------------------------------------------
  // [SVG Styles]
  // --------------------------------------------------------------------------

  virtual StringW getStyle(const StringW& name) const;
  virtual err_t setStyle(const StringW& name, const StringW& value);

  // --------------------------------------------------------------------------
  // [SVG Implementation]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t isBoundingBoxDirty() const { return _boundingBoxDirty; }

  err_t getBoundingBox(BoxF& box) const;
  err_t getBoundingBox(BoxF& box, const TransformF* tr) const;

  // --------------------------------------------------------------------------
  // [Visible]
  // --------------------------------------------------------------------------

  FOG_INLINE bool getVisible() const { return _visible; }
  FOG_INLINE void setVisible(bool value) { _visible = value; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable BoxF _boundingBox;

  mutable uint8_t _boundingBoxDirty;
  uint8_t _visible;
  uint8_t _unused_0;
  uint8_t _unused_1;

private:
  _FOG_NO_COPY(SvgElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SVG_SVGELEMENT_H
