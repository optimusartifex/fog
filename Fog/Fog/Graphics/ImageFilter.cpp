// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Strings.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/ColorMatrix.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/ImageFilterEngine.h>
#include <Fog/Graphics/ImageFilterParams.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog {

// ============================================================================
// [Fog::NullFilterEngine]
// ============================================================================

struct FOG_HIDDEN NullFilterEngine : public ImageFilterEngine
{
  NullFilterEngine();

  virtual bool isNop() const;
  virtual ImageFilterEngine* clone() const;

private:
  FOG_DISABLE_COPY(NullFilterEngine)
};

NullFilterEngine::NullFilterEngine() :
  ImageFilterEngine(IMAGE_FILTER_TYPE_NONE)
{
}

bool NullFilterEngine::isNop() const
{
  return true;
}

ImageFilterEngine* NullFilterEngine::clone() const
{
  refCount.inc();
  return const_cast<NullFilterEngine*>(this);
}

static Static<NullFilterEngine> nullFilterEngine;

// ============================================================================
// [Fog::ColorLutFilterEngine]
// ============================================================================

struct FOG_HIDDEN ColorLutFilterEngine : public ImageFilterEngine
{
  ColorLutFilterEngine(const ColorLutData& lut);

  virtual ColorFilterFn getColorFilterFn(uint32_t format) const;
  virtual const void* getContext() const;

  virtual ImageFilterEngine* clone() const;

  void setColorLut(const ColorLutData& lut);

  ColorLutData lut;

private:
  FOG_DISABLE_COPY(ColorLutFilterEngine)
};

ColorLutFilterEngine::ColorLutFilterEngine(const ColorLutData& lutData) :
  ImageFilterEngine(IMAGE_FILTER_TYPE_COLOR_LUT)
{
  setColorLut(lutData);

  characteristics |=
    IMAGE_FILTER_CHAR_SUPPORTS_ARGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_XRGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_A8     ;
}

ColorFilterFn ColorLutFilterEngine::getColorFilterFn(uint32_t format) const
{
  if (format >= IMAGE_FORMAT_COUNT) return NULL;
  return (ColorFilterFn)rasterFuncs.filter.color_lut[format];
}

const void* ColorLutFilterEngine::getContext() const
{
  return reinterpret_cast<const void*>(&lut);
}

ImageFilterEngine* ColorLutFilterEngine::clone() const
{
  return new(std::nothrow) ColorLutFilterEngine(lut);
}

void ColorLutFilterEngine::setColorLut(const ColorLutData& lutData)
{
  Memory::copy(&lut, &lutData, sizeof(ColorLutData));
}

// ============================================================================
// [Fog::ColorMatrixFilterEngine]
// ============================================================================

struct FOG_HIDDEN ColorMatrixFilterEngine : public ImageFilterEngine
{
  ColorMatrixFilterEngine(const ColorMatrixFilterEngine& other);
  ColorMatrixFilterEngine(const ColorMatrix& cm);

  virtual ColorFilterFn getColorFilterFn(uint32_t format) const;
  virtual const void* getContext() const;

  virtual ImageFilterEngine* clone() const;

  void setColorMatrix(const ColorMatrix& other);

  ColorMatrix cm;
};

ColorMatrixFilterEngine::ColorMatrixFilterEngine(const ColorMatrixFilterEngine& other) :
  ImageFilterEngine(IMAGE_FILTER_TYPE_COLOR_MATRIX),
  cm(other.cm)
{
  characteristics |=
    IMAGE_FILTER_CHAR_SUPPORTS_PRGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_ARGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_XRGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_A8     ;
}

ColorMatrixFilterEngine::ColorMatrixFilterEngine(const ColorMatrix& cm) :
  ImageFilterEngine(IMAGE_FILTER_TYPE_COLOR_MATRIX),
  cm(cm)
{
  characteristics |=
    IMAGE_FILTER_CHAR_SUPPORTS_PRGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_ARGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_XRGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_A8     ;
}

ColorFilterFn ColorMatrixFilterEngine::getColorFilterFn(uint32_t format) const
{
  if (format >= IMAGE_FORMAT_COUNT) return NULL;
  return (ColorFilterFn)rasterFuncs.filter.color_matrix[format];
}

const void* ColorMatrixFilterEngine::getContext() const
{
  return reinterpret_cast<const void*>(&cm);
}

ImageFilterEngine* ColorMatrixFilterEngine::clone() const
{
  return new(std::nothrow) ColorMatrixFilterEngine(*this);
}

void ColorMatrixFilterEngine::setColorMatrix(const ColorMatrix& other)
{
  cm = other;
}

// ============================================================================
// [Fog::BlurFilterEngine]
// ============================================================================

// This function is designed to make kernel for gaussian blur matrix. See
// wikipedia (http://en.wikipedia.org/wiki/Gaussian_function) for equations.
static void makeGaussianBlurKernel(List<float>& dst_, double radius)
{
  if (radius <= 0.63) { dst_.clear(); return; }

  int i, pos;
  int size = (int)floor(radius) * 2 + 1;

  double sigma = radius / 3.0;
  double sigma2 = sigma * sigma;

  // Reciprocals.
  double re = 1.0 / (2.0 * sigma2);
  double rs = 1.0 / Math::sqrt(2.0 * M_PI * sigma);

  double total = 0;

  if (dst_.resize(size) != ERR_OK) return;
  float* dst = dst_.getXData();

  for (i = 0, pos = -(int)ceil(radius); i < size; i++, pos++)
  {
    double dist = (double)pos * (double)pos;
    double val = exp(-dist * re) * rs;
    if (val < 0.0) val = 0.0;
    total += val;
    dst[i] = (float)val;
  }

  // Error correction.
  if (total >= 0.001) total -= 0.001;

  // Normalize (multiply by reciprocal).
  total = 1.0 / total;
  for (i = 0; i < size; i++) dst[i] *= (float)total;
}

struct FOG_HIDDEN BlurFilterEngine : public ImageFilterEngine
{
  BlurFilterEngine(const BlurParams& params);

  virtual ImageFilterFn getImageFilterFn(uint32_t format, uint32_t processing) const;
  virtual const void* getContext() const;

  virtual ImageFilterEngine* clone() const;

  void setParams(const BlurParams& params);
  void update();

  BlurParams params;
  FloatSymmetricConvolveParams convolve;

private:
  FOG_DISABLE_COPY(BlurFilterEngine)
};

BlurFilterEngine::BlurFilterEngine(const BlurParams& params) :
  ImageFilterEngine(IMAGE_FILTER_TYPE_COLOR_LUT), params(params)
{
  update();
}

ImageFilterFn BlurFilterEngine::getImageFilterFn(uint32_t format, uint32_t processing) const
{
  if (format >= IMAGE_FORMAT_COUNT) return NULL;

  switch (params.blur)
  {
    case IMAGE_FILTER_BLUR_BOX:
      if (processing == IMAGE_FILTER_CHAR_HORZ_PROCESSING) return (ImageFilterFn)rasterFuncs.filter.box_blur_h[format];
      if (processing == IMAGE_FILTER_CHAR_VERT_PROCESSING) return (ImageFilterFn)rasterFuncs.filter.box_blur_v[format];
      break;

    case IMAGE_FILTER_BLUR_LINEAR:
      if (processing == IMAGE_FILTER_CHAR_HORZ_PROCESSING) return (ImageFilterFn)rasterFuncs.filter.linear_blur_h[format];
      if (processing == IMAGE_FILTER_CHAR_VERT_PROCESSING) return (ImageFilterFn)rasterFuncs.filter.linear_blur_v[format];
      break;

    case IMAGE_FILTER_BLUR_GAUSSIAN:
    default:
      if (processing == IMAGE_FILTER_CHAR_HORZ_PROCESSING) return (ImageFilterFn)rasterFuncs.filter.symmetric_convolve_float_h[format];
      if (processing == IMAGE_FILTER_CHAR_VERT_PROCESSING) return (ImageFilterFn)rasterFuncs.filter.symmetric_convolve_float_v[format];
      break;
  }

  return NULL;
}

const void* BlurFilterEngine::getContext() const
{
  switch (params.blur)
  {
    case IMAGE_FILTER_BLUR_BOX:
    case IMAGE_FILTER_BLUR_LINEAR:
      return reinterpret_cast<const void*>(&params);
      return reinterpret_cast<const void*>(&params);

    case IMAGE_FILTER_BLUR_GAUSSIAN:
    default:
      return reinterpret_cast<const void*>(&convolve);
  }
}

ImageFilterEngine* BlurFilterEngine::clone() const
{
  return new(std::nothrow) BlurFilterEngine(params);
}

void BlurFilterEngine::setParams(const BlurParams& params)
{
  this->params = params;
  update();
}

void BlurFilterEngine::update()
{
  params.hRadius = Math::min<float>(Math::abs(params.hRadius), IMAGE_FILTER_BLUR_MAX_RADIUS);
  params.vRadius = Math::min<float>(Math::abs(params.vRadius), IMAGE_FILTER_BLUR_MAX_RADIUS);

  characteristics = IMAGE_FILTER_CHAR_CAN_EXTEND;

  switch (params.blur)
  {
    case IMAGE_FILTER_BLUR_BOX:
    case IMAGE_FILTER_BLUR_LINEAR:
      if (params.hRadius >= 1.0) characteristics |= IMAGE_FILTER_CHAR_HORZ_PROCESSING | IMAGE_FILTER_CHAR_HORZ_MEM_EQUAL;
      if (params.vRadius >= 1.0) characteristics |= IMAGE_FILTER_CHAR_VERT_PROCESSING | IMAGE_FILTER_CHAR_VERT_MEM_EQUAL;

      convolve.hMatrix.free();
      convolve.vMatrix.free();
      break;

    case IMAGE_FILTER_BLUR_GAUSSIAN:
    default:
      if (params.hRadius > 0.63) characteristics |= IMAGE_FILTER_CHAR_HORZ_PROCESSING | IMAGE_FILTER_CHAR_HORZ_MEM_EQUAL;
      if (params.vRadius > 0.63) characteristics |= IMAGE_FILTER_CHAR_VERT_PROCESSING | IMAGE_FILTER_CHAR_VERT_MEM_EQUAL;

      makeGaussianBlurKernel(convolve.hMatrix, params.hRadius);
      makeGaussianBlurKernel(convolve.vMatrix, params.vRadius);
      
      // Sync convolve params.
      convolve.borderExtend = params.borderExtend;
      convolve.borderColor = params.borderColor;
      break;
  }

  // Supported pixel formats - Note there is not PRGB32.
  characteristics |=
    IMAGE_FILTER_CHAR_SUPPORTS_ARGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_XRGB32 |
    IMAGE_FILTER_CHAR_SUPPORTS_A8     ;
}

// ============================================================================
// [Fog::ImageFilterBase]
// ============================================================================

ImageFilterBase::ImageFilterBase() :
  _d(nullFilterEngine->ref())
{
}

ImageFilterBase::ImageFilterBase(const ImageFilterBase& other) :
  _d(other._d->ref())
{
}

ImageFilterBase::ImageFilterBase(const ColorMatrix& colorMatrix) :
  _d(nullFilterEngine->ref())
{
  setColorMatrix(colorMatrix);
}

ImageFilterBase::ImageFilterBase(const ColorLut& colorLut) :
  _d(nullFilterEngine->ref())
{
  setColorLut(colorLut);
}

ImageFilterBase::ImageFilterBase(const ImageFilterEngine* engine) :
  _d(const_cast<ImageFilterEngine*>(engine))
{
}

ImageFilterBase::~ImageFilterBase()
{
  _d->deref();
}

err_t ImageFilterBase::getColorLut(ColorLut& colorLut) const
{
  if (_d->type != IMAGE_FILTER_TYPE_COLOR_LUT) return ERR_RT_INVALID_OBJECT;

  return colorLut.setData(&reinterpret_cast<ColorLutFilterEngine*>(_d)->lut);
}

err_t ImageFilterBase::getColorMatrix(ColorMatrix& colorMatrix) const
{
  if (_d->type != IMAGE_FILTER_TYPE_COLOR_MATRIX) return ERR_RT_INVALID_OBJECT;

  colorMatrix = reinterpret_cast<ColorMatrixFilterEngine*>(_d)->cm;
  return ERR_OK;
}

err_t ImageFilterBase::getBlur(BlurParams& params) const
{
  if (_d->type != IMAGE_FILTER_TYPE_BLUR) return ERR_RT_INVALID_OBJECT;

  params = reinterpret_cast<BlurFilterEngine*>(_d)->params;
  return ERR_OK;
}

err_t ImageFilterBase::setColorLut(const ColorLut& colorLut)
{
  if (_d->type == IMAGE_FILTER_TYPE_COLOR_LUT && _d->refCount.get() == 1)
  {
    // Not needed to create new ColorLutFilterEngine instance.
    reinterpret_cast<ColorLutFilterEngine *>(_d)->setColorLut(*colorLut.getData());
    return ERR_OK;
  }
  else
  {
    ImageFilterEngine* e = new(std::nothrow) ColorLutFilterEngine(*colorLut.getData());
    if (e == NULL) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, e)->deref();
    return ERR_OK;
  }
}

err_t ImageFilterBase::setColorMatrix(const ColorMatrix& colorMatrix)
{
  if (_d->type == IMAGE_FILTER_TYPE_COLOR_LUT && _d->refCount.get() == 1)
  {
    // Not needed to create new ColorMatrixFilterEngine instance.
    reinterpret_cast<ColorMatrixFilterEngine *>(_d)->setColorMatrix(colorMatrix);
    return ERR_OK;
  }
  else
  {
    ImageFilterEngine* e = new(std::nothrow) ColorMatrixFilterEngine(colorMatrix);
    if (e == NULL) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, e)->deref();
    return ERR_OK;
  }
}

err_t ImageFilterBase::setBlur(const BlurParams& params)
{
  if (_d->type == IMAGE_FILTER_TYPE_BLUR && _d->refCount.get() == 1)
  {
    // Not needed to create new BlurFilterEngine instance.
    reinterpret_cast<BlurFilterEngine *>(_d)->setParams(params);
    return ERR_OK;
  }
  else
  {
    ImageFilterEngine* e = new(std::nothrow) BlurFilterEngine(params);
    if (e == NULL) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, e)->deref();
    return ERR_OK;
  }
}

err_t ImageFilterBase::setOther(const ImageFilterBase& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_imagefilter_init(void)
{
  using namespace Fog;
  nullFilterEngine.init();

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_imagefilter_shutdown(void)
{
  using namespace Fog;
  nullFilterEngine.destroy();
}
