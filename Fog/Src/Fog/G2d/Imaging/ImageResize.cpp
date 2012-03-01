// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccC.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Function.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageResize_p.h>

namespace Fog {

// ============================================================================
// [Fog::ImageResize - Api]
// ============================================================================

static ImageResizeApi ImageResize_api;

// ============================================================================
// [Fog::ImageResize - Function - Nearest]
// ============================================================================

// Radius == 1.0
struct FOG_NO_EXPORT ImageResize_NearestFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t <= 0.5f)
      *dst = 1.0f;
    else
      *dst = 0.0f;

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Bilinear]
// ============================================================================

// Radius == 1.0
struct FOG_NO_EXPORT ImageResize_BilinearFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t < 1.0f)
      *dst = 1.0f - t;
    else
      *dst = 0.0f;   

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Bicubic]
// ============================================================================

// Radius == 2.0
struct FOG_NO_EXPORT ImageResize_BicubicFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    // 0.5t^3 - t^2 + 2/3 == (0.5t - 1.0) t^2 + 2/3
    if (t < 1.0f) 
      *dst = (0.5f * t - 1.0f) * Math::pow2(t) + 2.0f / 3.0f;
    else if (t < 2.0f)
      *dst = Math::pow3(2.0f - t) / 6.0f;
    else
      *dst = 0.0f;

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Bell]
// ============================================================================

// Radius == 1.5
struct FOG_NO_EXPORT ImageResize_BellFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t < 0.5f)
      *dst = 0.75f - t * t;
    else if (t < 1.5f)
      *dst = 0.5f * Math::pow2(t - 1.5f);
    else
      *dst = 0.0f;

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Gauss]
// ============================================================================

// Radius == 2.0
struct FOG_NO_EXPORT ImageResize_GaussFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t <= 2.0f)
      *dst = Math::exp(Math::pow2(t) * -2.0f) * Math::sqrt(float(MATH_2_DIV_PI));
    else
      *dst = 0.0f;  

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Hermite]
// ============================================================================

// Radius == 1.0
struct FOG_NO_EXPORT ImageResize_HermiteFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t < 1.0f)
      *dst = (2.0f * t - 3.0f) * Math::pow2(t) + 1.0f;
    else
      *dst = 0.0f;  

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Hanning]
// ============================================================================

// Radius == 1.0
struct FOG_NO_EXPORT ImageResize_HanningFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t <= 1.0f)
      *dst = 0.5f + 0.5f * Math::cos(t * float(MATH_PI));
    else
      *dst = 0.0f;
    
    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Catrom]
// ============================================================================

// Radius == 2.0
struct FOG_NO_EXPORT ImageResize_CatromFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t < 1.0f)
      *dst = 0.5f * (2.0f + t * t * (-5.0f + t * 3.0f));
    if (t < 2.0f)
      *dst = 0.5f * (4.0f + t * (-8.0f + t * (5.0f - t)));
    else
      *dst = 0.0f;

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Mitchell]
// ============================================================================

// Radius == 2.0
struct FOG_NO_EXPORT ImageResize_MitchellFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageResize_MitchellFunction() :
    b(float(MATH_1_DIV_3)),
    c(float(MATH_1_DIV_3))
  {
    init();
  }

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  FOG_NO_INLINE void init()
  {
    p0 = float( 1.0) - float(MATH_1_DIV_3) * b;
    p2 = float(-3.0) + float(2.0         ) * b + c;
    p3 = float( 2.0) - float(1.5         ) * b - c;

    q0 = float( MATH_4_DIV_3) * b + 4.0f * c;
    q1 = float(-2.0         ) * b - 8.0f * c;
    q2 =                        b + 5.0f * c;
    q3 = float(-MATH_1_DIV_6) * b -        c;
  }

  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t < 1.0f)
      *dst = p0 + t * t * (p2 + t * p3);
    if (t < 2.0f)
      *dst = q0 + t * (q1 + t * (q2 + t * q3));
    else
      *dst = 0.0f;

    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float b, c;

  float p0, p2, p3;
  float q0, q1, q2, q3;
};

// ============================================================================
// [Fog::ImageResize - Function - Bessel]
// ============================================================================

// Radius == 3.2383
struct FOG_NO_EXPORT ImageResize_BesselFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t == 0.0f)
      *dst = float(MATH_PI * 0.25);
    else if (t <= 3.2383f)
      *dst = Math::besj(float(MATH_PI) * t, 1) / (2.0f * t);
    else
      *dst = 0.0f;

    return ERR_OK;
  }
};

// ============================================================================
// [Fog::ImageResize - Function - Sinc]
// ============================================================================

// Radius == User Defined.
struct FOG_NO_EXPORT ImageResize_SincFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageResize_SincFunction() :
    radius(2.0f)
  {
  }

  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t == 0.0f)
    {
      *dst = 1.0f;
    }
    else if (t <= radius)
    {
      float x = t * float(MATH_PI);
      *dst = Math::sin(x) / x;
    }
    else
    {
      *dst = 0.0f;
    }
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float radius;
};

// ============================================================================
// [Fog::ImageResize - Function - Lanczos]
// ============================================================================

// Radius == User Defined.
struct FOG_NO_EXPORT ImageResize_LanczosFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageResize_LanczosFunction() :
    radius(2.0f)
  {
  }

  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t == 0.0f)
    {
      *dst = 1.0f;
    }
    else if (t <= radius)
    {
      float x = t * float(MATH_PI);
      float y = x / radius;
      *dst = (Math::sin(x) / x) * (Math::sin(y) / y);
    }
    else
    {
      *dst = 0.0f;
    }
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float radius;
};

// ============================================================================
// [Fog::ImageResize - Function - Blackman]
// ============================================================================

// Radius == User Defined.
struct FOG_NO_EXPORT ImageResize_BlackmanFunction : public MathFunctionF
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ImageResize_BlackmanFunction() :
    radius(2.0f)
  {
  }

  // --------------------------------------------------------------------------
  // [Override]
  // --------------------------------------------------------------------------

  virtual err_t evaluate(float* dst, float t) const
  {
    if (t == 0.0f)
    {
      *dst = 1.0f;
    }
    else if (t <= radius)
    {
      float x = t * float(MATH_PI);
      float y = x / radius;
      *dst = (Math::sin(x) / x) * (0.42f + 0.5f * Math::cos(y) + 0.08f * Math::cos(y * 2.0f));
    }
    else
    {
      *dst = 0.0f;
    }
    return ERR_OK;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  float radius;
};

// ============================================================================
// [Fog::ImageResize - Context - Init / Destroy]
// ============================================================================

static err_t FOG_CDECL ImageResizeContext_init(ImageResizeContext* ctx,
  uint8_t* dData, size_t dStride, int dw, int dh,
  uint8_t* sData, size_t sStride, int sw, int sh,
  uint32_t format,
  const MathFunctionF* func, float radius)
{
  uint32_t tmpBpp = ImageFormatDescription::getByFormat(format).getBytesPerPixel();

  if (format == IMAGE_FORMAT_I8)
    tmpBpp = 4;

  ctx->dData = dData;
  ctx->sData = sData;

  ctx->dStride = dStride;
  ctx->sStride = sStride;
  ctx->tStride = (size_t)(dw * tmpBpp + 15) & ~(size_t)15;

  ctx->dSize[0] = (uint)dw;
  ctx->dSize[1] = (uint)dh;

  ctx->sSize[0] = (uint)sw;
  ctx->sSize[1] = (uint)sh;

  ctx->scale[0] = float(dw) / float(sw);
  ctx->scale[1] = float(dh) / float(sh);

  ctx->factor[0] = 1.0f;
  ctx->factor[1] = 1.0f;

  ctx->radius[0] = radius;
  ctx->radius[1] = radius;

  if (ctx->scale[0] < 1.0f) { ctx->factor[0] = ctx->scale[0]; ctx->radius[0] = radius / ctx->scale[0]; }
  if (ctx->scale[1] < 1.0f) { ctx->factor[1] = ctx->scale[1]; ctx->radius[1] = radius / ctx->scale[1]; }

  ctx->kernelSize[0] = (int)(1.0f + 2.0f * ctx->radius[0]);
  ctx->kernelSize[1] = (int)(1.0f + 2.0f * ctx->radius[1]);

  ctx->isBound[0] = false;
  ctx->isBound[1] = false;

  ctx->func = func;

  {
    size_t hWeightSize = dw * ctx->kernelSize[0] * sizeof(int32_t);
    size_t vWeightSize = dh * ctx->kernelSize[1] * sizeof(int32_t);

    size_t hRecordSize = dw * sizeof(ImageResizeRecord);
    size_t vRecordSize = dh * sizeof(ImageResizeRecord);

    // Allocate the maximum possible space needed.
    ctx->weightList = reinterpret_cast<int32_t          *>(MemMgr::alloc(Math::max(hWeightSize, vWeightSize)));
    ctx->recordList = reinterpret_cast<ImageResizeRecord*>(MemMgr::alloc(Math::max(hRecordSize, vRecordSize)));
    ctx->tData      = reinterpret_cast<uint8_t          *>(MemMgr::alloc(sh * ctx->tStride));

    if (ctx->weightList == NULL || ctx->recordList == NULL || ctx->tData == NULL)
    {
      ImageResize_api.destroy(ctx);
      return ERR_RT_OUT_OF_MEMORY;
    }
  }

  return ERR_OK;
}

static void FOG_CDECL ImageResizeContext_destroy(ImageResizeContext* ctx)
{
  if (ctx->tData     ) MemMgr::free(ctx->tData     );
  if (ctx->recordList) MemMgr::free(ctx->recordList);
  if (ctx->weightList) MemMgr::free(ctx->weightList);
}

// ============================================================================
// [Fog::ImageResize - Context - Calc Weights]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doWeights(ImageResizeContext* ctx, uint dir)
{
  int32_t* weightList = ctx->weightList;
  ImageResizeRecord* recordList = ctx->recordList;

  uint dSize = ctx->dSize[dir];
  uint sSizeM1 = ctx->sSize[dir] - 1;
  uint isSubtracted = 0;

  float radius = ctx->radius[dir];
  float radius2 = radius * 2;

  float factor = ctx->factor[dir];
  const MathFunctionF* func = ctx->func;

  for (uint i = 0; i < dSize; i++)
  {
    float* wData = reinterpret_cast<float*>(weightList);
    float wSum = 0.0f;

    float center = ((float)(int)i + 0.5f) / ctx->scale[dir] - 0.5f;
    int left = (int)(center - radius);
    int right = (int)(left + radius2) + 1;

    uint wCount = 0;
    int j = left;

    if (j < 0)
    {
      float weight = 0.0f;

      do {
        float w;
        func->evaluate(&w, Math::abs((center - j) * factor));
        weight += w;
      } while (++j <= 0 && j < right);

      left = (weight == 0.0f);
      if (left == 0)
      {
        wData[wCount++] = weight;
        wSum += weight;
        isSubtracted |= (weight < 0.0f);
      }
    }

    int limit = Math::min<int>(sSizeM1, right);
    for (; j < limit; j++)
    {
      float weight;
      func->evaluate(&weight, Math::abs((center - j) * factor));

      if (wCount == 0)
      {
        left = j;
        if (weight == 0.0f)
          continue;
      }

      wData[wCount++] = weight;
      wSum += weight;
      isSubtracted |= (weight < 0.0f);
    }

    if (j < right)
    {
      if (wCount == 0)
        left = sSizeM1;

      float weight = 0.0f;
      do {
        float w;
        func->evaluate(&w, Math::abs((center - j) * factor));
        weight += w;
      } while (++j < right);

      if (weight != 0.0f)
      {
        wData[wCount++] = weight;
        wSum += weight;
        isSubtracted |= (weight < 0.0f);
      }
    }

    if (wCount != 0)
    {
      int32_t iSum = 0;
      int32_t iMax = 0;

      uint k;
      uint kMax = 0;

      for (k = 0; k < wCount; k++)
      {
        int32_t w = (int32_t)(wData[k] * 65536.0f / wSum) >> 8;
        weightList[k] = w;
        iSum += w;

        if (iMax <= w)
        {
          iMax = w;
          kMax = k;
        }
      }

      // Normalize weights, adding/subtracting the normalization value into
      // the strongest one.
      if (iSum != 0x100)
      {
        weightList[kMax] += (int32_t)0x100 - iSum;
      }

      // Remove all zero weights from the end of the weight array.
      k = wCount;
      do {
        if (weightList[--k] == 0)
          wCount = k;
        else
          break;
      } while (k != 0);

      // Remove all zero weights from the beginning of the array.
      k = 0;
      while (k < wCount)
      {
        if (weightList[k] != 0)
          break;
        k++;
      }

      if (k > 0)
      {
        left += k;
        wCount -= k;

        for (uint x = 0; x < wCount; x++)
          weightList[x] = weightList[x+k];
      }

      recordList[i].pos = left;
      recordList[i].count = wCount;
    }
    else
    {
      recordList[i].pos = 0;
      recordList[i].count = 0;
    }

    weightList += ctx->kernelSize[dir];
  }

  ctx->isBound[dir] = !isSubtracted;
}

// ============================================================================
// [Fog::ImageResize - Context - DoHorizontal - PRGB32]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doHorizontal_PRGB32(ImageResizeContext* ctx)
{
  uint kernelSize = ctx->kernelSize[0];

  uint dw = ctx->dSize[0];
  uint sh = ctx->sSize[1];

  uint8_t* sData = ctx->sData;
  uint8_t* tData = ctx->tData;

  ssize_t sStride = ctx->sStride;
  ssize_t tStride = ctx->tStride;

  if (ctx->isBound[0] == 1)
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        uint32_t cr_cb = 0x00800080;
        uint32_t ca_cg = 0x00800080;

        for (uint j = recordList->count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(sp)[0];
          uint32_t w0 = wp[0];

          ca_cg += ((p0 >> 8) & 0x00FF00FF) * w0;
          cr_cb += ((p0     ) & 0x00FF00FF) * w0;

          sp += 4;
          wp += 1;
        }

        reinterpret_cast<uint32_t*>(tp)[0] = _FOG_ACC_COMBINE_2(ca_cg & 0xFF00FF00, (cr_cb & 0xFF00FF00) >> 8);

        recordList += 1;
        weightList += kernelSize;

        tp += 4;
      }

      sData += sStride;
      tData += tStride;
    }
  }
  else
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        int32_t ca = 0x80;
        int32_t cr = 0x80;
        int32_t cg = 0x80;
        int32_t cb = 0x80;

        for (uint j = recordList->count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(sp)[0];
          int32_t w0 = wp[0];

          ca += (int32_t)Argb32::getAlpha(p0) * w0;
          cr += (int32_t)Argb32::getRed  (p0) * w0;
          cg += (int32_t)Argb32::getGreen(p0) * w0;
          cb += (int32_t)Argb32::getBlue (p0) * w0;

          sp += 4;
          wp += 1;
        }

        ca = Math::bound<int32_t>(ca >> 8, 0, 255);
        cr = Math::bound<int32_t>(cr >> 8, 0, ca);
        cg = Math::bound<int32_t>(cg >> 8, 0, ca);
        cb = Math::bound<int32_t>(cb >> 8, 0, ca);
        reinterpret_cast<uint32_t*>(tp)[0] = Argb32(ca, cr, cg, cb);

        recordList += 1;
        weightList += kernelSize;

        tp += 4;
      }

      sData += sStride;
      tData += tStride;
    }
  }
}

// ============================================================================
// [Fog::ImageResize - Context - DoHorizontal - XRGB32]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doHorizontal_XRGB32(ImageResizeContext* ctx)
{
  uint kernelSize = ctx->kernelSize[0];

  uint dw = ctx->dSize[0];
  uint sh = ctx->sSize[1];

  uint8_t* sData = ctx->sData;
  uint8_t* tData = ctx->tData;

  ssize_t sStride = ctx->sStride;
  ssize_t tStride = ctx->tStride;

  if (ctx->isBound[0] == 1)
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        uint32_t cx_cg = 0x00008000;
        uint32_t cr_cb = 0x00800080;

        for (uint j = recordList->count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(sp)[0];
          uint32_t w0 = wp[0];

          cx_cg += (p0 & 0x0000FF00) * w0;
          cr_cb += (p0 & 0x00FF00FF) * w0;

          sp += 4;
          wp += 1;
        }

        reinterpret_cast<uint32_t*>(tp)[0] = _FOG_ACC_COMBINE_2(0xFF000000, ((cx_cg & 0x00FF0000) | (cr_cb & 0xFF00FF00)) >> 8);

        recordList += 1;
        weightList += kernelSize;

        tp += 4;
      }

      sData += sStride;
      tData += tStride;
    }
  }
  else
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        int32_t cr = 0x80;
        int32_t cg = 0x80;
        int32_t cb = 0x80;

        for (uint j = recordList->count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(sp)[0];
          int32_t w0 = wp[0];

          cr += (int32_t)Argb32::getRed  (p0) * w0;
          cg += (int32_t)Argb32::getGreen(p0) * w0;
          cb += (int32_t)Argb32::getBlue (p0) * w0;

          sp += 4;
          wp += 1;
        }

        cr = Math::bound<int32_t>(cr >> 8, 0, 255);
        cg = Math::bound<int32_t>(cg >> 8, 0, 255);
        cb = Math::bound<int32_t>(cb >> 8, 0, 255);
        reinterpret_cast<uint32_t*>(tp)[0] = Argb32(0xFF, cr, cg, cb);

        recordList += 1;
        weightList += kernelSize;

        tp += 4;
      }

      sData += sStride;
      tData += tStride;
    }
  }
}

// ============================================================================
// [Fog::ImageResize - Context - DoHorizontal - RGB24]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doHorizontal_RGB24(ImageResizeContext* ctx)
{
  uint kernelSize = ctx->kernelSize[0];

  uint dw = ctx->dSize[0];
  uint sh = ctx->sSize[1];

  uint8_t* sData = ctx->sData;
  uint8_t* tData = ctx->tData;

  ssize_t sStride = ctx->sStride;
  ssize_t tStride = ctx->tStride;

  if (ctx->isBound[0] == 1)
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 3;
        const int32_t* wp = weightList;

        uint32_t cr = 0x80;
        uint32_t cg = 0x80;
        uint32_t cb = 0x80;

        for (uint j = recordList->count; j; j--)
        {
          uint32_t w0 = wp[0];

          cr += (uint32_t)sp[PIXEL_RGB24_POS_R] * w0;
          cg += (uint32_t)sp[PIXEL_RGB24_POS_G] * w0;
          cb += (uint32_t)sp[PIXEL_RGB24_POS_B] * w0;

          sp += 3;
          wp += 1;
        }

        tp[PIXEL_RGB24_POS_R] = (uint8_t)(cr >> 8);
        tp[PIXEL_RGB24_POS_G] = (uint8_t)(cg >> 8);
        tp[PIXEL_RGB24_POS_B] = (uint8_t)(cb >> 8);

        recordList += 1;
        weightList += kernelSize;

        tp += 3;
      }

      sData += sStride;
      tData += tStride;
    }
  }
  else
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 3;
        const int32_t* wp = weightList;

        int32_t cr = 0x80;
        int32_t cg = 0x80;
        int32_t cb = 0x80;

        for (uint j = recordList->count; j; j--)
        {
          int32_t w0 = wp[0];

          cr += (int32_t)sp[PIXEL_RGB24_POS_R] * w0;
          cg += (int32_t)sp[PIXEL_RGB24_POS_G] * w0;
          cb += (int32_t)sp[PIXEL_RGB24_POS_B] * w0;

          sp += 3;
          wp += 1;
        }

        tp[PIXEL_RGB24_POS_R] = (uint8_t)(uint32_t)Math::bound<int32_t>(cr >> 8, 0, 255);
        tp[PIXEL_RGB24_POS_G] = (uint8_t)(uint32_t)Math::bound<int32_t>(cg >> 8, 0, 255);
        tp[PIXEL_RGB24_POS_B] = (uint8_t)(uint32_t)Math::bound<int32_t>(cb >> 8, 0, 255);

        recordList += 1;
        weightList += kernelSize;

        tp += 3;
      }

      sData += sStride;
      tData += tStride;
    }
  }
}

// ============================================================================
// [Fog::ImageResize - Context - DoHorizontal - A8]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doHorizontal_A8(ImageResizeContext* ctx)
{
  uint kernelSize = ctx->kernelSize[0];

  uint dw = ctx->dSize[0];
  uint sh = ctx->sSize[1];

  uint8_t* sData = ctx->sData;
  uint8_t* tData = ctx->tData;

  ssize_t sStride = ctx->sStride;
  ssize_t tStride = ctx->tStride;

  if (ctx->isBound[0] == 1)
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 1;
        const int32_t* wp = weightList;

        uint32_t ca = 0x80;

        for (uint j = recordList->count; j; j--)
        {
          uint32_t p0 = sp[0];
          uint32_t w0 = wp[0];

          ca += p0 * w0;

          sp += 1;
          wp += 1;
        }

        tp[0] = (uint8_t)(ca >> 8);

        recordList += 1;
        weightList += kernelSize;

        tp += 1;
      }

      sData += sStride;
      tData += tStride;
    }
  }
  else
  {
    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 1;
        const int32_t* wp = weightList;

        int32_t ca = 0x80;

        for (uint j = recordList->count; j; j--)
        {
          uint32_t p0 = sp[0];
          int32_t w0 = wp[0];

          ca += (int32_t)p0 * w0;

          sp += 1;
          wp += 1;
        }

        tp[0] = (uint8_t)(uint32_t)Math::bound<int>(ca >> 8, 0, 255);

        recordList += 1;
        weightList += kernelSize;

        tp += 1;
      }

      sData += sStride;
      tData += tStride;
    }
  }
}

// ============================================================================
// [Fog::ImageResize - Context - DoVertical - PRGB32]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doVertical_PRGB32(ImageResizeContext* ctx)
{
  uint kernelSize = ctx->kernelSize[0];

  uint dw = ctx->dSize[0];
  uint dh = ctx->dSize[1];

  uint8_t* dData = ctx->dData;

  ssize_t dStride = ctx->dStride;
  ssize_t tStride = ctx->tStride;

  const ImageResizeRecord* recordList = ctx->recordList;
  const int32_t* weightList = ctx->weightList;

  if (ctx->isBound[1] == 1)
  {
    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        uint32_t cr_cb = 0x00800080;
        uint32_t ca_cg = 0x00800080;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          uint32_t w0 = wp[0];
        
          ca_cg += ((p0 >> 8) & 0x00FF00FF) * w0;
          cr_cb += ((p0     ) & 0x00FF00FF) * w0;

          tp += tStride;
          wp += 1;
        }

        reinterpret_cast<uint32_t*>(dp)[0] = _FOG_ACC_COMBINE_2(ca_cg & 0xFF00FF00, (cr_cb & 0xFF00FF00) >> 8);

        dp += 4;
        tData += 4;
      }

      recordList += 1;
      weightList += kernelSize;

      dData += dStride;
    }
  }
  else
  {
    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        int32_t ca = 0x80;
        int32_t cr = 0x80;
        int32_t cg = 0x80;
        int32_t cb = 0x80;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          int32_t w0 = wp[0];

          ca += (int32_t)Argb32::getAlpha(p0) * w0;
          cr += (int32_t)Argb32::getRed  (p0) * w0;
          cg += (int32_t)Argb32::getGreen(p0) * w0;
          cb += (int32_t)Argb32::getBlue (p0) * w0;

          tp += tStride;
          wp += 1;
        }

        ca = Math::bound<int32_t>(ca >> 8, 0, 255);
        cr = Math::bound<int32_t>(cr >> 8, 0, ca);
        cg = Math::bound<int32_t>(cg >> 8, 0, ca);
        cb = Math::bound<int32_t>(cb >> 8, 0, ca);
        reinterpret_cast<uint32_t*>(dp)[0] = Argb32(ca, cr, cg, cb);

        dp += 4;
        tData += 4;
      }

      recordList += 1;
      weightList += kernelSize;

      dData += dStride;
    }
  }
}

// ============================================================================
// [Fog::ImageResize - Context - DoVertical - XRGB32]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doVertical_XRGB32(ImageResizeContext* ctx)
{
  uint kernelSize = ctx->kernelSize[0];

  uint dw = ctx->dSize[0];
  uint dh = ctx->dSize[1];

  uint8_t* dData = ctx->dData;

  ssize_t dStride = ctx->dStride;
  ssize_t tStride = ctx->tStride;

  const ImageResizeRecord* recordList = ctx->recordList;
  const int32_t* weightList = ctx->weightList;

  if (ctx->isBound[1] == 1)
  {
    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        uint32_t cx_cg = 0x00008000;
        uint32_t cr_cb = 0x00800080;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          uint32_t w0 = wp[0];
        
          cx_cg += (p0 & 0x0000FF00) * w0;
          cr_cb += (p0 & 0x00FF00FF) * w0;

          tp += tStride;
          wp += 1;
        }

        reinterpret_cast<uint32_t*>(dp)[0] = _FOG_ACC_COMBINE_2(0xFF000000, ((cx_cg & 0x00FF0000) | (cr_cb & 0xFF00FF00)) >> 8);

        dp += 4;
        tData += 4;
      }

      recordList += 1;
      weightList += kernelSize;

      dData += dStride;
    }
  }
  else
  {
    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        int32_t cr = 0x80;
        int32_t cg = 0x80;
        int32_t cb = 0x80;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          int32_t w0 = wp[0];

          cr += (int32_t)Argb32::getRed  (p0) * w0;
          cg += (int32_t)Argb32::getGreen(p0) * w0;
          cb += (int32_t)Argb32::getBlue (p0) * w0;

          tp += tStride;
          wp += 1;
        }

        cr = Math::bound<int32_t>(cr >> 8, 0, 255);
        cg = Math::bound<int32_t>(cg >> 8, 0, 255);
        cb = Math::bound<int32_t>(cb >> 8, 0, 255);
        reinterpret_cast<uint32_t*>(dp)[0] = Argb32(0xFF, cr, cg, cb);

        dp += 4;
        tData += 4;
      }

      recordList += 1;
      weightList += kernelSize;

      dData += dStride;
    }
  }
}

// ============================================================================
// [Fog::ImageResize - Context - DoVertical - RGB24, A8]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doVertical_Bytes(ImageResizeContext* ctx, uint wScale)
{
  uint kernelSize = ctx->kernelSize[0];

  uint dw = ctx->dSize[0] * wScale;
  uint dh = ctx->dSize[1];

  uint8_t* dData = ctx->dData;

  ssize_t dStride = ctx->dStride;
  ssize_t tStride = ctx->tStride;

  const ImageResizeRecord* recordList = ctx->recordList;
  const int32_t* weightList = ctx->weightList;

  if (ctx->isBound[1] == 1)
  {
    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = 0;

      if (((size_t)dp & 0x7) == 0)
        goto _BoundLarge;
      i = 8 - ((uint)(size_t)dp & 0x7);

_BoundSmall:
      x -= i;
      do {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        uint32_t c0 = 0x80;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = tp[0];
          uint32_t w0 = wp[0];
        
          c0 += p0 * w0;

          tp += tStride;
          wp += 1;
        }

        dp[0] = (uint8_t)(c0 >> 8);

        dp += 1;
        tData += 1;
      } while (--i);

_BoundLarge:
      while (x >= 8)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        uint32_t c0 = 0x00800080;
        uint32_t c1 = 0x00800080;
        uint32_t c2 = 0x00800080;
        uint32_t c3 = 0x00800080;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          uint32_t p1 = reinterpret_cast<const uint32_t*>(tp)[1];
          uint32_t w0 = wp[0];

          c0 += ((p0     ) & 0x00FF00FF) * w0;
          c1 += ((p0 >> 8) & 0x00FF00FF) * w0;
          c2 += ((p1     ) & 0x00FF00FF) * w0;
          c3 += ((p1 >> 8) & 0x00FF00FF) * w0;

          tp += tStride;
          wp += 1;
        }

        reinterpret_cast<uint32_t*>(dp)[0] = _FOG_ACC_COMBINE_2((c0 & 0xFF00FF00) >> 8, c1 & 0xFF00FF00);
        reinterpret_cast<uint32_t*>(dp)[1] = _FOG_ACC_COMBINE_2((c2 & 0xFF00FF00) >> 8, c3 & 0xFF00FF00);

        dp += 8;
        tData += 8;

        x -= 8;
      }

      i = x;
      if (i != 0)
        goto _BoundSmall;

      recordList += 1;
      weightList += kernelSize;

      dData += dStride;
    }
  }
  else
  {
    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = 0;

      if (((size_t)dp & 0x3) == 0)
        goto _UnboundLarge;
      i = 4 - ((uint)(size_t)dp & 0x3);

_UnboundSmall:
      x -= i;
      do {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        int32_t c0 = 0x80;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = tp[0];
          int32_t w0 = wp[0];

          c0 += (int32_t)p0 * w0;

          tp += tStride;
          wp += 1;
        }

        dp[0] = (uint8_t)(uint32_t)Math::bound<int32_t>(c0 >> 8, 0, 255);

        dp += 1;
        tData += 1;
      } while (--i);

_UnboundLarge:
      while (x >= 4)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        int32_t c0 = 0x80;
        int32_t c1 = 0x80;
        int32_t c2 = 0x80;
        int32_t c3 = 0x80;

        for (uint j = count; j; j--)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          uint32_t w0 = wp[0];

          c0 += ((p0      ) & 0xFF) * w0;
          c1 += ((p0 >>  8) & 0xFF) * w0;
          c2 += ((p0 >> 16) & 0xFF) * w0;
          c3 += ((p0 >> 24)       ) * w0;

          tp += tStride;
          wp += 1;
        }

        reinterpret_cast<uint32_t*>(dp)[0] = _FOG_ACC_COMBINE_4(
          Math::bound<int32_t>(c0 >> 8, 0, 255)      ,
          Math::bound<int32_t>(c1 >> 8, 0, 255) <<  8,
          Math::bound<int32_t>(c2 >> 8, 0, 255) << 16,
          Math::bound<int32_t>(c3 >> 8, 0, 255) << 24);

        dp += 4;
        tData += 4;
        
        x -= 4;
      }

      i = x;
      if (i != 0)
        goto _UnboundSmall;

      recordList += 1;
      weightList += kernelSize;

      dData += dStride;
    }
  }
}

static void FOG_CDECL ImageResizeContext_doVertical_RGB24(ImageResizeContext* ctx)
{
  ImageResizeContext_doVertical_Bytes(ctx, 3);
}

static void FOG_CDECL ImageResizeContext_doVertical_A8(ImageResizeContext* ctx)
{
  ImageResizeContext_doVertical_Bytes(ctx, 1);
}

// ============================================================================
// [Fog::ImageResize - Resize]
// ============================================================================

static err_t FOG_CDECL ImageResize_resize(Image* dst, const SizeI* dSize, const Image* src, const RectI* sFragment, uint32_t resizeFunc, const Hash<StringW, Var>* params)
{
  switch (resizeFunc)
  {
    case IMAGE_RESIZE_NEAREST:
    {
      ImageResize_NearestFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 1.0f);
    }

    case IMAGE_RESIZE_BILINEAR:
    {
      ImageResize_BilinearFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 1.0f);
    }

    case IMAGE_RESIZE_BICUBIC:
    {
      ImageResize_BicubicFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 2.0f);
    }

    case IMAGE_RESIZE_BELL:
    {
      ImageResize_BellFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 1.5f);
    }

    case IMAGE_RESIZE_GAUSS:
    {
      ImageResize_GaussFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 2.0f);
    }

    case IMAGE_RESIZE_HERMITE:
    {
      ImageResize_HermiteFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 1.0f);
    }

    case IMAGE_RESIZE_HANNING:
    {
      ImageResize_HanningFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 1.0f);
    }

    case IMAGE_RESIZE_CATROM:
    {
      ImageResize_CatromFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 2.0f);
    }

    case IMAGE_RESIZE_MITCHELL:
    {
      ImageResize_MitchellFunction f;

      if (params)
      {
        const Var* b = params->getPtr(Ascii8("b"));
        const Var* c = params->getPtr(Ascii8("c"));

        if (b != NULL)
          FOG_RETURN_ON_ERROR(b->getFloat(f.b));

        if (c != NULL)
          FOG_RETURN_ON_ERROR(b->getFloat(f.c));

        if (b != NULL || c != NULL)
          f.init();
      }

      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 2.0f);
    }

    case IMAGE_RESIZE_BESSEL:
    {
      ImageResize_BesselFunction f;
      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, 3.2383f);
    }

    case IMAGE_RESIZE_SINC:
    {
      ImageResize_SincFunction f;

      if (params)
      {
        const Var* r = params->getPtr(Ascii8("radius"));
        if (r != NULL)
          FOG_RETURN_ON_ERROR(r->getFloat(f.radius, 1.0f, 16.0f));
      }

      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, f.radius);
    }

    case IMAGE_RESIZE_LANCZOS:
    {
      ImageResize_LanczosFunction f;

      if (params)
      {
        const Var* r = params->getPtr(Ascii8("radius"));
        if (r != NULL)
          FOG_RETURN_ON_ERROR(r->getFloat(f.radius, 1.0f, 16.0f));
      }

      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, f.radius);
    }

    case IMAGE_RESIZE_BLACKMAN:
    {
      ImageResize_BlackmanFunction f;

      if (params)
      {
        const Var* r = params->getPtr(Ascii8("radius"));
        if (r != NULL)
          FOG_RETURN_ON_ERROR(r->getFloat(f.radius, 1.0f, 16.0f));
      }

      return fog_api.image_resizeCustom(dst, dSize, src, sFragment, &f, f.radius);
    }

    default:
    {
      return ERR_RT_INVALID_ARGUMENT;
    }
  }
}

static err_t FOG_CDECL ImageResize_resizeCustom(Image* dst, const SizeI* dSize, const Image* src, const RectI* sFragment, const MathFunctionF* resizeFunc, float radius)
{
  if (FOG_IS_NULL(resizeFunc))
    return ERR_RT_INVALID_ARGUMENT;

  if (!Math::isFinite(radius) || radius < 1.0f || radius > 16.0f)
    return ERR_RT_INVALID_ARGUMENT;

  if (dSize && !dSize->isValid())
    return ERR_IMAGE_INVALID_SIZE;

  if (src->isEmpty())
  {
    dst->reset();
    return ERR_OK;
  }
  
  uint32_t format = src->getFormat();
  FOG_RETURN_ON_ERROR(dst->create(*dSize, format));

  ImageData* dst_d = dst->_d;
  ImageData* src_d = src->_d;

  ImageResizeContext ctx;

  FOG_RETURN_ON_ERROR(
    ImageResize_api.init(&ctx,
      dst_d->first, dst_d->stride, dst_d->size.w, dst_d->size.h,
      src_d->first, src_d->stride, src_d->size.w, src_d->size.h,
      format,
      resizeFunc, radius)
  );

  ImageResize_api.doWeights(&ctx, 0);
  ImageResize_api.doHorizontal[format](&ctx);

  ImageResize_api.doWeights(&ctx, 1);
  ImageResize_api.doVertical[format](&ctx);

  ImageResize_api.destroy(&ctx);
  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_CPU_DECLARE_INITIALIZER_SSE2( ImageResize_init_SSE2(ImageResizeApi* api) )

FOG_NO_EXPORT void ImageResize_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.image_resize = ImageResize_resize;
  fog_api.image_resizeCustom = ImageResize_resizeCustom;

  ImageResize_api.init = ImageResizeContext_init;
  ImageResize_api.destroy = ImageResizeContext_destroy;
  ImageResize_api.doWeights = ImageResizeContext_doWeights;

  ImageResize_api.doHorizontal[IMAGE_FORMAT_PRGB32] = ImageResizeContext_doHorizontal_PRGB32;
  ImageResize_api.doHorizontal[IMAGE_FORMAT_XRGB32] = ImageResizeContext_doHorizontal_XRGB32;
  ImageResize_api.doHorizontal[IMAGE_FORMAT_RGB24 ] = ImageResizeContext_doHorizontal_RGB24;
  ImageResize_api.doHorizontal[IMAGE_FORMAT_A8    ] = ImageResizeContext_doHorizontal_A8;
//ImageResize_api.doHorizontal[IMAGE_FORMAT_I8    ] = NONE;
  // TODO: 16-bit image processing.

  ImageResize_api.doVertical[IMAGE_FORMAT_PRGB32] = ImageResizeContext_doVertical_PRGB32;
  ImageResize_api.doVertical[IMAGE_FORMAT_XRGB32] = ImageResizeContext_doVertical_XRGB32;
  ImageResize_api.doVertical[IMAGE_FORMAT_RGB24 ] = ImageResizeContext_doVertical_RGB24;
  ImageResize_api.doVertical[IMAGE_FORMAT_A8    ] = ImageResizeContext_doVertical_A8;
//ImageResize_api.doVertical[IMAGE_FORMAT_I8    ] = NONE;
  // TODO: 16-bit image processing.

  // --------------------------------------------------------------------------
  // [CPU Based Optimizations]
  // --------------------------------------------------------------------------

  FOG_CPU_USE_INITIALIZER_SSE2( ImageResize_init_SSE2(&ImageResize_api) )
}

} // Fog namespace
