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
#include <Fog/G2d/Win/UxThemeLibrary.h>

namespace Fog {

// ============================================================================
// [Fog::UxThemeLibrary]
// ============================================================================

UxThemeLibrary::UxThemeLibrary() :
  err(0xFFFFFFFF)
{
}

UxThemeLibrary::~UxThemeLibrary()
{
  close();
}

err_t UxThemeLibrary::init()
{
  if (err == 0xFFFFFFFF)
  {
    FOG_ONCE_LOCK();
    if (err == 0xFFFFFFFF) err = _init();
    FOG_ONCE_UNLOCK();
  }

  return err;
}

void UxThemeLibrary::close()
{
  dll.close();
  err = 0xFFFFFFFF;
}

err_t UxThemeLibrary::_init()
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
    // UxTheme.dll not found.
    return ERR_THEME_NATIVE_NOT_AVAILABLE;
  }

  const char* badSymbol;

  // Load standard introduced by WinXP (initial).
  if (dll.getSymbols(xp_addr, xp_symbols, FOG_ARRAY_SIZE(xp_symbols),
    NUM_XP_SYMBOLS, (char**)&badSymbol) != NUM_XP_SYMBOLS)
  {
    // Some symbol failed to load? Inform about it.
    Debug::dbgFunc("Fog::UxThemeLibrary", "init", "Can't load symbol '%s'.\n", badSymbol);
    dll.close();
    return ERR_THEME_NATIVE_NOT_AVAILABLE;
  }

  // Load symbols introduced by WinVista.
  if (dll.getSymbols(vista_addr, vista_symbols, FOG_ARRAY_SIZE(vista_symbols),
    NUM_VISTA_SYMBOLS, (char**)&badSymbol) != NUM_VISTA_SYMBOLS)
  {
    // This is not a failure case, probably using older system than WinVista.
    MemOps::zero(vista_addr, sizeof(void*) * NUM_VISTA_SYMBOLS);
  }

  return ERR_OK;
}

} // Fog namespace
