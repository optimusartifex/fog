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
  ctFont = NULL;
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
static void MacFontFace_cgPathApplier(void* info, const CGPathElement* element)
{
  NumT_(Path)* dst = reinterpret_cast<NumT_(Path)*>(info);

  switch (element->type)
  {
    case kCGPathElementMoveToPoint:
      dst->moveTo(NumT(element->points[0].x), NumT(-element->points[0].y));
      break;

    case kCGPathElementAddLineToPoint:
      dst->lineTo(NumT(element->points[0].x), NumT(-element->points[0].y));
      break;
      
    case kCGPathElementAddQuadCurveToPoint:
      dst->quadTo(NumT(element->points[0].x), NumT(-element->points[0].y),
                  NumT(element->points[1].x), NumT(-element->points[1].y));
      break;
      
    case kCGPathElementAddCurveToPoint:
      dst->cubicTo(NumT(element->points[0].x), NumT(-element->points[0].y),
                   NumT(element->points[1].x), NumT(-element->points[1].y),
                   NumT(element->points[2].x), NumT(-element->points[2].y));
      break;

    case kCGPathElementCloseSubpath:
      dst->close();
      break;
      
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

template<typename NumT>
static err_t MacFontFace_renderGlyphOutline(MacFontFace* self, 
  NumT_(Path)& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, MacFontContext* ctx)
{
  UniChar ucArray[2] = { (UniChar)uc, 0 };
  CGGlyph ucGlyph[2];
  CFIndex ucSize = 1;

  if (CharW::isSurrogate(uc))
  {
    CharW::ucs4ToSurrogate(
      static_cast<uint16_t*>(&ucArray[0]),
      static_cast<uint16_t*>(&ucArray[1]),
      uc);
    ucSize++;
  }

  if (!CTFontGetGlyphsForCharacters(self->ctFont, ucArray, ucGlyph, ucSize))
    return ERR_FONT_INTERNAL;

  CGPathRef cgPath = CTFontCreatePathForGlyph(self->ctFont, ucGlyph[0], NULL);

  if (cgPath != NULL)
  {
    CGPathApply(cgPath, &dst, MacFontFace_cgPathApplier<NumT>);
    CGPathRelease(cgPath);
  }

  CGSize advance[1];
  CTFontGetAdvancesForGlyphs(self->ctFont, kCTFontHorizontalOrientation, ucGlyph, advance, 1);

  metrics._horizontalAdvance.x = advance[0].width;
  metrics._horizontalAdvance.y = advance[0].height;
  metrics._verticalAdvance.x = 0.0f;
  metrics._verticalAdvance.y = 0.0f;

  return ERR_OK;
}

err_t MacFontFace::_renderGlyphOutline(PathF& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return MacFontFace_renderGlyphOutline<float>(this, dst, metrics, d, uc, reinterpret_cast<MacFontContext*>(ctx));
}

err_t MacFontFace::_renderGlyphOutline(PathD& dst, GlyphMetricsF& metrics, const FontData* d, uint32_t uc, void* ctx)
{
  return MacFontFace_renderGlyphOutline<double>(this, dst, metrics, d, uc, reinterpret_cast<MacFontContext*>(ctx));
}

// ============================================================================
// [Fog::MacFontFace - Methods]
// ============================================================================

err_t MacFontFace::_init(const StringW& family, CTFontRef src)
{
  this->family = family;
  this->family.squeeze();

  this->ctFont = src;

  designMetrics.reset();
  designMetrics._ascent = CTFontGetAscent(ctFont);
  designMetrics._descent = CTFontGetDescent(ctFont);
  designMetrics._height = CTFontGetXHeight(ctFont);
  
  return ERR_OK;
}

void MacFontFace::_reset()
{
  if (kerningTable != NULL)
  {
    MemMgr::free(kerningTable);
    kerningTable = NULL;
  }

  CFRelease(ctFont);
  ctFont = NULL;
}

} // Fog namespace
