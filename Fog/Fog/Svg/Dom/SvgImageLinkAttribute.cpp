// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Svg/Dom/SvgElement.h>
#include <Fog/Svg/Dom/SvgImageLinkAttribute_p.h>

namespace Fog {

// ============================================================================
// [Fog::SvgImageLinkAttribute]
// ============================================================================

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
    err |= StringUtil::toBase64(dst, memio.getBuffer(), CONTAINER_OP_APPEND);

    if (FOG_IS_ERROR(err)) dst.reset();
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
        extension = fog_strings->getString(STR_G2D_EXTENSION_png);
      }
      else if (type == Ascii8("image/jpeg"))
      {
        extension = fog_strings->getString(STR_G2D_EXTENSION_jpeg);
      }
      else
      {
        // Standard talks only about PNG and JPEG, but we can use any attached
        // image that can be decoded by Fog-Imaging API.
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

      if (FOG_IS_ERROR(err))
      {
        // Something evil happened. I don't know what to do now, because image
        // seems to be corrupted or unsupported.
        _value.set(value);
        _image.reset();
        _embedded = false;
      }
      else
      {
        _value.reset();
        _embedded = true;
      }
    }
  }

  err = _value.set(value);
  if (_element) reinterpret_cast<SvgElement*>(_element)->_boundingBoxDirty = true;
  return err;
}

} // Fog namespace
