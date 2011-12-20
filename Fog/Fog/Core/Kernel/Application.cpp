// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Kernel/DefaultEventLoopImpl_p.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/EventLoopImpl.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <Fog/Core/Kernel/WinEventLoopImpl.h>
# include <Fog/Core/OS/WinUtil.h>
# include <io.h>
# if defined(FOG_BUILD_UI)
#  include <Fog/UI/Engine/WinUIEngine.h>
#  include <Fog/UI/Engine/WinUIEventLoopImpl_p.h>
# endif // FOG_BUILD_UI
#endif // FOG_OS_WINDOWS

// [Dependencies - Mac]
#if defined(FOG_OS_MAC)
# include <Fog/Core/Kernel/MacEventLoopImpl.h>
# if defined(FOG_BUILD_UI)
#  include <Fog/UI/Engine/MacUIEngine.h>
#  include <Fog/UI/Engine/MacUIEventLoopImpl_p.h>
# endif // FOG_BUILD_UI
#endif // FOG_OS_MAC

// [Dependencies - Posix, but not Mac]
#if defined(FOG_OS_POSIX) && !defined(FOG_OS_MAC)
# if defined(FOG_BUILD_UI)
#  include <Fog/UI/Engine/UIEngine.h>
# endif // FOG_BUILD_UI
# include <errno.h>
# include <unistd.h>
#endif // FOG_OS_POSIX

FOG_IMPLEMENT_OBJECT(Fog::Application)

namespace Fog {

// ============================================================================
// [Fog::Application - Local]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static void Application_unescapeArgument(StringW& s)
{
  size_t remain = s.getLength();
  if (!remain) return;

  CharW* beg = s.getDataX();
  CharW* cur = beg;
  CharW* dst = beg;

  while (remain)
  {
    if (cur[0] == CharW('\\') && remain > 1 &&
       (cur[1] == CharW('\"') ||
        cur[1] == CharW('\'') ||
        cur[1] == CharW(' ' ) ))
    {
      cur++;
      remain--;
    }
    *dst++ = *cur++;
    remain--;
  }

  s.resize((size_t)(dst - beg));
}

static void Application_parseWinCmdLine(const StringW& cmdLine, List<StringW>& dst)
{
  const CharW* cur = cmdLine.getData();
  const CharW* end = cur + cmdLine.getLength();

  const CharW* mark;
  CharW quote;
  bool isEscaped;
  size_t len;

  for (;;)
  {
    // Skip spaces.
    for (;;)
    {
      if (cur == end) goto end;
      if (cur->isSpace()) cur++;
      else break;
    }

    // Zero character means end.
    if (cur->isNull())
      goto end;

    // Parse quote character (if it's here).
    mark = cur;
    quote = 0;

    if (cur[0] == CharW('\"') ||
        cur[0] == CharW('\'') )
    {
      quote = cur[0];
    }
    cur++;

    // Parse argument
    isEscaped = false;

    for (;;)
    {
      if (cur == end) goto parsed;

      CharW c = cur[0];
      if (c.isNull()) goto parsed;
      if (c.isSpace() && quote.isNull()) goto parsed;

      // Quotes.
      if (c == quote)
      {
        cur++;
        goto parsed;
      }

      // Escape sequence.
      if (c == CharW('\\'))
      {
        if (cur < end - 1 && (cur[1] == CharW('\"') ||
                              cur[1] == CharW('\'') ||
                              cur[1] == CharW(' ')) )
        {
          cur++;
          isEscaped = true;
        }
      }

      cur++;
    }

parsed:
    len = (size_t)(cur - mark);
    if (quote.isNull())
    {
      mark++;
      len--;
      if (len && mark[len-1] == quote) len--;
    }

    {
      StringW arg(mark, len);
      
      if (isEscaped)
        Application_unescapeArgument(arg);
      
      dst.append(arg);
    }
  }

end:
  return;
}
#endif // FOG_OS_WINDOWS

struct FOG_NO_EXPORT Application_Local
{
#if defined(FOG_BUILD_UI)
  typedef Hash<StringW, UIEngineConstructor> UIEngineHash;
#endif // FOG_BUILD_UI
  typedef Hash<StringW, EventLoopConstructor> EventLoopHash;

  Lock lock;

#if defined(FOG_BUILD_UI)
  UIEngineHash uiEngineHash;
#endif // FOG_BUILD_UI
  EventLoopHash eventLoopHash;

  StringW applicationExecutable;
  List<StringW> applicationArguments;

  Application_Local();
  ~Application_Local();

#if defined(FOG_BUILD_UI)
  UIEngine* createUIEngine(const StringW& name);
#endif // FOG_BUILD_UI
  EventLoopImpl* createEventLoop(const StringW& name);

  void applicationArgumentsWasSet();
};

Application_Local::Application_Local()
{
#if defined(FOG_OS_WINDOWS)
  StringW applicationCommand;

  applicationCommand.set(reinterpret_cast<const CharW*>(::GetCommandLineW()));
  Application_parseWinCmdLine(applicationCommand, applicationArguments);

  applicationArgumentsWasSet();
#endif // FOG_OS_WINDOWS
}

Application_Local::~Application_Local()
{
}

#if defined(FOG_BUILD_UI)
UIEngine* Application_Local::createUIEngine(const StringW& name)
{
  if (name.startsWith(Ascii8("UI.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createUIEngine", "UIEngine can be created only by main thread.\n");
    return NULL;
  }

  AutoLock locked(lock);

  UIEngineConstructor ctor = uiEngineHash.get(name, NULL);
  return ctor ? ctor() : NULL;
}
#endif // FOG_BUILD_UI

EventLoopImpl* Application_Local::createEventLoop(const StringW& name)
{
  if (name.startsWith(Ascii8("UI.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createEventLoop", "Event loop for UI can be created by main thread only.\n");
    return NULL;
  }

  AutoLock locked(lock);
  EventLoopConstructor ctor = eventLoopHash.get(name, NULL);
  return ctor ? ctor() : NULL;
}

void Application_Local::applicationArgumentsWasSet()
{
  applicationExecutable = applicationArguments.getAt(0);
  FilePath::toAbsolute(applicationExecutable, applicationExecutable);

  StringW applicationDirectory;
  FilePath::extractDirectory(applicationDirectory, applicationExecutable);

  // Application directory usually contains plugins and library itself under
  // Windows, but we will add it also for POSIX OSes. It can help if application
  // is started from user home directory.
  Library::addLibraryPath(applicationDirectory, LIBRARY_PATH_PREPEND);
}

static Static<Application_Local> Application_local;

// ============================================================================
// [Fog::Application]
// ============================================================================

Application* Application::_instance = NULL;

Application::Application(const StringW& appType)
{
  _init(appType);
}

Application::Application(const StringW& appType, int argc, const char* argv[])
{
  ::fog_init_args(argc, argv);

  _init(appType);
}

void Application::_init(const StringW& appType)
{
  _uiEngine = NULL;

  if (appType.startsWith(Ascii8("UI")))
  {
#if defined(FOG_BUILD_UI)
    // Create the UIEngine by name.
    _uiEngine = createUIEngine(appType);
#else
    // Requested to create the UIEngine, but Fog/UI is disabled.
#endif
  }

  // Create EventLoop by of the required type.
  EventLoopImpl* impl = createEventLoop(appType);

  if (impl)
    _homeThread->_eventLoop.adopt(impl);

  // Set global application instance (singleton).
  if (_instance == NULL)
    _instance = this;
}

Application::~Application()
{
#if defined(FOG_BUILD_UI)
  // We will unload library here, not by UIEngine destructor, because
  // EventLoop may be also created by UIEngine.
  Library libraryToClose;

  // Delete UIEngine if associated.
  if (_uiEngine)
  {
    libraryToClose = _uiEngine->_library;

    fog_delete(_uiEngine);
    _uiEngine = NULL;
  }
#endif // FOG_BUILD_UI

  // Delete EventLoop if associated.
  Thread::getMainThread()->_eventLoop.reset();

  // Clear global application instance (singleton).
  if (_instance == this)
    _instance = NULL;
}

err_t Application::run()
{
  return _homeThread->getEventLoop().run();
}

err_t Application::quit()
{
  return _homeThread->getEventLoop().quit();
}

// ============================================================================
// [Fog::Application - Application Executable / Arguments]
// ============================================================================

StringW Application::getApplicationExecutable()
{
  return Application_local->applicationExecutable;
}

List<StringW> Application::getApplicationArguments()
{
  return Application_local->applicationArguments;
}

// ============================================================================
// [Fog::Application - Working Directory]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
err_t Application::getWorkingDirectory(StringW& dst)
{
  size_t capacity = 256;

  for (;;)
  {
    CharW* buf = dst._prepare(CONTAINER_OP_REPLACE, capacity);
    capacity = dst.getCapacity();

    if (FOG_IS_NULL(buf))
      return ERR_RT_OUT_OF_MEMORY;

    size_t length = ::GetCurrentDirectoryW(
      static_cast<DWORD>(Math::min<size_t>(dst.getCapacity() + 1, UINT32_MAX)),
      reinterpret_cast<wchar_t*>(buf));

    if (capacity < length)
    {
      capacity = length;
      continue;
    }

    dst._modified(buf + length);
    return dst.normalizeSlashes(SLASH_FORM_UNIX);
  }
}

err_t Application::setWorkingDirectory(const StringW& dir)
{
  StringW dirW;
  FOG_RETURN_ON_ERROR(WinUtil::makeWinPath(dirW, dir));

  if (SetCurrentDirectoryW(reinterpret_cast<const wchar_t*>(dirW.getData())) == 0)
    return ERR_OK;
  else
    return OSUtil::getErrFromOSLastError();
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
err_t Application::getWorkingDirectory(StringW& dst)
{
  StringTmpA<TEMPORARY_LENGTH> dir8;
  dst.clear();

  for (;;)
  {
    const char* ptr = ::getcwd(dir8.getDataX(), dir8.getCapacity() + 1);
    if (ptr) return TextCodec::local8().decode(dst, StubA(ptr, DETECT_LENGTH));

    if (errno != ERANGE) return errno;

    // Alloc more...
    FOG_RETURN_ON_ERROR(dir8.reserve(dir8.getCapacity() + 4096));
  }
}

err_t Application::setWorkingDirectory(const StringW& dir)
{
  StringTmpA<TEMPORARY_LENGTH> dir8;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(dir8, dir));

  if (::chdir(dir8.getData()) == 0)
    return ERR_OK;
  else
    return errno;
}
#endif // FOG_OS_POSIX

// ============================================================================
// [Fog::Application - UIEngine - Access]
// ============================================================================

#if defined(FOG_BUILD_UI)
UIEngine* Application::createUIEngine(const StringW& _name)
{
  StringW name(_name);

  // First try to detect UIEngine if not specified
  if (name == Ascii8("UI"))
    name = detectUIEngine();

  // Try to create registered native engine.
  UIEngine* engine = Application_local->createUIEngine(name);
  if (engine)
    return engine;

  // Otherwise try to load dynamically linked library (based on the UI engine name).
  if (!name.startsWith(Ascii8("UI.")))
    return NULL;

  Library lib;
  err_t err = lib.openPlugin(StringW::fromAscii8("Fog_UI"), name.substring(Range(3, DETECT_LENGTH)));
  if (FOG_IS_ERROR(err))
    return NULL;

  UIEngineConstructor ctor = (UIEngineConstructor)lib.getSymbol("createUIEngine");
  if (FOG_IS_NULL(ctor))
    return NULL;

  engine = ctor();
  if (FOG_IS_NULL(engine))
    return NULL;

  // Success.
  engine->_library = lib;
  return engine;
}

StringW Application::detectUIEngine()
{
#if defined(FOG_OS_WINDOWS)
  return StringW::fromAscii8("UI.Win");
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
# if defined(FOG_OS_MAC)
  return StringW::fromAscii8("UI.Mac");
# else
  return StringW::fromAscii8("UI.X11");
# endif
#endif // FOG_OS_POSIX
}
#endif // FOG_BUILD_UI

// ============================================================================
// [Fog::Application - UIEngine - Register / Unregister]
// ============================================================================

#if defined(FOG_BUILD_UI)
bool Application::registerUIEngine(const StringW& name, UIEngineConstructor ctor)
{
  AutoLock locked(Application_local->lock);
  return Application_local->uiEngineHash.put(name, ctor);
}

bool Application::unregisterUIEngine(const StringW& name)
{
  AutoLock locked(Application_local->lock);
  return Application_local->uiEngineHash.remove(name);
}
#endif // FOG_BUILD_UI

// ============================================================================
// [Fog::Application - EventLoop - Register / Unregister]
// ============================================================================

bool Application::registerEventLoop(const StringW& name, EventLoopConstructor ctor)
{
  AutoLock locked(Application_local->lock);
  return Application_local->eventLoopHash.put(name, ctor);
}

bool Application::unregisterEventLoop(const StringW& name)
{
  AutoLock locked(Application_local->lock);
  return Application_local->eventLoopHash.remove(name);
}

// ============================================================================
// [Fog::Application - EventLoop - Access]
// ============================================================================

EventLoopImpl* Application::createEventLoop(const StringW &_name)
{
  StringW name(_name);

#if defined(FOG_BUILD_UI)
  // First try to detect UI event loop if not specified.
  if (name == Ascii8("UI"))
    name = detectUIEngine();
#endif // FOG_BUILD_UI

  return Application_local->createEventLoop(name);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

// [Default]
static EventLoopImpl* FOG_CDECL Application_DefaultEventLoopConstructor() { return fog_new DefaultEventLoopImpl(); }

// [Windows]
#if defined(FOG_OS_WINDOWS)
static EventLoopImpl* FOG_CDECL Application_WinEventLoopConstructor() { return fog_new WinEventLoopImpl(); }

#if defined(FOG_BUILD_UI)
static UIEngine* FOG_CDECL Application_WinUIEngineConstructor() { return fog_new WinUIEngine(); }
static EventLoopImpl* FOG_CDECL Application_WinUIEventLoopConstructor() { return fog_new WinUIEventLoopImpl(); }
#endif // FOG_BUILD_UI
#endif // FOG_OS_WINDOWS

// [Mac]
#if defined(FOG_OS_MAC)
static EventLoopImpl* FOG_CDECL Application_MacEventLoopConstructor() { return fog_new MacEventLoopImpl(); }

#if defined(FOG_BUILD_UI)
static UIEngine* FOG_CDECL Application_MacUIEngineConstructor() { return fog_new MacUIEngine(); }
static EventLoopImpl* FOG_CDECL Application_MacUIEventLoopConstructor() { return fog_new MacUIEventLoopImpl(); }
#endif // FOG_BUILD_UI
#endif // FOG_OS_MAC

FOG_NO_EXPORT void Application_init(void)
{
  Application_local.init();
  StringW type;

  // [Default].
  type.set(FOG_STR_(APPLICATION_Core_Default));
  Application::registerEventLoop(type, Application_DefaultEventLoopConstructor);

  // [Windows].
#if defined(FOG_OS_WINDOWS)
  type.set(FOG_STR_(APPLICATION_Core_Win));
  Application::registerEventLoop(type, Application_WinEventLoopConstructor);

#if defined(FOG_BUILD_UI)
  type.set(FOG_STR_(APPLICATION_UI_Win));
  Application::registerUIEngine(type, Application_WinUIEngineConstructor);
  Application::registerEventLoop(type, Application_WinUIEventLoopConstructor);
#endif // FOG_BUILD_UI
#endif // FOG_OS_WINDOWS

  // [Mac].
#if defined(FOG_OS_MAC)
  type.set(FOG_STR_(APPLICATION_Core_Mac));
  Application::registerEventLoop(type, Application_MacEventLoopConstructor);

#if defined(FOG_BUILD_UI)
  type.set(FOG_STR_(APPLICATION_UI_Mac));
  Application::registerUIEngine(type, Application_MacUIEngineConstructor);
  Application::registerEventLoop(type, Application_MacUIEventLoopConstructor);
#endif // FOG_BUILD_UI && FOG_OS_MAC
#endif // FOG_OS_MAC
}

FOG_NO_EXPORT void Application_fini(void)
{
  Application_local.destroy();
}

} // Fog namespace

// ============================================================================
// [Fog::Application - initArguments]
// ============================================================================

FOG_CAPI_DECLARE void fog_init_args(int argc, const char* argv[])
{
  using namespace Fog;
  if (argc < 1) return;

  AutoLock locked(Application_local->lock);

  List<StringW>& arguments = Application_local->applicationArguments;
  if (arguments.getLength() != 0) return;

  for (int i = 0; i < argc; i++)
  {
    StringW arg;
    TextCodec::local8().decode(arg, StubA(argv[i], DETECT_LENGTH));
    arguments.append(arg);
  }

  Application_local->applicationArgumentsWasSet();
}
