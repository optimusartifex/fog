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
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/OS/System.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Gui/Engine/GuiEngine.h>

#if defined(FOG_OS_WINDOWS)
# include <Fog/Gui/Engine/WinGuiEngine.h>
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

static void unescapeArgument(StringW& s)
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

#if defined(FOG_OS_WINDOWS)
static void parseWinCmdLine(const StringW& cmdLine, List<StringW>& dst)
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
    if (cur->isNull()) goto end;

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
  typedef Hash<StringW, Application::GuiEngineConstructor> GuiEngineHash;
  typedef Hash<StringW, Application::EventLoopConstructor> EventLoopHash;

  Lock lock;

  GuiEngineHash guiEngineHash;
  EventLoopHash eventLoopHash;

  StringW applicationExecutable;
  List<StringW> applicationArguments;

  Application_Local();
  ~Application_Local();

  GuiEngine* createGuiEngine(const StringW& name);
  EventLoop* createEventLoop(const StringW& name);

  void applicationArgumentsWasSet();
};

Application_Local::Application_Local()
{
#if defined(FOG_OS_WINDOWS)
  StringW applicationCommand;

  applicationCommand.set(reinterpret_cast<const CharW*>(::GetCommandLineW()));
  parseWinCmdLine(applicationCommand, applicationArguments);

  applicationArgumentsWasSet();
#endif // FOG_OS_WINDOWS
}

Application_Local::~Application_Local()
{
}

GuiEngine* Application_Local::createGuiEngine(const StringW& name)
{
  if (name.startsWith(Ascii8("UI.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createGuiEngine", "UI engine can be created only by main thread.\n");
    return NULL;
  }

  AutoLock locked(lock);

  Application::GuiEngineConstructor ctor = guiEngineHash.get(name, NULL);
  return ctor ? ctor() : NULL;
}

EventLoop* Application_Local::createEventLoop(const StringW& name)
{
  if (name.startsWith(Ascii8("UI.")) && !Thread::isMainThread())
  {
    Debug::dbgFunc("Fog::Application", "createEventLoop", "UI event loop can be created only by main thread.\n");
    return NULL;
  }

  AutoLock locked(lock);

  Application::EventLoopConstructor ctor = eventLoopHash.get(name, NULL);
  return ctor ? ctor() : NULL;
}

void Application_Local::applicationArgumentsWasSet()
{
  applicationExecutable = applicationArguments.getAt(0);
  FileSystem::toAbsolutePath(applicationExecutable, StringW(), applicationExecutable);

  StringW applicationDirectory;
  FileSystem::extractDirectory(applicationDirectory, applicationExecutable);

  // Application directory usually contains plugins and library itself under
  // Windows, but we will add it also for POSIX OSes. It can help if application
  // is started from user home directory.
  Library::addLibraryPath(applicationDirectory, LIBRARY_PATH_PREPEND);
}

static Static<Application_Local> _core_application_local;

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
  _core_application_init_arguments(argc, argv);
  _init(type);
}

void Application::_init(const StringW& type)
{
  _eventLoop = NULL;
  _nativeEngine = NULL;

  // Create UIEngine by type.
  if (type.startsWith(Ascii8("UI")))
    _nativeEngine = createGuiEngine(type);

  // Create EventLoop by type.
  _eventLoop = createEventLoop(type);
  if (_eventLoop)
    Thread::_mainThread->_eventLoop = _eventLoop;

  // Set global application instance (singleton).
  if (_instance == NULL)
    _instance = this;

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
  return _core_application_local->applicationExecutable;
}

List<StringW> Application::getApplicationArguments()
{
  return _core_application_local->applicationArguments;
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
  FOG_RETURN_ON_ERROR(System::makeWindowsPath(dirW, dir));

  if (SetCurrentDirectoryW(reinterpret_cast<const wchar_t*>(dirW.getData())) == 0)
    return ERR_OK;
  else
    return System::errorFromOSLastError();
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

StringW Application::detectGuiEngine()
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

GuiEngine* Application::createGuiEngine(const StringW& _name)
{
  StringW name(_name);

  // First try to detect UIEngine if not specified
  if (name == Ascii8("UI"))
    name = detectGuiEngine();

  // Try to create registered native engine.
  GuiEngine* ge = _core_application_local->createGuiEngine(name);
  if (ge)
    return ge;

  // Otherwise try to load dynamically linked library (based on the UI engine name).
  if (!name.startsWith(Ascii8("UI.")))
    return NULL;

  Library lib;
  err_t err = lib.openPlugin(StringW::fromAscii8("Fog_UI"), name.substring(Range(3, DETECT_LENGTH)));
  if (FOG_IS_ERROR(err))
    return NULL;

  GuiEngineConstructor ctor = (GuiEngineConstructor)lib.getSymbol("createGuiEngine");
  if (FOG_IS_NULL(ctor))
    return NULL;

  ge = ctor();
  if (FOG_IS_NULL(ge))
    return NULL;

  // Success.
  ge->_library = lib;
  return ge;
}

// ============================================================================
// [Fog::Application - UIEngine - Register / Unregister]
// ============================================================================

bool Application::registerGuiEngine(const StringW& name, GuiEngineConstructor ctor)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->guiEngineHash.put(name, ctor);
}

bool Application::unregisterGuiEngine(const StringW& name)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->guiEngineHash.remove(name);
}

// ============================================================================
// [Fog::Application - EventLoop - Register / Unregister]
// ============================================================================

bool Application::registerEventLoop(const StringW& name, EventLoopConstructor ctor)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->eventLoopHash.put(name, ctor);
}

bool Application::unregisterEventLoop(const StringW& name)
{
  AutoLock locked(_core_application_local->lock);
  return _core_application_local->eventLoopHash.remove(name);
}

// ============================================================================
// [Fog::Application - EventLoop - Access]
// ============================================================================

EventLoop* Application::createEventLoop(const StringW &_name)
{
  StringW name(_name);

  // First try to detect UI event loop if not specified.
  if (name == Ascii8("UI"))
    name = detectGuiEngine();

  return _core_application_local->createEventLoop(name);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Application_init(void)
{
  _core_application_local.init();

  StringW type;

  // Default event loop.
  type.set(Ascii8("Default"));
  Application::registerEventLoopType<DefaultEventLoop>(type);

#if defined(FOG_OS_WINDOWS)
  type.set(Ascii8("UI.Windows"));
  Application::registerGuiEngineType<WinGuiEngine>(type);
  Application::registerEventLoopType<WinGuiEventLoop>(type);
#elif defined(FOG_OS_MAC)
  type.set(Ascii8("UI.Mac"));
  Application::registerGuiEngineType<MacGuiEngine>(type);
  Application::registerEventLoopType<MacMainEventLoop>(type);
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

  List<StringW>& arguments = _core_application_local->applicationArguments;
  if (arguments.getLength() != 0) return;

  for (int i = 0; i < argc; i++)
  {
    StringW arg;
    TextCodec::local8().decode(arg, StubA(argv[i], DETECT_LENGTH));
    arguments.append(arg);
  }

  _core_application_local->applicationArgumentsWasSet();
}

} // Fog namespace
