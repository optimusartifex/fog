// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Defines]
// ============================================================================

#define READ_8(ptr)  (((const uint8_t *)(ptr))[0])
#define READ_16(ptr) (((const uint16_t*)(ptr))[0])
#define READ_32(ptr) (((const uint32_t*)(ptr))[0])

// ============================================================================
// [Fog::RasterEngine::C - C_BLIT]
// ============================================================================

// Macros to help creating blitters functions.
//
// Purpose of these macros is to simplify making such functions. This inner loop
// is quite optimized so don't worry about it. You need only to create code that
// will look like:
//
//    C_BLIT_SPAN8_BEGIN(bytes_per_pixel)
//
//    C_BLIT_SPAN8_CASE_CMASK_GENERIC()
//
//      or
//
//    C_BLIT_SPAN8_CASE_CMASK_OPAQUE()
//    C_BLIT_SPAN8_CASE_CMASK_ALPHA()
//    {
//      'dst'  - Destination pointer where to write pixels.
//      'msk0' - Constant pixels mask (you can check for common case that
//               is 0xFF, never check for 0x00 - this never happens here).
//               msk0 is only accessible when using GENERIC or ALPHA suffix.
//      'w'    - Count of pixels to write (width).
//    }
//
//    C_BLIT_SPAN8_CASE_VMASK_A_GENERIC() 
//
//      or
//
//    C_BLIT_SPAN8_CASE_VMASK_A_DENSE() 
//    C_BLIT_SPAN8_CASE_VMASK_A_SPARSE() 
//    {
//      'dst'  - Destination pointer where to write pixels.
//      'msk'  - Pointer to alpha-mask array. Omit checks for nulls
//               and full opaque pixels here, this happens rarely.
//      'w'    - Count of pixels to write (width).
//    }
//
//    C_BLIT_SPAN8_CASE_VMASK_ARGB_GENERIC()
//
//      or
//
//    C_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE()
//    C_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE()
//    {
//      'dst'  - Destination pointer where to write pixels.
//      'msk'  - Pointer to argb-mask array. Omit checks for nulls
//               and full opaque pixels here, this happens rarely.
//      'w'    - Count of pixels to write (width).
//    }
//
//    C_BLIT_SPAN8_END()
//
// The V_BLIT_SPAN8_XXX functions are used when there is variable-source. The
// 'src' variable should be used inside of 'CASE' sections.

//! @brief CBlit - Span8 - Begin.
#define C_BLIT_SPAN8_BEGIN(BPP) \
  uint8_t* dstBase = dst; \
  \
  do { \
    uint x = (uint)span->getX1(); \
    int w = (int)((uint)span->getX2() - x); \
    FOG_ASSERT(w > 0); \
    \
    dst = dstBase + x * BPP; \
    const uint8_t* _msk = reinterpret_cast<const Span8*>(span)->getMaskPtr(); \
    \
    switch (span->getType()) \
    {

//! @brief CBlit - Span8 - CMask - Generic (for both Opaque and Alpha).
#define C_BLIT_SPAN8_CASE_CMASK_GENERIC() \
      case SPAN_TYPE_CMASK: \
      { \
        uint32_t msk0 = Span8::ptrToCMask(_msk); \
        FOG_ASSERT(msk0 <= 0xFF); \
        {

//! @brief CBlit - Span8 - CMask (OPAQUE).
#define C_BLIT_SPAN8_CASE_CMASK_OPAQUE() \
      case SPAN_TYPE_CMASK: \
      { \
        uint32_t msk0 = Span8::ptrToCMask(_msk); \
        FOG_ASSERT(msk0 <= 0xFF); \
        \
        if (msk0 == 0xFF) \
        {

//! @brief CBlit - Span8 - CMask (A).
#define C_BLIT_SPAN8_CASE_CMASK_ALPHA() \
        } \
        else \
        {

//! @brief CBlit - Span8 - VMask (A) - Generic (for both Dense and Sparse).
#define C_BLIT_SPAN8_CASE_VMASK_A_GENERIC() \
        } \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_A_DENSE: \
      case SPAN_TYPE_VMASK_A_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief CBlit - Span8 - VMask (A) - Dense.
#define C_BLIT_SPAN8_CASE_VMASK_A_DENSE() \
        } \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_A_DENSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief CBlit - Span8 - VMask (A) - Sparse.
#define C_BLIT_SPAN8_CASE_VMASK_A_SPARSE() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_A_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief CBlit - Span8 - VMask (ARGB) - Generic (for both Dense and Sparse).
#define C_BLIT_SPAN8_CASE_VMASK_ARGB_GENERIC() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_ARGB_DENSE: \
      case SPAN_TYPE_VMASK_ARGB_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief CBlit - Span8 - VMask (ARGB) - Sparse.
#define C_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_ARGB_DENSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief CBlit - Span8 - VMask (ARGB) - Sparse.
#define C_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_ARGB_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief CBlit - End.
#define C_BLIT_SPAN8_END() \
        break; \
      } \
    } \
  } while ((span = span->getNext()) != NULL);

// ============================================================================
// [Fog::RasterEngine::C - V_BLIT]
// ============================================================================

//! @brief VBlit - Span8 - Begin.
#define V_BLIT_SPAN8_BEGIN(BPP) \
  uint8_t* dstBase = dst; \
  \
  do { \
    uint x = (uint)span->getX1(); \
    int w = (int)((uint)span->getX2() - x); \
    FOG_ASSERT(w > 0); \
    \
    dst = dstBase + x * BPP; \
    const uint8_t* _msk = reinterpret_cast<const Span8*>(span)->getMaskPtr(); \
    const uint8_t* src = reinterpret_cast<const SpanExt8*>(span)->getData(); \
    \
    switch (span->getType()) \
    {

//! @brief VBlit - Span8 - CMask - Generic (for both Opaque and Alpha).
#define V_BLIT_SPAN8_CASE_CMASK_GENERIC() \
      case SPAN_TYPE_CMASK: \
      { \
        uint32_t msk0 = Span8::ptrToCMask(_msk); \
        FOG_ASSERT(msk0 <= 0xFF); \
        {

//! @brief VBlit - Span8 - CMask (OPAQUE).
#define V_BLIT_SPAN8_CASE_CMASK_OPAQUE() \
      case SPAN_TYPE_CMASK: \
      { \
        uint32_t msk0 = Span8::ptrToCMask(_msk); \
        FOG_ASSERT(msk0 <= 0xFF); \
        \
        if (msk0 == 0xFF) \
        {

//! @brief VBlit - Span8 - CMask (A).
#define V_BLIT_SPAN8_CASE_CMASK_ALPHA() \
        } \
        else \
        {

//! @brief VBlit - Span8 - VMask (A) - Generic (for both Dense and Sparse).
#define V_BLIT_SPAN8_CASE_VMASK_A_GENERIC() \
        } \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_A_DENSE: \
      case SPAN_TYPE_VMASK_A_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief VBlit - Span8 - VMask (A) - Dense.
#define V_BLIT_SPAN8_CASE_VMASK_A_DENSE() \
        } \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_A_DENSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief VBlit - Span8 - VMask (A) - Sparse.
#define V_BLIT_SPAN8_CASE_VMASK_A_SPARSE() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_A_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief VBlit - Span8 - VMask (ARGB) - Generic (for both Dense and Sparse).
#define V_BLIT_SPAN8_CASE_VMASK_ARGB_GENERIC() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_ARGB_DENSE: \
      case SPAN_TYPE_VMASK_ARGB_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief VBlit - Span8 - VMask (ARGB) - Sparse.
#define V_BLIT_SPAN8_CASE_VMASK_ARGB_DENSE() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_ARGB_DENSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief VBlit - Span8 - VMask (ARGB) - Sparse.
#define V_BLIT_SPAN8_CASE_VMASK_ARGB_SPARSE() \
        break; \
      } \
      \
      case SPAN_TYPE_VMASK_ARGB_SPARSE: \
      { \
        const uint8_t* msk = _msk;

//! @brief VBlit - End.
#define V_BLIT_SPAN8_END() \
        break; \
      } \
    } \
  } while ((span = span->getNext()) != NULL);

// ============================================================================
// [Fog::RasterEngine::C - P_FETCH]
// ============================================================================

// Example of fetch function:
//
// static void FOG_FASTCALL fetch(
//   RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
// {
//   // Initialize fetch function (some variables needed for span loop).
//   P_FETCH_SPAN8_INIT()
//
//   // Here place code that you need to initialize the fetcher. You can access
//   // these variables from here:
//   // 'x' - X position in target raster buffer
//   // 'y' - Y position in target raster buffer (you need inverted matrix to
//   //       X/Y to position in our pattern).
//   // 'w' - Width (number of pixels to fetch).
//   // 'dst' - Buffer
//   //
//   // This place should be used only for initialization, transforming X/Y
//   // to destination (texture) buffer, etc...
//
//   // Begin of span-loop.
//   P_FETCH_SPAN8_BEGIN()
//
//     // Here you must decide if you pass your own buffer to span (only 
//     // possible if mode is @c PATTERN_FETCH_CAN_USE_SRC). To set current
//     // buffer, use:
//
//     P_FETCH_SPAN8_SET_CURRENT()
//
//     // To set custom buffer (owned by pattern), use:
//
//     P_FETCH_SPAN8_SET_CUSTOM(yourCustomBuffer)
//
//     // If you used custom buffer you finished with this span, otherwise use
//     // 'dst' pointer (and increment it after each fetcher pixel). For 
//     // example:
//
//     do {
//       ((uint32_t*)dst)[0] = 0x00000000;
//     } while (--w);
//
//     // Here is place to tell fetcher how is the preferred way of handling
//     // "holes". Hole is space between two spans. If spans are neighbours,
//     // then hole is zero pixels width (this is common). But hole can be
//     // also one or more pixels width (this happens if this span->x2 is less
//     // than next span->x1).
//     //
//     // If you are not interested about holes, use simply:
//
//     P_FETCH_SPAN8_NEXT()
//
//     // Otherwise use:
//
//     P_FETCH_SPAN8_HOLE(
//     {
//       // Your code, you can use goto to skip to specific part in loop.
//       // If you are using 'x' then you must update it.
//       //
//       // Use 'hole' variable that contains number of pixels to skip.
//       x += hole;
//     })
//
//   // End of span-loop.
//   P_FETCH_SPAN8_END()
// }
//

#define P_FETCH_SPAN8_INIT() \
  uint8_t* dst = buffer; \
  \
  int x = (int)span->getX1(); \
  int w = (int)((uint)span->getX2() - (uint)x); \
  FOG_ASSERT(w > 0);

#define P_FETCH_SPAN8_BEGIN() \
  for (;;) \
  {

#define P_FETCH_SPAN8_SET_CURRENT() \
    reinterpret_cast<SpanExt8*>(span)->setData(dst);

#define P_FETCH_SPAN8_SET_CURRENT_AND_MERGE_NEIGHBOURS(__bpp__) \
    reinterpret_cast<SpanExt8*>(span)->setData(dst); \
    \
    { \
      Span* next = span->getNext(); \
      if (next && next->getX1() == span->getX2()) \
      { \
        do { \
          reinterpret_cast<SpanExt8*>(next)->setData(dst + (uint)w * __bpp__); \
          span = next; \
          w = (int)((uint)next->getX2() - (uint)x); \
          next = span->getNext(); \
        } while (next && next->getX1() == span->getX2()); \
      } \
    }

#define P_FETCH_SPAN8_SET_CUSTOM(__dst__) \
    FOG_ASSERT(mode == PATTERN_FETCH_CAN_USE_SRC); \
    reinterpret_cast<SpanExt8*>(span)->setData((uint8_t*)(__dst__));

#define P_FETCH_SPAN8_NEXT() \
    if ((span = span->getNext()) == NULL) break; \
    \
    x = (int)span->getX1(); \
    w = (int)((uint)span->getX2() - (uint)x); \
    FOG_ASSERT(w > 0);

#define P_FETCH_SPAN8_HOLE(__code__) \
    { \
      int endX = (int)span->getX2(); \
      if ((span = span->getNext()) == NULL) break; \
      \
      int startX = (int)span->getX1(); \
      FOG_ASSERT(endX <= startX); \
      \
      w = (int)((uint)span->getX2() - (uint)startX); \
      FOG_ASSERT(w > 0); \
      \
      if (endX != startX) \
      { \
        int hole = (startX - endX); \
        __code__ \
      } \
      \
    }

#define P_FETCH_SPAN8_END() \
  }

// ============================================================================
// [Fog::RasterEngine::C - C_LOOP - 8x4 - 8 bpp, 4 pixels per main loop]
// ============================================================================

#define C_LOOP_8x4_INIT() \
  FOG_ASSERT(w > 0);

#define C_LOOP_8x4_SMALL_BEGIN(__group__) \
  if (((sysuint_t)dst & 3) == 0) goto __group__##_small_skip; \
  \
__group__##_small_begin: \
  do {

#define C_LOOP_8x4_SMALL_END(__group__) \
    if (--w == 0) goto __group__##_end; \
  } while (((sysuint_t)dst & 3) != 0); \
  \
__group__##_small_skip: \
  ;

#define C_LOOP_8x4_MAIN_BEGIN(__group__) \
  w -= 4; \
  if (w < 0) goto __group__##_main_skip; \
  \
  do {

#define C_LOOP_8x4_MAIN_END(__group__) \
  } while ((w -= 4) >= 0); \
  \
__group__##_main_skip: \
  w += 4; \
  if (w != 0) goto __group__##_small_begin; \
  \
__group__##_end: \
  ;

// ============================================================================
// [Fog::RasterEngine::C - C_PATTERN_INTERPOLATE]
// ============================================================================

#if FOG_ARCH_BITS > 32

#define C_PATTERN_INTERPOLATE_32_2(DST, SRC0, WEIGHT0, SRC1, WEIGHT1) \
{ \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4 pixT1; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC1)); \
  \
  pixT0 *= (uint)(WEIGHT0); \
  pixT1 *= (uint)(WEIGHT1); \
  pixT0 += pixT1; \
  \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

#define C_PATTERN_INTERPOLATE_32_2_WITH_ZERO(DST, SRC0, WEIGHT0) \
{ \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  pixT0 *= (uint)(WEIGHT0); \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

#define C_PATTERN_INTERPOLATE_32_4(DST, SRC0, WEIGHT0, SRC1, WEIGHT1, SRC2, WEIGHT2, SRC3, WEIGHT3) \
{ \
  ByteUtil::byte1x4 pixT0; \
  ByteUtil::byte1x4 pixT1; \
  ByteUtil::byte1x4 pixT2; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT0, (SRC0)); \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC1)); \
  \
  pixT0 *= (uint)(WEIGHT0); \
  pixT1 *= (uint)(WEIGHT1); \
  pixT0 += pixT1; \
  \
  ByteUtil::byte1x4_unpack_0213(pixT1, (SRC2)); \
  ByteUtil::byte1x4_unpack_0213(pixT2, (SRC3)); \
  \
  pixT1 *= (uint)(WEIGHT2); \
  pixT2 *= (uint)(WEIGHT3); \
  pixT0 += pixT1; \
  pixT0 += pixT2; \
  \
  pixT0 &= FOG_UINT64_C(0xFF00FF00FF00FF00); \
  \
  DST = (uint32_t)((pixT0 >> 8) | (pixT0 >> 32)); \
}

#else

#define C_PATTERN_INTERPOLATE_32_2(DST, SRC0, WEIGHT0, SRC1, WEIGHT1) \
{ \
  ByteUtil::byte1x2 pixT0_0, pixT0_1; \
  ByteUtil::byte1x2 pixT1_0, pixT1_1; \
  \
  uint __weight; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT0_0, pixT0_1, (SRC0)); \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC1)); \
  \
  __weight = WEIGHT0; \
  pixT0_0 *= __weight; \
  pixT0_1 *= __weight; \
  \
  __weight = WEIGHT1; \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  pixT0_0 = (pixT0_0 >> 8) & 0x00FF00FF; \
  pixT0_1 = (pixT0_1     ) & 0xFF00FF00; \
  \
  DST = pixT0_0 | pixT0_1; \
}

#define C_PATTERN_INTERPOLATE_32_2_WITH_ZERO(DST, SRC0, WEIGHT0) \
{ \
  uint __weight; \
  \
  ByteUtil::byte1x2 pixT0_0, pixT0_1; \
  ByteUtil::byte2x2_unpack_0213(pixT0_0, pixT0_1, (SRC0)); \
  \
  __weight = WEIGHT0; \
  pixT0_0 *= __weight; \
  pixT0_1 *= __weight; \
  \
  pixT0_0 = (pixT0_0 >> 8) & 0x00FF00FF; \
  pixT0_1 = (pixT0_1     ) & 0xFF00FF00; \
  \
  DST = pixT0_0 | pixT0_1; \
}

#define C_PATTERN_INTERPOLATE_32_4(DST, SRC0, WEIGHT0, SRC1, WEIGHT1, SRC2, WEIGHT2, SRC3, WEIGHT3) \
{ \
  ByteUtil::byte1x2 pixT0_0, pixT0_1; \
  ByteUtil::byte1x2 pixT1_0, pixT1_1; \
  \
  uint __weight; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT0_0, pixT0_1, (SRC0)); \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC1)); \
  \
  __weight = (WEIGHT0); \
  pixT0_0 *= __weight; \
  pixT0_1 *= __weight; \
  \
  __weight = (WEIGHT1); \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC2)); \
  \
  __weight = (WEIGHT2); \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  ByteUtil::byte2x2_unpack_0213(pixT1_0, pixT1_1, (SRC3)); \
  \
  __weight = (WEIGHT3); \
  pixT1_0 *= __weight; \
  pixT1_1 *= __weight; \
  \
  pixT0_0 += pixT1_0; \
  pixT0_1 += pixT1_1; \
  \
  pixT0_0 = (pixT0_0 >> 8) & 0x00FF00FF; \
  pixT0_1 = (pixT0_1     ) & 0xFF00FF00; \
  \
  DST = pixT0_0 | pixT0_1; \
}

#endif // FOG_ARCH_BITS

} // RasterEngine namespace
} // Fog namespace
