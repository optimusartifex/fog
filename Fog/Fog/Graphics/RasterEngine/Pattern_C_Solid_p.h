// [Fog-Graphics Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// For some IDEs to enable code-assist.
#include <Fog/Core/Build.h>

#if defined(FOG_IDE)
#include <Fog/Graphics/RasterEngine/Defs_C_p.h>
#include <Fog/Graphics/RasterEngine/Dib_C_p.h>
#endif // FOG_IDE

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine::C - Pattern - Solid]
// ============================================================================

//! @internal
struct FOG_HIDDEN PatternSolidC
{
  // --------------------------------------------------------------------------
  // [Pattern - Solid - Init / Destroy]
  // --------------------------------------------------------------------------

  static err_t FOG_FASTCALL init(
    RasterPattern* ctx, uint32_t argb)
  {
    // RasterSolid fill is always using 32-bit format. We degrade to XRGB32 if alpha
    // channel is 0xFF (fully opaque).
    ctx->format = RasterUtil::isAlpha0xFF_ARGB32(argb)
      ? IMAGE_FORMAT_XRGB32
      : IMAGE_FORMAT_PRGB32;
    ctx->depth = 32;
    ctx->bytesPerPixel = 4;

    ctx->solid.argb = argb;
    ctx->solid.prgb = ColorUtil::premultiply(argb);
    ctx->fetch = fetch;
    ctx->destroy = destroy;

    ctx->initialized = true;
    return ERR_OK;
  }

  static void FOG_FASTCALL destroy(
    RasterPattern* ctx)
  {
    FOG_ASSERT(ctx->initialized);
    ctx->initialized = false;
  }

  // --------------------------------------------------------------------------
  // [Pattern - Solid - Fetch]
  // --------------------------------------------------------------------------

  static void FOG_FASTCALL fetch(
    const RasterPattern* ctx, Span* span, uint8_t* buffer, int y, uint32_t mode)
  {
    PatternCoreC::fetch_solid(span, buffer, mode, ctx->solid.prgb);
  }
};

} // RasterEngine namespace
} // Fog namespace
