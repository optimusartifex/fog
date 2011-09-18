// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGIMAGELINKATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGIMAGELINKATTRIBUTE_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Xml/XmlAttribute.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/G2d/Imaging/Image.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgImageLinkAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgImageLinkAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgImageLinkAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgImageLinkAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual StringW getValue() const;
  virtual err_t setValue(const StringW& value);

  // --------------------------------------------------------------------------
  // [Image]
  // --------------------------------------------------------------------------

  FOG_INLINE void setImage(const Image& image) { _image = image; }
  FOG_INLINE const Image& getImage() const { return _image; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
protected:

  Image _image;
  bool _embedded;

private:
  friend struct SvgImageElement;

  _FOG_NO_COPY(SvgImageLinkAttribute)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGIMAGELINKATTRIBUTE_P_H
