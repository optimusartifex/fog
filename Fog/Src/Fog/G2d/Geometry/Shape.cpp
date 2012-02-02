// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Private.h>
#include <Fog/G2d/Geometry/Shape.h>
#include <Fog/G2d/Geometry/PathTmp_p.h>
#include <Fog/G2d/Geometry/Transform.h>

namespace Fog {

// ============================================================================
// [Fog::Shape - GetBoundingBox]
// ============================================================================

template<typename NumT>
static err_t ShapeT_getBoundingBoxOfPoints(NumT_(Box)* dst,
  const NumT_(Point)* pts, size_t count, uint32_t transformType, const NumT_(Transform)* transform)
{
  if (count == 0)
    return ERR_GEOMETRY_NONE;

  size_t i = count;

  if (transformType <= TRANSFORM_TYPE_SWAP)
  {
    const NumT_(Point)* p = pts;

    NumT_(Point) pMin(p[0]);
    NumT_(Point) pMax(p[0]);

    while (--i)
    {
      p++;
      if (pMin.x > p->x)
        pMin.x = p->x;
      else if (pMax.x < p->x)
        pMax.x = p->x;

      if (pMin.y > p->y)
        pMin.y = p->y;
      else if (pMax.y < p->y)
        pMax.y = p->y;
    }

    dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
    
    if (transform != NULL)
      transform->mapBox(*dst, *dst);

    return ERR_OK;
  }
  else
  {
    NumT_(Point) tmp[128];

    NumT_(Point) pMin;
    NumT_(Point) pMax;
    bool isFirst = true;

    while (i)
    {
      NumT_(Point)* p = tmp;
      size_t j = Math::min<size_t>(i, FOG_ARRAY_SIZE(p));

      transform->mapPoints(p, pts, j);
      i -= j;
      pts += j;

      if (isFirst)
      {
        isFirst = false;
        pMin = p[0];
        pMax = p[0];

        p++;
        j--;
      }

      while (j)
      {
        if (pMin.x > p->x)
          pMin.x = p->x;
        else if (pMax.x < p->x)
          pMax.x = p->x;

        if (pMin.y > p->y)
          pMin.y = p->y;
        else if (pMax.y < p->y)
          pMax.y = p->y;

        p++;
        j--;
      }
    }

    dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
    return ERR_OK;
  }
}

template<typename NumT>
static void ShapeT_pointsFromRects(NumT_(Point)* dst, const NumT_(Rect)* src, size_t count)
{
  for (size_t i = 0; i < count; i++)
  {
    NumT x0 = src->x;
    NumT y0 = src->y;
    NumT x1 = x0 + src->w;
    NumT y1 = y0 + src->h;
  
    dst[0].set(x0, y0);
    dst[1].set(x0, y1);
    dst[2].set(x1, y0);
    dst[3].set(x1, y1);

    dst += 4;
    src += 1;
  }
}
  
template<typename NumT>
static err_t ShapeT_getBoundingBoxOfRects(NumT_(Box)* dst,
  const NumT_(Rect)* rects, size_t count, uint32_t transformType, const NumT_(Transform)* transform)
{
  if (count == 0)
    return ERR_GEOMETRY_NONE;

  size_t i = count;

  if (transformType <= TRANSFORM_TYPE_SWAP)
  {
    const NumT_(Rect)* p = rects;

    NumT_(Point) pMin(p->x, p->y);
    NumT_(Point) pMax(p->x + p->w, p->y + p->h);

    while (--i)
    {
      p++;
      
      NumT x0 = p->x;
      NumT y0 = p->y;
      NumT x1 = x0 + p->w;
      NumT y1 = y0 + p->h;

      if (pMin.x > x0)
        pMin.x = x0;
      else if (pMax.x < x1)
        pMax.x = x1;

      if (pMin.y > y0)
        pMin.y = y0;
      else if (pMax.y < y1)
        pMax.y = y1;
    }

    dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
    
    if (transform != NULL)
      transform->mapBox(*dst, *dst);

    return ERR_OK;
  }
  else
  {
    NumT_(Point) tmp[128];

    NumT_(Point) pMin;
    NumT_(Point) pMax;
    bool isFirst = true;

    while (i)
    {
      NumT_(Point)* p = tmp;
      size_t j = Math::min<size_t>(i, FOG_ARRAY_SIZE(p) / 4);

      ShapeT_pointsFromRects<NumT>(p, rects, j);
      i -= j;
      rects += j;

      j *= 4;
      transform->mapPoints(p, p, j);

      if (isFirst)
      {
        isFirst = false;
        pMin = p[0];
        pMax = p[0];

        p++;
        j--;
      }

      do {
        if (pMin.x > p->x)
          pMin.x = p->x;
        else if (pMax.x < p->x)
          pMax.x = p->x;

        if (pMin.y > p->y)
          pMin.y = p->y;
        else if (pMax.y < p->y)
          pMax.y = p->y;

        p++;
      } while (--j);
    }

    dst->setBox(pMin.x, pMin.y, pMax.x, pMax.y);
    return ERR_OK;
  }
}

template<typename NumT>
static err_t FOG_CDECL ShapeT_getBoundingBox(uint32_t shapeType, const void* shapeData,
  NumT_(Box)* dst, const NumT_(Transform)* transform)
{
  NumT_(Point) tmp[8];
  uint32_t transformType = TRANSFORM_TYPE_IDENTITY;

  if (transform != NULL)
  {
    transformType = transform->getType();
    if (transformType == TRANSFORM_TYPE_DEGENERATE)
    {
      dst->reset();
      return ERR_GEOMETRY_DEGENERATE;
    }
  }

  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
      dst->reset();
      return ERR_GEOMETRY_NONE;

    case SHAPE_TYPE_LINE:
      *dst = static_cast<const NumT_(Line)*>(shapeData)->getBoundingBox();
      if (transform) transform->mapBox(*dst, *dst);
      return ERR_OK;

    case SHAPE_TYPE_QBEZIER:
      if (transformType != TRANSFORM_TYPE_IDENTITY)
      {
        if (transformType >= TRANSFORM_TYPE_PROJECTION)
        {
          static const uint8_t cmd[4] =
          {
            PATH_CMD_MOVE_TO,
            PATH_CMD_QUAD_TO,
            PATH_CMD_DATA,
            0xFF
          };

          NumT_T1(PathTmp, 16) path;
          transform->mapPathData(path, cmd, static_cast<const NumT_(Point)*>(shapeData), 3);
          return path.getBoundingBox(*dst);
        }

        transform->_mapPoints(tmp, static_cast<const NumT_(Point)*>(shapeData), 3);
        shapeData = &tmp;
      }

      FOG_RETURN_ON_ERROR(static_cast<const NumT_(QBezier)*>(shapeData)->getBoundingBox(*dst));
      return ERR_OK;

    case SHAPE_TYPE_CBEZIER:
      if (transformType != TRANSFORM_TYPE_IDENTITY)
      {
        if (transformType >= TRANSFORM_TYPE_PROJECTION)
        {
          static const uint8_t cmd[4] =
          {
            PATH_CMD_MOVE_TO,
            PATH_CMD_CUBIC_TO,
            PATH_CMD_DATA,
            PATH_CMD_DATA
          };

          NumT_T1(PathTmp, 16) path;
          transform->mapPathData(path, cmd, static_cast<const NumT_(Point)*>(shapeData), 4);
          return path.getBoundingBox(*dst);
        }

        transform->_mapPoints(tmp, static_cast<const NumT_(Point)*>(shapeData), 4);
        shapeData = &tmp;
      }

      FOG_RETURN_ON_ERROR(static_cast<const NumT_(CBezier)*>(shapeData)->getBoundingBox(*dst));
      return ERR_OK;

    case SHAPE_TYPE_ARC:
      return static_cast<const NumT_(Arc)*>(shapeData)->_getBoundingBox(*dst, transform);

    case SHAPE_TYPE_RECT:
      *dst = *static_cast<const NumT_(Rect)*>(shapeData);
      if (transform) transform->mapBox(*dst, *dst);
      return ERR_OK;

    case SHAPE_TYPE_ROUND:
      return static_cast<const NumT_(Round)*>(shapeData)->_getBoundingBox(*dst, transform);

    case SHAPE_TYPE_CIRCLE:
      static_cast<const NumT_(Circle)*>(shapeData)->_getBoundingBox(*dst, transform);

    case SHAPE_TYPE_ELLIPSE:
      return static_cast<const NumT_(Ellipse)*>(shapeData)->_getBoundingBox(*dst, transform);

    case SHAPE_TYPE_CHORD:
      return static_cast<const NumT_(Chord)*>(shapeData)->_getBoundingBox(*dst, transform);

    case SHAPE_TYPE_PIE:
      return static_cast<const NumT_(Pie)*>(shapeData)->_getBoundingBox(*dst, transform);

    case SHAPE_TYPE_TRIANGLE:
      return static_cast<const NumT_(Triangle)*>(shapeData)->_getBoundingBox(*dst, transform);

    case SHAPE_TYPE_POLYLINE:
    case SHAPE_TYPE_POLYGON:
    {
      const NumT_(PointArray)* pa = static_cast<const NumT_(PointArray)*>(shapeData);
      return ShapeT_getBoundingBoxOfPoints<NumT>(dst, pa->getData(), pa->getLength(), transformType, transform);
    }

    case SHAPE_TYPE_RECT_ARRAY:
    {
      const NumT_(RectArray)* pa = static_cast<const NumT_(RectArray)*>(shapeData);
      return ShapeT_getBoundingBoxOfRects<NumT>(dst, pa->getData(), pa->getLength(), transformType, transform);
    }

    case SHAPE_TYPE_PATH:
      return static_cast<const NumT_(Path)*>(shapeData)->_getBoundingBox(*dst, transform);

    default:
      dst->reset();
      return ERR_RT_INVALID_ARGUMENT;
  }
}

// ============================================================================
// [Fog::Shape - HitTest]
// ============================================================================

template<typename NumT>
static FOG_INLINE bool ShapeT_hitTestPolygon(const NumT_(Point)* pts, size_t length, const NumT_(Point)* hitPt, uint32_t fillRule)
{
  const NumT_(Point)* ptsEnd = pts + length;

  if (length < 3)
    return false;

  NumT x0, y0;
  NumT x1, y1;

  NumT px, py;
  NumT dx, dy;

  x0 = pts[0].x;
  y0 = pts[0].y;
  pts++;
  
  int windingNumber = 0;
  px = hitPt->x;
  py = hitPt->y;

  do {
    x1 = pts[0].x;
    y1 = pts[0].y;

    dx = x1 - x0;
    dy = y1 - y0;

    if (dy > NumT(0.0))
    {
      if (py >= y0 && py < y1)
      {
        NumT ix = x0 + (py - y0) * dx / dy;
        windingNumber += (px >= ix);
      }
    }
    else if (dy < NumT(0.0))
    {
      if (py >= y1 && py < y0)
      {
        NumT ix = x0 + (py - y0) * dx / dy;
        windingNumber -= (px >= ix);
      }
    }

    x0 = x1;
    y0 = y1;
  } while (++pts != ptsEnd);

  if (fillRule == FILL_RULE_EVEN_ODD)
    windingNumber &= 1;
  return windingNumber != 0;
}

template<typename NumT>
static bool FOG_INLINE ShapeT_hitTestRects(const NumT_(Rect)* rects, size_t length, const NumT_(Point)* hitPt, uint32_t fillRule)
{
  if (fillRule == FILL_RULE_NON_ZERO)
  {
    for (size_t i = 0; i < length; i++)
    {
      if (rects[i].hitTest(*hitPt))
        return true;
    }
    return false;
  }
  else
  {
    int windingNumber = 0;
    for (size_t i = 0; i < length; i++)
    {
      windingNumber += rects[i].hitTest(*hitPt);
    }
    return windingNumber != 0;
  }
}

template<typename NumT>
static bool FOG_CDECL ShapeT_hitTest(uint32_t shapeType, const void* shapeData, const NumT_(Point)* hitPt, uint32_t fillRule)
{
  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    case SHAPE_TYPE_LINE:
    case SHAPE_TYPE_QBEZIER:
    case SHAPE_TYPE_CBEZIER:
    case SHAPE_TYPE_ARC:
      return false;

    case SHAPE_TYPE_RECT:
      return static_cast<const NumT_(Rect)*>(shapeData)->hitTest(*hitPt);

    case SHAPE_TYPE_ROUND:
      return static_cast<const NumT_(Round)*>(shapeData)->hitTest(*hitPt);

    case SHAPE_TYPE_CIRCLE:
      return static_cast<const NumT_(Circle)*>(shapeData)->hitTest(*hitPt);

    case SHAPE_TYPE_ELLIPSE:
      return static_cast<const NumT_(Ellipse)*>(shapeData)->hitTest(*hitPt);

    case SHAPE_TYPE_CHORD:
      return static_cast<const NumT_(Chord)*>(shapeData)->hitTest(*hitPt);

    case SHAPE_TYPE_PIE:
      return static_cast<const NumT_(Pie)*>(shapeData)->hitTest(*hitPt);

    case SHAPE_TYPE_TRIANGLE:
      return static_cast<const NumT_(Triangle)*>(shapeData)->hitTest(*hitPt);

    case SHAPE_TYPE_POLYLINE:
      return false;

    case SHAPE_TYPE_POLYGON:
    {
      const NumT_(PointArray)* pts = static_cast<const NumT_(PointArray)*>(shapeData);
      return ShapeT_hitTestPolygon<NumT>(pts->getData(), pts->getLength(), hitPt, fillRule);
    }

    case SHAPE_TYPE_RECT_ARRAY:
    {
      const NumT_(RectArray)* rects = static_cast<const NumT_(RectArray)*>(shapeData);
      return ShapeT_hitTestRects<NumT>(rects->getData(), rects->getLength(), hitPt, fillRule);
    }

    case SHAPE_TYPE_PATH:
      return static_cast<const NumT_(Path)*>(shapeData)->hitTest(*hitPt, fillRule);

    default:
      return false;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Shape_init(void)
{
  fog_api.shapef_getBoundingBox = ShapeT_getBoundingBox<float>;
  fog_api.shapef_hitTest = ShapeT_hitTest<float>;

  fog_api.shaped_getBoundingBox = ShapeT_getBoundingBox<double>;
  fog_api.shaped_hitTest = ShapeT_hitTest<double>;
}

} // Fog namespace
