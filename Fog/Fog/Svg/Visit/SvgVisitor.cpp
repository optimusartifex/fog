// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Visit/SvgVisitor.h>
#include <Fog/Xml/Dom/XmlDocument.h>

namespace Fog {

// ============================================================================
// [Fog::SvgVisitor - Construction / Destruction]
// ============================================================================

SvgVisitor::SvgVisitor()
{
  _visitorType = SVG_VISITOR_MEASURE;
  _transformDirty = true;

  _strokeStyle.type = SVG_SOURCE_NONE;
  _fillStyle.type = SVG_SOURCE_COLOR;

  _fillRule = FILL_RULE_EVEN_ODD;
  _opacity = 1.0f;

  _textCursor.reset();
}

SvgVisitor::~SvgVisitor()
{
}

// ============================================================================
// [Fog::SvgVisitor - MoveTo]
// ============================================================================

err_t SvgVisitor::advance(SvgElement* obj)
{
  FOG_ASSERT(obj != NULL);

  List<SvgElement*> elements;
  XmlElement* doc = obj->getDocument();

  do {
    XmlElement* parent = obj->getParent();
    elements.append(obj);
    if (parent == doc) break;

    if (!parent->isSvgElement())
      return ERR_RT_INVALID_STATE;

    obj = reinterpret_cast<SvgElement*>(parent);
  } while (obj != NULL);

  sysuint_t i, len = elements.getLength();
  for (i = 0; i < len; i++)
  {
    obj = elements.at(i);
    FOG_RETURN_ON_ERROR(obj->onPrepare(this, NULL));
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgVisitor - Interface]
// ============================================================================

err_t SvgVisitor::onVisit(SvgElement* obj)
{
  SvgGState state(this);

  FOG_RETURN_ON_ERROR(obj->onPrepare(this, &state));
  FOG_RETURN_ON_ERROR(obj->onProcess(this));

  return ERR_OK;
}

} // Fog namespace
