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
//! - @ref FOG_NO_EXPORT.
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



//! @defgroup Fog_Core_OS Operating system and application environment classes
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_IO Filesystem and streams
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_System Object-system, properties, events and timers
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Threading Threading, tasks, timers, event loop and atomic ops
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Tools Strings, byte arrays, formatting and other tooling classes
//! @ingroup Fog_Core_Public



//! @defgroup Fog_Core_Util Utility classes and functions
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
  // [Sharing]

  //! @brief Returns reference count of object data.
  sysuint_t getReference() const;

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
  err_t detach();

  //! @brief Private detach function.
  err_t _detach();

  //! @brief Reset the object to the construction state.
  //!
  //! All object data will be destroyed and all memory allocated by the object
  //! will be freed.
  //!
  //! If object data is allocated statically by template or other
  //! construction, it will be set to empty, but still ready to use.
  void reset();

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
  //! For example @c Fog::StringTmp<> templates sets this flag to
  //! true, but @c Fog::String family classes never set this flag.
  bool isStrong() const;
};

} // Doxygen namespace































#if defined(FOG_DOXYGEN)

// ============================================================================
// [Fog/G2d - Documentation]
// ============================================================================

//! @defgroup Fog_G2d_Public Fog-Graphics
//!
//! Public classes and functions related to Fog-Graphics library.



//! @defgroup Fog_G2d_Constants Constants
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Geometry Geometry
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Imaging Imaging
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Painting Painting
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Text Fonts and Text
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Tools Tools
//! @ingroup Fog_G2d_Public

// ============================================================================
// [Fog/G2d - Namespaces]
// ============================================================================

//! @namespace Fog::RasterEngine
//! @internal
//! @brief Low level raster based functions namespace.
//!
//! RasterEngine namespace contains lowest level pixel manipulation library that
//! is used in Fog library.
//!
//! @section Pixel format conversion and definitions.
//!
//! Fog library contains few pixel formats that can be usually combined together.
//! When using compositing function and destination not supports alpha channel,
//! the result is premultiplied and alpha is set to 0xFF. This is like blitting
//! the result into black destination using a source over operator.

#endif // FOG_DOXYGEN






















#if defined(FOG_DOXYGEN)

// ============================================================================
// [Fog-Gui Groups]
// ============================================================================

//! @defgroup Fog_Gui_Public Fog-Gui
//!
//! Public classes and functions related to Fog-Gui library.



//! @defgroup Fog_Gui_Constants Constants
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Engine Engine
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Layout Layout managers
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Theming Theming
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Widget Widgets
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Win Windows specific
//! @ingroup Fog_Gui_Public
//!
//! This module contains specific part of UI system for Windows.



//! @defgroup Fog_Gui_X11 X11 specific
//! @ingroup Fog_Gui_Public
//!
//! This module contains specific part of UI system for X Window System (xlib).



#endif // FOG_DOXYGEN



























// ============================================================================
// [Fog-Svg Documentation]
// ============================================================================

#if defined(FOG_DOXYGEN)
//! @defgroup Fog_Svg_Public Fog-Svg
//!
//! Public classes and functions related to Fog-Svg library.



//! @defgroup Fog_Svg_Constants Constants
//! @ingroup Fog_Svg_Public



//! @defgroup Fog_Svg_Dom Svg-DOM
//! @ingroup Fog_Svg_Public



//! @defgroup Fog_Svg_IO Svg-IO
//! @ingroup Fog_Svg_Public



//! @defgroup Fog_Svg_Util Svg utilities
//! @ingroup Fog_Svg_Public
#endif // FOG_DOXYGEN




















// ============================================================================
// [Fog-Xml Documentation]
// ============================================================================

#if defined(FOG_DOXYGEN)
//! @defgroup Fog_Xml_Public Fog-Xml
//!
//! Public classes and functions related to Fog-Xml library.



//! @defgroup Fog_Xml_Constants Constants
//! @ingroup Fog_Xml_Public



//! @defgroup Fog_Xml_Dom Xml-DOM
//! @ingroup Fog_Xml_Public



//! @defgroup Fog_Xml_IO Xml-IO
//! @ingroup Fog_Xml_Public



//! @defgroup Fog_Xml_Util Xml-Util
//! @ingroup Fog_Xml_Public
#endif // FOG_DOXYGEN
