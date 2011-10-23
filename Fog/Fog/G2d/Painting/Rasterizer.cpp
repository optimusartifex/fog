// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Face/FaceC.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Painting/RasterApi_p.h>
#include <Fog/G2d/Painting/RasterScanline_p.h>
#include <Fog/G2d/Painting/RasterSpan_p.h>
#include <Fog/G2d/Painting/RasterStructs_p.h>
#include <Fog/G2d/Painting/Rasterizer_p.h>
#include <Fog/G2d/Tools/Region.h>
#include <Fog/G2d/Tools/RegionUtil_p.h>

namespace Fog {

// ============================================================================
/*
struct CellLogger
{
  FILE* f;
  CellLogger()
  {
    f = fopen("Cell.txt", "w+");
  }

  static CellLogger instance;

  static void out(const char* fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(instance.f, fmt, ap);
    va_end(ap);
  }

  static void log(int x, int y, int cover, int area, const char* where_)
  {
    fprintf(instance.f, "%s [%3d %3d] ... [Cover=%d, Area=%d]\n", where_, x, y, cover, area);
  }

  static void logChunk(int y, PathRasterizer8::Chunk* chunk)
  {
    if (chunk == NULL)
    {
      fprintf(instance.f, "%d - Null\n", y);
    }
    else
    {
      PathRasterizer8::Chunk* first = chunk;
      fprintf(instance.f, "%d - ", y);
      int sum = 0;

      do {
        fprintf(instance.f, "%d->%d [", chunk->x0, chunk->x1);

        for (size_t i = 0; i < chunk->getLength(); i++)
        {
          fprintf(instance.f, "%d|%d ", chunk->cells[i].cover, chunk->cells[i].area);
          sum += chunk->cells[i].cover;
        }
        fprintf(instance.f, "] ");

        chunk = chunk->next;
      } while (chunk != first);

      fprintf(instance.f, "COVER_SUM=%d", sum);
      fprintf(instance.f, "\n");
      fflush(instance.f);
      FOG_ASSERT(sum == 0);
    }
  }
};
CellLogger CellLogger::instance;
*/
// ============================================================================

// ============================================================================
// [Helpers]
// ============================================================================

#define PATHRASTERIZER8_ALLOCATOR_SIZE (16384 - 84)
#define PATHRASTERIZER8_MAX_CHUNK_SIZE (PATHRASTERIZER8_ALLOCATOR_SIZE / (sizeof(PathRasterizer8::Cell) * 8))

enum A8_ENUM
{
  A8_SHIFT   = 8,             // 8
  A8_SHIFT_2 = A8_SHIFT + 1,  // 9

  A8_SCALE   = 1 << A8_SHIFT, // 256
  A8_SCALE_2 = A8_SCALE * 2,  // 512

  A8_MASK    = A8_SCALE - 1,  // 255
  A8_MASK_2  = A8_SCALE_2 - 1 // 511
};

static FOG_INLINE int upscale24x8(float  v) { return Math::ifloor(v * (float )256 + 0.5f); }
static FOG_INLINE int upscale24x8(double v) { return Math::ifloor(v * (double)256 + 0.5 ); }

// ============================================================================
// [Fog::RasterizerApi]
// ============================================================================

FOG_NO_EXPORT RasterizerApi Rasterizer_api;

// ============================================================================
// [Fog::BoxRasterizer8 - Init - 32x0]
// ============================================================================

static void FOG_CDECL BoxRasterizer8_init32x0(BoxRasterizer8* self, const BoxI* box)
{
  // The box should be already clipped to the scene-box.
  FOG_ASSERT(self->_sceneBox.subsumes(*box));
  FOG_ASSERT(self->_clipType < RASTER_CLIP_COUNT);

  self->_initialized = true;
  self->_boxBounds = *box;
  self->_render = Rasterizer_api.box8.render_32x0[self->_clipType];
}

// ============================================================================
// [Fog::BoxRasterizer8 - Init - 24x8]
// ============================================================================

static void FOG_CDECL BoxRasterizer8_init24x8(BoxRasterizer8* self, const BoxI* box24x8)
{
  // The box should be already clipped to the scene-box.
  FOG_ASSERT(self->_sceneBox24x8.subsumes(*box24x8));
  FOG_ASSERT(self->_clipType < RASTER_CLIP_COUNT);

  // Extract box coordinates.
  int x0 = box24x8->x0;
  int y0 = box24x8->y0;
  int x1 = box24x8->x1;
  int y1 = box24x8->y1;

  self->_initialized = (x0 < x1) & (y0 < y1);
  if (!self->_initialized) return;

  self->_boxBounds.x0 = (x0 >> 8);
  self->_boxBounds.y0 = (y0 >> 8);
  self->_boxBounds.x1 = (x1 >> 8);
  self->_boxBounds.y1 = (y1 >> 8);

  self->_box24x8.x0 = x0;
  self->_box24x8.y0 = y0;
  self->_box24x8.x1 = x1;
  self->_box24x8.y1 = y1;

  uint32_t fx0 = x0 & 0xFF;
  uint32_t fy0 = y0 & 0xFF;
  uint32_t fx1 = x1 & 0xFF;
  uint32_t fy1 = y1 & 0xFF;

  self->_boxBounds.x1 += fx1 != 0;
  self->_boxBounds.y1 += fy1 != 0;

  uint32_t hl = 256 - fx0;
  uint32_t hr = fx1;
  uint32_t vt = 256 - fy0;
  uint32_t vb = fy1;

  uint32_t opacity = self->_opacity;

  if ((x0 & ~0xFF) == (x1 & ~0xFF))
  {
    hr -= hl;
    hl = hr;
  }

  if ((y0 & ~0xFF) == (y1 & ~0xFF))
  {
    vb -= vt;
    vt = vb;
  }

  vt *= opacity;
  vb *= opacity;

  self->_ct[0] = (uint16_t)( (hl * vt) >> 16 );
  self->_ct[1] = (uint16_t)( (vt     ) >>  8 );
  self->_ct[2] = (uint16_t)( (hr * vt) >> 16 );

  self->_ci[0] = (uint16_t)( (hl * opacity) >> 8 );
  self->_ci[1] = (uint16_t)( opacity             );
  self->_ci[2] = (uint16_t)( (hr * opacity) >> 8 );

  self->_cb[0] = (uint16_t)( (hl * vb) >> 16 );
  self->_cb[1] = (uint16_t)( (vb     ) >>  8 );
  self->_cb[2] = (uint16_t)( (hr * vb) >> 16 );

  self->_render = Rasterizer_api.box8.render_24x8[self->_clipType];
}

// ============================================================================
// [Fog::BoxRasterizer8 - Render - Aligned]
// ============================================================================

static void FOG_CDECL BoxRasterizer8_render_32x0_st_clip_box(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* _scanline)
{
  BoxRasterizer8* self = reinterpret_cast<BoxRasterizer8*>(_self);
  FOG_UNUSED(_scanline);

  const BoxI& box = self->_boxBounds;
  int y0 = box.y0;
  int y1 = box.y1;

  int i;

  // Prepare.
  filler->prepare(y0);
  RasterFiller::ProcessFunc process = filler->_process;

  RasterSpanExt8 span[1];
  span[0].setPositionAndType(box.x0, box.x1, RASTER_SPAN_C);
  span[0].setConstMask(self->_opacity);
  span[0].setNext(NULL);

  // Process.
  for (i = y1 - y0; i; i--)
    process(filler, span);
}

static void FOG_CDECL BoxRasterizer8_render_32x0_st_clip_region(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* scanline)
{
  BoxRasterizer8* self = reinterpret_cast<BoxRasterizer8*>(_self);

  // TODO: Rasterizer.
#if 0
  y -= self->_boundingBox.y0;

  FOG_ASSERT(self->_isFinalized);
  FOG_ASSERT((uint)y < (uint)self->_boundingBox.y1);

  RasterScanline8* scanline = self->_outputScanline;

  int x0 = self->xLeft;
  int x1 = self->xRight;

  // Clipping.
  const BoxI* clipCur = clipBoxes;
  const BoxI* clipEnd = clipBoxes + count;
  if (FOG_UNLIKELY(clipCur == clipEnd)) return NULL;

  // Current clip box start / end point (not part of clip span).
  int clipX0;
  int clipX1 = clipCur->x1;

  // Advance clip (discard clip-boxes that can't intersect).
  while (clipX1 <= x0)
  {
    if (++clipCur == clipEnd) return NULL;
    clipX1 = clipCur->x1;
  }

  clipX0 = Math::max<int>(clipCur->x0, x0);
  if (clipX0 > x1) return NULL;

  if (scanline->begin(self->_boundingBox.x0, self->_boundingBox.x1) != ERR_OK)
    return NULL;

  const uint32_t* covers = self->_ci;
  if (FOG_UNLIKELY(y == 0))
    covers = self->_ct;
  else if (FOG_UNLIKELY(y == (shape->bounds.y1 - shape->bounds.y0)))
    covers = self->_cb;

  if (clipX0 == x0)
  {
    clipX0++;
    scanline->newA8Extra_buf(x0, clipX0)[0] = (uint16_t)covers[0];
  }

  uint16_t midcover = covers[1];
  for (;;)
  {
    if (clipX0 < x1)
    {
      scanline->lnkConstSpanOrMerge(clipX0, Math::min<int>(clipX1, x1), midcover);
    }
    if (clipX1 > x1)
    {
      scanline->lnkConstSpanOrMerge(x1, x1 + 1, covers[2]);
      break;
    }

    if (++clipBoxes == clipEnd) break;
    clipX0 = clipBoxes->x0;
    if (clipX0 > x1) break;
    clipX1 = clipBoxes->x1;
  }

  if (scanline->close() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  Rasterizer_dumpSpans(y0, scanline->getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline->getSpans();

#endif
}

static void FOG_CDECL BoxRasterizer8_render_32x0_st_clip_mask(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* scanline)
{
  BoxRasterizer8* self = reinterpret_cast<BoxRasterizer8*>(_self);

  // TODO: Rasterizer.
}

// ============================================================================
// [Fog::BoxRasterizer8 - Render - 24x8]
// ============================================================================

static void FOG_CDECL BoxRasterizer8_render_24x8_st_clip_box(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* _scanline)
{
  BoxRasterizer8* self = reinterpret_cast<BoxRasterizer8*>(_self);
  FOG_UNUSED(_scanline);

  const BoxI& box = self->_boxBounds;
  int y0 = box.y0;
  int y1 = box.y1 - 1;

  uint w = box.getWidth();
  uint i;

  // Prepare.
  filler->prepare(y0);
  RasterFiller::ProcessFunc process = filler->_process;

  RasterSpanExt8 span[3];
  uint16_t mask[10];

  // Process.
  if (w < 10)
  {
#define SETUP_MASK(_Coverage_) \
    FOG_MACRO_BEGIN \
      mask[0] = _Coverage_[0]; \
      for (i = 1; i < w-1; i++) mask[i] = _Coverage_[1]; \
      mask[i] = _Coverage_[2]; \
    FOG_MACRO_END

    span[0].setPositionAndType(box.x0, box.x1, RASTER_SPAN_AX_EXTRA);
    span[0].setA8Extra(reinterpret_cast<uint8_t*>(mask));
    span[0].setNext(NULL);

    // Render top part (or top+bottom part in case that the box is 1 pixel height).
    SETUP_MASK(self->_ct);
    process(filler, span);
    if (y0 >= y1) return;

    // Render inner part.
    SETUP_MASK(self->_ci);
    while (++y0 < y1)
      process(filler, span);

    // Render bottom part.
    SETUP_MASK(self->_cb);
    process(filler, span);
#undef SETUP_MASK
  }
  else
  {
#define SETUP_MASK(_Coverage_) \
    FOG_MACRO_BEGIN \
      span[0].setConstMask(_Coverage_[0]); \
      span[1].setConstMask(_Coverage_[1]); \
      span[2].setConstMask(_Coverage_[2]); \
    FOG_MACRO_END

    span[0].setPositionAndType(box.x0    , box.x0 + 1, RASTER_SPAN_C);
    span[0].setNext(&span[1]);

    span[1].setPositionAndType(box.x0 + 1, box.x1 - 1, RASTER_SPAN_C);
    span[1].setNext(&span[2]);

    span[2].setPositionAndType(box.x1 - 1, box.x1    , RASTER_SPAN_C);
    span[2].setNext(NULL);

    // Render top part (or top+bottom part in case that the box is 1 pixel height).
    SETUP_MASK(self->_ct);
    process(filler, span);
    if (y0 >= y1) return;

    // Render inner part.
    SETUP_MASK(self->_ci);
    while (++y0 < y1)
      process(filler, span);

    // Render bottom part.
    SETUP_MASK(self->_cb);
    process(filler, span);
#undef SETUP_MASK
  }
}

static void FOG_CDECL BoxRasterizer8_render_24x8_st_clip_region(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* scanline)
{
  BoxRasterizer8* self = reinterpret_cast<BoxRasterizer8*>(_self);

  // TODO: Rasterizer.
#if 0
  y -= self->_boundingBox.y0;

  FOG_ASSERT(self->_isFinalized);
  FOG_ASSERT((uint)y < (uint)self->_boundingBox.y1);

  int x0 = self->xLeft;
  int x1 = self->xRight;

  // Clipping.
  const BoxI* clipCur = clipBoxes;
  const BoxI* clipEnd = clipBoxes + count;
  if (FOG_UNLIKELY(clipCur == clipEnd)) return NULL;

  // Current clip box start / end point (not part of clip span).
  int clipX0;
  int clipX1 = clipCur->x1;

  // Advance clip (discard clip-boxes that can't intersect).
  while (clipX1 <= x0)
  {
    if (++clipCur == clipEnd) return NULL;
    clipX1 = clipCur->x1;
  }

  clipX0 = Math::max<int>(clipCur->x0, x0);
  if (clipX0 > x1) return NULL;

  if (scanline->begin(self->_boundingBox.x0, self->_boundingBox.x1) != ERR_OK)
    return NULL;

  const uint32_t* covers = self->_ci;
  if (FOG_UNLIKELY(y == 0))
    covers = self->_ct;
  else if (FOG_UNLIKELY(y == (shape->bounds.y1 - shape->bounds.y0)))
    covers = self->_cb;

  if (clipX0 == x0)
  {
    clipX0++;
    scanline->newA8Extra_buf(x0, clipX0)[0] = (uint16_t)covers[0];
  }

  uint16_t midcover = covers[1];
  for (;;)
  {
    if (clipX0 < x1)
    {
      scanline->lnkConstSpanOrMerge(clipX0, Math::min<int>(clipX1, x1), midcover);
    }
    if (clipX1 > x1)
    {
      scanline->lnkConstSpanOrMerge(x1, x1 + 1, covers[2]);
      break;
    }

    if (++clipBoxes == clipEnd) break;
    clipX0 = clipBoxes->x0;
    if (clipX0 > x1) break;
    clipX1 = clipBoxes->x1;
  }

  if (scanline->close() != ERR_OK) return NULL;
#if defined(FOG_DEBUG_RASTERIZER)
  Rasterizer_dumpSpans(y0, scanline->getSpans());
#endif // FOG_DEBUG_RASTERIZER
  return scanline->getSpans();

#endif
}

static void FOG_CDECL BoxRasterizer8_render_24x8_st_clip_mask(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* scanline)
{
  BoxRasterizer8* self = reinterpret_cast<BoxRasterizer8*>(_self);

  // TODO: Rasterizer.
}

// ============================================================================
// [Fog::PathRasterizer8 - Debug]
// ============================================================================

#if defined(FOG_DEBUG)
# define VERIFY_CHUNKS_8(_First_) \
  FOG_MACRO_BEGIN \
    PathRasterizer8::Chunk* _cVerifyFirst = (_First_); \
    PathRasterizer8::Chunk* _cVerifyPtr = _cVerifyFirst; \
    \
    int _cVerifySum = 0; \
    \
    FOG_ASSERT_X(_cVerifyPtr->x0 < _cVerifyFirst->prev->x1, \
      "Fog::PathRasterizer8::VERIFY_CHUNKS() - Invalid first chunk."); \
    \
    do { \
      PathRasterizer8::Chunk* _cVerifyNext = _cVerifyPtr->next; \
      \
      FOG_ASSERT(_cVerifyPtr->x0 <  _cVerifyPtr->x1); \
      FOG_ASSERT(_cVerifyPtr->x1 <= _cVerifyNext->x0 || _cVerifyNext == _cVerifyFirst); \
      \
      int _cVerifyLength = _cVerifyPtr->x1 - _cVerifyPtr->x0; \
      for (int _cVerifyI = 0; _cVerifyI < _cVerifyLength; _cVerifyI++) \
      { \
        _cVerifySum += _cVerifyPtr->cells[_cVerifyI].cover; \
      } \
      _cVerifyPtr = _cVerifyNext; \
    } while (_cVerifyPtr != _cVerifyFirst); \
    \
    FOG_ASSERT_X(_cVerifySum == 0, \
      "Fog::PathRasterizer8::VERIFY_CHUNKS() - Invalid sum of cells, must be zero."); \
    \
  FOG_MACRO_END
#else
# define VERIFY_CHUNKS_8(_First_) \
  FOG_NOP
#endif // FOG_DEBUG

// ============================================================================
// [Fog::PathRasterizer8 - Construction / Destruction]
// ============================================================================

PathRasterizer8::PathRasterizer8() :
  _allocator(PATHRASTERIZER8_ALLOCATOR_SIZE)
{
  // Default is no multithreading.
  _scope.reset();

  // Clear rows.
  _rowsCapacity = 0;
  _rows = NULL;

  reset();
}

PathRasterizer8::~PathRasterizer8()
{
  if (_rows != NULL)
    MemMgr::free(_rows);
}

// ============================================================================
// [Fog::PathRasterizer8 - Helpers]
// ============================================================================

// Forward Declarations.
static bool PathRasterizer8_renderLine(PathRasterizer8* self, Fixed24x8 x0, Fixed24x8 y0, Fixed24x8 x1, Fixed24x8 y1);

// ============================================================================
// [Fog::PathRasterizer8 - Reset]
// ============================================================================

void PathRasterizer8::reset()
{
  // Reset scene-box.
  _sceneBox.reset();
  _boundingBox.setBox(-1, -1, -1, -1);

  // Reset error.
  _error = ERR_OK;
  // Reset opacity.
  _opacity = 0x100;

  // Reset fill-rule.
  _fillRule = FILL_RULE_DEFAULT;
  // Not valid neither finalized.
  _isValid = false;
  _isFinalized = false;
}

// ============================================================================
// [Fog::PathRasterizer8 - Init]
// ============================================================================

err_t PathRasterizer8::init()
{
  _allocator.clear();
  _boundingBox.setBox(-1, -1, -1, -1);

  _error = ERR_OK;
  _isValid = false;
  _isFinalized = false;

  _size.set(_sceneBox.getWidth(), _sceneBox.getHeight());
  _size24x8.set(_size.w << A8_SHIFT, _size.h << A8_SHIFT);

  _offsetF.set(-_sceneBox.x0, -_sceneBox.y0);
  _offsetD.set(-_sceneBox.x0, -_sceneBox.y0);
  uint i = (uint)_sceneBox.getHeight() + 1;

  if (_rowsCapacity < i)
  {
    if (_rows != NULL)
      MemMgr::free(_rows);

    // Align...
    _rowsCapacity = (i + 255U) & ~255U;
    _rows = reinterpret_cast<Row*>(MemMgr::alloc(_rowsCapacity * sizeof(Row)));

    if (_rows == NULL)
    {
      _rowsCapacity = 0;
      setError(ERR_RT_OUT_OF_MEMORY);
      goto _End;
    }
  }

_End:
  return _error;
}

// ============================================================================
// [Fog::PathRasterizer8 - AddPath]
// ============================================================================

template<typename SrcT>
static void PathRasterizer8_addPathData(PathRasterizer8* self,
  const SrcT_(Point)* srcPts, const uint8_t* srcCmd, size_t count, const SrcT_(Point)& offset)
{
  if (count == 0)
    return;

  const uint8_t* srcEnd = srcCmd + count;

  // Current/Start moveTo x position.
  Fixed24x8 x0, startX0;
  // Current/Start moveTo y position.
  Fixed24x8 y0, startY0;

  // Current command.
  uint8_t c;

  // Stack.
  PointI curveStack[32 * 3 + 1];
  int levelStack[32];

_Start:
  // Find: 'move-to' command and then go to the second loop.
  for (;;)
  {
    c = srcCmd[0];
    srcPts++;
    srcCmd++;

    // This can happen only when 'move-to' is the last command. In this case
    // there is nothing to rasterize.
    if (srcCmd == srcEnd)
      return;

    // ------------------------------------------------------------------------
    // [MoveTo]
    // ------------------------------------------------------------------------

    if (PathCmd::isMoveTo(c))
    {
_MoveTo:
      x0 = startX0 = Math::bound<Fixed24x8>(upscale24x8(srcPts[-1].x + offset.x), 0, self->_size24x8.w);
      y0 = startY0 = Math::bound<Fixed24x8>(upscale24x8(srcPts[-1].y + offset.y), 0, self->_size24x8.h);
      break;
    }
  }

  // Process: 'line-to', 'quad-to' and 'cubic-to'.
  // Stop at: 'move-to', 'close'.
  for (;;)
  {
    c = srcCmd[0];

    // ------------------------------------------------------------------------
    // [LineTo]
    // ------------------------------------------------------------------------

    if (PathCmd::isLineTo(c))
    {
      Fixed24x8 x1 = Math::bound<Fixed24x8>(upscale24x8(srcPts[0].x + offset.x), 0, self->_size24x8.w);
      Fixed24x8 y1 = Math::bound<Fixed24x8>(upscale24x8(srcPts[0].y + offset.y), 0, self->_size24x8.h);

      if ((x0 != x1) | (y0 != y1) && !PathRasterizer8_renderLine(self, x0, y0, x1, y1))
        return;

      x0 = x1;
      y0 = y1;

      srcPts++;
      srcCmd++;
      if (srcCmd == srcEnd) goto _ClosePath;
    }

    // ------------------------------------------------------------------------
    // [QuadTo]
    // ------------------------------------------------------------------------

    else if (PathCmd::isQuadTo(c))
    {
      PointI* curve = curveStack;

      curve[0].x = upscale24x8(srcPts[ 1].x + offset.x);
      curve[0].y = upscale24x8(srcPts[ 1].y + offset.y);
      curve[1].x = upscale24x8(srcPts[ 0].x + offset.x);
      curve[1].y = upscale24x8(srcPts[ 0].y + offset.y);
      curve[2].x = x0;
      curve[2].y = y0;

      int d = Math::max(Math::abs( curve[2].x + curve[0].x - 2 * curve[1].x),
                        Math::abs( curve[2].y + curve[0].y - 2 * curve[1].y));

      int level = 0;
      int top = 0;

      while (d > (A8_SCALE / 6))
      {
        d >>= 2;
        level++;
      }

      levelStack[0] = level;
      top = 0;

      do {
        level = levelStack[top];
        if (level > 1)
        {
          int a, b;

          curve[4].x = curve[2].x;
          b = curve[1].x;
          a = curve[3].x = (curve[2].x + b) / 2;
          b = curve[1].x = (curve[0].x + b) / 2;
          curve[2].x = (a + b) / 2;

          curve[4].y = curve[2].y;
          b = curve[1].y;
          a = curve[3].y = (curve[2].y + b) / 2;
          b = curve[1].y = (curve[0].y + b) / 2;
          curve[2].y = (a + b) / 2;

          curve += 2;
          top++;
          levelStack[top] = levelStack[top - 1] = level - 1;
          continue;
        }

        // LineTo.
        Fixed24x8 x1 = Math::bound<Fixed24x8>(curve[0].x, 0, self->_size24x8.w);
        Fixed24x8 y1 = Math::bound<Fixed24x8>(curve[0].y, 0, self->_size24x8.h);

        if (!PathRasterizer8_renderLine(self, x0, y0, x1, y1))
          return;

        x0 = x1;
        y0 = y1;

        top--;
        curve -= 2;
      } while (top >= 0);

      srcPts += 2;
      srcCmd += 2;
      if (srcCmd == srcEnd) goto _ClosePath;
    }

    // ------------------------------------------------------------------------
    // [CubicTo]
    // ------------------------------------------------------------------------

    else if (PathCmd::isCubicTo(c))
    {
      PointI* curve = curveStack;
      PointI* curveEnd = curve + 31 * 3;

      curve[0].x = upscale24x8(srcPts[ 2].x + offset.x);
      curve[0].y = upscale24x8(srcPts[ 2].y + offset.y);
      curve[1].x = upscale24x8(srcPts[ 1].x + offset.x);
      curve[1].y = upscale24x8(srcPts[ 1].y + offset.y);
      curve[2].x = upscale24x8(srcPts[ 0].x + offset.x);
      curve[2].y = upscale24x8(srcPts[ 0].y + offset.y);
      curve[3].x = x0;
      curve[3].y = y0;

      for (;;)
      {
        // Decide whether to split or draw. See `Rapid Termination. Evaluation
        // for Recursive Subdivision of Bezier Curves' by Thomas F. Hain, at:
        // http://www.cis.southalabama.edu/~hain/general/Publications/Bezier/Camera-ready%20CISST02%202.pdf
        if (curve != curveEnd)
        {
          int dx, dy;
          int dx_, dy_;
          int dx1, dy1;
          int dx2, dy2;
          int L, s, limit;

          // dx and dy are x and y components of the P0-P3 chord vector.
          dx = curve[3].x - curve[0].x;
          dy = curve[3].y - curve[0].y;

          // L is an (under)estimate of the Euclidean distance P0-P3.
          //
          // If dx >= dy, then r = sqrt(dx^2 + dy^2) can be overestimated
          // with least maximum error by
          //
          //   r_upperbound = dx + (sqrt(2) - 1) * dy,
          //
          // where sqrt(2) - 1 can be (over)estimated by 107/256, giving an
          // error of no more than 8.4%.
          //
          // Similarly, some elementary calculus shows that r can be
          // underestimated with least maximum error by
          //
          //   r_lowerbound = sqrt(2 + sqrt(2)) / 2 * dx
          //                + sqrt(2 - sqrt(2)) / 2 * dy .
          //
          // 236/256 and 97/256 are (under)estimates of the two algebraic
          // numbers, giving an error of no more than 8.1%.
          dx_ = Math::abs(dx);
          dy_ = Math::abs(dy);

          // This is the same as:
          //
          //   L = (236 * max( dx_, dy_ ) + 97 * min( dx_, dy_ )) >> 8;
          L = ((dx_ > dy_) ? (236 * dx_ +  97 * dy_)
                           : ( 97 * dx_ + 236 * dy_)) >> 8;

          // Avoid possible arithmetic overflow below by splitting.
          if (L > 32767)
            goto _CBezierSplit;

          // Max deviation may be as much as (s/L) * 3/4 (if Hain's v = 1).
          limit = L * (int)(A8_SCALE / 6);

          // s is L * the perpendicular distance from P1 to the line P0-P3.
          dx1 = curve[1].x - curve[0].x;
          dy1 = curve[1].y - curve[0].y;
          s = Math::abs(dy * dx1 - dx * dy1);

          if (s > limit)
            goto _CBezierSplit;

          // s is L * the perpendicular distance from P2 to the line P0-P3.
          dx2 = curve[2].x - curve[0].x;
          dy2 = curve[2].y - curve[0].y;
          s = Math::abs(dy * dx2 - dx * dy2);

          if (s > limit)
            goto _CBezierSplit;

          // If P1 or P2 is outside P0-P3, split the curve.
          if ((dy * dy1 + dx * dx1 < 0) ||
              (dy * dy2 + dx * dx2 < 0) ||
              (dy * (curve[3].y - curve[1].y) + dx * (curve[3].x - curve[1].x) < 0) ||
              (dy * (curve[3].y - curve[2].y) + dx * (curve[3].x - curve[2].x) < 0))
            goto _CBezierSplit;
        }

        {
          Fixed24x8 x1 = Math::bound<Fixed24x8>(curve[0].x, 0, self->_size24x8.w);
          Fixed24x8 y1 = Math::bound<Fixed24x8>(curve[0].y, 0, self->_size24x8.h);

          if (!PathRasterizer8_renderLine(self, x0, y0, x1, y1))
            return;

          x0 = x1;
          y0 = y1;

          if (curve == curveStack)
            break;

          curve -= 3;
          continue;
        }

_CBezierSplit:
        {
          int a, b, c, d;

          curve[6].x = curve[3].x;
          c = curve[1].x;
          d = curve[2].x;
          curve[1].x = a = ( curve[0].x + c ) / 2;
          curve[5].x = b = ( curve[3].x + d ) / 2;
          c = ( c + d ) / 2;
          curve[2].x = a = ( a + c ) / 2;
          curve[4].x = b = ( b + c ) / 2;
          curve[3].x = ( a + b ) / 2;

          curve[6].y = curve[3].y;
          c = curve[1].y;
          d = curve[2].y;
          curve[1].y = a = ( curve[0].y + c ) / 2;
          curve[5].y = b = ( curve[3].y + d ) / 2;
          c = ( c + d ) / 2;
          curve[2].y = a = ( a + c ) / 2;
          curve[4].y = b = ( b + c ) / 2;
          curve[3].y = ( a + b ) / 2;
        }

        curve += 3;
        continue;
      }

      srcPts += 3;
      srcCmd += 3;
      if (srcCmd == srcEnd) goto _ClosePath;
    }

    // ------------------------------------------------------------------------
    // [MoveTo / Close]
    // ------------------------------------------------------------------------

    else
    {
      FOG_ASSERT(PathCmd::isMoveTo(c) || PathCmd::isClose(c));

_ClosePath:
      // Close the current polygon.
      if ((x0 != startX0) | (y0 != startY0) && !PathRasterizer8_renderLine(self, x0, y0, startX0, startY0))
        return;

      if (srcCmd == srcEnd) return;
      srcPts++;
      srcCmd++;
      if (srcCmd == srcEnd) return;

      if (PathCmd::isMoveTo(c))
        goto _MoveTo;
      else
        goto _Start;
    }
  }
}

void PathRasterizer8::addPath(const PathF& path)
{
  FOG_ASSERT(_isFinalized == false);
  if (_error != ERR_OK) return;

  PathRasterizer8_addPathData<float>(this, path.getVertices(), path.getCommands(), path.getLength(), _offsetF);
}

void PathRasterizer8::addPath(const PathF& path, const PointF& _offset)
{
  FOG_ASSERT(_isFinalized == false);
  if (_error != ERR_OK) return;

  PointF offset = _offset + _offsetF;
  PathRasterizer8_addPathData<float>(this, path.getVertices(), path.getCommands(), path.getLength(), offset);
}

void PathRasterizer8::addPath(const PathD& path)
{
  FOG_ASSERT(_isFinalized == false);
  if (_error != ERR_OK) return;

  PathRasterizer8_addPathData<double>(this, path.getVertices(), path.getCommands(), path.getLength(), _offsetD);
}

void PathRasterizer8::addPath(const PathD& path, const PointD& _offset)
{
  FOG_ASSERT(_isFinalized == false);
  if (_error != ERR_OK) return;

  PointD offset = _offset + _offsetD;
  PathRasterizer8_addPathData<double>(this, path.getVertices(), path.getCommands(), path.getLength(), offset);
}

// ============================================================================
// [Fog::PathRasterizer8 - AddBox]
// ============================================================================

static const uint8_t Rasterizer_boxCommands[8] =
{
  PATH_CMD_MOVE_TO,
  PATH_CMD_LINE_TO,
  PATH_CMD_LINE_TO,
  PATH_CMD_LINE_TO,
  PATH_CMD_CLOSE,
  0xFF,
  0xFF,
  0xFF
};

void PathRasterizer8::addBox(const BoxF& box)
{
  if (_error != ERR_OK) return;
  FOG_ASSERT(_isFinalized == false);

  PointF pts[5];
  pts[0].set(box.x0, box.y0);
  pts[1].set(box.x1, box.y0);
  pts[2].set(box.x1, box.y1);
  pts[3].set(box.x0, box.y1);
  pts[4].set(0.0f, 0.0f);

  PathRasterizer8_addPathData<float>(this, pts, Rasterizer_boxCommands, 5, _offsetF);
}

void PathRasterizer8::addBox(const BoxD& box)
{
  if (_error != ERR_OK) return;
  FOG_ASSERT(_isFinalized == false);

  PointD pts[5];
  pts[0].set(box.x0, box.y0);
  pts[1].set(box.x1, box.y0);
  pts[2].set(box.x1, box.y1);
  pts[3].set(box.x0, box.y1);
  pts[4].set(0.0, 0.0);

  PathRasterizer8_addPathData<double>(this, pts, Rasterizer_boxCommands, 5, _offsetD);
}

// ============================================================================
// [Fog::PathRasterizer8 - Render]
// ============================================================================

#define NEW_CHUNK(_Chunk_, _X_, _Length_) \
  FOG_MACRO_BEGIN \
    _Chunk_ = reinterpret_cast<PathRasterizer8::Chunk*>(self->_allocator.alloc(PathRasterizer8::Chunk::getSizeOf(_Length_))); \
    if (FOG_IS_NULL(_Chunk_)) goto _Bail; \
    \
    _Chunk_->x0 = (_X_); \
    _Chunk_->x1 = (_X_) + _Length_; \
  FOG_MACRO_END

#define ADD_SINGLE(_Name_, _X_, _Cover_, _Area_) \
  FOG_MACRO_BEGIN \
    PathRasterizer8::Chunk* _chunk = rPtr->first; \
    \
    if (_chunk == NULL) \
    { \
      NEW_CHUNK(_chunk, _X_, 1); \
      \
      _chunk->prev = _chunk; \
      _chunk->next = _chunk; \
      /*CellLogger::log(_X_, (int)(rPtr - self->_rows), _Cover_, _Area_, "H=One"); */\
      _chunk->cells[0].set(_Cover_, _Area_); \
      \
      rPtr->first = _chunk; \
    } \
    else if (_X_ < _chunk->x0) \
    { \
      PathRasterizer8::Chunk* _last = _chunk->prev; \
      PathRasterizer8::Chunk* _cNew; \
      \
      NEW_CHUNK(_cNew, _X_, 1); \
      \
      _cNew->prev = _last; \
      _cNew->next = _chunk; \
      /*CellLogger::log(_X_, (int)(rPtr - self->_rows), _Cover_, _Area_, "H=One"); */\
      _cNew->cells[0].set(_Cover_, _Area_); \
      \
      _last->next = _cNew; \
      _chunk->prev = _cNew; \
      \
      rPtr->first = _cNew; \
      FOG_ASSERT(_cNew->x1 <= _chunk->x0); \
    } \
    else \
    { \
      PathRasterizer8::Chunk* _last = _chunk->prev; \
      \
      if (_X_ < _last->x1) \
      { \
        if ((_X_ - _chunk->x0) <= (_last->x1 - _X_)) \
        { \
          for (;;) \
          { \
            if (_X_ < _chunk->x0) \
              break; \
            \
            if (_X_ < _chunk->x1) \
              goto _Name_##_Merge; \
            \
            FOG_ASSERT(_chunk != _last); \
            _chunk = _chunk->next; \
          } \
        } \
        else \
        { \
          _chunk = _last; \
          for (;;) \
          { \
            if (_X_ >= _chunk->x0) \
            { \
_Name_##_Merge: \
              /*CellLogger::log(_X_, (int)(rPtr - self->_rows), _Cover_, _Area_, "H+One"); */\
              _chunk->cells[_X_ - _chunk->x0].add(_Cover_, _Area_); \
              goto _Name_##_End; \
            } \
            \
            _chunk = _chunk->prev; \
            FOG_ASSERT(_chunk != _last); \
            \
            if (_X_ >= _chunk->x1) \
              break; \
          } \
          _chunk = _chunk->next; \
        } \
        FOG_ASSERT(_X_ + 1 <= _chunk->x0); \
      } \
      \
      PathRasterizer8::Chunk* _prev = _chunk->prev; \
      PathRasterizer8::Chunk* _cNew; \
      \
      NEW_CHUNK(_cNew, _X_, 1); \
      \
      _cNew->prev = _prev; \
      _cNew->next = _chunk; \
      /*CellLogger::log(_X_, (int)(rPtr - self->_rows), _Cover_, _Area_, "H=One"); */\
      _cNew->cells[0].set(_Cover_, _Area_); \
      \
      _prev->next = _cNew; \
      _chunk->prev = _cNew; \
    } \
_Name_##_End: \
    ; \
  FOG_MACRO_END \

static bool PathRasterizer8_renderLine(PathRasterizer8* self, Fixed24x8 x0, Fixed24x8 y0, Fixed24x8 x1, Fixed24x8 y1)
{
  int dx = x1 - x0;
  int dy = y1 - y0;

  //CellLogger::out("RenderLine [x0=%d y0=%d] -> [x1=%d y1=%d] | (dx=%d dy=%d)\n", x0, y0, x1, y1, dx, dy);

  if (dy == 0)
    return true;

  enum DXLimitEnum { DXLimit = 16384 << A8_SHIFT };
  if (FOG_UNLIKELY(!Math::isBounded<int>(dx, -DXLimit, DXLimit)))
  {
    int cx = (x0 + x1) >> 1;
    int cy = (y0 + y1) >> 1;

    if (!PathRasterizer8_renderLine(self, x0, y0, cx, cy))
      return false;
    if (!PathRasterizer8_renderLine(self, cx, cy, x1, y1))
      return false;

    return true;
  }

  int ex0 = x0 >> A8_SHIFT;
  int ex1 = x1 >> A8_SHIFT;
  int ey0 = y0 >> A8_SHIFT;
  int ey1 = y1 >> A8_SHIFT;
  int fy0 = y0 & A8_MASK;
  int fy1 = y1 & A8_MASK;

  //CellLogger::out("RenderLine [ex0=%d ey0=%d] -> [ex1=%d ey1=%d] | (fy0=%d fy1=%d)\n", ex0, ey0, ex1, ey1, fy0, fy1);

  int i = 1;
  int j = ey1 - ey0;

  int incr  = ey1 >= ey0;
  int first = A8_SCALE;

  if (!incr)
  {
    first = 0;
    incr  =-1;
    dy    =-dy;
    j     =-j;
  }

  PathRasterizer8::Row* rPtr = self->_rows;
  PathRasterizer8::Row* rEnd = rPtr;

  // --------------------------------------------------------------------------
  // [Initialize / Merge Bounding-Box]
  // --------------------------------------------------------------------------

  if (FOG_UNLIKELY(self->_boundingBox.y0 == -1))
  {
    // This is the first call to the renderLine(), thus initialize the bounding-box.
    self->_boundingBox.x0 = Math::min(ex0, ex1);
    self->_boundingBox.y0 = Math::min(ey0, ey1);
    self->_boundingBox.x1 = Math::max(ex0, ex1);
    self->_boundingBox.y1 = Math::max(ey0, ey1);

    // And clear all rows within it.
    int iy = ey0;
    for (;;)
    {
      rPtr[iy].first = NULL;
      if (iy == ey1)
        break;
      iy += incr;
    }
  }
  else
  {
    // This is not the first call to the renderLine(), thus extend the bounding-box
    // by x0/x1.
    {
      int ix0 = ex0;
      int ix1 = ex1;

      if (ex0 > ex1)
        swap(ix0, ix1);

      if (ix0 < self->_boundingBox.x0) self->_boundingBox.x0 = ix0;
      if (ix1 > self->_boundingBox.x1) self->_boundingBox.x1 = ix1;
    }

    // And purge new rows which will be accessed.
    {
      int iy0 = ey0;
      int iy1 = ey1;
      int ybox;

      if (ey0 > ey1)
        swap(iy0, iy1);

      if (iy0 < (ybox = self->_boundingBox.y0))
      {
        self->_boundingBox.y0 = iy0;
        do {
          rPtr[iy0++].first = NULL;
        } while (iy0 != ybox);
      }

      if (iy1 > (ybox = self->_boundingBox.y1))
      {
        self->_boundingBox.y1 = iy1;
        do {
          rPtr[++ybox].first = NULL;
        } while (ybox != iy1);
      }
    }
  }

  rPtr += ey0;
  rEnd += ey1 + incr;

  FOG_ASSERT(ex0 >= 0 && ey0 >= 0 && ex0 <= self->_size.w && ey0 <= self->_size.h);
  FOG_ASSERT(ex1 >= 0 && ey1 >= 0 && ex1 <= self->_size.w && ey1 <= self->_size.h);

  // --------------------------------------------------------------------------
  // [Vertical Line]
  // --------------------------------------------------------------------------

  // We have to calculate start and end cells, and then - the common values of
  // the area and coverage for all cells of the line. We know exactly there's
  // only one cell, so, we don't have to use complicated hline rendering.
  if (dx == 0)
  {
    int fx = (x0 & 0xFF) << 1;
    int cover = dy;

    if (ey0 != ey1)
      cover = first - fy0;

    //CellLogger::out("Vertical\n");

    for (;;)
    {
      int area = cover * fx;

      do {
        ADD_SINGLE(VLineSingleCell, ex0, cover, area);
        rPtr += incr;
      } while (--i);

      if (rPtr == rEnd)
        break;

      if (j > 1)
      {
        cover = (first << 1) - A8_SCALE;
        i = j - 1;
        j = 1;
      }
      else
      {
        cover = first + fy1 - A8_SCALE;
        i = j;
        j = 1;
      }
    }
    return true;
  }

  // --------------------------------------------------------------------------
  // [Generic Line]
  // --------------------------------------------------------------------------

  else
  {
    int xEnd = x1;
    int fyEnd = fy1;

    int p, rem, mod;
    int lift, delta;

    if (ey0 != ey1)
    {
      fy1   = first;
      p     = ((incr > 0) ? (A8_SCALE - fy0) : fy0) * dx;

      delta = p / dy;
      mod   = p % dy; if (mod < 0) { delta--; mod += dy; }

      x1 = x0 + delta;
    }

    for (;;)
    {
      for (;;)
      {
        // --------------------------------------------------------------------
        // [Generic - HLine - Init]
        // --------------------------------------------------------------------

        int px0 = x0;
        int px1 = x1;

        if (px0 > px1)
          swap(px0, px1);

        int dpx = px1 - px0;
        int dfy = fy1 - fy0;

        int ex0 = px0 >> A8_SHIFT;
        int ex1 = px1 >> A8_SHIFT;

        int fx0 = px0 & A8_MASK;
        int fx1 = px1 & A8_MASK;

        // --------------------------------------------------------------------
        // [Generic - HLine - Nop]
        // --------------------------------------------------------------------

        if (dfy == 0)
        {
          FOG_NOP;
        }

        // --------------------------------------------------------------------
        // [Generic - HLine - Single-Cell]
        // --------------------------------------------------------------------

        else if (ex0 == ex1)
        {
          int cover = dfy;
          int area = dfy * (fx0 + fx1);
          ADD_SINGLE(HLineSingleCell, ex0, cover, area);
        }

        // --------------------------------------------------------------------
        // [Generic - HLine - Multi-Cell]
        // --------------------------------------------------------------------

        else
        {
          int py0 = fy0;
          int stop;

          PathRasterizer8::Chunk* cFirst = rPtr->first;
          PathRasterizer8::Chunk* cEnd = NULL;

          PathRasterizer8::Chunk* cPtr = cFirst;
          PathRasterizer8::Cell* cell;

          // Create cover and area for the first cell.
          int h_p, h_mod;
          int h_lift, h_rem;
          int cover, area;

          h_p   = dfy * (A8_SCALE - fx0);
          cover = h_p / dpx;
          h_mod = h_p % dpx; if (h_mod < 0) { h_mod += dpx; cover--; }
          area  = cover * (fx0 + A8_SCALE);

          // Create cover and area for adjacent cells.
          if (ex1 - ex0 > 1)
          {
            h_p    = dfy * A8_SCALE;
            h_lift = h_p / dpx;
            h_rem  = h_p % dpx; if (h_rem < 0) { h_rem += dpx; h_lift--; }
            h_mod -= dpx;
          }

          // Turn off additive cell merging by default (it will be set to true
          // later if needed).
          int additive = false;

          // Skip the first cell if zero.
          int isFirst = (area != 0);
          int isLast;

          // Convert the [ex0, ex1] coordinates to the [ex0, ex1) format, thus ex1
          // becomes outside of the horizontal-line boundary (this is the standard
          // convention used in Fog).
          ex0 += (area == 0);
          ex1++;

          // ------------------------------------------------------------------
          // [Generic - HLine - Chunk - First]
          // ------------------------------------------------------------------

          if (cPtr == NULL)
          {
            int len = ex1 - ex0;
            if (FOG_UNLIKELY(len > PATHRASTERIZER8_MAX_CHUNK_SIZE))
              len = PATHRASTERIZER8_MAX_CHUNK_SIZE;

            NEW_CHUNK(cPtr, ex0, len);
            cPtr->prev = cPtr;
            cPtr->next = cPtr;

            cFirst = cPtr;
            cEnd = cPtr;

            rPtr->first = cPtr;

            cell = cPtr->cells;
            stop = ex0 + len;
            goto _HLine_Do;
          }

          cEnd = cPtr->prev;

          // ------------------------------------------------------------------
          // [Generic - HLine - Chunk - Prepend]
          // ------------------------------------------------------------------

          if (ex0 < cPtr->x0)
          {
            int len = Math::min(cPtr->x0, ex1) - ex0;
            if (FOG_UNLIKELY(len > PATHRASTERIZER8_MAX_CHUNK_SIZE))
              len = PATHRASTERIZER8_MAX_CHUNK_SIZE;

            PathRasterizer8::Chunk* cNext = cPtr;
            NEW_CHUNK(cPtr, ex0, len);

            cPtr->prev = cEnd;
            cPtr->next = cNext;

            cEnd->next = cPtr;
            cNext->prev = cPtr;

            rPtr->first = cFirst = cPtr;

            cell = cPtr->cells;
            stop = ex0 + len;
            goto _HLine_Do;
          }

          // ------------------------------------------------------------------
          // [Generic - HLine - Chunk - Append]
          // ------------------------------------------------------------------

          // Append a new chunk to the row chunk-list.
          if (ex0 >= cEnd->x1)
          {
            cPtr = cEnd;
            goto _HLine_Append;
          }

          // ------------------------------------------------------------------
          // [Generic - HLine - Chunk - Find]
          // ------------------------------------------------------------------

          if ((ex0 - cPtr->x0) <= (cEnd->x1 - ex0))
          {
            for (;;)
            {
              if (ex0 < cPtr->x1)
                goto _HLine_Merge;

              // The list is curcullar, but we know that the [ex0, ex1] intersects with
              // bounding-box of the chunk-list. This means that we can't skip into the
              // first chunk in this loop.
              cPtr = cPtr->next;
              FOG_ASSERT(cPtr != cFirst);
            }
          }
          else
          {
            cPtr = cEnd;
            for (;;)
            {
              if (ex0 >= cPtr->x0)
              {
                if (ex0 >= cPtr->x1)
                  cPtr = cPtr->next;
                goto _HLine_Merge;
              }

              // Again, we can't iterate forever.
              FOG_ASSERT(cPtr != cFirst);
              cPtr = cPtr->prev;
            }
          }

          // ------------------------------------------------------------------
          // [Generic - HLine - Chunk - Advance]
          // ------------------------------------------------------------------

_HLine_Advance:
          if (cPtr == cEnd)
          {
_HLine_Append:
            int len = ex1 - ex0;
            if (FOG_UNLIKELY(len > PATHRASTERIZER8_MAX_CHUNK_SIZE))
              len = PATHRASTERIZER8_MAX_CHUNK_SIZE;

            PathRasterizer8::Chunk* cPrev = cPtr;
            NEW_CHUNK(cPtr, ex0, len);

            cPtr->prev = cPrev;
            cPtr->next = cFirst;

            cFirst->prev = cPtr;
            cPrev->next = cPtr;
            cEnd = cPtr;

            cell = cPtr->cells;
            stop = ex0 + len;
          }
          else
          {
            cPtr = cPtr->next;

_HLine_Merge:
            if (ex0 < cPtr->x0)
            {
              int len = Math::min(ex1, cPtr->x0) - ex0;
              if (FOG_UNLIKELY(len > PATHRASTERIZER8_MAX_CHUNK_SIZE))
                len = PATHRASTERIZER8_MAX_CHUNK_SIZE;

              PathRasterizer8::Chunk* cPrev = cPtr->prev;
              PathRasterizer8::Chunk* cNext = cPtr;
              NEW_CHUNK(cPtr, ex0, len);

              cPtr->prev = cPrev;
              cPtr->next = cNext;

              cPrev->next = cPtr;
              cNext->prev = cPtr;

              cell = cPtr->cells;
              stop = ex0 + len;
            }
            else
            {
              FOG_ASSERT(ex0 < cPtr->x1);

              cell = &cPtr->cells[ex0 - cPtr->x0];
              stop = Math::min(cPtr->x1, ex1);

              additive = true;
            }
          }

          // ------------------------------------------------------------------
          // [Generic - HLine - Chunk - Fill]
          // ------------------------------------------------------------------

_HLine_Do:
          FOG_ASSERT(ex0 <  stop);
          FOG_ASSERT(ex1 >= stop);

          isLast = (stop == ex1);
          stop -= isLast;

          if (additive)
          {
            if (isFirst)
            {
              FOG_ASSERT(ex0 != stop);
              isFirst = false;
              //CellLogger::log(ex0, (int)(rPtr - self->_rows), cover, area, "H+1st");
              goto _HLine_DoAdd;
            }

            while (ex0 != stop)
            {
              cover = h_lift;
              h_mod  += h_rem; if (h_mod >= 0) { h_mod -= dpx; cover++; }
              area  = cover * A8_SCALE;

              //CellLogger::log(ex0, (int)(rPtr - self->_rows), cover, area, "H+Mid");
_HLine_DoAdd:
              cell->add(cover, area);
              cell++;

              py0 += cover;
              ex0++;
            }

            additive = false;
            if (!isLast)
              goto _HLine_Advance;

            cover = fy1 - py0;
            area = cover * fx1;

            //CellLogger::log(ex0, (int)(rPtr - self->_rows), cover, area, "H+End");
            cell->add(cover, area);
          }
          else
          {
            if (isFirst)
            {
              FOG_ASSERT(ex0 != stop);
              isFirst = false;
              //CellLogger::log(ex0, (int)(rPtr - self->_rows), cover, area, "H=1st");
              goto _HLine_DoSet;
            }

            while (ex0 != stop)
            {
              cover = h_lift;
              h_mod  += h_rem; if (h_mod >= 0) { h_mod -= dpx; cover++; }
              area  = cover * A8_SCALE;

              //CellLogger::log(ex0, (int)(rPtr - self->_rows), cover, area, "H=Mid");
_HLine_DoSet:
              cell->set(cover, area);
              cell++;

              py0 += cover;
              ex0++;
            }

            if (!isLast)
              goto _HLine_Advance;

            cover = fy1 - py0;
            area = cover * fx1;

            //CellLogger::log(ex0, (int)(rPtr - self->_rows), cover, area, "H=End");
            cell->set(cover, area);
          }
        }

        // --------------------------------------------------------------------
        // [Generic - HLine - Advance]
        // --------------------------------------------------------------------

        x0 = x1;

        rPtr += incr;
        if (--i == 0)
          break;

        delta = lift;
        mod  += rem; if (mod >= 0) { mod -= dy; delta++; }
        x1   += delta;
      }

      if (rPtr == rEnd)
        break;

      if (j > 1)
      {
        fy0   = A8_SCALE - first;
        p     = dx * A8_SCALE;

        lift  = p / dy;
        rem   = p % dy; if (rem < 0) { lift--; rem += dy; }
        mod  -= dy;

        delta = lift;
        mod  += rem; if (mod >= 0) { mod -= dy; delta++; }
        x1   += delta;

        i = j - 1;
        j = 1;
      }
      else
      {
        fy0   = A8_SCALE - first;
        fy1   = fyEnd;
        x1    = xEnd;

        i = j;
        j = 1;
      }
    }
    return true;
  }

_Bail:
  return false;
}

// ============================================================================
// [Fog::PathRasterizer8 - Render - Helpers]
// ============================================================================

template<int _RULE, int _USE_ALPHA>
static FOG_INLINE uint32_t PathRasterizer8_calculateAlpha2(const PathRasterizer8* self, int cover)
{
  if (cover < 0) cover = -cover;

  if (_RULE == FILL_RULE_NON_ZERO)
  {
    if (cover > A8_SCALE)
      cover = A8_SCALE;
  }
  else
  {
    cover &= A8_MASK_2;
    if (cover > A8_SCALE)
      cover = A8_SCALE_2 - cover;
  }

  if (_USE_ALPHA)
    cover = (cover * self->_opacity) >> 8;

  return cover;
}

// ============================================================================
// [Fog::PathRasterizer8 - Render - Clip-Box]
// ============================================================================

template<int _RULE, int _USE_ALPHA>
static void FOG_CDECL PathRasterizer8_render_st_clip_box(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* scanline)
{
  PathRasterizer8* self = reinterpret_cast<PathRasterizer8*>(_self);

  FOG_ASSERT(self->_isFinalized);

  if (FOG_IS_ERROR(scanline->prepare(self->_boundingBox.getWidth())))
    return;

  int y0 = self->_boundingBox.y0;
  int y1 = self->_boundingBox.y1;

  PathRasterizer8::Row* rows = self->_rows - self->_sceneBox.y0 + y0;

  int xOffset = self->_sceneBox.x0;
  int xEnd = self->_sceneBox.x1;

  filler->prepare(y0);
  for (;;)
  {
    // ------------------------------------------------------------------------
    // [Merge / Skip]
    // ------------------------------------------------------------------------

    PathRasterizer8::Chunk* first = rows->first;
    rows++;

    if (FOG_IS_NULL(first))
    {
      int mark = y0;
      do {
        if (++y0 >= y1) return;

        first = rows->first;
        rows++;
      } while (first == NULL);

      filler->skip(y0 - mark);
    }

    // ------------------------------------------------------------------------
    // [Fetch]
    // ------------------------------------------------------------------------

    scanline->begin();

    // ------------------------------------------------------------------------
    // [Declare]
    // ------------------------------------------------------------------------

    PathRasterizer8::Chunk* chunk = first;
    PathRasterizer8::Cell* cell = chunk->cells;

    VERIFY_CHUNKS_8(first);
    //CellLogger::logChunk(y0, first);

    uint i = (uint)chunk->getLength();

    int x;
    int xNext = chunk->x0 + xOffset;
    int cover = 0;
    int area;
    uint32_t alpha;

    for (;;)
    {
      if (xNext == xEnd)
        break;

      x = xNext;
      scanline->lnkA8Extra(x);

_Continue:
      x += i;
      if (x > xEnd && --i == 0)
      {
        FOG_ASSERT(x - 1 == xEnd);
        goto _Finalize;
      }

      do {
        area    = cell->area >> (A8_SHIFT_2);
        cover  += cell->cover;

        alpha = PathRasterizer8_calculateAlpha2<_RULE, _USE_ALPHA>(self, cover - area);
        scanline->valA8Extra(alpha);

        cell++;
      } while (--i);

      chunk = chunk->next;
      if (chunk == first)
      {
_Finalize:
        scanline->endA8Extra();
        break;
      }

      cell = chunk->cells;
      xNext = chunk->x0 + xOffset;
      i = (uint)chunk->getLength();

      if (x == xNext)
        goto _Continue;

      scanline->endA8Extra();

      if (area)
        alpha = PathRasterizer8_calculateAlpha2<_RULE, _USE_ALPHA>(self, cover);

      if (alpha)
        scanline->lnkConstSpanOrMerge(x, xNext, alpha);
    }

    // ------------------------------------------------------------------------
    // [Fill / Skip]
    // ------------------------------------------------------------------------

    if (FOG_IS_ERROR(scanline->close()) || scanline->getSpans() == NULL)
    {
_SkipScanline:
      filler->skip(1);
    }
    else
    {
#if defined(FOG_DEBUG_RASTERIZER)
      Rasterizer_dumpSpans(y0, scanline->getSpans());
#endif // FOG_DEBUG_RASTERIZER
      filler->process(scanline->getSpans());
    }

    if (++y0 >= y1)
      return;
  }
}





















// ============================================================================
// [Fog::PathRasterizer8 - Render - Clip-Region]
// ============================================================================

#define RASTER_FETCH_DECLARE() \
  PathRasterizer8::Chunk* chunk; \
  PathRasterizer8::Cell* cPtr; \
  PathRasterizer8::Cell* cEnd; \
  \
  int x; \
  int xNext; \
  int area; \
  int cover

#define RASTER_FETCH_BEGIN(_Skip_) \
  scanline->begin(); \
  \
  FOG_ASSERT(chunk != NULL); \
  cPtr = chunk->cells; \
  cEnd = cPtr + chunk->getLength(); \
  \
  xNext = chunk->x0 + xOffset; \
  cover = 0;

#define RASTER_FETCH_CELL() \
  FOG_MACRO_BEGIN \
    x      = xNext; \
    area   = cPtr->area; \
    cover += cPtr->cover << (A8_SHIFT + 1); \
    \
    for (;;) \
    { \
      if (++cPtr == cEnd) \
      { \
        chunk = chunk->next; \
        if (chunk == NULL) break; \
        \
        cPtr = chunk->cells; \
        cEnd = cPtr + chunk->getLength(); \
        xNext = chunk->x0 + xOffset; \
        FOG_ASSERT(x != xNext); \
      } \
      else \
      { \
        xNext++; \
      } \
      break; \
    } \
  FOG_MACRO_END

template<int _RULE, int _USE_ALPHA>
static void FOG_CDECL PathRasterizer8_render_st_clip_region(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* scanline)
{
  PathRasterizer8* self = reinterpret_cast<PathRasterizer8*>(_self);

#if 0
  FOG_ASSERT(self->_isFinalized);

  if (FOG_IS_ERROR(scanline->prepare(self->_boundingBox.getWidth())))
    return;

  int y0 = self->_boundingBox.y0;
  int y1 = self->_boundingBox.y1;

  void** rows = self->_rows - self->_sceneBox.y0;
  int xOffset = self->_sceneBox.x0;

  const BoxI* rectCur = self->_clip.region.data;
  const BoxI* rectEnd = rectCur + self->_clip.region.length;

  // Find the rectangle which is closest to the y0.
  rectCur = RegionUtil::getClosestBox(rectCur, self->_clip.region.length, y0);
  if (FOG_IS_NULL(rectCur))
    return;

  if (y0 < rectCur->y0)
  {
    y0 = rectCur->y0;
    if (y0 >= y1) return;
  }

  const BoxI* bandCur = rectCur;
  const BoxI* bandEnd = RegionUtil::getEndBand(rectCur, rectEnd);

  filler->prepare(y0);

  for (;;)
  {
    // ------------------------------------------------------------------------
    // [Declare]
    // ------------------------------------------------------------------------

    RASTER_FETCH_DECLARE();
    int bandX0;
    int bandX1 = bandCur->x1;

    // ------------------------------------------------------------------------
    // [Merge / Skip]
    // ------------------------------------------------------------------------

    numCells = PathRasterizer8_mergeCells<_CHUNK_TYPE, _CELL_TYPE>(self, rows[y0], &cellCur, buffer);
    if (FOG_UNLIKELY(numCells == 0))
    {
      int mark = y0;
      do {
        if (++y0 >= y1) return;
        numCells = PathRasterizer8_mergeCells<_CHUNK_TYPE, _CELL_TYPE>(self, rows[y0], &cellCur, buffer);
      } while (numCells == 0);
      filler->skip(y0 - mark);
    }

    // ------------------------------------------------------------------------
    // [Fetch]
    // ------------------------------------------------------------------------

    RASTER_FETCH_BEGIN(_SkipScanline);

    // Discard clip-boxes that can't intersect.
    while (bandX1 <= xNext)
    {
_AdvanceClip:
      if (++bandCur == bandEnd) goto _EndScanline;
      bandX1 = bandCur->x1;
    }

    bandX0 = bandCur->x0;
    FOG_ASSERT(xNext < bandX1);

    for (;;)
    {
      RASTER_FETCH_CELL();

      // ----------------------------------------------------------------------
      // [H-Line]
      // ----------------------------------------------------------------------

      if (x + 1 == xNext)
      {
        if (x < bandX0) continue;

        // Skip this cell if resulting mask is zero.
        uint32_t alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area);
        if (alpha == 0) continue;

        // Okay, it seems that we will now generate some masks, embedded to one
        // span instance.
        scanline->lnkA8Extra(x);
        scanline->valA8Extra(alpha);

        if (bandX1 <= xNext)
        {
          scanline->endA8Extra(xNext);
          goto _AdvanceClip;
        }

        for (;;)
        {
          RASTER_FETCH_CELL();
          alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area);

          if (++x == xNext)
          {
            scanline->valA8Extra(alpha);
            if (bandX1 <= xNext)
            {
              scanline->endA8Extra(x);
              goto _AdvanceClip;
            }
            continue;
          }
          break;
        }

        if (area != 0)
        {
          if (alpha != 0x00) scanline->valA8Extra(alpha);
        }
        else
        {
          x--;
        }

        if (numCells == 0) break;
      }

      // ----------------------------------------------------------------------
      // [V-Line]
      // ----------------------------------------------------------------------

      else
      {
        uint32_t alpha;
        if (area != 0)
        {
          if (x >= bandX0 && (alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area)) != 0)
          {
            scanline->lnkA8Extra(x);
            scanline->valA8Extra(alpha);
            scanline->endA8Extra(x + 1);
          }
          x++;
        }
        if (numCells == 0) break;
      }

      // ----------------------------------------------------------------------
      // [Area]
      // ----------------------------------------------------------------------

      {
        if (x >= bandX1)
        {
          if (++bandCur == bandEnd) goto _EndScanline;
          bandX0 = bandCur->x0;
          bandX1 = bandCur->x1;
          FOG_ASSERT(x < bandX1);
        }

        uint32_t alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover);
        if (alpha && xNext > bandX0)
        {
          if (x < bandX0) x = bandX0;

          if (x < bandX1)
          {
            FOG_ASSERT(x < bandX1);
            int toX = Math::min<int>(xNext, bandX1);
            scanline->lnkConstSpanOrMerge(x, toX, alpha);
            x = toX;
          }

          if (xNext > bandX1)
          {
            for (;;)
            {
              if (++bandCur == bandEnd) goto _EndScanline;
              bandX0 = bandCur->x0;
              bandX1 = bandCur->x1;
              if (xNext > bandX0)
              {
                scanline->lnkConstSpan(bandX0, Math::min<int>(xNext, bandX1), alpha);
                if (xNext >= bandX1) continue;
              }
              break;
            }
          }
        }

        if (xNext >= bandX1) goto _AdvanceClip;
      }
    }

    // ------------------------------------------------------------------------
    // [Fill / Skip]
    // ------------------------------------------------------------------------

_EndScanline:
    if (FOG_IS_ERROR(scanline->close()) || scanline->getSpans() == NULL)
    {
_SkipScanline:
      filler->skip(1);
    }
    else
    {
#if defined(FOG_DEBUG_RASTERIZER)
      Rasterizer_dumpSpans(y0, scanline->getSpans());
#endif // FOG_DEBUG_RASTERIZER
      filler->process(scanline->getSpans());
    }

    if (++y0 >= y1)
      return;

    // Advance clip to the next band if needed.
    if (rectCur->y1 <= y0)
    {
      if (bandEnd == rectEnd)
        return;

      y0 = bandEnd[0].y0;
      if (y0 >= y1)
        return;

      rectCur = bandEnd;
      bandEnd = RegionUtil::getEndBand(rectCur, rectEnd);
    }

    bandCur = rectCur;
  }
#endif
}

// ============================================================================
// [Fog::PathRasterizer8 - Render - Clip-Mask]
// ============================================================================

template<int _RULE, int _USE_ALPHA>
static void FOG_CDECL PathRasterizer8_render_st_clip_mask(
  Rasterizer8* _self, RasterFiller* filler, RasterScanline8* scanline)
{
  PathRasterizer8* self = reinterpret_cast<PathRasterizer8*>(_self);

#if 0
  FOG_ASSERT(self->_isFinalized);

  if (FOG_IS_ERROR(scanline->prepare(self->_boundingBox.getWidth())))
    return;

  int y0 = Math::max<int>(self->_boundingBox.y0, self->_clip.mask.y0);
  int y1 = Math::min<int>(self->_boundingBox.y1, self->_clip.mask.y1);

  if (y0 >= y1)
    return;

  void** rows = self->_rows - self->_sceneBox.y0;
  const RasterSpan8** mrows = self->_clip.mask.spans - self->_clip.mask.y0;
  int xOffset = self->_sceneBox.x0;

  filler->prepare(y0);

  for (;;)
  {
    // ------------------------------------------------------------------------
    // [Declare]
    // ------------------------------------------------------------------------

    RASTER_FETCH_DECLARE();
    const RasterSpan8* mask;                // Mask-span instance.
    int maskX0;                             // Mask-span x0.
    int maskX1;                             // Mask-span x1.
    uint32_t maskType;                      // Mask-span type.
    const uint8_t* maskData;                // Mask-span data pointer (or const-value).

    // ------------------------------------------------------------------------
    // [Merge / Skip]
    // ------------------------------------------------------------------------

    numCells = PathRasterizer8_mergeCells<_CHUNK_TYPE, _CELL_TYPE>(self, rows[y0], &cellCur, buffer);
    mask = mrows[y0];
    if (FOG_UNLIKELY(numCells == 0 || mask == NULL))
    {
      int mark = y0;
      do {
        if (++y0 >= y1) return;
        numCells = PathRasterizer8_mergeCells<_CHUNK_TYPE, _CELL_TYPE>(self, rows[y0], &cellCur, buffer);
        mask = mrows[y0];
      } while (numCells == 0 || mask == NULL);
      filler->skip(y0 - mark);
    }

    // ------------------------------------------------------------------------
    // [Fetch]
    // ------------------------------------------------------------------------

    RASTER_FETCH_BEGIN(_SkipScanline);

    // ------------------------------------------------------------------------
    // [Clip]
    // ------------------------------------------------------------------------

    maskX1 = mask->getX1();

#define MASK_SPAN_CHANGED() \
    FOG_MACRO_BEGIN \
      maskX0 = mask->getX0(); \
      FOG_ASSERT(maskX0 < maskX1); \
      \
      maskType = mask->getType(); \
      maskData = reinterpret_cast<uint8_t*>(mask->getGenericMask()); \
      maskData -= RasterSpan8::getMaskAdvance(maskType, maskX0); \
    FOG_MACRO_END

    // ------------------------------------------------------------------------
    // [Prepare]
    // ------------------------------------------------------------------------

    // Advance clip (discard clip-boxes that can't intersect).
    while (maskX1 <= xNext)
    {
_AdvanceClip:
      if ((mask = mask->getNext()) == NULL) goto _EndScanline;
      maskX1 = mask->getX1();
    }

    MASK_SPAN_CHANGED();
    FOG_ASSERT(xNext < maskX1);

    // ------------------------------------------------------------------------
    // [Main-Loop]
    // ------------------------------------------------------------------------

_Continue:
    for (;;)
    {
      RASTER_FETCH_CELL();
      FOG_ASSERT(x < maskX1);

      // ----------------------------------------------------------------------
      // [H-Line]
      // ----------------------------------------------------------------------

      if (x + 1 == xNext)
      {
        if (x < maskX0) continue;

        // Skip this cell if resulting mask is zero.
        uint32_t alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area);
        if (alpha == 0)
        {
          if (maskX1 <= xNext) goto _AdvanceClip;
          continue;
        }

        switch (maskType)
        {
          case RASTER_SPAN_C:
          {
            Face::p32 m = RasterSpan8::getConstMaskFromPointer(maskData);
            Face::p32MulDiv256SBW(alpha, alpha, m);

            if (alpha == 0x00)
            {
              if (maskX1 <= xNext) goto _AdvanceClip;
              goto _Continue;
            }

            scanline->lnkA8Extra(x);
            scanline->valA8Extra(alpha);
            if (maskX1 <= xNext) { scanline->endA8Extra(xNext); goto _AdvanceClip; }

            for (;;)
            {
              RASTER_FETCH_CELL();
              FOG_ASSERT(x >= maskX0 && x < maskX1);

              alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area);
              Face::p32MulDiv256SBW(alpha, alpha, m);

              if (++x != xNext) break;

              scanline->valA8Extra(alpha);
              if (maskX1 <= xNext)
              {
                scanline->endA8Extra(x);
                goto _AdvanceClip;
              }
            }

            if (area != 0)
            {
              if (alpha != 0) scanline->valA8Extra(alpha);
            }
            else
            {
              x--;
            }

            scanline->endA8Extra();
            break;
          }

          case RASTER_SPAN_A8_GLYPH:
          case RASTER_SPAN_AX_GLYPH:
          {
            Face::p32 m;

            Face::p32Load1b(m, maskData + x);
            Face::p32MulDiv256SBW(m, m, alpha);

            if (m == 0x00)
            {
              if (maskX1 <= xNext) goto _AdvanceClip;
              goto _Continue;
            }

            scanline->lnkA8Glyph(x);
            scanline->valA8Glyph(m);
            if (maskX1 <= xNext) { scanline->endA8Glyph(xNext); goto _AdvanceClip; }

            for (;;)
            {
              RASTER_FETCH_CELL();
              FOG_ASSERT(x >= maskX0 && x < maskX1);

              alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area);
              Face::p32Load1b(m, maskData + x);
              Face::p32MulDiv256SBW(m, m, alpha);

              if (++x == xNext)
              {
                scanline->valA8Glyph(m);
                if (maskX1 <= xNext) { scanline->endA8Glyph(x); goto _AdvanceClip; }
              }
              else
              {
                break;
              }
            }

            if (area != 0)
            {
              if (m != 0x00) scanline->valA8Glyph(m);
            }
            else
            {
              x--;
            }

            scanline->endA8Glyph();
            break;
          }

          case RASTER_SPAN_AX_EXTRA:
          {
            Face::p32 m;

            Face::p32Load2a(m, maskData + x * 2);
            Face::p32MulDiv256SBW(m, m, alpha);

            if (m == 0x00)
            {
              if (maskX1 <= xNext) goto _AdvanceClip;
              goto _Continue;
            }

            scanline->lnkA8Extra(x);
            scanline->valA8Extra(m);
            if (maskX1 <= xNext) { scanline->endA8Extra(xNext); goto _AdvanceClip; }

            for (;;)
            {
              RASTER_FETCH_CELL();
              FOG_ASSERT(x >= maskX0 && x < maskX1);

              alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area);
              Face::p32Load2a(m, maskData + x * 2);
              Face::p32MulDiv256SBW(m, m, alpha);

              if (++x == xNext)
              {
                scanline->valA8Extra(m);
                if (maskX1 <= xNext) { scanline->endA8Extra(x); goto _AdvanceClip; }
              }
              else
              {
                break;
              }
            }

            if (area != 0)
            {
              if (m != 0x00) scanline->valA8Extra(m);
            }
            else
            {
              x--;
            }

            scanline->endA8Extra();
            break;
          }

          case RASTER_SPAN_ARGB32_GLYPH:
          case RASTER_SPAN_ARGBXX_GLYPH:
          {
            Face::p32 m;

            Face::p32Load4a(m, maskData + x * 4);
            Face::p32MulDiv256PBB_SBW(m, m, alpha);

            if (m == 0x00000000)
            {
              if (maskX1 <= xNext) goto _AdvanceClip;
              goto _Continue;
            }

            scanline->lnkARGB32Glyph(x);
            scanline->valARGB32Glyph(m);
            if (maskX1 <= xNext) { scanline->endARGB32Glyph(xNext); goto _AdvanceClip; }

            for (;;)
            {
              RASTER_FETCH_CELL();
              FOG_ASSERT(x >= maskX0 && x < maskX1);

              alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area);
              Face::p32Load4a(m, maskData + x * 4);
              Face::p32MulDiv256PBB_SBW(m, m, alpha);

              if (++x == xNext)
              {
                scanline->valARGB32Glyph(m);
                if (maskX1 <= xNext) { scanline->endARGB32Glyph(x); goto _AdvanceClip; }
              }
              else
              {
                break;
              }
            }

            if (area != 0)
            {
              if (m != 0x00000000) scanline->valARGB32Glyph(m);
            }
            else
            {
              x--;
            }

            scanline->endARGB32Glyph();
            break;
          }

          default:
            FOG_ASSERT_NOT_REACHED();
        }
      }

      // ----------------------------------------------------------------------
      // [V-Line]
      // ----------------------------------------------------------------------

      else
      {
        uint32_t alpha;

        if (area != 0)
        {
          if (x >= maskX0 && (alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - area)) != 0)
          {
            FOG_ASSERT(x >= maskX0 && x < maskX1);
            Face::p32 m;

            switch (maskType)
            {
              case RASTER_SPAN_C:
                m = RasterSpan8::getConstMaskFromPointer(maskData);
                goto _VLine_AxExtra;

              case RASTER_SPAN_A8_GLYPH:
              case RASTER_SPAN_AX_GLYPH:
                Face::p32Load1b(m, maskData + x);
                Face::p32MulDiv256SBW(m, m, alpha);

                if (m != 0x00)
                {
                  scanline->lnkA8Glyph(x);
                  scanline->valA8Glyph(m);
                  scanline->endA8Glyph(x + 1);
                }
                break;

              case RASTER_SPAN_AX_EXTRA:
                Face::p32Load4a(m, maskData + x * 2);
_VLine_AxExtra:
                Face::p32MulDiv256SBW(m, m, alpha);

                if (m != 0x00)
                {
                  scanline->lnkA8Extra(x);
                  scanline->valA8Extra(m);
                  scanline->endA8Extra(x + 1);
                }
                break;

              case RASTER_SPAN_ARGB32_GLYPH:
              case RASTER_SPAN_ARGBXX_GLYPH:
                Face::p32Load4a(m, maskData + x * 4);
                Face::p32MulDiv256PBB_SBW(m, m, alpha);

                if (m != 0x00000000)
                {
                  scanline->lnkARGB32Glyph(x);
                  scanline->valARGB32Glyph(m);
                  scanline->endARGB32Glyph(x + 1);
                }
                break;

              default:
                FOG_ASSERT_NOT_REACHED();
            }
          }
          x++;
        }
      }

      if (numCells == 0) break;

      // ----------------------------------------------------------------------
      // [Area]
      // ----------------------------------------------------------------------

      if (xNext > maskX0)
      {
        if (maskX1 <= x)
        {
          if ((mask = mask->getNext()) == NULL) goto _EndScanline;
          maskX1 = mask->getX1();
          MASK_SPAN_CHANGED();
          FOG_ASSERT(x < maskX1);
        }

        uint32_t alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover);

        if (alpha)
        {
          if (x < maskX0) x = maskX0;

          if (x < maskX1)
          {
            int toX = Math::min<int>(xNext, maskX1);

            FOG_ASSERT(x < maskX1);
            FOG_ASSERT(x < toX);

            switch (maskType)
            {
              case RASTER_SPAN_C:
              {
                Face::p32 m = RasterSpan8::getConstMaskFromPointer(maskData);
                Face::p32MulDiv256SBW(m, m, alpha);
                if (m != 0x00) scanline->lnkConstSpanOrMerge(x, toX, m);
                break;
              }

              case RASTER_SPAN_A8_GLYPH:
              case RASTER_SPAN_AX_GLYPH:
              {
                // TODO: Rasterizer.
                /*
                _api_raster.mask[MASK_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_c(
                  scanline->lnkA8Extra_buf(x, toX),  // Destination.
                  maskData + x,                     // Source A.
                  alpha,                            // Source B.
                  toX - x);                         // Length.
                */
                break;
              }

              case RASTER_SPAN_AX_EXTRA:
              {
                // TODO: Rasterizer.
                break;
              }

              case RASTER_SPAN_ARGB32_GLYPH:
              case RASTER_SPAN_ARGBXX_GLYPH:
              {
                // TODO: Rasterizer.
                break;
              }

              default:
                FOG_ASSERT_NOT_REACHED();
            }

            x = toX;
          }

          if (xNext > maskX1)
          {
            for (;;)
            {
              if ((mask = mask->getNext()) == NULL) goto _EndScanline;

              maskX1 = mask->getX1();
              MASK_SPAN_CHANGED();

              if (xNext > maskX0)
              {
                int toX = Math::min<int>(xNext, maskX1);
                FOG_ASSERT(maskX0 < toX);

                switch (maskType)
                {
                  case RASTER_SPAN_C:
                  {
                    Face::p32 m = RasterSpan8::getConstMaskFromPointer(maskData);
                    Face::p32MulDiv256SBW(m, m, alpha);
                    if (m != 0x00) scanline->lnkConstSpanOrMerge(maskX0, toX, m);
                    break;
                  }

                  case RASTER_SPAN_A8_GLYPH:
                  case RASTER_SPAN_AX_GLYPH:
                  {
                    // TODO: Rasterizer.
                    /*
                    _api_raster.mask[MASK_OP_INTERSECT][IMAGE_FORMAT_A8].v_op_c(
                      scanline->lnkA8Extra_buf(maskX0, toX),  // Destination.
                      maskData + maskX0,                     // Source A.
                      alpha,                                 // Source B.
                      toX - maskX0);                         // Length.
                    */
                    break;
                  }

                  case RASTER_SPAN_AX_EXTRA:
                  {
                    // TODO: Rasterizer.
                    break;
                  }

                  case RASTER_SPAN_ARGB32_GLYPH:
                  case RASTER_SPAN_ARGBXX_GLYPH:
                  {
                    // TODO: Rasterizer.
                    break;
                  }

                  default:
                    FOG_ASSERT_NOT_REACHED();
                }
                if (maskX1 <= xNext) continue;
              }
              break;
            }
          }
        }

        if (maskX1 <= xNext) goto _AdvanceClip;
      }
    }

#undef MASK_SPAN_CHANGED

    // ------------------------------------------------------------------------
    // [Fill / Skip]
    // ------------------------------------------------------------------------

_EndScanline:
    if (FOG_IS_ERROR(scanline->close()) || scanline->getSpans() == NULL)
    {
_SkipScanline:
      filler->skip(1);
    }
    else
    {
#if defined(FOG_DEBUG_RASTERIZER)
      Rasterizer_dumpSpans(y0, scanline->getSpans());
#endif // FOG_DEBUG_RASTERIZER
      filler->process(scanline->getSpans());
    }

    if (++y0 >= y1)
      return;
  }
#endif
}

#undef RENDER_VARIABLES
#undef RASTER_FETCH_ROW
#undef RASTER_FETCH_CELL




















// ============================================================================
// [Fog::PathRasterizer8 - Finalize]
// ============================================================================

err_t PathRasterizer8::finalize()
{
  PathRasterizer8* self = this;

  // If already finalized this is the NOP.
  if (_error != ERR_OK || _isFinalized) return _error;

  if (_boundingBox.y0 == -1) goto _NotValid;

  // Translate bounding box to match _sceneBox.
  _boundingBox.translate(_sceneBox.x0, _sceneBox.y0);
  // Normalize bounding box to our standard, x1/y1 coordinates are outside.
  _boundingBox.x1++;
  _boundingBox.y1++;

  // Small tolerance here due to the floating point arithmetic. Sweep scanline
  // never return anything beyond the bounding box.
  FOG_ASSERT(_boundingBox.x1 <= _sceneBox.x1 + 1);
  FOG_ASSERT(_boundingBox.y1 <= _sceneBox.y1 + 1);

  if (_boundingBox.x1 > _sceneBox.x1) _boundingBox.x1 = _sceneBox.x1;
  if (_boundingBox.y1 > _sceneBox.y1) _boundingBox.y1 = _sceneBox.y1;

  // We added small tolerance to the rasterizer because of floating point based
  // clipping, but if there is no bounding box (due to correction above) then
  // there is no shape.
  _isValid = (_boundingBox.x0 != _boundingBox.x1 &&
              _boundingBox.y0 != _boundingBox.y1);
  _isFinalized = true;

  // Setup render method.
  if (self->_fillRule == FILL_RULE_NON_ZERO)
    self->_render = Rasterizer_api.path8.render_nonzero[self->_opacity != 0x100][self->_clipType];
  else
    self->_render = Rasterizer_api.path8.render_evenodd[self->_opacity != 0x100][self->_clipType];
  return ERR_OK;

_NotValid:
  _isValid = false;
  _isFinalized = true;
  return ERR_OK;

#undef SETUP_FUNCS
}

FOG_NO_EXPORT void Rasterizer_init(void)
{
  // --------------------------------------------------------------------------
  // [Fog::BoxRasterizer8]
  // --------------------------------------------------------------------------

  Rasterizer_api.box8.init32x0 = BoxRasterizer8_init32x0;
  Rasterizer_api.box8.init24x8 = BoxRasterizer8_init24x8;

  Rasterizer_api.box8.render_32x0[RASTER_CLIP_BOX   ] = BoxRasterizer8_render_32x0_st_clip_box;
  Rasterizer_api.box8.render_32x0[RASTER_CLIP_REGION] = BoxRasterizer8_render_32x0_st_clip_region;
  Rasterizer_api.box8.render_32x0[RASTER_CLIP_MASK  ] = BoxRasterizer8_render_32x0_st_clip_mask;

  Rasterizer_api.box8.render_24x8[RASTER_CLIP_BOX   ] = BoxRasterizer8_render_24x8_st_clip_box;
  Rasterizer_api.box8.render_24x8[RASTER_CLIP_REGION] = BoxRasterizer8_render_24x8_st_clip_region;
  Rasterizer_api.box8.render_24x8[RASTER_CLIP_MASK  ] = BoxRasterizer8_render_24x8_st_clip_mask;

  // --------------------------------------------------------------------------
  // [Fog::PathRasterizer8]
  // --------------------------------------------------------------------------

  Rasterizer_api.path8.render_nonzero[0][RASTER_CLIP_BOX   ] = PathRasterizer8_render_st_clip_box   <FILL_RULE_NON_ZERO, 0>;
  Rasterizer_api.path8.render_nonzero[0][RASTER_CLIP_REGION] = PathRasterizer8_render_st_clip_region<FILL_RULE_NON_ZERO, 0>;
  Rasterizer_api.path8.render_nonzero[0][RASTER_CLIP_MASK  ] = PathRasterizer8_render_st_clip_mask  <FILL_RULE_NON_ZERO, 0>;

  Rasterizer_api.path8.render_nonzero[1][RASTER_CLIP_BOX   ] = PathRasterizer8_render_st_clip_box   <FILL_RULE_NON_ZERO, 1>;
  Rasterizer_api.path8.render_nonzero[1][RASTER_CLIP_REGION] = PathRasterizer8_render_st_clip_region<FILL_RULE_NON_ZERO, 1>;
  Rasterizer_api.path8.render_nonzero[1][RASTER_CLIP_MASK  ] = PathRasterizer8_render_st_clip_mask  <FILL_RULE_NON_ZERO, 1>;

  Rasterizer_api.path8.render_evenodd[0][RASTER_CLIP_BOX   ] = PathRasterizer8_render_st_clip_box   <FILL_RULE_EVEN_ODD, 0>;
  Rasterizer_api.path8.render_evenodd[0][RASTER_CLIP_REGION] = PathRasterizer8_render_st_clip_region<FILL_RULE_EVEN_ODD, 0>;
  Rasterizer_api.path8.render_evenodd[0][RASTER_CLIP_MASK  ] = PathRasterizer8_render_st_clip_mask  <FILL_RULE_EVEN_ODD, 0>;

  Rasterizer_api.path8.render_evenodd[1][RASTER_CLIP_BOX   ] = PathRasterizer8_render_st_clip_box   <FILL_RULE_EVEN_ODD, 1>;
  Rasterizer_api.path8.render_evenodd[1][RASTER_CLIP_REGION] = PathRasterizer8_render_st_clip_region<FILL_RULE_EVEN_ODD, 1>;
  Rasterizer_api.path8.render_evenodd[1][RASTER_CLIP_MASK  ] = PathRasterizer8_render_st_clip_mask  <FILL_RULE_EVEN_ODD, 1>;
}

} // Fog namespace
