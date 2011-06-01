// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_CONSTANTS_H
#define _FOG_CORE_GLOBAL_CONSTANTS_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::CONTAINER_DATA]
// ============================================================================

//! @brief Container data flags, used by many Fog-Framework classes.
enum CONTAINER_DATA
{
  //! @brief Container data is adopted from other container / custom memory
  //! location.
  //!
  //! There are some templates that allows to allocate some container data
  //! structures on the stack (for better performance). There are also
  //! containers which allows to adopt another data pointer to be used with
  //! the container instance (for example @c Image class).
  CONTAINER_DATA_STATIC = 0x01,

  //! @brief Container data is read-only.
  CONTAINER_DATA_READ_ONLY = 0x02,

  // TODO: REMOVE.

  //! @brief Data allocated by the container are private. Private data can't
  //! be weak-referenced so any time the reference is requested the deep-copy
  //! is created.
  CONTAINER_DATA_PRIVATE = 0x08
};

// ============================================================================
// [Fog::CONTAINER_OP]
// ============================================================================

//! @brief Container operation.
//!
//! Container operation is useful when working with two containers (same or
//! different type). The operation can be transformation of data in the source
//! container to target container. Sometimes it's useful
enum CONTAINER_OP
{
  //! @brief Replace operation.
  CONTAINER_OP_REPLACE = 0,
  //! @brief Append operation.
  CONTAINER_OP_APPEND = 1
};

// ============================================================================
// [Fog::DETECT_LENGTH / INVALID_INDEX]
// ============================================================================

//! @brief Detect length means that string function should first detect it length.
static const sysuint_t DETECT_LENGTH  = (sysuint_t)-1;
//! @brief Returned by functions like indexOf() to indicate that element wasn't
//! found.
static const sysuint_t INVALID_INDEX  = (sysuint_t)-1;

// ============================================================================
// [Fog::NO_FLAGS]
// ============================================================================

//! @brief Convenience value that can be used instead of zero.
static const uint32_t NO_FLAGS = 0;

// ============================================================================
// [Fog::CPU_FEATURE]
// ============================================================================

//! @brief CPU features.
enum CPU_FEATURE
{
  // [X86, X64]

  //! @brief Cpu has RDTSC instruction.
  CPU_FEATURE_RDTSC = 1U << 0,
  //! @brief Cpu has RDTSCP instruction.
  CPU_FEATURE_RDTSCP = 1U << 1,
  //! @brief Cpu has CMOV instruction (conditional move)
  CPU_FEATURE_CMOV = 1U << 2,
  //! @brief Cpu has CMPXCHG8B instruction
  CPU_FEATURE_CMPXCHG8B = 1U << 3,
  //! @brief Cpu has CMPXCHG16B instruction (64 bit processors)
  CPU_FEATURE_CMPXCHG16B = 1U << 4,
  //! @brief Cpu has CLFUSH instruction
  CPU_FEATURE_CLFLUSH = 1U << 5,
  //! @brief Cpu has PREFETCH instruction
  CPU_FEATURE_PREFETCH = 1U << 6,
  //! @brief Cpu supports LAHF and SAHF instrictions.
  CPU_FEATURE_LAHF_SAHF = 1U << 7,
  //! @brief Cpu supports FXSAVE and FXRSTOR instructions.
  CPU_FEATURE_FXSR = 1U << 8,
  //! @brief Cpu supports FXSAVE and FXRSTOR instruction optimizations (FFXSR).
  CPU_FEATURE_FFXSR = 1U << 9,

  //! @brief Cpu has MMX.
  CPU_FEATURE_MMX = 1U << 10,
  //! @brief Cpu has extended MMX.
  CPU_FEATURE_MMX_EXT = 1U << 11,
  //! @brief Cpu has 3dNow!
  CPU_FEATURE_3DNOW = 1U << 12,
  //! @brief Cpu has enchanced 3dNow!
  CPU_FEATURE_3DNOW_EXT = 1U << 13,
  //! @brief Cpu has SSE.
  CPU_FEATURE_SSE = 1U << 14,
  //! @brief Cpu has Misaligned SSE (MSSE).
  CPU_FEATURE_MSSE = 1U << 15,
  //! @brief Cpu has SSE2.
  CPU_FEATURE_SSE2 = 1U << 16,
  //! @brief Cpu has SSE3.
  CPU_FEATURE_SSE3 = 1U << 17,
  //! @brief Cpu has Supplemental SSE3 (SSSE3).
  CPU_FEATURE_SSSE3 = 1U << 18,
  //! @brief Cpu has SSE4.A.
  CPU_FEATURE_SSE4_A = 1U << 19,
  //! @brief Cpu has SSE4.1.
  CPU_FEATURE_SSE4_1 = 1U << 20,
  //! @brief Cpu has SSE4.2.
  CPU_FEATURE_SSE4_2 = 1U << 21,
  //! @brief Cpu has SSE5.
  CPU_FEATURE_SSE5 = 1U << 22,
  //! @brief Cpu supports MONITOR and MWAIT instructions.
  CPU_FEATURE_MONITOR_MWAIT = 1U << 23,
  //! @brief Cpu supports POPCNT instruction.
  CPU_FEATURE_POPCNT = 1U << 24,
  //! @brief Cpu supports LZCNT instruction.
  CPU_FEATURE_LZCNT  = 1U << 25,
  //! @brief Cpu supports multithreading.
  CPU_FEATURE_MULTITHREADING = 1U << 29,
  //! @brief Cpu supports execute disable bit (execute protection).
  CPU_FEATURE_EXECUTE_DISABLE_BIT = 1U << 30,
  //! @brief Cpu supports 64 bits.
  CPU_FEATURE_64_BIT = 1U << 31
};

// ============================================================================
// [Fog::CPU_BUG]
// ============================================================================

enum CPU_BUG
{
  CPU_BUG_AMD_LOCK_MB = 1U << 0
};

// ============================================================================
// [Fog::TEMPORARY_LENGTH]
// ============================================================================

//! @brief how much to reserve stack for local strings in Fog library or all
//! additional libraries.
static const sysuint_t TEMPORARY_LENGTH = 256;

// ============================================================================
// [Fog::FORMAT_FLAGS]
// ============================================================================

static const sysuint_t NO_PRECISION = (sysuint_t)-1;
static const sysuint_t NO_WIDTH     = (sysuint_t)-1;

//! @brief String formatting flags.
enum FORMAT_FLAGS
{
  //! @brief Show group separators (sprintf ' flag).
  FORMAT_THOUSANDS_GROUP = 0x0001,

  //! @brief Align to left (sprintf '-' flag).
  FORMAT_LEFT_ADJUSTED = 0x0002,

  //! @brief Always show sign (sprintf '+' flag).
  FORMAT_SHOW_SIGN = 0x0004,

  //! @brief Blank character instead of positive sign (sprintf ' ' flag).
  FORMAT_BLANK_POSITIVE = 0x0008,

  //! @brief Use alternate form (sprintf '#' flag).
  FORMAT_ALTERNATE_FORM = 0x0010,

  //! @brief Zero padded (sprintf '0' flag).
  FORMAT_ZERO_PADDED = 0x0020,

  //! @brief Capitalize number output.
  FORMAT_CAPITALIZE = 0x0040,

  //! @brief Capitalize E (floats) or X (hexadecimal) in number output.
  FORMAT_CAPITALIZE_E_OR_X = 0x0080
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
// [Fog::MATH_INTEGRATION_METHOD]
// ============================================================================

//! @brief Type of integration methods implemented by Fog::Math::integrate().
enum MATH_INTEGRATION_METHOD
{
  //! @brief Gauss-Legendre numerical integration.
  MATH_INTEGRATION_METHOD_GAUSS = 0,

  //! @brief Count of integration methods.
  MATH_INTEGRATION_METHOD_COUNT = 1
};

// ============================================================================
// [Fog::OUTPUT_CASE]
// ============================================================================

// TODO: Refactor, this name is not good, should be connected with text/string.
enum OUTPUT_CASE
{
  OUTPUT_CASE_LOWER = 0,
  OUTPUT_CASE_UPPER = 1
};

// ============================================================================
// [Fog::SPLIT_BEHAVIOR]
// ============================================================================

enum SPLIT_BEHAVIOR
{
  SPLIT_KEEP_EMPTY_PARTS = 0,
  SPLIT_REMOVE_EMPTY_PARTS = 1
};

// ============================================================================
// [Fog::JUSTIFY]
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
// [Fog::LIBRARY_OPEN]
// ============================================================================

//! @brief Library open flags used in @c Library::open().
enum LIBRARY_OPEN
{
  //! @brief Don't use any flags.
  LIBRARY_OPEN_NO_FLAGS = 0,

  //! @brief Open library with system prefix (default @c true).
  //!
  //! System prefix is mainly for systems like unix, where library prefix
  //! is usually 'lib', but plugins are usually linked without this prefix,
  //! but default is to use system prefix.
  LIBRARY_OPEN_SYSTEM_PREFIX = 0x0001,

  //! @brief Open libray with system suffix (default @c true).
  //!
  //! System suffix = dot + library extension, for example in MS Windows
  //! this suffix is equal to '.dll', on unix like machines this suffix
  //! is '.so'. If you need to specify your own suffix, don't set this
  //! flag, because if you not use open flags argument. This flag is
  //! default.
  LIBRARY_OPEN_SYSTEM_SUFFIX = 0x0002,

  //! @brief Default open flags for Library constructors and @c Library::open()
  //! methods.
  //!
  //! Default is to use @c OpenSystemPrefix with @c OpenSystemSuffix
  LIBRARY_OPEN_DEFAULT = 0x0003
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
// [Fog::USER_DIRECTORY]
// ============================================================================

enum USER_DIRECTORY
{
  USER_DIRECTORY_HOME = 0,
  USER_DIRECTORY_DESKTOP = 1,
  USER_DIRECTORY_DOCUMENTS = 2,
  USER_DIRECTORY_MUSIC = 3,
  USER_DIRECTORY_PICTURES = 4,
  USER_DIRECTORY_VIDEOS = 5,

  USER_DIRECTORY_COUNT
};

// ============================================================================
// [Fog::VALUE_TYPE]
// ============================================================================

//! @brief Value type id.
enum VALUE_TYPE
{
  //! @brief Value is null.
  VALUE_TYPE_NULL = 0,
  //! @brief Value is signed 32-bit integer.
  VALUE_TYPE_INTEGER = 1,
  //! @brief Value is double-precision floating point.
  VALUE_TYPE_DOUBLE = 2,
  //! @brief Value is string.
  VALUE_TYPE_STRING = 3
};

// ============================================================================
// [Fog::TYPEINFO_TYPE]
// ============================================================================

//! @brief Types for @c Fog::TypeInfo, use together with @c _FOG_TYPEINFO_DECLARE
//! macro.
enum TYPEINFO_TYPE
{
  //! @brief Simple data type like @c int, @c long, ...
  //!
  //! Simple data can be copyed from one memory location into another.
  TYPEINFO_PRIMITIVE = 0,

  //! @brief Moveable data type line @c Fog::String, ...
  //!
  //! Moveable data type can't be copyed to another location, but
  //! can be moved in memory.
  TYPEINFO_MOVABLE = 1,

  //! @brief Class data type.
  //!
  //! Means that class cannot be moved to another location.
  TYPEINFO_CLASS = 2
};

// ============================================================================
// [Fog::TYPEINFO_FLAGS]
// ============================================================================

//! @brief Additional flags for @c _FOG_TYPEINFO_DECLARE. All flags are initially
//! unsed when used @c _FOG_TYPEINFO_DECLARE to declare information about a type.
enum TYPEINFO_FLAGS
{
  TYPEINFO_IS_POD_TYPE    = 0x00000100,
  TYPEINFO_IS_FLOAT_TYPE  = 0x00000200,
  TYPEINFO_IS_DOUBLE_TYPE = 0x00000400,
  TYPEINFO_HAS_COMPARE    = 0x00000800,
  TYPEINFO_HAS_EQ         = 0x00001000,
  TYPEINFO_MASK           = 0xFFFFFF00
};

// ============================================================================
// [Fog::OBJECT_EVENT_HANDLER_PROTOTYPE]
// ============================================================================

//! @brief Object event handler prototype
enum OBJECT_EVENT_HANDLER_PROTOTYPE
{
  //! @brief Event handler not accepts arguments.
  OBJECT_EVENT_HANDLER_VOID = 0,
  //! @brief Event handler accepts <code>Event*</code> argument.
  OBJECT_EVENT_HANDLER_EVENTPTR = 1
};

// ============================================================================
// [Fog::OBJECT_EVENT_POLICY]
// ============================================================================

//! @brief @ref Object event handler behavior.
//!
//! @sa @ref Object.
enum OBJECT_EVENT_HANDLER_BEHAVIOR
{
  OBJECT_EVENT_HANDLER_REVERSE = 0,
  OBJECT_EVENT_HANDLER_CASCADE = 1,
  OBJECT_EVENT_HANDLER_OVERRIDE = 2
};

// ============================================================================
// [Fog::OBJECT_FLAG]
// ============================================================================

//! @brief @ref Object flags.
enum OBJECT_FLAG
{
  //! @brief Object instance can be deleted by @c delete operator (this is the
  //! default).
  OBJECT_FLAG_CAN_DELETE = (1 << 0),

  // OBJECT TODO: Remove?
  //HasChildAddedEventHandler = (1 << 2),
  //HasChildRemovedEventHandler = (1 << 3),
  //HasParentChangedEventHandler = (1 << 4),

  //! @brief Object was created and create event was received.
  OBJECT_FLAG_CREATED = (1 << 5),
  //! @brief Object will be destroyed, method @c destroyLater() was called.
  OBJECT_FLAG_DESTROY_LATER = (1 << 6),

  // --------------------------------------------------------------------------
  // [Class Identification]
  // --------------------------------------------------------------------------

  // These flags were designed for fast object identification in a Fog-Gui
  // library. Generally all flags above 1 << 16 are used by Fog-Gui.

  //! @brief The @ref Object instance is @ref Widget.
  OBJECT_FLAG_IS_WIDGET = (1 << 16),
  //! @brief The @ref Object instance is @ref Layout.
  OBJECT_FLAG_IS_LAYOUT = (1 << 17)
};

// ============================================================================
// [Fog::STREAM_DEVICE_FLAGS]
// ============================================================================

//! @brief Flags describing @c StreamDevice.
enum STREAM_DEVICE_FLAGS
{
  STREAM_IS_OPEN     = (1 << 0),
  STREAM_IS_SEEKABLE = (1 << 1),
  STREAM_IS_READABLE = (1 << 2),
  STREAM_IS_WRITABLE = (1 << 3),
  STREAM_IS_CLOSABLE = (1 << 4),

  STREAM_IS_HFILE    = (1 << 16),
  STREAM_IS_FD       = (1 << 17),
  STREAM_IS_MEMORY   = (1 << 18),
  STREAM_IS_GROWABLE = (1 << 19)
};

// ============================================================================
// [Fog::STREAM_OPEN_FLAGS]
// ============================================================================

//! @brief Stream open flags.
enum STREAM_OPEN_FLAGS
{
  STREAM_OPEN_READ = (1 << 0),
  STREAM_OPEN_WRITE = (1 << 1),
  STREAM_OPEN_RW = STREAM_OPEN_READ | STREAM_OPEN_WRITE,
  STREAM_OPEN_TRUNCATE = (1 << 2),
  STREAM_OPEN_APPEND = (1 << 3),
  STREAM_OPEN_CREATE = (1 << 4),
  STREAM_OPEN_CREATE_PATH = (1 << 5),
  STREAM_OPEN_CREATE_ONLY = (1 << 6)
};

// ============================================================================
// [Fog::STREAM_SEEK_MODE]
// ============================================================================

//! @brief Stream seek mode.
enum STREAM_SEEK_MODE
{
  STREAM_SEEK_SET = 0,
  STREAM_SEEK_CUR = 1,
  STREAM_SEEK_END = 2
};

// ============================================================================
// [Fog::EVENT_CORE]
// ============================================================================

//! @brief Core event IDs.
enum EVENT_CORE_ENUM
{
  // --------------------------------------------------------------------------
  // [Null]
  // --------------------------------------------------------------------------

  //! @brief Null event code.
  //!
  //! Reserved, shouldn't be used.
  EVENT_NULL = 0,

  // --------------------------------------------------------------------------
  // [Object - Create / Destroy]
  // --------------------------------------------------------------------------

  EVENT_CREATE,
  EVENT_DESTROY,
  EVENT_DELETE,

  // --------------------------------------------------------------------------
  // [Object - Properties]
  // --------------------------------------------------------------------------

  EVENT_PROPERTY,

  // --------------------------------------------------------------------------
  // [Object - Hierarchy]
  // --------------------------------------------------------------------------

  EVENT_CHILD_ADD,
  EVENT_CHILD_REMOVE,

  // --------------------------------------------------------------------------
  // [Timer]
  // --------------------------------------------------------------------------

  EVENT_TIMER,

  // --------------------------------------------------------------------------
  // [User]
  // --------------------------------------------------------------------------

  //! @brief First user event.
  EVENT_USER = 65536,

  // --------------------------------------------------------------------------
  // [UID]
  // --------------------------------------------------------------------------

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

  //! @brief First error code that can be used by Fog-Core library.
  ERR_CORE_FIRST = 0x00010000,
  //! @brief Last error code that can be used by Fog-Core library.
  ERR_CORE_LAST  = 0x00010FFF,

  // --------------------------------------------------------------------------
  // [Runtime]
  // --------------------------------------------------------------------------

  //! @brief Failed to allocate memory.
  ERR_RT_OUT_OF_MEMORY = ERR_CORE_FIRST,

  //! @brief Failed to create thread (or to get thread from a thread pool).
  ERR_RT_OUT_OF_THREADS,

  //! @brief Assertion failure when running in release-mode.
  //!
  //! Please use issue tracker if you get this error, because it can be caused
  //! by your code or Fog-Framework itself. Fog never return this error code
  //! when compiled in debug-mode, instead the assertion is raised.
  ERR_RT_ASSERTION_FAILURE,

  //! @brief Some code you wanted to use is not implemented yet.
  //!
  //! Please use issue tracker if feature you need is not implemented yet.
  ERR_RT_NOT_IMPLEMENTED,

  //! @brief Invalid argument passed to the Fog-Framework function.
  //!
  //! If you encounter such error then it's problem on the your side and you
  //! should fix your code.
  ERR_RT_INVALID_ARGUMENT,

  //! @brief Invalid context means that member method you called can't do the
  //! job, because instance state not allows it to do it.
  ERR_RT_INVALID_OBJECT,

  //! @brief Invalid state.
  ERR_RT_INVALID_STATE,

  //! @brief Overflow in integer or floating point arithmetic.
  ERR_RT_OVERFLOW,

  ERR_RT_BUSY,

  ERR_RT_INVALID_HANDLE,
  ERR_RT_OBJECT_NOT_FOUND,
  ERR_RT_OBJECT_ALREADY_EXISTS,

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

  ERR_LIBRARY_LOAD_FAILED,
  ERR_LIBRARY_SYMBOL_NOT_FOUND,

  // --------------------------------------------------------------------------
  // [Environment]
  // --------------------------------------------------------------------------

  ERR_ENV_GET_FAILED,
  ERR_ENV_SET_FAILED,

  // --------------------------------------------------------------------------
  // [User]
  // --------------------------------------------------------------------------

  ERR_USER_NO_HOME_DIRECTORY,
  ERR_USER_NO_XDG_DIRECTORY,

  // --------------------------------------------------------------------------
  // [Object]
  // --------------------------------------------------------------------------

  //! @brief Object is not part of a hierarchy.
  //!
  //! Tried to remove object from a bad ascendant.
  //!
  //! @note This is very likely a runtime error and should be reported.
  ERR_OBJECT_NOT_PART_OF_HIERARCHY,

  //! @brief Object has already part of a different hierarchy.
  //!
  //! Tried to add object to another, but the object was already added to
  //! another else. You must first remove it from its current parent.
  //!
  //! @note This is very likely a runtime error and should be reported.
  ERR_OBJECT_ALREADY_PART_OF_HIERARCHY,

  //! @brief Property not exists.
  ERR_OBJECT_INVALID_PROPERTY,
  //! @brief Property is read-only.
  ERR_OBJECT_READ_ONLY_PROPERTY,

  // TODO: Shouldn't we use ERR_RT_INVALID_ARGUMENT instead?
  ERR_OBJECT_INVALID_VALUE
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_CONSTANTS_H
