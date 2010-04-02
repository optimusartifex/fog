// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_STRING_H
#define _FOG_CORE_STRING_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Basics.h>
#include <Fog/Core/Char.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Static.h>
#include <Fog/Core/TypeInfo.h>

#include <wchar.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

//! @addtogroup Fog_Core
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

// foreign classes
struct Locale;
struct TextCodec;
struct StringFilter;

template<typename T> struct List;

// ============================================================================
// [Fog::String]
// ============================================================================

struct FOG_API String
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
    static Data* adopt(void* address, sysuint_t capacity, const Char* str, sysuint_t length);

    static Data* alloc(sysuint_t capacity);
    static Data* alloc(sysuint_t capacity, const char* str, sysuint_t length);
    static Data* alloc(sysuint_t capacity, const Char* str, sysuint_t length);

    static Data* realloc(Data* d, sysuint_t capacity);
    static Data* copy(const Data* d);
    static void free(Data* d);

    // [Size]

    static FOG_INLINE sysuint_t sizeFor(sysuint_t capacity)
    { return sizeof(Data) + sizeof(Char) * capacity; }

    // [Members]

    mutable Atomic<sysuint_t> refCount;

    uint32_t flags;
    uint32_t hashCode;

    sysuint_t capacity;
    sysuint_t length;

    Char data[2];
  };

  static Static<Data> _dnull;

  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef bool (*EqFn)(const String* a, const String* b);
  typedef int (*CompareFn)(const String* a, const String* b);

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  String();

  String(Char ch, sysuint_t length);
  String(const String& other);
  String(const String& other1, const String& other2);
  String(const Char* str);
  String(const Char* str, sysuint_t length);

  String(const Ascii8& str);
  String(const Utf16& str);

  explicit FOG_INLINE String(Data* d) : _d(d) {}

  ~String();

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
  Char* beginManipulation(sysuint_t max, int outputMode);

  err_t reserve(sysuint_t to);
  err_t resize(sysuint_t to);
  err_t grow(sysuint_t by);

  void squeeze();
  void clear();
  void free();

  //! @brief Returns const pointer to string data.
  FOG_INLINE const Char* getData() const { return _d->data; }

  //! @brief Returns mutable pointer to string data.
  Char* getMData();

  FOG_INLINE Char* getXData()
  {
    FOG_ASSERT_X(isDetached(), "Fog::String::getXData() - Not detached data.");
    return _d->data;
  }

  FOG_INLINE void xFinalize()
  {
    FOG_ASSERT_X(isDetached(), "Fog::String::xFinalize() - Non detached data.");
    _d->hashCode = 0;
    FOG_ASSERT_X(_d->data[_d->length] == Char(0), "Fog::String::xFinalize() - Null terminator corrupted.");
  }

  FOG_INLINE void xFinalize(Char* end)
  {
    FOG_ASSERT_X(isDetached(), "Fog::String::xFinalize() - Non detached data.");
    _d->hashCode = 0;
    _d->length = (sysuint_t)(end - _d->data);
    *end = 0;
    FOG_ASSERT_X(_d->length <= _d->capacity, "Fog::String::xFinalize() - Buffer overflow.");
  }

  FOG_INLINE const Char& at(sysuint_t index) const
  {
    FOG_ASSERT_X(index < getLength(), "Fog::String::at() - Index out of range.");
    return _d->data[index];
  }

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  err_t set(Char ch, sysuint_t length = 1);
  err_t set(const Ascii8& str);
  err_t set(const Utf16& str);
  err_t set(const String& other);
  err_t set(const void* str, sysuint_t size, const TextCodec& tc);

  FOG_INLINE err_t set(const Char* s, sysuint_t length = DETECT_LENGTH) { return set(Utf16(s, length)); }

  err_t setUtf8(const char* s, sysuint_t length = DETECT_LENGTH);
  err_t setUtf32(const uint32_t* s, sysuint_t length = DETECT_LENGTH);

  FOG_INLINE err_t setUtf16(const uint16_t* s, sysuint_t length = DETECT_LENGTH)
  {
    return set(reinterpret_cast<const Char*>(s), length);
  }

  FOG_INLINE err_t setWChar(const wchar_t* s, sysuint_t length = DETECT_LENGTH)
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
  err_t wformat(const String& fmt, Char lex, const String* args, sysuint_t length);

  // --------------------------------------------------------------------------
  // [Append]
  // --------------------------------------------------------------------------

  err_t append(Char ch, sysuint_t length = 1);
  err_t append(const Ascii8& str);
  err_t append(const Utf16& str);
  err_t append(const String& other);
  err_t append(const void* str, sysuint_t size, const TextCodec& tc);

  FOG_INLINE err_t append(const Char* s, sysuint_t length = DETECT_LENGTH) { return append(Utf16(s, length)); }

  err_t appendUtf8(const char* s, sysuint_t length = DETECT_LENGTH);
  err_t appendUtf32(const uint32_t* s, sysuint_t length);

  FOG_INLINE err_t appendUtf16(const uint16_t* s, sysuint_t length = DETECT_LENGTH)
  {
    return append(reinterpret_cast<const Char*>(s), length);
  }

  FOG_INLINE err_t appendWChar(const wchar_t* s, sysuint_t length)
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
  err_t appendWformat(const String& fmt, Char lex, const String* args, sysuint_t length);

  // --------------------------------------------------------------------------
  // [Prepend]
  // --------------------------------------------------------------------------

  err_t prepend(Char ch, sysuint_t length = 1);
  err_t prepend(const Ascii8& str);
  err_t prepend(const Utf16& str);
  err_t prepend(const String& other);

  FOG_INLINE err_t prepend(const Char* s, sysuint_t length = DETECT_LENGTH) { return prepend(Utf16(s, length)); }

  // --------------------------------------------------------------------------
  // [Insert]
  // --------------------------------------------------------------------------

  err_t insert(sysuint_t index, Char ch, sysuint_t length = 1);
  err_t insert(sysuint_t index, const Ascii8& str);
  err_t insert(sysuint_t index, const Utf16& str);
  err_t insert(sysuint_t index, const String& other);

  FOG_INLINE err_t insert(sysuint_t index, const Char* s, sysuint_t length = DETECT_LENGTH) { return insert(index, Utf16(s, length)); }

  // --------------------------------------------------------------------------
  // [Remove]
  // --------------------------------------------------------------------------

  sysuint_t remove(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  sysuint_t remove(const String& other, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  sysuint_t remove(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  sysuint_t remove(const Range& range);
  sysuint_t remove(const Range* range, sysuint_t count);

  // --------------------------------------------------------------------------
  // [Replace]
  // --------------------------------------------------------------------------

  err_t replace(Char before, Char after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const String& before, const String& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));
  err_t replace(const StringFilter& filter, const String& after, uint cs = CASE_SENSITIVE, const Range& range = Range(0));

  err_t replace(const Range& range, const String& replacement);
  err_t replace(const Range* range, sysuint_t count, const Char* after, sysuint_t alen);

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
  err_t truncate(sysuint_t n);
  err_t justify(sysuint_t n, Char fill, uint32_t flags);

  String trimmed() const;
  String simplified() const;
  String truncated(sysuint_t n) const;
  String justified(sysuint_t n, Char fill, uint32_t flags) const;

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

  err_t atof(float* dst, const Locale* locale = NULL, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;
  err_t atod(double* dst, const Locale* locale = NULL, sysuint_t* end = NULL, uint32_t* parserFlags = NULL) const;

  // --------------------------------------------------------------------------
  // [Contains]
  // --------------------------------------------------------------------------

  bool contains(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [CountOf]
  // --------------------------------------------------------------------------

  sysuint_t countOf(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t countOf(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t countOf(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  // --------------------------------------------------------------------------
  // [IndexOf / LastIndexOf]
  // --------------------------------------------------------------------------

  sysuint_t indexOf(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t indexOf(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t indexOf(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  sysuint_t lastIndexOf(Char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t lastIndexOf(const String& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  sysuint_t lastIndexOf(const StringFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

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

  FOG_INLINE Char operator[](sysuint_t index) const { return at(index); }

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

  err_t validateUtf16(sysuint_t* invalidPos = NULL) const;
  err_t getNumUtf16Chars(sysuint_t* charsCount) const;

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
// [Fog::TemporaryString<N>]
// ============================================================================

template<sysuint_t N>
struct TemporaryString : public String
{
  // --------------------------------------------------------------------------
  // [Temporary Storage]
  // --------------------------------------------------------------------------

  // Keep 'Storage' name for this struct for Borland compiler
  struct Storage
  {
    Data _d;
    Char _str[N];
  } _storage;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TemporaryString() :
    String(Data::adopt((void*)&_storage, N))
  {
  }

  FOG_INLINE TemporaryString(Char ch) :
    String(Data::adopt((void*)&_storage, N, &ch, 1))
  {
  }

  FOG_INLINE TemporaryString(const Ascii8& str) :
    String(Data::adopt((void*)&_storage, N, str.getData(), str.getLength()))
  {
  }

  FOG_INLINE TemporaryString(const Utf16& str) :
    String(Data::adopt((void*)&_storage, N, str.getData(), str.getLength()))
  {
  }

  FOG_INLINE TemporaryString(const String& other) :
    String(Data::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  FOG_INLINE TemporaryString(const TemporaryString<N>& other) :
    String(Data::adopt((void*)&_storage, N, other.getData(), other.getLength()))
  {
  }

  FOG_INLINE TemporaryString(const Char* str) :
    String(Data::adopt((void*)&_storage, N, str, DETECT_LENGTH))
  {
  }

  // safe shareable TemporaryString creation
  FOG_INLINE TemporaryString(_CREATE_SHAREABLE) :
    String(Data::adopt((void*)&_storage, N))
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

  FOG_INLINE TemporaryString<N>& operator=(Char ch) { set(ch); return *this; }
  FOG_INLINE TemporaryString<N>& operator=(const Ascii8& str) { set(str); return *this; }
  FOG_INLINE TemporaryString<N>& operator=(const Utf16& str) { set(str); return *this; }
  FOG_INLINE TemporaryString<N>& operator=(const String& other) { set(other); return *this; }
  FOG_INLINE TemporaryString<N>& operator=(const TemporaryString<N>& other) { set(other); return *this; }
  FOG_INLINE TemporaryString<N>& operator=(const Char* str) { set(str); return *this; }
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

FOG_DECLARE_TYPEINFO(Fog::String, Fog::TYPEINFO_MOVABLE | Fog::TYPEINFO_HAS_COMPARE | Fog::TYPEINFO_HAS_EQ)

// [Guard]
#endif // _FOG_CORE_STRING_H
