// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Painting/NullPaintEngine_p.h>

namespace Fog {

// This template is for a new painter engine implementation.
#if 0

// ============================================================================
// [Fog::MyPainterVTable]
// ============================================================================

struct FOG_NO_EXPORT MyPainterVTable : public PainterVTable
{
};

static PainterVTable _MyPaintEngine_vtable;

// ============================================================================
// [Fog::MyPaintEngine]
// ============================================================================

struct FOG_NO_EXPORT MyPaintEngine : public PaintEngine
{
};

// ============================================================================
// [Fog::MyPainterImpl]
// ============================================================================

struct FOG_NO_EXPORT MyPainterImpl
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL release(Painter& self);

  // --------------------------------------------------------------------------
  // [Meta Params]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getMetaParams(const Painter& self, Region& region, PointI& origin);
  static err_t FOG_CDECL setMetaParams(Painter& self, const Region& region, const PointI& origin);
  static err_t FOG_CDECL resetMetaParams(Painter& self);

  // --------------------------------------------------------------------------
  // [User Params]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getUserParams(const Painter& self, Region& region, PointI& origin);
  static err_t FOG_CDECL setUserParams(Painter& self, const Region& region, const PointI& origin);
  static err_t FOG_CDECL resetUserParams(Painter& self);

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getParameter(const Painter& self, uint32_t parameterId, void* value);
  static err_t FOG_CDECL setParameter(Painter& self, uint32_t parameterId, const void* value);
  static err_t FOG_CDECL resetParameter(Painter& self, uint32_t parameterId);

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getSourceType(const Painter& self, uint32_t& val);
  static err_t FOG_CDECL getSourceColor(const Painter& self, Color& color);
  static err_t FOG_CDECL getSourcePatternF(const Painter& self, PatternF& pattern);
  static err_t FOG_CDECL getSourcePatternD(const Painter& self, PatternD& pattern);

  static err_t FOG_CDECL setSourceArgb32(Painter& self, uint32_t argb32);
  static err_t FOG_CDECL setSourceArgb64(Painter& self, const Argb64& argb64);
  static err_t FOG_CDECL setSourceColor(Painter& self, const Color& color);
  static err_t FOG_CDECL setSourcePatternF(Painter& self, const PatternF& pattern);
  static err_t FOG_CDECL setSourcePatternD(Painter& self, const PatternD& pattern);
  static err_t FOG_CDECL setSourceAbstract(Painter& self, uint32_t sourceId, const void* value, const void* tr);

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getTransformF(const Painter& self, TransformF& tr);
  static err_t FOG_CDECL getTransformD(const Painter& self, TransformD& tr);

  static err_t FOG_CDECL setTransformF(Painter& self, const TransformF& tr);
  static err_t FOG_CDECL setTransformD(Painter& self, const TransformD& tr);

  static err_t FOG_CDECL applyTransformF(Painter& self, uint32_t transformOp, const void* params);
  static err_t FOG_CDECL applyTransformD(Painter& self, uint32_t transformOp, const void* params);

  static err_t FOG_CDECL resetTransform(Painter& self);

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL save(Painter& self);
  static err_t FOG_CDECL restore(Painter& self);

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL mapPointF(const Painter& self, uint32_t mapOp, PointF& pt);
  static err_t FOG_CDECL mapPointD(const Painter& self, uint32_t mapOp, PointD& pt);

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL drawRectI(Painter& self, const RectI& r);
  static err_t FOG_CDECL drawRectF(Painter& self, const RectF& r);
  static err_t FOG_CDECL drawRectD(Painter& self, const RectD& r);

  static err_t FOG_CDECL drawPolylineI(Painter& self, const PointI* p, size_t count);
  static err_t FOG_CDECL drawPolylineF(Painter& self, const PointF* p, size_t count);
  static err_t FOG_CDECL drawPolylineD(Painter& self, const PointD* p, size_t count);

  static err_t FOG_CDECL drawPolygonI(Painter& self, const PointI* p, size_t count);
  static err_t FOG_CDECL drawPolygonF(Painter& self, const PointF* p, size_t count);
  static err_t FOG_CDECL drawPolygonD(Painter& self, const PointD* p, size_t count);

  static err_t FOG_CDECL drawShapeF(Painter& self, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL drawShapeD(Painter& self, uint32_t shapeType, const void* shapeData);

  static err_t FOG_CDECL drawPathF(Painter& self, const PathF& p);
  static err_t FOG_CDECL drawPathD(Painter& self, const PathD& p);

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL fillAll(Painter& self);

  static err_t FOG_CDECL fillRectI(Painter& self, const RectI& r);
  static err_t FOG_CDECL fillRectF(Painter& self, const RectF& r);
  static err_t FOG_CDECL fillRectD(Painter& self, const RectD& r);

  static err_t FOG_CDECL fillRectsI(Painter& self, const RectI* r, size_t count);
  static err_t FOG_CDECL fillRectsF(Painter& self, const RectF* r, size_t count);
  static err_t FOG_CDECL fillRectsD(Painter& self, const RectD* r, size_t count);

  static err_t FOG_CDECL fillPolygonI(Painter& self, const PointI* p, size_t count);
  static err_t FOG_CDECL fillPolygonF(Painter& self, const PointF* p, size_t count);
  static err_t FOG_CDECL fillPolygonD(Painter& self, const PointD* p, size_t count);

  static err_t FOG_CDECL fillShapeF(Painter& self, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL fillShapeD(Painter& self, uint32_t shapeType, const void* shapeData);

  static err_t FOG_CDECL fillPathF(Painter& self, const PathF& p);
  static err_t FOG_CDECL fillPathD(Painter& self, const PathD& p);

  static err_t FOG_CDECL fillTextAtI(Painter& self, const PointI& p, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL fillTextAtF(Painter& self, const PointF& p, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL fillTextAtD(Painter& self, const PointD& p, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL fillTextInI(Painter& self, const TextRectI& r, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL fillTextInF(Painter& self, const TextRectF& r, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL fillTextInD(Painter& self, const TextRectD& r, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL fillMaskAtI(Painter& self, const PointI& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskAtF(Painter& self, const PointF& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskAtD(Painter& self, const PointD& p, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL fillMaskInI(Painter& self, const RectI& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskInF(Painter& self, const RectF& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL fillMaskInD(Painter& self, const RectD& r, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL fillRegion(Painter& self, const Region& r);

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL blitImageAtI(Painter& self, const PointI& p, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageAtF(Painter& self, const PointF& p, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageAtD(Painter& self, const PointD& p, const Image& i, const RectI* iFragment);

  static err_t FOG_CDECL blitImageInI(Painter& self, const RectI& r, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageInF(Painter& self, const RectF& r, const Image& i, const RectI* iFragment);
  static err_t FOG_CDECL blitImageInD(Painter& self, const RectD& r, const Image& i, const RectI* iFragment);

  static err_t FOG_CDECL blitMaskedImageAtI(Painter& self, const PointI& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageAtF(Painter& self, const PointF& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageAtD(Painter& self, const PointD& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);

  static err_t FOG_CDECL blitMaskedImageInI(Painter& self, const RectI& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageInF(Painter& self, const RectF& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);
  static err_t FOG_CDECL blitMaskedImageInD(Painter& self, const RectD& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment);

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL clipRectI(Painter& self, uint32_t clipOp, const RectI& r);
  static err_t FOG_CDECL clipRectF(Painter& self, uint32_t clipOp, const RectF& r);
  static err_t FOG_CDECL clipRectD(Painter& self, uint32_t clipOp, const RectD& r);

  static err_t FOG_CDECL clipRectsI(Painter& self, uint32_t clipOp, const RectI* r, size_t count);
  static err_t FOG_CDECL clipRectsF(Painter& self, uint32_t clipOp, const RectF* r, size_t count);
  static err_t FOG_CDECL clipRectsD(Painter& self, uint32_t clipOp, const RectD* r, size_t count);

  static err_t FOG_CDECL clipPolygonI(Painter& self, uint32_t clipOp, const PointI* p, size_t count);
  static err_t FOG_CDECL clipPolygonF(Painter& self, uint32_t clipOp, const PointF* p, size_t count);
  static err_t FOG_CDECL clipPolygonD(Painter& self, uint32_t clipOp, const PointD* p, size_t count);

  static err_t FOG_CDECL clipShapeF(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL clipShapeD(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);

  static err_t FOG_CDECL clipPathF(Painter& self, uint32_t clipOp, const PathF& p);
  static err_t FOG_CDECL clipPathD(Painter& self, uint32_t clipOp, const PathD& p);

  static err_t FOG_CDECL clipTextAtI(Painter& self, uint32_t clipOp, const PointI& p, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL clipTextAtF(Painter& self, uint32_t clipOp, const PointF& p, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL clipTextAtD(Painter& self, uint32_t clipOp, const PointD& p, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL clipTextInI(Painter& self, uint32_t clipOp, const TextRectI& r, const String& text, const Font& font, const RectI* clip);
  static err_t FOG_CDECL clipTextInF(Painter& self, uint32_t clipOp, const TextRectF& r, const String& text, const Font& font, const RectF* clip);
  static err_t FOG_CDECL clipTextInD(Painter& self, uint32_t clipOp, const TextRectD& r, const String& text, const Font& font, const RectD* clip);

  static err_t FOG_CDECL clipMaskAtI(Painter& self, uint32_t clipOp, const PointI& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskAtF(Painter& self, uint32_t clipOp, const PointF& p, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskAtD(Painter& self, uint32_t clipOp, const PointD& p, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL clipMaskInI(Painter& self, uint32_t clipOp, const RectI& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskInF(Painter& self, uint32_t clipOp, const RectF& r, const Image& m, const RectI* mFragment);
  static err_t FOG_CDECL clipMaskInD(Painter& self, uint32_t clipOp, const RectD& r, const Image& m, const RectI* mFragment);

  static err_t FOG_CDECL clipRegion(Painter& self, uint32_t clipOp, const Region& r);

  static err_t FOG_CDECL resetClip(Painter& self);

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL beginLayer(Painter& self);
  static err_t FOG_CDECL endLayer(Painter& self);

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL flush(Painter& self, uint32_t flags);
};

// ============================================================================
// [Fog::MyPainterImpl - AddRef / Release]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::release(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Meta Params]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::getMetaParams(const Painter& self, Region& region, PointI& origin)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setMetaParams(Painter& self, const Region& region, const PointI& origin)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::resetMetaParams(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - User Params]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::getUserParams(const Painter& self, Region& region, PointI& origin)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setUserParams(Painter& self, const Region& region, const PointI& origin)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::resetUserParams(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Parameters]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::getParameter(const Painter& self, uint32_t parameterId, void* value)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setParameter(Painter& self, uint32_t parameterId, const void* value)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::resetParameter(Painter& self, uint32_t parameterId)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Source]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::getSourceType(const Painter& self, uint32_t& val)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::getSourceColor(const Painter& self, Color& color)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::getSourcePatternF(const Painter& self, PatternF& pattern)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::getSourcePatternD(const Painter& self, PatternD& pattern)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setSourceArgb32(Painter& self, uint32_t argb32)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setSourceArgb64(Painter& self, const Argb64& argb64)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setSourceColor(Painter& self, const Color& color)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setSourcePatternF(Painter& self, const PatternF& pattern)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setSourcePatternD(Painter& self, const PatternD& pattern)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setSourceAbstract(Painter& self, uint32_t sourceId, const void* value, const void* tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Transform]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::getTransformF(const Painter& self, TransformF& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::getTransformD(const Painter& self, TransformD& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setTransformF(Painter& self, const TransformF& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::setTransformD(Painter& self, const TransformD& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::applyTransformF(Painter& self, uint32_t transformOp, const void* params)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::applyTransformD(Painter& self, uint32_t transformOp, const void* params)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::resetTransform(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - State]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::save(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::restore(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Map]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::mapPointF(const Painter& self, uint32_t mapOp, PointF& pt)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::mapPointD(const Painter& self, uint32_t mapOp, PointD& pt)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Draw]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::drawRectI(Painter& self, const RectI& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawRectF(Painter& self, const RectF& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawRectD(Painter& self, const RectD& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPolylineI(Painter& self, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPolylineF(Painter& self, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPolylineD(Painter& self, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPolygonI(Painter& self, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPolygonF(Painter& self, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPolygonD(Painter& self, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawShapeF(Painter& self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawShapeD(Painter& self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPathF(Painter& self, const PathF& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::drawPathD(Painter& self, const PathD& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Fill]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::fillAll(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillRectI(Painter& self, const RectI& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillRectF(Painter& self, const RectF& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillRectD(Painter& self, const RectD& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillRectsI(Painter& self, const RectI* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillRectsF(Painter& self, const RectF* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillRectsD(Painter& self, const RectD* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillPolygonI(Painter& self, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillPolygonF(Painter& self, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillPolygonD(Painter& self, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillShapeF(Painter& self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillShapeD(Painter& self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillPathF(Painter& self, const PathF& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillPathD(Painter& self, const PathD& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillTextAtI(Painter& self, const PointI& p, const String& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillTextAtF(Painter& self, const PointF& p, const String& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillTextAtD(Painter& self, const PointD& p, const String& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillTextInI(Painter& self, const TextRectI& r, const String& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillTextInF(Painter& self, const TextRectF& r, const String& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillTextInD(Painter& self, const TextRectD& r, const String& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillMaskAtI(Painter& self, const PointI& p, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillMaskAtF(Painter& self, const PointF& p, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillMaskAtD(Painter& self, const PointD& p, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillMaskInI(Painter& self, const RectI& r, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillMaskInF(Painter& self, const RectF& r, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillMaskInD(Painter& self, const RectD& r, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::fillRegion(Painter& self, const Region& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Blit]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::blitImageAtI(Painter& self, const PointI& p, const Image& i, const RectI* iFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitImageAtF(Painter& self, const PointF& p, const Image& i, const RectI* iFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitImageAtD(Painter& self, const PointD& p, const Image& i, const RectI* iFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitImageInI(Painter& self, const RectI& r, const Image& i, const RectI* iFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitImageInF(Painter& self, const RectF& r, const Image& i, const RectI* iFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitImageInD(Painter& self, const RectD& r, const Image& i, const RectI* iFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitMaskedImageAtI(Painter& self, const PointI& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitMaskedImageAtF(Painter& self, const PointF& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitMaskedImageAtD(Painter& self, const PointD& p, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitMaskedImageInI(Painter& self, const RectI& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitMaskedImageInF(Painter& self, const RectF& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::blitMaskedImageInD(Painter& self, const RectD& r, const Image& i, const Image& m, const RectI* iFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Clip]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::clipRectI(Painter& self, uint32_t clipOp, const RectI& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipRectF(Painter& self, uint32_t clipOp, const RectF& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipRectD(Painter& self, uint32_t clipOp, const RectD& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipRectsI(Painter& self, uint32_t clipOp, const RectI* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipRectsF(Painter& self, uint32_t clipOp, const RectF* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipRectsD(Painter& self, uint32_t clipOp, const RectD* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipPolygonI(Painter& self, uint32_t clipOp, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipPolygonF(Painter& self, uint32_t clipOp, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipPolygonD(Painter& self, uint32_t clipOp, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipShapeF(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipShapeD(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipPathF(Painter& self, uint32_t clipOp, const PathF& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipPathD(Painter& self, uint32_t clipOp, const PathD& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipTextAtI(Painter& self, uint32_t clipOp, const PointI& p, const String& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipTextAtF(Painter& self, uint32_t clipOp, const PointF& p, const String& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipTextAtD(Painter& self, uint32_t clipOp, const PointD& p, const String& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipTextInI(Painter& self, uint32_t clipOp, const TextRectI& r, const String& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipTextInF(Painter& self, uint32_t clipOp, const TextRectF& r, const String& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipTextInD(Painter& self, uint32_t clipOp, const TextRectD& r, const String& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipMaskAtI(Painter& self, uint32_t clipOp, const PointI& p, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipMaskAtF(Painter& self, uint32_t clipOp, const PointF& p, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipMaskAtD(Painter& self, uint32_t clipOp, const PointD& p, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipMaskInI(Painter& self, uint32_t clipOp, const RectI& r, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipMaskInF(Painter& self, uint32_t clipOp, const RectF& r, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipMaskInD(Painter& self, uint32_t clipOp, const RectD& r, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::clipRegion(Painter& self, uint32_t clipOp, const Region& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::resetClip(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Layer]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::beginLayer(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL MyPainterImpl::endLayer(Painter& self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPainterImpl - Flush]
// ============================================================================

err_t FOG_CDECL MyPainterImpl::flush(Painter& self, uint32_t flags)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self._engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

static void _g2d_painter_init_my()
{
  PainterVTable& v = _MyPaintEngine_vtable;

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  v.release = MyPainterImpl::release;

  // --------------------------------------------------------------------------
  // Meta Params]
  // --------------------------------------------------------------------------

  v.getMetaParams = MyPainterImpl::getMetaParams;
  v.setMetaParams = MyPainterImpl::setMetaParams;
  v.resetMetaParams = MyPainterImpl::resetMetaParams;

  // --------------------------------------------------------------------------
  // [User Params]
  // --------------------------------------------------------------------------

  v.getUserParams = MyPainterImpl::getUserParams;
  v.setUserParams = MyPainterImpl::setUserParams;
  v.resetUserParams = MyPainterImpl::resetUserParams;

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  v.getParameter = MyPainterImpl::getParameter;
  v.setParameter = MyPainterImpl::setParameter;
  v.resetParameter = MyPainterImpl::resetParameter;

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  v.getSourceType = MyPainterImpl::getSourceType;

  v.getSourceColor = MyPainterImpl::getSourceColor;
  v.getSourcePatternF = MyPainterImpl::getSourcePatternF;
  v.getSourcePatternD = MyPainterImpl::getSourcePatternD;

  v.setSourceArgb32 = MyPainterImpl::setSourceArgb32;
  v.setSourceArgb64 = MyPainterImpl::setSourceArgb64;
  v.setSourceColor = MyPainterImpl::setSourceColor;
  v.setSourcePatternF = MyPainterImpl::setSourcePatternF;
  v.setSourcePatternD = MyPainterImpl::setSourcePatternD;
  v.setSourceAbstract = MyPainterImpl::setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  v.getTransformF = MyPainterImpl::getTransformF;
  v.getTransformD = MyPainterImpl::getTransformD;

  v.setTransformF = MyPainterImpl::setTransformF;
  v.setTransformD = MyPainterImpl::setTransformD;

  v.applyTransformF = MyPainterImpl::applyTransformF;
  v.applyTransformD = MyPainterImpl::applyTransformD;

  v.resetTransform = MyPainterImpl::resetTransform;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  v.save = MyPainterImpl::save;
  v.restore = MyPainterImpl::restore;

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  v.mapPointF = MyPainterImpl::mapPointF;
  v.mapPointD = MyPainterImpl::mapPointD;

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  v.drawRectI = MyPainterImpl::drawRectI;
  v.drawRectF = MyPainterImpl::drawRectF;
  v.drawRectD = MyPainterImpl::drawRectD;

  v.drawPolylineI = MyPainterImpl::drawPolylineI;
  v.drawPolylineF = MyPainterImpl::drawPolylineF;
  v.drawPolylineD = MyPainterImpl::drawPolylineD;

  v.drawPolygonI = MyPainterImpl::drawPolygonI;
  v.drawPolygonF = MyPainterImpl::drawPolygonF;
  v.drawPolygonD = MyPainterImpl::drawPolygonD;

  v.drawShapeF = MyPainterImpl::drawShapeF;
  v.drawShapeD = MyPainterImpl::drawShapeD;

  v.drawPathF = MyPainterImpl::drawPathF;
  v.drawPathD = MyPainterImpl::drawPathD;

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  v.fillAll = MyPainterImpl::fillAll;

  v.fillRectI = MyPainterImpl::fillRectI;
  v.fillRectF = MyPainterImpl::fillRectF;
  v.fillRectD = MyPainterImpl::fillRectD;

  v.fillRectsI = MyPainterImpl::fillRectsI;
  v.fillRectsF = MyPainterImpl::fillRectsF;
  v.fillRectsD = MyPainterImpl::fillRectsD;

  v.fillPolygonI = MyPainterImpl::fillPolygonI;
  v.fillPolygonF = MyPainterImpl::fillPolygonF;
  v.fillPolygonD = MyPainterImpl::fillPolygonD;

  v.fillShapeF = MyPainterImpl::fillShapeF;
  v.fillShapeD = MyPainterImpl::fillShapeD;

  v.fillPathF = MyPainterImpl::fillPathF;
  v.fillPathD = MyPainterImpl::fillPathD;

  v.fillTextAtI = MyPainterImpl::fillTextAtI;
  v.fillTextAtF = MyPainterImpl::fillTextAtF;
  v.fillTextAtD = MyPainterImpl::fillTextAtD;

  v.fillTextInI = MyPainterImpl::fillTextInI;
  v.fillTextInF = MyPainterImpl::fillTextInF;
  v.fillTextInD = MyPainterImpl::fillTextInD;

  v.fillMaskAtI = MyPainterImpl::fillMaskAtI;
  v.fillMaskAtF = MyPainterImpl::fillMaskAtF;
  v.fillMaskAtD = MyPainterImpl::fillMaskAtD;

  v.fillMaskInI = MyPainterImpl::fillMaskInI;
  v.fillMaskInF = MyPainterImpl::fillMaskInF;
  v.fillMaskInD = MyPainterImpl::fillMaskInD;

  v.fillRegion = MyPainterImpl::fillRegion;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v.blitImageAtI = MyPainterImpl::blitImageAtI;
  v.blitImageAtF = MyPainterImpl::blitImageAtF;
  v.blitImageAtD = MyPainterImpl::blitImageAtD;

  v.blitImageInI = MyPainterImpl::blitImageInI;
  v.blitImageInF = MyPainterImpl::blitImageInF;
  v.blitImageInD = MyPainterImpl::blitImageInD;

  v.blitMaskedImageAtI = MyPainterImpl::blitMaskedImageAtI;
  v.blitMaskedImageAtF = MyPainterImpl::blitMaskedImageAtF;
  v.blitMaskedImageAtD = MyPainterImpl::blitMaskedImageAtD;

  v.blitMaskedImageInI = MyPainterImpl::blitMaskedImageInI;
  v.blitMaskedImageInF = MyPainterImpl::blitMaskedImageInF;
  v.blitMaskedImageInD = MyPainterImpl::blitMaskedImageInD;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  v.clipRectI = MyPainterImpl::clipRectI;
  v.clipRectF = MyPainterImpl::clipRectF;
  v.clipRectD = MyPainterImpl::clipRectD;

  v.clipRectsI = MyPainterImpl::clipRectsI;
  v.clipRectsF = MyPainterImpl::clipRectsF;
  v.clipRectsD = MyPainterImpl::clipRectsD;

  v.clipPolygonI = MyPainterImpl::clipPolygonI;
  v.clipPolygonF = MyPainterImpl::clipPolygonF;
  v.clipPolygonD = MyPainterImpl::clipPolygonD;

  v.clipShapeF = MyPainterImpl::clipShapeF;
  v.clipShapeD = MyPainterImpl::clipShapeD;

  v.clipPathF = MyPainterImpl::clipPathF;
  v.clipPathD = MyPainterImpl::clipPathD;

  v.clipTextAtI = MyPainterImpl::clipTextAtI;
  v.clipTextAtF = MyPainterImpl::clipTextAtF;
  v.clipTextAtD = MyPainterImpl::clipTextAtD;

  v.clipTextInI = MyPainterImpl::clipTextInI;
  v.clipTextInF = MyPainterImpl::clipTextInF;
  v.clipTextInD = MyPainterImpl::clipTextInD;

  v.clipMaskAtI = MyPainterImpl::clipMaskAtI;
  v.clipMaskAtF = MyPainterImpl::clipMaskAtF;
  v.clipMaskAtD = MyPainterImpl::clipMaskAtD;

  v.clipMaskInI = MyPainterImpl::clipMaskInI;
  v.clipMaskInF = MyPainterImpl::clipMaskInF;
  v.clipMaskInD = MyPainterImpl::clipMaskInD;

  v.clipRegion = MyPainterImpl::clipRegion;

  v.resetClip = MyPainterImpl::resetClip;

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  v.beginLayer = MyPainterImpl::beginLayer;
  v.endLayer = MyPainterImpl::endLayer;

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  v.flush = MyPainterImpl::flush;
}

static void _g2d_painter_fini_my()
{
}

#endif

// ============================================================================
// [Fog::NullPainterVTable]
// ============================================================================

static PainterVTable NullPaintEngine_vtable;

// ============================================================================
// [Fog::NullPainterImpl]
// ============================================================================

struct FOG_NO_EXPORT NullPainterImpl
{
  struct FOG_NO_EXPORT Any {};

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL release(Painter& self);

  // --------------------------------------------------------------------------
  // [Meta Params]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getMetaParams(const Painter& self, Region& region, PointI& origin);
  static err_t FOG_CDECL setMetaParams(Painter& self, const Region& region, const PointI& origin);
  static err_t FOG_CDECL resetMetaParams(Painter& self);

  // --------------------------------------------------------------------------
  // [User Params]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getUserParams(const Painter& self, Region& region, PointI& origin);
  static err_t FOG_CDECL setUserParams(Painter& self, const Region& region, const PointI& origin);
  static err_t FOG_CDECL resetUserParams(Painter& self);

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getParameter(const Painter& self, uint32_t parameterId, void* value);
  static err_t FOG_CDECL setParameter(Painter& self, uint32_t parameterId, const void* value);
  static err_t FOG_CDECL resetParameter(Painter& self, uint32_t parameterId);

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getSourceType(const Painter& self, uint32_t& val);
  static err_t FOG_CDECL getSourceColor(const Painter& self, Color& color);
  static err_t FOG_CDECL getSourcePatternF(const Painter& self, PatternF& pattern);
  static err_t FOG_CDECL getSourcePatternD(const Painter& self, PatternD& pattern);

  static err_t FOG_CDECL setSourceArgb32(Painter& self, uint32_t argb32);
  static err_t FOG_CDECL setSourceArgb64(Painter& self, const Argb64& argb64);
  static err_t FOG_CDECL setSourceColor(Painter& self, const Color& color);
  static err_t FOG_CDECL setSourcePatternF(Painter& self, const PatternF& pattern);
  static err_t FOG_CDECL setSourcePatternD(Painter& self, const PatternD& pattern);
  static err_t FOG_CDECL setSourceAbstract(Painter& self, uint32_t sourceId, const void* value, const void* tr);

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL getTransform(const Painter& self, Any& tr);
  static err_t FOG_CDECL setTransform(Painter& self, const Any& tr);
  static err_t FOG_CDECL applyTransform(Painter& self, uint32_t transformOp, const void* params);
  static err_t FOG_CDECL resetTransform(Painter& self);

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL save(Painter& self);
  static err_t FOG_CDECL restore(Painter& self);

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL mapPoint(const Painter& self, uint32_t mapOp, Any& pt);

  // --------------------------------------------------------------------------
  // [Draw/Fill]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL fillAll(Painter& self);

  static err_t FOG_CDECL doRect(Painter& self, const Any& r);
  static err_t FOG_CDECL doRects(Painter& self, const Any* r, size_t count);
  static err_t FOG_CDECL doPoly(Painter& self, const Any* p, size_t count);
  static err_t FOG_CDECL doShape(Painter& self, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL doPath(Painter& self, const Any& p);
  static err_t FOG_CDECL doText(Painter& self, const Any& p, const String& text, const Font& font, const Any* clip);
  static err_t FOG_CDECL doMask(Painter& self, const Any& p, const Image& m, const Any* mFragment);
  static err_t FOG_CDECL doRegion(Painter& self, const Region& r);

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL blitImage(Painter& self, const Any& p, const Image& i, const Any* iFragment);
  static err_t FOG_CDECL blitMaskedImage(Painter& self, const Any& p, const Image& i, const Image& m, const Any* iFragment, const Any* mFragment);

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL clipRect(Painter& self, uint32_t clipOp, const Any& r);
  static err_t FOG_CDECL clipRects(Painter& self, uint32_t clipOp, const Any* r, size_t count);
  static err_t FOG_CDECL clipPoly(Painter& self, uint32_t clipOp, const Any* p, size_t count);
  static err_t FOG_CDECL clipShape(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);
  static err_t FOG_CDECL clipPath(Painter& self, uint32_t clipOp, const Any& p);
  static err_t FOG_CDECL clipText(Painter& self, uint32_t clipOp, const Any& p, const String& text, const Font& font, const Any* clip);
  static err_t FOG_CDECL clipMask(Painter& self, uint32_t clipOp, const Any& p, const Image& m, const Any* mFragment);
  static err_t FOG_CDECL clipRegion(Painter& self, uint32_t clipOp, const Region& r);
  static err_t FOG_CDECL resetClip(Painter& self);

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL beginLayer(Painter& self);
  static err_t FOG_CDECL endLayer(Painter& self);

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  static err_t FOG_CDECL flush(Painter& self, uint32_t flags);
};

// ============================================================================
// [Fog::NullPainterImpl - AddRef / Release]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::release(Painter& self)
{
  return ERR_OK;
}

// ============================================================================
// [Fog::NullPainterImpl - Meta Params]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::getMetaParams(const Painter& self, Region& region, PointI& origin)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL NullPainterImpl::setMetaParams(Painter& self, const Region& region, const PointI& origin)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL NullPainterImpl::resetMetaParams(Painter& self)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::NullPainterImpl - User Params]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::getUserParams(const Painter& self, Region& region, PointI& origin)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL NullPainterImpl::setUserParams(Painter& self, const Region& region, const PointI& origin)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t FOG_CDECL NullPainterImpl::resetUserParams(Painter& self)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::NullPainterImpl - Parameters]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::getParameter(const Painter& self, uint32_t parameterId, void* value)
{
  switch (parameterId)
  {
    case PAINTER_PARAMETER_SIZE_I:
    {
      reinterpret_cast<SizeI*>(value)->reset();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_F:
    {
      reinterpret_cast<SizeF*>(value)->reset();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_SIZE_D:
    {
      reinterpret_cast<SizeD*>(value)->reset();
      return ERR_OK;
    }

    case PAINTER_PARAMETER_FORMAT_I:
    {
      *reinterpret_cast<uint32_t*>(value) = IMAGE_FORMAT_NULL;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_DEVICE_I:
    {
      *reinterpret_cast<uint32_t*>(value) = PAINT_DEVICE_NULL;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MULTITHREADED_I:
    {
      *reinterpret_cast<uint32_t*>(value) = 0;
      return ERR_OK;
    }

    case PAINTER_PARAMETER_MAX_THREADS_I:
    {
      *reinterpret_cast<uint32_t*>(value) = 1;
      return ERR_OK;
    }

    default:
    {
      return ERR_RT_INVALID_STATE;
    }
  }
}

err_t FOG_CDECL NullPainterImpl::setParameter(Painter& self, uint32_t parameterId, const void* value)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::resetParameter(Painter& self, uint32_t parameterId)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - Source]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::getSourceType(const Painter& self, uint32_t& val)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::getSourceColor(const Painter& self, Color& color)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::getSourcePatternF(const Painter& self, PatternF& pattern)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::getSourcePatternD(const Painter& self, PatternD& pattern)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::setSourceArgb32(Painter& self, uint32_t argb32)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::setSourceArgb64(Painter& self, const Argb64& argb64)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::setSourceColor(Painter& self, const Color& color)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::setSourcePatternF(Painter& self, const PatternF& pattern)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::setSourcePatternD(Painter& self, const PatternD& pattern)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::setSourceAbstract(Painter& self, uint32_t sourceId, const void* value, const void* tr)
{
  return ERR_RT_INVALID_STATE;
}


// ============================================================================
// [Fog::NullPainterImpl - Transform]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::getTransform(const Painter& self, Any& tr)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::setTransform(Painter& self, const Any& tr)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::applyTransform(Painter& self, uint32_t transformOp, const void* params)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::resetTransform(Painter& self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - State]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::save(Painter& self)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::restore(Painter& self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - Map]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::mapPoint(const Painter& self, uint32_t mapOp, Any& pt)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - Draw/Fill]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::fillAll(Painter& self)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doRect(Painter& self, const Any& r)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doRects(Painter& self, const Any* r, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doPoly(Painter& self, const Any* p, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doShape(Painter& self, uint32_t shapeType, const void* shapeData)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doPath(Painter& self, const Any& p)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doText(Painter& self, const Any& p, const String& text, const Font& font, const Any* clip)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doMask(Painter& self, const Any& p, const Image& m, const Any* mFragment)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::doRegion(Painter& self, const Region& r)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - Blit]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::blitImage(Painter& self, const Any& p, const Image& i, const Any* iFragment)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::blitMaskedImage(Painter& self, const Any& p, const Image& i, const Image& m, const Any* iFragment, const Any* mFragment)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - Clip]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::clipRect(Painter& self, uint32_t clipOp, const Any& r)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::clipRects(Painter& self, uint32_t clipOp, const Any* r, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::clipPoly(Painter& self, uint32_t clipOp, const Any* p, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::clipShape(Painter& self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::clipPath(Painter& self, uint32_t clipOp, const Any& p)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::clipText(Painter& self, uint32_t clipOp, const Any& p, const String& text, const Font& font, const Any* clip)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::clipMask(Painter& self, uint32_t clipOp, const Any& p, const Image& m, const Any* mFragment)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::clipRegion(Painter& self, uint32_t clipOp, const Region& r)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::resetClip(Painter& self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - Layer]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::beginLayer(Painter& self)
{
  return ERR_RT_INVALID_STATE;
}

err_t FOG_CDECL NullPainterImpl::endLayer(Painter& self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPainterImpl - Flush]
// ============================================================================

err_t FOG_CDECL NullPainterImpl::flush(Painter& self, uint32_t flags)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Statics]
// ============================================================================

static NullPaintEngine NullPaintEngine_instance;

static PaintEngine* FOG_CDECL NullPaintEngine_getNullEngine()
{
  return &NullPaintEngine_instance;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Painter_initNull()
{
  _api.painter.getNullEngine = NullPaintEngine_getNullEngine;

  PainterVTable& v = NullPaintEngine_vtable;

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  v.release = NullPainterImpl::release;

  // --------------------------------------------------------------------------
  // Meta Params]
  // --------------------------------------------------------------------------

  v.getMetaParams = NullPainterImpl::getMetaParams;
  v.setMetaParams = NullPainterImpl::setMetaParams;
  v.resetMetaParams = NullPainterImpl::resetMetaParams;

  // --------------------------------------------------------------------------
  // [User Params]
  // --------------------------------------------------------------------------

  v.getUserParams = NullPainterImpl::getUserParams;
  v.setUserParams = NullPainterImpl::setUserParams;
  v.resetUserParams = NullPainterImpl::resetUserParams;

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  v.getParameter = NullPainterImpl::getParameter;
  v.setParameter = NullPainterImpl::setParameter;
  v.resetParameter = NullPainterImpl::resetParameter;

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  v.getSourceType = NullPainterImpl::getSourceType;

  v.getSourceColor = NullPainterImpl::getSourceColor;
  v.getSourcePatternF = NullPainterImpl::getSourcePatternF;
  v.getSourcePatternD = NullPainterImpl::getSourcePatternD;

  v.setSourceArgb32 = NullPainterImpl::setSourceArgb32;
  v.setSourceArgb64 = NullPainterImpl::setSourceArgb64;
  v.setSourceColor = NullPainterImpl::setSourceColor;
  v.setSourcePatternF = NullPainterImpl::setSourcePatternF;
  v.setSourcePatternD = NullPainterImpl::setSourcePatternD;
  v.setSourceAbstract = NullPainterImpl::setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  v.getTransformF = (PainterVTable::GetTransformF)NullPainterImpl::getTransform;
  v.getTransformD = (PainterVTable::GetTransformD)NullPainterImpl::getTransform;

  v.setTransformF = (PainterVTable::SetTransformF)NullPainterImpl::setTransform;
  v.setTransformD = (PainterVTable::SetTransformD)NullPainterImpl::setTransform;

  v.applyTransformF = (PainterVTable::ApplyTransformF)NullPainterImpl::applyTransform;
  v.applyTransformD = (PainterVTable::ApplyTransformD)NullPainterImpl::applyTransform;

  v.resetTransform = (PainterVTable::ResetTransform)NullPainterImpl::resetTransform;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  v.save = NullPainterImpl::save;
  v.restore = NullPainterImpl::restore;

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  v.mapPointF = (PainterVTable::MapPointF)NullPainterImpl::mapPoint;
  v.mapPointD = (PainterVTable::MapPointD)NullPainterImpl::mapPoint;

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  v.drawRectI = (PainterVTable::PaintRectI)NullPainterImpl::doRect;
  v.drawRectF = (PainterVTable::PaintRectF)NullPainterImpl::doRect;
  v.drawRectD = (PainterVTable::PaintRectD)NullPainterImpl::doRect;

  v.drawPolylineI = (PainterVTable::PaintPolylineI)NullPainterImpl::doPoly;
  v.drawPolylineF = (PainterVTable::PaintPolylineF)NullPainterImpl::doPoly;
  v.drawPolylineD = (PainterVTable::PaintPolylineD)NullPainterImpl::doPoly;

  v.drawPolygonI = (PainterVTable::PaintPolygonI)NullPainterImpl::doPoly;
  v.drawPolygonF = (PainterVTable::PaintPolygonF)NullPainterImpl::doPoly;
  v.drawPolygonD = (PainterVTable::PaintPolygonD)NullPainterImpl::doPoly;

  v.drawShapeF = (PainterVTable::PaintShapeF)NullPainterImpl::doShape;
  v.drawShapeD = (PainterVTable::PaintShapeD)NullPainterImpl::doShape;

  v.drawPathF = (PainterVTable::PaintPathF)NullPainterImpl::doPath;
  v.drawPathD = (PainterVTable::PaintPathD)NullPainterImpl::doPath;

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  v.fillAll = NullPainterImpl::fillAll;

  v.fillRectI = (PainterVTable::PaintRectI)NullPainterImpl::doRect;
  v.fillRectF = (PainterVTable::PaintRectF)NullPainterImpl::doRect;
  v.fillRectD = (PainterVTable::PaintRectD)NullPainterImpl::doRect;

  v.fillRectsI = (PainterVTable::PaintRectsI)NullPainterImpl::doRects;
  v.fillRectsF = (PainterVTable::PaintRectsF)NullPainterImpl::doRects;
  v.fillRectsD = (PainterVTable::PaintRectsD)NullPainterImpl::doRects;

  v.fillPolygonI = (PainterVTable::PaintPolygonI)NullPainterImpl::doPoly;
  v.fillPolygonF = (PainterVTable::PaintPolygonF)NullPainterImpl::doPoly;
  v.fillPolygonD = (PainterVTable::PaintPolygonD)NullPainterImpl::doPoly;

  v.fillShapeF = (PainterVTable::PaintShapeF)NullPainterImpl::doShape;
  v.fillShapeD = (PainterVTable::PaintShapeD)NullPainterImpl::doShape;

  v.fillPathF = (PainterVTable::PaintPathF)NullPainterImpl::doPath;
  v.fillPathD = (PainterVTable::PaintPathD)NullPainterImpl::doPath;

  v.fillTextAtI = (PainterVTable::PaintTextAtI)NullPainterImpl::doText;
  v.fillTextAtF = (PainterVTable::PaintTextAtF)NullPainterImpl::doText;
  v.fillTextAtD = (PainterVTable::PaintTextAtD)NullPainterImpl::doText;

  v.fillTextInI = (PainterVTable::PaintTextInI)NullPainterImpl::doText;
  v.fillTextInF = (PainterVTable::PaintTextInF)NullPainterImpl::doText;
  v.fillTextInD = (PainterVTable::PaintTextInD)NullPainterImpl::doText;

  v.fillMaskAtI = (PainterVTable::PaintMaskAtI)NullPainterImpl::doMask;
  v.fillMaskAtF = (PainterVTable::PaintMaskAtF)NullPainterImpl::doMask;
  v.fillMaskAtD = (PainterVTable::PaintMaskAtD)NullPainterImpl::doMask;

  v.fillMaskInI = (PainterVTable::PaintMaskInI)NullPainterImpl::doMask;
  v.fillMaskInF = (PainterVTable::PaintMaskInF)NullPainterImpl::doMask;
  v.fillMaskInD = (PainterVTable::PaintMaskInD)NullPainterImpl::doMask;

  v.fillRegion = NullPainterImpl::doRegion;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v.blitImageAtI = (PainterVTable::BlitImageAtI)NullPainterImpl::blitImage;
  v.blitImageAtF = (PainterVTable::BlitImageAtF)NullPainterImpl::blitImage;
  v.blitImageAtD = (PainterVTable::BlitImageAtD)NullPainterImpl::blitImage;

  v.blitImageInI = (PainterVTable::BlitImageInI)NullPainterImpl::blitImage;
  v.blitImageInF = (PainterVTable::BlitImageInF)NullPainterImpl::blitImage;
  v.blitImageInD = (PainterVTable::BlitImageInD)NullPainterImpl::blitImage;

  v.blitMaskedImageAtI = (PainterVTable::BlitMaskedImageAtI)NullPainterImpl::blitMaskedImage;
  v.blitMaskedImageAtF = (PainterVTable::BlitMaskedImageAtF)NullPainterImpl::blitMaskedImage;
  v.blitMaskedImageAtD = (PainterVTable::BlitMaskedImageAtD)NullPainterImpl::blitMaskedImage;

  v.blitMaskedImageInI = (PainterVTable::BlitMaskedImageInI)NullPainterImpl::blitMaskedImage;
  v.blitMaskedImageInF = (PainterVTable::BlitMaskedImageInF)NullPainterImpl::blitMaskedImage;
  v.blitMaskedImageInD = (PainterVTable::BlitMaskedImageInD)NullPainterImpl::blitMaskedImage;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  v.clipRectI = (PainterVTable::ClipRectI)NullPainterImpl::clipRect;
  v.clipRectF = (PainterVTable::ClipRectF)NullPainterImpl::clipRect;
  v.clipRectD = (PainterVTable::ClipRectD)NullPainterImpl::clipRect;

  v.clipRectsI = (PainterVTable::ClipRectsI)NullPainterImpl::clipRects;
  v.clipRectsF = (PainterVTable::ClipRectsF)NullPainterImpl::clipRects;
  v.clipRectsD = (PainterVTable::ClipRectsD)NullPainterImpl::clipRects;

  v.clipPolygonI = (PainterVTable::ClipPolygonI)NullPainterImpl::clipPoly;
  v.clipPolygonF = (PainterVTable::ClipPolygonF)NullPainterImpl::clipPoly;
  v.clipPolygonD = (PainterVTable::ClipPolygonD)NullPainterImpl::clipPoly;

  v.clipShapeF = (PainterVTable::ClipShapeF)NullPainterImpl::clipShape;
  v.clipShapeD = (PainterVTable::ClipShapeD)NullPainterImpl::clipShape;

  v.clipPathF = (PainterVTable::ClipPathF)NullPainterImpl::clipPath;
  v.clipPathD = (PainterVTable::ClipPathD)NullPainterImpl::clipPath;

  v.clipTextAtI = (PainterVTable::ClipTextAtI)NullPainterImpl::clipText;
  v.clipTextAtF = (PainterVTable::ClipTextAtF)NullPainterImpl::clipText;
  v.clipTextAtD = (PainterVTable::ClipTextAtD)NullPainterImpl::clipText;

  v.clipTextInI = (PainterVTable::ClipTextInI)NullPainterImpl::clipText;
  v.clipTextInF = (PainterVTable::ClipTextInF)NullPainterImpl::clipText;
  v.clipTextInD = (PainterVTable::ClipTextInD)NullPainterImpl::clipText;

  v.clipMaskAtI = (PainterVTable::ClipMaskAtI)NullPainterImpl::clipMask;
  v.clipMaskAtF = (PainterVTable::ClipMaskAtF)NullPainterImpl::clipMask;
  v.clipMaskAtD = (PainterVTable::ClipMaskAtD)NullPainterImpl::clipMask;

  v.clipMaskInI = (PainterVTable::ClipMaskInI)NullPainterImpl::clipMask;
  v.clipMaskInF = (PainterVTable::ClipMaskInF)NullPainterImpl::clipMask;
  v.clipMaskInD = (PainterVTable::ClipMaskInD)NullPainterImpl::clipMask;

  v.clipRegion = NullPainterImpl::clipRegion;

  v.resetClip = NullPainterImpl::resetClip;

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  v.beginLayer = NullPainterImpl::beginLayer;
  v.endLayer = NullPainterImpl::endLayer;

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  v.flush = NullPainterImpl::flush;

  // --------------------------------------------------------------------------
  // [NullPaintEngine]
  // --------------------------------------------------------------------------

  NullPaintEngine_instance.vtable = &NullPaintEngine_vtable;
}

} // Fog namespace
