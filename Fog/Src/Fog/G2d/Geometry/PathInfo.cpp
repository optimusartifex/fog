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
#include <Fog/Core/Memory/MemZoneAllocator.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/QBezier.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathInfo.h>

namespace Fog {

// ============================================================================
// [Fog::PathInfoT - Helpers]
// ============================================================================

struct PathInfoFigureItemF
{
  PathInfoFigureItemF* next;
  size_t start;
  size_t end;
  PathInfoFigureFlags flags;
};

struct PathInfoFigureItemD
{
  PathInfoFigureItemD* next;
  size_t start;
  size_t end;
  PathInfoFigureFlags flags;
};

_FOG_NUM_T(PathInfoFigureItem)
_FOG_NUM_F(PathInfoFigureItem)
_FOG_NUM_D(PathInfoFigureItem)

// ============================================================================
// [Fog::PathInfoT - Generate]
// ============================================================================

template<typename NumT>
static FOG_INLINE bool PathInfoT_isAcute(
  const NumT_(Point)& a,
  const NumT_(Point)& b,
  const NumT_(Point)& c)
{
  NumT_(Point) ab(b.x - a.x, b.y - a.y);
  NumT_(Point) cb(b.x - c.x, b.y - c.y);

  return (ab.x * cb.x + ab.y * cb.y) > NumT(0);
}

#define _MERGE_BBOX(NumT, _Box_, _Point_) \
  FOG_MACRO_BEGIN \
    NumT _px = (_Point_).x; \
    NumT _py = (_Point_).y; \
    \
    if ((_Box_).x0 > _px) (_Box_).x0 = _px; \
    if ((_Box_).y0 > _py) (_Box_).y0 = _py; \
    if ((_Box_).x1 < _px) (_Box_).x1 = _px; \
    if ((_Box_).y1 < _py) (_Box_).y1 = _py; \
  FOG_MACRO_END

template<typename NumT>
static NumT_(PathInfo)* FOG_CDECL PathInfoT_generate(const NumT_(Path)* path)
{
  const NumT_(PathData)* d = path->_d;
  size_t length = d->length;

  if (length == 0)
    return NULL;

  NumT_(PathInfo)* info = NULL;
  MemZoneAllocatorTmp<16 * sizeof(NumT_(PathInfoFigureItem))> allocator(4000);

  NumT_(PathInfoFigureItem)* firstFigure = NULL;
  NumT_(PathInfoFigureItem)* currentFigure = NULL;
  size_t numberOfFigures = 0;

  const NumT_(Point)* srcPts = d->vertices;
  const uint8_t* srcCmd = d->commands;

  size_t i = 0;
  uint8_t cmd;

  const uint32_t cleanFlags = PATH_FLAG_DIRTY_BBOX  |
                              PATH_FLAG_DIRTY_CMD   |
                              PATH_FLAG_HAS_BBOX    |
                              PATH_FLAG_HAS_QBEZIER |
                              PATH_FLAG_HAS_CBEZIER ;
  uint32_t newFlags = path->_d->vType & ~cleanFlags;

  // --------------------------------------------------------------------------
  // [Collect All Figures]
  // --------------------------------------------------------------------------

  for (;;)
  {
    cmd = srcCmd[i];
    if (++i == length)
      break;

    if (PathCmd::isMoveTo(cmd))
    {
      size_t start = i - 1;
      PathInfoFigureFlags flags;
      flags.packed = 0;

      do {
        cmd = srcCmd[i];

        if (PathCmd::isLineTo(cmd))
        {
          i += 1;
        }
        else if (PathCmd::isQuadTo(cmd))
        {
          FOG_ASSERT(length - i >= 1);
          i += 2;
          flags.hasQBezier = 1;
        }
        else if (PathCmd::isCubicTo(cmd))
        {
          FOG_ASSERT(length - i >= 2);
          i += 3;
          flags.hasCBezier = 1;
        }
        else if (PathCmd::isData(cmd))
        {
          goto _Invalid;
        }
        else
        {
          FOG_ASSERT(PathCmd::isMoveTo(cmd) || PathCmd::isClose(cmd));
          if (PathCmd::isClose(cmd))
            flags.isClosed = 1;
          break;
        }
      } while (i < length);

      NumT_(PathInfoFigureItem)* newFigure = reinterpret_cast<NumT_(PathInfoFigureItem)*>(
        allocator.alloc(sizeof(NumT_(PathInfoFigureItem)))
      );

      if (FOG_IS_NULL(newFigure))
        return NULL;

      if (currentFigure == NULL)
        firstFigure = newFigure;
      else
        currentFigure->next = newFigure;

      i += flags.isClosed;
      numberOfFigures++;

      currentFigure = newFigure;
      currentFigure->next = NULL;
      currentFigure->start = start;
      currentFigure->end = i;
      currentFigure->flags.packed = flags.packed;

      if (flags.hasQBezier) newFlags |= PATH_FLAG_HAS_QBEZIER;
      if (flags.hasCBezier) newFlags |= PATH_FLAG_HAS_CBEZIER;

      if (i >= length)
        break;
    }
    else
    {
      goto _Invalid;
    }
  }

  // --------------------------------------------------------------------------
  // [Create Path-Info]
  // --------------------------------------------------------------------------

  {
    if (numberOfFigures == 0)
      return NULL;

    size_t memSize = sizeof(NumT_(PathInfo)) +
                     numberOfFigures * sizeof(NumT_(PathInfoFigure)) +
                     length * sizeof(NumT);

    info = reinterpret_cast<NumT_(PathInfo)*>(MemMgr::alloc(memSize));
    if (FOG_IS_NULL(info))
      return NULL;

    info->_reference.init(1);
    info->_numberOfFigures = numberOfFigures;
    info->_distanceCapacity = length;
    info->_pathLength = NumT(0);

    NumT_(PathInfoFigure)* figureData = reinterpret_cast<NumT_(PathInfoFigure)*>(
      (uint8_t*)info + sizeof(NumT_(PathInfo)));
    NumT* distanceData = reinterpret_cast<NumT*>(
      (uint8_t*)figureData + numberOfFigures * sizeof(NumT_(PathInfoFigure)));

    info->_figureData = figureData;
    info->_distanceData = distanceData;

    // Whole path properties.
    double pathLength = NumT(0);
    NumT_(Box) pathBoundingBox;
    i = 0;

    for (currentFigure = firstFigure; currentFigure != NULL; currentFigure = currentFigure->next)
    {
      size_t start = currentFigure->start;
      size_t end = currentFigure->end;

      double figureLength = NumT(0);
      NumT commandDistance;

      bool hasAcuteEdges = false;

      NumT_(Box) boundingBox(UNINITIALIZED);
      NumT_(Box) vertexBox(UNINITIALIZED);

      // Fill the gap (just to doesn't have garbage in distanceData).
      while (i < start)
      {
        distanceData[i] = NumT(0);
        i++;
      }

      boundingBox.setBox(srcPts[i].x, srcPts[i].y,
                         srcPts[i].x, srcPts[i].y);
      vertexBox = boundingBox;

      // Skip the first move-to command and set distance to zero (move-to has
      // no distance).
      FOG_ASSERT(PathCmd::isMoveTo(srcCmd[i]));
      distanceData[i++] = NumT(0);

      // Calculate distances.
      while (i < end)
      {
        switch (srcCmd[i])
        {
          case PATH_CMD_LINE_TO:
            _MERGE_BBOX(NumT, boundingBox, srcPts[i]);

            commandDistance = Math::euclideanDistance(srcPts[i - 1].x, srcPts[i - 1].y, srcPts[i].x, srcPts[i].y);
            distanceData[i + 0] = NumT(commandDistance);

            i += 1;
            if (!hasAcuteEdges && i < end && PathCmd::isVertex(srcCmd[i]))
              hasAcuteEdges |= PathInfoT_isAcute<NumT>(srcPts[i - 2], srcPts[i - 1], srcPts[i]);
            break;

          case PATH_CMD_QUAD_TO:
            _MERGE_BBOX(NumT, vertexBox, srcPts[i]);
            _MERGE_BBOX(NumT, boundingBox, srcPts[i + 1]);

            if (boundingBox.x0 > srcPts[i + 0].x || boundingBox.y0 > srcPts[i + 0].y ||
                boundingBox.x1 < srcPts[i + 0].x || boundingBox.y1 < srcPts[i + 0].y)
            {
              NumT_(Box) bezierBox(UNINITIALIZED);
              if (FOG_IS_ERROR(reinterpret_cast<const NumT_(QBezier)*>(srcPts - 1)->getBoundingBox(bezierBox)))
                goto _Invalid;
              NumI_(Box)::bound(boundingBox, boundingBox, bezierBox);
            }

            commandDistance = reinterpret_cast<const NumT_(QBezier)*>(srcPts - 1)->getLength();
            distanceData[i + 0] = NumT(commandDistance);
            distanceData[i + 1] = NumT(0);

            if (!hasAcuteEdges)
              hasAcuteEdges |= PathInfoT_isAcute<NumT>(srcPts[i - 1], srcPts[i], srcPts[i + 1]);

            i += 2;
            break;

          case PATH_CMD_CUBIC_TO:
            _MERGE_BBOX(NumT, vertexBox, srcPts[i + 0]);
            _MERGE_BBOX(NumT, vertexBox, srcPts[i + 1]);
            _MERGE_BBOX(NumT, boundingBox, srcPts[i + 2]);

            if (boundingBox.x0 > srcPts[i + 0].x || boundingBox.y0 > srcPts[i + 0].y ||
                boundingBox.x1 < srcPts[i + 0].x || boundingBox.y1 < srcPts[i + 0].y ||
                boundingBox.x0 > srcPts[i + 1].x || boundingBox.y0 > srcPts[i + 1].y ||
                boundingBox.x1 < srcPts[i + 1].x || boundingBox.y1 < srcPts[i + 1].y)
            {
              NumT_(Box) bezierBox(UNINITIALIZED);
              if (FOG_IS_ERROR(reinterpret_cast<const NumT_(CBezier)*>(srcPts - 1)->getBoundingBox(bezierBox)))
                goto _Invalid;
              NumI_(Box)::bound(boundingBox, boundingBox, bezierBox);
            }

            commandDistance = reinterpret_cast<const NumT_(CBezier)*>(srcPts - 1)->getLength();
            distanceData[i + 0] = NumT(commandDistance);
            distanceData[i + 1] = NumT(0);
            distanceData[i + 2] = NumT(0);

            // Inflection points.
            reinterpret_cast<const NumT_(CBezier)*>(srcPts - 1)->getInflectionPoints(&distanceData[i + 1]);

            if (!hasAcuteEdges)
              hasAcuteEdges |= PathInfoT_isAcute<NumT>(srcPts[i - 1], srcPts[i], srcPts[i + 1]);

            i += 3;
            break;

          case PATH_CMD_CLOSE:
            commandDistance = Math::euclideanDistance(srcPts[i - 1].x, srcPts[i - 1].y, srcPts[start].x, srcPts[start].y);
            distanceData[i + 0] = NumT(commandDistance);

            if (!hasAcuteEdges || end - start > 2)
              hasAcuteEdges |= PathInfoT_isAcute<NumT>(srcPts[i - 2], srcPts[i - 1], srcPts[start]);

            i += 1;
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }

        figureLength += commandDistance;
      }

      // Store figure info.
      figureData->boundingBox = boundingBox;
      NumI_(Box)::bound(figureData->vertexBox, boundingBox, vertexBox);

      figureData->start = (uint)start;
      figureData->end = (uint)end;
      figureData->figureLength = NumT(figureLength);
      figureData->flags.packed = currentFigure->flags.packed;
      figureData->flags.hasAcuteEdges = hasAcuteEdges;
      figureData->flags.reserved = 0;
      figureData++;

      // Store whole path properties.
      pathLength += figureLength;
      if (currentFigure == firstFigure)
        pathBoundingBox = boundingBox;
      else
        NumI_(Box)::bound(pathBoundingBox, pathBoundingBox, boundingBox);
    }

    info->_pathLength = NumT(pathLength);
    path->_d->boundingBox = pathBoundingBox;
    path->_d->vType = newFlags | PATH_FLAG_HAS_BBOX;

    return info;
  }

  // --------------------------------------------------------------------------
  // [Invalid-Path]
  // --------------------------------------------------------------------------

_Invalid:
  if (info != NULL)
    MemMgr::free(info);

  return NULL;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void PathInfo_init(void)
{
  fog_api.pathinfof_generate = PathInfoT_generate<float>;
  fog_api.pathinfod_generate = PathInfoT_generate<double>;
}

} // Fog namespace
