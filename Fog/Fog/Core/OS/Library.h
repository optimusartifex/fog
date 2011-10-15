// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_LIBRARY_H
#define _FOG_CORE_OS_LIBRARY_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_OS
//! @{

// ============================================================================
// [Fog::LibraryData]
// ============================================================================

struct FOG_NO_EXPORT LibraryData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE LibraryData* addRef() const
  {
    reference.inc();
    return const_cast<LibraryData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      fog_api.library_dFree(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Library handle.
  void* handle;
};

// ============================================================================
// [Fog::Library]
// ============================================================================

//! @brief Dynamic library.
struct FOG_NO_EXPORT Library
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Library()
  {
    fog_api.library_ctor(this);
  }

  FOG_INLINE Library(const Library& other)
  {
    fog_api.library_ctorCopy(this, &other);
  }

  FOG_INLINE ~Library()
  {
    fog_api.library_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isOpen() const
  {
    return _d->handle != NULL;
  }

  FOG_INLINE void* getHandle() const
  {
    return _d->handle;
  }

  // --------------------------------------------------------------------------
  // [Open / Close]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t openLibrary(const StringW& fileName, uint32_t flags = LIBRARY_OPEN_DEFAULT)
  {
    return fog_api.library_openLibrary(this, &fileName, flags);
  }

  FOG_INLINE err_t openPlugin(const StringW& category, const StringW& name)
  {
    return fog_api.library_openPlugin(this, &category, &name);
  }

  FOG_INLINE void close()
  {
    fog_api.library_close(this);
  }

  // --------------------------------------------------------------------------
  // [GetSymbol]
  // --------------------------------------------------------------------------

  // Two functions for symbol name, because symbols are encoded in
  // 8 bit ASCII null terminated strings and we can avoid to use
  // text codecs from Fog::StringW <=> ASCII string. Only reason is
  // performance, nothing more.

  //! @brief Returns a symbol @a symbolName from opened library.
  //!
  //! Return value can be @c NULL if symbol is not found or library is
  //! not open.
  FOG_INLINE void* getSymbol(const char* sym)
  {
    StubA symA(sym, DETECT_LENGTH);
    return fog_api.library_getSymbolStubA(this, &symA);
  }

  FOG_INLINE void* getSymbol(const Ascii8& sym)
  {
    return fog_api.library_getSymbolStubA(this, &sym);
  }

  //! @overload
  FOG_INLINE void* getSymbol(const StringW& sym)
  {
    return fog_api.library_getSymbolStringW(this, &sym);
  }

  //! @brief Loads more symbols at the time.
  //!
  //! @param target Target array where to store pointers to functions.
  //! @param symbols Null terminated string that contains all symbols
  //! separated by @c NULL terminator, so valid string declaration for
  //! two symbols is <code>"FirstSymbol\000SecondSymbol"</code>.
  //! @param symbolsLength Length of @c symbols string. use sizeof() if
  //! your string is static ( or @c FOG_ARRAY_SIZE() )
  //! @param symbolsCount Count of symbols to load.
  //! @param fail here is stored first symbol that's not loaded.
  //!
  //! @return Count of loaded symbols, if return value is smaller than
  //! @a count parameter, @a fail parameter will be position to begin of
  //! symbol name that wasn't loaded.
  FOG_INLINE size_t getSymbols(void** target, const char* symbols, size_t symbolsLength, size_t symbolsCount, char** fail = NULL)
  {
    return fog_api.library_getSymbols(this, target, symbols, symbolsLength, symbolsCount, fail);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Library& operator=(const Library& other)
  {
    fog_api.library_setLibrary(this, &other);
    return *this;
  }

  FOG_INLINE bool operator==(const Library& other) { return _d == other._d; }
  FOG_INLINE bool operator!=(const Library& other) { return _d != other._d; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static FOG_INLINE StringW getSystemPrefix()
  {
    StringW result;
    fog_api.library_getSystemPrefix(&result);
    return result;
  }

  static FOG_INLINE List<StringW> getSystemExtensions()
  {
    List<StringW> result;
    fog_api.library_getSystemExtensions(&result);
    return result;
  }

  static FOG_INLINE List<StringW> getLibraryPaths()
  {
    List<StringW> result;
    fog_api.library_getLibraryPaths(&result);
    return result;
  }

  static FOG_INLINE err_t addLibraryPath(const StringW& path, uint32_t mode = LIBRARY_PATH_APPEND)
  {
    return fog_api.library_addLibraryPath(&path, mode);
  }

  static FOG_INLINE err_t removeLibraryPath(const StringW& path)
  {
    return fog_api.library_removeLibraryPath(&path);
  }

  static FOG_INLINE bool hasLibraryPath(const StringW& path)
  {
    return fog_api.library_hasLibraryPath(&path);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(LibraryData)
};

// ============================================================================
// [FOG_LIBRARY_DECLARE Macros - For Implementing Plugins]
// ============================================================================

//! @def FOG_LIBRARY_DECLARE(_Library_)
//! @brief Creates basic defines for dynamic loadable module.
//!
//! This macro should be placed only once per loadable object. Use it with
//! @c FOG_LIBRARY_ENTRY and @c FOG_LIBRARY_LEAVE macros together.

#if defined(FOG_OS_WINDOWS)

#define FOG_LIBRARY_DECLARE(_Library_) \
\
static void _Library_##Entry(void); \
static void _Library_##Leave(void); \
\
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) \
{ \
  switch (fdwReason) \
  { \
    case DLL_PROCESS_ATTACH: \
      _Library_##Entry(); \
      break; \
    case DLL_PROCESS_DETACH: \
      _Library_##Leave(); \
      break; \
  } \
  \
  return TRUE; \
}

#define FOG_LIBRARY_ENTRY(_Library_) static void _Library_##Entry(void)
#define FOG_LIBRARY_LEAVE(_Library_) static void _Library_##Leave(void)

#else

#define FOG_LIBRARY_DECLARE(_Library_) \
\
static void _Library_##Entry(void); \
static void _Library_##Leave(void); \
\
struct FOG_NO_EXPORT _Library_##Interface \
{ \
  _Library_##Interface::_Library_##Interface() { _Library_##Entry(); } \
  _Library_##Interface::~_Library_##Interface() { _Library_##Leave(); } \
}; \
static _Library_##Interface _Library_##Instance;

#define FOG_LIBRARY_ENTRY(_Library_) static void _Library_##Entry(void)
#define FOG_LIBRARY_LEAVE(_Library_) static void _Library_##Leave(void)

#endif

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_OS_LIBRARY_H
