// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {

// ===========================================================================
// [Fog::HashUtil - GetClosestPrime]
// ===========================================================================

static const uint32_t HashUtil_primeTable[] =
{
  7,
  17,
  31,
  47,
  83,
  107,
  157,
  211,
  353,
  509,
  769,
  997,
  1217,
  1567,
  2053,
  3011,
  4337,
  5939,
  7757,
  9973,
  13033,
  17783,
  23417,
  31517,
  37991,
  45161,
  50929,
  60637,
  71261,
  85469,
  95789,
  110273,
  131357,
  152287,
  175327,
  202777,
  238649,
  274019,
  310091,
  327869,
  354383,
  384437,
  410453,
  444443,
  488879,
  522229,
  555557,
  589759,
  626009,
  666989,
  708251,
  752797,
  802499,
  855601,
  939007,
  1007497,
  1155851,
  1333357,
  1553099,
  1815361,
  2554477,
  3557441,
  4981877,
  6977491,
  9854011,
  13289083,
  15289117,
  19709071,
  23025773,
  26725759,
  30534103,
  37534097,
  42555229,
  48555223,
  52555219,
  61555237,
  73698109,
  85698133,
  104395303,
  114395317,
  124395317,
  134395351,
  141650939,
  151150943,
  165150127,
  179424691,
  195424711,
  217645177,
  232645211,
  259123471,
  295075153,
  335075159,
  393342739,
  452876863,
  533910137,
  663910123,
  838041647,
  982451653
};

static FOG_INLINE uint32_t Hash_getClosestPrimeIndex(size_t value)
{
  uint32_t i = 0;

  // Optimize lookup in case that the value is too large.
  if (HashUtil_primeTable[64] < value)
    i = 64;
  else if (HashUtil_primeTable[48] < value)
    i = 48;
  else if (HashUtil_primeTable[32] < value)
    i = 32;
  else if (HashUtil_primeTable[16] < value)
    i = 16;

  while (i < FOG_ARRAY_SIZE(HashUtil_primeTable))
  {
    if (HashUtil_primeTable[i] >= value)
      return i;
    i++;
  }

  // Return the last one.
  return i - 1;
}

static size_t FOG_CDECL HashUtil_getClosestPrime(size_t value, int adjustTable)
{
  uint32_t index = Hash_getClosestPrimeIndex(value);

  if (adjustTable >= 0)
  {
    if ((uint)adjustTable >= FOG_ARRAY_SIZE(HashUtil_primeTable) - 1 - index)
      return HashUtil_primeTable[FOG_ARRAY_SIZE(HashUtil_primeTable) - 1];
    else
      return HashUtil_primeTable[index + (uint)adjustTable];
  }
  else
  {
    adjustTable = -adjustTable;

    if ((uint)adjustTable >= index)
      return HashUtil_primeTable[0];
    else
      return HashUtil_primeTable[index - (uint)adjustTable];
  }
}

// ===========================================================================
// [Fog::HashUtil - HashBinary]
// ===========================================================================

// From OSSP val - Value Access <http://www.ossp.org/pkg/lib/val/>
//
// BJDDJ Hash Function (Bob Jenkins, Dr. Dobbs Journal):
//
// This is a very complex but also very good hash function, as proposed
// in the March'97 issue of Dr. Dobbs Journal (DDJ) by Bob Jenkins (see
// http://burtleburtle.net/bob/hash/doobs.html for online version). He showed
// that this hash function has both very good distribution and performance and
// our own hash function comparison confirmed this. The only difference to the
// original function of B.J. here is that our version doesn't provide the
// 'level' (= previous hash) argument for consistency reasons with the other
// hash functions (i.e. same function signature). It can be definetely
// recommended as a good general purpose hash function.

static uint32_t FOG_CDECL HashUtil_hashBinary(const void* data, size_t length)
{
  const uint8_t* k = (const uint8_t*)data;
  uint32_t a, b, c;

  size_t i = length;
  if (i == 0) return 0;

  // Some abbreviations.
#define mix(a,b,c) \
  FOG_MACRO_BEGIN \
    a -= b; a -= c; a ^= (c>>13); \
    b -= c; b -= a; b ^= (a<< 8); \
    c -= a; c -= b; c ^= (b>>13); \
    a -= b; a -= c; a ^= (c>>12); \
    b -= c; b -= a; b ^= (a<<16); \
    c -= a; c -= b; c ^= (b>> 5); \
    a -= b; a -= c; a ^= (c>> 3); \
    b -= c; b -= a; b ^= (a<<10); \
    c -= a; c -= b; c ^= (b>>15); \
  FOG_MACRO_END

  // The golden ratio; an arbitrary value.
  a = b = 0x9E3779B9;
  c = 0;

  // Handle most of the key.
  while (i >= 12)
  {
    a += (k[0] + ((uint32_t)k[1]<<8) + ((uint32_t)k[ 2]<<16) + ((uint32_t)k[ 3]<<24));
    b += (k[4] + ((uint32_t)k[5]<<8) + ((uint32_t)k[ 6]<<16) + ((uint32_t)k[ 7]<<24));
    c += (k[8] + ((uint32_t)k[9]<<8) + ((uint32_t)k[10]<<16) + ((uint32_t)k[11]<<24));
    mix(a, b, c);

    k += 12;
    i -= 12;
  }

  // Handle the last 11 bytes.
  c += (uint32_t)i;
  switch (i)
  {
    // All the case statements fall through.
    case 11: c+=((uint32_t)k[10]<<24);
    case 10: c+=((uint32_t)k[ 9]<<16);
    case 9 : c+=((uint32_t)k[ 8]<< 8);
    // The first byte of c is reserved for the length.
    case 8 : b+=((uint32_t)k[ 7]<<24);
    case 7 : b+=((uint32_t)k[ 6]<<16);
    case 6 : b+=((uint32_t)k[ 5]<< 8);
    case 5 : b+=k[4];
    case 4 : a+=((uint32_t)k[ 3]<<24);
    case 3 : a+=((uint32_t)k[ 2]<<16);
    case 2 : a+=((uint32_t)k[ 1]<< 8);
    case 1 : a+=k[0];
    // case 0: nothing left to add.
  }
  mix(a, b, c);

#undef mix

  return c;
}

// ===========================================================================
// [Fog::HashUtil - HashVector]
// ===========================================================================

static uint32_t FOG_CDECL HashUtil_hashVectorD(const void* data, size_t length)
{
  const uint32_t* k = reinterpret_cast<const uint32_t*>(data);
  uint32_t h = 0;

  for (size_t i = 0; i < length; i++)
  {
    h += HashUtil::hashU32(k[i]);
  }

  return h;
}

static uint32_t FOG_CDECL HashUtil_hashVectorQ(const void* data, size_t length)
{
  const uint64_t* k = reinterpret_cast<const uint64_t*>(data);
  uint32_t h = 0;

  for (size_t i = 0; i < length; i++)
  {
    h += HashUtil::hashU64(k[i]);
  }

  return h;
}

// ===========================================================================
// [Fog::HashUtil - HashStubA]
// ===========================================================================

static uint32_t FOG_CDECL HashUtil_hashStubA(const StubA* stub)
{
  uint32_t h = 0;

  const uint8_t* k = reinterpret_cast<const uint8_t*>(stub->getData());
  size_t length = stub->getComputedLength();

  if (length == 0)
    return 0;

  do {
    uint32_t c = (uint32_t)*k++;
    h ^= ((h << 5) + (h >> 2) + c);
  } while (--length);

  return h;
}

// ===========================================================================
// [Fog::HashUtil - HashStubW]
// ===========================================================================

static uint32_t FOG_CDECL HashUtil_hashStubW(const StubW* stub)
{
  uint32_t h = 0;

  const uint16_t* k = reinterpret_cast<const uint16_t*>(stub->getData());
  size_t length = stub->getComputedLength();

  if (length == 0)
    return h;

  do {
    uint32_t c = (uint32_t)*k++;
    h ^= ((h << 5) + (h >> 2) + c);
  } while (--length);

  return h;
}

// ===========================================================================
// [Init / Fini]
// ===========================================================================

FOG_NO_EXPORT void HashUtil_init(void)
{
  // -------------------------------------------------------------------------
  // [Funcs]
  // -------------------------------------------------------------------------

  _api.hashutil_getClosestPrime = HashUtil_getClosestPrime;

  _api.hashutil_hashBinary = HashUtil_hashBinary;
  _api.hashutil_hashVectorD = HashUtil_hashVectorD;
  _api.hashutil_hashVectorQ = HashUtil_hashVectorQ;
  _api.hashutil_hashStubA = HashUtil_hashStubA;
  _api.hashutil_hashStubW = HashUtil_hashStubW;
}

} // Fog namespace
