// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/G2d/OS/WinUxTheme.h>

namespace Fog {

// ============================================================================
// [Fog::WinUxTheme]
// ============================================================================

WinUxTheme::WinUxTheme() :
  err(0xFFFFFFFF)
{
}

WinUxTheme::~WinUxTheme()
{
  close();
}

err_t WinUxTheme::init()
{
  if (err == 0xFFFFFFFF)
  {
    FOG_ONCE_LOCK();
    if (err == 0xFFFFFFFF) err = _init();
    FOG_ONCE_UNLOCK();
  }

  return err;
}

void WinUxTheme::close()
{
  dll.close();
  err = 0xFFFFFFFF;
}

err_t WinUxTheme::_init()
{
  static const char xp_symbols[] =
    "OpenThemeData\0"
    "CloseThemeData\0"
    "EnableTheming\0"

    "IsAppThemed\0"

    "DrawThemeBackground\0"
    "DrawThemeText\0"

    "GetThemeBackgroundRegion\0"
    "IsThemeBackgroundPartiallyTransparent\0"
    ;

  static const char vista_symbols[] =
    "BufferedPaintInit\0"
    "BufferedPaintUnInit\0"
    "GetBufferedPaintBits\0"

    "IsCompositionActive\0"
    ;

  // Ensure that we are not called twice (once initialization is done
  // we can't be called again).
  FOG_ASSERT(err == 0xFFFFFFFF);

  if (dll.openLibrary(StringW::fromAscii8("uxtheme")) != ERR_OK)
  {
    Logger::error("Fog::WinUxTheme", "init",
      "Failed to load uxtheme.dll.");

    return ERR_UI_WIN_UXTHEME_NOT_AVAILABLE;
  }

  const char* badSymbol;

  // Load standard introduced by WinXP (initial).
  if (dll.getSymbols(xp_addr, xp_symbols, FOG_ARRAY_SIZE(xp_symbols),
    NUM_XP_SYMBOLS, (char**)&badSymbol) != NUM_XP_SYMBOLS)
  {
    Logger::error("Fog::WinUxTheme", "init",
      "Failed to load symbol %s.", badSymbol);

    dll.close();
    return ERR_UI_WIN_UXTHEME_NOT_AVAILABLE;
  }

  // Load symbols introduced by WinVista.
  if (dll.getSymbols(vista_addr, vista_symbols, FOG_ARRAY_SIZE(vista_symbols),
    NUM_VISTA_SYMBOLS, (char**)&badSymbol) != NUM_VISTA_SYMBOLS)
  {
    Logger::debug("Fog::WinUxTheme", "init",
      "Failed to load symbol %s introduced by Windows Vista (Vista features disabled).", badSymbol);

    // This is not a failure case, probably using older system than WinVista.
    MemOps::zero(vista_addr, sizeof(void*) * NUM_VISTA_SYMBOLS);
  }

  return ERR_OK;
}

} // Fog namespace
