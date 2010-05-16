// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TEXTCODEC_H
#define _FOG_CORE_TEXTCODEC_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Char.h>
#include <Fog/Core/TypeInfo.h>
#include <Fog/Core/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Other
//! @{

// ============================================================================
// [Fog::TextCodec]
// ============================================================================

//! @brief Text codec.
//!
//! Text codec class provides easy way to encode or decode unicode text to various
//! encodings. Input and output class is usually String in combination with ByteArray.
//! 
//! Default text codec created by empty constructor is always @c Null.
//!
//! Text codec contains built-in codecs:
//! - @c Fog::TextCodec::ascii8() - Ascii text codec.
//! - @c Fog::TextCodec::local8() - Local 8 bit text codec (system dependent).
//! - @c Fog::TextCodec::utf8() - UTF-8 text codec.
//! - @c Fog::TextCodec::utf16() - UTF-16 text codec.
//! - @c Fog::TextCodec::utf32() - UTF-32 text codec.
//! - @c Fog::TextCodec::ucs2() - UCS-2 text codec.
//! - @c Fog::TextCodec::ucs4() - UCS-4 text codec.
//! - @c Fog::TextCodec::localW() - Local <code>wchar_t*</code> text codec.
struct FOG_API TextCodec
{
  // --------------------------------------------------------------------------
  // [Code]
  // --------------------------------------------------------------------------

  enum Code
  {
    // If you want to modify these enums, you must modify text codec table
    // in Core/TextCodec.cpp

    //! @brief None, not initialized or signalizes error.
    None = 0,
    //! @brief Built-in UTF-8 codec.
    UTF8 = 1,
    //! @brief Built-in UTF-16 codec.
    UTF16 = 2,
    //! @brief Built-in UTF-16 (byteswapped) codec.
    UTF16Swapped = 3,
    //! @brief Built-in UTF-32 codec.
    UTF32 = 4,
    //! @brief Built-in UTF-32 (byteswapped) codec.
    UTF32Swapped = 5,
    //! @brief Built-in UCS-2 codec.
    UCS2 = 6,
    //! @brief Built-in UCS-2 (byteswapped) codec.
    UCS2Swapped = 7,
    //! @brief Built-in UCS-4 codec.
    UCS4 = UTF32,
    //! @brief Built-in UCS-4 (byteswapped) codec.
    UCS4Swapped = UTF32Swapped,

#if FOG_BYTE_ORDER == FOG_LITTLE_ENDIAN
    //! @brief Built-in UTF-16LE codec.
    UTF16LE = UTF16,
    //! @brief Built-in UTF-32LE codec.
    UTF32LE = UTF32,
    //! @brief Built-in UCS-2LE codec.
    UCS2LE = UCS2,
    //! @brief Built-in UCS-4LE codec.
    UCS4LE = UCS4,
    //! @brief Built-in UTF-16BE codec.
    UTF16BE = UTF16Swapped,
    //! @brief Built-in UTF-32BE codec.
    UTF32BE = UTF32Swapped,
    //! @brief Built-in UCS-2BE codec.
    UCS2BE = UCS2Swapped,
    //! @brief Built-in UCS-4BE codec.
    UCS4BE = UCS4Swapped,
#else
    //! @brief Built-in UTF-16LE codec.
    UTF16LE = UTF16Swapped,
    //! @brief Built-in UTF-32LE codec.
    UTF32LE = UTF32Swapped,
    //! @brief Built-in UCS-2LE codec.
    UCS2LE = UCS2Swapped,
    //! @brief Built-in UCS-4LE codec.
    UCS4LE = UCS4Swapped,
    //! @brief Built-in UTF-16LE codec.
    UTF16BE = UTF16,
    //! @brief Built-in UTF-32BE codec.
    UTF32BE = UTF32,
    //! @brief Built-in UCS-2BE codec.
    UCS2BE = UCS2,
    //! @brief Built-in UCS-4BE codec.
    UCS4BE = UCS4,
#endif

#if FOG_SIZEOF_WCHAR_T == 2
    WideChar = UTF16,
#else
    WideChar = UTF32,
#endif

    //! @brief Built-in ISO-8859-1 codec.
    ISO8859_1 = 8,
    //! @brief Built-in ISO-8859-2 codec.
    ISO8859_2 = 9,
    //! @brief Built-in ISO-8859-3 codec.
    ISO8859_3 = 10,
    //! @brief Built-in ISO-8859-4 codec.
    ISO8859_4 = 11,
    //! @brief Built-in ISO-8859-5 codec.
    ISO8859_5 = 12,
    //! @brief Built-in ISO-8859-6 codec.
    ISO8859_6 = 13,
    //! @brief Built-in ISO-8859-7 codec.
    ISO8859_7 = 14,
    //! @brief Built-in ISO-8859-8 codec.
    ISO8859_8 = 15,
    //! @brief Built-in ISO-8859-9 codec.
    ISO8859_9 = 16,
    //! @brief Built-in ISO-8859-10 codec.
    ISO8859_10 = 17,
    //! @brief Built-in ISO-8859-11 codec.
    ISO8859_11 = 18,
    //! @brief Built-in ISO-8859-13 codec.
    ISO8859_13 = 19,
    //! @brief Built-in ISO-8859-14 codec.
    ISO8859_14 = 20,
    //! @brief Built-in ISO-8859-16 codec.
    ISO8859_16 = 21,

    //! @brief Built-in CP-850 codec.
    CP850 = 22,
    //! @brief Built-in CP-866 codec.
    CP866 = 23,
    //! @brief Built-in CP-874 codec.
    CP874 = 24,
    //! @brief Built-in CP-1250 codec.
    CP1250 = 25,
    //! @brief Built-in CP-1251 codec.
    CP1251 = 26,
    //! @brief Built-in CP-1252 codec.
    CP1252 = 27,
    //! @brief Built-in CP-1253 codec.
    CP1253 = 28,
    //! @brief Built-in CP-1254 codec.
    CP1254 = 29,
    //! @brief Built-in CP-1255 codec.
    CP1255 = 30,
    //! @brief Built-in CP-1256 codec.
    CP1256 = 31,
    //! @brief Built-in CP-1257 codec.
    CP1257 = 32,
    //! @brief Built-in CP-1258 codec.
    CP1258 = 33,

    //! @brief Built-in APPLE-ROMAN codec.
    AppleRoman = 34,

    //! @brief Built-in KOI8R codec.
    KOI8R = 35,
    //! @brief Built-in KOI8U codec.
    KOI8U = 36,

    //! @brief Built-in WINSAMI-2 codec.
    WinSami2 = 37,

    //! @brief Built-in ROMAN-8 codec.
    Roman8 = 38,

    //! @brief Built-in ARMSCII-8 codec.
    Armscii8 = 39,

    //! @brief Built-in GEORGIAN-ACADEMY codec.
    GeorgianAcademy = 40,
    //! @brief Built-in GEORGIAN-PS codec.
    GeorgianPS = 41,

    //! @brief Last built-in codec (not valid type).
    Invalid = 42
  };

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  //! @brief Text codec flags that's used in @c TextCodecPrivate::flags.
  enum Flag
  {
    //! @brief Null text codec.
    IsNull = (1U << 0),
    //! @brief Unicode (UTF8, UTF16 or UTF32) text codec.
    IsUnicode = (1U << 1),
    //! @brief 8 bit text codec.
    Is8Bit = (1U << 2),
    //! @brief 16 bit text codec.
    Is16Bit = (1U << 3),
    //! @brief 32 bit text codec.
    Is32Bit = (1U << 4),
    //! @brief Character code size is variable (UTF8, UTF16 codecs).
    IsVariableSize = (1U << 5),
    //! @brief Swapped byte order (UTF16 or UTF32 codecs).
    IsByteSwapped = (1U << 6)
  };

  // --------------------------------------------------------------------------
  // [Page8]
  // --------------------------------------------------------------------------

  //! @brief 8 bit text codecs table.
  //! 
  //! This table is used to convert text from 8 bit encoding to / from
  //! unicode encoding like UTF8, UTF16 and UTF32.
  struct FOG_HIDDEN Page8
  {
    struct FOG_HIDDEN Encode
    {
      //! Unicode characters for extended 8 bit characters (upper 128 characters)
      uint16_t uc[128];
    };

    struct FOG_HIDDEN Decode
    {
      //! 8 bit encoded characters
      uint8_t uc[256];
    };

    Encode* encode;
    Decode* decode[256];
  };

  // --------------------------------------------------------------------------
  // [State]
  // --------------------------------------------------------------------------

  //! @brief Text codec encoding state.
  struct FOG_HIDDEN State
  {
    //! @brief Count of bytes in buffer.
    uint32_t count;

    //! @brief @c true if BOM is initialized.
    //! 
    //! For UTF-8, UTF-16 and UTF-32 codecs.
    bool bomInitialized;

    //! @brief @c true if BOM is byteswapped.
    //!
    //! For UTF-16 and UTF32 codecs.
    bool bomSwapped;

    //! @brief Small buffer used to remember unfinished sequences.
    char buffer[8];

    FOG_INLINE State() { clear(); }
    FOG_INLINE ~State() {}

    //! @brief Clears all variables to it's constructor defaults (all zero).
    FOG_INLINE void clear() { memset(this, 0, sizeof(*this)); }

    FOG_INLINE void setBomSwapped(bool swapped)
    {
      bomInitialized = true;
      bomSwapped = swapped;
    }
  };

  // --------------------------------------------------------------------------
  // [Replacer]
  // --------------------------------------------------------------------------

  //! @brief Callback function that can replace character that can't be encoded
  //! to target encoding.
  //!
  //! @param dst Destination, where can be written replacement.
  //! @param ch Unicode character to replace.
  //!
  //! @note This function can use only low 0-127 ascii characters.
  typedef err_t (*Replacer)(ByteArray& dst, uint32_t ch);

  // --------------------------------------------------------------------------
  // [Engine]
  // --------------------------------------------------------------------------

  struct FOG_API Engine
  {
    typedef TextCodec::Replacer Replacer;
    typedef TextCodec::State State;
    typedef TextCodec::Page8 Page8;

    // [Construction / Destruction]

    Engine(uint32_t code, uint32_t flags, const char* mime, const Page8* page8 = NULL);
    virtual ~Engine();

    // [Abstract]

    virtual err_t appendFromUnicode(ByteArray& dst, const Char* src, sysuint_t length, Replacer replacer, State* state) const = 0;
    virtual err_t appendToUnicode(String& dst, const void* src, sysuint_t size, State* state) const = 0;

    // [Implicit Sharing]

    FOG_INLINE Engine* ref() const { refCount.inc(); return (Engine*)this; }
    FOG_INLINE void deref() { if (refCount.deref()) delete this; }

    // [Members]

    mutable Atomic<sysuint_t> refCount;

    //! @brief Text codec code
    uint32_t code;

    //! @brief Text codec flags
    uint32_t flags;

    //! @brief Text codec mime and aliases.
    //!
    //! First ASCII string is Mime, after @c NULL terminator are aliases list, if
    //! @c NULL teminator has two characters (two @c NULL terminators), list ends.
    //! It possible to has only mime and no aliases.
    //!
    //! @note This mechanism is designed to decrease library binary size and
    //! relocations.
    const char* mime;

    // [8 Bit Codecs]

    //! @brief Table for 8 bit codecs (@c NULL if no tables are needed).
    const Page8* page8;

  private:
    FOG_DISABLE_COPY(Engine)
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextCodec();
  TextCodec(const TextCodec& other);
  TextCodec(Engine* d);
  ~TextCodec();

  // --------------------------------------------------------------------------
  // [From]
  // --------------------------------------------------------------------------

  static TextCodec fromCode(uint32_t code);
  static TextCodec fromMime(const char* mime);
  static TextCodec fromMime(const String& mime);
  static TextCodec fromBom(const void* data, sysuint_t length);

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::refCount().
  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }

  void free();

  // --------------------------------------------------------------------------
  // [Code / Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCode() const { return _d->code; }
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }
  FOG_INLINE bool isNull() const { return _d->code == None; }
  FOG_INLINE bool isUnicode() const { return (_d->flags & IsUnicode) != 0; }
  FOG_INLINE bool is8Bit() const { return (_d->flags & Is8Bit) != 0; }
  FOG_INLINE bool is16Bit() const { return (_d->flags & Is16Bit) != 0; }
  FOG_INLINE bool is32Bit() const { return (_d->flags & Is32Bit) != 0; }
  FOG_INLINE bool isVariableSize() const { return (_d->flags & IsVariableSize) != 0; }
  FOG_INLINE bool isByteSwapped() const { return (_d->flags & IsByteSwapped) != 0; }

  // --------------------------------------------------------------------------
  // [Mime]
  // --------------------------------------------------------------------------

  FOG_INLINE const char* getMime() const { return _d->mime; }

  // --------------------------------------------------------------------------
  // [8 Bit Tables]
  // --------------------------------------------------------------------------

  FOG_INLINE const Page8* getPage8() const { return _d->page8; }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  TextCodec& operator=(const TextCodec& other);

  // --------------------------------------------------------------------------
  // [Set]
  // --------------------------------------------------------------------------

  err_t setCode(uint32_t code);
  err_t setMime(const char* mime);
  err_t setMime(const String& mime);

  // --------------------------------------------------------------------------
  // [From/AppendFrom]
  // --------------------------------------------------------------------------

  err_t fromUnicode(ByteArray& dst, const Char* src, sysuint_t length = DETECT_LENGTH, Replacer replacer = NULL, State* state = NULL) const;
  err_t fromUnicode(ByteArray& dst, const String& src, Replacer replacer = NULL, State* state = NULL) const;

  err_t appendFromUnicode(ByteArray& dst, const Char* src, sysuint_t length = DETECT_LENGTH, Replacer replacer = NULL, State* state = NULL) const;
  err_t appendFromUnicode(ByteArray& dst, const String& src, Replacer replacer = NULL, State* state = NULL) const;

  // --------------------------------------------------------------------------
  // [To/AppendTo]
  // --------------------------------------------------------------------------

  err_t toUnicode(String& dst, const void* src, sysuint_t size = DETECT_LENGTH, State* state = NULL) const;
  err_t toUnicode(String& dst, const ByteArray& src, State* state = NULL) const;

  err_t appendToUnicode(String& dst, const void* src, sysuint_t size = DETECT_LENGTH, State* state = NULL) const;
  err_t appendToUnicode(String& dst, const ByteArray& src, State* state = NULL) const;

  // --------------------------------------------------------------------------
  // [BuiltIn]
  // --------------------------------------------------------------------------

  enum BuiltIn
  {
    BuiltInNull = 0,
    BuiltInAscii = 1,
    BuiltInLocal = 2,
    BuiltInUTF8 = 3,
    BuiltInUTF16 = 4,
    BuiltInUTF32 = 5,

#if FOG_SIZEOF_WCHAR_T == 2
    BuiltInWChar = BuiltInUTF16,
#else
    BuiltInWChar = BuiltInUTF32,
#endif // FOG_SIZEOF_WCHAR_T

    BuiltInCount = 6
  };

  static void* _codecs[BuiltInCount];

  static FOG_INLINE const TextCodec& ascii8() { return ((const TextCodec*)_codecs)[BuiltInAscii]; }
  static FOG_INLINE const TextCodec& local8() { return ((const TextCodec*)_codecs)[BuiltInLocal]; }
  static FOG_INLINE const TextCodec& utf8()   { return ((const TextCodec*)_codecs)[BuiltInUTF8 ]; }
  static FOG_INLINE const TextCodec& utf16()  { return ((const TextCodec*)_codecs)[BuiltInUTF16]; }
  static FOG_INLINE const TextCodec& utf32()  { return ((const TextCodec*)_codecs)[BuiltInUTF32]; }
  static FOG_INLINE const TextCodec& localW() { return ((const TextCodec*)_codecs)[BuiltInWChar]; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(Engine)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

FOG_DECLARE_TYPEINFO(Fog::TextCodec, Fog::TYPEINFO_MOVABLE)
FOG_DECLARE_TYPEINFO(Fog::TextCodec::State, Fog::TYPEINFO_PRIMITIVE)

// [Guard]
#endif // _FOG_CORE_TEXTCODEC_H
