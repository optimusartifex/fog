// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/CpuInfo.h>
#include <Fog/Graphics/RasterEngine_p.h>

namespace Fog {
namespace RasterEngine {

// ============================================================================
// [Fog::RasterEngine - Data]
// ============================================================================

const uint16_t linear_blur8_mul[255] =
{
  512, 512, 456, 512, 328, 456, 335, 512, 405, 328, 271, 456, 388, 335, 292, 512,
  454, 405, 364, 328, 298, 271, 496, 456, 420, 388, 360, 335, 312, 292, 273, 512,
  482, 454, 428, 405, 383, 364, 345, 328, 312, 298, 284, 271, 259, 496, 475, 456,
  437, 420, 404, 388, 374, 360, 347, 335, 323, 312, 302, 292, 282, 273, 265, 512,
  497, 482, 468, 454, 441, 428, 417, 405, 394, 383, 373, 364, 354, 345, 337, 328,
  320, 312, 305, 298, 291, 284, 278, 271, 265, 259, 507, 496, 485, 475, 465, 456,
  446, 437, 428, 420, 412, 404, 396, 388, 381, 374, 367, 360, 354, 347, 341, 335,
  329, 323, 318, 312, 307, 302, 297, 292, 287, 282, 278, 273, 269, 265, 261, 512,
  505, 497, 489, 482, 475, 468, 461, 454, 447, 441, 435, 428, 422, 417, 411, 405,
  399, 394, 389, 383, 378, 373, 368, 364, 359, 354, 350, 345, 341, 337, 332, 328,
  324, 320, 316, 312, 309, 305, 301, 298, 294, 291, 287, 284, 281, 278, 274, 271,
  268, 265, 262, 259, 257, 507, 501, 496, 491, 485, 480, 475, 470, 465, 460, 456,
  451, 446, 442, 437, 433, 428, 424, 420, 416, 412, 408, 404, 400, 396, 392, 388,
  385, 381, 377, 374, 370, 367, 363, 360, 357, 354, 350, 347, 344, 341, 338, 335,
  332, 329, 326, 323, 320, 318, 315, 312, 310, 307, 304, 302, 299, 297, 294, 292,
  289, 287, 285, 282, 280, 278, 275, 273, 271, 269, 267, 265, 263, 261, 259
};

const uint8_t linear_blur8_shr[255] =
{
  9 , 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
  17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
  19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
  21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
  22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};

// ============================================================================
// [Fog::RasterEngine - FunctionMap]
// ============================================================================

static FunctionMap functionMapData;

FunctionMap* functionMap;

} // RasterEngine namespace
} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_EXTERN void fog_rasterengine_init_c(void);
#if defined(FOG_ARCH_X86)
FOG_INIT_EXTERN void fog_rasterengine_init_mmx(void);
FOG_INIT_EXTERN void fog_rasterengine_init_mmx3dnow(void);
FOG_INIT_EXTERN void fog_rasterengine_init_mmxsse(void);
#endif // FOG_ARCH_X86
FOG_INIT_EXTERN void fog_rasterengine_init_sse2(void);
FOG_INIT_EXTERN void fog_rasterengine_init_ssse3(void);

FOG_INIT_DECLARE err_t fog_rasterengine_init(void)
{
  using namespace Fog;
  using namespace Fog::RasterEngine;

  functionMap = &functionMapData;

  // Install C optimized code (default).
  fog_rasterengine_init_c();

  // Install MMX/MMX3dNow/MMXSSE optimized code if supported.
  //
  // Install it only if x86 architecture is used. In 64-bit mode the SSE2
  // implementation is always available and it's better choice.
#if defined(FOG_ARCH_X86)
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_MMX  )) fog_rasterengine_init_mmx();
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_3dNow)) fog_rasterengine_init_mmx3dnow();
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_SSE  )) fog_rasterengine_init_mmxsse();
#endif // FOG_ARCH_X86

  // Install SSE2 optimized code if supported.
#if defined(FOG_HARDCODE_SSE2)
  fog_rasterengine_init_sse2();
#elif defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_SSE2)) fog_rasterengine_init_sse2();
#endif // SSE2

  // Install SSSE3 optimized code if supported.
#if defined(FOG_ARCH_X86) || defined(FOG_ARCH_X86_64)
  if (cpuInfo->hasFeature(CpuInfo::FEATURE_SSSE3)) fog_rasterengine_init_ssse3();
#endif // SSSE3

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_rasterengine_shutdown(void)
{
}
