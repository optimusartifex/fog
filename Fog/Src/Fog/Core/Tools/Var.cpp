// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/Date.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Locale.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/RegExp.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/Stub.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/Core/Tools/VarId.h>
#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Chord.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Pie.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Polygon.h>
#include <Fog/G2d/Geometry/QBezier.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Geometry/Triangle.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Source/Argb.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Source/Texture.h>
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Tools/Matrix.h>
#include <Fog/G2d/Tools/Region.h>

namespace Fog {

// ============================================================================
// [Fog::VarNullData]
// ============================================================================

//! @brief Null variable data, which adds some zeros after the VarData to
//! support NULLs in other classes, for example @ref Pattern.
struct FOG_NO_EXPORT VarNullData : public VarData
{
  uint32_t zero[8];
};

// ============================================================================
// [Fog::Var - Global]
// ============================================================================

static Static<VarNullData> Var_dNull;
static Static<Var> Var_oNull;

// ============================================================================
// [Fog::Var - Helpers]
// ============================================================================

#define VAR_SIMPLE_C(_D_, _Type_) \
  ((VarSimpleData*)(_D_))->getInstanceT< _Type_ >()

#define VAR_OBJECT_C(_D_, _Type_) \
  ((_Type_*)_D_)

static FOG_INLINE size_t Var_dImplicitGetRef(VarData* d)
{
  return AtomicCore<size_t>::get(&d->unknown.reference);
}

static FOG_INLINE VarData* Var_dImplicitAddRef(VarData* d)
{
  AtomicCore<size_t>::inc(&d->unknown.reference);
  return d;
}

// ============================================================================
// [Fog::Var - DataSize]
// ============================================================================

// ${VAR_TYPE:BEGIN}
static const uint8_t Var_dataSize[] =
{
  /* 0000: VAR_TYPE_NULL                   */ 0,

  /* 0001: VAR_TYPE_BOOL                   */ sizeof(bool),
  /* 0002: VAR_TYPE_CHAR                   */ sizeof(CharW),
  /* 0003: VAR_TYPE_INT32                  */ sizeof(int32_t),
  /* 0004: VAR_TYPE_UINT32                 */ sizeof(uint32_t),
  /* 0005: VAR_TYPE_INT64                  */ sizeof(int64_t),
  /* 0006: VAR_TYPE_UINT64                 */ sizeof(uint64_t),
  /* 0007: VAR_TYPE_FLOAT                  */ sizeof(float),
  /* 0008: VAR_TYPE_DOUBLE                 */ sizeof(double),

  /* 0009: VAR_TYPE_STRING_A               */ 0, // Implicit.
  /* 0010: VAR_TYPE_STRING_W               */ 0, // Implicit.

  /* 0011: VAR_TYPE_LIST_STRING_A          */ 0, // Implicit.
  /* 0012: VAR_TYPE_LIST_STRING_W          */ 0, // Implicit.
  /* 0013: VAR_TYPE_LIST_VAR               */ 0, // Implicit.

  /* 0014: VAR_TYPE_HASH_STRING_A_STRING_A */ 0, // Implicit.
  /* 0015: VAR_TYPE_HASH_STRING_A_VAR      */ 0, // Implicit.
  /* 0016: VAR_TYPE_HASH_STRING_W_STRING_W */ 0, // Implicit.
  /* 0017: VAR_TYPE_HASH_STRING_W_VAR      */ 0, // Implicit.

  /* 0018: VAR_TYPE_REGEXP_A               */ 0, // Implicit.
  /* 0019: VAR_TYPE_REGEXP_W               */ 0, // Implicit.

  /* 0020: VAR_TYPE_LOCALE                 */ 0, // Implicit.

  /* 0021: VAR_TYPE_DATE                   */ sizeof(Date),
  /* 0022: VAR_TYPE_TIME                   */ sizeof(Time),

  // --------------------------------------------------------------------------

  /* 0023:                                 */ 0, // Reserved.
  /* 0024:                                 */ 0, // Reserved.
  /* 0025:                                 */ 0, // Reserved.
  /* 0026:                                 */ 0, // Reserved.
  /* 0027:                                 */ 0, // Reserved.
  /* 0028:                                 */ 0, // Reserved.
  /* 0029:                                 */ 0, // Reserved.
  /* 0030:                                 */ 0, // Reserved.
  /* 0031:                                 */ 0, // Reserved.

  // --------------------------------------------------------------------------

  /* 0032: VAR_TYPE_POINT_I                */ sizeof(PointI),
  /* 0033: VAR_TYPE_POINT_F                */ sizeof(PointF),
  /* 0034: VAR_TYPE_POINT_D                */ sizeof(PointD),
  /* 0035: VAR_TYPE_SIZE_I                 */ sizeof(SizeI),
  /* 0036: VAR_TYPE_SIZE_F                 */ sizeof(SizeF),
  /* 0037: VAR_TYPE_SIZE_D                 */ sizeof(SizeD),
  /* 0038: VAR_TYPE_BOX_I                  */ sizeof(BoxI),
  /* 0039: VAR_TYPE_BOX_F                  */ sizeof(BoxF),
  /* 0040: VAR_TYPE_BOX_D                  */ sizeof(BoxD),
  /* 0041: VAR_TYPE_RECT_I                 */ sizeof(RectI),
  /* 0042: VAR_TYPE_RECT_F                 */ sizeof(RectF),
  /* 0043: VAR_TYPE_RECT_D                 */ sizeof(RectD),
  /* 0044:                                 */ 0, // Reserved.
  /* 0045: VAR_TYPE_LINE_F                 */ sizeof(LineF),
  /* 0046: VAR_TYPE_LINE_D                 */ sizeof(LineD),
  /* 0047: VAR_TYPE_QBEZIER_F              */ sizeof(QBezierF),
  /* 0048: VAR_TYPE_QBEZIER_D              */ sizeof(QBezierD),
  /* 0049: VAR_TYPE_CBEZIER_F              */ sizeof(CBezierF),
  /* 0050: VAR_TYPE_CBEZIER_D              */ sizeof(CBezierD),
  /* 0051: VAR_TYPE_TRIANGLE_F             */ sizeof(TriangleF),
  /* 0052: VAR_TYPE_TRIANGLE_D             */ sizeof(TriangleD),
  /* 0053: VAR_TYPE_ROUND_F                */ sizeof(RoundF),
  /* 0054: VAR_TYPE_ROUND_D                */ sizeof(RoundD),
  /* 0055: VAR_TYPE_CIRCLE_F               */ sizeof(CircleF),
  /* 0056: VAR_TYPE_CIRCLE_D               */ sizeof(CircleD),
  /* 0057: VAR_TYPE_ELLIPSE_F              */ sizeof(EllipseF),
  /* 0058: VAR_TYPE_ELLIPSE_D              */ sizeof(EllipseD),
  /* 0059: VAR_TYPE_ARC_F                  */ sizeof(ArcF),
  /* 0060: VAR_TYPE_ARC_D                  */ sizeof(ArcD),
  /* 0061: VAR_TYPE_CHORD_F                */ sizeof(ChordF),
  /* 0062: VAR_TYPE_CHORD_D                */ sizeof(ChordD),
  /* 0063: VAR_TYPE_PIE_F                  */ sizeof(PieF),
  /* 0064: VAR_TYPE_PIE_D                  */ sizeof(PieD),
  /* 0065: VAR_TYPE_POLYGON_F              */ 0, // Reserved.
  /* 0066: VAR_TYPE_POLYGON_D              */ 0, // Reserved.
  /* 0067: VAR_TYPE_PATH_F                 */ 0, // Implicit.
  /* 0068: VAR_TYPE_PATH_D                 */ 0, // Implicit.
  /* 0069: VAR_TYPE_REGION                 */ 0, // Implicit.
  /* 0070: VAR_TYPE_TRANSFORM_F            */ sizeof(TransformF),
  /* 0071: VAR_TYPE_TRANSFORM_D            */ sizeof(TransformD),
  /* 0072: VAR_TYPE_MATRIX_F               */ 0, // Implicit.
  /* 0073: VAR_TYPE_MATRIX_D               */ 0, // Implicit.
  /* 0074: VAR_TYPE_COLOR                  */ 0, // Special.
  /* 0075: VAR_TYPE_PATTERN                */ 0, // Implicit.
  /* 0076: VAR_TYPE_COLOR_STOP             */ sizeof(ColorStop),
  /* 0077: VAR_TYPE_COLOR_STOP_LIST        */ 0, // Implicit.
  /* 0078: VAR_TYPE_IMAGE                  */ 0, // Implicit.
  /* 0079: VAR_TYPE_IMAGE_PALETTE          */ 0, // Implicit.
  /* 0080: VAR_TYPE_IMAGE_FILTER           */ 0, // Implicit.
  /* 0081: VAR_TYPE_FACE_INFO              */ 0, // Implicit.
  /* 0082: VAR_TYPE_FACE_COLLECTION        */ 0, // Implicit.
  /* 0083: VAR_TYPE_FONT                   */ 0  // Implicit.
};
// ${VAR_TYPE:END}

static FOG_INLINE size_t Var_getDataSize(uint32_t vType)
{
  FOG_ASSERT(vType < FOG_ARRAY_SIZE(Var_dataSize));
  size_t size = Var_dataSize[vType];

  FOG_ASSERT(size != 0);
  return size;
}

// ============================================================================
// [Fog::Var - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Var_ctor(Var* self)
{
  self->_d = &Var_dNull;
}

static void FOG_CDECL Var_ctorCopy(Var* self, const Var* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Var_ctorType(Var* self, uint32_t vType, const void* vData)
{
  switch (vType)
  {
    case VAR_TYPE_NULL:
_CreateNull:
      self->_d = &Var_dNull;
      return;

    case VAR_TYPE_BOOL:
    case VAR_TYPE_CHAR:
    case VAR_TYPE_INT32:
    case VAR_TYPE_UINT32:
    case VAR_TYPE_INT64:
    case VAR_TYPE_UINT64:
    case VAR_TYPE_FLOAT:
    case VAR_TYPE_DOUBLE:
      goto _CreateSimple;

    case VAR_TYPE_STRING_A:
    case VAR_TYPE_STRING_W:
    case VAR_TYPE_LIST_STRING_A:
    case VAR_TYPE_LIST_STRING_W:
    case VAR_TYPE_LIST_VAR:
    case VAR_TYPE_HASH_STRING_A_STRING_A:
    case VAR_TYPE_HASH_STRING_A_VAR:
    case VAR_TYPE_HASH_STRING_W_STRING_W:
    case VAR_TYPE_HASH_STRING_W_VAR:
    case VAR_TYPE_REGEXP_A:
    case VAR_TYPE_REGEXP_W:
    case VAR_TYPE_LOCALE:
_CreateImplicit:
      self->_d = static_cast<const Var*>(vData)->_d->addRef();
      return;

    case VAR_TYPE_DATE:
    case VAR_TYPE_TIME:
      goto _CreateSimple;

    case VAR_TYPE_POINT_I:
    case VAR_TYPE_POINT_F:
    case VAR_TYPE_POINT_D:
    case VAR_TYPE_SIZE_I:
    case VAR_TYPE_SIZE_F:
    case VAR_TYPE_SIZE_D:
    case VAR_TYPE_BOX_I:
    case VAR_TYPE_BOX_F:
    case VAR_TYPE_BOX_D:
    case VAR_TYPE_RECT_I:
    case VAR_TYPE_RECT_F:
    case VAR_TYPE_RECT_D:
    case VAR_TYPE_LINE_F:
    case VAR_TYPE_LINE_D:
    case VAR_TYPE_QBEZIER_F:
    case VAR_TYPE_QBEZIER_D:
    case VAR_TYPE_CBEZIER_F:
    case VAR_TYPE_CBEZIER_D:
    case VAR_TYPE_TRIANGLE_F:
    case VAR_TYPE_TRIANGLE_D:
    case VAR_TYPE_ROUND_F:
    case VAR_TYPE_ROUND_D:
    case VAR_TYPE_CIRCLE_F:
    case VAR_TYPE_CIRCLE_D:
    case VAR_TYPE_ELLIPSE_F:
    case VAR_TYPE_ELLIPSE_D:
    case VAR_TYPE_ARC_F:
    case VAR_TYPE_ARC_D:
    case VAR_TYPE_CHORD_F:
    case VAR_TYPE_CHORD_D:
    case VAR_TYPE_PIE_F:
    case VAR_TYPE_PIE_D:
      goto _CreateSimple;

    case VAR_TYPE_POLYGON_F:
    case VAR_TYPE_POLYGON_D:
    case VAR_TYPE_PATH_F:
    case VAR_TYPE_PATH_D:
    case VAR_TYPE_REGION:
      goto _CreateImplicit;

    case VAR_TYPE_TRANSFORM_F:
    case VAR_TYPE_TRANSFORM_D:
      goto _CreateSimple;

    case VAR_TYPE_MATRIX_F:
    case VAR_TYPE_MATRIX_D:
      goto _CreateImplicit;

    case VAR_TYPE_COLOR:
      fog_api.pattern_ctorColor(reinterpret_cast<Pattern*>(self), reinterpret_cast<const Color*>(vData));
      return;

    case VAR_TYPE_PATTERN:
      fog_api.pattern_ctorCopy(reinterpret_cast<Pattern*>(self), reinterpret_cast<const Pattern*>(vData));
      return;

    case VAR_TYPE_COLOR_STOP:
      goto _CreateSimple;
    case VAR_TYPE_COLOR_STOP_LIST:
      goto _CreateImplicit;

    case VAR_TYPE_IMAGE:
    case VAR_TYPE_IMAGE_PALETTE:
    case VAR_TYPE_IMAGE_FILTER:
      goto _CreateImplicit;

    case VAR_TYPE_FACE_INFO:
    case VAR_TYPE_FACE_COLLECTION:
    case VAR_TYPE_FONT:
      goto _CreateImplicit;

    default:
      goto _CreateNull;
  }

_CreateSimple:
  {
    size_t vSize = Var_getDataSize(vType);

    VarData* d = fog_api.var_dCreate(vSize);
    if (FOG_IS_NULL(d))
      goto _CreateNull;

    d->vType = vType;
    MemOps::copy_small(d->getData(), vData, vSize);

    self->_d = d;
  }
}

static void FOG_CDECL Var_dtor(Var* self)
{
  VarData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::Var - GetReference]
// ============================================================================

static size_t FOG_CDECL Var_getReference(const Var* self)
{
  VarData* d = self->_d;
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  if (vType == VAR_TYPE_NULL)
    return 1;

  if (vType != VAR_TYPE_OBJECT_REF)
  {
    return AtomicCore<size_t>::get(&d->unknown.reference);
  }
  else
  {
    // TODO: Var
    // return reinterpret_cast<Object*>(d);
    return 0;
  }
}

// ============================================================================
// [Fog::Var - Type]
// ============================================================================

static uint32_t FOG_CDECL Var_getVarType(const Var* self)
{
  return self->_d->vType & VAR_TYPE_MASK;
}

// ============================================================================
// [Fog::Var - Reset]
// ============================================================================

static void FOG_CDECL Var_reset(Var* self)
{
  fog_api.var_dRelease(atomicPtrXchg(&self->_d, reinterpret_cast<VarData*>(&Var_dNull)));
}

// ============================================================================
// [Fog::Var - Copy]
// ============================================================================

static err_t FOG_CDECL Var_copy(Var* self, const Var* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}

// ============================================================================
// [Fog::Var - Get]
// ============================================================================

static err_t FOG_CDECL Var_getI32(const Var* self, int32_t* dst)
{
  VarData* d = self->_d;
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  switch (vType)
  {
    case VAR_TYPE_BOOL:
    {
      *dst = (int32_t)*reinterpret_cast<const bool*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_CHAR:
    {
      *dst = (int32_t)reinterpret_cast<const CharW*>(d->getData())->getInt();
      return ERR_OK;
    }

    case VAR_TYPE_INT32:
    {
      *dst = *reinterpret_cast<const int32_t*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_UINT32:
    {
      uint32_t val = *reinterpret_cast<const uint32_t*>(d->getData());

      if (val > INT32_MAX)
      {
        *dst = INT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_INT64:
    {
      int64_t val = *reinterpret_cast<const int64_t*>(d->getData());

      if (val < INT32_MIN)
      {
        *dst = INT32_MIN;
        return ERR_RT_OVERFLOW;
      }

      if (val > INT32_MAX)
      {
        *dst = INT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_UINT64:
    {
      uint64_t val = *reinterpret_cast<const uint64_t*>(d->getData());

      if (val > INT32_MAX)
      {
        *dst = INT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_FLOAT:
    {
      float val = *reinterpret_cast<const float*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < (float)INT32_MIN)
      {
        *dst = INT32_MIN;
        return ERR_RT_OVERFLOW;
      }

      if (val > (float)INT32_MAX)
      {
        *dst = INT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_DOUBLE:
    {
      double val = *reinterpret_cast<const double*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < (double)INT32_MIN)
      {
        *dst = INT32_MIN;
        return ERR_RT_OVERFLOW;
      }

      if (val > (double)INT32_MAX)
      {
        *dst = INT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_STRING_A:
    {
      const StringA* val = reinterpret_cast<const StringA*>(self);
      return val->parseI32(dst);
    }

    case VAR_TYPE_STRING_W:
    {
      const StringW* val = reinterpret_cast<const StringW*>(self);
      return val->parseI32(dst);
    }

    default:
      return ERR_RT_NOT_COMPATIBLE;
  }
}

static err_t FOG_CDECL Var_getI32Bound(const Var* self, int32_t* dst, int32_t min, int32_t max)
{
  if (min > max)
    return ERR_RT_INVALID_ARGUMENT;

  int32_t value;
  FOG_RETURN_ON_ERROR(fog_api.var_getI32(self, &value));

  if (value < min || value > max)
  {
    *dst = min;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = value;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Var_getU32(const Var* self, uint32_t* dst)
{
  VarData* d = self->_d;
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  switch (vType)
  {
    case VAR_TYPE_BOOL:
    {
      *dst = (uint32_t)*reinterpret_cast<const bool*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_CHAR:
    {
      *dst = (uint32_t)reinterpret_cast<const CharW*>(d->getData())->getInt();
      return ERR_OK;
    }

    case VAR_TYPE_INT32:
    {
      int32_t val = *reinterpret_cast<const int32_t*>(d->getData());

      if (val < 0)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_UINT32:
    {
      *dst = *reinterpret_cast<const uint32_t*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_INT64:
    {
      int64_t val = *reinterpret_cast<const int64_t*>(d->getData());

      if (val < 0)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      if (val > UINT32_MAX)
      {
        *dst = UINT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint32_t)(uint64_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_UINT64:
    {
      uint64_t val = *reinterpret_cast<const uint64_t*>(d->getData());

      if (val > UINT32_MAX)
      {
        *dst = UINT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_FLOAT:
    {
      float val = *reinterpret_cast<const float*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < 0.0f)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      if (val > (float)UINT32_MAX)
      {
        *dst = UINT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_DOUBLE:
    {
      double val = *reinterpret_cast<const double*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < 0.0)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      if (val > (double)UINT32_MAX)
      {
        *dst = UINT32_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_STRING_A:
    {
      const StringA* val = reinterpret_cast<const StringA*>(self);
      return val->parseU32(dst);
    }

    case VAR_TYPE_STRING_W:
    {
      const StringW* val = reinterpret_cast<const StringW*>(self);
      return val->parseU32(dst);
    }

    default:
      return ERR_RT_NOT_COMPATIBLE;
  }
}

static err_t FOG_CDECL Var_getU32Bound(const Var* self, uint32_t* dst, uint32_t min, uint32_t max)
{
  if (min > max)
    return ERR_RT_INVALID_ARGUMENT;

  uint32_t value;
  FOG_RETURN_ON_ERROR(fog_api.var_getU32(self, &value));

  if (value < min || value > max)
  {
    *dst = min;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = value;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Var_getI64(const Var* self, int64_t* dst)
{
  VarData* d = self->_d;
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  switch (vType)
  {
    case VAR_TYPE_BOOL:
    {
      *dst = (int64_t)*reinterpret_cast<const bool*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_CHAR:
    {
      *dst = (int64_t)reinterpret_cast<const CharW*>(d->getData())->getInt();
      return ERR_OK;
    }

    case VAR_TYPE_INT32:
    {
      *dst = (int64_t)*reinterpret_cast<const int32_t*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_UINT32:
    {
      *dst = (int64_t)*reinterpret_cast<const uint32_t*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_INT64:
    {
      *dst = *reinterpret_cast<const int64_t*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_UINT64:
    {
      uint64_t val = *reinterpret_cast<const uint64_t*>(d->getData());

      if (val > INT64_MAX)
      {
        *dst = INT64_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int64_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_FLOAT:
    {
      float val = *reinterpret_cast<const float*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < (float)INT64_MIN)
      {
        *dst = INT64_MIN;
        return ERR_RT_OVERFLOW;
      }

      if (val > (float)INT64_MAX)
      {
        *dst = INT64_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int64_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_DOUBLE:
    {
      double val = *reinterpret_cast<const double*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < (double)INT64_MIN)
      {
        *dst = INT64_MIN;
        return ERR_RT_OVERFLOW;
      }

      if (val > (double)INT64_MAX)
      {
        *dst = INT64_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (int64_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_STRING_A:
    {
      const StringA* val = reinterpret_cast<const StringA*>(self);
      return val->parseI64(dst);
    }

    case VAR_TYPE_STRING_W:
    {
      const StringW* val = reinterpret_cast<const StringW*>(self);
      return val->parseI64(dst);
    }

    default:
      return ERR_RT_NOT_COMPATIBLE;
  }
}

static err_t FOG_CDECL Var_getI64Bound(const Var* self, int64_t* dst, int64_t min, int64_t max)
{
  if (min > max)
    return ERR_RT_INVALID_ARGUMENT;

  int64_t value;
  FOG_RETURN_ON_ERROR(fog_api.var_getI64(self, &value));

  if (value < min || value > max)
  {
    *dst = min;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = value;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Var_getU64(const Var* self, uint64_t* dst)
{
  VarData* d = self->_d;
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  switch (vType)
  {
    case VAR_TYPE_BOOL:
    {
      *dst = (uint64_t)*reinterpret_cast<const bool*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_CHAR:
    {
      *dst = (uint64_t)reinterpret_cast<const CharW*>(d->getData())->getInt();
      return ERR_OK;
    }

    case VAR_TYPE_INT32:
    {
      int32_t val = *reinterpret_cast<const int32_t*>(d->getData());

      if (val < 0)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint64_t)(uint32_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_UINT32:
    {
      *dst = (uint64_t)*reinterpret_cast<const uint32_t*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_INT64:
    {
      int64_t val = *reinterpret_cast<const int64_t*>(d->getData());

      if (val < 0)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint64_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_UINT64:
    {
      *dst = *reinterpret_cast<const uint64_t*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_FLOAT:
    {
      float val = *reinterpret_cast<const float*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < 0.0f)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      if (val > (float)UINT64_MAX)
      {
        *dst = UINT64_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint64_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_DOUBLE:
    {
      double val = *reinterpret_cast<const double*>(d->getData());

      if (!Math::isFinite(val))
        return ERR_RT_NOT_A_NUMBER;

      if (val < 0.0)
      {
        *dst = 0;
        return ERR_RT_OVERFLOW;
      }

      if (val > (double)UINT64_MAX)
      {
        *dst = UINT64_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = (uint64_t)val;
      return ERR_OK;
    }

    case VAR_TYPE_STRING_A:
    {
      const StringA* val = reinterpret_cast<const StringA*>(self);
      return val->parseU64(dst);
    }

    case VAR_TYPE_STRING_W:
    {
      const StringW* val = reinterpret_cast<const StringW*>(self);
      return val->parseU64(dst);
    }

    default:
      return ERR_RT_NOT_COMPATIBLE;
  }
}

static err_t FOG_CDECL Var_getU64Bound(const Var* self, uint64_t* dst, uint64_t min, uint64_t max)
{
  if (min > max)
    return ERR_RT_INVALID_ARGUMENT;

  uint64_t value;
  FOG_RETURN_ON_ERROR(fog_api.var_getU64(self, &value));

  if (value < min || value > max)
  {
    *dst = min;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = value;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Var_getFloat(const Var* self, float* dst)
{
  VarData* d = self->_d;
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  switch (vType)
  {
    case VAR_TYPE_BOOL:
    {
      *dst = float((int)*reinterpret_cast<const bool*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_CHAR:
    {
      *dst = float(reinterpret_cast<const CharW*>(d->getData())->getInt());
      return ERR_OK;
    }

    case VAR_TYPE_INT32:
    {
      *dst = float(*reinterpret_cast<const int32_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_UINT32:
    {
      *dst = float(*reinterpret_cast<const uint32_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_INT64:
    {
      *dst = float(*reinterpret_cast<const int64_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_UINT64:
    {
      *dst = float(*reinterpret_cast<const uint64_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_FLOAT:
    {
      *dst = *reinterpret_cast<const float*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_DOUBLE:
    {
      double val = *reinterpret_cast<const double*>(d->getData());

      if (val < double(FLOAT_MIN))
      {
        *dst = FLOAT_MIN;
        return ERR_RT_OVERFLOW;
      }

      if (val > double(FLOAT_MAX))
      {
        *dst = FLOAT_MAX;
        return ERR_RT_OVERFLOW;
      }

      *dst = float(val);
      return ERR_OK;
    }

    case VAR_TYPE_STRING_A:
    {
      const StringA* val = reinterpret_cast<const StringA*>(self);
      return val->parseReal(dst);
    }

    case VAR_TYPE_STRING_W:
    {
      const StringW* val = reinterpret_cast<const StringW*>(self);
      return val->parseReal(dst);
    }

    default:
      return ERR_RT_NOT_COMPATIBLE;
  }
}

static err_t FOG_CDECL Var_getFloatBound(const Var* self, float* dst, float min, float max)
{
  if (!Math::isFinite(min) || !Math::isFinite(max) || min > max)
    return ERR_RT_INVALID_ARGUMENT;

  float value;
  FOG_RETURN_ON_ERROR(fog_api.var_getFloat(self, &value));

  if (!Math::isFinite(value) || value < min || value > max)
  {
    *dst = min;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = value;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Var_getDouble(const Var* self, double* dst)
{
  VarData* d = self->_d;
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  switch (vType)
  {
    case VAR_TYPE_BOOL:
    {
      *dst = double((int)*reinterpret_cast<const bool*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_CHAR:
    {
      *dst = double(reinterpret_cast<const CharW*>(d->getData())->getInt());
      return ERR_OK;
    }

    case VAR_TYPE_INT32:
    {
      *dst = double(*reinterpret_cast<const int32_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_UINT32:
    {
      *dst = double(*reinterpret_cast<const uint32_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_INT64:
    {
      *dst = double(*reinterpret_cast<const int64_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_UINT64:
    {
      *dst = double(*reinterpret_cast<const uint64_t*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_FLOAT:
    {
      *dst = double(*reinterpret_cast<const float*>(d->getData()));
      return ERR_OK;
    }

    case VAR_TYPE_DOUBLE:
    {
      *dst = *reinterpret_cast<const double*>(d->getData());
      return ERR_OK;
    }

    case VAR_TYPE_STRING_A:
    {
      const StringA* val = reinterpret_cast<const StringA*>(self);
      return val->parseReal(dst);
    }

    case VAR_TYPE_STRING_W:
    {
      const StringW* val = reinterpret_cast<const StringW*>(self);
      return val->parseReal(dst);
    }

    default:
      return ERR_RT_NOT_COMPATIBLE;
  }
}

static err_t FOG_CDECL Var_getDoubleBound(const Var* self, double* dst, double min, double max)
{
  if (!Math::isFinite(min) || !Math::isFinite(max) || min > max)
    return ERR_RT_INVALID_ARGUMENT;

  double value;
  FOG_RETURN_ON_ERROR(fog_api.var_getDouble(self, &value));

  if (!Math::isFinite(value) || value < min || value > max)
  {
    *dst = min;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = value;
    return ERR_OK;
  }
}

static err_t FOG_CDECL Var_getType(const Var* self, uint32_t vType, void* vData)
{
  VarData* d = self->_d;
  uint32_t dType = d->vType & VAR_TYPE_MASK;

  if (dType == vType)
  {
    switch (vType)
    {
      case VAR_TYPE_BOOL:
      case VAR_TYPE_CHAR:
      case VAR_TYPE_INT32:
      case VAR_TYPE_UINT32:
      case VAR_TYPE_INT64:
      case VAR_TYPE_UINT64:
      case VAR_TYPE_FLOAT:
      case VAR_TYPE_DOUBLE:
_SetSimple:
        MemOps::copy(vData, d->getData(), Var_getDataSize(vType));
        return ERR_OK;

      case VAR_TYPE_STRING_A:
      case VAR_TYPE_STRING_W:
      case VAR_TYPE_LIST_STRING_A:
      case VAR_TYPE_LIST_STRING_W:
      case VAR_TYPE_LIST_VAR:
      case VAR_TYPE_HASH_STRING_A_STRING_A:
      case VAR_TYPE_HASH_STRING_A_VAR:
      case VAR_TYPE_HASH_STRING_W_STRING_W:
      case VAR_TYPE_HASH_STRING_W_VAR:
      case VAR_TYPE_REGEXP_A:
      case VAR_TYPE_REGEXP_W:
      case VAR_TYPE_LOCALE:
_SetImplicit:
        atomicPtrXchg(&static_cast<Var*>(vData)->_d, Var_dImplicitAddRef(d))->release();
        return ERR_OK;

      case VAR_TYPE_DATE:
      case VAR_TYPE_TIME:
        // ... Fall through ...

      case VAR_TYPE_POINT_I:
      case VAR_TYPE_POINT_F:
      case VAR_TYPE_POINT_D:
      case VAR_TYPE_SIZE_I:
      case VAR_TYPE_SIZE_F:
      case VAR_TYPE_SIZE_D:
      case VAR_TYPE_BOX_I:
      case VAR_TYPE_BOX_F:
      case VAR_TYPE_BOX_D:
      case VAR_TYPE_RECT_I:
      case VAR_TYPE_RECT_F:
      case VAR_TYPE_RECT_D:
      // Reserved for VAR_TYPE_LINEI.
      case VAR_TYPE_LINE_F:
      case VAR_TYPE_LINE_D:
      case VAR_TYPE_QBEZIER_F:
      case VAR_TYPE_QBEZIER_D:
      case VAR_TYPE_CBEZIER_F:
      case VAR_TYPE_CBEZIER_D:
      case VAR_TYPE_TRIANGLE_F:
      case VAR_TYPE_TRIANGLE_D:
      case VAR_TYPE_ROUND_F:
      case VAR_TYPE_ROUND_D:
      case VAR_TYPE_CIRCLE_F:
      case VAR_TYPE_CIRCLE_D:
      case VAR_TYPE_ELLIPSE_F:
      case VAR_TYPE_ELLIPSE_D:
      case VAR_TYPE_ARC_F:
      case VAR_TYPE_ARC_D:
      case VAR_TYPE_CHORD_F:
      case VAR_TYPE_CHORD_D:
      case VAR_TYPE_PIE_F:
      case VAR_TYPE_PIE_D:
        goto _SetSimple;

      case VAR_TYPE_PATH_F:
      case VAR_TYPE_PATH_D:
      case VAR_TYPE_REGION:
        goto _SetImplicit;

      case VAR_TYPE_TRANSFORM_F:
      case VAR_TYPE_TRANSFORM_D:
        goto _SetSimple;

      case VAR_TYPE_MATRIX_F:
      case VAR_TYPE_MATRIX_D:
        goto _SetImplicit;

      case VAR_TYPE_COLOR:
        *static_cast<Color*>(vData) = reinterpret_cast<PatternColorData*>(d)->color();
        return ERR_OK;
      case VAR_TYPE_PATTERN:
        goto _SetImplicit;

      case VAR_TYPE_COLOR_STOP:
        goto _SetSimple;
      case VAR_TYPE_COLOR_STOP_LIST:
        goto _SetImplicit;

      case VAR_TYPE_IMAGE:
      case VAR_TYPE_IMAGE_PALETTE:
      case VAR_TYPE_IMAGE_FILTER:
        goto _SetImplicit;

      case VAR_TYPE_FACE_INFO:
      case VAR_TYPE_FACE_COLLECTION:
      case VAR_TYPE_FONT:
        goto _SetImplicit;
        
      // TODO: Var::getType() - Support other types as well.

      default:
        break;
    }
  }

  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::Var - Set]
// ============================================================================

static err_t FOG_CDECL Var_setType(Var* self, uint32_t vType, const void* vData)
{
  VarData* d = self->_d;

  switch (vType)
  {
    case VAR_TYPE_BOOL:
    case VAR_TYPE_CHAR:
    case VAR_TYPE_INT32:
    case VAR_TYPE_UINT32:
    case VAR_TYPE_INT64:
    case VAR_TYPE_UINT64:
    case VAR_TYPE_FLOAT:
    case VAR_TYPE_DOUBLE:
_SetSimple:
    {
      size_t dataSize = Var_getDataSize(vType);
      if ((d->vType & VAR_TYPE_MASK) != vType || Var_dImplicitGetRef(d) != 1)
      {
        d = fog_api.var_dCreate(dataSize);
        if (FOG_IS_NULL(d))
          return ERR_RT_OUT_OF_MEMORY;

        d->vType = vType;
        atomicPtrXchg(&self->_d, d)->release();
      }
      MemOps::copy(d->getData(), vData, dataSize);
      return ERR_OK;
    }

    case VAR_TYPE_STRING_A:
    case VAR_TYPE_STRING_W:
    case VAR_TYPE_LIST_STRING_A:
    case VAR_TYPE_LIST_STRING_W:
    case VAR_TYPE_LIST_VAR:
    case VAR_TYPE_HASH_STRING_A_STRING_A:
    case VAR_TYPE_HASH_STRING_A_VAR:
    case VAR_TYPE_HASH_STRING_W_STRING_W:
    case VAR_TYPE_HASH_STRING_W_VAR:
    case VAR_TYPE_REGEXP_A:
    case VAR_TYPE_REGEXP_W:
    case VAR_TYPE_LOCALE:
_SetImplicit:
      atomicPtrXchg(&self->_d, Var_dImplicitAddRef(static_cast<const Var*>(vData)->_d))->release();
      return ERR_OK;

    case VAR_TYPE_DATE:
    case VAR_TYPE_TIME:
      // ... Fall through ...

    case VAR_TYPE_POINT_I:
    case VAR_TYPE_POINT_F:
    case VAR_TYPE_POINT_D:
    case VAR_TYPE_SIZE_I:
    case VAR_TYPE_SIZE_F:
    case VAR_TYPE_SIZE_D:
    case VAR_TYPE_BOX_I:
    case VAR_TYPE_BOX_F:
    case VAR_TYPE_BOX_D:
    case VAR_TYPE_RECT_I:
    case VAR_TYPE_RECT_F:
    case VAR_TYPE_RECT_D:
    // Reserved for VAR_TYPE_LINEI.
    case VAR_TYPE_LINE_F:
    case VAR_TYPE_LINE_D:
    case VAR_TYPE_QBEZIER_F:
    case VAR_TYPE_QBEZIER_D:
    case VAR_TYPE_CBEZIER_F:
    case VAR_TYPE_CBEZIER_D:
    case VAR_TYPE_TRIANGLE_F:
    case VAR_TYPE_TRIANGLE_D:
    case VAR_TYPE_ROUND_F:
    case VAR_TYPE_ROUND_D:
    case VAR_TYPE_CIRCLE_F:
    case VAR_TYPE_CIRCLE_D:
    case VAR_TYPE_ELLIPSE_F:
    case VAR_TYPE_ELLIPSE_D:
    case VAR_TYPE_ARC_F:
    case VAR_TYPE_ARC_D:
    case VAR_TYPE_CHORD_F:
    case VAR_TYPE_CHORD_D:
    case VAR_TYPE_PIE_F:
    case VAR_TYPE_PIE_D:
      goto _SetSimple;

    case VAR_TYPE_PATH_F:
    case VAR_TYPE_PATH_D:
    case VAR_TYPE_REGION:
      goto _SetImplicit;

    case VAR_TYPE_TRANSFORM_F:
    case VAR_TYPE_TRANSFORM_D:
      goto _SetSimple;

    case VAR_TYPE_MATRIX_F:
    case VAR_TYPE_MATRIX_D:
      goto _SetImplicit;

    case VAR_TYPE_COLOR:
      if ((d->vType & VAR_TYPE_MASK) == VAR_TYPE_COLOR &&
          (reinterpret_cast<PatternColorData*>(d)->reference.get() == 1))
      {
        reinterpret_cast<PatternColorData*>(d)->color() = *static_cast<const Color*>(vData);
        return ERR_OK;
      }
      else
      {
        d->release();
        fog_api.pattern_ctorColor(reinterpret_cast<Pattern*>(self), static_cast<const Color*>(vData));
        return ERR_OK;
      }

    case VAR_TYPE_PATTERN:
      goto _SetImplicit;
    
    case VAR_TYPE_COLOR_STOP:
      goto _SetSimple;
    case VAR_TYPE_COLOR_STOP_LIST:
      goto _SetImplicit;

    case VAR_TYPE_IMAGE:
    case VAR_TYPE_IMAGE_PALETTE:
    case VAR_TYPE_IMAGE_FILTER:
      goto _SetImplicit;

    case VAR_TYPE_FACE_INFO:
    case VAR_TYPE_FACE_COLLECTION:
    case VAR_TYPE_FONT:
      goto _SetImplicit;

    // TODO: Var::setType() - Support other types as well.

    default:
      return ERR_RT_INVALID_STATE;
  }
}

// ============================================================================
// [Fog::Var - Equality]
// ============================================================================

static FOG_INLINE int _sign(int32_t value)
{
  if (value > 0) return 1;
  if (value < 0) return -1;

  return 0;
}

static FOG_INLINE int _sign(int64_t value)
{
  if (value > 0) return 1;
  if (value < 0) return -1;

  return 0;
}

static FOG_INLINE int _sign(float value)
{
  if (value == Math::getNInfF())
    return 101;

  if (value == Math::getPInfF())
    return 102;

  if (Math::isNaN(value))
    return 103;

  if (value > 0.0f) return 1;
  if (value < 0.0f) return -1;

  return 1;
}

static FOG_INLINE int _sign(double value)
{
  if (value == Math::getNInfD())
    return 101;

  if (value == Math::getPInfD())
    return 102;

  if (Math::isNaN(value))
    return 103;

  if (value > 0.0f) return 1;
  if (value < 0.0f) return -1;

  return 0;
}

static int Var_getSign(const VarData* d, uint32_t vType)
{
  FOG_ASSERT(vType <= _VAR_TYPE_NUMBER_END);

  switch (vType)
  {
    case VAR_TYPE_NULL:
      return 100;

    case VAR_TYPE_BOOL:
      return VAR_SIMPLE_C(d, bool);

    case VAR_TYPE_CHAR:
      return VAR_SIMPLE_C(d, CharW).getInt() != 0;

    case VAR_TYPE_INT32:
      return _sign(VAR_SIMPLE_C(d, int32_t));

    case VAR_TYPE_UINT32:
      return VAR_SIMPLE_C(d, uint32_t) != 0;

    case VAR_TYPE_INT64:
      return _sign(VAR_SIMPLE_C(d, int64_t));

    case VAR_TYPE_UINT64:
      return VAR_SIMPLE_C(d, uint64_t) != 0;

    case VAR_TYPE_FLOAT:
      return _sign(VAR_SIMPLE_C(d, float));

    case VAR_TYPE_DOUBLE:
      return _sign(VAR_SIMPLE_C(d, double));

    default:
      FOG_ASSERT_NOT_REACHED();
  }
}

static bool Var_toDouble(double& dst, VarData* d, uint32_t vType)
{
  switch (vType)
  {
    case VAR_TYPE_BOOL:
      dst = double(VAR_SIMPLE_C(d, bool));
      return true;

    case VAR_TYPE_CHAR:
      dst = double(VAR_SIMPLE_C(d, CharW).getInt());
      return true;

    case VAR_TYPE_INT32:
      dst = double(VAR_SIMPLE_C(d, int32_t));
      return true;

    case VAR_TYPE_UINT32:
      dst = double(VAR_SIMPLE_C(d, uint32_t));
      return true;

    case VAR_TYPE_INT64 :
      dst = double(VAR_SIMPLE_C(d, int64_t));
      return true;

    case VAR_TYPE_UINT64:
      dst = double(VAR_SIMPLE_C(d, uint64_t));
      return true;

    case VAR_TYPE_FLOAT :
      dst = double(VAR_SIMPLE_C(d, float));
      return (dst >= double(INT64_MIN) && dst <= double(INT64_MAX));

    case VAR_TYPE_DOUBLE:
      dst = VAR_SIMPLE_C(d, double);
      return (dst >= double(INT64_MIN) && dst <= double(INT64_MAX));

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return true;
}

static bool FOG_CDECL Var_eq(const Var* a, const Var* b)
{
  VarData* a_d = a->_d;
  VarData* b_d = b->_d;

  if (a_d == b_d)
    return true;

  uint32_t aType = a_d->vType & VAR_TYPE_MASK;
  uint32_t bType = b_d->vType & VAR_TYPE_MASK;

  // --------------------------------------------------------------------------
  // [aType == bType]
  // --------------------------------------------------------------------------

  // Both types are equal. In such case we use the equality method implemented
  // by the type. There is of course room for some optimizations. For example
  // we can create a table where we store size of types which can be checked
  // for equality using binary compare - MemOps::eq(), but it could be more
  // complicated than the current approach, mainly in cases that equality
  // method is redesigned in some class.

  if (aType == bType)
  {
    switch (aType)
    {
      case VAR_TYPE_NULL:
        return true;

      case VAR_TYPE_BOOL:
        return VAR_SIMPLE_C(a_d, bool) == VAR_SIMPLE_C(b_d, bool);

      case VAR_TYPE_CHAR:
        return VAR_SIMPLE_C(a_d, CharW) == VAR_SIMPLE_C(b_d, CharW);

      case VAR_TYPE_INT32:
      case VAR_TYPE_UINT32:
      case VAR_TYPE_FLOAT:
        return VAR_SIMPLE_C(a_d, uint32_t) == VAR_SIMPLE_C(b_d, uint32_t);

      case VAR_TYPE_INT64:
      case VAR_TYPE_UINT64:
      case VAR_TYPE_DOUBLE:
        return VAR_SIMPLE_C(a_d, uint64_t) == VAR_SIMPLE_C(b_d, uint64_t);

      case VAR_TYPE_STRING_A:
        return *reinterpret_cast<const StringA*>(a) == *reinterpret_cast<const StringA*>(b);
      case VAR_TYPE_STRING_W:
        return *reinterpret_cast<const StringW*>(a) == *reinterpret_cast<const StringW*>(b);

      case VAR_TYPE_LIST_STRING_A:
        return *reinterpret_cast<const List<StringA>*>(a) == *reinterpret_cast<const List<StringA>*>(b);
      case VAR_TYPE_LIST_STRING_W:
        return *reinterpret_cast<const List<StringW>*>(a) == *reinterpret_cast<const List<StringW>*>(b);
      case VAR_TYPE_LIST_VAR:
        return *reinterpret_cast<const List<Var>*>(a) == *reinterpret_cast<const List<Var>*>(b);

      case VAR_TYPE_HASH_STRING_A_STRING_A:
        return *reinterpret_cast<const Hash<StringA, StringA>*>(a) == *reinterpret_cast<const Hash<StringA, StringA>*>(b);
      case VAR_TYPE_HASH_STRING_A_VAR:
        return *reinterpret_cast<const Hash<StringA, Var>*>(a) == *reinterpret_cast<const Hash<StringA, Var>*>(b);

      case VAR_TYPE_HASH_STRING_W_STRING_W:
        return *reinterpret_cast<const Hash<StringW, StringW>*>(a) == *reinterpret_cast<const Hash<StringW, StringW>*>(b);
      case VAR_TYPE_HASH_STRING_W_VAR:
        return *reinterpret_cast<const Hash<StringW, Var>*>(a) == *reinterpret_cast<const Hash<StringW, Var>*>(b);

      case VAR_TYPE_REGEXP_A:
        //return *reinterpret_cast<const RegExpA*>(a) == *reinterpret_cast<const RegExpA*>(b);
        // TODO:
        return false;
      case VAR_TYPE_REGEXP_W:
        //return *reinterpret_cast<const RegExpW*>(a) == *reinterpret_cast<const RegExpW*>(b);
        // TODO:
        return false;

      case VAR_TYPE_LOCALE:
        return *reinterpret_cast<const Locale*>(a) == *reinterpret_cast<const Locale*>(b);

      case VAR_TYPE_DATE:
        return VAR_SIMPLE_C(a_d, Date) == VAR_SIMPLE_C(b_d, Date);
      case VAR_TYPE_TIME:
        return VAR_SIMPLE_C(a_d, Time) == VAR_SIMPLE_C(b_d, Time);

      case VAR_TYPE_POINT_I:
        return VAR_SIMPLE_C(a_d, PointI) == VAR_SIMPLE_C(b_d, PointI);
      case VAR_TYPE_POINT_F:
        return VAR_SIMPLE_C(a_d, PointF) == VAR_SIMPLE_C(b_d, PointF);
      case VAR_TYPE_POINT_D:
        return VAR_SIMPLE_C(a_d, PointD) == VAR_SIMPLE_C(b_d, PointD);

      case VAR_TYPE_SIZE_I:
        return VAR_SIMPLE_C(a_d, SizeI) == VAR_SIMPLE_C(b_d, SizeI);
      case VAR_TYPE_SIZE_F:
        return VAR_SIMPLE_C(a_d, SizeF) == VAR_SIMPLE_C(b_d, SizeF);
      case VAR_TYPE_SIZE_D:
        return VAR_SIMPLE_C(a_d, SizeD) == VAR_SIMPLE_C(b_d, SizeD);

      case VAR_TYPE_BOX_I:
        return VAR_SIMPLE_C(a_d, BoxI) == VAR_SIMPLE_C(b_d, BoxI);
      case VAR_TYPE_BOX_F:
        return VAR_SIMPLE_C(a_d, BoxF) == VAR_SIMPLE_C(b_d, BoxF);
      case VAR_TYPE_BOX_D:
        return VAR_SIMPLE_C(a_d, BoxD) == VAR_SIMPLE_C(b_d, BoxD);

      case VAR_TYPE_RECT_I:
        return VAR_SIMPLE_C(a_d, RectI) == VAR_SIMPLE_C(b_d, RectI);
      case VAR_TYPE_RECT_F:
        return VAR_SIMPLE_C(a_d, RectF) == VAR_SIMPLE_C(b_d, RectF);
      case VAR_TYPE_RECT_D:
        return VAR_SIMPLE_C(a_d, RectD) == VAR_SIMPLE_C(b_d, RectD);

      case VAR_TYPE_LINE_F:
        return VAR_SIMPLE_C(a_d, LineF) == VAR_SIMPLE_C(b_d, LineF);
      case VAR_TYPE_LINE_D:
        return VAR_SIMPLE_C(a_d, LineD) == VAR_SIMPLE_C(b_d, LineD);

      case VAR_TYPE_QBEZIER_F:
        return VAR_SIMPLE_C(a_d, QBezierF) == VAR_SIMPLE_C(b_d, QBezierF);
      case VAR_TYPE_QBEZIER_D:
        return VAR_SIMPLE_C(a_d, QBezierD) == VAR_SIMPLE_C(b_d, QBezierD);

      case VAR_TYPE_CBEZIER_F:
        return VAR_SIMPLE_C(a_d, CBezierF) == VAR_SIMPLE_C(b_d, CBezierF);
      case VAR_TYPE_CBEZIER_D:
        return VAR_SIMPLE_C(a_d, CBezierD) == VAR_SIMPLE_C(b_d, CBezierD);

      case VAR_TYPE_TRIANGLE_F:
        return VAR_SIMPLE_C(a_d, TriangleF) == VAR_SIMPLE_C(b_d, TriangleF);
      case VAR_TYPE_TRIANGLE_D:
        return VAR_SIMPLE_C(a_d, TriangleD) == VAR_SIMPLE_C(b_d, TriangleD);

      case VAR_TYPE_ROUND_F:
        return VAR_SIMPLE_C(a_d, RoundF) == VAR_SIMPLE_C(b_d, RoundF);
      case VAR_TYPE_ROUND_D:
        return VAR_SIMPLE_C(a_d, RoundD) == VAR_SIMPLE_C(b_d, RoundD);

      case VAR_TYPE_CIRCLE_F:
        return VAR_SIMPLE_C(a_d, CircleF) == VAR_SIMPLE_C(b_d, CircleF);
      case VAR_TYPE_CIRCLE_D:
        return VAR_SIMPLE_C(a_d, CircleD) == VAR_SIMPLE_C(b_d, CircleD);

      case VAR_TYPE_ELLIPSE_F:
        return VAR_SIMPLE_C(a_d, EllipseF) == VAR_SIMPLE_C(b_d, EllipseF);
      case VAR_TYPE_ELLIPSE_D:
        return VAR_SIMPLE_C(a_d, EllipseD) == VAR_SIMPLE_C(b_d, EllipseD);

      case VAR_TYPE_ARC_F:
        return VAR_SIMPLE_C(a_d, ArcF) == VAR_SIMPLE_C(b_d, ArcF);
      case VAR_TYPE_ARC_D:
        return VAR_SIMPLE_C(a_d, ArcD) == VAR_SIMPLE_C(b_d, ArcD);

      case VAR_TYPE_CHORD_F:
        return VAR_SIMPLE_C(a_d, ChordF) == VAR_SIMPLE_C(b_d, ChordF);
      case VAR_TYPE_CHORD_D:
        return VAR_SIMPLE_C(a_d, ChordD) == VAR_SIMPLE_C(b_d, ChordD);

      case VAR_TYPE_PIE_F:
        return VAR_SIMPLE_C(a_d, PieF) == VAR_SIMPLE_C(b_d, PieF);
      case VAR_TYPE_PIE_D:
        return VAR_SIMPLE_C(a_d, PieD) == VAR_SIMPLE_C(b_d, PieD);

      case VAR_TYPE_POLYGON_F:
        // TODO: Var.
        return false;
      case VAR_TYPE_POLYGON_D:
        // TODO: Var.
        return false;

      case VAR_TYPE_PATH_F:
        return *reinterpret_cast<const PathF*>(a) == *reinterpret_cast<const PathF*>(b);
      case VAR_TYPE_PATH_D:
        return *reinterpret_cast<const PathD*>(a) == *reinterpret_cast<const PathD*>(b);

      case VAR_TYPE_REGION:
        return *reinterpret_cast<const Region*>(a) == *reinterpret_cast<const Region*>(b);

      case VAR_TYPE_TRANSFORM_F:
        return *reinterpret_cast<const TransformF*>(a) == *reinterpret_cast<const TransformF*>(b);
      case VAR_TYPE_TRANSFORM_D:
        return *reinterpret_cast<const TransformD*>(a) == *reinterpret_cast<const TransformD*>(b);

      case VAR_TYPE_MATRIX_F:
        return *reinterpret_cast<const MatrixF*>(a) == *reinterpret_cast<const MatrixF*>(b);
      case VAR_TYPE_MATRIX_D:
        return *reinterpret_cast<const MatrixD*>(a) == *reinterpret_cast<const MatrixD*>(b);

      case VAR_TYPE_COLOR:
        return VAR_OBJECT_C(a_d, PatternColorData)->color() == VAR_OBJECT_C(b_d, PatternColorData)->color();
      case VAR_TYPE_PATTERN:
        return *reinterpret_cast<const Pattern*>(a) == *reinterpret_cast<const Pattern*>(b);

      case VAR_TYPE_COLOR_STOP:
        return VAR_SIMPLE_C(a_d, ColorStop) == VAR_SIMPLE_C(b_d, ColorStop);

      case VAR_TYPE_COLOR_STOP_LIST:
        return *reinterpret_cast<const ColorStopList*>(a) == *reinterpret_cast<const ColorStopList*>(b);

      case VAR_TYPE_IMAGE:
        return *reinterpret_cast<const Image*>(a) == *reinterpret_cast<const Image*>(b);

      case VAR_TYPE_IMAGE_PALETTE:
        return *reinterpret_cast<const ImagePalette*>(a) == *reinterpret_cast<const ImagePalette*>(b);

      case VAR_TYPE_IMAGE_FILTER:
        return *reinterpret_cast<const ImageFilter*>(a) == *reinterpret_cast<const ImageFilter*>(b);

      case VAR_TYPE_FACE_INFO:
        return *reinterpret_cast<const FaceInfo*>(a) == *reinterpret_cast<const FaceInfo*>(b);

      case VAR_TYPE_FACE_COLLECTION:
        return *reinterpret_cast<const FaceCollection*>(a) == *reinterpret_cast<const FaceCollection*>(b);

      case VAR_TYPE_FONT:
        return *reinterpret_cast<const Font*>(a) == *reinterpret_cast<const Font*>(b);

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }

  // --------------------------------------------------------------------------
  // [aType != bType - Numbers]
  // --------------------------------------------------------------------------

  // Both are numbers. If the type of 'a' is different to the type of 'b' then
  // it doesn't mean that variables aren't equal. It's needed to compare the
  // numbers, not only types, because (int32_t)5 == (uint64_t)5. We first
  // compare signs and then values.
  //
  // As a side effect, we compare also Number vs NULL and vica-versa.

  if (Math::isBounded<uint32_t>(aType, VAR_TYPE_NULL, _VAR_TYPE_NUMBER_END) &&
      Math::isBounded<uint32_t>(bType, VAR_TYPE_NULL, _VAR_TYPE_NUMBER_END))
  {
    int aSign = Var_getSign(a_d, aType);
    int bSign = Var_getSign(b_d, bType);

    // If signs do not match then numbers are not equal.
    if (aSign != bSign)
      return false;

    // If both numbers are special (Infinity, NaN, Null) then they are always
    // equal.
    if (aSign > 1)
      return true;

    // Different types, but the same number - zero.
    if (aSign == 0)
      return true;

    // If we are here then we know that the signs of 'a' and 'b' are equal, and
    // numbers are finite (non NaN or Infinity in case that 'a' or 'b' floating
    // point). We also know that types of numbers are not the same, so for
    // example 'float' vs 'float' comparison is not possible. We use that
    // information to cast the number to the most precise type which can be then
    // compared for equality.

    int aIsFloat = Math::isBounded<uint32_t>(aType, VAR_TYPE_FLOAT, VAR_TYPE_DOUBLE);
    int bIsFloat = Math::isBounded<uint32_t>(bType, VAR_TYPE_FLOAT, VAR_TYPE_DOUBLE);

    // Start with FLOAT <-> DOUBLE comparison.
    if (aIsFloat & bIsFloat)
    {
      // We know that one of 'a' or 'b' is double so we use double to prevent
      // loosing precision.
      DoubleBits aBits;
      DoubleBits bBits;

      if (aType == VAR_TYPE_FLOAT)
      {
        aBits.d   = VAR_SIMPLE_C(a_d, float);
        bBits.u64 = VAR_SIMPLE_C(b_d, uint64_t);
      }
      else
      {
        aBits.u64 = VAR_SIMPLE_C(a_d, uint64_t);
        bBits.d   = VAR_SIMPLE_C(b_d, float);
      }

      // Do binary comparison. It's probably not needed here, because all
      // special numbers were checked earlier and we can't be here if 'a' or
      // 'b' is NaN, Infinity, ...
      return aBits.u64 == bBits.u64;
    }

    // Continue comparing floating point type and integral type. Floating point
    // range is much higher than integral range, but floating point precision
    // affected (for example int64_t can't be stored in losslessly in float or
    // double).
    if (aIsFloat | bIsFloat)
    {
      DoubleBits aBits;
      DoubleBits bBits;

      if (!Var_toDouble(aBits.d, a_d, aType))
        return false;

      if (!Var_toDouble(bBits.d, b_d, bType))
        return false;

      return aBits.u64 == bBits.u64;
    }

    // Continue comparing integer types. We know that the signs of 'a' and 'b'
    // are equal, so we sign extend both values to 64-bit integers and compare
    // them for equality. This works for signed vs signed and unsigned vs
    // unsigned numbers.
    {
      int64_t aInt;
      int64_t bInt;

      switch (aType)
      {
        case VAR_TYPE_BOOL:
          aInt = (int64_t)VAR_SIMPLE_C(a_d, bool);
          break;

        case VAR_TYPE_CHAR:
          aInt = (int64_t)VAR_SIMPLE_C(a_d, CharW).getInt();
          break;

        case VAR_TYPE_INT32:
        case VAR_TYPE_UINT32:
          aInt = (int64_t)VAR_SIMPLE_C(a_d, int32_t);
          break;

        case VAR_TYPE_INT64:
        case VAR_TYPE_UINT64:
          aInt = (int64_t)VAR_SIMPLE_C(a_d, int64_t);
          break;
      }

      switch (bType)
      {
        case VAR_TYPE_BOOL:
          bInt = (int64_t)VAR_SIMPLE_C(b_d, bool);
          break;

        case VAR_TYPE_CHAR:
          bInt = (int64_t)VAR_SIMPLE_C(b_d, CharW).getInt();
          break;

        case VAR_TYPE_INT32:
        case VAR_TYPE_UINT32:
          bInt = (int64_t)VAR_SIMPLE_C(b_d, int32_t);
          break;

        case VAR_TYPE_INT64:
        case VAR_TYPE_UINT64:
          bInt = (int64_t)VAR_SIMPLE_C(b_d, int64_t);
          break;
      }

      return aInt == bInt;
    }
  }

  // --------------------------------------------------------------------------
  // [aType != bType - ]
  // --------------------------------------------------------------------------

  // TODO:
  return false;
}

// ============================================================================
// [Fog::Var - Compare]
// ============================================================================

static int FOG_CDECL Var_compare(const Var* a, const Var* b)
{
  // TODO:
  return 0;
}

// ============================================================================
// [Fog::Var - Data]
// ============================================================================

static VarData* FOG_CDECL Var_dCreate(size_t dataSize)
{
  size_t memSize = sizeof(VarData) + dataSize;
  if (memSize < sizeof(VarSimpleData))
    memSize = sizeof(VarSimpleData);

  VarData* d = reinterpret_cast<VarData*>(MemMgr::alloc(memSize));
  if (FOG_IS_NULL(d))
    return NULL;

  d->unknown.reference = 1;
  d->vType = VAR_TYPE_NULL;
  FOG_PADDING_ZERO_64(d->padding_0_32);

  return d;
}

static VarData* FOG_CDECL Var_dAddRef(VarData* d)
{
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  // NULL type has no reference.
  if (vType == VAR_TYPE_NULL)
    return d;

  if (vType != VAR_TYPE_OBJECT_REF)
  {
    AtomicCore<size_t>::inc(&d->unknown.reference);
    return d;
  }
  else
  {
    // TODO: Var
    return d;
  }
}

static void FOG_CDECL Var_dRelease(VarData* d)
{
  uint32_t vType = d->vType & VAR_TYPE_MASK;

  switch (vType)
  {
    case VAR_TYPE_NULL:
      return;

    case VAR_TYPE_BOOL:
    case VAR_TYPE_CHAR:
    case VAR_TYPE_INT32:
    case VAR_TYPE_UINT32:
    case VAR_TYPE_INT64:
    case VAR_TYPE_UINT64:
    case VAR_TYPE_FLOAT:
    case VAR_TYPE_DOUBLE:
      goto _ReleaseSimple;

    case VAR_TYPE_STRING_A:
      reinterpret_cast<StringDataA*>(d)->release();
      return;

    case VAR_TYPE_STRING_W:
      reinterpret_cast<StringDataW*>(d)->release();
      return;

    case VAR_TYPE_LIST_STRING_A:
      if (reinterpret_cast<ListUntypedData*>(d)->reference.deref())
        fog_api.list_unknown_dFree(reinterpret_cast<ListUntypedData*>(d), fog_api.list_stringa_vTable);
      return;

    case VAR_TYPE_LIST_STRING_W:
      if (reinterpret_cast<ListUntypedData*>(d)->reference.deref())
        fog_api.list_unknown_dFree(reinterpret_cast<ListUntypedData*>(d), fog_api.list_stringw_vTable);
      return;

    case VAR_TYPE_LIST_VAR:
      if (reinterpret_cast<ListUntypedData*>(d)->reference.deref())
        fog_api.list_unknown_dFree(reinterpret_cast<ListUntypedData*>(d), fog_api.list_var_vTable);
      return;

    case VAR_TYPE_HASH_STRING_A_STRING_A:
      if (reinterpret_cast<HashUntypedData*>(d)->reference.deref())
        fog_api.hash_stringa_stringa_dFree(reinterpret_cast<HashUntypedData*>(d));
      return;

    case VAR_TYPE_HASH_STRING_A_VAR:
      if (reinterpret_cast<HashUntypedData*>(d)->reference.deref())
        fog_api.hash_stringa_var_dFree(reinterpret_cast<HashUntypedData*>(d));
      return;

    case VAR_TYPE_HASH_STRING_W_STRING_W:
      if (reinterpret_cast<HashUntypedData*>(d)->reference.deref())
        fog_api.hash_stringw_stringw_dFree(reinterpret_cast<HashUntypedData*>(d));
      return;

    case VAR_TYPE_HASH_STRING_W_VAR:
      if (reinterpret_cast<HashUntypedData*>(d)->reference.deref())
        fog_api.hash_stringw_var_dFree(reinterpret_cast<HashUntypedData*>(d));
      return;

    case VAR_TYPE_REGEXP_A:
      reinterpret_cast<RegExpDataA*>(d)->release();
      return;

    case VAR_TYPE_REGEXP_W:
      reinterpret_cast<RegExpDataW*>(d)->release();
      return;

    case VAR_TYPE_LOCALE:
      reinterpret_cast<LocaleData*>(d)->release();
      return;

    case VAR_TYPE_DATE:
    case VAR_TYPE_TIME:
      goto _ReleaseSimple;

    case VAR_TYPE_POINT_I:
    case VAR_TYPE_POINT_F:
    case VAR_TYPE_POINT_D:
    case VAR_TYPE_SIZE_I:
    case VAR_TYPE_SIZE_F:
    case VAR_TYPE_SIZE_D:
    case VAR_TYPE_BOX_I:
    case VAR_TYPE_BOX_F:
    case VAR_TYPE_BOX_D:
    case VAR_TYPE_RECT_I:
    case VAR_TYPE_RECT_F:
    case VAR_TYPE_RECT_D:
    case VAR_TYPE_LINE_F:
    case VAR_TYPE_LINE_D:
    case VAR_TYPE_QBEZIER_F:
    case VAR_TYPE_QBEZIER_D:
    case VAR_TYPE_CBEZIER_F:
    case VAR_TYPE_CBEZIER_D:
    case VAR_TYPE_TRIANGLE_F:
    case VAR_TYPE_TRIANGLE_D:
    case VAR_TYPE_ROUND_F:
    case VAR_TYPE_ROUND_D:
    case VAR_TYPE_CIRCLE_F:
    case VAR_TYPE_CIRCLE_D:
    case VAR_TYPE_ELLIPSE_F:
    case VAR_TYPE_ELLIPSE_D:
    case VAR_TYPE_ARC_F:
    case VAR_TYPE_ARC_D:
    case VAR_TYPE_CHORD_F:
    case VAR_TYPE_CHORD_D:
    case VAR_TYPE_PIE_F:
    case VAR_TYPE_PIE_D:
      goto _ReleaseSimple;

    case VAR_TYPE_POLYGON_F:
      // TODO: Var.
      return;

    case VAR_TYPE_POLYGON_D:
      // TODO: Var.
      return;

    case VAR_TYPE_PATH_F:
      reinterpret_cast<PathDataF*>(d)->release();
      return;

    case VAR_TYPE_PATH_D:
      reinterpret_cast<PathDataD*>(d)->release();
      return;

    case VAR_TYPE_REGION:
      reinterpret_cast<RegionData*>(d)->release();
      return;

    case VAR_TYPE_TRANSFORM_F:
    case VAR_TYPE_TRANSFORM_D:
      goto _ReleaseSimple;

    case VAR_TYPE_MATRIX_F:
      reinterpret_cast<MatrixDataF*>(d)->release();
      return;

    case VAR_TYPE_MATRIX_D:
      reinterpret_cast<MatrixDataD*>(d)->release();
      return;

    case VAR_TYPE_COLOR:
    case VAR_TYPE_PATTERN:
      reinterpret_cast<PatternData*>(d)->release();
      return;

    case VAR_TYPE_COLOR_STOP:
      goto _ReleaseSimple;

    case VAR_TYPE_COLOR_STOP_LIST:
      reinterpret_cast<ColorStopListData*>(d)->release();
      return;

    case VAR_TYPE_IMAGE:
      reinterpret_cast<ImageData*>(d)->release();
      return;

    case VAR_TYPE_IMAGE_PALETTE:
      reinterpret_cast<ImagePaletteData*>(d)->release();
      return;

    case VAR_TYPE_IMAGE_FILTER:
      reinterpret_cast<ImageFilterData*>(d)->release();
      return;

    case VAR_TYPE_FACE_INFO:
      reinterpret_cast<FaceInfoData*>(d)->release();
      return;

    case VAR_TYPE_FACE_COLLECTION:
      reinterpret_cast<FaceCollectionData*>(d)->release();
      return;

    case VAR_TYPE_FONT:
      reinterpret_cast<FontData*>(d)->release();
      return;

    default:
      return;
  }

_ReleaseSimple:
  if (AtomicCore<size_t>::deref(&d->unknown.reference))
  {
    if ((d->vType & VAR_FLAG_STATIC) == 0)
      MemMgr::free(d);
  }
  return;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Var_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.var_ctor = Var_ctor;
  fog_api.var_ctorCopy = Var_ctorCopy;
  fog_api.var_ctorType = Var_ctorType;
  fog_api.var_dtor = Var_dtor;

  fog_api.var_getReference = Var_getReference;
  fog_api.var_getVarType = Var_getVarType;

  fog_api.var_reset = Var_reset;
  fog_api.var_copy = Var_copy;

  fog_api.var_getI32 = Var_getI32;
  fog_api.var_getI32Bound = Var_getI32Bound;

  fog_api.var_getU32 = Var_getU32;
  fog_api.var_getU32Bound = Var_getU32Bound;

  fog_api.var_getI64 = Var_getI64;
  fog_api.var_getI64Bound = Var_getI64Bound;

  fog_api.var_getU64 = Var_getU64;
  fog_api.var_getU64Bound = Var_getU64Bound;

  fog_api.var_getFloat = Var_getFloat;
  fog_api.var_getFloatBound = Var_getFloatBound;

  fog_api.var_getDouble = Var_getDouble;
  fog_api.var_getDoubleBound = Var_getDoubleBound;

  fog_api.var_getType = Var_getType;
  fog_api.var_setType = Var_setType;

  fog_api.var_eq = Var_eq;
  fog_api.var_compare = Var_compare;

  fog_api.var_dCreate = Var_dCreate;
  fog_api.var_dAddRef = Var_dAddRef;
  fog_api.var_dRelease = Var_dRelease;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  VarData* d = &Var_dNull;

  d->unknown.reference = 1;
  d->vType = VAR_TYPE_NULL;

  fog_api.var_oNull = Var_oNull.initCustom1(d);
}

} // Fog namespace
