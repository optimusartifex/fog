// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Char.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Svg/SvgContext.h>
#include <Fog/Svg/SvgDom.h>
#include <Fog/Svg/SvgUtil.h>

namespace Fog {

// ============================================================================
// [Fog::Forward Declarations]
// ============================================================================

struct SvgImageElement;

// ============================================================================
// [Fog::SvgEnumList]
// ============================================================================

struct SvgEnumList
{
  char name[20];
  int value;
};

static const SvgEnumList svgEnumList_gradientUnits[] =
{
  { "userSpaceOnUse", SVG_USER_SPACE_ON_USE },
  { "objectBoundingBox", SVG_OBJECT_BOUNDING_BOX },
  { "", -1 }
};

static const SvgEnumList svgEnumList_spreadMethod[] =
{
  { "pad", PATTERN_SPREAD_PAD },
  { "reflect", PATTERN_SPREAD_REFLECT },
  { "repeat", PATTERN_SPREAD_REPEAT },
  { "", -1 }
};

static const SvgEnumList svgEnumList_fillRule[] =
{
  { "nonzero", FILL_NON_ZERO },
  { "evenodd", FILL_EVEN_ODD },
  { "", -1 }
};

static const SvgEnumList svgEnumList_strokeLineCap[] =
{
  { "butt", LINE_CAP_BUTT },
  { "round", LINE_CAP_ROUND },
  { "square", LINE_CAP_SQUARE },
  { "", -1 }
};

static const SvgEnumList svgEnumList_strokeLineJoin[] =
{
  { "miter", LINE_JOIN_MITER },
  { "round", LINE_JOIN_ROUND },
  { "bevel", LINE_JOIN_BEVEL },
  { "", -1 }
};

static int getSvgEnumId(const String& value, const SvgEnumList* list)
{
  while (list->name[0] != '\0')
  {
    if (value == Ascii8(list->name)) break;
    list++;
  }

  return list->value;
}

// ============================================================================
// [Fog::SvgStyleAttribute]
// ============================================================================

static int styleToId(const ManagedString& name)
{
  int i;

  for (i = 0; i < SVG_STYLE_INVALID; i++)
  {
    if (name == fog_strings->getString(i + STR_SVG_STYLE_NAMES)) break;
  }

  return i;
}

#include <Fog/Core/Pack/PackByte.h>
struct FOG_HIDDEN SvgStyleAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgStyleAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgStyleAttribute();

  // [Methods]

  virtual String getValue() const;
  virtual err_t setValue(const String& value);

  // [Getters]

  FOG_INLINE uint32_t getMask() const { return _mask; }
  FOG_INLINE bool hasStyle(int id) const { return (_mask & (1 << id)) != 0; }

  // [Style]

  String getStyle(const ManagedString& name) const;
  err_t setStyle(const ManagedString& name, const String& value);

  String getStyle(int styleId) const;
  err_t setStyle(int styleId, const String& value);

  static int styleToId(const ManagedString& name);

  // [Members]

  uint32_t _mask;

  uint8_t _clipRule;
  uint8_t _fillSource;
  uint8_t _fillRule;
  uint8_t _strokeSource;
  uint8_t _strokeLineCap;
  uint8_t _strokeLineJoin;
  uint8_t _reserved[2];

  ArgbI _fillColor;
  ArgbI _strokeColor;
  ArgbI _stopColor;

  float _opacity;
  float _fillOpacity;
  float _strokeOpacity;
  float _stopOpacity;

  SvgCoord _strokeDashOffset;
  SvgCoord _strokeMiterLimit;
  SvgCoord _strokeWidth;
  SvgCoord _fontSize;
  SvgCoord _letterSpacing;

  PathD _clipPath;
  List<double> _dashArray;

  String _fillUri;
  String _strokeUri;
  String _fontFamily;

private:
  FOG_DISABLE_COPY(SvgStyleAttribute)
};
#include <Fog/Core/Pack/PackRestore.h>

SvgStyleAttribute::SvgStyleAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset),

  _mask(0),

  _clipRule(FILL_DEFAULT),
  _fillSource(SVG_SOURCE_NONE),
  _fillRule(FILL_DEFAULT),
  _strokeSource(SVG_SOURCE_NONE),
  _strokeLineCap(LINE_CAP_DEFAULT),
  _strokeLineJoin(LINE_JOIN_DEFAULT),

  _fillColor(0x00000000),
  _strokeColor(0x00000000),
  _stopColor(0x00000000),

  _opacity(0.0),
  _fillOpacity(0.0),
  _strokeOpacity(0.0),
  _stopOpacity(0.0),

  _strokeDashOffset(0.0, SVG_UNIT_PX),
  _strokeMiterLimit(0.0, SVG_UNIT_PX),
  _strokeWidth(0.0, SVG_UNIT_PX),
  _fontSize(0.0, SVG_UNIT_PX),
  _letterSpacing(0.0, SVG_UNIT_PX)
{
}

SvgStyleAttribute::~SvgStyleAttribute()
{
}

String SvgStyleAttribute::getValue() const
{
  String result;
  result.reserve(128);

  int i;
  for (i = 0; i < SVG_STYLE_INVALID; i++)
  {
    if (_mask & (1 << i))
    {
      result.append(fog_strings->getString(i + STR_SVG_STYLE_NAMES));
      result.append(Char(':'));
      result.append(getStyle(i));
      result.append(Char(';'));
    }
  }

  return result;
}

err_t SvgStyleAttribute::setValue(const String& value)
{
  // Parse all "name: value;" pairs.
  const Char* strCur = value.getData();
  const Char* strEnd = strCur + value.getLength();

  ManagedString styleName;
  String styleValue;

  for (;;)
  {
    if (strCur == strEnd) break;

    const Char* styleNameBegin;
    const Char* styleNameEnd;
    const Char* styleValueBegin;
    const Char* styleValueEnd;

    err_t err;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto bail;
    }

    // Parse style name.
    styleNameBegin = strCur;
    while (*strCur != Char(':') && !strCur->isSpace())
    {
      if (++strCur == strEnd) goto bail;
    }
    styleNameEnd = strCur;

    if (strCur->isSpace())
    {
      while (*strCur != Char(':'))
      {
        if (++strCur == strEnd) goto bail;
      }
    }

    // Skip ':'.
    if (++strCur == strEnd) goto bail;

    // Skip spaces.
    while (strCur->isSpace())
    {
      if (++strCur == strEnd) goto bail;
    }

    // Parse style value.
    styleValueBegin = strCur;
    while (*strCur != Char(';'))
    {
      if (++strCur == strEnd) break;
    }
    styleValueEnd = strCur;

    // Remove trailing spaces.
    //
    // We can't cause buffer underflow, because we already parsed ':' that's
    // not space.
    while (styleValueEnd[-1].isSpace()) styleValueEnd--;

    // Skip ';'.
    if (strCur != strEnd) strCur++;

    err = styleName.set(styleNameBegin, sysuint_t(styleNameEnd - styleNameBegin));
    if (err) continue;

    err = styleValue.set(styleValueBegin, sysuint_t(styleValueEnd - styleValueBegin));
    if (err) continue;

    reinterpret_cast<SvgElement*>(getElement())->setStyle(styleName, styleValue);
  }

bail:
  return ERR_OK;
}

String SvgStyleAttribute::getStyle(int styleId) const
{
  String result;

  // Don't process non-used style values.
  if ((_mask & (1 << styleId)) == 0) goto end;

  switch (styleId)
  {
    case SVG_STYLE_CLIP_PATH:
      // SVG TODO:
      break;

    case SVG_STYLE_CLIP_RULE:
      FOG_ASSERT(_clipRule < FILL_RULE_COUNT);
      result.set(Ascii8(svgEnumList_fillRule[_clipRule].name));
      break;

    case SVG_STYLE_ENABLE_BACKGROUND:
      // SVG TODO:
      break;

    case SVG_STYLE_FILL:
      switch (_fillSource)
      {
        case SVG_SOURCE_NONE:
          result.append(Ascii8("none"));
          break;
        case SVG_SOURCE_COLOR:
          SvgUtil::serializeColor(result, _fillColor);
          break;
        case SVG_SOURCE_URI:
          result = _fillUri;
          break;
      }
      break;

    case SVG_STYLE_FILL_OPACITY:
      result.setDouble(_fillOpacity);
      break;

    case SVG_STYLE_FILL_RULE:
      FOG_ASSERT(_fillRule < FILL_RULE_COUNT);
      result.set(Ascii8(svgEnumList_fillRule[_fillRule].name));
      break;

    case SVG_STYLE_FILTER:
      // SVG TODO:
      break;

    case SVG_STYLE_FONT_FAMILY:
      result.set(_fontFamily);
      break;

    case SVG_STYLE_FONT_SIZE:
      SvgUtil::serializeCoord(result, _fontSize);
      break;

    case SVG_STYLE_LETTER_SPACING:
      SvgUtil::serializeCoord(result, _letterSpacing);
      break;

    case SVG_STYLE_MASK:
      // SVG TODO:
      break;

    case SVG_STYLE_OPACITY:
      result.setDouble(_opacity);
      break;

    case SVG_STYLE_STOP_COLOR:
      SvgUtil::serializeColor(result, _stopColor);
      break;

    case SVG_STYLE_STOP_OPACITY:
      result.setDouble(_stopOpacity);
      break;

    case SVG_STYLE_STROKE:
      switch (_strokeSource)
      {
        case SVG_SOURCE_NONE:
          result.append(Ascii8("none"));
          break;
        case SVG_SOURCE_COLOR:
          SvgUtil::serializeColor(result, _strokeColor);
          break;
        case SVG_SOURCE_URI:
          result = _strokeUri;
          break;
      }
      break;

    case SVG_STYLE_STROKE_DASH_ARRAY:
      // SVG TODO:
      break;

    case SVG_STYLE_STROKE_DASH_OFFSET:
      SvgUtil::serializeCoord(result, _strokeDashOffset);
      break;

    case SVG_STYLE_STROKE_LINE_CAP:
      FOG_ASSERT(_strokeLineCap < LINE_CAP_COUNT);
      result.set(Ascii8(svgEnumList_strokeLineCap[_strokeLineCap].name));
      break;

    case SVG_STYLE_STROKE_LINE_JOIN:
      FOG_ASSERT(_strokeLineJoin < LINE_JOIN_COUNT);
      result.set(Ascii8(svgEnumList_strokeLineJoin[_strokeLineJoin].name));
      break;

    case SVG_STYLE_STROKE_MITER_LIMIT:
      SvgUtil::serializeCoord(result, _strokeMiterLimit);
      break;

    case SVG_STYLE_STROKE_OPACITY:
      result.setDouble(_strokeOpacity);
      break;

    case SVG_STYLE_STROKE_WIDTH:
      SvgUtil::serializeCoord(result, _strokeWidth);
      break;

    default:
      break;
  }
end:
  return result;
}

err_t SvgStyleAttribute::setStyle(int styleId, const String& value)
{
  err_t err = ERR_OK;
  int i;

  switch (styleId)
  {
    case SVG_STYLE_CLIP_PATH:
      // SVG TODO:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;

    case SVG_STYLE_CLIP_RULE:
      i = getSvgEnumId(value, svgEnumList_fillRule);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _clipRule = (uint8_t)(uint)i;
      break;

    case SVG_STYLE_ENABLE_BACKGROUND:
      // SVG TODO:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;

    case SVG_STYLE_FILL:
      _fillSource = (uint8_t)(uint)SvgUtil::parseColor(value, &_fillColor);
      switch (_fillSource)
      {
        case SVG_SOURCE_NONE:
        case SVG_SOURCE_COLOR:
          break;
        case SVG_SOURCE_URI:
          _fillUri = value;
          break;
        case SVG_SOURCE_INVALID:
          err = ERR_SVG_INVALID_STYLE_VALUE;
          break;
      }
      break;

    case SVG_STYLE_FILL_OPACITY:
      err = SvgUtil::parseOpacity(value, &_fillOpacity);
      break;

    case SVG_STYLE_FILL_RULE:
      i = getSvgEnumId(value, svgEnumList_fillRule);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _fillRule = (uint8_t)(uint)i;
      break;

    case SVG_STYLE_FILTER:
      // SVG TODO:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;

    case SVG_STYLE_FONT_FAMILY:
      err = _value.set(value);
      break;

    case SVG_STYLE_FONT_SIZE:
      _fontSize = SvgUtil::parseCoord(value);
      if (_fontSize.unit == SVG_UNIT_INVALID)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      break;

    case SVG_STYLE_LETTER_SPACING:
      _letterSpacing = SvgUtil::parseCoord(value);
      if (_letterSpacing.unit == SVG_UNIT_INVALID)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      break;

    case SVG_STYLE_MASK:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;

    case SVG_STYLE_OPACITY:
      err = SvgUtil::parseOpacity(value, &_opacity);
      break;

    case SVG_STYLE_STOP_COLOR:
      if (SvgUtil::parseColor(value, &_stopColor) != SVG_SOURCE_COLOR)
      {
        err = ERR_SVG_INVALID_STYLE_VALUE;
      }
      break;

    case SVG_STYLE_STOP_OPACITY:
      err = SvgUtil::parseOpacity(value, &_stopOpacity);
      break;

    case SVG_STYLE_STROKE:
      _strokeSource = (uint8_t)SvgUtil::parseColor(value, &_strokeColor);
      switch (_strokeSource)
      {
        case SVG_SOURCE_NONE:
        case SVG_SOURCE_COLOR:
          break;
        case SVG_SOURCE_URI:
          _strokeUri = value;
          break;
        case SVG_SOURCE_INVALID:
          err = ERR_SVG_INVALID_STYLE_VALUE;
          break;
      }
      break;

    case SVG_STYLE_STROKE_DASH_ARRAY:
      err = ERR_RT_NOT_IMPLEMENTED;
      break;

    case SVG_STYLE_STROKE_DASH_OFFSET:
      _strokeDashOffset = SvgUtil::parseCoord(value);
      if (_strokeDashOffset.unit == SVG_UNIT_INVALID)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      break;

    case SVG_STYLE_STROKE_LINE_CAP:
      i = getSvgEnumId(value, svgEnumList_strokeLineCap);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _strokeLineCap = (uint8_t)(uint)i;
      break;

    case SVG_STYLE_STROKE_LINE_JOIN:
      i = getSvgEnumId(value, svgEnumList_strokeLineJoin);
      if (i == -1)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      else
        _strokeLineJoin = (uint8_t)(uint)i;
      break;

    case SVG_STYLE_STROKE_MITER_LIMIT:
      _strokeMiterLimit = SvgUtil::parseCoord(value);
      if (_strokeMiterLimit.unit == SVG_UNIT_INVALID)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      break;

    case SVG_STYLE_STROKE_OPACITY:
      err = SvgUtil::parseOpacity(value, &_strokeOpacity);
      break;

    case SVG_STYLE_STROKE_WIDTH:
      _strokeWidth = SvgUtil::parseCoord(value);
      if (_strokeWidth.unit == SVG_UNIT_INVALID)
        err = ERR_SVG_INVALID_STYLE_VALUE;
      break;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (err == ERR_OK)
    _mask |= (1 << styleId);
  else
    _mask &= ~(1 << styleId);

  return err;
}

// ============================================================================
// [Fog::SvgTransformAttribute]
// ============================================================================

struct FOG_HIDDEN SvgTransformAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgTransformAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgTransformAttribute();

  // [Methods]

  virtual err_t setValue(const String& value);

  // [Data]

  FOG_INLINE const TransformD& getTransform() const { return _transform; }
  FOG_INLINE bool isValid() const { return _isValid; }

  // [Members]
protected:
  TransformD _transform;
  bool _isValid;

private:
  FOG_DISABLE_COPY(SvgTransformAttribute)
};

SvgTransformAttribute::SvgTransformAttribute(XmlElement* element, const ManagedString& name, int offset) :
    XmlAttribute(element, name, offset)
{
}

SvgTransformAttribute::~SvgTransformAttribute()
{
}

err_t SvgTransformAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (err) return err;

  _isValid = (SvgUtil::parseMatrix(value, &_transform) == ERR_OK);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgCoordAttribute]
// ============================================================================

struct FOG_HIDDEN SvgCoordAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgCoordAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgCoordAttribute();

  // [Methods]

  virtual err_t setValue(const String& value);

  // [Coords]

  FOG_INLINE const SvgCoord& getCoord() const { return _coord; }
  FOG_INLINE double getDouble() const { return _coord.value; }
  FOG_INLINE uint32_t getUnit() const { return _coord.unit; }

  // [Members]
protected:
  SvgCoord _coord;

private:
  FOG_DISABLE_COPY(SvgCoordAttribute)
};

SvgCoordAttribute::SvgCoordAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset)
{
  _coord.value = 0.0;
  _coord.unit = SVG_UNIT_NONE;
}

SvgCoordAttribute::~SvgCoordAttribute()
{
}

err_t SvgCoordAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (err) return err;

  _coord = SvgUtil::parseCoord(value);

  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingRectDirty = true;
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgOffsetAttribute]
// ============================================================================

struct FOG_HIDDEN SvgOffsetAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgOffsetAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgOffsetAttribute();

  // [Methods]

  virtual err_t setValue(const String& value);

  // [Coords]

  FOG_INLINE float getOffset() const { return _offset; }

  // [Members]
protected:
  float _offset;

private:
  FOG_DISABLE_COPY(SvgOffsetAttribute)
};

SvgOffsetAttribute::SvgOffsetAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset)
{
  _offset = 0.0;
}

SvgOffsetAttribute::~SvgOffsetAttribute()
{
}

err_t SvgOffsetAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (err) return err;

  sysuint_t end;
  if (value.atof(&_offset, NULL, &end) == ERR_OK)
  {
    if (end < value.getLength() && value.at(end) == Char('%')) _offset *= 0.01f;
    _offset = Math::bound<float>(_offset, 0.0f, 1.0f);
  }
  else
  {
    _offset = 0.0f;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPathAttribute]
// ============================================================================

struct FOG_HIDDEN SvgPathAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgPathAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgPathAttribute();

  // [Methods]

  virtual err_t setValue(const String& value);

  // [Coords]

  FOG_INLINE const PathD& getPath() const { return _path; }

  // [Members]
protected:
  PathD _path;

private:
  FOG_DISABLE_COPY(SvgPathAttribute)
};

SvgPathAttribute::SvgPathAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset)
{
}

SvgPathAttribute::~SvgPathAttribute()
{
}

err_t SvgPathAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (err) return err;

  _path = SvgUtil::parsePath(value);
  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingRectDirty = true;
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPointsAttribute]
// ============================================================================

struct FOG_HIDDEN SvgPointsAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgPointsAttribute(XmlElement* element, const ManagedString& name, bool closePath, int offset);
  virtual ~SvgPointsAttribute();

  // [Methods]

  virtual err_t setValue(const String& value);

  // [Coords]

  FOG_INLINE const PathD& getPath() const { return _path; }

  // [Members]
protected:
  PathD _path;
  bool _closePath;

private:
  FOG_DISABLE_COPY(SvgPointsAttribute)
};

SvgPointsAttribute::SvgPointsAttribute(XmlElement* element, const ManagedString& name, bool closePath, int offset) :
  XmlAttribute(element, name, offset),
  _closePath(closePath)
{
}

SvgPointsAttribute::~SvgPointsAttribute()
{
}

err_t SvgPointsAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (err) return err;

  _path = SvgUtil::parsePoints(value, _closePath);
  if (!_path.isEmpty() && _closePath) _path.closePolygon();

  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingRectDirty = true;
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgEnumAttribute]
// ============================================================================

struct FOG_HIDDEN SvgEnumAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgEnumAttribute(XmlElement* element, const ManagedString& name, int offset, const SvgEnumList* enumList);
  virtual ~SvgEnumAttribute();

  // [Methods]

  virtual err_t setValue(const String& value);

  // [Coords]

  FOG_INLINE int getEnum() const { return _valueEnum; }

  // [Members]
protected:
  const SvgEnumList* _enumList;
  int _valueEnum;

private:
  FOG_DISABLE_COPY(SvgEnumAttribute)
};

SvgEnumAttribute::SvgEnumAttribute(XmlElement* element, const ManagedString& name, int offset, const SvgEnumList* enumList) :
    XmlAttribute(element, name, offset),
    _enumList(enumList),
    _valueEnum(-1)
{
}

SvgEnumAttribute::~SvgEnumAttribute()
{
}

err_t SvgEnumAttribute::setValue(const String& value)
{
  err_t err = _value.set(value);
  if (err) return err;

  _valueEnum = getSvgEnumId(value, _enumList);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgCoordAttribute]
// ============================================================================

struct FOG_HIDDEN SvgImageLinkAttribute : public XmlAttribute
{
  // [Construction / Destruction]

  typedef XmlAttribute base;

  SvgImageLinkAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgImageLinkAttribute();

  // [Methods]

  virtual String getValue() const;
  virtual err_t setValue(const String& value);

  // [Image]

  FOG_INLINE void setImage(const Image& image) { _image = image; }
  FOG_INLINE const Image& getImage() const { return _image; }

  // [Members]
protected:
  Image _image;
  bool _embedded;

private:
  friend struct SvgImageElement;

  FOG_DISABLE_COPY(SvgImageLinkAttribute)
};

SvgImageLinkAttribute::SvgImageLinkAttribute(XmlElement* element, const ManagedString& name, int offset) :
  XmlAttribute(element, name, offset),
  _embedded(false)
{
}

SvgImageLinkAttribute::~SvgImageLinkAttribute()
{
}

String SvgImageLinkAttribute::getValue() const
{
  if (_embedded)
  {
    err_t err = ERR_OK;

    String dst;
    Stream memio;

    err |= dst.append(Ascii8("data:image/png;base64,"));
    err |= memio.openBuffer();

    _image.writeToStream(memio, Ascii8("png"));
    err |= StringUtil::toBase64(dst, memio.getBuffer(), OUTPUT_MODE_APPEND);

    if (err) dst.free();
    return dst;
  }
  else
  {
    return _value;
  }
}

err_t SvgImageLinkAttribute::setValue(const String& value)
{
  err_t err = ERR_OK;

  if (value.startsWith(Ascii8("data:")))
  {
    sysuint_t semicolon = value.indexOf(Char(';'));
    sysuint_t separator = value.indexOf(Char(','));

    if (semicolon != INVALID_INDEX && separator != INVALID_INDEX)
    {
      String type = value.substring(Range(5, semicolon));
      String extension;
      String encoding = value.substring(Range(semicolon + 1, separator));

      ByteArray memio;
      Stream stream;

      if (type == Ascii8("image/png"))
      {
        extension = fog_strings->getString(STR_GRAPHICS_png);
      }
      else if (type == Ascii8("image/jpeg"))
      {
        extension = fog_strings->getString(STR_GRAPHICS_jpeg);
      }
      else
      {
        // Standard talks only about PNG and JPEG, but we can use any attached
        // image that can be decoded by ImageIO.
      }

      if (encoding == Ascii8("base64"))
      {
        err |= StringUtil::fromBase64(memio, value.getData() + separator + 1, value.getLength() - separator - 1);
      }
      else
      {
        // Maybe in future something else will be supported by SVG. For now
        // this is error.
        return ERR_SVG_INVALID_DATA_ENCODING;
      }

      err |= stream.openBuffer(memio);
      err |= _image.readFromStream(stream, extension);

      if (err)
      {
        // Something evil happened. I don't know what to do now, because image
        // seems to be corrupted or unsupported.
        _value.set(value);
        _image.free();
        _embedded = false;
      }
      else
      {
        _value.free();
        _embedded = true;
      }
    }
  }

  err = _value.set(value);
  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingRectDirty = true;
  return err;
}

// ============================================================================
// [Fog::SvgElement]
// ============================================================================

SvgElement::SvgElement(const ManagedString& tagName, uint32_t svgType) :
  XmlElement(tagName),
  _svgType(svgType),
  _boundingRectDirty(true),
  _unused(0)
#if 0
,
  _styles(NULL)
#endif
{
  _type |= SVG_ELEMENT_MASK;
  _flags &= ~(XML_ALLOWED_TAG);
}

SvgElement::~SvgElement()
{
  // Class that inherits us must destroy all attributes.
  FOG_ASSERT(_attributes.isEmpty());
}

SvgElement* SvgElement::clone() const
{
  SvgElement* e = reinterpret_cast<SvgElement*>(SvgDocument::createElementStatic(_tagName));
  if (e) _copyAttributes(e, const_cast<SvgElement*>(this));
  return e;
}

XmlAttribute* SvgElement::_createAttribute(const ManagedString& name) const
{
  return base::_createAttribute(name);
}

static Utf16 parseCssLinkId(const String& url)
{
  const Char* idStr;
  const Char* idEnd;
  const Char* idMark;

  if (url.getLength() < 7) goto bail;

  idStr = url.getData() + 5;
  idEnd = idStr + url.getLength() - 5;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd) goto bail;
  }

  idMark = idStr;
  while (*idStr != Char(')'))
  {
    if (++idStr == idEnd) goto bail;
  }
  return Utf16(idMark, (sysuint_t)(idStr - idMark));

bail:
  return Utf16((const Char*)NULL, 0);
}

static Utf16 parseHtmlLinkId(const String& url)
{
  const Char* idStr;
  const Char* idEnd;
  const Char* idMark;
  Char c;

  if (url.getLength() < 2) goto bail;

  idStr = url.getData();
  idEnd = idStr + url.getLength();

  if (*idStr != Char('#')) goto bail;
  idStr++;

  while (idStr->isSpace())
  {
    if (++idStr == idEnd) goto bail;
  }

  idMark = idStr;
  while ((c = *idStr).isAlnum() || c == Char('_'))
  {
    if (++idStr == idEnd) break;
  }
  return Utf16(idMark, (sysuint_t)(idStr - idMark));

bail:
  return Utf16((const Char*)NULL, 0);
}

err_t SvgElement::onRender(SvgContext* context) const
{
  // Default is to render nothing and stop traversing
  return ERR_OK;
}

err_t SvgElement::onRenderShape(SvgContext* context) const
{
  return ERR_OK;
}

err_t SvgElement::onApplyPattern(SvgContext* context, SvgElement* obj, int paintType) const
{
  return ERR_RT_INVALID_OBJECT;
}

err_t SvgElement::onCalcBoundingBox(RectD* box) const
{
  return ERR_RT_INVALID_OBJECT;
}

err_t SvgElement::_walkAndRender(const XmlElement* root, SvgContext* context)
{
  err_t err = ERR_OK;
  XmlElement* e;

  for (e = root->firstChild(); e; e = e->nextSibling())
  {
    if (e->isSvgElement())
    {
      err = reinterpret_cast<SvgElement*>(e)->onRender(context);
      if (err) break;
    }
    else if (e->hasChildNodes())
    {
      _walkAndRender(e, context);
    }
  }

  return err;
}

String SvgElement::getStyle(const String& name) const
{
  return String();
}

err_t SvgElement::setStyle(const String& name, const String& value)
{
  return ERR_RT_INVALID_OBJECT;
}

const RectD& SvgElement::getBoundingRect() const
{
  if (_boundingRectDirty)
  {
    onCalcBoundingBox(&_boundingRect);
    _boundingRectDirty = false;
  }

  return _boundingRect;
}

// ============================================================================
// [Fog::SvgStylableElement]
// ============================================================================

// This is not final element, must be overriden.
struct FOG_HIDDEN SvgStyledElement : public SvgElement
{
  // [Construction / Destruction]

  typedef SvgElement base;

  SvgStyledElement(const ManagedString& tagName, uint32_t svgType);
  virtual ~SvgStyledElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  virtual err_t _setAttribute(const ManagedString& name, const String& value);
  virtual err_t _removeAttribute(const ManagedString& name);

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;

  // [SVG/CSS Styles]

  virtual String getStyle(const String& name) const;
  virtual err_t setStyle(const String& name, const String& value);

  // [Embedded Attributes]

  SvgStyleAttribute a_style;
  SvgTransformAttribute a_transform;

private:
  FOG_DISABLE_COPY(SvgStyledElement)
};

SvgStyledElement::SvgStyledElement(const ManagedString& tagName, uint32_t svgType) :
  SvgElement(tagName, svgType),
  a_style(this, fog_strings->getString(STR_XML_style), FOG_OFFSET_OF(SvgStyledElement, a_style)),
  a_transform(NULL, fog_strings->getString(STR_SVG_transform), FOG_OFFSET_OF(SvgStyledElement, a_transform))
{
  // Style attribute is always added as default and can't be removed.
  _attributes.append(&a_style);
}

SvgStyledElement::~SvgStyledElement()
{
  // Class that inherits us must destroy all attributes.
  // (a_style must be removed from _attributes too)
  FOG_ASSERT(_attributes.isEmpty());
}

XmlAttribute* SvgStyledElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_XML_style)) return (XmlAttribute*)&a_style;
  if (name == fog_strings->getString(STR_SVG_transform)) return (XmlAttribute*)&a_transform;

  return base::_createAttribute(name);
}

err_t SvgStyledElement::_setAttribute(const ManagedString& name, const String& value)
{
  // Add css-style instead of attribute.
  int id = styleToId(name);
  if ((uint)id < SVG_STYLE_INVALID)
  {
    // Do not return an error value here, because if Xml/Svg parser see error
    // then parsing is over. This means that there is probability we can't
    // render correctly some image, but we want to render what we can.
    a_style.setStyle(id, value);
    return ERR_OK;
  }

  return base::_setAttribute(name, value);
}

err_t SvgStyledElement::_removeAttribute(const ManagedString& name)
{
  if (name == fog_strings->getString(STR_XML_style)) return ERR_XML_ATTRIBUTE_CANT_BE_REMOVED;

  return base::_removeAttribute(name);
}

err_t SvgStyledElement::onRender(SvgContext* context) const
{
  SvgContextBackup backup;
  err_t err = ERR_OK;

  uint32_t styleMask = a_style.getMask();
  bool transformed = a_transform.isAssigned() & a_transform.isValid();

  // Before render: Apply transformations and setup styles defined by this element.
  if (styleMask != 0 || transformed)
  {
    backup.init(context);

    // Transformations.
    if (transformed)
    {
      backup._transform = context->getPainter()->getTransform();
      backup._transformBackup = true;

      context->getPainter()->transform(a_transform.getTransform());
    }

    // Setup fill parameters.
    if (styleMask & (1 << SVG_STYLE_FILL               ) |
                    (1 << SVG_STYLE_FILL_OPACITY       ) |
                    (1 << SVG_STYLE_FILL_RULE          ) )
    {
      if (styleMask & (1 << SVG_STYLE_FILL))
      {
        switch (a_style._fillSource)
        {
          case SVG_SOURCE_NONE:
          {
            context->setFillNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            context->setFillColor(a_style._fillColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._fillUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onApplyPattern(context, const_cast<SvgStyledElement*>(this), SVG_PAINT_FILL);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_FILL_OPACITY))
      {
        context->setFillOpacity(a_style._fillOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_FILL_RULE))
      {
        context->setFillRule(a_style._fillRule);
      }
    }

    // Setup stroke parameters.
    if (styleMask & (1 << SVG_STYLE_STROKE             ) |
                    (1 << SVG_STYLE_STROKE_DASH_ARRAY  ) |
                    (1 << SVG_STYLE_STROKE_DASH_OFFSET ) |
                    (1 << SVG_STYLE_STROKE_LINE_CAP    ) |
                    (1 << SVG_STYLE_STROKE_LINE_JOIN   ) |
                    (1 << SVG_STYLE_STROKE_MITER_LIMIT ) |
                    (1 << SVG_STYLE_STROKE_OPACITY     ) |
                    (1 << SVG_STYLE_STROKE_WIDTH       ) )
    {
      if (styleMask & (1 << SVG_STYLE_STROKE))
      {
        switch (a_style._strokeSource)
        {
          case SVG_SOURCE_NONE:
          {
            context->setStrokeNone();
            break;
          }
          case SVG_SOURCE_COLOR:
          {
            context->setStrokeColor(a_style._strokeColor);
            break;
          }
          case SVG_SOURCE_URI:
          {
            XmlElement* r = getDocument()->getElementById(parseCssLinkId(a_style._strokeUri));
            if (r && r->isSvg())
            {
              reinterpret_cast<SvgElement*>(r)->onApplyPattern(context, const_cast<SvgStyledElement*>(this), SVG_PAINT_STROKE);
            }
            break;
          }
        }
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_CAP))
      {
        context->setLineCaps(a_style._strokeLineCap);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_LINE_JOIN))
      {
        context->setLineJoin(a_style._strokeLineJoin);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_MITER_LIMIT))
      {
        double miterLimit = context->translateCoord(
          a_style._strokeMiterLimit.value, a_style._strokeMiterLimit.unit);
        context->setMiterLimit(miterLimit);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_OPACITY))
      {
        context->setStrokeOpacity(a_style._strokeOpacity);
      }

      if (styleMask & (1 << SVG_STYLE_STROKE_WIDTH))
      {
        double lineWidth = context->translateCoord(
          a_style._strokeWidth.value, a_style._strokeWidth.unit);
        context->setLineWidth(lineWidth);
      }
    }
  }

  // Actual render: object or children (depends to Svg type).
  err = onRenderShape(context);

  // After render: SvgContextBackup destructor will restore SvgContext state
  // if modified.
  return err;
}

String SvgStyledElement::getStyle(const String& name) const
{
  ManagedString managedName;
  String result;
  int id;

  if (managedName.setIfManaged(name) == ERR_OK &&
      (uint)(id = styleToId(managedName)) < SVG_STYLE_INVALID)
  {
    result = a_style.getStyle(id);
  }

  return result;
}

err_t SvgStyledElement::setStyle(const String& name, const String& value)
{
  ManagedString managedName;
  err_t err = ERR_SVG_INVALID_STYLE_NAME;
  int id;

  if (managedName.setIfManaged(name) == ERR_OK &&
      (uint)(id = styleToId(managedName)) < SVG_STYLE_INVALID)
  {
    err = a_style.setStyle(id, value);
  }

fail:
  return err;
}

// ============================================================================
// [Fog::SvgCircleElement]
// ============================================================================

struct FOG_HIDDEN SvgCircleElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgCircleElement();
  virtual ~SvgCircleElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_r;

private:
  FOG_DISABLE_COPY(SvgCircleElement)
};

SvgCircleElement::SvgCircleElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_circle), SVG_ELEMENT_CIRCLE),
  a_cx(NULL, fog_strings->getString(STR_SVG_cx), FOG_OFFSET_OF(SvgCircleElement, a_cx)),
  a_cy(NULL, fog_strings->getString(STR_SVG_cy), FOG_OFFSET_OF(SvgCircleElement, a_cy)),
  a_r (NULL, fog_strings->getString(STR_SVG_r ), FOG_OFFSET_OF(SvgCircleElement, a_r ))
{
}

SvgCircleElement::~SvgCircleElement()
{
  _removeAttributes();
}

XmlAttribute* SvgCircleElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_cx)) return (XmlAttribute*)&a_cx;
  if (name == fog_strings->getString(STR_SVG_cy)) return (XmlAttribute*)&a_cy;
  if (name == fog_strings->getString(STR_SVG_r )) return (XmlAttribute*)&a_r;

  return base::_createAttribute(name);
}

err_t SvgCircleElement::onRenderShape(SvgContext* context) const
{
  if (a_r.isAssigned())
  {
    double cx = a_cx.isAssigned() ? a_cx.getCoord().value : 0.0;
    double cy = a_cy.isAssigned() ? a_cy.getCoord().value : 0.0;
    double r = Math::abs(a_r.getCoord().value);

    if (r <= 0.0) return ERR_OK;

    context->drawEllipse(PointD(cx, cy), PointD(r, r));
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgCircleElement::onCalcBoundingBox(RectD* box) const
{
  if (a_r.isAssigned())
  {
    double cx = a_cx.isAssigned() ? a_cx.getCoord().value : 0.0;
    double cy = a_cy.isAssigned() ? a_cy.getCoord().value : 0.0;
    double r = Math::abs(a_r.getCoord().value);

    if (r <= 0.0) goto fail;

    box->set(cx - r, cy - r, r * 2.0, r * 2.0);
    return ERR_OK;
  }

fail:
  box->reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgDefsElement]
// ============================================================================

struct FOG_HIDDEN SvgDefsElement : public SvgElement
{
  // [Construction / Destruction]

  typedef SvgElement base;

  SvgDefsElement();
  virtual ~SvgDefsElement();

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;

private:
  FOG_DISABLE_COPY(SvgDefsElement)
};

SvgDefsElement::SvgDefsElement() : 
  SvgElement(fog_strings->getString(STR_SVG_defs), SVG_ELEMENT_DEFS)
{
}

SvgDefsElement::~SvgDefsElement()
{
  _removeAttributes();
}

err_t SvgDefsElement::onRender(SvgContext* context) const
{
  // <defs> section is used only to define shared resources or gradients.
  // Don't go inside.
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgEllipseElement]
// ============================================================================

struct FOG_HIDDEN SvgEllipseElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgEllipseElement();
  virtual ~SvgEllipseElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_rx;
  SvgCoordAttribute a_ry;

private:
  FOG_DISABLE_COPY(SvgEllipseElement)
};

SvgEllipseElement::SvgEllipseElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_ellipse), SVG_ELEMENT_ELLIPSE),
  a_cx(NULL, fog_strings->getString(STR_SVG_cx), FOG_OFFSET_OF(SvgEllipseElement, a_cx)),
  a_cy(NULL, fog_strings->getString(STR_SVG_cy), FOG_OFFSET_OF(SvgEllipseElement, a_cy)),
  a_rx(NULL, fog_strings->getString(STR_SVG_rx), FOG_OFFSET_OF(SvgEllipseElement, a_rx)),
  a_ry(NULL, fog_strings->getString(STR_SVG_ry), FOG_OFFSET_OF(SvgEllipseElement, a_ry))
{
}

SvgEllipseElement::~SvgEllipseElement()
{
  _removeAttributes();
}

XmlAttribute* SvgEllipseElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_cx)) return (XmlAttribute*)&a_cx;
  if (name == fog_strings->getString(STR_SVG_cy)) return (XmlAttribute*)&a_cy;
  if (name == fog_strings->getString(STR_SVG_rx)) return (XmlAttribute*)&a_rx;
  if (name == fog_strings->getString(STR_SVG_ry)) return (XmlAttribute*)&a_ry;

  return base::_createAttribute(name);
}

err_t SvgEllipseElement::onRenderShape(SvgContext* context) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    double cx = a_cx.isAssigned() ? a_cx.getCoord().value : 0.0;
    double cy = a_cy.isAssigned() ? a_cy.getCoord().value : 0.0;
    double rx = Math::abs(a_rx.getCoord().value);
    double ry = Math::abs(a_ry.getCoord().value);

    if (rx <= 0.0 || ry <= 0.0) return ERR_OK;

    context->drawEllipse(PointD(cx, cy), PointD(rx, ry));
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgEllipseElement::onCalcBoundingBox(RectD* box) const
{
  if (a_rx.isAssigned() && a_ry.isAssigned())
  {
    double cx = a_cx.isAssigned() ? a_cx.getCoord().value : 0.0;
    double cy = a_cy.isAssigned() ? a_cy.getCoord().value : 0.0;

    double rx = a_rx.getDouble();
    double ry = a_ry.getDouble();

    if (rx <= 0.0 || ry <= 0.0) goto fail;

    box->set(cx - rx, cy - ry, rx * 2.0, ry * 2.0);
    return ERR_OK;
  }

fail:
  box->reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgGElement]
// ============================================================================

struct FOG_HIDDEN SvgGElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgGElement();
  virtual ~SvgGElement();

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;

private:
  FOG_DISABLE_COPY(SvgGElement)
};

SvgGElement::SvgGElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_g), SVG_ELEMENT_G)
{
}

SvgGElement::~SvgGElement()
{
  _removeAttributes();
}

err_t SvgGElement::onRenderShape(SvgContext* context) const
{
  if (hasChildNodes())
    return _walkAndRender(this, context);
  else
    return ERR_OK;
}

// ============================================================================
// [Fog::SvgLineElement]
// ============================================================================

struct FOG_HIDDEN SvgLineElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgLineElement();
  virtual ~SvgLineElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

  SvgCoordAttribute a_x1;
  SvgCoordAttribute a_y1;
  SvgCoordAttribute a_x2;
  SvgCoordAttribute a_y2;

private:
  FOG_DISABLE_COPY(SvgLineElement)
};

SvgLineElement::SvgLineElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_line), SVG_ELEMENT_LINE),
  a_x1(NULL, fog_strings->getString(STR_SVG_x1), FOG_OFFSET_OF(SvgLineElement, a_x1)),
  a_y1(NULL, fog_strings->getString(STR_SVG_y1), FOG_OFFSET_OF(SvgLineElement, a_y1)),
  a_x2(NULL, fog_strings->getString(STR_SVG_x2), FOG_OFFSET_OF(SvgLineElement, a_x2)),
  a_y2(NULL, fog_strings->getString(STR_SVG_y2), FOG_OFFSET_OF(SvgLineElement, a_y2))
{
}

SvgLineElement::~SvgLineElement()
{
  _removeAttributes();
}

XmlAttribute* SvgLineElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_x1)) return (XmlAttribute*)&a_x1;
  if (name == fog_strings->getString(STR_SVG_y1)) return (XmlAttribute*)&a_y1;
  if (name == fog_strings->getString(STR_SVG_x2)) return (XmlAttribute*)&a_x2;
  if (name == fog_strings->getString(STR_SVG_y2)) return (XmlAttribute*)&a_y2;

  return base::_createAttribute(name);
}

err_t SvgLineElement::onRenderShape(SvgContext* context) const
{
  double x1 = a_x1.isAssigned() ? a_x1.getCoord().value : 0.0;
  double y1 = a_y1.isAssigned() ? a_y1.getCoord().value : 0.0;
  double x2 = a_x2.isAssigned() ? a_x2.getCoord().value : 0.0;
  double y2 = a_y2.isAssigned() ? a_y2.getCoord().value : 0.0;

  context->drawLine(PointD(x1, y1), PointD(x2, y2));
  return ERR_OK;
}

err_t SvgLineElement::onCalcBoundingBox(RectD* box) const
{
  double x1 = a_x1.isAssigned() ? a_x1.getCoord().value : 0.0;
  double y1 = a_y1.isAssigned() ? a_y1.getCoord().value : 0.0;
  double x2 = a_x2.isAssigned() ? a_x2.getCoord().value : 0.0;
  double y2 = a_y2.isAssigned() ? a_y2.getCoord().value : 0.0;

  double x = (x1 < x2) ? x1 : x2;
  double y = (y1 < y2) ? y1 : y2;
  double w = (x1 < x2) ? x2 - x1 : x1 - x2;
  double h = (y1 < y2) ? y2 - y1 : y1 - y2;

  box->set(x, y, w, h);
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgPathElement]
// ============================================================================

struct FOG_HIDDEN SvgPathElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgPathElement();
  virtual ~SvgPathElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

  SvgPathAttribute a_d;

private:
  FOG_DISABLE_COPY(SvgPathElement)
};

SvgPathElement::SvgPathElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_path), SVG_ELEMENT_PATH),
  a_d(NULL, fog_strings->getString(STR_SVG_d), FOG_OFFSET_OF(SvgPathElement, a_d))
{
}

SvgPathElement::~SvgPathElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPathElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_d)) return (XmlAttribute*)&a_d;

  return base::_createAttribute(name);
}

err_t SvgPathElement::onRenderShape(SvgContext* context) const
{
  if (a_d.isAssigned())
  {
    const PathD& path = a_d.getPath();
    context->drawPath(path);
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgPathElement::onCalcBoundingBox(RectD* box) const
{
  if (a_d.isAssigned())
  {
    const PathD& path = a_d.getPath();
    box->set(path.getBoundingRect());
    return ERR_OK;
  }
  else
  {
    box->reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgPolygonElement]
// ============================================================================

struct FOG_HIDDEN SvgPolygonElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgPolygonElement();
  virtual ~SvgPolygonElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

  SvgPointsAttribute a_points;

private:
  FOG_DISABLE_COPY(SvgPolygonElement)
};

SvgPolygonElement::SvgPolygonElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_polygon), SVG_ELEMENT_POLYGON),
  a_points(NULL, fog_strings->getString(STR_SVG_points), true, FOG_OFFSET_OF(SvgPolygonElement, a_points))
{
}

SvgPolygonElement::~SvgPolygonElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPolygonElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_points)) return (XmlAttribute*)&a_points;

  return base::_createAttribute(name);
}

err_t SvgPolygonElement::onRenderShape(SvgContext* context) const
{
  if (a_points.isAssigned())
  {
    const PathD& path = a_points.getPath();
    context->drawPath(path);
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgPolygonElement::onCalcBoundingBox(RectD* box) const
{
  if (a_points.isAssigned())
  {
    const PathD& path = a_points.getPath();
    box->set(path.getBoundingRect());
    return ERR_OK;
  }
  else
  {
    box->reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgPolyLineElement]
// ============================================================================

struct FOG_HIDDEN SvgPolyLineElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgPolyLineElement();
  virtual ~SvgPolyLineElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

  SvgPointsAttribute a_points;

private:
  FOG_DISABLE_COPY(SvgPolyLineElement)
};

SvgPolyLineElement::SvgPolyLineElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_polyline), SVG_ELEMENT_POLYLINE),
  a_points(NULL, fog_strings->getString(STR_SVG_points), false, FOG_OFFSET_OF(SvgPolygonElement, a_points))
{
}

SvgPolyLineElement::~SvgPolyLineElement()
{
  _removeAttributes();
}

XmlAttribute* SvgPolyLineElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_points)) return (XmlAttribute*)&a_points;

  return base::_createAttribute(name);
}

err_t SvgPolyLineElement::onRenderShape(SvgContext* context) const
{
  if (a_points.isAssigned())
  {
    const PathD& path = a_points.getPath();
    context->drawPath(path);
    return ERR_OK;
  }
  else
  {
    return ERR_OK;
  }
}

err_t SvgPolyLineElement::onCalcBoundingBox(RectD* box) const
{
  if (a_points.isAssigned())
  {
    const PathD& path = a_points.getPath();
    box->set(path.getBoundingRect());
    return ERR_OK;
  }
  else
  {
    box->reset();
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgRectElement]
// ============================================================================

struct FOG_HIDDEN SvgRectElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgRectElement();
  virtual ~SvgRectElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgCoordAttribute a_rx;
  SvgCoordAttribute a_ry;

private:
  FOG_DISABLE_COPY(SvgRectElement)
};

SvgRectElement::SvgRectElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_rect), SVG_ELEMENT_RECT),
  a_x     (NULL, fog_strings->getString(STR_SVG_x     ), FOG_OFFSET_OF(SvgRectElement, a_x     )),
  a_y     (NULL, fog_strings->getString(STR_SVG_y     ), FOG_OFFSET_OF(SvgRectElement, a_y     )),
  a_width (NULL, fog_strings->getString(STR_SVG_width ), FOG_OFFSET_OF(SvgRectElement, a_width )),
  a_height(NULL, fog_strings->getString(STR_SVG_height), FOG_OFFSET_OF(SvgRectElement, a_height)),
  a_rx    (NULL, fog_strings->getString(STR_SVG_rx    ), FOG_OFFSET_OF(SvgRectElement, a_rx    )),
  a_ry    (NULL, fog_strings->getString(STR_SVG_ry    ), FOG_OFFSET_OF(SvgRectElement, a_ry    ))
{
}

SvgRectElement::~SvgRectElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRectElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_x     )) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_y     )) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_width )) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_height)) return (XmlAttribute*)&a_height;
  if (name == fog_strings->getString(STR_SVG_rx    )) return (XmlAttribute*)&a_rx;
  if (name == fog_strings->getString(STR_SVG_ry    )) return (XmlAttribute*)&a_ry;

  return base::_createAttribute(name);
}

err_t SvgRectElement::onRenderShape(SvgContext* context) const
{
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    double w = a_width.getCoord().value;
    double h = a_height.getCoord().value;
    if (w <= 0.0 || h <= 0.0) goto fail;

    double x = a_x.isAssigned() ? a_x.getCoord().value : 0.0;
    double y = a_y.isAssigned() ? a_y.getCoord().value : 0.0;

    double rx = a_rx.isAssigned() ? a_rx.getCoord().value : 0.0;
    double ry = a_ry.isAssigned() ? a_ry.getCoord().value : 0.0;

    if (!a_rx.isAssigned() && a_ry.isAssigned()) rx = ry;
    if (!a_ry.isAssigned() && a_rx.isAssigned()) ry = rx;

    if (rx <= 0.0 || ry <= 0.0)
      context->drawRect(RectD(x, y, w, h));
    else
      context->drawRound(RectD(x, y, w, h), PointD(rx, ry));
  }

fail:
  return ERR_OK;
}

err_t SvgRectElement::onCalcBoundingBox(RectD* box) const
{
  if (a_width.isAssigned() && a_height.isAssigned())
  {
    double w = a_width.getCoord().value;
    double h = a_height.getCoord().value;
    if (w < 0.0 || h < 0.0) goto fail;

    double x = a_x.isAssigned() ? a_x.getCoord().value : 0.0;
    double y = a_y.isAssigned() ? a_y.getCoord().value : 0.0;

    box->set(x, y, w, h);
    return ERR_OK;
  }

fail:
  box->reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

struct FOG_HIDDEN SvgUseElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgUseElement();
  virtual ~SvgUseElement();

  // [SVG Rendering]

  virtual err_t onRenderShape(SvgContext* context) const;
  virtual err_t onCalcBoundingBox(RectD* box) const;

  // [Embedded Attributes]

private:
  FOG_DISABLE_COPY(SvgUseElement)
};

SvgUseElement::SvgUseElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_rect), SVG_ELEMENT_USE)
{
}

SvgUseElement::~SvgUseElement()
{
  _removeAttributes();
}

err_t SvgUseElement::onRenderShape(SvgContext* context) const
{
  String link = _getAttribute(fog_strings->getString(STR_SVG_xlink_href));
  XmlElement* ref = getDocument()->getElementById(parseHtmlLinkId(link));

  if (ref && ref->isSvgElement())
  {
    reinterpret_cast<SvgElement*>(ref)->onRender(context);
    return ERR_OK;
  }

  return ERR_OK;
}

err_t SvgUseElement::onCalcBoundingBox(RectD* box) const
{
  box->reset();
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgSolidColorElement]
// ============================================================================

struct FOG_HIDDEN SvgSolidColorElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgSolidColorElement();
  virtual ~SvgSolidColorElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;

  // [Embedded Attributes]

private:
  FOG_DISABLE_COPY(SvgSolidColorElement)
};

SvgSolidColorElement::SvgSolidColorElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_solidColor), SVG_ELEMENT_SOLID_COLOR)
{
}

SvgSolidColorElement::~SvgSolidColorElement()
{
  _removeAttributes();
}

XmlAttribute* SvgSolidColorElement::_createAttribute(const ManagedString& name) const
{
  return base::_createAttribute(name);
}

err_t SvgSolidColorElement::onRender(SvgContext* context) const
{
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgStopElement]
// ============================================================================

struct FOG_HIDDEN SvgStopElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgStopElement();
  virtual ~SvgStopElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;

  // [Embedded Attributes]

  SvgOffsetAttribute a_offset;

private:
  FOG_DISABLE_COPY(SvgStopElement)
};

SvgStopElement::SvgStopElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_stop), SVG_ELEMENT_STOP),
  a_offset(NULL, fog_strings->getString(STR_SVG_offset), FOG_OFFSET_OF(SvgStopElement, a_offset))
{
}

SvgStopElement::~SvgStopElement()
{
  _removeAttributes();
}

XmlAttribute* SvgStopElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_offset)) return (XmlAttribute*)&a_offset;

  return base::_createAttribute(name);
}

err_t SvgStopElement::onRender(SvgContext* context) const
{
  return ERR_OK;
}

// ============================================================================
// [Fog::SvgAbstractGradientElement]
// ============================================================================

struct FOG_HIDDEN SvgAbstractGradientElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgAbstractGradientElement(const ManagedString& tagName, uint32_t svgType);
  virtual ~SvgAbstractGradientElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [Embedded Attributes]

  SvgEnumAttribute a_spreadMethod;
  SvgEnumAttribute a_gradientUnits;
  SvgTransformAttribute a_gradientTransform;

private:
  FOG_DISABLE_COPY(SvgAbstractGradientElement)
};

SvgAbstractGradientElement::SvgAbstractGradientElement(const ManagedString& tagName, uint32_t svgType) :
  SvgStyledElement(tagName, svgType),
  a_spreadMethod(NULL, fog_strings->getString(STR_SVG_spreadMethod), FOG_OFFSET_OF(SvgAbstractGradientElement, a_spreadMethod), svgEnumList_spreadMethod),
  a_gradientUnits(NULL, fog_strings->getString(STR_SVG_gradientUnits), FOG_OFFSET_OF(SvgAbstractGradientElement, a_gradientUnits), svgEnumList_gradientUnits),
  a_gradientTransform(NULL, fog_strings->getString(STR_SVG_gradientTransform), FOG_OFFSET_OF(SvgAbstractGradientElement, a_gradientTransform))
{
}

SvgAbstractGradientElement::~SvgAbstractGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgAbstractGradientElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_spreadMethod)) return (XmlAttribute*)&a_spreadMethod;
  if (name == fog_strings->getString(STR_SVG_gradientUnits)) return (XmlAttribute*)&a_gradientUnits;
  if (name == fog_strings->getString(STR_SVG_gradientTransform)) return (XmlAttribute*)&a_gradientTransform;

  return base::_createAttribute(name);
}

static void walkAndAddArgbStops(XmlElement* root, Pattern& pattern)
{
  bool stopsParsed = false;
  XmlElement* stop;
  int depth = 0;

start:
  for (stop = root->firstChild(); stop; stop = stop->nextSibling())
  {
    if (stop->isSvg() && reinterpret_cast<SvgElement*>(stop)->getSvgType() == SVG_ELEMENT_STOP)
    {
      SvgStopElement* _stop = reinterpret_cast<SvgStopElement*>(stop);

      if (_stop->a_offset.isAssigned() && _stop->a_style.hasStyle(SVG_STYLE_STOP_COLOR))
      {
        float offset = _stop->a_offset.getOffset();
        ArgbI color = _stop->a_style._stopColor;

        if (_stop->a_style.hasStyle(SVG_STYLE_STOP_OPACITY))
        {
          color.setAlpha((int)(_stop->a_style._stopOpacity * 255));
        }

        pattern.addStop(ArgbStop(offset, color));
        stopsParsed = true;
      }
    }
  }

  if (!stopsParsed)
  {
    XmlElement* e;
    String link = root->_getAttribute(fog_strings->getString(STR_SVG_xlink_href));

    if ((!link.isEmpty() && link.at(0) == Char('#')) && 
        (e = root->getDocument()->getElementById(Utf16(link.getData() + 1, link.getLength() - 1))))
    {
      root = e;
      if (++depth == 32) return;
      goto start;
    }
  }
}

// ============================================================================
// [Fog::SvgLinearGradientElement]
// ============================================================================

struct FOG_HIDDEN SvgLinearGradientElement : public SvgAbstractGradientElement
{
  // [Construction / Destruction]

  typedef SvgAbstractGradientElement base;

  SvgLinearGradientElement();
  virtual ~SvgLinearGradientElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;
  virtual err_t onApplyPattern(SvgContext* context, SvgElement* obj, int paintType) const;

  // [Embedded Attributes]

  SvgCoordAttribute a_x1;
  SvgCoordAttribute a_y1;
  SvgCoordAttribute a_x2;
  SvgCoordAttribute a_y2;

private:
  FOG_DISABLE_COPY(SvgLinearGradientElement)
};

SvgLinearGradientElement::SvgLinearGradientElement() :
  SvgAbstractGradientElement(fog_strings->getString(STR_SVG_linearGradient), SVG_ELEMENT_LINEAR_GRADIENT),
  a_x1(NULL, fog_strings->getString(STR_SVG_x1), FOG_OFFSET_OF(SvgLinearGradientElement, a_x1)),
  a_y1(NULL, fog_strings->getString(STR_SVG_y1), FOG_OFFSET_OF(SvgLinearGradientElement, a_y1)),
  a_x2(NULL, fog_strings->getString(STR_SVG_x2), FOG_OFFSET_OF(SvgLinearGradientElement, a_x2)),
  a_y2(NULL, fog_strings->getString(STR_SVG_y2), FOG_OFFSET_OF(SvgLinearGradientElement, a_y2))
{
}

SvgLinearGradientElement::~SvgLinearGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgLinearGradientElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_x1)) return (XmlAttribute*)&a_x1;
  if (name == fog_strings->getString(STR_SVG_y1)) return (XmlAttribute*)&a_y1;
  if (name == fog_strings->getString(STR_SVG_x2)) return (XmlAttribute*)&a_x2;
  if (name == fog_strings->getString(STR_SVG_y2)) return (XmlAttribute*)&a_y2;

  return base::_createAttribute(name);
}

err_t SvgLinearGradientElement::onRender(SvgContext* context) const
{
  return ERR_OK;
}

err_t SvgLinearGradientElement::onApplyPattern(SvgContext* context, SvgElement* obj, int paintType) const
{
  // SVG TODO: Linear Gradient.

  Pattern pattern;

  // Set gradient type to linear gradient.
  pattern.setType(PATTERN_LINEAR_GRADIENT);

  // Set gradient transformation matrix.
  if (a_gradientTransform.isAssigned()) pattern.setTransform(a_gradientTransform.getTransform());

  // Set spread method.
  if (a_spreadMethod.isAssigned()) pattern.setSpread(a_spreadMethod.getEnum());

  // Setup start and end points.
  if (!a_gradientUnits.isAssigned() || a_gradientUnits.getEnum() == SVG_OBJECT_BOUNDING_BOX)
  {
    // BoundingBox coordinates.
    RectD bbox = obj->getBoundingRect();

    double x1 = a_x1.isAssigned() ? context->translateCoord(a_x1.getDouble(), a_x1.getUnit()) : 0.0;
    double y1 = a_y1.isAssigned() ? context->translateCoord(a_y1.getDouble(), a_y1.getUnit()) : 0.0;
    double x2 = a_x2.isAssigned() ? context->translateCoord(a_x2.getDouble(), a_x2.getUnit()) : 1.0;
    double y2 = a_y2.isAssigned() ? context->translateCoord(a_y2.getDouble(), a_y2.getUnit()) : 0.0;

    x1 = bbox.x + bbox.w * x1;
    y1 = bbox.y + bbox.h * y1;
    x2 = bbox.x + bbox.w * x2;
    y2 = bbox.y + bbox.h * y2;

    pattern.setPoints(PointD(x1, y1), PointD(x2, y2));
  }
  else if (a_x1.isAssigned() && a_y1.isAssigned() && a_x2.isAssigned() && a_y2.isAssigned())
  {
    // UserSpaceOnUse coordinates.
    double x1 = context->translateCoord(a_x1.getDouble(), a_x1.getUnit());
    double y1 = context->translateCoord(a_y1.getDouble(), a_y1.getUnit());
    double x2 = context->translateCoord(a_x2.getDouble(), a_x2.getUnit());
    double y2 = context->translateCoord(a_y2.getDouble(), a_y2.getUnit());

    pattern.setPoints(PointD(x1, y1), PointD(x2, y2));
  }
  else
  {
    // SVG TODO: Is this error?
    fog_debug("Fog::SvgLinearGradient - Unsupported combination...");
  }

  // Add color stops.
  walkAndAddArgbStops(const_cast<SvgLinearGradientElement*>(this), pattern);

  if (paintType == SVG_PAINT_FILL)
    context->setFillPattern(pattern);
  else
    context->setStrokePattern(pattern);

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgRadialGradientElement]
// ============================================================================

struct FOG_HIDDEN SvgRadialGradientElement : public SvgAbstractGradientElement
{
  // [Construction / Destruction]

  typedef SvgAbstractGradientElement base;

  SvgRadialGradientElement();
  virtual ~SvgRadialGradientElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;
  virtual err_t onApplyPattern(SvgContext* context, SvgElement* obj, int paintType) const;

  // [Embedded Attributes]

  SvgCoordAttribute a_cx;
  SvgCoordAttribute a_cy;
  SvgCoordAttribute a_fx;
  SvgCoordAttribute a_fy;
  SvgCoordAttribute a_r;

private:
  FOG_DISABLE_COPY(SvgRadialGradientElement)
};

SvgRadialGradientElement::SvgRadialGradientElement() :
  SvgAbstractGradientElement(fog_strings->getString(STR_SVG_radialGradient), SVG_ELEMENT_RADIAL_GRADIENT),
  a_cx(NULL, fog_strings->getString(STR_SVG_cx), FOG_OFFSET_OF(SvgRadialGradientElement, a_cx)),
  a_cy(NULL, fog_strings->getString(STR_SVG_cy), FOG_OFFSET_OF(SvgRadialGradientElement, a_cy)),
  a_fx(NULL, fog_strings->getString(STR_SVG_fx), FOG_OFFSET_OF(SvgRadialGradientElement, a_fx)),
  a_fy(NULL, fog_strings->getString(STR_SVG_fy), FOG_OFFSET_OF(SvgRadialGradientElement, a_fx)),
  a_r(NULL, fog_strings->getString(STR_SVG_r), FOG_OFFSET_OF(SvgRadialGradientElement, a_r))
{
}

SvgRadialGradientElement::~SvgRadialGradientElement()
{
  _removeAttributes();
}

XmlAttribute* SvgRadialGradientElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_cx)) return (XmlAttribute*)&a_cx;
  if (name == fog_strings->getString(STR_SVG_cy)) return (XmlAttribute*)&a_cy;
  if (name == fog_strings->getString(STR_SVG_fx)) return (XmlAttribute*)&a_fx;
  if (name == fog_strings->getString(STR_SVG_fy)) return (XmlAttribute*)&a_fy;
  if (name == fog_strings->getString(STR_SVG_r)) return (XmlAttribute*)&a_r;

  return base::_createAttribute(name);
}

err_t SvgRadialGradientElement::onRender(SvgContext* context) const
{
  return ERR_OK;
}

err_t SvgRadialGradientElement::onApplyPattern(SvgContext* context, SvgElement* obj, int paintType) const
{
  // SVG TODO: Radial Gradient.

  Pattern pattern;

  // Set gradient type to radial gradient.
  pattern.setType(PATTERN_RADIAL_GRADIENT);

  // Set gradient transformation matrix.
  if (a_gradientTransform.isAssigned()) pattern.setTransform(a_gradientTransform.getTransform());

  // Set spread method.
  if (a_spreadMethod.isAssigned()) pattern.setSpread(a_spreadMethod.getEnum());

  // Setup start and end points.
  if (!a_gradientUnits.isAssigned() || a_gradientUnits.getEnum() == SVG_OBJECT_BOUNDING_BOX)
  {
    RectD bbox = obj->getBoundingRect();

    double cx = a_cx.isAssigned() ? context->translateCoord(a_cx.getDouble(), a_cx.getUnit()) : 0.5;
    double cy = a_cy.isAssigned() ? context->translateCoord(a_cy.getDouble(), a_cy.getUnit()) : 0.5;
    double fx = a_fx.isAssigned() ? context->translateCoord(a_fx.getDouble(), a_fx.getUnit()) : cx;
    double fy = a_fy.isAssigned() ? context->translateCoord(a_fy.getDouble(), a_fy.getUnit()) : cy;
    double r = a_r.isAssigned() ? context->translateCoord(a_r.getDouble(), a_r.getUnit()) : 0.5;

    cx = bbox.x + bbox.w * cx;
    cy = bbox.y + bbox.h * cy;
    fx = bbox.x + bbox.w * fx;
    fy = bbox.y + bbox.h * fy;
    r = Math::min(bbox.w, bbox.h) * r;

    pattern.setPoints(PointD(cx, cy), PointD(fx, fy));
    pattern.setRadius(r);
  }
  else if (a_cx.isAssigned() && a_cy.isAssigned() && a_cx.isAssigned() && a_r.isAssigned())
  {
    double cx = context->translateCoord(a_cx.getDouble(), a_cx.getUnit());
    double cy = context->translateCoord(a_cy.getDouble(), a_cy.getUnit());
    double fx = a_fx.isAssigned() ? context->translateCoord(a_fx.getDouble(), a_fx.getUnit()) : cx;
    double fy = a_fy.isAssigned() ? context->translateCoord(a_fy.getDouble(), a_fy.getUnit()) : cy;
    double r = context->translateCoord(a_r.getDouble(), a_r.getUnit());

    pattern.setPoints(PointD(cx, cy), PointD(fx, fy));
    pattern.setRadius(r);
  }
  else
  {
    // SVG TODO: Is this error?
    fog_debug("Fog::SvgRadialGradient - Unsupported combination...");
  }

  // Add color stops.
  walkAndAddArgbStops(const_cast<SvgRadialGradientElement*>(this), pattern);

  if (paintType == SVG_PAINT_FILL)
    context->setFillPattern(pattern);
  else
    context->setStrokePattern(pattern);

  return ERR_OK;
}

// ============================================================================
// [Fog::SvgImageElement]
// ============================================================================

struct FOG_HIDDEN SvgImageElement : public SvgStyledElement
{
  // [Construction / Destruction]

  typedef SvgStyledElement base;

  SvgImageElement();
  virtual ~SvgImageElement();

  // [Attributes]

  virtual XmlAttribute* _createAttribute(const ManagedString& name) const;

  // [SVG Rendering]

  virtual err_t onRender(SvgContext* context) const;
  virtual err_t onRenderShape(SvgContext* context) const;

  // [Embedded Attributes]

  SvgCoordAttribute a_x;
  SvgCoordAttribute a_y;
  SvgCoordAttribute a_width;
  SvgCoordAttribute a_height;
  SvgImageLinkAttribute a_href;

private:
  FOG_DISABLE_COPY(SvgImageElement)
};

SvgImageElement::SvgImageElement() :
  SvgStyledElement(fog_strings->getString(STR_SVG_image), SVG_ELEMENT_IMAGE),
  a_x(NULL, fog_strings->getString(STR_SVG_x), FOG_OFFSET_OF(SvgImageElement, a_x)),
  a_y(NULL, fog_strings->getString(STR_SVG_y), FOG_OFFSET_OF(SvgImageElement, a_y)),
  a_width(NULL, fog_strings->getString(STR_SVG_width), FOG_OFFSET_OF(SvgImageElement, a_width)),
  a_height(NULL, fog_strings->getString(STR_SVG_height), FOG_OFFSET_OF(SvgImageElement, a_height)),
  a_href(NULL, fog_strings->getString(STR_SVG_xlink_href), FOG_OFFSET_OF(SvgImageElement, a_href))
{
}

SvgImageElement::~SvgImageElement()
{
  _removeAttributes();
}

XmlAttribute* SvgImageElement::_createAttribute(const ManagedString& name) const
{
  if (name == fog_strings->getString(STR_SVG_x)) return (XmlAttribute*)&a_x;
  if (name == fog_strings->getString(STR_SVG_y)) return (XmlAttribute*)&a_y;
  if (name == fog_strings->getString(STR_SVG_width)) return (XmlAttribute*)&a_width;
  if (name == fog_strings->getString(STR_SVG_height)) return (XmlAttribute*)&a_height;
  if (name == fog_strings->getString(STR_SVG_xlink_href)) return (XmlAttribute*)&a_href;

  return base::_createAttribute(name);
}

err_t SvgImageElement::onRender(SvgContext* context) const
{
  err_t err = ERR_OK;

  TransformD backupMatrix(DONT_INITIALIZE);
  bool transformed = a_transform.isAssigned() & a_transform.isValid();

  // There is only transformation that can be applied to the image.
  if (transformed)
  {
    backupMatrix = context->getPainter()->getTransform();
    context->getPainter()->transform(a_transform.getTransform());
  }

  // Render image.
  err = onRenderShape(context);

  // There is only transformation that can be applied to the image.
  if (transformed)
  {
    context->getPainter()->setTransform(backupMatrix);
  }

  // After render: SvgContextBackup destructor will restore SvgContext state
  // if modified.
  return err;
}

err_t SvgImageElement::onRenderShape(SvgContext* context) const
{
  if (a_href._embedded)
  {
    double x = a_x.isAssigned() ? a_x.getCoord().value : 0.0;
    double y = a_y.isAssigned() ? a_y.getCoord().value : 0.0;

    context->drawImage(PointD(x, y), a_href._image);
    return ERR_OK;
  }
  else
  {
    // TODO: Error code?
    return ERR_OK;
  }
}

// ============================================================================
// [Fog::SvgDocument]
// ============================================================================

SvgDocument::SvgDocument()
{
  _type |= SVG_ELEMENT_MASK;
}

SvgDocument::~SvgDocument()
{
}

XmlElement* SvgDocument::clone() const
{
  SvgDocument* doc = fog_new SvgDocument();
  if (!doc) return NULL;

  for (XmlElement* ch = firstChild(); ch; ch = ch->nextSibling())
  {
    XmlElement* e = ch->clone();
    if (e && doc->appendChild(e) != ERR_OK) { fog_delete(e); goto fail; }
  }

  return doc;

fail:
  fog_delete(doc);
  return NULL;
}

XmlElement* SvgDocument::createElement(const ManagedString& tagName)
{
  return createElementStatic(tagName);
}

XmlElement* SvgDocument::createElementStatic(const ManagedString& tagName)
{
  if (tagName == fog_strings->getString(STR_SVG_circle        )) return fog_new SvgCircleElement();
  if (tagName == fog_strings->getString(STR_SVG_defs          )) return fog_new SvgDefsElement();
  if (tagName == fog_strings->getString(STR_SVG_ellipse       )) return fog_new SvgEllipseElement();
  if (tagName == fog_strings->getString(STR_SVG_g             )) return fog_new SvgGElement();
  if (tagName == fog_strings->getString(STR_SVG_image         )) return fog_new SvgImageElement();
  if (tagName == fog_strings->getString(STR_SVG_line          )) return fog_new SvgLineElement();
  if (tagName == fog_strings->getString(STR_SVG_linearGradient)) return fog_new SvgLinearGradientElement();
  if (tagName == fog_strings->getString(STR_SVG_path          )) return fog_new SvgPathElement();
  if (tagName == fog_strings->getString(STR_SVG_polygon       )) return fog_new SvgPolygonElement();
  if (tagName == fog_strings->getString(STR_SVG_polyline      )) return fog_new SvgPolyLineElement();
  if (tagName == fog_strings->getString(STR_SVG_radialGradient)) return fog_new SvgRadialGradientElement();
  if (tagName == fog_strings->getString(STR_SVG_rect          )) return fog_new SvgRectElement();
  if (tagName == fog_strings->getString(STR_SVG_solidColor    )) return fog_new SvgSolidColorElement();
  if (tagName == fog_strings->getString(STR_SVG_stop          )) return fog_new SvgStopElement();
  if (tagName == fog_strings->getString(STR_SVG_use           )) return fog_new SvgUseElement();

  // If element is not SVG, use base class to create a default element
  // for the given tagName.
  return XmlDocument::createElementStatic(tagName);
}

err_t SvgDocument::onRender(SvgContext* context) const
{
  return SvgElement::_walkAndRender(this, context);
}

} // Fog namespace
