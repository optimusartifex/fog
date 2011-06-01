// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Guard]
#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Debug.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Win/WinUxThemeLibrary.h>

namespace Fog {

// ============================================================================
// [Fog::WinUxThemeLibrary]
// ============================================================================

WinUxThemeLibrary::WinUxThemeLibrary() :
  err(0xFFFFFFFF)
{
}

WinUxThemeLibrary::~WinUxThemeLibrary()
{
  close();
}

err_t WinUxThemeLibrary::init()
{
  if (err == 0xFFFFFFFF)
  {
    FOG_ONCE_LOCK();
    if (err == 0xFFFFFFFF) err = _init();
    FOG_ONCE_UNLOCK();
  }

  return err;
}

void WinUxThemeLibrary::close()
{
  dll.close();
  err = 0xFFFFFFFF;
}

err_t WinUxThemeLibrary::_init()
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

  if (dll.open(Ascii8("uxtheme")) != ERR_OK)
  {
    // UxTheme.dll not found.
    return ERR_THEME_NATIVE_NOT_AVAILABLE;
  }

  const char* badSymbol;

  // Load standard introduced by WinXP (initial).
  if (dll.getSymbols(xp_addr, xp_symbols, FOG_ARRAY_SIZE(xp_symbols),
    NUM_XP_SYMBOLS, (char**)&badSymbol) != NUM_XP_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    Debug::dbgFunc("Fog::WinUxThemeLibrary", "init", "Can't load symbol '%s'.\n", badSymbol);
    dll.close();
    return ERR_THEME_NATIVE_NOT_AVAILABLE;
  }

  // Load symbols introduced by WinVista.
  if (dll.getSymbols(vista_addr, vista_symbols, FOG_ARRAY_SIZE(vista_symbols),
    NUM_VISTA_SYMBOLS, (char**)&badSymbol) != NUM_VISTA_SYMBOLS)
  {
    // This is not a failure case, probably using older system than WinVista.
    Memory::zero(vista_addr, sizeof(void*) * NUM_VISTA_SYMBOLS);
  }

  return ERR_OK;
}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
