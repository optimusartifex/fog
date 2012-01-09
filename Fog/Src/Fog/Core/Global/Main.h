// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_GLOBAL_MAIN_H
#define _FOG_CORE_GLOBAL_MAIN_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CPP_BASE_H)
#error "Fog/Core/C++/StdMain.h can be only included by Fog/Core/C++/Base.h"
#endif // _FOG_CORE_CPP_BASE_H

// ============================================================================
// [Fog::Core::Build - Main]
// ============================================================================

// Defined also in Fog/Core/Global/Api.h. It's defined here to prevent
// compilation errors when using FOG_CORE_MAIN() or FOG_UI_MAIN() and
// this header file is not included.
FOG_CAPI_EXTERN void fog_init_args(int argc, const char* argv[]);

//! @addtogroup Fog_Core_Global
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
static int ui_main(int argc, char* argv[]); \
\
int main(int argc, char* argv[]) \
{ \
  fog_init_args(argc, (const char**)argv); \
  return ui_main(argc, argv); \
} \
\
static int ui_main(int argc, char* argv[])

#endif // FOG_OS_...

#endif // _FOG_CORE_GLOBAL_MAIN_H
