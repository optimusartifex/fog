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
// [Debug]
// ============================================================================

// #define FOG_DEBUG_RASTERIZER

#if defined(FOG_DEBUG_RASTERIZER)
struct FOG_NO_EXPORT RasterizerLogger
{
  static RasterizerLogger instance;

  FILE* f;
  RasterizerLogger()
  {
    f = fopen("Cell.txt", "w+");
  }

  static void out(const char* fmt, ...)
  {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(instance.f, fmt, ap);
    va_end(ap);
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

  static void logCell(const char* _where, int x, int y, int cover, int area)
  {
    fprintf(instance.f, "%s [%3d %3d] ... [Cover=%d, Area=%d (%d*%d)]\n",
      _where, x, y,
      cover,
      area, cover ? area / cover : 0, cover);
    fflush(instance.f);
  }
};
RasterizerLogger RasterizerLogger::instance;

static void Rasterizer_dumpSpans(int y, const RasterSpan8* span)
{
  StringA b;
  b.appendFormat("Y=%d - ", y);

  while (span)
  {
    if (span->isConst())
    {
      b.appendFormat("C[%d %d]%0.2X", span->_x0, span->_x1, span->getConstMask());
    }
    else
    {
      b.appendFormat("M[%d %d]", span->_x0, span->_x1);
      for (int x = 0; x < span->getLength(); x++)
        b.appendFormat("%0.2X", reinterpret_cast<uint8_t*>(span->_mask)[x]);
    }
    b.append(' ');
    span = span->getNext();
  }

  printf("%s\n", b.getData());
}

static void PathRasterizer8_verifyChunks(PathRasterizer8::Chunk* first)
{
  PathRasterizer8::Chunk* chunk = first;

  int sum = 0;

  FOG_ASSERT_X(chunk->x0 < first->prev->x1,
    "Fog::PathRasterizer8::verifyChunks() - Invalid first chunk.");

  do {
    PathRasterizer8::Chunk* next = chunk->next;

    FOG_ASSERT(chunk->x0 <  chunk->x1);
    FOG_ASSERT(chunk->x1 <= next->x0 || next == first);

    int length = chunk->x1 - chunk->x0;
    for (int i = 0; i < length; i++)
    {
      sum += chunk->cells[i].cover;
    }

    chunk = next;
  } while (chunk != first);

  FOG_ASSERT_X(sum == 0,
    "Fog::PathRasterizer8::verifyChunks() - Invalid sum of cells, must be zero.");
}

# define LOG_CELL(_Where_, _X_, _Y_, _Cover_, _Area_) RasterizerLogger::logCell(_Where_, _X_, _Y_, _Cover_, _Area_)
# define VERIFY_CHUNKS_8(_First_) PathRasterizer8_verifyChunks(_First_)
#else
# define LOG_CELL(_Where_, _X_, _Y_, _Cover_, _Area_) FOG_NOP
# define VERIFY_CHUNKS_8(_First_) FOG_NOP
#endif // FOG_DEBUG_RASTERIZER

// ============================================================================
// [Helpers]
// ============================================================================

enum A8_ENUM
{
  A8_SHIFT   = 8,             // 8
  A8_SHIFT_2 = A8_SHIFT + 1,  // 9

  A8_SCALE   = 1 << A8_SHIFT, // 256
  A8_SCALE_2 = A8_SCALE * 2,  // 512

  A8_MASK    = A8_SCALE - 1,  // 255
  A8_MASK_2  = A8_SCALE_2 - 1,// 511

  A8_I32_COORD_LIMIT = 16384 << A8_SHIFT,

  A8_ALLOCATOR_SIZE = 16384 - 84,
  A8_MAX_CHUNK_LENGTH = A8_ALLOCATOR_SIZE / (sizeof(PathRasterizer8::Cell) * 8)
};

enum CELL_OP
{
  CELL_OP_COPY  = 0x0,
  CELL_OP_MERGE = 0x2,

  CELL_OP_POSITIVE = 0x0,
  CELL_OP_NEGATIVE = 0x1
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

  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

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
    hl  = hr;
  }

  if ((y0 & ~0xFF) == (y1 & ~0xFF))
  {
    vb -= vt;
    vt  = vb;
  }

  vt *= opacity;
  vb *= opacity;

  // --------------------------------------------------------------------------
  // [Corners]
  // --------------------------------------------------------------------------

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
// [Fog::BoxRasterizer8 - Render - 32x0]
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

  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  filler->prepare(y0);
  RasterFiller::ProcessFunc process = filler->_process;

  RasterSpanExt8 span[1];
  span[0].setPositionAndType(box.x0, box.x1, RASTER_SPAN_C);
  span[0].setConstMask(self->_opacity);
  span[0].setNext(NULL);

  // --------------------------------------------------------------------------
  // [Process]
  // --------------------------------------------------------------------------

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
// [Fog::PathRasterizer8 - Construction / Destruction]
// ============================================================================

PathRasterizer8::PathRasterizer8() :
  _allocator(A8_ALLOCATOR_SIZE)
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

template<typename FixedT>
static bool PathRasterizer8_renderLine(PathRasterizer8* self, FixedT x0, FixedT y0, FixedT x1, FixedT y1);

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

  _offsetF.set(0, -_sceneBox.y0);
  _offsetD.set(0, -_sceneBox.y0);
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

      if ((x0 != x1) | (y0 != y1) && !PathRasterizer8_renderLine<int>(self, x0, y0, x1, y1))
        return;

      x0 = x1;
      y0 = y1;

      srcPts++;
      srcCmd++;

      if (srcCmd == srcEnd)
        goto _ClosePath;
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

        if (!PathRasterizer8_renderLine<int>(self, x0, y0, x1, y1))
          return;

        x0 = x1;
        y0 = y1;

        top--;
        curve -= 2;
      } while (top >= 0);

      srcPts += 2;
      srcCmd += 2;

      if (srcCmd == srcEnd)
        goto _ClosePath;
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

          if (!PathRasterizer8_renderLine<int>(self, x0, y0, x1, y1))
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

      if (srcCmd == srcEnd)
        goto _ClosePath;
    }

    // ------------------------------------------------------------------------
    // [MoveTo / Close]
    // ------------------------------------------------------------------------

    else
    {
      FOG_ASSERT(PathCmd::isMoveTo(c) || PathCmd::isClose(c));

_ClosePath:
      // Close the current polygon.
      if ((x0 != startX0) | (y0 != startY0) && !PathRasterizer8_renderLine<int>(self, x0, y0, startX0, startY0))
        return;

      if (srcCmd == srcEnd)
        return;
      
      srcPts++;
      srcCmd++;

      if (srcCmd == srcEnd)
        return;

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
// [Fog::PathRasterizer8 - ROW_NEW_CHUNK]
// ============================================================================

#define ROW_NEW_CHUNK(_Chunk_, _X_, _Length_) \
  FOG_MACRO_BEGIN \
    _Chunk_ = reinterpret_cast<PathRasterizer8::Chunk*>(self->_allocator.alloc(PathRasterizer8::Chunk::getSizeOf(_Length_))); \
    if (FOG_IS_NULL(_Chunk_)) goto _Bail; \
    \
    _Chunk_->x0 = (_X_); \
    _Chunk_->x1 = (_X_) + _Length_; \
  FOG_MACRO_END

// ============================================================================
// [Fog::PathRasterizer8 - ROW_ADD_ONE]
// ============================================================================

#define ROW_ADD_ONE(_Name_, _Row_, _X_, _Cover_, _Area_) \
  FOG_MACRO_BEGIN \
    PathRasterizer8::Chunk* _chunk = _Row_->first; \
    PathRasterizer8::Chunk* _cNew; \
    PathRasterizer8::Chunk* _cLast; \
    \
    /* ------------------------------------------------------------------- */ \
    /* [First]                                                             */ \
    /* ------------------------------------------------------------------- */ \
    \
    if (_chunk == NULL) \
    { \
      ROW_NEW_CHUNK(_cNew, _X_, 1); \
      _cNew->prev = _cNew; \
      _cNew->next = _cNew; \
      \
      _Row_->first = _cNew; \
      goto _Name_##_Add_One_Set; \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Prepend]                                                           */ \
    /* ------------------------------------------------------------------- */ \
    \
    _cLast = _chunk->prev; \
    \
    if (_X_ < _chunk->x0) \
    { \
      ROW_NEW_CHUNK(_cNew, _X_, 1); \
      _cNew->prev = _cLast; \
      _cNew->next = _chunk; \
      \
      _cLast->next = _cNew; \
      _chunk->prev = _cNew; \
      \
      _Row_->first = _cNew; \
      goto _Name_##_Add_One_Set; \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Insert / Append]                                                   */ \
    /* ------------------------------------------------------------------- */ \
    \
    if (_X_ < _cLast->x1) \
    { \
      if ((_X_ - _chunk->x0) <= (_cLast->x1 - _X_)) \
      { \
        for (;;) \
        { \
          if (_X_ < _chunk->x0) \
            break; \
          \
          if (_X_ < _chunk->x1) \
            goto _Name_##_Add_One_Merge; \
          \
          FOG_ASSERT(_chunk != _cLast); \
          _chunk = _chunk->next; \
        } \
      } \
      else \
      { \
        _chunk = _cLast; \
        for (;;) \
        { \
          if (_X_ >= _chunk->x0) \
          { \
_Name_##_Add_One_Merge: \
            LOG_CELL("Add_One", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
            _chunk->cells[_X_ - _chunk->x0].add(_Cover_, _Area_); \
            goto _Name_##_Add_One_End; \
          } \
          \
          _chunk = _chunk->prev; \
          FOG_ASSERT(_chunk != _cLast); \
          \
          if (_X_ >= _chunk->x1) \
            break; \
        } \
        _chunk = _chunk->next; \
      } \
      FOG_ASSERT(_X_ <= _chunk->x0); \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Create]                                                            */ \
    /* ------------------------------------------------------------------- */ \
    \
    _cLast = _chunk->prev; \
    \
    ROW_NEW_CHUNK(_cNew, _X_, 1); \
    _cNew->prev = _cLast; \
    _cNew->next = _chunk; \
    \
    _cLast->next = _cNew; \
    _chunk->prev = _cNew; \
    \
_Name_##_Add_One_Set: \
    LOG_CELL("Add_One", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
    _cNew->cells[0].set(_Cover_, _Area_); \
    \
_Name_##_Add_One_End: \
    ; \
  FOG_MACRO_END \

// ============================================================================
// [Fog::PathRasterizer8 - ROW_ADD_TWO]
// ============================================================================

#define ROW_ADD_TWO(_Name_, _Row_, _X_, _Cover_, _Area_, _Advance_) \
  FOG_MACRO_BEGIN \
    PathRasterizer8::Chunk* _chunk = _Row_->first; \
    PathRasterizer8::Chunk* _cNew; \
    PathRasterizer8::Chunk* _cLast; \
    \
    int _count; \
    \
    /* ------------------------------------------------------------------- */ \
    /* [First]                                                             */ \
    /* ------------------------------------------------------------------- */ \
    \
    if (_chunk == NULL) \
    { \
      ROW_NEW_CHUNK(_chunk, _X_, 2); \
      _chunk->prev = _chunk; \
      _chunk->next = _chunk; \
      \
      _Row_->first = _chunk; \
      \
      LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
      _chunk->cells[0].set(_Cover_, _Area_); \
      \
      _Advance_ \
      \
      LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
      _chunk->cells[1].set(_Cover_, _Area_); \
      \
      goto _Name_##_Add_Two_End; \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Prepend]                                                           */ \
    /* ------------------------------------------------------------------- */ \
    \
    _cLast = _chunk->prev; \
    \
    if (_X_ < _chunk->x0) \
    { \
      _count = Math::min(_chunk->x0 - _X_, 2); \
      FOG_ASSERT(_count > 0); \
      \
      ROW_NEW_CHUNK(_cNew, _X_, _count); \
      _cNew->prev = _cLast; \
      _cNew->next = _chunk; \
      \
      _cLast->next = _cNew; \
      _chunk->prev = _cNew; \
      \
      _Row_->first = _cNew; \
      FOG_ASSERT(_cNew->x1 <= _chunk->x0); \
      \
      goto _Name_##_Add_Two_Set; \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Insert / Append]                                                   */ \
    /* ------------------------------------------------------------------- */ \
    \
    if (_X_ < _cLast->x1) \
    { \
      if ((_X_ - _chunk->x0) <= (_cLast->x1 - _X_)) \
      { \
        for (;;) \
        { \
          if (_X_ < _chunk->x0) \
            break; \
          \
          if (_X_ < _chunk->x1) \
            goto _Name_##_Add_Two_Merge; \
          \
          FOG_ASSERT(_chunk != _cLast); \
          _chunk = _chunk->next; \
        } \
      } \
      else \
      { \
        _chunk = _cLast; \
        for (;;) \
        { \
          if (_X_ >= _chunk->x0) \
          { \
_Name_##_Add_Two_Merge: \
            LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
            _chunk->cells[_X_ - _chunk->x0].add(_Cover_, _Area_); \
            \
            _Advance_ \
            \
            if (_X_ < _chunk->x1) \
            { \
_Name_##_Add_Two_Merge_Second: \
              LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
              _chunk->cells[_X_ - _chunk->x0].add(_Cover_, _Area_); \
              goto _Name_##_Add_Two_End; \
            } \
            \
            _cLast = _chunk; \
            _chunk = _chunk->next; \
            \
            if (_X_ >= _chunk->x0 && _X_ < _chunk->x1) \
              goto _Name_##_Add_Two_Merge_Second; \
            \
            ROW_NEW_CHUNK(_cNew, _X_, 1); \
            _cNew->prev = _cLast; \
            _cNew->next = _chunk; \
            \
            _cLast->next = _cNew; \
            _chunk->prev = _cNew; \
            \
            LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
            _cNew->cells[0].set(_Cover_, _Area_); \
            goto _Name_##_Add_Two_End; \
          } \
          \
          _chunk = _chunk->prev; \
          FOG_ASSERT(_chunk != _cLast); \
          \
          if (_X_ >= _chunk->x1) \
            break; \
        } \
        _chunk = _chunk->next; \
      } \
      FOG_ASSERT(_X_ <= _chunk->x0); \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Create]                                                            */ \
    /* ------------------------------------------------------------------- */ \
    \
    _cLast = _chunk->prev; \
    _count = _chunk->x0 - _X_; \
    \
    if ((uint)_count >= 2) _count = 2; \
    \
    ROW_NEW_CHUNK(_cNew, _X_, _count); \
    _cNew->prev = _cLast; \
    _cNew->next = _chunk; \
    \
    _cLast->next = _cNew; \
    _chunk->prev = _cNew; \
    \
_Name_##_Add_Two_Set: \
    LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
    _cNew->cells[0].set(_Cover_, _Area_); \
    \
    _Advance_ \
    \
    if (_count > 1) \
    { \
      FOG_ASSERT(_cNew->x1 - _cNew->x0 == 2); \
      \
      LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
      _cNew->cells[1].set(_Cover_, _Area_); \
    } \
    else \
    { \
      FOG_ASSERT(_cNew->next == _chunk); \
      FOG_ASSERT(_cNew->x1 <= _chunk->x0); \
      \
      LOG_CELL("Add_Two", _X_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
      _chunk->cells[0].add(_Cover_, _Area_); \
    } \
    \
_Name_##_Add_Two_End: \
    ; \
  FOG_MACRO_END

// ============================================================================
// [Fog::PathRasterizer8 - ROW_ADD_N]
// ============================================================================

#define ROW_ADD_N(_Name_, _Row_, _X0_, _X1_, _Cover_, _Area_, _Advance_, _Last_) \
  FOG_MACRO_BEGIN \
    PathRasterizer8::Chunk* _cFirst = _Row_->first; \
    PathRasterizer8::Chunk* _cLast = NULL; \
    \
    PathRasterizer8::Chunk* _chunk = _cFirst; \
    PathRasterizer8::Cell* _cell; \
    \
    int _stop; \
    \
    /* ------------------------------------------------------------------- */ \
    /* [First]                                                             */ \
    /* ------------------------------------------------------------------- */ \
    \
    if (_chunk == NULL) \
    { \
      int _length = _X1_ - _X0_; \
      \
      if (FOG_UNLIKELY(_length > A8_MAX_CHUNK_LENGTH)) \
        _length = A8_MAX_CHUNK_LENGTH; \
      \
      ROW_NEW_CHUNK(_chunk, _X0_, _length); \
      _chunk->prev = _chunk; \
      _chunk->next = _chunk; \
      \
      _cFirst = _chunk; \
      _cLast = _chunk; \
      \
      _Row_->first = _chunk; \
      \
      _cell = _chunk->cells; \
      _stop = _X0_ + _length; \
      \
      goto _Name_##_Add_N_Fill; \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Prepend]                                                           */ \
    /* ------------------------------------------------------------------- */ \
    \
    _cLast = _chunk->prev; \
    \
    if (_X0_ < _chunk->x0) \
    { \
      int _length = Math::min(_chunk->x0, _X1_) - _X0_; \
      \
      if (FOG_UNLIKELY(_length > A8_MAX_CHUNK_LENGTH)) \
        _length = A8_MAX_CHUNK_LENGTH; \
      \
      PathRasterizer8::Chunk* _cNext = _chunk; \
      ROW_NEW_CHUNK(_chunk, _X0_, _length); \
      \
      _chunk->prev = _cLast; \
      _chunk->next = _cNext; \
      \
      _cLast->next = _chunk; \
      _cNext->prev = _chunk; \
      \
      _cFirst = _chunk; \
      _Row_->first = _chunk; \
      \
      _cell = _chunk->cells; \
      _stop = _X0_ + _length; \
      \
      goto _Name_##_Add_N_Fill; \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Append]                                                            */ \
    /* ------------------------------------------------------------------- */ \
    \
    if (_X0_ >= _cLast->x1) \
    { \
      _chunk = _cLast; \
      \
      goto _Name_##_Add_N_Append; \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Insert]                                                            */ \
    /* ------------------------------------------------------------------- */ \
    \
    if ((_X0_ - _chunk->x0) <= (_cLast->x1 - _X0_)) \
    { \
      for (;;) \
      { \
        if (_X0_ < _chunk->x1) \
          goto _Name_##_Add_N_Merge; \
        \
        _chunk = _chunk->next; \
        \
        /* The list is circular, but we know that the [x0, x1] intersects  */ \
        /* with the bounding-box of the chunk-list. This means that we     */ \
        /* can't skip into the first chunk in this loop.                   */ \
        FOG_ASSERT(_chunk != _cFirst); \
      } \
    } \
    else \
    { \
      _chunk = _cLast; \
      \
      for (;;) \
      { \
        if (_X0_ >= _chunk->x0) \
        { \
          if (_X0_ >= _chunk->x1) \
            _chunk = _chunk->next; \
          \
          goto _Name_##_Add_N_Merge; \
        } \
        \
        /* Again, we can't iterate forever. */ \
        FOG_ASSERT(_chunk != _cFirst); \
        _chunk = _chunk->prev; \
      } \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Advance]                                                           */ \
    /* ------------------------------------------------------------------- */ \
    \
_Name_##_Add_N_Advance: \
    if (_chunk == _cLast) \
    { \
_Name_##_Add_N_Append: \
      int _length = _X1_ - _X0_; \
      \
      if (FOG_UNLIKELY(_length > A8_MAX_CHUNK_LENGTH)) \
        _length = A8_MAX_CHUNK_LENGTH; \
      \
      PathRasterizer8::Chunk* _cPrev = _chunk; \
      \
      ROW_NEW_CHUNK(_chunk, _X0_, _length); \
      _chunk->prev = _cPrev; \
      _chunk->next = _cFirst; \
      \
      _cFirst->prev = _chunk; \
      _cPrev->next = _chunk; \
      \
      _cLast = _chunk; \
      _cell = _chunk->cells; \
      \
      _stop = _X0_ + _length; \
    } \
    else \
    { \
      _chunk = _chunk->next; \
      \
_Name_##_Add_N_Merge: \
      if (_X0_ < _chunk->x0) \
      { \
        int _length = Math::min(_X1_, _chunk->x0) - _X0_; \
        \
        if (FOG_UNLIKELY(_length > A8_MAX_CHUNK_LENGTH)) \
          _length = A8_MAX_CHUNK_LENGTH; \
        \
        PathRasterizer8::Chunk* _cPrev = _chunk->prev; \
        PathRasterizer8::Chunk* _cNext = _chunk; \
        \
        ROW_NEW_CHUNK(_chunk, _X0_, _length); \
        _chunk->prev = _cPrev; \
        _chunk->next = _cNext; \
        \
        _cPrev->next = _chunk; \
        _cNext->prev = _chunk; \
        \
        _cell = _chunk->cells; \
        _stop = _X0_ + _length; \
      } \
      else \
      { \
        FOG_ASSERT(_X0_ < _chunk->x1); \
        \
        _cell = &_chunk->cells[_X0_ - _chunk->x0]; \
        _stop = Math::min(_chunk->x1, _X1_); \
        \
        _cellOp |= CELL_OP_MERGE; \
      } \
    } \
    \
    /* ------------------------------------------------------------------- */ \
    /* [Fill]                                                              */ \
    /* ------------------------------------------------------------------- */ \
    \
_Name_##_Add_N_Fill: \
    { \
      FOG_ASSERT(_X0_ <  _stop); \
      FOG_ASSERT(_X1_ >= _stop); \
      \
      bool _isLast = (_stop == _X1_); \
      _stop -= _isLast; \
      \
      switch (_cellOp) \
      { \
        case CELL_OP_COPY | CELL_OP_POSITIVE: \
          if (_X0_ != _stop) \
          { \
            for (;;) \
            { \
              LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
              _cell->set(_Cover_, _Area_); \
              _cell++; \
              \
              _Advance_ \
              \
              if (++_X0_ == _stop) \
                break; \
            } \
            \
            if (!_isLast) \
              goto _Name_##_Add_N_Advance; \
          } \
          \
          _Last_ \
          \
          LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
          _cell->set(_Cover_, _Area_); \
          break; \
        \
        case CELL_OP_COPY | CELL_OP_NEGATIVE: \
          if (_X0_ != _stop) \
          { \
            for (;;) \
            { \
              LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), -(_Cover_), -(_Area_)); \
              _cell->set(-(_Cover_), -(_Area_)); \
              _cell++; \
              \
              _Advance_ \
              \
              if (++_X0_ == _stop) \
                break; \
            } \
            \
            if (!_isLast) \
              goto _Name_##_Add_N_Advance; \
          } \
          \
          _Last_ \
          \
          LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), -(_Cover_), -(_Area_)); \
          _cell->set(-(_Cover_), -(_Area_)); \
          break; \
        \
        case CELL_OP_MERGE | CELL_OP_POSITIVE: \
          _cellOp &= ~CELL_OP_MERGE; \
          \
          if (_X0_ != _stop) \
          { \
            for (;;) \
            { \
              LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
              _cell->add(_Cover_, _Area_); \
              _cell++; \
              \
              _Advance_ \
              \
              if (++_X0_ == _stop) \
                break; \
            } \
            \
            if (!_isLast) \
              goto _Name_##_Add_N_Advance; \
          } \
          \
          _Last_ \
          \
          LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
          _cell->add(_Cover_, _Area_); \
          break; \
        \
        case CELL_OP_MERGE | CELL_OP_NEGATIVE: \
          _cellOp &= ~CELL_OP_MERGE; \
          \
          if (_X0_ != _stop) \
          { \
            for (;;) \
            { \
              LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
              _cell->sub(_Cover_, _Area_); \
              _cell++; \
              \
              _Advance_ \
              \
              if (++_X0_ == _stop) \
                break; \
            } \
            \
            if (!_isLast) \
              goto _Name_##_Add_N_Advance; \
          } \
          \
          _Last_ \
          \
          LOG_CELL("Add_N", _X0_, (int)(_Row_ - self->_rows), _Cover_, _Area_); \
          _cell->sub(_Cover_, _Area_); \
          break; \
        \
        default: \
          FOG_ASSERT_NOT_REACHED(); \
      } \
    } \
    \
_Name_##_Add_N_End: \
    ; \
  FOG_MACRO_END

template<typename FixedT>
static bool PathRasterizer8_renderLine(PathRasterizer8* self, FixedT x0, FixedT y0, FixedT x1, FixedT y1)
{
  // --------------------------------------------------------------------------
  // [Prepare]
  // --------------------------------------------------------------------------

  FixedT dx = x1 - x0;
  FixedT dy = y1 - y0;
  
  // The rasterizer does nothing in such case.
  if (dy == FixedT(0))
    return true;

  int cover = int(dy);
  int area;

  if (dx < 0)
    dx = -dx;

  if (dy < 0)
    dy = -dy;

  // Instead of subdividing a line to fit into A8_I32_COORD_LIMIT, we use 64-bit
  // integer version of renderLine() to do the job. This situation can happen
  // only when drawing to a screen where one or both dimensions is larger than
  // 16384 pixels.
  if (sizeof(FixedT) < sizeof(int64_t) && (dx >= FixedT(A8_I32_COORD_LIMIT) || dy >= FixedT(A8_I32_COORD_LIMIT)))
    return PathRasterizer8_renderLine<int64_t>(self, int64_t(x0), int64_t(y0), int64_t(x1), int64_t(y1));

  int rInc = 1;
  int coverSign = 1;

  // The new rasterizer algorithm is based on traversing the line from the left
  // to the right. The start and end coordinates can be swapped, but it's needed
  // to change the coverSign in such case.
  if (x0 > x1)
  {
    swap(x0, x1);
    swap(y0, y1);

    coverSign = -1;
  }

  // If we are going from bottom to top then we need to invert the fractional
  // parts of y0/y1. In case that the inversion overflow it's needed to subtract
  // A8_SCALE*2 to normalize the position into the opposite pixel.
  if (y0 > y1)
  {
    static const int norm[2] = { 1, 1 - A8_SCALE * 2};

    y0 ^= A8_MASK;
    y0 += norm[(int(y0) & A8_MASK) == A8_MASK];
    y1  = y0 + dy;

    rInc = -1;
    coverSign = -coverSign;
  }

  // Extract the raster and fractional coordinates.
  int ex0 = int(x0 >> A8_SHIFT);
  int fx0 = int(x0) & A8_MASK;

  int ey0 = int(y0 >> A8_SHIFT);
  int fy0 = int(y0) & A8_MASK;

  int ex1 = int(x1 >> A8_SHIFT);
  int fy1 = int(y1) & A8_MASK;

  // How many Y iterations to do.
  int i = 1;
  // How many Y iterations to do next.
  int j = int(y1 >> A8_SHIFT) - ey0;

  // NOTE: Variable 'i' is just loop counter. Because we need to make sure to
  // handle the start and end points of the line (start point is always based
  // on the y0 coordinate and end point is always based on the y1 coordinate),
  // we need two variables:
  //
  //  - 'i' - Used for current loop.
  //  - 'j' - Used for next loop.

  // Prepare the current and end row pointers.
  PathRasterizer8::Row* rPtr = self->_rows;
  PathRasterizer8::Row* rEnd = self->_rows;

  // --------------------------------------------------------------------------
  // [Bounding-Box]
  // --------------------------------------------------------------------------

  {
    int by0 = ey0;
    int by1 = ey0 + (j - (fy1 == 0)) * rInc;

    if (by0 > by1)
      swap(by0, by1);
    
    if (FOG_UNLIKELY(self->_boundingBox.y0 == -1))
    {
      // Initialize the bounding-box.
      self->_boundingBox.x0 = ex0;
      self->_boundingBox.y0 = by0;
      self->_boundingBox.x1 = ex1;
      self->_boundingBox.y1 = by1;

      // Initialize the rows.
      for (;;)
      {
        rPtr[by0].first = NULL;
        if (by0 == by1)
          break;
        by0++;
      }
    }
    else
    {
      int bEnd;

      // Merge the bounding-box with the line.
      if (ex0 < self->_boundingBox.x0) self->_boundingBox.x0 = ex0;
      if (ex1 > self->_boundingBox.x1) self->_boundingBox.x1 = ex1;

      // Clear the rows (clear).
      bEnd = self->_boundingBox.y0;
      if (by0 < bEnd)
      {
        self->_boundingBox.y0 = by0;
        do {
          rPtr[by0++].first = NULL;
        } while (by0 != bEnd);
      }

      bEnd = self->_boundingBox.y1;
      if (by1 > bEnd)
      {
        self->_boundingBox.y1 = by1;
        do {
          rPtr[++bEnd].first = NULL;
        } while (by1 != bEnd);
      }
    }
  }

  rPtr += ey0;
  rEnd += ey0 + (j + (fy1 != 0)) * rInc;

  // --------------------------------------------------------------------------
  // [Point]
  // --------------------------------------------------------------------------

  // If the line is so small that it covers only one cell in one scanline, we
  // can use this fast-path to return as early as possible from renderLine().
  //
  // This can happen when approximating very small circular arcs (for example
  // small radius when painting rounded rectangle) or when painting an object
  // which is heavily down-scaled.
  if ((j | ((fx0 + int(dx)) > 256)) == 0)
  {
    fx0 = fx0 * 2 + int(dx);
    goto _One;
  }

  // --------------------------------------------------------------------------
  // [Vertical Only]
  // --------------------------------------------------------------------------

  if (dx == 0)
  {
    // We have to calculate start and end cells, and then - the common values
    // of the area and coverage for all cells of the line. We know exactly 
    // there's only one cell per scanline, thus, we don't have to use
    // complicated scanline rasterization.
    if (j > 0)
      cover = (A8_SCALE - fy0) * coverSign;

    fx0 *= 2;
    fy0 = A8_SCALE;

    // Instead of making two versions of this function (one for positive covers
    // and one for negative covers) we just negate the important variables if
    // needed.
    if (coverSign < 0)
    {
      fy0 = -fy0;
      fy1 = -fy1;
    }

    for (;;)
    {
_One:
      area = fx0 * cover;
      do {
        ROW_ADD_ONE(_Vert_Only, rPtr, ex0, cover, area);
        rPtr += rInc;
      } while (--i);

      if (rPtr == rEnd)
        break;

      i = j;
      j = 1;
      cover = fy1;

      if (i > 1)
      {
        cover = fy0;
        i--;
      }
    }
    return true;
  }

  // --------------------------------------------------------------------------
  // [Vertical / Horizontal Orientation]
  // --------------------------------------------------------------------------

  else
  {
    // Only one or two cells are generated per scanline.
    FixedT xErr = -dy / 2, xBase, xLift, xRem, xDlt = dx;
    FixedT yErr = -dx / 2, yBase, yLift, yRem, yDlt = dy;

    xBase = dx * A8_SCALE;
    xLift = xBase / dy;
    xRem  = xBase % dy;

    yBase = dy * A8_SCALE;
    yLift = yBase / dx;
    yRem  = yBase % dx;

    if (j != 0)
    {
      FixedT p = FixedT(A8_SCALE - fy0) * dx;

      xDlt  = p / dy;
      xErr += p % dy;

      fy1 = A8_SCALE;
    }

    if (ex0 != ex1)
    {
      FixedT p = FixedT(A8_SCALE - fx0) * dy;

      yDlt = p / dx;
      yErr += p % dx;
    }

    // ------------------------------------------------------------------------
    // [Vertical Orientation]
    // ------------------------------------------------------------------------

    if (dy >= dx)
    {
      int yAcc = int(y0) + int(yDlt);

      if (coverSign > 0)
      {
        goto _Vert_P_Skip;

        for (;;)
        {
          do {
            xDlt = xLift;
            xErr += xRem;
            if (xErr >= 0) { xErr -= dy; xDlt++; }

_Vert_P_Skip:
            area = fx0;
            fx0 += int(xDlt);

            if (fx0 <= 256)
            {
              cover = fy1 - fy0; // Positive.
              area += fx0;

_Vert_P_Single:
              area *= cover;
              ROW_ADD_ONE(_Vert_P, rPtr, ex0, cover, area);

              if (fx0 == 256)
              {
                ex0++;
                fx0 = 0;
                goto _Vert_P_Advance;
              }
            }
            else
            {
              yAcc &= 0xFF;
              FOG_ASSERT(yAcc >= fy0 && yAcc <= fy1);
              
              cover = yAcc - fy0; // Positive.
              fx0  &= A8_MASK;

              // Improve the count of generated cells in case that the resulting
              // cover is zero using the 'ROW_ADD_ONE'. The 'goto' ensures that
              // the ROW_ADD_ONE() macro will be expanded only once.
              if (cover == 0)
              {
                cover = fy1 - yAcc; // Positive.
                area  = fx0;
                ex0++;
                goto _Vert_P_Single;
              }
              else
              {
                area = (area + A8_SCALE) * cover;
                ROW_ADD_TWO(_Vert_P, rPtr, ex0, cover, area,
                {
                  cover = fy1 - yAcc; // Positive.
                  area  = fx0 * cover;
                  ex0++;
                });
              }

_Vert_P_Advance:
              yAcc += int(yLift);
              yErr += yRem;
              if (yErr >= 0) { yErr -= dx; yAcc++; }
            }

            rPtr += rInc;
          } while (--i);

          if (rPtr == rEnd)
            break;

          i = j;
          j = 1;

          if (i > 1)
          {
            fy0 = 0;
            fy1 = A8_SCALE;
            i--;
          }
          else
          {
            fy0 = 0;
            fy1 = int(y1) & A8_MASK;

            xDlt = x1 - (ex0 << 8) - fx0;
            goto _Vert_P_Skip;
          }
        }
      }
      else
      {
        goto _Vert_N_Skip;

        for (;;)
        {
          do {
            xDlt = xLift;
            xErr += xRem;
            if (xErr >= 0) { xErr -= dy; xDlt++; }

_Vert_N_Skip:
            area = fx0;
            fx0 += int(xDlt);

            if (fx0 <= 256)
            {
              cover = fy0 - fy1; // Negative.
              area += fx0;

_Vert_N_Single:
              area *= cover;
              ROW_ADD_ONE(_Vert_N, rPtr, ex0, cover, area);

              if (fx0 == 256)
              {
                ex0++;
                fx0 = 0;
                goto _Vert_N_Advance;
              }
            }
            else
            {
              yAcc &= 0xFF;
              FOG_ASSERT(yAcc >= fy0 && yAcc <= fy1);

              cover = fy0 - yAcc; // Negative.
              fx0  &= A8_MASK;

              // Improve the count of generated cells in case that the resulting
              // cover is zero using the 'ROW_ADD_ONE'. The 'goto' ensures that
              // the ROW_ADD_ONE() macro will be expanded only once.
              if (cover == 0)
              {
                cover = yAcc - fy1; // Negative.
                area  = fx0;
                ex0++;
                goto _Vert_N_Single;
              }
              else
              {
                area = (area + A8_SCALE) * cover;
                ROW_ADD_TWO(_Vert_N, rPtr, ex0, cover, area,
                {
                  cover = yAcc - fy1; // Negative.
                  area  = fx0 * cover;
                  ex0++;
                });
              }

_Vert_N_Advance:
              yAcc += int(yLift);
              yErr += yRem;
              if (yErr >= 0) { yErr -= dx; yAcc++; }
            }

            rPtr += rInc;
          } while (--i);

          if (rPtr == rEnd)
            break;

          i = j;
          j = 1;

          if (i > 1)
          {
            fy0 = 0;
            fy1 = A8_SCALE;
            i--;
          }
          else
          {
            fy0 = 0;
            fy1 = int(y1) & A8_MASK;
            xDlt = x1 - (ex0 << 8) - fx0;
            goto _Vert_N_Skip;
          }
        }
      }

      return true;
    }

    // ------------------------------------------------------------------------
    // [Horizontal Orientation]
    // ------------------------------------------------------------------------

    else
    {
      // Cell operator, directly used/modified by ROW_ADD_N() macro.
      int _cellOp = coverSign > 0 ? CELL_OP_POSITIVE : CELL_OP_NEGATIVE;

      int fx1;
      int coverAcc = fy0;

      cover = int(yDlt);
      coverAcc += cover;

      if (j != 0)
        fy1 = A8_SCALE;

      if (fx0 + int(xDlt) > 256)
        goto _Horz_Inside;

      x0 += xDlt;

      cover = (fy1 - fy0) * coverSign;
      area = (fx0 * 2 + int(xDlt)) * cover;

_Horz_Single:
      ROW_ADD_ONE(_Horz, rPtr, ex0, cover, area);
      
      rPtr += rInc;
      if (rPtr == rEnd)
        return true;
      
      if (fx0 + int(xDlt) == 256)
      {
        coverAcc += int(yLift);
        yErr += yRem;
        if (yErr >= 0) { yErr -= dx; coverAcc++; }
      }

      if (--i == 0)
        goto _Horz_After;

      for (;;)
      {
        do {
          xDlt = xLift;
          xErr += xRem;
          if (xErr >= 0) { xErr -= dy; xDlt++; }

          ex0 = int(x0 >> A8_SHIFT);
          fx0 = int(x0) & A8_MASK;

_Horz_Skip:
          coverAcc -= 256;
          cover = coverAcc;
          FOG_ASSERT(cover >= 0 && cover <= 256);

_Horz_Inside:
          x0 += xDlt;

          ex1 = int(x0 >> A8_SHIFT);
          fx1 = int(x0) & A8_MASK;

          FOG_ASSERT(ex0 != ex1);

          if (fx1 == 0)
            fx1 = A8_SCALE;
          else
            ex1++;

          // Calculate the first cover/area pair. All cells inside the line
          // will be calculated inside ROW_ADD_N() loop.
          area = (fx0 + A8_SCALE) * cover;

          ROW_ADD_N(_Horz, rPtr, ex0, ex1, cover, area,
          { /* Advance: */
            cover = int(yLift);
            yErr += yRem;
            if (yErr >= 0) { yErr -= dx; cover++; }

            coverAcc += cover;
            area = A8_SCALE * cover;
          },
          { /* Last: */
            cover += fy1 - coverAcc;
            area = fx1 * cover;

            if (fx1 == A8_SCALE)
            {
              coverAcc += int(yLift);
              yErr += yRem;
              if (yErr >= 0) { yErr -= dx; coverAcc++; }
            }
          });

          rPtr += rInc;
        } while (--i);

        if (rPtr == rEnd)
          break;

_Horz_After:
        i = j;
        j = 1;

        if (i > 1)
        {
          fy1 = A8_SCALE;
          i--;
        }
        else
        {
          fy1 = int(y1) & A8_MASK;
          xDlt = x1 - x0;

          ex0 = int(x0 >> A8_SHIFT);
          fx0 = int(x0) & A8_MASK;

          if (fx0 + int(xDlt) <= 256)
          {
            cover = fy1 * coverSign;
            area = (fx0 * 2 + int(xDlt)) * cover;
            goto _Horz_Single;
          }
          else
          {
            goto _Horz_Skip;
          }
        }
      }

      return true;
    }
  }

_Bail:
  return false;
}

// ============================================================================
// [Fog::PathRasterizer8 - Render - Helpers]
// ============================================================================

template<int _RULE, int _USE_ALPHA>
static FOG_INLINE uint32_t PathRasterizer8_calculateAlpha(const PathRasterizer8* self, int cover)
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

    VERIFY_CHUNKS_8(first);

    // ------------------------------------------------------------------------
    // [Fetch]
    // ------------------------------------------------------------------------

    scanline->begin();

    PathRasterizer8::Chunk* chunk = first;
    PathRasterizer8::Cell* cell = chunk->cells;

    uint i = (uint)chunk->getLength();

    int x;
    int xNext = chunk->x0;
    int cover = 0;
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
        cover += cell->cover;
        alpha  = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover - (cell->area >> A8_SHIFT_2));

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
      xNext = chunk->x0;
      i = (uint)chunk->getLength();

      if (x == xNext)
        goto _Continue;
      scanline->endA8Extra();

      alpha = PathRasterizer8_calculateAlpha<_RULE, _USE_ALPHA>(self, cover);
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
  xNext = chunk->x0; \
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
        xNext = chunk->x0; \
        FOG_ASSERT(x != xNext); \
      } \
      else \
      { \
        xNext++; \
      } \
      break; \
    } \
  FOG_MACRO_END

























// TODO: This should be really completed early.

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
  if (_error != ERR_OK || _isFinalized)
    return _error;

  if (_boundingBox.y0 == -1)
    goto _NotValid;

  // Translate bounding box to match _sceneBox.
  _boundingBox.translate(0, _sceneBox.y0);
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
