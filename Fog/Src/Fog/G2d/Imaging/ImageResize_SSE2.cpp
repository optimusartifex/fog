// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Acc/AccSse2.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Function.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageResize_p.h>

namespace Fog {

// ============================================================================
// [Fog::ImageResize - Constants (SSE2)]
// ============================================================================

FOG_XMM_DECLARE_CONST_PI16_VAR(ImageResizeHalf16ZZLo_PRGB32, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080, 0x0080, 0x0080, 0x0080);
FOG_XMM_DECLARE_CONST_PI16_VAR(ImageResizeHalf16ZZLo_XRGB32, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0080, 0x0080, 0x0080);
FOG_XMM_DECLARE_CONST_PI16_VAR(ImageResizeHalf16HiLo_PRGB32, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080);
FOG_XMM_DECLARE_CONST_PI16_VAR(ImageResizeHalf16HiLo_XRGB32, 0xFFFF, 0x0080, 0x0080, 0x0080, 0xFFFF, 0x0080, 0x0080, 0x0080);

FOG_XMM_DECLARE_CONST_PI32_VAR(ImageResizeHalf32, 0x00000080, 0x00000080, 0x00000080, 0x00000080);

// ============================================================================
// [Fog::ImageResize - Context - DoHorizontal - PRGB32 (SSE2)]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doHorizontal_PRGB32_SSE2(ImageResizeContext* ctx)
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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf16ZZLo_PRGB32);

    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        int j = (int)recordList->count;

        if ((j -= 4) >= 0)
        {
          __m128i xmmAcc1;
          Acc::m128iZero(xmmAcc1);

          do {
            __m128i xmmPixel0;
            __m128i xmmPixel1;

            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixel0, sp + 0);
            Acc::m128iLoad8(xmmPixel1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad8(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight1, xmmWeight1);

            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight1, xmmWeight1);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight1);

            Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
            Acc::m128iAddPI16(xmmAcc1, xmmAcc1, xmmPixel1);

            sp += 16;
            wp += 4;
          } while ((j -= 4) >= 0);

          __m128i xmmTmp0;
          __m128i xmmTmp1;

          Acc::m128iShufflePI32<1, 0, 3, 2>(xmmTmp0, xmmAcc0);
          Acc::m128iShufflePI32<1, 0, 3, 2>(xmmTmp1, xmmAcc1);

          Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmTmp0);
          Acc::m128iAddPI16(xmmAcc1, xmmAcc1, xmmTmp1);
          Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmAcc1);
        }

        switch (j + 4)
        {
          case 3:
          {
            __m128i xmmPixel0;
            __m128i xmmPixel1;

            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixel0, sp + 0);
            Acc::m128iLoad4(xmmPixel1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad4(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight1, xmmWeight1);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight1);

            Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
            Acc::m128iShufflePI32<1, 0, 3, 2>(xmmPixel0, xmmPixel0);
            Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel1);
            Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
            break;
          }

          case 2:
          {
            __m128i xmmPixel0;
            __m128i xmmWeight0;

            Acc::m128iLoad8(xmmPixel0, sp + 0);
            Acc::m128iLoad8(xmmWeight0, wp + 0);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);

            Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
            Acc::m128iShufflePI32<1, 0, 3, 2>(xmmPixel0, xmmPixel0);
            Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
            break;
          }

          case 1:
          {
            __m128i xmmPixel0;
            __m128i xmmWeight0;

            Acc::m128iLoad4(xmmWeight0, wp);
            Acc::m128iLoad4(xmmPixel0, sp);

            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);
            Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
            break;
          }

          case 0:
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }

        Acc::m128iRShiftPU16<8>(xmmAcc0, xmmAcc0);
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0);
        Acc::m128iStore4(tp, xmmAcc0);

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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf32);

    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        int j = (int)recordList->count;
 
        if ((j -= 4) >= 0)
        {
          do {
            __m128i xmmPixelLo0;
            __m128i xmmPixelLo1;
            __m128i xmmPixelHi0;
            __m128i xmmPixelHi1;
            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixelLo0, sp + 0);
            Acc::m128iLoad8(xmmPixelLo1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad8(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo1, xmmPixelLo1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight1, xmmWeight1);

            Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);
            Acc::m128iCopy(xmmPixelHi1, xmmPixelLo1);

            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight1, xmmWeight1);

            Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixelLo1, xmmPixelLo1, xmmWeight1);

            Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi1, xmmPixelHi1, xmmWeight1);

            Acc::m128iCopy(xmmWeight0, xmmPixelLo0);
            Acc::m128iCopy(xmmWeight1, xmmPixelLo1);

            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo1, xmmPixelLo1, xmmPixelHi1);

            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight0, xmmWeight0, xmmPixelHi0);
            Acc::m128iAddPI32(xmmPixelLo0, xmmPixelLo0, xmmPixelLo1);
            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight1, xmmWeight1, xmmPixelHi1);

            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
            Acc::m128iAddPI32(xmmWeight0, xmmWeight0, xmmWeight1);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmWeight0);

            sp += 16;
            wp += 4;
          } while ((j -= 4) >= 0);
        }

        switch (j + 4)
        {
          case 3:
          {
            __m128i xmmPixelLo0;
            __m128i xmmPixelLo1;
            __m128i xmmPixelHi0;
            __m128i xmmPixelHi1;
            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixelLo0, sp + 0);
            Acc::m128iLoad4(xmmPixelLo1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad4(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo1, xmmPixelLo1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight1, xmmWeight1);

            Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);
            Acc::m128iCopy(xmmPixelHi1, xmmPixelLo1);

            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixelLo1, xmmPixelLo1, xmmWeight1);

            Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi1, xmmPixelHi1, xmmWeight1);

            Acc::m128iCopy(xmmWeight0, xmmPixelLo0);

            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo1, xmmPixelLo1, xmmPixelHi1);

            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight0, xmmWeight0, xmmPixelHi0);
            Acc::m128iAddPI32(xmmPixelLo0, xmmPixelLo0, xmmPixelLo1);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmWeight0);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
            break;
          }

          case 2:
          {
            __m128i xmmPixelLo;
            __m128i xmmPixelHi;
            __m128i xmmWeight0;

            Acc::m128iLoad8(xmmPixelLo, sp);
            Acc::m128iLoad8(xmmWeight0, wp);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo, xmmPixelLo);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iCopy(xmmPixelHi, xmmPixelLo);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixelLo, xmmPixelLo, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi, xmmPixelHi, xmmWeight0);

            Acc::m128iCopy(xmmWeight0, xmmPixelLo);
            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo, xmmPixelLo, xmmPixelHi);
            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight0, xmmWeight0, xmmPixelHi);

            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmWeight0);
            break;
          }

          case 1:
          {
            __m128i xmmPixelLo;
            __m128i xmmPixelHi;
            __m128i xmmWeight0;

            Acc::m128iLoad4(xmmPixelLo, sp);
            Acc::m128iLoad4(xmmWeight0, wp);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo, xmmPixelLo);
            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iCopy(xmmPixelHi, xmmPixelLo);

            Acc::m128iMulLoPI16(xmmPixelLo, xmmPixelLo, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi, xmmPixelHi, xmmWeight0);

            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo, xmmPixelLo, xmmPixelHi);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo);
            break;
          }

          case 0:
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }

        {
          __m128i xmmTmp0 = _mm_setzero_si128();

          Acc::m128iRShiftPI32<8>(xmmAcc0, xmmAcc0);
          Acc::m128iPackPI16FromPI32(xmmAcc0, xmmAcc0);
          Acc::m128iMaxPI16(xmmAcc0, xmmAcc0, xmmTmp0);
          Acc::m128iShufflePI16Lo<3, 3, 3, 3>(xmmTmp0, xmmAcc0);
          Acc::m128iMinPI16(xmmAcc0, xmmAcc0, xmmTmp0);
          Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0);
          Acc::m128iStore4(tp, xmmAcc0);
        }

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
// [Fog::ImageResize - Context - DoHorizontal - XRGB32 (SSE2)]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doHorizontal_XRGB32_SSE2(ImageResizeContext* ctx)
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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf16ZZLo_XRGB32);

    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        int j = (int)recordList->count;

        if ((j -= 4) >= 0)
        {
          __m128i xmmAcc1;
          Acc::m128iZero(xmmAcc1);

          do {
            __m128i xmmPixel0;
            __m128i xmmPixel1;

            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixel0, sp + 0);
            Acc::m128iLoad8(xmmPixel1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad8(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight1, xmmWeight1);

            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight1, xmmWeight1);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight1);

            Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel0);
            Acc::m128iAddusPU16(xmmAcc1, xmmAcc1, xmmPixel1);

            sp += 16;
            wp += 4;
          } while ((j -= 4) >= 0);

          __m128i xmmTmp0;
          __m128i xmmTmp1;

          Acc::m128iShufflePI32<1, 0, 3, 2>(xmmTmp0, xmmAcc0);
          Acc::m128iShufflePI32<1, 0, 3, 2>(xmmTmp1, xmmAcc1);

          Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmTmp0);
          Acc::m128iAddusPU16(xmmAcc1, xmmAcc1, xmmTmp1);
          Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmAcc1);
        }

        switch (j + 4)
        {
          case 3:
          {
            __m128i xmmPixel0;
            __m128i xmmPixel1;

            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixel0, sp + 0);
            Acc::m128iLoad4(xmmPixel1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad4(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight1, xmmWeight1);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight1);

            Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel0);
            Acc::m128iShufflePI32<1, 0, 3, 2>(xmmPixel0, xmmPixel0);
            Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel1);
            Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel0);
            break;
          }

          case 2:
          {
            __m128i xmmPixel0;
            __m128i xmmWeight0;

            Acc::m128iLoad8(xmmPixel0, sp + 0);
            Acc::m128iLoad8(xmmWeight0, wp + 0);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);

            Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel0);
            Acc::m128iShufflePI32<1, 0, 3, 2>(xmmPixel0, xmmPixel0);
            Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel0);
            break;
          }

          case 1:
          {
            __m128i xmmPixel0;
            __m128i xmmWeight0;

            Acc::m128iLoad4(xmmWeight0, wp);
            Acc::m128iLoad4(xmmPixel0, sp);

            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);

            Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight0);
            Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel0);
            break;
          }

          case 0:
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }

        Acc::m128iRShiftPU16<8>(xmmAcc0, xmmAcc0);
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0);
        Acc::m128iStore4(tp, xmmAcc0);

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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf32);

    for (uint y = 0; y < sh; y++)
    {
      const ImageResizeRecord* recordList = ctx->recordList;
      const int32_t* weightList = ctx->weightList;

      uint8_t* tp = tData;

      for (uint x = 0; x < dw; x++)
      {
        const uint8_t* sp = sData + recordList->pos * 4;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        int j = (int)recordList->count;
 
        if ((j -= 4) >= 0)
        {
          do {
            __m128i xmmPixelLo0;
            __m128i xmmPixelLo1;
            __m128i xmmPixelHi0;
            __m128i xmmPixelHi1;
            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixelLo0, sp + 0);
            Acc::m128iLoad8(xmmPixelLo1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad8(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo1, xmmPixelLo1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight1, xmmWeight1);

            Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);
            Acc::m128iCopy(xmmPixelHi1, xmmPixelLo1);

            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight1, xmmWeight1);

            Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixelLo1, xmmPixelLo1, xmmWeight1);

            Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi1, xmmPixelHi1, xmmWeight1);

            Acc::m128iCopy(xmmWeight0, xmmPixelLo0);
            Acc::m128iCopy(xmmWeight1, xmmPixelLo1);

            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo1, xmmPixelLo1, xmmPixelHi1);

            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight0, xmmWeight0, xmmPixelHi0);
            Acc::m128iAddPI32(xmmPixelLo0, xmmPixelLo0, xmmPixelLo1);
            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight1, xmmWeight1, xmmPixelHi1);

            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
            Acc::m128iAddPI32(xmmWeight0, xmmWeight0, xmmWeight1);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmWeight0);

            sp += 16;
            wp += 4;
          } while ((j -= 4) >= 0);
        }

        switch (j + 4)
        {
          case 3:
          {
            __m128i xmmPixelLo0;
            __m128i xmmPixelLo1;
            __m128i xmmPixelHi0;
            __m128i xmmPixelHi1;
            __m128i xmmWeight0;
            __m128i xmmWeight1;

            Acc::m128iLoad8(xmmPixelLo0, sp + 0);
            Acc::m128iLoad4(xmmPixelLo1, sp + 8);

            Acc::m128iLoad8(xmmWeight0, wp + 0);
            Acc::m128iLoad4(xmmWeight1, wp + 2);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo1, xmmPixelLo1);

            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight1, xmmWeight1);

            Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);
            Acc::m128iCopy(xmmPixelHi1, xmmPixelLo1);

            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight0);
            Acc::m128iMulLoPI16(xmmPixelLo1, xmmPixelLo1, xmmWeight1);

            Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi1, xmmPixelHi1, xmmWeight1);

            Acc::m128iCopy(xmmWeight0, xmmPixelLo0);

            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo1, xmmPixelLo1, xmmPixelHi1);

            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight0, xmmWeight0, xmmPixelHi0);
            Acc::m128iAddPI32(xmmPixelLo0, xmmPixelLo0, xmmPixelLo1);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmWeight0);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
            break;
          }

          case 2:
          {
            __m128i xmmPixelLo;
            __m128i xmmPixelHi;
            __m128i xmmWeight0;

            Acc::m128iLoad8(xmmPixelLo, sp);
            Acc::m128iLoad8(xmmWeight0, wp);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo, xmmPixelLo);
            Acc::m128iShufflePI16Lo<2, 2, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iCopy(xmmPixelHi, xmmPixelLo);
            Acc::m128iShufflePI32<0, 0, 1, 1>(xmmWeight0, xmmWeight0);

            Acc::m128iMulLoPI16(xmmPixelLo, xmmPixelLo, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi, xmmPixelHi, xmmWeight0);

            Acc::m128iCopy(xmmWeight0, xmmPixelLo);
            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo, xmmPixelLo, xmmPixelHi);
            Acc::m128iUnpackPI32FromPI16Hi(xmmWeight0, xmmWeight0, xmmPixelHi);

            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmWeight0);
            break;
          }

          case 1:
          {
            __m128i xmmPixelLo;
            __m128i xmmPixelHi;
            __m128i xmmWeight0;

            Acc::m128iLoad4(xmmPixelLo, sp);
            Acc::m128iLoad4(xmmWeight0, wp);

            Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo, xmmPixelLo);
            Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight0, xmmWeight0);
            Acc::m128iCopy(xmmPixelHi, xmmPixelLo);

            Acc::m128iMulLoPI16(xmmPixelLo, xmmPixelLo, xmmWeight0);
            Acc::m128iMulHiPI16(xmmPixelHi, xmmPixelHi, xmmWeight0);

            Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo, xmmPixelLo, xmmPixelHi);
            Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo);
            break;
          }

          case 0:
            break;

          default:
            FOG_ASSERT_NOT_REACHED();
        }

        Acc::m128iRShiftPI32<8>(xmmAcc0, xmmAcc0);
        Acc::m128iPackPI16FromPI32(xmmAcc0, xmmAcc0);
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0);
        Acc::m128iOr(xmmAcc0, xmmAcc0, FOG_XMM_GET_CONST_PI(0000000000000000_00000000FF000000));
        Acc::m128iStore4(tp, xmmAcc0);

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
// [Fog::ImageResize - Context - DoVertical - PRGB32 (SSE2)]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doVertical_PRGB32_SSE2(ImageResizeContext* ctx)
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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf16HiLo_PRGB32);

    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = dw;

      if (((size_t)dp & 0x3) != 0)
        goto _BoundSmall;
      if (((size_t)dp & 0xF) == 0)
        goto _BoundLarge;
      i = 4 - (((uint)(size_t)dp & 0xF) >> 2);

_BoundSmall:
      x -= i;
      do {
        uint8_t* tp = tData;

        uint32_t cr_cb = 0x00800080;
        uint32_t ca_cg = 0x00800080;

        for (uint j = 0; j < count; j++)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          uint32_t w0 = weightList[j];
        
          ca_cg += ((p0 >> 8) & 0x00FF00FF) * w0;
          cr_cb += ((p0     ) & 0x00FF00FF) * w0;

          tp += tStride;
        }

        reinterpret_cast<uint32_t*>(dp)[0] = _FOG_ACC_COMBINE_2(ca_cg & 0xFF00FF00, (cr_cb & 0xFF00FF00) >> 8);

        dp += 4;
        tData += 4;
      } while (--i);

_BoundLarge:
      while (x >= 8)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        __m128i xmmAcc1 = xmmHalf;
        __m128i xmmAcc2 = xmmHalf;
        __m128i xmmAcc3 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixel0;
          __m128i xmmPixel1;

          __m128i xmmWeight;
          Acc::m128iLoad4(xmmWeight, wp);

          Acc::m128iLoad8(xmmPixel0, tp + 0);
          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iLoad8(xmmPixel1, tp + 8);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

          Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight);

          Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
          Acc::m128iAddPI16(xmmAcc1, xmmAcc1, xmmPixel1);

          Acc::m128iLoad8(xmmPixel0, tp + 16);
          Acc::m128iLoad8(xmmPixel1, tp + 24);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

          Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight);

          Acc::m128iAddPI16(xmmAcc2, xmmAcc2, xmmPixel0);
          Acc::m128iAddPI16(xmmAcc3, xmmAcc3, xmmPixel1);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPU16<8>(xmmAcc0, xmmAcc0);
        Acc::m128iRShiftPU16<8>(xmmAcc1, xmmAcc1);
        Acc::m128iRShiftPU16<8>(xmmAcc2, xmmAcc2);
        Acc::m128iRShiftPU16<8>(xmmAcc3, xmmAcc3);

        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iPackPU8FromPU16(xmmAcc2, xmmAcc2, xmmAcc3);

        Acc::m128iStore16a(dp +  0, xmmAcc0);
        Acc::m128iStore16a(dp + 16, xmmAcc2);

        dp += 32;
        tData += 32;

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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf32);

    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = dw;

      if (((size_t)dp & 0x3) != 0)
        goto _UnboundSmall;
      if (((size_t)dp & 0xF) == 0)
        goto _UnboundLarge;
      i = 4 - (((uint)(size_t)dp & 0xF) >> 2);

_UnboundSmall:
      x -= i;
      do {
        uint8_t* tp = tData;

        int32_t ca = 0x80;
        int32_t cr = 0x80;
        int32_t cg = 0x80;
        int32_t cb = 0x80;

        for (uint j = 0; j < count; j++)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          int32_t w0 = weightList[j];

          ca += (int32_t)Argb32::getAlpha(p0) * w0;
          cr += (int32_t)Argb32::getRed  (p0) * w0;
          cg += (int32_t)Argb32::getGreen(p0) * w0;
          cb += (int32_t)Argb32::getBlue (p0) * w0;

          tp += tStride;
        }

        ca = Math::bound<int32_t>(ca >> 8, 0, 255);
        cr = Math::bound<int32_t>(cr >> 8, 0, ca);
        cg = Math::bound<int32_t>(cg >> 8, 0, ca);
        cb = Math::bound<int32_t>(cb >> 8, 0, ca);
        reinterpret_cast<uint32_t*>(dp)[0] = Argb32(ca, cr, cg, cb);

        dp += 4;
        tData += 4;
      } while (--i);

_UnboundLarge:
      while (x >= 4)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        __m128i xmmAcc1 = xmmHalf;
        __m128i xmmAcc2 = xmmHalf;
        __m128i xmmAcc3 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixelLo0;
          __m128i xmmPixelLo1;

          __m128i xmmPixelHi0;
          __m128i xmmPixelHi1;

          __m128i xmmWeight;

          Acc::m128iLoad4(xmmWeight, wp);

          Acc::m128iLoad8(xmmPixelLo0, tp + 0);
          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iLoad8(xmmPixelLo1, tp + 8);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo1, xmmPixelLo1);

          Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);
          Acc::m128iCopy(xmmPixelHi1, xmmPixelLo1);

          Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixelLo1, xmmPixelLo1, xmmWeight);

          Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight);
          Acc::m128iMulHiPI16(xmmPixelHi1, xmmPixelHi1, xmmWeight);

          Acc::m128iCopy(xmmWeight, xmmPixelLo0);
          Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
          Acc::m128iUnpackPI32FromPI16Hi(xmmWeight, xmmWeight, xmmPixelHi0);

          Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
          Acc::m128iAddPI32(xmmAcc1, xmmAcc1, xmmWeight);

          Acc::m128iCopy(xmmWeight, xmmPixelLo1);
          Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo1, xmmPixelLo1, xmmPixelHi1);
          Acc::m128iUnpackPI32FromPI16Hi(xmmWeight, xmmWeight, xmmPixelHi1);

          Acc::m128iAddPI32(xmmAcc2, xmmAcc2, xmmPixelLo1);
          Acc::m128iAddPI32(xmmAcc3, xmmAcc3, xmmWeight);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPI32<8>(xmmAcc0, xmmAcc0);
        Acc::m128iRShiftPI32<8>(xmmAcc1, xmmAcc1);
        Acc::m128iRShiftPI32<8>(xmmAcc2, xmmAcc2);
        Acc::m128iRShiftPI32<8>(xmmAcc3, xmmAcc3);
          
        Acc::m128iPackPI16FromPI32(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iPackPI16FromPI32(xmmAcc2, xmmAcc2, xmmAcc3);

        Acc::m128iShufflePI16Lo<3, 3, 3, 3>(xmmAcc1, xmmAcc0);
        Acc::m128iShufflePI16Lo<3, 3, 3, 3>(xmmAcc3, xmmAcc2);
          
        Acc::m128iMinPI16(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iMinPI16(xmmAcc2, xmmAcc2, xmmAcc3);
          
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0, xmmAcc2);
        Acc::m128iStore16a(dp, xmmAcc0);

        dp += 16;
        tData += 16;

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

// ============================================================================
// [Fog::ImageResize - Context - DoVertical - XRGB32 (SSE2)]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doVertical_XRGB32_SSE2(ImageResizeContext* ctx)
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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf16HiLo_XRGB32);

    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = dw;

      if (((size_t)dp & 0x3) != 0)
        goto _BoundSmall;
      if (((size_t)dp & 0xF) == 0)
        goto _BoundLarge;
      i = 4 - (((uint)(size_t)dp & 0xF) >> 2);

_BoundSmall:
      x -= i;
      do {
        uint8_t* tp = tData;

        uint32_t cr_cb = 0x00800080;
        uint32_t cx_cg = 0x00008000;

        for (uint j = 0; j < count; j++)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          uint32_t w0 = weightList[j];

          cx_cg += (p0 & 0x0000FF00) * w0;
          cr_cb += (p0 & 0x00FF00FF) * w0;

          tp += tStride;
        }

        reinterpret_cast<uint32_t*>(dp)[0] = _FOG_ACC_COMBINE_2(0xFF000000, ((cx_cg & 0x00FF0000) | (cr_cb & 0xFF00FF00)) >> 8);

        dp += 4;
        tData += 4;
      } while (--i);

_BoundLarge:
      while (x >= 8)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        __m128i xmmAcc1 = xmmHalf;
        __m128i xmmAcc2 = xmmHalf;
        __m128i xmmAcc3 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixel0;
          __m128i xmmPixel1;

          __m128i xmmWeight;
          Acc::m128iLoad4(xmmWeight, wp);

          Acc::m128iLoad8(xmmPixel0, tp + 0);
          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iLoad8(xmmPixel1, tp + 8);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

          Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight);

          Acc::m128iAddusPU16(xmmAcc0, xmmAcc0, xmmPixel0);
          Acc::m128iAddusPU16(xmmAcc1, xmmAcc1, xmmPixel1);

          Acc::m128iLoad8(xmmPixel0, tp + 16);
          Acc::m128iLoad8(xmmPixel1, tp + 24);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

          Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight);

          Acc::m128iAddusPU16(xmmAcc2, xmmAcc2, xmmPixel0);
          Acc::m128iAddusPU16(xmmAcc3, xmmAcc3, xmmPixel1);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPU16<8>(xmmAcc0, xmmAcc0);
        Acc::m128iRShiftPU16<8>(xmmAcc1, xmmAcc1);
        Acc::m128iRShiftPU16<8>(xmmAcc2, xmmAcc2);
        Acc::m128iRShiftPU16<8>(xmmAcc3, xmmAcc3);

        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iPackPU8FromPU16(xmmAcc2, xmmAcc2, xmmAcc3);

        Acc::m128iStore16a(dp +  0, xmmAcc0);
        Acc::m128iStore16a(dp + 16, xmmAcc2);

        dp += 32;
        tData += 32;

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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf32);

    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = dw;

      if (((size_t)dp & 0x3) != 0)
        goto _UnboundSmall;
      if (((size_t)dp & 0xF) == 0)
        goto _UnboundLarge;
      i = 4 - (((uint)(size_t)dp & 0xF) >> 2);

_UnboundSmall:
      x -= i;
      do {
        uint8_t* tp = tData;

        int32_t cr = 0x80;
        int32_t cg = 0x80;
        int32_t cb = 0x80;

        for (uint j = 0; j < count; j++)
        {
          uint32_t p0 = reinterpret_cast<const uint32_t*>(tp)[0];
          int32_t w0 = weightList[j];

          cr += (int32_t)Argb32::getRed  (p0) * w0;
          cg += (int32_t)Argb32::getGreen(p0) * w0;
          cb += (int32_t)Argb32::getBlue (p0) * w0;

          tp += tStride;
        }

        cr = Math::bound<int32_t>(cr >> 8, 0, 255);
        cg = Math::bound<int32_t>(cg >> 8, 0, 255);
        cb = Math::bound<int32_t>(cb >> 8, 0, 255);
        reinterpret_cast<uint32_t*>(dp)[0] = Argb32(0xFF, cr, cg, cb);

        dp += 4;
        tData += 4;
      } while (--i);

_UnboundLarge:
      while (x >= 4)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        __m128i xmmAcc1 = xmmHalf;
        __m128i xmmAcc2 = xmmHalf;
        __m128i xmmAcc3 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixelLo0;
          __m128i xmmPixelLo1;

          __m128i xmmPixelHi0;
          __m128i xmmPixelHi1;

          __m128i xmmWeight;

          Acc::m128iLoad4(xmmWeight, wp);

          Acc::m128iLoad8(xmmPixelLo0, tp + 0);
          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iLoad8(xmmPixelLo1, tp + 8);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo1, xmmPixelLo1);

          Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);
          Acc::m128iCopy(xmmPixelHi1, xmmPixelLo1);

          Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixelLo1, xmmPixelLo1, xmmWeight);

          Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight);
          Acc::m128iMulHiPI16(xmmPixelHi1, xmmPixelHi1, xmmWeight);

          Acc::m128iCopy(xmmWeight, xmmPixelLo0);
          Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
          Acc::m128iUnpackPI32FromPI16Hi(xmmWeight, xmmWeight, xmmPixelHi0);

          Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
          Acc::m128iAddPI32(xmmAcc1, xmmAcc1, xmmWeight);

          Acc::m128iCopy(xmmWeight, xmmPixelLo1);
          Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo1, xmmPixelLo1, xmmPixelHi1);
          Acc::m128iUnpackPI32FromPI16Hi(xmmWeight, xmmWeight, xmmPixelHi1);

          Acc::m128iAddPI32(xmmAcc2, xmmAcc2, xmmPixelLo1);
          Acc::m128iAddPI32(xmmAcc3, xmmAcc3, xmmWeight);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPI32<8>(xmmAcc0, xmmAcc0);
        Acc::m128iRShiftPI32<8>(xmmAcc1, xmmAcc1);
        Acc::m128iRShiftPI32<8>(xmmAcc2, xmmAcc2);
        Acc::m128iRShiftPI32<8>(xmmAcc3, xmmAcc3);
          
        Acc::m128iPackPI16FromPI32(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iPackPI16FromPI32(xmmAcc2, xmmAcc2, xmmAcc3);

        Acc::m128iShufflePI16Lo<3, 3, 3, 3>(xmmAcc1, xmmAcc0);
        Acc::m128iShufflePI16Lo<3, 3, 3, 3>(xmmAcc3, xmmAcc2);
          
        Acc::m128iMinPI16(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iMinPI16(xmmAcc2, xmmAcc2, xmmAcc3);
          
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0, xmmAcc2);
        Acc::m128iOr(xmmAcc0, xmmAcc0, FOG_XMM_GET_CONST_PI(FF000000FF000000_FF000000FF000000));
        Acc::m128iStore16a(dp, xmmAcc0);

        dp += 16;
        tData += 16;

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

// ============================================================================
// [Fog::ImageResize - Context - DoVertical - RGB24, A8]
// ============================================================================

static void FOG_CDECL ImageResizeContext_doVertical_Bytes_SSE2(ImageResizeContext* ctx, uint wScale)
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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf16HiLo_PRGB32);

    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = 0;

      if (((size_t)dp & 0xF) == 0)
        goto _BoundLarge;
      i = 16 - ((uint)(size_t)dp & 0xF);

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
      while (x >= 32)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        __m128i xmmAcc1 = xmmHalf;
        __m128i xmmAcc2 = xmmHalf;
        __m128i xmmAcc3 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixel0;
          __m128i xmmPixel1;

          __m128i xmmWeight;
          Acc::m128iLoad4(xmmWeight, wp);

          Acc::m128iLoad8(xmmPixel0, tp + 0);
          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iLoad8(xmmPixel1, tp + 8);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

          Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight);

          Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);
          Acc::m128iAddPI16(xmmAcc1, xmmAcc1, xmmPixel1);

          Acc::m128iLoad8(xmmPixel0, tp + 16);
          Acc::m128iLoad8(xmmPixel1, tp + 24);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel1, xmmPixel1);

          Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixel1, xmmPixel1, xmmWeight);

          Acc::m128iAddPI16(xmmAcc2, xmmAcc2, xmmPixel0);
          Acc::m128iAddPI16(xmmAcc3, xmmAcc3, xmmPixel1);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPU16<8>(xmmAcc0, xmmAcc0);
        Acc::m128iRShiftPU16<8>(xmmAcc1, xmmAcc1);
        Acc::m128iRShiftPU16<8>(xmmAcc2, xmmAcc2);
        Acc::m128iRShiftPU16<8>(xmmAcc3, xmmAcc3);

        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iPackPU8FromPU16(xmmAcc2, xmmAcc2, xmmAcc3);

        Acc::m128iStore16a(dp +  0, xmmAcc0);
        Acc::m128iStore16a(dp + 16, xmmAcc2);

        dp += 32;
        tData += 32;

        x -= 32;
      }

      while (x >= 8)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixel0;
          __m128i xmmWeight;

          Acc::m128iLoad4(xmmWeight, wp);
          Acc::m128iLoad8(xmmPixel0, tp);

          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixel0, xmmPixel0);
          Acc::m128iMulLoPI16(xmmPixel0, xmmPixel0, xmmWeight);
          Acc::m128iAddPI16(xmmAcc0, xmmAcc0, xmmPixel0);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPU16<8>(xmmAcc0, xmmAcc0);
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0);
        Acc::m128iStore8(dp, xmmAcc0);

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
    __m128i xmmHalf = FOG_XMM_GET_CONST_PI(ImageResizeHalf32);

    for (uint y = 0; y < dh; y++)
    {
      uint8_t* tData = ctx->tData + (ssize_t)recordList->pos * tStride;
      uint8_t* dp = dData;
      uint count = recordList->count;

      uint x = dw;
      uint i = 0;

      if (((size_t)dp & 0xF) == 0)
        goto _UnboundLarge;
      i = 16 - ((uint)(size_t)dp & 0xF);

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
      while (x >= 16)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        __m128i xmmAcc1 = xmmHalf;
        __m128i xmmAcc2 = xmmHalf;
        __m128i xmmAcc3 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixelLo0;
          __m128i xmmPixelLo1;

          __m128i xmmPixelHi0;
          __m128i xmmPixelHi1;

          __m128i xmmWeight;

          Acc::m128iLoad4(xmmWeight, wp);

          Acc::m128iLoad8(xmmPixelLo0, tp + 0);
          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iLoad8(xmmPixelLo1, tp + 8);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
          Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo1, xmmPixelLo1);

          Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);
          Acc::m128iCopy(xmmPixelHi1, xmmPixelLo1);

          Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight);
          Acc::m128iMulLoPI16(xmmPixelLo1, xmmPixelLo1, xmmWeight);

          Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight);
          Acc::m128iMulHiPI16(xmmPixelHi1, xmmPixelHi1, xmmWeight);

          Acc::m128iCopy(xmmWeight, xmmPixelLo0);
          Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
          Acc::m128iUnpackPI32FromPI16Hi(xmmWeight, xmmWeight, xmmPixelHi0);

          Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
          Acc::m128iAddPI32(xmmAcc1, xmmAcc1, xmmWeight);

          Acc::m128iCopy(xmmWeight, xmmPixelLo1);
          Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo1, xmmPixelLo1, xmmPixelHi1);
          Acc::m128iUnpackPI32FromPI16Hi(xmmWeight, xmmWeight, xmmPixelHi1);

          Acc::m128iAddPI32(xmmAcc2, xmmAcc2, xmmPixelLo1);
          Acc::m128iAddPI32(xmmAcc3, xmmAcc3, xmmWeight);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPI32<8>(xmmAcc0, xmmAcc0);
        Acc::m128iRShiftPI32<8>(xmmAcc1, xmmAcc1);
        Acc::m128iRShiftPI32<8>(xmmAcc2, xmmAcc2);
        Acc::m128iRShiftPI32<8>(xmmAcc3, xmmAcc3);

        Acc::m128iPackPI16FromPI32(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iPackPI16FromPI32(xmmAcc2, xmmAcc2, xmmAcc3);
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0, xmmAcc2);
        Acc::m128iStore16a(dp, xmmAcc0);

        dp += 16;
        tData += 16;

        x -= 16;
      }

      while (x >= 8)
      {
        const uint8_t* tp = tData;
        const int32_t* wp = weightList;

        __m128i xmmAcc0 = xmmHalf;
        __m128i xmmAcc1 = xmmHalf;

        for (uint j = count; j; j--)
        {
          __m128i xmmPixelLo0;
          __m128i xmmPixelHi0;

          __m128i xmmWeight;

          Acc::m128iLoad4(xmmWeight, wp);

          Acc::m128iLoad8(xmmPixelLo0, tp);
          Acc::m128iShufflePI16Lo<0, 0, 0, 0>(xmmWeight, xmmWeight);

          Acc::m128iUnpackPI16FromPI8Lo(xmmPixelLo0, xmmPixelLo0);
          Acc::m128iShufflePI32<0, 0, 0, 0>(xmmWeight, xmmWeight);
          Acc::m128iCopy(xmmPixelHi0, xmmPixelLo0);

          Acc::m128iMulLoPI16(xmmPixelLo0, xmmPixelLo0, xmmWeight);
          Acc::m128iMulHiPI16(xmmPixelHi0, xmmPixelHi0, xmmWeight);

          Acc::m128iCopy(xmmWeight, xmmPixelLo0);
          Acc::m128iUnpackPI32FromPI16Lo(xmmPixelLo0, xmmPixelLo0, xmmPixelHi0);
          Acc::m128iUnpackPI32FromPI16Hi(xmmWeight, xmmWeight, xmmPixelHi0);

          Acc::m128iAddPI32(xmmAcc0, xmmAcc0, xmmPixelLo0);
          Acc::m128iAddPI32(xmmAcc1, xmmAcc1, xmmWeight);

          tp += tStride;
          wp += 1;
        }

        Acc::m128iRShiftPI32<8>(xmmAcc0, xmmAcc0);
        Acc::m128iRShiftPI32<8>(xmmAcc1, xmmAcc1);

        Acc::m128iPackPI16FromPI32(xmmAcc0, xmmAcc0, xmmAcc1);
        Acc::m128iPackPU8FromPU16(xmmAcc0, xmmAcc0);
        Acc::m128iStore8(dp, xmmAcc0);

        dp += 8;
        tData += 8;

        x -= 8;
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

static void FOG_CDECL ImageResizeContext_doVertical_RGB24_SSE2(ImageResizeContext* ctx)
{
  ImageResizeContext_doVertical_Bytes_SSE2(ctx, 3);
}

static void FOG_CDECL ImageResizeContext_doVertical_A8_SSE2(ImageResizeContext* ctx)
{
  ImageResizeContext_doVertical_Bytes_SSE2(ctx, 1);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImageResize_init_SSE2(ImageResizeApi* api)
{
  api->doHorizontal[IMAGE_FORMAT_PRGB32] = ImageResizeContext_doHorizontal_PRGB32_SSE2;
  api->doHorizontal[IMAGE_FORMAT_XRGB32] = ImageResizeContext_doHorizontal_XRGB32_SSE2;

  api->doVertical[IMAGE_FORMAT_PRGB32] = ImageResizeContext_doVertical_PRGB32_SSE2;
  api->doVertical[IMAGE_FORMAT_XRGB32] = ImageResizeContext_doVertical_XRGB32_SSE2;
  api->doVertical[IMAGE_FORMAT_RGB24 ] = ImageResizeContext_doVertical_RGB24_SSE2;
  api->doVertical[IMAGE_FORMAT_A8    ] = ImageResizeContext_doVertical_A8_SSE2;
}

} // Fog namespace
