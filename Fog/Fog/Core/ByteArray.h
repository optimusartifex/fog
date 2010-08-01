// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_BYTEARRAY_H
#define _FOG_CORE_BYTEARRAY_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Format.h>
#include <Fog/Core/Range.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/Stub.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_String
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct ByteArrayFilter;
struct Locale;
struct TextCodec;

template<typename T> struct List;

// ============================================================================
// [Fog::ByteArray]
// ============================================================================

//! @brief Byte array.
struct FOG_API ByteArray
{
  // --------------------------------------------------------------------------
  // [String Data]
  // --------------------------------------------------------------------------

  //! @brief String data
  struct FOG_API Data
  {
    // [Flags]

    //! @brief String data flags.
    enum
    {
      //! @brief String data are created on the heap.
      //!
      //! Object is created by function like @c Fog::Memory::alloc() or by
      //! @c new operator. It this flag is not set, you can't delete object from
      //! the heap and object is probabbly only temporary (short life object).
      IsDynamic = (1U << 0),

      //! @brief String data are shareable.
      //!
      //! Object can be directly referenced by internal method @c ref().
      //! Sharable data are usually created on the heap and together
      //! with this flag is set also @c IsDynamic, but it isn't prerequisite.
      IsSharable = (1U << 1),

      //! @brief String data are strong to weak assignments.
      //!
      //! This flag means:
      //!   "Don't assign other data to me, instead, copy them to me!".
      IsStrong = (1U << 2)
    };

    // [Ref]

    Data* ref() const;
    void deref();

    FOG_INLINE Data* refAlways() const
    {
      refCount.inc();
      return (Data*)this;
    }

    FOG_INLINE void derefInline()
    {
      if (refCount.deref() && (flags & IsDynamic) != 0) free(this);
    }

    // [Statics]

    static Data* adopt(void* address, sysuint_t capacity);
    static Data* adopt(void* address, sysuint_t capacity, const char* str, sysuint_t length);

    static Data* alloc(sysuint_t capacity);
    static Data* alloc(sysuint_t capacity, const char* str, sysuint_t length);

    static Data* realloc(Data* d, sysuint_t capacity);
    static Data* copy(const Data* d);
    static void free(Data* d);

    // [Size]

    static FOG_INLINE sysuint_t sizeFor(sysuint_t capacity)
    { return sizeof(Data) + sizeof(char) * capacity; }

    // [Members]

    mutable Atomic<sysuint_t> refCount;

    uint32_t flags;
    uint32_t hashCode;

    sysuint_t capacity;
    sysuint_t length;

    char data[4];
  };

  static Static<Data> _dnull;

  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef bool (*EqFn)(const ByteArray* a, const ByteArray* b);
  typedef int (*CompareFn)(const ByteArray* a, const ByteArray* b);

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ByteArray();

  ByteArray(char ch, sysuint_t length);
  ByteArray(const ByteArray& other);
  ByteArray(const ByteArray& other1, const ByteArray& other2);

  explicit ByteArray(const char* str);
  ByteArray(const char* str, sysuint_t length);
  ByteArray(const Stub8& str);

  explicit FOG_INLINE ByteArray(Data* d) : _d(d) {}

  ~ByteArray();

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return refCount() == 1; }
  //! @copydoc Doxygen::Implicit::detach().
  FOG_INLINE err_t detach() { return isDetached() ? ERR_OK : _detach(); }

  //! @copydoc Doxygen::Implicit::_detach().
  err_t _detach();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getFlags().
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }
  //! @copydoc Doxygen::Implicit::isNull().
  FOG_INLINE bool isNull() const { return _d == _dnull.instancep(); }
  //! @copydoc Doxygen::Implicit::isDynamic().
  FOG_INLINE bool isDynamic() const { return (_d->flags & Data::IsDynamic) != 0; }
  //! @copydoc Doxygen::Implicit::isSharable().
  FOG_INLINE bool isSharable() const { return (_d->flags & Data::IsSharable) != 0; }
  //! @copydoc Doxygen::Implicit::isStrong().
  FOG_INLINE bool isStrong() const { return (_d->flags & Data::IsStrong) != 0; }

  //! @copydoc Doxygen::Implicit::setSharable().
  void setSharable(bool val);
  //! @copydoc Doxygen::Implicit::setStrong().
  void setStrong(bool val);

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  //! @brief Returns count of allocated characters (capacity).
  FOG_INLINE sysuint_t getCapacity() const { return _d->capacity; }
  //! @brief Returns count of used characters (length).
  FOG_INLINE sysuint_t getLength() const { return _d->length; }
  //! @brief Returns @c true if string is empty (length == 0).
  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  err_t prepare(sysuint_t capacity);
  char* beginManipulation(sysuint_t max, int outputMode);

  err_t reserve(sysuint_t to);
  err_t resize(sysuint_t to);
  err_t grow(sysuint_t by);

  void squeeze();
  void clear();
  void free();

  //! @brief Returns const pointer to string data.
  FOG_INLINE const char* getData() const { return _d->data; }

  //! @brief Returns mutable pointer to string data.
  char* getMData();

  FOG_INLINE char* getXData()
  {
    FOG_ASSERT_X(isDetached(), "Fog::ByteArray::getXData() - Not detached data.");
    return _d->data;
  }

  FOG_INLINE void xFinalize()
  {
    FOG_ASSERT_X(isDetached(), "Fog::ByteArray::xFinalize() - Non detached data.");
    _d->hashCode = 0;
    FOG_ASSERT_X(_d->data[_d->length] == 0, "Fog::ByteArray::xFinalize() - Null terminator corrupted.");
  }

  FOG_INLINE void xFinalize(char* end)
  {
    FOG_ASSERT_X(isDetached(), "Fog::ByteArray::xFinalize() - Non detached data.");

    _d->hashCode = 0;
    _d->length = (sysuint_t)(end - _d->data);
    *end = 0;

    FOG_ASSERT_X(_d->length <= _d->capacity, "Fog::ByteArray::xFinalize() - Buffer overflow.");
  }

  FOG_INLINE char at(sysuint_t index) const
  {
    FOG_ASSERT_X(index < getLength(), "Fog::ByteArray::at() - Index out of range.");
    return _d->data[index];
  }

  const void* nullTerminated() const;

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  err_t set(char ch, sysuint_t length = 1);
  err_t set(const Stub8& str);
  err_t set(const ByteArray& other);

  FOG_INLINE err_t set(const char* s, sysuint_t length = DETECT_LENGTH) { return set(Stub8(s, length)); }

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
  err_t wformat(const ByteArray& fmt, char lex, const ByteArray* args, sysuint_t length);

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  err_t append(char ch, sysuint_t length = 1);
  err_t append(const Stub8& other);
  err_t append(const ByteArray& other);

  FOG_INLINE err_t append(const char* s, sysuint_t length = DETECT_LENGTH) { return append(Stub8(s, length)); }

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
  err_t appendWformat(const ByteArray& fmt, char lex, const ByteArray* args, sysuint_t length);

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  err_t prepend(char ch, sysuint_t length = 1);
  err_t prepend(const Stub8& other);
  err_t prepend(const ByteArray& other);

  FOG_INLINE err_t prepend(const char* s, sysuint_t length = DETECT_LENGTH) { return prepend(Stub8(s, length)); }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  err_t insert(sysuint_t index, char ch, sysuint_t length = 1);
  err_t insert(sysuint_t index, const Stub8& other);
  err_t insert(sysuint_t index, const ByteArray& other);

  FOG_INLINE err_t insert(sysuint_t index, const char* s, sysuint_t length = DETECT_LENGTH) { return insert(index, Stub8(s, length)); }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  sysuint_t remove(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  sysuint_t remove(const ByteArray& other, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  sysuint_t remove(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  sysuint_t remove(const Range& range);
  sysuint_t remove(const Range* range, sysuint_t count);

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  err_t replace(char before, char after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const ByteArray& before, const ByteArray& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const ByteArrayFilter& filter, const ByteArray& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  err_t replace(const Range& range, const ByteArray& replacement);
  err_t replace(const Range* range, sysuint_t count, const char* after, sysuint_t alen);

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
  err_t truncate(sysuint_t n);
  err_t justify(sysuint_t n, char fill, uint32_t flags);

  ByteArray trimmed() const;
  ByteArray simplified() const;
  ByteArray truncated(sysuint_t n) const;
  ByteArray justified(sysuint_t n, char fill, uint32_t flags) const;

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

  err_t atob(bool* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi8(int8_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou8(uint8_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi16(int16_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou16(uint16_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi32(int32_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou32(uint32_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atoi64(int64_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atou64(uint64_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;

#if FOG_SIZEOF_LONG == 32
  FOG_INLINE err_t atol(long* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi32((int32_t*)dst, base, end, parserFlags); }
  FOG_INLINE ulong atoul(ulong* dst, uint base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou32((uint32_t*)dst, base, end, parserFlags); }
#else
  FOG_INLINE err_t atol(long* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi64((int64_t*)dst, base, end, parserFlags); }
  FOG_INLINE ulong atoul(ulong* dst, uint base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou64((uint64_t*)dst, base, end, parserFlags); }
#endif

#if FOG_ARCH_BITS == 32
  FOG_INLINE err_t atosysint(sysint_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi32((int32_t*)dst, base, end, parserFlags); }
  FOG_INLINE err_t atosysuint(sysuint_t* dst, uint base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou32((uint32_t*)dst, base, end, parserFlags); }
#else
  FOG_INLINE err_t atosysint(sysint_t* dst, int base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atoi64((int64_t*)dst, base, end, parserFlags); }
  FOG_INLINE err_t atosysuint(sysuint_t* dst, uint base = 0, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const
  { return atou64((uint64_t*)dst, base, end, parserFlags); }
#endif

  err_t atof(float* dst, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atod(double* dst, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  bool contains(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [CountOf]
  // --------------------------------------------------------------------------

  sysuint_t countOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t countOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t countOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [IndexOf / LastIndexOf]
  // --------------------------------------------------------------------------

  sysuint_t indexOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t indexOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t indexOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  sysuint_t lastIndexOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t lastIndexOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t lastIndexOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [IndexOfAny / LastIndexOfAny]
  // --------------------------------------------------------------------------

  sysuint_t indexOfAny(const char* chars, sysuint_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t lastIndexOfAny(const char* chars, sysuint_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

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

  FOG_INLINE char operator[](sysuint_t index) const { return at(index); }

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

  err_t validateUtf8(sysuint_t* invalidPos = NULL) const;
  err_t getNumUtf8Chars(sysuint_t* charsCount) const;

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

  FOG_DECLARE_D(Data)
};

// ============================================================================
// [Fog::TemporaryByteArray<N>]
// ============================================================================

template<sysuint_t N>
struct TemporaryByteArray : public ByteArray
{
  // --------------------------------------------------------------------------
  // [Temporary Storage]
  // --------------------------------------------------------------------------

  // Keep 'Storage' name for this struct for Borland compiler
  struct Storage
  {
    Data _d;
    char _str[N];
  } _storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TemporaryByteArray() :
    ByteArray(Data::adopt((void*)&_storage, N))
  {
  }

  FOG_INLINE TemporaryByteArray(const char ch) :
    ByteArray(Data::adopt((void*)&_storage, N, &ch, 1))
  {
  }

  FOG_INLINE TemporaryByteArray(const Stub8& str) :
    ByteArray(Data::adopt((void*)&_storage, N, str.getData(), str.getLength()))
  {
  }

  FOG_INLINE TemporaryByteArray(const char* str) :
    ByteArray(Data::adopt((void*)&_storage, N, str, DETECT_LENGTH))
  {
  }

  FOG_INLINE TemporaryByteArray(const ByteArray& other) :
    ByteArray(Data::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  FOG_INLINE TemporaryByteArray(const TemporaryByteArray<N>& other) :
    ByteArray(Data::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  // Safe shareable TemporaryByteArray creation.
  FOG_INLINE TemporaryByteArray(_CREATE_SHAREABLE) :
    ByteArray(Data::adopt((void*)&_storage, N))
  {
    _d->flags |= Data::IsSharable;
  }

  // --------------------------------------------------------------------------
  // [Implicit Data]
  // --------------------------------------------------------------------------

  FOG_INLINE void free()
  {
    if ((void*)_d != (void*)&_storage)
    {
      atomicPtrXchg(&_d, Data::adopt((void*)&_storage, N))->deref();
    }
  }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  // These overloads are needed to succesfully use this template (or implicit
  // conversion will break template and new string will be allocated).

  FOG_INLINE TemporaryByteArray<N>& operator=(char ch) { set(ch); return *this; }
  FOG_INLINE TemporaryByteArray<N>& operator=(const Stub8& str) { set(str); return *this; }
  FOG_INLINE TemporaryByteArray<N>& operator=(const char* str) { set(str); return *this; }
  FOG_INLINE TemporaryByteArray<N>& operator=(const ByteArray& other) { set(other); return *this; }
  FOG_INLINE TemporaryByteArray<N>& operator=(const TemporaryByteArray<N>& other) { set(other); return *this; }
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

FOG_DECLARE_TYPEINFO(Fog::ByteArray, Fog::TYPEINFO_MOVABLE | Fog::TYPEINFO_HAS_COMPARE | Fog::TYPEINFO_HAS_EQ)

// [Guard]
#endif // _FOG_CORE_BYTEARRAY_H
