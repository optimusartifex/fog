// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/G2d/Geometry/Shape.h>
#include <Fog/G2d/Global/Api.h>
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

template<typename Number>
static void FOG_CDECL _G2d_ShapeT_getBoundingRect(uint32_t shapeType, const void* shapeData, typename RectT<Number>::T* dst)
{
  switch (shapeType)
  {
    case SHAPE_TYPE_NONE:
    default:
      dst->reset();
      break;

    case SHAPE_TYPE_LINE:
      *dst = reinterpret_cast<const typename LineT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_QUAD:
      *dst = reinterpret_cast<const typename QuadCurveT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_CUBIC:
      *dst = reinterpret_cast<const typename CubicCurveT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_ARC:
      *dst = reinterpret_cast<const typename ArcT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_RECT:
      *dst = *reinterpret_cast<const typename RectT<Number>::T*>(shapeData);
      break;

    case SHAPE_TYPE_ROUND:
      *dst = reinterpret_cast<const typename RoundT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_CIRCLE:
      *dst = reinterpret_cast<const typename CircleT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_ELLIPSE:
      *dst = reinterpret_cast<const typename EllipseT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_CHORD:
      *dst = reinterpret_cast<const typename ChordT<Number>::T*>(shapeData)->getBoundingRect();
      break;

    case SHAPE_TYPE_PIE:
      *dst = reinterpret_cast<const typename PieT<Number>::T*>(shapeData)->getBoundingRect();
      break;
  }
}

// ============================================================================
// [Fog::G2d - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _g2d_shape_init(void)
{
  _g2d.shapef.getBoundingRect = _G2d_ShapeT_getBoundingRect<float>;
  _g2d.shaped.getBoundingRect = _G2d_ShapeT_getBoundingRect<double>;
}

FOG_NO_EXPORT void _g2d_shape_fini(void)
{
}

} // Fog namespace
