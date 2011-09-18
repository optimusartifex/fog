// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_CPU_H
#define _FOG_CORE_TOOLS_CPU_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::CPU_CACHE_LINE]
// ============================================================================

enum { CPU_CACHE_LINE = 64 };

// ============================================================================
// [Fog::Cpu]
// ============================================================================

//! @brief Information about CPU features.
//!
//! Easy way to read CPU features. CPU is detected in startup time, so this class
//! has method only to read the detected state. Use @c Cpu::features to
//! retrieve information about your CPU and @c use Cpu::Features enumeration
//! to get interesting features for you.
//!
//! Small example how to check for SSE2 support.
//!
//! @verbatim
//! if (Fog::Cpu::get()->hasFeature(Fog::CPU_FEATURE_SSE2))
//! {
//!   // Cpu has SSE2 support.
//! }
//! else
//! {
//!   // Cpu hasn't SSE2 support.
//! }
//! @endverbatim
struct FOG_NO_EXPORT Cpu
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const char* getVendor() const { return _vendor; }
  FOG_INLINE const char* getBrand() const { return _brand; }

  FOG_INLINE uint32_t getVendorId() const { return _vendorId; }
  FOG_INLINE uint32_t getFamily() const { return _family; }
  FOG_INLINE uint32_t getModel() const { return _model; }
  FOG_INLINE uint32_t getStepping() const { return _stepping; }
  FOG_INLINE uint32_t getNumberOfProcessors() const { return _numberOfProcessors; }

  FOG_INLINE uint32_t getFeatures() const { return _features; }
  FOG_INLINE bool hasFeature(uint32_t feature) const { return (_features & feature) != 0; }
  FOG_INLINE bool hasFeatures(uint32_t features) const { return (_features & features) == features; }

  FOG_INLINE uint32_t getBugs() const { return _bugs; }
  FOG_INLINE bool hasBug(uint32_t bug) const { return (_bugs & bug) != 0; }

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Get the global @c Cpu instance.
  static FOG_INLINE const Cpu* get()
  {
    return _api.cpu.instance;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Cpu short vendor string.
  char _vendor[16];
  //! @brief Cpu long vendor string (brand).
  char _brand[64];

  //! @brief Cpu vendor id (see @c CPU_VENDOR).
  uint32_t _vendorId;
  //! @brief Cpu family ID.
  uint32_t _family;
  //! @brief Cpu model ID.
  uint32_t _model;
  //! @brief Cpu stepping.
  uint32_t _stepping;
  //! @brief Number of processors or cores.
  uint32_t _numberOfProcessors;
  //! @brief Cpu features bitfield, see @c CPU_FEATURE).
  uint32_t _features;
  //! @brief Cpu bugs bitfield, see @c CPU_BUG).
  uint32_t _bugs;
};

// ============================================================================
// [FOG_CPU - MMX]
// ============================================================================

#if defined(FOG_OPTIMIZE_MMX)
#define FOG_CPU_DECLARE_INITIALIZER_MMX(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_MMX)
#define FOG_CPU_USE_INITIALIZER_MMX(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_MMX(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_MMX)) _Initializer_();
#endif // FOG_HARDCODE_MMX

#else
#define FOG_CPU_DECLARE_INITIALIZER_MMX(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_MMX(_Initializer_)
#endif // FOG_OPTIMIZE_MMX

// ============================================================================
// [FOG_CPU - 3DNOW!]
// ============================================================================

#if defined(FOG_OPTIMIZE_3DNOW)
#define FOG_CPU_DECLARE_INITIALIZER_3DNOW(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_3DNOW)
#define FOG_CPU_USE_INITIALIZER_3DNOW(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_3DNOW(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_3DNOW)) _Initializer_();
#endif // FOG_HARDCODE_3DNOW

#else
#define FOG_CPU_DECLARE_INITIALIZER_3DNOW(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_3DNOW(_Initializer_)
#endif // FOG_OPTIMIZE_3DNOW

// ============================================================================
// [FOG_CPU - SSE]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSE)
#define FOG_CPU_DECLARE_INITIALIZER_SSE(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSE)
#define FOG_CPU_USE_INITIALIZER_SSE(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSE(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSE)) _Initializer_();
#endif // FOG_HARDCODE_SSE

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSE(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSE(_Initializer_)
#endif // FOG_OPTIMIZE_SSE

// ============================================================================
// [FOG_CPU - SSE2]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSE2)
#define FOG_CPU_DECLARE_INITIALIZER_SSE2(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSE2)
#define FOG_CPU_USE_INITIALIZER_SSE2(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSE2(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSE2)) _Initializer_();
#endif // FOG_HARDCODE_SSE2

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSE2(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSE2(_Initializer_)
#endif // FOG_OPTIMIZE_SSE2

// ============================================================================
// [FOG_CPU - SSE3]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSE3)
#define FOG_CPU_DECLARE_INITIALIZER_SSE3(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSE3)
#define FOG_CPU_USE_INITIALIZER_SSE3(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSE3(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSE3)) _Initializer_();
#endif // FOG_HARDCODE_SSE3

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSE3(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSE3(_Initializer_)
#endif // FOG_OPTIMIZE_SSE3

// ============================================================================
// [FOG_CPU - SSSE3]
// ============================================================================

#if defined(FOG_OPTIMIZE_SSSE3)
#define FOG_CPU_DECLARE_INITIALIZER_SSSE3(_Initializer_) \
  FOG_NO_EXPORT void _Initializer_(void);

#if defined(FOG_HARDCODE_SSSE3)
#define FOG_CPU_USE_INITIALIZER_SSSE3(_Initializer_) \
  _Initializer_();
#else
#define FOG_CPU_USE_INITIALIZER_SSSE3(_Initializer_) \
  if (::Fog::Cpu::get()->hasFeature(::Fog::CPU_FEATURE_SSSE3)) _Initializer_();
#endif // FOG_HARDCODE_SSSE3

#else
#define FOG_CPU_DECLARE_INITIALIZER_SSSE3(_Initializer_)
#define FOG_CPU_USE_INITIALIZER_SSSE3(_Initializer_)
#endif // FOG_OPTIMIZE_SSSE3

//! @}

} // Fog namespace


// [Guard]
#endif // _FOG_CORE_TOOLS_CPU_H
