// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_IMAGERESIZE_P_H
#define _FOG_G2D_IMAGING_IMAGERESIZE_P_H

// [Dependencies]
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Imaging/Image.h>

namespace Fog {

//! @addtogroup Fog_G2d_Imaging
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ImageResizeApi;
struct ImageResizeContext;

// ============================================================================
// [Fog::ImageResizeApi]
// ============================================================================

struct FOG_NO_EXPORT ImageResizeApi
{
  typedef err_t (FOG_CDECL* InitFunc)(ImageResizeContext* ctx,
    uint8_t* dData, size_t dStride, int dw, int dh,
    uint8_t* sData, size_t sStride, int sw, int sh,
    uint32_t format,
    const MathFunctionF* func, float radius);
  typedef void (FOG_CDECL* DestroyFunc)(ImageResizeContext* ctx);

  typedef void (FOG_CDECL* DoWeightsFunc)(ImageResizeContext* ctx, uint dir);
  typedef void (FOG_CDECL* DoHorizontalFunc)(ImageResizeContext* ctx);
  typedef void (FOG_CDECL* DoVerticalFunc)(ImageResizeContext* ctx);

  InitFunc init;
  DestroyFunc destroy;

  DoWeightsFunc doWeights;
  DoHorizontalFunc doHorizontal[IMAGE_FORMAT_COUNT];
  DoVerticalFunc doVertical[IMAGE_FORMAT_COUNT];
};

// ============================================================================
// [Fog::ImageResizeOffset]
// ============================================================================

struct FOG_NO_EXPORT ImageResizeRecord
{
  uint32_t pos;
  uint32_t count;
};

// ============================================================================
// [Fog::ImageResizeContext]
// ============================================================================

struct FOG_NO_EXPORT ImageResizeContext
{
  uint8_t* dData;
  uint8_t* sData;
  uint8_t* tData;

  ssize_t dStride;
  ssize_t sStride;
  ssize_t tStride;

  int dSize[2];
  int sSize[2];

  float scale[2];
  float factor[2];
  float radius[2];

  uint kernelSize[2];
  uint isBound[2];

  int32_t* weightList;
  ImageResizeRecord* recordList;

  const MathFunctionF* func;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_IMAGERESIZE_P_H
