// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_STRING_H
#define _FOG_CORE_TOOLS_STRING_H

// [Dependencies]
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/Format.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/Stub.h>

#include <wchar.h>

//! @addtogroup Fog_Core_Tools
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

// foreign classes
struct Locale;
struct TextCodec;
struct StringFilter;
template<typename T> struct List;

// ============================================================================
// [Fog::StringData]
// ============================================================================

struct FOG_API StringData
{
  FOG_INLINE StringData* ref() const
  {
    refCount.inc();
    return (StringData*)this;
  }

  FOG_INLINE void deref()
  {
    if (refCount.deref() && (flags & CONTAINER_DATA_STATIC) == 0) Memory::free(this);
  }

  // [Statics]

  static StringData* adopt(void* address, size_t capacity);
  static StringData* adopt(void* address, size_t capacity, const char* str, size_t length);
  static StringData* adopt(void* address, size_t capacity, const Char* str, size_t length);

  static StringData* alloc(size_t capacity);
  static StringData* alloc(size_t capacity, const char* str, size_t length);
  static StringData* alloc(size_t capacity, const Char* str, size_t length);

  static StringData* realloc(StringData* d, size_t capacity);
  static StringData* copy(const StringData* d);

  // [Size]

  static FOG_INLINE size_t sizeFor(size_t capacity)
  { return sizeof(StringData) + sizeof(Char) * capacity; }

  // [Members]

  mutable Atomic<size_t> refCount;

  uint32_t flags;
  uint32_t hashCode;

  size_t capacity;
  size_t length;

  Char data[2];
};

// ============================================================================
// [Fog::String]
// ============================================================================

//! @brief String.
struct FOG_API String
{
  static Static<StringData> _dnull;

  // --------------------------------------------------------------------------
  // [Function Prototypes]
  // --------------------------------------------------------------------------

  typedef bool (*EqFn)(const String* a, const String* b);
  typedef int (*CompareFn)(const String* a, const String* b);

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  String();

  String(Char ch, size_t length);
  String(const String& other);
  String(const String& other1, const String& other2);
  String(const Char* str);
  String(const Char* str, size_t length);

  String(const Ascii8& str);
  String(const Utf8& str);
  String(const Utf16& str);

  explicit FOG_INLINE String(StringData* d) : _d(d) {}

  ~String();

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
  // [StringData]
  // --------------------------------------------------------------------------

  //! @brief Returns count of allocated characters (capacity).
  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  //! @brief Returns count of used characters (length).
  FOG_INLINE size_t getLength() const { return _d->length; }
  //! @brief Returns @c true if string is empty (length == 0).
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  err_t prepare(size_t capacity);
  Char* beginManipulation(size_t max, uint32_t op);

  err_t reserve(size_t to);
  err_t resize(size_t to);
  err_t grow(size_t by);

  void squeeze();
  void clear();
  void reset();

  //! @brief Returns const pointer to string data.
  FOG_INLINE const Char* getData() const { return _d->data; }

  FOG_INLINE Char* getDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::String::getDataX() - Called on non-detached object.");
    return _d->data;
  }

  FOG_INLINE void finishDataX()
  {
    FOG_ASSERT_X(isDetached(), "Fog::String::finishDataX() - Called on non-detached object.");
    FOG_ASSERT_X(_d->data[_d->length] == Char(0), "Fog::String::finishDataX() - Null terminator corrupted.");

    _d->hashCode = 0;
  }

  FOG_INLINE void finishDataX(Char* end)
  {
    FOG_ASSERT_X(isDetached(),
      "Fog::ByteArray::finishDataX() - Called on non-detached object.");
    FOG_ASSERT_X(end <= _d->data + _d->capacity,
      "Fog::ByteArray::finishDataX() - Buffer overflow.");

    *end = 0;

    _d->hashCode = 0;
    _d->length = (size_t)(end - _d->data);
  }

  FOG_INLINE const Char& getAt(size_t index) const
  {
    FOG_ASSERT_X(index < getLength(), "Fog::String::getAt() - Index out of range.");
    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  err_t set(Char ch, size_t length = 1);
  err_t set(const Ascii8& str);
  err_t set(const Utf16& str);
  err_t set(const String& other);
  err_t set(const void* str, size_t size, const TextCodec& tc);

  FOG_INLINE err_t set(const Char* s, size_t length = DETECT_LENGTH) { return set(Utf16(s, length)); }

  err_t setUtf8(const char* s, size_t length = DETECT_LENGTH);
  err_t setUtf32(const uint32_t* s, size_t length = DETECT_LENGTH);

  FOG_INLINE err_t setUtf16(const uint16_t* s, size_t length = DETECT_LENGTH)
  {
    return set(reinterpret_cast<const Char*>(s), length);
  }

  FOG_INLINE err_t setWChar(const wchar_t* s, size_t length = DETECT_LENGTH)
  {
    if (sizeof(wchar_t) == 2)
      return set(reinterpret_cast<const Char*>(s), length);
    else
      return setUtf32(reinterpret_cast<const uint32_t*>(s), length);
  }

  err_t setDeep(const String& other);

  err_t setBool(bool b);

  err_t setInt(int32_t n, int base = 10);
  err_t setInt(uint32_t n, int base = 10);
  err_t setInt(int64_t n, int base = 10);
  err_t setInt(uint64_t n, int base = 10);

  err_t setInt(int32_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);
  err_t setInt(uint32_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);
  err_t setInt(int64_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);
  err_t setInt(uint64_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);

  err_t setDouble(double d, int doubleForm = DF_SIGNIFICANT_DIGITS);
  err_t setDouble(double d, int doubleForm, const FormatFlags& ff, const Locale* locale = NULL);

  err_t format(const char* fmt, ...);
  err_t vformat(const char* fmt, va_list ap);

  err_t wformat(const String& fmt, Char lex, const List<String>& args);
  err_t wformat(const String& fmt, Char lex, const String* args, size_t length);

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  err_t append(Char ch, size_t length = 1);
  err_t append(const Ascii8& str);
  err_t append(const Utf16& str);
  err_t append(const String& other);
  err_t append(const void* str, size_t size, const TextCodec& tc);

  FOG_INLINE err_t append(const Char* s, size_t length = DETECT_LENGTH) { return append(Utf16(s, length)); }

  err_t appendUtf8(const char* s, size_t length = DETECT_LENGTH);
  err_t appendUtf32(const uint32_t* s, size_t length);

  FOG_INLINE err_t appendUtf16(const uint16_t* s, size_t length = DETECT_LENGTH)
  {
    return append(reinterpret_cast<const Char*>(s), length);
  }

  FOG_INLINE err_t appendWChar(const wchar_t* s, size_t length)
  {
    if (sizeof(wchar_t) == 2)
      return append(reinterpret_cast<const Char*>(s), length);
    else
      return appendUtf32(reinterpret_cast<const uint32_t*>(s), length);
  }

  err_t appendBool(bool b);
  err_t appendInt(int32_t n, int base = 10);
  err_t appendInt(uint32_t n, int base = 10);
  err_t appendInt(int64_t n, int base = 10);
  err_t appendInt(uint64_t n, int base = 10);

  err_t appendInt(int32_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);
  err_t appendInt(uint32_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);
  err_t appendInt(int64_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);
  err_t appendInt(uint64_t n, int base, const FormatFlags& ff, const Locale* locale = NULL);

  err_t appendDouble(double d, int doubleForm = DF_SIGNIFICANT_DIGITS);
  err_t appendDouble(double d, int doubleForm, const FormatFlags& ff, const Locale* locale = NULL);

  err_t appendFormat(const char* fmt, ...);
  err_t appendVformat(const char* fmt, va_list ap);

  err_t appendWformat(const String& fmt, Char lex, const List<String>& args);
  err_t appendWformat(const String& fmt, Char lex, const String* args, size_t length);

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  err_t prepend(Char ch, size_t length = 1);
  err_t prepend(const Ascii8& str);
  err_t prepend(const Utf16& str);
  err_t prepend(const String& other);

  FOG_INLINE err_t prepend(const Char* s, size_t length = DETECT_LENGTH) { return prepend(Utf16(s, length)); }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  err_t insert(size_t index, Char ch, size_t length = 1);
  err_t insert(size_t index, const Ascii8& str);
  err_t insert(size_t index, const Utf16& str);
  err_t insert(size_t index, const String& other);

  FOG_INLINE err_t insert(size_t index, const Char* s, size_t length = DETECT_LENGTH) { return insert(index, Utf16(s, length)); }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  size_t remove(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  size_t remove(const String& other, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  size_t remove(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  size_t remove(const Range& range);
  size_t remove(const Range* range, size_t count);

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  err_t replace(Char before, Char after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const String& before, const String& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const StringFilter& filter, const String& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  err_t replace(const Range& range, const String& replacement);
  err_t replace(const Range* range, size_t count, const Char* after, size_t alen);

  // --------------------------------------------------------------------------
  // [Lower / Upper]
  // --------------------------------------------------------------------------

  err_t lower();
  err_t upper();

  String lowered() const;
  String uppered() const;

  // --------------------------------------------------------------------------
  // [Whitespaces / Justification]
  // --------------------------------------------------------------------------

  err_t trim();
  err_t simplify();
  err_t truncate(size_t n);
  err_t justify(size_t n, Char fill, uint32_t flags);

  String trimmed() const;
  String simplified() const;
  String truncated(size_t n) const;
  String justified(size_t n, Char fill, uint32_t flags) const;

  // --------------------------------------------------------------------------
  // [Split / Join]
  // --------------------------------------------------------------------------

  List<String> split(Char ch, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  List<String> split(const String& pattern, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  List<String> split(const StringFilter& filter, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;

  static String join(const List<String>& seq, const Char separator);
  static String join(const List<String>& seq, const String& separator);

  // --------------------------------------------------------------------------
  // [Substring]
  // --------------------------------------------------------------------------

  String substring(const Range& range) const;

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

  err_t atof(float* dst, const Locale* locale = NULL, size_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atod(double* dst, const Locale* locale = NULL, size_t* end = NULL, uint32_t* parserFlags = NULL) const;

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  bool contains(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [CountOf]
  // --------------------------------------------------------------------------

  size_t countOf(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t countOf(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t countOf(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [IndexOf / LastIndexOf]
  // --------------------------------------------------------------------------

  size_t indexOf(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t indexOf(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t indexOf(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  size_t lastIndexOf(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOf(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOf(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [IndexOfAny / LastIndexOfAny]
  // --------------------------------------------------------------------------

  size_t indexOfAny(const Char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOfAny(const Char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [StartsWith / EndsWith]
  // --------------------------------------------------------------------------

  bool startsWith(const Ascii8& str, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const Utf16& str, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const String& str, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const StringFilter& filter, uint cs = CASE_SENSITIVE) const;

  FOG_INLINE bool startsWith(const Char* str, uint cs = CASE_SENSITIVE) const { return startsWith(Utf16(str), cs); }

  bool endsWith(const Ascii8& str, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const Utf16& str, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const String& str, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const StringFilter& filter, uint cs = CASE_SENSITIVE) const;

  FOG_INLINE bool endsWith(const Char* str, uint cs = CASE_SENSITIVE) const { return endsWith(Utf16(str), cs); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  FOG_INLINE String& operator=(Char ch) { set(ch); return *this; }
  FOG_INLINE String& operator=(const Ascii8& str) { set(str); return *this; }
  FOG_INLINE String& operator=(const Utf16& str) { set(str); return *this; }
  FOG_INLINE String& operator=(const String& other) { set(other); return *this; }
  FOG_INLINE String& operator=(const Char* str) { set(str); return *this; }

  FOG_INLINE String& operator+=(Char ch) { append(ch); return *this; }
  FOG_INLINE String& operator+=(const Ascii8& str) { append(str); return *this; }
  FOG_INLINE String& operator+=(const Utf16& str) { append(str); return *this; }
  FOG_INLINE String& operator+=(const String& other) { append(other); return *this; }
  FOG_INLINE String& operator+=(const Char* str) { append(str); return *this; }

  FOG_INLINE Char operator[](size_t index) const { return getAt(index); }

  // --------------------------------------------------------------------------
  // [ByteSwap]
  // --------------------------------------------------------------------------

  err_t bswap();

  // --------------------------------------------------------------------------
  // [Comparison]
  // --------------------------------------------------------------------------

  static bool eq(const String* a, const String* b);
  static bool ieq(const String* a, const String* b);

  static int compare(const String* a, const String* b);
  static int icompare(const String* a, const String* b);

  bool eq(const Ascii8& other, uint cs = CASE_SENSITIVE) const;
  bool eq(const Utf16& other, uint cs = CASE_SENSITIVE) const;
  bool eq(const String& other, uint cs = CASE_SENSITIVE) const;

  int compare(const Ascii8& other, uint cs = CASE_SENSITIVE) const;
  int compare(const Utf16& other, uint cs = CASE_SENSITIVE) const;
  int compare(const String& other, uint cs = CASE_SENSITIVE) const;

  // --------------------------------------------------------------------------
  // [Utf16]
  // --------------------------------------------------------------------------

  err_t validateUtf16(size_t* invalidPos = NULL) const;
  err_t getNumUtf16Chars(size_t* charsCount) const;

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

  _FOG_CLASS_D(StringData)
};

} // Fog namespace

// ============================================================================
// [Global Operator Overload]
// ============================================================================

static FOG_INLINE const Fog::String operator+(const Fog::String& a, const Fog::String& b) { return Fog::String(a, b); }

static FOG_INLINE const Fog::String operator+(const Fog::String& a, const Fog::Char& b) { Fog::String t(a); t.append(b); return t; }
static FOG_INLINE const Fog::String operator+(const Fog::Char& a, const Fog::String& b) { Fog::String t(b); t.append(a); return t; }

static FOG_INLINE const Fog::String operator+(const Fog::String& a, const Fog::Ascii8& b) { Fog::String t(a); t.append(b); return t; }
static FOG_INLINE const Fog::String operator+(const Fog::String& a, const Fog::Utf16& b) { Fog::String t(a); t.append(b); return t; }

static FOG_INLINE const Fog::String operator+(const Fog::Ascii8& b, const Fog::String& a) { Fog::String t(b); t.append(a); return t; }
static FOG_INLINE const Fog::String operator+(const Fog::Utf16& b, const Fog::String& a) { Fog::String t(b); t.append(a); return t; }

static FOG_INLINE bool operator==(const Fog::String& a, const Fog::String& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::String& a, const Fog::String& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::String& a, const Fog::String& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::String& a, const Fog::String& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::String& a, const Fog::String& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::String& a, const Fog::String& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::String& a, const Fog::Ascii8& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::String& a, const Fog::Ascii8& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::String& a, const Fog::Ascii8& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::String& a, const Fog::Ascii8& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::String& a, const Fog::Ascii8& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::String& a, const Fog::Ascii8& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::Ascii8& a, const Fog::String& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const Fog::Ascii8& a, const Fog::String& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const Fog::Ascii8& a, const Fog::String& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const Fog::Ascii8& a, const Fog::String& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const Fog::Ascii8& a, const Fog::String& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const Fog::Ascii8& a, const Fog::String& b) { return  b.compare(a) <  0; }

static FOG_INLINE bool operator==(const Fog::String& a, const Fog::Utf16& b) { return  a.eq(b); }
static FOG_INLINE bool operator!=(const Fog::String& a, const Fog::Utf16& b) { return !a.eq(b); }
static FOG_INLINE bool operator<=(const Fog::String& a, const Fog::Utf16& b) { return  a.compare(b) <= 0; }
static FOG_INLINE bool operator< (const Fog::String& a, const Fog::Utf16& b) { return  a.compare(b) <  0; }
static FOG_INLINE bool operator>=(const Fog::String& a, const Fog::Utf16& b) { return  a.compare(b) >= 0; }
static FOG_INLINE bool operator> (const Fog::String& a, const Fog::Utf16& b) { return  a.compare(b) >  0; }

static FOG_INLINE bool operator==(const Fog::Utf16& a, const Fog::String& b) { return  b.eq(a); }
static FOG_INLINE bool operator!=(const Fog::Utf16& a, const Fog::String& b) { return !b.eq(a); }
static FOG_INLINE bool operator<=(const Fog::Utf16& a, const Fog::String& b) { return  b.compare(a) >= 0; }
static FOG_INLINE bool operator< (const Fog::Utf16& a, const Fog::String& b) { return  b.compare(a) >  0; }
static FOG_INLINE bool operator>=(const Fog::Utf16& a, const Fog::String& b) { return  b.compare(a) <= 0; }
static FOG_INLINE bool operator> (const Fog::Utf16& a, const Fog::String& b) { return  b.compare(a) <  0; }

//! @}

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::String, Fog::TYPEINFO_MOVABLE | Fog::TYPEINFO_HAS_COMPARE | Fog::TYPEINFO_HAS_EQ)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::String)

// [Guard]
#endif // _FOG_CORE_TOOLS_STRING_H
