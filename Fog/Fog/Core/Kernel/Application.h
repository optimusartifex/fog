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
#include <Fog/UI/Animation/Animation.h>

namespace Fog {

//! @addtogroup Fog_Core_Kernel
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

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
  // [GuiEngine - Access]
  // --------------------------------------------------------------------------

  //! @brief Get application @c GuiEngine (can be NULL if GUI is not used).
  FOG_INLINE GuiEngine* getGuiEngine() const { return _nativeEngine; }

  //! @brief Detect best @c GuiEngine for current platform and configuration.
  static StringW detectGuiEngine();

  //! @brief Create @c GuiEngine based on @a name.
  static GuiEngine* createGuiEngine(const StringW& name);

  // --------------------------------------------------------------------------
  // [GuiEngine - Register / Unregister]
  // --------------------------------------------------------------------------

  typedef GuiEngine* (*GuiEngineConstructor)(void);

  static bool registerGuiEngine(const StringW& name, GuiEngineConstructor ctor);
  static bool unregisterGuiEngine(const StringW& name);

  template<typename GuiEngineType>
  struct _GuiEngineCtorHelper
  {
    static GuiEngine* ctor() { return fog_new GuiEngineType(); }
  };

  template<typename GuiEngineType>
  static FOG_INLINE bool registerGuiEngineType(const StringW& name)
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
  static EventLoop* createEventLoop(const StringW& name);

  // --------------------------------------------------------------------------
  // [EventLoop - Register / Unregister]
  // --------------------------------------------------------------------------

  typedef EventLoop* (*EventLoopConstructor)();

  static bool registerEventLoop(const StringW& name, EventLoopConstructor ctor);
  static bool unregisterEventLoop(const StringW& name);

  template<typename EventLoopType>
  struct _EventLoopCtorHelper
  {
    static EventLoop* ctor() { return fog_new EventLoopType(); }
  };

  template<typename EventLoopType>
  static FOG_INLINE bool registerEventLoopType(const StringW& name)
  {
    return registerEventLoop(name, _EventLoopCtorHelper<EventLoopType>::ctor);
  }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static Application* _instance;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  EventLoop* _eventLoop;
  GuiEngine* _nativeEngine;
  AnimationDispatcher _animation;

private:
  _FOG_NO_COPY(Application)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_KERNEL_APPLICATION_H
