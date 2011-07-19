// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_TEXTCODEC_H
#define _FOG_CORE_TOOLS_TEXTCODEC_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/Ops.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Typedefs]
// ============================================================================

//! @brief Text-codec destroy function.
typedef void (FOG_CDECL *TextCodecDestroyFn)(TextCodecData* d);

//! @brief Text-codec encode function.
typedef err_t (FOG_CDECL *TextCodecEncodeFn)(const TextCodecData* d,
  ByteArray& dst, const Char* src, size_t srcLength, TextCodecState* state, TextCodecHandler* handler);

//! @brief Text-codec decode function.
typedef err_t (FOG_CDECL *TextCodecDecodeFn)(const TextCodecData* d,
  String& dst, const void* src, size_t srcSize, TextCodecState* state);

// ============================================================================
// [Fog::TextCodecPage8]
// ============================================================================

//! @brief Text-codec page used to convert between unicode and 8-bit encoding.
struct FOG_NO_EXPORT TextCodecPage8
{
  //! @brief Encode-table (from 8-bit code-point to unicode).
  struct FOG_NO_EXPORT Encode
  {
    //! Unicode characters from extended 8-bit characters (128 and above).
    uint16_t uc[128];
  };

  //! @brief Decode-table (from unicode to 8-bit code-point).
  struct FOG_NO_EXPORT Decode
  {
    //! 8 bit encoded characters
    uint8_t uc[256];
  };

  //! @brief Encode-table instance.
  Encode* encode;
  //! @brief Decode-table instances.
  Decode* decode[256];
};

// ============================================================================
// [Fog::TextCodecHandler]
// ============================================================================

//! @brief Text-codec handler
//!
//! Text-code handler can be used to catch characters which can't be encoded
//! to the target encoding. These characters can be replaced by the special
//! character or by sequence of other characters (for example xml-entity).
struct FOG_API TextCodecHandler
{
  virtual ~TextCodecHandler() = 0;

  //! @brief Callback to replace the character which can't be encoded into the
  //! target stream (buffer).
  //!
  //! @param dst Destination where to write the replacement.
  //! @param ch Unicode character to replace.
  //! @param pos Position relative to the input buffer used by the last
  //! call to TextCodec::encode() - for logging or extended error handling.
  //!
  //! @note This function should use only low 0-127 ascii characters on the
  //! output.
  virtual err_t replaceCharacter(ByteArray& dst, uint32_t uc) = 0;
};

// ============================================================================
// [Fog::TextCodecState]
// ============================================================================

//! @brief Text-codec state.
struct FOG_NO_EXPORT TextCodecState
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE TextCodecState()
  {
    Memory::zero_t<TextCodecState>(this);
  }

  FOG_INLINE ~TextCodecState()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint64_t getProcessedBytes() const { return _processedBytes; }
  FOG_INLINE void setProcessedBytes(uint64_t val) { _processedBytes = val; }

  FOG_INLINE uint32_t getBufferLength() const { return _bufferLength; }
  FOG_INLINE void setBufferLength(uint32_t len) { _bufferLength = (uint8_t)len; }

  FOG_INLINE bool isIncomplete() const { return _bufferLength > 0; }

  FOG_INLINE uint8_t hasBom() const { return _hasBom; }
  FOG_INLINE uint8_t isByteSwapped() const { return _isByteSwapped; }

  FOG_INLINE uint32_t getInvalidUtf8Codec() const { return _invalidUtf8Codec; }
  FOG_INLINE void setInvalidUtf8Codec(uint32_t codec) { _invalidUtf8Codec = (uint8_t)codec; }

  FOG_INLINE const uint8_t* getBuffer() const { return _buffer; }
  FOG_INLINE uint8_t* getBuffer() { return _buffer; }

  FOG_INLINE void setByteSwapped(uint8_t val)
  {
    _hasBom = true;
    _isByteSwapped = val;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Clears all variables to it's constructor defaults (all zero).
  FOG_INLINE void reset()
  {
    memset(this, 0, sizeof(*this));
  }

  FOG_INLINE void resetProcessedBytes()
  {
    _processedBytes = FOG_UINT64_C(0);
  }

  FOG_INLINE void resetBufferLength()
  {
    _bufferLength = 0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Count of bytes processed by the text-codec.
  uint64_t _processedBytes;

  //! @brief Count of bytes in buffer.
  uint8_t _bufferLength;

  //! @brief Whether the input text stream contains BOM (byte-order mark).
  uint8_t _hasBom;

  //! @brief Whteher the input text is byte-swapped (may change during advance).
  uint8_t _isByteSwapped;

  //! @brief Text-codec (ID) to use when an invalid UTF-8 sequence is found.
  uint8_t _invalidUtf8Codec;

  //! @brief Small buffer used to remember unfinished sequences.
  uint8_t _buffer[4];
};

// ============================================================================
// [Fog::TextCodecData]
// ============================================================================

struct FOG_NO_EXPORT TextCodecData
{
  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> refCount;

  //! @brief Destroy function (called when reference count is decreased to zero).
  TextCodecDestroyFn destroy;
  //! @brief Encode function.
  TextCodecEncodeFn encode;
  //! @brief Decode function.
  TextCodecDecodeFn decode;

  //! @brief Text-codec code (see @c TEXT_CODEC).
  uint32_t code;

  //! @brief Flags (see @c TEXT_CODEC_FLAGS).
  uint32_t flags;

  //! @brief Text codec mime and aliases.
  //!
  //! First ASCII string is Mime. After the @c NULL terminator there is alias-list.
  //! If alias-list contains two @c NULL teminators, list ends, otherwise there
  //! is the next alias. It is possible to have only MIME without aliases.
  //!
  //! This mechanism is designed to decrease run-time relocations needed to
  //! load the Fog-Framework library.
  const char* mime;

  //! @brief Table for 8 bit codecs (@c NULL if no tables are needed).
  const TextCodecPage8* page8;
};

// ============================================================================
// [Fog::TextCodec]
// ============================================================================

//! @brief Text-codec.
//!
//! Text-codec provides conversion methods from various text-encoding into
//! unicode (UTF-16) and vica-versa.
//!
//! Default text codec created by empty constructor is always @c TEXT_CODEC_NONE.
//!
//! Text codec contains built-in codecs:
//! - @c Fog::TextCodec::ascii8() - Ascii text-codec.
//! - @c Fog::TextCodec::local8() - Local 8-bit text-codec (depends on system).
//! - @c Fog::TextCodec::localW() - Local <code>wchar_t</code> text-codec (depends on OS).
//! - @c Fog::TextCodec::utf8() - UTF-8 text codec.
//! - @c Fog::TextCodec::utf16() - UTF-16 text codec.
//! - @c Fog::TextCodec::utf32() - UTF-32 text codec.
struct FOG_API TextCodec
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  TextCodec();
  TextCodec(const TextCodec& other);
  explicit FOG_INLINE TextCodec(TextCodecData* d) : _d(d) {}
  ~TextCodec();

  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->refCount.get(); }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCode() const { return _d->code; }
  FOG_INLINE uint32_t getFlags() const { return _d->flags; }
  FOG_INLINE const char* getMime() const { return _d->mime; }

  FOG_INLINE bool isTable() const { return (_d->flags & TEXT_ENCODING_IS_TABLE) != 0; }
  FOG_INLINE bool isUnicode() const { return (_d->flags & TEXT_ENCODING_IS_UNICODE) != 0; }

  FOG_INLINE bool isLittleEndian() const { return (_d->flags & TEXT_ENCODING_IS_LE) != 0; }
  FOG_INLINE bool isBigEndian() const { return (_d->flags & TEXT_ENCODING_IS_LE) != 0; }

  FOG_INLINE bool is8Bit() const { return (_d->flags & TEXT_ENCODING_IS_8BIT) != 0; }
  FOG_INLINE bool is16Bit() const { return (_d->flags & TEXT_ENCODING_IS_16BIT) != 0; }
  FOG_INLINE bool is32Bit() const { return (_d->flags & TEXT_ENCODING_IS_32BIT) != 0; }
  FOG_INLINE bool isVarLength() const { return (_d->flags & TEXT_ENCODING_IS_VARLEN) != 0; }

  FOG_INLINE const TextCodecPage8* getPage8() const { return _d->page8; }

  // --------------------------------------------------------------------------
  // [Create]
  // --------------------------------------------------------------------------

  err_t createFromCode(uint32_t code);
  err_t createFromMime(const char* mime);
  err_t createFromMime(const String& mime);
  err_t createFromBom(const void* data, size_t length);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Encode / Decode]
  // --------------------------------------------------------------------------

  err_t encode(ByteArray& dst, const Utf16& src, TextCodecState* state = NULL,
    TextCodecHandler* handler = NULL, uint32_t cntOp = CONTAINER_OP_REPLACE) const;

  err_t encode(ByteArray& dst, const String& src, TextCodecState* state = NULL,
    TextCodecHandler* handler = NULL, uint32_t cntOp = CONTAINER_OP_REPLACE) const;

  err_t decode(String& dst, const Stub8& src, TextCodecState* state = NULL,
    uint32_t cntOp = CONTAINER_OP_REPLACE) const;

  err_t decode(String& dst, const ByteArray& src, TextCodecState* state = NULL,
    uint32_t cntOp = CONTAINER_OP_REPLACE) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  TextCodec& operator=(const TextCodec& other);

  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  static uint8_t _cache[sizeof(void*) * TEXT_CODEC_CACHE_COUNT];

  static FOG_INLINE const TextCodec& ascii8() { return ((const TextCodec*)_cache)[TEXT_CODEC_CACHE_ASCII]; }
  static FOG_INLINE const TextCodec& local8() { return ((const TextCodec*)_cache)[TEXT_CODEC_CACHE_LOCAL]; }
  static FOG_INLINE const TextCodec& utf8()   { return ((const TextCodec*)_cache)[TEXT_CODEC_CACHE_UTF8 ]; }
  static FOG_INLINE const TextCodec& utf16()  { return ((const TextCodec*)_cache)[TEXT_CODEC_CACHE_UTF16]; }
  static FOG_INLINE const TextCodec& utf32()  { return ((const TextCodec*)_cache)[TEXT_CODEC_CACHE_UTF32]; }
  static FOG_INLINE const TextCodec& localW() { return ((const TextCodec*)_cache)[TEXT_CODEC_CACHE_WCHAR]; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(TextCodecData)
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPEINFO_DECLARE(Fog::TextCodec, Fog::TYPEINFO_MOVABLE)
_FOG_TYPEINFO_DECLARE(Fog::TextCodecState, Fog::TYPEINFO_PRIMITIVE)

// ============================================================================
// [Fog::Swap]
// ============================================================================

_FOG_SWAP_D(Fog::TextCodec)

// [Guard]
#endif // _FOG_CORE_TOOLS_TEXTCODEC_H
