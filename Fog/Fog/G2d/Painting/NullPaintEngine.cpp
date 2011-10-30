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

// Skeleton for new paint-engine.
#if 0
// ============================================================================
// [Fog::MyPaintEngine]
// ============================================================================

struct FOG_NO_EXPORT MyPaintEngine : public PaintEngine
{
};

// ============================================================================
// [Fog::MyPaintEngine - AddRef / Release]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_release(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Meta Params]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_getMetaParams(const Painter* self, Region& region, PointI& origin)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setMetaParams(Painter* self, const Region& region, const PointI& origin)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_resetMetaParams(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Parameters]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_getParameter(const Painter* self, uint32_t parameterId, void* value)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setParameter(Painter* self, uint32_t parameterId, const void* value)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_resetParameter(Painter* self, uint32_t parameterId)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Source]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_getSourceType(const Painter* self, uint32_t& val)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_getSourceColor(const Painter* self, Color& color)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_getSourcePattern(const Painter* self, Pattern& pattern)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}


static err_t FOG_CDECL MyPaintEngine_setSourceNone(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setSourceArgb32(Painter* self, uint32_t argb32)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setSourceArgb64(Painter* self, const Argb64& argb64)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setSourceColor(Painter* self, const Color& color)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setSourcePattern(Painter* self, const Pattern& pattern)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setSourceAbstract(Painter* self, uint32_t sourceId, const void* value, const void* tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Transform]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_getTransformF(const Painter* self, TransformF& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_getTransformD(const Painter* self, TransformD& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setTransformF(Painter* self, const TransformF& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_setTransformD(Painter* self, const TransformD& tr)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_applyTransform(Painter* self, uint32_t transformOp, const void* params)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_resetTransform(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - State]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_save(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_restore(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Map]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_mapPointF(const Painter* self, uint32_t mapOp, PointF& pt)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_mapPointD(const Painter* self, uint32_t mapOp, PointD& pt)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Draw]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_drawRectI(Painter* self, const RectI& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawRectF(Painter* self, const RectF& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawRectD(Painter* self, const RectD& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPolylineI(Painter* self, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPolylineF(Painter* self, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPolylineD(Painter* self, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPolygonI(Painter* self, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPolygonF(Painter* self, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPolygonD(Painter* self, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawShapeF(Painter* self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawShapeD(Painter* self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPathF(Painter* self, const PathF& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_drawPathD(Painter* self, const PathD& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Fill]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_fillAll(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillRectI(Painter* self, const RectI& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillRectF(Painter* self, const RectF& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillRectD(Painter* self, const RectD& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillRectsI(Painter* self, const RectI* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillRectsF(Painter* self, const RectF* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillRectsD(Painter* self, const RectD* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillPolygonI(Painter* self, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillPolygonF(Painter* self, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillPolygonD(Painter* self, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillShapeF(Painter* self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillShapeD(Painter* self, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillPathF(Painter* self, const PathF& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillPathD(Painter* self, const PathD& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillTextAtI(Painter* self, const PointI& p, const StringW& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillTextAtF(Painter* self, const PointF& p, const StringW& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillTextAtD(Painter* self, const PointD& p, const StringW& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillTextInI(Painter* self, const TextRectI& r, const StringW& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillTextInF(Painter* self, const TextRectF& r, const StringW& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillTextInD(Painter* self, const TextRectD& r, const StringW& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillMaskAtI(Painter* self, const PointI& p, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillMaskAtF(Painter* self, const PointF& p, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillMaskAtD(Painter* self, const PointD& p, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillMaskInI(Painter* self, const RectI& r, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillMaskInF(Painter* self, const RectF& r, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillMaskInD(Painter* self, const RectD& r, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_fillRegion(Painter* self, const Region& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Blit]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_blitImageAtI(Painter* self, const PointI& p, const Image& src, const RectI* srcFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitImageAtF(Painter* self, const PointF& p, const Image& src, const RectI* srcFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitImageAtD(Painter* self, const PointD& p, const Image& src, const RectI* srcFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitImageInI(Painter* self, const RectI& r, const Image& src, const RectI* sFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitImageInF(Painter* self, const RectF& r, const Image& src, const RectI* sFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitImageInD(Painter* self, const RectD& r, const Image& src, const RectI* sFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitMaskedImageAtI(Painter* self, const PointI& p, const Image& src, const Image& mask, const RectI* sFragment, const RectI* maskFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitMaskedImageAtF(Painter* self, const PointF& p, const Image& src, const Image& mask, const RectI* sFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitMaskedImageAtD(Painter* self, const PointD& p, const Image& src, const Image& mask, const RectI* sFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitMaskedImageInI(Painter* self, const RectI& r, const Image& src, const Image& mask, const RectI* sFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitMaskedImageInF(Painter* self, const RectF& r, const Image& src, const Image& mask, const RectI* sFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_blitMaskedImageInD(Painter* self, const RectD& r, const Image& src, const Image& mask, const RectI* sFragment, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Clip]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_clipRectI(Painter* self, uint32_t clipOp, const RectI& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipRectF(Painter* self, uint32_t clipOp, const RectF& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipRectD(Painter* self, uint32_t clipOp, const RectD& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipRectsI(Painter* self, uint32_t clipOp, const RectI* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipRectsF(Painter* self, uint32_t clipOp, const RectF* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipRectsD(Painter* self, uint32_t clipOp, const RectD* r, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipPolygonI(Painter* self, uint32_t clipOp, const PointI* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipPolygonF(Painter* self, uint32_t clipOp, const PointF* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipPolygonD(Painter* self, uint32_t clipOp, const PointD* p, size_t count)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipShapeF(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipShapeD(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipPathF(Painter* self, uint32_t clipOp, const PathF& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipPathD(Painter* self, uint32_t clipOp, const PathD& p)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipTextAtI(Painter* self, uint32_t clipOp, const PointI& p, const StringW& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipTextAtF(Painter* self, uint32_t clipOp, const PointF& p, const StringW& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipTextAtD(Painter* self, uint32_t clipOp, const PointD& p, const StringW& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipTextInI(Painter* self, uint32_t clipOp, const TextRectI& r, const StringW& text, const Font& font, const RectI* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipTextInF(Painter* self, uint32_t clipOp, const TextRectF& r, const StringW& text, const Font& font, const RectF* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipTextInD(Painter* self, uint32_t clipOp, const TextRectD& r, const StringW& text, const Font& font, const RectD* clip)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipMaskAtI(Painter* self, uint32_t clipOp, const PointI& p, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipMaskAtF(Painter* self, uint32_t clipOp, const PointF& p, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipMaskAtD(Painter* self, uint32_t clipOp, const PointD& p, const Image& m, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipMaskInI(Painter* self, uint32_t clipOp, const RectI& r, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipMaskInF(Painter* self, uint32_t clipOp, const RectF& r, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipMaskInD(Painter* self, uint32_t clipOp, const RectD& r, const Image& mask, const RectI* mFragment)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_clipRegion(Painter* self, uint32_t clipOp, const Region& r)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_resetClip(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Layer]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_beginLayer(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL MyPaintEngine_endLayer(Painter* self)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - Flush]
// ============================================================================

static err_t FOG_CDECL MyPaintEngine_flush(Painter* self, uint32_t flags)
{
  MyPaintEngine* engine = reinterpret_cast<MyPaintEngine*>(self->_engine);
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MyPaintEngine - VTable]
// ============================================================================

static PaintEngineVTable MyPaintEngine_vtable;

// ============================================================================
// [Fog::MyPaintEngine - Init / Fini]
// ============================================================================

static void MyPaintEngine_init()
{
  PaintEngineVTable& v = MyPaintEngine_vtable;

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  v.release = MyPaintEngine_release;

  // --------------------------------------------------------------------------
  // Meta Params]
  // --------------------------------------------------------------------------

  v.getMetaParams = MyPaintEngine_getMetaParams;
  v.setMetaParams = MyPaintEngine_setMetaParams;
  v.resetMetaParams = MyPaintEngine_resetMetaParams;

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  v.getParameter = MyPaintEngine_getParameter;
  v.setParameter = MyPaintEngine_setParameter;
  v.resetParameter = MyPaintEngine_resetParameter;

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  v.getSourceType = MyPaintEngine_getSourceType;

  v.getSourceColor = MyPaintEngine_getSourceColor;
  v.getSourcePattern = MyPaintEngine_getSourcePattern;

  v.setSourceNone = MyPaintEngine_setSourceNone;
  v.setSourceArgb32 = MyPaintEngine_setSourceArgb32;
  v.setSourceArgb64 = MyPaintEngine_setSourceArgb64;
  v.setSourceColor = MyPaintEngine_setSourceColor;
  v.setSourcePattern = MyPaintEngine_setSourcePattern;
  v.setSourceAbstract = MyPaintEngine_setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  v.getTransformF = MyPaintEngine_getTransformF;
  v.getTransformD = MyPaintEngine_getTransformD;

  v.setTransformF = MyPaintEngine_setTransformF;
  v.setTransformD = MyPaintEngine_setTransformD;

  v.applyTransform = MyPaintEngine_applyTransform;

  v.resetTransform = MyPaintEngine_resetTransform;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  v.save = MyPaintEngine_save;
  v.restore = MyPaintEngine_restore;

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  v.mapPointF = MyPaintEngine_mapPointF;
  v.mapPointD = MyPaintEngine_mapPointD;

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  v.drawRectI = MyPaintEngine_drawRectI;
  v.drawRectF = MyPaintEngine_drawRectF;
  v.drawRectD = MyPaintEngine_drawRectD;

  v.drawPolylineI = MyPaintEngine_drawPolylineI;
  v.drawPolylineF = MyPaintEngine_drawPolylineF;
  v.drawPolylineD = MyPaintEngine_drawPolylineD;

  v.drawPolygonI = MyPaintEngine_drawPolygonI;
  v.drawPolygonF = MyPaintEngine_drawPolygonF;
  v.drawPolygonD = MyPaintEngine_drawPolygonD;

  v.drawShapeF = MyPaintEngine_drawShapeF;
  v.drawShapeD = MyPaintEngine_drawShapeD;

  v.drawPathF = MyPaintEngine_drawPathF;
  v.drawPathD = MyPaintEngine_drawPathD;

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  v.fillAll = MyPaintEngine_fillAll;

  v.fillRectI = MyPaintEngine_fillRectI;
  v.fillRectF = MyPaintEngine_fillRectF;
  v.fillRectD = MyPaintEngine_fillRectD;

  v.fillRectsI = MyPaintEngine_fillRectsI;
  v.fillRectsF = MyPaintEngine_fillRectsF;
  v.fillRectsD = MyPaintEngine_fillRectsD;

  v.fillPolygonI = MyPaintEngine_fillPolygonI;
  v.fillPolygonF = MyPaintEngine_fillPolygonF;
  v.fillPolygonD = MyPaintEngine_fillPolygonD;

  v.fillShapeF = MyPaintEngine_fillShapeF;
  v.fillShapeD = MyPaintEngine_fillShapeD;

  v.fillPathF = MyPaintEngine_fillPathF;
  v.fillPathD = MyPaintEngine_fillPathD;

  v.fillTextAtI = MyPaintEngine_fillTextAtI;
  v.fillTextAtF = MyPaintEngine_fillTextAtF;
  v.fillTextAtD = MyPaintEngine_fillTextAtD;

  v.fillTextInI = MyPaintEngine_fillTextInI;
  v.fillTextInF = MyPaintEngine_fillTextInF;
  v.fillTextInD = MyPaintEngine_fillTextInD;

  v.fillMaskAtI = MyPaintEngine_fillMaskAtI;
  v.fillMaskAtF = MyPaintEngine_fillMaskAtF;
  v.fillMaskAtD = MyPaintEngine_fillMaskAtD;

  v.fillMaskInI = MyPaintEngine_fillMaskInI;
  v.fillMaskInF = MyPaintEngine_fillMaskInF;
  v.fillMaskInD = MyPaintEngine_fillMaskInD;

  v.fillRegion = MyPaintEngine_fillRegion;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v.blitImageAtI = MyPaintEngine_blitImageAtI;
  v.blitImageAtF = MyPaintEngine_blitImageAtF;
  v.blitImageAtD = MyPaintEngine_blitImageAtD;

  v.blitImageInI = MyPaintEngine_blitImageInI;
  v.blitImageInF = MyPaintEngine_blitImageInF;
  v.blitImageInD = MyPaintEngine_blitImageInD;

  v.blitMaskedImageAtI = MyPaintEngine_blitMaskedImageAtI;
  v.blitMaskedImageAtF = MyPaintEngine_blitMaskedImageAtF;
  v.blitMaskedImageAtD = MyPaintEngine_blitMaskedImageAtD;

  v.blitMaskedImageInI = MyPaintEngine_blitMaskedImageInI;
  v.blitMaskedImageInF = MyPaintEngine_blitMaskedImageInF;
  v.blitMaskedImageInD = MyPaintEngine_blitMaskedImageInD;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  v.clipRectI = MyPaintEngine_clipRectI;
  v.clipRectF = MyPaintEngine_clipRectF;
  v.clipRectD = MyPaintEngine_clipRectD;

  v.clipRectsI = MyPaintEngine_clipRectsI;
  v.clipRectsF = MyPaintEngine_clipRectsF;
  v.clipRectsD = MyPaintEngine_clipRectsD;

  v.clipPolygonI = MyPaintEngine_clipPolygonI;
  v.clipPolygonF = MyPaintEngine_clipPolygonF;
  v.clipPolygonD = MyPaintEngine_clipPolygonD;

  v.clipShapeF = MyPaintEngine_clipShapeF;
  v.clipShapeD = MyPaintEngine_clipShapeD;

  v.clipPathF = MyPaintEngine_clipPathF;
  v.clipPathD = MyPaintEngine_clipPathD;

  v.clipTextAtI = MyPaintEngine_clipTextAtI;
  v.clipTextAtF = MyPaintEngine_clipTextAtF;
  v.clipTextAtD = MyPaintEngine_clipTextAtD;

  v.clipTextInI = MyPaintEngine_clipTextInI;
  v.clipTextInF = MyPaintEngine_clipTextInF;
  v.clipTextInD = MyPaintEngine_clipTextInD;

  v.clipMaskAtI = MyPaintEngine_clipMaskAtI;
  v.clipMaskAtF = MyPaintEngine_clipMaskAtF;
  v.clipMaskAtD = MyPaintEngine_clipMaskAtD;

  v.clipMaskInI = MyPaintEngine_clipMaskInI;
  v.clipMaskInF = MyPaintEngine_clipMaskInF;
  v.clipMaskInD = MyPaintEngine_clipMaskInD;

  v.clipRegion = MyPaintEngine_clipRegion;

  v.resetClip = MyPaintEngine_resetClip;

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  v.beginLayer = MyPaintEngine_beginLayer;
  v.endLayer = MyPaintEngine_endLayer;

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  v.flush = MyPaintEngine_flush;
}
#endif

// ============================================================================
// [Fog::NullPaintEngine - Helpers]
// ============================================================================

struct FOG_NO_EXPORT Any {};

// ============================================================================
// [Fog::NullPaintEngine - VTable]
// ============================================================================

static PaintEngineVTable NullPaintEngine_vtable;

// ============================================================================
// [Fog::NullPaintEngine - AddRef / Release]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_release(Painter* self)
{
  return ERR_OK;
}

// ============================================================================
// [Fog::NullPaintEngine - Meta Params]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_getMetaParams(const Painter* self, Region& region, PointI& origin)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL NullPaintEngine_setMetaParams(Painter* self, const Region& region, const PointI& origin)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

static err_t FOG_CDECL NullPaintEngine_resetMetaParams(Painter* self)
{
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::NullPaintEngine - Parameters]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_getParameter(const Painter* self, uint32_t parameterId, void* value)
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

static err_t FOG_CDECL NullPaintEngine_setParameter(Painter* self, uint32_t parameterId, const void* value)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_resetParameter(Painter* self, uint32_t parameterId)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Source]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_getSourceType(const Painter* self, uint32_t& val)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_getSourceColor(const Painter* self, Color& color)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_getSourcePattern(const Painter* self, Pattern& pattern)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_setSourceNone(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_setSourceArgb32(Painter* self, uint32_t argb32)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_setSourceAny(Painter* self, const Any& any)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_setSourceAbstract(Painter* self, uint32_t sourceId, const void* value, const void* tr)
{
  return ERR_RT_INVALID_STATE;
}


// ============================================================================
// [Fog::NullPaintEngine - Transform]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_getTransform(const Painter* self, Any& tr)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_setTransform(Painter* self, const Any& tr)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_applyTransform(Painter* self, uint32_t transformOp, const void* params)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_resetTransform(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - State]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_save(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_restore(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Map]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_mapPoint(const Painter* self, uint32_t mapOp, Any& pt)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Draw/Fill]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_fillAll(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doRect(Painter* self, const Any& r)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doRects(Painter* self, const Any* r, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doPoly(Painter* self, const Any* p, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doShape(Painter* self, uint32_t shapeType, const void* shapeData)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doPath(Painter* self, const Any& p)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doText(Painter* self, const Any& p, const StringW& text, const Font& font, const Any* clip)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doMask(Painter* self, const Any& p, const Image& mask, const Any* mFragment)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_doRegion(Painter* self, const Region& r)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Blit]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_blitImage(Painter* self, const Any& p, const Image& src, const Any* sFragment)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_blitMaskedImage(Painter* self, const Any& p, const Image& src, const Image& mask, const Any* sFragment, const Any* mFragment)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Clip]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_clipRect(Painter* self, uint32_t clipOp, const Any& r)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_clipRects(Painter* self, uint32_t clipOp, const Any* r, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_clipPoly(Painter* self, uint32_t clipOp, const Any* p, size_t count)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_clipShape(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_clipPath(Painter* self, uint32_t clipOp, const Any& p)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_clipText(Painter* self, uint32_t clipOp, const Any& p, const StringW& text, const Font& font, const Any* clip)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_clipMask(Painter* self, uint32_t clipOp, const Any& p, const Image& mask, const Any* mFragment)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_clipRegion(Painter* self, uint32_t clipOp, const Region& r)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_resetClip(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Layer]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_beginLayer(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

static err_t FOG_CDECL NullPaintEngine_endLayer(Painter* self)
{
  return ERR_RT_INVALID_STATE;
}

// ============================================================================
// [Fog::NullPaintEngine - Flush]
// ============================================================================

static err_t FOG_CDECL NullPaintEngine_flush(Painter* self, uint32_t flags)
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
// [Fog::NullPaintEngine - Init / Fini]
// ============================================================================

FOG_NO_EXPORT void NullPaintEngine_init()
{
  fog_api.painter_getNullEngine = NullPaintEngine_getNullEngine;
  NullPaintEngine_instance.vtable = &NullPaintEngine_vtable;

  PaintEngineVTable& v = NullPaintEngine_vtable;

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  v.release = NullPaintEngine_release;

  // --------------------------------------------------------------------------
  // Meta Params]
  // --------------------------------------------------------------------------

  v.getMetaParams = NullPaintEngine_getMetaParams;
  v.setMetaParams = NullPaintEngine_setMetaParams;
  v.resetMetaParams = NullPaintEngine_resetMetaParams;

  // --------------------------------------------------------------------------
  // [Parameters]
  // --------------------------------------------------------------------------

  v.getParameter = NullPaintEngine_getParameter;
  v.setParameter = NullPaintEngine_setParameter;
  v.resetParameter = NullPaintEngine_resetParameter;

  // --------------------------------------------------------------------------
  // [Source]
  // --------------------------------------------------------------------------

  v.getSourceType = NullPaintEngine_getSourceType;

  v.getSourceColor = NullPaintEngine_getSourceColor;
  v.getSourcePattern = NullPaintEngine_getSourcePattern;

  v.setSourceNone = NullPaintEngine_setSourceNone;
  v.setSourceArgb32 = NullPaintEngine_setSourceArgb32;
  v.setSourceArgb64 = (PaintEngineVTable::SetSourceArgb64)NullPaintEngine_setSourceAny;
  v.setSourceColor = (PaintEngineVTable::SetSourceColor)NullPaintEngine_setSourceAny;
  v.setSourcePattern = (PaintEngineVTable::SetSourcePattern)NullPaintEngine_setSourceAny;
  v.setSourceAbstract = NullPaintEngine_setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Transform]
  // --------------------------------------------------------------------------

  v.getTransformF = (PaintEngineVTable::GetTransformF)NullPaintEngine_getTransform;
  v.getTransformD = (PaintEngineVTable::GetTransformD)NullPaintEngine_getTransform;

  v.setTransformF = (PaintEngineVTable::SetTransformF)NullPaintEngine_setTransform;
  v.setTransformD = (PaintEngineVTable::SetTransformD)NullPaintEngine_setTransform;

  v.applyTransform = (PaintEngineVTable::ApplyTransform)NullPaintEngine_applyTransform;
  v.resetTransform = (PaintEngineVTable::ResetTransform)NullPaintEngine_resetTransform;

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  v.save = NullPaintEngine_save;
  v.restore = NullPaintEngine_restore;

  // --------------------------------------------------------------------------
  // [Map]
  // --------------------------------------------------------------------------

  v.mapPointF = (PaintEngineVTable::MapPointF)NullPaintEngine_mapPoint;
  v.mapPointD = (PaintEngineVTable::MapPointD)NullPaintEngine_mapPoint;

  // --------------------------------------------------------------------------
  // [Draw]
  // --------------------------------------------------------------------------

  v.drawRectI = (PaintEngineVTable::PaintRectI)NullPaintEngine_doRect;
  v.drawRectF = (PaintEngineVTable::PaintRectF)NullPaintEngine_doRect;
  v.drawRectD = (PaintEngineVTable::PaintRectD)NullPaintEngine_doRect;

  v.drawPolylineI = (PaintEngineVTable::PaintPolylineI)NullPaintEngine_doPoly;
  v.drawPolylineF = (PaintEngineVTable::PaintPolylineF)NullPaintEngine_doPoly;
  v.drawPolylineD = (PaintEngineVTable::PaintPolylineD)NullPaintEngine_doPoly;

  v.drawPolygonI = (PaintEngineVTable::PaintPolygonI)NullPaintEngine_doPoly;
  v.drawPolygonF = (PaintEngineVTable::PaintPolygonF)NullPaintEngine_doPoly;
  v.drawPolygonD = (PaintEngineVTable::PaintPolygonD)NullPaintEngine_doPoly;

  v.drawShapeF = (PaintEngineVTable::PaintShapeF)NullPaintEngine_doShape;
  v.drawShapeD = (PaintEngineVTable::PaintShapeD)NullPaintEngine_doShape;

  v.drawPathF = (PaintEngineVTable::PaintPathF)NullPaintEngine_doPath;
  v.drawPathD = (PaintEngineVTable::PaintPathD)NullPaintEngine_doPath;

  // --------------------------------------------------------------------------
  // [Fill]
  // --------------------------------------------------------------------------

  v.fillAll = NullPaintEngine_fillAll;

  v.fillRectI = (PaintEngineVTable::PaintRectI)NullPaintEngine_doRect;
  v.fillRectF = (PaintEngineVTable::PaintRectF)NullPaintEngine_doRect;
  v.fillRectD = (PaintEngineVTable::PaintRectD)NullPaintEngine_doRect;

  v.fillRectsI = (PaintEngineVTable::PaintRectsI)NullPaintEngine_doRects;
  v.fillRectsF = (PaintEngineVTable::PaintRectsF)NullPaintEngine_doRects;
  v.fillRectsD = (PaintEngineVTable::PaintRectsD)NullPaintEngine_doRects;

  v.fillPolygonI = (PaintEngineVTable::PaintPolygonI)NullPaintEngine_doPoly;
  v.fillPolygonF = (PaintEngineVTable::PaintPolygonF)NullPaintEngine_doPoly;
  v.fillPolygonD = (PaintEngineVTable::PaintPolygonD)NullPaintEngine_doPoly;

  v.fillShapeF = (PaintEngineVTable::PaintShapeF)NullPaintEngine_doShape;
  v.fillShapeD = (PaintEngineVTable::PaintShapeD)NullPaintEngine_doShape;

  v.fillPathF = (PaintEngineVTable::PaintPathF)NullPaintEngine_doPath;
  v.fillPathD = (PaintEngineVTable::PaintPathD)NullPaintEngine_doPath;

  v.fillTextAtI = (PaintEngineVTable::PaintTextAtI)NullPaintEngine_doText;
  v.fillTextAtF = (PaintEngineVTable::PaintTextAtF)NullPaintEngine_doText;
  v.fillTextAtD = (PaintEngineVTable::PaintTextAtD)NullPaintEngine_doText;

  v.fillTextInI = (PaintEngineVTable::PaintTextInI)NullPaintEngine_doText;
  v.fillTextInF = (PaintEngineVTable::PaintTextInF)NullPaintEngine_doText;
  v.fillTextInD = (PaintEngineVTable::PaintTextInD)NullPaintEngine_doText;

  v.fillMaskAtI = (PaintEngineVTable::PaintMaskAtI)NullPaintEngine_doMask;
  v.fillMaskAtF = (PaintEngineVTable::PaintMaskAtF)NullPaintEngine_doMask;
  v.fillMaskAtD = (PaintEngineVTable::PaintMaskAtD)NullPaintEngine_doMask;

  v.fillMaskInI = (PaintEngineVTable::PaintMaskInI)NullPaintEngine_doMask;
  v.fillMaskInF = (PaintEngineVTable::PaintMaskInF)NullPaintEngine_doMask;
  v.fillMaskInD = (PaintEngineVTable::PaintMaskInD)NullPaintEngine_doMask;

  v.fillRegion = NullPaintEngine_doRegion;

  // --------------------------------------------------------------------------
  // [Blit]
  // --------------------------------------------------------------------------

  v.blitImageAtI = (PaintEngineVTable::BlitImageAtI)NullPaintEngine_blitImage;
  v.blitImageAtF = (PaintEngineVTable::BlitImageAtF)NullPaintEngine_blitImage;
  v.blitImageAtD = (PaintEngineVTable::BlitImageAtD)NullPaintEngine_blitImage;

  v.blitImageInI = (PaintEngineVTable::BlitImageInI)NullPaintEngine_blitImage;
  v.blitImageInF = (PaintEngineVTable::BlitImageInF)NullPaintEngine_blitImage;
  v.blitImageInD = (PaintEngineVTable::BlitImageInD)NullPaintEngine_blitImage;

  v.blitMaskedImageAtI = (PaintEngineVTable::BlitMaskedImageAtI)NullPaintEngine_blitMaskedImage;
  v.blitMaskedImageAtF = (PaintEngineVTable::BlitMaskedImageAtF)NullPaintEngine_blitMaskedImage;
  v.blitMaskedImageAtD = (PaintEngineVTable::BlitMaskedImageAtD)NullPaintEngine_blitMaskedImage;

  v.blitMaskedImageInI = (PaintEngineVTable::BlitMaskedImageInI)NullPaintEngine_blitMaskedImage;
  v.blitMaskedImageInF = (PaintEngineVTable::BlitMaskedImageInF)NullPaintEngine_blitMaskedImage;
  v.blitMaskedImageInD = (PaintEngineVTable::BlitMaskedImageInD)NullPaintEngine_blitMaskedImage;

  // --------------------------------------------------------------------------
  // [Clip]
  // --------------------------------------------------------------------------

  v.clipRectI = (PaintEngineVTable::ClipRectI)NullPaintEngine_clipRect;
  v.clipRectF = (PaintEngineVTable::ClipRectF)NullPaintEngine_clipRect;
  v.clipRectD = (PaintEngineVTable::ClipRectD)NullPaintEngine_clipRect;

  v.clipRectsI = (PaintEngineVTable::ClipRectsI)NullPaintEngine_clipRects;
  v.clipRectsF = (PaintEngineVTable::ClipRectsF)NullPaintEngine_clipRects;
  v.clipRectsD = (PaintEngineVTable::ClipRectsD)NullPaintEngine_clipRects;

  v.clipPolygonI = (PaintEngineVTable::ClipPolygonI)NullPaintEngine_clipPoly;
  v.clipPolygonF = (PaintEngineVTable::ClipPolygonF)NullPaintEngine_clipPoly;
  v.clipPolygonD = (PaintEngineVTable::ClipPolygonD)NullPaintEngine_clipPoly;

  v.clipShapeF = (PaintEngineVTable::ClipShapeF)NullPaintEngine_clipShape;
  v.clipShapeD = (PaintEngineVTable::ClipShapeD)NullPaintEngine_clipShape;

  v.clipPathF = (PaintEngineVTable::ClipPathF)NullPaintEngine_clipPath;
  v.clipPathD = (PaintEngineVTable::ClipPathD)NullPaintEngine_clipPath;

  v.clipTextAtI = (PaintEngineVTable::ClipTextAtI)NullPaintEngine_clipText;
  v.clipTextAtF = (PaintEngineVTable::ClipTextAtF)NullPaintEngine_clipText;
  v.clipTextAtD = (PaintEngineVTable::ClipTextAtD)NullPaintEngine_clipText;

  v.clipTextInI = (PaintEngineVTable::ClipTextInI)NullPaintEngine_clipText;
  v.clipTextInF = (PaintEngineVTable::ClipTextInF)NullPaintEngine_clipText;
  v.clipTextInD = (PaintEngineVTable::ClipTextInD)NullPaintEngine_clipText;

  v.clipMaskAtI = (PaintEngineVTable::ClipMaskAtI)NullPaintEngine_clipMask;
  v.clipMaskAtF = (PaintEngineVTable::ClipMaskAtF)NullPaintEngine_clipMask;
  v.clipMaskAtD = (PaintEngineVTable::ClipMaskAtD)NullPaintEngine_clipMask;

  v.clipMaskInI = (PaintEngineVTable::ClipMaskInI)NullPaintEngine_clipMask;
  v.clipMaskInF = (PaintEngineVTable::ClipMaskInF)NullPaintEngine_clipMask;
  v.clipMaskInD = (PaintEngineVTable::ClipMaskInD)NullPaintEngine_clipMask;

  v.clipRegion = NullPaintEngine_clipRegion;

  v.resetClip = NullPaintEngine_resetClip;

  // --------------------------------------------------------------------------
  // [Layer]
  // --------------------------------------------------------------------------

  v.beginLayer = NullPaintEngine_beginLayer;
  v.endLayer = NullPaintEngine_endLayer;

  // --------------------------------------------------------------------------
  // [Flush]
  // --------------------------------------------------------------------------

  v.flush = NullPaintEngine_flush;
}

} // Fog namespace
