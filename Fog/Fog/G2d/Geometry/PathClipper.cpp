// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Internals_p.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// ============================================================================
// [Clipping Area]
// ============================================================================
//
// +----------+----------+----------+---->
// |          |          |          | (x)
// | CLIP_Y0  | CLIP_Y0  | CLIP_Y0  |
// | CLIP_X0  |          | CLIP_X1  |
// |          |          |          |
// |          |          |          |
// +----------+----------+----------+
// |          |..........|          |
// |          |..........|          |
// | CLIP_X0  |.VISIBLE..| CLIP_X1  |
// |          |..........|          |
// |          |..........|          |
// +----------+----------+----------+
// |          |          |          |
// |          |          |          |
// | CLIP_X0  |          | CLIP_X1  |
// | CLIP_Y1  | CLIP_Y1  | CLIP_Y1  |
// |          |          |          |
// +----------+----------+----------+
// | (y)
// v

// ============================================================================
// [Line Properties]
// ============================================================================
//
// +--------------+---->
// |              | (x)
// |   . p0       |
// |    \         |
// |     \        |
// |      \       |
// |       \. p1  |
// |              |
// +--------------+
// | (y)
// v
//
// dx = p1.x - p0.x
// dy = p1.y - p0.y
// slope = dx / dy
//
// To intersect with clip box axis (cx or cy), use:
//
//   _x = cx
//   _y = p0.y + (cx - p0.x) * dy / dx
//
//   _x = p0.x + (cy - p0.y) * dx / dy
//   _y = cy

#define NO_INITIAL_FLAGS 0xFFFFFFFF

// ============================================================================
// [Fog::PathClipper - Helpers - GetFlags]
// ============================================================================

template<typename NumT>
static FOG_INLINE uint32_t PathClipperT_getFlags(const NumT_(Point)& pt, const NumT_(Box)& box)
{
  return ((uint32_t)(pt.x < box.x0) << CLIP_SHIFT_X0) | ((uint32_t)(pt.y < box.y0) << CLIP_SHIFT_Y0) |
         ((uint32_t)(pt.x > box.x1) << CLIP_SHIFT_X1) | ((uint32_t)(pt.y > box.y1) << CLIP_SHIFT_Y1) ;
}

// ============================================================================
// [Fog::PathClipper - Helpers - ClipPoint]
// ============================================================================

template<typename NumT>
static FOG_INLINE void PathClipperT_clipPoint(NumT_(Point)& pt, const NumT_(Box)& box)
{
  if (pt.x < box.x0) pt.x = box.x0;
  if (pt.y < box.y0) pt.y = box.y0;
  if (pt.x > box.x1) pt.x = box.x1;
  if (pt.y > box.y1) pt.y = box.y1;
}

// ============================================================================
// [Fog::PathClipper - Helpers - RemoveRedundantLines]
// ============================================================================

// Remove the redundant lines from the given array.
template<typename NumT>
static FOG_INLINE NumT_(Point)* PathClipperT_removeRedundantLines(NumT_(Point)* pts, size_t length)
{
  size_t i = length;
  if (i < 3) return pts + length;

  // Detect Loop.
  bool redundantX = (pts[0].x == pts[1].x);
  bool redundantY = (pts[0].y == pts[1].y);

  i -= 2;
  pts += 2;

  do {
    bool currentX = pts[-1].x == pts[0].x;
    bool currentY = pts[-1].y == pts[0].y;

    if ((redundantX & currentX) | (redundantY & currentY))
      goto _RemoveLoop;

    redundantX = currentX;
    redundantY = currentY;

    i--;
    pts++;
  } while (i);

  return pts;

_RemoveLoop:
  {
    NumT_(Point)* cur = pts--;

    do {
      bool currentX = cur[-1].x == cur[0].x;
      bool currentY = cur[-1].y == cur[0].y;

      if ((redundantX & currentX) | (redundantY & currentY))
      {
        redundantX &= currentX;
        redundantY &= currentY;
      }
      else
      {
        redundantX = currentX;
        redundantY = currentY;
        pts++;
      }

      pts[0] = cur[0];

      i--;
      cur++;
    } while (i);
  }

  return ++pts;
}

// ============================================================================
// [Fog::PathClipper - Helpers - T]
// ============================================================================

template<typename NumT>
static FOG_INLINE void keepInterestingTSValues(NumT* t, uint32_t* s, int& length)
{
  NumT* pt = t;
  uint32_t* st = s;

  for (int i = 0; i < length; i++)
  {
    if (t[i] <= NumT(0.0) || t[i] >= NumT(1.0)) continue;
    pt[0] = t[i]; pt++;
    st[0] = s[i]; st++;
  }

  length = (int)(ssize_t)(pt - t);
}

template<typename NumT>
static FOG_INLINE void sortTSValues(NumT* t, uint32_t* s, int length)
{
  NumT* tm;
  NumT* tl;

  uint32_t* sm;
  uint32_t* sl;

  for (tm = t + 1, sm = s + 1; tm < t + length; tm++, sm++)
  {
    for (tl = tm, sl = sm; tl > t && tl[-1] > tl[0]; tl--, sl--)
    {
      swap(tl[-1], tl[0]);
      swap(sl[-1], sl[0]);
    }
  }
}

// ============================================================================
// [Fog::PathClipper - MeasurePath]
// ============================================================================

template<typename NumT>
static uint32_t FOG_CDECL PathClipperT_measurePath(NumT_(PathClipper)* self, const NumT_(Path)* src)
{
  self->_lastIndex = INVALID_INDEX;

  // If path bounding box is not dirty then we can simply use it instead of
  // performing the calculation.
  if (src->_d->hasBoundingBox() && self->_clipBox.subsumes(src->_d->boundingBox))
  {
    _FOG_PATH_VERIFY_BOUNDING_BOX(*src);
    return PATH_CLIPPER_MEASURE_BOUNDED;
  }

  size_t i = src->getLength();
  const uint8_t* cmd = src->getCommands();
  const NumT_(Point)* pts = src->getVertices();
  const NumT_(Box)& clipBox = self->_clipBox;

  bool hasInitial = false;

  while (i)
  {
    switch (cmd[0])
    {
      // ----------------------------------------------------------------------
      // [Move-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_MOVE_TO:
        self->_lastMoveTo = pts[0];
        hasInitial = true;

      // ----------------------------------------------------------------------
      // [Line-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_LINE_TO:
        if (FOG_UNLIKELY(!hasInitial)) goto _Invalid;

        if (pts[0].x < clipBox.x0 || pts[0].y < clipBox.y0 ||
            pts[0].x > clipBox.x1 || pts[0].y > clipBox.y1)
        {
          goto _Unbounded;
        }

        i--;
        cmd++;
        pts++;
        break;

      // ----------------------------------------------------------------------
      // [Quad-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_QUAD_TO:
        FOG_ASSERT(i >= 2);
        if (FOG_UNLIKELY(!hasInitial)) goto _Invalid;

        if (pts[0].x < clipBox.x0 || pts[0].y < clipBox.y0 ||
            pts[0].x > clipBox.x1 || pts[0].y > clipBox.y1 ||
            pts[1].x < clipBox.x0 || pts[1].y < clipBox.y0 ||
            pts[1].x > clipBox.x1 || pts[1].y > clipBox.y1)
        {
          goto _Unbounded;
        }

        i   -= 2;
        cmd += 2;
        pts += 2;
        break;

      // ----------------------------------------------------------------------
      // [Cubic-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_CUBIC_TO:
        FOG_ASSERT(i >= 3);
        if (FOG_UNLIKELY(!hasInitial)) goto _Invalid;

        if (pts[0].x < clipBox.x0 || pts[0].y < clipBox.y0 ||
            pts[0].x > clipBox.x1 || pts[0].y > clipBox.y1 ||
            pts[1].x < clipBox.x0 || pts[1].y < clipBox.y0 ||
            pts[1].x > clipBox.x1 || pts[1].y > clipBox.y1 ||
            pts[2].x < clipBox.x0 || pts[2].y < clipBox.y0 ||
            pts[2].x > clipBox.x1 || pts[2].y > clipBox.y1)
        {
          goto _Unbounded;
        }

        i   -= 3;
        cmd += 3;
        pts += 3;
        break;

      // ----------------------------------------------------------------------
      // [Close]
      // ----------------------------------------------------------------------

      case PATH_CMD_CLOSE:
        hasInitial = false;

        i--;
        cmd++;
        pts++;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }
  return PATH_CLIPPER_MEASURE_BOUNDED;

_Unbounded:
  self->_lastIndex = (size_t)(cmd - src->getCommands());
  return PATH_CLIPPER_MEASURE_UNBOUNDED;

_Invalid:
  return PATH_CLIPPER_MEASURE_INVALID;
}

// ============================================================================
// [Fog::PathClipper - ContinuePath]
// ============================================================================
template<typename NumT>
static err_t FOG_CDECL PathClipperT_continuePath(NumT_(PathClipper)* self,
  NumT_(Path)* dst, const NumT_(Path)* src)
{
  // Prevent using the same source and destination.
  if (FOG_UNLIKELY(dst == src))
  {
    NumT_(Path) tmp(*src);
    return self->continuePathData(*dst, tmp.getVertices(), tmp.getCommands(), tmp.getLength());
  }
  else
  {
    return self->continuePathData(*dst, src->getVertices(), src->getCommands(), src->getLength());
  }
}

template<typename NumT>
static err_t FOG_CDECL PathClipperT_continuePathData(NumT_(PathClipper)* self,
  NumT_(Path)* dst, const NumT_(Point)* srcPts, const uint8_t* srcCmd, size_t srcLength)
{
  size_t i = srcLength;
  if (i == 0) return ERR_OK;

  size_t dstInitialLength = dst->getLength();
  size_t dstIndex;

  size_t dstCapacity = dstInitialLength + srcLength + 64;
  if (dstCapacity < dst->getLength()) return ERR_RT_OVERFLOW;
  FOG_RETURN_ON_ERROR(dst->reserve(dstCapacity));

  size_t startIndex = self->_lastIndex;
  if (startIndex != INVALID_INDEX && startIndex >= i)
    return ERR_RT_INVALID_STATE;

  const uint8_t* unchangedCmd = srcCmd;
  const NumT_(Box)& clipBox = self->_clipBox;

  NumT_(Point) initialPoint(NumT(0.0), NumT(0.0));

  uint32_t initialFlags = NO_INITIAL_FLAGS;
  uint32_t previousFlags = CLIP_SIDE_NONE;
  uint32_t currentFlags = CLIP_SIDE_NONE;

  if (startIndex != INVALID_INDEX && startIndex != 0)
  {
    i      -= startIndex;
    srcCmd += startIndex;
    srcPts += startIndex;

    if (srcCmd[-1] != PATH_CMD_CLOSE)
    {
      FOG_ASSERT(srcPts[-1].x >= clipBox.x0);
      FOG_ASSERT(srcPts[-1].y >= clipBox.y0);
      FOG_ASSERT(srcPts[-1].x <= clipBox.x1);
      FOG_ASSERT(srcPts[-1].y <= clipBox.y1);

      initialFlags = CLIP_SIDE_NONE;
      initialPoint = self->_lastMoveTo;
    }
  }
  goto _DetectLoopDo;

_DetectLoop:
  // Record the current position in the source path so the unchanged vertices
  // can be later copied into the destination path.
  unchangedCmd = srcCmd;

_DetectLoopDo:
  while (i)
  {
    switch (srcCmd[0])
    {
      // ----------------------------------------------------------------------
      // [Move-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_MOVE_TO:
        // Need to clip the line to connect to the initial point?
        if (initialFlags != NO_INITIAL_FLAGS && (initialFlags | previousFlags) != CLIP_SIDE_NONE)
          goto _ClipLoop;

        initialFlags = NO_INITIAL_FLAGS;
        currentFlags = PathClipperT_getFlags<NumT>(srcPts[0], clipBox);

        if (currentFlags != CLIP_SIDE_NONE)
          goto _ClipLoop;

        initialPoint = srcPts[0];
        initialFlags = currentFlags;

        i--;
        srcCmd++;
        srcPts++;
        break;

      // ----------------------------------------------------------------------
      // [Line-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_LINE_TO:
        if (FOG_UNLIKELY(initialFlags == NO_INITIAL_FLAGS))
          goto _Invalid;

        currentFlags = PathClipperT_getFlags<NumT>(srcPts[0], clipBox);
        if (currentFlags != CLIP_SIDE_NONE)
          goto _ClipLoop;

        i--;
        srcCmd++;
        srcPts++;
        break;

      // ----------------------------------------------------------------------
      // [Quad-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_QUAD_TO:
        FOG_ASSERT(i >= 2);
        if (FOG_UNLIKELY(initialFlags == NO_INITIAL_FLAGS))
          goto _Invalid;

        currentFlags = PathClipperT_getFlags<NumT>(srcPts[0], clipBox) |
                       PathClipperT_getFlags<NumT>(srcPts[1], clipBox) ;
        if (currentFlags != CLIP_SIDE_NONE)
          goto _ClipLoop;

        i      -= 2;
        srcCmd += 2;
        srcPts += 2;
        break;

      // ----------------------------------------------------------------------
      // [Cubic-To]
      // ----------------------------------------------------------------------

      case PATH_CMD_CUBIC_TO:
        FOG_ASSERT(i >= 3);
        if (FOG_UNLIKELY(initialFlags == NO_INITIAL_FLAGS))
          goto _Invalid;

        currentFlags = PathClipperT_getFlags<NumT>(srcPts[0], clipBox) |
                       PathClipperT_getFlags<NumT>(srcPts[1], clipBox) |
                       PathClipperT_getFlags<NumT>(srcPts[2], clipBox) ;
        if (currentFlags != CLIP_SIDE_NONE)
          goto _ClipLoop;

        i      -= 3;
        srcCmd += 3;
        srcPts += 3;
        break;

      // ----------------------------------------------------------------------
      // [Close]
      // ----------------------------------------------------------------------

      case PATH_CMD_CLOSE:
        if (FOG_UNLIKELY(initialFlags == NO_INITIAL_FLAGS))
          goto _Invalid;

        // Need clip to connect with the initial point?
        if ((initialFlags | previousFlags) != CLIP_SIDE_NONE)
          goto _ClipLoop;

        initialFlags = NO_INITIAL_FLAGS;
        currentFlags = CLIP_SIDE_NONE;

        i--;
        srcCmd++;
        srcPts++;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }

    previousFlags = currentFlags;
  }

_ClipLoop:
  // Copy the vertices which passed through detect-loop.
  if (unchangedCmd != srcCmd)
  {
    size_t copyLength = (size_t)(srcCmd - unchangedCmd);
    if ((dstIndex = dst->_add(copyLength)) == INVALID_INDEX)
      goto _OutOfMemory;

    MemOps::copy(dst->getVerticesX() + dstIndex, srcPts - copyLength, copyLength * sizeof(NumT_(Point)));
    MemOps::copy(dst->getCommandsX() + dstIndex, srcCmd - copyLength, copyLength);
  }

  if (i)
  {
    uint8_t* dstCmd;
    NumT_(Point)* dstMark = NULL;
    NumT_(Point)* dstPts = NULL;
    NumT_(Point)* dstMax = NULL;

    NumT lx, ly;
    NumT ux, uy;
    uint32_t lf;
    uint32_t uf;

    // Reset the current flags, they will be calculated again.
    currentFlags = CLIP_SIDE_NONE;

_ClipRealloc:
    // Allocate some space for clipped vertices/commands.
    {
      const NumT_(Point)* old = dst->getVertices();

      if (dstPts != NULL)
      {
        dst->_d->length = (size_t)(dstPts - old);
      }

      size_t need = Math::max<size_t>(i * 2, 256);
      size_t dstMarkIndex = 0;

      if (dstMark)
      {
        dst->_d->length = (size_t)(dstPts - old);
        dstMarkIndex = (size_t)(dstMark - old);
      }

      if ((dstIndex = dst->_add(need)) == INVALID_INDEX)
        goto _OutOfMemory;

      dstCmd = dst->getCommandsX() + dstIndex;
      dstPts = dst->getVerticesX() + dstIndex;
      dstMark = dstMark != NULL ? dst->getVerticesX() + dstMarkIndex : dstPts;

      // Decrease the destination capacity by some safe number so we can
      // omit buffer-overflow checks in the main loop. Now it's safe to
      // use the "dstIndex >= dstCapacity" comparison in the main loop.
      dstMax = dst->getVerticesX() + dst->getCapacity() - 40;
      goto _ClipLoopDo;
    }

#define _CLIP_ADD_LINE(_X_, _Y_) \
  FOG_MACRO_BEGIN \
    dstPts[0].set(_X_, _Y_); \
    dstCmd[0] = PATH_CMD_LINE_TO; \
    \
    dstPts++; \
    dstCmd++; \
  FOG_MACRO_END

    while (i)
    {
      if (dstPts >= dstMax) goto _ClipRealloc;

_ClipLoopDo:
      switch (srcCmd[0])
      {
        // --------------------------------------------------------------------
        // [Move-To]
        // --------------------------------------------------------------------

        case PATH_CMD_MOVE_TO:
        {
          if (initialFlags != NO_INITIAL_FLAGS)
          {
            // If initial flags and current flags do match then there is nothing
            // to do, because the line is clipped in the same region. MOVE_TO is
            // simply starting a new figure.
            if ((initialFlags & currentFlags) != CLIP_SIDE_NONE)
            {
              initialFlags = NO_INITIAL_FLAGS;
            }
            else if (initialFlags == currentFlags)
            {
              FOG_ASSERT(initialFlags == CLIP_SIDE_NONE);
              FOG_ASSERT(currentFlags == CLIP_SIDE_NONE);
            }
            else
            {
              lf = previousFlags;uf = initialFlags;
              lx = srcPts[-1].x; ux = initialPoint.x;
              ly = srcPts[-1].y; uy = initialPoint.y;

              initialFlags = NO_INITIAL_FLAGS;
              goto _ClipLineCmd;
            }
          }

          currentFlags = PathClipperT_getFlags<NumT>(srcPts[0], clipBox);
          initialPoint = srcPts[0];
          initialFlags = currentFlags;

          dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts - dstMark));
          dstCmd = dst->getCommandsX() + (size_t)(dstPts - dst->getVertices());

          dstPts[0] = srcPts[0];
          dstCmd[0] = PATH_CMD_MOVE_TO;
          PathClipperT_clipPoint<NumT>(dstPts[0], clipBox);
          dstPts++;
          dstCmd++;

          dstMark = dstPts;

          i--;
          srcCmd++;
          srcPts++;
          break;
        }

        // --------------------------------------------------------------------
        // [Line-To]
        // --------------------------------------------------------------------

        case PATH_CMD_LINE_TO:
        {
          currentFlags = PathClipperT_getFlags<NumT>(srcPts[0], clipBox);

          // Fully visible, finish here and jump to detection loop.
          if ((previousFlags | currentFlags) == 0)
          {
            dstPts[0] = srcPts[0];
            dstCmd[0] = PATH_CMD_LINE_TO;

            dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts + 1 - dstMark));
            dst->_d->length = (size_t)(dstPts - dst->getVertices());

            i--;
            srcPts++;
            srcCmd++;

            previousFlags = currentFlags;
            goto _DetectLoop;
          }

          // Initialize coordinates.
          lf = previousFlags;uf = currentFlags;
          lx = srcPts[-1].x; ux = srcPts[0].x;
          ly = srcPts[-1].y; uy = srcPts[0].y;

          i--;
          srcPts++;
          srcCmd++;

_ClipLineCmd:
          // Not visible.
          if (lf & uf)
          {
            dstPts[0].set(ux, uy);
            dstCmd[0] = PATH_CMD_LINE_TO;
            PathClipperT_clipPoint<NumT>(dstPts[0], clipBox);
            dstPts++;
            dstCmd++;
          }
          // Maybe visible.
          else
          {
            NumT dx = ux - lx;
            NumT dy = uy - ly;

            NumT cx;
            NumT cy;

            switch (lf)
            {
              case CLIP_SIDE_NONE:
                goto _ClipLineCmd_SkipLineTo0;

              case CLIP_SIDE_LT:
                cx = clipBox.x0;
                cy = ly + (cx - lx) * dy / dx;
                if (cy >= clipBox.y0) break;
                goto _ClipLineCmd_TryTop0;

              case CLIP_SIDE_RT:
                cx = clipBox.x1;
                cy = ly + (cx - lx) * dy / dx;
                if (cy >= clipBox.y0) break;
                goto _ClipLineCmd_TryTop0;

              case CLIP_SIDE_TOP:
_ClipLineCmd_TryTop0:
                cy = clipBox.y0;
                cx = lx + (cy - ly) * dx / dy;

                if (cx < clipBox.x0) cx = clipBox.x0;
                if (cx > clipBox.x1) cx = clipBox.x1;
                break;

              case CLIP_SIDE_LB:
                cx = clipBox.x0;
                cy = ly + (cx - lx) * dy / dx;
                if (cy <= clipBox.y1) break;
                goto _ClipLineCmd_TryBottom0;

              case CLIP_SIDE_RB:
                cx = clipBox.x1;
                cy = ly + (cx - lx) * dy / dx;
                if (cy <= clipBox.y1) break;
                goto _ClipLineCmd_TryBottom0;

              case CLIP_SIDE_BOTTOM:
_ClipLineCmd_TryBottom0:
                cy = clipBox.y1;
                cx = lx + (clipBox.y1 - ly) * dx / dy;

                if (cx < clipBox.x0) cx = clipBox.x0;
                if (cx > clipBox.x1) cx = clipBox.x1;
                break;

              case CLIP_SIDE_RIGHT:
                cx = clipBox.x1;
                cy = ly + (cx - lx) * dy / dx;
                goto _ClipLineCmd_LeftRight0;

              case CLIP_SIDE_LEFT:
                cx = clipBox.x0;
                cy = ly + (cx - lx) * dy / dx;

_ClipLineCmd_LeftRight0:
                if (cy < clipBox.y0) cy = clipBox.y0;
                if (cy > clipBox.y1) cy = clipBox.y1;
                break;

              default:
                FOG_ASSERT_NOT_REACHED();
            }

            _CLIP_ADD_LINE(cx, cy);

_ClipLineCmd_SkipLineTo0:
            switch (uf)
            {
              case CLIP_SIDE_NONE:
                _CLIP_ADD_LINE(ux, uy);
                goto _ClipLineCmd_Done;

              case CLIP_SIDE_LT:
                cx = clipBox.x0;
                cy = uy + (cx - ux) * dy / dx;
                if (cy >= clipBox.y0) break;
                goto _ClipLineCmd_TryTop1;

              case CLIP_SIDE_RT:
                cx = clipBox.x1;
                cy = uy + (cx - ux) * dy / dx;
                if (cy >= clipBox.y0) break;
                goto _ClipLineCmd_TryTop1;

              case CLIP_SIDE_TOP:
_ClipLineCmd_TryTop1:
                cy = clipBox.y0;
                cx = ux + (cy - uy) * dx / dy;

                if (cx < clipBox.x0) cx = clipBox.x0;
                if (cx > clipBox.x1) cx = clipBox.x1;
                break;

              case CLIP_SIDE_LB:
                cx = clipBox.x0;
                cy = uy + (cx - ux) * dy / dx;
                if (cy <= clipBox.y1) break;
                goto _ClipLineCmd_TryBottom1;

              case CLIP_SIDE_RB:
                cx = clipBox.x1;
                cy = uy + (cx - ux) * dy / dx;
                if (cy <= clipBox.y1) break;
                goto _ClipLineCmd_TryBottom1;

              case CLIP_SIDE_BOTTOM:
_ClipLineCmd_TryBottom1:
                cy = clipBox.y1;
                cx = ux + (cy - uy) * dx / dy;

                if (cx < clipBox.x0) cx = clipBox.x0;
                if (cx > clipBox.x1) cx = clipBox.x1;
                break;

              case CLIP_SIDE_RIGHT:
                cx = clipBox.x1;
                cy = uy + (cx - ux) * dy / dx;
                goto _ClipLineCmd_LeftRight1;

              case CLIP_SIDE_LEFT:
                cx = clipBox.x0;
                cy = uy + (cx - ux) * dy / dx;

_ClipLineCmd_LeftRight1:
                if (cy < clipBox.y0) cy = clipBox.y0;
                if (cy > clipBox.y1) cy = clipBox.y1;
                break;

              default:
                FOG_ASSERT_NOT_REACHED();
            }

            _CLIP_ADD_LINE(cx, cy);

            switch (uf)
            {
              case CLIP_SIDE_LT    : ux = clipBox.x0; uy = clipBox.y0; break;
              case CLIP_SIDE_RT    : ux = clipBox.x1; uy = clipBox.y0; break;
              case CLIP_SIDE_LB    : ux = clipBox.x0; uy = clipBox.y1; break;
              case CLIP_SIDE_RB    : ux = clipBox.x1; uy = clipBox.y1; break;

              case CLIP_SIDE_NONE  :
              case CLIP_SIDE_TOP   :
              case CLIP_SIDE_BOTTOM:
              case CLIP_SIDE_RIGHT :
              case CLIP_SIDE_LEFT  :
                goto _ClipLineCmd_Done;

              default:
                FOG_ASSERT_NOT_REACHED();
            }

            if (cx != ux || cy != uy) _CLIP_ADD_LINE(ux, uy);
          }

_ClipLineCmd_Done:
          if (initialFlags == NO_INITIAL_FLAGS)
          {
            dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts - dstMark));
            dstMark = dstPts;
            dstCmd = dst->getCommandsX() + (size_t)(dstPts - dst->getVertices());
          }
          break;
        }

        // --------------------------------------------------------------------
        // [Quad-To]
        // --------------------------------------------------------------------

        case PATH_CMD_QUAD_TO:
        {
          const NumT_(Point)* p = srcPts - 1;

          uint32_t cp1 = PathClipperT_getFlags<NumT>(p[1], clipBox);
          currentFlags = PathClipperT_getFlags<NumT>(p[2], clipBox);

          uint32_t sides = previousFlags | cp1 | currentFlags;

          // Fully visible, finish here and jump to detection loop.
          if (sides == CLIP_SIDE_NONE)
          {
            dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts - dstMark));
            dstCmd = dst->getCommandsX() + (size_t)(dstPts - dst->getVertices());

            dstPts[0] = srcPts[0];
            dstPts[1] = srcPts[1];
            dstCmd[0] = PATH_CMD_QUAD_TO;
            dstCmd[1] = PATH_CMD_QUAD_TO;
            dstPts += 2;
            dstCmd += 2;

            dstMark = dstPts;
            dst->_d->length = (size_t)(dstPts - dst->getVertices());

            i -= 2;
            srcPts += 2;
            srcCmd += 2;

            previousFlags = currentFlags;
            goto _DetectLoop;
          }

          // Not visible.
          if ((previousFlags & cp1 & currentFlags) != CLIP_SIDE_NONE)
          {
            dstPts[0] = srcPts[1];
            dstCmd[0] = PATH_CMD_LINE_TO;
            PathClipperT_clipPoint<NumT>(dstPts[0], clipBox);
            dstPts++;
            dstCmd++;
          }
          // Maybe visible.
          else
          {
            NumT ax, ay, bx, by, cx, cy;
            _FOG_QUAD_EXTRACT_PARAMETERS(NumT, ax, ay, bx, by, cx, cy, p);

            NumT func[3];
            NumT t[8+1];
            uint32_t s[8+1];

            int tIndex = 0;
            int tLength = 0;

            func[0] = ax;
            func[1] = bx;

            if (sides & CLIP_SIDE_X0)
            {
              func[2] = cx - clipBox.x0;
              s[tLength + 0] = CLIP_SIDE_X0;
              s[tLength + 1] = CLIP_SIDE_X0;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_QUADRATIC);
            }

            if (sides & CLIP_SIDE_X1)
            {
              func[2] = cx - clipBox.x1;
              s[tLength + 0] = CLIP_SIDE_X1;
              s[tLength + 1] = CLIP_SIDE_X1;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_QUADRATIC);
            }

            func[0] = ay;
            func[1] = by;

            if (sides & CLIP_SIDE_Y0)
            {
              func[2] = cy - clipBox.y0;
              s[tLength + 0] = CLIP_SIDE_Y0;
              s[tLength + 1] = CLIP_SIDE_Y0;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_QUADRATIC);
            }

            if (sides & CLIP_SIDE_Y1)
            {
              func[2] = cy - clipBox.y1;
              s[tLength + 0] = CLIP_SIDE_Y1;
              s[tLength + 1] = CLIP_SIDE_Y1;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_QUADRATIC);
            }

            // Remove t-values outside of [0, 1] range and sort them.
            keepInterestingTSValues(t, s, tLength);
            sortTSValues(t, s, tLength);

            // Include t=1.
            t[tLength] = NumT(1.0);
            s[tLength] = CLIP_SIDE_NONE;
            tLength++;

            NumT tVal;
            NumT tCut = NumT(0.0);

            uint32_t tSide = CLIP_SIDE_NONE;

            for (tIndex = 0; tIndex < tLength; tIndex++)
            {
              NumT_(Point) tp(UNINITIALIZED);
              uint32_t tf;

              tVal   = t[tIndex];
              tSide |= s[tIndex];

              // Do not process the same t-value again.
              if (tVal == tCut) continue;

              // Calculate the point position.
              switch (tSide)
              {
                case CLIP_SIDE_NONE:
                  if (tVal == NumT(1.0))
                  {
                    tp.x = p[2].x;
                    tp.y = p[2].y;
                  }
                  else
                  {
                    tp.x = ax * Math::pow2(tVal) + bx * tVal + cx;
                    tp.y = ay * Math::pow2(tVal) + by * tVal + cy;
                  }
                  break;

                case CLIP_SIDE_LT:
                  tp.x = clipBox.x0;
                  tp.y = clipBox.y0;
                  break;

                case CLIP_SIDE_RT:
                  tp.x = clipBox.x1;
                  tp.y = clipBox.y0;
                  break;

                case CLIP_SIDE_TOP:
                  tp.y = clipBox.y0;
                  goto _ClipQuadCmd_EvaluateX;

                case CLIP_SIDE_LB:
                  tp.x = clipBox.x0;
                  tp.y = clipBox.y1;
                  break;

                case CLIP_SIDE_RB:
                  tp.x = clipBox.x1;
                  tp.y = clipBox.y1;
                  break;

                case CLIP_SIDE_BOTTOM:
                  tp.y = clipBox.y1;
_ClipQuadCmd_EvaluateX:
                  tp.x = ax * Math::pow2(tVal) + bx * tVal + cx;
                  break;

                case CLIP_SIDE_RIGHT:
                  tp.x = clipBox.x1;
                  goto _ClipQuadCmd_EvaluateY;

                case CLIP_SIDE_LEFT:
                  tp.x = clipBox.x0;
_ClipQuadCmd_EvaluateY:
                  tp.y = ay * Math::pow2(tVal) + by * tVal + cy;
                  break;

                default:
                  FOG_ASSERT_NOT_REACHED();
              }

              if ((tf = PathClipperT_getFlags<NumT>(tp, clipBox)) != CLIP_SIDE_NONE || previousFlags != CLIP_SIDE_NONE)
              {
                // The point is clipped-out, emit line.
                PathClipperT_clipPoint<NumT>(tp, clipBox);
                dstPts[0] = tp;
                dstCmd[0] = PATH_CMD_LINE_TO;

                dstPts++;
                dstCmd++;

                previousFlags = CLIP_SIDE_NONE;
              }
              else
              {
                // Test whether the curve segment is visible.
                NumT_(Point) center(UNINITIALIZED);
                NumT tMiddle = tCut + (tVal - tCut) * NumT(0.5);
                center.x = ax * Math::pow2(tMiddle) + bx * tMiddle + cx;
                center.y = ay * Math::pow2(tMiddle) + by * tMiddle + cy;

                if (PathClipperT_getFlags<NumT>(center, clipBox) == CLIP_SIDE_NONE)
                {
                  NumT dt = (tVal - tCut) * NumT(0.5);

                  dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts - dstMark));
                  dstCmd = dst->getCommandsX() + (size_t)(dstPts - dst->getVertices());

                  // Derivative: 2a*t + b.
                  NumT_(Point) cp1(NumT(2.0) * ax * tVal + bx, NumT(2.0) * ay * tVal + by);

                  cp1.x *= dt;
                  cp1.y *= dt;

                  dstPts[0].x = tp.x - cp1.x;
                  dstPts[0].y = tp.y - cp1.y;
                  dstPts[1].x = tp.x;
                  dstPts[1].y = tp.y;

                  dstCmd[0] = PATH_CMD_QUAD_TO;
                  dstCmd[1] = PATH_CMD_QUAD_TO;

                  dstPts += 2;
                  dstCmd += 2;

                  dstMark = dstPts;
                }
                else
                {
                  dstPts[0] = tp;
                  dstCmd[0] = PATH_CMD_LINE_TO;

                  dstPts++;
                  dstCmd++;
                }
              }

              tSide = CLIP_SIDE_NONE;
              tCut = tVal;
            }
          }

          i -= 2;
          srcPts += 2;
          srcCmd += 2;
          break;
        }

        // --------------------------------------------------------------------
        // [Cubic-To]
        // --------------------------------------------------------------------

        case PATH_CMD_CUBIC_TO:
        {
          const NumT_(Point)* p = srcPts - 1;

          uint32_t cp1 = PathClipperT_getFlags<NumT>(p[1], clipBox);
          uint32_t cp2 = PathClipperT_getFlags<NumT>(p[2], clipBox);
          currentFlags = PathClipperT_getFlags<NumT>(p[3], clipBox);

          uint32_t sides = previousFlags | cp1 | cp2 | currentFlags;

          // Fully visible, finish here and jump to detection loop.
          if (sides == CLIP_SIDE_NONE)
          {
            dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts - dstMark));
            dstCmd = dst->getCommandsX() + (size_t)(dstPts - dst->getVertices());

            dstPts[0] = srcPts[0];
            dstPts[1] = srcPts[1];
            dstPts[2] = srcPts[2];
            dstCmd[0] = PATH_CMD_CUBIC_TO;
            dstCmd[1] = PATH_CMD_CUBIC_TO;
            dstCmd[2] = PATH_CMD_CUBIC_TO;
            dstPts += 3;
            dstCmd += 3;

            dstMark = dstPts;
            dst->_d->length = (size_t)(dstPts - dst->getVertices());

            i -= 3;
            srcPts += 3;
            srcCmd += 3;

            previousFlags = currentFlags;
            goto _DetectLoop;
          }

          // Not visible.
          if ((previousFlags & cp1 & cp2 & currentFlags) != CLIP_SIDE_NONE)
          {
            dstPts[0] = srcPts[2];
            dstCmd[0] = PATH_CMD_LINE_TO;
            PathClipperT_clipPoint<NumT>(dstPts[0], clipBox);
            dstPts++;
            dstCmd++;
          }
          // Maybe visible.
          else
          {
            NumT ax, ay, bx, by, cx, cy, dx, dy;
            _FOG_CUBIC_EXTRACT_PARAMETERS(NumT, ax, ay, bx, by, cx, cy, dx, dy, p);

            NumT func[4];
            NumT t[12+1];
            uint32_t s[12+1];

            int tIndex = 0;
            int tLength = 0;

            func[0] = ax;
            func[1] = bx;
            func[2] = cx;

            if (sides & CLIP_SIDE_X0)
            {
              func[3] = dx - clipBox.x0;
              s[tLength + 0] = CLIP_SIDE_X0;
              s[tLength + 1] = CLIP_SIDE_X0;
              s[tLength + 2] = CLIP_SIDE_X0;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_CUBIC);
            }

            if (sides & CLIP_SIDE_X1)
            {
              func[3] = dx - clipBox.x1;
              s[tLength + 0] = CLIP_SIDE_X1;
              s[tLength + 1] = CLIP_SIDE_X1;
              s[tLength + 2] = CLIP_SIDE_X1;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_CUBIC);
            }

            func[0] = ay;
            func[1] = by;
            func[2] = cy;

            if (sides & CLIP_SIDE_Y0)
            {
              func[3] = dy - clipBox.y0;
              s[tLength + 0] = CLIP_SIDE_Y0;
              s[tLength + 1] = CLIP_SIDE_Y0;
              s[tLength + 2] = CLIP_SIDE_Y0;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_CUBIC);
            }

            if (sides & CLIP_SIDE_Y1)
            {
              func[3] = dy - clipBox.y1;
              s[tLength + 0] = CLIP_SIDE_Y1;
              s[tLength + 1] = CLIP_SIDE_Y1;
              s[tLength + 2] = CLIP_SIDE_Y1;
              tLength += Math::solve(t + tLength, func, MATH_SOLVE_CUBIC);
            }

            // Remove t-values outside of [0, 1] range and sort them.
            keepInterestingTSValues(t, s, tLength);
            sortTSValues(t, s, tLength);

            // Include t=1.
            t[tLength] = NumT(1.0);
            s[tLength] = CLIP_SIDE_NONE;
            tLength++;

            NumT tVal;
            NumT tCut = NumT(0.0);

            uint32_t tSide = CLIP_SIDE_NONE;

            for (tIndex = 0; tIndex < tLength; tIndex++)
            {
              NumT_(Point) tp(UNINITIALIZED);
              uint32_t tf;

              tVal   = t[tIndex];
              tSide |= s[tIndex];

              // Do not process the same t-value again.
              if (tVal == tCut) continue;

              // Calculate the point position.
              switch (tSide)
              {
                case CLIP_SIDE_NONE:
                  if (tVal == NumT(1.0))
                  {
                    tp.x = p[3].x;
                    tp.y = p[3].y;
                  }
                  else
                  {
                    tp.x = ax * Math::pow3(tVal) + bx * Math::pow2(tVal) + cx * tVal + dx;
                    tp.y = ay * Math::pow3(tVal) + by * Math::pow2(tVal) + cy * tVal + dy;
                  }
                  break;

                case CLIP_SIDE_LT:
                  tp.x = clipBox.x0;
                  tp.y = clipBox.y0;
                  break;

                case CLIP_SIDE_RT:
                  tp.x = clipBox.x1;
                  tp.y = clipBox.y0;
                  break;

                case CLIP_SIDE_TOP:
                  tp.y = clipBox.y0;
                  goto _ClipCubicCmd_EvaluateX;

                case CLIP_SIDE_LB:
                  tp.x = clipBox.x0;
                  tp.y = clipBox.y1;
                  break;

                case CLIP_SIDE_RB:
                  tp.x = clipBox.x1;
                  tp.y = clipBox.y1;
                  break;

                case CLIP_SIDE_BOTTOM:
                  tp.y = clipBox.y1;
_ClipCubicCmd_EvaluateX:
                  tp.x = ax * Math::pow3(tVal) + bx * Math::pow2(tVal) + cx * tVal + dx;
                  break;

                case CLIP_SIDE_RIGHT:
                  tp.x = clipBox.x1;
                  goto _ClipCubicCmd_EvaluateY;

                case CLIP_SIDE_LEFT:
                  tp.x = clipBox.x0;
_ClipCubicCmd_EvaluateY:
                  tp.y = ay * Math::pow3(tVal) + by * Math::pow2(tVal) + cy * tVal + dy;
                  break;

                default:
                  FOG_ASSERT_NOT_REACHED();
              }

              if ((tf = PathClipperT_getFlags<NumT>(tp, clipBox)) != CLIP_SIDE_NONE || previousFlags != CLIP_SIDE_NONE)
              {
                // The point is clipped-out, emit line.
                PathClipperT_clipPoint<NumT>(tp, clipBox);
                dstPts[0] = tp;
                dstCmd[0] = PATH_CMD_LINE_TO;

                dstPts++;
                dstCmd++;

                previousFlags = CLIP_SIDE_NONE;
              }
              else
              {
                // Test whether the curve segment is visible.
                NumT_(Point) center(UNINITIALIZED);
                NumT tMiddle = tCut + (tVal - tCut) * NumT(0.5);
                center.x = ax * Math::pow3(tMiddle) + bx * Math::pow2(tMiddle) + cx * tMiddle + dx;
                center.y = ay * Math::pow3(tMiddle) + by * Math::pow2(tMiddle) + cy * tMiddle + dy;

                if (PathClipperT_getFlags<NumT>(center, clipBox) == CLIP_SIDE_NONE)
                {
                  NumT dt = (tVal - tCut) * NumT(1.0 / 3.0);

                  dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts - dstMark));
                  dstCmd = dst->getCommandsX() + (size_t)(dstPts - dst->getVertices());

                  // Derivative: 3*a*t^2 + 2*b*t + c.
                  NumT_(Point) cp1(
                    NumT(3.0) * ax * Math::pow2(tCut) + NumT(2.0) * bx * tCut + cx,
                    NumT(3.0) * ay * Math::pow2(tCut) + NumT(2.0) * by * tCut + cy);
                  NumT_(Point) cp2(
                    NumT(3.0) * ax * Math::pow2(tVal) + NumT(2.0) * bx * tVal + cx,
                    NumT(3.0) * ay * Math::pow2(tVal) + NumT(2.0) * by * tVal + cy);

                  cp1.x *= dt; cp1.y *= dt;
                  cp2.x *= dt; cp2.y *= dt;

                  dstPts[0].x = dstPts[-1].x + cp1.x;
                  dstPts[0].y = dstPts[-1].y + cp1.y;
                  dstPts[1].x = tp.x - cp2.x;
                  dstPts[1].y = tp.y - cp2.y;
                  dstPts[2].x = tp.x;
                  dstPts[2].y = tp.y;

                  dstCmd[0] = PATH_CMD_CUBIC_TO;
                  dstCmd[1] = PATH_CMD_CUBIC_TO;
                  dstCmd[2] = PATH_CMD_CUBIC_TO;

                  dstPts += 3;
                  dstCmd += 3;

                  dstMark = dstPts;
                }
                else
                {
                  dstPts[0] = tp;
                  dstCmd[0] = PATH_CMD_LINE_TO;

                  dstPts++;
                  dstCmd++;
                }
              }

              tSide = CLIP_SIDE_NONE;
              tCut = tVal;
            }
          }

          i -= 3;
          srcPts += 3;
          srcCmd += 3;
          break;
        }

        // --------------------------------------------------------------------
        // [Close]
        // --------------------------------------------------------------------

        case PATH_CMD_CLOSE:
        {
          if (initialFlags != NO_INITIAL_FLAGS)
          {
            lf = previousFlags;uf = initialFlags;
            lx = srcPts[-1].x; ux = initialPoint.x;
            ly = srcPts[-1].y; uy = initialPoint.y;

            initialFlags = NO_INITIAL_FLAGS;
            goto _ClipLineCmd;
          }

          dstPts[0] = srcPts[0];
          dstCmd[0] = PATH_CMD_CLOSE;

          dstPts++;
          dstCmd++;

          i--;
          srcCmd++;
          srcPts++;
          break;
        }

        default:
          FOG_ASSERT_NOT_REACHED();
      }
      previousFlags = currentFlags;
    }

    // Handle the End-Path case.
    if (initialFlags != NO_INITIAL_FLAGS && (initialFlags | previousFlags) != CLIP_SIDE_NONE)
    {
      lf = previousFlags;uf = initialFlags;
      lx = srcPts[-1].x; ux = initialPoint.x;
      ly = srcPts[-1].y; uy = initialPoint.y;

      initialFlags = NO_INITIAL_FLAGS;
      goto _ClipLineCmd;
    }

    // Finalize.
    dstPts = PathClipperT_removeRedundantLines<NumT>(dstMark, (size_t)(dstPts - dstMark));
    dst->_d->length = (size_t)(dstPts - dst->getVertices());
  }

  dst->_d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_CMD;
  return ERR_OK;

_Invalid:
  dst->_d->length = dstInitialLength;
  dst->_d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_CMD;
  return ERR_GEOMETRY_INVALID;

_OutOfMemory:
  dst->_d->length = dstInitialLength;
  dst->_d->vType |= PATH_FLAG_DIRTY_BBOX | PATH_FLAG_DIRTY_CMD;
  return ERR_RT_OUT_OF_MEMORY;
}

// ============================================================================
// [Fog::PathClipper - ClipPath]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL PathClipperT_clipPath(NumT_(PathClipper)* self,
  NumT_(Path)* dst, const NumT_(Path)* src, const
  NumT_(Transform)* tr)
{
  self->_lastIndex = INVALID_INDEX;

  uint32_t transformType = (tr != NULL) ? tr->getType() : TRANSFORM_TYPE_IDENTITY;

  switch (transformType)
  {
    // If there is no transform the continuePath() is safe and fast.
    case TRANSFORM_TYPE_IDENTITY:
    {
      return self->continuePath(*dst, *src);
    }

    // Translation and Scaling may be considered as a rect-to-rect transform.
    // It is safe to do transformation as a second step. If the path is
    // heavily clipped then we save some CPU cycles, if the path isn't clipped
    // then it is the same as continuePath().
    case TRANSFORM_TYPE_TRANSLATION:
    case TRANSFORM_TYPE_SCALING:
    {
      NumT_(Transform) inv(UNINITIALIZED);

      if (NumI_(Transform)::invert(inv, *tr))
      {
        // Save the clip box and create new.
        NumT_(Box) box(self->_clipBox);
        inv.mapBox(self->_clipBox, self->_clipBox);

        // Clip and transform.
        size_t dstIndex = dst->getLength();
        err_t err = self->continuePath(*dst, *src);
        dst->transform(*tr, Range(dstIndex, DETECT_LENGTH));

        // Restore the clip box and return possible error.
        self->_clipBox = box;
        return err;
      }
      // ... Fall through ...
    }

    // TODO: Specialize the SWAP/ROTATION/AFFINE case.

    default:
    {
      if (dst == src)
      {
        // This is an incorrect use.
        NumT_(Path) tmp;
        FOG_RETURN_ON_ERROR(tr->mapPath(tmp, *src));

        return self->continuePath(*dst, tmp);
      }
      else
      {
        NumT_T1(PathTmp, 200) tmp;
        FOG_RETURN_ON_ERROR(tr->mapPath(tmp, *src));

        return self->continuePathData(*dst, tmp.getVertices(), tmp.getCommands(), tmp.getLength());
      }
    }
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE( PathClipper_init_SSE(void) )
FOG_CPU_DECLARE_INITIALIZER_SSE2( PathClipper_init_SSE2(void) )

FOG_NO_EXPORT void PathClipper_init(void)
{
  fog_api.pathclipperf_measurePath = PathClipperT_measurePath<float>;
  fog_api.pathclipperf_continuePath = PathClipperT_continuePath<float>;
  fog_api.pathclipperf_continuePathData = PathClipperT_continuePathData<float>;
  fog_api.pathclipperf_clipPath = PathClipperT_clipPath<float>;

  fog_api.pathclipperd_measurePath = PathClipperT_measurePath<double>;
  fog_api.pathclipperd_continuePath = PathClipperT_continuePath<double>;
  fog_api.pathclipperd_continuePathData = PathClipperT_continuePathData<double>;
  fog_api.pathclipperd_clipPath = PathClipperT_clipPath<double>;

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE( PathClipper_init_SSE() )
  FOG_CPU_USE_INITIALIZER_SSE2( PathClipper_init_SSE2() )
}

} // Fog namespace
