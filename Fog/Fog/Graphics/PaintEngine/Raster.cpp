// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Atomic.h>
#include <Fog/Core/AutoLock.h>
#include <Fog/Core/AutoUnlock.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/CpuInfo.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/OS.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Core/ThreadPool.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/Constants_p.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Glyph.h>
#include <Fog/Graphics/GlyphSet.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/PaintEngine/Raster_p.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Graphics/Rasterizer_p.h>
#include <Fog/Graphics/RasterEngine_p.h>
#include <Fog/Graphics/RasterEngine/C_p.h>
#include <Fog/Graphics/Scanline_p.h>
#include <Fog/Graphics/Stroker.h>

namespace Fog {

// ============================================================================
// [Fog::BlockAllocator]
// ============================================================================

BlockAllocator::BlockAllocator() :
  blocks(NULL)
{
}

BlockAllocator::~BlockAllocator()
{
  reset();
}

void* BlockAllocator::alloc(sysuint_t size)
{
  size += sizeof(Header);

  FOG_ASSERT(size <= BLOCK_SIZE);

  if (FOG_UNLIKELY(!blocks || (blocks->size - blocks->pos) < size))
  {
    // Traverse to previous blocks and try to find complete free one.
    Block* cur = blocks;
    Block* prev = NULL;

    while (cur)
    {
      if (cur->used.get() == 0)
      {
        // Make it first.
        if (prev)
        {
          prev->next = cur->next;
          cur->next = blocks;
          blocks = cur;
        }

        cur->pos = 0;
        goto allocFromBlocks;
      }

      prev = cur;
      cur = cur->next;
    }

    // If we are here, it's needed to allocate new chunk of memory.
    cur = (Block*)Memory::alloc(sizeof(Block));
    if (!cur) return NULL;

    cur->size = BLOCK_SIZE;
    cur->pos = 0;
    cur->used.init(0);

    cur->next = blocks;
    blocks = cur;
  }

allocFromBlocks:
  Header* header = reinterpret_cast<Header*>(blocks->memory + blocks->pos);

  header->block = blocks;
  header->size = size;

  blocks->pos += size;
  blocks->used.add(size);

  return reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(header) + sizeof(Header));
}

void BlockAllocator::reset()
{
  Block* cur = blocks;
  blocks = NULL;

  while (cur)
  {
    // It's failure if Block::used is not zero when reseting.
    FOG_ASSERT(cur->used.get() == 0);

    Block* next = cur->next;
    Memory::free(cur);
    cur = next;
  }
}

// ============================================================================
// [Fog::ZoneAllocator]
// ============================================================================

ZoneAllocator::ZoneAllocator(sysuint_t chunkSize)
  : _chunkSize(chunkSize)
{
  // We need almost one chunk.
  _chunks = reinterpret_cast<Chunk*>(Memory::alloc(sizeof(Chunk) - sizeof(void*) + _chunkSize));
  _chunks->prev = NULL;
  _chunks->pos = 0;
  _chunks->size = _chunkSize;

  // TODO: We need almost one chunk, but memory allocation can fail, what to do?
  FOG_ASSERT(_chunks != NULL);
}

ZoneAllocator::~ZoneAllocator()
{
  reset();
  if (_chunks) Memory::free(_chunks);
}

void* ZoneAllocator::_alloc(sysuint_t size)
{
  Chunk* cur = _chunks;

  if (cur->getRemainingBytes() < size)
  {
    // This allocator wasn't designed to alloc huge amount of memory (larger
    // than the chunk size), so never do it!
    FOG_ASSERT(_chunkSize < size);

    cur = reinterpret_cast<Chunk*>(Memory::alloc(sizeof(Chunk) - sizeof(void*) + _chunkSize));
    if (!cur) return NULL;

    cur->prev = _chunks;
    cur->pos = 0;
    cur->size = _chunkSize;
    _chunks = cur;
  }

  uint8_t* p = cur->data + cur->pos;
  cur->pos += size;
  return (void*)p;
}

void ZoneAllocator::reset()
{
  Chunk* cur = _chunks;
  if (!cur) return;

  _chunks->pos = 0;
  _chunks->prev = NULL;

  cur = cur->prev;
  while (cur)
  {
    Chunk* prev = cur->prev;
    Memory::free(cur);
    cur = prev;
  }
}

// ============================================================================
// [Utils]
// ============================================================================

static FOG_INLINE int alignToDelta(int y, int offset, int delta)
{
  FOG_ASSERT(offset >= 0);
  FOG_ASSERT(delta >= 1);
  FOG_ASSERT(offset < delta);

  int mody = y % delta;
  if (mody < 0) mody += delta;

  int newy = y - (mody) + offset;
  if (newy < y) newy += delta;

  return newy;
}

// ============================================================================
// [Fog::RasterClipBuffer]
// ============================================================================

RasterClipBuffer::RasterClipBuffer()
{
  refCount.init(1);
  rows = NULL;

  box.clear();
  clip.clear();
}

RasterClipBuffer::~RasterClipBuffer()
{
  if (rows) Memory::free(rows);
}

err_t RasterClipBuffer::create(int w, int h, uint32_t format)
{
  err_t err;
  if ((err = reallocRows(h)) != ERR_OK) goto fail;

  box.set(0, 0, w, h);
  clip.clear();

  if ((err = image.create(w, h, format)) != ERR_OK) goto fail;
  return ERR_OK;

fail:
  free();
  return err;
}

err_t RasterClipBuffer::adopt(Image& other)
{
  int w = other.getWidth();
  int h = other.getHeight();

  if (w <= 0 || h <= 0)
  {
    free();
    return ERR_RT_INVALID_ARGUMENT;
  }

  err_t err;
  if ((err = reallocRows(h)) != ERR_OK) goto fail;
  if ((err = image.set(other)) != ERR_OK) goto fail;

  return ERR_OK;

fail:
  free();
  return err;
}

err_t RasterClipBuffer::reallocRows(int h)
{
  if (box.getHeight() == h)
  {
    Memory::zero(rows + box.y1, box.getHeight() * sizeof(Row));
  }
  else
  {
    if (rows != NULL) Memory::free(rows);
    if ((rows = (Row*)Memory::calloc((uint)h * sizeof(Row))) == NULL) return ERR_RT_OUT_OF_MEMORY;
  }
  return ERR_OK;
}

void RasterClipBuffer::free()
{
  image.free();
  if (rows) { Memory::free(rows); rows = NULL; }

  box.clear();
  clip.clear();
}


// ============================================================================
// [Fog::RasterPaintContext]
// ============================================================================

RasterPaintContext::RasterPaintContext()
{
  engine = NULL;

  id = -1;
  offset = 0;
  delta = 1;

  ops.op = OPERATOR_SRC_OVER;
  ops.sourceType = PAINTER_SOURCE_ARGB;
  ops.clipType = 0;
  ops.reserved = 0;

  solid.argb = 0xFF000000;
  solid.prgb = 0xFF000000;

  pctx = NULL;
  funcs = NULL;

  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  metaOrigin.clear();
  workOrigin.clear();

  clipBox.clear();

  buffer = NULL;
  bufferSize = 0;

  // Scanline must be reset before using it.
  scanline.reset();
}

RasterPaintContext::~RasterPaintContext()
{
  if (buffer != NULL) Memory::free(buffer);
}

uint8_t* RasterPaintContext::reallocBuffer(sysuint_t size)
{
  // Use RasterPaintContext::getBuffer() that will return buffer if it's 
  // already allocated for a needed size.
  FOG_ASSERT(size > bufferSize);

  size = (size < 4096) ? (4096) : ((size + 1023) & ~1023);

  if (buffer) Memory::free(buffer);
  buffer = (uint8_t*)Memory::alloc(size);
  bufferSize = buffer ? size : 0;

  return buffer;
}

// ============================================================================
// [Fog::RasterPaintMainContext]
// ============================================================================

RasterPaintMainContext::RasterPaintMainContext()
{
  hints.data = 0;
  changed = 0;
  userOrigin.clear();
  metaOrigin.clear();
  workOrigin.clear();
  workTranslate.clear();
  approximationScale = 1.0;
}

RasterPaintMainContext::~RasterPaintMainContext()
{
}

// ============================================================================
// [Fog::RasterRenderImageAffineBound]
// ============================================================================

bool RasterRenderImageAffineBound::init(
  const Image& image, const IntRect& irect,
  const DoubleMatrix& matrix,
  const IntBox& clipBox, uint32_t interpolationType)
{
  // Don't call init() after it was initialized.
  FOG_ASSERT(ictx.initialized == false);

  uint i;

  // Transform points and convert to integers.
  {
    double w = (double)irect.w;
    double h = (double)irect.h;

    pts[0].set(0.0, 0.0);
    pts[1].set(w+1, 0.0);
    pts[2].set(w+1, h+1);
    pts[3].set(0.0, h+1);

    matrix.transformPoints(pts, pts, 4);

    // Calculate min/max and top/bottom point indexes.
    leftStart = 0;
    rightStart = 0;

    double xmind = pts[0].x;
    double ymind = pts[0].y;
    double xmaxd = pts[0].x;
    double ymaxd = pts[0].y;

    for (i = 1; i < 4; i++)
    {
      double x = pts[i].x;
      double y = pts[i].y;

      if (x < xmind) { xmind = x; }
      if (y < ymind) { ymind = y; leftStart = i; }

      if (x > xmaxd) { xmaxd = x; }
      if (y > ymaxd) { ymaxd = y; }
    }

    for (i = 0; i < 4; i++) pty[i] = (int)floor(pts[i].y);

    xmin = (int)floor(xmind);
    ymin = (int)floor(ymind);
    xmax = (int)floor(xmaxd);
    ymax = (int)floor(ymaxd);
  }

  // Save ymin to ytop, ymin may be overriden by bounding box intersection and
  // we need top most vertex Y to be saved.
  ytop = ymin;

  // Now get left and right direction. If left and right vertexes are not the
  // same we need to find and assign them.
  rightStart = leftStart;

  int neighbourIndex;
  int neighbourDir;

  if ((pty[(neighbourIndex = (leftStart + (neighbourDir = -1)) & 3)] == ymin) ||
      (pty[(neighbourIndex = (leftStart + (neighbourDir =  1)) & 3)] == ymin) )
  {
    if (pts[neighbourIndex].x < pts[leftStart].x)
    {
      leftStart = neighbourIndex;
      leftDirection = neighbourDir;

      rightDirection = -leftDirection;
    }
    else
    {
      rightStart = neighbourIndex;
      rightDirection = neighbourDir;

      leftDirection = -rightDirection;
    }
  }
  else
  {
    leftDirection = -1;
    rightDirection = 1;

    if (pts[(leftStart - 1) & 3].x > pts[(leftStart + 1) & 3].x)
    {
      leftDirection = 1;
      rightDirection = -1;
    }
  }

  // Get bounding box and clip.
  IntBox bbox;
  bbox.x1 = (int)xmin;
  bbox.y1 = (int)ymin;
  bbox.x2 = (int)xmax + 1;
  bbox.y2 = (int)ymax + 1;

  // Intersect bounding box with a given clip box, returning false if there is
  // no intersection.
  if (!IntBox::intersect(bbox, bbox, clipBox)) return false;

  // Fix ymin/ymax and xmin/xmax.
  xmin = bbox.x1;
  ymin = bbox.y1;

  xmax = bbox.x2;
  ymax = bbox.y2;

  return RasterEngine::functionMap->pattern.texture_init_blit(&ictx, 
    image, irect, matrix, SPREAD_NONE, interpolationType) == ERR_OK;
}

void RasterRenderImageAffineBound::render(RasterPaintContext* ctx)
{
  // Rasterize transformed rectangle without antialiasing, but with correct
  // bounding box.
  //
  // How rasterization works? A given rectangle is transformed using affine
  // matrix to screen coordinates. Affine transform assumption is that two
  // opposite rectangle lines share same angle, so we can use these rules
  // in this rasterizer.
  //
  // Advantages:
  //
  //   1. Unlike other rasterizers, sorting is not needed here.
  //   2. It's pretty fast and it can run in parallel.
  //
  // Disadventages:
  //
  //   1. It uses floating point math at this time.
  //
  // If there is better solution, please contact me!

  // Create new pattern context (based on the image).
  uint8_t* pBuf = ctx->getBuffer((uint)(xmax - xmin) * 4);
  if (!pBuf) return;

  // Rasterize it using dda algorithm that matches image boundary with
  // antialiasing.
  //
  // (We don't need to antialias here, because compositing operator is bound so
  // the pattern fetched will be already antialiased - using SPREAD_NONE).

  // Current vertices (index).
  int iLeft = leftStart;
  int iRight = rightStart;

  // Current Y.
  int y = ytop;

  // Dest Y.
  int y2LeftAligned = y;
  int y2RightAligned = y;

  double y1Left = pts[leftStart].y;
  double y1Right = pts[rightStart].y;

  double y2Left = y1Left;
  double y2Right = y1Right;

  // Current left/right X.
  double x1Left = pts[leftStart].x;
  double x1Right = pts[rightStart].x;

  // Dest left/right X.
  double x2Left = x1Left;
  double x2Right = x1Right;

  double dxLeft;
  double dxRight;

  double dyLeft;
  double dyRight;

  double slopeLeft;
  double slopeRight;

  bool reconfigureLeft = true;
  bool reconfigureRight = true;

  // Painting pointers / structures.
  sysint_t stride = ctx->layer.stride;
  uint8_t* pBase = ctx->layer.pixels;
  uint8_t* pCur = NULL;

  RasterEngine::VSpanFn vspan = ctx->funcs->vspan[(uint)ictx.format];

  RasterEngine::Closure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

// TODO
// #define RENDER_LOOP()

  // Singlethreaded.
  if (ctx->id == -1)
  {
    pCur = pBase + (sysint_t)ymin * stride;

    for (;;)
    {
      // Reconfigure left line.
      if (reconfigureLeft)
      {
        // Advance current left point.
        iLeft += leftDirection;
        iLeft &= 3;

        // Advance.
        x1Left = x2Left;
        y1Left = y2Left;

        x2Left = pts[iLeft].x;
        y2Left = pts[iLeft].y;

        y2LeftAligned = pty[iLeft] + 1;

        // Calculate dda.
        dxLeft = (x2Left - x1Left);
        dyLeft = (y2Left - y1Left);
        slopeLeft = dyLeft > Math::DEFAULT_DOUBLE_EPSILON ? (dxLeft / dyLeft) : 0.0;

        x1Left -= Math::abs(slopeLeft) + 0.5;
        x1Left += slopeLeft * ((double)y - y1Left);
      }

      // Reconfigure right line.
      if (reconfigureRight)
      {
        iRight += rightDirection;
        iRight &= 3;

        // Advance.
        x1Right = x2Right;
        y1Right = y2Right;

        x2Right = pts[iRight].x;
        y2Right = pts[iRight].y;

        y2RightAligned = pty[iRight] + 1;

        // Calculate dda.
        dxRight = (x2Right - x1Right);
        dyRight = (y2Right - y1Right);
        slopeRight = dyRight > Math::DEFAULT_DOUBLE_EPSILON ? (dxRight / dyRight) : 0.0;

        x1Right += Math::abs(slopeRight) + 1.5;
        x1Right += slopeRight * ((double)y - y1Right);
      }

      // Clip (in case that this is first iteration).
      if (y < ymin)
      {
        int maxSkipToY = Math::min(y2LeftAligned, y2RightAligned, ymin);
        int deltaY = maxSkipToY - y;
        if (deltaY < 0) break;

        // Advance.
        y += deltaY;
        if (y >= ymax) break;
        x1Left += slopeLeft * deltaY;
        x1Right += slopeRight * deltaY;

        reconfigureLeft = (y == y2LeftAligned);
        reconfigureRight = (y == y2RightAligned);

        if (y == ymin) pCur = pBase + (sysint_t)y * stride;
        continue;
      }
      else if (y >= ymax)
      {
        break;
      }

      // Fetch image scanlines and composite them with target buffer.
      int yStop = Math::min(y2LeftAligned, y2RightAligned, ymax);

      for (;;)
      {
        int xiLeft = (int)x1Left;
        int xiRight = (int)x1Right;

        if (xiLeft < xmin) xiLeft = xmin;
        if (xiRight > xmax) xiRight = xmax;

        if (xiLeft < xiRight)
        {
          int w = xiRight - xiLeft;
          // Fetch pattern and composite.
          vspan(pCur + (uint)xiLeft * 4,
            ictx.fetch(&ictx, pBuf, xiLeft, y, w),
            (sysuint_t)w, &closure);
        }

        x1Left += slopeLeft;
        x1Right += slopeRight;

        pCur += stride;
        if (++y >= yStop) break;
      }
      if (y >= ymax) break;

      // Set flags for reconfiguring.
      reconfigureLeft = (yStop == y2LeftAligned);
      reconfigureRight = (yStop == y2RightAligned);
    }
  }
  // Multithreaded.
  else
  {
    int offset = ctx->offset;
    int delta = ctx->delta;

    int ymin = alignToDelta(this->ymin, offset, delta);
    if (ymin >= ymax) return;

    double slopeLeftWithDelta;
    double slopeRightWithDelta;

    sysint_t strideWithDelta = stride * delta;
    pCur = pBase + (sysint_t)ymin * stride;

    for (;;)
    {
      // Reconfigure left line.
      if (reconfigureLeft)
      {
        // Advance current left point.
        iLeft += leftDirection;
        iLeft &= 3;

        // Advance.
        x1Left = x2Left;
        y1Left = y2Left;

        x2Left = pts[iLeft].x;
        y2Left = pts[iLeft].y;

        y2LeftAligned = pty[iLeft] + 1;

        // Calculate dda.
        dxLeft = (x2Left - x1Left);
        dyLeft = (y2Left - y1Left);
        slopeLeft = dyLeft > Math::DEFAULT_DOUBLE_EPSILON ? (dxLeft / dyLeft) : 0.0;
        slopeLeftWithDelta = slopeLeft * delta;

        x1Left -= Math::abs(slopeLeft) + 0.5;
        x1Left += slopeLeft * ((double)y - y1Left);
      }

      // Reconfigure right line.
      if (reconfigureRight)
      {
        iRight += rightDirection;
        iRight &= 3;

        // Advance.
        x1Right = x2Right;
        y1Right = y2Right;

        x2Right = pts[iRight].x;
        y2Right = pts[iRight].y;

        y2RightAligned = pty[iRight] + 1;

        // Calculate dda.
        dxRight = (x2Right - x1Right);
        dyRight = (y2Right - y1Right);
        slopeRight = dyRight > Math::DEFAULT_DOUBLE_EPSILON ? (dxRight / dyRight) : 0.0;
        slopeRightWithDelta = slopeRight * delta;

        x1Right += Math::abs(slopeRight) + 1.5;
        x1Right += slopeRight * ((double)y - y1Right);
      }

      // Clip.
      if (y < ymin)
      {
        int maxSkipToY = Math::min(y2LeftAligned, y2RightAligned, ymin);
        int deltaY = maxSkipToY - y;
        if (deltaY < 0) break;

        // Advance.
        y += deltaY;
        if (y >= ymax) break;

        x1Left += slopeLeft * deltaY;
        x1Right += slopeRight * deltaY;

        reconfigureLeft = (y == y2LeftAligned);
        reconfigureRight = (y == y2RightAligned);

        if (y == ymin)
        {
          pCur = pBase + (sysint_t)y * stride;
        }
        continue;
      }
      else if (y >= ymax)
      {
        break;
      }

      // Fetch image scanlines and composite them with target buffer.
      int yStop = Math::min(y2LeftAligned, y2RightAligned, ymax);
      for (;;)
      {
        int xiLeft = (int)x1Left;
        int xiRight = (int)x1Right;

        if (xiLeft < xmin) xiLeft = xmin;
        if (xiRight > xmax) xiRight = xmax;

        if (xiLeft < xiRight)
        {
          int w = xiRight - xiLeft;
          // Fetch pattern and composite.
          vspan(pCur + (uint)xiLeft * 4,
            ictx.fetch(&ictx, pBuf, xiLeft, y, w),
            (sysuint_t)w, &closure);
        }

        x1Left += slopeLeftWithDelta;
        x1Right += slopeRightWithDelta;

        pCur += strideWithDelta;
        y += delta;
        if (y >= yStop) break;
      }
      if (y >= ymax) break;

      // Go back if we traversed out of the vertex.
      yStop = y - yStop;

      ymin = y;
      y -= yStop;

      // Set flags for reconfiguring.
      reconfigureLeft = (yStop == y2LeftAligned);
      reconfigureRight = (yStop == y2RightAligned);

      if (yStop)
      {
        x1Left -= slopeLeft * yStop;
        x1Right -= slopeRight * yStop;
      }
    }
  }
}

// ============================================================================
// [Fog::RasterPaintCmdLayerChange]
// ============================================================================

void RasterPaintCmdLayerChange::run(RasterPaintContext* ctx)
{
  ctx->layer = layer;
}

void RasterPaintCmdLayerChange::release(RasterPaintContext* ctx)
{
}

// ============================================================================
// [Fog::RasterPaintCmdRegionChange]
// ============================================================================

void RasterPaintCmdRegionChange::run(RasterPaintContext* ctx)
{
  ctx->metaOrigin = metaOrigin;
  ctx->metaRegion = metaRegion;

  ctx->workOrigin = workOrigin;
  ctx->workRegion = workRegion;
}

void RasterPaintCmdRegionChange::release(RasterPaintContext* ctx)
{
}

// ============================================================================
// [Fog::RasterPaintCmdBoxes]
// ============================================================================

void RasterPaintCmdBoxes::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_renderBoxes(ctx, boxes, count);
  _afterPaint(ctx);
}

void RasterPaintCmdBoxes::release(RasterPaintContext* ctx)
{
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCmdImage]
// ============================================================================

void RasterPaintCmdImage::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_renderImage(ctx, dst, image.instance(), src);
  _afterPaint(ctx);
}

void RasterPaintCmdImage::release(RasterPaintContext* ctx)
{
  image.destroy();
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCmdImageAffineBound]
// ============================================================================

void RasterPaintCmdImageAffineBound::run(RasterPaintContext* ctx)
{
  _beforeBlit(ctx);
  renderer.instance().render(ctx);
}

void RasterPaintCmdImageAffineBound::release(RasterPaintContext* ctx)
{
  renderer.destroy();
}

// ============================================================================
// [Fog::RasterPaintCmdGlyphSet]
// ============================================================================

void RasterPaintCmdGlyphSet::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_renderGlyphSet(ctx, pt, glyphSet.instance(), boundingBox);
  _afterPaint(ctx);
}

void RasterPaintCmdGlyphSet::release(RasterPaintContext* ctx)
{
  glyphSet.destroy();
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCmdPath]
// ============================================================================

void RasterPaintCmdPath::run(RasterPaintContext* ctx)
{
  _beforePaint(ctx);
  ctx->engine->_renderPath(ctx, ras);
  _afterPaint(ctx);
}

void RasterPaintCmdPath::release(RasterPaintContext* ctx)
{
  Rasterizer::releaseRasterizer(ras);
  _releasePattern(ctx);
}

// ============================================================================
// [Fog::RasterPaintCalcFillPath]
// ============================================================================

void RasterPaintCalcFillPath::run(RasterPaintContext* ctx)
{
  RasterPaintCmdPath* cmd = reinterpret_cast<RasterPaintCmdPath*>(relatedTo);
  Rasterizer* ras = Rasterizer::getRasterizer();
  if (ras == NULL) { cmd->status.set(RASTER_COMMAND_SKIP); return; }

  bool noTransform = (
    transformType == RASTER_TRANSFORM_EXACT &&
    Math::feq(matrix->tx, 0.0) &&
    Math::feq(matrix->ty, 0.0) == 0);

  DoublePath dst;
  path->flattenTo(dst, noTransform ? NULL : matrix.instancep(), approximationScale);

  ras->reset();
  ras->setClipBox(clipBox);
  ras->setFillRule(fillMode);

  ras->addPath(dst);
  ras->finalize();

  cmd->ras = ras;
  cmd->status.set(ras->hasCells() ? RASTER_COMMAND_READY : RASTER_COMMAND_SKIP);
}

void RasterPaintCalcFillPath::release(RasterPaintContext* ctx)
{
  path.destroy();
}

// ============================================================================
// [Fog::RasterPaintCalcStrokePath]
// ============================================================================

void RasterPaintCalcStrokePath::run(RasterPaintContext* ctx)
{
  RasterPaintCmdPath* cmd = reinterpret_cast<RasterPaintCmdPath*>(relatedTo);
  Rasterizer* ras = Rasterizer::getRasterizer();
  if (ras == NULL) { cmd->status.set(RASTER_COMMAND_SKIP); return; }

  bool noTransform = (
    transformType == RASTER_TRANSFORM_EXACT &&
    Math::feq(matrix->tx, 0.0) &&
    Math::feq(matrix->ty, 0.0) == 0);

  DoublePath dst;
  stroker->stroke(dst, path.instance());

  if (transformType >= RASTER_TRANSFORM_AFFINE)
    dst.applyMatrix(matrix.instance());
  else if (!noTransform)
    dst.translate(matrix->tx, matrix->ty);

  ras->reset();
  ras->setClipBox(clipBox);

  // Stroke not respects fill mode set in caps
  // state, instead we are using FILL_NON_ZERO.
  ras->setFillRule(FILL_NON_ZERO);

  ras->addPath(dst);
  ras->finalize();

  cmd->ras = ras;
  cmd->status.set(ras->hasCells() ? RASTER_COMMAND_READY : RASTER_COMMAND_SKIP);
}

void RasterPaintCalcStrokePath::release(RasterPaintContext* ctx)
{
  path.destroy();
  stroker.destroy();
}

// ============================================================================
// [Fog::RasterPaintTask]
// ============================================================================

RasterPaintTask::RasterPaintTask(Lock* lock) :
  cond(lock)
{
  state.init(RUNNING);
  shouldQuit.init(false);

  calcCurrent = 0;
  cmdCurrent = 0;
}

RasterPaintTask::~RasterPaintTask()
{
}

void RasterPaintTask::run()
{
  RasterPaintEngine* engine = ctx.engine;
  RasterWorkerManager* mgr = engine->workerManager;

  int id = ctx.id;
  int delta = ctx.delta;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter[Worker #%d]::run() - ThreadID=%d", ctx.id, Thread::getCurrent()->getId());
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  for (;;)
  {
    // Do calculations and commands.
    int cont = 0;
    bool wait = false;

    do {
      wait = false;

      // Do calculations (highest priority than commands).
      if (calcCurrent < mgr->calcPosition)
      {
        // Here is high thread concurrency and we are doing this without locking,
        // atomic operations helps us to get RasterPaintCalc* or NULL. If we get NULL,
        // it's being calculated or done.
        RasterPaintCalc** pclc = (RasterPaintCalc**)&mgr->calcData[calcCurrent];
        RasterPaintCalc* clc = *pclc;

        if (clc && AtomicOperation<RasterPaintCalc*>::cmpXchg(pclc, clc, NULL))
        {
#if defined(FOG_DEBUG_RASTER_COMMANDS)
          fog_debug("Fog::Painter[Worker #%d]::run() - calculation %d (%p)", ctx.id, (int)calcCurrent, clc);
#endif // FOG_DEBUG_RASTER_COMMANDS

          // If we are here, we won a battle with other threads
          // and RasterPaintCalc* is ours.
          RasterPaintCmd* cmd = clc->relatedTo;

          clc->run(&ctx);
          clc->release(&ctx);

          AutoLock locked(mgr->lock);
          mgr->wakeUpScheduled(this);
        }

        calcCurrent++;
        cont = 0;
      }

      // Do command.
      if (cmdCurrent < mgr->cmdPosition)
      {
        RasterPaintCmd* cmd = mgr->cmdData[cmdCurrent];

#if defined(FOG_DEBUG_RASTER_COMMANDS)
        static const char* statusMsg[] = { "WAIT", "READY", "SKIP" };
        fog_debug("Fog::Painter[Worker #%d]::run() - command %d (%p) status=%s", ctx.id, (int)cmdCurrent, cmd, statusMsg[cmd->status.get()]);
#endif // FOG_DEBUG_RASTER_COMMANDS

        switch (cmd->status.get())
        {
          case RASTER_COMMAND_READY:
          {
            cmd->run(&ctx);
            if (cmd->refCount.deref()) cmd->release(&ctx);

            cmdCurrent++;
            cont = 0;
            break;
          }

          case RASTER_COMMAND_SKIP:
          {
            cmdCurrent++;
            if (cmd->refCount.deref()) cmd->release(&ctx);
            cont = 0;
            break;
          }

          case RASTER_COMMAND_WAIT:
          {
            wait = true;
            goto skipCommands;
          }
        }
      }
skipCommands:
      cont++;
      // We try two times to get work before we wait or quit.
    } while (cont <= 2);

    {
      AutoLock locked(mgr->lock);

      if (calcCurrent < mgr->calcPosition || (cmdCurrent < mgr->cmdPosition && !wait))
      {
        continue;
      }

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::Painter[Worker #%d]::run() - going to wait (currently waiting=%d)",
        ctx.id,
        (int)mgr->waitingWorkers.get());
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

      state.set(WAITING);

      if (mgr->waitingWorkers.addXchg(1) + 1 == mgr->numWorkers)
      {
        // If count of waiting workers will be now count of workers (so this is
        // the last running one), we need to check if everything was completed
        // and if we can fire allFinishedCondition signal.
        if (mgr->isCompleted())
        {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
          fog_debug("Fog::Painter[Worker #%d]::run() - everything done, signaling allFinished...", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
          mgr->allFinishedCondition.signal();
        }
        else
        {
          if (mgr->wakeUpScheduled(this) == this)
          {
            mgr->waitingWorkers.dec();
            continue;
          }
        }
      }

      if (shouldQuit.get() == (int)true && cmdCurrent == mgr->cmdPosition)
      {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
        fog_debug("Fog::Painter[Worker #%d]::run() - quitting...", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

        // Set Running state so wakeUpSleeping() can't return us.
        state.set(RUNNING);
        mgr->wakeUpSleeping(this);
        state.set(DONE);
        return;
      }

      cond.wait();
      state.set(RUNNING);
      mgr->waitingWorkers.dec();

      if (shouldQuit.get() == (int)true)
        mgr->wakeUpSleeping(this);
      else
        mgr->wakeUpScheduled(this);
    }
  }
}

void RasterPaintTask::destroy()
{
  RasterPaintEngine* engine = ctx.engine;
  RasterWorkerManager* mgr = engine->workerManager;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter[Worker #%d]::WorkerTask::destroy()", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  if (mgr->finishedWorkers.addXchg(1) + 1 == mgr->numWorkers)
  {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter[Worker #%d]::WorkerTask::destroy() - I'm last, signaling release event...", ctx.id);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

    mgr->releaseEvent->signal();
  }
}

// ============================================================================
// [Fog::RasterWorkerManager]
// ============================================================================

RasterWorkerManager::RasterWorkerManager() :
  allFinishedCondition(&lock),
  releaseEvent(NULL),
  zoneAllocator(16000)
{
}

RasterWorkerManager::~RasterWorkerManager()
{
}

RasterPaintTask* RasterWorkerManager::wakeUpScheduled(RasterPaintTask* calledFrom)
{
  for (sysuint_t i = 0; i < numWorkers; i++)
  {
    RasterPaintTask* task = tasks[i].instancep();

    if (task->state.get() == RasterPaintTask::WAITING && task->cmdCurrent < cmdPosition)
    {
      if (calledFrom != task) task->cond.signal();
      return task;
    }
  }

  return NULL;
}

RasterPaintTask* RasterWorkerManager::wakeUpSleeping(RasterPaintTask* calledFrom)
{
  for (sysuint_t i = 0; i < numWorkers; i++)
  {
    RasterPaintTask* task = tasks[i].instancep();

    if (task->state.get() == RasterPaintTask::WAITING)
    {
      AutoLock locked(lock);
      if (calledFrom != task) task->cond.signal();
      return task;
    }
  }

  return NULL;
}

bool RasterWorkerManager::isCompleted()
{
  sysuint_t done = 0;

  for (sysuint_t i = 0; i < numWorkers; i++)
  {
    RasterPaintTask* task = tasks[i].instancep();
    if (task->cmdCurrent == cmdPosition) done++;
  }

  return done == numWorkers;
}

// ============================================================================
// [Fog::RasterPaintEngine - Construction / Destruction]
// ============================================================================

RasterPaintEngine::RasterPaintEngine(const ImageBuffer& buffer, ImageData* imaged, uint32_t initFlags) :
  workerManager(NULL)
{
  // BASIC SETUP (setup variables that will never change):
  // - Setup primary context.
  // - Rasterizer.
  ctx.engine = this;
  ctx.pctx = NULL;

  ras = Rasterizer::getRasterizer();

  // Setup primary layer.
  ctx.layer.pixels = buffer.data;
  ctx.layer.width = buffer.width;
  ctx.layer.height = buffer.height;
  ctx.layer.format = buffer.format;
  ctx.layer.stride = buffer.stride;
  _setupLayer(&ctx.layer);

  // Setup clip / caps state to defaults.
  _setClipDefaults();
  _setCapsDefaults();

  // May be set by _setClipDefaults() or _setCapsDefaults(), but we are starting
  // so there is not a change actually.
  ctx.changed = 0;

  // Setup multithreading if possible. If the painting buffer if too small,
  // we will not use multithreading, because it has no sense.
  if ((initFlags & PAINTER_INIT_MT) != 0 && cpuInfo->numberOfProcessors > 1)
  {
    uint64_t total = (uint64_t)buffer.width * (uint64_t)buffer.height;

    if (total >= RASTER_MIN_SIZE_THRESHOLD)
    {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::RasterPaintEngine::new() - Using MT for %dx%d image.", buffer.width, buffer.height);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
      setEngine(PAINTER_ENGINE_RASTER_MT);
    }
  }
}

RasterPaintEngine::~RasterPaintEngine()
{
  // First set engine to singlethreaded (this means flush and releasing all
  // threads), then we can destroy engine.
  if (!isSingleThreaded()) setEngine(PAINTER_ENGINE_RASTER_ST);

  _deleteStates();

  // This is our context, it's imposible that other thread is using it at this 
  // time!
  if (ctx.pctx)
  {
    if (ctx.pctx->initialized) ctx.pctx->destroy(ctx.pctx);

    // This is not important in release mode. In debug mode it's needed to
    // free it because of assertion in the Fog::BlockAllocator.
    blockAllocator.free(ctx.pctx);
  }

  Rasterizer::releaseRasterizer(ras);
}

// ============================================================================
// [Fog::RasterPaintEngine - Width / Height / Format]
// ============================================================================

int RasterPaintEngine::getWidth() const
{
  return ctx.layer.width;
}

int RasterPaintEngine::getHeight() const
{
  return ctx.layer.height;
}

uint32_t RasterPaintEngine::getFormat() const
{
  return ctx.layer.format;
}

// ============================================================================
// [Fog::RasterPaintEngine - Engine / Flush]
// ============================================================================

uint32_t RasterPaintEngine::getEngine() const
{
  return isSingleThreaded()
    ? PAINTER_ENGINE_RASTER_ST
    : PAINTER_ENGINE_RASTER_MT;
}

void RasterPaintEngine::setEngine(uint32_t engine, uint32_t cores)
{
  int i;
  bool mt = (engine == PAINTER_ENGINE_RASTER_MT);

  // If we already initialized demanded engine we can bail safely.
  if ((workerManager != NULL) == mt) return;

  // We are using global thread pool.
  ThreadPool* threadPool = ThreadPool::getInstance();

  // Start multithreading...
  if (mt)
  {
    int max = Math::min<int>(cores > 0 ? cores : cpuInfo->numberOfProcessors, RASTER_MAX_WORKERS);

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - starting multithreading (%d threads)", max);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

    workerManager = new(std::nothrow) RasterWorkerManager;
    if (workerManager == NULL) return;

    // This is for testing multithreaded rendering on single cores.
    if (max < 2)
    {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::Painter::setEngine() - cpu detection says 1, switching to 2");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
      max = 2;
    }

    for (i = 0; i < max; i++)
    {
      if ((workerManager->threads[i] = threadPool->getThread(i)) == NULL) break;
    }

    // Failed to get workers. This can happen if there are many threads that
    // uses multithreaded painter, we must destroy all resources and return.
    if (i <= 1)
    {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::Painter::setEngine() - failed to get %d threads from pool, releasing...", max);
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
      if (workerManager->threads[0])
      {
        threadPool->releaseThread(workerManager->threads[0]);
        workerManager->threads[0] = NULL;
      }

      delete workerManager;
      workerManager = NULL;

      // Bailout
      return;
    }

    int count = i;

    workerManager->numWorkers = count;

    workerManager->finishedWorkers.init(0);
    workerManager->waitingWorkers.init(0);

    workerManager->cmdPosition = 0;
    workerManager->calcPosition = 0;

    // Set threads affinity.
    // if (count >= (int)cpuInfo->numberOfProcessors)
    // {
    //   for (i = 0; i < count; i++) workerManager->threads[i]->setAffinity(1 << i);
    // }

    // Create worker tasks.
    for (i = 0; i < count; i++)
    {
      workerManager->tasks[i].initCustom1(&workerManager->lock);
      RasterPaintContext& taskCtx = workerManager->tasks[i]->ctx;

      taskCtx.engine = this;
      taskCtx.id = i;
      taskCtx.offset = i;
      taskCtx.delta = count;

      taskCtx.copyFromMaster(ctx);
    }

    // Post worker tasks.
    for (i = 0; i < count; i++)
    {
      RasterPaintTask* task = workerManager->tasks[i].instancep();
      workerManager->threads[i]->getEventLoop()->postTask(task);
    }

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - done");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
  }
  // Stop multithreading
  else
  {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - stopping multithreading...");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

    int count = workerManager->numWorkers;

    ThreadEvent releaseEvent(false, false);
    workerManager->releaseEvent = &releaseEvent;

    // Release threads.
    for (i = 0; i < count; i++)
    {
      workerManager->tasks[i]->shouldQuit.set(true);
    }

    // Flush everything and wait for completion.
    flushWithQuit();

    releaseEvent.wait();

    // Reset threads affinity.
    // if (count >= (int)cpuInfo->numberOfProcessors)
    // {
    //   for (i = 0; i < count; i++) workerManager->threads[i]->resetAffinity();
    // }

    for (i = 0; i < count; i++)
    {
      threadPool->releaseThread(workerManager->threads[i], i);
      workerManager->tasks[i].destroy();
    }

    delete workerManager;
    workerManager = NULL;

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
    fog_debug("Fog::Painter::setEngine() - done");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
  }
}

void RasterPaintEngine::flush(uint32_t flags)
{
  if (isSingleThreaded()) return;
  if (workerManager->cmdPosition == 0) return;

  // TODO: flags ignored, it will always wait.
  {
    AutoLock locked(workerManager->lock);
    if (!workerManager->isCompleted())
    {
      workerManager->wakeUpScheduled(NULL);
      workerManager->allFinishedCondition.wait();
    }
  }

  // Reset command position and local command/calculation counters.
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter::flush() - done");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  workerManager->zoneAllocator.reset();
  workerManager->cmdPosition = 0;
  workerManager->calcPosition = 0;

  for (sysuint_t i = 0; i < workerManager->numWorkers; i++)
  {
    workerManager->tasks[i]->cmdCurrent = 0;
    workerManager->tasks[i]->calcCurrent = 0;
  }
}

void RasterPaintEngine::flushWithQuit()
{
  FOG_ASSERT(workerManager);

#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
  fog_debug("Fog::Painter::flushWithQuit() - quitting...");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION

  AutoLock locked(workerManager->lock);
  workerManager->wakeUpSleeping(NULL);
}

// ============================================================================
// [Fog::RasterPaintEngine - Hints]
// ============================================================================

int RasterPaintEngine::getHint(uint32_t hint) const
{
  switch (hint)
  {
    case PAINTER_HINT_ANTIALIASING_QUALITY:
      return (int)ctx.hints.aaQuality;

    case PAINTER_HINT_IMAGE_INTERPOLATION:
      return (int)ctx.hints.imageInterpolation;

    case PAINTER_HINT_GRADIENT_INTERPOLATION:
      return (int)ctx.hints.gradientInterpolation;

    case PAINTER_HINT_OUTLINE_TEXT:
      return (int)ctx.hints.forceOutlineText;

    default:
      return -1;
  }
}

err_t RasterPaintEngine::setHint(uint32_t hint, int value)
{
  switch (hint)
  {
    case PAINTER_HINT_ANTIALIASING_QUALITY:
      if (((uint)value >= ANTI_ALIASING_COUNT))
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.aaQuality = (uint8_t)value;
      return ERR_OK;

    case PAINTER_HINT_IMAGE_INTERPOLATION:
      if (((uint)value >= INTERPOLATION_INVALID))
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.imageInterpolation = (uint8_t)value;
      return ERR_OK;

    case PAINTER_HINT_GRADIENT_INTERPOLATION:
      if (((uint)value >= INTERPOLATION_INVALID))
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.gradientInterpolation = (uint8_t)value;
      return ERR_OK;

    case PAINTER_HINT_OUTLINE_TEXT:
      if (((uint)value >= 2)) 
        return ERR_RT_INVALID_ARGUMENT;

      ctx.hints.forceOutlineText = (uint8_t)value;
      return ERR_OK;

    default:
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Meta]
// ============================================================================

void RasterPaintEngine::setMetaVars(const Region& region, const IntPoint& origin)
{
  _deleteStates();

  ctx.metaOrigin = origin;
  ctx.metaRegion = region;

  ctx.userOrigin.set(0, 0);
  ctx.userRegion = Region::infinite();

  ctx.hints.metaRegionUsed = !region.isInfinite();
  ctx.hints.userRegionUsed = false;

  // We must set workOrigin here, because _setCapsDefaults() will set default
  // transformation matrix to identity + workOrigin.
  ctx.workOrigin.set(origin);

  _setCapsDefaults();
  _updateWorkRegion();
}

void RasterPaintEngine::resetMetaVars()
{
  setMetaVars(Region::infinite(), IntPoint(0, 0));
}

void RasterPaintEngine::setUserVars(const Region& region, const IntPoint& origin)
{
  ctx.userOrigin = origin;
  ctx.userRegion = region;
  ctx.hints.userRegionUsed = !region.isInfinite();

  _updateWorkRegion();
}

void RasterPaintEngine::setUserOrigin(const IntPoint& origin, uint32_t originOp)
{
  switch (originOp)
  {
    case PAINTER_ORIGIN_SET:
      ctx.userOrigin.set(origin);
      break;
    case PAINTER_ORIGIN_TRANSLATE:
      ctx.userOrigin.translate(origin);
      break;
  }

  _updateWorkRegion();
}

void RasterPaintEngine::resetUserVars()
{
  setUserVars(Region::infinite(), IntPoint(0, 0));
}

Region RasterPaintEngine::getMetaRegion() const
{
  return ctx.metaRegion;
}

Region RasterPaintEngine::getUserRegion() const
{
  return ctx.userRegion;
}

IntPoint RasterPaintEngine::getMetaOrigin() const
{
  return ctx.metaOrigin;
}

IntPoint RasterPaintEngine::getUserOrigin() const
{
  return ctx.userOrigin;
}

// ============================================================================
// [Fog::RasterPaintEngine - Operator]
// ============================================================================

uint32_t RasterPaintEngine::getOperator() const
{
  return ctx.ops.op;
}

err_t RasterPaintEngine::setOperator(uint32_t op)
{
  if (op >= OPERATOR_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.ops.op = op;
  ctx.funcs = RasterEngine::getCompositeFuncs(ctx.layer.format, op);

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Source]
// ============================================================================

uint32_t RasterPaintEngine::getSourceType() const
{
  return ctx.ops.sourceType;
}

Argb RasterPaintEngine::getSourceArgb() const
{
  if (ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
    return Argb(ctx.solid.argb);
  else // ctx.ops.sourceType == PAINTER_SOURCE_PATTERN
    return Argb(0x00000000);
}

Pattern RasterPaintEngine::getSourcePattern() const
{
  if (ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
    return Pattern(ctx.solid.argb);
  else // ctx.ops.sourceType == PAINTER_SOURCE_PATTERN
    return Pattern(ctx.pattern.instance());
}

err_t RasterPaintEngine::setSource(Argb argb)
{
  if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN)
  {
    ctx.ops.sourceType = PAINTER_SOURCE_ARGB;
    ctx.pattern.destroy();
    _resetPatternContext();
  }

  ctx.solid.argb = argb;
  ctx.solid.prgb = argb;
  if (argb.a != 0xFF) ctx.solid.prgb = ArgbUtil::premultiply(argb);

  return ERR_OK;
}

err_t RasterPaintEngine::setSource(const Pattern& pattern)
{
  if (pattern.isSolid()) return setSource(pattern.getColor());

  // Destroy old source object if needed.
  if (ctx.ops.sourceType == PAINTER_SOURCE_ARGB)
  {
    ctx.ops.sourceType = PAINTER_SOURCE_PATTERN;
    ctx.pattern.init(pattern);
  }
  else // ctx.ops.sourceType == PAINTER_SOURCE_PATTERN
  {
    ctx.pattern.instance() = pattern;
  }

  _resetPatternContext();
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Fill Parameters]
// ============================================================================

uint32_t RasterPaintEngine::getFillMode() const
{
  return ctx.hints.fillMode;
}

err_t RasterPaintEngine::setFillMode(uint32_t mode)
{
  if (mode >= FILL_MODE_COUNT) return ERR_RT_INVALID_ARGUMENT;
  ctx.hints.fillMode = mode;

  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Stroke Parameters]
// ============================================================================

StrokeParams RasterPaintEngine::getStrokeParams() const
{
  return ctx.strokeParams;
}

err_t RasterPaintEngine::setStrokeParams(const StrokeParams& strokeParams)
{
  if (ctx.strokeParams.getStartCap() >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if (ctx.strokeParams.getEndCap() >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;
  if (ctx.strokeParams.getLineJoin() >= LINE_JOIN_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams = strokeParams;
  _updateLineWidth();
  return ERR_OK;
}

double RasterPaintEngine::getLineWidth() const
{
  return ctx.strokeParams.getLineWidth();
}

err_t RasterPaintEngine::setLineWidth(double lineWidth)
{
  ctx.strokeParams.setLineWidth(lineWidth);
  _updateLineWidth();
  return ERR_OK;
}

uint32_t RasterPaintEngine::getStartCap() const
{
  return ctx.strokeParams.getStartCap();
}

err_t RasterPaintEngine::setStartCap(uint32_t startCap)
{
  if (startCap >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setStartCap(startCap);
  _updateLineWidth();
  return ERR_OK;
}

uint32_t RasterPaintEngine::getEndCap() const
{
  return ctx.strokeParams.getEndCap();
}

err_t RasterPaintEngine::setEndCap(uint32_t endCap)
{
  if (endCap >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setEndCap(endCap);
  _updateLineWidth();
  return ERR_OK;
}

err_t RasterPaintEngine::setLineCaps(uint32_t lineCaps)
{
  if (lineCaps >= LINE_CAP_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setLineCaps(lineCaps);
  _updateLineWidth();
  return ERR_OK;
}

uint32_t RasterPaintEngine::getLineJoin() const
{
  return ctx.strokeParams.getLineJoin();
}

err_t RasterPaintEngine::setLineJoin(uint32_t lineJoin)
{
  if (lineJoin >= LINE_JOIN_COUNT) return ERR_RT_INVALID_ARGUMENT;

  ctx.strokeParams.setLineJoin(lineJoin);
  return ERR_OK;
}

double RasterPaintEngine::getMiterLimit() const
{
  return ctx.strokeParams.getMiterLimit();
}

err_t RasterPaintEngine::setMiterLimit(double miterLimit)
{
  ctx.strokeParams.setMiterLimit(miterLimit);
  _updateLineWidth();
  return ERR_OK;
}

List<double> RasterPaintEngine::getDashes() const
{
  return ctx.strokeParams.getDashes();
}

err_t RasterPaintEngine::setDashes(const double* dashes, sysuint_t count)
{
  ctx.strokeParams.setDashes(dashes, count);
  _updateLineWidth();
  return ERR_OK;
}

err_t RasterPaintEngine::setDashes(const List<double>& dashes)
{
  ctx.strokeParams.setDashes(dashes);
  _updateLineWidth();
  return ERR_OK;
}

double RasterPaintEngine::getDashOffset() const
{
  return ctx.strokeParams.getDashOffset();
}

err_t RasterPaintEngine::setDashOffset(double offset)
{
  ctx.strokeParams.setDashOffset(offset);
  _updateLineWidth();
  return ERR_OK;
}

// ============================================================================
// [Fog::RasterPaintEngine - Transformations]
// ============================================================================

void RasterPaintEngine::setMatrix(const DoubleMatrix& m)
{
  ctx.userMatrix = m;
  _updateTransform(false);
}

void RasterPaintEngine::resetMatrix()
{
  ctx.userMatrix.reset();
  _updateTransform(false);
}

DoubleMatrix RasterPaintEngine::getMatrix() const
{
  return ctx.userMatrix;
}

void RasterPaintEngine::rotate(double angle, uint32_t order)
{
  ctx.userMatrix.rotate(angle, order);
  _updateTransform(false);
}

void RasterPaintEngine::scale(double sx, double sy, uint32_t order)
{
  ctx.userMatrix.scale(sx, sy, order);
  _updateTransform(false);
}

void RasterPaintEngine::skew(double sx, double sy, uint32_t order)
{
  ctx.userMatrix.scale(sx, sy, order);
  _updateTransform(false);
}

void RasterPaintEngine::translate(double x, double y, uint32_t order)
{
  ctx.userMatrix.translate(x, y, order);
  _updateTransform(true);
}

void RasterPaintEngine::transform(const DoubleMatrix& m, uint32_t order)
{
  ctx.userMatrix.multiply(m, order);
  _updateTransform(false);
}

void RasterPaintEngine::worldToScreen(DoublePoint* pt) const
{
  if (pt == NULL) return;

  if (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
  {
    ctx.userMatrix.transformPoint(&pt->x, &pt->y);
  }
  else
  {
    pt->x += ctx.userMatrix.tx;
    pt->y += ctx.userMatrix.ty;
  }
}

void RasterPaintEngine::screenToWorld(DoublePoint* pt) const
{
  if (pt == NULL) return;

  if (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
  {
    ctx.userMatrix.inverted().transformPoint(&pt->x, &pt->y);
  }
  else
  {
    pt->x -= ctx.userMatrix.tx;
    pt->y -= ctx.userMatrix.ty;
  }
}

void RasterPaintEngine::alignPoint(DoublePoint* pt) const
{
  if (pt == NULL) return;

  RasterPaintEngine::worldToScreen(pt);
  pt->set(floor(pt->x) + 0.5, floor(pt->y) + 0.5);
  RasterPaintEngine::screenToWorld(pt);
}

// ============================================================================
// [Fog::RasterPaintEngine - State]
// ============================================================================

void RasterPaintEngine::save()
{
  // PAINTER TODO
/*
  RasterSavedState s;

  s.pctx = NULL;

  if (ctx.pctx && ctx.pctx->initialized)
  {
    s.pctx = ctx.pctx;
    s.pctx->refCount.inc();
  }

  states.append(s);
*/
}

void RasterPaintEngine::restore()
{
  // PAINTER TODO
/*
  if (states.isEmpty()) return;

  RasterSavedState s = states.takeLast();

  if (ctx.pctx && ctx.pctx->refCount.deref())
  {
    if (ctx.pctx->initialized) ctx.pctx->destroy(ctx.pctx);
    allocator.free(ctx.pctx);
  }

  ctx.pctx = s.pctx;
*/
}

// ============================================================================
// [Fog::RasterPaintEngine - Raster drawing]
// ============================================================================

void RasterPaintEngine::drawPoint(const IntPoint& p)
{
  RasterPaintEngine::drawPoint(
    DoublePoint((double)p.x + 0.5, (double)p.y + 0.5));
}

void RasterPaintEngine::drawLine(const IntPoint& start, const IntPoint& end)
{
  RasterPaintEngine::drawLine(
    DoublePoint((double)start.x + 0.5, (double)start.y + 0.5),
    DoublePoint((double)end.x   + 0.5, (double)end.y   + 0.5));
}

void RasterPaintEngine::drawRect(const IntRect& r)
{
  if (!r.isValid()) return;

  if (ctx.hints.transformType != RASTER_TRANSFORM_EXACT || !ctx.hints.lineIsSimple)
  {
    RasterPaintEngine::drawRect(
      DoubleRect((double)r.x + 0.5, (double)r.y + 0.5,
                 (double)r.w      , (double)r.h)     );
  }
  else
  {
    int tx = ctx.workTranslate.x;
    int ty = ctx.workTranslate.y;

    int x1 = r.getX1() + tx, y1 = r.getY1() + ty;
    int x2 = r.getX2() + tx, y2 = r.getY2() + ty;

    TemporaryRegion<4> box;
    RegionData* box_d = box._d;

    box_d->extents.set(x1, y1, x2, y2);

    if (r.w <= 2 || r.h <= 2)
    {
      box_d->rects[0] = box_d->extents;
      box_d->length = 1;
    }
    else
    {
      box_d->rects[0].set(x1    , y1    , x2    , y1 + 1);
      box_d->rects[1].set(x1    , y1 + 1, x1 + 1, y2 - 1);
      box_d->rects[2].set(x2 - 1, y1 + 1, x2    , y2 - 1);
      box_d->rects[3].set(x1    , y2 - 1, x2    , y2    );
      box_d->length = 4;
    }

    if (!(ctx.ops.clipType & RASTER_CLIP_SIMPLE) || ctx.clipBox.subsumes(box_d->extents))
    {
      _serializeBoxes(box_d->rects, box_d->length);
    }
    else
    {
      Region::combine(tmpRegion, box, ctx.workRegion, REGION_OP_INTERSECT);
      sysuint_t len = tmpRegion.getLength();
      if (len) _serializeBoxes(tmpRegion.getData(), len);
    }
  }
}

void RasterPaintEngine::drawRound(const IntRect& r, const IntPoint& radius)
{
  RasterPaintEngine::drawRound(
    DoubleRect(
      (double)r.x + 0.5,
      (double)r.y + 0.5,
      (double)r.getWidth(),
      (double)r.getHeight()),
    DoublePoint(
      (double)radius.x,
      (double)radius.y));
}

void RasterPaintEngine::fillRect(const IntRect& r)
{
  if (!r.isValid()) return;

  if (ctx.hints.transformType != RASTER_TRANSFORM_EXACT)
  {
    RasterPaintEngine::fillRect(
      DoubleRect((double)r.x, (double)r.y,
                 (double)r.w, (double)r.h));
  }
  else
  {
    int tx = ctx.workTranslate.x;
    int ty = ctx.workTranslate.y;

    int x1 = r.getX1() + tx, y1 = r.getY1() + ty;
    int x2 = r.getX2() + tx, y2 = r.getY2() + ty;

    IntBox box(x1, y1, x2, y2);

    if (ctx.ops.clipType & RASTER_CLIP_SIMPLE)
    {
      IntBox::intersect(box, box, ctx.clipBox);
      if (box.isValid()) _serializeBoxes(&box, 1);
    }
    else
    {
      Region::combine(tmpRegion, ctx.workRegion, TemporaryRegion<1>(box), REGION_OP_INTERSECT);

      sysuint_t len = tmpRegion.getLength();
      if (len) _serializeBoxes(tmpRegion.getData(), len);
    }
  }
}

void RasterPaintEngine::fillRects(const IntRect* r, sysuint_t count)
{
  if (!count) return;

  if (ctx.hints.transformType == RASTER_TRANSFORM_EXACT)
  {
    tmpRegion.set(r, count);
    Region::translateAndClip(tmpRegion, tmpRegion, ctx.workTranslate, ctx.clipBox);

    if (ctx.ops.clipType & RASTER_CLIP_COMPLEX)
    {
      Region::combine(tmpRegion, tmpRegion, ctx.workRegion, REGION_OP_INTERSECT);
    }

    sysuint_t len = tmpRegion.getLength();
    if (len) _serializeBoxes(tmpRegion.getData(), len);
  }
  else
  {
    tmpPath.clear();
    for (sysuint_t i = 0; i < count; i++)
    {
      tmpPath.addRect(
        DoubleRect((double)r[i].x, (double)r[i].y,
                   (double)r[i].w, (double)r[i].h));
    }
    fillPath(tmpPath);
  }
}

void RasterPaintEngine::fillRound(const IntRect& r, const IntPoint& radius)
{
  RasterPaintEngine::fillRound(
    DoubleRect((double)r.x, (double)r.y, (double)r.w, (double)r.h),
    DoublePoint((double)radius.x, (double)radius.y));
}

void RasterPaintEngine::fillRegion(const Region& region)
{
  if (region.isInfinite()) { fillAll(); return; }
  if (region.isEmpty()) return;

  if (ctx.hints.transformType == RASTER_TRANSFORM_EXACT)
  {
    region.translateAndClip(tmpRegion, region, ctx.workTranslate, ctx.clipBox);

    if (ctx.ops.clipType & RASTER_CLIP_COMPLEX)
    {
      Region::combine(tmpRegion, tmpRegion, ctx.workRegion, REGION_OP_INTERSECT);
    }

    sysuint_t len = tmpRegion.getLength();
    if (len) _serializeBoxes(region.getData(), len);
  }
  else
  {
    const IntBox* r = region.getData();
    sysuint_t count = region.getLength();

    tmpPath.clear();
    for (sysuint_t i = 0; i < count; i++)
    {
      tmpPath.addRect(
        DoubleRect((double)r[i].getX(), (double)r[i].getY(),
                   (double)r[i].getWidth(), (double)r[i].getHeight()));
    }
    fillPath(tmpPath);
  }
}

void RasterPaintEngine::fillAll()
{
  if (ctx.ops.clipType & RASTER_CLIP_SIMPLE)
  {
    _serializeBoxes(&ctx.clipBox, 1);
  }
  else
  {
    const Region& r = ctx.workRegion;
    _serializeBoxes(r.getData(), r.getLength());
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Vector drawing]
// ============================================================================

void RasterPaintEngine::drawPoint(const DoublePoint& p)
{
  tmpPath.clear();
  tmpPath.moveTo(p);
  tmpPath.lineTo(p.x, p.y + 0.0001);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawLine(const DoublePoint& start, const DoublePoint& end)
{
  tmpPath.clear();
  tmpPath.moveTo(start);
  tmpPath.lineTo(end);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawLine(const DoublePoint* pts, sysuint_t count)
{
  if (!count) return;

  tmpPath.clear();
  tmpPath.moveTo(pts[0]);
  if (count > 1)
    tmpPath.lineTo(pts + 1, count - 1);
  else
    tmpPath.lineTo(pts[0].x, pts[0].y + 0.0001);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawPolygon(const DoublePoint* pts, sysuint_t count)
{
  if (!count) return;

  tmpPath.clear();
  tmpPath.moveTo(pts[0]);
  if (count > 1)
    tmpPath.lineTo(pts + 1, count - 1);
  else
    tmpPath.lineTo(pts[0].x, pts[0].y + 0.0001);
  tmpPath.closePolygon();
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawRect(const DoubleRect& r)
{
  if (!r.isValid()) return;

  tmpPath.clear();
  tmpPath.addRect(r);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawRects(const DoubleRect* r, sysuint_t count)
{
  if (!count) return;

  tmpPath.clear();
  tmpPath.addRects(r, count);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawRound(const DoubleRect& r, const DoublePoint& radius)
{
  tmpPath.clear();
  tmpPath.addRound(r, radius);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawEllipse(const DoublePoint& cp, const DoublePoint& r)
{
  tmpPath.clear();
  tmpPath.addEllipse(cp, r);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep)
{
  tmpPath.clear();
  tmpPath.addArc(cp, r, start, sweep);
  _serializePath(tmpPath, true);
}

void RasterPaintEngine::drawPath(const DoublePath& path)
{
  _serializePath(path, true);
}

void RasterPaintEngine::fillPolygon(const DoublePoint* pts, sysuint_t count)
{
  if (!count) return;

  tmpPath.clear();
  tmpPath.moveTo(pts[0]);
  if (count > 1)
    tmpPath.lineTo(pts + 1, count - 1);
  else
    tmpPath.lineTo(pts[0].x, pts[0].y + 0.0001);
  tmpPath.closePolygon();
  _serializePath(tmpPath, false);
}

void RasterPaintEngine::fillRect(const DoubleRect& r)
{
  if (!r.isValid()) return;

  if (ctx.hints.transformType < RASTER_TRANSFORM_AFFINE)
  {
    int64_t rx = Math::iround((r.x + ctx.workMatrix.tx) * 256.0);
    if ((rx & 0xFF) != 0x00) goto usePath;

    int64_t ry = Math::iround((r.y + ctx.workMatrix.ty) * 256.0);
    if ((ry & 0xFF) != 0x00) goto usePath;

    int64_t rw = Math::iround((r.w) * 256.0);
    if ((rw & 0xFF) != 0x00) goto usePath;

    int64_t rh = Math::iround((r.h) * 256.0);
    if ((rh & 0xFF) != 0x00) goto usePath;

    IntBox box;
    box.x1 = (int)(rx >> 8);
    box.y1 = (int)(ry >> 8);
    box.x2 = box.x1 + (int)(rw >> 8);
    box.y2 = box.y1 + (int)(rh >> 8);

    if (ctx.ops.clipType & RASTER_CLIP_SIMPLE)
    {
      IntBox::intersect(box, box, ctx.clipBox);
      if (box.isValid()) _serializeBoxes(&box, 1);
    }
    else
    {
      Region::combine(tmpRegion, ctx.workRegion, TemporaryRegion<1>(box), REGION_OP_INTERSECT);

      sysuint_t len = tmpRegion.getLength();
      if (len) _serializeBoxes(tmpRegion.getData(), len);
    }
    return;
  }

usePath:
  tmpPath.clear();
  tmpPath.addRect(r);
  _serializePath(tmpPath, false);
}

void RasterPaintEngine::fillRects(const DoubleRect* r, sysuint_t count)
{
  if (!count) return;

  // Single rect fills can be more optimized.
  if (count == 1) fillRect(r[0]);

  tmpPath.clear();
  tmpPath.addRects(r, count);
  _serializePath(tmpPath, false);
}

void RasterPaintEngine::fillRound(const DoubleRect& r, const DoublePoint& radius)
{
  tmpPath.clear();
  tmpPath.addRound(r, radius);
  _serializePath(tmpPath, false);
}

void RasterPaintEngine::fillEllipse(const DoublePoint& cp, const DoublePoint& r)
{
  tmpPath.clear();
  tmpPath.addEllipse(cp, r);
  _serializePath(tmpPath, false);
}

void RasterPaintEngine::fillArc(const DoublePoint& cp, const DoublePoint& r, double start, double sweep)
{
  tmpPath.clear();
  tmpPath.addArc(cp, r, start, sweep);
  _serializePath(tmpPath, false);
}

void RasterPaintEngine::fillPath(const DoublePath& path)
{
  _serializePath(path, false);
}

// ============================================================================
// [Fog::RasterPaintEngine - Glyph / Text Drawing]
// ============================================================================

void RasterPaintEngine::drawGlyph(const IntPoint& pt_, const Glyph& glyph, const IntRect* clip_)
{
  tmpGlyphSet.clear();

  int tx = ctx.workTranslate.x;
  int ty = ctx.workTranslate.y;

  IntPoint pt(pt_.x + tx, pt_.y + ty);
  IntRect clip;

  if (clip_)
  {
    clip.set(clip_->x + tx, clip_->y + ty, clip_->w, clip_->h);
    clip_ = &clip;
  }

  err_t err;
  if ((err = tmpGlyphSet.begin(1))) return;
  tmpGlyphSet._add(glyph._d->ref());
  if ((err = tmpGlyphSet.end())) return;

  _serializeGlyphSet(pt, tmpGlyphSet, clip_);
}

void RasterPaintEngine::drawGlyphSet(const IntPoint& pt_, const GlyphSet& glyphSet, const IntRect* clip_)
{
  int tx = ctx.workTranslate.x;
  int ty = ctx.workTranslate.y;

  IntPoint pt(pt_.x + tx, pt_.y + ty);
  IntRect clip;

  if (clip_)
  {
    clip.set(clip_->x + tx, clip_->y + ty, clip_->w, clip_->h);
    clip_ = &clip;
  }

  _serializeGlyphSet(pt, glyphSet, clip_);
}

void RasterPaintEngine::drawText(const IntPoint& pt_, const String& text, const Font& font, const IntRect* clip_)
{
  int tx = ctx.workTranslate.x;
  int ty = ctx.workTranslate.y;

  IntPoint pt(pt_.x + tx, pt_.y + ty);
  IntRect clip;

  if (clip_)
  {
    clip.set(clip_->x + tx, clip_->y + ty, clip_->w, clip_->h);
    clip_ = &clip;
  }

  tmpGlyphSet.clear();
  if (font.getGlyphSet(text.getData(), text.getLength(), tmpGlyphSet)) return;

  _serializeGlyphSet(pt, tmpGlyphSet, clip_);
}

void RasterPaintEngine::drawText(const IntRect& r, const String& text, const Font& font, uint32_t align, const IntRect* clip_)
{
  int tx = ctx.workTranslate.x;
  int ty = ctx.workTranslate.y;

  IntRect clip;

  if (clip_)
  {
    clip.set(clip_->x + tx, clip_->y + ty, clip_->w, clip_->h);
    clip_ = &clip;
  }

  tmpGlyphSet.clear();
  if (font.getGlyphSet(text.getData(), text.getLength(), tmpGlyphSet)) return;

  int wsize = tmpGlyphSet.getAdvance();
  int hsize = font.getHeight();

  int x = r.x + tx;
  int y = r.y + ty;
  int w = r.w;
  int h = r.h;

  switch (align & TEXT_ALIGN_HMASK)
  {
    case TEXT_ALIGN_LEFT:
      break;
    case TEXT_ALIGN_RIGHT:
      x = x + w - wsize;
      break;
    case TEXT_ALIGN_HCENTER:
      x = x + (w - wsize) / 2;
      break;
  }

  switch (align & TEXT_ALIGN_VMASK)
  {
    case TEXT_ALIGN_TOP:
      break;
    case TEXT_ALIGN_BOTTOM:
      y = y + h - hsize;
      break;
    case TEXT_ALIGN_VCENTER:
      y = y + (h - hsize) / 2;
      break;
  }

  _serializeGlyphSet(IntPoint(x, y), tmpGlyphSet, clip_);
}

// ============================================================================
// [Fog::RasterPaintEngine - Image drawing]
// ============================================================================

void RasterPaintEngine::blitImage(const IntPoint& p, const Image& image, const IntRect* irect)
{
  if (image.isEmpty()) return;

  if (ctx.hints.transformType == RASTER_TRANSFORM_EXACT)
  {
    int srcx = 0;
    int srcy = 0;
    int dstx = p.x + ctx.workTranslate.x;
    int dsty = p.y + ctx.workTranslate.y;
    int dstw;
    int dsth;

    if (irect == NULL)
    {
      dstw = image.getWidth();
      dsth = image.getHeight();
    }
    else
    {
      if (!irect->isValid()) return;

      srcx = irect->x;
      if (srcx < 0) return;
      srcy = irect->y;
      if (srcy < 0) return;

      dstw = Math::min(image.getWidth(), irect->getWidth());
      if (dstw == 0) return;
      dsth = Math::min(image.getHeight(), irect->getHeight());
      if (dsth == 0) return;
    }

    int d;

    if ((uint)(d = dstx - ctx.clipBox.getX1()) >= (uint)ctx.clipBox.getWidth())
    {
      if (d < 0)
      {
        if ((dstw += d) <= 0) return;
        dstx = 0;
        srcx = -d;
      }
      else
      {
        return;
      }
    }

    if ((uint)(d = dsty - ctx.clipBox.getY1()) >= (uint)ctx.clipBox.getHeight())
    {
      if (d < 0)
      {
        if ((dsth += d) <= 0) return;
        dsty = 0;
        srcy = -d;
      }
      else
      {
        return;
      }
    }

    if ((d = ctx.clipBox.x2 - dstx) < dstw) dstw = d;
    if ((d = ctx.clipBox.y2 - dsty) < dsth) dsth = d;

    IntRect dst(dstx, dsty, dstw, dsth);
    IntRect src(srcx, srcy, dstw, dsth);
    _serializeImage(dst, image, src);
  }
  else
  {
    DoublePoint pd((double)p.x, (double)p.y);
    _serializeImageAffine(pd, image, irect);
  }
}

void RasterPaintEngine::blitImage(const DoublePoint& p, const Image& image, const IntRect* irect)
{
  if (image.isEmpty()) return;

  if (ctx.hints.transformType <= RASTER_TRANSFORM_SUBPX)
  {
    int64_t xbig = (int64_t)((p.x + ctx.workMatrix.tx) * 256.0);
    int64_t ybig = (int64_t)((p.y + ctx.workMatrix.ty) * 256.0);

    int xf = (int)(xbig & 0xFF);
    int yf = (int)(ybig & 0xFF);

    if (xf == 0x00 && yf == 0x00)
    {
      int srcx = 0;
      int srcy = 0;
      int dstx = (int)(xbig >> 8);
      int dsty = (int)(ybig >> 8);
      int dstw;
      int dsth;

      if (irect == NULL)
      {
        dstw = image.getWidth();
        dsth = image.getHeight();
      }
      else
      {
        if (!irect->isValid()) return;

        srcx = irect->x;
        if (srcx < 0) return;
        srcy = irect->y;
        if (srcy < 0) return;

        dstw = Math::min(image.getWidth(), irect->getWidth());
        if (dstw == 0) return;
        dsth = Math::min(image.getHeight(), irect->getHeight());
        if (dsth == 0) return;
      }

      int d;

      if ((uint)(d = dstx - ctx.clipBox.x1) >= (uint)ctx.clipBox.getWidth())
      {
        if (d < 0)
        {
          if ((dstw += d) <= 0) return;
          dstx = 0;
          srcx = -d;
        }
        else
        {
          return;
        }
      }

      if ((uint)(d = dsty - ctx.clipBox.y1) >= (uint)ctx.clipBox.getHeight())
      {
        if (d < 0)
        {
          if ((dsth += d) <= 0) return;
          dsty = 0;
          srcy = -d;
        }
        else
        {
          return;
        }
      }

      if ((d = ctx.clipBox.x2 - dstx) < dstw) dstw = d;
      if ((d = ctx.clipBox.y2 - dsty) < dsth) dsth = d;

      IntRect dst(dstx, dsty, dstw, dsth);
      IntRect src(srcx, srcy, dstw, dsth);
      _serializeImage(dst, image, src);

      return;
    }
  }

  _serializeImageAffine(p, image, irect);
}

// ============================================================================
// [Fog::RasterPaintEngine - Helpers]
// ============================================================================

err_t RasterPaintEngine::switchTo(const ImageBuffer& buffer, ImageData* d)
{
  // Sync, making all changes visible to current image / layer.
  flush(PAINTER_FLUSH_SYNC);

  // Delete all saved states.
  _deleteStates();

  // Destroy pattern context.
  if (ctx.pctx && ctx.pctx->initialized) ctx.pctx->destroy(ctx.pctx);

  // Setup primary layer.
  ctx.layer.pixels = buffer.data;
  ctx.layer.width = buffer.width;
  ctx.layer.height = buffer.height;
  ctx.layer.format = buffer.format;
  ctx.layer.stride = buffer.stride;
  _setupLayer(&ctx.layer);

  // Setup clip / caps state to defaults.
  _setClipDefaults();
  _setCapsDefaults();

  return ERR_OK;
}

void RasterPaintEngine::_updateWorkRegion()
{
  // Work origin is point that is added to painter translation matrix and it
  // ensured that raster will be always from [0, 0] -> [W-1, H-1] inclusive.
  IntPoint workOrigin(ctx.metaOrigin + ctx.userOrigin);
  bool workOriginChanged = ctx.workOrigin != workOrigin;

  // Default clip box is no clip.
  ctx.ops.clipType &= ~(RASTER_CLIP_COMPLEX | RASTER_CLIP_SIMPLE);
  ctx.clipBox.set(0, 0, ctx.layer.width, ctx.layer.height);
  ctx.workOrigin = workOrigin;

  // Calculate final clip region.
  uint clipBits = ((uint)ctx.hints.metaRegionUsed << 1) | ((uint)(ctx.hints.userRegionUsed) << 0);

  switch (clipBits)
  {
    // User region only.
    case 0x00000001:
      Region::translateAndClip(ctx.workRegion, ctx.userRegion, ctx.metaOrigin, ctx.clipBox);
      break;

    // Meta region only.
    case 0x00000002:
      Region::translateAndClip(ctx.workRegion, ctx.metaRegion, IntPoint(0, 0), ctx.clipBox);
      break;

    // Meta region & user region.
    case 0x00000003:
    {
      Region::translate(tmpRegion, ctx.userRegion, ctx.metaOrigin);
      Region::intersectAndClip(ctx.workRegion, ctx.metaRegion, tmpRegion, ctx.clipBox);
      break;
    }
  }

  if (clipBits)
  {
    ctx.clipBox = ctx.workRegion.extents();
    ctx.ops.clipType |= (ctx.workRegion.getLength() <= 1) 
      ? RASTER_CLIP_SIMPLE
      : RASTER_CLIP_COMPLEX;
  }
  else
  {
    ctx.ops.clipType |= RASTER_CLIP_SIMPLE;
  }

  ctx.hints.workRegionUsed = (clipBits != 0) && !(ctx.ops.clipType & RASTER_CLIP_SIMPLE);
  ctx.changed != RASTER_CHANGED_REGION;

  if (workOriginChanged) _updateTransform(true);
}

void RasterPaintEngine::_updateTransform(bool translationOnly)
{
  // Apply meta matrix if used.
  if (ctx.hints.metaMatrixUsed)
  {
    ctx.workMatrix = ctx.metaMatrix;
    ctx.workMatrix.multiply(ctx.userMatrix, MATRIX_PREPEND);
    translationOnly = false;
  }
  else
  {
    ctx.workMatrix = ctx.userMatrix;
  }

  // Translate work matrix by work origin (meta origin + user origin).
  ctx.workMatrix.tx += ctx.workOrigin.x;
  ctx.workMatrix.ty += ctx.workOrigin.y;

  // Update translation in pixels.
  ctx.workTranslate.set(Math::iround(ctx.workMatrix.tx),
                        Math::iround(ctx.workMatrix.ty));

  // If only matrix translation was changed, we can skip some expensive
  // calculations and checking.
  if (translationOnly)
  {
    if (ctx.hints.transformType < RASTER_TRANSFORM_AFFINE)
    {
      bool isExact = 
        ((Math::iround(ctx.userMatrix.tx * 256.0) & 0xFF) == 0x00) &
        ((Math::iround(ctx.userMatrix.ty * 256.0) & 0xFF) == 0x00) ;

      ctx.hints.transformType = isExact ? RASTER_TRANSFORM_EXACT : RASTER_TRANSFORM_SUBPX;
    }
  }
  else
  {
    // Just check for matrix characteristics.
    bool isIdentity = 
      (Math::feq(ctx.workMatrix.sx , 1.0)) &
      (Math::feq(ctx.workMatrix.sy , 1.0)) &
      (Math::feq(ctx.workMatrix.shx, 0.0)) &
      (Math::feq(ctx.workMatrix.shy, 0.0)) ;
    bool isExact = isIdentity && (
      ((Math::iround(ctx.workMatrix.tx * 256.0) & 0xFF) == 0x00) &
      ((Math::iround(ctx.workMatrix.ty * 256.0) & 0xFF) == 0x00) );

    // Use matrix characteristics to set correct transform type.
    uint transformType = isIdentity
      ? (isExact ? RASTER_TRANSFORM_EXACT : RASTER_TRANSFORM_SUBPX)
      : (RASTER_TRANSFORM_AFFINE);

    ctx.hints.transformType = (uint8_t)transformType;

    // sqrt(2.0)/2 ~~ 0.7071068
    if (transformType >= RASTER_TRANSFORM_AFFINE)
    {
      double x = ctx.workMatrix.sx + ctx.workMatrix.shx;
      double y = ctx.workMatrix.sy + ctx.workMatrix.shy;
      ctx.approximationScale = Math::sqrt(x * x + y * y) * 0.7071068;
    }
    else
    {
      ctx.approximationScale = 1.0;
    }
  }

  // Free pattern context, because transform was changed.
  if (ctx.pctx && ctx.pctx->initialized) _resetPatternContext();
}

void RasterPaintEngine::_setClipDefaults()
{
  ctx.ops.clipType = RASTER_CLIP_SIMPLE;

  ctx.hints.metaMatrixUsed = false;

  ctx.hints.metaRegionUsed = false;
  ctx.hints.userRegionUsed = false;
  ctx.hints.workRegionUsed = false;

  ctx.workMatrix.tx -= ctx.workOrigin.x;
  ctx.workMatrix.ty -= ctx.workOrigin.y;

  ctx.metaOrigin.clear();
  ctx.userOrigin.clear();
  ctx.workOrigin.clear();

  ctx.clipBox.set(0, 0, ctx.layer.width, ctx.layer.height);
  ctx.changed |= RASTER_CHANGED_REGION;
}

void RasterPaintEngine::_setCapsDefaults()
{
  if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN) ctx.pattern.destroy();
  if (ctx.pctx && ctx.pctx->initialized) _resetPatternContext();

  ctx.ops.op = OPERATOR_SRC_OVER;
  ctx.ops.sourceType = PAINTER_SOURCE_ARGB;

  ctx.hints.transformType = RASTER_TRANSFORM_EXACT;
  ctx.hints.fillMode = FILL_DEFAULT;
  ctx.hints.aaQuality = ANTI_ALIASING_SMOOTH;
  ctx.hints.imageInterpolation = INTERPOLATION_SMOOTH;
  ctx.hints.gradientInterpolation = INTERPOLATION_SMOOTH;
  ctx.hints.forceOutlineText = false;
  ctx.hints.lineIsSimple = true;

  ctx.funcs = RasterEngine::getCompositeFuncs(ctx.layer.format, OPERATOR_SRC_OVER);

  ctx.solid.argb = 0xFF000000;
  ctx.solid.prgb = 0xFF000000;

  ctx.strokeParams.reset();

  ctx.userMatrix.reset();
  ctx.workMatrix.set(
    1.0, 0.0,
    0.0, 1.0,
    0.0 + (double)ctx.workOrigin.x, 0.0 + (double)ctx.workOrigin.y);
  ctx.workTranslate = ctx.workOrigin;
  ctx.approximationScale = 1.0;
}

RasterEngine::PatternContext* RasterPaintEngine::_getPatternContext()
{
  // Sanity, calling _getPatternContext() for other than pattern 
  // source is not allowed.
  FOG_ASSERT(ctx.ops.sourceType == PAINTER_SOURCE_PATTERN);

  RasterEngine::PatternContext* pctx = ctx.pctx;
  err_t err = ERR_OK;

  if (!pctx)
  {
    pctx = ctx.pctx = (RasterEngine::PatternContext*)blockAllocator.alloc(sizeof(RasterEngine::PatternContext));
    if (!pctx) return NULL;

    pctx->refCount.init(1);
    pctx->initialized = false;
  }

  if (!pctx->initialized)
  {
    const Pattern& pattern = ctx.pattern.instance();
    const DoubleMatrix& matrix = ctx.workMatrix;

    switch (pattern.getType())
    {
      case PATTERN_TEXTURE:
        err = RasterEngine::functionMap->pattern.texture_init(pctx, pattern, matrix, ctx.hints.imageInterpolation);
        break;
      case PATTERN_LINEAR_GRADIENT:
        err = RasterEngine::functionMap->pattern.linear_gradient_init(pctx, pattern, matrix, ctx.hints.gradientInterpolation);
        break;
      case PATTERN_RADIAL_GRADIENT:
        err = RasterEngine::functionMap->pattern.radial_gradient_init(pctx, pattern, matrix, ctx.hints.gradientInterpolation);
        break;
      case PATTERN_CONICAL_GRADIENT:
        err = RasterEngine::functionMap->pattern.conical_gradient_init(pctx, pattern, matrix, ctx.hints.gradientInterpolation);
        break;
      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  if (err)
  {
    fog_debug("Fog::RasterPaintEngine::_getPatternContext() - Failed to get context (error %d)\n", err);
    return NULL;
  }

  // Be sure that pattern context is always marked as initialized. If this 
  // assert fails then the problem is in RasterEngine pattern initializer.
  FOG_ASSERT(pctx->initialized);

  return pctx;
}

void RasterPaintEngine::_resetPatternContext()
{
  RasterEngine::PatternContext* pctx = ctx.pctx;

  // Ignore non-initialized context.
  if (pctx && pctx->initialized)
  {
    // Each initialized context must have reference count larger or equal to one.
    // If we dereference it here it means that it's not used by another thread
    // so we can reuse allocated memory later.
    if (pctx->refCount.deref())
    {
      // Destroy context and reuse memory location later.
      pctx->destroy(ctx.pctx);
      pctx->refCount.init(1);
    }
    else
    {
      // Other thread will destroy it.
      ctx.pctx = NULL;
    }
  }
}

void RasterPaintEngine::_deleteStates()
{
  if (states.isEmpty()) return;

  List<RasterPaintState*>::ConstIterator it(states);
  for (it.toStart(); it.isValid(); it.toNext())
  {
    RasterPaintState* state = it.value();

    // Destroy pattern context associated with the state.
    RasterEngine::PatternContext* pctx = state->pctx;
    if (pctx && pctx->refCount.deref())
    {
      pctx->destroy(pctx);
      blockAllocator.free(pctx);
    }

    // Free memory.
    blockAllocator.free(state);
  }

  states.clear();
}

// ============================================================================
// [Fog::RasterPaintEngine - Layer]
// ============================================================================

void RasterPaintEngine::_setupLayer(RasterPaintLayer* layer)
{
  switch (layer->format)
  {
    case PIXEL_FORMAT_PRGB32:
    case PIXEL_FORMAT_XRGB32:
      // Direct rendering in all cases, nothing to setup.
      layer->type = RASTER_LAYER_TYPE_DIRECT32;
      layer->bytesPerPixel = 4;

      layer->secondaryFormat = PIXEL_FORMAT_NULL;
      layer->toSecondary = NULL;
      layer->fromSecondary = NULL;
      break;

    case PIXEL_FORMAT_ARGB32:
      // Direct rendering if possible, bail to PRGB32 if ARGB32 is not
      // implemented for particular task.
      layer->type = RASTER_LAYER_TYPE_DIRECT32;
      layer->bytesPerPixel = 4;

      layer->secondaryFormat = PIXEL_FORMAT_NULL;
      layer->toSecondary = RasterEngine::functionMap->composite
        [OPERATOR_SRC][PIXEL_FORMAT_PRGB32].vspan[PIXEL_FORMAT_ARGB32];
      layer->fromSecondary = RasterEngine::functionMap->composite
        [OPERATOR_SRC][PIXEL_FORMAT_ARGB32].vspan[PIXEL_FORMAT_PRGB32];
      break;

    case PIXEL_FORMAT_A8:
      // TODO: Not implemented.
      FOG_ASSERT(0);
      break;

    case PIXEL_FORMAT_I8:
      // I8 format is not supported by the Painter and must be catched earlier.
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Serializers]
// ============================================================================

// Called by serializers to ensure that pattern context is created if needed.
#define RASTER_SERIALIZE_ENSURE_PATTERN() \
  FOG_BEGIN_MACRO \
    if (ctx.ops.sourceType == PAINTER_SOURCE_PATTERN && !_getPatternContext()) return; \
  FOG_END_MACRO

void RasterPaintEngine::_serializeBoxes(const IntBox* box, sysuint_t count)
{
  RASTER_SERIALIZE_ENSURE_PATTERN();

  // Singlethreaded.
  if (isSingleThreaded())
  {
    _renderBoxes(&ctx, box, count);
  }
  // Multithreaded.
  else
  {
    while (count > 0)
    {
      _beforeNewAction();

      sysuint_t n = Math::min<sysuint_t>(count, 128);
      RasterPaintCmdBoxes* cmd = _createCommand<RasterPaintCmdBoxes>(sizeof(RasterPaintCmdBoxes) - sizeof(IntBox) + n * sizeof(IntBox));
      if (!cmd) return;

      cmd->count = n;
      for (sysuint_t j = 0; j < n; j++) cmd->boxes[j] = box[j];

      cmd->_initPaint(&ctx);
      _postCommand(cmd);

      count -= n;
      box += n;
    }
  }
}

void RasterPaintEngine::_serializeImage(const IntRect& dst, const Image& image, const IntRect& src)
{
  // Singlethreaded.
  if (isSingleThreaded())
  {
    _renderImage(&ctx, dst, image, src);
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    RasterPaintCmdImage* cmd = _createCommand<RasterPaintCmdImage>();
    if (!cmd) return;

    cmd->dst = dst;
    cmd->src = src;
    cmd->image.init(image);
    cmd->_initBlit(&ctx);
    _postCommand(cmd);
  }
}

void RasterPaintEngine::_serializeImageAffine(const DoublePoint& pt, const Image& image, const IntRect* irect)
{
  FOG_ASSERT(!image.isEmpty());

  // Create new transformation matrix (based on current matrix and point
  // where the image should be drawn).
  const DoubleMatrix& tr = ctx.workMatrix;
  DoublePoint pt_(pt);
  tr.transformPoint(&pt_.x, &pt_.y);
  DoubleMatrix matrix(tr.sx, tr.shy, tr.shx, tr.sy, pt_.x, pt_.y);

  IntRect ir(0, 0, image.getWidth(), image.getHeight());
  if (irect) ir = *irect;

  if (!(OperatorCharacteristics[ctx.ops.op] & OPERATOR_CHAR_UNBOUND))
  {
    // Compositing operator is BOUND so we can take advantage of it. We create
    // new transformation matrix and then we use RasterPaintRenderImageAffine
    // renderer that fetches image using SPRAD_NONE and then blends fetched data
    // with target buffer.

    // Singlethreaded.
    if (isSingleThreaded())
    {
      RasterRenderImageAffineBound renderer;
      if (!renderer.init(image, ir, matrix, ctx.clipBox, ctx.hints.imageInterpolation)) return;

      renderer.render(&ctx);
    }
    // Multithreaded.
    else
    {
      _beforeNewAction();

      RasterPaintCmdImageAffineBound* cmd = _createCommand<RasterPaintCmdImageAffineBound>();
      if (!cmd) return;

      if (!cmd->renderer.instance().init(image, ir, matrix, ctx.clipBox, ctx.hints.imageInterpolation))
      {
        cmd->release(&ctx);
        return;
      }

      cmd->_initBlit(&ctx);
      _postCommand(cmd, NULL);
    }
  }
  else
  {
    // Compositing operator is not BOUND so we use classic rasterizer to
    // rasterize the image rectangle. We set painter source to pattern for
    // this job.

    // Make the path.
    tmpPath.clear();
    tmpPath.addRect(DoubleRect(pt.x, pt.y, (double)ir.w, (double)ir.h));

    // Singlethreaded.
    if (isSingleThreaded())
    {
      if (_rasterizePath(tmpPath, false))
      {
        // Save current pattern context (it will be replaced by context created
        // for the image).
        uint32_t oldSourceType = ctx.ops.sourceType;
        RasterEngine::PatternContext* oldPctx = ctx.pctx;

        // Create new pattern context (based on the image).
        RasterEngine::PatternContext imagectx;
        imagectx.initialized = false;
        RasterEngine::functionMap->pattern.texture_init_blit(&imagectx, 
          image, ir, matrix, SPREAD_PAD, ctx.hints.imageInterpolation);

        ctx.ops.sourceType = PAINTER_SOURCE_PATTERN;
        ctx.pctx = &imagectx;

        // Render path using specific pattern context.
        _renderPath(&ctx, ras);

        // Destroy pattern context.
        imagectx.destroy(&imagectx);

        // Restore old pattern context and source type.
        ctx.ops.sourceType = oldSourceType;
        ctx.pctx = oldPctx;
      }
    }
    // Multithreaded.
    else
    {
      _beforeNewAction();

      // This is a bit code duplication (see singlethreaded mode), but it quite
      // different. Here we are not interested about rectangle rasterization and
      // replacing pattern context in ctx, but we instead serialize path command
      // with new pattern context (current context will not be replaced at all).

      // Create new pattern context (based on the image).
      RasterEngine::PatternContext* imagectx =
        reinterpret_cast<RasterEngine::PatternContext*>(
          blockAllocator.alloc(sizeof(RasterEngine::PatternContext)));
      if (!imagectx) return;

      imagectx->initialized = false;
      RasterEngine::functionMap->pattern.texture_init_blit(
        imagectx, image, ir, matrix, SPREAD_PAD, ctx.hints.imageInterpolation);
      imagectx->refCount.init(1);

      RasterPaintCmdPath* cmd = _createCommand<RasterPaintCmdPath>();
      if (!cmd)
      {
        imagectx->destroy(imagectx);
        blockAllocator.free(imagectx);
        return;
      }

      RasterPaintCalcFillPath* clc = _createCalc<RasterPaintCalcFillPath>();
      if (!clc) { cmd->release(&ctx); return; }

      clc->relatedTo = cmd;
      clc->path.init(tmpPath);
      clc->matrix.init(ctx.workMatrix);
      clc->transformType = ctx.hints.transformType;
      clc->fillMode = FILL_EVEN_ODD;
      clc->approximationScale = ctx.approximationScale;

      cmd->status.init(RASTER_COMMAND_WAIT);
      cmd->calculation = clc;
      cmd->ras = NULL; // Will be initialized by calculation.

      // Init paint specialized for affine blit.
      cmd->ops = ctx.ops;
      cmd->ops.sourceType = PAINTER_SOURCE_PATTERN;
      cmd->pctx = imagectx;

      _postCommand(cmd, clc);
    }
  }
}

void RasterPaintEngine::_serializeGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip)
{
  IntBox boundingBox = ctx.clipBox;

  if (clip)
  {
    IntBox::intersect(boundingBox, boundingBox, IntBox(*clip));
    if (!boundingBox.isValid()) return;
  }

  RASTER_SERIALIZE_ENSURE_PATTERN();

  // Singlethreaded.
  if (isSingleThreaded())
  {
    _renderGlyphSet(&ctx, pt, glyphSet, boundingBox);
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    RasterPaintCmdGlyphSet* cmd = _createCommand<RasterPaintCmdGlyphSet>();
    if (!cmd) return;

    cmd->pt = pt;
    cmd->boundingBox = boundingBox;
    cmd->glyphSet.init(glyphSet);
    cmd->_initPaint(&ctx);
    _postCommand(cmd);
  }
}

void RasterPaintEngine::_serializePath(const DoublePath& path, bool stroke)
{
  RASTER_SERIALIZE_ENSURE_PATTERN();

  // Singlethreaded.
  if (isSingleThreaded())
  {
    if (_rasterizePath(path, stroke)) _renderPath(&ctx, ras);
  }
  // Multithreaded.
  else
  {
    _beforeNewAction();

    RasterPaintCmdPath* cmd = _createCommand<RasterPaintCmdPath>();
    if (!cmd) return;

    cmd->status.init(RASTER_COMMAND_WAIT);
    cmd->ras = NULL; // Will be initialized by calculation.

    if (stroke)
    {
      RasterPaintCalcStrokePath* clc = _createCalc<RasterPaintCalcStrokePath>();
      if (!clc) { cmd->release(&ctx); return; }

      clc->relatedTo = cmd;
      clc->path.init(path);
      clc->matrix.init(ctx.workMatrix);
      clc->clipBox = ctx.clipBox;
      clc->transformType = ctx.hints.transformType;
      clc->stroker.initCustom2(ctx.strokeParams, ctx.approximationScale);

      cmd->calculation = clc;
      cmd->_initPaint(&ctx);
      _postCommand(cmd, clc);
    }
    else
    {
      RasterPaintCalcFillPath* clc = _createCalc<RasterPaintCalcFillPath>();
      if (!clc) { cmd->release(&ctx); return; }

      clc->relatedTo = cmd;
      clc->path.init(path);
      clc->matrix.init(ctx.workMatrix);
      clc->clipBox = ctx.clipBox;
      clc->transformType = ctx.hints.transformType;
      clc->fillMode = ctx.hints.fillMode;
      clc->approximationScale = ctx.approximationScale;

      cmd->calculation = clc;
      cmd->_initPaint(&ctx);
      _postCommand(cmd, clc);
    }
  }
}

void RasterPaintEngine::_postCommand(RasterPaintCmd* cmd, RasterPaintCalc* clc)
{
#if defined(FOG_DEBUG_RASTER_COMMANDS)
  fog_debug("Fog::Painter::_postCommand() - posting...");
#endif // FOG_DEBUG_RASTER_COMMANDS

  if (FOG_UNLIKELY(ctx.changed))
  {
    uint32_t changed = ctx.changed;
    ctx.changed = 0;

    if (changed & RASTER_CHANGED_REGION)
    {
      RasterPaintCmdRegionChange* ccmd = _createCommand<RasterPaintCmdRegionChange>();
      ccmd->metaOrigin = ctx.metaOrigin;
      ccmd->metaRegion = ctx.metaRegion;
      ccmd->workOrigin = ctx.workOrigin;
      ccmd->workRegion = ctx.workRegion;
      _postCommand(ccmd);
    }
  }

  if (clc)
  {
    sysuint_t pos = workerManager->calcPosition;
    workerManager->calcData[pos] = clc;
    workerManager->calcPosition++;
  }

  {
    sysuint_t pos = workerManager->cmdPosition;
    workerManager->cmdData[pos] = cmd;
    workerManager->cmdPosition++;

    if ((pos & 15) == 0 && workerManager->waitingWorkers.get() > 0)
    {
#if defined(FOG_DEBUG_RASTER_SYNCHRONIZATION)
      fog_debug("Fog::Painter::_postCommand() - waking up...");
#endif // FOG_DEBUG_RASTER_SYNCHRONIZATION
      AutoLock locked(workerManager->lock);
      workerManager->wakeUpScheduled(NULL);
    }
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Rasterizer]
// ============================================================================

bool RasterPaintEngine::_rasterizePath(const DoublePath& path, bool stroke)
{
  // Use transformation matrix only if it makes sense.
  const DoubleMatrix* matrix = (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
    ? matrix = &ctx.workMatrix
    : NULL;

  DoublePath dst;

  ras->reset();
  ras->setClipBox(ctx.clipBox);

  bool noTransform = (
    ctx.hints.transformType == RASTER_TRANSFORM_EXACT &&
    ctx.workTranslate.x == 0 &&
    ctx.workTranslate.y == 0);

  if (stroke)
  {
    // Stroke mode. Stroke will flatten the path.
    Stroker(ctx.strokeParams, ctx.approximationScale).stroke(dst, path);

    if (ctx.hints.transformType >= RASTER_TRANSFORM_AFFINE)
    {
      dst.applyMatrix(ctx.workMatrix);
    }
    else if (!noTransform)
    {
      dst.translate(ctx.workMatrix.tx, ctx.workMatrix.ty);
    }

    // Stroke not respects fill mode set in caps
    // state, instead we are using FILL_NON_ZERO.
    ras->setFillRule(FILL_NON_ZERO);
  }
  else
  {
    // Fill mode.
    path.flattenTo(dst, noTransform ? NULL : &ctx.workMatrix, ctx.approximationScale);

    // Fill respects fill mode set in caps state.
    ras->setFillRule(ctx.hints.fillMode);
  }

  ras->addPath(dst);
  ras->finalize();

  return ras->hasCells();
}

// ============================================================================
// [Fog::RasterPaintEngine - Renderer]
// ============================================================================

static const int pixelFormatGroupId[] =
{
   1, // PIXEL_FORMAT_PRGB32
  -1, // PIXEL_FORMAT_ARGB32
   1, // PIXEL_FORMAT_XRGB32
  -2, // PIXEL_FORMAT_A8
  -3  // PIXEL_FORMAT_I8
};

// Get whether blit of two pixel formats is fully opaque operation (dst pixel
// is not needed in this case). This method is used to optimize some fast paths.
static FOG_INLINE bool isRawOpaqueBlit(uint32_t dstFormat, uint32_t srcFormat, uint32_t op)
{
  if ((op == OPERATOR_SRC || (op == OPERATOR_SRC_OVER && srcFormat == PIXEL_FORMAT_XRGB32)) &&
      (pixelFormatGroupId[dstFormat] == pixelFormatGroupId[srcFormat]))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Fill rectangles aligned to a pixel grid.
//
// Input data characteristics:
// - Already clipped to SIMPLE or COMPLEX clip region.
void RasterPaintEngine::_renderBoxes(RasterPaintContext* ctx, const IntBox* box, sysuint_t count)
{
#define RENDER_LOOP(NAME, BPP, CODE) \
  for (sysuint_t i = 0; i < count; i++) \
  { \
    int x1 = box[i].getX1(); \
    int y1 = box[i].getY1(); \
    int y2 = box[i].getY2(); \
    \
    int w = box[i].getWidth(); \
    if (w <= 0) continue; \
    \
    if (delta != 1) y1 = alignToDelta(y1, offset, delta); \
    if (y1 >= y2) continue; \
    \
    uint8_t* pCur = pixels + \
                    (sysint_t)(uint)y1 * stride + \
                    (sysint_t)(uint)x1 * BPP; \
    do { \
      CODE \
      \
      pCur += strideWithDelta; \
      y1 += delta; \
    } while (y1 < y2); \
  }

  if (!count) return;

  uint8_t* pixels = ctx->layer.pixels;
  sysint_t stride = ctx->layer.stride;

  int offset = ctx->offset;
  int delta = ctx->delta;
  sysint_t strideWithDelta = stride * delta;

  switch (ctx->ops.sourceType)
  {
    // Solid source type.
    case PAINTER_SOURCE_ARGB:
    {
      RasterEngine::CSpanFn cspan = ctx->funcs->cspan;

      switch (ctx->layer.type)
      {
        case RASTER_LAYER_TYPE_DIRECT32:
        case RASTER_LAYER_TYPE_INDIRECT32:
          if (cspan != NULL)
          {
            RENDER_LOOP(bltOpaque, 4,
            {
              cspan(pCur, &ctx->solid, (sysuint_t)w, &ctx->closure);
            });
          }
          else
          {
            cspan = RasterEngine::getCompositeFuncs(
              ctx->layer.secondaryFormat, ctx->ops.op)->cspan;

            RENDER_LOOP(bltOpaque, 4,
            {
              ctx->layer.toSecondary(pCur, pCur, w, NULL);
              cspan(pCur, &ctx->solid, (sysuint_t)w, &ctx->closure);
              ctx->layer.fromSecondary(pCur, pCur, w, NULL);
            });
          }
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // Pattern source type.
    case PAINTER_SOURCE_PATTERN:
    {
      RasterEngine::PatternContext* pctx = ctx->pctx;
      if (!pctx) return;

      uint32_t format = ctx->layer.format;

      RasterEngine::VSpanFn vspan;

      // FastPath: Do not copy pattern to extra buffer if compositing operator
      // is OPERATOR_SRC. We need to match pixel formats and make sure that
      // operator is OPERATOR_SRC or OPERATOR_SRC_OVER without alpha channel.
      if (isRawOpaqueBlit(format, pctx->format, ctx->ops.op))
      {
        // NOTE: isRawOpaqueBlit can return true for PRGB <- XRGB operation, but
        // texture fetchers always set alpha byte to 0xFF. In case that fetcher
        // not return the buffer we provided (aka DO-NOT-COPY fast path) we must
        // use blitter to copy these data to the raster.
        //
        // Vspan function ensures that alpha byte will be set to 0xFF on alpha
        // enabled pixel formats.
        vspan = RasterEngine::getCompositeFuncs(
          format, OPERATOR_SRC)->vspan[pctx->format];

        // OPERATOR_SRC vspan must be always supported for any format
        // combination.
        FOG_ASSERT(vspan != NULL);

        switch (ctx->layer.type)
        {
          case RASTER_LAYER_TYPE_DIRECT32:
            RENDER_LOOP(bltPatternFastDirect32, 4,
            {
              uint8_t* f = pctx->fetch(pctx, pCur, x1, y1, w);
              if (f != pCur) vspan(pCur, f, w, &ctx->closure);
            })
            break;

          case RASTER_LAYER_TYPE_INDIRECT32:
            RENDER_LOOP(bltPatternFastIndirect32, 4,
            {
              uint8_t* f = pctx->fetch(pctx, pCur, x1, y1, w);
              vspan(pCur, f, w, &ctx->closure);
            })
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }
      }
      else
      {
        vspan = ctx->funcs->vspan[pctx->format];

        uint8_t* pBuf = ctx->getBuffer((uint)(ctx->clipBox.getWidth()) * 4);
        if (!pBuf) return;

        switch (ctx->layer.type)
        {
          case RASTER_LAYER_TYPE_DIRECT32:
          case RASTER_LAYER_TYPE_INDIRECT32:
            if (vspan != NULL)
            {
              RENDER_LOOP(bltPatternCommonDirect32, 4,
              {
                uint8_t* f = pctx->fetch(pctx, pBuf, x1, y1, w);
                vspan(pCur, f, w, &ctx->closure);
              });
            }
            else
            {
              vspan = RasterEngine::getCompositeFuncs(
                ctx->layer.secondaryFormat, ctx->ops.op)->vspan[pctx->format];

              RENDER_LOOP(bltPatternCommonIndirect32, 4,
              {
                uint8_t* f = pctx->fetch(pctx, pBuf, x1, y1, w);
                ctx->layer.toSecondary(pCur, pCur, w, NULL);
                vspan(pCur, f, w, &ctx->closure);
                ctx->layer.fromSecondary(pCur, pCur, w, NULL);
              });
            }
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }
      }
      break;
    }
  }
#undef RENDER_LOOP
}

// Blit image aligned to a pixel grid.
//
// Input data characteristics:
// - Already clipped to SIMPLE clip rectangle. If clip region is COMPLEX,
//   additional clipping must be performed by callee (here).
void RasterPaintEngine::_renderImage(RasterPaintContext* ctx, const IntRect& dst, const Image& image, const IntRect& src)
{
#define RENDER_LOOP(NAME, BPP, CODE) \
  /* Simple clip. */ \
  if (FOG_LIKELY(ctx->ops.clipType & RASTER_CLIP_SIMPLE)) \
  { \
    int x = dst.x; \
    \
    uint8_t* dstCur = ctx->layer.pixels + (sysint_t)(uint)(x) * BPP; \
    const uint8_t* srcCur = image_d->first + (sysint_t)(uint)(src.x) * image_d->bytesPerPixel; \
    \
    /* Singlethreaded rendering (delta == 1, offset == 0). */ \
    if (FOG_LIKELY(delta == 1)) \
    { \
      dstCur += (sysint_t)(uint)(y) * dstStride; \
      srcCur += (sysint_t)(uint)(src.y) * srcStride; \
    } \
    /* Multithreaded rendering (delta == X, offset == X). */ \
    else \
    { \
      int offset = ctx->offset; \
      y = alignToDelta(y, offset, delta); \
      if (y >= yMax) return; \
       \
      dstCur += (sysint_t)(uint)(y) * dstStride; \
      srcCur += (sysint_t)(uint)(src.y + y - dst.y) * srcStride; \
      \
      dstStride *= delta; \
      srcStride *= delta; \
    } \
    \
    sysint_t w = dst.w; \
    \
    do { \
      CODE \
      \
      dstCur += dstStride; \
      srcCur += srcStride; \
      y += delta; \
    } while (y < yMax); \
  } \
  /* Complex clip. */ \
  else \
  { \
    const IntBox* clipCur = ctx->workRegion.getData(); \
    const IntBox* clipTo; \
    const IntBox* clipNext; \
    const IntBox* clipEnd = clipCur + ctx->workRegion.getLength(); \
    \
    uint8_t* dstBase; \
    const uint8_t* srcBase; \
    \
    int xMin = dst.x; \
    int xMax = xMin + dst.w; \
    \
    if (delta == 1) \
    { \
      /* Advance clip pointer. */ \
NAME##_nodelta_advance: \
      while (clipCur->y2 <= y) \
      { \
        if (++clipCur == clipEnd) goto NAME##_end; \
      } \
      \
      /* Advance to end of the current span list (same y1, y2). */ \
      clipNext = clipCur + 1; \
      while (clipNext != clipEnd && clipCur->y1 == clipNext->y1) clipNext++; \
      \
      /* Skip some rows if needed. */ \
      if (y < clipCur->y1) \
      { \
        y = clipCur->y1; \
        if (y >= yMax) goto NAME##_end; \
      } \
      \
      /* Fix clip width (subtract clip rects if some rects are outside the paint) */ \
      while (clipCur->x2 <= xMin) \
      { \
        if (++clipCur == clipNext) \
        { \
          if (clipCur == clipEnd) \
            goto NAME##_end; \
          else \
            goto NAME##_nodelta_advance; \
        } \
      } \
      \
      clipTo = clipNext - 1; \
      while (clipTo->x1 >= xMax) \
      { \
        if (clipTo == clipCur) \
        { \
          clipCur = clipNext; \
          if (clipCur == clipEnd) \
            goto NAME##_end; \
          else \
            goto NAME##_nodelta_advance; \
        } \
        clipTo--; \
      } \
      \
      dstBase = ctx->layer.pixels + (sysint_t)(uint)(y) * dstStride; \
      srcBase = image_d->first + (sysint_t)(uint)(y + src.y - dst.y) * srcStride; \
      \
      for (; y < yMax; y++, dstBase += dstStride, srcBase += srcStride) \
      { \
        /* Advance clip pointer if needed. */ \
        if (FOG_UNLIKELY(y >= clipCur->y2)) \
        { \
          clipCur = clipNext; \
          if (clipCur == clipEnd) goto NAME##_end; \
          goto NAME##_nodelta_advance; \
        } \
        \
        const IntBox* cbox = clipCur; \
        int x1 = cbox->x1; \
        if (x1 < xMin) x1 = xMin; \
        \
        uint8_t* dstCur = dstBase + (sysint_t)(uint)(x1) * BPP; \
        const uint8_t* srcCur = srcBase + (sysint_t)(uint)(x1 + src.x - dst.x) * image_d->bytesPerPixel; \
        \
        while (cbox != clipTo) \
        { \
          sysint_t w = cbox->x2 - x1; \
          FOG_ASSERT(w > 0); \
          \
          CODE \
          \
          cbox++; \
          \
          uint adv = (uint)(cbox->x1 - x1); \
          x1 += adv; \
          \
          dstCur += adv * BPP; \
          srcCur += adv * image_d->bytesPerPixel; \
        } \
        \
        int x2 = cbox->x2; \
        if (x2 > xMax) x2 = xMax; \
        { \
          sysint_t w = x2 - x1; \
          FOG_ASSERT(w > 0); \
          \
          CODE \
        } \
      } \
    } \
    else \
    { \
      sysint_t srcStrideWithDelta = srcStride * delta; \
      sysint_t dstStrideWithDelta = dstStride * delta; \
      \
      y = alignToDelta(y, ctx->offset, delta); \
      \
      /* Advance clip pointer. */ \
NAME##_delta_advance: \
      while (clipCur->y2 <= y) \
      { \
        if (++clipCur == clipEnd) goto NAME##_end; \
      } \
      \
      /* Advance to end of the current span list (same y1, y2). */ \
      clipNext = clipCur + 1; \
      while (clipNext != clipEnd && clipCur->y1 == clipNext->y1) clipNext++; \
      \
      /* Skip some rows if needed. */ \
      if (y < clipCur->y1) \
      { \
        y = alignToDelta(clipCur->y1, ctx->offset, delta); \
        if (y >= yMax) goto NAME##_end; \
        if (y >= clipCur->y2) \
        { \
          clipCur = clipNext; \
          if (clipCur == clipEnd) goto NAME##_end; \
          goto NAME##_delta_advance; \
        } \
      } \
      \
      /* Fix clip width (subtract clip rects if some rects are outside the paint) */ \
      while (clipCur->x2 <= xMin) \
      { \
        if (++clipCur == clipNext) \
        { \
          if (clipCur == clipEnd) \
            goto NAME##_end; \
          else \
            goto NAME##_delta_advance; \
        } \
      } \
      \
      clipTo = clipNext - 1; \
      while (clipTo->x1 >= xMax) \
      { \
        if (clipTo == clipCur) \
        { \
          clipCur = clipNext; \
          if (clipCur == clipEnd) \
            goto NAME##_end; \
          else \
            goto NAME##_delta_advance; \
        } \
        clipTo--; \
      } \
      \
      dstBase = ctx->layer.pixels + (sysint_t)(uint)(y) * dstStride; \
      srcBase = image_d->first + (sysint_t)(uint)(y + src.y - dst.y) * srcStride; \
      \
      for (; y < yMax; y += delta, dstBase += dstStrideWithDelta, srcBase += srcStrideWithDelta) \
      { \
        /* Advance clip pointer if needed. */ \
        if (FOG_UNLIKELY(y >= clipCur->y2)) \
        { \
          clipCur = clipNext; \
          if (clipCur == clipEnd) goto NAME##_end; \
          goto NAME##_delta_advance; \
        } \
        \
        const IntBox* cbox = clipCur; \
        int x1 = cbox->x1; \
        if (x1 < xMin) x1 = xMin; \
        \
        uint8_t* dstCur = dstBase + (sysint_t)(uint)(x1) * BPP; \
        const uint8_t* srcCur = srcBase + (sysint_t)(uint)(x1 + src.x - dst.x) * image_d->bytesPerPixel; \
        \
        while (cbox != clipTo) \
        { \
          sysint_t w = cbox->x2 - x1; \
          FOG_ASSERT(w > 0); \
          \
          CODE \
          \
          cbox++; \
          \
          uint adv = (uint)(cbox->x1 - x1); \
          x1 += adv; \
          \
          dstCur += adv * BPP; \
          srcCur += adv * image_d->bytesPerPixel; \
        } \
        \
        int x2 = cbox->x2; \
        if (x2 > xMax) x2 = xMax; \
        { \
          sysint_t w = x2 - x1; \
          FOG_ASSERT(w > 0); \
          \
          CODE \
        } \
      } \
    } \
  } \
NAME##_end: \
  ; \

  ImageData* image_d = image._d;
  RasterEngine::VSpanFn vspan = ctx->funcs->vspan[image_d->format];

  sysint_t dstStride = ctx->layer.stride;
  sysint_t srcStride = image_d->stride;

  RasterEngine::Closure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = image._d->palette.getData();

  int y = dst.y;
  int yMax = dst.y + dst.h;
  int delta = ctx->delta;

  switch (ctx->layer.type)
  {
    case RASTER_LAYER_TYPE_DIRECT32:
    case RASTER_LAYER_TYPE_INDIRECT32:
      if (vspan != NULL)
      {
        RENDER_LOOP(bltImageDirect32, 4,
        {
          vspan(dstCur, srcCur, w, &closure);
        })
      }
      else
      {
        vspan = RasterEngine::getCompositeFuncs(
          ctx->layer.secondaryFormat, ctx->ops.op)->vspan[image_d->format];

        RENDER_LOOP(bltImageIndirect32, 4,
        {
          ctx->layer.toSecondary(dstCur, dstCur, w, NULL);
          vspan(dstCur, srcCur, w, &closure);
          ctx->layer.fromSecondary(dstCur, dstCur, w, NULL);
        })
      }
      break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

#undef RENDER_LOOP
}

void RasterPaintEngine::_renderGlyphSet(RasterPaintContext* ctx, const IntPoint& pt, const GlyphSet& glyphSet, const IntBox& boundingBox)
{
  // TODO: Hardcoded to A8 glyph format.
  // TODO: Clipping.

#define RENDER_LOOP(NAME, BPP, CODE) \
  for (sysuint_t i = 0; i < count; i++) \
  { \
    GlyphData* glyphd = glyphs[i]._d; \
    ImageData* bitmapd = glyphd->bitmap._d; \
    \
    int px1 = px + glyphd->offset.x; \
    int py1 = py + glyphd->offset.y; \
    int px2 = px1 + bitmapd->width; \
    int py2 = py1 + bitmapd->height; \
    \
    px += glyphd->advance; \
    \
    int x1 = px1; if (x1 < boundingBox.x1) x1 = boundingBox.x1; \
    int y1 = py1; if (y1 < boundingBox.y1) y1 = boundingBox.y1; \
    int x2 = px2; if (x2 > boundingBox.x2) x2 = boundingBox.x2; \
    int y2 = py2; if (y2 > boundingBox.y2) y2 = boundingBox.y2; \
    \
    if (delta != 1) y1 = alignToDelta(y1, offset, delta); \
    \
    int w = x2 - x1; if (w <= 0) continue; \
    int h = y2 - y1; if (h <= 0) continue; \
    \
    uint8_t* pCur = pixels; \
    pCur += (sysint_t)(uint)y1 * stride; \
    pCur += (sysint_t)(uint)x1 * BPP; \
    \
    sysint_t glyphStride = bitmapd->stride; \
    const uint8_t* pGlyph = bitmapd->first; \
    \
    pGlyph += (sysint_t)(uint)(y1 - py1) * glyphStride; \
    pGlyph += (sysint_t)(uint)(x1 - px1); \
    \
    if (delta != 1) glyphStride *= delta; \
    \
    do { \
      CODE \
      \
      pCur += strideWithDelta; \
      pGlyph += glyphStride; \
      y1 += delta; \
    } while (y1 < y2); \
  }

  if (!glyphSet.getLength()) return;

  int offset = ctx->offset;
  int delta = ctx->delta;

  const Glyph* glyphs = glyphSet.glyphs();
  sysuint_t count = glyphSet.getLength();

  int px = pt.x;
  int py = pt.y;

  uint8_t* pixels = ctx->layer.pixels;
  sysint_t stride = ctx->layer.stride;
  sysint_t strideWithDelta = stride * delta;

  RasterEngine::Closure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  switch (ctx->ops.sourceType)
  {
    // Solid source type.
    case PAINTER_SOURCE_ARGB:
    {
      RasterEngine::CSpanMskFn cspan_a8 = ctx->funcs->cspan_a8;

      switch (ctx->layer.type)
      {
        case RASTER_LAYER_TYPE_DIRECT32:
        case RASTER_LAYER_TYPE_INDIRECT32:
          if (FOG_LIKELY(cspan_a8 != NULL))
          {
            RENDER_LOOP(bltSolidDirect32, 4,
            {
              cspan_a8(pCur, &ctx->solid, pGlyph, w, &closure);
            })
          }
          else
          {
            cspan_a8 = RasterEngine::getCompositeFuncs(
              ctx->layer.secondaryFormat, ctx->ops.op)->cspan_a8;

            RENDER_LOOP(bltSolidIndirect32, 4,
            {
              ctx->layer.toSecondary(pCur, pCur, w, NULL);
              cspan_a8(pCur, &ctx->solid, pGlyph, w, &closure);
              ctx->layer.fromSecondary(pCur, pCur, w, NULL);
            })
          }
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // Pattern source type.
    case PAINTER_SOURCE_PATTERN:
    {
      RasterEngine::PatternContext* pctx = ctx->pctx;
      if (!pctx) return;

      RasterEngine::VSpanMskFn vspan_a8 = ctx->funcs->vspan_a8[pctx->format];

      switch (ctx->layer.type)
      {
        case RASTER_LAYER_TYPE_DIRECT32:
        case RASTER_LAYER_TYPE_INDIRECT32:
        {
          uint8_t* pbuf = ctx->getBuffer((uint)(ctx->clipBox.getWidth()) * 4);
          if (!pbuf) return;

          if (FOG_LIKELY(vspan_a8 != NULL))
          {
            RENDER_LOOP(bltPatternDirect32, 4,
            {
              uint8_t* f = pctx->fetch(pctx, pbuf, x1, y1, w);
              vspan_a8(pCur, f, pGlyph, w, &closure);
            })
          }
          else
          {
            vspan_a8 = RasterEngine::getCompositeFuncs(
              ctx->layer.secondaryFormat, ctx->ops.op)->vspan_a8[pctx->format];

            RENDER_LOOP(bltPatternIndirect32, 4,
            {
              uint8_t* f = pctx->fetch(pctx, pbuf, x1, y1, w);

              ctx->layer.toSecondary(pCur, pCur, w, NULL);
              vspan_a8(pCur, f, pGlyph, w, &closure);
              ctx->layer.fromSecondary(pCur, pCur, w, NULL);
            })
          }
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      break;
    }
  }
#undef RENDER_LOOP
}

void RasterPaintEngine::_renderPath(RasterPaintContext* ctx, Rasterizer* ras)
{
#define RENDER_LOOP(NAME, BPP, CODE) \
  /* Simple clip. */ \
  if (ctx->ops.clipType & RASTER_CLIP_SIMPLE) \
  { \
    pBase = ctx->layer.pixels + y * stride; \
    for (; y < y_end; y += delta, pBase += strideWithDelta) \
    { \
      uint numSpans = ras->sweepScanline(scanline, y); \
      if (numSpans == 0) continue; \
      \
      CODE \
    } \
  } \
  /* Complex clip. */ \
  else \
  { \
    const IntBox* clipCur = ctx->workRegion.getData(); \
    const IntBox* clipTo; \
    const IntBox* clipEnd = clipCur + ctx->workRegion.getLength(); \
    sysuint_t clipLen; \
    \
    /* Advance clip pointer. */ \
NAME##_advance: \
    while (clipCur->y2 <= y) \
    { \
      if (++clipCur == clipEnd) goto NAME##_end; \
    } \
    /* Advance to end of the current span list (same y1, y2). */ \
    clipTo = clipCur + 1; \
    while (clipTo != clipEnd && clipCur->y1 == clipTo->y1) clipTo++; \
    clipLen = (sysuint_t)(clipTo - clipCur); \
    \
    /* Skip some rows if needed. */ \
    if (y < clipCur->y1) \
    { \
      y = clipCur->y1; \
      if (ctx->id != -1) \
      { \
        y = alignToDelta(y, ctx->offset, delta); \
        if (y >= clipCur->y2) \
        { \
          clipCur = clipTo; \
          if (clipCur == clipEnd) goto NAME##_end; \
          goto NAME##_advance; \
        } \
      } \
    } \
    \
    pBase = ctx->layer.pixels + y * stride; \
    for (; y < y_end; y += delta, pBase += strideWithDelta) \
    { \
      /* Advance clip pointer if needed. */ \
      if (FOG_UNLIKELY(y >= clipCur->y2)) \
      { \
        clipCur = clipTo; \
        if (clipCur == clipEnd) goto NAME##_end; \
        goto NAME##_advance; \
      } \
      \
      uint numSpans = ras->sweepScanline(scanline, y, clipCur, clipLen); \
      if (numSpans == 0) continue; \
      \
      CODE \
    } \
  } \
NAME##_end: \
  ;

  Scanline32* scanline = &ctx->scanline;
  if (scanline->init(ras->getCellsBounds().x1, ras->getCellsBounds().x2) != ERR_OK) return;

  int y = ras->getCellsBounds().y1;
  int y_end = ras->getCellsBounds().y2;
  int delta = ctx->delta;

  if (ctx->id != -1)
  {
    y = alignToDelta(y, ctx->offset, delta);
    if (y >= y_end) return;
  }

  sysint_t stride = ctx->layer.stride;
  sysint_t strideWithDelta = stride * delta;

  uint8_t* pBase;
  uint8_t* pCur;

  RasterEngine::Closure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  uint sourceType = ctx->ops.sourceType;
  const RasterEngine::FunctionMap::CompositeFuncs* funcs = ctx->funcs;

  switch (sourceType)
  {
    // Solid source type.
    case PAINTER_SOURCE_ARGB:
    {
      switch (ctx->layer.type)
      {
        case RASTER_LAYER_TYPE_DIRECT32:
        case RASTER_LAYER_TYPE_INDIRECT32:
          if (funcs->cspan_a8_scanline)
          {
            RENDER_LOOP(bltSolidDirect32, 4,
            {
              funcs->cspan_a8_scanline(pBase, &ctx->solid, scanline->getSpansData(), numSpans, &closure);
            });
          }
          else
          {
            funcs = RasterEngine::getCompositeFuncs(
              ctx->layer.secondaryFormat, OPERATOR_SRC);

            RENDER_LOOP(bltSolidIndirect32, 4,
            {
              const Scanline32::Span* span = scanline->getSpansData();

              for (;;)
              {
                int x = span->x;
                int len = span->len;

                pCur = pBase + (uint)x * 4;

                if (len > 0)
                {
                  ctx->layer.toSecondary(pCur, pCur, len, NULL);
                  funcs->cspan_a8(pCur, &ctx->solid, span->covers, len, &closure);
                  ctx->layer.fromSecondary(pCur, pCur, len, NULL);
                }
                else
                {
                  len = -len;
                  FOG_ASSERT(len > 0);

                  uint32_t cover = (uint32_t)*(span->covers);

                  ctx->layer.toSecondary(pCur, pCur, len, NULL);
                  if (cover == 0xFF)
                    funcs->cspan(pCur, &ctx->solid, len, &closure);
                  else
                    funcs->cspan_a8_const(pCur, &ctx->solid, cover, len, &closure);
                  ctx->layer.fromSecondary(pCur, pCur, len, NULL);
                }

                if (--numSpans == 0) break;
                ++span;
              }
            });
          }
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }

    // Pattern source type.
    case PAINTER_SOURCE_PATTERN:
    {
      RasterEngine::PatternContext* pctx = ctx->pctx;
      if (!pctx) return;

      uint8_t* pBuf = ctx->getBuffer((uint)(ctx->clipBox.getWidth()) * 4);
      if (!pBuf) return;

      RasterEngine::VSpanFn vspan;
      RasterEngine::VSpanMskFn vspan_a8;
      RasterEngine::VSpanMskConstFn vspan_a8_const;

      vspan = funcs->vspan[pctx->format];
      vspan_a8 = funcs->vspan_a8[pctx->format];
      vspan_a8_const = funcs->vspan_a8_const[pctx->format];

      switch (ctx->layer.type)
      {
        case RASTER_LAYER_TYPE_DIRECT32:
        case RASTER_LAYER_TYPE_INDIRECT32:
          if (vspan          != NULL &&
              vspan_a8       != NULL &&
              vspan_a8_const != NULL)
          {
            RENDER_LOOP(bltPatternDirect32, 4,
            {
              const Scanline32::Span* span = scanline->getSpansData();

              for (;;)
              {
                int x = span->x;
                int len = span->len;

                pCur = pBase + (uint)x * 4;

                if (len > 0)
                {
                  uint8_t* f = pctx->fetch(pctx, pBuf, x, y, len);
                  vspan_a8(pCur, f, span->covers, len, &closure);
                }
                else
                {
                  len = -len;
                  FOG_ASSERT(len > 0);

                  uint32_t cover = (uint32_t)*(span->covers);
                  uint8_t* f = pctx->fetch(pctx, pBuf, x, y, len);

                  if (cover == 0xFF)
                    vspan(pCur, f, len, &closure);
                  else
                    vspan_a8_const(pCur, f, cover, len, &closure);
                }

                if (--numSpans == 0) break;
                ++span;
              }
            });
          }
          else
          {
            funcs = RasterEngine::getCompositeFuncs(
              ctx->layer.secondaryFormat, OPERATOR_SRC);

            vspan = funcs->vspan[pctx->format];
            vspan_a8 = funcs->vspan_a8[pctx->format];
            vspan_a8_const = funcs->vspan_a8_const[pctx->format];

            RENDER_LOOP(bltPatternIndirect32, 4,
            {
              const Scanline32::Span* span = scanline->getSpansData();

              for (;;)
              {
                int x = span->x;
                int len = span->len;

                pCur = pBase + (uint)x * 4;

                if (len > 0)
                {
                  ctx->layer.toSecondary(pCur, pCur, len, NULL);
                  uint8_t* f = pctx->fetch(pctx, pBuf, x, y, len);
                  vspan_a8(pCur, f, span->covers, len, &closure);
                  ctx->layer.fromSecondary(pCur, pCur, len, NULL);
                }
                else
                {
                  len = -len;
                  FOG_ASSERT(len > 0);

                  uint32_t cover = (uint32_t)*(span->covers);
                  uint8_t* f = pctx->fetch(pctx, pBuf, x, y, len);

                  ctx->layer.toSecondary(pCur, pCur, len, NULL);

                  if (cover == 0xFF)
                  {
                    vspan(pCur, f, len, &closure);
                  }
                  else
                  {
                    vspan_a8_const(pCur, f, cover, len, &closure);
                  }

                  ctx->layer.fromSecondary(pCur, pCur, len, NULL);
                }

                if (--numSpans == 0) break;
                ++span;
              }
            });
          }
          break;

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      break;
    }
  }
#undef RENDER_LOOP
}

} // Fog namespace
