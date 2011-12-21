// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_RANDOM_H
#define _FOG_CORE_TOOLS_RANDOM_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::RandomState]
// ============================================================================

struct FOG_NO_EXPORT RandomState
{
};

// ============================================================================
// [Fog::Random]
// ============================================================================

//! @brief Simple RNG.
//!
//! 
struct FOG_NO_EXPORT Random
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Random()
  {
  }

  FOG_INLINE Random(const Random& other) :
  {
  }

  FOG_INLINE ~Random()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const RandomState& getState() const
  {
    return _state;
  }

  FOG_INLINE void setState(const RandomState& state)
  {
    _state = state;
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Random& other)
  {
    return _state == other._state;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Random& operator=(const Random& other)
  {
    return *this;
  }

  FOG_INLINE bool operator==(const Random& other) const { return  eq(other); }
  FOG_INLINE bool operator==(const Random& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RandomState _state;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_RANDOM_H
