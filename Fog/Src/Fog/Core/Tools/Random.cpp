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
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Tools/Random.h>

namespace Fog {

// ============================================================================
// [Fog::Random - Constants]
// ============================================================================

// This magic constant comes from the original random number generator. It's
// just random.
#define FOG_RANDOM_MAGIC FOG_UINT64_C(0x1F123BB5159A55E5)

#define FOG_RANDOM_SEED_W 0x1E9A109F
#define FOG_RANDOM_SEED_Z 0x9A13F031

// ============================================================================
// [Fog::Random - Reset]
// ============================================================================

static void FOG_CDECL Random_reset(Random* self)
{
  self->seed(FOG_RANDOM_MAGIC);
}

// ============================================================================
// [Fog::Random - Seed / Restart]
// ============================================================================

static void FOG_CDECL Random_seed(Random* self, uint64_t n)
{
  RandomState& s = self->_state;

  s._seed = n;

  s._w = uint32_t(n      ) & 0xFFFFFFFF;
  s._z = uint32_t(n >> 32) & 0xFFFFFFFF;

  // It's important to have no zero seed. Because in such case the random
  // generator won't work.
  if (s._w == 0) s._w = FOG_RANDOM_SEED_W;
  if (s._z == 0) s._z = FOG_RANDOM_SEED_Z;
}

static void FOG_CDECL Random_rewind(Random* self)
{
  self->seed(self->_state._seed);
}

// ============================================================================
// [Fog::Random - Next]
// ============================================================================

#define FOG_RANDOM_NEXT(_Where_) \
  FOG_MACRO_BEGIN \
    RandomState& s = self->_state; \
    \
    s._z = 36969 * (s._z & 65535) + (s._z >> 16); \
    s._w = 18000 * (s._w & 65535) + (s._w >> 16); \
    \
    _Where_ = (s._z << 16) + s._w; \
  FOG_MACRO_END 

static uint32_t FOG_CDECL Random_nextUInt32(Random* self)
{
  uint32_t n;
  FOG_RANDOM_NEXT(n);

  return n;
}

static double FOG_CDECL Random_nextDouble(Random* self)
{
  uint32_t n;
  FOG_RANDOM_NEXT(n);

  // (n + 1) + 1/(2^32 + 2). The result is strictly between (0, 1), exclusive.
  return (double(n) + 1.0) + 2.328306435454494e-10;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Random_init(void)
{
  fog_api.random_ctor = Random_reset;
  fog_api.random_reset = Random_reset;
  
  fog_api.random_seed = Random_seed;
  fog_api.random_rewind = Random_rewind;
  
  fog_api.random_nextUInt32 = Random_nextUInt32;
  fog_api.random_nextDouble = Random_nextDouble;
}

} // Fog namespace
