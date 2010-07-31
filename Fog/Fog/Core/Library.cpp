// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Lock.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/String.h>
#include <Fog/Core/TextCodec.h>

#if defined(FOG_LIBRARY_WINDOWS)
#include <windows.h>
#endif // FOG_LIBRARY_WINDOWS

#if defined(FOG_LIBRARY_DL)
#include <dlfcn.h>
#include <errno.h>
#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif
#endif // FOG_LIBRARY_DL

namespace Fog {

// ============================================================================
// [Fog::Library - Local]
// ============================================================================

struct Library_Local
{
  Lock lock;
  List<String> paths;

  String prefix;
  List<String> extensions;
};

static Static<Library_Local> library_local;

// ============================================================================
// [Fog::Library - Platform]
// ============================================================================

#if defined(FOG_LIBRARY_WINDOWS)
static err_t systemOpenLibrary(const String& fileName, void** handle)
{
  TemporaryString<TEMPORARY_LENGTH> fileNameW;
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
#endif // FOG_LIBRARY_WINDOWS

#if defined(FOG_LIBRARY_DL)
static err_t systemOpenLibrary(const String& fileName, void** handle)
{
  err_t err;

  TemporaryByteArray<TEMPORARY_LENGTH> fileName8;
  if ((err = TextCodec::local8().appendFromUnicode(fileName8, fileName))) return err;

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
#endif // FOG_LIBRARY_DL

// ============================================================================
// [Fog::LibraryData]
// ============================================================================

static LibraryData* newLibraryData(void* handle)
{
  LibraryData* d = reinterpret_cast<LibraryData*>(Memory::alloc(sizeof(LibraryData)));
  if (!d) return NULL;

  d->refCount.init(1);
  d->handle = handle;
  return d;
}

static LibraryData* refLibraryData(LibraryData* d)
{
  d->refCount.inc();
  return d;
}

static void derefLibraryData(LibraryData* d)
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
  _d(refLibraryData(_dnull.instancep()))
{
}

Library::Library(const Library& other) :
  _d(refLibraryData(other._d))
{
}

Library::Library(const String& fileName, uint32_t openFlags) :
  _d(refLibraryData(_dnull.instancep()))
{
  open(fileName, openFlags);
}

Library::~Library()
{
  derefLibraryData(_d);
}

err_t Library::open(const String& _fileName, uint32_t openFlags)
{
  close();

  err_t err;
  err_t libOpenError = ERR_LIBRARY_LOAD_FAILED;

  TemporaryString<TEMPORARY_LENGTH> fileName;
  List<String> extensions = library_local->extensions;

  for (sysuint_t i = 0; i < extensions.getLength(); i++)
  {
    if ( (err = fileName.setDeep(_fileName)) ) goto fail;

    if ((openFlags & LIBRARY_OPEN_SYSTEM_PREFIX) != 0)
    {
      if ( (err = fileName.insert(fileName.lastIndexOf(Char('/')) + 1, library_local->prefix)) ) goto fail;
    }
    if ((openFlags & LIBRARY_OPEN_SYSTEM_SUFFIX) != 0)
    {
      if ( (err = fileName.append(Char('.'))) || (err = fileName.append(extensions.at(i))) ) goto fail;
    }

    void* handle;
    if ((libOpenError = systemOpenLibrary(fileName, &handle)))
    {
      if (openFlags & LIBRARY_OPEN_SYSTEM_SUFFIX) continue;
      goto end;
    }

    LibraryData* newd = newLibraryData(handle);
    if (!newd)
    {
      systemCloseLibrary(handle);
      return ERR_RT_OUT_OF_MEMORY;
    }

    derefLibraryData(atomicPtrXchg(&_d, newd));
    return ERR_OK;
  }

end:
  err = libOpenError;

fail:
  return err;
}

err_t Library::openPlugin(const String& category, const String& fileName)
{
  close();

  err_t err;
  err_t libOpenError = ERR_LIBRARY_LOAD_FAILED;

  TemporaryString<TEMPORARY_LENGTH> relative;
  TemporaryString<TEMPORARY_LENGTH> absolute;

  List<String> extensions = library_local->extensions;

  for (sysuint_t i = 0; i < extensions.getLength(); i++)
  {
    if ( (err = relative.setDeep(library_local->prefix)) ) goto fail;
    if (!category.isEmpty())
    {
      if ((err = relative.append(category)) ||
          (err = relative.append(Char('_')))) goto fail;
    }
    if ((err = relative.append(fileName)) ||
        (err = relative.append(Char('.'))) ||
        (err = relative.append(extensions.at(i))) ) goto fail;

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

fail:
  return err;
}

void Library::close()
{
  derefLibraryData(
    atomicPtrXchg(&_d, refLibraryData(_dnull.instancep()))
  );
}

void* Library::getSymbol(const char* symbolName)
{
  return systemLoadSymbol(_d->handle, symbolName);
}

void* Library::getSymbol(const String& symbolName)
{
  TemporaryByteArray<TEMPORARY_LENGTH> symb8;
  TextCodec::utf8().fromUnicode(symb8, symbolName);
  return systemLoadSymbol(_d->handle, symb8.getData());
}

sysuint_t Library::getSymbols(void** target, const char* symbols, sysuint_t symbolsLength, sysuint_t symbolsCount, char** fail)
{
  sysuint_t i;

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
      if (symbolsCur == symbolsEnd) goto end;
      if (*symbolsCur++ == 0) break;
    }

    if ((address = systemLoadSymbol(handle, current)) == NULL)
    {
      if (fail) *fail = (char*)current;
      goto end;
    }

    target[i] = address;
  }

end:
  return i;
}

// ============================================================================
// [Fog::Library::Operator Overload]
// ============================================================================

Library& Library::operator=(const Library& other)
{
  derefLibraryData(
    atomicPtrXchg(&_d, refLibraryData(other._d))
  );
  return *this;
}

// ============================================================================
// [Fog::Library::Paths]
// ============================================================================

List<String> Library::paths()
{
  AutoLock locked(library_local.instance().lock);
  return library_local.instance().paths;
}

bool Library::addPath(const String& path, int pathMode)
{
  AutoLock locked(library_local.instance().lock);
  if (path.isEmpty()) return false;
  if (library_local.instance().paths.indexOf(path) != INVALID_INDEX) return false;

  if (pathMode == PATH_APPEND)
    library_local.instance().paths.append(path);
  else
    library_local.instance().paths.prepend(path);
  return true;
}

bool Library::removePath(const String& path)
{
  AutoLock locked(library_local.instance().lock);
  return library_local.instance().paths.remove(path) != 0;
}

bool Library::hasPath(const String& path)
{
  AutoLock locked(library_local.instance().lock);
  return library_local.instance().paths.indexOf(path) != INVALID_INDEX;
}

// ============================================================================
// [Fog::Library::System]
// ============================================================================

String Library::getSystemPrefix()
{
  return library_local->prefix;
}

List<String> Library::getSystemExtensions()
{
  return library_local->extensions;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_library_init(void)
{
  using namespace Fog;

  Library::_dnull.init();
  LibraryData* d = Library::_dnull.instancep();
  d->refCount.init(1);
  d->handle = NULL;

  library_local.init();

#if defined(FOG_OS_WINDOWS)
  // There is no system prefix under Windows (empty).
  library_local->extensions.append(Ascii8("dll"));
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
# if defined(FOG_OS_HPUX)
  library_local->prefix.set(Ascii8("lib"));
  library_local->extensions.append(Ascii8("sl"));
# elif defined(FOG_OS_MAC)
  library_local->prefix.set(Ascii8("lib"));
  // It seems that there are used three library extensions under MAC:
  library_local->extensions.append(Ascii8("bundle"));
  library_local->extensions.append(Ascii8("dylib"));
  library_local->extensions.append(Ascii8("so"));
# else
  library_local->prefix.set(Ascii8("lib"));
  library_local->extensions.append(Ascii8("so"));
# endif
#endif // FOG_OS_POSIX

#if defined(FOG_OS_POSIX)
#if FOG_ARCH_BITS == 32
  library_local->paths.append(Ascii8("/usr/lib"));
  library_local->paths.append(Ascii8("/usr/local/lib"));
#else
  library_local->paths.append(Ascii8("/usr/lib64"));
  library_local->paths.append(Ascii8("/usr/local/lib64"));
#endif // FOG_ARCH_BITS
#endif // FOG_OS_POSIX

#if defined(FOG_OS_MAC)
  // The default path of MacPorts.
  library_local->paths.append(Ascii8("/opt/local/lib"));
#endif // FOG_OS_MAC

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_library_shutdown(void)
{
  using namespace Fog;

  library_local.destroy();
  Library::_dnull->refCount.dec();
}
