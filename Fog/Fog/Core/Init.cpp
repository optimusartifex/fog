// [Fog Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Std.h>

// ============================================================================
// [Library Initializers]
// ============================================================================

// #define FOG_DEBUG_INIT

#define FOG_INIT_FUNC(entry) \
  FOG_INIT_EXTERN err_t entry##_init(void); \
  FOG_INIT_EXTERN void entry##_shutdown(void)

// Fog-Core
FOG_INIT_FUNC(fog_std);

FOG_INIT_FUNC(fog_cpuinfo);
FOG_INIT_FUNC(fog_lock);
FOG_INIT_FUNC(fog_memory);
FOG_INIT_FUNC(fog_math);
FOG_INIT_FUNC(fog_list);
FOG_INIT_FUNC(fog_bytearray);
FOG_INIT_FUNC(fog_string);
FOG_INIT_FUNC(fog_managedstring);
FOG_INIT_FUNC(fog_textcodec);
FOG_INIT_FUNC(fog_hash);
FOG_INIT_FUNC(fog_strings);
FOG_INIT_FUNC(fog_locale);
FOG_INIT_FUNC(fog_value);
FOG_INIT_FUNC(fog_library);
FOG_INIT_FUNC(fog_os);
FOG_INIT_FUNC(fog_stream);
FOG_INIT_FUNC(fog_thread);
FOG_INIT_FUNC(fog_threadpool);
FOG_INIT_FUNC(fog_object);
FOG_INIT_FUNC(fog_application);

// Fog-Graphics
FOG_INIT_FUNC(fog_rasterizer);
FOG_INIT_FUNC(fog_rasterengine);
FOG_INIT_FUNC(fog_palette);
FOG_INIT_FUNC(fog_path);
FOG_INIT_FUNC(fog_pathutil);
FOG_INIT_FUNC(fog_region);
FOG_INIT_FUNC(fog_image);
FOG_INIT_FUNC(fog_imageio);
FOG_INIT_FUNC(fog_imagefilter);
FOG_INIT_FUNC(fog_glyph);
FOG_INIT_FUNC(fog_glyphset);
FOG_INIT_FUNC(fog_pattern);
FOG_INIT_FUNC(fog_font);
FOG_INIT_FUNC(fog_painter);

// Fog-Gui
// reserved...

// Fog-Svg
// reserved...

// Fog-Xml
FOG_INIT_FUNC(fog_xmlentity);

// ============================================================================
// [Fog::Init]
// ============================================================================

// #define FOG_DEBUG_INIT

struct FogInitEntry
{
  err_t (*init)(void);
  void (*shutdown)(void);
#if defined(FOG_DEBUG_INIT)
  const char* name;
#endif // FOG_DEBUG_INIT
};

#if defined(FOG_DEBUG_INIT)
#define INIT_ENTRY(entry) { entry##_init, entry##_shutdown, #entry }
#else
#define INIT_ENTRY(entry) { entry##_init, entry##_shutdown }
#endif

static const FogInitEntry fog_init_entries[] =
{
  // Fog-Core
  INIT_ENTRY(fog_cpuinfo),          // No dependency
  INIT_ENTRY(fog_lock),             // No dependency
  INIT_ENTRY(fog_memory),           // Depends to Cpu and Lock
  INIT_ENTRY(fog_math),             // Depends to Cpu
  INIT_ENTRY(fog_list),             // No dependency
  INIT_ENTRY(fog_bytearray),        // No dependency
  INIT_ENTRY(fog_string),           // No dependency
  INIT_ENTRY(fog_textcodec),        // No dependency
  INIT_ENTRY(fog_hash),             // No dependency
  INIT_ENTRY(fog_managedstring),    // Depends to String and Lock
  INIT_ENTRY(fog_strings),          // Depends to StringCache
  INIT_ENTRY(fog_locale),           // Depends to TextCodec
  INIT_ENTRY(fog_value),            // No dependency
  INIT_ENTRY(fog_library),          // No dependency
  INIT_ENTRY(fog_os),               // Depends to String and TextCodec
  INIT_ENTRY(fog_stream),           // No dependency
  INIT_ENTRY(fog_thread),           // No dependency
  INIT_ENTRY(fog_threadpool),       // No dependency
  INIT_ENTRY(fog_object),           // Depends to String
  INIT_ENTRY(fog_application),      // Depends to Many

  // Fog-Graphics
  INIT_ENTRY(fog_rasterizer),
  INIT_ENTRY(fog_rasterengine),
  INIT_ENTRY(fog_palette),
  INIT_ENTRY(fog_path),
  INIT_ENTRY(fog_pathutil),
  INIT_ENTRY(fog_region),
  INIT_ENTRY(fog_image),
  INIT_ENTRY(fog_imageio),
  INIT_ENTRY(fog_imagefilter),
  INIT_ENTRY(fog_glyph),            // Depends to Image
  INIT_ENTRY(fog_glyphset),
  INIT_ENTRY(fog_pattern),
  INIT_ENTRY(fog_font),
  INIT_ENTRY(fog_painter),

  // Fog-Gui

  // Fog-Xml
  INIT_ENTRY(fog_xmlentity)

  // Fog-Svg
};

FOG_CAPI_DECLARE err_t fog_init(void)
{
  using namespace Fog;

  err_t err = ERR_OK;

  const FogInitEntry* entryCur = fog_init_entries;
  const FogInitEntry* entryEnd = entryCur + FOG_ARRAY_SIZE(fog_init_entries);

  // This must be initialized, because FOG_DEBUG_INIT can use
  // fog_stderr_msg()
  fog_std_init();

  for (; entryCur != entryEnd; entryCur++)
  {
#if defined(FOG_DEBUG_INIT)
    fog_stderr_msg(NULL, "fog_init", "%s", entryCur->name);
#endif // FOG_DEBUG_INIT
    if ( (err = entryCur->init()) )
    {
#if defined(FOG_DEBUG_INIT)
      fog_stderr_msg(NULL, "fog_init", "%s - failed (error=%d)", entryCur->name, err);
#endif // FOG_DEBUG_INIT
      goto fail;
    }
#if defined(FOG_DEBUG_INIT)
    else
    {
      fog_stderr_msg(NULL, "fog_init", "%s - ok", entryCur->name);
    }
#endif // FOG_DEBUG_INIT
  }

  return err;

fail:
  entryEnd = fog_init_entries - 1;
  while (--entryCur != entryEnd) entryCur->shutdown();
  return err;
}

FOG_CAPI_DECLARE void fog_shutdown(void)
{
  using namespace Fog;

  const FogInitEntry* entryEnd = fog_init_entries - 1;
  const FogInitEntry* entryCur = entryEnd + FOG_ARRAY_SIZE(fog_init_entries);

  for (; entryCur != entryEnd; entryCur--)
  {
#if defined(FOG_DEBUG_INIT)
    fog_stderr_msg(NULL, "fog_shutdown", "%s", entryCur->name);
#endif // FOG_DEBUG_INIT
    entryCur->shutdown();
#if defined(FOG_DEBUG_INIT)
    fog_stderr_msg(NULL, "fog_shutdown", "%s - ok", entryCur->name);
#endif // FOG_DEBUG_INIT
  }

  // See comments in fog_init() for next line
  fog_std_shutdown();
}

// Static instance of class that will call fog_init() and
// FOG_shutdown().
struct FOG_HIDDEN FogAutoInit
{
  err_t err;

  FogAutoInit()
  {
    if ( (err = fog_init()) )
    {
      fog_stderr_msg(NULL, "FogAutoInit", "Failed to initialize Fog library, error=%d", err);
      exit(Fog::ERR_OK);
    }
  }

  ~FogAutoInit()
  {
    // We shutdown only if there is no error and fog_failed is zero. This is
    // because for example a ThreadPool wants to release all threads, but an
    // assertion can be raised by the thread owned by ThreadPool.
    //
    // So if we failed, never call fog_shutdown() and exit quickly.
    if (err == Fog::ERR_OK && fog_failed == 0) fog_shutdown();
  }
};

static FogAutoInit fog_auto_init;
