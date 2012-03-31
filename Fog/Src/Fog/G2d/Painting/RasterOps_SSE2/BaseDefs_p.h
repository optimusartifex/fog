// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_PAINTING_RASTEROPS_SSE2_BASEDEFS_P_H
#define _FOG_G2D_PAINTING_RASTEROPS_SSE2_BASEDEFS_P_H

// [Dependencies]
#include <Fog/G2d/Acc/AccSse.h>
#include <Fog/G2d/Acc/AccSse2.h>

// [Dependencies - RasterOps_C]
#include <Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h>

// [Dependencies - RasterOps_SSE2]
#include <Fog/G2d/Painting/RasterOps_SSE2/BaseAccess_p.h>

// ============================================================================
// [FOG_BLIT_LOOP - 32x4 - 32-bits per pixel, 4 pixels in a main loop]
// ============================================================================

#define FOG_BLIT_LOOP_32x4_SSE2_INIT() \
  FOG_ASSUME(w > 0); \
  int _tmpWidth = w;

#define FOG_BLIT_LOOP_32x4_SSE2_ONE_BEGIN(_Group_) \
  \
  if (w >= 7) \
  { \
    _tmpWidth = (int)(intptr_t)dst & 15; \
    \
    if (_tmpWidth == 0) \
      goto _##_Group_##_TryMain; \
    \
    _tmpWidth = 4 - (_tmpWidth >> 2); \
    FOG_ASSERT(_tmpWidth < 4); \
    \
  } \
  \
  w -= _tmpWidth; \
  \
_##_Group_##_OneBegin: \
  if (_tmpWidth & 1) \
  {

#define FOG_BLIT_LOOP_32x4_SSE2_ONE_END(_Group_) \
    _tmpWidth--; \
  }

#define FOG_BLIT_LOOP_32x4_SSE2_TWO_BEGIN(_Group_) \
  while (_tmpWidth) \
  {

#define FOG_BLIT_LOOP_32x4_SSE2_TWO_END(_Group_) \
    _tmpWidth -= 2; \
  } \
  \
  if (w == 0) \
    goto _##_Group_##_End; \
  \
_##_Group_##_TryMain:

#define FOG_BLIT_LOOP_32x4_SSE2_MAIN_BEGIN(_Group_) \
  FOG_ASSERT(w >= 4); \
  \
  _tmpWidth = w; \
  w >>= 2; \
  \
  for (;;) {

#define FOG_BLIT_LOOP_32x4_SSE2_MAIN_CONTINUE(_Group_) \
    if (--w == 0) break;

#define FOG_BLIT_LOOP_32x4_SSE2_MAIN_END(_Group_) \
    if (--w == 0) break; \
  } \
  \
  _tmpWidth &= 3; \
  if (_tmpWidth != 0) \
    goto _##_Group_##_OneBegin; \
  \
_##_Group_##_End: \
  ;















#define FOG_BLIT_LOOP_32x8_SSE2_INIT() \
  FOG_ASSUME(w > 0); \
  int _tmpWidth = w;

#define FOG_BLIT_LOOP_32x8_SSE2_ONE_BEGIN(_Group_) \
  \
  if (w >= 11) \
  { \
    _tmpWidth = (int)(intptr_t)dst & 15; \
    \
    if (_tmpWidth == 0) \
      goto _##_Group_##_TryMain; \
    \
    _tmpWidth = 4 - (_tmpWidth >> 2); \
    FOG_ASSERT(_tmpWidth < 4); \
    \
  } \
  \
  w -= _tmpWidth; \
  \
_##_Group_##_OneBegin: \
  if (_tmpWidth & 1) \
  {

#define FOG_BLIT_LOOP_32x8_SSE2_ONE_END(_Group_) \
    _tmpWidth--; \
  }

#define FOG_BLIT_LOOP_32x8_SSE2_TWO_BEGIN(_Group_) \
  while (_tmpWidth) \
  {

#define FOG_BLIT_LOOP_32x8_SSE2_TWO_END(_Group_) \
    _tmpWidth -= 2; \
  } \
  \
  if (w == 0) \
    goto _##_Group_##_End; \
  \
_##_Group_##_TryMain:

#define FOG_BLIT_LOOP_32x8_SSE2_MAIN_BEGIN(_Group_) \
  FOG_ASSERT(w >= 8); \
  \
  _tmpWidth = w; \
  w >>= 3; \
  \
  for (;;) {

#define FOG_BLIT_LOOP_32x8_SSE2_MAIN_CONTINUE(_Group_) \
    if (--w == 0) break;

#define FOG_BLIT_LOOP_32x8_SSE2_MAIN_END(_Group_) \
    if (--w == 0) break; \
  } \
  \
  _tmpWidth &= 7; \
  if (_tmpWidth != 0) \
    goto _##_Group_##_OneBegin; \
  \
_##_Group_##_End: \
  ;

// [Guard]
#endif // _FOG_G2D_PAINTING_RASTEROPS_SSE2_BASEDEFS_P_H
