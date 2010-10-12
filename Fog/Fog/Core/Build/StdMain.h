// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_BUILD_STDMAIN_H
#define _FOG_CORE_BUILD_STDMAIN_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_BUILD_H)
#error "Fog::Core::Build - StdMain.h can be only included by Fog/Core/Build.h"
#endif // _FOG_CORE_BUILD_H

// ============================================================================
// [Fog::Core::Build - Main]
// ============================================================================

// Defined also in Fog/Core/Application.h. It's defined here to prevent
// compilation errors when using FOG_CORE_MAIN() or FOG_GUI_MAIN() and
// this header file is not included.
FOG_API void fog_arguments_init(int argc, char* argv[]);

//! @addtogroup Fog_Core_Macros
//! @{

//! @def FOG_GUI_MAIN
//! @brief Application entry point declaration.
//!
//! Usage:
//!
//! FOG_GUI_MAIN()
//! {
//!   // Your main() here ...
//!   return Fog::ExitSuccess;
//! }
#if defined(FOG_OS_WINDOWS)
#define FOG_GUI_MAIN() \
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#define FOG_GUI_MAIN() \
static int _fog_main(int argc, char* argv[]); \
\
int main(int argc, char* argv[]) \
{ \
  fog_arguments_init(argc, argv); \
  \
  return _fog_main(argc, argv); \
} \
\
static int _fog_main(int argc, char* argv[])
#endif

#endif // _FOG_CORE_BUILD_STDMAIN_H
