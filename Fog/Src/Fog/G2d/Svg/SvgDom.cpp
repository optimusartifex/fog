// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/Property.h>
#include <Fog/Core/Tools/InternedString.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/G2d/Svg/SvgContext.h>
#include <Fog/G2d/Svg/SvgDom.h>
#include <Fog/G2d/Svg/SvgUtil.h>

namespace Fog {

// TODO: SVG - List of unimplemented elements:
//   - <clipPath>
//   - <desc>
//   - <marker>
//   - <mask>
//   - <textPath>
//   - <title>
//   - <tref>

// ============================================================================
// [Fog::SvgDom - Helpers]
// ============================================================================

static FOG_INLINE float svgGetCoord(const SvgDocument* ownerDocument, float value, uint32_t unit)
{
  return ownerDocument->_dpi.toDeviceSpace(value, unit);
}

static int svgGetEnum(const StringW& value, const PropertyEnum* pairs)
{
  while (pairs->name[0] != '\0')
  {
    if (value == Ascii8(pairs->name))
      break;
    pairs++;
  }

  return pairs->value;
}

static StubW parseHtmlLinkId(const StringW& url)
{
  const CharW* idStr;
  const CharW* idEnd;
  const CharW* idMark;
  CharW c;

  if (url.getLength() < 2)
    goto bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  if (*idStr != CharW('#'))
    goto bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd)
      goto bail;
  }

  idMark = idStr;
  while ((c = *idStr).isAsciiNumlet() || c == CharW('-') || c == CharW('_'))
  {
    if (++idStr == idEnd)
      break;
  }
  return StubW(idMark, (size_t)(idStr - idMark));

bail:
  return StubW((const CharW*)NULL, 0);
}

static StubW parseCssLinkId(const StringW& url)
{
  const CharW* idStr;
  const CharW* idEnd;
  const CharW* idMark;

  CharW quot;

  if (url.getLength() < 7) goto _Bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  // Add url(
  idStr += 4;

  // Detect quot, if used.
  quot = idStr[0];
  if (quot == CharW('\"') || quot == CharW('\''))
  {
    idStr++;
    idEnd--;
  }
  else
  {
    quot = CharW(')');
  }

  // Invalid ID.
  if (idStr + 1 >= idEnd || idStr[0] != CharW('#'))
    goto _Bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd)
      goto _Bail;
  }

  idMark = idStr;
  while (idStr[0] != quot)
  {
    if (++idStr == idEnd)
      goto _Bail;
  }
  return StubW(idMark, (size_t)(idStr - idMark));

_Bail:
  return StubW((const CharW*)NULL, 0);
}

// ============================================================================
// [Fog::SvgDom - Enumerations]
// ============================================================================

static const PropertyEnum svgEnum_compOp[] =
{
  { "src"           , COMPOSITE_SRC         },
  { "src-over"      , COMPOSITE_SRC_OVER    },
  { "src-in"        , COMPOSITE_SRC_IN      },
  { "src-out"       , COMPOSITE_SRC_OUT     },
  { "src-atop"      , COMPOSITE_SRC_ATOP    },
  { "dst"           , COMPOSITE_DST         },
  { "dst-over"      , COMPOSITE_DST_OVER    },
  { "dst-in"        , COMPOSITE_DST_IN      },
  { "dst-out"       , COMPOSITE_DST_OUT     },
  { "dst-atop"      , COMPOSITE_DST_ATOP    },
  { "xor"           , COMPOSITE_XOR         },
  { "clear"         , COMPOSITE_CLEAR       },
  { "plus"          , COMPOSITE_PLUS        },
  { "minus"         , COMPOSITE_MINUS       }, // Extension.
  { "multiply"      , COMPOSITE_MULTIPLY    },
  { "screen"        , COMPOSITE_SCREEN      },
  { "overlay"       , COMPOSITE_OVERLAY     },
  { "darken"        , COMPOSITE_DARKEN      },
  { "lighten"       , COMPOSITE_LIGHTEN     },
  { "color-dodge"   , COMPOSITE_COLOR_DODGE },
  { "color-burn"    , COMPOSITE_COLOR_BURN  },
  { "hard-light"    , COMPOSITE_HARD_LIGHT  },
  { "soft-light"    , COMPOSITE_SOFT_LIGHT  },
  { "difference"    , COMPOSITE_DIFFERENCE  },
  { "exclusion"     , COMPOSITE_EXCLUSION   },

  { "inherit"       , SVG_INHERIT           },
  { ""              , COMPOSITE_SRC_OVER    }
};

static const PropertyEnum svgEnum_fillRule[] =
{
  { "nonzero", FILL_RULE_NON_ZERO },
  { "evenodd", FILL_RULE_EVEN_ODD },
  { "", 0 }
};

static const PropertyEnum svgEnum_gradientUnits[3] =
{
  { "userSpaceOnUse", SVG_USER_SPACE_ON_USE },
  { "objectBoundingBox", SVG_OBJECT_BOUNDING_BOX },
  { "", 0 }
};

static const PropertyEnum svgEnum_lengthAdjust[3] =
{
  { "spacing", SVG_LENGTH_ADJUST_SPACING },
  { "spacingAndGlyphs", SVG_LENGTH_ADJUST_SPACING_AND_GLYPHS },
  { "", 0 }
};

static const PropertyEnum svgEnum_spreadMethod[4] =
{
  { "pad", GRADIENT_SPREAD_PAD },
  { "reflect", GRADIENT_SPREAD_REFLECT },
  { "repeat", GRADIENT_SPREAD_REPEAT },
  { "", 0 }
};

static const PropertyEnum svgEnum_strokeLineCap[] =
{
  { "butt", LINE_CAP_BUTT },
  { "round", LINE_CAP_ROUND },
  { "square", LINE_CAP_SQUARE },
  { "", 0 }
};

static const PropertyEnum svgEnum_strokeLineJoin[] =
{
  { "miter", LINE_JOIN_MITER },
  { "round", LINE_JOIN_ROUND },
  { "bevel", LINE_JOIN_BEVEL },
  { "", 0 }
};

// ============================================================================
// [Fog::SvgDom - PropertyIO]
// ============================================================================

struct FOG_NO_EXPORT SvgDomIO_OffsetF
{
  FOG_INLINE err_t parse(float& dst, const StringW& src) { return SvgUtil::parseOffset(dst, src); }
  FOG_INLINE err_t serialize(StringW& dst, const float& src) { return SvgUtil::serializeOffset(dst, src); }
};

struct FOG_NO_EXPORT SvgDomIO_OpacityF
{
  FOG_INLINE err_t parse(float& dst, const StringW& src) { return SvgUtil::parseOpacity(dst, src); }
  FOG_INLINE err_t serialize(StringW& dst, const float& src) { return SvgUtil::serializeOpacity(dst, src); }
};

struct FOG_NO_EXPORT SvgDomIO_CoordF
{
  FOG_INLINE err_t parse(CoordF& dst, const StringW& src) { return SvgUtil::parseCoord(dst, src); }
  FOG_INLINE err_t serialize(StringW& dst, const CoordF& src) { return SvgUtil::serializeCoord(dst, src); }
};

struct FOG_NO_EXPORT SvgDomIO_ViewBoxF
{
  FOG_INLINE err_t parse(BoxF& dst, const StringW& src) { return SvgUtil::parseViewBox(dst, src); }
  FOG_INLINE err_t serialize(StringW& dst, const BoxF& src) { return SvgUtil::serializeViewBox(dst, src); }
};

struct FOG_NO_EXPORT SvgDomIO_TransformF
{
  FOG_INLINE err_t parse(TransformF& dst, const StringW& src) { return SvgUtil::parseTransform(dst, src); }
  FOG_INLINE err_t serialize(StringW& dst, const TransformF& src) { return SvgUtil::serializeTransform(dst, src); }
};

struct FOG_NO_EXPORT SvgDomIO_PathF
{
  FOG_INLINE err_t parse(PathF& dst, const StringW& src) { return SvgUtil::parsePath(dst, src); }
  FOG_INLINE err_t serialize(StringW& dst, const PathF& src) { return SvgUtil::serializePath(dst, src); }
};

struct FOG_NO_EXPORT SvgDomIO_PointsF
{
  FOG_INLINE SvgDomIO_PointsF(bool closePath) : _closePath(closePath) {}

  FOG_INLINE err_t parse(PathF& dst, const StringW& src) { return SvgUtil::parsePoints(dst, src, _closePath); }
  FOG_INLINE err_t serialize(StringW& dst, const PathF& src) { return SvgUtil::serializePoints(dst, src); }

  bool _closePath;
};

struct FOG_NO_EXPORT SvgDomIO_Enum
{
  FOG_INLINE SvgDomIO_Enum(const PropertyEnum* pairs) : _pairs(pairs) {}

  FOG_INLINE err_t parse(uint32_t& dst, const StringW& src) { dst = svgGetEnum(src, _pairs); return ERR_OK; }
  FOG_INLINE err_t serialize(StringW& dst, const uint32_t& src) { return dst.append(Ascii8(_pairs[src].name)); }

  const PropertyEnum* _pairs;
};

// ============================================================================
// [Fog::SvgElement - Construction / Destruction]
// ============================================================================

SvgElement::SvgElement(
  DomDocument* ownerDocument,
  const InternedStringW& tagName,
  uint32_t svgType)
  :
  DomElement(ownerDocument, tagName),
  _computedBoundingBox(0.0f, 0.0f, 0.0f, 0.0f),
  _boundingBoxDirty(true),
  _visible(true),
  _unused_0(0),
  _unused_1(0)
{
  _nodeFlags |= DOM_NODE_FLAG_IS_SVG;
  _objectType = svgType;
  FOG_DOM_ELEMENT_INIT();
}

SvgElement::~SvgElement()
{
}

// ============================================================================
// [Fog::SvgElement - SVG Interface]
// ============================================================================

err_t SvgElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  return ERR_OK;
}

err_t SvgElement::onProcess(SvgContext* context) const
{
  // Should be reimplemented.
  return ERR_OK;
}

err_t SvgElement::onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const
{
  // Should be reimplemented if SvgElement is SvgPattern, SvgLinearGradient or
  // SvgRadialGradient.
  return ERR_RT_INVALID_STATE;
}

err_t SvgElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  return ERR_RT_INVALID_STATE;
}

err_t SvgElement::onStrokeBoundingBox(BoxF& box, const PathStrokerParamsF& stroke, const TransformF* tr) const
{
  // TODO: SVG - onStrokeBoundingBox.
  return ERR_RT_INVALID_STATE;
}

err_t SvgElement::_visitContainer(SvgContext* context) const
{
  err_t err = ERR_OK;
  DomNode* node;

  for (node = getFirstChild(); node != NULL; node = node->getNextSibling())
  {
    if (node->isSvgNode(DOM_NODE_TYPE_ELEMENT) &&
        static_cast<SvgElement*>(node)->getVisible())
    {
      err = context->onVisit(static_cast<SvgElement*>(node));
      if (FOG_IS_ERROR(err))
        break;
    }
  }

  return err;
}

err_t SvgElement::getBoundingBox(BoxF& box) const
{
  if (_boundingBoxDirty)
  {
    // TODO: SVG - BoundingBoxDirty.
    FOG_RETURN_ON_ERROR(onGeometryBoundingBox(_computedBoundingBox, NULL));
    _boundingBoxDirty = false;
  }

  box = _computedBoundingBox;
  return ERR_OK;
}

err_t SvgElement::getBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (tr == NULL) return getBoundingBox(box);

  switch (tr->getType())
  {
    case TRANSFORM_TYPE_IDENTITY:
      return getBoundingBox(box);

    case TRANSFORM_TYPE_TRANSLATION:
    case TRANSFORM_TYPE_SCALING:
    case TRANSFORM_TYPE_SWAP:
      FOG_RETURN_ON_ERROR(getBoundingBox(box));
      tr->mapBox(box, box);
      return ERR_OK;

    default:
      return onGeometryBoundingBox(box, tr);
  }
}

// ============================================================================
// [Fog::SvgStyleData - Construction / Destruction]
// ============================================================================

SvgStyleData::SvgStyleData() :
  clipRule(FILL_RULE_NON_ZERO),
  fillRule(FILL_RULE_NON_ZERO),
  fillSource(SVG_SOURCE_NONE),
  strokeSource(SVG_SOURCE_NONE),
  strokeLineCap(LINE_CAP_DEFAULT),
  strokeLineJoin(LINE_JOIN_DEFAULT),
  strokeDashOffsetUnit(UNIT_NONE),
  strokeMiterLimitUnit(UNIT_NONE),
  strokeWidthUnit(UNIT_NONE),
  fontSizeUnit(UNIT_NONE),
  compOp(COMPOSITE_SRC_OVER),
  letterSpacingUnit(UNIT_NONE),
  wordSpacingUnit(UNIT_NONE),
  unused(0),
  fillColor(0x00000000),
  strokeColor(0x00000000),
  stopColor(0x00000000),
  opacity(1.0f),
  fillOpacity(1.0f),
  strokeOpacity(1.0f),
  stopOpacity(1.0f),
  strokeDashOffsetValue(0.0f),
  strokeMiterLimitValue(4.0f),
  strokeWidthValue(1.0f),
  fontSizeValue(12.0f),
  letterSpacingValue(0.0f),
  clipPath(),
  strokeDashArray(),
  fontFamily(),
  fillUri(),
  strokeUri()
{
}

SvgStyleData::~SvgStyleData()
{
}

// ============================================================================
// [Fog::SvgStyle - Construction / Destruction]
// ============================================================================

SvgStyle::SvgStyle(SvgElement* ownerElement) :
  _styleMask(FOG_UINT64_C(0)),
  _ownerElement(ownerElement)
{
}

SvgStyle::~SvgStyle()
{
}

// ============================================================================
// [Fog::SvgStyle - AddRef / Release]
// ============================================================================

DomObj* SvgStyle::_addRef()
{
  _ownerElement->addRef();
  return this;
}

void SvgStyle::_release()
{
  _ownerElement->release();
}

// ============================================================================
// [Fog::SvgStyle - GC]
// ============================================================================

bool SvgStyle::_canCollect() const
{
  return false;
}

// ============================================================================
// [Fog::SvgStyle - Properties]
// ============================================================================

static const uint32_t SvgStyle_nameToIdData[] =
{
  STR_font,
  STR_font_family,
  STR_font_size,
  STR_direction,
  STR_letter_spacing,
  STR_text_decoration,
  STR_word_spacing,
  STR_color,
  STR_clip_path,
  STR_clip_rule,
  STR_mask,
  STR_opacity,
  STR_enable_background,
  STR_filter,
  STR_flood_color,
  STR_flood_opacity,
  STR_lighting_color,
  STR_stop_color,
  STR_stop_opacity,
  STR_comp_op,
  STR_fill,
  STR_fill_opacity,
  STR_fill_rule,
  STR_image_rendering,
  STR_marker,
  STR_marker_end,
  STR_marker_mid,
  STR_marker_start,
  STR_shape_rendering,
  STR_stroke,
  STR_stroke_dasharray,
  STR_stroke_dashoffset,
  STR_stroke_linecap,
  STR_stroke_linejoin,
  STR_stroke_miterlimit,
  STR_stroke_opacity,
  STR_stroke_width,
  STR_text_rendering
};

size_t SvgStyle::_getPropertyIndex(const InternedStringW& name) const
{
  for (size_t i = 0; i < FOG_ARRAY_SIZE(SvgStyle_nameToIdData); i++)
  {
    if (InternedStringCacheW::get()->getString(SvgStyle_nameToIdData[i]).eq(name))
      return i;
  }

  return INVALID_INDEX;
}

size_t SvgStyle::_getPropertyIndex(const CharW* name, size_t length) const
{
  for (size_t i = 0; i < FOG_ARRAY_SIZE(SvgStyle_nameToIdData); i++)
  {
    if (InternedStringCacheW::get()->getString(SvgStyle_nameToIdData[i]).eqInline(name, length))
      return i;
  }
  
  return INVALID_INDEX;
}

err_t SvgStyle::_getPropertyInfo(size_t index, PropertyInfo& info) const
{
  if (index >= FOG_ARRAY_SIZE(SvgStyle_nameToIdData))
    return ERR_OBJ_PROPERTY_NOT_FOUND;

  info.setName(InternedStringCacheW::get()->getString(SvgStyle_nameToIdData[index]));
  info.setIndex(index);
  info.setType(0);
  info.setFlags(NO_FLAGS);

  return ERR_OK;
}

err_t SvgStyle::_getProperty(size_t index, StringW& value) const
{
  if (index >= FOG_ARRAY_SIZE(SvgStyle_nameToIdData))
    return ERR_OBJ_PROPERTY_NOT_FOUND;

  // Don't process non-used style values.
  if ((_styleMask & ((uint64_t)1 << index)) == 0)
    goto _End;

  switch (index)
  {
    // ------------------------------------------------------------------------
    // [Font Properties]
    // ------------------------------------------------------------------------

    // TODO: SVG - Svg font.
    case SVG_STYLE_FONT:
      break;

    case SVG_STYLE_FONT_FAMILY:
      value.set(_d.fontFamily);
      break;

    case SVG_STYLE_FONT_SIZE:
      SvgUtil::serializeCoord(value, getFontSize());
      break;

    // ------------------------------------------------------------------------
    // [Text Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_LETTER_SPACING:
      SvgUtil::serializeCoord(value, getLetterSpacing());
      break;

    // ------------------------------------------------------------------------
    // [Other Properties for Visual Media]
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // [Clipping, Masking, and Compositing Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_CLIP_PATH:
      // TODO: SVG - Clip path. 
      break;

    case SVG_STYLE_CLIP_RULE:
      FOG_ASSERT(_d.clipRule < FILL_RULE_COUNT);
      value.set(Ascii8(svgEnum_fillRule[_d.clipRule].name));
      break;

    case SVG_STYLE_MASK:
      // TODO: SVG - Mask.
      break;

    case SVG_STYLE_OPACITY:
      value.setReal(_d.opacity);
      break;

    // ------------------------------------------------------------------------
    // [Filter Effects Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_ENABLE_BACKGROUND:
      // TODO: SVG - Enable background.
      break;

    case SVG_STYLE_FILTER:
      // TODO: SVG - Filter.
      break;

    // ------------------------------------------------------------------------
    // [Gradient Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_STOP_COLOR:
      SvgUtil::serializeColor(value, _d.stopColor);
      break;

    case SVG_STYLE_STOP_OPACITY:
      value.setReal(_d.stopOpacity);
      break;

    // ------------------------------------------------------------------------
    // [Color and Painting Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_COMP_OP:
      if (_d.compOp == SVG_INHERIT)
        value.append(Ascii8("inherit"));
      else
        value.append(Ascii8(svgEnum_compOp[_d.compOp].name));
      break;

    case SVG_STYLE_FILL:
      switch (_d.fillSource)
      {
        case SVG_SOURCE_NONE:
          value.append(Ascii8("none"));
          break;
        case SVG_SOURCE_COLOR:
          SvgUtil::serializeColor(value, _d.fillColor);
          break;
        case SVG_SOURCE_URI:
          value = _d.fillUri;
          break;
      }
      break;

    case SVG_STYLE_FILL_OPACITY:
      value.setReal(_d.fillOpacity);
      break;

    case SVG_STYLE_FILL_RULE:
      FOG_ASSERT(_d.fillRule < FILL_RULE_COUNT);
      value.set(Ascii8(svgEnum_fillRule[_d.fillRule].name));
      break;

    case SVG_STYLE_STROKE:
      switch (_d.strokeSource)
      {
        case SVG_SOURCE_NONE:
          value.append(Ascii8("none"));
          break;
        case SVG_SOURCE_COLOR:
          SvgUtil::serializeColor(value, _d.strokeColor);
          break;
        case SVG_SOURCE_URI:
          value = _d.strokeUri;
          break;
      }
      break;

    case SVG_STYLE_STROKE_DASH_ARRAY:
      // TODO: SVG - DashArray.
      break;

    case SVG_STYLE_STROKE_DASH_OFFSET:
      SvgUtil::serializeCoord(value, getStrokeDashOffset());
      break;

    case SVG_STYLE_STROKE_LINE_CAP:
      FOG_ASSERT(_d.strokeLineCap < LINE_CAP_COUNT);
      value.set(Ascii8(svgEnum_strokeLineCap[_d.strokeLineCap].name));
      break;

    case SVG_STYLE_STROKE_LINE_JOIN:
      FOG_ASSERT(_d.strokeLineJoin < LINE_JOIN_COUNT);
      value.set(Ascii8(svgEnum_strokeLineJoin[_d.strokeLineJoin].name));
      break;

    case SVG_STYLE_STROKE_MITER_LIMIT:
      SvgUtil::serializeCoord(value, getStrokeMiterLimit());
      break;

    case SVG_STYLE_STROKE_OPACITY:
      value.setReal(_d.strokeOpacity);
      break;

    case SVG_STYLE_STROKE_WIDTH:
      SvgUtil::serializeCoord(value, getStrokeWidth());
      break;

    default:
      return ERR_RT_NOT_IMPLEMENTED;
  }

_End:
  return ERR_OK;
}

err_t SvgStyle::_setProperty(size_t index, const StringW& value)
{
  if (index >= FOG_ARRAY_SIZE(SvgStyle_nameToIdData))
    return ERR_OBJ_PROPERTY_NOT_FOUND;

  err_t err = ERR_OK;
  int i;

  if (value.isEmpty())
    return _resetProperty(index);

  switch (index)
  {
    // ------------------------------------------------------------------------
    // [Font Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_FONT_FAMILY:
    {
      err = _d.fontFamily.set(value);
      break;
    }

    case SVG_STYLE_FONT_SIZE:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;

      setFontSize(coord);
      break;
    }

    // ------------------------------------------------------------------------
    // [Text Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_LETTER_SPACING:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setLetterSpacing(coord);
      break;
    }

    // ------------------------------------------------------------------------
    // [Other Properties for Visual Media]
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // [Clipping, Masking, and Compositing Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_CLIP_PATH:
    {
      // TODO: SVG - ClipPath.
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_CLIP_RULE:
    {
      i = svgGetEnum(value, svgEnum_fillRule);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _d.clipRule = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_MASK:
    {
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_OPACITY:
    {
      err = SvgUtil::parseOpacity(_d.opacity, value);
      break;
    }

    // ------------------------------------------------------------------------
    // [Filter Effects Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_ENABLE_BACKGROUND:
    {
      // TODO: SVG - EnableBackground.
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_FILTER:
    {
      // TODO: SVG - Filter.
      err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    // ------------------------------------------------------------------------
    // [Gradient Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_STOP_COLOR:
    {
      if (SvgUtil::parseColor(_d.stopColor, value) != SVG_SOURCE_COLOR)
      {
        err = ERR_SVG_INVALID_STYLE_VALUE;
      }
      break;
    }

    case SVG_STYLE_STOP_OPACITY:
    {
      err = SvgUtil::parseOpacity(_d.stopOpacity, value);
      break;
    }

    // ------------------------------------------------------------------------
    // [Color and Painting Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_COMP_OP:
      i = svgGetEnum(value, svgEnum_compOp);
      _d.compOp = i;
      break;

    case SVG_STYLE_FILL:
    {
      _d.fillSource = (uint8_t)SvgUtil::parseColor(_d.fillColor, value);
      switch (_d.fillSource)
      {
        case SVG_SOURCE_NONE:
        case SVG_SOURCE_COLOR:
          break;
        case SVG_SOURCE_URI:
          _d.fillUri = value;
          break;
        case SVG_SOURCE_INVALID:
          err = ERR_SVG_INVALID_STYLE_VALUE;
          break;
      }
      break;
    }

    case SVG_STYLE_FILL_OPACITY:
    {
      err = SvgUtil::parseOpacity(_d.fillOpacity, value);
      break;
    }

    case SVG_STYLE_FILL_RULE:
    {
      i = svgGetEnum(value, svgEnum_fillRule);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _d.fillRule = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_STROKE:
    {
      _d.strokeSource = (uint8_t)SvgUtil::parseColor(_d.strokeColor, value);
      switch (_d.strokeSource)
      {
        case SVG_SOURCE_NONE:
        case SVG_SOURCE_COLOR:
          break;
        case SVG_SOURCE_URI:
          _d.strokeUri = value;
          break;
        case SVG_SOURCE_INVALID:
          err = ERR_SVG_INVALID_STYLE_VALUE;
          break;
      }
      break;
    }

    case SVG_STYLE_STROKE_DASH_ARRAY:
    {
      //err = ERR_RT_NOT_IMPLEMENTED;
      break;
    }

    case SVG_STYLE_STROKE_DASH_OFFSET:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setStrokeDashOffset(coord);
      break;
    }

    case SVG_STYLE_STROKE_LINE_CAP:
    {
      i = svgGetEnum(value, svgEnum_strokeLineCap);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _d.strokeLineCap = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_STROKE_LINE_JOIN:
    {
      i = svgGetEnum(value, svgEnum_strokeLineJoin);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _d.strokeLineJoin = (uint8_t)(uint)i;
      break;
    }

    case SVG_STYLE_STROKE_MITER_LIMIT:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setStrokeMiterLimit(coord);
      break;
    }

    case SVG_STYLE_STROKE_OPACITY:
    {
      err = SvgUtil::parseOpacity(_d.strokeOpacity, value);
      break;
    }

    case SVG_STYLE_STROKE_WIDTH:
    {
      CoordF coord(UNINITIALIZED);
      if (SvgUtil::parseCoord(coord, value) != ERR_OK)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      setStrokeWidth(coord);
      break;
    }

    default:
      // return ERR_RT_NOT_IMPLEMENTED;
      break;
  }

  if (err == ERR_OK)
    _styleMask |= ((uint64_t)1 << index);
  else
    _resetProperty(index);

  return err;
}

err_t SvgStyle::_resetProperty(size_t index)
{
  if (index >= FOG_ARRAY_SIZE(SvgStyle_nameToIdData))
    return ERR_OBJ_PROPERTY_NOT_FOUND;

  switch (index)
  {
    // ------------------------------------------------------------------------
    // [Font Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_FONT_FAMILY:
      _d.fontFamily.reset();
      break;

    case SVG_STYLE_FONT_SIZE:
      _d.fontSizeValue = 0.0f;
      _d.fontSizeUnit = UNIT_NONE;
      break;

    // ------------------------------------------------------------------------
    // [Text Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_LETTER_SPACING:
      _d.letterSpacingValue = 0.0f;
      _d.letterSpacingUnit = UNIT_NONE;
      break;

    // ------------------------------------------------------------------------
    // [Other Properties for Visual Media]
    // ------------------------------------------------------------------------

    // ------------------------------------------------------------------------
    // [Clipping, Masking, and Compositing Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_CLIP_PATH:
      break;

    case SVG_STYLE_CLIP_RULE:
      _d.clipRule = FILL_RULE_NON_ZERO;
      break;

    case SVG_STYLE_MASK:
      break;

    case SVG_STYLE_OPACITY:
      _d.opacity = 1.0f;
      break;

    // ------------------------------------------------------------------------
    // [Filter Effects Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_ENABLE_BACKGROUND:
      break;

    case SVG_STYLE_FILTER:
      break;

    // ------------------------------------------------------------------------
    // [Gradient Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_STOP_COLOR:
      _d.stopColor.reset();
      break;

    case SVG_STYLE_STOP_OPACITY:
      _d.stopOpacity = 1.0f;
      break;

    // ------------------------------------------------------------------------
    // [Color and Painting Properties]
    // ------------------------------------------------------------------------

    case SVG_STYLE_COMP_OP:
      _d.compOp = COMPOSITE_SRC_OVER;
      break;

    case SVG_STYLE_FILL:
      _d.fillSource = SVG_SOURCE_NONE;
      _d.fillUri.reset();
      break;

    case SVG_STYLE_FILL_OPACITY:
      _d.fillOpacity = 1.0f;
      break;

    case SVG_STYLE_FILL_RULE:
      _d.fillRule = FILL_RULE_NON_ZERO;
      break;

    case SVG_STYLE_STROKE:
      _d.strokeSource = SVG_SOURCE_NONE;
      _d.strokeUri.reset();
      break;

    case SVG_STYLE_STROKE_DASH_ARRAY:
      break;

    case SVG_STYLE_STROKE_DASH_OFFSET:
      _d.strokeDashOffsetValue = 0.0f;
      _d.strokeDashOffsetUnit = UNIT_NONE;
      break;

    case SVG_STYLE_STROKE_LINE_CAP:
      _d.strokeLineCap = LINE_CAP_DEFAULT;
      break;

    case SVG_STYLE_STROKE_LINE_JOIN:
      _d.strokeLineJoin = LINE_JOIN_DEFAULT;
      break;

    case SVG_STYLE_STROKE_MITER_LIMIT:
      _d.strokeMiterLimitValue = 4.0f;
      _d.strokeMiterLimitUnit = UNIT_NONE;
      break;

    case SVG_STYLE_STROKE_OPACITY:
      _d.strokeOpacity = 1.0f;
      break;

    case SVG_STYLE_STROKE_WIDTH:
      _d.strokeWidthValue = 1.0f;
      _d.strokeWidthUnit = UNIT_NONE;
      break;

    default:
      break;
  }

  _styleMask &= ~(1 << index);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgStyle - Style]
// ============================================================================

static err_t FOG_CDECL SvgStyle_parserFunc(void* ctx, const StringW* styleName, const StringW* styleValue)
{
  return static_cast<SvgStyle*>(ctx)->setProperty(*styleName, *styleValue);
}

err_t SvgStyle::getStyle(StringW& value) const
{
  // TODO: SVG - Posibility for overlap - "font", "font-size", ...
  StringW data;

  for (size_t i = 0; i < FOG_ARRAY_SIZE(SvgStyle_nameToIdData); i++)
  {
    if (_styleMask & ((uint64_t)1 << i))
    {
      data.clear();
      getProperty(i, data);

      value.append(InternedStringCacheW::get()->getString(SvgStyle_nameToIdData[i]));
      value.append(CharW(':'));
      value.append(data);
      value.append(CharW(';'));
    }
  }

  return ERR_OK;
}

err_t SvgStyle::setStyle(const StringW& value)
{
  resetStyle();
  return SvgUtil::parseCSSStyle(value, SvgStyle_parserFunc, this);
}

err_t SvgStyle::resetStyle()
{
  _styleMask = 0;

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgStylableElement - Construction / Destruction]
// ============================================================================

SvgStylableElement::SvgStylableElement(DomDocument* ownerDocument, const InternedStringW& tagName, uint32_t svgType) :
  SvgElement(ownerDocument, tagName, svgType),
  _style(this)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgStylableElement::~SvgStylableElement()
{
}

// ============================================================================
// [Fog::SvgStylableElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgStylableElement)
  // "style" property.
  FOG_CORE_OBJ_PROPERTY_ACCESS(Style, FOG_S(style), _style.getStyle, _style.setStyle, _style.resetStyle)

  // "style" redirect.
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Font            , FOG_S(font             ), &_style, SVG_STYLE_FONT              )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(FontFamily      , FOG_S(font_family      ), &_style, SVG_STYLE_FONT_FAMILY       )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(FontSize        , FOG_S(font_size        ), &_style, SVG_STYLE_FONT_SIZE         )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Direction       , FOG_S(direction        ), &_style, SVG_STYLE_DIRECTION         )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(LetterSpacing   , FOG_S(letter_spacing   ), &_style, SVG_STYLE_LETTER_SPACING    )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(TextDecoration  , FOG_S(text_decoration  ), &_style, SVG_STYLE_TEXT_DECORATION   )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(WordSpacing     , FOG_S(word_spacing     ), &_style, SVG_STYLE_WORD_SPACING      )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Color           , FOG_S(color            ), &_style, SVG_STYLE_COLOR             )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(ClipPath        , FOG_S(clip_path        ), &_style, SVG_STYLE_CLIP_PATH         )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(ClipRule        , FOG_S(clip_rule        ), &_style, SVG_STYLE_CLIP_RULE         )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Mask            , FOG_S(mask             ), &_style, SVG_STYLE_MASK              )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Opacity         , FOG_S(opacity          ), &_style, SVG_STYLE_OPACITY           )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(EnableBackground, FOG_S(enable_background), &_style, SVG_STYLE_ENABLE_BACKGROUND )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Filter          , FOG_S(filter           ), &_style, SVG_STYLE_FILTER            )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(FloodColor      , FOG_S(flood_color      ), &_style, SVG_STYLE_FLOOD_COLOR       )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(FloodOpacity    , FOG_S(flood_opacity    ), &_style, SVG_STYLE_FLOOD_OPACITY     )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(LightingColor   , FOG_S(lighting_color   ), &_style, SVG_STYLE_LIGHTING_COLOR    )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StopColor       , FOG_S(stop_color       ), &_style, SVG_STYLE_STOP_COLOR        )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StopOpacity     , FOG_S(stop_opacity     ), &_style, SVG_STYLE_STOP_OPACITY      )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(CompOp          , FOG_S(comp_op          ), &_style, SVG_STYLE_COMP_OP           )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Fill            , FOG_S(fill             ), &_style, SVG_STYLE_FILL              )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(FillOpacity     , FOG_S(fill_opacity     ), &_style, SVG_STYLE_FILL_OPACITY      )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(FillRule        , FOG_S(fill_rule        ), &_style, SVG_STYLE_FILL_RULE         )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(ImageRendering  , FOG_S(image_rendering  ), &_style, SVG_STYLE_IMAGE_RENDERING   )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Marker          , FOG_S(marker           ), &_style, SVG_STYLE_MARKER            )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(MarkerEnd       , FOG_S(marker_end       ), &_style, SVG_STYLE_MARKER_END        )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(MarkerMid       , FOG_S(marker_mid       ), &_style, SVG_STYLE_MARKER_MID        )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(MarkerStart     , FOG_S(marker_start     ), &_style, SVG_STYLE_MARKER_START      )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(ShapeRendering  , FOG_S(shape_rendering  ), &_style, SVG_STYLE_SHAPE_RENDERING   )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(Stroke          , FOG_S(stroke           ), &_style, SVG_STYLE_STROKE            )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StrokeDashArray , FOG_S(stroke_dasharray ), &_style, SVG_STYLE_STROKE_DASH_ARRAY )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StrokeDashOffset, FOG_S(stroke_dashoffset), &_style, SVG_STYLE_STROKE_DASH_OFFSET)
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StrokeLineCap   , FOG_S(stroke_linecap   ), &_style, SVG_STYLE_STROKE_LINE_CAP   )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StrokeLineJoin  , FOG_S(stroke_linejoin  ), &_style, SVG_STYLE_STROKE_LINE_JOIN  )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StrokeMiterLimit, FOG_S(stroke_miterlimit), &_style, SVG_STYLE_STROKE_MITER_LIMIT)
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StrokeOpacity   , FOG_S(stroke_opacity   ), &_style, SVG_STYLE_STROKE_OPACITY    )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(StrokeWidth     , FOG_S(stroke_width     ), &_style, SVG_STYLE_STROKE_WIDTH      )
  FOG_CORE_OBJ_PROPERTY_REDIRECT(TextRendering   , FOG_S(text_rendering   ), &_style, SVG_STYLE_TEXT_RENDERING    )
FOG_CORE_OBJ_END()

// ============================================================================
// [Fog::SvgStylableElement - SVG Interface]
// ============================================================================

err_t SvgStylableElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  // Apply transformations and setup styles defined by this element.
  uint64_t styleMask = _style.getStyleMask();

  if (styleMask != 0)
  {
    SvgDocument* doc = reinterpret_cast<SvgDocument*>(getOwnerDocument());

    // Comp-op.
    if (styleMask & (((uint64_t)1 << SVG_STYLE_COMP_OP)))
    {
      if (_style._d.compOp != SVG_INHERIT)
        context->setCompOp(_style._d.compOp);
    }
    else
    {
      // Non-inheritable value.
      context->setCompOp(COMPOSITE_SRC_OVER);
    }

    // Opacity.
    if (styleMask & (((uint64_t)1 << SVG_STYLE_OPACITY)))
    {
      context->setOpacity(_style._d.opacity);
    }

    // Setup font parameters.
    if (styleMask & (((uint64_t)1 << SVG_STYLE_FONT_FAMILY) |
                     ((uint64_t)1 << SVG_STYLE_FONT_SIZE  )))
    {
      if (state)
        state->saveFont();

      StringW family = context->_font.getFamily();
      float size = context->_font.getHeight();

      if (styleMask & (1 << SVG_STYLE_FONT_FAMILY))
      {
        family = _style._d.fontFamily;
      }

      if (styleMask & (1 << SVG_STYLE_FONT_SIZE))
      {
        size = doc->_dpi.toDeviceSpace(
          _style._d.fontSizeValue, _style._d.fontSizeUnit);
      }

      context->_font.create(family, size, UNIT_NONE);
    }

    // Setup fill parameters.
    if (styleMask & (((uint64_t)1 << SVG_STYLE_FILL               ) |
                     ((uint64_t)1 << SVG_STYLE_FILL_OPACITY       ) |
                     ((uint64_t)1 << SVG_STYLE_FILL_RULE          )))
    {
      if (state)
        state->saveFill();

      if (styleMask & (1 << SVG_STYLE_FILL))
      {
        switch (_style._d.fillSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            context->setFillNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            context->setFillColor(_style._d.fillColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            DomElement* uriRef = getOwnerDocument()->getElementById(
              parseCssLinkId(_style._d.fillUri));

            if (uriRef != NULL && uriRef->isSvgNode(DOM_NODE_TYPE_ELEMENT))
              context->setFillPattern(static_cast<SvgElement*>(uriRef));
            else
              context->setFillNone();
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_FILL_OPACITY))
      {
        context->setFillOpacity(_style._d.fillOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_FILL_RULE))
      {
        context->setFillRule(_style._d.fillRule);
      }
    }

    // Setup stroke parameters.
    if (styleMask & (((uint64_t)1 << SVG_STYLE_STROKE             ) |
                     ((uint64_t)1 << SVG_STYLE_STROKE_DASH_ARRAY  ) |
                     ((uint64_t)1 << SVG_STYLE_STROKE_DASH_OFFSET ) |
                     ((uint64_t)1 << SVG_STYLE_STROKE_LINE_CAP    ) |
                     ((uint64_t)1 << SVG_STYLE_STROKE_LINE_JOIN   ) |
                     ((uint64_t)1 << SVG_STYLE_STROKE_MITER_LIMIT ) |
                     ((uint64_t)1 << SVG_STYLE_STROKE_OPACITY     ) |
                     ((uint64_t)1 << SVG_STYLE_STROKE_WIDTH       )))
    {
      if (state)
        state->saveStroke();

      if (styleMask & (1 << SVG_STYLE_STROKE))
      {
        switch (_style._d.strokeSource)
        {
          case SVG_SOURCE_NONE:
          case SVG_SOURCE_INVALID:
          {
            context->setStrokeNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            context->setStrokeColor(_style._d.strokeColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            DomElement* uriRef = getOwnerDocument()->getElementById(
              parseCssLinkId(_style._d.strokeUri));

            if (uriRef != NULL && uriRef->isSvgNode(DOM_NODE_TYPE_ELEMENT))
              context->setStrokePattern(static_cast<SvgElement*>(uriRef));
            else
              context->setStrokeNone();
            break;
          }
        }
      }

      if (styleMask & ((uint64_t)1 << SVG_STYLE_STROKE_LINE_CAP))
      {
        context->setLineCaps(_style._d.strokeLineCap);
      }

      if (styleMask & ((uint64_t)1 << SVG_STYLE_STROKE_LINE_JOIN))
      {
        context->setLineJoin(_style._d.strokeLineJoin);
      }

      if (styleMask & ((uint64_t)1 << SVG_STYLE_STROKE_MITER_LIMIT))
      {
        float miterLimit = doc->_dpi.toDeviceSpace(
          _style._d.strokeMiterLimitValue, _style._d.strokeMiterLimitUnit);
        context->setMiterLimit(miterLimit);
      }

      if (styleMask & ((uint64_t)1 << SVG_STYLE_STROKE_OPACITY))
      {
        context->setStrokeOpacity(_style._d.strokeOpacity);
      }

      if (styleMask & ((uint64_t)1 << SVG_STYLE_STROKE_WIDTH))
      {
        float lineWidth = doc->_dpi.toDeviceSpace(
          _style._d.strokeWidthValue, _style._d.strokeWidthUnit);
        context->setLineWidth(lineWidth);
      }
    }
  }
  else
  {
    // Set compositing operator to src-over (default, non-ihneritable).
    context->setCompOp(COMPOSITE_SRC_OVER);
    // Set opacity 1.0 (default, non-ihneritable).
    context->setOpacity(1.0f);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgTransformableElement]
// ============================================================================

SvgTransformableElement::SvgTransformableElement(DomDocument* ownerDocument,
  const InternedStringW& tagName, uint32_t svgType)
  :
  SvgStylableElement(ownerDocument, tagName, svgType)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgTransformableElement::~SvgTransformableElement()
{
}

// ============================================================================
// [Fog::SvgTransformableElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgTransformableElement)
  FOG_CORE_OBJ_PROPERTY_BASE(Transform, FOG_S(transform), SvgDomIO_TransformF())
FOG_CORE_OBJ_END()

err_t SvgTransformableElement::setTransform(const TransformF& transform)
{
  FOG_DOM_ELEMENT_INIT();

  _transform = transform;
  _setDirty();

  return ERR_OK;
}

err_t SvgTransformableElement::resetTransform()
{
  _transform.reset();
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgTransformableElement - SVG Interface]
// ============================================================================

err_t SvgTransformableElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  if (!_transform.isIdentity())
  {
    if (state && !state->hasState(SvgContextGState::SAVED_TRANSFORM))
      state->saveTransform();
    context->transform(_transform);
  }

  return Base::onPrepare(context, state);
}

// ============================================================================
// [Fog::SvgRootElement - Construction / Destruction]
// ============================================================================

SvgRootElement::SvgRootElement(DomDocument* ownerDocument) :
  SvgStylableElement(ownerDocument, FOG_S(svg), SVG_ELEMENT_SVG),
  _viewBox(0.0f, 0.0f, 0.0f, 0.0f),
  _x(0.0f),
  _y(0.0f),
  _width(0.0f),
  _height(0.0f),
  _xUnit(UNIT_NONE),
  _yUnit(UNIT_NONE),
  _widthUnit(UNIT_NONE),
  _heightUnit(UNIT_NONE)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgRootElement::~SvgRootElement()
{
}

// ============================================================================
// [Fog::SvgRootElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgRootElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X, FOG_S(x), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y, FOG_S(y), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Width, FOG_S(width), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Height, FOG_S(height), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(ViewBox, FOG_S(viewBox), SvgDomIO_ViewBoxF())
FOG_CORE_OBJ_END()

err_t SvgRootElement::setViewBox(const BoxF& viewBox)
{
  _viewBox = viewBox;
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::setX(const CoordF& x)
{
  _x = x.getValue();
  _xUnit = x.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::setY(const CoordF& y)
{
  _y = y.getValue();
  _yUnit = y.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::setWidth(const CoordF& width)
{
  _width = width.getValue();
  _widthUnit = width.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::setHeight(const CoordF& height)
{
  _height = height.getValue();
  _heightUnit = height.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::resetViewBox()
{
  _viewBox.reset();
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::resetX()
{
  _x = 0.0f;
  _xUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::resetY()
{
  _y = 0.0f;
  _yUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::resetWidth()
{
  _height = 0.0f;
  _heightUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRootElement::resetHeight()
{
  _height = 0.0f;
  _heightUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgRootElement - SVG Interface]
// ============================================================================

err_t SvgRootElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  if (_viewBox.isValid())
  {
    if (state)
      state->saveTransform();

    SizeF size = getRootSize();
    const BoxF& box = _viewBox;

    TransformF tr(
      size.w / box.getWidth(), 0.0f,
      0.0f, size.h / box.getHeight(),
      -box.x0, -box.y0);
    context->transform(tr);
  }

  return Base::onPrepare(context, state);
}

err_t SvgRootElement::onProcess(SvgContext* context) const
{
  if (!hasChildNodes())
    return ERR_OK;

  return _visitContainer(context);
}

err_t SvgRootElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO: SVG - onGeometryBoundingBox.
  return ERR_RT_NOT_IMPLEMENTED;
}

SizeF SvgRootElement::getRootSize() const
{
  SizeF size(0.0f, 0.0f);

  // Width/Height of document are assigned.
  if (_width > 0.0f && _height > 0.0f)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float w = svgGetCoord(doc, _width, _widthUnit);
    float h = svgGetCoord(doc, _height, _heightUnit);
 
    if (_widthUnit == UNIT_PERCENTAGE || _heightUnit == UNIT_PERCENTAGE)
    {
      if (_viewBox.isValid())
      {
        w = _viewBox.getWidth();
        h = _viewBox.getHeight();
      }
      else
      {
        w = 0.0f;
        h = 0.0f;
      }
    }

    size.set(w, h);
  }

  // If size is not specified of value is invalid, initialize it to
  // safe values.
  if (size.w <= 0.0f || size.h <= 0.0f)
  {
    size.w = 128.0f;
    size.h = 128.0f;
  }

  return size;
}

// ============================================================================
// [Fog::SvgSolidColorElement]
// ============================================================================

SvgSolidColorElement::SvgSolidColorElement(DomDocument* ownerDocument) :
  SvgStylableElement(ownerDocument, FOG_S(solidColor), SVG_ELEMENT_SOLID_COLOR)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgSolidColorElement::~SvgSolidColorElement()
{
}

err_t SvgSolidColorElement::onProcess(SvgContext* context) const
{
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgGradientElement - Construction / Destruction]
// ============================================================================

SvgGradientElement::SvgGradientElement(DomDocument* ownerDocument, const InternedStringW& tagName, uint32_t svgType) :
  SvgStylableElement(ownerDocument, tagName, svgType),
  _gradientTransform(),
  _spreadMethod(GRADIENT_SPREAD_PAD),
  _gradientUnits(SVG_OBJECT_BOUNDING_BOX)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgGradientElement::~SvgGradientElement()
{
}

// ============================================================================
// [Fog::SvgGradientElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgGradientElement)
  FOG_CORE_OBJ_PROPERTY_BASE(SpreadMethod, FOG_S(spreadMethod), SvgDomIO_Enum(svgEnum_spreadMethod))
  FOG_CORE_OBJ_PROPERTY_BASE(GradientUnits, FOG_S(gradientUnits), SvgDomIO_Enum(svgEnum_gradientUnits))
  FOG_CORE_OBJ_PROPERTY_BASE(GradientTransform, FOG_S(gradientTransform), SvgDomIO_TransformF())
FOG_CORE_OBJ_END()

err_t SvgGradientElement::setSpreadMethod(uint32_t spreadMethod)
{
  if (spreadMethod >= GRADIENT_SPREAD_COUNT)
    return ERR_OBJ_INVALID_VALUE;

  _spreadMethod = spreadMethod;
  _setDirty();

  return ERR_OK;
}

err_t SvgGradientElement::resetSpreadMethod()
{
  _spreadMethod = GRADIENT_SPREAD_PAD;
  _setDirty();

  return ERR_OK;
}

err_t SvgGradientElement::setGradientUnits(uint32_t gradientUnits)
{
  if (gradientUnits >= SVG_PATTERN_UNITS_COUNT)
    return ERR_OBJ_INVALID_VALUE;

  _gradientUnits = gradientUnits;
  _setDirty();

  return ERR_OK;
}

err_t SvgGradientElement::resetGradientUnits()
{
  _gradientUnits = SVG_OBJECT_BOUNDING_BOX;
  _setDirty();

  return ERR_OK;
}

err_t SvgGradientElement::setGradientTransform(const TransformF& gradientTransform)
{
  _gradientTransform = gradientTransform;
  _setDirty();

  return ERR_OK;
}

err_t SvgGradientElement::resetGradientTransform()
{
  _gradientTransform.reset();
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgGradientElement - SVG Interface]
// ============================================================================

err_t SvgGradientElement::onProcess(SvgContext* context) const
{
  return ERR_OK;
}

void SvgGradientElement::_walkAndAddColorStops(DomElement* root, GradientF& gradient)
{
  bool stopsParsed = false;
  uint depth = 0;

  DomNode* node;
  for (;;)
  {
    for (node = root->getFirstChild(); node != NULL; node = node->getNextSibling())
    {
      if (node->isSvgObject(SVG_ELEMENT_STOP))
      {
        SvgStopElement* stop = static_cast<SvgStopElement*>(node);

        if (stop->_offsetAssigned)
        {
          float offset = stop->_offset;
          Argb32 argb(0xFF000000);
          
          if (stop->_style.hasStyle(SVG_STYLE_STOP_COLOR))
            argb = stop->_style.getStopColor();

          if (stop->_style.hasStyle(SVG_STYLE_STOP_OPACITY))
            argb.setAlpha(Math::iround(stop->_style.getStopOpacity() * 255.0f));

          gradient.addStop(offset, argb);
          stopsParsed = true;
        }
      }
    }
    
    if (!stopsParsed)
    {
      StringW link = root->getAttribute(FOG_S(xlink_href));
      if (link.startsWith(CharW('#')))
      {
        DomDocument* document = root->getOwnerDocument();
        node = document->getElementById(StubW(link.getData() + 1, link.getLength() - 1));

        if (node != NULL)
        {
          root = static_cast<DomElement*>(node);
          if (++depth == 32)
            return;
          continue;
        }
      }
    }

    break;
  }
}

// ============================================================================
// [Fog::SvgLinearGradientElement - Construction / Destruction]
// ============================================================================

SvgLinearGradientElement::SvgLinearGradientElement(DomDocument* ownerDocument) :
  SvgGradientElement(ownerDocument, FOG_S(linearGradient), SVG_ELEMENT_LINEAR_GRADIENT),
  _x1(0.0f),
  _y1(0.0f),
  _x2(1.0f),
  _y2(0.0f),
  _x1Unit(UNIT_NONE),
  _y1Unit(UNIT_NONE),
  _x2Unit(UNIT_NONE),
  _y2Unit(UNIT_NONE)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgLinearGradientElement::~SvgLinearGradientElement()
{
}

// ============================================================================
// [Fog::SvgLinearGradientElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgLinearGradientElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X1, FOG_S(x1), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y1, FOG_S(y1), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(X2, FOG_S(x2), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y2, FOG_S(y2), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgLinearGradientElement::setX1(const CoordF& x1)
{
  _x1 = x1.getValue();
  _x1Unit = x1.getUnit();

  return ERR_OK;
}

err_t SvgLinearGradientElement::resetX1()
{
  _x1 = 0.0f;
  _x1Unit = UNIT_NONE;

  return ERR_OK;
}

err_t SvgLinearGradientElement::setY1(const CoordF& y1)
{
  _y1 = y1.getValue();
  _y1Unit = y1.getUnit();

  return ERR_OK;
}

err_t SvgLinearGradientElement::resetY1()
{
  _y1 = 0.0f;
  _y1Unit = UNIT_NONE;

  return ERR_OK;
}

err_t SvgLinearGradientElement::setX2(const CoordF& x2)
{
  _x2 = x2.getValue();
  _x2Unit = x2.getUnit();

  return ERR_OK;
}

err_t SvgLinearGradientElement::resetX2()
{
  _x2 = 1.0f;
  _x2Unit = UNIT_NONE;

  return ERR_OK;
}

err_t SvgLinearGradientElement::setY2(const CoordF& y2)
{
  _y2 = y2.getValue();
  _y2Unit = y2.getUnit();

  return ERR_OK;
}

err_t SvgLinearGradientElement::resetY2()
{
  _y2 = 0.0f;
  _y2Unit = UNIT_NONE;

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgLinearGradientElement - SVG Interface]
// ============================================================================

err_t SvgLinearGradientElement::onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const
{
  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

  LinearGradientF gradient;
  gradient.setGradientSpread(_spreadMethod);

  TransformF tr;

  // Setup start and end points.
  if (_gradientUnits == SVG_OBJECT_BOUNDING_BOX)
  {
    // "objectBoundingBox".
    BoxF bbox;
    obj->getBoundingBox(bbox);

    tr._type = TRANSFORM_TYPE_SCALING;
    tr._00 = bbox.getWidth();
    tr._11 = bbox.getHeight();
    tr._20 = bbox.getX();
    tr._21 = bbox.getY();

    gradient.setStart(_x1, _y1);
    gradient.setEnd(_x2, _y2);
  }
  else
  {
    // "userSpaceOnUse".
    float x1 = svgGetCoord(doc, _x1, _x1Unit);
    float y1 = svgGetCoord(doc, _y1, _y1Unit);
    float x2 = svgGetCoord(doc, _x2, _x2Unit);
    float y2 = svgGetCoord(doc, _y2, _y2Unit);

    // TODO: SVG - Percentages to the current view-port.
    // if (!_x1.isAssigned() || _x1.getUnit() == UNIT_PERCENT)
    // if (!_y1.isAssigned() || _y1.getUnit() == UNIT_PERCENT)
    // if (!_x2.isAssigned() || _x2.getUnit() == UNIT_PERCENT)
    // if (!_y2.isAssigned() || _y2.getUnit() == UNIT_PERCENT)

    gradient.setStart(x1, y1);
    gradient.setEnd(x2, y2);
  }

  // Add color stops.
  _walkAndAddColorStops(const_cast<SvgLinearGradientElement*>(this), gradient);

  // Create Pattern instance.
  tr.transform(_gradientTransform, MATRIX_ORDER_APPEND);
  return dst->createGradient(gradient, tr);
}

// ============================================================================
// [Fog::SvgRadialGradientElement - Construction / Destruction]
// ============================================================================

SvgRadialGradientElement::SvgRadialGradientElement(DomDocument* ownerDocument) :
  SvgGradientElement(ownerDocument, FOG_S(radialGradient), SVG_ELEMENT_RADIAL_GRADIENT),
  _cx(0.5f),
  _cy(0.5f),
  _fx(0.0f),
  _fy(0.0f),
  _r(0.5f),
  _cxUnit(UNIT_NONE),
  _cyUnit(UNIT_NONE),
  _fxUnit(UNIT_NONE),
  _fyUnit(UNIT_NONE),
  _rUnit(UNIT_NONE),
  _fxAssigned(false),
  _fyAssigned(false)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgRadialGradientElement::~SvgRadialGradientElement()
{
}

// ============================================================================
// [Fog::SvgRadialGradientElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgRadialGradientElement)
  FOG_CORE_OBJ_PROPERTY_BASE(Cx, FOG_S(cx), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Cy, FOG_S(cy), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Fx, FOG_S(fx), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Fy, FOG_S(fy), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(R, FOG_S(r), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgRadialGradientElement::setCx(const CoordF& cx)
{
  _cx = cx.getValue();
  _cxUnit = cx.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::resetCx()
{
  _cx = 0.5f;
  _cxUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::setCy(const CoordF& cy)
{
  _cy = cy.getValue();
  _cyUnit = cy.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::resetCy()
{
  _cy = 0.5f;
  _cyUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::setFx(const CoordF& fx)
{
  _fx = fx.getValue();
  _fxUnit = fx.getUnit();
  _fxAssigned = true;
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::resetFx()
{
  _fx = 0.0f;
  _fxUnit = UNIT_NONE;
  _fxAssigned = false;
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::setFy(const CoordF& fy)
{
  _fy = fy.getValue();
  _fyUnit = fy.getUnit();
  _fyAssigned = true;
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::resetFy()
{
  _fy = 0.0f;
  _fyUnit = UNIT_NONE;
  _fyAssigned = false;

  return ERR_OK;
}

err_t SvgRadialGradientElement::setR(const CoordF& r)
{
  _r = r.getValue();
  _rUnit = r.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRadialGradientElement::resetR()
{
  _r = 0.5f;
  _rUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgRadialGradientElement - SVG Interface]
// ============================================================================

err_t SvgRadialGradientElement::onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const
{
  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

  // TODO: SVG - Radial Gradient.
  RadialGradientF gradient;
  gradient.setGradientSpread(_spreadMethod);

  // Setup start and end points.
  if (_gradientUnits == SVG_OBJECT_BOUNDING_BOX)
  {
    BoxF bbox;
    obj->getBoundingBox(bbox);

    float bw = bbox.getWidth();
    float bh = bbox.getHeight();

    float cx = _cx;
    float cy = _cy;

    float fx = _fxAssigned ? _fx : cx;
    float fy = _fyAssigned ? _fy : cy;

    float rx = _r;
    float ry = rx;

    cx = bbox.x0 + bw * cx;
    cy = bbox.y0 + bh * cy;
    fx = bbox.x0 + bw * fx;
    fy = bbox.y0 + bh * fy;
    rx = bw * rx;
    ry = bh * ry;

    gradient.setCenter(cx, cy);
    gradient.setFocal(fx, fy);
    gradient.setRadius(rx, ry);
  }
  else
  {
    float cx = svgGetCoord(doc, _cx, _cxUnit);
    float cy = svgGetCoord(doc, _cy, _cyUnit);

    float fx = _fxAssigned ? svgGetCoord(doc, _fx, _fxUnit) : cx;
    float fy = _fyAssigned ? svgGetCoord(doc, _fy, _fyUnit) : cy;

    float rx = svgGetCoord(doc, _r, _rUnit);
    float ry = rx;

    // TODO: SVG - Percentages to the current view-port.
    gradient.setCenter(cx, cy);
    gradient.setFocal(fx, fy);
    gradient.setRadius(rx, ry);
  }

  // Add color stops.
  _walkAndAddColorStops(const_cast<SvgRadialGradientElement*>(this), gradient);

  // Create Pattern instance.
  return dst->createGradient(gradient, _gradientTransform);
}

// ============================================================================
// [Fog::SvgPatternElement - Construction / Destruction]
// ============================================================================

SvgPatternElement::SvgPatternElement(DomDocument* ownerDocument) :
  SvgElement(ownerDocument, FOG_S(pattern), SVG_ELEMENT_PATTERN),
  _patternTransform(),
  _viewBox(0.0f, 0.0f, 0.0f, 0.0f),
  _x(0.0f),
  _y(0.0f),
  _width(0.0f),
  _height(0.0f),
  _xUnit(UNIT_NONE),
  _yUnit(UNIT_NONE),
  _widthUnit(UNIT_NONE),
  _heightUnit(UNIT_NONE),
  _patternUnits(SVG_OBJECT_BOUNDING_BOX)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgPatternElement::~SvgPatternElement()
{
}

// ============================================================================
// [Fog::SvgPatternElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgPatternElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X, FOG_S(x), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y, FOG_S(y), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Width, FOG_S(width), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Height, FOG_S(height), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(PatternTransform, FOG_S(patternTransform), SvgDomIO_TransformF())
  FOG_CORE_OBJ_PROPERTY_BASE(PatternUnits, FOG_S(patternUnits), SvgDomIO_Enum(svgEnum_gradientUnits))
  FOG_CORE_OBJ_PROPERTY_BASE(ViewBox, FOG_S(viewBox), SvgDomIO_ViewBoxF())
FOG_CORE_OBJ_END()

err_t SvgPatternElement::setX(const CoordF& x)
{
  _x = x.getValue();
  _xUnit = x.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::resetX()
{
  _x = 0.0f;
  _xUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::setY(const CoordF& y)
{
  _y = y.getValue();
  _yUnit = y.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::resetY()
{
  _y = 0.0f;
  _yUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::setWidth(const CoordF& width)
{
  _width = width.getValue();
  _widthUnit = width.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::resetWidth()
{
  _width = 0.0f;
  _widthUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::setHeight(const CoordF& height)
{
  _height = height.getValue();
  _heightUnit = height.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::resetHeight()
{
  _height = 0.0f;
  _heightUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::setPatternTransform(const TransformF& patternTransform)
{
  _patternTransform = patternTransform;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::resetPatternTransform()
{
  _patternTransform.reset();
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::setPatternUnits(uint32_t patternUnits)
{
  if (patternUnits >= SVG_PATTERN_UNITS_COUNT)
    return ERR_OBJ_INVALID_VALUE;

  _patternUnits = patternUnits;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::resetPatternUnits()
{
  _patternUnits = SVG_OBJECT_BOUNDING_BOX;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::setViewBox(const BoxF& viewBox)
{
  _viewBox = viewBox;
  _setDirty();

  return ERR_OK;
}

err_t SvgPatternElement::resetViewBox()
{
  _viewBox.reset();
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPatternElement - SVG Interface]
// ============================================================================

err_t SvgPatternElement::onProcess(SvgContext* context) const
{
  return ERR_OK;
}

err_t SvgPatternElement::onPattern(SvgContext* context, SvgElement* obj, Pattern* dst) const
{
  return _createPattern(*dst, obj);
}

err_t SvgPatternElement::_createPattern(Pattern& pattern, SvgElement* obj) const
{
  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());
  StringW link = getAttribute(FOG_S(xlink_href));

  if (!link.isEmpty() && link.getAt(0) == CharW('#'))
  {
    DomElement* element = doc->getElementById(StubW(link.getData() + 1, link.getLength() - 1));

    if (element != NULL &&
        element->isSvgObject(SVG_ELEMENT_PATTERN))
    {
      SvgPatternElement* pe = static_cast<SvgPatternElement*>(element);
      FOG_RETURN_ON_ERROR(pe->_createPattern(pattern, obj));
      goto _AssignTransform;
    }
  }

  // TODO: SVG - Object bounding box support.
  {
    int w = Math::iround(svgGetCoord(doc, _width, _widthUnit));
    int h = Math::iround(svgGetCoord(doc, _height, _heightUnit));

    if (w == 0 || h == 0)
      return ERR_IMAGE_INVALID_SIZE;

    Image image;
    FOG_RETURN_ON_ERROR(image.create(SizeI(w, h), IMAGE_FORMAT_PRGB32));

    Painter painter(image);
    painter.setSource(Argb32(0x00000000));
    painter.setCompositingOperator(COMPOSITE_SRC);
    painter.fillAll();

    if (_viewBox.isValid())
    {
      const BoxF& box = _viewBox;
      float bbw = box.getWidth();
      float bbh = box.getHeight();

      if (bbw > MATH_EPSILON_F && bbh > MATH_EPSILON_F)
      {
        TransformF tr(
          float(w) / box.getWidth(), 0.0f,
          0.0f, float(h) / box.getHeight(),
          -box.x0, -box.y0);
        painter.setTransform(tr);
      }
    }

    SvgRenderContext ctx(doc->_createContextExtension(NULL));

    ctx.setPainter(&painter);
    ctx.initPainter();
    _visitContainer(&ctx);
    ctx.resetPainter();
    painter.end();

    float tx = svgGetCoord(doc, _x, _xUnit);
    float ty = svgGetCoord(doc, _y, _yUnit);

    pattern.createTexture(Texture(image, TEXTURE_TILE_REPEAT));
    pattern.translate(PointF(tx, ty));
  }

_AssignTransform:
  if (!_patternTransform.isIdentity())
    pattern.transform(_patternTransform);

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgStopElement - Construction / Destruction]
// ============================================================================

SvgStopElement::SvgStopElement(DomDocument* ownerDocument) :
  SvgStylableElement(ownerDocument, FOG_S(stop), SVG_ELEMENT_STOP),
  _offset(0.0f),
  _offsetAssigned(false)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgStopElement::~SvgStopElement()
{
}

// ============================================================================
// [Fog::SvgStopElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgStopElement)
  FOG_CORE_OBJ_PROPERTY_BASE(Offset, FOG_S(offset), SvgDomIO_OffsetF())
FOG_CORE_OBJ_END()

err_t SvgStopElement::setOffset(float offset)
{
  if (!Math::isFinite(offset))
    return ERR_OBJ_INVALID_VALUE;

  _offset = Math::bound<float>(offset, 0.0f, 1.0f);
  _offsetAssigned = true;
  _setDirty();

  return ERR_OK;
}

err_t SvgStopElement::resetOffset()
{
  _offset = 0.0f;
  _offsetAssigned = false;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgStopElement - SVG Interface]
// ============================================================================

err_t SvgStopElement::onProcess(SvgContext* context) const
{
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgDefsElement - Construction / Destruction]
// ============================================================================

SvgDefsElement::SvgDefsElement(DomDocument* ownerDocument) :
  SvgElement(ownerDocument, FOG_S(defs), SVG_ELEMENT_DEFS)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgDefsElement::~SvgDefsElement()
{
}

// ============================================================================
// [Fog::SvgDefsElement - SVG Interface]
// ============================================================================

err_t SvgDefsElement::onProcess(SvgContext* context) const
{
  // <defs> section is used only to define shared resources. Never go inside.
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgGElement - Construction / Destruction]
// ============================================================================

SvgGElement::SvgGElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(g), SVG_ELEMENT_G)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgGElement::~SvgGElement()
{
}

// ============================================================================
// [Fog::SvgGElement - SVG Interface]
// ============================================================================

err_t SvgGElement::onProcess(SvgContext* context) const
{
  if (!hasChildNodes())
    return ERR_OK;
  return _visitContainer(context);
}

err_t SvgGElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (!hasChildNodes())
  {
    box.reset();
    return ERR_GEOMETRY_NONE;
  }

  // TODO: SVG - It seems that there is nothing to do, because all render specific
  // options are related to a single SVG element, not to a group.
  // return _walkAndMergeBBox(this);

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::SvgSymbolElement - Construction / Destruction]
// ============================================================================

SvgSymbolElement::SvgSymbolElement(DomDocument* ownerDocument) :
  SvgStylableElement(ownerDocument, FOG_S(symbol), SVG_ELEMENT_SYMBOL)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgSymbolElement::~SvgSymbolElement()
{
}

// ============================================================================
// [Fog::SvgSymbolElement - SVG Interface]
// ============================================================================

err_t SvgSymbolElement::onProcess(SvgContext* context) const
{
  if (!hasChildNodes())
    return ERR_OK;

  return _visitContainer(context);
}

// ============================================================================
// [Fog::SvgUseElement - Construction / Destruction]
// ============================================================================

SvgUseElement::SvgUseElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(use), SVG_ELEMENT_USE),
  _x(0.0f),
  _y(0.0f),
  _width(0.0f),
  _height(0.0f),
  _xUnit(UNIT_NONE),
  _yUnit(UNIT_NONE),
  _widthUnit(UNIT_NONE),
  _heightUnit(UNIT_NONE),
  _widthAssigned(false),
  _heightAssigned(false)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgUseElement::~SvgUseElement()
{
}

// ============================================================================
// [Fog::SvgUseElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgUseElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X, FOG_S(x), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y, FOG_S(y), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Width, FOG_S(width), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Height, FOG_S(height), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgUseElement::setX(const CoordF& x)
{
  _x = x.getValue();
  _xUnit = x.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgUseElement::resetX()
{
  _x = 0.0f;
  _xUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgUseElement::setY(const CoordF& y)
{
  _y = y.getValue();
  _yUnit = y.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgUseElement::resetY()
{
  _y = 0.0f;
  _yUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgUseElement::setWidth(const CoordF& width)
{
  _width = width.getValue();
  _widthUnit = width.getUnit();
  _widthAssigned = true;
  _setDirty();

  return ERR_OK;
}

err_t SvgUseElement::resetWidth()
{
  _width = 0.0f;
  _widthUnit = UNIT_NONE;
  _widthAssigned = false;
  _setDirty();

  return ERR_OK;
}

err_t SvgUseElement::setHeight(const CoordF& height)
{
  _height = height.getValue();
  _heightUnit = height.getUnit();
  _heightAssigned = true;
  _setDirty();

  return ERR_OK;
}

err_t SvgUseElement::resetHeight()
{
  _height = 0.0f;
  _heightUnit = UNIT_NONE;
  _heightAssigned = false;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUseElement - SVG Methods]
// ============================================================================

SvgElement* SvgUseElement::getLinkedElement() const
{
  StringW link;

  if (getProperty(FOG_S(xlink_href), link) != ERR_OK)
    return NULL;

  if (link.isEmpty())
    return NULL;

  DomElement* ref = _ownerDocument->getElementById(parseHtmlLinkId(link));
  if (ref == NULL)
    return NULL;

  if (ref->isSvgNode(DOM_NODE_TYPE_ELEMENT))
    return static_cast<SvgElement*>(ref);

  return NULL;
}

// ============================================================================
// [Fog::SvgUseElement - SVG Interface]
// ============================================================================

err_t SvgUseElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  err_t err =  Base::onPrepare(context, state);

  if (err == ERR_OK)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float tx = svgGetCoord(doc, _x, _xUnit);
    float ty = svgGetCoord(doc, _y, _yUnit);

    if (_widthAssigned || _heightAssigned)
    {
      SvgElement* ref = getLinkedElement();
      if (ref != NULL)
      {
        BoxF bbox(UNINITIALIZED);
        ref->getBoundingBox(bbox);

        
        // TODO: SVG - <use> width/height support.
      }
    }

    if (tx != 0.0f || ty != 0.0f)
    {
      if (state && !state->hasState(SvgContextGState::SAVED_TRANSFORM))
        state->saveTransform();
      context->translate(PointF(tx, ty));
    }
  }

  return err;
}

err_t SvgUseElement::onProcess(SvgContext* context) const
{
  SvgElement* ref = getLinkedElement();
  if (ref == NULL)
    return ERR_OK;
  return context->onVisit(ref);
}

err_t SvgUseElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  box.reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgViewElement - Construction / Destruction]
// ============================================================================

SvgViewElement::SvgViewElement(DomDocument* ownerDocument) :
  SvgElement(ownerDocument, FOG_S(svg), SVG_ELEMENT_VIEW),
  _viewBox(0.0f, 0.0f, 0.0f, 0.0f)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgViewElement::~SvgViewElement()
{
}

// ============================================================================
// [Fog::SvgViewElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgViewElement)
  FOG_CORE_OBJ_PROPERTY_BASE(ViewBox, FOG_S(viewBox), SvgDomIO_ViewBoxF())
FOG_CORE_OBJ_END()

err_t SvgViewElement::setViewBox(const BoxF& viewBox)
{
  _viewBox = viewBox;
  _setDirty();

  return ERR_OK;
}

err_t SvgViewElement::resetViewBox()
{
  _viewBox.reset();
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgViewElement - SVG Interface]
// ============================================================================

err_t SvgViewElement::onProcess(SvgContext* context) const
{
  if (!hasChildNodes()) return ERR_OK;
  return _visitContainer(context);
}

err_t SvgViewElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO: SVG - onGeometryBoundingBox.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::SvgCircleElement - Construction / Destruction]
// ============================================================================

SvgCircleElement::SvgCircleElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(circle), SVG_ELEMENT_CIRCLE),
  _cx(0.0f),
  _cy(0.0f),
  _r(0.0f),
  _cxUnit(UNIT_NONE),
  _cyUnit(UNIT_NONE),
  _rUnit(UNIT_NONE)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgCircleElement::~SvgCircleElement()
{
}

// ============================================================================
// [Fog::SvgCircleElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgCircleElement)
  FOG_CORE_OBJ_PROPERTY_BASE(Cx, FOG_S(cx), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Cy, FOG_S(cy), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(R, FOG_S(r), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgCircleElement::setCx(const CoordF& cx)
{
  _cx = cx.getValue();
  _cxUnit = cx.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgCircleElement::resetCx()
{
  _cx = 0.0f;
  _cxUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgCircleElement::setCy(const CoordF& cy)
{
  _cy = cy.getValue();
  _cyUnit = cy.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgCircleElement::resetCy()
{
  _cy = 0.0f;
  _cyUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgCircleElement::setR(const CoordF& r)
{
  _r = r.getValue();
  _rUnit = r.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgCircleElement::resetR()
{
  _r = 0.0f;
  _rUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgCircleElement - SVG Interface]
// ============================================================================

err_t SvgCircleElement::onProcess(SvgContext* context) const
{
  if (_r > 0.0f)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float cx = svgGetCoord(doc, _cx, _cxUnit);
    float cy = svgGetCoord(doc, _cy, _cyUnit);
    float r = Math::abs(svgGetCoord(doc, _r, _rUnit));

    if (r <= 0.0f)
      return ERR_OK;

    CircleF circle(cx, cy, r);
    return context->onShape((SvgElement*)this, ShapeF(&circle));
  }

  return ERR_OK;
}

err_t SvgCircleElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (_r > 0.0f)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float cx = svgGetCoord(doc, _cx, _cxUnit);
    float cy = svgGetCoord(doc, _cy, _cyUnit);
    float r = Math::abs(svgGetCoord(doc, _r, _rUnit));

    if (r <= 0.0f)
      goto _Fail;

    return CircleF(PointF(cx, cy), r)._getBoundingBox(box, tr);
  }

_Fail:
  box.reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgEllipseElement]
// ============================================================================

SvgEllipseElement::SvgEllipseElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(ellipse), SVG_ELEMENT_ELLIPSE),
  _cx(0.0f),
  _cy(0.0f),
  _rx(0.0f),
  _ry(0.0f),
  _cxUnit(UNIT_NONE),
  _cyUnit(UNIT_NONE),
  _rxUnit(UNIT_NONE),
  _ryUnit(UNIT_NONE)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgEllipseElement::~SvgEllipseElement()
{
}

// ============================================================================
// [Fog::SvgEllipseElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgEllipseElement)
  FOG_CORE_OBJ_PROPERTY_BASE(Cx, FOG_S(cx), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Cy, FOG_S(cy), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Rx, FOG_S(rx), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Ry, FOG_S(ry), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgEllipseElement::setCx(const CoordF& cx)
{
  _cx = cx.getValue();
  _cxUnit = cx.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgEllipseElement::resetCx()
{
  _cx = 0.0f;
  _cxUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgEllipseElement::setCy(const CoordF& cy)
{
  _cy = cy.getValue();
  _cyUnit = cy.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgEllipseElement::resetCy()
{
  _cy = 0.0f;
  _cyUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgEllipseElement::setRx(const CoordF& rx)
{
  _rx = rx.getValue();
  _rxUnit = rx.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgEllipseElement::resetRx()
{
  _rx = 0.0f;
  _rxUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgEllipseElement::setRy(const CoordF& ry)
{
  _ry = ry.getValue();
  _ryUnit = ry.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgEllipseElement::resetRy()
{
  _ry = 0.0f;
  _ryUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgEllipseElement - SVG Interface]
// ============================================================================

err_t SvgEllipseElement::onProcess(SvgContext* context) const
{
  if (_rx > 0.0f && _ry > 0.0f)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float cx = svgGetCoord(doc, _cx, _cxUnit);
    float cy = svgGetCoord(doc, _cy, _cyUnit);

    float rx = Math::abs(svgGetCoord(doc, _rx, _rxUnit));
    float ry = Math::abs(svgGetCoord(doc, _ry, _ryUnit));

    if (rx <= 0.0f || ry <= 0.0f)
      return ERR_OK;

    EllipseF ellipse(cx, cy, rx, ry);
    return context->onShape((SvgElement*)this, ShapeF(&ellipse));
  }

  return ERR_OK;
}

err_t SvgEllipseElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (_rx > 0.0f && _ry > 0.0f)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float cx = svgGetCoord(doc, _cx, _cxUnit);
    float cy = svgGetCoord(doc, _cy, _cyUnit);

    float rx = Math::abs(svgGetCoord(doc, _rx, _rxUnit));
    float ry = Math::abs(svgGetCoord(doc, _ry, _ryUnit));

    if (rx <= 0.0f || ry <= 0.0f)
      goto _Fail;

    return EllipseF(PointF(cx, cy), PointF(rx, ry))._getBoundingBox(box, tr);
  }

_Fail:
  box.reset();
  return ERR_GEOMETRY_INVALID;
}

// ============================================================================
// [Fog::SvgLineElement - Construction / Destruction]
// ============================================================================

SvgLineElement::SvgLineElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(line), SVG_ELEMENT_LINE),
  _x1(0.0f),
  _y1(0.0f),
  _x2(0.0f),
  _y2(0.0f),
  _x1Unit(UNIT_NONE),
  _y1Unit(UNIT_NONE),
  _x2Unit(UNIT_NONE),
  _y2Unit(UNIT_NONE)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgLineElement::~SvgLineElement()
{
}

// ============================================================================
// [Fog::SvgLineElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgLineElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X1, FOG_S(x1), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y1, FOG_S(y1), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(X2, FOG_S(x2), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y2, FOG_S(y2), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgLineElement::setX1(const CoordF& x1)
{
  _x1 = x1.getValue();
  _x1Unit = x1.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgLineElement::resetX1()
{
  _x1 = 0.0f;
  _x1Unit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgLineElement::setY1(const CoordF& y1)
{
  _y1 = y1.getValue();
  _y1Unit = y1.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgLineElement::resetY1()
{
  _y1 = 0.0f;
  _y1Unit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgLineElement::setX2(const CoordF& x2)
{
  _x2 = x2.getValue();
  _x2Unit = x2.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgLineElement::resetX2()
{
  _x2 = 0.0f;
  _x2Unit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgLineElement::setY2(const CoordF& y2)
{
  _y2 = y2.getValue();
  _y2Unit = y2.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgLineElement::resetY2()
{
  _y2 = 0.0f;
  _y2Unit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgLineElement - SVG Interface]
// ============================================================================

err_t SvgLineElement::onProcess(SvgContext* context) const
{
  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

  float x1 = svgGetCoord(doc, _x1, _x1Unit);
  float y1 = svgGetCoord(doc, _y1, _y1Unit);
  float x2 = svgGetCoord(doc, _x2, _x2Unit);
  float y2 = svgGetCoord(doc, _y2, _y2Unit);

  LineF line(x1, y1, x2, y2);
  return context->onShape((SvgElement*)this, ShapeF(&line));
}

err_t SvgLineElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

  float x1 = svgGetCoord(doc, _x1, _x1Unit);
  float y1 = svgGetCoord(doc, _y1, _y1Unit);
  float x2 = svgGetCoord(doc, _x2, _x2Unit);
  float y2 = svgGetCoord(doc, _y2, _y2Unit);

  box.setBox(x1, y1, x2, y2);
  if (tr != NULL)
    tr->mapBox(box, box);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPathElement - Construction / Destruction]
// ============================================================================

SvgPathElement::SvgPathElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(path), SVG_ELEMENT_PATH)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgPathElement::~SvgPathElement()
{
}

// ============================================================================
// [Fog::SvgPathElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgPathElement)
  FOG_CORE_OBJ_PROPERTY_BASE(D, FOG_S(d), SvgDomIO_PathF())
FOG_CORE_OBJ_END()

err_t SvgPathElement::setD(const PathF& d)
{
  _d = d;
  _setDirty();
  
  return ERR_OK;
}

err_t SvgPathElement::resetD()
{
  _d.reset();
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPathElement - SVG Interface]
// ============================================================================

err_t SvgPathElement::onProcess(SvgContext* context) const
{
  if (!_d.isEmpty())
    return context->onShape((SvgElement*)this, ShapeF(&_d));
  else
    return ERR_OK;
}

err_t SvgPathElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (!_d.isEmpty())
  {
    return _d._getBoundingBox(box, tr);
  }
  else
  {
    box.reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgPolygonElement - Construction / Destruction]
// ============================================================================

SvgPolygonElement::SvgPolygonElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(polygon), SVG_ELEMENT_POLYGON),
  _points()
{
  FOG_DOM_ELEMENT_INIT();
}

SvgPolygonElement::~SvgPolygonElement()
{
}

// ============================================================================
// [Fog::SvgPolygonElement - SVG Propreties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgPolygonElement)
  FOG_CORE_OBJ_PROPERTY_BASE(Points, FOG_S(points), SvgDomIO_PointsF(true))
FOG_CORE_OBJ_END()

err_t SvgPolygonElement::setPoints(const PathF& points)
{
  _points = points;
  _setDirty();
  
  return ERR_OK;
}

err_t SvgPolygonElement::resetPoints()
{
  _points.reset();
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPolygonElement - SVG Interface]
// ============================================================================

err_t SvgPolygonElement::onProcess(SvgContext* context) const
{
  if (!_points.isEmpty())
    return context->onShape((SvgElement*)this, ShapeF(&_points));
  else
    return ERR_OK;
}

err_t SvgPolygonElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (!_points.isEmpty())
  {
    return _points._getBoundingBox(box, tr);
  }
  else
  {
    box.reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgPolylineElement - Construction / Destruction]
// ============================================================================

SvgPolylineElement::SvgPolylineElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(polyline), SVG_ELEMENT_POLYLINE),
  _points()
{
  FOG_DOM_ELEMENT_INIT();
}

SvgPolylineElement::~SvgPolylineElement()
{
}

// ============================================================================
// [Fog::SvgPolylineElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgPolylineElement)
  FOG_CORE_OBJ_PROPERTY_BASE(Points, FOG_S(points), SvgDomIO_PointsF(false))
FOG_CORE_OBJ_END()

err_t SvgPolylineElement::setPoints(const PathF& points)
{
  _points = points;
  _setDirty();
  
  return ERR_OK;
}

err_t SvgPolylineElement::resetPoints()
{
  _points.reset();
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPolylineElement - SVG Interface]
// ============================================================================

err_t SvgPolylineElement::onProcess(SvgContext* context) const
{
  if (!_points.isEmpty())
    return context->onShape((SvgElement*)this, ShapeF(&_points));
  else
    return ERR_OK;
}

err_t SvgPolylineElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (!_points.isEmpty())
  {
    return _points._getBoundingBox(box, tr);
  }
  else
  {
    box.reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgRectElement - Construction / Destruction]
// ============================================================================

SvgRectElement::SvgRectElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(rect), SVG_ELEMENT_RECT),
  _x(0.0f),
  _y(0.0f),
  _width(0.0f),
  _height(0.0f),
  _rx(0.0f),
  _ry(0.0f),
  _xUnit(UNIT_NONE),
  _yUnit(UNIT_NONE),
  _widthUnit(UNIT_NONE),
  _heightUnit(UNIT_NONE),
  _rxUnit(UNIT_NONE),
  _ryUnit(UNIT_NONE),
  _rxAssigned(false),
  _ryAssigned(false)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgRectElement::~SvgRectElement()
{
}

// ============================================================================
// [Fog::SvgRectElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgRectElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X, FOG_S(x), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y, FOG_S(y), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Width, FOG_S(width), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Height, FOG_S(height), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Rx, FOG_S(rx), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Ry, FOG_S(ry), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgRectElement::setX(const CoordF& x)
{
  _x = x.getValue();
  _xUnit = x.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::resetX()
{
  _x = 0.0f;
  _xUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::setY(const CoordF& y)
{
  _y = y.getValue();
  _yUnit = y.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::resetY()
{
  _y = 0.0f;
  _yUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::setWidth(const CoordF& width)
{
  _width = width.getValue();
  _widthUnit = width.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::resetWidth()
{
  _width = 0.0f;
  _widthUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::setHeight(const CoordF& height)
{
  _height = height.getValue();
  _heightUnit = height.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::resetHeight()
{
  _height = 0.0f;
  _heightUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::setRx(const CoordF& rx)
{
  _rx = rx.getValue();
  _rxUnit = rx.getUnit();
  _rxAssigned = true;
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::resetRx()
{
  _rx = 0.0f;
  _rxUnit = UNIT_NONE;
  _rxAssigned = false;
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::setRy(const CoordF& ry)
{
  _ry = ry.getValue();
  _ryUnit = ry.getUnit();
  _ryAssigned = true;
  _setDirty();

  return ERR_OK;
}

err_t SvgRectElement::resetRy()
{
  _ry = 0.0f;
  _ryUnit = UNIT_NONE;
  _ryAssigned = false;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgRectElement - SVG Interface]
// ============================================================================

err_t SvgRectElement::onProcess(SvgContext* context) const
{
  if (_width > 0.0f && _height > 0.0f)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float w = svgGetCoord(doc, _width, _widthUnit);
    float h = svgGetCoord(doc, _height, _heightUnit);

    if (w <= 0.0f || h <= 0.0f)
      return ERR_GEOMETRY_INVALID;

    float x = svgGetCoord(doc, _x, _xUnit);
    float y = svgGetCoord(doc, _y, _yUnit);

    float rx = svgGetCoord(doc, _rx, _rxUnit);
    float ry = svgGetCoord(doc, _ry, _ryUnit);

    if (!_rxAssigned && _ryAssigned) rx = ry;
    if (!_ryAssigned && _rxAssigned) ry = rx;

    if (rx <= 0.0f || ry <= 0.0f)
    {
      RectF rect(x, y, w, h);
      return context->onShape((SvgElement*)this, ShapeF(&rect));
    }
    else
    {
      RoundF round(x, y, w, h, rx, ry);
      return context->onShape((SvgElement*)this, ShapeF(&round));
    }
  }

  return ERR_OK;
}

err_t SvgRectElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (_width > 0.0f && _height > 0.0f)
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

    float w = svgGetCoord(doc, _width, _widthUnit);
    float h = svgGetCoord(doc, _height, _heightUnit);

    if (w <= 0.0f || h <= 0.0f)
      goto _Fail;

    float x = svgGetCoord(doc, _x, _xUnit);
    float y = svgGetCoord(doc, _y, _yUnit);

    box.setRect(x, y, w, h);
    if (tr) tr->mapBox(box, box);

    return ERR_OK;
  }

_Fail:
  box.reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgImageElement - Construction / Destruction]
// ============================================================================

SvgImageElement::SvgImageElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(image), SVG_ELEMENT_IMAGE),
  _href(),
  _resource(NULL),
  _x(0.0f),
  _y(0.0f),
  _width(0.0f),
  _height(0.0f),
  _xUnit(UNIT_NONE),
  _yUnit(UNIT_NONE),
  _widthUnit(UNIT_NONE),
  _heightUnit(UNIT_NONE)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgImageElement::~SvgImageElement()
{
  if (_resource != NULL)
    _resource->release();
}

// ============================================================================
// [Fog::SvgImageElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgImageElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X, FOG_S(x), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y, FOG_S(y), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Width, FOG_S(width), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Height, FOG_S(height), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_DEFAULT(Href, FOG_S(xlink_href))
FOG_CORE_OBJ_END()

err_t SvgImageElement::setX(const CoordF& x)
{
  _x = x.getValue();
  _xUnit = x.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::resetX()
{
  _x = 0.0f;
  _xUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::setY(const CoordF& y)
{
  _y = y.getValue();
  _yUnit = y.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::resetY()
{
  _y = 0.0f;
  _yUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::setWidth(const CoordF& width)
{
  _width = width.getValue();
  _widthUnit = width.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::resetWidth()
{
  _width = 0.0f;
  _widthUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::setHeight(const CoordF& height)
{
  _height = height.getValue();
  _heightUnit = height.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::resetHeight()
{
  _height = 0.0f;
  _heightUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgImageElement::setHref(const StringW& href)
{
  if (_href == href)
    return ERR_OK;

  FOG_RETURN_ON_ERROR(_href.set(href));
  _setDirty();

  if (_resource != NULL)
  {
    _resource->release();
    _resource = NULL;
  }

  err_t err = ERR_OK;
  if (_href.isEmpty())
    return err;

  DomResourceManager* mgr = getOwnerDocument()->getResourceManager();
  if (_href.startsWith(Ascii8("data:")))
  {
    size_t semicolon = href.indexOf(CharW(';'));
    size_t separator = href.indexOf(CharW(','));

    if (semicolon != INVALID_INDEX && separator != INVALID_INDEX)
    {
      StringW type = href.substring(Range(5, semicolon));
      StringW extension;
      StringW encoding = href.substring(Range(semicolon + 1, separator));

      StringA memio;
      Stream stream;

      // Standard mentions only PNG and JPEG, but we can use generally
      // any attached image which can be decoded by Fog-Framework API.
      if (type == Ascii8("image/png"))
        extension = FOG_S(png);
      else if (type == Ascii8("image/jpeg"))
        extension = FOG_S(jpeg);

      if (encoding == Ascii8("base64"))
      {
        err = StringA::base64Decode(memio, CONTAINER_OP_REPLACE,
          href.getData() + separator + 1, href.getLength() - separator - 1);
      }
      else
      {
        // Maybe in future something else will be supported by the SVG standard.
        err = ERR_SVG_INVALID_DATA_ENCODING;
      }

      if (FOG_IS_ERROR(err))
        return err;

      err = stream.openBuffer(memio);
      if (FOG_IS_ERROR(err))
        return err;

      Image image;
      err = image.readFromStream(stream, extension);
      if (FOG_IS_ERROR(err))
        return err;

      _resource = mgr->createInternalResource(Var::fromImage(image));
    }
  }
  else
  {
    _resource = mgr->createExternalResource(_href);
  }

  return ERR_OK;
}

err_t SvgImageElement::resetHref()
{
  _href.reset();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgImageElement - SVG Interface]
// ============================================================================

err_t SvgImageElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  // There is only transformation attribute which can be applied to the image.
  if (!_transform.isIdentity())
  {
    if (state && !state->hasState(SvgContextGState::SAVED_TRANSFORM))
      state->saveTransform();
    context->transform(_transform);
  }

  return ERR_OK;
}

err_t SvgImageElement::onProcess(SvgContext* context) const
{
  if (_resource != NULL && _resource->isLoaded())
  {
    SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());
    Image image;

    err_t err = _resource->getData().getImage(image);
    if (FOG_IS_ERROR(err))
      return err;

    float x = svgGetCoord(doc, _x, _xUnit);
    float y = svgGetCoord(doc, _y, _yUnit);
    return context->onImage((SvgElement*)this, PointF(x, y), image);
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgTextPositioningElement - Construction / Destruction]
// ============================================================================

SvgTextPositioningElement::SvgTextPositioningElement(DomDocument* ownerDocument,
  const InternedStringW& tagName, uint32_t svgType)
  :
  SvgTransformableElement(ownerDocument, tagName, svgType),
  _x(0.0f),
  _y(0.0f),
  _dx(0.0f),
  _dy(0.0f),
  _xUnit(UNIT_NONE),
  _yUnit(UNIT_NONE),
  _dxUnit(UNIT_NONE),
  _dyUnit(UNIT_NONE)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgTextPositioningElement::~SvgTextPositioningElement()
{
}

// ============================================================================
// [Fog::SvgTextPositioningElement - SVG Properties]
// ============================================================================

FOG_CORE_OBJ_DEF(SvgTextPositioningElement)
  FOG_CORE_OBJ_PROPERTY_BASE(X, FOG_S(x), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Y, FOG_S(y), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Dx, FOG_S(dx), SvgDomIO_CoordF())
  FOG_CORE_OBJ_PROPERTY_BASE(Dy, FOG_S(dy), SvgDomIO_CoordF())
FOG_CORE_OBJ_END()

err_t SvgTextPositioningElement::setX(const CoordF& x)
{
  _x = x.getValue();
  _xUnit = x.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgTextPositioningElement::resetX()
{
  _x = 0.0f;
  _xUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgTextPositioningElement::setY(const CoordF& y)
{
  _y = y.getValue();
  _yUnit = y.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgTextPositioningElement::resetY()
{
  _y = 0.0f;
  _yUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgTextPositioningElement::setDx(const CoordF& dx)
{
  _dx = dx.getValue();
  _dxUnit = dx.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgTextPositioningElement::resetDx()
{
  _dx = 0.0f;
  _dxUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

err_t SvgTextPositioningElement::setDy(const CoordF& dy)
{
  _dy = dy.getValue();
  _dyUnit = dy.getUnit();
  _setDirty();

  return ERR_OK;
}

err_t SvgTextPositioningElement::resetDy()
{
  _dy = 0.0f;
  _dyUnit = UNIT_NONE;
  _setDirty();

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgTextElement - Construction / Destruction]
// ============================================================================

SvgTextElement::SvgTextElement(DomDocument* ownerDocument) :
  SvgTextPositioningElement(ownerDocument, FOG_S(text), SVG_ELEMENT_TEXT)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgTextElement::~SvgTextElement()
{
}

// ============================================================================
// [Fog::SvgTextElement - SVG Interface]
// ============================================================================

err_t SvgTextElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  Base::onPrepare(context, state);
  if (state && !state->hasState(SvgContextGState::SAVED_GLOBAL))
    state->saveGlobal();

  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());

  float x = svgGetCoord(doc, _x, _xUnit);
  float y = svgGetCoord(doc, _y, _yUnit);
  context->_textCursor.set(x, y);

  return ERR_OK;
}

err_t SvgTextElement::onProcess(SvgContext* context) const
{
  err_t err = ERR_OK;

  float x = context->_textCursor.x;
  float y = context->_textCursor.y;

  DomNode* node;
  for (node = getFirstChild(); node != NULL; node = node->getNextSibling())
  {
    if (node->isSvgNode(DOM_NODE_TYPE_ELEMENT) && static_cast<SvgElement*>(node)->getVisible())
    {
      err = context->onVisit(static_cast<SvgElement*>(node));
      if (FOG_IS_ERROR(err))
        break;
    }

    if (node->isText())
    {
      StringW text = node->getTextContent();
      text.simplify();

      // TODO: SVG - Not optimal, just initial support for text rendering.
      PathF path;
      context->_font.getTextOutline(path, CONTAINER_OP_APPEND, PointF(x, y), text);

      err = context->onShape((SvgElement*)this, ShapeF(&path));
      if (FOG_IS_ERROR(err))
        break;
    }
  }

  return err;
}

err_t SvgTextElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  // TODO: SVG - SVGText bounding box.
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::SvgTSpanElement - Construction / Destruction]
// ============================================================================

SvgTSpanElement::SvgTSpanElement(DomDocument* ownerDocument) :
  SvgTextPositioningElement(ownerDocument, FOG_S(tspan), SVG_ELEMENT_TSPAN)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgTSpanElement::~SvgTSpanElement()
{
}

// ============================================================================
// [Fog::SvgTSpanElement - SVG Interface]
// ============================================================================

err_t SvgTSpanElement::onPrepare(SvgContext* context, SvgContextGState* state) const
{
  Base::onPrepare(context, state);

  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());
  if (state && !state->hasState(SvgContextGState::SAVED_GLOBAL))
    state->saveGlobal();

  float x = svgGetCoord(doc, _x, _xUnit);
  float y = svgGetCoord(doc, _y, _yUnit);
  context->_textCursor.set(x, y);

  return ERR_OK;
}

err_t SvgTSpanElement::onProcess(SvgContext* context) const
{
  SvgDocument* doc = static_cast<SvgDocument*>(getOwnerDocument());
  err_t err = ERR_OK;

  float x = context->_textCursor.x + svgGetCoord(doc, _dx, _dxUnit);
  float y = context->_textCursor.y + svgGetCoord(doc, _dy, _dyUnit);

  StringW text = getTextContent();
  text.simplify();

  // TODO: SVG - Not optimal, just initial support for text rendering.
  PathF path;

  err = context->_font.getTextOutline(path, CONTAINER_OP_APPEND, PointF(x, y), text);
  if (FOG_IS_ERROR(err)) return err;

  err = context->onShape((SvgElement*)this, ShapeF(&path));
  context->_textCursor.set(x, y);
  return err;
}

// ============================================================================
// [Fog::SvgAElement - Construction / Destruction]
// ============================================================================

SvgAElement::SvgAElement(DomDocument* ownerDocument) :
  SvgTransformableElement(ownerDocument, FOG_S(a), SVG_ELEMENT_A)
{
  FOG_DOM_ELEMENT_INIT();
}

SvgAElement::~SvgAElement()
{
}

// ============================================================================
// [Fog::SvgAElement - SVG Interface]
// ============================================================================

err_t SvgAElement::onProcess(SvgContext* context) const
{
  return _visitContainer(context);
}

// ============================================================================
// [Fog::SvgDocument - Construction / Destruction]
// ============================================================================

SvgDocument::SvgDocument() :
  _dpi(96.0f)
{
  _nodeFlags |= DOM_NODE_FLAG_IS_SVG;
  _objectType = SVG_ELEMENT_NONE;
}

SvgDocument::~SvgDocument()
{
}

// ============================================================================
// [Fog::SvgDocument - DOM Interface]
// ============================================================================

DomDocument* SvgDocument::_createDocument()
{
  return fog_new SvgDocument();
}

DomElement* SvgDocument::_createElement(const InternedStringW& tagName)
{
  if (tagName == FOG_S(a             )) return _newElementT<SvgAElement>();
  if (tagName == FOG_S(circle        )) return _newElementT<SvgCircleElement>();
  if (tagName == FOG_S(defs          )) return _newElementT<SvgDefsElement>();
  if (tagName == FOG_S(ellipse       )) return _newElementT<SvgEllipseElement>();
  if (tagName == FOG_S(g             )) return _newElementT<SvgGElement>();
  if (tagName == FOG_S(image         )) return _newElementT<SvgImageElement>();
  if (tagName == FOG_S(line          )) return _newElementT<SvgLineElement>();
  if (tagName == FOG_S(linearGradient)) return _newElementT<SvgLinearGradientElement>();
  if (tagName == FOG_S(path          )) return _newElementT<SvgPathElement>();
  if (tagName == FOG_S(pattern       )) return _newElementT<SvgPatternElement>();
  if (tagName == FOG_S(polygon       )) return _newElementT<SvgPolygonElement>();
  if (tagName == FOG_S(polyline      )) return _newElementT<SvgPolylineElement>();
  if (tagName == FOG_S(radialGradient)) return _newElementT<SvgRadialGradientElement>();
  if (tagName == FOG_S(rect          )) return _newElementT<SvgRectElement>();
  if (tagName == FOG_S(solidColor    )) return _newElementT<SvgSolidColorElement>();
  if (tagName == FOG_S(stop          )) return _newElementT<SvgStopElement>();
  if (tagName == FOG_S(svg           )) return _newElementT<SvgRootElement>();
  if (tagName == FOG_S(symbol        )) return _newElementT<SvgSymbolElement>();
  if (tagName == FOG_S(text          )) return _newElementT<SvgTextElement>();
  if (tagName == FOG_S(tspan         )) return _newElementT<SvgTSpanElement>();
  if (tagName == FOG_S(use           )) return _newElementT<SvgUseElement>();
  if (tagName == FOG_S(view          )) return _newElementT<SvgViewElement>();

  // If element is not SVG, use the base class to create a default element
  // for the given tagName. But remember, this element won't be processed.
  return Base::_createElement(tagName);
}

// ============================================================================
// [Fog::SvgDocument - SVG Interface]
// ============================================================================

SvgContextExtension* SvgDocument::_createContextExtension(const SvgContextExtension* ex)
{
  return NULL;
}

// ============================================================================
// [Fog::SvgDocument - DPI / Size]
// ============================================================================


err_t SvgDocument::setDpi(float dpi)
{
  return _dpi.setDpi(dpi);
}

SizeF SvgDocument::getDocumentSize() const
{
  DomElement* root = getDocumentElement();

  if (root == NULL || !root->isSvgObject(SVG_ELEMENT_SVG))
    return SizeF(0.0f, 0.0f);
  else
    return static_cast<SvgRootElement*>(root)->getRootSize();
}

// ============================================================================
// [Fog::SvgDocument - Context]
// ============================================================================

err_t SvgDocument::onProcess(SvgContext* context)
{
  DomElement* root = getDocumentElement();

  if (root == NULL || !root->isSvgObject(SVG_ELEMENT_SVG))
    return ERR_OK;

  return context->onVisit(static_cast<SvgElement*>(root));
}

err_t SvgDocument::render(Painter* painter)
{
  SvgRenderContext ctx(_createContextExtension(NULL));

  ctx.setPainter(painter);
  ctx.initPainter();
  err_t err = onProcess(&ctx);
  ctx.resetPainter();

  return err;
}

List<SvgElement*> SvgDocument::hitTest(const PointF& pt, const TransformF* tr)
{
  SvgHitTestContext ctx(_createContextExtension(NULL));
  ctx.setPoint(pt);

  if (tr != NULL)
    ctx.setTransform(*tr);

  onProcess(&ctx);
  return ctx._result;
}

} // Fog namespace
