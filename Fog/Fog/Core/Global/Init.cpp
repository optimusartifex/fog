// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>

// ============================================================================
// [Fog::All - Library Initializers]
// ============================================================================

static size_t _fog_init_counter;

FOG_CAPI_DECLARE void _fog_init(void)
{
  using namespace Fog;

  if (++_fog_init_counter != 1)
    return;

  // [Core/Cpu]
  Cpu_init();

  // [Core/Threading]
  Lock_init(),

  // [Core/Error]
  Error_init();

  // [Core/Memory]
  MemoryOps_init();              // Depends on Cpu.
  MemoryAlloc_init();
  MemoryCleanup_init();
  MemoryManager_init();

  // [Core/Threading]
  ThreadLocal_init();

  // [Core/Math]
  Math_init();                   // Depends on Cpu.

  // [Core/DateTime]
  Time_init();                   // Depends on Lock.
  Date_init();

  // [Core/Collection]
  List_init();
  Hash_init();

  // [Core/Tools]
  ByteArray_init();
  String_init();
  TextCodec_init();
  ManagedString_init();          // Depends on String and Lock.
  Strings_init();                // Depends on StringCache.
  Locale_init();                 // Depends on TextCodec.

  // [Core/Variant]
  Var_init();

  // [Core/Library]
  Library_init();

  // [Core/OS]
  OS_init();                     // Depends on String and TextCodec.

  // [Core/Streaming]
  Stream_init();

  // [Core/Threading]
  Thread_init();
  ThreadPool_init();

  // [Core/Object]
  Object_init();                 // Depends on String.

  // [Core/Xml
  XmlEntity_init();

  // [Core/Application]
  Application_init();

  // [G2d/Geometry]
  Line_init();
  QBezier_init();
  CBezier_init();
  Arc_init();
  Circle_init();
  Ellipse_init();
  Chord_init();
  Pie_init();
  Round_init();
  Triangle_init();
  Path_init();
  Shape_init();

  Transform_init();
  PathClipper_init();

  // [G2d/Tools]
  Region_init();
  Matrix_init();

  // [G2d/Painting]
  Rasterizer_init();

  // [G2d/Render]
  Render_init();

  // [G2d/Source]
  Color_init();
  ColorStopList_init();
  Pattern_init();

  // [G2d/Imaging]
  ImagePalette_init();
  ImageFormatDescription_init();
  Image_init();
  ImageConverter_init();
  ImageCodecProvider_init();
  ImageEffect_init();

  // [G2d/Painting]
  PaintDeviceInfo_init();
  Painter_initNull();
  Painter_initRaster();

  // [G2d/Text]
  GlyphBitmap_init();
  GlyphOutline_init();
  FontManager_init();
}

FOG_CAPI_DECLARE void _fog_fini(void)
{
  using namespace Fog;

  if (--_fog_init_counter != 0)
    return;

  // [G2d/Text]
  FontManager_fini();
  GlyphOutline_fini();
  GlyphBitmap_fini();

  // [G2d/Imaging]
  ImageCodecProvider_fini();

  // [Core/Application]
  Application_fini();

  // [Core/Object]
  Object_fini();

  // [Core/Threading]
  ThreadPool_fini();
  Thread_fini();

  // [Core/Streaming]
  Stream_fini();

  // [Core/OS]
  OS_fini();

  // [Core/Library]
  Library_fini();

  // [Core/Variant]
  Var_fini();

  // [Core/Tools]
  ManagedString_fini();
  TextCodec_fini();

  // [Core/Threading]
  ThreadLocal_fini();

  // [Core/Memory]
  MemoryAlloc_fini();
}

// ============================================================================
// [_FogInitHelper]
// ============================================================================

// Static instance of class that will call _fog_init() and _fog_fini().
struct FOG_NO_EXPORT _FogInitHelper
{
  _FogInitHelper()
  {
    _fog_init();
  }

  ~_FogInitHelper()
  {
    // We shutdown Fog only in case that application terminated normally (this
    // means that fog_failed is zero). This is needed because for example a
    // Fog::ThreadPool might need to release all threads, but an assertion can
    // be raised by the thread owned by it.
    if (fog_failed == 0)
    {
      _fog_fini();
    }
  }
};

static _FogInitHelper _fog_init_helper;

#if defined(FOG_BUILD_STATIC)
FOG_CAPI_DECLARE void _fog_init_static(void)
{
  // Nothing to do here.
}
#endif // FOG_BUILD_STATIC
