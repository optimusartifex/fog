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

  typedef err_t (FOG_CDECL* MathF_Integrate)(float* dst, const FunctionF& func, const IntervalF& interval, uint32_t steps);
  typedef err_t (FOG_CDECL* MathD_Integrate)(double* dst, const FunctionD& func, const IntervalD& interval, uint32_t steps);

  typedef int (FOG_CDECL* MathF_Solve)(float* dst, const float* func);
  typedef int (FOG_CDECL* MathD_Solve)(double* dst, const double* func);

  typedef int (FOG_CDECL* MathF_SolveAt)(float* dst, const float* func, const IntervalF& interval);
  typedef int (FOG_CDECL* MathD_SolveAt)(double* dst, const double* func, const IntervalD& interval);

  typedef void (FOG_CDECL* MathF_VecFloatFromDouble)(float* dst, const double* src, size_t length);
  typedef void (FOG_CDECL* MathD_VecDoubleFromFloat)(double* dst, const float* src, size_t length);

  struct FOG_NO_EXPORT _FuncsMathF
  {
    MathF_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];

    MathF_Solve solve[MATH_SOLVE_COUNT];
    MathF_SolveAt solveAt[MATH_SOLVE_COUNT];

    MathF_VecFloatFromDouble vecFloatFromDouble;
  } mathf;

  struct FOG_NO_EXPORT _FuncsMathD
  {
    MathD_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];

    MathD_Solve solve[MATH_SOLVE_COUNT];
    MathD_SolveAt solveAt[MATH_SOLVE_COUNT];

    MathD_VecDoubleFromFloat vecDoubleFromFloat;
  } mathd;

  // --------------------------------------------------------------------------
  // [ByteArray]
  // --------------------------------------------------------------------------

  /*
  typedef void (FOG_CDECL *ByteArray_Ctor)(ByteArray* self);
  typedef void (FOG_CDECL *ByteArray_CtorCopy)(ByteArray* self, const ByteArray& stub);
  typedef void (FOG_CDECL *ByteArray_CtorCopy2)(ByteArray* self, const ByteArray& first, const ByteArray& second);
  typedef void (FOG_CDECL *ByteArray_CtorStub)(ByteArray* self, const Stub8& stub);
  typedef void (FOG_CDECL *ByteArray_CtorStub2)(ByteArray* self, const Stub8& first, const Stub8& second);
  typedef void (FOG_CDECL *ByteArray_Dtor)(ByteArray* self);

  typedef err_t (FOG_CDECL *ByteArray_Detach)(ByteArray* self);

  typedef err_t (FOG_CDECL *ByteArray_Reserve)(ByteArray* self, size_t min);
  typedef err_t (FOG_CDECL *ByteArray_Resize)(ByteArray* self, size_t len);
  typedef void (FOG_CDECL *ByteArray_Squeeze)(ByteArray* self);

  typedef void (FOG_CDECL *ByteArray_Clear)(ByteArray* self);
  typedef void (FOG_CDECL *ByteArray_Reset)(ByteArray* self);

  typedef char* (FOG_CDECL *ByteArray_Prepare)(ByteArray* self, size_t len, uint32_t cntOp);
  typedef char* (FOG_CDECL *ByteArray_Add)(ByteArray* self, size_t len);

  typedef err_t (FOG_CDECL *ByteArray_Copy)(ByteArray* self, const ByteArray& other);

  typedef err_t (FOG_CDECL *ByteArray_SetStub8)(ByteArray* self, const Stub8& stub);
  typedef err_t (FOG_CDECL *ByteArray_SetOther)(ByteArray* self, const ByteArray& other);

  typedef err_t (FOG_CDECL *ByteArray_AppendStub8)(ByteArray* self, const Stub8& stub);
  typedef err_t (FOG_CDECL *ByteArray_AppendOther)(ByteArray* self, const ByteArray& other);

  typedef err_t (FOG_CDECL *ByteArray_FromBool)(ByteArray* self, bool b);
  typedef err_t (FOG_CDECL *ByteArray_FromI32)(ByteArray* self, int32_t n, uint32_t base);
  typedef err_t (FOG_CDECL *ByteArray_FromU32)(ByteArray* self, uint32_t n, uint32_t base);
  typedef err_t (FOG_CDECL *ByteArray_FromI64)(ByteArray* self, int64_t n, uint32_t base);
  typedef err_t (FOG_CDECL *ByteArray_FromU64)(ByteArray* self, uint64_t n, uint32_t base);
  typedef err_t (FOG_CDECL *ByteArray_FromI32Ex)(ByteArray* self, int32_t n, uint32_t base, const FormatFlags& format);
  typedef err_t (FOG_CDECL *ByteArray_FromU32Ex)(ByteArray* self, uint32_t n, uint32_t base, const FormatFlags& format);
  typedef err_t (FOG_CDECL *ByteArray_FromI64Ex)(ByteArray* self, int64_t n, uint32_t base, const FormatFlags& format);
  typedef err_t (FOG_CDECL *ByteArray_FromU64Ex)(ByteArray* self, uint64_t n, uint32_t base, const FormatFlags& format);

  typedef err_t (FOG_CDECL *ByteArray_FromDouble)(ByteArray* self, double d, uint32_t df);
  typedef err_t (FOG_CDECL *ByteArray_FromDoubleEx)(ByteArray* self, double d, uint32_t df, const FormatFlags& ff);

  typedef err_t (FOG_CDECL *ByteArray_FromChars)(ByteArray* self, char ch, size_t len);

  typedef err_t (FOG_CDECL *ByteArray_FromFormatA)(ByteArray* self, const char* fmt, ...);
  typedef err_t (FOG_CDECL *ByteArray_FromFormatC)(ByteArray* self, const char* fmt, const TextCodec& tc, ...);
  typedef err_t (FOG_CDECL *ByteArray_FromVFormatA)(ByteArray* self, const char* fmt, va_list ap);
  typedef err_t (FOG_CDECL *ByteArray_FromVFormatC)(ByteArray* self, const char* fmt, const TextCodec& tc, va_list ap);

  typedef err_t (FOG_CDECL *ByteArray_FromWFormat)(ByteArray* self, const ByteArray& fmt, char lex, const List<ByteArray>& args);
  typedef err_t (FOG_CDECL *ByteArray_FromWFormat)(ByteArray* self, const ByteArray& fmt, char lex, const ByteArray* args, size_t len);

  typedef err_t (FOG_CDECL *ByteArray_PrependChars)(ByteArray* self, char ch, size_t len);
  typedef err_t (FOG_CDECL *ByteArray_PrependStub8)(ByteArray* self, const Stub8& stub);
  typedef err_t (FOG_CDECL *ByteArray_PrependOther)(ByteArray* self, const ByteArray& other);

  typedef err_t (FOG_CDECL *ByteArray_InsertChars)(ByteArray* self, size_t at, char ch, size_t len);
  typedef err_t (FOG_CDECL *ByteArray_InsertStub8)(ByteArray* self, size_t at, const Stub8& stub);
  typedef err_t (FOG_CDECL *ByteArray_InsertOther)(ByteArray* self, size_t at, const ByteArray& other);

  typedef size_t (FOG_CDECL *ByteArray_RemoveRange)(ByteArray* self, const Range& range);
  typedef size_t (FOG_CDECL *ByteArray_RemoveChars)(ByteArray* self, const Range* range, char ch, uint32_t cs);
  typedef size_t (FOG_CDECL *ByteArray_RemoveOther)(ByteArray* self, const Range* range, const ByteArray& other, uint32_t cs);
  typedef size_t (FOG_CDECL *ByteArray_RemoveFilter)(ByteArray* self, const Range* range, const ByteArrayFilter& filter, uint32_t cs);

  typedef err_t (FOG_CDECL *ByteArray_ReplaceRangeOther)(ByteArray* self, const Range& range, const ByteArray& after);
  typedef err_t (FOG_CDECL *ByteArray_ReplaceRangeStub8)(ByteArray* self, const Range& range, const Stub8& after);

  typedef err_t (FOG_CDECL *ByteArray_ReplaceChars)(ByteArray* self, const Range* range, char before, char after, uint cs = CASE_SENSITIVE);
  typedef err_t (FOG_CDECL *ByteArray_ReplaceString)(ByteArray* self, const Range* range, const ByteArray& before, const ByteArray& after, uint cs = CASE_SENSITIVE);
  typedef err_t (FOG_CDECL *ByteArray_ReplaceFilter)(ByteArray* self, const Range* range, const ByteArrayFilter& filter, const ByteArray& after, uint cs = CASE_SENSITIVE);

  // Maybe move to single function, TEXT_OP_UPPERCASE, TEXT_OP_LOWERCASE, TEXT_OP_CAPITALIZE, ... ?
  typedef err_t (FOG_CDECL *ByteArray_Lower)(ByteArray* self, const Range* range);
  typedef err_t (FOG_CDECL *ByteArray_Upper)(ByteArray* self, const Range* range);

  typedef err_t (FOG_CDECL *ByteArray_Trim)(ByteArray* self);
  typedef err_t (FOG_CDECL *ByteArray_Simplify)(ByteArray* self);
  typedef err_t (FOG_CDECL *ByteArray_Truncate)(ByteArray* self, size_t n);
  typedef err_t (FOG_CDECL *ByteArray_Justify)(ByteArray* self, size_t n, char ch, uint32_t flags);

  typedef err_t (FOG_CDECL *ByteArray_Split)(const ByteArray* self, List<ByteArray>& dst, char ch, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  typedef err_t (FOG_CDECL *ByteArray_Split)(const ByteArray* self, List<ByteArray>& dst, const ByteArray& pattern, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  typedef err_t (FOG_CDECL *ByteArray_Split)(const ByteArray* self, List<ByteArray>& dst, const ByteArrayFilter& filter, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;

  typedef err_t (FOG_CDECL *ByteArray_Slice)(ByteArray* self, const ByteArray& src, const Range& range);
  typedef err_t (FOG_CDECL *ByteArray_Join)(ByteArray* self, const List<ByteArray>& seq, const char separator);
  typedef err_t (FOG_CDECL *ByteArray_Join)(ByteArray* self, const List<ByteArray>& seq, const ByteArray& separator);

  typedef err_t (FOG_CDECL *ByteArray_ToBool)(const ByteArray* self, bool* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToI8)(const ByteArray* self, int8_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToU8)(const ByteArray* self, uint8_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToI16)(const ByteArray* self, int16_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToU16)(const ByteArray* self, uint16_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToI32)(const ByteArray* self, int32_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToU32)(const ByteArray* self, uint32_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToI64)(const ByteArray* self, int64_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToU64)(const ByteArray* self, uint64_t* dst, int base = 0, size_t* end = NULL, uint32_t* parserFlags) const;

  typedef err_t (FOG_CDECL *ByteArray_ToFloat)(const ByteArray* self, float* dst, size_t* end = NULL, uint32_t* parserFlags) const;
  typedef err_t (FOG_CDECL *ByteArray_ToDouble)(const ByteArray* self, double* dst, size_t* end = NULL, uint32_t* parserFlags) const;

  typedef bool (FOG_CDECL *ByteArray_Contains)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef bool (FOG_CDECL *ByteArray_Contains)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef bool (FOG_CDECL *ByteArray_Contains)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  typedef size_t (FOG_CDECL *ByteArray_CountOf)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef size_t (FOG_CDECL *ByteArray_CountOf)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef size_t (FOG_CDECL *ByteArray_CountOf)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  typedef size_t (FOG_CDECL *ByteArray_IndexOf)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef size_t (FOG_CDECL *ByteArray_IndexOf)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef size_t (FOG_CDECL *ByteArray_IndexOf)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  typedef size_t (FOG_CDECL *ByteArray_LastIndexOf)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef size_t (FOG_CDECL *ByteArray_LastIndexOf)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef size_t (FOG_CDECL *ByteArray_LastIndexOf)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  typedef size_t (FOG_CDECL *ByteArray_IndexOfAny)(const ByteArray* self, const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;
  typedef size_t (FOG_CDECL *ByteArray_LastIndexOfAny)(const ByteArray* self, const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range = Range(0)) const;

  typedef bool (FOG_CDECL *ByteArray_StartsWith)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_StartsWith)(const ByteArray* self, const Stub8& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_StartsWith)(const ByteArray* self, const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_StartsWith)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  typedef bool (FOG_CDECL *ByteArray_EndsWith)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_EndsWith)(const ByteArray* self, const Stub8& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_EndsWith)(const ByteArray* self, const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_EndsWith)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  typedef bool (FOG_CDECL *ByteArray_eq)(const ByteArray* a, const ByteArray* b);
  typedef bool (FOG_CDECL *ByteArray_ieq)(const ByteArray* a, const ByteArray* b);

  typedef int (FOG_CDECL *ByteArray_compare)(const ByteArray* a, const ByteArray* b);
  typedef int (FOG_CDECL *ByteArray_icompare)(const ByteArray* a, const ByteArray* b);

  typedef bool (FOG_CDECL *ByteArray_eq)(const Stub8& other, uint cs) const;
  typedef bool (FOG_CDECL *ByteArray_eq)(const ByteArray& other, uint cs) const;

  typedef int (FOG_CDECL *ByteArray_compare)(const Stub8& other, uint cs) const;
  typedef int (FOG_CDECL *ByteArray_compare)(const ByteArray& other, uint cs) const;

  typedef err_t (FOG_CDECL *ByteArray_validateUtf8)(size_t* invalidPos = NULL) const;
  typedef err_t (FOG_CDECL *ByteArray_getNumUtf8Chars)(size_t* charsCount) const;

  typedef err_t (FOG_CDECL *ByteArray_slashesToPosix)();
  typedef err_t (FOG_CDECL *ByteArray_slashesToWin)();

  typedef uint32_t (FOG_CDECL *ByteArray_getHashCode)() const;
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
