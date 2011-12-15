// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_APPLICATION_H
#define _FOG_CORE_KERNEL_APPLICATION_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Tools/String.h>

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
  Application(const StringW& type);
  //! @brief Application constructor that allows to set argc and argv[].
  Application(const StringW& type, int argc, const char* argv[]);

  virtual ~Application();

private:
  void _init(const StringW& type);

public:
  virtual err_t run();
  virtual void quit();

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
  // [FbEngine]
  // --------------------------------------------------------------------------

#if defined(FOG_BUILD_UI)
  //! @brief Get application @c GuiEngine (can be NULL if GUI is not used).
  FOG_INLINE GuiEngine* getFbEngine() const { return _fbEngine; }
  //! @brief Create @c GuiEngine based on @a name.
  static GuiEngine* createFbEngine(const StringW& name);

  //! @brief Detect best @c GuiEngine for current platform and configuration.
  static StringW detectFbEngine();

  static bool registerFbEngine(const StringW& name, FbEngineConstructor ctor);
  static bool unregisterFbEngine(const StringW& name);
#endif // FOG_BUILD_UI

  // --------------------------------------------------------------------------
  // [EventLoop]
  // --------------------------------------------------------------------------

  //! @brief Get application event loop (can be NULL).
  FOG_INLINE EventLoop* getEventLoop() const { return _eventLoop; }
  //! @brief Create event loop.
  static EventLoop* createEventLoop(const StringW& name);

  static bool registerEventLoop(const StringW& name, EventLoopConstructor ctor);
  static bool unregisterEventLoop(const StringW& name);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Application* _instance;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // TODO: Rename to FbEngine.
#if defined(FOG_BUILD_UI)
  GuiEngine* _fbEngine;
#else
  void* _fbEngine;
#endif // FOG_BUILD_UI

  //! @brief Application main event loop.
  EventLoop* _eventLoop;

private:
  _FOG_NO_COPY(Application)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_KERNEL_APPLICATION_H
