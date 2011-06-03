// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>

namespace Fog {
namespace HashUtil {

// ===========================================================================
// [Fog::HashUtil::makeDataHash]
// ===========================================================================

// From OSSP val - Value Access <http://www.ossp.org/pkg/lib/val/>
//
// BJDDJ Hash Function (Bob Jenkins, Dr. Dobbs Journal):
// This is a very complex but also very good hash function, as proposed
// in the March'97 issue of Dr. Dobbs Journal (DDJ) by Bob Jenkins (see
// http://burtleburtle.net/bob/hash/doobs.html for online version). He
// showed that this hash function has both very good distribution and
// performance and our own hash function comparison confirmed this. The
// only difference to the original function of B.J. here is that our
// version doesn't provide the `level' (= previous hash) argument for
// consistency reasons with the other hash functions (i.e. same function
// signature). It can be definetely recommended as a good general
// purpose hash function.
uint32_t makeDataHash(const void* data, size_t size)
{
  const uint8_t* k = (const uint8_t*)data;
  uint32_t a, b, c;
  size_t len;

  if (size == 0) return 0;

  // some abbreviations
#define ub4 uint32_t
#define mix(a,b,c) { \
      a -= b; a -= c; a ^= (c>>13); \
      b -= c; b -= a; b ^= (a<< 8); \
      c -= a; c -= b; c ^= (b>>13); \
      a -= b; a -= c; a ^= (c>>12); \
      b -= c; b -= a; b ^= (a<<16); \
      c -= a; c -= b; c ^= (b>> 5); \
      a -= b; a -= c; a ^= (c>> 3); \
      b -= c; b -= a; b ^= (a<<10); \
      c -= a; c -= b; c ^= (b>>15); \
  }

  // setup the internal state
  len = size;
  // the golden ratio; an arbitrary value
  a = b = 0x9E3779B9;
  c = 0;

  // handle most of the key
  while (len >= 12)
  {
    a += (k[0] + ((ub4)k[1]<<8) + ((ub4)k[ 2]<<16) + ((ub4)k[ 3]<<24));
    b += (k[4] + ((ub4)k[5]<<8) + ((ub4)k[ 6]<<16) + ((ub4)k[ 7]<<24));
    c += (k[8] + ((ub4)k[9]<<8) + ((ub4)k[10]<<16) + ((ub4)k[11]<<24));
    mix(a, b, c);
    k += 12;
    len -= 12;
  }

  // handle the last 11 bytes
  c += (uint32_t)size;
  switch (len)
  {
    // all the case statements fall through
    case 11: c+=((ub4)k[10]<<24);
    case 10: c+=((ub4)k[ 9]<<16);
    case 9 : c+=((ub4)k[ 8]<< 8);
    // the first byte of c is reserved for the length
    case 8 : b+=((ub4)k[ 7]<<24);
    case 7 : b+=((ub4)k[ 6]<<16);
    case 6 : b+=((ub4)k[ 5]<< 8);
    case 5 : b+=k[4];
    case 4 : a+=((ub4)k[ 3]<<24);
    case 3 : a+=((ub4)k[ 2]<<16);
    case 2 : a+=((ub4)k[ 1]<< 8);
    case 1 : a+=k[0];
    // case 0: nothing left to add
  }
  mix(a, b, c);

#undef ub4
#undef mix

  // report the result
  return c;
}

// ===========================================================================
// [Fog::HashUtil::makeStringHash]
// ===========================================================================

uint32_t makeStringHash(const char* key, size_t length)
{
  uint32_t hash = 0x12345678;

  if (length == DETECT_LENGTH) length = StringUtil::len(key);
  if (!length) return 0;

  const uint8_t* k = reinterpret_cast<const uint8_t*>(key);

  do {
    uint32_t c = *k++;
    hash ^= ((hash << 5) + (hash >> 2) + c);
  } while (--length);

  return hash;
}

uint32_t makeStringHash(const Char* key, size_t length)
{
  uint32_t hash = 0x12345678;

  if (length == DETECT_LENGTH) length = StringUtil::len(key);
  if (!length) return 0;

  const uint16_t* k = reinterpret_cast<const uint16_t*>(key);

  do {
    uint32_t c = *k++;
    hash ^= ((hash << 5) + (hash >> 2) + c);
  } while (--length);

  return hash;
}

} // HashUtil namespace
} // Fog namespace
