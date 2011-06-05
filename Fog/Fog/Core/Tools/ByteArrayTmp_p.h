// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_BYTEARRAYTMP_P_H
#define _FOG_CORE_TOOLS_BYTEARRAYTMP_P_H

// [Dependencies]
#include <Fog/Core/Tools/ByteArray.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::ByteArrayTmp<N>]
// ============================================================================

template<size_t N>
struct ByteArrayTmp : public ByteArray
{
  struct _Storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE ByteArrayTmp() :
    ByteArray(ByteArrayData::adopt((void*)&_storage, N))
  {
  }

  FOG_INLINE ByteArrayTmp(const char ch) :
    ByteArray(ByteArrayData::adopt((void*)&_storage, N, &ch, 1))
  {
  }

  FOG_INLINE ByteArrayTmp(const Stub8& str) :
    ByteArray(ByteArrayData::adopt((void*)&_storage, N, str.getData(), str.getLength()))
  {
  }

  FOG_INLINE ByteArrayTmp(const char* str) :
    ByteArray(ByteArrayData::adopt((void*)&_storage, N, str, DETECT_LENGTH))
  {
  }

  FOG_INLINE ByteArrayTmp(const ByteArray& other) :
    ByteArray(ByteArrayData::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  FOG_INLINE ByteArrayTmp(const ByteArrayTmp<N>& other) :
    ByteArray(ByteArrayData::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  FOG_INLINE ~ByteArrayTmp()
  {
    FOG_ASSERT( (_d == &_storage.d && _storage.d.refCount.get() == 1) ||
                (_d != &_storage.d && _storage.d.refCount.get() == 0) );
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  FOG_INLINE void reset()
  {
    if ((void*)_d != (void*)&_storage)
    {
      atomicPtrXchg(&_d, ByteArrayData::adopt((void*)&_storage, N))->deref();
    }
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  // These overloads are needed to succesfully use this template (or implicit
  // conversion will break template and new string will be allocated).

  FOG_INLINE ByteArrayTmp<N>& operator=(char ch) { set(ch); return *this; }
  FOG_INLINE ByteArrayTmp<N>& operator=(const Stub8& str) { set(str); return *this; }
  FOG_INLINE ByteArrayTmp<N>& operator=(const char* str) { set(str); return *this; }
  FOG_INLINE ByteArrayTmp<N>& operator=(const ByteArray& other) { set(other); return *this; }
  FOG_INLINE ByteArrayTmp<N>& operator=(const ByteArrayTmp<N>& other) { set(other); return *this; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  struct _Storage
  {
    ByteArrayData d;
    char data[N];
  } _storage;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_BYTEARRAYTMP_P_H
