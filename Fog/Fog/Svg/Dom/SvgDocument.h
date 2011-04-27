// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGDOCUMENT_H
#define _FOG_SVG_DOM_SVGDOCUMENT_H

// [Dependencies]
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Tools/Dpi.h>
#include <Fog/Xml/Dom/XmlDocument.h>
#include <Fog/Svg/Dom/SvgObject.h>
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Painter;
struct SvgRenderContext;
struct SvgVisitor;

// ============================================================================
// [Fog::SvgDocument]
// ============================================================================

struct FOG_API SvgDocument :
  public XmlDocument,
  public SvgObject
{
  typedef XmlDocument base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgDocument();
  virtual ~SvgDocument();

  // --------------------------------------------------------------------------
  // [Clone]
  // --------------------------------------------------------------------------

  virtual XmlElement* clone() const;

  // --------------------------------------------------------------------------
  // [SVG Document Extensions]
  // --------------------------------------------------------------------------

  virtual XmlElement* createElement(const ManagedString& tagName);
  static XmlElement* createElementStatic(const ManagedString& tagName);

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRender(SvgRenderContext* context) const;

  // --------------------------------------------------------------------------
  // [SVG Public]
  // --------------------------------------------------------------------------

  FOG_INLINE float getDpi() const { return _dpi.getDpi(); }
  err_t setDpi(float dpi);

  SizeF getDocumentSize() const;
  err_t render(Painter* painter, SvgVisitor* visitor = NULL) const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Dpi _dpi;

private:
  FOG_DISABLE_COPY(SvgDocument)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGDOCUMENT_H
