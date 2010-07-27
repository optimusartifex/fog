// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Build.h>

#if defined(FOG_FONT_MAC)

#include <Fog/Core/AutoLock.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/TextLayout.h>
#include <Fog/Core/TextCodec.h>

#include <Fog/Graphics/FontEngine/FontEngine_Mac.h>

namespace Fog {

// ============================================================================
// [Fog::MacFontEngine]
// ============================================================================

MacFontEngine::MacFontEngine() : FontEngine(FONT_FACE_MAC)
{
}

MacFontEngine::~MacFontEngine()
{
}

List<String> MacFontEngine::getFontList()
{
  List<String> fonts;

  // Create a font collection of all the fonts available in the system.
  RetainPtr<CTFontCollectionRef> fontCollection =
      CTFontCollectionCreateFromAvailableFonts(NULL);

  // Get the font descriptors for the font collection.
  RetainPtr<CFArrayRef> fontDescriptors = CTFontCollectionCreateMatchingFontDescriptors(fontCollection.get());

  // Go through the array of font descriptors and append the font each for each
  // descriptor to the result string list.
  sysint_t count = CFArrayGetCount(fontDescriptors.get());
  for (sysint_t i=0; i<count; ++i)
  {
      // Get the font descriptor for index.
    RetainPtr<CTFontDescriptorRef> value = static_cast<CTFontDescriptorRef>(CFArrayGetValueAtIndex(fontDescriptors.get(), i));
    if(value == NULL)
    {
      fog_stderr_msg("Fog::MacFontEngine", "getFontList", "Invalid cast");
      break;
    }

    // Get the font name from the descriptor.
    RetainPtr<CFStringRef> name = static_cast<CFStringRef>(CTFontDescriptorCopyAttribute(value.get(), kCTFontNameAttribute));
    if(name == NULL)
    {
        fog_stderr_msg("Fog::MacEngine", "getFontList", "Invalid cast");
        break;
    }

    // Append to the result string list of fonts.
    fonts.append(stringFromCFString(name.get()));
  }

  return fonts;
}

FontFace* MacFontEngine::createDefaultFace()
{
  return createFace(Ascii8("Times"), 12, FontOptions(), FloatMatrix());
}

FontFace* MacFontEngine::createFace(
  const String& family,
  float size, 
  const FontOptions& options,
  const FloatMatrix& matrix)
{
  MacFontFace* face = new(std::nothrow) MacFontFace();

  CGAffineTransform* transform = new CGAffineTransform;
  transform->a = matrix.sx;
  transform->b = matrix.shy;
  transform->c = matrix.shx;
  transform->d = matrix.sy;
  face->font = CTFontCreateWithName(CFStringFromString(family), size, transform);
  face->family = family;
  face->family.squeeze();
  face->metrics._size = size;
  face->metrics._ascent = CTFontGetAscent(face->font.get());
  face->metrics._descent = CTFontGetDescent(face->font.get());
  // face->metrics._averageWidth = ??
  // face->metrics._maximumWidth = ??
  face->metrics._height = CTFontGetXHeight(face->font.get());
  face->options = options;
  face->matrix = matrix;

  return face;
}

// ============================================================================
// [Fog::MacFontMaster]
// ============================================================================

MacFontMaster::MacFontMaster() :
  family(family)
{
  refCount.init(1);

  font = NULL;

  kerningPairs = NULL;
  kerningCount = 0;
}

MacFontMaster::~MacFontMaster()
{
}

// ============================================================================
// [Fog::MacFontFace]
// ============================================================================

MacFontFace::MacFontFace() :
  font(NULL)
{
  type = FONT_FACE_MAC;
  master = NULL;
}

MacFontFace::~MacFontFace()
{
}

err_t MacFontFace::getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet)
{
  // Painter is forced to draw text by outline, so there's no need to implement this
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [getOutline() Helpers]
// ============================================================================

struct ApplyInfo
{
    ApplyInfo(DoublePath& path, const DoublePoint& offset) : path(path), offset(offset) { }
   
    DoublePath& path;
    DoublePoint offset;
};

static void pathFromCGPathApply(void* info, const CGPathElement* element)
{
  DoublePath& path = static_cast<ApplyInfo*>(info)->path;
  DoublePoint offset = static_cast<ApplyInfo*>(info)->offset;

  switch(element->type)
  {
    case kCGPathElementMoveToPoint:
      path.moveTo(element->points[0].x + offset.getX(), offset.getY() - element->points[0].y);
      break;

    case kCGPathElementAddLineToPoint:
      path.lineTo(element->points[0].x + offset.getX(), offset.getY() - element->points[0].y);
      break;

    case kCGPathElementAddQuadCurveToPoint:
      path.curveTo(
         element->points[0].x + offset.getX(), offset.getY() - element->points[0].y,
         element->points[1].x + offset.getX(), offset.getY() - element->points[1].y);
      break;

    case kCGPathElementAddCurveToPoint:
      path.cubicTo(
         element->points[0].x + offset.getX(), offset.getY() - element->points[0].y,
         element->points[1].x + offset.getX(), offset.getY() - element->points[1].y,
         element->points[2].x + offset.getX(), offset.getY() - element->points[2].y);
      break;

    case kCGPathElementCloseSubpath:
      path.closePolygon();
      break;

    default:
      fog_debug("Unknown path element type %d", element->type);
      break;
  }
}

DoublePath MacFontFace::renderGlyph(uint32_t uc, DoublePoint& offset)
{
  UniChar unichar = static_cast<UniChar>(uc);
  CGGlyph glyph;
  if (!CTFontGetGlyphsForCharacters(font.get(), &unichar, &glyph, 1))
  {
    fog_stderr_msg("Fog::MacFontFace", "renderGlyph", "Could not encode unicode character");
    return DoublePath();
  }

  CGAffineTransform transform;
  transform.a = matrix.sx;
  transform.b = matrix.shy;
  transform.c = matrix.shx;
  transform.d = matrix.sy;
  transform.tx = matrix.tx;
  transform.ty = matrix.ty;

  DoublePath p;
  CGSize sz;

  RetainPtr<CGPathRef> path = CTFontCreatePathForGlyph(font.get(), glyph, &transform);
  if (!path)
  {
    fog_stderr_msg("Fog::MacFontFace", "renderGlyph", "Could not create path for glyph %i", uc);
    return p;
  }

  CGPathApply(path.get(), new ApplyInfo(p, offset), pathFromCGPathApply);
  CTFontGetAdvancesForGlyphs(font.get(), kCTFontHorizontalOrientation, &glyph, &sz, 1);
  offset.translate(sz.width, 0.0);

  return p;
}

err_t MacFontFace::getOutline(const Char* str, sysuint_t length, DoublePath& dst)
{
  // TODO: What is this
  ByteArray result;
  TextCodec::local8().fromUnicode(result, str, length);

  AutoLock locked(lock);

  DoublePoint offset(0.0, 0.0);
  for (sysuint_t i = 0; i < length; i++)
  {
    dst.addPath(renderGlyph(str[i].ch(), offset), offset);
  }

  return ERR_OK;
}

err_t MacFontFace::getTextExtents(const Char* str, sysuint_t length, TextExtents& extents)
{
  TemporaryGlyphSet<128> glyphSet;
  FOG_RETURN_ON_ERROR(getGlyphSet(str, length, glyphSet));

  extents.advance = glyphSet.getAdvance();
  extents.beginWidth = 0;
  extents.endWidth = 0;
  return ERR_OK;
}

} // Fog namespace

#endif // FOG_FONT_MAC
