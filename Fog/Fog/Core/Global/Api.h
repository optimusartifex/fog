// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_API_H
#define _FOG_CORE_GLOBAL_API_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>
#include <Fog/Core/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

template<typename KeyT, typename ValueT> struct Hash;
template<typename KeyT, typename ValueT> struct HashP;

template<typename KeyT, typename ValueT> struct Map;
template<typename KeyT, typename ValueT> struct MapP;

// Collection.
template<typename ItemT> struct List;
template<typename ItemT> struct ListP;

// Math.
struct FunctionF;
struct FunctionD;
struct IntervalF;
struct IntervalD;

// Tools.
struct Byte;
struct ByteArray;
struct ByteArrayData;
struct ByteArrayFilter;
struct ByteArrayMatcher;
struct Char;
struct Locale;
struct Range;
struct String;
struct StringData;
struct StringFilter;
struct StringMatcher;
struct Var;
struct VarData;

// ============================================================================
// [Fog::CoreApi]
// ============================================================================

//! @internal
//!
//! @brief Fog/Core functions.
struct FOG_NO_EXPORT CoreApi
{
  // --------------------------------------------------------------------------
  // [Memory]
  // --------------------------------------------------------------------------

  typedef void* (FOG_CDECL *Memory_Alloc)(size_t size);
  typedef void* (FOG_CDECL *Memory_Calloc)(size_t size);
  typedef void* (FOG_CDECL *Memory_Realloc)(void* ptr, size_t size);
  typedef void (FOG_CDECL *Memory_Free)(void* ptr);

  typedef void (FOG_CDECL *Memory_Cleanup)();
  typedef err_t (FOG_CDECL *Memory_RegisterCleanupHandler)(void* handler, void* closure);
  typedef err_t (FOG_CDECL *Memory_UnregisterCleanupHandler)(void* handler, void* closure);

  typedef void* (FOG_CDECL *Memory_Copy)(void* dst, const void* src, size_t size);
  typedef void* (FOG_CDECL *Memory_Move)(void* dst, const void* src, size_t size);

  typedef void* (FOG_CDECL *Memory_Zero)(void* dst, size_t size);
  typedef void* (FOG_CDECL *Memory_Set)(void* dst, uint val, size_t size);

  typedef void (FOG_CDECL *Memory_Xchg)(void* mem0, void* mem1, size_t size);

  struct FOG_NO_EXPORT _FuncsMemory
  {
    // These functions are prefixed by _m_, because some memory leak detectors
    // use macros to replace malloc(), realloc(), free(), etc... This prevents
    // to be messed with that.
    Memory_Alloc _m_alloc;
    Memory_Calloc _m_calloc;
    Memory_Realloc _m_realloc;
    Memory_Free _m_free;

    Memory_Cleanup cleanup;
    Memory_RegisterCleanupHandler registerCleanupHandler;
    Memory_UnregisterCleanupHandler unregisterCleanupHandler;

    Memory_Copy copy;
    Memory_Move move;
    Memory_Zero zero;
    Memory_Set set;

    Memory_Copy copy_nt;
    Memory_Zero zero_nt;
    Memory_Set set_nt;

    Memory_Xchg xchg;
  } memory;

  // --------------------------------------------------------------------------
  // [Math]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL* MathF_VFloatFromDouble)(float* dst, const double* src, size_t length);
  typedef void (FOG_CDECL* MathD_VDoubleFromFloat)(double* dst, const float* src, size_t length);

  typedef err_t (FOG_CDECL* MathF_Integrate)(float* dst, const FunctionF& func, const IntervalF& interval, uint32_t steps);
  typedef err_t (FOG_CDECL* MathD_Integrate)(double* dst, const FunctionD& func, const IntervalD& interval, uint32_t steps);

  // TODO: Use MATH_SOLVE and single prototype instead of these.
  typedef int (FOG_CDECL* MathF_SolveQuadraticFunction)(float* dst, const float* src);
  typedef int (FOG_CDECL* MathD_SolveQuadraticFunction)(double* dst, const double* src);

  typedef int (FOG_CDECL* MathF_SolveQuadraticFunctionAt)(float* dst, const float* src, const IntervalF& interval);
  typedef int (FOG_CDECL* MathD_SolveQuadraticFunctionAt)(double* dst, const double* src, const IntervalD& interval);

  typedef int (FOG_CDECL* MathF_SolveCubicFunction)(float* dst, const float* src);
  typedef int (FOG_CDECL* MathD_SolveCubicFunction)(double* dst, const double* src);

  typedef int (FOG_CDECL* MathF_SolveCubicFunctionAt)(float* dst, const float* src, const IntervalF& interval);
  typedef int (FOG_CDECL* MathD_SolveCubicFunctionAt)(double* dst, const double* src, const IntervalD& interval);

  struct FOG_NO_EXPORT _FuncsMathF
  {
    MathF_VFloatFromDouble vFloatFromDouble;
    MathF_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];
    MathF_SolveQuadraticFunction solveQuadraticFunction;
    MathF_SolveQuadraticFunctionAt solveQuadraticFunctionAt;
    MathF_SolveCubicFunction solveCubicFunction;
    MathF_SolveCubicFunctionAt solveCubicFunctionAt;
  } mathf;

  struct FOG_NO_EXPORT _FuncsMathD
  {
    MathD_VDoubleFromFloat vDoubleFromFloat;
    MathD_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];
    MathD_SolveQuadraticFunction solveQuadraticFunction;
    MathD_SolveQuadraticFunctionAt solveQuadraticFunctionAt;
    MathD_SolveCubicFunction solveCubicFunction;
    MathD_SolveCubicFunctionAt solveCubicFunctionAt;
  } mathd;

  // --------------------------------------------------------------------------
  // [ByteArray]
  // --------------------------------------------------------------------------

  /*
  typedef void (FOG_CDECL *ByteArray_Ctor)(ByteArray* self);
  typedef void (FOG_CDECL *ByteArray_CtorCopy)(ByteArray* self, const ByteArray* other);
  typedef void (FOG_CDECL *ByteArray_CtorCopy2)(ByteArray* self, const ByteArray* first, const ByteArray* second);
  typedef void (FOG_CDECL *ByteArray_CtorStub)(ByteArray* self, const Stub8* stub);
  typedef void (FOG_CDECL *ByteArray_CtorStub2)(ByteArray* self, const Stub8* first, const Stub8* second);
  typedef void (FOG_CDECL *ByteArray_Dtor)(ByteArray* self);

  typedef err_t (FOG_CDECL *ByteArray_detach)(ByteArray* self);

  typedef err_t (FOG_CDECL *reserve)(ByteArray* self, size_t min);
  typedef err_t (FOG_CDECL *resize)(ByteArray* self, size_t len);
  typedef void (FOG_CDECL *squeeze)(ByteArray* self);

  typedef void (FOG_CDECL *clear)(ByteArray* self);
  typedef void (FOG_CDECL *reset)(ByteArray* self);

  typedef char* (FOG_CDECL *ByteArray_prepare)(ByteArray* self, size_t len, uint32_t cntOp);
  typedef char* (FOG_CDECL *ByteArray_add)(ByteArray* self, size_t len);

  typedef err_t (FOG_CDECL *fill)(ByteArray* self, char ch, size_t len);

  typedef err_t set(const Stub8& str);
  typedef err_t set(const ByteArray& other);

  typedef err_t setDeep(const ByteArray& other);

  typedef err_t setBool(bool b);

  typedef err_t setInt(int32_t n, int base = 10);
  typedef err_t setInt(uint32_t n, int base = 10);
  typedef err_t setInt(int64_t n, int base = 10);
  typedef err_t setInt(uint64_t n, int base = 10);

  typedef err_t setInt(int32_t n, int base, const FormatFlags& ff);
  typedef err_t setInt(uint32_t n, int base, const FormatFlags& ff);
  typedef err_t setInt(int64_t n, int base, const FormatFlags& ff);
  typedef err_t setInt(uint64_t n, int base, const FormatFlags& ff);

  typedef err_t setDouble(double d, int doubleForm = DF_SIGNIFICANT_DIGITS);
  typedef err_t setDouble(double d, int doubleForm, const FormatFlags& ff);

  typedef err_t format(const char* fmt, ...);
  typedef err_t formatc(const char* fmt, const TextCodec& tc, ...);
  typedef err_t vformat(const char* fmt, va_list ap);
  typedef err_t vformatc(const char* fmt, const TextCodec& tc, va_list ap);

  typedef err_t wformat(const ByteArray& fmt, char lex, const List<ByteArray>& args);
  typedef err_t wformat(const ByteArray& fmt, char lex, const ByteArray* args, size_t length);

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

  err_t atob(bool* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atoi8(int8_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atou8(uint8_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atoi16(int16_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atou16(uint16_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atoi32(int32_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atou32(uint32_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atoi64(int64_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atou64(uint64_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;

  err_t atof(float* dst, size_t* end = NULL, uint32_t* parserFlags) const;
  err_t atod(double* dst, size_t* end = NULL, uint32_t* parserFlagsNULL) const;

  bool contains(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  bool contains(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  size_t countOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t countOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t countOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  size_t indexOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t indexOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t indexOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  size_t lastIndexOf(char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOf(const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOf(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  size_t indexOfAny(const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  size_t lastIndexOfAny(const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  bool startsWith(char ch, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const Stub8& str, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  bool startsWith(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  bool endsWith(char ch, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const Stub8& str, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  bool endsWith(const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  static bool eq(const ByteArray* a, const ByteArray* b);
  static bool ieq(const ByteArray* a, const ByteArray* b);

  static int compare(const ByteArray* a, const ByteArray* b);
  static int icompare(const ByteArray* a, const ByteArray* b);

  bool eq(const Stub8& other, uint cs) const;
  bool eq(const ByteArray& other, uint cs) const;

  int compare(const Stub8& other, uint cs) const;
  int compare(const ByteArray& other, uint cs) const;

  err_t validateUtf8(size_t* invalidPos = NULL) const;
  err_t getNumUtf8Chars(size_t* charsCount) const;

  err_t slashesToPosix();
  err_t slashesToWin();

  uint32_t getHashCode() const;
  */










  // --------------------------------------------------------------------------
  // [String]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [StringUtil]
  // --------------------------------------------------------------------------
};

extern FOG_API CoreApi _core;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_API_H
