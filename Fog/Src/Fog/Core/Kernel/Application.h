// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_APPLICATION_H
#define _FOG_CORE_KERNEL_APPLICATION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Tools/String.h>

FOG_CVAR_EXTERN uint32_t fog_application_safe_to_log;
FOG_CVAR_EXTERN uint32_t fog_application_terminated;

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Fog::Application]
// ============================================================================

//! @brief Main application object.
struct FOG_API Application : public Object
{
  FOG_DECLARE_OBJECT(Application, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  //! @brief Application constructor.
  Application(const StringW& appType);
  //! @brief Application constructor that allows to set argc and argv[].
  Application(const StringW& appType, int argc, const char* argv[]);

  //! @brief Application destroctor, called by Fog-Framework.
  virtual ~Application();

  // --------------------------------------------------------------------------
  // [Init]
  // --------------------------------------------------------------------------

  void _init(const StringW& appType);

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

#if defined(FOG_BUILD_UI)
  //! @brief Get @ref UIEngine (can be NULL if UI is not initialized).
  FOG_INLINE UIEngine* getUIEngine() const
  {
    return _uiEngine;
  }
#endif // FOG_BUILD_UI

  // --------------------------------------------------------------------------
  // [Run / Quit]
  // --------------------------------------------------------------------------

  err_t run();
  err_t quit();

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Application* _instance;

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  static FOG_INLINE Application* get() { return _instance; }
  
  // --------------------------------------------------------------------------
  // [Application Executable / Arguments]
  // --------------------------------------------------------------------------

  static StringW getApplicationExecutable();
  static List<StringW> getApplicationArguments();

  // --------------------------------------------------------------------------
  // [Working Directory]
  // --------------------------------------------------------------------------

  static err_t getWorkingDirectory(StringW& dir);
  static err_t setWorkingDirectory(const StringW& dir);

  // --------------------------------------------------------------------------
  // [Terminate]
  // --------------------------------------------------------------------------

  static void FOG_NO_RETURN terminate(int code);

  // --------------------------------------------------------------------------
  // [UIEngine]
  // --------------------------------------------------------------------------

#if defined(FOG_BUILD_UI)
  //! @brief Create @c UIEngine based on @a name.
  static UIEngine* createUIEngine(const StringW& name);

  //! @brief Detect best @c UIEngine for current platform and configuration.
  static StringW detectUIEngine();

  static bool registerUIEngine(const StringW& name, UIEngineConstructor ctor);
  static bool unregisterUIEngine(const StringW& name);
#endif // FOG_BUILD_UI

  // --------------------------------------------------------------------------
  // [EventLoop]
  // --------------------------------------------------------------------------

  //! @brief Create event loop.
  static EventLoopImpl* createEventLoop(const StringW& name);

  static bool registerEventLoop(const StringW& name, EventLoopConstructor ctor);
  static bool unregisterEventLoop(const StringW& name);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

#if defined(FOG_BUILD_UI)
  //! @brief UIEngine (can be @c NULL).
  UIEngine* _uiEngine;
#else
  //! @brief For binary compatibility, always @c NULL if compiled without UI.
  void* _uiEngine;
#endif // FOG_BUILD_UI

private:
  _FOG_NO_COPY(Application)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_KERNEL_APPLICATION_H
