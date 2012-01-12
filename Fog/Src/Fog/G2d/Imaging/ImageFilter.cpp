// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/Filters/FeBase.h>
#include <Fog/G2d/Imaging/Filters/FeBlur.h>
#include <Fog/G2d/Imaging/Filters/FeBorder.h>
#include <Fog/G2d/Imaging/Filters/FeColorLut.h>
#include <Fog/G2d/Imaging/Filters/FeColorLutArray.h>
#include <Fog/G2d/Imaging/Filters/FeColorMatrix.h>
#include <Fog/G2d/Imaging/Filters/FeComponentFunction.h>
#include <Fog/G2d/Imaging/Filters/FeComponentTransfer.h>
#include <Fog/G2d/Imaging/Filters/FeConvolveMatrix.h>
#include <Fog/G2d/Imaging/Filters/FeConvolveSeparable.h>
#include <Fog/G2d/Imaging/Filters/FeMorphology.h>

namespace Fog {

// ============================================================================
// [Fog::ImageFilter - Global]
// ============================================================================

struct FOG_NO_EXPORT ImageFilterNullData : public ImageFilterData
{
  uint32_t _feType;
};

static Static<ImageFilterNullData> ImageFilter_dNull;
static Static<ImageFilter> ImageFilter_oNull;

// ============================================================================
// [Fog::ImageFilter - Size]
// ============================================================================

// ${FE_TYPE:BEGIN}
static const uint32_t ImageFilter_dSize[FE_TYPE_COUNT] =
{
  /* 00: FE_TYPE_NONE               */ sizeof(ImageFilterData) + sizeof(uint32_t),
  /* 01: FE_TYPE_COLOR_LUT          */ sizeof(ImageFilterData) + sizeof(FeColorLut),
  /* 02: FE_TYPE_COLOR_MATRIX       */ sizeof(ImageFilterData) + sizeof(FeColorMatrix),
  /* 03: FE_TYPE_COMPONENT_TRANSFER */ sizeof(ImageFilterData) + sizeof(FeComponentTransfer),
  /* 04: FE_TYPE_BLUR               */ sizeof(ImageFilterData) + sizeof(FeBlur),
  /* 05: FE_TYPE_CONVOLVE_MATRIX    */ sizeof(ImageFilterData) + sizeof(FeConvolveMatrix),
  /* 06: FE_TYPE_CONVOLVE_SEPARABLE */ sizeof(ImageFilterData) + sizeof(FeConvolveSeparable),
  /* 07: FE_TYPE_MORPHOLOGY         */ sizeof(ImageFilterData) + sizeof(FeMorphology)
};
// ${FE_TYPE:END}

// ============================================================================
// [Fog::ImageFilter - Construction / Destruction]
// ============================================================================

static void FOG_CDECL ImageFilter_ctor(ImageFilter* self)
{
  self->_d = ImageFilter_dNull->addRef();
}

static void FOG_CDECL ImageFilter_ctorCopy(ImageFilter* self, const ImageFilter* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL ImageFilter_dtor(ImageFilter* self)
{
  ImageFilterData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::ImageFilter - Accessors]
// ============================================================================

#define FE_GET_DATA(_Type_) \
  *reinterpret_cast<_Type_*>(feData) = *reinterpret_cast<const _Type_*>(d->getFeData());

static err_t FOG_CDECL ImageFilter_getData(const ImageFilter* self, FeBase* feData)
{
  ImageFilterData* d = self->_d;
  uint32_t type = feData->_feType;

  if (d->getFeType() != feData->_feType)
    return ERR_RT_INVALID_STATE;

  switch (type)
  {
    case FE_TYPE_NONE              : return ERR_OK;
    case FE_TYPE_COLOR_LUT         : FE_GET_DATA(FeColorLut)         ; return ERR_OK;
    case FE_TYPE_COLOR_MATRIX      : FE_GET_DATA(FeColorMatrix)      ; return ERR_OK;
    case FE_TYPE_COMPONENT_TRANSFER: FE_GET_DATA(FeComponentTransfer); return ERR_OK;
    case FE_TYPE_BLUR              : FE_GET_DATA(FeBlur)             ; return ERR_OK;
    case FE_TYPE_CONVOLVE_MATRIX   : FE_GET_DATA(FeConvolveMatrix)   ; return ERR_OK;
    case FE_TYPE_CONVOLVE_SEPARABLE: FE_GET_DATA(FeConvolveSeparable); return ERR_OK;
    case FE_TYPE_MORPHOLOGY        : FE_GET_DATA(FeMorphology)       ; return ERR_OK;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

#undef FE_GET_DATA

#define FE_SET_DATA(_Type_) \
  *reinterpret_cast<_Type_*>(d->getFeData()) = *reinterpret_cast<const _Type_*>(feData);

static err_t FOG_CDECL ImageFilter_setData(ImageFilter* self, const FeBase* feData)
{
  ImageFilterData* d = self->_d;
  uint32_t type = feData->_feType;

  if (type >= FE_TYPE_COUNT)
    return ERR_RT_INVALID_STATE;

  if (d->getFeType() != type || d->reference.get() != 1)
  {
    ImageFilterData* newd = fog_api.imagefilter_dCreate(feData);
    if (FOG_IS_NULL(newd))
      return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&self->_d, newd)->release();
    return ERR_OK;
  }

  switch (type)
  {
    case FE_TYPE_NONE              : return ERR_OK;
    case FE_TYPE_COLOR_LUT         : FE_SET_DATA(FeColorLut)         ; return ERR_OK;
    case FE_TYPE_COLOR_MATRIX      : FE_SET_DATA(FeColorMatrix)      ; return ERR_OK;
    case FE_TYPE_COMPONENT_TRANSFER: FE_SET_DATA(FeComponentTransfer); return ERR_OK;
    case FE_TYPE_BLUR              : FE_SET_DATA(FeBlur)             ; return ERR_OK;
    case FE_TYPE_CONVOLVE_MATRIX   : FE_SET_DATA(FeConvolveMatrix)   ; return ERR_OK;
    case FE_TYPE_CONVOLVE_SEPARABLE: FE_SET_DATA(FeConvolveSeparable); return ERR_OK;
    case FE_TYPE_MORPHOLOGY        : FE_SET_DATA(FeMorphology)       ; return ERR_OK;

    default:
      return ERR_RT_INVALID_STATE;
  }
}

#undef FE_SET_DATA

// ============================================================================
// [Fog::ImageFilter - Reset]
// ============================================================================

static void FOG_CDECL ImageFilter_reset(ImageFilter* self)
{
  atomicPtrXchg(&self->_d, ImageFilter_dNull->addRef())->release();
}

// ============================================================================
// [Fog::ImageFilter - Copy]
// ============================================================================

static err_t FOG_CDECL ImageFilter_copy(ImageFilter* self, const ImageFilter* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
  return ERR_OK;
}


// ============================================================================
// [Fog::ImageFilter - Equality]
// ============================================================================

#define FOG_FE_EQ(_Type_) ( *reinterpret_cast<const _Type_*>(a_d->getFeData()) == \
                            *reinterpret_cast<const _Type_*>(b_d->getFeData()) )

static bool FOG_CDECL ImageFilter_eq(const ImageFilter* a, const ImageFilter* b)
{
  const ImageFilterData* a_d = a->_d;
  const ImageFilterData* b_d = b->_d;

  uint32_t type = a_d->getFeType();
  if (type != b_d->getFeType())
    return false;

  switch (type)
  {
    case FE_TYPE_NONE              : return true;
    case FE_TYPE_COLOR_LUT         : return FOG_FE_EQ(FeColorLut);
    case FE_TYPE_COLOR_MATRIX      : return FOG_FE_EQ(FeColorMatrix);
    case FE_TYPE_COMPONENT_TRANSFER: return FOG_FE_EQ(FeComponentTransfer);
    case FE_TYPE_BLUR              : return FOG_FE_EQ(FeBlur);
    case FE_TYPE_CONVOLVE_MATRIX   : return FOG_FE_EQ(FeConvolveMatrix);
    case FE_TYPE_CONVOLVE_SEPARABLE: return FOG_FE_EQ(FeConvolveSeparable);
    case FE_TYPE_MORPHOLOGY        : return FOG_FE_EQ(FeMorphology);
    default                        : return false;
  }
}

#undef FOG_FE_EQ

// ============================================================================
// [Fog::ImageFilter - ImageFilterData]
// ============================================================================

#define FOG_FE_CREATE(_Type_) \
  FOG_ASSERT(dSize == sizeof(ImageFilterData) + sizeof(_Type_)); \
  fog_new_p(d->getFeData()) _Type_(*reinterpret_cast<const _Type_*>(feData))

static ImageFilterData* FOG_CDECL ImageFilter_dCreate(const FeBase* feData)
{
  uint32_t type = feData->_feType;

  if (type == FE_TYPE_NONE || type >= FE_TYPE_COUNT)
    return ImageFilter_dNull->addRef();

  uint32_t dSize = ImageFilter_dSize[type];
  ImageFilterData* d = reinterpret_cast<ImageFilterData*>(MemMgr::alloc(dSize));

  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->vType = VAR_TYPE_IMAGE_FILTER | VAR_FLAG_NONE;
  FOG_PADDING_ZERO_64(d->padding0_32);

  switch (feData->_feType)
  {
    case FE_TYPE_COLOR_LUT         : FOG_FE_CREATE(FeColorLut)         ; break;
    case FE_TYPE_COLOR_MATRIX      : FOG_FE_CREATE(FeColorMatrix)      ; break;
    case FE_TYPE_COMPONENT_TRANSFER: FOG_FE_CREATE(FeComponentTransfer); break;
    case FE_TYPE_BLUR              : FOG_FE_CREATE(FeBlur)             ; break;
    case FE_TYPE_CONVOLVE_MATRIX   : FOG_FE_CREATE(FeConvolveMatrix)   ; break;
    case FE_TYPE_CONVOLVE_SEPARABLE: FOG_FE_CREATE(FeConvolveSeparable); break;
    case FE_TYPE_MORPHOLOGY        : FOG_FE_CREATE(FeMorphology)       ; break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return d;
}

#undef FOG_FE_CREATE

#define FOG_FE_FREE(_Type_) \
  reinterpret_cast<_Type_*>(d->getFeData())->~_Type_()

static void FOG_CDECL ImageFilter_dFree(ImageFilterData* d)
{
  switch (d->getFeType())
  {
    case FE_TYPE_NONE: break;
    case FE_TYPE_COLOR_LUT         : FOG_FE_FREE(FeColorLut)         ; break;
    case FE_TYPE_COLOR_MATRIX      : FOG_FE_FREE(FeColorMatrix)      ; break;
    case FE_TYPE_COMPONENT_TRANSFER: FOG_FE_FREE(FeComponentTransfer); break;
    case FE_TYPE_BLUR              : FOG_FE_FREE(FeBlur)             ; break;
    case FE_TYPE_CONVOLVE_MATRIX   : FOG_FE_FREE(FeConvolveMatrix)   ; break;
    case FE_TYPE_CONVOLVE_SEPARABLE: FOG_FE_FREE(FeConvolveSeparable); break;
    case FE_TYPE_MORPHOLOGY        : FOG_FE_FREE(FeMorphology)       ; break;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  if ((d->vType & VAR_FLAG_STATIC) == 0)
    MemMgr::free(d);
}

#undef FOG_FE_FREE

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImageFilter_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.imagefilter_ctor = ImageFilter_ctor;
  fog_api.imagefilter_ctorCopy = ImageFilter_ctorCopy;
  fog_api.imagefilter_dtor = ImageFilter_dtor;

  fog_api.imagefilter_getData = ImageFilter_getData;
  fog_api.imagefilter_setData = ImageFilter_setData;

  fog_api.imagefilter_reset = ImageFilter_reset;
  fog_api.imagefilter_copy = ImageFilter_copy;
  fog_api.imagefilter_eq = ImageFilter_eq;

  fog_api.imagefilter_dCreate = ImageFilter_dCreate;
  fog_api.imagefilter_dFree = ImageFilter_dFree;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  ImageFilterNullData* d = &ImageFilter_dNull;

  d->reference.init(1);
  d->vType = VAR_TYPE_IMAGE_FILTER | VAR_FLAG_NONE;
  d->_feType = FE_TYPE_NONE;

  ImageFilter_oNull.initCustom1(d);
}

} // Fog namespace
