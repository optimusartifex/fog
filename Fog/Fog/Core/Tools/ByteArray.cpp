// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Collection/Algorithms.h>
#include <Fog/Core/Collection/HashUtil.h>
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Tools/Byte.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/ByteArrayFilter.h>
#include <Fog/Core/Tools/ByteArrayMatcher.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/TextCodec.h>

#include <stdarg.h>

namespace Fog {

// ============================================================================
// [Fog::ByteArray - Helpers]
// ============================================================================

static FOG_INLINE bool fitToRange(
  const ByteArray& self, sysuint_t& _start, sysuint_t& _end, const Range& range)
{
  _start = range.getStart();
  _end = Math::min(range.getEnd(), self.getLength());
  return _start < _end;
}

// ============================================================================
// [Fog::ByteArray - Construction / Destruction]
// ============================================================================

ByteArray::ByteArray()
{
  _d = _dnull->refAlways();
}

ByteArray::ByteArray(char ch, sysuint_t length)
{
  _d = Data::alloc(length);
  if (FOG_IS_NULL(_d)) { _d = _dnull->refAlways(); return; }
  if (!length) return;

  StringUtil::fill(_d->data, ch, length);
  _d->length = length;
  _d->data[length] = 0;
}

ByteArray::ByteArray(const ByteArray& other)
{
  _d = other._d->ref();
}

ByteArray::ByteArray(const ByteArray& other1, const ByteArray& other2)
{
  if (other1.getLength() == 0)
  {
    _d = other2._d->ref();
  }
  else
  {
    _d = other1._d->ref();
    append(other2);
  }
}

ByteArray::ByteArray(const char* str)
{
  sysuint_t length = StringUtil::len(str);

  _d = Data::alloc(0, str, length);
  if (FOG_IS_NULL(_d)) _d = _dnull->refAlways();
}

ByteArray::ByteArray(const char* str, sysuint_t length)
{
  if (length == DETECT_LENGTH) length = StringUtil::len(str);

  _d = Data::alloc(0, str, length);
  if (FOG_IS_NULL(_d)) _d = _dnull->refAlways();
}

ByteArray::ByteArray(const Stub8& str)
{
  const char* sData = str.getData();
  sysuint_t sLength = str.getComputedLength();

  _d = Data::alloc(0, sData, sLength);
  if (FOG_IS_NULL(_d)) _d = _dnull->refAlways();
}

ByteArray::~ByteArray()
{
  _d->derefInline();
}

// ============================================================================
// [Fog::ByteArray - Implicit Sharing]
// ============================================================================

err_t ByteArray::_detach()
{
  if (isDetached()) return ERR_OK;

  Data* newd = Data::copy(_d);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;
}

// ============================================================================
// [Fog::ByteArray::Flags]
// ============================================================================

void ByteArray::setSharable(bool val)
{
  if (isSharable() == val) return;

  // TODO: Return error?
  detach();

  if (val)
    _d->flags |= Data::IsSharable;
  else
    _d->flags &= ~Data::IsSharable;
}

void ByteArray::setStrong(bool val)
{
  if (isStrong() == val) return;

  // TODO: Return error?
  detach();

  if (val)
    _d->flags |= Data::IsStrong;
  else
    _d->flags &= ~Data::IsStrong;
}

// ============================================================================
// [Fog::ByteArray::Data]
// ============================================================================

err_t ByteArray::prepare(sysuint_t capacity)
{
  Data* d = _d;

  if (d->refCount.get() == 1 && d->capacity >= capacity)
  {
    d->hashCode = 0;
    d->length = 0;
    d->data[0] = 0;
    return ERR_OK;
  }

  d = Data::alloc(capacity);
  if (FOG_IS_NULL(d)) return ERR_RT_OUT_OF_MEMORY;

  atomicPtrXchg(&_d, d)->deref();
  return ERR_OK;
}

char* ByteArray::beginManipulation(sysuint_t max, int outputMode)
{
  Data* d = _d;

  if (outputMode == OUTPUT_MODE_SET)
  {
    if (d->refCount.get() == 1 && d->capacity >= max)
    {
      d->hashCode = 0;
      d->length = 0;
      d->data[0] = 0;
      return d->data;
    }

    d = Data::alloc(max);
    if (FOG_IS_NULL(d)) return NULL;

    atomicPtrXchg(&_d, d)->deref();
    return d->data;
  }
  else
  {
    sysuint_t length = d->length;
    sysuint_t newmax = length + max;

    // Overflow.
    if (length > newmax) return NULL;

    if (d->refCount.get() == 1 && d->capacity >= newmax)
      return d->data + length;

    if (d->refCount.get() > 1)
    {
      sysuint_t optimalCapacity = Util::getGrowCapacity(
        sizeof(ByteArray::Data), sizeof(char), length, newmax);

      d = Data::alloc(optimalCapacity, d->data, d->length);
      if (FOG_IS_NULL(d)) return NULL;

      atomicPtrXchg(&_d, d)->deref();
      return d->data + length;
    }
    else
    {
      sysuint_t optimalCapacity = Util::getGrowCapacity(
        sizeof(ByteArray::Data), sizeof(char), length, newmax);

      d = Data::realloc(_d, optimalCapacity);
      if (FOG_IS_NULL(d)) return NULL;

      _d = d;
      return d->data + length;
    }
  }
}

err_t ByteArray::reserve(sysuint_t to)
{
  if (to < _d->length) to = _d->length;
  if (_d->refCount.get() == 1 && _d->capacity >= to) goto done;

  if (_d->refCount.get() > 1)
  {
    Data* newd = Data::alloc(to, _d->data, _d->length);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, newd)->deref();
  }
  else
  {
    Data* newd = Data::realloc(_d, to);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    _d = newd;
  }
done:
  return ERR_OK;
}

err_t ByteArray::resize(sysuint_t to)
{
  if (_d->refCount.get() == 1 && _d->capacity >= to) goto done;

  if (_d->refCount.get() > 1)
  {
    Data* newd = Data::alloc(to, _d->data, to < _d->length ? to : _d->length);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, newd)->deref();
  }
  else
  {
    Data* newd = Data::realloc(_d, to);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    _d = newd;
  }

done:
  _d->hashCode = 0;
  _d->length = to;
  _d->data[to] = 0;
  return ERR_OK;
}

err_t ByteArray::grow(sysuint_t by)
{
  sysuint_t lengthBefore = _d->length;
  sysuint_t lengthAfter = lengthBefore + by;

  FOG_ASSERT(lengthBefore <= lengthAfter);

  if (_d->refCount.get() == 1 && _d->capacity >= lengthAfter) goto done;

  if (_d->refCount.get() > 1)
  {
    sysuint_t optimalCapacity = Util::getGrowCapacity(
      sizeof(ByteArray::Data), sizeof(char), lengthBefore, lengthAfter);

    Data* newd = Data::alloc(optimalCapacity, _d->data, _d->length);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, newd)->deref();
  }
  else
  {
    sysuint_t optimalCapacity = Util::getGrowCapacity(
      sizeof(ByteArray::Data), sizeof(char), lengthBefore, lengthAfter);

    Data* newd = Data::realloc(_d, optimalCapacity);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    _d = newd;
  }

done:
  _d->hashCode = 0;
  _d->length = lengthAfter;
  _d->data[lengthAfter] = 0;
  return ERR_OK;
}

void ByteArray::squeeze()
{
  sysuint_t i = _d->length;
  sysuint_t c = _d->capacity;

  // Pad to 8 bytes
  i = (i + 7) & ~7;

  if (i < c)
  {
    Data* newd = Data::alloc(0, _d->data, _d->length);
    if (FOG_IS_NULL(newd)) return;
    atomicPtrXchg(&_d, newd)->deref();
  }
}

void ByteArray::clear()
{
  if (_d->refCount.get() > 1)
  {
    atomicPtrXchg(&_d, _dnull->refAlways())->deref();
    return;
  }

  _d->hashCode = 0;
  _d->length = 0;
  _d->data[0] = 0;
}

void ByteArray::reset()
{
  atomicPtrXchg(&_d, _dnull->refAlways())->deref();
}

const void* ByteArray::nullTerminated() const
{
  Data* d = _d;
  uint32_t* s = reinterpret_cast<uint32_t*>(d->data + d->length);
  if (*s) *s = 0;
  return d->data;
}

static char* _prepareSet(ByteArray* self, sysuint_t length)
{
  ByteArray::Data* d = self->_d;
  if (FOG_UNLIKELY(length == 0)) goto skip;

  if (d->refCount.get() > 1)
  {
    d = ByteArray::Data::alloc(length);
    if (FOG_IS_NULL(d)) return NULL;
    atomicPtrXchg(&self->_d, d)->derefInline();
  }
  else if (d->capacity < length)
  {
    sysuint_t optimalCapacity = Util::getGrowCapacity(sizeof(ByteArray::Data), sizeof(char), d->length, length);
    d = ByteArray::Data::realloc(d, optimalCapacity);
    if (FOG_IS_NULL(d)) return NULL;
    self->_d = d;
  }

  d->hashCode = 0;
  d->length = length;
  d->data[d->length] = 0;
skip:
  return d->data;
}

static char* _prepareAppend(ByteArray* self, sysuint_t length)
{
  ByteArray::Data* d = self->_d;

  sysuint_t lengthBefore = d->length;
  sysuint_t lengthAfter = lengthBefore + length;

  if (FOG_UNLIKELY(length == 0)) goto skip;

  if (d->refCount.get() > 1)
  {
    d = ByteArray::Data::alloc(lengthAfter, d->data, d->length);
    if (FOG_IS_NULL(d)) return NULL;
    atomicPtrXchg(&self->_d, d)->derefInline();
  }
  else if (d->capacity < lengthAfter)
  {
    sysuint_t optimalCapacity = Util::getGrowCapacity(
      sizeof(ByteArray::Data), sizeof(char), lengthBefore, lengthAfter);

    d = ByteArray::Data::realloc(d, optimalCapacity);
    if (FOG_IS_NULL(d)) return NULL;
    self->_d = d;
  }

  d->hashCode = 0;
  d->length = lengthAfter;
  d->data[lengthAfter] = 0;
skip:
  return d->data + lengthBefore;
}

static char* _prepareInsert(ByteArray* self, sysuint_t index, sysuint_t length)
{
  ByteArray::Data* d = self->_d;

  sysuint_t lengthBefore = d->length;
  sysuint_t lengthAfter = lengthBefore + length;
  sysuint_t moveBy;

  if (index > lengthBefore) index = lengthBefore;
  // If data length is zero we can just skip all this machinery.
  if (FOG_UNLIKELY(!length)) goto skip;

  moveBy = lengthBefore - index;

  if (d->refCount.get() > 1 || d->capacity < lengthAfter)
  {
    sysuint_t optimalCapacity = Util::getGrowCapacity(
      sizeof(ByteArray::Data), sizeof(char), lengthBefore, lengthAfter);

    d = ByteArray::Data::alloc(optimalCapacity, d->data, index);
    if (FOG_IS_NULL(d)) return NULL;

    StringUtil::copy(
      d->data + index + length, self->_d->data + index, moveBy);
    atomicPtrXchg(&self->_d, d)->derefInline();
  }
  else
  {
    StringUtil::move(
      d->data + index + length, d->data + index, moveBy);
  }

  d->hashCode = 0;
  d->length = lengthAfter;
  d->data[lengthAfter] = 0;
skip:
  return d->data + index;
}

static char* _prepareReplace(ByteArray* self, sysuint_t index, sysuint_t range, sysuint_t replacementLength)
{
  ByteArray::Data* d = self->_d;

  sysuint_t lengthBefore = d->length;

  FOG_ASSERT(index <= lengthBefore);
  if (lengthBefore - index > range) range = lengthBefore - index;

  sysuint_t lengthAfter = lengthBefore - range + replacementLength;

  if (d->refCount.get() > 1 || d->capacity < lengthAfter)
  {
    sysuint_t optimalCapacity = Util::getGrowCapacity(
      sizeof(ByteArray::Data), sizeof(char), lengthBefore, lengthAfter);

    d = ByteArray::Data::alloc(optimalCapacity, d->data, index);
    if (FOG_IS_NULL(d)) return NULL;

    StringUtil::copy(
      d->data + index + replacementLength,
      self->_d->data + index + range, lengthBefore - index - range);
    atomicPtrXchg(&self->_d, d)->derefInline();
  }
  else
  {
    StringUtil::move(
      d->data + index + replacementLength,
      d->data + index + range, lengthBefore - index - range);
  }

  d->hashCode = 0;
  d->length = lengthAfter;
  d->data[lengthAfter] = 0;
  return d->data + index;
}

// ============================================================================
// [Fog::ByteArray::Set]
// ============================================================================

err_t ByteArray::set(char ch, sysuint_t length)
{
  if (length == DETECT_LENGTH) return ERR_RT_INVALID_ARGUMENT;

  char* p = _prepareSet(this, length);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::fill(p, ch, length);
  return ERR_OK;
}

err_t ByteArray::set(const Stub8& str)
{
  const char* sData = str.getData();
  sysuint_t sLength = str.getComputedLength();

  char* p = _prepareSet(this, sLength);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::copy(p, sData, sLength);
  return ERR_OK;
}

err_t ByteArray::set(const ByteArray& other)
{
  Data* self_d = _d;
  Data* other_d = other._d;
  if (self_d == other_d) return ERR_OK;

  if ((self_d->flags & Data::IsStrong) != 0 || (other_d->flags & Data::IsSharable) == 0)
  {
    return setDeep(other);
  }
  else
  {
    atomicPtrXchg(&_d, other_d->refAlways())->derefInline();
    return ERR_OK;
  }
}

err_t ByteArray::setDeep(const ByteArray& other)
{
  Data* self_d = _d;
  Data* other_d = other._d;
  if (self_d == other_d) return ERR_OK;

  char* p = _prepareSet(this, other_d->length);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::copy(p, other.getData(), other.getLength());
  return ERR_OK;
}

err_t ByteArray::setBool(bool b)
{
  return set(Ascii8(b ? "true" : "false"));
}

err_t ByteArray::setInt(int32_t n, int base)
{
  clear();
  return appendInt((int64_t)n, base, FormatFlags());
}

err_t ByteArray::setInt(uint32_t n, int base)
{
  clear();
  return appendInt((uint64_t)n, base, FormatFlags());
}

err_t ByteArray::setInt(int64_t n, int base)
{
  clear();
  return appendInt(n, base, FormatFlags());
}

err_t ByteArray::setInt(uint64_t n, int base)
{
  clear();
  return appendInt(n, base, FormatFlags());
}

err_t ByteArray::setInt(int32_t n, int base, const FormatFlags& ff)
{
  clear();
  return appendInt((int64_t)n, base, ff);
}

err_t ByteArray::setInt(uint32_t n, int base, const FormatFlags& ff)
{
  clear();
  return appendInt((uint64_t)n, base, ff);
}

err_t ByteArray::setInt(int64_t n, int base, const FormatFlags& ff)
{
  clear();
  return appendInt(n, base, ff);
}

err_t ByteArray::setInt(uint64_t n, int base, const FormatFlags& ff)
{
  clear();
  return appendInt(n, base, ff);
}

err_t ByteArray::setDouble(double d, int doubleForm)
{
  clear();
  return appendDouble(d, doubleForm, FormatFlags());
}

err_t ByteArray::setDouble(double d, int doubleForm, const FormatFlags& ff)
{
  clear();
  return appendDouble(d, doubleForm, ff);
}

err_t ByteArray::format(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  clear();
  err_t err = appendVformatc(fmt, TextCodec::local8(), ap);

  va_end(ap);
  return err;
}

err_t ByteArray::formatc(const char* fmt, const TextCodec& tc, ...)
{
  va_list ap;
  va_start(ap, tc);

  clear();
  err_t err = appendVformatc(fmt, tc, ap);

  va_end(ap);
  return err;
}

err_t ByteArray::vformat(const char* fmt, va_list ap)
{
  clear();
  return appendVformatc(fmt, TextCodec::local8(), ap);
}

err_t ByteArray::vformatc(const char* fmt, const TextCodec& tc, va_list ap)
{
  clear();
  return appendVformatc(fmt, tc, ap);
}

err_t ByteArray::wformat(const ByteArray& fmt, char lex, const List<ByteArray>& args)
{
  clear();
  return appendWformat(fmt, lex, args.getData(), args.getLength());
}

err_t ByteArray::wformat(const ByteArray& fmt, char lex, const ByteArray* args, sysuint_t length)
{
  clear();
  return appendWformat(fmt, lex, args, length);
}

// ============================================================================
// [Fog::ByteArray::Append]
// ============================================================================

static err_t append_ntoa(ByteArray* self, uint64_t n, int base, const FormatFlags& ff, StringUtil::NTOAOut* out)
{
  char prefixBuffer[4];
  char* prefix = prefixBuffer;

  sysuint_t width = ff.width;
  sysuint_t precision = ff.precision;
  uint32_t fmt = ff.flags;

  if (out->negative)
    *prefix++ = '-';
  else if (fmt & FORMAT_SHOW_SIGN)
    *prefix++ = '+';
  else if (fmt & FORMAT_BLANK_POSITIVE)
    *prefix++ = ' ';

  if (fmt & FORMAT_ALTERNATE_FORM)
  {
    if (base == 8)
    {
      if (n != 0) { out->result--; *(out->result) = '0'; out->length++; }
    }
    else if (base == 16)
    {
      *prefix++ = '0';
      *prefix++ = (fmt & FORMAT_CAPITALIZE_E_OR_X) ? 'X' : 'x';
    }
  }

  sysuint_t prefixLength = (sysuint_t)(prefix - prefixBuffer);
  sysuint_t resultLength = out->length;

  if (width == NO_WIDTH) width = 0;
  if ((fmt & FORMAT_ZERO_PADDED) &&
      precision == NO_PRECISION &&
      width > prefixLength + resultLength) precision = width - prefixLength;
  if (precision == NO_PRECISION) precision = 0;

  sysuint_t fillLength = (resultLength < precision) ? precision - resultLength : 0;
  sysuint_t fullLength = prefixLength + resultLength + fillLength;
  sysuint_t widthLength = (fullLength < width) ? width - fullLength : 0;

  fullLength += widthLength;

  char* p = _prepareAppend(self, fullLength);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  // Left justification.
  if (!(fmt & FORMAT_LEFT_ADJUSTED))
  {
    StringUtil::fill(p, ' ', widthLength); p += widthLength;
  }

  // Number with prefix and precision.
  StringUtil::copy(p, prefixBuffer, prefixLength); p += prefixLength;

  // Body.
  StringUtil::fill(p, '0', fillLength); p += fillLength;
  StringUtil::copy(p, out->result, resultLength); p += resultLength;

  // Right justification.
  if (fmt & FORMAT_LEFT_ADJUSTED)
  {
    StringUtil::fill(p, ' ', widthLength);
  }

  return ERR_OK;
}

static char* append_exponent(char* dest, uint exp, char zero)
{
  uint t;

  if (exp > 99) { t = exp / 100; *dest++ = zero + char(t); exp -= t * 100; }
  t = exp / 10; *dest++ = zero + char(t); exp -= t * 10;
  *dest++ = zero + char(exp);

  return dest;
}

err_t ByteArray::append(char ch, sysuint_t length)
{
  if (length == DETECT_LENGTH) return ERR_RT_INVALID_ARGUMENT;

  char* p = _prepareAppend(this, length);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::fill(p, ch, length);
  return ERR_OK;
}

err_t ByteArray::append(const Stub8& other)
{
  const char* s = other.getData();
  sysuint_t length = other.getComputedLength();

  char* p = _prepareAppend(this, length);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::copy(p, s, length);
  return ERR_OK;
}

err_t ByteArray::append(const ByteArray& _other)
{
  if (getLength() == 0) return set(_other);

  ByteArray other(_other);

  char* p = _prepareAppend(this, other.getLength());
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::copy(p, other.getData(), other.getLength());
  return ERR_OK;
}

err_t ByteArray::appendBool(bool b)
{
  return append(Ascii8(b ? "true" : "false"));
}

err_t ByteArray::appendInt(int32_t n, int base)
{
  return appendInt((int64_t)n, base, FormatFlags());
}

err_t ByteArray::appendInt(uint32_t n, int base)
{
  return appendInt((uint64_t)n, base, FormatFlags());
}

err_t ByteArray::appendInt(int64_t n, int base)
{
  return appendInt(n, base, FormatFlags());
}

err_t ByteArray::appendInt(uint64_t n, int base)
{
  return appendInt(n, base, FormatFlags());
}

err_t ByteArray::appendInt(int32_t n, int base, const FormatFlags& ff)
{
  return appendInt((int64_t)n, base, ff);
}

err_t ByteArray::appendInt(uint32_t n, int base, const FormatFlags& ff)
{
  return appendInt((uint64_t)n, base, ff);
}

err_t ByteArray::appendInt(int64_t n, int base, const FormatFlags& ff)
{
  StringUtil::NTOAOut out;
  StringUtil::itoa(n, base, (ff.flags & FORMAT_CAPITALIZE) != 0, &out);

  return append_ntoa(this, (uint64_t)n, base, ff, &out);
}

err_t ByteArray::appendInt(uint64_t n, int base, const FormatFlags& ff)
{
  StringUtil::NTOAOut out;
  StringUtil::utoa(n, base, (ff.flags & FORMAT_CAPITALIZE) != 0, &out);

  return append_ntoa(this, n, base, ff, &out);
}

err_t ByteArray::appendDouble(double d, int doubleForm)
{
  return appendDouble(d, doubleForm, FormatFlags());
}

// Defined in StringUtil_dtoa.cpp;
namespace StringUtil { FOG_NO_EXPORT double _mprec_log10(int dig); }

err_t ByteArray::appendDouble(double d, int doubleForm, const FormatFlags& ff)
{
  err_t err = ERR_OK;

  StringUtil::NTOAOut out;

  sysuint_t width = ff.width;
  sysuint_t precision = ff.precision;
  uint32_t fmt = ff.flags;

  sysuint_t beginLength = _d->length;
  sysuint_t numberLength;
  sysuint_t i;
  sysuint_t savedPrecision = precision;
  int decpt;

  char* bufCur;
  char* bufEnd;

  char* dest;
  char sign = 0;

  if (precision == NO_PRECISION) precision = 6;

  if (d < 0.0)
    { sign = '-'; d = -d; }
  else if (fmt & FORMAT_SHOW_SIGN)
    sign = '+';
  else if (fmt & FORMAT_BLANK_POSITIVE)
    sign = ' ';

  if (sign != 0) append(sign);

  // Decimal form.
  if (doubleForm == DF_DECIMAL)
  {
    StringUtil::dtoa(d, 3, (uint32_t)precision, &out);

    decpt = out.decpt;
    if (out.decpt == 9999) goto __InfOrNaN;

    bufCur = out.result;
    bufEnd = bufCur + out.length;

    // Reserve some space for number.
    i = precision + 16;
    if (decpt > 0) i += (sysuint_t)decpt;

    dest = beginManipulation(i, OUTPUT_MODE_APPEND);
    if (FOG_IS_NULL(dest)) { err = ERR_RT_OUT_OF_MEMORY; goto __ret; }

    while (bufCur != bufEnd && decpt > 0) { *dest++ = *bufCur++; decpt--; }
    // Even if not in buffer.
    while (decpt > 0) { *dest++ = '0'; decpt--; }

    if ((fmt & FORMAT_ALTERNATE_FORM) != 0 || bufCur != bufEnd || precision > 0)
    {
      if (bufCur == out.result) *dest++ = '0';
      *dest++ = '.';
      while (decpt < 0 && precision > 0) { *dest++ = '0'; decpt++; precision--; }

      // Print rest of stuff.
      while (bufCur != bufEnd && precision > 0) { *dest++ = *bufCur++; precision--; }
      // And trailing zeros.
      while (precision > 0) { *dest++ = '0'; precision--; }
    }

    finishDataX(dest);
  }
  // Exponential form.
  else if (doubleForm == DF_EXPONENT)
  {
__exponentialForm:
    StringUtil::dtoa(d, 2, precision + 1, &out);

    decpt = out.decpt;
    if (decpt == 9999) goto __InfOrNaN;

    // Reserve some space for number, we need +X.{PRECISION}e+123
    dest = beginManipulation(precision + 10, OUTPUT_MODE_APPEND);
    if (FOG_IS_NULL(dest)) { err = ERR_RT_OUT_OF_MEMORY; goto __ret; }

    bufCur = out.result;
    bufEnd = bufCur + out.length;

    *dest++ = *bufCur++;
    if ((fmt & FORMAT_ALTERNATE_FORM) != 0 || precision > 0)
    {
      if (bufCur != bufEnd || doubleForm == DF_EXPONENT) *dest++ = '.';
    }
    while (bufCur != bufEnd && precision > 0)
    {
      *dest++ = *bufCur++;
      precision--;
    }

    // Add trailing zeroes to fill out to ndigits unless this is
    // DF_SIGNIFICANT_DIGITS.
    if (doubleForm == DF_EXPONENT)
    {
      for (i = precision; i; i--) *dest++ = '0';
    }

    // Add the exponent.
    if (doubleForm == DF_EXPONENT || decpt > 1)
    {
      *dest++ = (ff.flags & FORMAT_CAPITALIZE_E_OR_X) ? 'E' : 'e';
      decpt--;
      if (decpt < 0)
        { *dest++ = '-'; decpt = -decpt; }
      else
        *dest++ = '+';

      dest = append_exponent(dest, decpt, '0');
    }

    finishDataX(dest);
  }
  // Significant digits form.
  else /* if (doubleForm == DF_SIGNIFICANT_DIGITS) */
  {
    char* save;
    if (d <= 0.0001 || d >= StringUtil::_mprec_log10(precision))
    {
      if (precision > 0) precision--;
      goto __exponentialForm;
    }

    if (d < 1.0)
    {
      // What we want is ndigits after the point.
      StringUtil::dtoa(d, 3, precision, &out);
    }
    else
    {
      StringUtil::dtoa(d, 2, precision, &out);
    }

    decpt = out.decpt;
    if (decpt == 9999) goto __InfOrNaN;

    // Reserve some space for number.
    i = precision + 16;
    if (decpt > 0) i += (sysuint_t)decpt;

    dest = beginManipulation(i, OUTPUT_MODE_APPEND);
    if (FOG_IS_NULL(dest)) { err = ERR_RT_OUT_OF_MEMORY; goto __ret; }

    save = dest;

    bufCur = out.result;
    bufEnd = bufCur + out.length;

    while (bufCur != bufEnd && decpt > 0) { *dest++ = *bufCur++; decpt--; precision--; }
    // Even if not in buffer.
    while (decpt > 0 && precision > 0) { *dest++ = '0'; decpt--; precision--; }

    if ((fmt & FORMAT_ALTERNATE_FORM) != 0 || bufCur != bufEnd)
    {
      if (dest == save) *dest++ = '0';
      *dest++ = '.';
      while (decpt < 0 && precision > 0) { *dest++ = '0'; decpt++; precision--; }

      // Print rest of stuff.
      while (bufCur != bufEnd && precision > 0){ *dest++ = *bufCur++; precision--; }
      // And trailing zeros.
      // while (precision > 0) { *dest++ = '0'; precision--; }
    }

    finishDataX(dest);
  }
  goto __ret;

__InfOrNaN:
  err |= append(Stub8((const char*)out.result, out.length));
__ret:
  // Apply padding.
  numberLength = _d->length - beginLength;
  if (width != (sysuint_t)-1 && width > numberLength)
  {
    sysuint_t fill = width - numberLength;

    if ((fmt & FORMAT_LEFT_ADJUSTED) == 0)
    {
      if (savedPrecision == NO_PRECISION)
        err |= insert(beginLength + (sign != 0), '0', fill);
      else
        err |= insert(beginLength, ' ', fill);
    }
    else
    {
      err |= append(' ', fill);
    }
  }
  return err;
}

err_t ByteArray::appendFormat(const char* fmt, ...)
{
  FOG_ASSERT(fmt);

  va_list ap;
  va_start(ap, fmt);
  err_t err = appendVformat(fmt, ap);
  va_end(ap);

  return err;
}

err_t ByteArray::appendFormatc(const char* fmt, const TextCodec& tc, ...)
{
  FOG_ASSERT(fmt);

  va_list ap;
  va_start(ap, tc);
  err_t err = appendVformatc(fmt, tc, ap);
  va_end(ap);

  return err;
}

err_t ByteArray::appendVformat(const char* fmt, va_list ap)
{
  return appendVformatc(fmt, TextCodec::local8(), ap);
}

err_t ByteArray::appendVformatc(const char* fmt, const TextCodec& tc, va_list ap)
{
#define __VFORMAT_PARSE_NUMBER(_Ptr_, _Out_)         \
  {                                                  \
    /* ----- Clean-up ----- */                       \
    _Out_ = 0;                                       \
                                                     \
    /* ----- Remove zeros ----- */                   \
    while (*_Ptr_ == '0') _Ptr_++;                   \
                                                     \
    /* ----- Parse number ----- */                   \
    while ((c = (uint8_t)*_Ptr_) >= '0' && c <= '9') \
    {                                                \
      _Out_ = 10 * _Out_ + (c - '0');                \
      _Ptr_++;                                       \
    }                                                \
  }

  if (fmt == NULL) return ERR_RT_INVALID_ARGUMENT;

  const char* fmtBeginChunk = fmt;
  uint32_t c;
  sysuint_t beginLength = getLength();

  for (;;)
  {
    c = (uint8_t)*fmt;

    if (c == '%')
    {
      uint directives = 0;
      uint sizeFlags = 0;
      sysuint_t fieldWidth = NO_WIDTH;
      sysuint_t precision = NO_PRECISION;
      uint base = 10;

      if (fmtBeginChunk != fmt) append(Ascii8(fmtBeginChunk, (sysuint_t)(fmt - fmtBeginChunk)));

      // Parse directives.
      for (;;)
      {
        c = (uint8_t)*(++fmt);

        if      (c == '#')  directives |= FORMAT_ALTERNATE_FORM;
        else if (c == '0')  directives |= FORMAT_ZERO_PADDED;
        else if (c == '-')  directives |= FORMAT_LEFT_ADJUSTED;
        else if (c == ' ')  directives |= FORMAT_BLANK_POSITIVE;
        else if (c == '+')  directives |= FORMAT_SHOW_SIGN;
        else if (c == '\'') directives |= FORMAT_THOUSANDS_GROUP;
        else break;
      }

      // Parse field width.
      if (Byte::isDigit(c))
      {
        __VFORMAT_PARSE_NUMBER(fmt, fieldWidth)
      }
      else if (c == '*')
      {
        c = *++fmt;

        int _fieldWidth = va_arg(ap, int);
        if (_fieldWidth < 0) _fieldWidth = 0;
        if (_fieldWidth > 4096) _fieldWidth = 4096;
        fieldWidth = (sysuint_t)_fieldWidth;
      }

      // Parse precision.
      if (c == '.')
      {
        c = *++fmt;

        if (Byte::isDigit(c))
        {
          __VFORMAT_PARSE_NUMBER(fmt, precision);
        }
        else if (c == '*')
        {
          c = *++fmt;

          int _precision = va_arg(ap, int);
          if (_precision < 0) _precision = 0;
          if (_precision > 4096) _precision = 4096;
          precision = (sysuint_t)_precision;
        }
      }

      // Parse argument type.
      enum
      {
        ARG_SIZE_H   = 0x01,
        ARG_SIZE_HH  = 0x02,
        ARG_SIZE_L   = 0x04,
        ARG_SIZE_LL  = 0x08,
        ARG_SIZE_M   = 0x10,
#if (CORE_ARCH_BITS == 32)
        ARG_SIZE_64  = ARG_SIZE_LL
#else
        ARG_SIZE_64  = ARG_SIZE_L
#endif
      };

      // 'h' and 'hh'.
      if (c == 'h')
      {
        c = (uint8_t)*(++fmt);
        if (c == 'h')
        {
          c = (uint8_t)*(++fmt);
          sizeFlags |= ARG_SIZE_HH;
        }
        else
        {
          sizeFlags |= ARG_SIZE_H;
        }
      }
      // 'L'.
      else if (c == 'L')
      {
        c = (uint8_t)*(++fmt);
        sizeFlags |= ARG_SIZE_LL;
      }
      // 'l' and 'll'.
      else if (c == 'l')
      {
        c = (uint8_t)*(++fmt);
        if (c == 'l')
        {
          c = (uint8_t)*(++fmt);
          sizeFlags |= ARG_SIZE_LL;
        }
        else
        {
          sizeFlags |= ARG_SIZE_L;
        }
      }
      // 'j'.
      else if (c == 'j')
      {
        c = (uint8_t)*(++fmt);
        sizeFlags |= ARG_SIZE_LL;
      }
      // 'z'.
      else if (c == 'z' || c == 'Z')
      {
        c = (uint8_t)*(++fmt);
        if (sizeof(size_t) > sizeof(long))
          sizeFlags |= ARG_SIZE_LL;
        else if (sizeof(size_t) > sizeof(int))
          sizeFlags |= ARG_SIZE_L;
      }
      // 't'.
      else if (c == 't')
      {
        c = (uint8_t)*(++fmt);
        if (sizeof(size_t) > sizeof(long))
          sizeFlags |= ARG_SIZE_LL;
        else if (sizeof(size_t) > sizeof(int))
          sizeFlags |= ARG_SIZE_L;
      }
      // 'M' = max type (Core extension).
      else if (c == 'M')
      {
        c = (uint8_t)*(++fmt);
        sizeFlags |= ARG_SIZE_M;
      }

      // Parse conversion character.
      switch (c)
      {
        // Signed integers.
        case 'd':
        case 'i':
        {
          int64_t i = (sizeFlags >= ARG_SIZE_64) ? va_arg(ap, int64_t) : va_arg(ap, int32_t);

          if (precision == NO_PRECISION && fieldWidth == NO_WIDTH && directives == 0)
            appendInt(i, base);
          else
            appendInt(i, base, FormatFlags(precision, fieldWidth, directives));
          break;
        }

        // Unsigned integers.
        case 'o':
          base = 8;
          goto ffUnsigned;
        case 'X':
          directives |= FORMAT_CAPITALIZE;
        case 'x':
          base = 16;
        case 'u':
ffUnsigned:
        {
          uint64_t i = (sizeFlags >= ARG_SIZE_64) ? va_arg(ap, uint64_t) : va_arg(ap, uint32_t);

          if (precision == NO_PRECISION && fieldWidth == NO_WIDTH && directives == 0)
            appendInt(i, base);
          else
            appendInt(i, base, FormatFlags(precision, fieldWidth, directives));
          break;
        }

        // Floats, doubles, long doubles.
        case 'F':
        case 'E':
        case 'G':
          directives |= FORMAT_CAPITALIZE_E_OR_X;
        case 'f':
        case 'e':
        case 'g':
        {
          double i;
          uint doubleForm = 0; // Be quite

          if (c == 'e' || c == 'E')
            doubleForm = DF_EXPONENT;
          else if (c == 'f' || c == 'F')
            doubleForm = DF_DECIMAL;
          else if (c == 'g' || c == 'G')
            doubleForm = DF_SIGNIFICANT_DIGITS;

          i = va_arg(ap, double);
          appendDouble(i, doubleForm, FormatFlags(precision, fieldWidth, directives));
          break;
        }

        // Characters (latin1 or unicode...).
        case 'C':
          sizeFlags |= ARG_SIZE_L;
        case 'c':
        {
          if (precision == NO_PRECISION) precision = 1;
          if (fieldWidth == NO_WIDTH) fieldWidth = 0;

          sysuint_t fill = (fieldWidth > precision) ? fieldWidth - precision : 0;

          if (fill && (directives & FORMAT_LEFT_ADJUSTED) == 0) append(char(' '), fill);
          append(char(va_arg(ap, uint)), precision);
          if (fill && (directives & FORMAT_LEFT_ADJUSTED) != 0) append(char(' '), fill);
          break;
        }

        // Strings.
        case 'S':
#if FOG_SIZEOF_WCHAR_T == 2
          sizeFlags |= ARG_SIZE_L;
#else
          sizeFlags |= ARG_SIZE_LL;
#endif
        case 's':
          if (fieldWidth == NO_WIDTH) fieldWidth = 0;

          // TODO: Not correct.
          if (sizeFlags >= ARG_SIZE_LL)
          {
#if 0
            // UTF-32 string (uint32_t*).
            const uint32_t* s = va_arg(ap, const uint32_t*);
            sysuint_t slen = (precision != NO_PRECISION)
              ? (sysuint_t)StringUtil::nlen(s, precision)
              : (sysuint_t)StringUtil::len(s);
            sysuint_t fill = (fieldWidth > slen) ? fieldWidth - slen : 0;

            if (fill && (directives & FORMAT_LEFT_ADJUSTED) == 0) append(' ', fill);
            append(Utf32(s, slen), tc);
            if (fill && (directives & FORMAT_LEFT_ADJUSTED) != 0) append(' ', fill);
#endif
          }
          else if (sizeFlags >= ARG_SIZE_L)
          {
#if 0
            // UTF-16 string (uint16_t*).
            const Char* s = va_arg(ap, const Char*);
            sysuint_t slen = (precision != NO_PRECISION)
              ? (sysuint_t)StringUtil::nlen(s, precision)
              : (sysuint_t)StringUtil::len(s);
            sysuint_t fill = (fieldWidth > slen) ? fieldWidth - slen : 0;

            if (fill && (directives & FORMAT_LEFT_ADJUSTED) == 0) append(' ', fill);
            append(Utf16(s, slen), tc);
            if (fill && (directives & FORMAT_LEFT_ADJUSTED) != 0) append(' ', fill);
#endif
          }
          else
          {
            // 8-bit string (char*).
            const char* s = va_arg(ap, const char*);
            sysuint_t slen = (precision != NO_PRECISION)
              ? (sysuint_t)StringUtil::nlen(s, precision)
              : (sysuint_t)StringUtil::len(s);
            sysuint_t fill = (fieldWidth > slen) ? fieldWidth - slen : 0;

            if (fill && (directives & FORMAT_LEFT_ADJUSTED) == 0) append(' ', fill);
            append(s, slen);
            if (fill && (directives & FORMAT_LEFT_ADJUSTED) != 0) append(' ', fill);
          }
          break;

        // Pointer.
        case 'p':
          directives |= FORMAT_ALTERNATE_FORM;
#if FOG_ARCH_BITS == 32
          sizeFlags = 0;
#elif FOG_ARCH_BITS == 64
          sizeFlags = ARG_SIZE_LL;
#endif // FOG_ARCH_BITS
          goto ffUnsigned;

        // Position receiver 'n'.
        case 'n':
        {
          void* pointer = va_arg(ap, void*);
          sysuint_t n = getLength() - beginLength;

          switch (sizeFlags)
          {
            case ARG_SIZE_M:
            case ARG_SIZE_LL: *(uint64_t *)pointer = (uint64_t)(n); break;
            case ARG_SIZE_L:  *(ulong    *)pointer = (ulong   )(n); break;
            case ARG_SIZE_HH: *(uchar    *)pointer = (uchar   )(n); break;
            case ARG_SIZE_H:  *(uint16_t *)pointer = (uint16_t)(n); break;
            default:          *(uint     *)pointer = (uint    )(n); break;
          }
          break;
        }

        // Extensions
        case 'W':
        {
          if (fieldWidth == NO_WIDTH) fieldWidth = 0;

          ByteArray* string = va_arg(ap, ByteArray*);

          const char* s = string->getData();
          sysuint_t slen = string->getLength();
          if (precision != NO_PRECISION)  slen = Math::min(slen, precision);
          sysuint_t fill = (fieldWidth > slen) ? fieldWidth - slen : 0;

          if (fill && (directives & FORMAT_LEFT_ADJUSTED) == 0) append(char(' '), fill);
          append(Stub8(s, slen));
          if (fill && (directives & FORMAT_LEFT_ADJUSTED) != 0) append(char(' '), fill);
          break;
        }

        // Percent.
        case '%':
          // skip one "%" if its legal "%%", otherwise send everything
          // to output.
          if (fmtBeginChunk + 1 == fmt) fmtBeginChunk++;
          break;

        // Unsupported or end of input.
        default:
          goto end;
      }
      fmtBeginChunk = fmt+1;
    }

end:
    if (c == '\0')
    {
      if (fmtBeginChunk != fmt) append(Stub8(fmtBeginChunk, (sysuint_t)(fmt - fmtBeginChunk)));
      break;
    }

    fmt++;
  }
  return ERR_OK;

#undef __VFORMAT_PARSE_NUMBER
}

err_t ByteArray::appendWformat(const ByteArray& fmt, char lex, const List<ByteArray>& args)
{
  return appendWformat(fmt, lex, args.getData(), args.getLength());
}

err_t ByteArray::appendWformat(const ByteArray& fmt, char lex, const ByteArray* args, sysuint_t length)
{
  const char* fmtBeg = fmt.getData();
  const char* fmtEnd = fmtBeg + fmt.getLength();
  const char* fmtCur;

  err_t err = ERR_OK;

  for (fmtCur = fmtBeg; fmtCur != fmtEnd; )
  {
    if (*fmtCur == lex)
    {
      fmtBeg = fmtCur;
      if ( (err = append(Stub8(fmtBeg, (sysuint_t)(fmtCur - fmtBeg)))) ) goto done;

      if (++fmtCur != fmtEnd)
      {
        char ch = *fmtCur;

        if (ch >= char('0') && ch <= char('9'))
        {
          uint32_t n = (uint32_t)(uint8_t)ch - (uint32_t)'0';
          if (n < length)
          {
            if ( (err = append(args[n])) ) goto done;
            fmtBeg = fmtCur + 1;
          }
        }
        else if (ch == lex)
          fmtBeg++;
      }
      else
        break;
    }
    fmtCur++;
  }

  if (fmtCur != fmtBeg)
  {
    err = append(Stub8(fmtBeg, (sysuint_t)(fmtCur - fmtBeg)));
  }

done:
  return err;
}

// ============================================================================
// [Fog::ByteArray::Prepend]
// ============================================================================

err_t ByteArray::prepend(char ch, sysuint_t length)
{
  return insert(0, ch, length);
}

err_t ByteArray::prepend(const Stub8& other)
{
  return insert(0, other);
}

err_t ByteArray::prepend(const ByteArray& other)
{
  return insert(0, other);
}

// ============================================================================
// [Fog::ByteArray::Insert]
// ============================================================================

err_t ByteArray::insert(sysuint_t index, char ch, sysuint_t length)
{
  if (length == DETECT_LENGTH) return ERR_RT_INVALID_ARGUMENT;

  char* p = _prepareInsert(this, index, length);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::fill(p, ch, length);
  return ERR_OK;
}

err_t ByteArray::insert(sysuint_t index, const Stub8& other)
{
  const char* s = other.getData();
  sysuint_t length = other.getComputedLength();

  char* p = _prepareInsert(this, index, length);
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::copy(p, s, length);
  return ERR_OK;
}

err_t ByteArray::insert(sysuint_t index, const ByteArray& _other)
{
  ByteArray other(_other);

  char* p = _prepareInsert(this, index, other.getLength());
  if (FOG_IS_NULL(p)) return ERR_RT_OUT_OF_MEMORY;

  StringUtil::copy(p, other.getData(), other.getLength());
  return ERR_OK;
}

// ============================================================================
// [Fog::ByteArray - Remove]
// ============================================================================

sysuint_t ByteArray::remove(const Range& range)
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  sysuint_t lenPart1 = rstart;
  sysuint_t lenPart2 = getLength() - rend;
  sysuint_t lenAfter = lenPart1 + lenPart2;

  if (_d->refCount.get() > 1)
  {
    Data* newd = Data::alloc(lenAfter);
    if (FOG_IS_NULL(newd)) return 0;

    StringUtil::copy(newd->data, _d->data, lenPart1);
    StringUtil::copy(newd->data + rstart, _d->data + rend, lenPart2);
    newd->length = lenAfter;
    newd->data[lenAfter] = 0;

    atomicPtrXchg(&_d, newd)->deref();
  }
  else
  {
    StringUtil::move(_d->data + rstart, _d->data + rend, lenPart2);
    _d->length = lenAfter;
    _d->data[lenAfter] = 0;
    _d->hashCode = 0;
  }

  return rend - rstart;
}

sysuint_t ByteArray::remove(char ch, uint cs, const Range& range)
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  Data* d = _d;
  sysuint_t length = d->length;

  char* strBeg = d->data;
  char* strCur = strBeg + rstart;
  char* strEnd = strBeg + rend;
  char* destCur;

  if (cs == CASE_SENSITIVE)
  {
caseSensitive:
    while (strCur != strEnd)
    {
      if (*strCur == ch) goto caseSensitiveRemove;
      strCur++;
    }
    return 0;

caseSensitiveRemove:
    if (d->refCount.get() > 1)
    {
      rstart = strCur - strBeg;
      if (detach() != ERR_OK) return 0;

      d = _d;
      strBeg = d->data;

      strCur = strBeg + rstart;
      strEnd = strBeg + rend;
    }
    destCur = strCur;

    while (strCur != strEnd)
    {
      if (*strCur != ch) *destCur++ = *strCur;
      strCur++;
    }
  }
  else
  {
    char chLower = Byte::toLower(ch);
    char chUpper = Byte::toUpper(ch);
    if (chLower == chUpper) goto caseSensitive;

    while (strCur != strEnd)
    {
      if (*strCur == chLower || *strCur == chUpper) goto caseInsensitiveRemove;
      strCur++;
    }
    return 0;

caseInsensitiveRemove:
    if (d->refCount.get() > 1)
    {
      rstart = strCur - strBeg;
      if (detach() != ERR_OK) return 0;

      d = _d;
      strBeg = d->data;

      strCur = strBeg + rstart;
      strEnd = strBeg + rend;
    }
    destCur = strCur;

    while (strCur != strEnd)
    {
      if (*strCur != chLower && *strCur != chUpper) *destCur++ = *strCur;
      strCur++;
    }
  }

  sysuint_t tail = length - rend;
  StringUtil::copy(destCur, strCur, tail);

  sysuint_t after = (sysuint_t)(destCur - d->data) + tail;
  d->length = after;
  d->data[after] = 0;
  d->hashCode = 0;
  return length - after;
}

sysuint_t ByteArray::remove(const ByteArray& other, uint cs, const Range& range)
{
  sysuint_t len = other.getLength();
  if (len == 0) return 0;
  if (len == 1) return remove(other.getAt(0), cs, range);

  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  if (rend - rstart >= 256)
  {
    // Match using StringMatcher.
    ByteArrayMatcher matcher;
    if (matcher.setPattern(other)) return 0;

    return remove(matcher, cs, range);
  }
  else
  {
    // Match using naive algorithm.
    const char* aStr = getData();
    const char* bStr = other.getData();

    // Maximal length is 256 and minimal pattern size is 2.
    Range ranges[128];
    sysuint_t count = 0;

    for (;;)
    {
      sysuint_t i = StringUtil::indexOf(aStr + rstart, rend - rstart, bStr, len);
      if (i == INVALID_INDEX) break;
      rstart += i;
      ranges[count++].setRange(rstart, rstart + len);
      rstart += len;
    }

    return remove(ranges, count);
  }
}

sysuint_t ByteArray::remove(const ByteArrayFilter& filter, uint cs, const Range& range)
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  const char* str = getData();
  sysuint_t len = getLength();

  List<Range> ranges;
  for (;;)
  {
    Range r = filter.indexOf(str, len, cs, Range(rstart, rend));
    if (r.getStart() == INVALID_INDEX) break;

    ranges.append(r);
    rstart = r.getEnd();
  }
  return remove(ranges.getData(), ranges.getLength());
}

sysuint_t ByteArray::remove(const Range* range, sysuint_t count)
{
  if (range == NULL || count == 0) return 0;

  sysuint_t i;
  sysuint_t len = getLength();

  if (_d->refCount.get() == 1)
  {
    char* s = _d->data;
    sysuint_t dstPos = range[0].getStart();
    sysuint_t srcPos = dstPos;

    i = 0;
    do {
      FOG_ASSERT(range[i].isValid());

      srcPos += range[i].getLengthNoCheck();
      sysuint_t j = ((++i == count) ? len : range[i].getStart()) - srcPos;

      StringUtil::copy(s + dstPos, s + srcPos, j);

      dstPos += j;
      srcPos += j;
    } while (i != count);

    _d->length = dstPos;
    _d->hashCode = 0;
    s[dstPos] = 0;
  }
  else
  {
    sysuint_t deleteLength = 0;
    sysuint_t lengthAfter;

    for (i = 0; i < count; i++) deleteLength += range[i].getLengthNoCheck();
    FOG_ASSERT(len >= deleteLength);

    lengthAfter = len - deleteLength;

    Data* newd = Data::alloc(lengthAfter);
    if (FOG_IS_NULL(newd)) return 0;

    i = 0;
    char* dstData = newd->data;
    char* srgetData = _d->data;

    sysuint_t dstPos = range[0].getStart();
    sysuint_t srcPos = dstPos;

    StringUtil::copy(dstData, srgetData, dstPos);

    do {
      FOG_ASSERT(range[i].isValid());

      srcPos += range[i].getLengthNoCheck();
      sysuint_t j = ((++i == count) ? len : range[i].getStart()) - srcPos;

      StringUtil::copy(dstData + dstPos, srgetData + srcPos, j);

      dstPos += j;
      srcPos += j;
    } while (i != count);

    newd->length = lengthAfter;
    newd->data[lengthAfter] = 0;

    atomicPtrXchg(&_d, newd)->deref();
  }
  return count;
}

// ============================================================================
// [Fog::ByteArray - Replace]
// ============================================================================

err_t ByteArray::replace(const Range& range, const ByteArray& replacement)
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return ERR_OK;

  const char* replacementData = replacement.getData();
  sysuint_t replacementLength = replacement.getLength();

  if (_d->refCount.get() == 1 && _d != replacement._d)
  {
    sysuint_t len = getLength();
    sysuint_t lenAfter = len - (rend - rstart) + replacementLength;
    if (lenAfter < len) return ERR_RT_OVERFLOW;

    if (_d->capacity >= lenAfter)
    {
      char* sdata = _d->data;
      char* sstart = sdata + rstart;

      StringUtil::move(sstart + replacementLength, sdata + rend, len - rend);
      StringUtil::copy(sstart, replacementData, replacementLength);

      _d->length = lenAfter;
      _d->hashCode = 0;
      _d->data[lenAfter] = 0;
      return ERR_OK;
    }
  }

  Range r(rstart, rend);
  return replace(&r, 1, replacementData, replacementLength);
}

err_t ByteArray::replace(char before, char after, uint cs, const Range& range)
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return ERR_OK;

  Data* d = _d;

  char* strCur = d->data;
  char* strEnd = strCur;

  strCur += rstart;
  strEnd += rend;

  if (cs == CASE_SENSITIVE)
  {
caseSensitive:
    while (strCur != strEnd)
    {
      if (*strCur == before) goto caseSensitiveReplace;
      strCur++;
    }
    return ERR_OK;

caseSensitiveReplace:
    if (d->refCount.get() > 1)
    {
      rstart = (sysuint_t)(strCur - d->data);

      FOG_RETURN_ON_ERROR(detach());
      d = _d;

      strCur = d->data;
      strEnd = strCur;

      strCur += rstart;
      strEnd += rend;
    }

    while (strCur != strEnd)
    {
      if (*strCur == before) *strCur = after;
      strCur++;
    }
  }
  else
  {
    char beforeLower = Byte::toLower(before);
    char beforeUpper = Byte::toUpper(before);

    if (beforeLower == beforeUpper) goto caseSensitive;

    while (strCur != strEnd)
    {
      if (*strCur == beforeLower || *strCur == beforeUpper) goto caseInsensitiveReplace;
      strCur++;
    }
    return ERR_OK;

caseInsensitiveReplace:
    if (d->refCount.get() > 1)
    {
      rstart = (sysuint_t)(strCur - d->data);

      FOG_RETURN_ON_ERROR(detach());
      d = _d;

      strCur = d->data;
      strEnd = strCur;

      strCur += rstart;
      strEnd += rend;
    }

    while (strCur != strEnd)
    {
      if (*strCur == beforeLower || *strCur == beforeUpper) *strCur = after;
      strCur++;
    }
  }

  d->hashCode = 0;
  return ERR_OK;
}

err_t ByteArray::replace(const ByteArray& before, const ByteArray& after, uint cs, const Range& range)
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  sysuint_t len = before.getLength();
  if (len == 0) return 0;

  if (rend - rstart >= 256)
  {
    // Match using StringMatcher.
    ByteArrayMatcher matcher;
    if (matcher.setPattern(before)) return 0;

    return replace(matcher, after, cs, range);
  }
  else
  {
    // Match using naive algorithm.
    const char* aStr = getData();
    const char* bStr = before.getData();

    Range ranges[256];
    sysuint_t count = 0;

    for (;;)
    {
      sysuint_t i = StringUtil::indexOf(aStr + rstart, rend - rstart, bStr, len);
      if (i == INVALID_INDEX) break;

      rstart += i;
      ranges[count++].setRange(rstart, rstart + len);
      rstart += len;
    }

    return replace(ranges, count, after.getData(), after.getLength());
  }
}

err_t ByteArray::replace(const ByteArrayFilter& filter, const ByteArray& after, uint cs, const Range& range)
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  const char* str = getData();
  sysuint_t len = getLength();

  List<Range> ranges;

  for (;;)
  {
    Range r = filter.indexOf(str, len, cs, Range(rstart, rend));
    if (r.getStart() == INVALID_INDEX) break;

    ranges.append(r);
    rstart = r.getEnd();
  }

  return replace(ranges.getData(), ranges.getLength(), after.getData(), after.getLength());
}

err_t ByteArray::replace(const Range* m, sysuint_t mcount, const char* after, sysuint_t alen)
{
  sysuint_t i;
  sysuint_t pos = 0;
  sysuint_t len = getLength();
  const char* cur = getData();

  // Get total count of characters we remove.
  sysuint_t mtotal = 0;
  for (i = 0; i < mcount; i++)
  {
    sysuint_t rstart = m[i].getStart();
    sysuint_t rend = m[i].getEnd();
    if (rstart >= len || rstart >= rend) return ERR_RT_INVALID_ARGUMENT;

    mtotal += rend - rstart;
  }

  // Get total count of characters we add.
  sysuint_t atotal = alen * mcount;

  // Get target length.
  sysuint_t lenAfter = len - mtotal + atotal;

  Data* newd = Data::alloc(lenAfter);
  if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

  char* p = newd->data;
  sysuint_t remain = lenAfter;
  sysuint_t t;

  // Serialize
  for (i = 0; i < mcount; i++)
  {
    sysuint_t mstart = m[i].getStart();
    sysuint_t mend = m[i].getEnd();

    // Begin
    t = mstart - pos;
    if (t > remain) goto overflow;
    StringUtil::copy(p, cur + pos, t);
    p += t; remain -= t;

    // Replacement
    if (alen > remain) goto overflow;
    StringUtil::copy(p, after, alen);
    p += alen; remain -= alen;

    pos = mend;
  }

  // Last piece of string
  t = getLength() - pos;
  if (t > remain) goto overflow;
  StringUtil::copy(p, cur + pos, t);
  p += t;

  // Be sure that calculated length is correct (if this assert fails, the
  // 'm' and 'mcount' parameters are incorrect).
  FOG_ASSERT(p == newd->data + lenAfter);

  newd->length = lenAfter;
  newd->data[lenAfter] = 0;

  atomicPtrXchg(&_d, newd)->deref();
  return ERR_OK;

overflow:
  newd->deref();
  return ERR_RT_OVERFLOW;
}

// ============================================================================
// [Fog::ByteArray - Lower / Upper]
// ============================================================================

err_t ByteArray::lower()
{
  Data* d = _d;

  char* strCur = d->data;
  char* strEnd = strCur + d->length;

  for (; strCur != strEnd; strCur++)
  {
    if (Byte::isUpper(*strCur)) goto modify;
  }
  return ERR_OK;

modify:
  {
    sysuint_t n = (sysuint_t)(strCur - d->data);

    FOG_RETURN_ON_ERROR(detach());
    d = _d;

    strCur = d->data + n;
    strEnd = d->data + d->length;

    for (; strCur != strEnd; strCur++) *strCur = Byte::toLower(*strCur);
  }
  d->hashCode = 0;
  return ERR_OK;
}

err_t ByteArray::upper()
{
  Data* d = _d;

  char* strCur = d->data;
  char* strEnd = strCur + d->length;

  for (; strCur != strEnd; strCur++)
  {
    if (Byte::isLower(*strCur)) goto modify;
  }
  return ERR_OK;

modify:
  {
    sysuint_t n = (sysuint_t)(strCur - d->data);

    FOG_RETURN_ON_ERROR(detach());
    d = _d;

    strCur = d->data + n;
    strEnd = d->data + d->length;

    for (; strCur != strEnd; strCur++) *strCur = Byte::toUpper(*strCur);
  }
  d->hashCode = 0;
  return ERR_OK;
}

ByteArray ByteArray::lowered() const
{
  ByteArray t(*this);
  t.lower();
  return t;
}

ByteArray ByteArray::uppered() const
{
  ByteArray t(*this);
  t.upper();
  return t;
}

// ============================================================================
// [Fog::ByteArray - Whitespaces / Justification]
// ============================================================================

err_t ByteArray::trim()
{
  Data* d = _d;
  sysuint_t len = d->length;

  if (!len) return ERR_OK;

  const char* strCur = d->data;
  const char* strEnd = strCur + len;

  while (strCur != strEnd   && Byte::isSpace(*strCur)) strCur++;
  while (strCur != strEnd-- && Byte::isSpace(*strEnd)) continue;

  if (strCur != d->data || ++strEnd != d->data + len)
  {
    len = (sysuint_t)(strEnd - strCur);
    if (d->refCount.get() > 1)
    {
      Data* newd = Data::alloc(len, strCur, len);
      if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
      atomicPtrXchg(&_d, newd)->deref();
    }
    else
    {
      if (strCur != d->data) StringUtil::move(d->data, strCur, len);
      d->length = len;
      d->data[len] = 0;
      d->hashCode = 0;
    }
  }

  return ERR_OK;
}

err_t ByteArray::simplify()
{
  Data* d = _d;
  sysuint_t len = d->length;

  if (!len) return ERR_OK;

  const char* strBeg;
  const char* strCur = d->data;
  const char* strEnd = strCur + len;

  char* dest;

  while (strCur != strEnd   && Byte::isSpace(*strCur)) strCur++;
  while (strCur != strEnd-- && Byte::isSpace(*strEnd)) continue;

  strBeg = strCur;

  // Left and Right trim is complete...

  if (strCur != d->data || strEnd + 1 != d->data + len) goto simp;

  for (; strCur < strEnd; strCur++)
  {
    if (Byte::isSpace(strCur[0]) && Byte::isSpace(strCur[1])) goto simp;
  }
  return ERR_OK;

simp:
  strCur = strBeg;
  strEnd++;

  // this is a bit messy, but I will describe how it works. We can't
  // change string that's shared between another one, so if reference
  // count is larger than 1 we will alloc a new D. At the end of this
  // function we will dereference the D and in case that string
  // is shared it wont be destroyed. Only one problem is that we need
  // to increase reference count if string is detached.
  if (d->refCount.get() > 1)
  {
    Data* newd = Data::alloc((sysuint_t)(strEnd - strCur));
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
    _d = newd;
  }
  else
  {
    d->refCount.inc();
  }

  dest = _d->data;

  do {
    if    (strCur != strEnd &&  Byte::isSpace(*strCur)) *dest++ = ' ';
    while (strCur != strEnd &&  Byte::isSpace(*strCur)) strCur++;
    while (strCur != strEnd && !Byte::isSpace(*strCur)) *dest++ = *strCur++;
  } while (strCur != strEnd);

  _d->length = (dest - _d->data);
  _d->data[_d->length] = 0;
  _d->hashCode = 0;

  d->deref();

  return ERR_OK;
}

err_t ByteArray::truncate(sysuint_t n)
{
  Data* d = _d;
  if (d->length <= n) return ERR_OK;

  if (d->refCount.get() > 1)
  {
    Data* newd = Data::alloc(n, d->data, n);
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;
    atomicPtrXchg(&_d, newd)->deref();
  }
  else
  {
    d->length = n;
    d->data[d->length] = 0;
    d->hashCode = 0;
  }
  return ERR_OK;
}

err_t ByteArray::justify(sysuint_t n, char fill, uint32_t flags)
{
  Data* d = _d;
  sysuint_t length = d->length;

  if (n <= length) return ERR_OK;

  sysuint_t t = n - length;
  sysuint_t left = 0;
  sysuint_t right = 0;

  if ((flags & JUSTIFY_CENTER) == JUSTIFY_CENTER)
  {
    left = t >> 1;
    right = t - left;
  }
  else if ((flags & JUSTIFY_LEFT) == JUSTIFY_LEFT)
  {
    right = t;
  }
  else if ((flags & JUSTIFY_RIGHT) == JUSTIFY_RIGHT)
  {
    left = t;
  }

  err_t err;
  if ( (err = reserve(n)) ) return err;
  if ( (err = prepend(fill, left)) ) return err;
  return append(fill, right);
}

ByteArray ByteArray::trimmed() const
{
  ByteArray t(*this);
  t.trim();
  return t;
}

ByteArray ByteArray::simplified() const
{
  ByteArray t(*this);
  t.simplify();
  return t;
}

ByteArray ByteArray::truncated(sysuint_t n) const
{
  ByteArray t(*this);
  t.truncate(n);
  return t;
}

ByteArray ByteArray::justified(sysuint_t n, char fill, uint32_t flags) const
{
  ByteArray t(*this);
  t.justify(n, fill, flags);
  return t;
}

// ============================================================================
// [Fog::ByteArray - Split / Join]
// ============================================================================

List<ByteArray> ByteArray::split(char ch, uint splitBehavior, uint cs) const
{
  List<ByteArray> result;
  Data* d = _d;

  if (d->length == 0) return result;

  const char* strBeg = d->data;
  const char* strCur = strBeg;
  const char* strEnd = strCur + d->length;

  if (cs == CASE_SENSITIVE)
  {
__caseSensitive:
    for (;;)
    {
      if (strCur == strEnd || *strCur == ch)
      {
        sysuint_t splitLength = (sysuint_t)(strCur - strBeg);
        if ((splitLength == 0 && splitBehavior == SPLIT_KEEP_EMPTY_PARTS) || splitLength != 0)
        {
          result.append(ByteArray(strBeg, splitLength));
        }
        if (strCur == strEnd) break;
        strBeg = ++strCur;
      }
      else
        strCur++;
    }
  }
  else
  {
    char cLower = Byte::toLower(ch);
    char cUpper = Byte::toUpper(ch);
    if (cLower == cUpper) goto __caseSensitive;

    for (;;)
    {
      if (strCur == strEnd || *strCur == cLower || *strCur == cUpper)
      {
        sysuint_t splitLength = (sysuint_t)(strCur - strBeg);
        if ((splitLength == 0 && splitBehavior == SPLIT_KEEP_EMPTY_PARTS) || splitLength != 0)
        {
          result.append(ByteArray(strBeg, splitLength));
        }
        if (strCur == strEnd) break;
        strBeg = ++strCur;
      }
      else
        strCur++;
    }
  }
  return result;
}

List<ByteArray> ByteArray::split(const ByteArray& pattern, uint splitBehavior, uint cs) const
{
  sysuint_t plen = pattern.getLength();

  if (!plen)
  {
    List<ByteArray> result;
    result.append(*this);
    return result;
  }
  else if (plen == 1)
  {
    return split(pattern.getAt(0), splitBehavior, cs);
  }
  else
  {
    ByteArrayMatcher matcher(pattern);
    return split(matcher, splitBehavior, cs);
  }
}

List<ByteArray> ByteArray::split(const ByteArrayFilter& filter, uint splitBehavior, uint cs) const
{
  List<ByteArray> result;
  Data* d = _d;

  sysuint_t length = d->length;

  const char* strCur = d->data;
  const char* strEnd = strCur + length;

  for (;;)
  {
    sysuint_t remain = (sysuint_t)(strEnd - strCur);
    Range m = filter.match(strCur, remain, cs, Range(0, remain));
    sysuint_t splitLength = (m.getStart() != INVALID_INDEX) ? m.getStart() : remain;

    if ((splitLength == 0 && splitBehavior == SPLIT_KEEP_EMPTY_PARTS) || splitLength != 0)
      result.append(ByteArray(strCur, splitLength));

    if (m.getStart() == INVALID_INDEX) break;

    strCur += m.getEnd();
  }

  return result;
}

ByteArray ByteArray::join(const List<ByteArray>& seq, const char separator)
{
  TemporaryByteArray<1> sept(separator);
  return join(seq, sept);
}

ByteArray ByteArray::join(const List<ByteArray>& seq, const ByteArray& separator)
{
  ByteArray result;

  sysuint_t seqLength = 0;
  sysuint_t sepLength = separator.getLength();

  List<ByteArray>::ConstIterator it(seq);

  for (it.toStart(); it.isValid(); it.toNext())
  {
    sysuint_t len = it.value().getLength();

    // Prevent for possible overflow (shouldn't normally happen)
    if (!it.atStart())
    {
      if (seqLength + sepLength < seqLength) return result;
      seqLength += sepLength;
    }

    // Prevent for possible overflow (shouldn't normally happen)
    if (seqLength + len < seqLength) return result;
    seqLength += len;
  }

  // Allocate memory for all strings in seq and for separators
  if (result.reserve(seqLength) != ERR_OK) return result;

  char* cur = result._d->data;
  const char* sep = separator.getData();

  // Serialize
  for (it.toStart(); it.isValid(); it.toNext())
  {
    sysuint_t len = it.value().getLength();

    if (!it.atStart())
    {
      StringUtil::copy(cur, sep, sepLength);
      cur += sepLength;
    }

    StringUtil::copy(cur, it.value().getData(), len);
    cur += len;
  }

  cur[0] = char(0);
  result._d->length = (sysuint_t)(cur - result._d->data);
  return result;
}

// ============================================================================
// [Fog::ByteArray - Substring]
// ============================================================================

ByteArray ByteArray::substring(const Range& range) const
{
  sysuint_t rstart, rend;
  if (fitToRange(*this, rstart, rend, range))
    return ByteArray(Stub8(getData() + rstart, rend - rstart));
  else
    return ByteArray();
}

// ============================================================================
// [Fog::ByteArray - Conversion]
// ============================================================================

err_t ByteArray::atob(bool* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atob(getData(), getLength(), dst, end, parserFlags);
}

err_t ByteArray::atoi8(int8_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atoi8(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atou8(uint8_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atou8(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atoi16(int16_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atoi16(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atou16(uint16_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atou16(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atoi32(int32_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atoi32(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atou32(uint32_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atou32(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atoi64(int64_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atoi64(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atou64(uint64_t* dst, int base, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atou64(getData(), getLength(), dst, base, end, parserFlags);
}

err_t ByteArray::atof(float* dst, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atof(getData(), getLength(), dst, '.', end, parserFlags);
}

err_t ByteArray::atod(double* dst, sysuint_t* end, uint32_t* parserFlags) const
{
  return StringUtil::atod(getData(), getLength(), dst, '.', end, parserFlags);
}

// ============================================================================
// [Fog::ByteArray - Contains]
// ============================================================================

bool ByteArray::contains(char ch, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (fitToRange(*this, rstart, rend, range))
    return StringUtil::indexOf(getData() + rstart, rend - rstart, ch, cs) != INVALID_INDEX;
  else
    return false;
}

bool ByteArray::contains(const ByteArray& pattern, uint cs, const Range& range) const
{
  return indexOf(pattern, cs, range) != INVALID_INDEX;
}

bool ByteArray::contains(const ByteArrayFilter& filter, uint cs, const Range& range) const
{
  Range m = filter.indexOf(getData(), getLength(), cs, range);
  return m.getStart() != INVALID_INDEX;
}

// ============================================================================
// [Fog::ByteArray - CountOf]
// ============================================================================

sysuint_t ByteArray::countOf(char ch, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (fitToRange(*this, rstart, rend, range))
    return StringUtil::countOf(getData() + rstart, rend - rstart, ch, cs);
  else
    return 0;
}

sysuint_t ByteArray::countOf(const ByteArray& pattern, uint cs, const Range& range) const
{
  sysuint_t len = pattern.getLength();
  if (len == 0) return 0;
  if (len == 1) return countOf(pattern.getAt(0), cs, range);

  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  if (rend - rstart >= 256)
  {
    // Match using StringMatcher.
    ByteArrayMatcher matcher;
    if (matcher.setPattern(pattern)) return 0;

    return countOf(matcher, cs, range);
  }
  else
  {
    // Match using naive algorithm.
    const char* aStr = getData();
    const char* bStr = pattern.getData();

    sysuint_t count = 0;

    for (;;)
    {
      sysuint_t i = StringUtil::indexOf(aStr + rstart, rend - rstart, bStr, len);
      if (i == INVALID_INDEX) break;
      rstart += i;

      count++;
      rstart += len;
    }

    return count;
  }
}

sysuint_t ByteArray::countOf(const ByteArrayFilter& filter, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return 0;

  const char* str = getData();
  sysuint_t len = getLength();
  sysuint_t count = 0;

  for (;;)
  {
    Range r = filter.indexOf(str, len, cs, Range(rstart, rend));
    if (r.getStart() == INVALID_INDEX) break;

    count++;
    rstart = r.getEnd();
  }

  return count;
}

// ============================================================================
// [Fog::ByteArray - IndexOf / LastIndexOf]
// ============================================================================

sysuint_t ByteArray::indexOf(char ch, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  sysuint_t i = StringUtil::indexOf(getData() + rstart, rend - rstart, ch, cs);
  return i != INVALID_INDEX ? i + rstart : i;
}

sysuint_t ByteArray::indexOf(const ByteArray& pattern, uint cs, const Range& range) const
{
  sysuint_t len = pattern.getLength();
  if (len == 0) return INVALID_INDEX;
  if (len == 1) return indexOf(pattern.getAt(0), cs, range);

  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  if (rend - rstart >= 256)
  {
    // Match using StringMatcher.
    ByteArrayMatcher matcher;
    if (matcher.setPattern(pattern)) return 0;

    return indexOf(matcher, cs, range);
  }
  else
  {
    // Match using naive algorithm.
    sysuint_t i = StringUtil::indexOf(getData() + rstart, rend - rstart, pattern.getData(), len, cs);
    return (i == INVALID_INDEX) ? i : i + rstart;
  }
}

sysuint_t ByteArray::indexOf(const ByteArrayFilter& filter, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  Range m = filter.match(getData(), getLength(), cs, Range(rstart, rend));
  return m.getStart();
}

sysuint_t ByteArray::lastIndexOf(char ch, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  sysuint_t i = StringUtil::lastIndexOf(getData() + rstart, rend - rstart, ch, cs);
  return i != INVALID_INDEX ? i + rstart : i;
}

sysuint_t ByteArray::lastIndexOf(const ByteArray& pattern, uint cs, const Range& range) const
{
  sysuint_t len = pattern.getLength();
  if (len == 0) return INVALID_INDEX;
  if (len == 1) return lastIndexOf(pattern.getAt(0), cs, range);

  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  if (rend - rstart >= 256)
  {
    // Match using StringMatcher.
    ByteArrayMatcher matcher;
    if (matcher.setPattern(pattern)) return 0;

    return lastIndexOf(matcher, cs, range);
  }
  else
  {
    // Match using naive algorithm.
    const char* aData = getData();
    const char* bData = pattern.getData();

    sysuint_t result = INVALID_INDEX;

    for (;;)
    {
      sysuint_t i = StringUtil::indexOf(aData + rstart, rend - rstart, bData, len);
      if (i == INVALID_INDEX) break;

      result = i + rstart;
      rstart = result + len;
    }
    return result;
  }
}

sysuint_t ByteArray::lastIndexOf(const ByteArrayFilter& filter, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (!fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  sysuint_t result = INVALID_INDEX;
  for (;;)
  {
    Range m = filter.match(getData(), getLength(), cs, Range(rstart, rend));
    if (m.getStart() == INVALID_INDEX) break;

    result = m.getStart();
    rstart = m.getEnd();
  }

  return result;
}

// ============================================================================
// [Fog::ByteArray - IndexOfAny / LastIndexOfAny]
// ============================================================================

sysuint_t ByteArray::indexOfAny(const char* chars, sysuint_t numChars, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (!chars || !fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  sysuint_t i = StringUtil::indexOfAny(getData() + rstart, rend - rstart, chars, numChars, cs);
  return i != INVALID_INDEX ? i + rstart : i;
}

sysuint_t ByteArray::lastIndexOfAny(const char* chars, sysuint_t numChars, uint cs, const Range& range) const
{
  sysuint_t rstart, rend;
  if (!chars || !fitToRange(*this, rstart, rend, range)) return INVALID_INDEX;

  sysuint_t i = StringUtil::lastIndexOfAny(getData() + rstart, rend - rstart, chars, numChars, cs);
  return i != INVALID_INDEX ? i + rstart : i;
}

// ============================================================================
// [Fog::ByteArray - StartsWith / EndsWith]
// ============================================================================

bool ByteArray::startsWith(char ch, uint cs) const
{
  return startsWith(Stub8(&ch, 1), cs);
}

bool ByteArray::startsWith(const Stub8& str, uint cs) const
{
  const char* s = str.getData();
  sysuint_t length = str.getComputedLength();

  return getLength() >= length && StringUtil::eq(getData() + getLength() - length, s, length, cs);
}

bool ByteArray::startsWith(const ByteArray& str, uint cs) const
{
  return getLength() >= str.getLength() &&
    StringUtil::eq(getData(), str.getData(), str.getLength(), cs);
}

bool ByteArray::startsWith(const ByteArrayFilter& filter, uint cs) const
{
  sysuint_t flen = filter.getLength();
  if (flen == INVALID_INDEX) flen = getLength();

  return filter.match(getData(), getLength(), cs, Range(0, flen)).getStart() == 0;
}

bool ByteArray::endsWith(char ch, uint cs) const
{
  return endsWith(Stub8(&ch, 1), cs);
}

bool ByteArray::endsWith(const Stub8& str, uint cs) const
{
  const char* sData = str.getData();
  sysuint_t sLength = str.getComputedLength();

  return sLength < getLength() && StringUtil::eq(getData() + getLength() - sLength, sData, sLength, cs);
}

bool ByteArray::endsWith(const ByteArray& str, uint cs) const
{
  return getLength() >= str.getLength() &&
    StringUtil::eq(getData() + getLength() - str.getLength(), str.getData(), str.getLength(), cs);
}

bool ByteArray::endsWith(const ByteArrayFilter& filter, uint cs) const
{
  sysuint_t flen = filter.getLength();

  if (flen == INVALID_INDEX)
  {
    sysuint_t i = 0;
    sysuint_t len = getLength();

    for (;;)
    {
      Range r = filter.match(getData(), len, cs, Range(i));
      if (r.getStart() == INVALID_INDEX) return false;
      if ((i = r.getEnd()) == len) return true;
    }
  }
  else
  {
    return flen <= getLength() &&
      filter.match(
        getData() + getLength() - flen, getLength(), cs, Range(0, flen)).getStart() == 0;
  }
}

// ============================================================================
// [Fog::ByteArray - Comparison]
// ============================================================================

bool ByteArray::eq(const ByteArray* a, const ByteArray* b)
{
  sysuint_t alen = a->getLength();
  sysuint_t blen = b->getLength();
  if (alen != blen) return false;

  return StringUtil::eq(a->getData(), b->getData(), alen, CASE_SENSITIVE);
}

bool ByteArray::ieq(const ByteArray* a, const ByteArray* b)
{
  sysuint_t alen = a->getLength();
  sysuint_t blen = b->getLength();
  if (alen != blen) return false;

  return StringUtil::eq(a->getData(), b->getData(), alen, CASE_INSENSITIVE);
}

int ByteArray::compare(const ByteArray* a, const ByteArray* b)
{
  sysuint_t aLen = a->getLength();
  sysuint_t bLen = b->getLength();
  const uint8_t* aCur = reinterpret_cast<const uint8_t*>(a->getData());
  const uint8_t* bCur = reinterpret_cast<const uint8_t*>(b->getData());
  const uint8_t* aEnd = reinterpret_cast<const uint8_t*>(aCur + aLen);

  int c;
  if (bLen < aLen) aEnd = aCur + bLen;

  for (; aCur != aEnd; aCur++, bCur++)
    if ((c = (int)*aCur - (int)*bCur)) return c;

  return (int)((sysint_t)aLen - (sysint_t)bLen);
}

int ByteArray::icompare(const ByteArray* a, const ByteArray* b)
{
  sysuint_t aLen = a->getLength();
  sysuint_t bLen = b->getLength();
  const uint8_t* aCur = reinterpret_cast<const uint8_t*>(a->getData());
  const uint8_t* bCur = reinterpret_cast<const uint8_t*>(b->getData());
  const uint8_t* aEnd = reinterpret_cast<const uint8_t*>(aCur + aLen);

  int c;
  if (bLen < aLen) aEnd = aCur + bLen;

  for (; aCur != aEnd; aCur++, bCur++)
    if ((c = (int)Byte::toLower(*aCur) - (int)Byte::toLower(*bCur))) return c;

  return (int)((sysint_t)aLen - (sysint_t)bLen);
}

bool ByteArray::eq(const Stub8& other, uint cs) const
{
  sysuint_t len = other.getLength();
  if (len == DETECT_LENGTH)
  {
    const uint8_t* aCur = reinterpret_cast<const uint8_t*>(getData());
    const uint8_t* bCur = reinterpret_cast<const uint8_t*>(other.getData());

    if (cs == CASE_SENSITIVE)
    {
      for (sysuint_t i = getLength(); i; i--, aCur++, bCur++)
      {
        if (FOG_UNLIKELY(bCur == 0)) return false;
        if (*aCur != *bCur) return false;
      }
    }
    else
    {
      for (sysuint_t i = getLength(); i; i--, aCur++, bCur++)
      {
        if (!*bCur) return false;
        if (Byte::toLower(*aCur) != Byte::toLower(*bCur)) return false;
      }
    }
    return *bCur == 0;
  }
  else
    return getLength() == len && StringUtil::eq(getData(), other.getData(), len, cs);
}

bool ByteArray::eq(const ByteArray& other, uint cs) const
{
  return getLength() == other.getLength() && StringUtil::eq(getData(), other.getData(), getLength(), cs);
}

int ByteArray::compare(const Stub8& other, uint cs) const
{
  sysuint_t aLen = getLength();
  sysuint_t bLen = other.getLength();
  const uint8_t* aCur = reinterpret_cast<const uint8_t*>(getData());
  const uint8_t* aEnd = reinterpret_cast<const uint8_t*>(aCur + aLen);
  const uint8_t* bCur = reinterpret_cast<const uint8_t*>(other.getData());

  int c;

  if (bLen == DETECT_LENGTH)
  {
    if (cs == CASE_SENSITIVE)
    {
      for (;;)
      {
        if (FOG_UNLIKELY(aCur == aEnd)) return *bCur ? -1 : 0;
        if ((c = (int)*aCur - (int)*bCur)) return c;
        aCur++;
        bCur++;
      }
    }
    else
    {
      for (;;)
      {
        if (FOG_UNLIKELY(aCur == aEnd)) return *bCur ? -1 : 0;
        if ((c = (int)Byte::toLower(*aCur) - (int)Byte::toLower(*bCur))) return c;
        aCur++;
        bCur++;
      }
    }
  }
  else
  {
    if (bLen < aLen) aEnd = aCur + bLen;

    if (cs == CASE_SENSITIVE)
    {
      for (; aCur != aEnd; aCur++, bCur++)
        if ((c = (int)*aCur - (int)*bCur)) return c;
    }
    else
    {
      for (; aCur != aEnd; aCur++, bCur++)
        if ((c = (int)Byte::toLower(*aCur) - (int)Byte::toLower(*bCur))) return c;
    }

    return (int)((sysint_t)aLen - (sysint_t)bLen);
  }
}

int ByteArray::compare(const ByteArray& other, uint cs) const
{
  sysuint_t aLen = getLength();
  sysuint_t bLen = other.getLength();
  const uint8_t* aCur = reinterpret_cast<const uint8_t*>(getData());
  const uint8_t* aEnd = reinterpret_cast<const uint8_t*>(aCur + aLen);
  const uint8_t* bCur = reinterpret_cast<const uint8_t*>(other.getData());

  int c;
  if (bLen < aLen) aEnd = aCur + bLen;

  if (cs == CASE_SENSITIVE)
  {
    for (; aCur != aEnd; aCur++, bCur++)
      if ((c = (int)(uint8_t)*aCur - (int)(uint8_t)*bCur)) return c;
  }
  else
  {
    for (; aCur != aEnd; aCur++, bCur++)
      if ((c = (int)(uint8_t)Byte::toLower(*aCur) - (int)(uint8_t)Byte::toLower(*bCur))) return c;
  }

  return (int)((sysint_t)aLen - (sysint_t)bLen);
}

// ============================================================================
// [Fog::ByteArray::Utf8]
// ============================================================================

err_t ByteArray::validateUtf8(sysuint_t* invalidPos) const
{
  return StringUtil::validateUtf8(getData(), getLength());
}

err_t ByteArray::getNumUtf8Chars(sysuint_t* charsCount) const
{
  return StringUtil::getNumUtf8Chars(getData(), getLength(), charsCount);
}

// ============================================================================
// [Fog::ByteArray::FileSystem]
// ============================================================================

err_t ByteArray::slashesToPosix()
{
  return replace(char('\\'), char('/'));
}

err_t ByteArray::slashesToWin()
{
  return replace(char('/'), char('\\'));
}

// ============================================================================
// [Fog::ByteArray::Hash]
// ============================================================================

uint32_t ByteArray::getHashCode() const
{
  uint32_t h = _d->hashCode;
  if (h) return h;

  return (_d->hashCode = HashUtil::makeStringHash(getData(), getLength()));
}

// ============================================================================
// [Fog::ByteArray::Data]
// ============================================================================

ByteArray::Data* ByteArray::Data::ref() const
{
  if ((flags & IsSharable) != 0)
    return refAlways();
  else
    return copy(this);
}

void ByteArray::Data::deref()
{
  derefInline();
}

ByteArray::Data* ByteArray::Data::adopt(void* address, sysuint_t capacity)
{
  if (capacity == 0) return ByteArray::_dnull->refAlways();

  Data* d = (Data*)address;
  d->refCount.init(1);
  d->flags = 0;
  d->hashCode = 0;
  d->length = 0;
  d->capacity = capacity;
  d->data[0] = 0;

  return d;
}

ByteArray::Data* ByteArray::Data::adopt(void* address, sysuint_t capacity, const char* str, sysuint_t length)
{
  if (length == DETECT_LENGTH) length = StringUtil::len(str);

  if (length <= capacity)
  {
    Data* d = adopt(address, capacity);
    d->length = length;
    StringUtil::copy(d->data, str, length);
    d->data[length] = 0;
    return d;
  }
  else
  {
    return alloc(0, str, length);
  }
}

ByteArray::Data* ByteArray::Data::alloc(sysuint_t capacity)
{
  if (capacity == 0) return ByteArray::_dnull->refAlways();

  // Pad to 8 bytes
  capacity = (capacity + 7) & ~7;

  sysuint_t dsize = sizeFor(capacity);
  Data* d = (Data *)Memory::alloc(dsize);
  if (FOG_IS_NULL(d)) return NULL;

  d->refCount.init(1);
  d->flags = IsDynamic | IsSharable;
  d->hashCode = 0;
  d->capacity = capacity;
  d->length = 0;

  return d;
}

ByteArray::Data* ByteArray::Data::alloc(sysuint_t capacity, const char* str, sysuint_t length)
{
  if (length == DETECT_LENGTH) length = StringUtil::len(str);
  if (length > capacity) capacity = length;

  if (capacity == 0) return ByteArray::_dnull->refAlways();

  Data* d = alloc(capacity);
  if (FOG_IS_NULL(d)) return NULL;

  d->length = length;
  StringUtil::copy(d->data, str, length);
  d->data[length] = 0;

  return d;
}

ByteArray::Data* ByteArray::Data::realloc(Data* d, sysuint_t capacity)
{
  FOG_ASSERT(capacity >= d->length);

  sysuint_t dsize = ByteArray::Data::sizeFor(capacity);

  if ((d->flags & ByteArray::Data::IsDynamic) != 0)
  {
    if ((d = (Data *)Memory::realloc((void*)d, dsize)) != NULL)
    {
      d->capacity = capacity;
    }
  }
  else
  {
    Data* newd = alloc(capacity, d->data, d->length);
    if (FOG_IS_NULL(newd)) return NULL;

    d->deref();
    d = newd;
  }

  return d;
}

ByteArray::Data* ByteArray::Data::copy(const Data* d)
{
  return alloc(0, d->data, d->length);
}

void ByteArray::Data::free(Data* d)
{
  Memory::free((void*)d);
}

Static<ByteArray::Data> ByteArray::_dnull;

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_bytearray_init(void)
{
  ByteArray::Data* d = ByteArray::_dnull.instancep();
  d->refCount.init(1);
  d->flags |= ByteArray::Data::IsSharable;
  d->hashCode = 0;
  d->capacity = 0;
  d->length = 0;
  memset(d->data, 0, sizeof(d->data));
}

FOG_NO_EXPORT void _core_bytearray_fini(void)
{
}

} // Fog namespace
