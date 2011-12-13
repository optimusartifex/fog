// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/G2d/OS/WinGdiPlus.h>

namespace Fog {

// ===========================================================================
// [Fog::GdipLibrary]
// ===========================================================================

GdipLibrary::GdipLibrary() :
  _gdipToken((ULONG_PTR)NULL)
{
  static const char symbolNames[] =
    "GdiplusStartup\0"
    "GdiplusShutdown\0"
    "GdipLoadImageFromStream\0"
    "GdipSaveImageToStream\0"
    "GdipDisposeImage\0"
    "GdipGetImageType\0"
    "GdipGetImageWidth\0"
    "GdipGetImageHeight\0"
    "GdipGetImageFlags\0"
    "GdipGetImagePixelFormat\0"
    "GdipGetImageGraphicsContext\0"
    "GdipImageGetFrameCount\0"
    "GdipImageSelectActiveFrame\0"
    "GdipCreateBitmapFromScan0\0"
    "GdipSetCompositingMode\0"
    "GdipDrawImageI\0"
    "GdipFlush\0"
    "GdipDeleteGraphics\0"

    "GdipGetImageEncoders\0"
    "GdipGetImageEncodersSize\0"
    ;

  // Open the GdiPlus library and load all required symbols.
  if (_library.openLibrary(StringW::fromAscii8("gdiplus")) != ERR_OK)
  {
    // gdiplus.dll not found.
    Debug::dbgFunc("Fog::GdipLibrary", "GdipLibrary", "Can't load gdiplus.dll.\n");
  }

  const char* badSymbol;
  if (_library.getSymbols(_symbols, symbolNames, FOG_ARRAY_SIZE(symbolNames), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Symbol failed to load.
    Debug::dbgFunc("Fog::GdipLibrary", "GdipLibrary", "Can't load symbol %s.\n", badSymbol);
    _library.close();
    return;
  }

  // Startup the GdiPlus library.
  GpGdiplusStartupInput startupInput;
  startupInput.GdiplusVersion = 1;
  startupInput.DebugEventCallback = NULL;
  startupInput.SuppressBackgroundThread = false;
  startupInput.SuppressExternalCodecs = false;

  GpStatus status = _GdiplusStartup(&_gdipToken, &startupInput, NULL);
  if (status != GpOk)
  {
    Debug::dbgFunc("Fog::GdipLibrary", "init", "GdiplusStartup() failed (%u).\n", status);
    _library.close();
  }
}

GdipLibrary::~GdipLibrary()
{
  if (_gdipToken != (ULONG_PTR)NULL)
    _GdiplusShutdown(_gdipToken);
}

// ============================================================================
// [Fog::GdipLibrary - Statics]
// ============================================================================

static GdipLibrary* _GdipLibrary_global;
static Static<GdipLibrary> _GdipLibrary_instance;

// TODO: Mini-Leak, should be destroyed by the application exit, atexit, or similar.
GdipLibrary* GdipLibrary::get()
{
  GdipLibrary* p = AtomicCore<GdipLibrary*>::get(&_GdipLibrary_global);

  if (FOG_UNLIKELY((uintptr_t)p <= (uintptr_t)0x1))
  {
    if (AtomicCore<GdipLibrary*>::cmpXchg(&_GdipLibrary_global, NULL, (GdipLibrary*)0x1))
    {
      p = _GdipLibrary_instance.init();
      AtomicCore<GdipLibrary*>::set(&_GdipLibrary_global, p);
    }
    else
    {
      while ((p = AtomicCore<GdipLibrary*>::get(&_GdipLibrary_global)) == (GdipLibrary*)0x1)
        Thread::yield();
    }
  }

_Done:
  return (p->_library.isOpen()) ? p : NULL;
}

} // Fog namespace
