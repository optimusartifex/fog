// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_SYSTEM_APPLICATION_H
#define _FOG_CORE_SYSTEM_APPLICATION_H

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/System/Object.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Gui/Animation/Animation.h>

namespace Fog {

//! @addtogroup Fog_Core_Application
//! @{

//! @brief Function that should be called if application arguments are not
//! initialized and Application object not exists or it's not kwown that
//! it will exist in the future.
//!
//! This method is called automatically by @c FOG_CORE_MAIN and
//! @c FOG_GUI_MAIN macros.
FOG_API void _core_application_init_arguments(int argc, const char* argv[]);

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Application;
struct EventLoop;

// Declared in Fog-Gui
struct GuiEngine;

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
  Application(const String& type);
  //! @brief Application constructor that allows to set argc and argv[].
  Application(const String& type, int argc, const char* argv[]);

  virtual ~Application();

private:
  void _init(const String& type);

public:
  virtual err_t run();
  virtual void quit();

  static FOG_INLINE Application* getInstance() { return _instance; }

  // --------------------------------------------------------------------------
  // [Application Executable / Arguments]
  // --------------------------------------------------------------------------

  static String getApplicationExecutable();
  static List<String> getApplicationArguments();

  // --------------------------------------------------------------------------
  // [Working Directory]
  // --------------------------------------------------------------------------

  static err_t getWorkingDirectory(String& dir);
  static err_t setWorkingDirectory(const String& dir);

  // --------------------------------------------------------------------------
  // [GuiEngine - Access]
  // --------------------------------------------------------------------------

  //! @brief Get application @c GuiEngine (can be NULL if GUI is not used).
  FOG_INLINE GuiEngine* getGuiEngine() const { return _nativeEngine; }

  //! @brief Detect best @c GuiEngine for current platform and configuration.
  static String detectGuiEngine();

  //! @brief Create @c GuiEngine based on @a name.
  static GuiEngine* createGuiEngine(const String& name);

  // --------------------------------------------------------------------------
  // [GuiEngine - Register / Unregister]
  // --------------------------------------------------------------------------

  typedef GuiEngine* (*GuiEngineConstructor)();

  static bool registerGuiEngine(const String& name, GuiEngineConstructor ctor);
  static bool unregisterGuiEngine(const String& name);

  template<typename GuiEngineType>
  struct _GuiEngineCtorHelper
  {
    static GuiEngine* ctor() { return fog_new GuiEngineType(); }
  };

  template<typename GuiEngineType>
  static FOG_INLINE bool registerGuiEngineType(const String& name)
  {
    return registerGuiEngine(name, _GuiEngineCtorHelper<GuiEngineType>::ctor);
  }

  // --------------------------------------------------------------------------
  // [EventLoop - Access]
  // --------------------------------------------------------------------------

  //! @brief Get application event loop (can be NULL).
  FOG_INLINE EventLoop* getEventLoop() const { return _eventLoop; }
  FOG_INLINE AnimationDispatcher* getAnimationDispatcher() { return &_animation; }

  //! @brief Create event loop.
  static EventLoop* createEventLoop(const String& name);

  // --------------------------------------------------------------------------
  // [EventLoop - Register / Unregister]
  // --------------------------------------------------------------------------

  typedef EventLoop* (*EventLoopConstructor)();

  static bool registerEventLoop(const String& name, EventLoopConstructor ctor);
  static bool unregisterEventLoop(const String& name);

  template<typename EventLoopType>
  struct _EventLoopCtorHelper
  {
    static EventLoop* ctor() { return fog_new EventLoopType(); }
  };

  template<typename EventLoopType>
  static FOG_INLINE bool registerEventLoopType(const String& name)
  {
    return registerEventLoop(name, _EventLoopCtorHelper<EventLoopType>::ctor);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  EventLoop* _eventLoop;
  GuiEngine* _nativeEngine;
  AnimationDispatcher _animation;

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Application* _instance;

private:
  _FOG_CLASS_NO_COPY(Application)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_SYSTEM_APPLICATION_H
