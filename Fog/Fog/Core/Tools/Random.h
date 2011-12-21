// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_RANDOM_H
#define _FOG_CORE_TOOLS_RANDOM_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::RandomState]
// ============================================================================

struct FOG_NO_EXPORT RandomState
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint64_t getSeed() const { return _seed; }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const RandomState& other) const
  {
    return _seed == other._seed &&
           _w    == other._w    &&
           _z    == other._z    ;
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE bool operator==(const RandomState& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const RandomState& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint64_t _seed;
  uint32_t _w;
  uint32_t _z;
};

// ============================================================================
// [Fog::Random]
// ============================================================================

//! @brief Simple random number generator.
struct FOG_NO_EXPORT Random
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE Random()
  {
    fog_api.random_ctor(this);
  }

  FOG_INLINE Random(const Random& other)
  {
    MemOps::copy_t<Random>(this, &other);
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint64_t getSeed() const { return _state._seed; }

  FOG_INLINE const RandomState& getState() const { return _state; }
  FOG_INLINE void setState(const RandomState& state) { _state = state; }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    fog_api.random_reset(this);
  }

  // --------------------------------------------------------------------------
  // [Seed / Restart]
  // --------------------------------------------------------------------------

  FOG_INLINE void seed(uint64_t n)
  {
    fog_api.random_reset(this);
  }

  FOG_INLINE void rewind()
  {
    fog_api.random_rewind(this);
  }

  // --------------------------------------------------------------------------
  // [Generator]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t nextUInt32()
  {
    return fog_api.random_nextUInt32(this);
  }
  
  FOG_INLINE double nextDouble()
  {
    return fog_api.random_nextDouble(this);
  }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  FOG_INLINE bool eq(const Random& other) const
  {
    return _state.eq(other._state);
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE Random& operator=(const Random& other)
  {
    return *this;
  }

  FOG_INLINE bool operator==(const Random& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const Random& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  RandomState _state;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_RANDOM_H
