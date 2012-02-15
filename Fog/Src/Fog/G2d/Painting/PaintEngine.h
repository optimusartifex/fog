// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_PAINTENGINE_H
#define _FOG_G2D_PAINTING_PAINTENGINE_H

// [Dependencies]
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

//! @addtogroup Fog_G2d_Painting
//! @{

// ============================================================================
// [Fog::PaintEngineVTable]
// ============================================================================

struct FOG_NO_EXPORT PaintEngineVTable
{
  // --------------------------------------------------------------------------
  // [Types - AddRef / Release]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *Release)(Painter* self);

  // --------------------------------------------------------------------------
  // [Funcs - AddRef / Release]
  // --------------------------------------------------------------------------

  Release release;

  // --------------------------------------------------------------------------
  // [Types - Parameters]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *GetMetaParams)(const Painter* self, Region* region, PointI* origin);
  typedef err_t (FOG_CDECL *SetMetaParams)(Painter* self, const Region* region, const PointI* origin);
  typedef err_t (FOG_CDECL *ResetMetaParams)(Painter* self);

  typedef err_t (FOG_CDECL *GetParameter)(const Painter* self, uint32_t parameterId, void* value);
  typedef err_t (FOG_CDECL *SetParameter)(Painter* self, uint32_t parameterId, const void* value);
  typedef err_t (FOG_CDECL *ResetParameter)(Painter* self, uint32_t parameterId);

  // --------------------------------------------------------------------------
  // [Funcs - Parameters]
  // --------------------------------------------------------------------------

  GetMetaParams getMetaParams;
  SetMetaParams setMetaParams;
  ResetMetaParams resetMetaParams;

  GetParameter getParameter;
  SetParameter setParameter;
  ResetParameter resetParameter;

  // --------------------------------------------------------------------------
  // [Types - Source]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *GetSourceType)(const Painter* self, uint32_t* val);
  typedef err_t (FOG_CDECL *GetSourceColor)(const Painter* self, Color* color);
  typedef err_t (FOG_CDECL *GetSourcePattern)(const Painter* self, Pattern* pattern);

  typedef err_t (FOG_CDECL *SetSourceNone)(Painter* self);
  typedef err_t (FOG_CDECL *SetSourceArgb32)(Painter* self, uint32_t argb32);
  typedef err_t (FOG_CDECL *SetSourceArgb64)(Painter* self, const Argb64* argb64);
  typedef err_t (FOG_CDECL *SetSourceColor)(Painter* self, const Color* color);
  typedef err_t (FOG_CDECL *SetSourcePattern)(Painter* self, const Pattern* pattern);
  typedef err_t (FOG_CDECL *SetSourceAbstract)(Painter* self, uint32_t sourceId, const void* value, const void* tr);

  // --------------------------------------------------------------------------
  // [Funcs - Source]
  // --------------------------------------------------------------------------

  GetSourceType getSourceType;
  GetSourceColor getSourceColor;
  GetSourcePattern getSourcePattern;

  SetSourceNone setSourceNone;
  SetSourceArgb32 setSourceArgb32;
  SetSourceArgb64 setSourceArgb64;
  SetSourceColor setSourceColor;
  SetSourcePattern setSourcePattern;
  SetSourceAbstract setSourceAbstract;

  // --------------------------------------------------------------------------
  // [Types - Transform]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *GetTransformF)(const Painter* self, TransformF* tr);
  typedef err_t (FOG_CDECL *GetTransformD)(const Painter* self, TransformD* tr);

  typedef err_t (FOG_CDECL *SetTransformF)(Painter* self, const TransformF* tr);
  typedef err_t (FOG_CDECL *SetTransformD)(Painter* self, const TransformD* tr);

  typedef err_t (FOG_CDECL *ApplyTransform)(Painter* self, uint32_t transformOp, const void* params);
  typedef err_t (FOG_CDECL *ResetTransform)(Painter* self);

  // --------------------------------------------------------------------------
  // [Funcs - Transform]
  // --------------------------------------------------------------------------

  GetTransformF getTransformF;
  GetTransformD getTransformD;

  SetTransformF setTransformF;
  SetTransformD setTransformD;

  ApplyTransform applyTransform;
  ResetTransform resetTransform;

  // --------------------------------------------------------------------------
  // [Types - State]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *Save)(Painter* self);
  typedef err_t (FOG_CDECL *Restore)(Painter* self);

  // --------------------------------------------------------------------------
  // [Funcs - State]
  // --------------------------------------------------------------------------

  Save save;
  Restore restore;

  // --------------------------------------------------------------------------
  // [Types - Map]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *MapPointF)(const Painter* self, uint32_t mapOp, PointF* pt);
  typedef err_t (FOG_CDECL *MapPointD)(const Painter* self, uint32_t mapOp, PointD* pt);

  // --------------------------------------------------------------------------
  // [Funcs - Map]
  // --------------------------------------------------------------------------

  MapPointF mapPointF;
  MapPointD mapPointD;

  // --------------------------------------------------------------------------
  // [Types - Fill/Stroke]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *FillAll)(Painter* self);

  typedef err_t (FOG_CDECL *PaintRectI)(Painter* self, const RectI* r);
  typedef err_t (FOG_CDECL *PaintRectF)(Painter* self, const RectF* r);
  typedef err_t (FOG_CDECL *PaintRectD)(Painter* self, const RectD* r);

  typedef err_t (FOG_CDECL *PaintRectsI)(Painter* self, const RectI* r, size_t count);
  typedef err_t (FOG_CDECL *PaintRectsF)(Painter* self, const RectF* r, size_t count);
  typedef err_t (FOG_CDECL *PaintRectsD)(Painter* self, const RectD* r, size_t count);

  typedef err_t (FOG_CDECL *PaintPolylineI)(Painter* self, const PointI* p, size_t count);
  typedef err_t (FOG_CDECL *PaintPolygonI)(Painter* self, const PointI* p, size_t count);

  typedef err_t (FOG_CDECL *PaintShapeF)(Painter* self, uint32_t shapeType, const void* shapeData);
  typedef err_t (FOG_CDECL *PaintShapeD)(Painter* self, uint32_t shapeType, const void* shapeData);

  typedef err_t (FOG_CDECL *PaintTextAtI)(Painter* self, const PointI* p, const StringW* text, const Font* font, const RectI* clip);
  typedef err_t (FOG_CDECL *PaintTextAtF)(Painter* self, const PointF* p, const StringW* text, const Font* font, const RectF* clip);
  typedef err_t (FOG_CDECL *PaintTextAtD)(Painter* self, const PointD* p, const StringW* text, const Font* font, const RectD* clip);

  typedef err_t (FOG_CDECL *PaintTextInI)(Painter* self, const TextRectI* r, const StringW* text, const Font* font, const RectI* clip);
  typedef err_t (FOG_CDECL *PaintTextInF)(Painter* self, const TextRectF* r, const StringW* text, const Font* font, const RectF* clip);
  typedef err_t (FOG_CDECL *PaintTextInD)(Painter* self, const TextRectD* r, const StringW* text, const Font* font, const RectD* clip);

  typedef err_t (FOG_CDECL *PaintMaskAtI)(Painter* self, const PointI* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *PaintMaskAtF)(Painter* self, const PointF* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *PaintMaskAtD)(Painter* self, const PointD* p, const Image* mask, const RectI* mFragment);

  typedef err_t (FOG_CDECL *PaintMaskInI)(Painter* self, const RectI* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *PaintMaskInF)(Painter* self, const RectF* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *PaintMaskInD)(Painter* self, const RectD* p, const Image* mask, const RectI* mFragment);

  typedef err_t (FOG_CDECL *PaintRegion)(Painter* self, const Region* r);

  // --------------------------------------------------------------------------
  // [Funcs - Stroke]
  // --------------------------------------------------------------------------

  PaintRectI drawRectI;
  PaintRectF drawRectF;
  PaintRectD drawRectD;

  PaintPolylineI drawPolylineI;
  PaintPolygonI drawPolygonI;

  PaintShapeF drawShapeF;
  PaintShapeD drawShapeD;

  // --------------------------------------------------------------------------
  // [Funcs - Fill]
  // --------------------------------------------------------------------------

  FillAll fillAll;

  PaintRectI fillRectI;
  PaintRectF fillRectF;
  PaintRectD fillRectD;

  PaintRectsI fillRectsI;
  PaintPolygonI fillPolygonI;

  PaintShapeF fillShapeF;
  PaintShapeD fillShapeD;

  PaintTextAtI fillTextAtI;
  PaintTextAtF fillTextAtF;
  PaintTextAtD fillTextAtD;

  PaintTextInI fillTextInI;
  PaintTextInF fillTextInF;
  PaintTextInD fillTextInD;

  PaintMaskAtI fillMaskAtI;
  PaintMaskAtF fillMaskAtF;
  PaintMaskAtD fillMaskAtD;

  PaintMaskInI fillMaskInI;
  PaintMaskInF fillMaskInF;
  PaintMaskInD fillMaskInD;

  PaintRegion fillRegion;

  // --------------------------------------------------------------------------
  // [Types - Blit]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *BlitImageAtI)(Painter* self, const PointI* p, const Image* src, const RectI* sFragment);
  typedef err_t (FOG_CDECL *BlitImageAtF)(Painter* self, const PointF* p, const Image* src, const RectI* sFragment);
  typedef err_t (FOG_CDECL *BlitImageAtD)(Painter* self, const PointD* p, const Image* src, const RectI* sFragment);

  typedef err_t (FOG_CDECL *BlitImageInI)(Painter* self, const RectI* r, const Image* src, const RectI* sFragment);
  typedef err_t (FOG_CDECL *BlitImageInF)(Painter* self, const RectF* r, const Image* src, const RectI* sFragment);
  typedef err_t (FOG_CDECL *BlitImageInD)(Painter* self, const RectD* r, const Image* src, const RectI* sFragment);

  typedef err_t (FOG_CDECL *BlitMaskedImageAtI)(Painter* self, const PointI* p, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment);
  typedef err_t (FOG_CDECL *BlitMaskedImageAtF)(Painter* self, const PointF* p, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment);
  typedef err_t (FOG_CDECL *BlitMaskedImageAtD)(Painter* self, const PointD* p, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment);

  typedef err_t (FOG_CDECL *BlitMaskedImageInI)(Painter* self, const RectI* r, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment);
  typedef err_t (FOG_CDECL *BlitMaskedImageInF)(Painter* self, const RectF* r, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment);
  typedef err_t (FOG_CDECL *BlitMaskedImageInD)(Painter* self, const RectD* r, const Image* src, const Image* mask, const RectI* sFragment, const RectI* mFragment);

  // --------------------------------------------------------------------------
  // [Funcs - Blit]
  // --------------------------------------------------------------------------

  BlitImageAtI blitImageAtI;
  BlitImageAtF blitImageAtF;
  BlitImageAtD blitImageAtD;

  BlitImageInI blitImageInI;
  BlitImageInF blitImageInF;
  BlitImageInD blitImageInD;

  BlitMaskedImageAtI blitMaskedImageAtI;
  BlitMaskedImageAtF blitMaskedImageAtF;
  BlitMaskedImageAtD blitMaskedImageAtD;

  BlitMaskedImageInI blitMaskedImageInI;
  BlitMaskedImageInF blitMaskedImageInF;
  BlitMaskedImageInD blitMaskedImageInD;

  // --------------------------------------------------------------------------
  // [Types - Filter]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *FilterAll)(Painter* self, const FeBase* fe);

  typedef err_t (FOG_CDECL *FilterRectI)(Painter* self, const FeBase* fe, const RectI* r);
  typedef err_t (FOG_CDECL *FilterRectF)(Painter* self, const FeBase* fe, const RectF* r);
  typedef err_t (FOG_CDECL *FilterRectD)(Painter* self, const FeBase* fe, const RectD* r);

  typedef err_t (FOG_CDECL *FilterShapeF)(Painter* self, const FeBase* fe, uint32_t shapeType, const void* shapeData);
  typedef err_t (FOG_CDECL *FilterShapeD)(Painter* self, const FeBase* fe, uint32_t shapeType, const void* shapeData);

  typedef err_t (FOG_CDECL *FilterStrokedShapeF)(Painter* self, const FeBase* fe, uint32_t shapeType, const void* shapeData);
  typedef err_t (FOG_CDECL *FilterStrokedShapeD)(Painter* self, const FeBase* fe, uint32_t shapeType, const void* shapeData);

  // --------------------------------------------------------------------------
  // [Funcs - Filter]
  // --------------------------------------------------------------------------

  FilterAll filterAll;

  FilterRectI filterRectI;
  FilterRectF filterRectF;
  FilterRectD filterRectD;

  FilterShapeF filterShapeF;
  FilterShapeD filterShapeD;

  FilterShapeF filterStrokedShapeF;
  FilterShapeD filterStrokedShapeD;

  // --------------------------------------------------------------------------
  // [Types - Clip]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *ClipRectI)(Painter* self, uint32_t clipOp, const RectI* r);
  typedef err_t (FOG_CDECL *ClipRectF)(Painter* self, uint32_t clipOp, const RectF* r);
  typedef err_t (FOG_CDECL *ClipRectD)(Painter* self, uint32_t clipOp, const RectD* r);

  typedef err_t (FOG_CDECL *ClipRectsI)(Painter* self, uint32_t clipOp, const RectI* r, size_t count);
  typedef err_t (FOG_CDECL *ClipPolygonI)(Painter* self, uint32_t clipOp, const PointI* p, size_t count);

  typedef err_t (FOG_CDECL *ClipShapeF)(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);
  typedef err_t (FOG_CDECL *ClipShapeD)(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);

  typedef err_t (FOG_CDECL *ClipStrokedShapeF)(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);
  typedef err_t (FOG_CDECL *ClipStrokedShapeD)(Painter* self, uint32_t clipOp, uint32_t shapeType, const void* shapeData);

  typedef err_t (FOG_CDECL *ClipTextAtI)(Painter* self, uint32_t clipOp, const PointI* p, const StringW* text, const Font* font, const RectI* clip);
  typedef err_t (FOG_CDECL *ClipTextAtF)(Painter* self, uint32_t clipOp, const PointF* p, const StringW* text, const Font* font, const RectF* clip);
  typedef err_t (FOG_CDECL *ClipTextAtD)(Painter* self, uint32_t clipOp, const PointD* p, const StringW* text, const Font* font, const RectD* clip);

  typedef err_t (FOG_CDECL *ClipTextInI)(Painter* self, uint32_t clipOp, const TextRectI* r, const StringW* text, const Font* font, const RectI* clip);
  typedef err_t (FOG_CDECL *ClipTextInF)(Painter* self, uint32_t clipOp, const TextRectF* r, const StringW* text, const Font* font, const RectF* clip);
  typedef err_t (FOG_CDECL *ClipTextInD)(Painter* self, uint32_t clipOp, const TextRectD* r, const StringW* text, const Font* font, const RectD* clip);

  typedef err_t (FOG_CDECL *ClipMaskAtI)(Painter* self, uint32_t clipOp, const PointI* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *ClipMaskAtF)(Painter* self, uint32_t clipOp, const PointF* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *ClipMaskAtD)(Painter* self, uint32_t clipOp, const PointD* p, const Image* mask, const RectI* mFragment);

  typedef err_t (FOG_CDECL *ClipMaskInI)(Painter* self, uint32_t clipOp, const RectI* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *ClipMaskInF)(Painter* self, uint32_t clipOp, const RectF* p, const Image* mask, const RectI* mFragment);
  typedef err_t (FOG_CDECL *ClipMaskInD)(Painter* self, uint32_t clipOp, const RectD* p, const Image* mask, const RectI* mFragment);

  typedef err_t (FOG_CDECL *ClipRegion)(Painter* self, uint32_t clipOp, const Region* r);

  typedef err_t (FOG_CDECL *ResetClip)(Painter* self);

  // --------------------------------------------------------------------------
  // [Funcs - Clip]
  // --------------------------------------------------------------------------

  ClipRectI clipRectI;
  ClipRectF clipRectF;
  ClipRectD clipRectD;

  ClipRectsI clipRectsI;
  ClipPolygonI clipPolygonI;

  ClipShapeF clipShapeF;
  ClipShapeD clipShapeD;

  ClipShapeF clipStrokedShapeF;
  ClipShapeD clipStrokedShapeD;

  ClipTextAtI clipTextAtI;
  ClipTextAtF clipTextAtF;
  ClipTextAtD clipTextAtD;

  ClipTextInI clipTextInI;
  ClipTextInF clipTextInF;
  ClipTextInD clipTextInD;

  ClipMaskAtI clipMaskAtI;
  ClipMaskAtF clipMaskAtF;
  ClipMaskAtD clipMaskAtD;

  ClipMaskInI clipMaskInI;
  ClipMaskInF clipMaskInF;
  ClipMaskInD clipMaskInD;

  ClipRegion clipRegion;

  ResetClip resetClip;

  // --------------------------------------------------------------------------
  // [Types - Group]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *NewGroup)(Painter* self, uint32_t flags);
  typedef err_t (FOG_CDECL *EndGroup)(Painter* self);

  // --------------------------------------------------------------------------
  // [Funcs - Group]
  // --------------------------------------------------------------------------

  NewGroup newGroup;
  EndGroup endGroup;

  // --------------------------------------------------------------------------
  // [Types - Flush]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *Flush)(Painter* self, uint32_t flags);

  // --------------------------------------------------------------------------
  // [Funcs - Flush]
  // --------------------------------------------------------------------------

  Flush flush;
};

// ============================================================================
// [Fog::PaintEngine]
// ============================================================================

//! @brief Painter engine is an abstract @c Painter backend.
struct FOG_NO_EXPORT PaintEngine
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief The pointer to the painter-engine virtual table.
  const PaintEngineVTable* vtable;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_PAINTING_PAINTENGINE_H
