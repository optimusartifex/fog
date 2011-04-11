// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_LIBRARY_LIBRARY_H
#define _FOG_CORE_LIBRARY_LIBRARY_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Library
//! @{

// ============================================================================
// [Fog::LibraryData]
// ============================================================================

struct FOG_NO_EXPORT LibraryData
{
  // [Members]

  mutable Atomic<sysuint_t> refCount;
  void* handle;
};

// ============================================================================
// [Fog::Library]
// ============================================================================

//! @brief Dynamic library.
struct FOG_API Library
{
  static Static<LibraryData> _dnull;

  // [Construction / Destruction]

  Library();
  Library(const Library& other);
  Library(const String& fileName, uint32_t openFlags = LIBRARY_OPEN_DEFAULT);
  ~Library();

  // [Implicit Sharing]

  //! @copydoc Doxygen::Implicit::getRefCount().
  FOG_INLINE sysuint_t getRefCount() const { return _d->refCount.get(); }

  // [Flags]

  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d->handle == NULL; }

  FOG_INLINE void* getHandle() const { return _d->handle; }
  FOG_INLINE bool isOpen() const { return _d->handle != NULL; }

  err_t open(const String& fileName, uint32_t openFlags = LIBRARY_OPEN_DEFAULT);
  err_t openPlugin(const String& category, const String& fileName);
  void close();

  // Two functions for symbol name, because symbols are encoded in
  // 8 bit ASCII null terminated strings and we can avoid to use
  // text codecs from Fog::String <=> ASCII string. Only reason is
  // performance, nothing more.

  //! @brief Returns a symbol @a symbolName from opened library. 
  //!
  //! Return value can be @c NULL if symbol is not found or library is
  //! not open.
  void* getSymbol(const char* symbolName);
  //! @overload
  void* getSymbol(const String& symbolName);

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
  sysuint_t getSymbols(void** target, const char* symbols, sysuint_t symbolsLength, sysuint_t symbolsCount, char** fail);

  // [Operator Overload]

  Library& operator=(const Library& other);

  // [Paths]

  enum PATH_MODE
  {
    PATH_PREPEND = 0,
    PATH_APPEND = 1
  };

  static List<String> paths();
  static bool addPath(const String& path, int pathMode = PATH_APPEND);
  static bool removePath(const String& path);
  static bool hasPath(const String& path);

  // [System]

  static String getSystemPrefix();
  static List<String> getSystemExtensions();

  // [Members]

  FOG_DECLARE_D(LibraryData)
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

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::Library, Fog::TYPEINFO_MOVABLE)

// [Guard]
#endif // _FOG_CORE_LIBRARY_LIBRARY_H
