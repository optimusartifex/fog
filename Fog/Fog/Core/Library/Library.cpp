// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/ByteArrayTmp_p.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>

// [Dependencies - Windows]
#if defined(FOG_OS_WINDOWS)
# include <windows.h>
#endif // FOG_OS_WINDOWS

// [Dependencies - Posix]
#if defined(FOG_OS_POSIX)
# include <dlfcn.h>
# include <errno.h>
# ifndef RTLD_NOW
#  define RTLD_NOW 0
# endif
#endif

namespace Fog {

// ============================================================================
// [Fog::Library - Local]
// ============================================================================

struct FOG_NO_EXPORT Library_Local
{
  Lock lock;
  List<String> paths;

  String prefix;
  List<String> extensions;
};

static Static<Library_Local> _core_library_local;

// ============================================================================
// [Fog::Library - Windows Support]
// ============================================================================

#if defined(FOG_OS_WINDOWS)

static err_t systemOpenLibrary(const String& fileName, void** handle)
{
  StringTmp<TEMPORARY_LENGTH> fileNameW;
  err_t err;

  if ((err = fileNameW.set(fileName)) || (err = fileNameW.slashesToWin()))
  {
    return err;
  }

  HMODULE hLibrary = ::LoadLibraryW(reinterpret_cast<const wchar_t*>(fileNameW.getData()));
  if (!hLibrary) return GetLastError();

  *handle = (void*)hLibrary;
  return ERR_OK;
}

static void systemCloseLibrary(void* handle)
{
  ::FreeLibrary((HMODULE)handle);
}

static void* systemLoadSymbol(void* handle, const char* symbol)
{
  if (handle == NULL) return NULL;
  return (void*)GetProcAddress((HINSTANCE)handle, symbol);
}

// ============================================================================
// [Fog::Library - Posix Support]
// ============================================================================

#else

static err_t systemOpenLibrary(const String& fileName, void** handle)
{
  ByteArrayTmp<TEMPORARY_LENGTH> fileName8;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(fileName8, fileName));

  void* h = (void*)::dlopen(fileName8.getData(), RTLD_NOW);
  if (h == NULL)
  {
    // If something bad happens...
    // printf("ERROR: %s\n", dlerror());
    return ERR_LIBRARY_LOAD_FAILED;
  }

  *handle = h;
  return ERR_OK;
}

static void systemCloseLibrary(void* handle)
{
  ::dlclose(handle);
}

static void* systemLoadSymbol(void* handle, const char* symbol)
{
  if (handle == NULL) return NULL;
  return (void *)::dlsym(handle, symbol);
}

#endif

// ============================================================================
// [Fog::LibraryData]
// ============================================================================

static LibraryData* _Library_alloc(void* handle)
{
  LibraryData* d = reinterpret_cast<LibraryData*>(Memory::alloc(sizeof(LibraryData)));
  if (FOG_IS_NULL(d)) return NULL;

  d->refCount.init(1);
  d->handle = handle;
  return d;
}

static FOG_INLINE LibraryData* _Library_ref(LibraryData* d)
{
  d->refCount.inc();
  return d;
}

static FOG_INLINE void _Library_deref(LibraryData* d)
{
  if (d->refCount.deref())
  {
    if (d->handle) systemCloseLibrary(d->handle);
    Memory::free(d);
  }
}

// ============================================================================
// [Fog::Library]
// ============================================================================

Static<LibraryData> Library::_dnull;

Library::Library() :
  _d(_Library_ref(_dnull.instancep()))
{
}

Library::Library(const Library& other) :
  _d(_Library_ref(other._d))
{
}

Library::Library(const String& fileName, uint32_t openFlags) :
  _d(_Library_ref(_dnull.instancep()))
{
  open(fileName, openFlags);
}

Library::~Library()
{
  _Library_deref(_d);
}

err_t Library::open(const String& _fileName, uint32_t openFlags)
{
  close();

  err_t err;
  err_t libOpenError = ERR_LIBRARY_LOAD_FAILED;

  StringTmp<TEMPORARY_LENGTH> fileName;
  List<String> extensions = _core_library_local->extensions;

  for (size_t i = 0; i < extensions.getLength(); i++)
  {
    if ( (err = fileName.setDeep(_fileName)) ) goto _Fail;

    if ((openFlags & LIBRARY_OPEN_SYSTEM_PREFIX) != 0)
    {
      if ( (err = fileName.insert(fileName.lastIndexOf(Char('/')) + 1, _core_library_local->prefix)) ) goto _Fail;
    }
    if ((openFlags & LIBRARY_OPEN_SYSTEM_SUFFIX) != 0)
    {
      if ( (err = fileName.append(Char('.'))) || (err = fileName.append(extensions.at(i))) ) goto _Fail;
    }

    void* handle;
    if ((libOpenError = systemOpenLibrary(fileName, &handle)))
    {
      if (openFlags & LIBRARY_OPEN_SYSTEM_SUFFIX) continue;
      goto _End;
    }

    LibraryData* newd = _Library_alloc(handle);
    if (FOG_IS_NULL(newd))
    {
      systemCloseLibrary(handle);
      return ERR_RT_OUT_OF_MEMORY;
    }

    _Library_deref(atomicPtrXchg(&_d, newd));
    return ERR_OK;
  }

_End:
  err = libOpenError;

_Fail:
  return err;
}

err_t Library::openPlugin(const String& category, const String& fileName)
{
  close();

  err_t err;
  err_t libOpenError = ERR_LIBRARY_LOAD_FAILED;

  StringTmp<TEMPORARY_LENGTH> relative;
  StringTmp<TEMPORARY_LENGTH> absolute;

  List<String> extensions = _core_library_local->extensions;

  for (size_t i = 0; i < extensions.getLength(); i++)
  {
    if ( (err = relative.setDeep(_core_library_local->prefix)) ) goto _Fail;
    if (!category.isEmpty())
    {
      if ((err = relative.append(category)) ||
          (err = relative.append(Char('_')))) goto _Fail;
    }
    if ((err = relative.append(fileName)) ||
        (err = relative.append(Char('.'))) ||
        (err = relative.append(extensions.at(i))) ) goto _Fail;

    if (FileSystem::findFile(paths(), relative, absolute))
    {
      if ((libOpenError = open(absolute, 0)) == ERR_OK) return ERR_OK;
    }
    else
    {
      libOpenError = ERR_IO_FILE_NOT_EXISTS;
    }
  }

  err = libOpenError;

_Fail:
  return err;
}

void Library::close()
{
  _Library_deref(
    atomicPtrXchg(&_d, _Library_ref(_dnull.instancep()))
  );
}

void* Library::getSymbol(const char* symbolName)
{
  return systemLoadSymbol(_d->handle, symbolName);
}

void* Library::getSymbol(const String& symbolName)
{
  ByteArrayTmp<TEMPORARY_LENGTH> symb8;
  TextCodec::utf8().encode(symb8, symbolName, NULL);
  return systemLoadSymbol(_d->handle, symb8.getData());
}

size_t Library::getSymbols(void** target, const char* symbols, size_t symbolsLength, size_t symbolsCount, char** fail)
{
  size_t i;

  void* address;
  void* handle = _d->handle;
  const char* current;
  const char* symbolsCur = symbols;
  const char* symbolsEnd = symbols + symbolsLength;

  if (fail) *fail = NULL;

  for (i = 0; i != symbolsCount; i++)
  {
    current = symbolsCur;
    for (;;)
    {
      if (symbolsCur == symbolsEnd) goto _End;
      if (*symbolsCur++ == 0) break;
    }

    if ((address = systemLoadSymbol(handle, current)) == NULL)
    {
      if (fail) *fail = (char*)current;
      goto _End;
    }

    target[i] = address;
  }

_End:
  return i;
}

// ============================================================================
// [Fog::Library::Operator Overload]
// ============================================================================

Library& Library::operator=(const Library& other)
{
  _Library_deref(
    atomicPtrXchg(&_d, _Library_ref(other._d))
  );
  return *this;
}

// ============================================================================
// [Fog::Library::Paths]
// ============================================================================

List<String> Library::paths()
{
  AutoLock locked(_core_library_local.instance().lock);
  return _core_library_local.instance().paths;
}

bool Library::addPath(const String& path, int pathMode)
{
  AutoLock locked(_core_library_local.instance().lock);
  if (path.isEmpty()) return false;
  if (_core_library_local.instance().paths.indexOf(path) != INVALID_INDEX) return false;

  if (pathMode == PATH_APPEND)
    _core_library_local.instance().paths.append(path);
  else
    _core_library_local.instance().paths.prepend(path);
  return true;
}

bool Library::removePath(const String& path)
{
  AutoLock locked(_core_library_local.instance().lock);
  return _core_library_local.instance().paths.remove(path) != 0;
}

bool Library::hasPath(const String& path)
{
  AutoLock locked(_core_library_local.instance().lock);
  return _core_library_local.instance().paths.indexOf(path) != INVALID_INDEX;
}

// ============================================================================
// [Fog::Library::System]
// ============================================================================

String Library::getSystemPrefix()
{
  return _core_library_local->prefix;
}

List<String> Library::getSystemExtensions()
{
  return _core_library_local->extensions;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Library_init(void)
{
  LibraryData* d = Library::_dnull.init();
  d->refCount.init(1);
  d->handle = NULL;

  _core_library_local.init();

#if defined(FOG_OS_WINDOWS)
  // There is no system prefix under Windows (empty).
  _core_library_local->extensions.append(Ascii8("dll"));
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
# if defined(FOG_OS_HPUX)
  _core_library_local->prefix.set(Ascii8("lib"));
  _core_library_local->extensions.append(Ascii8("sl"));
# elif defined(FOG_OS_MAC)
  _core_library_local->prefix.set(Ascii8("lib"));
  // It seems that there are used three library extensions under MAC:
  _core_library_local->extensions.append(Ascii8("bundle"));
  _core_library_local->extensions.append(Ascii8("dylib"));
  _core_library_local->extensions.append(Ascii8("so"));
# else
  _core_library_local->prefix.set(Ascii8("lib"));
  _core_library_local->extensions.append(Ascii8("so"));
# endif
#endif // FOG_OS_POSIX

#if defined(FOG_OS_POSIX)
#if FOG_ARCH_BITS == 32
  _core_library_local->paths.append(Ascii8("/usr/lib"));
  _core_library_local->paths.append(Ascii8("/usr/local/lib"));
#else
  _core_library_local->paths.append(Ascii8("/usr/lib64"));
  _core_library_local->paths.append(Ascii8("/usr/local/lib64"));
#endif // FOG_ARCH_BITS
#endif // FOG_OS_POSIX

#if defined(FOG_OS_MAC)
  // The default path of MacPorts.
  _core_library_local->paths.append(Ascii8("/opt/local/lib"));
#endif // FOG_OS_MAC
}

FOG_NO_EXPORT void Library_fini(void)
{
  _core_library_local.destroy();
  Library::_dnull->refCount.dec();
}

} // Fog namespace
