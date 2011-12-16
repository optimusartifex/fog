// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

// [Guard]
#ifndef _FOG_CORE_H
#define _FOG_CORE_H

// ============================================================================
// [Fog-Framework]
// ============================================================================

//! @mainpage Fog-Framework Documentation
//!
//! @section Main_Page_Intro Introduction
//!
//! Fog library is a high performance library written in C++ language that was
//! designed to write cross-platform applications and libraries. It tries to
//! abstract an operating system API and enables to develop applications using
//! single source-code for multiple operating systems or backends. The primary
//! goal of the library is to work with vector and raster graphics, but now it
//! is suitable for development of any kind of application or library.
//!
//! Fog-Framework is quite big library which provides rich API, thus it was
//! divided into several sections called modules or parts. The coding style and
//! API of all modules is uniform and modules are generally connected together
//! into a single library, thus there are no extra setup steps needed to access
//! various modules (or parts) within the library.

// ============================================================================
// [Fog/Core]
// ============================================================================

//! @defgroup Fog_Core Fog/Core
//!
//! Public classes and functions related to Fog/Core module.
//!
//! The Fog/Core module is divided into these categories:
//!
//! - @ref Fog_Core_Cpp "Fog/Core/C++" - Detection of C++ compiler, version,
//!   features, and host architecture / operating system. Can contain also
//!   various workarounds used to be able to successfully build the 
//!   Fog-Framework product.
//!
//! - @ref Fog_Core_Global "Fog/Core/Global" - Essential classes, templates,
//!   and global definitions (constants) used by the rest of the Fog-Framework.
//!
//! - @ref Fog_Core_Kernel "Fog/Core/Kernel" - Implementation of Fog-Framework
//!   object system - @ref Object, @ref EventLoop, @c Timer, and helper classes.
//!   Generally don't needed to use or learn if you need to start only with 2d
//!   graphics.
//!
//! - @ref Fog_Core_Math "Fog/Core/Math" - Implementation of the @ref Math
//!   namespace, including classes and functions for numerical mathematics and
//!   analysis.
//!
//! - @ref Fog_Core_Memory "Fog/Core/Memory" - Memory management, including 
//!   customized memory allocators, pools, and optimized memory operations.
//!
//! - @ref Fog_Core_OS "Fog/Core/OS" - Operating system abstraction and
//!   utility functions. Includes also access to OS specific features.
//!
//! - @ref Fog_Core_Threading "Fog/Core/Threading" - Classes and functions used
//!   by multithreaded applications, including atomic operations, locks, 
//!   conditional variables, threads, thread-local-storage, and thread pools.
//!
//! - @ref Fog_Core_Tools "Fog/Core/Tools" - Uncategorized, contains classes
//!   and functions to work with strings (including access to unicode character
//!   database, text conversion, and localization), collections (lists, hash
//!   tables, and algorithms related to these types of collections), date&time,
//!   and variant data type called @ref Var.
//!
//! - @ref Fog_Core_Xml "Fog/Core/Xml" - Xml SAX parser/serializer and DOM,
//!   extensible by custom DOM elements, see @c XmlDocument.

// ============================================================================
// [Fog/Core/C++]
// ============================================================================

//! @defgroup Fog_Core_Cpp Code generation and compiler specific macros (C++).
//! @ingroup Fog_Core
//!
//! This section contains compiler macros to control class, variables, and
//! function visibility (dll-export/import), structure packing, code-generation,
//! and features which depends on specific compiler or operating system.
//!
//! @section Fog_Core_Cpp_OS Operating system
//!
//! Operating system checking at compile-time. If your application directly
//! depends on some specific features of target OS, you can easily detect
//! various operating systems using conditional compilation.
//!
//! List of main OS definitions:
//!
//! - @ref FOG_OS_MAC (defined if compiling for Mac or iOS).
//!   - @ref FOG_OS_IOS (defined if compiling for iOS - iPhone, iPad).
//! - @ref FOG_OS_POSIX (defined if compiling for posix platform - Linux/BSD/Mac).
//! - @ref FOG_OS_WINDOWS (defined if compiling for Windows).
//!
//! List of all OS definitions:
//!
//! - @ref FOG_OS_BSD (defines also @ref FOG_OS_POSIX).
//! - @ref FOG_OS_FREEBSD (defines also @ref FOG_OS_POSIX).
//! - @ref FOG_OS_HPUX (defines also @ref FOG_OS_POSIX).
//! - @ref FOG_OS_IOS (defines also @ref FOG_OS_MAC and @ref FOG_OS_POSIX).
//! - @ref FOG_OS_LINUX (defines also @ref FOG_OS_POSIX).
//! - @ref FOG_OS_MAC (defines also @ref FOG_OS_POSIX).
//! - @ref FOG_OS_NETBSD (defines also @ref FOG_OS_POSIX).
//! - @ref FOG_OS_OPENBSD (defines also @ref FOG_OS_POSIX).
//! - @ref FOG_OS_WINDOWS (never defines @ref FOG_OS_POSIX).
//!
//! @section Fog_Core_Cpp_Arch Architecture
//!
//! Target CPU architecture checking at compile-time.
//!
//! List of architecture definitions:
//!
//! - @ref FOG_ARCH_X86 (32-bit x86 target).
//! - @ref FOG_ARCH_X86_64 (64-bit x64 target).
//! - @ref FOG_ARCH_ARM (ARM target).
//! - @ref FOG_ARCH_ARM_NEON (ARM target with NEON instruction set).
//! - @ref FOG_ARCH_ITANIUM (64-bit Itanium target).
//! - @ref FOG_ARCH_PPC (32-bit PowerPC target).
//! - @ref FOG_ARCH_PPC (32-bit PowerPC target).
//!
//! List of architecture helpers (always defined):
//!
//! - @ref FOG_ARCH_BITS (target CPU architecture word width, 32 or 64).
//! - @ref FOG_ARCH_UNALIGNED_ACCESS_32 (32-bit unaligned access possible)
//! - @ref FOG_ARCH_UNALIGNED_ACCESS_32 (64-bit unaligned access possible)
//!
//! @note Only 32-bit and 64-bit architectures are supported.
//!
//! @section Fog_Core_Cpp_Hardcode Hardcoding
//!
//! Hardcoding macros can be used to hardcode some code using conditional
//! compilation. Currently hardcoding is very intensively used by the
//! Fog-Framework itself to generate better code on machines with SSE2
//! instruction set.
//!
//! List of x86/x64 hardcode definitions:
//!
//! - @ref FOG_HARDCODE_MMX (hardcode for MMX).
//! - @ref FOG_HARDCODE_MMX2 (hardcode for MMX Extensions).
//! - @ref FOG_HARDCODE_SSE (hardcode for SSE).
//! - @ref FOG_HARDCODE_SSE2 (hardcode for SSE2).
//! - @ref FOG_HARDCODE_SSE3 (hardcode for SSE3).
//! - @ref FOG_HARDCODE_SSSE3 (hardcode for SSSE3).
//!
//! List of ARM hardcode definitions:
//!
//! - @ref FOG_HARDCODE_MMX (hardcode for MMX).
//! - @ref FOG_HARDCODE_NEON (hardcode for NEON).
//!
//! @section Fog_Core_Cpp_Features C++ compiler vendor and features
//!
//! Definitions whilc can be used to check for a specific C++ compiler
//! or C++ language feature.
//!
//! List of compiler definitions:
//!
//! - @ref FOG_CC_BORLAND (Borland/CodeGear C/C++ compiler)
//! - @ref FOG_CC_CLANG (Clang C/C++ compiler)
//! - @ref FOG_CC_IDE (IDE, hack to enable IDE specific code-assistance tools)
//! - @ref FOG_CC_GNU (GNU C/C++ compiler)
//! - @ref FOG_CC_MSC (Microsoft C/C++ compiler)
//!
//! List of compiler features:
//!
//! - @ref FOG_CC_HAS_DECLTYPE (C++ compiler supports decltype).
//! - @ref FOG_CC_HAS_DEFAULT_FUNCTION (C++ compiler supports default-function).
//! - @ref FOG_CC_HAS_DELETE_FUNCTION (C++ compiler supports delete-function).
//! - @ref FOG_CC_HAS_INITIALIZER_LIST (C++ compiler supports initializer list).
//! - @ref FOG_CC_HAS_LAMBDA (C++ compiler supports lambda expressions).
//! - @ref FOG_CC_HAS_NULLPTR (C++ compiler supports nullptr).
//! - @ref FOG_CC_HAS_OVERRIDE (C++ compiler supports override).
//! - @ref FOG_CC_HAS_PARTIAL_TEMPLATE_SPECIALIZATION (C++ compiler supports PTS).
//! - @ref FOG_CC_HAS_RVALUE (C++ compiler supports rvalues).
//! - @ref FOG_CC_HAS_STATIC_ASSERT (C++ compiler supports static_assert).
//!
//! @section Fog_Core_Cpp_Decorators Decorators
//!
//! Decorators are used when compiling Fog-Framework and when including
//! Fog-Framework headers from other sources. Decorators can be used to
//! control the visibility, function calling conventions, and other compiler
//! or operating system specific features.
//!
//! List of standard decorators:
//!
//! - @ref FOG_ALIGNED_TYPE (declare aligned data-type).
//! - @ref FOG_ALIGNED_VAR (declare aligned variable).
//! - @ref FOG_API (export Fog-Framework function, variable, or class).
//! - @ref FOG_CDECL (use CDECL C calling convention).
//! - @ref FOG_DEPRECATED (deprecated function, variable, or class).
//! - @ref FOG_DLL_IMPORT (Windows specific DLL-Import).
//! - @ref FOG_DLL_EXPORT (Windows specific DLL-Export).
//! - @ref FOG_FASTCALL (use FASTCALL C calling convention).
//! - @ref FOG_INLINE (make function inline, if possible).
//! - @ref FOG_LIKELY (the condition is likely to be taken).
//! - @ref FOG_MACRO_BEGIN (begin of block of code-expansion macro).
//! - @ref FOG_MACRO_END (end of block of code-expansion macro).
//! - @ref FOG_NO_EXPORT (use CDECL C calling convention).
//! - @ref FOG_NO_INLINE (never inline the function).
//! - @ref FOG_NO_RETURN (function never returns).
//! - @ref FOG_NOTHROW (function/method never throws an exception).
//! - @ref FOG_RESTRICT (defines compiler specific restrict keyword, or nothing).
//! - @ref FOG_STDCALL (use STDCALL C calling convention).
//! - @ref FOG_UNLIKELY (the condition is unlikely to be taken).
//! - @ref FOG_UNUSED (mark unused variable to prevent compiler warning).
//!
//! @section Fog_Core_Cpp_PCE Predicting conditional expressions
//!
//! Macros that can help to optimize the code using compiler specific
//! decorators to tell compiler expected result of the expression. Currently
//! these macros are only used to check whether the function returned an
//! error or NULL pointer.
//!
//! List of PCE macros:
//!
//! - @ref FOG_LIKELY.
//! - @ref FOG_UNLIKELY.
//!
//! @note Currently only when compiling by GCC these macros are used, when
//! compiling for example by MSVC there is no such functionality (when using
//! this compiler try to use profile based optimization instead).

#include <Fog/Core/C++/Base.h>

// ============================================================================
// [Fog/Core/Collection]
// ============================================================================

// TODO: To be removed...

#include <Fog/Core/Collection/StackP.h>
#include <Fog/Core/Collection/Util.h>

// ============================================================================
// [Fog/Core/Global]
// ============================================================================

//! @defgroup Fog_Core_Global Global definitions, constants, and macros
//! @ingroup Fog_Core

#include <Fog/Core/Global/Global.h>

// ============================================================================
// [Fog/Core/Kernel]
// ============================================================================

//! @defgroup Fog_Core_Kernel Object-system, properties, events and timers
//! @ingroup Fog_Core

#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Kernel/Delegate.h>
#include <Fog/Core/Kernel/Event.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>

// ============================================================================
// [Fog/Core/Math]
// ============================================================================

//! @defgroup Fog_Core_Math Math functions
//! @ingroup Fog_Core

#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/FloatBits.h>
#include <Fog/Core/Math/FloatControl.h>
#include <Fog/Core/Math/Function.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Integrate.h>
#include <Fog/Core/Math/Interval.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/MathVec.h>
#include <Fog/Core/Math/Solve.h>

// ============================================================================
// [Fog/Core/Memory]
// ============================================================================

//! @defgroup Fog_Core_Memory Memory management and operations
//! @ingroup Fog_Core

#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/MemBlockAllocator.h>
#include <Fog/Core/Memory/MemBuffer.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Memory/MemPool.h>
#include <Fog/Core/Memory/MemZoneAllocator.h>

// ============================================================================
// [Fog/Core/OS]
// ============================================================================

//! @defgroup Fog_Core_OS Operating system and application environment classes
//! @ingroup Fog_Core

#include <Fog/Core/OS/DirIterator.h>
#include <Fog/Core/OS/Environment.h>
#include <Fog/Core/OS/FileInfo.h>
#include <Fog/Core/OS/FileMapping.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/OS/FileUtil.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/OS/OSInfo.h>
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/Core/OS/UserUtil.h>

// ============================================================================
// [Fog/Core/Threading]
// ============================================================================

//! @defgroup Fog_Core_Threading Threading, tasks, timers, event loop and atomic ops
//! @ingroup Fog_Core

#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/AtomicPadding.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Threading/ThreadLocal.h>
#include <Fog/Core/Threading/ThreadPool.h>

// ============================================================================
// [Fog/Core/Tools]
// ============================================================================

//! @defgroup Fog_Core_Tools Strings, collections, streaming and other helper classes
//! @ingroup Fog_Core

#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharData.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Date.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/HashString.h>
#include <Fog/Core/Tools/HashUntyped.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ListReal.h>
#include <Fog/Core/Tools/ListString.h>
#include <Fog/Core/Tools/ListUntyped.h>
#include <Fog/Core/Tools/ListVar.h>
#include <Fog/Core/Tools/Locale.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/RegExp.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/Stub.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Core/Tools/TextIterator.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/Core/Tools/VarId.h>

// ============================================================================
// [Fog/Core/Xml]
// ============================================================================

//! @defgroup Fog_Core_Xml XML SAX/IO and DOM
//! @ingroup Fog_Core

#include <Fog/Core/Xml/XmlAttribute.h>
#include <Fog/Core/Xml/XmlCDATA.h>
#include <Fog/Core/Xml/XmlComment.h>
#include <Fog/Core/Xml/XmlDocument.h>
#include <Fog/Core/Xml/XmlDomReader.h>
#include <Fog/Core/Xml/XmlDomWriter.h>
#include <Fog/Core/Xml/XmlElement.h>
#include <Fog/Core/Xml/XmlIdManager.h>
#include <Fog/Core/Xml/XmlNoText.h>
#include <Fog/Core/Xml/XmlSaxReader.h>
#include <Fog/Core/Xml/XmlSaxWriter.h>
#include <Fog/Core/Xml/XmlText.h>

// [Guard]
#endif // _FOG_CORE_H
