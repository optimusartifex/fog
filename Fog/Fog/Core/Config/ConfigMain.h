// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CONFIG_CONFIGMAIN_H
#define _FOG_CORE_CONFIG_CONFIGMAIN_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CONFIG_CONFIG_H)
#error "Fog::Core::Config - ConfigMain.h can be only included by Fog/Core/Config/Config.h"
#endif // _FOG_CORE_CONFIG_CONFIG_H

// ============================================================================
// [Fog::Core::Build - Main]
// ============================================================================

namespace Fog {

// Defined also in Fog/Core/System/Application.h. It's defined here to prevent
// compilation errors when using FOG_CORE_MAIN() or FOG_UI_MAIN() and
// this header file is not included.
FOG_API void _core_application_init_arguments(int argc, const char* argv[]);

} // Fog namespace

//! @addtogroup Fog_Core_Macros
//! @{

//! @def FOG_UI_MAIN
//! @brief Application entry point declaration.
//!
//! Usage:
//!
//! FOG_UI_MAIN()
//! {
//!   // Your main() here ...
//!   return Fog::ExitSuccess;
//! }
#if defined(FOG_OS_WINDOWS)
#define FOG_UI_MAIN() \
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define FOG_UI_MAIN() \
static int _fog_main(int argc, char* argv[]); \
\
int main(int argc, char* argv[]) \
{ \
  ::Fog::_core_application_init_arguments(argc, (const char**)argv); \
  \
  return _fog_main(argc, argv); \
} \
\
static int _fog_main(int argc, char* argv[])
#endif

#endif // _FOG_CORE_CONFIG_CONFIGMAIN_H
