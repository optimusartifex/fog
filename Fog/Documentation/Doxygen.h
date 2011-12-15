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
  size_t getReference() const;

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
};

} // Doxygen namespace

#if defined(FOG_DOXYGEN)

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
// [Fog-UI Groups]
// ============================================================================

//! @defgroup Fog_UI Fog/UI
//!
//! Public classes and functions related to Fog-Gui library.

//! @defgroup Fog_UI_Engine Engine
//! @ingroup Fog_UI

//! @defgroup Fog_UI_Layout Layout managers
//! @ingroup Fog_UI

//! @defgroup Fog_UI_Widget Widgets
//! @ingroup Fog_UI

#endif // FOG_DOXYGEN
