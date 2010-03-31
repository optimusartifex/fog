// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CONSTANTS_H
#define _FOG_CORE_CONSTANTS_H

// [Dependencies]
#include <Fog/Build/Build.h>

//! @addtogroup Fog_Core
//! @{

namespace Fog {

// ============================================================================
// [Fog::CreateSharable]
// ============================================================================

enum _CREATE_SHAREABLE { CREATE_SHAREABLE };
enum _DONT_INITIALIZE { DONT_INITIALIZE };

// ============================================================================
// [Fog::Constants]
// ============================================================================

//! @brief Detect length means that string function should first detect it length.
static const sysuint_t DETECT_LENGTH  = (sysuint_t)-1;
//! @brief Returned by functions like indexOf() to indicate that element wasn't
//! found.
static const sysuint_t INVALID_INDEX  = (sysuint_t)-1;

//! @brief Convenience value that can be used instead of zero.
static const uint32_t  NO_FLAGS = 0;

//! @brief how much to reserve stack for local strings in Fog library or all
//! additional libraries
static const sysuint_t TEMP_LENGTH = 256;

// ============================================================================
// [Fog::FORMAT_FLAGS]
// ============================================================================

static const sysuint_t NO_PRECISION = (sysuint_t)-1;
static const sysuint_t NO_WIDTH     = (sysuint_t)-1;

//! @brief String formatting flags.
enum FORMAT_FLAGS
{
  //! @brief Show group separators (sprintf ' flag).
  FORMAT_THOUSANDS_GROUP = (1 << 0),
  //! @brief Align to left (sprintf '-' flag).
  FORMAT_LEFT_ADJUSTED = (1 << 1),
  //! @brief Always show sign (sprintf '+' flag).
  FORMAT_SHOW_SIGN = (1 << 2),
  //! @brief Blank character instead of positive sign (sprintf ' ' flag).
  FORMAT_BLANK_POSITIVE = (1 << 3),
  //! @brief Use alternate form (sprintf '#' flag).
  FORMAT_ALTERNATE_FORM = (1 << 4),
  //! @brief Zero padded (sprintf '0' flag).
  FORMAT_ZERO_PADDED = (1 << 5),
  //! @brief Capitalize number output.
  FORMAT_CAPITALIZE = (1 << 6),
  //! @brief Capitalize E (floats) or X (hexadecimal) in number output.
  FORMAT_CAPITALIZE_E_OR_X = (1 << 7)
};

// ============================================================================
// [Fog::CASE_SENSITIVITY]
// ============================================================================

//! @brief Case sensitivity, used by @c String and @c ByteArray classes.
enum CASE_SENSITIVITY
{
  //! @brief Case insensitive (search, replace, indexOf, ...).
  CASE_INSENSITIVE = 0,
  //! @brief Case sensitive (search, replace, indexOf, ...).
  CASE_SENSITIVE = 1,

#if defined(FOG_OS_WINDOWS)
  CASE_FILESYSTEM = CASE_INSENSITIVE
#else
  CASE_FILESYSTEM = CASE_SENSITIVE
#endif // FOG_OS_WINDOWS
};

// ============================================================================
// [Fog::OUTPUT_MODE]
// ============================================================================

enum OUTPUT_MODE
{
  OUTPUT_MODE_SET = 0,
  OUTPUT_MODE_APPEND = 1
};

// ============================================================================
// [Fog::OUTPUT_CASE]
// ============================================================================

enum OUTPUT_CASE
{
  OUTPUT_CASE_LOWER = 0,
  OUTPUT_CASE_UPPER = 1
};

// ============================================================================
// [Fog::SplitBehavior]
// ============================================================================

enum SPLIT_BEHAVIOR
{
  SPLIT_KEEP_EMPTY_PARTS = 0,
  SPLIT_REMOVE_EMPTY_PARTS = 1
};

// ============================================================================
// [Fog::Justification]
// ============================================================================

enum JUSTIFY
{
  JUSTIFY_LEFT = 0x1,
  JUSTIFY_RIGHT = 0x2,
  JUSTIFY_CENTER = 0x3
};

// ============================================================================
// [Fog::SORT_ORDER]
// ============================================================================

enum SORT_ORDER
{
  SORT_ORDER_ASCENT = 0,
  SORT_ORDER_DESCENT = 1
};

// ============================================================================
// [Fog::DOUBLE_FORM]
// ============================================================================

enum DOUBLE_FORM
{
  //! @brief Decimal double form (sprintf %f).
  DF_DECIMAL = 0,
  //! @brief Exponent double form (sprintf %e).
  DF_EXPONENT = 1,
  //! @brief Significant digits (sprintf %g).
  DF_SIGNIFICANT_DIGITS = 2
};

// ============================================================================
// [Fog::LIBRARY_OPEN_MODE]
// ============================================================================

//! @brief Library open flags used in @c Library::open().
enum LIBRARY_OPEN_MODE
{
  //! @brief Don't use any flags.
  LIBRARY_OPEN_NONE = 0,

  //! @brief Open library with system prefix (default @c true).
  //!
  //! System prefix is mainly for systems like unix, where library prefix
  //! is usually 'lib', but plugins are usually linked without this prefix,
  //! but default is to use system prefix.
  LIBRARY_OPEN_SYSTEM_PREFIX = (1 << 0),

  //! @brief Open libray with system suffix (default @c true).
  //!
  //! System suffix = dot + library extension, for example in MS Windows
  //! this suffix is equal to '.dll', on unix like machines this suffix
  //! is '.so'. If you need to specify your own suffix, don't set this
  //! flag, because if you not use open flags argument. This flag is
  //! default.
  LIBRARY_OPEN_SYSTEM_SUFFIX = (1 << 1),

  //! @brief Default open flags for Library constructors and @c Library::open()
  //! methods.
  //!
  //! Default is to use @c OpenSystemPrefix with @c OpenSystemSuffix
  LIBRARY_OPEN_DEFAULT = LIBRARY_OPEN_SYSTEM_PREFIX | LIBRARY_OPEN_SYSTEM_SUFFIX
};

// ============================================================================
// [Fog::LOCALE_CHAR]
// ============================================================================

enum LOCALE_CHAR
{
  LOCALE_CHAR_DECIMAL_POINT = 0,
  LOCALE_CHAR_THOUSANDS_GROUP = 1,
  LOCALE_CHAR_ZERO = 2,
  LOCALE_CHAR_PLUS = 3,
  LOCALE_CHAR_MINUS = 4,
  LOCALE_CHAR_SPACE = 5,
  LOCALE_CHAR_EXPONENTIAL = 6,
  LOCALE_CHAR_FIRST_THOUSANDS_GROUP = 7,
  LOCALE_CHAR_NEXT_THOUSANDS_GROUP = 8,
  LOCALE_CHAR_RESERVED = 9,

  LOCALE_CHAR_INVALID
};

// ============================================================================
// [Fog::EVENT_CORE]
// ============================================================================

//! @brief Core event IDs.
enum EVENT_CORE_ENUM
{
  // [0 is reserved]

  EVENT_NULL = 0,

  // [Object events]

  EVENT_CREATE,
  EVENT_DESTROY,
  EVENT_DELETE,
  EVENT_TIMER,
  EVENT_CHANGE_PROPERTY = 10,

  //! @brief First user event.
  EVENT_USER = 65536,

  //! @brief Last event that can be used by all libraries.
  //!
  //! This number is first retrieved by Fog::Event::uid() and incremented
  //! each time that function is called.
  EVENT_UID = 10000000
};

// ============================================================================
// [Fog::ERR_OK_ENUM]
// ============================================================================

//! @brief Single enumeration to define @c ERR_OK value.
enum ERR_OK_ENUM
{
  //! @brief Function proceed without errors (success).
  ERR_OK = 0
};

// ============================================================================
// [Fog::ERR_CORE_ENUM]
// ============================================================================

//! @brief Error codes used in Fog-Core.
enum ERR_CORE_ENUM
{
  // --------------------------------------------------------------------------
  // [Core]
  // --------------------------------------------------------------------------

  ERR_CORE_START = 0x00010000,
  ERR_CORE_LAST  = 0x00010FFF,

  // --------------------------------------------------------------------------
  // [Runtime]
  // --------------------------------------------------------------------------

  ERR_RT_OUT_OF_MEMORY = ERR_CORE_START,
  ERR_RT_NOT_IMPLEMENTED,

  ERR_RT_INVALID_ARGUMENT,
  ERR_RT_INVALID_HANDLE,
  ERR_RT_INVALID_CONTEXT,

  ERR_RT_OBJECT_NOT_FOUND,
  ERR_RT_OBJECT_ALREADY_EXISTS,

  ERR_RT_OVERFLOW,

  // --------------------------------------------------------------------------
  // [Property]
  // --------------------------------------------------------------------------

  ERR_PROPERTY_INVALID_NAME,
  ERR_PROPERTY_INVALID_VALUE,
  ERR_PROPERTY_IS_READ_ONLY,

  // --------------------------------------------------------------------------
  // [IO]
  // --------------------------------------------------------------------------

  // TODO: What is difference between ERR_IO_TOO_BIG and ERR_IO_FILE_TOO_BIG.

  ERR_IO_TOO_BIG,
  ERR_IO_NOT_A_FILE,
  ERR_IO_NOT_A_DIRECTORY,
  ERR_IO_FILE_IS_EMPTY,

  ERR_IO_FILE_TOO_BIG,

  ERR_IO_CANT_READ,
  ERR_IO_CANT_WRITE,
  ERR_IO_CANT_SEEK,
  ERR_IO_CANT_RESIZE,
  ERR_IO_CANT_TRUNCATE,

  ERR_IO_FILE_NOT_EXISTS,
  ERR_IO_DIR_ALREADY_EXISTS,

  // --------------------------------------------------------------------------
  // [Library]
  // --------------------------------------------------------------------------

  ERR_LIB_LOAD_FAILED,
  ERR_LIB_SYMBOL_NOT_FOUND,

  // --------------------------------------------------------------------------
  // [String / TextCodec]
  // --------------------------------------------------------------------------

  //! @brief Invalid text input (converting strings to numbers).
  ERR_STRING_INVALID_INPUT,

  //! @brief Invalid text codec (Null) used to encode / decode string.
  ERR_STRING_INVALID_CODEC,

  //! @brief Invalid UTF-8 sequence.
  ERR_STRING_INVALID_UTF8,

  //! @brief Invalid UTF-16 sequence (surrogate pair error).
  ERR_STRING_INVALID_UTF16,

  //! @brief Invalid UCS-2 sequence (surrogate pair in UCS-2).
  ERR_STRING_INVALID_UCS2,

  //! @brief Invalid Unicode character.
  ERR_STRING_INVALID_CHAR,

  //! @brief Truncated (incomplete) input buffer.
  //!
  //! This error can be returned if you passed UTF-16 string to some function
  //! and the string ends with UTF-16 lead surrogate character.
  ERR_STRING_TRUNCATED,

  //! @brief Some characters lost during conversion (replaced by '?' or a given replacer).
  //!
  //! This can only happen when converting unicode to non-unicode encoding.
  ERR_STRING_LOST,

  // --------------------------------------------------------------------------
  // [Environment]
  // --------------------------------------------------------------------------

  ERR_ENV_GET_FAILED,
  ERR_ENV_SET_FAILED,

  // --------------------------------------------------------------------------
  // [User]
  // --------------------------------------------------------------------------

  ERR_USER_NO_HOME_DIRECTORY,
  ERR_USER_NO_XDG_DIRECTORY
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_CONSTANTS_H
