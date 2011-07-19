// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_RENDER_RENDERUTIL_P_H
#define _FOG_G2D_RENDER_RENDERUTIL_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/G2d/Render/RenderConstants_p.h>

namespace Fog {
namespace RenderUtil {

//! @addtogroup Fog_G2d_Render
//! @{

// ============================================================================
// [Fog::RenderUtil]
// ============================================================================

static FOG_INLINE uint32_t getCompatFormat(uint32_t dstFormat, uint32_t srcFormat)
{
  FOG_ASSERT(dstFormat <= IMAGE_FORMAT_COUNT);
  FOG_ASSERT(srcFormat <= IMAGE_FORMAT_COUNT);

  return _g2d_render_compatibleFormat[dstFormat][srcFormat].srcFormat;
}

static FOG_INLINE uint32_t getCompatVBlitId(uint32_t dstFormat, uint32_t srcFormat)
{
  FOG_ASSERT(dstFormat <= IMAGE_FORMAT_COUNT);
  FOG_ASSERT(srcFormat <= IMAGE_FORMAT_COUNT);

  return _g2d_render_compatibleFormat[dstFormat][srcFormat].vblitId;
}

static FOG_INLINE bool isCompositeCoreOperator(uint32_t compositingOperator)
{
  return compositingOperator < RENDER_COMPOSITE_CORE_COUNT;
}

static FOG_INLINE bool isCompositeExtOperator(uint32_t compositingOperator)
{
  return compositingOperator >= RENDER_COMPOSITE_CORE_COUNT;
}

// TODO: Remove
#if 0
//! @brief Extend 8-bit mask to 32-bit value.
static FOG_INLINE uint32_t extendMask8(uint32_t msk0) { return msk0 * 0x01010101; }

//! @brief Get whether the msk0 mask is 32-bit extended mask (used in assertions).
static FOG_INLINE bool isMask8Extended(uint32_t msk0)
{
  return ((msk0 & 0x000000FF) == ((msk0 & 0x0000FF00) >>  8) ||
          (msk0 & 0x000000FF) == ((msk0 & 0x00FF0000) >> 16) ||
          (msk0 & 0x000000FF) == ((msk0 & 0xFF000000) >> 24) );
}
#endif

//! @}

} // RenderUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_RENDER_RENDERUTIL_P_H
