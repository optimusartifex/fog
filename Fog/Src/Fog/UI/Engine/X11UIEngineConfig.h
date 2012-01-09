// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_X11UIENGINECONFIG_H
#define _FOG_UI_ENGINE_X11UIENGINECONFIG_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

// [API]
#define FOG_UI_X11_API FOG_API

#if defined(FOG_BUILD_UI_X11_MODULE)
# undef FOG_UI_X11_API
# if defined(FogUI_X11_EXPORTS)
#  define FOG_UI_X11_API FOG_DLL_EXPORT
# else
#  define FOG_UI_X11_API FOG_DLL_IMPORT
# endif // FogUI_X11_EXPORTS
#endif // FOG_BUILD_UI_X11

// [Guard]
#endif // _FOG_UI_ENGINE_X11UIENGINECONFIG_H
