// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/BitArray.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Std.h>

namespace Fog {

// ============================================================================
// [Fog::BitArray]
// ============================================================================

// byte fill tables
static const uint8_t nToBitsINC[8] =
{
  0x00, // 00000000
  0x01, // 00000001
  0x03, // 00000011
  0x07, // 00000111
  0x0F, // 00001111
  0x1F, // 00011111
  0x3F, // 00111111
  0x7F  // 01111111
};

static const uint8_t nToBitsDEC[8] =
{
  // Simple conversion, result is nToBitsINC[n] << (8 - n).
  0x00, // 00000000
  0x80, // 10000000
  0xC0, // 11000000
  0xE0, // 11100000
  0xF0, // 11110000
  0xF8, // 11111000
  0xFC, // 11111100
  0xFE  // 11111110
};

// Need is always larger than capacity.
static sysuint_t getOptimalCapacity(sysuint_t capacity, sysuint_t need)
{
  FOG_ASSERT(need >= capacity);

  if (capacity < 128 && need < 128) return 128;

  sysuint_t result = capacity;

  if (result <= 1024*1024*8) result <<= 1;
  if (result < need) result = need;

  return result;
}

// Basic functions for manipulating with RAW bits.

// Copy bits from source to destination, bit offsets means first bit in data buffer,
// count is always means in bits. Source and destination pointers can overlap.
static void _copyBits(uint8_t* dst, sysuint_t dstBitOffset, const uint8_t* src, sysuint_t srcBitOffset, sysuint_t _count)
{
#if 1
  sysuint_t i = dstBitOffset & 7;
  sysuint_t count = _count;

  if (i)
  {
    // Calculate count of bits thats needed to align the destination.
    i = 8 - i;
    if (i > count) i = count;

    count -= i;
    dstBitOffset += i;
    srcBitOffset += i;
  }

  // Destination is aligned, make from source one BYTE and copy it to destination.
  i = srcBitOffset & 7;

  dst += (dstBitOffset >> 3);
  src += (srcBitOffset >> 3);

  sysuint_t count_div8 = count >> 3;

  // Source and destination are aligned.
  if (i == 0)
  {
    for (i = 0; i != count_div8; i++, dst++, src++) dst[0] = src[0];

    // Tail BYTE.
    uint8_t mask = ~nToBitsINC[count & 7];
    dst[0] = (dst[0] & ~mask) | (src[0] & mask);
  }
  // Source and destination aren't aligned.
  else
  {
    // Count of bits in mask1 and mask2 have to be 8.
    uint8_t shft0 = 8 - (uint8_t)i;
    uint8_t shft1 = (uint8_t)i;
    uint8_t mask0 = nToBitsDEC[8 - i];
    uint8_t mask1 = nToBitsINC[i];

    uint8_t src0 = src[0];
    for (i = 0; i != count_div8; i++, dst++, src++)
    {
      uint8_t src1 = src[1];
      dst[0] = ((src0 & mask0) << shft0) | ((src1 & mask1) >> shft1);
      src0 = src1;
    }

    // Tail BYTE.
    uint8_t mask = ~nToBitsINC[count & 7];
    if (mask)
    {
      dst[0] = (dst[0] & ~mask) | (((src0 & mask0) << shft0) | ((src[1] & mask1) >> shft1) & mask);
    }
  }
#endif

#if 0
  // When copying bits, we need some logic in it. So we will first align dstBitOffset
  // and then we copy bits as 1 BYTE to the destination.
  sysuint_t dstByteOffset = dstBitOffset >> 3;
  sysuint_t srcByteOffset = srcBitOffset >> 3;

  dstBitOffset &= 7;
  srcBitOffset &= 7;

  dst += dstByteOffset;
  src += srcByteOffset;

  sysuint_t align = (8 - (dstBitOffset)) & 7;
  sysuint_t tail = (_count - align) & 7;
  sysuint_t count = _count - align - tail;
  sysuint_t bcount = count >> 3;

  sysuint_t i;

  dst += dstByteOffset;
  src += srcByteOffset;
  
  if (dstBitOffset == srcBitOffset)
  {
    if (align)
    {
      uint8_t mask = nToBitsINC[align];
      dst[0] = (dst[0] & ~mask) | (src[0] & mask);
      dst++;
      src++;
    }

    for (i = 0; i != bcount; i++, dst++, src++) dst[0] = src[0];

    if (tail)
    {
      uint8_t mask = nToBitsDEC[align];
      dst[0] = (dst[0] & ~mask) | (src[0] & mask);
    }
  }
  else
  {
    uint8_t shft0 = (8 - (uint8_t)srcBitOffset) & 7;
    uint8_t shft1 = (uint8_t)srcBitOffset;
    uint8_t mask0 = nToBitsDEC[8 - srcBitOffset];
    uint8_t mask1 = nToBitsINC[srcBitOffset];

    uint8_t src0 = src[0];

    if (align)
    {
      uint8_t src1 = src[1];
      uint8_t mask = nToBitsINC[align];
      dst[0] = (dst[0] & ~mask) | (src[0] & mask);
      src0 = src1;

      dst++;
      src++;
    }

    for (i = 0; i != bcount; i++, dst++, src++)
    {
      uint8_t src1 = src[1];
      dst[0] = ((src0 & mask1) << shft1) | ((src1 & mask2) >> shft2);
      src0 = src1;
    }

    if (tail)
    {
      uint8_t mask = nToBitsDEC[align];
      dst[0] = (dst[0] & ~mask) | (src[0] & mask);
    }
  }

  if (i)
  {
    // Calculate count of bits thats needed to align the destination.
    i = 8 - i;
    if (i > count) i = count;

    count -= i;
    dstBitOffset += i;
    srcBitOffset += i;
  }

  // Destination is aligned, make from source one BYTE and copy it to destination.
  i = srcBitOffset & 7;

  dst += (dstBitOffset >> 3);
  src += (srcBitOffset >> 3);

  sysuint_t count_div8 = count >> 3;

  // Source and destination are aligned.
  if (i == 0)
  {
    for (i = 0; i != count_div8; i++, dst++, src++) dst[0] = src[0];

    // Copy the last BYTE (but we must OR it to the destination).

    // Tail mask.
    uint8_t mask = ~nToBitsINC[count & 7];
    // Clear remaining bits in dst.
    *dst &= mask;
    // Set remaining bits in dst from src.
    *dst |= *src & mask;
  }
  // Source and destination aren't aligned.
  else
  {
    // Count of bits in mask1 and mask2 have to be 8.
    uint8_t shft1 = 8 - (uint8_t)i;
    uint8_t shft2 = (uint8_t)i;
    uint8_t mask1 = nToBitsDEC[8 - i];
    uint8_t mask2 = nToBitsINC[i];

    uint8_t src0 = src[0];
    for (i = 0; i != count_div8; i++, dst++, src++)
    {
      uint8_t dst0 = ((src0 & mask1) << shft1);
      src0 = src[1];
      dst0 |= ((src0 & mask2) >> shft2);
      dst[0] = dst0;
    }

    // Tail mask.
    uint8_t mask = ~nToBitsINC[count & 7];
    // Clear remaining bits in dst.
    *dst &= mask;
    // Set remaining bits in dst from src.
    *dst |= ((src[0] & mask1) << shft1) | ((src[1] & mask2) >> shft2) & mask;
  }
#endif
}

static void _setBits(uint8_t* dst, sysuint_t dstBitOffset, uint32_t bit, sysuint_t _count)
{
  sysuint_t i;

  sysuint_t byteoffset = dstBitOffset >> 3;
  sysuint_t bitoffset = dstBitOffset & 7;
  sysuint_t count = _count;
  sysuint_t align = (8 - bitoffset) & 7;

  if (align > count) { align = count; count = 0; }
  else { count -= align; }

  sysuint_t tail = count & 7;

  // align contains bits that needs to be aligned
  // count >> 3 will contain full BYTES that's needed to fill by pattern
  // tail contains bits that needs to do after BYTE fill

  sysuint_t count_div8 = count >> 3;
  uint8_t pattern = bit ? 0xFF : 0x00;

  // advance dst pointer
  dst += byteoffset;

  if (align)
  {
    uint8_t mask = nToBitsINC[align] << bitoffset;
    if (bit)
      *dst++ |= mask;
    else
      *dst++ &= ~mask;
  }

  // BYTE fill
  for (i = 0; i != count_div8; i++) *dst++ = pattern;

  // do the tail
  if (tail)
  {
    uint8_t mask = nToBitsINC[tail];

    if (bit)
      *dst |= mask;
    else
      *dst &= ~mask;
  }
}

static void _xorBits(uint8_t* dst, sysuint_t dstBitOffset, sysuint_t _count)
{
  sysuint_t i;

  sysuint_t byteoffset = dstBitOffset >> 3;
  sysuint_t bitoffset = dstBitOffset & 7;
  sysuint_t count = _count;
  sysuint_t align = (8 - bitoffset) & 7;

  if (align > count) { align = count; count = 0; }
  else { count -= align; }

  sysuint_t tail = count & 7;

  // align contains bits that needs to be aligned
  // count >> 3 will contain full BYTES that's needed to fill by pattern
  // tail contains bits that needs to do after BYTE fill

  sysuint_t count_div8 = count >> 3;

  // advance dst pointer
  dst += byteoffset;

  if (align)
  {
    uint8_t mask = nToBitsINC[align] << bitoffset;
    *dst++ ^= mask;
  }

  // BYTE xor
  for (i = 0; i != count_div8; i++) *dst++ ^= 0xFF;

  // do the tail
  if (tail)
  {
    uint8_t mask = nToBitsINC[tail];
    *dst ^= mask;
  }
}

// basic ideas: 1 byte contains 8 bits, so it's very safe to use shift
// operator aritmetic, so:
// 1. (count >> 3) means (count / 8)
// 2. (count + 31) & ~31 will round count of bits to 32
//                       (0->32, 1->32, 32->32, 33->64, etc...)

BitArray::BitArray()
  : _d(_dnull->ref())
{
}

BitArray::BitArray(const BitArray& other)
  : _d(other._d->ref())
{
}

BitArray::~BitArray()
{
  _d->deref();
}

err_t BitArray::_detach()
{
  Data* d = _d;

  if (d->refCount.get() > 1)
  {
    d = Data::create(0, d->data, 0, d->length);
    if (!d) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, d)->deref();
  }

  return ERR_OK;
}

void BitArray::free()
{
  atomicPtrXchg(&_d, _dnull->ref())->deref();
}

err_t BitArray::reserve(sysuint_t to)
{
  Data* d = _d;

  if (to < d->length) to = d->length;
  to = (to + 31) & ~31;

  if (d->refCount.get() > 1)
  {
    d = Data::create(to);
    if (!d) return ERR_RT_OUT_OF_MEMORY;
    d->length = _d->length;
    memcpy(d->data, _d->data, (_d->length + 7) >> 3);
    atomicPtrXchg(&_d, d)->deref();
  }
  else if (to > d->capacity)
  {
    d = (Data *)Memory::realloc(d, sizeof(Data) - (sizeof(char) * 4) + (to >> 3));
    if (!d) return ERR_RT_OUT_OF_MEMORY;

    _d = d;
    d->capacity = to;
  }

  return ERR_OK;
}

err_t BitArray::resize(sysuint_t to, uint32_t fill)
{
  Data* d = _d;

  if (d->refCount.get() > 1)
  {
    d = Data::create(to);
    if (!d) return 0;
    d->length = to;
    memcpy(d->data, _d->data, ((to <= _d->length ? to : _d->length) + 7) >> 3);
    _setBits(d->data, _d->length, fill, to - _d->length);
    atomicPtrXchg(&_d, d)->deref();
    return ERR_OK;
  }
  else if (to > d->capacity)
  {
    sysuint_t c = ((to + 31) & ~31);
    d = (Data *)Memory::realloc(d, sizeof(Data) - (sizeof(char)*4) + (c >> 3));
    if (!d) return ERR_RT_OUT_OF_MEMORY;

    _d = d;
    d->capacity = c;
  }

  _setBits(d->data, d->length, fill, to - d->length);
  d->length = to;
  return ERR_OK;
}

err_t BitArray::grow(sysuint_t by)
{
  Data* d = _d;
  sysuint_t length = d->length;
  sysuint_t capacity = length + by;

  FOG_ASSERT(capacity >= d->capacity);
  return reserve(capacity);
}

void BitArray::squeeze()
{
  Data* d = _d;

  sysuint_t length = d->length;
  if (length + 31 < d->capacity)
  {
    sysuint_t c = ((length + 31) & ~31);
    if (d->refCount.get() > 1)
    {
      d = Data::create(c);
      if (!d) return;

      d->length = length;
      memcpy(d->data, _d->data, (length + 7) >> 3);
      atomicPtrXchg(&_d, d)->deref();
    }
    else
    {
      d = (Data *)Memory::realloc(d, sizeof(Data) - (sizeof(char) * 4) + (c >> 3));
      if (!d) return;

      d->capacity = length;
      _d = d;
    }
  }
}

void BitArray::clear()
{
  Data* d = _d;

  if (d->refCount.get() > 1)
    atomicPtrXchg(&_d, _dnull->ref())->deref();
  else
    d->length = 0;
}

err_t BitArray::setBit(sysuint_t index)
{
  Data* d = _d;

  // If index is out of bounds, return.
  if (index >= d->length) return ERR_RT_OVERFLOW;

  sysuint_t byteIndex = index >> 3;
  uint8_t byteMask = 1 << (index & 7);

  if (FOG_UNLIKELY(d->refCount.get() > 1))
  {
    // Check if bit is set, if yes, there is nothing to to and there are not needed
    // next steps (detach and set).
    if ((d->data[byteIndex] & byteMask) != 0) return ERR_OK;

    // Detach, if bit array data is shared (and update 'd', because it will be in new address).
    FOG_RETURN_ON_ERROR(_detach());
    d = _d;
  }

  // Set bit in data.
  d->data[byteIndex] |= byteMask;

  return ERR_OK;
}

err_t BitArray::setBit(sysuint_t index, uint32_t bit)
{
  Data* d = _d;

  // If index is out of bounds, return.
  if (index >= d->length) return ERR_RT_OVERFLOW;

  sysuint_t byteIndex = index >> 3;
  uint8_t byteMask = 1 << (index & 7);

  if (FOG_UNLIKELY(d->refCount.get() > 1))
  {
    // Check if bit is equal to demanded one, if yes, there is nothing to
    // to and there are not needed next steps (detach and set).
    if (!!(d->data[byteIndex] & byteMask) == bit) return ERR_OK;

    // Detach, if bit array data is shared (and update 'd', because it will be in new address).
    FOG_RETURN_ON_ERROR(_detach());
    d = _d;
  }

  // Set bit in data.
  if (bit)
    d->data[byteIndex] |= byteMask;
  else
    d->data[byteIndex] &= ~byteMask;

  return ERR_OK;
}

err_t BitArray::resetBit(sysuint_t index)
{
  Data* d = _d;

  // If index is out of bounds, return.
  if (index >= d->length) return ERR_RT_OVERFLOW;

  sysuint_t byteIndex = index >> 3;
  uint8_t byteMask = 1 << (index & 7);

  if (d->refCount.get() > 1)
  {
    // Check if bit isn't set, if yes, there is nothing to to and there are not needed.
    // next steps (detach and set)
    if ((d->data[byteIndex] & byteMask) == 0) return ERR_OK;

    // Detach, if bit array data is shared (and update 'd', because it will be in new address).
    FOG_RETURN_ON_ERROR(_detach());
    d = _d;
  }

  // Unset bit in data.
  d->data[byteIndex] &= ~byteMask;

  return ERR_OK;
}

err_t BitArray::invertBit(sysuint_t index)
{
  // Bail out if index is out of bounds.
  if (index >= _d->length) return ERR_RT_OVERFLOW;

  FOG_RETURN_ON_ERROR(detach());

  sysuint_t byteIndex = index >> 3;
  uint8_t byteMask = 1 << (index & 7);

  // Xor bit in data.
  _d->data[byteIndex] ^= byteMask;

  return ERR_OK;
}

err_t BitArray::fill(uint32_t bit)
{
  FOG_RETURN_ON_ERROR(detach());

  memset(_d->data, bit ? 0xFF : 0x00, (getLength() + 7) >> 3);
  return ERR_OK;
}

err_t BitArray::fillAt(sysuint_t index, sysuint_t count, uint32_t bit)
{
  Data* d = _d;
  sysuint_t length = d->length;

  if (index >= length) return ERR_RT_OVERFLOW;
  if (length - index < count) count = length - index;

  if (count == 0) return ERR_OK;
  FOG_RETURN_ON_ERROR(detach());

  _setBits(_d->data, index, bit, count);
  return ERR_OK;
}

err_t BitArray::invert()
{
  FOG_RETURN_ON_ERROR(detach());

  uint8_t* bits = _d->data;
  sysuint_t i, count_div8 = (_d->length + 7) >> 3;
  for (i = 0; i != count_div8; i++) *bits++ ^= 0xFF;

  return ERR_OK;
}

err_t BitArray::invertAt(sysuint_t index, sysuint_t count)
{
  Data* d = _d;
  sysuint_t length = d->length;

  if (index >= length) return ERR_RT_OVERFLOW;
  if (length - index < count) count = length - index;

  if (count == 0) return ERR_OK;
  FOG_RETURN_ON_ERROR(detach());

  _xorBits(_d->data, index, count);

  return ERR_OK;
}

err_t BitArray::set(const BitArray& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return ERR_OK;
}

err_t BitArray::setDeep(const BitArray& other)
{
  Data* self_d = _d;
  Data* other_d = other._d;

  if (self_d == other_d) return ERR_OK;

  if (self_d->refCount.get() == 1 && self_d->capacity >= other_d->length)
  {
    sysuint_t length = other_d->length;
    _copyBits(self_d->data, 0, other_d->data, 0, length);
    self_d->length = length;
  }
  else
  {
    Data* d = other_d->ref();
    if (!d) return ERR_RT_OUT_OF_MEMORY;
    atomicPtrXchg(&_d, d)->deref();
  }

  return ERR_OK;
}

err_t BitArray::append(uint32_t bit)
{
  Data* d = _d;
  sysuint_t length = d->length;

  if (FOG_UNLIKELY(d->refCount.get() > 1))
  {
    err_t err = reserve(length + 1);
    if (err) return err;
 
    d = _d;
  }
  else if (FOG_UNLIKELY(d->capacity == length))
  {
    err_t err = reserve(length + getOptimalCapacity(length, 1));
    if (err) return err;

    d = _d;
  }

  // Append bit.
  if (bit)
    d->data[length >> 3] |= (1 << (length & 7));
  else
    d->data[length >> 3] &= ~(1 << (length & 7));
  d->length++;

  return ERR_OK;
}

err_t BitArray::append(uint32_t bit, sysuint_t count)
{
  Data* d = _d;
  sysuint_t before = d->length;
  sysuint_t after = before + count;

  FOG_ASSERT(after >= before);

  if (d->refCount.get() > 1)
  {
    err_t err = reserve(after);
    if (err) return err;

    d = _d;
  }
  else if (d->capacity < after)
  {
    err_t err = reserve(before + getOptimalCapacity(before, count));
    if (err) return err;

    d = _d;
  }

  // Append bits.
  _setBits(d->data, before, bit, count);
  d->length = after;

  return ERR_OK;
}

err_t BitArray::append(const BitArray& other)
{
  Data* d = _d;
  sysuint_t before = d->length;
  sysuint_t after = before + other.getLength();

  FOG_ASSERT(after >= before);

  if (d->refCount.get() > 1)
  {
    err_t err = reserve(after);
    if (err) return err;

    d = _d;
  }
  else if (d->capacity < after)
  {
    err_t err = reserve(before + getOptimalCapacity(before, other.getLength()));
    if (err) return err;

    d = _d;
  }

  // Append bit array.
  _copyBits(d->data, before, other._d->data, 0, other.getLength());
  d->length = after;

  return ERR_OK;
}

err_t BitArray::prepend(uint32_t bit)
{
  return insert(0, bit);
}

err_t BitArray::prepend(uint32_t bit, sysuint_t count)
{
  return insert(0, bit, count);
}

err_t BitArray::prepend(const BitArray& other)
{
  return insert(0, other);
}

err_t BitArray::insert(sysuint_t index, uint32_t bit)
{
  // TODO
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t BitArray::insert(sysuint_t index, uint32_t bit, sysuint_t count)
{
  // TODO
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t BitArray::insert(sysuint_t index, const BitArray& other)
{
  // TODO
  return ERR_RT_NOT_IMPLEMENTED;
}

bool BitArray::eq(const BitArray* a, const BitArray* b)
{
  Data* a_d = a->_d;
  Data* b_d = b->_d;

  if (a_d->length != b_d->length) return false;
  if (a_d == b_d) return true;

  sysuint_t i;

  sysuint_t bytesCount = a_d->length >> 3;
  sysuint_t tail = a_d->length & 7;

  const uint8_t* a_data = a_d->data;
  const uint8_t* b_data = b_d->data;

  // BYTE compare
  for (i = 0; i != bytesCount; i++)
  {
    if (*a_data++ != *b_data++) return false;
  }

  // compare tail (but needed to modify trailing bits)
  if (tail)
    return ((*a_data ^ *b_data ) & nToBitsINC[tail]) == 0;
  else
    return true;
}

static FOG_INLINE int compareHelper(sysuint_t a, sysuint_t b)
{
  ulong i;

  for (i = 1; i != 0x100; i <<= 1)
  {
    if ((a & i) != (b & i))
    {
      return (int)(a & i) - (int)(b & i);
    }
  }

  // Shouldn't happen
  FOG_ASSERT_NOT_REACHED();
  return 0;
}

int BitArray::compare(const BitArray* a, const BitArray* b)
{
  Data* a_d = a->_d;
  Data* b_d = b->_d;

  if (a_d == b_d) return 0;

  sysuint_t i;
  sysuint_t bitsCompare = Math::min(a_d->length, b_d->length);
  sysuint_t bytesCompare = bitsCompare >> 3;

  const uint8_t* a_data = a_d->data;
  const uint8_t* b_data = b_d->data;

  uint8_t a_i;
  uint8_t b_i;

  // BYTE compare
  for (i = 0; i != bytesCompare; i++)
  {
    a_i = *a_data++;
    b_i = *b_data++;

    if (a_i != b_i) return compareHelper(a_i, b_i);
  }

  // compare tail (but needed to modify trailing bits)
  bitsCompare &= 7;
  if (bitsCompare)
  {
    uint8_t mask = nToBitsINC[bitsCompare];
    a_i = (*a_data) & mask;
    b_i = (*b_data) & mask;

    if (a_i != b_i) return compareHelper(a_i, b_i);
  }

  if (a_d->length == b_d->length)
    return 0;
  else if (a_d->length < b_d->length)
    return 1;
  else
    return -1;
}

// ============================================================================
// [Fog::BitArray::Data]
// ============================================================================

void BitArray::Data::deref()
{
  if (refCount.deref()) Memory::free(this);
}

BitArray::Data* BitArray::Data::create(sysuint_t capacity)
{
  capacity = (capacity + 31) & ~31;

  sysuint_t dsize = sizeof(Data) - (sizeof(uint8_t) * 4) + (capacity >> 3);
  Data* d = (Data*)Memory::alloc(dsize);
  if (!d) return NULL;

  d->refCount.init(1);
  d->capacity = capacity;
  d->length = 0;

  return d;
}

BitArray::Data* BitArray::Data::create(sysuint_t capacity, const uint8_t* data, sysuint_t bitoffset, sysuint_t count)
{
  if (capacity < count) capacity = count;

  Data* d = create(capacity);
  if (!d) return NULL;

  d->length = count;
  _copyBits(d->data, 0, data, bitoffset, count);
  return d;
}

BitArray::Data* BitArray::Data::copy(const Data* other)
{
  Data* d = create(other->length);
  if (!d) return NULL;

  d->length = other->length;
  Memory::copy(d->data, other->data, (d->length + 7) >> 3);

  return d;
}

Static<BitArray::Data> BitArray::_dnull;

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_bitarray_init(void)
{
  using namespace Fog;

  BitArray::_dnull.init();

  BitArray::Data* d = BitArray::_dnull.instancep();
  d->refCount.init(1);
  d->capacity = 0;
  d->length = 0;
  *((uint32_t *)reinterpret_cast<void*>(d->data)) = 0U;

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_bitarray_shutdown(void)
{
  using namespace Fog;

  BitArray::_dnull.instancep()->refCount.dec();
  BitArray::_dnull.destroy();
}
