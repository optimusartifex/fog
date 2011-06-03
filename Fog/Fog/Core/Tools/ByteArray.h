// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_BYTEARRAY_H
#define _FOG_CORE_TOOLS_BYTEARRAY_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Format.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/Stub.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ByteArrayFilter;
struct Locale;
struct TextCodec;
template<typename T> struct List;

// ============================================================================
// [Fog::ByteArrayData]
// ============================================================================

struct FOG_API ByteArrayData
{
  FOG_INLINE ByteArrayData* ref() const
  {
    refCount.inc();
    return (ByteArrayData*)this;
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref() && (flags & CONTAINER_DATA_STATIC) == 0) Memory::free(this);
  }

  // [Statics]

  static ByteArrayData* adopt(void* address, size_t capacity);
  static ByteArrayData* adopt(void* address, size_t capacity, const char* str, size_t length);

  static ByteArrayData* alloc(size_t capacity);
  static ByteArrayData* alloc(size_t capacity, const char* str, size_t length);

  static ByteArrayData* realloc(ByteArrayData* d, size_t capacity);
  static ByteArrayData* copy(const ByteArrayData* d);

  // [Size]

  static FOG_INLINE size_t sizeFor(size_t capacity)
  { return sizeof(ByteArrayData) + sizeof(char) * capacity; }

  // [Members]

  mutable Atomic<size_t> refCount;

  uint32_t flags;
  uint32_t hashCode;

  size_t capacity;
  size_t length;

  char data[4];
};

// ============================================================================
// [Fog::ByteArray]
// ============================================================================

//! @brief Byte array.
struct FOG_API ByteArray
{
  static Static<ByteArrayData> _dnull;

  // --------------------------------------------------------------------------
  // [Function Prototypes]
  // --------------------------------------------------------------------------

  typedef bool (*EqFn)(const ByteArray* a, const ByteArray* b);
  typedef int (*CompareFn)(const ByteArray* a, const ByteArray* b);

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ByteArray();

  ByteArray(char ch, size_t length);
  ByteArray(const ByteArray& other);
  ByteArray(const ByteArray& other1, const ByteArray& other2);

  explicit ByteArray(const char* str);
  ByteArray(const char* str, size_t length);
  ByteArray(const Stub8& str);

  explicit FOG_INLINE ByteArray(ByteArrayData* d) : _d(d) {}

  ~ByteArray();

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? (err_t)ERR_OK : _detach(); }

  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getFlags().
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }
  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }
  //! @copydoc Doxygen::Implicit::isStatic().
  FOG_INLINE bool isStatic() const { return (_d->flags & CONTAINER_DATA_STATIC) != 0; }

  // --------------------------------------------------------------------------
  // [ByteArrayData]
  // --------------------------------------------------------------------------

  //! @brief Returns count of allocated characters (capacity).
  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  //! @brief Returns count of used characters (length).
  FOG_INLINE size_t getLength() const { return _d->length; }
  //! @brief Returns @c true if string is empty (length == 0).
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  err_t prepare(size_t capacity);
  char* beginManipulation(size_t max, uint32_t op);

  err_t reserve(size_t to);
  err_t resize(size_t to);
  err_t grow(size_t by);

  void squeeze();
  void clear();
  void reset();

  //! @brief Returns const pointer to string data.
  FOG_INLINE const char* getData() const { return _d->data; }

  FOG_INLINE char* getDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::ByteArray::getDataX() - Called on non-detached object.");
    return _d->data;
  }

  FOG_INLINE void finishDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::ByteArray::finishDataX() - Called on non-detached object.");
    _d->hashCode = 0;
    FOG_ASSERT_X(_d->data[_d->length] == 0, "Fog::ByteArray::finishDataX() - Null terminator corrupted.");
  }

  FOG_INLINE void finishDataX(char* end)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::ByteArray::finishDataX() - Called on non-detached object.");
    FOG_ASSERT_X(end <= _d->data + _d->capacity,
      "Fog::ByteArray::finishDataX() - Buffer overflow.");

    *end = 0;

    _d->hashCode = 0;
    _d->length = (size_t)(end - _d->data);
  }

  FOG_INLINE char getAt(size_t index) const
  {
    FOG_ASSERT_X(index < getLength(), "Fog::ByteArray::getAt() - Index out of range.");
    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  err_t set(char ch, size_t length = 1);
  err_t set(const Stub8& str);
  err_t set(const ByteArray& other);

  FOG_INLINE err_t set(const char* s, size_t length = DETECT_LENGTH) { return set(Stub8(s, length)); }

  err_t setDeep(const ByteArray& other);

  err_t setBool(bool b);

  err_t setInt(int32_t n, int base = 10);
  err_t setInt(uint32_t n, int base = 10);
  err_t setInt(int64_t n, int base = 10);
  err_t setInt(uint64_t n, int base = 10);

  err_t setInt(int32_t n, int base, const FormatFlags& ff);
  err_t setInt(uint32_t n, int base, const FormatFlags& ff);
  err_t setInt(int64_t n, int base, const FormatFlags& ff);
  err_t setInt(uint64_t n, int base, const FormatFlags& ff);

  err_t setDouble(double d, int doubleForm = DF_SIGNIFICANT_DIGITS);
  err_t setDouble(double d, int doubleForm, const FormatFlags& ff);

  err_t format(const char* fmt, ...);
  err_t formatc(const char* fmt, const TextCodec& tc, ...);
  err_t vformat(const char* fmt, va_list ap);
  err_t vformatc(const char* fmt, const TextCodec& tc, va_list ap);

  err_t wformat(const ByteArray& fmt, char lex, const List<ByteArray>& args);
  err_t wformat(const ByteArray& fmt, char lex, const ByteArray* args, size_t length);

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  err_t append(char ch, size_t length = 1);
  err_t append(const Stub8& other);
  err_t append(const ByteArray& other);

  FOG_INLINE err_t append(const char* s, size_t length = DETECT_LENGTH) { return append(Stub8(s, length)); }

  err_t appendBool(bool b);
  err_t appendInt(int32_t n, int base = 10);
  err_t appendInt(uint32_t n, int base = 10);
  err_t appendInt(int64_t n, int base = 10);
  err_t appendInt(uint64_t n, int base = 10);

  err_t appendInt(int32_t n, int base, const FormatFlags& ff);
  err_t appendInt(uint32_t n, int base, const FormatFlags& ff);
  err_t appendInt(int64_t n, int base, const FormatFlags& ff);
  err_t appendInt(uint64_t n, int base, const FormatFlags& ff);

  err_t appendDouble(double d, int doubleForm = DF_SIGNIFICANT_DIGITS);
  err_t appendDouble(double d, int doubleForm, const FormatFlags& ff);

  err_t appendFormat(const char* fmt, ...);
  err_t appendFormatc(const char* fmt, const TextCodec& tc, ...);
  err_t appendVformat(const char* fmt, va_list ap);
  err_t appendVformatc(const char* fmt, const TextCodec& tc, va_list ap);

  err_t appendWformat(const ByteArray& fmt, char lex, const List<ByteArray>& args);
  err_t appendWformat(const ByteArray& fmt, char lex, const ByteArray* args, size_t length);

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  err_t prepend(char ch, size_t length = 1);
  err_t prepend(const Stub8& other);
  err_t prepend(const ByteArray& other);

  FOG_INLINE err_t prepend(const char* s, size_t length = DETECT_LENGTH) { return prepend(Stub8(s, length)); }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  err_t insert(size_t index, char ch, size_t length = 1);
  err_t insert(size_t index, const Stub8& other);
  err_t insert(size_t index, const ByteArray& other);

  FOG_INLINE err_t insert(size_t index, const char* s, size_t length = DETECT_LENGTH) { return insert(index, Stub8(s, length)); }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  size_t remove(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  size_t remove(const ByteArray& other, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  size_t remove(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  size_t remove(const Range& range);
  size_t remove(const Range* range, size_t count);

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  err_t replace(char before, char after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const ByteArray& before, const ByteArray& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const ByteArrayFilter& filter, const ByteArray& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  err_t replace(const Range& range, const ByteArray& replacement);
  err_t replace(const Range* range, size_t count, const char* after, size_t alen);

  // --------------------------------------------------------------------------
  // [Lower / Upper]
  // --------------------------------------------------------------------------

  err_t lower();
  err_t upper();

  ByteArray lowered() const;
  ByteArray uppered() const;

  // --------------------------------------------------------------------------
  // [Whitespaces / Justification]
  // --------------------------------------------------------------------------

  err_t trim();
  err_t simplify();
  err_t truncate(size_t n);
  err_t justify(size_t n, char fill, uint32_t flags);

  ByteArray trimmed() const;
  ByteArray simplified() const;
  ByteArray truncated(size_t n) const;
  ByteArray justified(size_t n, char fill, uint32_t flags) const;

  // --------------------------------------------------------------------------
  // [Split / Join]
  // --------------------------------------------------------------------------

  List<ByteArray> split(char ch, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  List<ByteArray> split(const ByteArray& pattern, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  List<ByteArray> split(const ByteArrayFilter& filter, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;

  static ByteArray join(const List<ByteArray>& seq, const char separator);
  static ByteArray join(const List<ByteArray>& seq, const ByteArray& separator);

  // --------------------------------------------------------------------------
  // [Substring]
  // --------------------------------------------------------------------------

  ByteArray substring(const Range& range) const;

  // --------------------------------------------------------------------------
  // [Conversion]
  // --------------------------------------------------------------------------

  err_t atob(bool* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi8(int8_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou8(uint8_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi16(int16_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou16(uint16_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi32(int32_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou32(uint32_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi64(int64_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou64(uint64_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const;

#if FOG_SIZEOF_LONG == 32
  FOG_INLINE err_t atol(long* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi32((int32_t*)dst, base, end, parserFlags); }
  FOG_INLINE ulong atoul(ulong* dst, uint base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou32((uint32_t*)dst, base, end, parserFlags); }
#else
  FOG_INLINE err_t atol(long* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi64((int64_t*)dst, base, end, parserFlags); }
  FOG_INLINE ulong atoul(ulong* dst, uint base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou64((uint64_t*)dst, base, end, parserFlags); }
#endif

#if FOG_ARCH_BITS == 32
  FOG_INLINE err_t atosysint(sysint_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi32((int32_t*)dst, base, end, parserFlags); }
  FOG_INLINE err_t atosysuint(size_t* dst, uint base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou32((uint32_t*)dst, base, end, parserFlags); }
#else
  FOG_INLINE err_t atosysint(sysint_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi64((int64_t*)dst, base, end, parserFlags); }
  FOG_INLINE err_t atosysuint(size_t* dst, uint base = 0, size_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou64((uint64_t*)dst, base, end, parserFlags); }
#endif

  err_t atof(float* dst, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atod(double* dst, size_t* end = NULL, uint32_t* parserFlags = NULL) const;

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  bool contains(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [CountOf]
  // --------------------------------------------------------------------------

  size_t countOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t countOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t countOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [IndexOf / LastIndexOf]
  // --------------------------------------------------------------------------

  size_t indexOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t indexOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t indexOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  size_t lastIndexOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [IndexOfAny / LastIndexOfAny]
  // --------------------------------------------------------------------------

  size_t indexOfAny(const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOfAny(const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [StartsWith / EndsWith]
  // --------------------------------------------------------------------------

  bool startsWith(char ch, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const Stub8& str, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  FOG_INLINE bool startsWith(const char* str, uint cs = CASE_SENSITIVE) const { return startsWith(Stub8(str), cs); }

  bool endsWith(char ch, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const Stub8& str, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  FOG_INLINE bool endsWith(const char* str, uint cs = CASE_SENSITIVE) const { return endsWith(Stub8(str), cs); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE ByteArray& operator=(char ch) { set(ch); return *this; }
  FOG_INLINE ByteArray& operator=(const Stub8& str) { set(str); return *this; }
  FOG_INLINE ByteArray& operator=(const ByteArray& other) { set(other); return *this; }
  FOG_INLINE ByteArray& operator=(const char* str) { set(str); return *this; }

  FOG_INLINE ByteArray& operator+=(char ch) { append(ch); return *this; }
  FOG_INLINE ByteArray& operator+=(const Stub8& str) { append(str); return *this; }
  FOG_INLINE ByteArray& operator+=(const ByteArray& other) { append(other); return *this; }
  FOG_INLINE ByteArray& operator+=(const char* str) { append(str); return *this; }

  FOG_INLINE char operator[](size_t index) const { return getAt(index); }

  // --------------------------------------------------------------------------
  // [Comparison]
  // --------------------------------------------------------------------------

  static bool eq(const ByteArray* a, const ByteArray* b);
  static bool ieq(const ByteArray* a, const ByteArray* b);

  static int compare(const ByteArray* a, const ByteArray* b);
  static int icompare(const ByteArray* a, const ByteArray* b);

  bool eq(const Stub8& other, uint cs = CASE_SENSITIVE) const;
  bool eq(const ByteArray& other, uint cs = CASE_SENSITIVE) const;

  FOG_INLINE bool eq(const char* other, uint cs = CASE_SENSITIVE) const { return eq(Stub8(other), cs); }

  int compare(const Stub8& other, uint cs = CASE_SENSITIVE) const;
  int compare(const ByteArray& other, uint cs = CASE_SENSITIVE) const;

  FOG_INLINE int compare(const char* other, uint cs = CASE_SENSITIVE) const { return compare(Stub8(other), cs); }

  // --------------------------------------------------------------------------
  // [Utf8 support]
  // --------------------------------------------------------------------------

  err_t validateUtf8(size_t* invalidPos = NULL) const;
  err_t getNumUtf8Chars(size_t* charsCount) const;

  // --------------------------------------------------------------------------
  // [FileSystem]
  // --------------------------------------------------------------------------

  err_t slashesToPosix();
  err_t slashesToWin();

  // --------------------------------------------------------------------------
  // [HashCode]
  // --------------------------------------------------------------------------

  uint32_t getHashCode() const;

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(ByteArrayData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Global Operator Overload]
// ============================================================================

//! @addtogroup Fog_Core_String
//! @{

static FOG_INLINE const Fog::ByteArray operator+(const Fog::ByteArray& a, const Fog::ByteArray& b) { return Fog::ByteArray(a, b); }

static FOG_INLINE const Fog::ByteArray operator+(const Fog::ByteArray& a, char b) { Fog::ByteArray t(a); t.append(b); return t; }
static FOG_INLINE const Fog::ByteArray operator+(char a, const Fog::ByteArray& b) { Fog::ByteArray t(b); t.append(a); return t; }

static FOG_INLINE const Fog::ByteArray operator+(const Fog::ByteArray& a, const Fog::Stub8& b) { Fog::ByteArray t(a); t.append(b); return t; }
static FOG_INLINE const Fog::ByteArray operator+(const Fog::Stub8& b, const Fog::ByteArray& a) { Fog::ByteArray t(b); t.append(a); return t; }

static FOG_INLINE const Fog::ByteArray operator+(const Fog::ByteArray& a, const char* b) { Fog::ByteArray t(a); t.append(b); return t; }
static FOG_INLINE const Fog::ByteArray operator+(const char* b, const Fog::ByteArray& a) { Fog::ByteArray t(b); t.append(a); return t; }

static FOG_INLINE bool operator==(const Fog::ByteArray& a, const Fog::ByteArray& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::ByteArray& a, const Fog::ByteArray& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::ByteArray& a, const Fog::ByteArray& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::ByteArray& a, const Fog::ByteArray& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::ByteArray& a, const Fog::ByteArray& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::ByteArray& a, const Fog::ByteArray& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::ByteArray& a, const Fog::Stub8& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::ByteArray& a, const Fog::Stub8& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::ByteArray& a, const Fog::Stub8& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::ByteArray& a, const Fog::Stub8& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::ByteArray& a, const Fog::Stub8& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::ByteArray& a, const Fog::Stub8& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::ByteArray& a, const char* b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::ByteArray& a, const char* b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::ByteArray& a, const char* b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::ByteArray& a, const char* b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::ByteArray& a, const char* b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::ByteArray& a, const char* b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::Stub8& a, const Fog::ByteArray& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const Fog::Stub8& a, const Fog::ByteArray& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const Fog::Stub8& a, const Fog::ByteArray& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const Fog::Stub8& a, const Fog::ByteArray& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const Fog::Stub8& a, const Fog::ByteArray& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const Fog::Stub8& a, const Fog::ByteArray& b) { return  b.compare(a) <  0; }

static FOG_INLINE bool operator==(const char* a, const Fog::ByteArray& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const char* a, const Fog::ByteArray& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const char* a, const Fog::ByteArray& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const char* a, const Fog::ByteArray& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const char* a, const Fog::ByteArray& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const char* a, const Fog::ByteArray& b) { return  b.compare(a) <  0; }

//! @}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::ByteArray, Fog::TYPEINFO_MOVABLE | Fog::TYPEINFO_HAS_COMPARE | Fog::TYPEINFO_HAS_EQ)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::ByteArray)

// [Guard]
#endif // _FOG_CORE_TOOLS_BYTEARRAY_H
