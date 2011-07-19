// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/System/Application.h>
#include <Fog/Core/System/EventLoop.h>
#include <Fog/Core/System/Object.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/ByteArrayTmp_p.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Core/Win/WinUtil_p.h>
#include <Fog/Gui/Engine/GuiEngine.h>

#if defined(FOG_OS_WINDOWS)
# include <Fog/Gui/Engine/WinGuiEngine.h>
// windows.h is already included by Fog/Core/Config/Config.h
# include <io.h>
#elif defined(FOG_OS_MAC)
# include <Fog/Gui/Engine/MacGuiEngine.h>
#endif

#if defined(FOG_HAVE_UNISTD_H)
# include <errno.h>
# include <unistd.h>
#endif // FOG_HAVE_UNISTD_H

FOG_IMPLEMENT_OBJECT(Fog::Application)

namespace Fog {

// ============================================================================
// [Fog::Application - Local]
// ============================================================================

static void unescapeArgument(String& s)
{
  size_t remain = s.getLength();
  if (!remain) return;

  Char* beg = s.getDataX();
  Char* cur = beg;
  Char* dst = beg;

  while (remain)
  {
    if (cur[0] == Char('\\') && remain > 1 &&
       (cur[1] == Char('\"') ||
        cur[1] == Char('\'') ||
        cur[1] == Char(' ' ) ))
    {
      cur++;
      remain--;
    }
    *dst++ = *cur++;
    remain--;
  }

  s.resize((size_t)(dst - beg));
}

#if defined(FOG_OS_WINDOWS)
static void parseWinCmdLine(const String& cmdLine, List<String>& dst)
{
  const Char* cur = cmdLine.getData();
  const Char* end = cur + cmdLine.getLength();

  const Char* mark;
  Char quote;
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
    if (cur->isNull()) goto end;

    // Parse quote character (if it's here).
    mark = cur;
    quote = 0;

    if (cur[0] == Char('\"') ||
        cur[0] == Char('\'') )
    {
      quote = cur[0];
    }
    cur++;

    // Parse argument
    isEscaped = false;

    for (;;)
    {
      if (cur == end) goto parsed;

      Char c = cur[0];
      if (c.isNull()) goto parsed;
      if (c.isSpace() && quote.isNull()) goto parsed;

      // Quotes.
      if (c == quote)
      {
        cur++;
        goto parsed;
      }

      // Escape sequence.
      if (c == Char('\\'))
      {
        if (cur < end - 1 && (cur[1] == Char('\"') ||
                              cur[1] == Char('\'') ||
                              cur[1] == Char(' ')) )
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
      String arg(mark, len);
      if (isEscaped) unescapeArgument(arg);
      dst.append(arg);
    }
  }

end:
  return;
}
#endif // FOG_OS_WINDOWS

struct FOG_NO_EXPORT Application_Local
{
  typedef UnorderedHash<String, Application::GuiEngineConstructor> GuiEngineHash;
  typedef UnorderedHash<String, Application::EventLoopConstructor> EventLoopHash;

  Lock lock;

  GuiEngineHash guiEngineHash;
  EventLoopHash eventLoopHash;

  String applicationExecutable;
  List<String> applicationArguments;

  Application_Local();
  ~Application_Local();

  GuiEngine* createGuiEngine(const String& name);
  EventLoop* createEventLoop(const String& name);

  void applicationArgumentsWasSet();
};

Application_Local::Application_Local()
{
#if defined(FOG_OS_WINDOWS)
  String applicationCommand;

  applicationCommand.set(reinterpret_cast<const Char*>(::GetCommandLineW()));
  parseWinCmdLine(applicationCommand, applicationArguments);

  applicationArgumentsWasSet();
#endif // FOG_OS_WINDOWS
}

Application_Local::~Application_Local()
{
}

GuiEngine* Application_Local::createGuiEngine(const String& name)
{
  if (name.startsWith(Ascii8("Gui.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createGuiEngine", "Gui engine can be created only by main thread.\n");
    return NULL;
  }

  AutoLock locked(lock);

  Application::GuiEngineConstructor ctor = guiEngineHash.value(name, NULL);
  return ctor ? ctor() : NULL;
}

EventLoop* Application_Local::createEventLoop(const String& name)
{
  if (name.startsWith(Ascii8("Gui.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createEventLoop()", "Gui event loop can be created only by main thread.\n");
    return NULL;
  }

  AutoLock locked(lock);

  Application::EventLoopConstructor ctor = eventLoopHash.value(name, NULL);
  return ctor ? ctor() : NULL;
}

void Application_Local::applicationArgumentsWasSet()
{
  applicationExecutable = applicationArguments.at(0);
  FileSystem::toAbsolutePath(applicationExecutable, String(), applicationExecutable);

  String applicationDirectory;
  FileSystem::extractDirectory(applicationDirectory, applicationExecutable);

  // Application directory usually contains plugins and library itself under
  // Windows, but we will add it also for posix OSes. It can help if application
  // is started from user home directory.
  Library::addPath(applicationDirectory, Library::PATH_PREPEND);
}

static Static<Application_Local> _core_application_local;

// ============================================================================
// [Fog::Application]
// ============================================================================

Application* Application::_instance = NULL;

Application::Application(const String& type)
{
  _init(type);
}

Application::Application(const String& type, int argc, const char* argv[])
{
  _core_application_init_arguments(argc, argv);
  _init(type);
}

void Application::_init(const String& type)
{
  _eventLoop = NULL;
  _nativeEngine = NULL;

  // Create UIEngine by type.
  if (type.startsWith(Ascii8("Gui"))) _nativeEngine = createGuiEngine(type);

  // Create EventLoop by type.
  _eventLoop = createEventLoop(type);
  if (_eventLoop) Thread::_mainThread->_eventLoop = _eventLoop;

  // Set global application instance (singleton).
  if (_instance == NULL) _instance = this;

  // Set main thread event loop (can be NULL if no success).
  Thread::getMainThread()->_eventLoop = _eventLoop;
}

Application::~Application()
{
  // We will unload library here, not by UIEngine destructor, because
  // EventLoop may be also created by UIEngine.
  Library uiToClose;

  // Delete UIEngine if associated.
  if (_nativeEngine)
  {
    uiToClose = _nativeEngine->_library;

    fog_delete(_nativeEngine);
    _nativeEngine = NULL;
  }

  // Delete EventLoop if associated.
  if (_eventLoop)
  {
    fog_delete(_eventLoop);
    _eventLoop = NULL;

    // Unset main thread event loop (this is safe, we will destroy it later
    // if it exists).
    Thread::_mainThread->_eventLoop = NULL;
  }

  // Clear global application instance (singleton).
  if (_instance == this) _instance = NULL;
}

err_t Application::run()
{
  if (!_eventLoop) return ERR_RT_INVALID_HANDLE;

  _eventLoop->run();
  return ERR_OK;
}

void Application::quit()
{
  if (!_eventLoop) return;

  _eventLoop->quit();
}

// ============================================================================
// [Fog::Application - Application Executable / Arguments]
// ============================================================================

String Application::getApplicationExecutable()
{
  return _core_application_local->applicationExecutable;
}

List<String> Application::getApplicationArguments()
{
  return _core_application_local->applicationArguments;
}

// ============================================================================
// [Fog::Application - Working Directory]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
err_t Application::getWorkingDirectory(String& dst)
{
  FOG_RETURN_ON_ERROR(dst.prepare(256));

  for (;;)
  {
    DWORD size = GetCurrentDirectoryW((DWORD)dst.getCapacity() + 1, reinterpret_cast<wchar_t*>(dst.getDataX()));
    if (size >= dst.getCapacity())
    {
      FOG_RETURN_ON_ERROR(dst.reserve(size));
      continue;
    }
    else
    {
      FOG_RETURN_ON_ERROR(dst.resize(size));
      return dst.slashesToPosix();
    }
  }
}

err_t Application::setWorkingDirectory(const String& _dir)
{
  err_t err;
  String dir = _dir;

  if ((err = dir.slashesToWin())) return err;

  if (SetCurrentDirectoryW(reinterpret_cast<const wchar_t*>(dir.getData())) == 0)
    return ERR_OK;
  else
    return GetLastError();
}
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
err_t Application::getWorkingDirectory(String& dst)
{
  ByteArrayTmp<TEMPORARY_LENGTH> dir8;
  dst.clear();

  for (;;)
  {
    const char* ptr = ::getcwd(dir8.getDataX(), dir8.getCapacity() + 1);
    if (ptr) return TextCodec::local8().decode(dst, Stub8(ptr, DETECT_LENGTH));

    if (errno != ERANGE) return errno;

    // Alloc more...
    FOG_RETURN_ON_ERROR(dir8.reserve(dir8.getCapacity() + 4096));
  }
}

err_t Application::setWorkingDirectory(const String& dir)
{
  ByteArrayTmp<TEMPORARY_LENGTH> dir8;
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

String Application::detectGuiEngine()
{
#if defined(FOG_OS_WINDOWS)
  return Ascii8("Gui.Windows");
#elif defined(FOG_OS_MAC)
  return Ascii8("Gui.Mac");
#elif defined(FOG_OS_POSIX)
  return Ascii8("Gui.X11");
#endif // FOG_OS_POSIX
}

GuiEngine* Application::createGuiEngine(const String& _name)
{
  String name(_name);

  // First try to detect UIEngine if not specified
  if (name == Ascii8("Gui")) name = detectGuiEngine();

  // Try to create registered native engine.
  GuiEngine* ge = _core_application_local->createGuiEngine(name);
  if (ge) return ge;

  // Otherwise try to load dynamically linked library (based on the gui engine name).
  if (!name.startsWith(Ascii8("Gui."))) return NULL;

  Library lib;
  err_t err = lib.openPlugin(Ascii8("Fog_Gui"), name.substring(Range(4, DETECT_LENGTH)));
  if (FOG_IS_ERROR(err)) return NULL;

  GuiEngineConstructor ctor = (GuiEngineConstructor)lib.getSymbol(Ascii8("createGuiEngine"));
  if (FOG_IS_NULL(ctor)) return NULL;

  ge = ctor();
  if (FOG_IS_NULL(ge)) return NULL;

  // Success
  ge->_library = lib;
  return ge;
}

// ============================================================================
// [Fog::Application - UIEngine - Register / Unregister]
// ============================================================================

bool Application::registerGuiEngine(const String& name, GuiEngineConstructor ctor)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->guiEngineHash.put(name, ctor);
}

bool Application::unregisterGuiEngine(const String& name)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->guiEngineHash.remove(name);
}

// ============================================================================
// [Fog::Application - EventLoop - Register / Unregister]
// ============================================================================

bool Application::registerEventLoop(const String& name, EventLoopConstructor ctor)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->eventLoopHash.put(name, ctor);
}

bool Application::unregisterEventLoop(const String& name)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->eventLoopHash.remove(name);
}

// ============================================================================
// [Fog::Application - EventLoop - Access]
// ============================================================================

EventLoop* Application::createEventLoop(const String &_name)
{
  String name(_name);

  // First try to detect Gui event loop if not specified.
  if (name == Ascii8("Gui")) name = detectGuiEngine();

  return _core_application_local->createEventLoop(name);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Application_init(void)
{
  _core_application_local.init();
  Application::registerEventLoopType<DefaultEventLoop>(Ascii8("Default"));

#if defined(FOG_OS_WINDOWS)
  {
    String winGuiEngineName(Ascii8("Gui.Windows"));
    Application::registerGuiEngineType<WinGuiEngine>(winGuiEngineName);
    Application::registerEventLoopType<WinGuiEventLoop>(winGuiEngineName);
  }
#elif defined(FOG_OS_MAC)
  {
    Application::registerGuiEngineType<MacGuiEngine>(Ascii8("Gui.Mac"));
    Application::registerEventLoopType<MacMainEventLoop>(Ascii8("Gui.Mac"));
  }
#endif
}

FOG_NO_EXPORT void Application_fini(void)
{
  _core_application_local.destroy();
}

// ============================================================================
// [Fog::Application - initArguments]
// ============================================================================

void _core_application_init_arguments(int argc, const char* argv[])
{
  if (argc < 1) return;

  AutoLock locked(_core_application_local->lock);

  List<String>& arguments = _core_application_local->applicationArguments;
  if (arguments.getLength() != 0) return;

  for (int i = 0; i < argc; i++)
  {
    String arg;
    TextCodec::local8().decode(arg, Stub8(argv[i], DETECT_LENGTH));
    arguments.append(arg);
  }

  _core_application_local->applicationArgumentsWasSet();
}

} // Fog namespace
