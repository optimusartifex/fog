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
#include <Fog/Core/Kernel/EventLoop.h>
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
# if defined(FOG_BUILD_UI)
#  include <Fog/UI/Engine/WinGuiEngine.h>
# endif // FOG_BUILD_UI
# include <Fog/Core/OS/WinUtil.h>
# include <io.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Mac]
#if defined(FOG_OS_MAC)
# if defined(FOG_BUILD_UI)
#  include <Fog/UI/Engine/MacGuiEngine.h>
# endif // FOG_BUILD_UI
#endif // FOG_OS_MAC

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# if defined(FOG_BUILD_UI)
#  include <Fog/UI/Engine/GuiEngine.h>
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
  typedef Hash<StringW, EventLoopConstructor> EventLoopHash;
#if defined(FOG_BUILD_UI)
  typedef Hash<StringW, FbEngineConstructor> FbEngineHash;
#endif // FOG_BUILD_UI

  Lock lock;

  EventLoopHash eventLoopHash;
#if defined(FOG_BUILD_UI)
  FbEngineHash fbEngineHash;
#endif // FOG_BUILD_UI

  StringW applicationExecutable;
  List<StringW> applicationArguments;

  Application_Local();
  ~Application_Local();

  EventLoop* createEventLoop(const StringW& name);
#if defined(FOG_BUILD_UI)
  GuiEngine* createFbEngine(const StringW& name);
#endif // FOG_BUILD_UI

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
GuiEngine* Application_Local::createFbEngine(const StringW& name)
{
  if (name.startsWith(Ascii8("UI.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createFbEngine", "FbEngine can be created only by main thread.\n");
    return NULL;
  }

  AutoLock locked(lock);

  FbEngineConstructor ctor = fbEngineHash.get(name, NULL);
  return ctor ? ctor() : NULL;
}
#endif // FOG_BUILD_UI

EventLoop* Application_Local::createEventLoop(const StringW& name)
{
  if (name.startsWith(Ascii8("UI.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createEventLoop", "FbEventLoop can be created only by main thread.\n");
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

Application::Application(const StringW& type)
{
  _init(type);
}

Application::Application(const StringW& type, int argc, const char* argv[])
{
  ::fog_init_args(argc, argv);

  _init(type);
}

void Application::_init(const StringW& type)
{
  _eventLoop = NULL;
  _fbEngine = NULL;

  if (type.startsWith(Ascii8("UI")))
  {
#if defined(FOG_BUILD_UI)
    // Create the FbEngine by name.
    _fbEngine = createFbEngine(type);
#else
    // Requested to create the FbEngine, but Fog/UI is disabled.
#endif
  }

  // Create EventLoop by type.
  _eventLoop = createEventLoop(type);
  if (_eventLoop)
    Thread::getMainThread()->_eventLoop = _eventLoop;

  // Set global application instance (singleton).
  if (_instance == NULL)
    _instance = this;

  // Set main thread event loop (can be NULL if no success).
  Thread::getMainThread()->_eventLoop = _eventLoop;
}

Application::~Application()
{
#if defined(FOG_BUILD_UI)
  // We will unload library here, not by UIEngine destructor, because
  // EventLoop may be also created by UIEngine.
  Library libraryToClose;

  // Delete UIEngine if associated.
  if (_fbEngine)
  {
    libraryToClose = _fbEngine->_library;

    fog_delete(_fbEngine);
    _fbEngine = NULL;
  }
#endif // FOG_BUILD_UI

  // Delete EventLoop if associated.
  if (_eventLoop)
  {
    fog_delete(_eventLoop);
    _eventLoop = NULL;

    // Unset main thread event loop (this is safe, we will destroy it later
    // if it exists).
    Thread::getMainThread()->_eventLoop = NULL;
  }

  // Clear global application instance (singleton).
  if (_instance == this)
    _instance = NULL;
}

err_t Application::run()
{
  if (!_eventLoop)
    return ERR_RT_INVALID_HANDLE;

  _eventLoop->run();
  return ERR_OK;
}

void Application::quit()
{
  if (!_eventLoop)
    return;

  _eventLoop->quit();
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
GuiEngine* Application::createFbEngine(const StringW& _name)
{
  StringW name(_name);

  // First try to detect UIEngine if not specified
  if (name == Ascii8("UI"))
    name = detectFbEngine();

  // Try to create registered native engine.
  GuiEngine* fbEngine = Application_local->createFbEngine(name);
  if (fbEngine)
    return fbEngine;

  // Otherwise try to load dynamically linked library (based on the UI engine name).
  if (!name.startsWith(Ascii8("UI.")))
    return NULL;

  Library lib;
  err_t err = lib.openPlugin(StringW::fromAscii8("Fog_UI"), name.substring(Range(3, DETECT_LENGTH)));
  if (FOG_IS_ERROR(err))
    return NULL;

  FbEngineConstructor ctor = (FbEngineConstructor)lib.getSymbol("createFbEngine");
  if (FOG_IS_NULL(ctor))
    return NULL;

  fbEngine = ctor();
  if (FOG_IS_NULL(fbEngine))
    return NULL;

  // Success.
  fbEngine->_library = lib;
  return fbEngine;
}

StringW Application::detectFbEngine()
{
#if defined(FOG_OS_WINDOWS)
  return StringW::fromAscii8("UI.Windows");
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
bool Application::registerFbEngine(const StringW& name, FbEngineConstructor ctor)
{
  AutoLock locked(Application_local->lock);
  return Application_local->fbEngineHash.put(name, ctor);
}

bool Application::unregisterFbEngine(const StringW& name)
{
  AutoLock locked(Application_local->lock);
  return Application_local->fbEngineHash.remove(name);
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

EventLoop* Application::createEventLoop(const StringW &_name)
{
  StringW name(_name);

#if defined(FOG_BUILD_UI)
  // First try to detect UI event loop if not specified.
  if (name == Ascii8("UI"))
    name = detectFbEngine();
#endif // FOG_BUILD_UI

  return Application_local->createEventLoop(name);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

static EventLoop* FOG_CDECL Application_DefaultEventLoopConstructor() { return fog_new DefaultEventLoop(); }

#if defined(FOG_BUILD_UI) && defined(FOG_OS_WINDOWS)
static GuiEngine* FOG_CDECL Application_WinFbEngineConstructor() { return fog_new WinGuiEngine(); }
static EventLoop* FOG_CDECL Application_WinFbEventLoopConstructor() { return fog_new WinGuiEventLoop(); }
#endif // FOG_BUILD_UI && FOG_OS_WINDOWS

#if defined(FOG_BUILD_UI) && defined(FOG_OS_MAC)
static GuiEngine* FOG_CDECL Application_MacFbEngineConstructor() { return fog_new MacGuiEngine(); }
static EventLoop* FOG_CDECL Application_MacFbEventLoopConstructor() { return fog_new MacMainEventLoop(); }
#endif // FOG_BUILD_UI && FOG_OS_MAC

FOG_NO_EXPORT void Application_init(void)
{
  Application_local.init();
  StringW type;

  // Default event loop.
  type.set(Ascii8("Default"));
  Application::registerEventLoop(type, Application_DefaultEventLoopConstructor);

#if defined(FOG_BUILD_UI) && defined(FOG_OS_WINDOWS)
  type.set(Ascii8("UI.Windows"));
  Application::registerFbEngine(type, Application_WinFbEngineConstructor);
  Application::registerEventLoop(type, Application_WinFbEventLoopConstructor);
#endif // FOG_BUILD_UI && FOG_OS_WINDOWS

#if defined(FOG_BUILD_UI) && defined(FOG_OS_MAC)
  type.set(Ascii8("UI.Mac"));
  Application::registerFbEngine(type, Application_MacFbEngineConstructor);
  Application::registerEventLoop(type, Application_MacFbEventLoopConstructor);
#endif // FOG_BUILD_UI && FOG_OS_MAC
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
