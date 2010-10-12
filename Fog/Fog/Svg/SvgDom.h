// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_SVGDOM_H
#define _FOG_SVG_SVGDOM_H

// [Dependencies]
#include <Fog/Core/Hash.h>
#include <Fog/Core/TypeInfo.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Xml/XmlDom.h>
#include <Fog/Svg/SvgBase.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct SvgElement;
struct SvgContext;

// ============================================================================
// [Fog::ISvgObject]
// ============================================================================

struct FOG_API ISvgObject
{
  virtual err_t onRender(SvgContext* context) const = 0;
};

// ============================================================================
// [Fog::SvgElement]
// ============================================================================

struct FOG_API SvgElement :
  public XmlElement,
  public ISvgObject
{
  // [Construction / Destruction]

  typedef XmlElement base;

  SvgElement(const ManagedString& tagName, uint32_t svgType);
  virtual ~SvgElement();

  FOG_INLINE int getSvgType() const { return _svgType; }

  // [Clone]

  virtual SvgElement* clone() const;

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;
  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onApplyPattern(SvgContext* context, SvgElement* obj, int paintType) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  static err_t _walkAndRender(const XmlElement* root, SvgContext* context);

  // [SVG/CSS Styles]

  virtual String getStyle(const String& name) const;
  virtual err_t setStyle(const String& name, const String& value);

  // [SVG Implementation]

  FOG_INLINE uint32_t isBoundingRectDirty() const { return _boundingRectDirty; }

  const RectD& getBoundingRect() const;

protected:
  uint8_t _svgType;
public:
  mutable uint8_t _boundingRectDirty;
  uint16_t _unused;

protected:
  mutable RectD _boundingRect;

private:
  FOG_DISABLE_COPY(SvgElement)
};

// ============================================================================
// [Fog::SvgDocument]
// ============================================================================

struct FOG_API SvgDocument :
  public XmlDocument,
  public ISvgObject
{
  // [Construction / Destruction]

  typedef XmlDocument base;

  SvgDocument();
  virtual ~SvgDocument();

  // [Clone]

  virtual XmlElement* clone() const;

  // [Document Extensions]

  virtual XmlElement* createElement(const ManagedString& tagName);
  static XmlElement* createElementStatic(const ManagedString& tagName);

  // [Rendering]

  virtual err_t onRender(SvgContext* context) const;

private:
  FOG_DISABLE_COPY(SvgDocument)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_SVGDOM_H
