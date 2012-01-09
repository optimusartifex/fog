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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/OS/FileUtil.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringTmp_p.h>
#include <Fog/Core/Tools/TextCodec.h>

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
// [Fog::Library - Global]
// ============================================================================

static Static<LibraryData> Library_dNone;

// ============================================================================
// [Fog::Library - Helpers]
// ============================================================================

struct FOG_NO_EXPORT LibraryGlobal
{
  Lock lock;
  List<StringW> paths;

  StringW prefix;
  List<StringW> extensions;
};

static Static<LibraryGlobal> Library_global;

// ============================================================================
// [Fog::Library - Windows Support]
// ============================================================================

#if defined(FOG_OS_WINDOWS)
static FOG_INLINE err_t LibraryHandle_open(const StringW& fileName, void** handle)
{
  StringTmpW<TEMPORARY_LENGTH> fileNameW;
  FOG_RETURN_ON_ERROR(fileNameW.setAndNormalizeSlashes(fileName, SLASH_FORM_WINDOWS));

  HMODULE hLibrary = ::LoadLibraryW(reinterpret_cast<const wchar_t*>(fileNameW.getData()));
  if (!hLibrary)
    return OSUtil::getErrFromOSLastError();

  *handle = (void*)hLibrary;
  return ERR_OK;
}

static FOG_INLINE void LibraryHandle_close(void* handle)
{
  ::FreeLibrary((HMODULE)handle);
}

static FOG_INLINE void* LibraryHandle_getSymbol(void* handle, const char* sym)
{
  return (void*)GetProcAddress((HINSTANCE)handle, sym);
}
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Library - Posix Support]
// ============================================================================

#if defined(FOG_OS_POSIX)
static FOG_INLINE err_t LibraryHandle_open(const StringW& fileName, void** handle)
{
  StringTmpA<TEMPORARY_LENGTH> fileName8;
  FOG_RETURN_ON_ERROR(TextCodec::local8().encode(fileName8, fileName));

  void* h = (void*)::dlopen(fileName8.getData(), RTLD_NOW);
  if (h == NULL)
  {
    // If something bad happens...
    Logger::error("Fog::Library", "open", "Failed to dlopen() library: %s", dlerror());
    return ERR_LIBRARY_LOAD_FAILED;
  }

  *handle = h;
  return ERR_OK;
}

static FOG_INLINE void LibraryHandle_close(void* handle)
{
  ::dlclose(handle);
}

static FOG_INLINE void* LibraryHandle_getSymbol(void* handle, const char* sym)
{
  return (void *)::dlsym(handle, sym);
}
#endif

// ============================================================================
// [Fog::Library - Construction / Destruction]
// ============================================================================

static void FOG_CDECL Library_ctor(Library* self)
{
  self->_d = Library_dNone->addRef();
}

static void FOG_CDECL Library_ctorCopy(Library* self, const Library* other)
{
  self->_d = other->_d->addRef();
}

static void FOG_CDECL Library_dtor(Library* self)
{
  LibraryData* d = self->_d;

  if (d != NULL)
    d->release();
}

// ============================================================================
// [Fog::Library - Open / Close]
// ============================================================================

static void FOG_CDECL Library_setLibrary(Library* self, const Library* other)
{
  atomicPtrXchg(&self->_d, other->_d->addRef())->release();
}

static err_t FOG_CDECL Library_openLibrary(Library* self, const StringW* fileName, uint32_t flags)
{
  self->close();
  err_t libOpenError = ERR_LIBRARY_LOAD_FAILED;

  StringTmpW<TEMPORARY_LENGTH> libraryName;
  List<StringW> extensions = Library_global->extensions;

  for (size_t i = 0; i < extensions.getLength(); i++)
  {
    FOG_RETURN_ON_ERROR(libraryName.setDeep(*fileName));

    if ((flags & LIBRARY_OPEN_SYSTEM_PREFIX) != 0)
    {
      FOG_RETURN_ON_ERROR(libraryName.insert(libraryName.lastIndexOf(CharW('/')) + 1, Library_global->prefix));
    }

    if ((flags & LIBRARY_OPEN_SYSTEM_SUFFIX) != 0)
    {
      FOG_RETURN_ON_ERROR(libraryName.append(CharW('.')));
      FOG_RETURN_ON_ERROR(libraryName.append(extensions.getAt(i)));
    }

    void* handle;
    libOpenError = LibraryHandle_open(libraryName, &handle);

    if (FOG_IS_ERROR(libOpenError))
    {
      if (flags & LIBRARY_OPEN_SYSTEM_SUFFIX)
        continue;
      break;
    }

    LibraryData* newd = fog_api.library_dCreate(handle);
    if (FOG_IS_NULL(newd))
    {
      LibraryHandle_close(handle);
      return ERR_RT_OUT_OF_MEMORY;
    }

    atomicPtrXchg(&self->_d, newd)->release();
    return ERR_OK;
  }

  return libOpenError;
}

static err_t FOG_CDECL Library_openPlugin(Library* self, const StringW* category, const StringW* name)
{
  self->close();
  err_t libOpenError = ERR_LIBRARY_LOAD_FAILED;

  StringTmpW<TEMPORARY_LENGTH> relative;
  StringTmpW<TEMPORARY_LENGTH> absolute;

  List<StringW> paths = Library::getLibraryPaths();
  List<StringW> extensions = Library_global->extensions;

  for (size_t i = 0; i < extensions.getLength(); i++)
  {
    FOG_RETURN_ON_ERROR(relative.setDeep(Library_global->prefix));

    if (!category->isEmpty())
    {
      FOG_RETURN_ON_ERROR(relative.append(*category));
      FOG_RETURN_ON_ERROR(relative.append(CharW('_')));
    }

    FOG_RETURN_ON_ERROR(relative.append(*name));
    FOG_RETURN_ON_ERROR(relative.append(CharW('.')));
    FOG_RETURN_ON_ERROR(relative.append(extensions.getAt(i)));

    if (FileUtil::findFile(absolute, relative, paths))
    {
      if ((libOpenError = self->openLibrary(absolute, NO_FLAGS)) == ERR_OK)
        return ERR_OK;
    }
    else
    {
      libOpenError = ERR_FILE_NOT_FOUND;
    }
  }

  return libOpenError;
}

static void FOG_CDECL Library_close(Library* self)
{
  atomicPtrXchg(&self->_d, Library_dNone->addRef())->release();
}

// ============================================================================
// [Fog::Library - GetSymbol]
// ============================================================================

static void* FOG_CDECL Library_getSymbolStubA(const Library* self, const StubA* sym)
{
  void* handle = self->_d->handle;
  if (!handle)
    return NULL;

  if (sym->getLength() == DETECT_LENGTH || sym->getData()[sym->getLength() == 0])
    return (void *)LibraryHandle_getSymbol(handle, sym->getData());

  StringTmpA<128> symA;
  if (symA.set(*sym) != ERR_OK)
    return NULL;

  return (void *)LibraryHandle_getSymbol(handle, symA.getData());
}

static void* FOG_CDECL Library_getSymbolStringW(const Library* self, const StringW* sym)
{
  void* handle = self->_d->handle;
  if (!handle)
    return NULL;

  StringTmpA<TEMPORARY_LENGTH> symA;
  if (TextCodec::utf8().encode(symA, *sym, NULL) != ERR_OK)
    return NULL;

  return (void *)LibraryHandle_getSymbol(handle, symA.getData());
}

// ============================================================================
// [Fog::Library - GetSymbols]
// ============================================================================

static size_t FOG_CDECL Library_getSymbols(const Library* self, void** target, const char* symbolsData, size_t symbolsLength, size_t symbolsCount, char** fail)
{
  size_t i;

  void* address;
  void* handle = self->_d->handle;
  const char* current;

  const char* symbolsPtr = symbolsData;
  const char* symbolsEnd = symbolsData + symbolsLength;

  if (fail)
    *fail = NULL;

  if (handle == NULL)
    return 0;

  for (i = 0; i < symbolsCount; i++)
  {
    current = symbolsPtr;
    for (;;)
    {
      if (symbolsPtr == symbolsEnd) goto _End;
      if (*symbolsPtr++ == 0) break;
    }

    if ((address = LibraryHandle_getSymbol(handle, current)) == NULL)
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
// [Fog::Library - LibraryData]
// ============================================================================

static LibraryData* Library_dCreate(void* handle)
{
  LibraryData* d = reinterpret_cast<LibraryData*>(MemMgr::alloc(sizeof(LibraryData)));
  if (FOG_IS_NULL(d))
    return NULL;

  d->reference.init(1);
  d->handle = handle;
  return d;
}

static FOG_INLINE void Library_dFree(LibraryData* d)
{
  LibraryHandle_close(d->handle);
  MemMgr::free(d);
}

// ============================================================================
// [Fog::Library - Statics]
// ============================================================================

static err_t FOG_CDECL Library_getSystemPrefix(StringW* dst)
{
  return dst->set(Library_global->prefix);
}

static err_t FOG_CDECL Library_getSystemExtensions(List<StringW>* dst)
{
  *dst = Library_global->extensions;
  return ERR_OK;
}

static err_t FOG_CDECL Library_getLibraryPaths(List<StringW>* dst)
{
  AutoLock locked(Library_global->lock);

  *dst = Library_global->paths;
  return ERR_OK;
}

static err_t FOG_CDECL Library_addLibraryPath(const StringW* path, uint32_t mode)
{
  AutoLock locked(Library_global->lock);

  if (path->isEmpty())
    return ERR_RT_INVALID_ARGUMENT;

  if (Library_global->paths.indexOf(*path) != INVALID_INDEX)
    return ERR_RT_OBJECT_ALREADY_EXISTS;

  if (mode == LIBRARY_PATH_APPEND)
    return Library_global->paths.append(*path);
  else
    return Library_global->paths.prepend(*path);
}

static err_t FOG_CDECL Library_removeLibraryPath(const StringW* path)
{
  AutoLock locked(Library_global->lock);

  size_t index = Library_global->paths.indexOf(*path);
  if (index != INVALID_INDEX)
    return Library_global->paths.removeAt(index);
  else
    return ERR_OK;
}

static bool FOG_CDECL Library_hasLibraryPath(const StringW* path)
{
  AutoLock locked(Library_global->lock);

  return Library_global->paths.indexOf(*path) != INVALID_INDEX;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Library_init(void)
{
  // --------------------------------------------------------------------------
  // [Global]
  // --------------------------------------------------------------------------

  Library_global.init();

  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.library_ctor = Library_ctor;
  fog_api.library_ctorCopy = Library_ctorCopy;
  fog_api.library_dtor = Library_dtor;

  fog_api.library_setLibrary = Library_setLibrary;
  fog_api.library_openLibrary = Library_openLibrary;
  fog_api.library_openPlugin = Library_openPlugin;
  fog_api.library_close = Library_close;
  fog_api.library_getSymbolStubA = Library_getSymbolStubA;
  fog_api.library_getSymbolStringW = Library_getSymbolStringW;
  fog_api.library_getSymbols = Library_getSymbols;

  fog_api.library_dCreate = Library_dCreate;
  fog_api.library_dFree = Library_dFree;

  fog_api.library_getSystemPrefix = Library_getSystemPrefix;
  fog_api.library_getSystemExtensions = Library_getSystemExtensions;
  fog_api.library_getLibraryPaths = Library_getLibraryPaths;
  fog_api.library_addLibraryPath = Library_addLibraryPath;
  fog_api.library_removeLibraryPath = Library_removeLibraryPath;
  fog_api.library_hasLibraryPath = Library_hasLibraryPath;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  LibraryData* d = &Library_dNone();
  d->reference.init(1);
  d->handle = NULL;

  fog_api.library_dNone = d;

  // --------------------------------------------------------------------------
  // [Windows Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS)
  // There is no system prefix under Windows (empty).
  Library_global->extensions.append(Ascii8("dll"));
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Posix Support]
  // --------------------------------------------------------------------------

#if defined(FOG_OS_POSIX)

#if FOG_ARCH_BITS == 32
  Library_global->paths.append(Ascii8("/usr/lib"));
  Library_global->paths.append(Ascii8("/usr/local/lib"));
  Library_global->paths.append(Ascii8("/opt/local/lib"));
#else
  Library_global->paths.append(Ascii8("/usr/lib64"));
  Library_global->paths.append(Ascii8("/usr/local/lib64"));
  Library_global->paths.append(Ascii8("/opt/local/lib64"));
#endif // FOG_ARCH_BITS

  // HPUX specific .sl extension. The .so extension is not used.
#if defined(FOG_OS_HPUX)
  Library_global->extensions.append(Ascii8("sl"));
# define _FOG_LIBRARY_EXTENSION_DEFINED
#endif // FOG_OS_HPUX

  // MAC Specific .bundle and .dylib. The .so extension is used as well.
#if defined(FOG_OS_MAC)
  Library_global->extensions.append(Ascii8("bundle"));
  Library_global->extensions.append(Ascii8("dylib"));
#endif // FOG_OS_MAC

#if !defined(_FOG_LIBRARY_PREFIX_DEFINED)
  Library_global->prefix.set(Ascii8("lib"));
#endif // !FOG_LIBRARY_PREFIX_DEFINED

#if !defined(_FOG_LIBRARY_EXTENSION_DEFINED)
  Library_global->extensions.append(Ascii8("so"));
#endif // !FOG_LIBRARY_EXTENSION_DEFINED

#endif // FOG_OS_POSIX
}

FOG_NO_EXPORT void Library_fini(void)
{
  // --------------------------------------------------------------------------
  // [Global]
  // --------------------------------------------------------------------------

  Library_global.destroy();
}

} // Fog namespace
