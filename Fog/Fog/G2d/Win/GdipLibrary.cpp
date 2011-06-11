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
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Win/GdipLibrary.h>

namespace Fog {

// ===========================================================================
// [Fog::GdipLibrary]
// ===========================================================================

GdipLibrary::GdipLibrary() :
  err(0xFFFFFFFF),
  gdiplusToken(0)
{
}

GdipLibrary::~GdipLibrary()
{
  close();
}

err_t GdipLibrary::prepare()
{
  if (err == 0xFFFFFFFF)
  {
    FOG_ONCE_LOCK();
    if (err == 0xFFFFFFFF) err = init();
    FOG_ONCE_UNLOCK();
  }

  return err;
}

err_t GdipLibrary::init()
{
  static const char symbols[] =
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

  // Ensure that we are not called twice (once initialization is done
  // we can't be called again).
  FOG_ASSERT(err == 0xFFFFFFFF);

  if (dll.open(Ascii8("gdiplus")) != ERR_OK)
  {
    // gdiplus.dll not found.
    return ERR_IMAGE_GDIPLUS_NOT_LOADED;
  }

  const char* badSymbol;
  if (dll.getSymbols(addr, symbols, FOG_ARRAY_SIZE(symbols), NUM_SYMBOLS, (char**)&badSymbol) != NUM_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    Debug::dbgFunc("Fog::GdipLibrary", "init", "Can't load symbol '%s'.\n", badSymbol);
    dll.close();
    return ERR_IMAGE_GDIPLUS_NOT_LOADED;
  }

  // GdiPlus - Startup.
  GpGdiplusStartupInput startupInput;
  startupInput.GdiplusVersion = 1;
  startupInput.DebugEventCallback = NULL;
  startupInput.SuppressBackgroundThread = false;
  startupInput.SuppressExternalCodecs = false;

  GpStatus status = pGdiplusStartup(&gdiplusToken, &startupInput, NULL);
  if (status != GpOk)
  {
    Debug::dbgFunc("Fog::GdipLibrary", "init", "GdiplusStartup() failed (%u).\n", status);
    dll.close();
    return ERR_IMAGE_GDIPLUS_NOT_LOADED;
  }

  return ERR_OK;
}

void GdipLibrary::close()
{
  // GdiPlus - Shutdown.
  if (err == ERR_OK)
  {
    pGdiplusShutdown(gdiplusToken);
    gdiplusToken = 0;
  }

  dll.close();
  err = 0xFFFFFFFF;
}

} // Fog namespace
