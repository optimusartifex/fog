// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_H
#define _FOG_CORE_H

#if defined(_MSVC)
#pragma once
#endif // _MSVC

#if defined(FOG_DOXYGEN)

// ============================================================================
// [Fog - Core - Main]
// ============================================================================

//! @mainpage Fog Library Documentation
//!
//! @section Main_Page_Intro Introduction
//!
//! Fog library is high performance library written in C++  language that 
//! abstracts operating system dependencies and enables multi platform 
//! development through single source compatibility. This library was created 
//! as an abstraction layer between Windows and Unix like operating systems.

// ============================================================================
// [Fog - Core - Documentation]
// ============================================================================

//! @defgroup Fog_Core_Public Fog-Core
//!
//! Public classes and functions related to Fog-Core library.

//! @internal
//! @defgroup Fog_Core_Private Fog-Core (private)
//!
//! Private classes and functions related to Fog-Core library.



//! @defgroup Fog_Core_Application Application
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Compiler Compiler macros
//! @ingroup Fog_Core_Public
//!
//! Section that contains compiler macros to control class / function
//! visibility (export / import), structure packing and compiler intrinsics.
//!
//! @section Fog_Core_Compiler_OS Operating system
//!
//! Operating system checking at compile-time. If your application directly
//! depends to specific features of operating system, you can easily check
//! for it and include these features at compile-time.
//!
//! List of main macros:
//!
//! - @ref FOG_OS_MAC.
//! - @ref FOG_OS_POSIX.
//! - @ref FOG_OS_WINDOWS.
//!
//! List of all operating system macros:
//!
//! - @ref FOG_OS_BSD.
//! - @ref FOG_OS_FREEBSD.
//! - @ref FOG_OS_HPUX.
//! - @ref FOG_OS_LINUX.
//! - @ref FOG_OS_MAC.
//! - @ref FOG_OS_NETBSD.
//! - @ref FOG_OS_OPENBSD.
//! - @ref FOG_OS_WINDOWS.
//!
//! @section Fog_Core_Compiler_Arch Architecture
//!
//! Fog-Framework contains macros that helps to get architecture for the
//! library is being compiled.
//!
//! List of architecture macros:
//!
//! - @ref FOG_ARCH_X86.
//! - @ref FOG_ARCH_X86_64.
//! - @ref FOG_ARCH_PPC.
//! - @ref FOG_ARCH_BITS.
//!
//! The last (@ref FOG_ARCH_BITS) macro is generic and it tells you the target
//! architecture word size.
//!
//! @note Only 32-bit and 64-bit architectures are supported.
//!
//! @section Fog_Core_Compiler_Hardcoding Hardcoding
//!
//! Hardcoding macros can be used to hardcode some code using processor
//! extensions. In Fog-Framework hardcoding is mainly used to compile some
//! performance critical code using SSE2.
//!
//! List of hardcoding macros:
//! 
//! - @ref FOG_HARDCODE_MMX.
//! - @ref FOG_HARDCODE_MMX2.
//! - @ref FOG_HARDCODE_SSE.
//! - @ref FOG_HARDCODE_SSE2.
//! - @ref FOG_HARDCODE_SSE3.
//!
//! @section Fog_Core_Compiler_Decorators Decorators
//!
//! Decorators are used when compiling Fog library and when including Fog
//! header files from other sources. Decorators can be used to control
//! API visibility, function calling conventions, etc...
//!
//! List of standard decorators:
//!
//! - @ref FOG_ALIGNED_TYPE.
//! - @ref FOG_ALIGNED_VAR.
//! - @ref FOG_API.
//! - @ref FOG_CDECL.
//! - @ref FOG_DEPRECATED.
//! - @ref FOG_DLL_IMPORT.
//! - @ref FOG_DLL_EXPORT.
//! - @ref FOG_FASTCALL.
//! - @ref FOG_HIDDEN.
//! - @ref FOG_INLINE.
//! - @ref FOG_NO_RETURN.
//! - @ref FOG_STDCALL.
//! - @ref FOG_UNUSED.
//!
//! @section Fog_Core_Compiler_PCE Predicting conditional expressions
//!
//! Macros that can help to optimize the code using compiler specific 
//! decorators to tell compiler expected result of the expression.
//!
//! List of macros:
//!
//! - @ref FOG_LIKELY.
//! - @ref FOG_UNLIKELY.
//!
//! @note Currently only when compiling by gcc these macros are used, when 
//! compiling for example by MSVC there is no such functionality (when using
//! this compiler try to use profile based optimizations instead).



//! @defgroup Fog_Core_Constants Constants
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_DateTime Date and time
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Debugging Debugging
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Environment Environment and operating system related classes
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Language Language (C++) compile-time information and tools
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Library Libraries and plugins
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Localization Localization and internationalization
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Math Math functions
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Memory Memory management
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Object Object-system, properties and events
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Streaming Filesystem and streams
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_String Strings, byte arrays and formatting
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Threading Threading, tasks, timers and event loop
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Util Utility classes and functions
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Other Non-categorized classes and functions
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Macros Macros
//! @ingroup Fog_Core_Public
//!
//! Macros that helps with compiling Fog-Framework and all applications.

// ============================================================================
// [Fog-Core Shared]
// ============================================================================

//! @brief Namespace for shared documentation between classes.
namespace Doxygen {

//! @brief Not real structure, used only for doxygen.
struct Implicit
{
  // [Implicit Sharing]

  //! @brief Returns reference count of object data.
  sysuint_t refCount() const;

  //! @brief Returns @c true if object is not sharing data with another.
  //!
  //! This method will return @c true if reference count is equal to 1.
  bool isDetached() const;

  //! @brief Detaches object data if they are shared between another object.
  //!
  //! If data of this object are read only or reference count is larger
  //! than 1, the data will be detached.
  //!
  //! Detaching means creating exact copy of data.
  void detach();

  //! @brief Private detach function.
  void _detach();

  //! @brief Frees all memory allocated by this object.
  //!
  //! If object data is allocated by dynamic memory allocation,
  //! they will be freed and object will share null data.
  //!
  //! If object data is allocated statically by template or other
  //! construction, it will be set to empty, but still ready to use.
  void free();

  // [Flags]

  //! @brief Returns object flags.
  //!
  //! Object flags contains boolean information about object data. There
  //! are always information about object allocation, shareability and
  //! null.
  uint32_t getFlags() const;

  //! @brief Returns @c true if object data are allocated by dynamic memory
  //! allocation.
  //!
  //! Dynamic memory allocation is used for every dynamic container object,
  //! but there are usually choices to use static (stack) version for temporary
  //! object data.
  bool isDynamic() const;

  //! @brief Returns @c true if object data are shareable.
  //!
  //! Sharable data are usually allocated on the heap and in rare cases
  //! on the stack. Implicit sharing system allows to share equal data
  //! between more objects, so use @c isSharable() in cases that you
  //! want to know if the object data can be shared (And will be shared
  //! if assign operation will be called).
  bool isSharable() const;

  //! @brief Returns @c true if this is null object (no data).
  //!
  //! In real life there is no difference between null and empty
  //! objects. Null is just convenience for library, because null
  //! objects are allocated by library and shared between all
  //! null instances (empty instances).
  bool isNull() const;

  //! @brief Returns @c true if object data are alive.
  //!
  //! Alive data means, that assign operations will ever free these data
  //! and deep copy will be used instead.
  //!
  //! This flag is set only by template versions of original objects.
  //! For example @c Fog::TemporaryString<> templates sets this flag to 
  //! true, but @c Fog::String family classes never set this flag.
  bool isStrong() const;
};

} // Doxygen namespace

#endif // FOG_DOXYGEN

// ============================================================================
// [Fog-Core Include Files]
// ============================================================================

#include <Fog/Core/Build.h>

#include <Fog/Core/Algorithms.h>
#include <Fog/Core/Application.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Byte.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/ByteArrayFilter.h>
#include <Fog/Core/ByteArrayMatcher.h>
#include <Fog/Core/Char.h>
#include <Fog/Core/CharUtil.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/CpuInfo.h>
#include <Fog/Core/Delegate.h>
#include <Fog/Core/DirIterator.h>
#include <Fog/Core/Event.h>
#include <Fog/Core/EventLoop.h>
#include <Fog/Core/FileSystem.h>
#include <Fog/Core/Format.h>
#include <Fog/Core/Hash.h>
#include <Fog/Core/HashUtil.h>
#include <Fog/Core/Lazy.h>
#include <Fog/Core/Library.h>
#include <Fog/Core/List.h>
#include <Fog/Core/Locale.h>
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/MapFile.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/MemoryBuffer.h>
#include <Fog/Core/MemoryManager.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/OS.h>
#include <Fog/Core/Object.h>
#include <Fog/Core/Range.h>
#include <Fog/Core/SequenceInfo.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringFilter.h>
#include <Fog/Core/StringMatcher.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/Strings.h>
#include <Fog/Core/Stub.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Core/Thread.h>
#include <Fog/Core/ThreadCondition.h>
#include <Fog/Core/ThreadEvent.h>
#include <Fog/Core/ThreadLocalStorage.h>
#include <Fog/Core/ThreadPool.h>
#include <Fog/Core/Time.h>
#include <Fog/Core/Timer.h>
#include <Fog/Core/TypeInfo.h>
#include <Fog/Core/UserInfo.h>
#include <Fog/Core/Value.h>

// [Guard]
#endif // _FOG_CORE_H
