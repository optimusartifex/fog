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
#include <Fog/Core/Kernel/Application.h>

// ============================================================================
// [Fog::All - Library Initializers]
// ============================================================================

static size_t _fog_init_counter;

FOG_CAPI_DECLARE void _fog_init(void)
{
  using namespace Fog;

  if (++_fog_init_counter != 1)
    return;

  // These two must be initialized first, because some other initializers might
  // depend on CPU features or need to create Lock to protect global data.
  Cpu_init();
  Lock_init();

  // [Core/Memory]
  MemOps_init();                 // Depends on Cpu.
  MemMgr_init();

  // [Core/Memory]
  MemBlockAllocator_init();
  MemZoneAllocator_init();
  MemPool_init();

  // [Core/Threading]
  ThreadLocal_init();
  ThreadEvent_init();
  ThreadCondition_init();

  // [Core/Math]
  Math_init();                   // Depends on Cpu.

  // [Core/Tools]
  CharUtil_init();
  String_init();
  Logger_init();

  Var_init();
  RegExp_init();

  HashUtil_init();
  Hash_init();
  List_init();

  StringUtil_init();
  StringUtil_init_dtoa();

  TextCodec_init();
  Locale_init();
  InternedString_init();          // Depends on String and Lock.

  Time_init();                   // Depends on Lock.
  Date_init();

  Random_init();

  // [Core/OS]
  Environment_init();

#if defined(FOG_OS_WINDOWS)
  WinUtil_init();
  WinCOM_init();
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_MAC)
  MacUtil_init();
#endif // FOG_OS_MAC

  OSInfo_init();
  OSUtil_init();
  Library_init();
  UserUtil_init();

  FileInfo_init();
  FileMapping_init();
  FilePath_init();
  FileUtil_init();
  DirIterator_init();

  // [Core/Tools]
  Stream_init();
  XmlEntity_init();

  // [Core/Threading]
  EventLoop_init();
  Thread_init();                 // Depends on EventLoop.
  ThreadPool_init();

  // [Core/Kernel]
  MemGCAllocator_init();

  Object_init();                 // Depends on String.
  EventLoopObserverList_init();  // Depends on List<>.
  Application_init();

  // [G2d/Tools]
  Dpi_init();
  Matrix_init();
  Region_init();

#if defined(FOG_OS_WINDOWS)
  Region_init_win();
#endif // FOG_OS_WINDOWS

  RegionUtil_init();

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
  PathStroker_init();
  PathInfo_init();

  // [G2d/Source]
  Color_init();
  ColorStopList_init();
  Gradient_init();
  Pattern_init();

  // [G2d/Imaging]
  ImageFormatDescription_init();
  ImagePalette_init();
  Image_init();

#if defined(FOG_OS_WINDOWS)
  Image_init_win();
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_MAC)
  Image_init_mac();
#endif // FOG_OS_MAC

  ImageResize_init();
  ImageConverter_init();
  ImageFilter_init();
  ImageCodecProvider_init();

  FeColorLutArray_init();
  FeColorLut_init();
  FeColorMatrix_init();
  FeComponentFunction_init();
  FeComponentTransfer_init();

  FeBlur_init();
  FeConvolveMatrix_init();
  FeConvolveSeparable_init();
  FeMorphology_init();

  // [G2d/Painting]
  RasterOps_init();
  Rasterizer_init();
  PaintDeviceInfo_init();

  NullPaintEngine_init();
  RasterPaintEngine_init();

  // [G2d/Text]
  GlyphBitmap_init();
  GlyphOutline_init();
  Font_init();
  FontManager_init();
}

FOG_CAPI_DECLARE void _fog_fini(void)
{
  using namespace Fog;

  if (--_fog_init_counter != 0)
    return;

  // [G2d/Text]
  FontManager_fini();
  Font_fini();
  GlyphOutline_fini();
  GlyphBitmap_fini();

  // [G2d/Imaging]
  ImageCodecProvider_fini();

  // [Core/Application]
  Application_fini();

  // [Core/Kernel]
  Object_fini();

  // [Core/Threading]
  ThreadPool_fini();
  Thread_fini();

  // [Core/Streaming]
  Stream_fini();

  // [Core/OS]
  Library_fini();

  // [Core/Tools]
  Locale_fini();
  InternedString_fini();
  TextCodec_fini();
  Logger_fini();

  // [Core/Threading]
  ThreadLocal_fini();

  // [Core/Memory]
  MemMgr_fini();
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
    // means that fog_application_terminated is zero). This is needed because
    // for example a Fog::ThreadPool might need to release all threads, but an 
    // assertion can be raised by the thread owned by it.
    if (fog_application_terminated != 0)
      return;

    _fog_fini();
  }
};

static _FogInitHelper _fog_init_helper;

#if defined(FOG_BUILD_STATIC)
FOG_CAPI_DECLARE void _fog_init_static(void)
{
  // Nothing to do here.
}
#endif // FOG_BUILD_STATIC
