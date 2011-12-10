// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/OS/MacUtil.h>
#include <Fog/G2d/Text/MacFontFace.h>
#include <Fog/G2d/Text/MacFontProvider.h>

// [Dependencies - Mac]
#import <Cocoa/Cocoa.h>

namespace Fog {
  
// ============================================================================
// [Fog::MacFontContext]
// ============================================================================
  
struct FOG_NO_EXPORT MacFontContext
{
  FOG_INLINE MacFontContext() {}
  FOG_INLINE ~MacFontContext() {}
};
  
// ============================================================================
// [Fog::MacFontFace - Construction / Destruction]
// ============================================================================

MacFontFace::MacFontFace(MacFontProviderData* pd) :
  pd(pd)
{
  nsFont = nil;
  kerningTable = NULL;
}

MacFontFace::~MacFontFace()
{
  _reset();
}

// ============================================================================
// [Fog::MacFontFace - Interface]
// ============================================================================

err_t MacFontFace::getTextOutline(PathF& dst, const FontData* d, const PointF& pt, const StubW& str)
{
  AutoLock locked(pd->lock);
  
  MacFontContext ctx;
  return _getTextOutline(dst, &outlineCache, d, pt, str, &ctx);
}

err_t MacFontFace::getTextOutline(PathD& dst, const FontData* d, const PointD& pt, const StubW& str)
{
  AutoLock locked(pd->lock);
  
  MacFontContext ctx;
  return _getTextOutline(dst, &outlineCache, d, pt, str, &ctx);
}

err_t MacFontFace::getTextExtents(TextExtentsF& extents, const FontData* d, const StubW& str)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t MacFontFace::getTextExtents(TextExtentsD& extents, const FontData* d, const StubW& str)
{
  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

FontKerningTableF* MacFontFace::getKerningTable(const FontData* d)
{
  return kerningTable;
}

// ============================================================================
// [Fog::MacFontFace - Private]
// ============================================================================

template<typename NumT>
static err_t _G2d_MacFontFace_renderGlyphOutline(MacFontFace* self, 
  NumT_(Path)& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, MacFontContext* ctx)
{
  UniChar uc_array[1] = { (UniChar)uc };
  NSGlyph uc_glyph[1];

  if (!CTFontGetGlyphsForCharacters((CTFontRef)self->nsFont, uc_array, (CGGlyph *)uc_glyph, 1))
    return ERR_FONT_INTERNAL;

  NSBezierPath* path = [NSBezierPath bezierPath];
  [path moveToPoint: NSMakePoint(0.0f, 0.0f)];
  [path appendBezierPathWithGlyphs: uc_glyph count: 1 inFont: self->nsFont];
  
  NSInteger i, len = [path elementCount];
  
  for (i = 0; i < len; i++)
  {
    NSPoint pts[3];
    NSBezierPathElement cmd = [path elementAtIndex: i associatedPoints: pts];

    switch (cmd)
    {
      case NSMoveToBezierPathElement:
        dst.moveTo(NumT_(Point)((float)pts[0].x, (float)-pts[0].y));
        break;
      case NSLineToBezierPathElement:
        dst.lineTo(NumT_(Point)((float)pts[0].x, (float)-pts[0].y));
        break;
      case NSCurveToBezierPathElement:
        dst.cubicTo(NumT_(Point)((float)pts[0].x, (float)-pts[0].y),
                    NumT_(Point)((float)pts[1].x, (float)-pts[1].y),
                    NumT_(Point)((float)pts[2].x, (float)-pts[2].y));
        break;
      case NSClosePathBezierPathElement:
        dst.close();
        break;
    }
  }

  metrics._horizontalAdvance.reset();
  metrics._verticalAdvance.reset();

  CGSize advance[1];
  metrics._horizontalAdvance.x = (float)CTFontGetAdvancesForGlyphs(
    (CTFontRef)self->nsFont, kCTFontHorizontalOrientation, (CGGlyph*)uc_glyph, advance, 1);
  
  metrics._horizontalAdvance.x = advance[0].width;
  metrics._horizontalAdvance.y = advance[0].height;
  
  return ERR_OK;
}
  
err_t MacFontFace::_renderGlyphOutline(PathF& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return _G2d_MacFontFace_renderGlyphOutline<float>(this, dst, metrics, d, uc, reinterpret_cast<MacFontContext*>(ctx));
}

err_t MacFontFace::_renderGlyphOutline(PathD& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return _G2d_MacFontFace_renderGlyphOutline<double>(this, dst, metrics, d, uc, reinterpret_cast<MacFontContext*>(ctx));
}

// ============================================================================
// [Fog::MacFontFace - Methods]
// ============================================================================

err_t MacFontFace::_init(const StringW& family, NSFont* src)
{
  this->family = family;
  this->family.squeeze();

  [src retain];
  nsFont = src;

  designMetrics.reset();
  designMetrics._ascent = [nsFont ascender];
  designMetrics._descent = [nsFont descender];
  designMetrics._height = [nsFont xHeight];
  
  return ERR_OK;
}

void MacFontFace::_reset()
{
  if (kerningTable != NULL)
  {
    MemMgr::free(kerningTable);
    kerningTable = NULL;
  }

  [nsFont release];
  nsFont = nil;
}

} // Fog namespace
