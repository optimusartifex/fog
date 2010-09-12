// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Build.h>

#if defined(FOG_FONT_MAC)

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

#include <Fog/Graphics/FontEngine/MacFontEngine.h>

#import <Cocoa/Cocoa.h>

namespace Fog {

// TODO move it to Core/MacUtil.h
static NSString* toNSString(const String& str)
{
  ByteArray tmp;
  TextCodec::local8().fromUnicode(tmp, str);
  return [[NSString alloc] initWithCString:tmp.getData() encoding:NSUTF8StringEncoding];
}

static String fromNSString(NSString* str) 
{
	return (Char*)[str UTF8String];
}

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

  NSFontManager* fontManager = [NSFontManager sharedFontManager];
  NSEnumerator* e = [[fontManager availableFonts] objectEnumerator];
  NSString* name;
  while (name = [e nextObject])
  {
    fonts.append(fromNSString(name));
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
  MacFontFace* face = fog_new MacFontFace();

  NSAffineTransform* transform = [NSAffineTransform transform];
  [transform setTransformStruct: (NSAffineTransformStruct){matrix.sx, matrix.shy, matrix.shx, matrix.sy, 0, 0}];
  [transform scaleBy: size]; // Font size isn’t part of the glyph -> we have to scale it here
  [transform scaleXBy:1.0 yBy:-1.0]; // flip
  face->font = [NSFont fontWithDescriptor: [NSFontDescriptor fontDescriptorWithName: toNSString(family) size: size]
                            textTransform: transform];
  face->family = family;
  face->family.squeeze();
  face->metrics._size = size;
  face->metrics._ascent = [face->font ascender];
  face->metrics._descent = [face->font descender];
  // face->metrics._averageWidth = ??
  // face->metrics._maximumWidth = ??
  face->metrics._height = [face->font xHeight];
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
  font = NULL;
}

MacFontFace::~MacFontFace()
{
  [font release];
}

err_t MacFontFace::getGlyphSet(const Char* str, sysuint_t length, GlyphSet& glyphSet)
{
  // Painter is forced to draw text by outline, so there's no need to implement this
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t MacFontFace::getOutline(const Char* str, sysuint_t length, DoublePath& dst)
{
  AutoLock locked(lock);

  NSTextStorage* storage = [[NSTextStorage alloc] initWithString:toNSString(str)];
  NSLayoutManager* manager = [[NSLayoutManager alloc] init];
  NSTextContainer* container = [[NSTextContainer alloc] init];
  
  [storage addLayoutManager:manager];
  [manager addTextContainer:container];
  
  NSUInteger glyphCount = [manager numberOfGlyphs];
  NSGlyph* glyphs = fog_new NSGlyph[glyphCount];
  [manager getGlyphs:glyphs range:NSMakeRange(0, length)];

  NSBezierPath* path = [NSBezierPath bezierPath];
  [path moveToPoint: NSMakePoint(0, 0)]; // If I don't do this I get an exception that currentPoint doesn't exist ...
  [path appendBezierPathWithGlyphs:glyphs count:length inFont:font];

  delete[] glyphs;

  // NSBezierPath -> DoublePath
  for (NSInteger i=0; i<[path elementCount]; ++i)
  {
    NSPoint controlPoints[3];
    NSBezierPathElement el = [path elementAtIndex:i associatedPoints:controlPoints];
    if (el == NSMoveToBezierPathElement)
    {
      dst.moveTo(controlPoints[0].x, controlPoints[0].y);
    }
    else if (el == NSLineToBezierPathElement)
    {
      dst.lineTo(controlPoints[0].x, controlPoints[0].y);
    }
    else if (el == NSCurveToBezierPathElement)
    {
      dst.cubicTo(
         controlPoints[0].x, controlPoints[0].y,
         controlPoints[1].x, controlPoints[1].y,
         controlPoints[2].x, controlPoints[2].y);
    }	
    else if (el == NSClosePathBezierPathElement) 
    {
	  dst.closePolygon();
    }
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
