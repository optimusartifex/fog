// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_FACE_FACEC_P64_H
#define _FOG_CORE_FACE_FACEC_P64_H

// [Dependencies]
#include <Fog/Core/Face/Constants.h>
#include <Fog/Core/Face/FaceC_P32.h>
#include <Fog/Core/Face/FaceC_Types.h>
#include <Fog/Core/Global/Global.h>

namespace Fog {
namespace Face {

//! @addtogroup Fog_Face_C
//! @{

// ============================================================================
// [Fog::Face - P64 - Types]
// ============================================================================

#if defined(FOG_FACE_P64_IS_TYPE)

#define _FOG_FACE_U64(_Val_) _Val_
#define _FOG_FACE_U64_LO(_Val_) ((uint32_t)(_Val_))
#define _FOG_FACE_U64_HI(_Val_) ((uint32_t)(_Val_ >> 32))

#define _FOG_FACE_U64_SET(_Val_, _Lo_, _Hi_) \
  FOG_MACRO_BEGIN \
    _Val_ = ( (uint64_t)(_Lo_) | ((uint64_t)(_Hi_) << 32) ) \
  FOG_MACRO_END

#else

#define _FOG_FACE_U64(_Val_) _Val_.u64
#define _FOG_FACE_U64_LO(_Val_) _Val_.u32Lo
#define _FOG_FACE_U64_HI(_Val_) _Val_.u32Hi

#define _FOG_FACE_U64_SET(_Val_, _Lo_, _Hi_) \
  FOG_MACRO_BEGIN \
    _Val_.u32Lo = _Lo_; \
    _Val_.u32Hi = _Hi_; \
  FOG_MACRO_END

#endif // FOG_FACE_P64_IS_TYPE

// Shortcuts.
#define _U64 _FOG_FACE_U64

// ============================================================================
// [Fog::Face - U64 - Read / Store]
// ============================================================================

#if defined(FOG_FACE_P64_IS_TYPE)

static FOG_INLINE void p64Load1b(p64& dst0, const void* srcp) { dst0 = ((const uint8_t*)srcp)[0]; }
static FOG_INLINE void p64Store1b(void* dstp, const p64& src0) { ((uint8_t *)dstp)[0] = (uint8_t )src0; }

static FOG_INLINE void p64Load2a(p64& dst0, const void* srcp) { dst0 = ((const uint16_t*)srcp)[0]; }
static FOG_INLINE void p64Load2u(p64& dst0, const void* srcp) { dst0 = ((const uint16_t*)srcp)[0]; }

static FOG_INLINE void p64Load2aBSwap(p64& dst0, const void* srcp) { dst0 = Memory::bswap16( ((const uint16_t*)srcp)[0] ); }
static FOG_INLINE void p64Load2uBSwap(p64& dst0, const void* srcp) { dst0 = Memory::bswap16( ((const uint16_t*)srcp)[0] ); }

static FOG_INLINE void p64Store2a(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0; }
static FOG_INLINE void p64Store2u(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0; }

static FOG_INLINE void p64Store2aBSwap(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = Memory::bswap16( (uint16_t)src0 ); }
static FOG_INLINE void p64Store2uBSwap(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = Memory::bswap16( (uint16_t)src0 ); }

static FOG_INLINE void p64Load4a(p64& dst0, const void* srcp) { dst0 = ((const uint32_t*)srcp)[0]; }
static FOG_INLINE void p64Load4u(p64& dst0, const void* srcp) { dst0 = ((const uint32_t*)srcp)[0]; }

static FOG_INLINE void p64Load4aBSwap(p64& dst0, const void* srcp) { dst0 = Memory::bswap32( ((const uint32_t*)srcp)[0] ); }
static FOG_INLINE void p64Load4uBSwap(p64& dst0, const void* srcp) { dst0 = Memory::bswap32( ((const uint32_t*)srcp)[0] ); }

static FOG_INLINE void p64Store4a(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0; }
static FOG_INLINE void p64Store4u(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0; }

static FOG_INLINE void p64Store4aBSwap(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = Memory::bswap32( (uint32_t)src0 ); }
static FOG_INLINE void p64Store4uBSwap(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = Memory::bswap32( (uint32_t)src0 ); }

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN

static FOG_INLINE void p64Load6a(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
}

static FOG_INLINE void p64Load6u(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
}

static FOG_INLINE void p64Load6aBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
  dst0 = Memory::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Load6uBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint32_t*)(src8 + 0))[0] ),
                              (uint64_t)( ((const uint16_t*)(src8 + 4))[0] ) << 32);
  dst0 = Memory::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Store6a(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(src0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(src0 >> 32);
}

static FOG_INLINE void p64Store6u(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(src0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(src0 >> 32);
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = Memory::bswap64(src0) >> 16;

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(x0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(x0 >> 32);
}

static FOG_INLINE void p64Store6uBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = Memory::bswap64(src0) >> 16;

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)(x0);
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)(x0 >> 32);
}

#else

static FOG_INLINE void p64Load6a(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
}

static FOG_INLINE void p64Load6u(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
}

static FOG_INLINE void p64Load6aBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
  dst0 = Memory::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Load6uBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0 = _FOG_FACE_COMBINE_2( (uint64_t)( ((const uint16_t*)(src8 + 0))[0] ) << 32,
                              (uint64_t)( ((const uint32_t*)(src8 + 2))[0] ) );
  dst0 = Memory::bswap64(dst0) >> 16;
}

static FOG_INLINE void p64Store6a(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(src0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(src0);
}

static FOG_INLINE void p64Store6u(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(src0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(src0);
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = Memory::bswap64(src0);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(x0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(x0);
}

static FOG_INLINE void p64Store6uBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);
  uint64_t x0 = Memory::bswap64(src0);

  ((uint16_t*)(dst8 + 0))[0] = (uint32_t)(x0 >> 32);
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)(x0);
}

#endif // FOG_BYTE_ORDER

static FOG_INLINE void p64Load8a(p64& dst0, const void* srcp) { dst0 = ((const uint64_t*)srcp)[0]; }
static FOG_INLINE void p64Load8u(p64& dst0, const void* srcp) { dst0 = ((const uint64_t*)srcp)[0]; }

static FOG_INLINE void p64Load8aBSwap(p64& dst0, const void* srcp) { dst0 = Memory::bswap64( ((const uint64_t*)srcp)[0] ); }
static FOG_INLINE void p64Load8uBSwap(p64& dst0, const void* srcp) { dst0 = Memory::bswap64( ((const uint64_t*)srcp)[0] ); }

static FOG_INLINE void p64Store8a(void* dstp, const p64& src0) { ((uint64_t*)dstp)[0] = (uint64_t)src0; }
static FOG_INLINE void p64Store8u(void* dstp, const p64& src0) { ((uint64_t*)dstp)[0] = (uint64_t)src0; }

static FOG_INLINE void p64Store8aBSwap(void* dstp, const p64& src0) { ((uint64_t*)dstp)[0] = Memory::bswap64( (uint64_t)src0 ); }
static FOG_INLINE void p64Store8uBSwap(void* dstp, const p64& src0) { ((uint64_t*)dstp)[0] = Memory::bswap64( (uint64_t)src0 ); }

#else

static FOG_INLINE void p64Load1b(p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint8_t*)srcp)[0]; }
static FOG_INLINE void p64Store1b(void* dstp, const p64& src0) { ((uint8_t *)dstp)[0] = (uint8_t )src0.u32Lo; }

static FOG_INLINE void p64Load2a(p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint16_t*)srcp)[0]; }
static FOG_INLINE void p64Load2u(p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint16_t*)srcp)[0]; }

static FOG_INLINE void p64Load2aBSwap(p64& dst0, const void* srcp) { dst0.u32Lo = Memory::bswap16( ((const uint16_t*)srcp)[0] ); }
static FOG_INLINE void p64Load2uBSwap(p64& dst0, const void* srcp) { dst0.u32Lo = Memory::bswap16( ((const uint16_t*)srcp)[0] ); }

static FOG_INLINE void p64Store2a(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0.u32Lo; }
static FOG_INLINE void p64Store2u(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = (uint16_t)src0.u32Lo; }

static FOG_INLINE void p64Store2aBSwap(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = Memory::bswap16( (uint16_t)src0.u32Lo ); }
static FOG_INLINE void p64Store2uBSwap(void* dstp, const p64& src0) { ((uint16_t*)dstp)[0] = Memory::bswap16( (uint16_t)src0.u32Lo ); }

static FOG_INLINE void p64Load4a(p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint32_t*)srcp)[0]; }
static FOG_INLINE void p64Load4u(p64& dst0, const void* srcp) { dst0.u32Lo = ((const uint32_t*)srcp)[0]; }

static FOG_INLINE void p64Load4aBSwap(p64& dst0, const void* srcp) { dst0.u32Lo = Memory::bswap32( ((const uint32_t*)srcp)[0] ); }
static FOG_INLINE void p64Load4uBSwap(p64& dst0, const void* srcp) { dst0.u32Lo = Memory::bswap32( ((const uint32_t*)srcp)[0] ); }

static FOG_INLINE void p64Store4a(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0.u32Lo; }
static FOG_INLINE void p64Store4u(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = (uint32_t)src0.u32Lo; }

static FOG_INLINE void p64Store4aBSwap(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = Memory::bswap32( (uint32_t)src0.u32Lo ); }
static FOG_INLINE void p64Store4uBSwap(void* dstp, const p64& src0) { ((uint32_t*)dstp)[0] = Memory::bswap32( (uint32_t)src0.u32Lo ); }

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN

static FOG_INLINE void p64Load6a(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = ((const uint32_t*)(src8 + 0))[0];
  dst0.u32Hi = ((const uint16_t*)(src8 + 4))[0];
}

static FOG_INLINE void p64Load6u(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = ((const uint32_t*)(src8 + 0))[0];
  dst0.u32Hi = ((const uint16_t*)(src8 + 4))[0];
}

static FOG_INLINE void p64Load6aBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = Memory::bswap16( ((const uint16_t*)(src8 + 0))[0] );
  dst0.u32Lo = Memory::bswap32( ((const uint32_t*)(src8 + 2))[0] );
}

static FOG_INLINE void p64Load6uBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = Memory::bswap16( ((const uint16_t*)(src8 + 0))[0] );
  dst0.u32Lo = Memory::bswap32( ((const uint32_t*)(src8 + 2))[0] );
}

static FOG_INLINE void p64Store6a(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)src0.u32Lo;
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)src0.u32Hi;
}

static FOG_INLINE void p64Store6u(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = (uint32_t)src0.u32Lo;
  ((uint16_t*)(dst8 + 4))[0] = (uint16_t)src0.u32Hi;
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = Memory::bswap16((uint16_t)src0.u32Hi);
  ((uint32_t*)(dst8 + 2))[0] = Memory::bswap32((uint32_t)src0.u32Lo);
}

static FOG_INLINE void p64Store6uBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = Memory::bswap16((uint16_t)src0.u32Hi);
  ((uint32_t*)(dst8 + 2))[0] = Memory::bswap32((uint32_t)src0.u32Lo);
}

#else

static FOG_INLINE void p64Load6a(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = ((const uint16_t*)(src8 + 0))[0];
  dst0.u32Lo = ((const uint32_t*)(src8 + 2))[0];
}

static FOG_INLINE void p64Load6u(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Hi = ((const uint16_t*)(src8 + 0))[0];
  dst0.u32Lo = ((const uint32_t*)(src8 + 2))[0];
}

static FOG_INLINE void p64Load6aBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = Memory::bswap32( ((const uint32_t*)(src8 + 0))[0] );
  dst0.u32Hi = Memory::bswap16( ((const uint16_t*)(src8 + 4))[0] );
}

static FOG_INLINE void p64Load6uBSwap(p64& dst0, const void* srcp)
{
  const uint8_t* src8 = reinterpret_cast<const uint8_t*>(srcp);

  dst0.u32Lo = Memory::bswap32( ((const uint32_t*)(src8 + 0))[0] );
  dst0.u32Hi = Memory::bswap16( ((const uint16_t*)(src8 + 4))[0] );
}

static FOG_INLINE void p64Store6a(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint16_t)src0.u32Hi;
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)src0.u32Lo;

}
static FOG_INLINE void p64Store6u(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint16_t*)(dst8 + 0))[0] = (uint16_t)src0.u32Hi;
  ((uint32_t*)(dst8 + 2))[0] = (uint32_t)src0.u32Lo;
}

static FOG_INLINE void p64Store6aBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = Memory::bswap32( (uint32_t)src0.u32Lo );
  ((uint16_t*)(dst8 + 4))[0] = Memory::bswap16( (uint16_t)src0.u32Hi );

}
static FOG_INLINE void p64Store6uBSwap(void* dstp, const p64& src0)
{
  uint8_t* dst8 = reinterpret_cast<uint8_t*>(dstp);

  ((uint32_t*)(dst8 + 0))[0] = Memory::bswap32( (uint32_t)src0.u32Lo );
  ((uint16_t*)(dst8 + 4))[0] = Memory::bswap16( (uint16_t)src0.u32Hi );
}

#endif // FOG_BYTE_ORDER

static FOG_INLINE void p64Load8a(p64& dst0, const void* srcp)
{
  dst0.u32Lo = ((const uint32_t*)srcp)[0];
  dst0.u32Hi = ((const uint32_t*)srcp)[1];
}

static FOG_INLINE void p64Load8u(p64& dst0, const void* srcp)
{
  dst0.u32Lo = ((const uint32_t*)srcp)[0];
  dst0.u32Hi = ((const uint32_t*)srcp)[1];
}

static FOG_INLINE void p64Load8aBSwap(p64& dst0, const void* srcp)
{
  dst0.u32Hi = Memory::bswap32( ((const uint32_t*)srcp)[0] );
  dst0.u32Lo = Memory::bswap32( ((const uint32_t*)srcp)[1] );
}

static FOG_INLINE void p64Load8uBSwap(p64& dst0, const void* srcp)
{
  dst0.u32Hi = Memory::bswap32( ((const uint32_t*)srcp)[0] );
  dst0.u32Lo = Memory::bswap32( ((const uint32_t*)srcp)[1] );
}

static FOG_INLINE void p64Store8a(void* dstp, const p64& src0)
{
  ((uint32_t*)dstp)[0] = src0.u32Lo;
  ((uint32_t*)dstp)[1] = src0.u32Hi;
}

static FOG_INLINE void p64Store8u(void* dstp, const p64& src0)
{
  ((uint32_t*)dstp)[0] = src0.u32Lo;
  ((uint32_t*)dstp)[1] = src0.u32Hi;
}

static FOG_INLINE void p64Store8aBSwap(void* dstp, const p64& src0)
{
  ((uint32_t*)dstp)[0] = Memory::bswap32( src0.u32Hi );
  ((uint32_t*)dstp)[1] = Memory::bswap32( src0.u32Lo );
}

static FOG_INLINE void p64Store8uBSwap(void* dstp, const p64& src0)
{
  ((uint32_t*)dstp)[0] = Memory::bswap32( src0.u32Hi );
  ((uint32_t*)dstp)[1] = Memory::bswap32( src0.u32Lo );
}

#endif // FOG_FACE_P64_IS_TYPE

// ============================================================================
// [Fog::Face - P64 - Clear]
// ============================================================================

//! @brief Clear the @a dst0.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! dst0.u32[1] = 0
//! @endverbatim
static FOG_INLINE void p64Clear(p64& dst0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = 0;
#else
  dst0.u32Lo = 0;
  dst0.u32Hi = 0;
#endif
}

//! @brief Clear the @a dst0/dst1.
//!
//! @verbatim
//! dst0.u32[0] = 0
//! dst0.u32[1] = 0
//! dst1.u32[0] = 0
//! dst1.u32[1] = 0
//! @endverbatim
static FOG_INLINE void p64Clear_2x(p64& dst0, p64& dst1)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = 0;
  dst1 = 0;
#else
  dst0.u32Lo = 0;
  dst0.u32Hi = 0;
  dst1.u32Lo = 0;
  dst1.u32Hi = 0;
#endif
}

// ============================================================================
// [Fog::Face - P64 - Copy]
// ============================================================================

//! @brief Copy @a x0 to @a dst0.
//!
//! @verbatim
//! dst0 = x0
//! @endverbatim
static FOG_INLINE void p64Copy(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
#endif
}

//! @brief Copy @a x0/x1 to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = x0
//! dst1 = x1
//! @endverbatim
static FOG_INLINE void p64Copy_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0;
  dst1 = x1;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
  dst1.u32Lo = x1.u32Lo;
  dst1.u32Hi = x1.u32Hi;
#endif
}

// ============================================================================
// [Fog::Face - P64 - From]
// ============================================================================

static FOG_INLINE p64 p64FromU32(const uint32_t& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  return (uint64_t)x0;
#else
  p64 packed;
  packed.u32Lo = x0;
  return packed;
#endif
}

static FOG_INLINE p64 p64FromU64(const uint64_t& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  return (uint64_t)x0;
#else
  p64 packed;
  packed.u64 = x0;
  return packed;
#endif
}

static FOG_INLINE uint64_t u64FromP64(const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  return (uint64_t)x0;
#else
  return x0.u64;
#endif
}

static FOG_INLINE uint32_t u32FromP64PDD0(const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  return (uint32_t)x0;
#else
  return x0.u32Lo;
#endif
}

static FOG_INLINE uint32_t u32FromP64PDD1(const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  return (uint32_t)(x0 >> 32);
#else
  return x0.u32Hi;
#endif
}

// ============================================================================
// [Fog::Face - P64 - As]
// ============================================================================

#if defined(FOG_FACE_P64_IS_TYPE)

static FOG_INLINE uint32_t p64PBB0AsU32(const p64& x0) { return (uint32_t)(x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB1AsU32(const p64& x0) { return (uint32_t)(x0 >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB2AsU32(const p64& x0) { return (uint32_t)(x0 >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB3AsU32(const p64& x0) { return (uint32_t)(x0 >> 24) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB4AsU32(const p64& x0) { return (uint32_t)(x0 >> 32) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB5AsU32(const p64& x0) { return (uint32_t)(x0 >> 40) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB6AsU32(const p64& x0) { return (uint32_t)(x0 >> 48) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB7AsU32(const p64& x0) { return (uint32_t)(x0 >> 56) & 0xFFU; }

static FOG_INLINE uint32_t p64PBW0AsU32(const p64& x0) { return (uint32_t)(x0      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW1AsU32(const p64& x0) { return (uint32_t)(x0 >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW2AsU32(const p64& x0) { return (uint32_t)(x0 >> 32) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW3AsU32(const p64& x0) { return (uint32_t)(x0 >> 48); }

static FOG_INLINE uint32_t p64PWW0AsU32(const p64& x0) { return (uint32_t)(x0      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW1AsU32(const p64& x0) { return (uint32_t)(x0 >> 16) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW2AsU32(const p64& x0) { return (uint32_t)(x0 >> 32) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW3AsU32(const p64& x0) { return (uint32_t)(x0 >> 48); }

static FOG_INLINE uint32_t p64PWD0AsU32(const p64& x0) { return (uint32_t)(x0); }
static FOG_INLINE uint32_t p64PWD1AsU32(const p64& x0) { return (uint32_t)(x0 >> 32); }

static FOG_INLINE uint32_t p64PDD0AsU32(const p64& x0) { return (uint32_t)(x0); }
static FOG_INLINE uint32_t p64PDD1AsU32(const p64& x0) { return (uint32_t)(x0 >> 32); }

#else

static FOG_INLINE uint32_t p64PBB0AsU32(const p64& x0) { return (x0.u32Lo      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB1AsU32(const p64& x0) { return (x0.u32Lo >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB2AsU32(const p64& x0) { return (x0.u32Lo >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB3AsU32(const p64& x0) { return (x0.u32Lo >> 24);         }

static FOG_INLINE uint32_t p64PBB4AsU32(const p64& x0) { return (x0.u32Hi      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB5AsU32(const p64& x0) { return (x0.u32Hi >>  8) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB6AsU32(const p64& x0) { return (x0.u32Hi >> 16) & 0xFFU; }
static FOG_INLINE uint32_t p64PBB7AsU32(const p64& x0) { return (x0.u32Hi >> 24);         }

static FOG_INLINE uint32_t p64PBW0AsU32(const p64& x0) { return (x0.u32Lo      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW1AsU32(const p64& x0) { return (x0.u32Lo >> 16);         }
static FOG_INLINE uint32_t p64PBW2AsU32(const p64& x0) { return (x0.u32Hi      ) & 0xFFU; }
static FOG_INLINE uint32_t p64PBW3AsU32(const p64& x0) { return (x0.u32Hi >> 16);         }

static FOG_INLINE uint32_t p64PWW0AsU32(const p64& x0) { return (x0.u32Lo      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW1AsU32(const p64& x0) { return (x0.u32Lo >> 16);           }
static FOG_INLINE uint32_t p64PWW2AsU32(const p64& x0) { return (x0.u32Hi      ) & 0xFFFFU; }
static FOG_INLINE uint32_t p64PWW3AsU32(const p64& x0) { return (x0.u32Hi >> 16);           }

static FOG_INLINE uint32_t p64PWD0AsU32(const p64& x0) { return (x0.u32Lo); }
static FOG_INLINE uint32_t p64PWD1AsU32(const p64& x0) { return (x0.u32Hi); }

static FOG_INLINE uint32_t p64PDD0AsU32(const p64& x0) { return (x0.u32Lo); }
static FOG_INLINE uint32_t p64PDD1AsU32(const p64& x0) { return (x0.u32Hi); }

#endif // FOG_FACE_P64_IS_TYPE

// ============================================================================
// [Fog::Face - P64 - Pack]
// ============================================================================

//! @brief Pack u64_1x4b value into DWORD.
static FOG_INLINE void p64Pack2031(p32& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_U64(x0), _U64(x0) >> 24));
#else
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_FOG_FACE_U64_LO(x0), _FOG_FACE_U64_HI(x0) << 8));
#endif
}

//! @brief Pack u64_1x4b value into DWORD.
static FOG_INLINE void p64Pack2031_RShift8(p32& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_U64(x0 >> 8), _U64(x0) >> 32));
#else
  dst0 = (uint32_t)(_FOG_FACE_COMBINE_2(_FOG_FACE_U64_LO(x0) >> 8, _FOG_FACE_U64_HI(x0)));
#endif
}

// ============================================================================
// [Fog::Face - P64 - Unpack]
// ============================================================================

//! @brief Unpack 2031 BYTEs of @a x0 into one p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB2031(
  p64& dst0, const p32& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  _U64(dst0) = (uint64_t)((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0 >> 8) & 0x00FF00FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Unpack 2031 BYTEs of @a x0 into one p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB2031(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  _U64(dst0) = (uint64_t)(_U64(x0) | (_U64(x0) << 24)) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Lo >> 8) & 0x00FF00FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Unpack '20_1' bytes of DWORD into one p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB20Z1(
  p64& dst0, const p32& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  _U64(dst0) = (uint64_t)((uint64_t)x0 | ((uint64_t)x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
#else
  dst0.u32Lo = (x0     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0 >> 8) & 0x000000FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Unpack '20_1' bytes of DWORD into one p64 value.
static FOG_INLINE void p64UnpackPBWFromPBB20Z1(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  _U64(dst0) = (uint64_t)(x0 | (x0 << 24)) & FOG_UINT64_C(0x000000FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo     ) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Lo >> 8) & 0x000000FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

// ============================================================================
// [Fog::Face - P64 - Zero / Fill]
// ============================================================================

#if defined(FOG_FACE_P64_IS_TYPE)

static FOG_INLINE void p64ZeroPBB0(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFFFFFF); }
static FOG_INLINE void p64ZeroPBB1(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFF00FF); }
static FOG_INLINE void p64ZeroPBB2(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFF00FFFF); }
static FOG_INLINE void p64ZeroPBB3(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFF00FFFFFF); }
static FOG_INLINE void p64ZeroPBB4(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFF00FFFFFFFF); }
static FOG_INLINE void p64ZeroPBB5(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFF00FFFFFFFFFF); }
static FOG_INLINE void p64ZeroPBB6(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFF00FFFFFFFFFFFF); }
static FOG_INLINE void p64ZeroPBB7(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0x00FFFFFFFFFFFFFF); }

static FOG_INLINE void p64ZeroPBW0(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFFFF00); }
static FOG_INLINE void p64ZeroPBW1(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFF00FFFF); }
static FOG_INLINE void p64ZeroPBW2(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFF00FFFFFFFF); }
static FOG_INLINE void p64ZeroPBW3(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFF00FFFFFFFFFFFF); }

static FOG_INLINE void p64ZeroPWW0(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFF0000); }
static FOG_INLINE void p64ZeroPWW1(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFF0000FFFF); }
static FOG_INLINE void p64ZeroPWW2(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFF0000FFFFFFFF); }
static FOG_INLINE void p64ZeroPWW3(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0x0000FFFFFFFFFFFF); }

static FOG_INLINE void p64ZeroPWD0(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFFFFFF0000); }
static FOG_INLINE void p64ZeroPWD1(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFF0000FFFFFFFF); }

static FOG_INLINE void p64ZeroPDD0(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0xFFFFFFFF00000000); }
static FOG_INLINE void p64ZeroPDD1(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0x00000000FFFFFFFF); }

static FOG_INLINE void p64FillPBB0(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000000000FF); }
static FOG_INLINE void p64FillPBB1(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000000000FF00); }
static FOG_INLINE void p64FillPBB2(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000000000FF0000); }
static FOG_INLINE void p64FillPBB3(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000FF000000); }
static FOG_INLINE void p64FillPBB4(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000FF00000000); }
static FOG_INLINE void p64FillPBB5(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000FF0000000000); }
static FOG_INLINE void p64FillPBB6(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00FF000000000000); }
static FOG_INLINE void p64FillPBB7(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFF00000000000000); }

static FOG_INLINE void p64FillPBW0(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000000000FF); }
static FOG_INLINE void p64FillPBW1(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000000000FF0000); }
static FOG_INLINE void p64FillPBW2(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000FF00000000); }
static FOG_INLINE void p64FillPBW3(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00FF000000000000); }

static FOG_INLINE void p64FillPWW0(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000000000FFFF); }
static FOG_INLINE void p64FillPWW1(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000FFFF0000); }
static FOG_INLINE void p64FillPWW2(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000FFFF00000000); }
static FOG_INLINE void p64FillPWW3(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFFFF000000000000); }

static FOG_INLINE void p64FillPWD0(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x000000000000FFFF); }
static FOG_INLINE void p64FillPWD1(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x0000FFFF00000000); }

static FOG_INLINE void p64FillPDD0(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0x00000000FFFFFFFF); }
static FOG_INLINE void p64FillPDD1(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFFFFFFFF00000000); }

static FOG_INLINE void p64ZeroPBB3_7(p64& dst0, const p64& x0) { dst0 = x0 & FOG_UINT64_C(0x00FFFFFF00FFFFFF); }
static FOG_INLINE void p64FillPBB3_7(p64& dst0, const p64& x0) { dst0 = x0 | FOG_UINT64_C(0xFF000000FF000000); }

#else

static FOG_INLINE void p64ZeroPBB0(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFFFF00U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFF00FFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB2(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFF00FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB3(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0x00FFFFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBB4(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFFFF00U; }
static FOG_INLINE void p64ZeroPBB5(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFF00FFU; }
static FOG_INLINE void p64ZeroPBB6(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFF00FFFFU; }
static FOG_INLINE void p64ZeroPBB7(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0x00FFFFFFU; }

static FOG_INLINE void p64ZeroPBW0(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFFFF00U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBW1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFF00FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPBW2(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFFFF00U; }
static FOG_INLINE void p64ZeroPBW3(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFF00FFFFU; }

static FOG_INLINE void p64ZeroPWW0(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0xFFFF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPWW1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0x0000FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPWW2(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0xFFFF0000U; }
static FOG_INLINE void p64ZeroPWW3(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi & 0x0000FFFFU; }

static FOG_INLINE void p64ZeroPWD0(p64& dst0, const p64& x0) { dst0.u32Lo = 0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPWD1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0; }

static FOG_INLINE void p64ZeroPDD0(p64& dst0, const p64& x0) { dst0.u32Lo = 0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ZeroPDD1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0; }

static FOG_INLINE void p64FillPBB0(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0x000000FFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0x0000FF00U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB2(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0x00FF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB3(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0xFF000000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBB4(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x000000FFU; }
static FOG_INLINE void p64FillPBB5(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x0000FF00U; }
static FOG_INLINE void p64FillPBB6(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x00FF0000U; }
static FOG_INLINE void p64FillPBB7(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0xFF000000U; }

static FOG_INLINE void p64FillPBW0(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0x000000FFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBW1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0x00FF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPBW2(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x000000FFU; }
static FOG_INLINE void p64FillPBW3(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x00FF0000U; }

static FOG_INLINE void p64FillPWW0(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0x0000FFFFU; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPWW1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0xFFFF0000U; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPWW2(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0x0000FFFFU; }
static FOG_INLINE void p64FillPWW3(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = x0.u32Hi | 0xFFFF0000U; }

static FOG_INLINE void p64FillPWD0(p64& dst0, const p64& x0) { dst0.u32Lo = 0x0000FFFF; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPWD1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0x0000FFFF; }

static FOG_INLINE void p64FillPDD0(p64& dst0, const p64& x0) { dst0.u32Lo = 0xFFFFFFFF; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64FillPDD1(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = 0xFFFFFFFF; }

static FOG_INLINE void p64ZeroPBB3_7(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo & 0x00FFFFFFU; dst0.u32Hi = x0.u32Hi & 0x00FFFFFFU; }
static FOG_INLINE void p64FillPBB3_7(p64& dst0, const p64& x0) { dst0.u32Lo = x0.u32Lo | 0xFF000000U; dst0.u32Hi = x0.u32Hi | 0xFF000000U; }

#endif // FOG_FACE_P64_IS_TYPE

// ============================================================================
// [Fog::Face - P64 - Replace]
// ============================================================================

#if defined(FOG_FACE_P64_IS_TYPE)

static FOG_INLINE void p64ReplacePBB0(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), u0      ); }
static FOG_INLINE void p64ReplacePBB1(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FF00), u0 <<  8); }
static FOG_INLINE void p64ReplacePBB2(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), u0 << 16); }
static FOG_INLINE void p64ReplacePBB3(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FF000000), u0 << 24); }
static FOG_INLINE void p64ReplacePBB4(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), u0 << 32); }
static FOG_INLINE void p64ReplacePBB5(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FF0000000000), u0 << 40); }
static FOG_INLINE void p64ReplacePBB6(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), u0 << 48); }
static FOG_INLINE void p64ReplacePBB7(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFF00000000000000), u0 << 56); }

static FOG_INLINE void p64ReplacePBW0(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), u0      ); }
static FOG_INLINE void p64ReplacePBW1(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), u0 << 16); }
static FOG_INLINE void p64ReplacePBW2(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), u0 << 32); }
static FOG_INLINE void p64ReplacePBW3(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), u0 << 48); }

static FOG_INLINE void p64ReplacePWW0(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), u0      ); }
static FOG_INLINE void p64ReplacePWW1(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFF0000), u0 << 16); }
static FOG_INLINE void p64ReplacePWW2(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), u0 << 32); }
static FOG_INLINE void p64ReplacePWW3(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFF000000000000), u0 << 48); }

static FOG_INLINE void p64ReplacePWD0(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), u0      ); }
static FOG_INLINE void p64ReplacePWD1(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), u0 << 32); }

static FOG_INLINE void p64ReplacePDD0(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFFFFFF), u0      ); }
static FOG_INLINE void p64ReplacePDD1(p64& dst0, const p64& x0, const p64& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFFFFFF00000000), u0 << 32); }

static FOG_INLINE void p64ReplacePBB0(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePBB1(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FF00), (uint64_t)u0 <<  8); }
static FOG_INLINE void p64ReplacePBB2(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), (uint64_t)u0 << 16); }
static FOG_INLINE void p64ReplacePBB3(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FF000000), (uint64_t)u0 << 24); }
static FOG_INLINE void p64ReplacePBB4(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), (uint64_t)u0 << 32); }
static FOG_INLINE void p64ReplacePBB5(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FF0000000000), (uint64_t)u0 << 40); }
static FOG_INLINE void p64ReplacePBB6(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), (uint64_t)u0 << 48); }
static FOG_INLINE void p64ReplacePBB7(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFF00000000000000), (uint64_t)u0 << 56); }

static FOG_INLINE void p64ReplacePBW0(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000000000FF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePBW1(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000000000FF0000), (uint64_t)u0 << 16); }
static FOG_INLINE void p64ReplacePBW2(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000FF00000000), (uint64_t)u0 << 32); }
static FOG_INLINE void p64ReplacePBW3(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00FF000000000000), (uint64_t)u0 << 48); }

static FOG_INLINE void p64ReplacePWW0(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePWW1(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFF0000), (uint64_t)u0 << 16); }
static FOG_INLINE void p64ReplacePWW2(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), (uint64_t)u0 << 32); }
static FOG_INLINE void p64ReplacePWW3(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFF000000000000), (uint64_t)u0 << 48); }

static FOG_INLINE void p64ReplacePWD0(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x000000000000FFFF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePWD1(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x0000FFFF00000000), (uint64_t)u0 << 32); }

static FOG_INLINE void p64ReplacePDD0(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0x00000000FFFFFFFF), (uint64_t)u0      ); }
static FOG_INLINE void p64ReplacePDD1(p64& dst0, const p64& x0, const p32& u0) { dst0 = _FOG_FACE_COMBINE_2(x0 & FOG_UINT64_C(0xFFFFFFFF00000000), (uint64_t)u0 << 32); }

#else

static FOG_INLINE void p64ReplacePBB0(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0.u32Lo      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB1(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FF00U, u0.u32Lo <<  8); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB2(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0.u32Lo << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB3(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFF000000U, u0.u32Lo << 24); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB4(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0.u32Lo      ); }
static FOG_INLINE void p64ReplacePBB5(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FF00U, u0.u32Lo <<  8); }
static FOG_INLINE void p64ReplacePBB6(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0.u32Lo << 16); }
static FOG_INLINE void p64ReplacePBB7(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFF000000U, u0.u32Lo << 24); }

static FOG_INLINE void p64ReplacePBW0(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0.u32Lo      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW1(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0.u32Lo << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW2(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0.u32Lo      ); }
static FOG_INLINE void p64ReplacePBW3(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0.u32Lo << 16); }

static FOG_INLINE void p64ReplacePWW0(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FFFFU, u0.u32Lo      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW1(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFFFF0000U, u0.u32Lo << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW2(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FFFFU, u0.u32Lo      ); }
static FOG_INLINE void p64ReplacePWW3(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFFFF0000U, u0.u32Lo << 16); }

static FOG_INLINE void p64ReplacePWD0(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = u0.u32Lo; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWD1(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0.u32Lo; }

static FOG_INLINE void p64ReplacePDD0(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = u0.u32Lo; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePDD1(p64& dst0, const p64& x0, const p64& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0.u32Lo; }

static FOG_INLINE void p64ReplacePBB0(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB1(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FF00U, u0 <<  8); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB2(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0 << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB3(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFF000000U, u0 << 24); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBB4(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0     ); }
static FOG_INLINE void p64ReplacePBB5(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FF00U, u0 <<  8); }
static FOG_INLINE void p64ReplacePBB6(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0 << 16); }
static FOG_INLINE void p64ReplacePBB7(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFF000000U, u0 << 24); }

static FOG_INLINE void p64ReplacePBW0(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x000000FFU, u0      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW1(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, u0 << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePBW2(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x000000FFU, u0      ); }
static FOG_INLINE void p64ReplacePBW3(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x00FF0000U, u0 << 16); }

static FOG_INLINE void p64ReplacePWW0(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x0000FFFFU, u0      ); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW1(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFFFF0000U, u0 << 16); dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWW2(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0x0000FFFFU, u0      ); }
static FOG_INLINE void p64ReplacePWW3(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi & 0xFFFF0000U, u0 << 16); }

static FOG_INLINE void p64ReplacePWD0(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = u0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePWD1(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0; }

static FOG_INLINE void p64ReplacePDD0(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = u0; dst0.u32Hi = x0.u32Hi; }
static FOG_INLINE void p64ReplacePDD1(p64& dst0, const p64& x0, const p32& u0) { dst0.u32Lo = x0.u32Lo; dst0.u32Hi = u0; }

#endif // FOG_FACE_P64_IS_TYPE

// ============================================================================
// [Fog::Face - P64 - Logical / Arithmetic]
// ============================================================================

#if defined(FOG_FACE_P64_IS_TYPE)
#define _FOG_FACE_64BIT_OP(_Dst_, _X_, _Y_, _Op_) \
  FOG_MACRO_BEGIN \
    _Dst_ = _X_ _Op_ _Y_; \
  FOG_MACRO_END
#else
#define _FOG_FACE_64BIT_OP(_Dst_, _X_, _Y_, _Op_) \
  FOG_MACRO_BEGIN \
    _Dst_.u32Lo = _X_.u32Lo _Op_ _Y_.u32Lo; \
    _Dst_.u32Hi = _X_.u32Hi _Op_ _Y_.u32Hi; \
  FOG_MACRO_END
#endif

//! @brief Scalar combine (AND or OR, depends on platform).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64Combine(
  p64& dst0, const p64& x0, const p64& y0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = _FOG_FACE_COMBINE_2(x0, y0);
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo, y0.u32Lo);
  dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi, y0.u32Hi);
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Scalar combine (AND or OR, depends on platform) (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] +| y0.u32[0]
//! dst1.u32[0] = x1.u32[0] +| y1.u32[0]
//! @endverbatim
static FOG_INLINE void p64Combine_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = _FOG_FACE_COMBINE_2(x0, y0);
  dst1 = _FOG_FACE_COMBINE_2(x1, y1);
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo, y0.u32Lo);
  dst0.u32Hi = _FOG_FACE_COMBINE_2(x0.u32Hi, y0.u32Hi);
  dst1.u32Lo = _FOG_FACE_COMBINE_2(x1.u32Lo, y1.u32Lo);
  dst1.u32Hi = _FOG_FACE_COMBINE_2(x1.u32Hi, y1.u32Hi);
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Scalar AND.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64And(
  p64& dst0, const p64& x0, const p64& y0)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, &);
}

//! @brief Scalar AND (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] & y0.u32[0]
//! dst1.u32[0] = x1.u32[0] & y1.u32[0]
//! @endverbatim
static FOG_INLINE void p64And_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, &);
  _FOG_FACE_64BIT_OP(dst1, x1, y1, &);
}

//! @brief Scalar OR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64Or(
  p64& dst0, const p64& x0, const p64& y0)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, |);
}

//! @brief Scalar OR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] | y0.u32[0]
//! dst0.u32[1] = x0.u32[1] | y0.u32[1]
//! dst1.u32[0] = x1.u32[0] | y1.u32[0]
//! dst1.u32[1] = x1.u32[1] | y1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Or_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, |);
  _FOG_FACE_64BIT_OP(dst1, x1, y1, |);
}

//! @brief Scalar XOR.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! dst0.u32[1] = x0.u32[1] ^ y0.u32[1]
//! @endverbatim
static FOG_INLINE void p64Xor(
  p64& dst0, const p64& x0, const p64& y0)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, ^);
}

//! @brief Scalar XOR (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] ^ y0.u32[0]
//! dst0.u32[1] = x0.u32[1] ^ y0.u32[1]
//! dst1.u32[0] = x1.u32[0] ^ y1.u32[0]
//! dst1.u32[1] = x1.u32[1] ^ y1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Xor_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  _FOG_FACE_64BIT_OP(dst0, x0, y0, ^);
  _FOG_FACE_64BIT_OP(dst1, x1, y1, ^);
}

//! @brief Scalar negate.
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! dst0.u32[1] = ~x0.u32[1]
//! @endverbatim
static FOG_INLINE void p64Neg(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = ~x0;
#else
  dst0.u32Lo = ~x0.u32Lo;
  dst0.u32Hi = ~x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Scalar negate (2x).
//!
//! @verbatim
//! dst0.u32[0] = ~x0.u32[0]
//! dst0.u32[1] = ~x0.u32[1]
//! dst1.u32[0] = ~x1.u32[0]
//! dst1.u32[1] = ~x1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Neg_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = ~x0;
  dst1 = ~x1;
#else
  dst0.u32Lo = ~x0.u32Lo;
  dst0.u32Hi = ~x0.u32Hi;
  dst1.u32Lo = ~x1.u32Lo;
  dst1.u32Hi = ~x1.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Scalar add.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! @endverbatim
static FOG_INLINE void p64Add(
  p64& dst0, const p64& x0, const p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) + _FOG_FACE_U64(y0);
}

//! @brief Scalar add (2x).
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0]
//! dst0.u32[1] = x0.u32[1] + y0.u32[1]
//! dst1.u32[0] = x1.u32[0] + y1.u32[0]
//! dst1.u32[1] = x1.u32[1] + y1.u32[1]
//! @endverbatim
static FOG_INLINE void p64Add_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) + _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) + _FOG_FACE_U64(y1);
}

//! @brief Scalar subtract.
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] - y0.u64[0]
//! @endverbatim
static FOG_INLINE void p64Sub(
  p64& dst0, const p64& x0, const p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) - _FOG_FACE_U64(y0);
}

//! @brief Scalar subtract (2x).
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] - y0.u64[0]
//! dst1.u64[0] = x1.u64[0] - y1.u64[0]
//! @endverbatim
static FOG_INLINE void p64Sub_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) - _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) - _FOG_FACE_U64(y1);
}

//! @brief Scalar multiply.
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] * y0.u64[0]
//! @endverbatim
static FOG_INLINE void p64Mul(
  p64& dst0, const p64& x0, const p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) * _FOG_FACE_U64(y0);
}

//! @brief Scalar multiply (2x).
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] * y0.u64[0]
//! dst1.u64[0] = x1.u64[0] * y1.u64[0]
//! @endverbatim
static FOG_INLINE void p64Mul_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) * _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) * _FOG_FACE_U64(y1);
}

//! @brief Scalar divide.
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] / y0.u64[0]
//! @endverbatim
static FOG_INLINE void p64Div(
  p64& dst0, const p64& x0, const p64& y0)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) / _FOG_FACE_U64(y0);
}

//! @brief Scalar divide (2x).
//!
//! @verbatim
//! dst0.u64[0] = x0.u64[0] / y0.u64[0]
//! dst1.u64[0] = x1.u64[0] / y1.u64[0]
//! @endverbatim
static FOG_INLINE void p64Div_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  _FOG_FACE_U64(dst0) = _FOG_FACE_U64(x0) / _FOG_FACE_U64(y0);
  _FOG_FACE_U64(dst1) = _FOG_FACE_U64(x1) / _FOG_FACE_U64(y1);
}

// ============================================================================
// [Fog::Face - P64 - Cvt]
// ============================================================================

static FOG_INLINE void p64Cvt256SBWFrom255SBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 + (x0 > 127);
#else
  dst0.u32Lo = x0.u32Lo + (x0.u32Lo > 127);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Cvt256PBWFrom255PBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 + ((x0 >> 7) & FOG_UINT64_C(0x0001000100010001));
#else
  dst0.u32Lo = x0.u32Lo + ((x0.u32Lo >> 7) & 0x00010001U);
  dst0.u32Hi = x0.u32Hi + ((x0.u32Hi >> 7) & 0x00010001U);
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Cvt256PBWFrom255PBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Cvt256PBWFrom255PBW(dst0, x0);
  p64Cvt256PBWFrom255PBW(dst1, x1);
}

static FOG_INLINE void p64Cvt255SBWFrom256SBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 - (x0 > 127);
#else
  dst0.u32Lo = x0.u32Lo - (x0.u32Lo > 127);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Cvt255PBWFrom256PBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 - ((x0 >> 7) & FOG_UINT64_C(0x0001000100010001));
#else
  dst0.u32Lo = x0.u32Lo - ((x0.u32Lo >> 7) * 0x00010001U);
  dst0.u32Hi = x0.u32Hi - ((x0.u32Hi >> 7) * 0x00010001U);
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Cvt255PBWFrom256PBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Cvt255PBWFrom256PBW(dst0, x0);
  p64Cvt255PBWFrom256PBW(dst1, x1);
}

// ============================================================================
// [Fog::Face - P64 - Extend]
// ============================================================================

//! @brief Extend the LO byte in @c x0 and copy result into @c dst0.
//!
//! @verbatim
//! dst0.u8[0] = x0.u8[0]
//! dst0.u8[1] = x0.u8[0]
//! dst0.u8[2] = x0.u8[0]
//! dst0.u8[3] = x0.u8[0]
//! dst0.u8[4] = x0.u8[0]
//! dst0.u8[5] = x0.u8[0]
//! dst0.u8[6] = x0.u8[0]
//! dst0.u8[7] = x0.u8[0]
//! @endverbatim
static FOG_INLINE void p64ExtendPBBFromSBB(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = x0 * FOG_UINT64_C(0x0101010101010101);
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 <<  8);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_FACE_HAS_FAST_MUL

#else

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0.u32Lo = x0.u32Lo * 0x01010101U;
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Lo = _FOG_FACE_COMBINE_2(dst0.u32Lo, dst0.u32Lo <<  8);
  dst0.u32Lo = _FOG_FACE_COMBINE_2(dst0.u32Lo, dst0.u32Lo << 16);
  dst0.u32Lo = _FOG_FACE_COMBINE_2(dst0.u32Lo, dst0.u32Lo << 32);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_FACE_HAS_FAST_MUL

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Extend src0.B0 to dst0, src0.B1 must be zero before call.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst0.u16[2] = x0.u16[0]
//! dst0.u16[3] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExtendPBWFromSBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = x0 * FOG_UINT64_C(0x0001000100010001);
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_FACE_HAS_FAST_MUL

#else

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0.u32Lo = x0.u32Lo * FOG_UINT64_C(0x00010001);
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo, x0.u32Lo << 16);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_FACE_HAS_FAST_MUL

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Extend src0.B0 to dst0, src0.B1 must be zero before call.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst0.u16[2] = x0.u16[0]
//! dst0.u16[3] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExtendPWWFromSWW(
  p64& dst0, const p64& x0)
{
  p64ExtendPBWFromSBW(dst0, x0);
}
// ============================================================================
// [Fog::Face - P64 - Expand]
// ============================================================================

//! @brief Expand scalar byte into p64.
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW0(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = (x0 & FOG_UINT64_C(0xFF)) * FOG_UINT64_C(0x0001000100010001);
#else
  dst0 = x0;
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_FACE_HAS_FAST_MUL

#else

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0.u32Lo = (x0.u32Lo & 0xFF) * 0x00010001U;
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0xFF, x0.u32Lo << 16);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_FACE_HAS_FAST_MUL

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Expand src0.B0 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[0]
//! dst0.u16[1] = x0.u16[0]
//! dst1.u16[0] = x1.u16[0]
//! dst1.u16[1] = x1.u16[0]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW0_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64ExpandPBWFromPBW0(dst0, x0);
  p64ExpandPBWFromPBW0(dst1, x1);
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW1(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0 = ((x0 >> 16) & FOG_UINT64_C(0xFF)) * FOG_UINT64_C(0x0001000100010001);
#else
  dst0 = x0 & FOG_UINT64_C(0x0000000000FF0000);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 >> 16);
  dst0 = _FOG_FACE_COMBINE_2(dst0, dst0 << 32);
#endif // FOG_FACE_HAS_FAST_MUL

#else

#if defined(FOG_FACE_HAS_FAST_MUL)
  dst0.u32Lo = (x0.u32Lo >> 16) * 0x00010001U;
  dst0.u32Hi = dst0.u32Lo;
#else
  dst0.u32Lo = _FOG_FACE_COMBINE_2(x0.u32Lo & 0x00FF0000U, x0.u32Lo >> 16);
  dst0.u32Hi = dst0.u32Lo;
#endif // FOG_FACE_HAS_FAST_MUL

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Expand src0.B1 to [dst0.B0, dst0.B1] and [dst1.B0, dst1.B1].
//!
//! @verbatim
//! dst0.u16[0] = x0.u16[1]
//! dst0.u16[1] = x0.u16[1]
//! dst1.u16[0] = x1.u16[1]
//! dst1.u16[1] = x1.u16[1]
//! @endverbatim
static FOG_INLINE void p64ExpandPBWFromPBW1_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64ExpandPBWFromPBW1(dst0, x0);
  p64ExpandPBWFromPBW1(dst1, x1);
}

static FOG_INLINE void p64ExpandPWWFromPWW0(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 & FOG_UINT64_C(0xFFFF);
#else
  dst0.u32Lo = x0.u32Lo & 0xFFFF;
  dst0.u32Hi = 0;
#endif // FOG_FACE_P64_IS_TYPE

  p64ExtendPWWFromSWW(dst0, dst0);
}

static FOG_INLINE void p64ExpandPWWFromPWW1(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (x0 >> 16) & FOG_UINT64_C(0xFFFF);
#else
  dst0.u32Lo = x0.u32Lo >> 16;
  dst0.u32Hi = 0;
#endif // FOG_FACE_P64_IS_TYPE

  p64ExtendPWWFromSWW(dst0, dst0);
}

static FOG_INLINE void p64ExpandPWWFromPWW2(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (x0 >> 32) & FOG_UINT64_C(0xFFFF);
#else
  dst0.u32Lo = x0.u32Hi;
  dst0.u32Hi = 0;
#endif // FOG_FACE_P64_IS_TYPE

  p64ExtendPWWFromSWW(dst0, dst0);
}

static FOG_INLINE void p64ExpandPWWFromPWW3(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 >> 48;
#else
  dst0.u32Lo = x0.u32Hi >> 16;
  dst0.u32Hi = 0;
#endif // FOG_FACE_P64_IS_TYPE

  p64ExtendPWWFromSWW(dst0, dst0);
}

// ============================================================================
// [Fog::Face - P64 - LShift / RShift]
// ============================================================================

static FOG_INLINE void p64LShift(
  p64& dst0, const p64& x0, uint32_t s0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 << s0;
#else
  dst0.u32Lo = x0.u32Lo << s0;
  dst0.u32Hi = x0.u32Hi << s0;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64LShift_2x(
  p64& dst0, const p64& x0, uint32_t s0,
  p64& dst1, const p64& x1, uint32_t s1)
{
  p64LShift(dst0, x0, s0);
  p64LShift(dst1, x1, s1);
}

static FOG_INLINE void p64LShiftTruncatePBW(
  p64& dst0, const p64& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (x0 << s0) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo << s0) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Hi << s0) & 0x00FF00FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64LShiftTruncatePBW_2x(
  p64& dst0, const p64& x0, uint32_t s0,
  p64& dst1, const p64& x1, uint32_t s1)
{
  p64LShiftTruncatePBW(dst0, x0, s0);
  p64LShiftTruncatePBW(dst1, x1, s1);
}

static FOG_INLINE void p64LShiftBy1(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

#if defined(FOG_FACE_HAS_FAST_ADD)
  dst0 = x0 + x0;
#else
  dst0 = x0 << 1;
#endif // FOG_FACE_HAS_FAST_ADD

#else

#if defined(FOG_FACE_HAS_FAST_ADD)
  dst0.u32Lo = x0.u32Lo + x0.u32Lo;
  dst0.u32Hi = x0.u32Hi + x0.u32Hi;
#else
  dst0.u32Lo = x0.u32Lo << 1;
  dst0.u32Hi = x0.u32Hi << 1;
#endif // FOG_FACE_HAS_FAST_ADD

#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64LShiftBy1_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64LShiftBy1(dst0, x0);
  p64LShiftBy1(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW0(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x00000000000000FF));
#else
  dst0.u32Lo = x0.u32Lo + (x0.u32Lo & 0x000000FFU);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64LShiftBy1PBW0_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64LShiftBy1PBW0(dst0, x0);
  p64LShiftBy1PBW0(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW1(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x0000000000FF0000));
#else
  dst0.u32Lo = x0.u32Lo + (x0.u32Lo & 0x00FF0000U);
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64LShiftBy1PBW1_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64LShiftBy1PBW1(dst0, x0);
  p64LShiftBy1PBW1(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW2(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x000000FF00000000));
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi + (x0.u32Hi & 0x000000FFU);
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64LShiftBy1PBW2_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64LShiftBy1PBW2(dst0, x0);
  p64LShiftBy1PBW2(dst1, x1);
}

static FOG_INLINE void p64LShiftBy1PBW3(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 + (x0 & FOG_UINT64_C(0x00FF000000000000));
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi + (x0.u32Hi & 0x00FF0000U);
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64LShiftBy1PBW3_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64LShiftBy1PBW3(dst0, x0);
  p64LShiftBy1PBW3(dst1, x1);
}

static FOG_INLINE void p64RShift(
  p64& dst0, const p64& x0, uint32_t s0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 >> s0;
#else
  dst0.u32Lo = x0.u32Lo >> s0;
  dst0.u32Hi = x0.u32Hi >> s0;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64RShift_2x(
  p64& dst0, const p64& x0, uint32_t s0,
  p64& dst1, const p64& x1, uint32_t s1)
{
  p64RShift(dst0, x0, s0);
  p64RShift(dst1, x1, s1);
}

static FOG_INLINE void p64RShiftTruncatePBW(
  p64& dst0, const p64& x0, uint32_t s0)
{
  FOG_ASSERT(s0 <= 8);

#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (x0 >> s0) & FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo >> s0) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Hi >> s0) & 0x00FF00FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64RShiftTruncatePBW_2x(
  p64& dst0, const p64& x0, uint32_t s0,
  p64& dst1, const p64& x1, uint32_t s1)
{
  p64RShiftTruncatePBW(dst0, x0, s0);
  p64RShiftTruncatePBW(dst1, x1, s1);
}

// ============================================================================
// [Fog::Face - P64 - Negate255/256]
// ============================================================================

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate255SBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 ^ FOG_UINT64_C(0xFF);
#else
  dst0.u32Lo = x0.u32Lo ^ 0xFFU;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst1.u16[0] = 255 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate255SBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate255SBW(dst0, x0);
  p64Negate255SBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate256SBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = FOG_UINT64_C(256) - x0;
#else
  dst0.u32Lo = 256U - x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst1.u16[0] = 256 - x1.u16[0]
//! @endverbatim
static FOG_INLINE void p64Negate256SBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate256SBW(dst0, x0);
  p64Negate256SBW(dst1, x1);
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! @endverbatim
static FOG_INLINE void p64Negate255PBB(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = ~x0;
#else
  dst0.u32Lo = ~x0.u32Lo;
  dst0.u32Hi = ~x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @verbatim
//! dst0.u8[0] = 255 - x0.u8[0]
//! dst0.u8[1] = 255 - x0.u8[1]
//! dst0.u8[2] = 255 - x0.u8[2]
//! dst0.u8[3] = 255 - x0.u8[3]
//! dst1.u8[0] = 255 - x1.u8[0]
//! dst1.u8[1] = 255 - x1.u8[1]
//! dst1.u8[2] = 255 - x1.u8[2]
//! dst1.u8[3] = 255 - x1.u8[3]
//! @endverbatim
static FOG_INLINE void p64Negate255PBB_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate255PBB(dst0, x0);
  p64Negate255PBB(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate255PBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 ^ FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = x0.u32Lo ^ 0x00FF00FFU;
  dst0.u32Hi = x0.u32Hi ^ 0x00FF00FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @verbatim
//! dst0.u16[0] = 255 - x0.u16[0]
//! dst0.u16[1] = 255 - x0.u16[1]
//! dst1.u16[0] = 255 - x1.u16[0]
//! dst1.u16[1] = 255 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate255PBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate255PBW(dst0, x0);
  p64Negate255PBW(dst1, x1);
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate256PBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = FOG_UINT64_C(0x0100010001000100) - x0;
#else
  dst0.u32Lo = 0x01000100U - x0.u32Lo;
  dst0.u32Hi = 0x01000100U - x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @verbatim
//! dst0.u16[0] = 256 - x0.u16[0]
//! dst0.u16[1] = 256 - x0.u16[1]
//! dst1.u16[0] = 256 - x1.u16[0]
//! dst1.u16[1] = 256 - x1.u16[1]
//! @endverbatim
static FOG_INLINE void p64Negate256PBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate256PBW(dst0, x0);
  p64Negate256PBW(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW0(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 ^ FOG_UINT64_C(0x00000000000000FF);
#else
  dst0.u32Lo = x0.u32Lo ^ 0x000000FFU;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Negate255PBW0_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate255PBW0(dst0, x0);
  p64Negate255PBW0(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW1(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 ^ FOG_UINT64_C(0x0000000000FF0000);
#else
  dst0.u32Lo = x0.u32Lo ^ 0x00FF0000U;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Negate255PBW1_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate255PBW1(dst0, x0);
  p64Negate255PBW1(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW2(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 ^ FOG_UINT64_C(0x000000FF00000000);
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi ^ 0x000000FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Negate255PBW2_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate255PBW2(dst0, x0);
  p64Negate255PBW2(dst1, x1);
}

static FOG_INLINE void p64Negate255PBW3(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 ^ FOG_UINT64_C(0x00FF000000000000);
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi ^ 0x00FF0000U;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Negate255PBW3_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Negate255PBW3(dst0, x0);
  p64Negate255PBW3(dst1, x1);
}

// ============================================================================
// [Fog::Face - P64 - Min]
// ============================================================================

//! @brief Take smaller value from @a u0 and @a u and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! @endverbatim
static FOG_INLINE void p64MinPBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t x0_0 = x0 & FOG_UINT64_C(0x00000000000000FF);
  uint64_t x0_1 = x0 & FOG_UINT64_C(0x0000000000FF0000);
  uint64_t x0_2 = x0 & FOG_UINT64_C(0x000000FF00000000);
  uint64_t x0_3 = x0 & FOG_UINT64_C(0x00FF000000000000);
  uint64_t t0 = u0;

  if (x0_0 > t0) x0_0 = t0;
  t0 <<= 16;
  if (x0_1 > t0) x0_1 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);
  if (x0_2 > t0) x0_2 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_2);
  if (x0_3 > t0) x0_3 = t0;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_3);

  dst0 = x0_0;

#else

  uint32_t x0_0 = x0.u32Lo & 0x000000FFU;
  uint32_t x0_1 = x0.u32Lo & 0x00FF0000U;
  uint32_t x0_2 = x0.u32Hi & 0x000000FFU;
  uint32_t x0_3 = x0.u32Hi & 0x00FF0000U;

  uint32_t t0_0 = u0.u32Lo;
  uint32_t t0_1 = u0.u32Lo << 16;

  if (x0_0 > t0_0) x0_0 = t0_0;
  if (x0_1 > t0_1) x0_1 = t0_1;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);

  if (x0_2 > t0_0) x0_2 = t0_0;
  if (x0_3 > t0_1) x0_3 = t0_1;
  x0_2 = _FOG_FACE_COMBINE_2(x0_2, x0_3);

  dst0.u32Lo = x0_0;
  dst0.u32Hi = x0_2;

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Take smaller value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, u0)
//! dst1 = min(x1, u1)
//! @endverbatim
static FOG_INLINE void p64MinPBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64MinPBW_SBW(dst0, x0, u0);
  p64MinPBW_SBW(dst1, x1, u1);
}

//! @brief Take smaller value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! @endverbatim
static FOG_INLINE void p64MinPBW(
  p64& dst0, const p64& x0, const p64& y0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t t0_0;
  uint64_t t0_1;
  uint64_t t0_2;

  t0_0 = x0 & FOG_UINT64_C(0x00000000000000FF);
  t0_1 = y0 & FOG_UINT64_C(0x00000000000000FF);
  if (t0_0 > t0_1) t0_0 = t0_1;

  t0_1 = x0 & FOG_UINT64_C(0x0000000000FF0000);
  t0_2 = y0 & FOG_UINT64_C(0x0000000000FF0000);
  if (t0_1 > t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0x000000FF00000000);
  t0_2 = y0 & FOG_UINT64_C(0x000000FF00000000);
  if (t0_1 > t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0x00FF000000000000);
  t0_2 = y0 & FOG_UINT64_C(0x00FF000000000000);
  if (t0_1 > t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);
  dst0 = t0_0;

#else

  uint32_t t0_0;
  uint32_t t0_1;
  uint32_t t0_2;

  t0_0 = x0.u32Lo & 0x000000FFU;
  t0_1 = y0.u32Lo & 0x000000FFU;
  if (t0_0 > t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Lo & 0x00FF0000U;
  t0_2 = y0.u32Lo & 0x00FF0000U;
  if (t0_1 > t0_2) t0_1 = t0_2;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_0 = x0.u32Hi & 0x000000FFU;
  t0_1 = y0.u32Hi & 0x000000FFU;
  if (t0_0 > t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Hi & 0x00FF0000U;
  t0_2 = y0.u32Hi & 0x00FF0000U;
  if (t0_1 > t0_2) t0_1 = t0_2;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0_0, t0_1);

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Take smaller value from @a x0/y0 and @c x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = min(x0, y0)
//! dst1 = min(x1, y1)
//! @endverbatim
static FOG_INLINE void p64MinPBW_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  p64MinPBW(dst0, x0, y0);
  p64MinPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P64 - Max]
// ============================================================================

//! @brief Take larger value from @a x0/u0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! @endverbatim
static FOG_INLINE void p64MaxPBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t x0_0 = x0 & FOG_UINT64_C(0x00000000000000FF);
  uint64_t x0_1 = x0 & FOG_UINT64_C(0x0000000000FF0000);
  uint64_t x0_2 = x0 & FOG_UINT64_C(0x000000FF00000000);
  uint64_t x0_3 = x0 & FOG_UINT64_C(0x00FF000000000000);
  uint64_t t0 = u0;

  if (x0_0 < t0) x0_0 = t0;
  t0 <<= 16;
  if (x0_1 < t0) x0_1 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);
  if (x0_2 < t0) x0_2 = t0;
  t0 <<= 16;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_2);
  if (x0_3 < t0) x0_3 = t0;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_3);

  dst0 = x0_0;

#else

  uint32_t x0_0 = x0.u32Lo & 0x000000FFU;
  uint32_t x0_1 = x0.u32Lo & 0x00FF0000U;
  uint32_t x0_2 = x0.u32Hi & 0x000000FFU;
  uint32_t x0_3 = x0.u32Hi & 0x00FF0000U;

  uint32_t t0_0 = u0.u32Lo;
  uint32_t t0_1 = u0.u32Lo << 16;

  if (x0_0 < t0_0) x0_0 = t0_0;
  if (x0_1 < t0_1) x0_1 = t0_1;
  x0_0 = _FOG_FACE_COMBINE_2(x0_0, x0_1);

  if (x0_2 < t0_0) x0_2 = t0_0;
  if (x0_3 < t0_1) x0_3 = t0_1;
  x0_2 = _FOG_FACE_COMBINE_2(x0_2, x0_3);

  dst0.u32Lo = x0_0;
  dst0.u32Hi = x0_2;

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Take larger value from @a x0/u0 and @a x1/u1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, u0)
//! dst1 = max(x1, u1)
//! @endverbatim
static FOG_INLINE void p64MaxPBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64MaxPBW_SBW(dst0, x0, u0);
  p64MaxPBW_SBW(dst1, x1, u1);
}

//! @brief Take larger value from @a x0/y0 and store it to @a dst0.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! @endverbatim
static FOG_INLINE void p64MaxPBW(
  p64& dst0, const p64& x0, const p64& y0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t t0_0;
  uint64_t t0_1;
  uint64_t t0_2;

  t0_0 = x0 & FOG_UINT64_C(0x00000000000000FF);
  t0_1 = y0 & FOG_UINT64_C(0x00000000000000FF);
  if (t0_0 < t0_1) t0_0 = t0_1;

  t0_1 = x0 & FOG_UINT64_C(0x0000000000FF0000);
  t0_2 = y0 & FOG_UINT64_C(0x0000000000FF0000);
  if (t0_1 < t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0x000000FF00000000);
  t0_2 = y0 & FOG_UINT64_C(0x000000FF00000000);
  if (t0_1 < t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_1 = x0 & FOG_UINT64_C(0x00FF000000000000);
  t0_2 = y0 & FOG_UINT64_C(0x00FF000000000000);
  if (t0_1 < t0_2) t0_1 = t0_2;

  t0_0 = _FOG_FACE_COMBINE_2(t0_0, t0_1);
  dst0 = t0_0;

#else

  uint32_t t0_0;
  uint32_t t0_1;
  uint32_t t0_2;

  t0_0 = x0.u32Lo & 0x000000FFU;
  t0_1 = y0.u32Lo & 0x000000FFU;
  if (t0_0 < t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Lo & 0x00FF0000U;
  t0_2 = y0.u32Lo & 0x00FF0000U;
  if (t0_1 < t0_2) t0_1 = t0_2;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0_0, t0_1);

  t0_0 = x0.u32Hi & 0x000000FFU;
  t0_1 = y0.u32Hi & 0x000000FFU;
  if (t0_0 < t0_1) t0_0 = t0_1;

  t0_1 = x0.u32Hi & 0x00FF0000U;
  t0_2 = y0.u32Hi & 0x00FF0000U;
  if (t0_1 < t0_2) t0_1 = t0_2;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0_0, t0_1);

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Take larger value from @a x0/y0 and @a x1/y1 and store it to @a dst0/dst1.
//!
//! @verbatim
//! dst0 = max(x0, y0)
//! dst1 = max(x1, y1)
//! @endverbatim
static FOG_INLINE void p64MaxPBW_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  p64MaxPBW(dst0, x0, y0);
  p64MaxPBW(dst1, x1, y1);
}

// ============================================================================
// [Fog::Face - P64 - Saturate]
// ============================================================================

static FOG_INLINE void p64Saturate255SBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

#if defined(FOG_FACE_HAS_CMOV)
  dst0 = x0;
  if (dst0 > 0xFF) dst0 = 0xFF;
#else
  dst0 = (x0 | (FOG_UINT64_C(0x0100) - (x0 >> 8))) & 0xFFU;
#endif

#else

#if defined(FOG_FACE_HAS_CMOV)
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;
  if (dst0.u32Lo > 0xFF) dst0.u32Lo = 0xFF;
#else
  dst0.u32Lo = (x0.u32Lo | (0x0100U - (x0.u32Lo >> 8))) & 0xFFU;
  dst0.u32Hi = x0.u32Hi;
#endif

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! @endverbatim
static FOG_INLINE void p64Saturate255PBW(
  p64& dst0, const p64& x0)
{
  // NOTE: The following code will work (and is more precise) too:
  //
  // dst0  = x0 | (FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x0001000100010001)));
  // dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  //
  // The reason why the 0x00FF00FF is also used to mask (x0 >> 8) is that the
  // mask can be stored in register so it will be reused by another computation.

#if defined(FOG_FACE_P64_IS_TYPE)

  dst0  = x0 | (FOG_UINT64_C(0x0100010001000100) - ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)));
  dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

#else

  dst0.u32Lo = (x0.u32Lo | (0x01000100U - ((x0.u32Lo >> 8) & 0x00FF00FFU))) & 0x00FF00FFU;
  dst0.u32Hi = (x0.u32Hi | (0x01000100U - ((x0.u32Hi >> 8) & 0x00FF00FFU))) & 0x00FF00FFU;

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 255)
//! dst1 = min(dst1, 255)
//! @endverbatim
static FOG_INLINE void p64Saturate255PBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Saturate255PBW(dst0, x0);
  p64Saturate255PBW(dst1, x1);
}

//! @brief Saturate @a dst0. Can be used after two BYTEs were added and
//! resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! @endverbatim
static FOG_INLINE void p64Saturate511PBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  dst0  = x0 | (FOG_UINT64_C(0x0200020002000200) - ((x0 >> 9) & FOG_UINT64_C(0x0001000100010001)));
  dst0 &= FOG_UINT64_C(0x01FF01FF01FF01FF);

#else

  dst0.u32Lo = (x0.u32Lo | (0x02000200U - ((x0.u32Lo >> 9) & 0x00010001U))) & 0x01FF01FFU;
  dst0.u32Hi = (x0.u32Hi | (0x02000200U - ((x0.u32Hi >> 9) & 0x00010001U))) & 0x01FF01FFU;

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Saturate @a dst0/dst1. Can be used after two BYTEs were added
//! and resulting value may be larger than BYTE.
//!
//! @verbatim
//! dst0 = min(dst0, 511)
//! dst1 = min(dst1, 511)
//! @endverbatim
static FOG_INLINE void p64Saturate511PBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1)
{
  p64Saturate511PBW(dst0, x0);
  p64Saturate511PBW(dst1, x1);
}

// ============================================================================
// [Fog::Face - P64 - Add / Addus]
// ============================================================================

static FOG_INLINE void p64AddPBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
  p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Add(dst0, x0, t0);
}

static FOG_INLINE void p64AddPBW_SBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1, const p64& u01)
{
  p64 t01;
  p64ExtendPBWFromSBW(t01, u01);
  p64Add_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p64AddPBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Add_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p64Addus255SBW(
  p64& dst0, const p64& x0, const p64& y0)
{
  p64Add(dst0, x0, y0);
  p64Saturate255SBW(dst0, dst0);
}

static FOG_INLINE void p64Addus255PBB(
  p64& dst0, const p64& x0, const p64& y0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t t0 = (x0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  uint64_t t1 = (x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 += (y0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 += (y0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF);

  t0 |= FOG_UINT64_C(0x0100010001000100) - ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));
  t1 |= FOG_UINT64_C(0x0100010001000100) - ((t1 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF));

  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1 << 8);

#else

  uint32_t t0;
  uint32_t t1;

  t0 = ((x0.u32Lo     ) & 0x00FF00FFU) + ((y0.u32Lo     ) & 0x00FF00FFU);
  t1 = ((x0.u32Lo >> 8) & 0x00FF00FFU) + ((y0.u32Lo >> 8) & 0x00FF00FFU);

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1 << 8);

  t0 = ((x0.u32Hi     ) & 0x00FF00FFU) + ((y0.u32Hi     ) & 0x00FF00FFU);
  t1 = ((x0.u32Hi >> 8) & 0x00FF00FFU) + ((y0.u32Hi >> 8) & 0x00FF00FFU);

  t0 |= 0x01000100U - ((t0 >> 8) & 0x00FF00FFU);
  t1 |= 0x01000100U - ((t1 >> 8) & 0x00FF00FFU);

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1 << 8);

#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Addus255PBW(
  p64& dst0, const p64& x0, const p64& y0)
{
  p64Add(dst0, x0, y0);
  p64Saturate255PBW(dst0, dst0);
}

static FOG_INLINE void p64Addus255PBW_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  p64Add_2x(dst0, x0, y0, dst1, x1, y1);
  p64Saturate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p64Addus255PBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
  p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Addus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p64Addus255PBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Addus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}

// ============================================================================
// [Fog::Face - P64 - Sub / Subus]
// ============================================================================

static FOG_INLINE void p64SubPBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
  p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Sub(dst0, x0, t0);
}

static FOG_INLINE void p64SubPBW_SBW_2x(
  p64& dst0, const p64& x0,
  p64& dst1, const p64& x1, const p64& u01)
{
  p64 t01;
  p64ExtendPBWFromSBW(t01, u01);
  p64Sub_2x(dst0, x0, t01, dst1, x1, t01);
}

static FOG_INLINE void p64SubPBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Sub_2x(dst0, x0, t0, dst1, x1, t1);
}

static FOG_INLINE void p64Subus255PBW(
  p64& dst0, const p64& x0, const p64& y0)
{
  p64Negate255PBW(dst0, x0);
  p64Add(dst0, dst0, y0);
  p64Saturate255PBW(dst0, dst0);
  p64Negate255PBW(dst0, dst0);
}

static FOG_INLINE void p64Subus255PBW_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  p64Negate255PBW_2x(dst0, x0, dst1, x1);
  p64Add_2x(dst0, dst0, y0, dst1, dst1, y1);
  p64Saturate255PBW_2x(dst0, dst0, dst1, dst1);
  p64Negate255PBW_2x(dst0, dst0, dst1, dst1);
}

static FOG_INLINE void p64Subus255SBW(
  p64& dst0, const p64& x0, const p64& y0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  dst0 = x0 - y0;

#if defined(FOG_FACE_HAS_CMOV)
  if ((int64_t)dst0 < 0) dst0 = 0;
#else
  dst0 &= (dst0 >> 56) ^ FOG_UINT64_C(0xFF);
#endif // FOG_FACE_HAS_CMOV

#else

  dst0.u32Lo = x0.u32Lo - y0.u32Lo;
  dst0.u32Hi = x0.u32Hi;

#if defined(FOG_FACE_HAS_CMOV)
  if ((int32_t)dst0.u32Lo < 0) dst0.u32Lo = 0;
#else
  dst0.u32Lo &= (dst0.u32Lo >> 24) ^ 0xFFU;
#endif // FOG_FACE_HAS_CMOV

#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64Subus255PBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
  p64 t0;
  p64ExtendPBWFromSBW(t0, u0);
  p64Subus255PBW(dst0, x0, t0);
}

static FOG_INLINE void p64Subus255PBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64 t0, t1;
  p64ExtendPBWFromSBW(t0, u0);
  p64ExtendPBWFromSBW(t1, u1);
  p64Subus255PBW_2x(dst0, x0, t0, dst1, x1, t1);
}
















// ============================================================================
// [Fog::Face - P64 - AddSub / AddSubus]
// ============================================================================

//! @brief Scalar add and subtract.
//!
//! @verbatim
//! dst0.u32[0] = x0.u32[0] + y0.u32[0] - z0.u32[0]
//! @endverbatim
static FOG_INLINE void p64AddSub(
  p64& dst0, const p64& x0, const p64& y0, const p64& z0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 + y0 - z0;
#else
  dst0.u32Lo = x0.u32Lo + y0.u32Lo - z0.u32Lo;
  dst0.u32Hi = x0.u32Hi + y0.u32Hi - z0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

static FOG_INLINE void p64AddSub_2x(
  p64& dst0, const p64& x0, const p64& y0, const p64& z0,
  p64& dst1, const p64& x1, const p64& y1, const p64& z1)
{
  p64AddSub(dst0, x0, y0, z0);
  p64AddSub(dst1, x1, y1, z1);
}

//! @verbatim
//! dst0 = saturate255(x0 + y0 - z0)
//! @endverbatim
static FOG_INLINE void p64AddSubus255PBW(
  p64& dst0, const p64& x0, const p64& y0, const p64& z0)
{
  p64 t0;

  p64Add(t0, x0, y0);
  p64Xor(t0, t0, p64FromU64(FOG_UINT64_C(0x01FF01FF01FF01FF)));
  p64Add(t0, t0, z0);
  p64Saturate511PBW(t0, t0);
  p64Xor(t0, t0, p64FromU64(FOG_UINT64_C(0x01FF01FF01FF01FF)));
  p64Saturate255PBW(t0, t0);
}

//! @verbatim
//! dst0 = saturate255(x0 + y0 - z0)
//! dst1 = saturate255(x1 + y1 - z1)
//! @endverbatim
static FOG_INLINE void p64AddSubus255PBW_2x(
  p64& dst0, const p64& x0, const p64& y0, const p64& z0,
  p64& dst1, const p64& x1, const p64& y1, const p64& z1)
{
  p64AddSubus255PBW(dst0, x0, y0, z0);
  p64AddSubus255PBW(dst1, x1, y1, z1);
}

// ============================================================================
// [Fog::Face - P64 - Div]
// ============================================================================

//! @brief Scalar divide by 255.
//!
//! @verbatim
//! dst0 = (x0 / 255).
//! @endverbatim
static FOG_INLINE void p64Div255SBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = ((x0 << 8U) + x0 + 256U) >> 16U;
#else
  dst0.u32Lo = ((x0.u32Lo << 8U) + x0.u32Lo + 256U) >> 16U;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Scalar Divide by 256.
//!
//! @verbatim
//! dst0 = (x0 / 256).
//! @endverbatim
static FOG_INLINE void p64Div256SBW(
  p64& dst0, const p64& x0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 >> 8;
#else
  dst0.u32Lo = x0.u32Lo >> 8;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

// ============================================================================
// [Fog::Face - P64 - MulDiv]
// ============================================================================

//! @brief Scalar Multiply and divide by 255.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] * u0.u32[0]) / 255
//! dst0.u32[1] = (x0.u32[1])
//! @endverbatim
static FOG_INLINE void p64MulDiv255SBW(
  p64 &dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 * u0;
  dst0 = (dst0 + (dst0 >> 8) + 0x80U) >> 8;
#else
  dst0.u32Lo = x0.u32Lo;
  dst0.u32Hi = x0.u32Hi;

  dst0.u32Lo = dst0.u32Lo * u0.u32Lo;
  dst0.u32Lo = (dst0.u32Lo + (dst0.u32Lo >> 8) + 0x80U) >> 8;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Scalar Multiply and divide by 256.
//!
//! @verbatim
//! dst0.u32[0] = (x0.u32[0] * u0.u32[0]) / 256
//! dst0.u32[1] = (x0.u32[1])
//! @endverbatim
static FOG_INLINE void p64MulDiv256SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (x0 * u0) >> 8;
#else
  dst0.u32Lo = (x0.u32Lo * u0.u32Lo) >> 8;
  dst0.u32Hi = x0.u32Hi;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 255
//! dst0.u8[3] = (x0.u8[3] * u0.u64[0]) / 255
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 255
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 255
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 255
//! dst0.u8[7] = (x0.u8[7] * u0.u64[0]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t t0 = ((x0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;

  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t1 = (t1 + ((t1 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080));

  t0 = t0 & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 = t1 & FOG_UINT64_C(0xFF00FF00FF00FF00);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);

#else

  uint32_t t0;
  uint32_t t1;

  t0 = ((x0.u32Lo     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Lo >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080)     ) & 0xFF00FF00;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1);

  t0 = ((x0.u32Hi     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Hi >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = ((t0 + ((t0 >> 8) & 0x00FF00FF) + 0x00800080) >> 8) & 0x00FF00FF;
  t1 = ((t1 + ((t1 >> 8) & 0x00FF00FF) + 0x00800080)     ) & 0xFF00FF00;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1);

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 256
//! dst0.u8[3] = (x0.u8[3] * u0.u64[0]) / 256
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 256
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 256
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 256
//! dst0.u8[7] = (x0.u8[7] * u0.u64[0]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBB_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t t0 = ((x0     ) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = ((x0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;

  t0 = (t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF);
  t1 = (t1     ) & FOG_UINT64_C(0xFF00FF00FF00FF00);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1);

#else

  uint32_t t0;
  uint32_t t1;

  t0 = ((x0.u32Lo     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Lo >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = (t0 >> 8) & 0x00FF00FF;
  t1 = (t1     ) & 0xFF00FF00;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1);

  t0 = ((x0.u32Hi     ) & 0x00FF00FFU) * u0.u32Lo;
  t1 = ((x0.u32Hi >> 8) & 0x00FF00FFU) * u0.u32Lo;

  t0 = (t0 >> 8) & 0x00FF00FF;
  t1 = (t1     ) & 0xFF00FF00;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1);

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 255
//! dst0.u8[3] = 0x00
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 255
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 255
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 255
//! dst0.u8[7] = 0x00
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_SBW_Z654Z210(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t t0 = (x0 & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = (x0 & FOG_UINT64_C(0x0000FF000000FF00)) * u0;

  t0 = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080));
  t1 = (t1 + ((t1 >> 8) & FOG_UINT64_C(0x0000FF000000FF00)) + FOG_UINT64_C(0x0000800000008000));

  t0 &= FOG_UINT64_C(0xFF00FF00FF00FF00);
  t1 &= FOG_UINT64_C(0x00FF000000FF0000);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#else

  uint32_t t0;
  uint32_t t1;

  t0 = (x0.u32Lo & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Lo & 0x0000FF00U) * u0.u32Lo;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x0000FF00U) + 0x00008000U);

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

  t0 = (x0.u32Hi & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Hi & 0x0000FF00U) * u0.u32Lo;

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U);
  t1 = (t1 + ((t1 >> 8) & 0x0000FF00U) + 0x00008000U);

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 256
//! dst0.u8[1] = (x0.u8[2] * u0.u64[0]) / 256
//! dst0.u8[2] = (x0.u8[3] * u0.u64[0]) / 256
//! dst0.u8[3] = 0x00
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 256
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 256
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 256
//! dst0.u8[7] = 0x00
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBB_SBW_Z654Z210(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)

  uint64_t t0 = (x0 & FOG_UINT64_C(0x00FF00FF00FF00FF)) * u0;
  uint64_t t1 = (x0 & FOG_UINT64_C(0x0000FF000000FF00)) * u0;

  t0 = t0 & FOG_UINT64_C(0xFF00FF00FF00FF00);
  t1 = t1 & FOG_UINT64_C(0x00FF000000FF0000);

  dst0 = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#else

  uint32_t t0;
  uint32_t t1;

  t0 = (x0.u32Lo & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Lo & 0x0000FF00U) * u0.u32Lo;

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Lo = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

  t0 = (x0.u32Hi & 0x00FF00FFU) * u0.u32Lo;
  t1 = (x0.u32Hi & 0x0000FF00U) * u0.u32Lo;

  t0 &= 0xFF00FF00;
  t1 &= 0x00FF0000;

  dst0.u32Hi = _FOG_FACE_COMBINE_2(t0, t1) >> 8;

#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.u64[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * u0.u64[0]) / 255
//! dst0.u8[2] = (x0.u8[2] * u0.u64[0]) / 255
//! dst0.u8[3] = 0xFF
//! dst0.u8[4] = (x0.u8[4] * u0.u64[0]) / 255
//! dst0.u8[5] = (x0.u8[5] * u0.u64[0]) / 255
//! dst0.u8[6] = (x0.u8[6] * u0.u64[0]) / 255
//! dst0.u8[7] = 0xFF
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_SBW_F654F210(
  p64& dst0, const p64& x0, const p64& u0)
{
  p64MulDiv255PBB_SBW_F654F210(dst0, x0, u0);

#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 |= FOG_UINT64_C(0xFF000000FF000000);
#else
  dst0.u32Lo |= 0xFF000000U;
  dst0.u32Hi |= 0xFF000000U;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * u0.p64[0]) / 256
//! dst0.u8[1] = (x0.u8[1] * u0.p64[0]) / 256
//! dst0.u8[2] = (x0.u8[2] * u0.p64[0]) / 256
//! dst0.u8[3] = 0xFF
//! dst0.u8[4] = (x0.u8[4] * u0.p64[0]) / 256
//! dst0.u8[5] = (x0.u8[5] * u0.p64[0]) / 256
//! dst0.u8[6] = (x0.u8[6] * u0.p64[0]) / 256
//! dst0.u8[7] = 0xFF
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBB_SBW_F654F210(
  p64& dst0, const p64& x0, const p64& u0)
{
  p64MulDiv256PBB_SBW_F654F210(dst0, x0, u0);

#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 |= FOG_UINT64_C(0xFF000000FF000000);
#else
  dst0.u32Lo |= 0xFF000000U;
  dst0.u32Hi |= 0xFF000000U;
#endif // FOG_FACE_P64_IS_TYPE
}

/*

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = (x0.u8[3] * y0.u8[3]) / 255
//! dst0.u8[4] = (x0.u8[4] * y0.u8[4]) / 255
//! dst0.u8[5] = (x0.u8[5] * y0.u8[5]) / 255
//! dst0.u8[6] = (x0.u8[6] * y0.u8[6]) / 255
//! dst0.u8[7] = (x0.u8[7] * y0.u8[7]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB(
  p64& dst0, const p64& x0, const p64& y0)
{
}

//! @brief Packed multiply and divide by 255.
//!
//! @verbatim
//! dst0.u8[0] = (x0.u8[0] * y0.u8[0]) / 255
//! dst0.u8[1] = (x0.u8[1] * y0.u8[1]) / 255
//! dst0.u8[2] = (x0.u8[2] * y0.u8[2]) / 255
//! dst0.u8[3] = 0x00
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBB_Z654Z210(
  p64& dst0, const p64& x0, const p64& y0)
{
}

*/

//! @brief Packed<-Scalar multiply and divide by 255.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 255
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 255
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = x0 * u0;
  dst0 = (dst0 + ((dst0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = x0.u32Lo * u0.u32Lo;
  dst0.u32Hi = x0.u32Hi * u0.u32Lo;

  dst0.u32Lo = (dst0.u32Lo + ((dst0.u32Lo >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;
  dst0.u32Hi = (dst0.u32Hi + ((dst0.u32Hi >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;

  dst0.u32Lo &= 0x00FF00FFU;
  dst0.u32Hi &= 0x00FF00FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 255 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 255
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 255
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 255
//! dst1.u16[0] = (x1.u16[0] * u1.u64[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * u1.u64[0]) / 255
//! dst1.u16[2] = (x1.u16[2] * u1.u64[0]) / 255
//! dst1.u16[3] = (x1.u16[3] * u1.u64[0]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64MulDiv255PBW_SBW(dst0, x0, u0);
  p64MulDiv255PBW_SBW(dst1, x1, u1);
}

//! @brief Packed<-Scalar multiply and divide by 256.
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 256
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 256
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW_SBW(
  p64& dst0, const p64& x0, const p64& u0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  dst0 = (x0 * u0) >> 8;
  dst0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);
#else
  dst0.u32Lo = (x0.u32Lo * u0.u32Lo) >> 8;
  dst0.u32Hi = (x0.u32Hi * u0.u32Lo) >> 8;

  dst0.u32Lo &= 0x00FF00FFU;
  dst0.u32Hi &= 0x00FF00FFU;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @brief Packed<-Scalar multiply and divide by 256 (2x).
//!
//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * u0.u64[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * u0.u64[0]) / 256
//! dst0.u16[2] = (x0.u16[2] * u0.u64[0]) / 256
//! dst0.u16[3] = (x0.u16[3] * u0.u64[0]) / 256
//! dst1.u16[0] = (x1.u16[0] * u1.u64[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * u1.u64[0]) / 256
//! dst1.u16[2] = (x1.u16[2] * u1.u64[0]) / 256
//! dst1.u16[3] = (x1.u16[3] * u1.u64[0]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW_SBW_2x(
  p64& dst0, const p64& x0, const p64& u0,
  p64& dst1, const p64& x1, const p64& u1)
{
  p64MulDiv256PBW_SBW(dst0, x0, u0);
  p64MulDiv256PBW_SBW(dst1, x1, u1);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! dst0.u16[2] = (x0.u16[2] * y0.u16[2]) / 255
//! dst0.u16[3] = (x0.u16[3] * y0.u16[3]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW(
  p64& dst0, const p64& x0, const p64& y0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  uint64_t ym = y0;
  uint64_t t0;
  uint64_t t1;

  t0 = (x0 & FOG_UINT64_C(0x00000000000000FF)) * (ym & 0xFFU); ym >>= 16;
  t1 = (x0 & FOG_UINT64_C(0x0000000000FF0000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x000000FF00000000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x00FF000000000000)) * (ym & 0xFFU);
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t0  = (t0 + ((t0 >> 8) & FOG_UINT64_C(0x00FF00FF00FF00FF)) + FOG_UINT64_C(0x0080008000800080)) >> 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = t0;
#else
  uint32_t t0 = _FOG_FACE_COMBINE_2( (x0.u32Lo & 0x000000FFU) * (y0.u32Lo & 0xFFU), (x0.u32Lo & 0x00FF0000U) * (y0.u32Lo >> 16) );
  uint32_t t1 = _FOG_FACE_COMBINE_2( (x0.u32Hi & 0x000000FFU) * (y0.u32Hi & 0xFFU), (x0.u32Hi & 0x00FF0000U) * (y0.u32Hi >> 16) );

  t0 = (t0 + ((t0 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;
  t1 = (t1 + ((t1 >> 8) & 0x00FF00FFU) + 0x00800080U) >> 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Lo = t0;
  dst0.u32Hi = t1;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 255
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 255
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 255
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 255
//! @endverbatim
static FOG_INLINE void p64MulDiv255PBW_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  p64MulDiv255PBW(dst0, x0, y0);
  p64MulDiv255PBW(dst1, x1, y1);
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW(
  p64& dst0, const p64& x0, const p64& y0)
{
#if defined(FOG_FACE_P64_IS_TYPE)
  uint64_t ym = y0;
  uint64_t t0;
  uint64_t t1;

  t0 = (x0 & FOG_UINT64_C(0x00000000000000FF)) * (ym & 0xFFU); ym >>= 16;
  t1 = (x0 & FOG_UINT64_C(0x0000000000FF0000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x000000FF00000000)) * (ym & 0xFFU); ym >>= 16;
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t1 = (x0 & FOG_UINT64_C(0x00FF000000000000)) * (ym & 0xFFU);
  t0 = _FOG_FACE_COMBINE_2(t0, t1);

  t0 >>= 8;
  t0 &= FOG_UINT64_C(0x00FF00FF00FF00FF);

  dst0 = t0;
#else
  uint32_t t0 = _FOG_FACE_COMBINE_2( (x0.u32Lo & 0x000000FFU) * (y0.u32Lo & 0xFFU), (x0.u32Lo & 0x00FF0000U) * (y0.u32Lo >> 16) );
  uint32_t t1 = _FOG_FACE_COMBINE_2( (x0.u32Hi & 0x000000FFU) * (y0.u32Hi & 0xFFU), (x0.u32Hi & 0x00FF0000U) * (y0.u32Hi >> 16) );

  t0 >>= 8;
  t1 >>= 8;

  t0 &= 0x00FF00FFU;
  t1 &= 0x00FF00FFU;

  dst0.u32Lo = t0;
  dst0.u32Hi = t1;
#endif // FOG_FACE_P64_IS_TYPE
}

//! @verbatim
//! dst0.u16[0] = (x0.u16[0] * y0.u16[0]) / 256
//! dst0.u16[1] = (x0.u16[1] * y0.u16[1]) / 256
//! dst1.u16[0] = (x1.u16[0] * y1.u16[0]) / 256
//! dst1.u16[1] = (x1.u16[1] * y1.u16[1]) / 256
//! @endverbatim
static FOG_INLINE void p64MulDiv256PBW_2x(
  p64& dst0, const p64& x0, const p64& y0,
  p64& dst1, const p64& x1, const p64& y1)
{
  p64MulDiv256PBW(dst0, x0, y0);
  p64MulDiv256PBW(dst1, x1, y1);
}

// Cleanup.
#undef _U64

//! @}

} // Face namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_FACE_FACEC_P64_H
