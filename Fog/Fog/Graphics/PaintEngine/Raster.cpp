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
// [Fog::MemoryAllocator]
// ============================================================================

MemoryAllocator::MemoryAllocator() :
  blocks(NULL)
{
}

MemoryAllocator::~MemoryAllocator()
{
  reset();
}

void* MemoryAllocator::alloc(sysuint_t size)
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

void MemoryAllocator::reset()
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
// [Fog::RasterPaintClipState]
// ============================================================================

RasterPaintClipState::RasterPaintClipState()
{
  refCount.init(1);
}

RasterPaintClipState::RasterPaintClipState(const RasterPaintClipState& other) :
  metaOrigin(other.metaOrigin),
  userOrigin(other.userOrigin),
  workOrigin(other.workOrigin),

  metaRegion(other.metaRegion),
  userRegion(other.userRegion),
  workRegion(other.workRegion),

  clipBox(other.clipBox),

  metaRegionUsed(other.metaRegionUsed),
  userRegionUsed(other.userRegionUsed),
  workRegionUsed(other.workRegionUsed),
  clipSimple(other.clipSimple)
{
  refCount.init(1);
}

RasterPaintClipState::~RasterPaintClipState()
{
}

FOG_INLINE RasterPaintClipState* RasterPaintClipState::ref() const
{
  refCount.inc();
  return const_cast<RasterPaintClipState*>(this);
}

RasterPaintClipState& RasterPaintClipState::operator=(const RasterPaintClipState& other)
{
  metaOrigin = other.metaOrigin;
  userOrigin = other.userOrigin;
  workOrigin = other.workOrigin;

  metaRegion = other.metaRegion;
  userRegion = other.userRegion;
  workRegion = other.workRegion;

  clipBox = other.clipBox;

  metaRegionUsed = other.metaRegionUsed;
  userRegionUsed = other.userRegionUsed;
  workRegionUsed = other.workRegionUsed;
  clipSimple = other.clipSimple;

  return *this;
}

// ============================================================================
// [Fog::RasterPaintCapsState]
// ============================================================================

RasterPaintCapsState::RasterPaintCapsState()
{
  // We assume that size of data is QWORD and we like to know if this is not
  // true.
  FOG_ASSERT(sizeof(data) == 8);

  refCount.init(1);
  sourceType = PAINTER_SOURCE_ARGB;
}

RasterPaintCapsState::RasterPaintCapsState(const RasterPaintCapsState& other) :
  rops(other.rops),
  strokeParams(other.strokeParams),
  transform(other.transform),
  approximationScale(other.approximationScale),
  transformTranslateSaved(other.transformTranslateSaved),
  transformTranslateInt(other.transformTranslateInt)
{
  refCount.init(1);
  Memory::copy8B(data, other.data);

  switch (sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      solid.argb = other.solid.argb;
      solid.prgb = other.solid.prgb;
      break;
    case PAINTER_SOURCE_PATTERN:
      pattern.initCustom1(other.pattern.instance());
      break;
    case PAINTER_SOURCE_COLOR_FILTER:
      colorFilter.initCustom1(other.colorFilter.instance());
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

RasterPaintCapsState::~RasterPaintCapsState()
{
  switch (sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      break;
    case PAINTER_SOURCE_PATTERN:
      pattern.destroy();
      break;
    case PAINTER_SOURCE_COLOR_FILTER:
      colorFilter.destroy();
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

FOG_INLINE RasterPaintCapsState* RasterPaintCapsState::ref() const
{
  refCount.inc();
  return const_cast<RasterPaintCapsState*>(this);
}

RasterPaintCapsState& RasterPaintCapsState::operator=(const RasterPaintCapsState& other)
{
  switch (sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      break;
    case PAINTER_SOURCE_PATTERN:
      pattern.destroy();
      break;
    case PAINTER_SOURCE_COLOR_FILTER:
      colorFilter.destroy();
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  Memory::copy8B(data, other.data);

  rops = other.rops;
  strokeParams = other.strokeParams;
  transform = other.transform;
  approximationScale = other.approximationScale;
  transformTranslateSaved = other.transformTranslateSaved;
  transformTranslateInt = other.transformTranslateInt;

  switch (other.sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      solid.argb = other.solid.argb;
      solid.prgb = other.solid.prgb;
      break;
    case PAINTER_SOURCE_PATTERN:
      pattern.initCustom1(other.pattern.instance());
      break;
    case PAINTER_SOURCE_COLOR_FILTER:
      colorFilter.initCustom1(other.colorFilter.instance());
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return *this;
}

// ============================================================================
// [Fog::RasterPaintContext]
// ============================================================================

RasterPaintContext::RasterPaintContext()
{
  // Scanline must be reset before using it.
  scanline.reset();

  engine = NULL;

  clipState = NULL;
  capsState = NULL;

  pctx = NULL;

  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  id = -1;
  offset = 0;
  delta = 1;

  buffer = bufferStatic;
  bufferSize = FOG_ARRAY_SIZE(bufferStatic);
}

RasterPaintContext::~RasterPaintContext()
{
  if (buffer != bufferStatic) Memory::free(buffer);
}

uint8_t* RasterPaintContext::getBuffer(sysint_t size)
{
  if (size > bufferSize)
  {
    if (buffer != bufferStatic) Memory::free(buffer);

    // First dynamic memory will be twice as bufferStatic.
    sysint_t capacity = FOG_ARRAY_SIZE(bufferStatic) << 1;
    while (size < capacity) capacity <<= 1;

    buffer = (uint8_t*)Memory::alloc(size);
    if (buffer == NULL)
    {
      // Error, set to safe defaults and return NULL, painter can't continue
      // if this happenned.
      buffer = bufferStatic;
      bufferSize = FOG_ARRAY_SIZE(bufferStatic);
      return NULL;
    }

    bufferSize = capacity;
  }

  return buffer;
}

// ============================================================================
// [Fog::RasterRenderImageAffineBound]
// ============================================================================

bool RasterRenderImageAffineBound::init(
  const Image& image, const IntRect& irect,
  const DoubleMatrix& matrix,
  const IntBox& clipBox, int interpolationType)
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

  RasterPaintLayer* layer = ctx->layer;
  RasterPaintCapsState* capsState = ctx->capsState;
  RasterPaintClipState* clipState = ctx->clipState;

  // Create new pattern context (based on the image).
  uint8_t* pBuf = ctx->getBuffer((uint)(xmax - xmin) * 4);
  if (!pBuf) return;

  // Rasterize it using dda algorithm that matches image boundary with
  // antialiasing.
  //
  // (We don't need to antialias here, because compositing operator is bound so
  // the pattern fetched will be already antialiased - SPREAD_NONE).

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
  sysint_t stride = layer->stride;
  uint8_t* pBase = layer->pixels;
  uint8_t* pCur = NULL;

  RasterEngine::VSpanFn vspan = capsState->rops->vspan[(uint)ictx.format];

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
// [Fog::RasterPaintAction]
// ============================================================================

FOG_INLINE void RasterPaintAction::_free()
{
  engine->allocator.free(this);
}

// ============================================================================
// [Fog::RasterPaintCmd]
// ============================================================================

FOG_INLINE void RasterPaintCmd::_releaseObjects()
{
  engine->_derefClipState(clipState);
  engine->_derefCapsState(capsState);

  if (pctx && pctx->refCount.deref())
  {
    pctx->destroy(pctx);
    engine->allocator.free(pctx);
  }
}

// ============================================================================
// [Fog::RasterPaintCmdBoxes]
// ============================================================================

void RasterPaintCmdBoxes::run(RasterPaintContext* ctx)
{
  ctx->engine->_renderBoxes(ctx, boxes, count);
}

void RasterPaintCmdBoxes::release()
{
  _releaseObjects();
  _free();
}

// ============================================================================
// [Fog::RasterPaintCmdImage]
// ============================================================================

void RasterPaintCmdImage::run(RasterPaintContext* ctx)
{
  ctx->engine->_renderImage(ctx, dst, image.instance(), src);
}

void RasterPaintCmdImage::release()
{
  image.destroy();

  _releaseObjects();
  _free();
}

// ============================================================================
// [Fog::RasterPaintCmdImageAffineBound]
// ============================================================================

void RasterPaintCmdImageAffineBound::run(RasterPaintContext* ctx)
{
  renderer.instance().render(ctx);
}

void RasterPaintCmdImageAffineBound::release()
{
  renderer.destroy();
  _releaseObjects();
  _free();
}

// ============================================================================
// [Fog::RasterPaintCmdGlyphSet]
// ============================================================================

void RasterPaintCmdGlyphSet::run(RasterPaintContext* ctx)
{
  ctx->engine->_renderGlyphSet(ctx, pt, glyphSet.instance(), boundingBox);
}

void RasterPaintCmdGlyphSet::release()
{
  glyphSet.destroy();

  _releaseObjects();
  _free();
}

// ============================================================================
// [Fog::RasterPaintCmdPath]
// ============================================================================

void RasterPaintCmdPath::run(RasterPaintContext* ctx)
{
  ctx->engine->_renderPath(ctx, ras, textureBlit);
}

void RasterPaintCmdPath::release()
{
  Rasterizer::releaseRasterizer(ras);

  _releaseObjects();
  _free();
}

// ============================================================================
// [Fog::RasterPaintCalcPath]
// ============================================================================

void RasterPaintCalcPath::run(RasterPaintContext* ctx)
{
  RasterPaintCmdPath* cmd = reinterpret_cast<RasterPaintCmdPath*>(relatedTo);
  cmd->ras = Rasterizer::getRasterizer();

  bool ok = RasterPaintEngine::_rasterizePath(ctx, cmd->ras, path.instance(), stroke);
  cmd->status.set(ok ? COMMAND_STATUS_READY : COMMAND_STATUS_SKIP);
}

void RasterPaintCalcPath::release()
{
  path.destroy();
  _free();
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
  RasterPaintWorkerManager* mgr = engine->workerManager;

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
          ctx.layer = cmd->layer;
          ctx.clipState = cmd->clipState;
          ctx.capsState = cmd->capsState;
          ctx.pctx = cmd->pctx;

          clc->run(&ctx);
          clc->release();

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
          case COMMAND_STATUS_READY:
          {
            ctx.layer = cmd->layer;
            ctx.clipState = cmd->clipState;
            ctx.capsState = cmd->capsState;
            ctx.pctx = cmd->pctx;

            cmd->run(&ctx);
            if (cmd->refCount.deref()) cmd->release();

            cmdCurrent++;
            cont = 0;
            break;
          }

          case COMMAND_STATUS_SKIP:
          {
            cmdCurrent++;
            if (cmd->refCount.deref()) cmd->release();
            cont = 0;
            break;
          }

          case COMMAND_STATUS_WAIT:
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
  RasterPaintWorkerManager* mgr = engine->workerManager;

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
// [Fog::RasterPaintWorkerManager]
// ============================================================================

RasterPaintWorkerManager::RasterPaintWorkerManager() :
  allFinishedCondition(&lock),
  releaseEvent(NULL)
{
}

RasterPaintWorkerManager::~RasterPaintWorkerManager()
{
}

RasterPaintTask* RasterPaintWorkerManager::wakeUpScheduled(RasterPaintTask* calledFrom)
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

RasterPaintTask* RasterPaintWorkerManager::wakeUpSleeping(RasterPaintTask* calledFrom)
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

bool RasterPaintWorkerManager::isCompleted()
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

RasterPaintEngine::RasterPaintEngine(const ImageBuffer& buffer, uint32_t initFlags) :
  workerManager(NULL)
{
  // Setup primary context.
  ctx.engine = this;

  // Setup primary layer.
  main.pixels = buffer.data;
  main.width = buffer.width;
  main.height = buffer.height;
  main.format = buffer.format;
  main.stride = buffer.stride;

  ctx.layer = &main;
  _setupLayer(&main);

  // Setup primary rasterizer.
  ras = Rasterizer::getRasterizer();

  // Setup clip / caps state.
  ctx.clipState = new( allocator.alloc(sizeof(RasterPaintClipState)) ) RasterPaintClipState();
  ctx.capsState = new( allocator.alloc(sizeof(RasterPaintCapsState)) ) RasterPaintCapsState();

  _setClipDefaults();
  _setCapsDefaults();

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
  if (workerManager) setEngine(PAINTER_ENGINE_RASTER_ST);

  _deleteStates();

  // This is our context, it's imposible that other thread is using it!
  if (ctx.pctx)
  {
    if (ctx.pctx->initialized) ctx.pctx->destroy(ctx.pctx);

    // This is not important in release mode. In debug mode it's needed to
    // free it because of assertion in Fog::MemoryAllocator.
    allocator.free(ctx.pctx);
  }

  _derefClipState(ctx.clipState);
  _derefCapsState(ctx.capsState);

  Rasterizer::releaseRasterizer(ras);
}

// ============================================================================
// [Fog::RasterPaintEngine - Width / Height / Format]
// ============================================================================

int RasterPaintEngine::getWidth() const
{
  return main.width;
}

int RasterPaintEngine::getHeight() const
{
  return main.height;
}

int RasterPaintEngine::getFormat() const
{
  return main.format;
}

// ============================================================================
// [Fog::RasterPaintEngine - Engine / Flush]
// ============================================================================

uint32_t RasterPaintEngine::getEngine() const
{
  return (workerManager != NULL)
    ? PAINTER_ENGINE_RASTER_MT
    : PAINTER_ENGINE_RASTER_ST;
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

    workerManager = new(std::nothrow) RasterPaintWorkerManager;
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
      RasterPaintTask* task = workerManager->tasks[i].instancep();
      task->ctx.engine = this;
      task->ctx.id = i;
      task->ctx.offset = i;
      task->ctx.delta = count;
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
  RasterPaintCapsState* capsState = ctx.capsState;

  switch (hint)
  {
    case PAINTER_HINT_ANTIALIASING_QUALITY:
      return (int)capsState->aaQuality;

    case PAINTER_HINT_IMAGE_INTERPOLATION:
      return (int)capsState->imageInterpolation;

    case PAINTER_HINT_GRADIENT_INTERPOLATION:
      return (int)capsState->gradientInterpolation;

    case PAINTER_HINT_OUTLINE_TEXT:
      return (int)capsState->forceOutlineText;

    default:
      return -1;
  }
}

void RasterPaintEngine::setHint(uint32_t hint, int value)
{
  RasterPaintCapsState* capsState = ctx.capsState;

  switch (hint)
  {
    case PAINTER_HINT_ANTIALIASING_QUALITY:
      if ((capsState->aaQuality == value) | ((uint)value >= ANTI_ALIASING_COUNT)) return;
      if (!(capsState = _detachCapsState())) return;

      capsState->aaQuality = (uint8_t)value;
      break;

    case PAINTER_HINT_IMAGE_INTERPOLATION:
      if ((capsState->imageInterpolation == value) | ((uint)value >= INTERPOLATION_INVALID)) return;
      if (!(capsState = _detachCapsState())) return;

      capsState->imageInterpolation = (uint8_t)value;
      break;

    case PAINTER_HINT_GRADIENT_INTERPOLATION:
      if ((capsState->gradientInterpolation == value) | ((uint)value >= INTERPOLATION_INVALID)) return;
      if (!(capsState = _detachCapsState())) return;

      capsState->gradientInterpolation = (uint8_t)value;
      break;

    case PAINTER_HINT_OUTLINE_TEXT:
      if ((capsState->forceOutlineText == value) | ((uint)value >= 2)) return;
      if (!(capsState = _detachCapsState())) return;

      capsState->forceOutlineText = (uint8_t)value;
      break;

    default:
      break;
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Meta]
// ============================================================================

void RasterPaintEngine::setMetaVariables(
  const IntPoint& metaOrigin,
  const Region& metaRegion,
  bool useMetaRegion,
  bool reset)
{
  RasterPaintClipState* clipState;
  if (!(clipState = _detachClipState())) return;

  clipState->metaOrigin = metaOrigin;
  clipState->metaRegion = metaRegion;
  clipState->metaRegionUsed = useMetaRegion;

  if (reset)
  {
    _deleteStates();

    clipState->userOrigin.set(0, 0);
    clipState->userRegion.clear();
    clipState->userRegionUsed = false;

    // We must set workOrigin here, because _setCapsDefaults() will set default
    // transformation matrix to identity + workOrigin.
    clipState->workOrigin.set(metaOrigin);
    if (_detachCapsState()) _setCapsDefaults();
  }

  _updateWorkRegion();
}

void RasterPaintEngine::setMetaOrigin(const IntPoint& pt)
{
  RasterPaintClipState* clipState = ctx.clipState;

  if (clipState->metaOrigin == pt) return;
  if (!(clipState = _detachClipState())) return;

  clipState->metaOrigin = pt;
  _updateWorkRegion();
}

void RasterPaintEngine::setUserOrigin(const IntPoint& pt)
{
  RasterPaintClipState* clipState = ctx.clipState;

  if (clipState->userOrigin == pt) return;
  if (!(clipState = _detachClipState())) return;

  clipState->userOrigin = pt;
  _updateWorkRegion();
}

void RasterPaintEngine::translateMetaOrigin(const IntPoint& pt)
{
  if (pt.x == 0 && pt.y == 0) return;

  RasterPaintClipState* clipState;
  if (!(clipState = _detachClipState())) return;

  clipState->metaOrigin += pt;
  _updateWorkRegion();
}

void RasterPaintEngine::translateUserOrigin(const IntPoint& pt)
{
  if (pt.x == 0 && pt.y == 0) return;

  RasterPaintClipState* clipState;
  if (!(clipState = _detachClipState())) return;

  clipState->userOrigin += pt;
  _updateWorkRegion();
}

void RasterPaintEngine::setUserRegion(const IntRect& r)
{
  RasterPaintClipState* clipState;
  if (!(clipState = _detachClipState())) return;

  clipState->userRegion = r;
  clipState->userRegionUsed = true;
  _updateWorkRegion();
}

void RasterPaintEngine::setUserRegion(const Region& r)
{
  RasterPaintClipState* clipState;
  if (!(clipState = _detachClipState())) return;

  clipState->userRegion = r;
  clipState->userRegionUsed = true;
  _updateWorkRegion();
}

void RasterPaintEngine::resetMetaVars()
{
  RasterPaintClipState* clipState;
  if (!(clipState = _detachClipState())) return;

  clipState->metaOrigin.set(0, 0);
  clipState->metaRegion.clear();
  clipState->metaRegionUsed = false;
  _updateWorkRegion();
}

void RasterPaintEngine::resetUserVars()
{
  RasterPaintClipState* clipState;
  if (!(clipState = _detachClipState())) return;

  clipState->userOrigin.set(0, 0);
  clipState->userRegion.clear();
  clipState->userRegionUsed = false;
  _updateWorkRegion();
}

IntPoint RasterPaintEngine::getMetaOrigin() const
{
  RasterPaintClipState* clipState = ctx.clipState;

  return clipState->metaOrigin;
}

IntPoint RasterPaintEngine::getUserOrigin() const
{
  RasterPaintClipState* clipState = ctx.clipState;

  return clipState->userOrigin;
}

Region RasterPaintEngine::getMetaRegion() const
{
  RasterPaintClipState* clipState = ctx.clipState;

  return clipState->metaRegion;
}

Region RasterPaintEngine::getUserRegion() const
{
  RasterPaintClipState* clipState = ctx.clipState;

  return clipState->userRegion;
}

bool RasterPaintEngine::isMetaRegionUsed() const
{
  RasterPaintClipState* clipState = ctx.clipState;

  return clipState->metaRegionUsed;
}

bool RasterPaintEngine::isUserRegionUsed() const
{
  RasterPaintClipState* clipState = ctx.clipState;

  return clipState->userRegionUsed;
}

// ============================================================================
// [Fog::RasterPaintEngine - Operator]
// ============================================================================

uint32_t RasterPaintEngine::getOperator() const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  return capsState->op;
}

void RasterPaintEngine::setOperator(uint32_t op)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if ((capsState->op == op) | ((uint)op >= OPERATOR_COUNT)) return;

  if (!(capsState = _detachCapsState())) return;
  capsState->op = op;
  capsState->rops = RasterEngine::getCompositeFuncs(ctx.layer->format, op);
}

// ============================================================================
// [Fog::RasterPaintEngine - Source]
// ============================================================================

uint32_t RasterPaintEngine::getSourceType() const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  return capsState->sourceType;
}

err_t RasterPaintEngine::getSourceArgb(Argb& argb) const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  switch (capsState->sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      argb.set(capsState->solid.argb);
      return ERR_OK;

    default:
      argb.set(0x00000000);
      return ERR_RT_INVALID_CONTEXT;
  }
}

err_t RasterPaintEngine::getSourcePattern(Pattern& pattern) const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  switch (capsState->sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      return pattern.setColor(capsState->solid.argb);

    case PAINTER_SOURCE_PATTERN:
      pattern = capsState->pattern.instance();
      return ERR_OK;

    default:
      pattern.reset();
      return ERR_RT_INVALID_CONTEXT;
  }
}

void RasterPaintEngine::setSource(Argb argb)
{
  RasterPaintCapsState* capsState;
  if (!(capsState = _detachCapsState())) return;

  // Destroy old source object if needed.
  switch (capsState->sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      goto wasArgb;
    case PAINTER_SOURCE_PATTERN:
      capsState->pattern.destroy();
      _resetPatternRasterPaintContext();
      break;
    case PAINTER_SOURCE_COLOR_FILTER:
      capsState->colorFilter.destroy();
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  capsState->sourceType = PAINTER_SOURCE_ARGB;

wasArgb:
  capsState->solid.argb = argb.value;
  capsState->solid.prgb = argb.value;

  if (argb.a == 0xFF) return;
  capsState->solid.prgb = ArgbUtil::premultiply(argb.value);
}

void RasterPaintEngine::setSource(const Pattern& pattern)
{
  if (pattern.isSolid())
  {
    setSource(pattern.getColor());
    return;
  }

  RasterPaintCapsState* capsState;
  if (!(capsState = _detachCapsState())) return;

  // Destroy old source object if needed.
  switch (capsState->sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      break;
    case PAINTER_SOURCE_PATTERN:
      capsState->pattern.instance() = pattern;
      goto done;
    case PAINTER_SOURCE_COLOR_FILTER:
      capsState->colorFilter.destroy();
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  capsState->sourceType = PAINTER_SOURCE_PATTERN;
  capsState->pattern.initCustom1(pattern);

done:
  _resetPatternRasterPaintContext();
}

void RasterPaintEngine::setSource(const ColorFilter& colorFilter)
{
  RasterPaintCapsState* capsState;
  if (!(capsState = _detachCapsState())) return;

  // Destroy old source object if needed.
  switch (capsState->sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      break;
    case PAINTER_SOURCE_PATTERN:
      capsState->pattern.destroy();
      _resetPatternRasterPaintContext();
      break;
    case PAINTER_SOURCE_COLOR_FILTER:
      capsState->colorFilter.instance() = colorFilter;
      goto done;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  capsState->sourceType = PAINTER_SOURCE_COLOR_FILTER;
  capsState->colorFilter.initCustom1(colorFilter);
done:
  ;
}

// ============================================================================
// [Fog::RasterPaintEngine - Fill Parameters]
// ============================================================================

uint32_t RasterPaintEngine::getFillMode() const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  return capsState->fillMode;
}

void RasterPaintEngine::setFillMode(uint32_t mode)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if ((capsState->fillMode == mode) | ((uint)mode >= FILL_MODE_COUNT)) return;
  if (!(capsState = _detachCapsState())) return;

  capsState->fillMode = mode;
}

// ============================================================================
// [Fog::RasterPaintEngine - Stroke Parameters]
// ============================================================================

void RasterPaintEngine::getStrokeParams(StrokeParams& strokeParams) const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  strokeParams = capsState->strokeParams;
}

void RasterPaintEngine::setStrokeParams(const StrokeParams& strokeParams)
{
  RasterPaintCapsState* capsState;
  if (!(capsState = _detachCapsState())) return;

  ctx.capsState->strokeParams = strokeParams;

  if (ctx.capsState->strokeParams.getStartCap() >= LINE_CAP_COUNT) ctx.capsState->strokeParams.setStartCap(LINE_CAP_DEFAULT);
  if (ctx.capsState->strokeParams.getEndCap() >= LINE_CAP_COUNT) ctx.capsState->strokeParams.setEndCap(LINE_CAP_DEFAULT);
  if (ctx.capsState->strokeParams.getLineJoin() >= LINE_JOIN_COUNT) ctx.capsState->strokeParams.setLineJoin(LINE_JOIN_DEFAULT);

  _updateLineWidth();
}

double RasterPaintEngine::getLineWidth() const
{
  return ctx.capsState->strokeParams.getLineWidth();
}

void RasterPaintEngine::setLineWidth(double lineWidth)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if (ctx.capsState->strokeParams.getLineWidth() == lineWidth) return;

  if (!(capsState = _detachCapsState())) return;
  ctx.capsState->strokeParams.setLineWidth(lineWidth);

  _updateLineWidth();
}

uint32_t RasterPaintEngine::getStartCap() const
{
  RasterPaintCapsState* capsState = ctx.capsState;
  return capsState->strokeParams.getStartCap();
}

void RasterPaintEngine::setStartCap(uint32_t startCap)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if ((capsState->strokeParams.getStartCap() == startCap) | ((uint)startCap >= LINE_CAP_COUNT)) return;

  if (!(capsState = _detachCapsState())) return;
  capsState->strokeParams.setStartCap(startCap);
}

uint32_t RasterPaintEngine::getEndCap() const
{
  RasterPaintCapsState* capsState = ctx.capsState;
  return capsState->strokeParams.getEndCap();
}

void RasterPaintEngine::setEndCap(uint32_t endCap)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if ((capsState->strokeParams.getEndCap() == endCap) | ((uint)endCap >= LINE_CAP_COUNT)) return;

  if (!(capsState = _detachCapsState())) return;
  capsState->strokeParams.setEndCap(endCap);
}

void RasterPaintEngine::setLineCaps(uint32_t lineCaps)
{
  RasterPaintCapsState* capsState = ctx.capsState;

  if ((capsState->strokeParams.getStartCap() == lineCaps) |
      (capsState->strokeParams.getEndCap() == lineCaps) |
      ((uint)lineCaps >= LINE_CAP_COUNT)) return;

  if (!(capsState = _detachCapsState())) return;
  capsState->strokeParams.setLineCaps(lineCaps);
}

uint32_t RasterPaintEngine::getLineJoin() const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  return capsState->strokeParams.getLineJoin();
}

void RasterPaintEngine::setLineJoin(uint32_t lineJoin)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if ((capsState->strokeParams.getLineJoin() == lineJoin) | ((uint)lineJoin >= LINE_JOIN_COUNT)) return;

  if (!(capsState = _detachCapsState())) return;
  capsState->strokeParams.setLineJoin(lineJoin);
}

double RasterPaintEngine::getMiterLimit() const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  return capsState->strokeParams.getMiterLimit();
}

void RasterPaintEngine::setMiterLimit(double miterLimit)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if (capsState->strokeParams.getMiterLimit() == miterLimit) return;

  if (!(capsState = _detachCapsState())) return;
  capsState->strokeParams.setMiterLimit(miterLimit);
}

List<double> RasterPaintEngine::getDashes() const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  return capsState->strokeParams.getDashes();
}

void RasterPaintEngine::setDashes(const double* dashes, sysuint_t count)
{
  RasterPaintCapsState* capsState;
  if (!(capsState = _detachCapsState())) return;

  capsState->strokeParams.setDashes(dashes, count);
  _updateLineWidth();
}

void RasterPaintEngine::setDashes(const List<double>& dashes)
{
  RasterPaintCapsState* capsState;
  if (!(capsState = _detachCapsState())) return;

  capsState->strokeParams.setDashes(dashes);
  _updateLineWidth();
}

double RasterPaintEngine::getDashOffset() const
{
  RasterPaintCapsState* capsState = ctx.capsState;
  return capsState->strokeParams.getDashOffset();
}

void RasterPaintEngine::setDashOffset(double offset)
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if (ctx.capsState->strokeParams.getDashOffset() == offset) return;

  if (!(capsState = _detachCapsState())) return;
  ctx.capsState->strokeParams.setDashOffset(offset);

  _updateLineWidth();
}

// ============================================================================
// [Fog::RasterPaintEngine - Transformations]
// ============================================================================

#define RASTER_BEFORE_MATRIX_OP(__clipState__, __capsState__) \
  __capsState__->transform.tx = __capsState__->transformTranslateSaved.x; \
  __capsState__->transform.ty = __capsState__->transformTranslateSaved.y;

#define RASTER_AFTER_MATRIX_OP(__clipState__, __capsState__) \
  __capsState__->transformTranslateSaved.x = __capsState__->transform.tx; \
  __capsState__->transformTranslateSaved.y = __capsState__->transform.ty; \
  \
  __capsState__->transform.tx += (double)__clipState__->workOrigin.x; \
  __capsState__->transform.ty += (double)__clipState__->workOrigin.y;

void RasterPaintEngine::setMatrix(const DoubleMatrix& m)
{
  RasterPaintCapsState* capsState = _detachCapsState();
  RasterPaintClipState* clipState = ctx.clipState;

  if (!capsState) return;

  capsState->transform = m;
  RASTER_AFTER_MATRIX_OP(clipState, capsState)

  _updateTransform(false);
}

void RasterPaintEngine::resetMatrix()
{
  RasterPaintCapsState* capsState = _detachCapsState();
  RasterPaintClipState* clipState = ctx.clipState;

  if (!capsState) return;

  capsState->transform.reset();
  capsState->approximationScale = 1.0;
  capsState->transformType = TRANSFORM_TRANSLATE_EXACT;
  RASTER_AFTER_MATRIX_OP(clipState, capsState)

  // Free pattern resource if not needed.
  if (capsState->sourceType == PAINTER_SOURCE_PATTERN) _resetPatternRasterPaintContext();
}

DoubleMatrix RasterPaintEngine::getMatrix() const
{
  RasterPaintCapsState* capsState = ctx.capsState;

  return DoubleMatrix(
    capsState->transform.sx,
    capsState->transform.shy,
    capsState->transform.shx,
    capsState->transform.sy,
    capsState->transformTranslateSaved.x,
    capsState->transformTranslateSaved.y);
}

void RasterPaintEngine::rotate(double angle, uint32_t order)
{
  RasterPaintCapsState* capsState = _detachCapsState();
  RasterPaintClipState* clipState = ctx.clipState;

  if (!capsState) return;

  RASTER_BEFORE_MATRIX_OP(clipState, capsState)
  capsState->transform.rotate(angle, order);
  RASTER_AFTER_MATRIX_OP(clipState, capsState)

  _updateTransform(false);
}

void RasterPaintEngine::scale(double sx, double sy, uint32_t order)
{
  RasterPaintCapsState* capsState = _detachCapsState();
  RasterPaintClipState* clipState = ctx.clipState;

  if (!capsState) return;

  RASTER_BEFORE_MATRIX_OP(clipState, capsState)
  capsState->transform.scale(sx, sy, order);
  RASTER_AFTER_MATRIX_OP(clipState, capsState)

  _updateTransform(false);
}

void RasterPaintEngine::skew(double sx, double sy, uint32_t order)
{
  RasterPaintCapsState* capsState = _detachCapsState();
  RasterPaintClipState* clipState = ctx.clipState;

  if (!capsState) return;

  RASTER_BEFORE_MATRIX_OP(clipState, capsState)
  capsState->transform.skew(sx, sy, order);
  RASTER_AFTER_MATRIX_OP(clipState, capsState)

  _updateTransform(false);
}

void RasterPaintEngine::translate(double x, double y, uint32_t order)
{
  RasterPaintCapsState* capsState = _detachCapsState();
  RasterPaintClipState* clipState = ctx.clipState;

  if (!capsState) return;

  RASTER_BEFORE_MATRIX_OP(clipState, capsState)
  capsState->transform.translate(x, y, order);
  RASTER_AFTER_MATRIX_OP(clipState, capsState)

  _updateTransform(true);
}

void RasterPaintEngine::transform(const DoubleMatrix& m, uint32_t order)
{
  RasterPaintCapsState* capsState = _detachCapsState();
  RasterPaintClipState* clipState = ctx.clipState;

  if (!capsState) return;

  RASTER_BEFORE_MATRIX_OP(clipState, capsState)
  capsState->transform.multiply(m, order);
  RASTER_AFTER_MATRIX_OP(clipState, capsState)

  _updateTransform(false);
}

// Cleanup.
#undef RASTER_BEFORE_MATRIX_OP
#undef RASTER_AFTER_MATRIX_OP

void RasterPaintEngine::worldToScreen(DoublePoint* pt) const
{
  FOG_ASSERT(pt != NULL);

  RasterPaintCapsState* capsState = ctx.capsState;

  if (capsState->transformType >= TRANSFORM_AFFINE)
  {
    capsState->transform.transformVector(&pt->x, &pt->y);
  }

  pt->x += capsState->transformTranslateSaved.x;
  pt->y += capsState->transformTranslateSaved.y;
}

void RasterPaintEngine::screenToWorld(DoublePoint* pt) const
{
  FOG_ASSERT(pt != NULL);

  RasterPaintCapsState* capsState = ctx.capsState;

  if (capsState->transformType >= TRANSFORM_AFFINE)
  {
    // TODO:
    DoubleMatrix inverse = capsState->transform.inverted();
    inverse.transformVector(&pt->x, &pt->y);
  }

  pt->x -= capsState->transformTranslateSaved.x;
  pt->y -= capsState->transformTranslateSaved.y;
}

void RasterPaintEngine::alignPoint(DoublePoint* pt) const
{
  FOG_ASSERT(pt != NULL);

  RasterPaintEngine::worldToScreen(pt);
  pt->setX(floor(pt->x) + 0.5);
  pt->setY(floor(pt->y) + 0.5);
  RasterPaintEngine::screenToWorld(pt);
}

// ============================================================================
// [Fog::RasterPaintEngine - State]
// ============================================================================

void RasterPaintEngine::save()
{
  RasterPaintStoredState s;

  s.clipState = ctx.clipState->ref();
  s.capsState = ctx.capsState->ref();
  s.pctx = NULL;

  if (ctx.pctx && ctx.pctx->initialized)
  {
    s.pctx = ctx.pctx;
    s.pctx->refCount.inc();
  }

  states.append(s);
}

void RasterPaintEngine::restore()
{
  if (states.isEmpty()) return;

  RasterPaintStoredState s = states.takeLast();

  _derefClipState(ctx.clipState);
  _derefCapsState(ctx.capsState);

  if (ctx.pctx && ctx.pctx->refCount.deref())
  {
    if (ctx.pctx->initialized) ctx.pctx->destroy(ctx.pctx);
    allocator.free(ctx.pctx);
  }

  ctx.clipState = s.clipState;
  ctx.capsState = s.capsState;
  ctx.pctx = s.pctx;
}

// ============================================================================
// [Fog::RasterPaintEngine - Raster drawing]
// ============================================================================

void RasterPaintEngine::clear()
{
  RasterPaintClipState* clipState = ctx.clipState;

  if (clipState->clipSimple)
  {
    _serializeBoxes(&clipState->clipBox, 1);
  }
  else
  {
    const Region& r = clipState->workRegion;
    _serializeBoxes(r.getData(), r.getLength());
  }
}

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

  RasterPaintClipState* clipState = ctx.clipState;
  RasterPaintCapsState* capsState = ctx.capsState;

  if (capsState->transformType != TRANSFORM_TRANSLATE_EXACT || !capsState->lineIsSimple)
  {
    RasterPaintEngine::drawRect(
      DoubleRect((double)r.x + 0.5, (double)r.y + 0.5,
            (double)r.w      , (double)r.h)     );
  }
  else
  {
    TemporaryRegion<4> box;
    TemporaryRegion<16> boxISect;

    {
      RegionData* box_d = box._d;

      int tx = capsState->transformTranslateInt.x;
      int ty = capsState->transformTranslateInt.y;

      box_d->extents.set(r.getX1() + tx, r.getY1() + ty, r.getX2() + tx, r.getY2() + ty);

      if (r.w <= 2 || r.h <= 2)
      {
        box_d->rects[0] = box_d->extents;
        box_d->length = 1;
      }
      else
      {
        box_d->rects[0].set(r.getX1()     + tx, r.getY1()     + ty, r.getX2()     + tx, r.getY1() + 1 + ty);
        box_d->rects[1].set(r.getX1()     + tx, r.getY1() + 1 + ty, r.getX1() + 1 + tx, r.getY2() - 1 + ty);
        box_d->rects[2].set(r.getX2() - 1 + tx, r.getY1() + 1 + ty, r.getX2()     + tx, r.getY2() - 1 + ty);
        box_d->rects[3].set(r.getX1()     + tx, r.getY2() - 1 + ty, r.getX2()     + tx, r.getY2()     + ty);
        box_d->length = 4;
      }
    }

    if (clipState->clipSimple)
    {
      Region::translateAndClip(boxISect, box, IntPoint(0, 0), clipState->clipBox);
    }
    else
    {
      Region::intersect(boxISect, box, clipState->workRegion);
    }

    sysuint_t len = boxISect.getLength();
    if (len) _serializeBoxes(boxISect.getData(), len);
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

  RasterPaintClipState* clipState = ctx.clipState;
  RasterPaintCapsState* capsState = ctx.capsState;

  if (capsState->transformType != TRANSFORM_TRANSLATE_EXACT)
  {
    RasterPaintEngine::fillRect(
      DoubleRect((double)r.x, (double)r.y,
            (double)r.w, (double)r.h));
  }
  else
  {
    int tx = capsState->transformTranslateInt.x;
    int ty = capsState->transformTranslateInt.y;

    IntBox box(r.getX1() + tx, r.getY1() + ty, r.getX2() + tx, r.getY2() + ty);

    if (clipState->clipSimple)
    {
      IntBox::intersect(box, box, clipState->clipBox);
      if (!box.isValid()) return;

      _serializeBoxes(&box, 1);
    }
    else
    {
      TemporaryRegion<1> box_(box);
      TemporaryRegion<16> boxISect(box);

      Region::intersect(boxISect, box_, clipState->workRegion);
      if (boxISect.isEmpty()) return;

      _serializeBoxes(boxISect.getData(), boxISect.getLength());
    }
  }
}

void RasterPaintEngine::fillRects(const IntRect* r, sysuint_t count)
{
  if (!count) return;

  RasterPaintCapsState* capsState = ctx.capsState;

  if (capsState->transformType != TRANSFORM_TRANSLATE_EXACT)
  {
    tmpPath.clear();
    for (sysuint_t i = 0; i < count; i++)
    {
      if (r[i].isValid()) tmpPath.addRect(
        DoubleRect((double)r[i].x, (double)r[i].y,
              (double)r[i].w, (double)r[i].h));
    }
    fillPath(tmpPath);
  }
  else
  {
    Region region;
    region.set(r, count);
    region.translateAndClip(region, region, capsState->transformTranslateInt, ctx.clipState->clipBox);
    if (region.isEmpty()) return;

    if (!ctx.clipState->clipSimple)
    {
      Region::intersect(region, region, ctx.clipState->workRegion);
      if (region.isEmpty()) return;
    }

    _serializeBoxes(region.getData(), region.getLength());
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
  if (region.isEmpty()) return;

  RasterPaintCapsState* capsState = ctx.capsState;

  if (capsState->transformType != TRANSFORM_TRANSLATE_EXACT)
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
  else
  {
    Region regionISect;
    region.translateAndClip(regionISect, region, capsState->transformTranslateInt, ctx.clipState->clipBox);
    if (regionISect.isEmpty()) return;

    if (!ctx.clipState->clipSimple)
    {
      Region::intersect(regionISect, regionISect, ctx.clipState->workRegion);
      if (region.isEmpty()) return;
    }

    _serializeBoxes(region.getData(), region.getLength());
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

  RasterPaintCapsState* capsState = ctx.capsState;

  if (capsState->transformType < TRANSFORM_AFFINE)
  {
    RasterPaintClipState* clipState = ctx.clipState;

    int64_t rx = Math::iround((r.x + capsState->transform.tx) * 256.0);
    if ((rx & 0xFF) != 0x80) goto usePath;

    int64_t ry = Math::iround((r.y + capsState->transform.ty) * 256.0);
    if ((ry & 0xFF) != 0x80) goto usePath;

    int64_t rw = Math::iround((r.w) * 256.0);
    if ((rw & 0xFF) != 0x80) goto usePath;

    int64_t rh = Math::iround((r.h) * 256.0);
    if ((rh & 0xFF) != 0x80) goto usePath;

    IntBox box;
    box.x1 = (int)(rx >> 8);
    box.y1 = (int)(ry >> 8);
    box.x2 = box.x1 + (int)(rw >> 8);
    box.y2 = box.y1 + (int)(rh >> 8);

    if (clipState->clipSimple)
    {
      IntBox::intersect(box, box, clipState->clipBox);
      if (!box.isValid()) return;

      _serializeBoxes(&box, 1);
    }
    else
    {
      TemporaryRegion<1> box_(box);
      TemporaryRegion<16> boxISect(box);

      Region::intersect(boxISect, box_, clipState->workRegion);
      if (boxISect.isEmpty()) return;

      _serializeBoxes(boxISect.getData(), boxISect.getLength());
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

  int tx = ctx.capsState->transformTranslateInt.x;
  int ty = ctx.capsState->transformTranslateInt.y;

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
  int tx = ctx.capsState->transformTranslateInt.x;
  int ty = ctx.capsState->transformTranslateInt.y;

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
  int tx = ctx.capsState->transformTranslateInt.x;
  int ty = ctx.capsState->transformTranslateInt.y;

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
  int tx = ctx.capsState->transformTranslateInt.x;
  int ty = ctx.capsState->transformTranslateInt.y;

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

  RasterPaintCapsState* capsState = ctx.capsState;
  if (capsState->transformType == TRANSFORM_TRANSLATE_EXACT)
  {
    RasterPaintClipState* clipState = ctx.clipState;

    int srcx = 0;
    int srcy = 0;
    int dstx = p.x + capsState->transformTranslateInt.x;
    int dsty = p.y + capsState->transformTranslateInt.y;
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

    if ((uint)(d = dstx - clipState->clipBox.getX1()) >= (uint)clipState->clipBox.getWidth())
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

    if ((uint)(d = dsty - clipState->clipBox.getY1()) >= (uint)clipState->clipBox.getHeight())
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

    if ((d = clipState->clipBox.getX2() - dstx) < dstw) dstw = d;
    if ((d = clipState->clipBox.getY2() - dsty) < dsth) dsth = d;

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

  RasterPaintCapsState* capsState = ctx.capsState;
  if (capsState->transformType <= TRANSFORM_TRANSLATE_SUBPX)
  {
    int64_t xbig = (int64_t)((p.x + capsState->transform.tx) * 256.0);
    int64_t ybig = (int64_t)((p.y + capsState->transform.ty) * 256.0);

    int xf = (int)(xbig & 0xFF);
    int yf = (int)(ybig & 0xFF);

    if (xf == 0x00 && yf == 0x00)
    {
      RasterPaintClipState* clipState = ctx.clipState;

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

      if ((uint)(d = dstx - clipState->clipBox.getX1()) >= (uint)clipState->clipBox.getWidth())
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

      if ((uint)(d = dsty - clipState->clipBox.getY1()) >= (uint)clipState->clipBox.getHeight())
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

      if ((d = clipState->clipBox.getX2() - dstx) < dstw) dstw = d;
      if ((d = clipState->clipBox.getY2() - dsty) < dsth) dsth = d;

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

void RasterPaintEngine::_updateWorkRegion()
{
  RasterPaintLayer* layer = ctx.layer;
  RasterPaintClipState* clipState = ctx.clipState;

  // Caller must ensure this.
  FOG_ASSERT(clipState->refCount.get() == 1);

  // Work origin is point that is added to painter translation matrix and it
  // ensured that raster will be always from [0, 0] -> [W-1, H-1] inclusive.
  IntPoint workOrigin(clipState->metaOrigin + clipState->userOrigin);
  bool workOriginChanged = clipState->workOrigin != workOrigin;

  // Default clip box is no clip.
  clipState->clipBox.set(0, 0, layer->width, layer->height);
  clipState->clipSimple = true;
  clipState->workOrigin = workOrigin;

  // Calculate final clip region.
  uint clipBits = ((uint)clipState->metaRegionUsed << 1) | ((uint)(clipState->userRegionUsed) << 0);

  switch (clipBits)
  {
    // User region only.
    case 0x00000001:
      Region::translateAndClip(clipState->workRegion, clipState->userRegion, clipState->metaOrigin, clipState->clipBox);
      break;

    // Meta region only.
    case 0x00000002:
      Region::translateAndClip(clipState->workRegion, clipState->metaRegion, IntPoint(0, 0), clipState->clipBox);
      break;

    // Meta region & user region.
    case 0x00000003:
    {
      Region tmp;

      Region::translate(tmp, clipState->userRegion, clipState->metaOrigin);
      Region::intersectAndClip(clipState->workRegion, clipState->metaRegion, tmp, clipState->clipBox);
      break;
    }
  }

  if (clipBits)
  {
    clipState->clipBox = clipState->workRegion.extents();
    clipState->clipSimple = clipState->workRegion.getLength() <= 1;
  }

  clipState->workRegionUsed = (clipBits != 0) && !clipState->clipSimple;
  if (workOriginChanged) _updateTransform(true);
}

void RasterPaintEngine::_updateTransform(bool translationOnly)
{
  RasterPaintCapsState* capsState = ctx.capsState;

  // Update translation in pixels.
  capsState->transformTranslateInt.set(
    Math::iround(capsState->transform.tx),
    Math::iround(capsState->transform.ty));

  // If only matrix translation was changed, we can skip some expensive
  // calculations and checking.
  if (translationOnly)
  {
    if (capsState->transformType < TRANSFORM_AFFINE)
    {
      bool isExact = 
        ((Math::iround(capsState->transform.tx * 256.0) & 0xFF) == 0x00) &
        ((Math::iround(capsState->transform.ty * 256.0) & 0xFF) == 0x00) ;

      capsState->transformType = (isExact) 
        ? TRANSFORM_TRANSLATE_EXACT
        : TRANSFORM_TRANSLATE_SUBPX;
    }
  }
  else
  {
    // Just check for matrix characteristics.
    bool isIdentity = 
      (Math::feq(capsState->transform.sx , 1.0)) &
      (Math::feq(capsState->transform.sy , 1.0)) &
      (Math::feq(capsState->transform.shx, 0.0)) &
      (Math::feq(capsState->transform.shy, 0.0)) ;
    bool isExact = isIdentity && (
      ((Math::iround(capsState->transform.tx * 256.0) & 0xFF) == 0x00) &
      ((Math::iround(capsState->transform.ty * 256.0) & 0xFF) == 0x00) );

    // Use matrix characteristics to set correct transform type.
    uint transformType = isIdentity
      ? (isExact ? TRANSFORM_TRANSLATE_EXACT : TRANSFORM_TRANSLATE_SUBPX)
      : (TRANSFORM_AFFINE);

    capsState->transformType = (uint8_t)transformType;

    // sqrt(2.0)/2 ~~ 0.7071068
    if (transformType >= TRANSFORM_AFFINE)
    {
      double x = capsState->transform.sx + capsState->transform.shx;
      double y = capsState->transform.sy + capsState->transform.shy;
      capsState->approximationScale = Math::sqrt(x * x + y * y) * 0.7071068;
    }
    else
    {
      capsState->approximationScale = 1.0;
    }
  }

  // Free pattern context, because transform was changed.
  if (ctx.pctx && ctx.pctx->initialized)
  {
    _resetPatternRasterPaintContext();
  }
}

void RasterPaintEngine::_setClipDefaults()
{
  RasterPaintClipState* clipState = ctx.clipState;

  // Called must ensure this.
  FOG_ASSERT(clipState->refCount.get() == 1);

  clipState->metaOrigin.set(0, 0);
  clipState->userOrigin.set(0, 0);
  clipState->workOrigin.set(0, 0);
  clipState->clipBox.set(0, 0, ctx.layer->width, ctx.layer->height);
  clipState->metaRegionUsed = false;
  clipState->userRegionUsed = false;
  clipState->workRegionUsed = false;
  clipState->clipSimple = true;
}

void RasterPaintEngine::_setCapsDefaults()
{
  RasterPaintClipState* clipState = ctx.clipState;
  RasterPaintCapsState* capsState = ctx.capsState;

  // Caller must ensure this.
  FOG_ASSERT(capsState->refCount.get() == 1);

  switch (capsState->sourceType)
  {
    case PAINTER_SOURCE_ARGB:
      break;
    case PAINTER_SOURCE_PATTERN:
      capsState->pattern.destroy();
      break;
    case PAINTER_SOURCE_COLOR_FILTER:
      capsState->colorFilter.destroy();
      break;
    default:
      FOG_ASSERT_NOT_REACHED();
  }

  capsState->op = OPERATOR_SRC_OVER;
  capsState->sourceType = PAINTER_SOURCE_ARGB;
  capsState->transformType = TRANSFORM_TRANSLATE_EXACT;
  capsState->fillMode = FILL_DEFAULT;
  capsState->aaQuality = ANTI_ALIASING_SMOOTH;
  capsState->imageInterpolation = INTERPOLATION_SMOOTH;
  capsState->gradientInterpolation = INTERPOLATION_SMOOTH;
  capsState->forceOutlineText = false;
  capsState->lineIsSimple = true;

  capsState->solid.argb = 0xFF000000;
  capsState->solid.prgb = 0xFF000000;

  capsState->rops = RasterEngine::getCompositeFuncs(ctx.layer->format, OPERATOR_SRC_OVER);

  capsState->strokeParams.reset();
  capsState->transform.set(
    1.0, 0.0,
    0.0, 1.0,
    (double)clipState->workOrigin.x, (double)clipState->workOrigin.y);
  capsState->approximationScale = 1.0;
  capsState->transformTranslateSaved.set(0.0, 0.0);
  capsState->transformTranslateInt.set(clipState->workOrigin);
}

RasterEngine::PatternContext* RasterPaintEngine::_getPatternRasterPaintContext()
{
  RasterPaintCapsState* capsState = ctx.capsState;

  // Sanity, calling _getPatternRasterPaintContext() for other than pattern 
  // source is not allowed.
  FOG_ASSERT(capsState->sourceType == PAINTER_SOURCE_PATTERN);

  RasterEngine::PatternContext* pctx = ctx.pctx;
  err_t err = ERR_OK;

  if (!pctx)
  {
    pctx = ctx.pctx = (RasterEngine::PatternContext*)allocator.alloc(sizeof(RasterEngine::PatternContext));
    if (!pctx) return NULL;

    pctx->refCount.init(1);
    pctx->initialized = false;
  }

  if (!pctx->initialized)
  {
    const Pattern& pattern = capsState->pattern.instance();
    const DoubleMatrix& matrix = capsState->transform;

    switch (pattern.getType())
    {
      case PATTERN_TEXTURE:
        err = RasterEngine::functionMap->pattern.texture_init(pctx, pattern, matrix, capsState->imageInterpolation);
        break;
      case PATTERN_LINEAR_GRADIENT:
        err = RasterEngine::functionMap->pattern.linear_gradient_init(pctx, pattern, matrix, capsState->gradientInterpolation);
        break;
      case PATTERN_RADIAL_GRADIENT:
        err = RasterEngine::functionMap->pattern.radial_gradient_init(pctx, pattern, matrix, capsState->gradientInterpolation);
        break;
      case PATTERN_CONICAL_GRADIENT:
        err = RasterEngine::functionMap->pattern.conical_gradient_init(pctx, pattern, matrix, capsState->gradientInterpolation);
        break;
      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  if (err)
  {
    fog_debug("Fog::RasterPaintEngine::_getPatternRasterPaintContext() - Failed to get context (error %d)\n", err);
    return NULL;
  }

  // Be sure that pattern context is always marked as initialized.
  FOG_ASSERT(pctx->initialized);

  return pctx;
}

void RasterPaintEngine::_resetPatternRasterPaintContext()
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

RasterPaintClipState* RasterPaintEngine::_detachClipState()
{
  RasterPaintClipState* clipState = ctx.clipState;
  if (clipState->refCount.get() == 1) return clipState;

  clipState = reinterpret_cast<RasterPaintClipState*>(allocator.alloc(sizeof(RasterPaintClipState)));
  if (!clipState) return NULL;

  _derefClipState(
    atomicPtrXchg(&ctx.clipState, new(clipState) RasterPaintClipState(*(ctx.clipState)))
  );
  return clipState;
}

RasterPaintCapsState* RasterPaintEngine::_detachCapsState()
{
  RasterPaintCapsState* capsState = ctx.capsState;
  if (capsState->refCount.get() == 1) return capsState;

  capsState = reinterpret_cast<RasterPaintCapsState*>(allocator.alloc(sizeof(RasterPaintCapsState)));
  if (!capsState) return NULL;

  _derefCapsState(
    atomicPtrXchg(&ctx.capsState, new(capsState) RasterPaintCapsState(*(ctx.capsState)))
  );
  return capsState;
}

FOG_INLINE void RasterPaintEngine::_derefClipState(RasterPaintClipState* clipState)
{
  if (clipState->refCount.deref())
  {
    clipState->~RasterPaintClipState();
    allocator.free(clipState);
  }
}

FOG_INLINE void RasterPaintEngine::_derefCapsState(RasterPaintCapsState* capsState)
{
  if (capsState->refCount.deref())
  {
    capsState->~RasterPaintCapsState();
    allocator.free(capsState);
  }
}

void RasterPaintEngine::_deleteStates()
{
  if (states.isEmpty()) return;

  List<RasterPaintStoredState>::ConstIterator it(states);
  for (it.toStart(); it.isValid(); it.toNext())
  {
    RasterPaintStoredState& s = const_cast<RasterPaintStoredState&>(it.value());
    _derefClipState(s.clipState);
    _derefCapsState(s.capsState);

    if (s.pctx && (!s.pctx->initialized || s.pctx->refCount.deref()))
    {
      if (s.pctx->initialized) s.pctx->destroy(s.pctx);
      allocator.free(s.pctx);
    }
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
      layer->type = LAYER_TYPE_DIRECT32;
      layer->bytesPerPixel = 4;

      layer->secondaryFormat = PIXEL_FORMAT_NULL;
      layer->toSecondary = NULL;
      layer->fromSecondary = NULL;
      break;

    case PIXEL_FORMAT_ARGB32:
      // Direct rendering if possible, bail to PRGB32 if ARGB32 is not
      // implemented for particular task.
      layer->type = LAYER_TYPE_DIRECT32;
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
      // I8 format is not supported by Fog::Painter.
    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

// ============================================================================
// [Fog::RasterPaintEngine - Serializers]
// ============================================================================

void RasterPaintEngine::_serializeBoxes(const IntBox* box, sysuint_t count)
{
  // Pattern context must be always set up before _render() methods are called.
  if (ctx.capsState->sourceType == PAINTER_SOURCE_PATTERN && !_getPatternRasterPaintContext()) return;

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
      sysuint_t n = Math::min<sysuint_t>(count, 128);
      RasterPaintCmdBoxes* cmd = _createCommand<RasterPaintCmdBoxes>(sizeof(RasterPaintCmdBoxes) - sizeof(IntBox) + n * sizeof(IntBox));
      if (!cmd) return;

      cmd->count = n;
      for (sysuint_t j = 0; j < n; j++) cmd->boxes[j] = box[j];
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
    RasterPaintCmdImage* cmd = _createCommand<RasterPaintCmdImage>(sizeof(RasterPaintCmdImage), NULL);
    if (!cmd) return;

    cmd->dst = dst;
    cmd->src = src;
    cmd->image.init(image);
    _postCommand(cmd);
  }
}

void RasterPaintEngine::_serializeImageAffine(const DoublePoint& pt, const Image& image, const IntRect* irect)
{
  FOG_ASSERT(!image.isEmpty());

  // Create new transformation matrix (based on current matrix and point
  // where the image should be drawn).
  const DoubleMatrix& tr = ctx.capsState->transform;
  DoublePoint pt_(pt);
  tr.transformPoint(&pt_.x, &pt_.y);
  DoubleMatrix matrix(tr.sx, tr.shy, tr.shx, tr.sy, pt_.x, pt_.y);

  IntRect ir(0, 0, image.getWidth(), image.getHeight());
  if (irect) ir = *irect;

  if (!(OperatorCharacteristics[ctx.capsState->op] & OPERATOR_CHAR_UNBOUND))
  {
    // Compositing operator is BOUND so we can take advantage of it. We create
    // new transformation matrix and then we use RasterPaintRenderImageAffine
    // renderer that fetches image using SPRAD_NONE and then blends fetched data
    // with target buffer.

    // Singlethreaded.
    if (isSingleThreaded())
    {
      RasterRenderImageAffineBound renderer;
      if (!renderer.init(image, ir, matrix, ctx.clipState->clipBox, ctx.capsState->imageInterpolation)) return;

      renderer.render(&ctx);
    }
    // Multithreaded.
    else
    {
      RasterPaintCmdImageAffineBound* cmd = _createCommand<RasterPaintCmdImageAffineBound>(sizeof(RasterPaintCmdImageAffineBound), NULL);
      if (!cmd) return;

      if (!cmd->renderer.instance().init(image, ir, matrix, ctx.clipState->clipBox, ctx.capsState->imageInterpolation))
      {
        cmd->release();
        return;
      }

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
      if (_rasterizePath(&ctx, ras, tmpPath, false))
      {
        // Save current pattern context (it will be replaced by context created
        // for the image).
        RasterEngine::PatternContext* oldpctx = ctx.pctx;

        // Create new pattern context (based on the image).
        RasterEngine::PatternContext imagectx;
        imagectx.initialized = false;
        RasterEngine::functionMap->pattern.texture_init_blit(&imagectx, 
          image, ir, matrix, SPREAD_PAD, ctx.capsState->imageInterpolation);
        ctx.pctx = &imagectx;

        // Render path using specific pattern context.
        _renderPath(&ctx, ras, true);

        // Destroy pattern context.
        imagectx.destroy(&imagectx);

        // Restore old pattern context.
        ctx.pctx = oldpctx;
      }
    }
    // Multithreaded.
    else
    {
      // This is a bit code duplication (see singlethreaded mode), but it quite
      // different. Here we are not interested about rectangle rasterization and
      // replacing pattern context in ctx, but we instead serialize path command
      // with new pattern context (current context will not be replaced at all).

      // Create new pattern context (based on the image).
      RasterEngine::PatternContext* imagectx =
        reinterpret_cast<RasterEngine::PatternContext*>(
          allocator.alloc(sizeof(RasterEngine::PatternContext)));
      if (!imagectx) return;

      imagectx->initialized = false;
      RasterEngine::functionMap->pattern.texture_init_blit(
        imagectx, image, ir, matrix, SPREAD_PAD, ctx.capsState->imageInterpolation);
      imagectx->refCount.init(1);

      RasterPaintCmdPath* cmd = _createCommand<RasterPaintCmdPath>(sizeof(RasterPaintCmdPath), imagectx);
      if (!cmd)
      {
        imagectx->destroy(imagectx);
        allocator.free(imagectx);
        return;
      }

      RasterPaintCalcPath* clc = _createCalc<RasterPaintCalcPath>();
      if (!clc)
      {
        // This is enough, because release() will dereference pattern context and
        // free it.
        cmd->release();
        return;
      }

      clc->relatedTo = cmd;
      clc->path.init(tmpPath);
      clc->stroke = false;
      cmd->status.init(COMMAND_STATUS_WAIT);
      cmd->calculation = clc;
      cmd->ras = NULL; // Will be initialized by calculation.
      cmd->textureBlit = true;
      _postCommand(cmd, clc);
    }
  }
}

void RasterPaintEngine::_serializeGlyphSet(const IntPoint& pt, const GlyphSet& glyphSet, const IntRect* clip)
{
  IntBox boundingBox = ctx.clipState->clipBox;

  if (clip)
  {
    IntBox::intersect(boundingBox, boundingBox, IntBox(*clip));
    if (!boundingBox.isValid()) return;
  }

  // Pattern context must be always set up before _render() methods are called.
  if (ctx.capsState->sourceType == PAINTER_SOURCE_PATTERN && !_getPatternRasterPaintContext()) return;

  // Singlethreaded.
  if (isSingleThreaded())
  {
    _renderGlyphSet(&ctx, pt, glyphSet, boundingBox);
  }
  // Multithreaded.
  else
  {
    RasterPaintCmdGlyphSet* cmd = _createCommand<RasterPaintCmdGlyphSet>();
    if (!cmd) return;

    cmd->pt = pt;
    cmd->boundingBox = boundingBox;
    cmd->glyphSet.init(glyphSet);
    _postCommand(cmd);
  }
}

void RasterPaintEngine::_serializePath(const DoublePath& path, bool stroke)
{
  // Pattern context must be always set up before _render() methods are called.
  if (ctx.capsState->sourceType == PAINTER_SOURCE_PATTERN && !_getPatternRasterPaintContext()) return;

  // Singlethreaded.
  if (isSingleThreaded())
  {
    if (_rasterizePath(&ctx, ras, path, stroke)) _renderPath(&ctx, ras, false);
  }
  // Multithreaded.
  else
  {
    RasterPaintCmdPath* cmd = _createCommand<RasterPaintCmdPath>();
    if (!cmd) return;

    RasterPaintCalcPath* clc = _createCalc<RasterPaintCalcPath>();
    if (!clc) { cmd->release(); return; }

    clc->relatedTo = cmd;
    clc->path.init(path);
    clc->stroke = stroke;
    cmd->status.init(COMMAND_STATUS_WAIT);
    cmd->calculation = clc;
    cmd->ras = NULL; // Will be initialized by calculation.
    cmd->textureBlit = false;
    _postCommand(cmd, clc);
  }
}

template <typename T>
FOG_INLINE T* RasterPaintEngine::_createCommand(sysuint_t size)
{
  T* command = reinterpret_cast<T*>(workerManager->allocator.alloc(size));
  if (!command) return NULL;

  new(command) T;

  command->refCount.init((uint)workerManager->numWorkers);
  command->status.init(COMMAND_STATUS_READY);

  command->engine = this;
  command->layer = ctx.layer;
  command->clipState = ctx.clipState->ref();
  command->capsState = ctx.capsState->ref();
  command->pctx = NULL;
  command->calculation = NULL;

  // Initialize pattern context if source is pattern type.
  if (ctx.capsState->sourceType == PAINTER_SOURCE_PATTERN)
  {
    RasterEngine::PatternContext* pctx = ctx.pctx;
    FOG_ASSERT(pctx && pctx->initialized);
    pctx->refCount.inc();
    command->pctx = pctx;
  }

  return command;
}

template <typename T>
FOG_INLINE T* RasterPaintEngine::_createCommand(sysuint_t size, RasterEngine::PatternContext* pctx)
{
  T* command = reinterpret_cast<T*>(workerManager->allocator.alloc(size));
  if (!command) return NULL;

  new(command) T;

  command->refCount.init(workerManager->numWorkers);
  command->engine = this;
  command->layer = ctx.layer;
  command->clipState = ctx.clipState->ref();
  command->capsState = ctx.capsState->ref();
  command->pctx = pctx;
  command->status.init(COMMAND_STATUS_READY);
  command->calculation = NULL;

  return command;
}
template<typename T>
T* RasterPaintEngine::_createCalc(sysuint_t size)
{
  T* calculation = reinterpret_cast<T*>(workerManager->allocator.alloc(size));
  if (!calculation) return NULL;

  new(calculation) T;
  calculation->engine = this;
  return calculation;
}

void RasterPaintEngine::_postCommand(RasterPaintCmd* cmd, RasterPaintCalc* clc)
{
#if defined(FOG_DEBUG_RASTER_COMMANDS)
  fog_debug("Fog::Painter::_postCommand() - posting...");
#endif // FOG_DEBUG_RASTER_COMMANDS

  // Flush everything if commands get maximum.
  if (workerManager->cmdPosition == RASTER_MAX_COMMANDS)
  {
#if defined(FOG_DEBUG_RASTER_COMMANDS)
    fog_debug("Fog::Painter::_postCommand() - command buffer is full");
#endif // FOG_DEBUG_RASTER_COMMANDS
    flush(PAINTER_FLUSH_SYNC);
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

bool RasterPaintEngine::_rasterizePath(RasterPaintContext* ctx, Rasterizer* ras, const DoublePath& path, bool stroke)
{
  RasterPaintClipState* clipState = ctx->clipState;
  RasterPaintCapsState* capsState = ctx->capsState;

  // Use transformation matrix only if it makes sense.
  const DoubleMatrix* matrix = NULL;
  if (capsState->transformType >= TRANSFORM_AFFINE) matrix = &capsState->transform;

  DoublePath dst;

  ras->reset();
  ras->setClipBox(clipState->clipBox);

  bool noTransform = (
    capsState->transformType == TRANSFORM_TRANSLATE_EXACT &&
    capsState->transformTranslateInt.x == 0 &&
    capsState->transformTranslateInt.y == 0);

  if (stroke)
  {
    // Stroke mode. Stroke will flatten the path.
    Stroker(capsState->strokeParams, capsState->approximationScale).stroke(dst, path);

    if (capsState->transformType >= TRANSFORM_AFFINE)
    {
      dst.applyMatrix(capsState->transform);
    }
    else if (!noTransform)
    {
      dst.translate(capsState->transform.tx, capsState->transform.ty);
    }

    // Stroke not respects fill mode set in caps state, instead we are using
    // FILL_NON_ZERO.
    ras->setFillRule(FILL_NON_ZERO);
  }
  else
  {
    // Fill mode.
    path.flattenTo(dst, noTransform ? NULL : &capsState->transform, capsState->approximationScale);

    // Fill respects fill mode set in caps state.
    ras->setFillRule(capsState->fillMode);
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

  RasterPaintLayer* layer = ctx->layer;
  RasterPaintCapsState* capsState = ctx->capsState;

  uint8_t* pixels = layer->pixels;
  sysint_t stride = layer->stride;

  RasterEngine::Closure* closure = &ctx->closure;

  int offset = ctx->offset;
  int delta = ctx->delta;
  sysint_t strideWithDelta = stride * delta;

  switch (capsState->sourceType)
  {
    // Solid source type.
    case PAINTER_SOURCE_ARGB:
    {
      const RasterEngine::Solid* source = &capsState->solid;
      RasterEngine::CSpanFn cspan = capsState->rops->cspan;

      switch (layer->type)
      {
        case LAYER_TYPE_DIRECT32:
        case LAYER_TYPE_INDIRECT32:
          if (cspan != NULL)
          {
            RENDER_LOOP(bltOpaque, 4,
            {
              cspan(pCur, source, (sysuint_t)w, closure);
            });
          }
          else
          {
            cspan = RasterEngine::getCompositeFuncs(
              layer->secondaryFormat, capsState->op)->cspan;

            RENDER_LOOP(bltOpaque, 4,
            {
              layer->toSecondary(pCur, pCur, w, NULL);
              cspan(pCur, source, (sysuint_t)w, closure);
              layer->fromSecondary(pCur, pCur, w, NULL);
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

      uint32_t format = layer->format;

      RasterEngine::VSpanFn vspan;

      // FastPath: Do not copy pattern to extra buffer if compositing operator
      // is OPERATOR_SRC. We need to match pixel formats and make sure that
      // operator is OPERATOR_SRC or OPERATOR_SRC_OVER without alpha channel.
      if (isRawOpaqueBlit(format, pctx->format, capsState->op))
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

        switch (layer->type)
        {
          case LAYER_TYPE_DIRECT32:
            RENDER_LOOP(bltPatternFastDirect32, 4,
            {
              uint8_t* f = pctx->fetch(pctx, pCur, x1, y1, w);
              if (f != pCur) vspan(pCur, f, w, closure);
            })
            break;

          case LAYER_TYPE_INDIRECT32:
            RENDER_LOOP(bltPatternFastIndirect32, 4,
            {
              uint8_t* f = pctx->fetch(pctx, pCur, x1, y1, w);
              vspan(pCur, f, w, closure);
            })
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }
      }
      else
      {
        vspan = capsState->rops->vspan[pctx->format];

        uint8_t* pBuf = ctx->getBuffer((uint)(ctx->clipState->clipBox.getWidth()) * 4);
        if (!pBuf) return;

        switch (layer->type)
        {
          case LAYER_TYPE_DIRECT32:
          case LAYER_TYPE_INDIRECT32:
            if (vspan != NULL)
            {
              RENDER_LOOP(bltPatternCommonDirect32, 4,
              {
                uint8_t* f = pctx->fetch(pctx, pBuf, x1, y1, w);
                vspan(pCur, f, w, closure);
              });
            }
            else
            {
              vspan = RasterEngine::getCompositeFuncs(
                layer->secondaryFormat, capsState->op)->vspan[pctx->format];

              RENDER_LOOP(bltPatternCommonIndirect32, 4,
              {
                uint8_t* f = pctx->fetch(pctx, pBuf, x1, y1, w);
                layer->toSecondary(pCur, pCur, w, NULL);
                vspan(pCur, f, w, closure);
                layer->fromSecondary(pCur, pCur, w, NULL);
              });
            }
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }
      }
      break;
    }

#if 0
    // Color filter.
    case PAINTER_SOURCE_COLOR_FILTER:
    {
      const ImageFilterEngine* cfEngine = capsState->colorFilter->getEngine();
      const void* cfRasterPaintContext = cfEngine->getContext();
      ColorFilterFn cspan = cfEngine->getColorFilterFn(layer->format);

      RENDER_LOOP(bltColorFilter, 4,
      {
        cspan(cfRasterPaintContext, pCur, pCur, (sysuint_t)w);
      });

      cfEngine->releaseContext(cfRasterPaintContext);
      break;
    }
#endif
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
  if (FOG_LIKELY(clipState->clipSimple)) \
  { \
    int x = dst.x; \
    \
    uint8_t* dstCur = layer->pixels + (sysint_t)(uint)(x) * BPP; \
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
    const IntBox* clipCur = clipState->workRegion.getData(); \
    const IntBox* clipTo; \
    const IntBox* clipNext; \
    const IntBox* clipEnd = clipCur + clipState->workRegion.getLength(); \
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
      dstBase = layer->pixels + (sysint_t)(uint)(y) * dstStride; \
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
      dstBase = layer->pixels + (sysint_t)(uint)(y) * dstStride; \
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

  RasterPaintLayer* layer = ctx->layer;

  RasterPaintClipState* clipState = ctx->clipState;
  RasterPaintCapsState* capsState = ctx->capsState;

  Image::Data* image_d = image._d;
  RasterEngine::VSpanFn vspan = capsState->rops->vspan[image_d->format];

  sysint_t dstStride = layer->stride;
  sysint_t srcStride = image_d->stride;

  RasterEngine::Closure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = image._d->palette.getData();

  int y = dst.y;
  int yMax = dst.y + dst.h;
  int delta = ctx->delta;

  switch (layer->type)
  {
    case LAYER_TYPE_DIRECT32:
    case LAYER_TYPE_INDIRECT32:
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
          layer->secondaryFormat, capsState->op)->vspan[image_d->format];

        RENDER_LOOP(bltImageIndirect32, 4,
        {
          layer->toSecondary(dstCur, dstCur, w, NULL);
          vspan(dstCur, srcCur, w, &closure);
          layer->fromSecondary(dstCur, dstCur, w, NULL);
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
    Image::Data* bitmapd = glyphd->bitmap._d; \
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

  RasterPaintLayer* layer = ctx->layer;
  RasterPaintCapsState* capsState = ctx->capsState;
  RasterPaintClipState* clipState = ctx->clipState;

  if (!glyphSet.getLength()) return;

  int offset = ctx->offset;
  int delta = ctx->delta;

  const Glyph* glyphs = glyphSet.glyphs();
  sysuint_t count = glyphSet.getLength();

  int px = pt.x;
  int py = pt.y;

  uint8_t* pixels = layer->pixels;
  sysint_t stride = layer->stride;
  sysint_t strideWithDelta = stride * delta;

  RasterEngine::Closure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  switch (capsState->sourceType)
  {
    // Solid source type.
    case PAINTER_SOURCE_ARGB:
    {
      RasterEngine::CSpanMskFn cspan_a8 = capsState->rops->cspan_a8;
      const RasterEngine::Solid* source = &capsState->solid;

      switch (layer->type)
      {
        case LAYER_TYPE_DIRECT32:
        case LAYER_TYPE_INDIRECT32:
          if (FOG_LIKELY(cspan_a8 != NULL))
          {
            RENDER_LOOP(bltSolidDirect32, 4,
            {
              cspan_a8(pCur, source, pGlyph, w, &closure);
            })
          }
          else
          {
            cspan_a8 = RasterEngine::getCompositeFuncs(
              layer->secondaryFormat, capsState->op)->cspan_a8;

            RENDER_LOOP(bltSolidIndirect32, 4,
            {
              layer->toSecondary(pCur, pCur, w, NULL);
              cspan_a8(pCur, source, pGlyph, w, &closure);
              layer->fromSecondary(pCur, pCur, w, NULL);
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

      RasterEngine::VSpanMskFn vspan_a8 = capsState->rops->vspan_a8[pctx->format];

      switch (layer->type)
      {
        case LAYER_TYPE_DIRECT32:
        case LAYER_TYPE_INDIRECT32:
        {
          uint8_t* pbuf = ctx->getBuffer((uint)(clipState->clipBox.getWidth()) * 4);
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
              layer->secondaryFormat, capsState->op)->vspan_a8[pctx->format];

            RENDER_LOOP(bltPatternIndirect32, 4,
            {
              uint8_t* f = pctx->fetch(pctx, pbuf, x1, y1, w);

              layer->toSecondary(pCur, pCur, w, NULL);
              vspan_a8(pCur, f, pGlyph, w, &closure);
              layer->fromSecondary(pCur, pCur, w, NULL);
            })
          }
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }

      break;
    }

#if 0
    // Color filter.
    case PAINTER_SOURCE_COLOR_FILTER:
    {
      // TODO:
      break;
    }
#endif
  }
#undef RENDER_LOOP
}

void RasterPaintEngine::_renderPath(RasterPaintContext* ctx, Rasterizer* ras, bool textureBlit)
{
#define RENDER_LOOP(NAME, BPP, CODE) \
  /* Simple clip. */ \
  if (clipState->clipSimple) \
  { \
    pBase = layer->pixels + y * stride; \
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
    const IntBox* clipCur = clipState->workRegion.getData(); \
    const IntBox* clipTo; \
    const IntBox* clipEnd = clipCur + clipState->workRegion.getLength(); \
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
    pBase = layer->pixels + y * stride; \
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

  RasterPaintLayer* layer = ctx->layer;
  RasterPaintClipState* clipState = ctx->clipState;
  RasterPaintCapsState* capsState = ctx->capsState;
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

  sysint_t stride = layer->stride;
  sysint_t strideWithDelta = stride * delta;

  uint8_t* pBase;
  uint8_t* pCur;

  RasterEngine::Closure closure;
  closure.dstPalette = NULL;
  closure.srcPalette = NULL;

  uint sourceType = textureBlit ? PAINTER_SOURCE_PATTERN : capsState->sourceType;
  const RasterEngine::FunctionMap::CompositeFuncs* rops = capsState->rops;

  switch (sourceType)
  {
    // Solid source type.
    case PAINTER_SOURCE_ARGB:
    {
      const RasterEngine::Solid* source = &capsState->solid;

      switch (layer->type)
      {
        case LAYER_TYPE_DIRECT32:
        case LAYER_TYPE_INDIRECT32:
          if (rops->cspan_a8_scanline)
          {
            RENDER_LOOP(bltSolidDirect32, 4,
            {
              rops->cspan_a8_scanline(pBase, source, scanline->getSpansData(), numSpans, &closure);
            });
          }
          else
          {
            rops = RasterEngine::getCompositeFuncs(
              layer->secondaryFormat, OPERATOR_SRC);

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
                  layer->toSecondary(pCur, pCur, len, NULL);
                  rops->cspan_a8(pCur, source, span->covers, len, &closure);
                  layer->fromSecondary(pCur, pCur, len, NULL);
                }
                else
                {
                  len = -len;
                  FOG_ASSERT(len > 0);

                  uint32_t cover = (uint32_t)*(span->covers);

                  layer->toSecondary(pCur, pCur, len, NULL);
                  if (cover == 0xFF)
                    rops->cspan(pCur, source, len, &closure);
                  else
                    rops->cspan_a8_const(pCur, source, cover, len, &closure);
                  layer->fromSecondary(pCur, pCur, len, NULL);
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

      uint8_t* pBuf = ctx->getBuffer((uint)(clipState->clipBox.getWidth()) * 4);
      if (!pBuf) return;

      RasterEngine::VSpanFn vspan;
      RasterEngine::VSpanMskFn vspan_a8;
      RasterEngine::VSpanMskConstFn vspan_a8_const;

      vspan = rops->vspan[pctx->format];
      vspan_a8 = rops->vspan_a8[pctx->format];
      vspan_a8_const = rops->vspan_a8_const[pctx->format];

      switch (layer->type)
      {
        case LAYER_TYPE_DIRECT32:
        case LAYER_TYPE_INDIRECT32:
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
            rops = RasterEngine::getCompositeFuncs(
              layer->secondaryFormat, OPERATOR_SRC);

            vspan = rops->vspan[pctx->format];
            vspan_a8 = rops->vspan_a8[pctx->format];
            vspan_a8_const = rops->vspan_a8_const[pctx->format];

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
                  layer->toSecondary(pCur, pCur, len, NULL);
                  uint8_t* f = pctx->fetch(pctx, pBuf, x, y, len);
                  vspan_a8(pCur, f, span->covers, len, &closure);
                  layer->fromSecondary(pCur, pCur, len, NULL);
                }
                else
                {
                  len = -len;
                  FOG_ASSERT(len > 0);

                  uint32_t cover = (uint32_t)*(span->covers);
                  uint8_t* f = pctx->fetch(pctx, pBuf, x, y, len);

                  layer->toSecondary(pCur, pCur, len, NULL);

                  if (cover == 0xFF)
                  {
                    vspan(pCur, f, len, &closure);
                  }
                  else
                  {
                    vspan_a8_const(pCur, f, cover, len, &closure);
                  }

                  layer->fromSecondary(pCur, pCur, len, NULL);
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

#if 0
    // Color filter source type.
    case PAINTER_SOURCE_COLOR_FILTER:
    {
      uint8_t* pBuf = ctx->getBuffer((uint)(clipState->clipBox.getWidth()) * 4);
      if (!pBuf) return;

      int format = layer->format;

      const ImageFilterEngine* cfEngine = capsState->colorFilter->getEngine();
      const void* cfRasterPaintContext = cfEngine->getContext();
      ColorFilterFn cspan = cfEngine->getColorFilterFn(format);

      RasterEngine::VSpanMskFn vspan_a8 =
        RasterEngine::functionMap->composite[OPERATOR_SRC][format].vspan_a8[format];
      RasterEngine::VSpanMskConstFn vspan_a8_const =
        RasterEngine::functionMap->composite[OPERATOR_SRC][format].vspan_a8_const[format];

      RENDER_LOOP(bltColorFilter,
      {
        const Scanline32::Span* span = scanline->getSpansData();

        for (;;)
        {
          int x = span->x;
          int len = span->len;

          pCur = pBase + x * bpp;

          if (len > 0)
          {
            cspan(cfRasterPaintContext, pBuf, pCur, len);
            vspan_a8(pCur, pBuf, span->covers, len, &closure);
          }
          else
          {
            len = -len;
            FOG_ASSERT(len > 0);

            uint32_t cover = (uint32_t)*(span->covers);
            if (cover == 0xFF)
            {
              cspan(cfRasterPaintContext, pCur, pCur, len);
            }
            else
            {
              cspan(cfRasterPaintContext, pBuf, pCur, len);
              vspan_a8_const(pCur, pBuf, cover, len, &closure);
            }
          }

          if (--numSpans == 0) break;
          ++span;
        }
      });

      cfEngine->releaseContext(cfRasterPaintContext);
      break;
    }
#endif
  }
#undef RENDER_LOOP
}

} // Fog namespace
