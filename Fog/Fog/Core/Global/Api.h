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
// [Fog::Forward Declarations]
// ============================================================================

// Core/Collection.
template<typename ItemT> struct List;
template<typename ItemT> struct ListP;

template<typename KeyT, typename ValueT> struct Hash;
template<typename KeyT, typename ValueT> struct HashP;

template<typename KeyT, typename ValueT> struct Map;
template<typename KeyT, typename ValueT> struct MapP;

// Core/Cpu.
struct Cpu;

// Core/DateTime.
struct Date;
struct DateDelta;
struct Time;
struct TimeDelta;
struct TimeTicks;

// Core/Global.
struct Range;

// Core/IO.
struct DirEntry;
struct DirIterator;
struct MapFile;
struct Stream;

// Core/Math.
struct FunctionF;
struct FunctionD;
struct IntervalF;
struct IntervalD;

// Core/System.
struct Application;
struct Event;
struct EventLoop;
struct Object;
struct Task;
struct Timer;

// Core/Threading.
struct Lock;
struct Thread;
struct ThreadCondition;
struct ThreadEvent;
struct ThreadLocal;
struct ThreadPool;

// Core/Tools.
struct Ascii8;
struct Byte;
struct ByteArray;
struct ByteArrayData;
struct ByteArrayFilter;
struct ByteArrayMatcher;
struct Char;
struct Local8;
struct Locale;
struct Range;
struct String;
struct StringData;
struct StringFilter;
struct StringMatcher;
struct Stub8;
struct TextCodec;
struct TextCodecData;
struct TextCodecHandler;
struct TextCodecState;
struct Utf8;
struct Utf16;

// Core/Variant.
struct Var;
struct VarData;

// Core/Xml
struct XmlDocument;
struct XmlElement;

// G2d/Geometry.
struct ArcF;
struct ArcD;
struct BoxI;
struct BoxF;
struct BoxD;
struct CBezierF;
struct CBezierD;
struct ChordF;
struct ChordD;
struct CircleF;
struct CircleD;
struct EllipseF;
struct EllipseD;
struct LineF;
struct LineD;
struct PathF;
struct PathD;
struct PathClipperF;
struct PathClipperD;
struct PathFlattenParamsF;
struct PathFlattenParamsD;
struct PathStrokerF;
struct PathStrokerD;
struct PathStrokerParamsF;
struct PathStrokerParamsD;
struct PieF;
struct PieD;
struct PointI;
struct PointF;
struct PointD;
struct QBezierF;
struct QBezierD;
struct RectI;
struct RectF;
struct RectD;
struct RoundF;
struct RoundD;
struct ShapeF;
struct ShapeD;
struct SizeI;
struct SizeF;
struct SizeD;
struct TransformF;
struct TransformD;
struct TriangleF;
struct TriangleD;

// G2d/Imaging.
struct Image;
struct ImageBits;
struct ImageCodec;
struct ImageCodecProvider;
struct ImageConverter;
struct ImageConverterClosure;
struct ImageConverterData;
struct ImageData;
struct ImageDecoder;
struct ImageEncoder;
struct ImageFormatDescription;
struct ImagePalette;
struct ImagePaletteData;

// G2d/Painting.
struct Painter;
struct PaintDevice;
struct PaintDeviceInfo;
struct PaintEngine;
struct PaintParamsF;
struct PaintParamsD;

// G2d/Text.

// G2d/Tools.
struct Dpi;
struct MatrixF;
struct MatrixD;
struct Region;
struct RegionData;

// G2d/Source.
struct AcmykF;
struct AcmykBaseF;
struct AhslF;
struct AhslBaseF;
struct AhsvF;
struct AhsvBaseF;
struct Argb32;
struct ArgbBase32;
struct Argb64;
struct ArgbBase64;
struct ArgbF;
struct ArgbBaseF;
struct Color;
struct ColorBase;
struct ColorStop;
struct ColorStopCache;
struct ColorStopList;
struct ConicalGradientF;
struct ConicalGradientD;
struct GradientF;
struct GradientD;
struct LinearGradientF;
struct LinearGradientD;
struct PatternF;
struct PatternD;
struct RadialGradientF;
struct RadialGradientD;
struct RectangularGradientF;
struct RectangularGradientD;
struct Texture;

// G2d/Text
struct Font;
struct FontData;
struct FontFace;
union  FontKerningChars;
struct FontKerningPairI;
struct FontKerningPairF;
struct FontKerningTableI;
struct FontKerningTableF;
struct FontManager;
struct FontManagerData;
struct FontProvider;
struct FontProviderData;
struct TextRectI;
struct TextRectF;
struct TextRectD;

// TODO:
struct ColorLutFx;
struct ColorLutTable;

// ============================================================================
// [Fog::Api]
// ============================================================================

//! @internal
//!
//! @brief Fog-Framework C-API.
struct FOG_NO_EXPORT Api
{
  // --------------------------------------------------------------------------
  // [Core/DateTime - Date]
  // --------------------------------------------------------------------------

  typedef int   (FOG_CDECL* Date_GetValue)(const Date* self, uint32_t key);
  typedef err_t (FOG_CDECL* Date_SetValue)(Date* self, uint32_t key, int value);
  typedef err_t (FOG_CDECL* Date_AddValue)(Date* self, uint32_t key, int64_t value);

  typedef err_t (FOG_CDECL* Date_SetYMD)(Date* self, int year, int month, int day);
  typedef err_t (FOG_CDECL* Date_SetHMS)(Date* self, int hour, int minute, int second, int us);
  typedef err_t (FOG_CDECL* Date_FromTime)(Date* self, const Time* time);

  typedef bool (FOG_CDECL* Date_IsLeapYear)(const Date* self, int year);
  typedef int (FOG_CDECL* Date_GetNumberOfDaysInYear)(const Date* self, int year);
  typedef int (FOG_CDECL* Date_GetNumberOfDaysInMonth)(const Date* self, int year, int month);

  typedef err_t (FOG_CDECL* Date_Convert)(Date* dst, const Date* src, uint32_t zone);

  struct FOG_NO_EXPORT _FuncsDate
  {
    Date_GetValue getValue;
    Date_SetValue setValue;
    Date_AddValue addValue;

    Date_SetYMD setYMD;
    Date_SetHMS setHMS;
    Date_FromTime fromTime;

    Date_IsLeapYear isLeapYear;
    Date_GetNumberOfDaysInYear getNumberOfDaysInYear;
    Date_GetNumberOfDaysInMonth getNumberOfDaysInMonth;

    Date_Convert convert;
  } date;

  // --------------------------------------------------------------------------
  // [Core/DateTime - Time]
  // --------------------------------------------------------------------------

  typedef int64_t (FOG_CDECL* Time_Now)(void);
  typedef time_t (FOG_CDECL* Time_ToTimeT)(int64_t us);
  typedef int64_t (FOG_CDECL* Time_FromTimeT)(time_t t);

  struct FOG_NO_EXPORT _FuncsTime
  {
    Time_Now now;
    Time_ToTimeT toTimeT;
    Time_FromTimeT fromTimeT;
  } time;

  // --------------------------------------------------------------------------
  // [Core/DateTime - TimeTicks]
  // --------------------------------------------------------------------------

  typedef int64_t (FOG_CDECL* TimeTicks_Now)(uint32_t ticksPrecision);

  struct FOG_NO_EXPORT _FuncsTimeTicks
  {
    TimeTicks_Now now;
  } timeticks;

  // --------------------------------------------------------------------------
  // [Core/Math - Math]
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
  // [Core/Threading - ThreadLocal]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL* ThreadLocal_Create)(uint32_t* slot, void* dtor);
  typedef err_t (FOG_CDECL* ThreadLocal_Destroy)(uint32_t slot);
  typedef void* (FOG_CDECL* ThreadLocal_Get)(uint32_t slot);
  typedef err_t (FOG_CDECL* ThreadLocal_Set)(uint32_t slot, void* val);

  struct FOG_NO_EXPORT _FuncsThreadLocal
  {
    ThreadLocal_Create create;
    ThreadLocal_Destroy destroy;
    ThreadLocal_Get get;
    ThreadLocal_Set set;
  } threadlocal;

  // --------------------------------------------------------------------------
  // [Core/Memory - Memory]
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
    // to be messed with them.
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
  // [Core/Tools - ByteArray]
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

  typedef err_t (FOG_CDECL *ByteArray_Lower)(ByteArray* self, const Range* range);
  typedef err_t (FOG_CDECL *ByteArray_Upper)(ByteArray* self, const Range* range);

  typedef err_t (FOG_CDECL *ByteArray_Trim)(ByteArray* self);
  typedef err_t (FOG_CDECL *ByteArray_Simplify)(ByteArray* self);
  typedef err_t (FOG_CDECL *ByteArray_Truncate)(ByteArray* self, size_t n);
  typedef err_t (FOG_CDECL *ByteArray_Justify)(ByteArray* self, size_t n, char ch, uint32_t flags);

  typedef err_t (FOG_CDECL *ByteArray_SplitCh)(const ByteArray* self, List<ByteArray>& dst, char ch, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  typedef err_t (FOG_CDECL *ByteArray_SplitBa)(const ByteArray* self, List<ByteArray>& dst, const ByteArray& pattern, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;
  typedef err_t (FOG_CDECL *ByteArray_SplitFl)(const ByteArray* self, List<ByteArray>& dst, const ByteArrayFilter& filter, uint splitBehavior = SPLIT_REMOVE_EMPTY_PARTS, uint cs = CASE_SENSITIVE) const;

  typedef err_t (FOG_CDECL *ByteArray_Slice)(const ByteArray* self, ByteArray* dst, const Range& range);
  typedef err_t (FOG_CDECL *ByteArray_JoinCh)(ByteArray* self, const List<ByteArray>& list, const char sep);
  typedef err_t (FOG_CDECL *ByteArray_JoinBa)(ByteArray* self, const List<ByteArray>& list, const ByteArray& sep);

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

  typedef bool (FOG_CDECL *ByteArray_ContainsCh)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef bool (FOG_CDECL *ByteArray_ContainsBa)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef bool (FOG_CDECL *ByteArray_ContainsFl)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range) const;

  typedef size_t (FOG_CDECL *ByteArray_CountOfCh)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef size_t (FOG_CDECL *ByteArray_CountOfBa)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef size_t (FOG_CDECL *ByteArray_CountOfFl)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range) const;

  typedef size_t (FOG_CDECL *ByteArray_IndexOfCh)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef size_t (FOG_CDECL *ByteArray_IndexOfBa)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef size_t (FOG_CDECL *ByteArray_IndexOfFl)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range) const;

  typedef size_t (FOG_CDECL *ByteArray_LastIndexOfCh)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef size_t (FOG_CDECL *ByteArray_LastIndexOfBa)(const ByteArray* self, const ByteArray& pattern, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef size_t (FOG_CDECL *ByteArray_LastIndexOfFl)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE, const Range& range) const;

  typedef size_t (FOG_CDECL *ByteArray_IndexOfAny)(const ByteArray* self, const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range) const;
  typedef size_t (FOG_CDECL *ByteArray_LastIndexOfAny)(const ByteArray* self, const char* chars, size_t numChars, uint cs = CASE_SENSITIVE, const Range& range) const;

  typedef bool (FOG_CDECL *ByteArray_StartsWithCh)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_StartsWithSb)(const ByteArray* self, const Stub8& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_StartsWithBa)(const ByteArray* self, const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_StartsWithFl)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  typedef bool (FOG_CDECL *ByteArray_EndsWithCh)(const ByteArray* self, char ch, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_EndsWithSb)(const ByteArray* self, const Stub8& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_EndsWithBa)(const ByteArray* self, const ByteArray& str, uint cs = CASE_SENSITIVE) const;
  typedef bool (FOG_CDECL *ByteArray_EndsWithFl)(const ByteArray* self, const ByteArrayFilter& filter, uint cs = CASE_SENSITIVE) const;

  typedef bool (FOG_CDECL *ByteArray_Eq)(const ByteArray* a, const ByteArray* b);
  typedef bool (FOG_CDECL *ByteArray_EqI)(const ByteArray* a, const ByteArray* b);

  typedef int (FOG_CDECL *ByteArray_Compare)(const ByteArray* a, const ByteArray* b);
  typedef int (FOG_CDECL *ByteArray_CompareI)(const ByteArray* a, const ByteArray* b);

  typedef bool (FOG_CDECL *ByteArray_Eq)(const Stub8& other, uint cs) const;
  typedef bool (FOG_CDECL *ByteArray_Eq)(const ByteArray& other, uint cs) const;

  typedef int (FOG_CDECL *ByteArray_Compare)(const Stub8& other, uint cs) const;
  typedef int (FOG_CDECL *ByteArray_Compare)(const ByteArray& other, uint cs) const;

  typedef err_t (FOG_CDECL *ByteArray_ValidateUtf8)(size_t* invalidPos = NULL) const;
  typedef err_t (FOG_CDECL *ByteArray_GetUtf8Length)(size_t* charsCount) const;

  typedef err_t (FOG_CDECL *ByteArray_slashesToPosix)();
  typedef err_t (FOG_CDECL *ByteArray_slashesToWin)();

  typedef uint32_t (FOG_CDECL *ByteArray_GetHashCode)() const;
  */

  // --------------------------------------------------------------------------
  // [Core/Tools - String]
  // --------------------------------------------------------------------------

  // --------------------------------------------------------------------------
  // [Core/Tools - StringUtil]
  // --------------------------------------------------------------------------













  // --------------------------------------------------------------------------
  // [G2d/Geometry - Arc]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *ArcF_GetBoundingBox)(const ArcF* self, BoxF* dst, const TransformF* tr, bool includeCenterPoint);
  typedef err_t (FOG_CDECL *ArcD_GetBoundingBox)(const ArcD* self, BoxD* dst, const TransformD* tr, bool includeCenterPoint);

  typedef uint (FOG_CDECL *ArcF_ToCSpline)(const ArcF* self, PointF* pts);
  typedef uint (FOG_CDECL *ArcD_ToCSpline)(const ArcD* self, PointD* pts);

  struct FOG_NO_EXPORT _FuncsArcF
  {
    ArcF_GetBoundingBox getBoundingBox;
    ArcF_ToCSpline toCSpline;
  } arcf;

  struct FOG_NO_EXPORT _FuncsArcD
  {
    ArcD_GetBoundingBox getBoundingBox;
    ArcD_ToCSpline toCSpline;
  } arcd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - CBezier]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *CBezierF_GetBoundingBox)(const PointF* self, BoxF* dst);
  typedef err_t (FOG_CDECL *CBezierD_GetBoundingBox)(const PointD* self, BoxD* dst);

  typedef err_t (FOG_CDECL *CBezierF_GetSplineBBox)(const PointF* self, size_t length, BoxF* dst);
  typedef err_t (FOG_CDECL *CBezierD_GetSplineBBox)(const PointD* self, size_t length, BoxD* dst);

  typedef void (FOG_CDECL *CBezierF_GetLength)(const PointF* self, float* length);
  typedef void (FOG_CDECL *CBezierD_GetLength)(const PointD* self, double* length);

  typedef int (FOG_CDECL *CBezierF_GetInflectionPoints)(const PointF* self, float*  t);
  typedef int (FOG_CDECL *CBezierD_GetInflectionPoints)(const PointD* self, double* t);

  typedef int (FOG_CDECL *CBezierF_SimplifyForProcessing)(const PointF* self, PointF* pts, float flatness);
  typedef int (FOG_CDECL *CBezierD_SimplifyForProcessing)(const PointD* self, PointD* pts, double flatness);

  typedef err_t (FOG_CDECL *CBezierF_Flatten)(const PointF* self, PathF& dst, uint8_t initialCommand, float flatness);
  typedef err_t (FOG_CDECL *CBezierD_Flatten)(const PointD* self, PathD& dst, uint8_t initialCommand, double flatness);

  struct FOG_NO_EXPORT _FuncsCBezierF
  {
    CBezierF_GetBoundingBox getBoundingBox;
    CBezierF_GetSplineBBox getSplineBBox;
    CBezierF_GetLength getLength;
    CBezierF_GetInflectionPoints getInflectionPoints;
    CBezierF_SimplifyForProcessing simplifyForProcessing;
    CBezierF_Flatten flatten;
  } cubiccurvef;

  struct FOG_NO_EXPORT _FuncsCBezierD
  {
    CBezierD_GetBoundingBox getBoundingBox;
    CBezierD_GetSplineBBox getSplineBBox;
    CBezierD_GetLength getLength;
    CBezierD_GetInflectionPoints getInflectionPoints;
    CBezierD_SimplifyForProcessing simplifyForProcessing;
    CBezierD_Flatten flatten;
  } cubiccurved;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Chord]
  // --------------------------------------------------------------------------

  typedef bool (FOG_CDECL *ChordF_HitTest)(const ChordF* self, const PointF* pt);
  typedef bool (FOG_CDECL *ChordD_HitTest)(const ChordD* self, const PointD* pt);

  struct FOG_NO_EXPORT _FuncsChordF
  {
    ChordF_HitTest hitTest;
  } chordf;

  struct FOG_NO_EXPORT _FuncsChordD
  {
    ChordD_HitTest hitTest;
  } chordd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Circle]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *CircleF_GetBoundingBox)(const CircleF* self, BoxF* dst, const TransformF* tr);
  typedef err_t (FOG_CDECL *CircleD_GetBoundingBox)(const CircleD* self, BoxD* dst, const TransformD* tr);

  typedef bool (FOG_CDECL *CircleF_HitTest)(const CircleF* self, const PointF* pt);
  typedef bool (FOG_CDECL *CircleD_HitTest)(const CircleD* self, const PointD* pt);

  typedef uint (FOG_CDECL *CircleF_ToCSpline)(const CircleF* self, PointF* pts);
  typedef uint (FOG_CDECL *CircleD_ToCSpline)(const CircleD* self, PointD* pts);

  struct FOG_NO_EXPORT _FuncsCircleF
  {
    CircleF_GetBoundingBox getBoundingBox;
    CircleF_HitTest hitTest;
    CircleF_ToCSpline toCSpline;
  } circlef;

  struct FOG_NO_EXPORT _FuncsCircleD
  {
    CircleD_GetBoundingBox getBoundingBox;
    CircleD_HitTest hitTest;
    CircleD_ToCSpline toCSpline;
  } circled;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Ellipse]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *EllipseF_GetBoundingBox)(const EllipseF* self, BoxF* dst, const TransformF* tr);
  typedef err_t (FOG_CDECL *EllipseD_GetBoundingBox)(const EllipseD* self, BoxD* dst, const TransformD* tr);

  typedef bool (FOG_CDECL *EllipseF_HitTest)(const EllipseF* self, const PointF* pt);
  typedef bool (FOG_CDECL *EllipseD_HitTest)(const EllipseD* self, const PointD* pt);

  typedef uint (FOG_CDECL *EllipseF_ToCSpline)(const EllipseF* self, PointF* pts);
  typedef uint (FOG_CDECL *EllipseD_ToCSpline)(const EllipseD* self, PointD* pts);

  struct FOG_NO_EXPORT _FuncsEllipseF
  {
    EllipseF_GetBoundingBox getBoundingBox;
    EllipseF_HitTest hitTest;
    EllipseF_ToCSpline toCSpline;
  } ellipsef;

  struct FOG_NO_EXPORT _FuncsEllipseD
  {
    EllipseD_GetBoundingBox getBoundingBox;
    EllipseD_HitTest hitTest;
    EllipseD_ToCSpline toCSpline;
  } ellipsed;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Line]
  // --------------------------------------------------------------------------

  typedef uint32_t (FOG_CDECL *LineF_Intersect)(
    PointF* dst,
    const PointF* lineA,
    const PointF* lineB);

  typedef uint32_t (FOG_CDECL *LineD_Intersect)(
    PointD* dst,
    const PointD* lineA,
    const PointD* lineB);

  struct FOG_NO_EXPORT _FuncsLineF
  {
    LineF_Intersect intersect;
  } linef;

  struct FOG_NO_EXPORT _FuncsLineD
  {
    LineD_Intersect intersect;
  } lined;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Path]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *PathF_Ctor)(PathF& self);
  typedef void (FOG_CDECL *PathD_Ctor)(PathD& self);

  typedef void (FOG_CDECL *PathF_CtorCopyF)(PathF& self, const PathF& other);
  typedef void (FOG_CDECL *PathD_CtorCopyD)(PathD& self, const PathD& other);

  typedef void (FOG_CDECL *PathF_Dtor)(PathF& self);
  typedef void (FOG_CDECL *PathD_Dtor)(PathD& self);

  typedef err_t (FOG_CDECL *PathF_Detach)(PathF& self);
  typedef err_t (FOG_CDECL *PathD_Detach)(PathD& self);

  typedef err_t (FOG_CDECL *PathF_Reserve)(PathF& self, size_t capacity);
  typedef err_t (FOG_CDECL *PathD_Reserve)(PathD& self, size_t capacity);

  typedef void (FOG_CDECL *PathF_Squeeze)(PathF& self);
  typedef void (FOG_CDECL *PathD_Squeeze)(PathD& self);

  typedef size_t (FOG_CDECL *PathF_Prepare)(PathF& self, size_t count, uint32_t cntOp);
  typedef size_t (FOG_CDECL *PathD_Prepare)(PathD& self, size_t count, uint32_t cntOp);

  typedef size_t (FOG_CDECL *PathF_Add)(PathF& self, size_t count);
  typedef size_t (FOG_CDECL *PathD_Add)(PathD& self, size_t count);

  typedef void (FOG_CDECL *PathF_UpdateFlat)(const PathF& self);
  typedef void (FOG_CDECL *PathD_UpdateFlat)(const PathD& self);

  typedef void (FOG_CDECL *PathF_Clear)(PathF& self);
  typedef void (FOG_CDECL *PathD_Clear)(PathD& self);

  typedef void (FOG_CDECL *PathF_Reset)(PathF& self);
  typedef void (FOG_CDECL *PathD_Reset)(PathD& self);

  typedef err_t (FOG_CDECL *PathF_SetPathF)(PathF& self, const PathF& other);
  typedef err_t (FOG_CDECL *PathD_SetPathF)(PathD& self, const PathF& other);
  typedef err_t (FOG_CDECL *PathD_SetPathD)(PathD& self, const PathD& other);

  typedef err_t (FOG_CDECL *PathF_SetDeepF)(PathF& self, const PathF& other);
  typedef err_t (FOG_CDECL *PathD_SetDeepD)(PathD& self, const PathD& other);

  typedef Range (FOG_CDECL *PathF_GetSubpathRange)(const PathF& self, size_t index);
  typedef Range (FOG_CDECL *PathD_GetSubpathRange)(const PathD& self, size_t index);

  typedef err_t (FOG_CDECL *PathF_MoveTo)(PathF& self, const PointF& pt0);
  typedef err_t (FOG_CDECL *PathD_MoveTo)(PathD& self, const PointD& pt0);

  typedef err_t (FOG_CDECL *PathF_LineTo)(PathF& self, const PointF& pt0);
  typedef err_t (FOG_CDECL *PathD_LineTo)(PathD& self, const PointD& pt0);

  typedef err_t (FOG_CDECL *PathF_HLineTo)(PathF& self, float x);
  typedef err_t (FOG_CDECL *PathD_HLineTo)(PathD& self, double x);

  typedef err_t (FOG_CDECL *PathF_VLineTo)(PathF& self, float y);
  typedef err_t (FOG_CDECL *PathD_VLineTo)(PathD& self, double y);

  typedef err_t (FOG_CDECL *PathF_PolyTo)(PathF& self, const PointF* pts, size_t count);
  typedef err_t (FOG_CDECL *PathD_PolyTo)(PathD& self, const PointD* pts, size_t count);

  typedef err_t (FOG_CDECL *PathF_QuadTo)(PathF& self, const PointF& pt1, const PointF& pt2);
  typedef err_t (FOG_CDECL *PathD_QuadTo)(PathD& self, const PointD& pt1, const PointD& pt2);

  typedef err_t (FOG_CDECL *PathF_CubicTo)(PathF& self, const PointF& pt1, const PointF& pt2, const PointF& pt3);
  typedef err_t (FOG_CDECL *PathD_CubicTo)(PathD& self, const PointD& pt1, const PointD& pt2, const PointD& pt3);

  typedef err_t (FOG_CDECL *PathF_SmoothQuadTo)(PathF& self, const PointF& pt2);
  typedef err_t (FOG_CDECL *PathD_SmoothQuadTo)(PathD& self, const PointD& pt2);

  typedef err_t (FOG_CDECL *PathF_SmoothCubicTo)(PathF& self, const PointF& pt1, const PointF& pt2);
  typedef err_t (FOG_CDECL *PathD_SmoothCubicTo)(PathD& self, const PointD& pt1, const PointD& pt2);

  typedef err_t (FOG_CDECL *PathF_ArcTo)(PathF& self, const PointF& cp, const PointF& rp, float start, float sweep, bool startPath);
  typedef err_t (FOG_CDECL *PathD_ArcTo)(PathD& self, const PointD& cp, const PointD& rp, double start, double sweep, bool startPath);

  typedef err_t (FOG_CDECL *PathF_SvgArcTo)(PathF& self, const PointF& rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF& pt);
  typedef err_t (FOG_CDECL *PathD_SvgArcTo)(PathD& self, const PointD& rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD& pt);

  typedef err_t (FOG_CDECL *PathF_Close)(PathF& self);
  typedef err_t (FOG_CDECL *PathD_Close)(PathD& self);

  typedef err_t (FOG_CDECL *PathF_BoxI)(PathF& self, const BoxI& r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxI)(PathD& self, const BoxI& r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_BoxF)(PathF& self, const BoxF& r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxF)(PathD& self, const BoxF& r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxD)(PathD& self, const BoxD& r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectI)(PathF& self, const RectI& r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectI)(PathD& self, const RectI& r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectF)(PathF& self, const RectF& r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectF)(PathD& self, const RectF& r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectD)(PathD& self, const RectD& r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_BoxesI)(PathF& self, const BoxI* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxesI)(PathD& self, const BoxI* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_BoxesF)(PathF& self, const BoxF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxesF)(PathD& self, const BoxF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxesD)(PathD& self, const BoxD* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectsI)(PathF& self, const RectI* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectsI)(PathD& self, const RectI* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectsF)(PathF& self, const RectF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectsF)(PathD& self, const RectF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectsD)(PathD& self, const RectD* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_Region)(PathF& self, const Region& r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_Region)(PathD& self, const Region& r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolylineI)(PathF& self, const PointI* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolylineI)(PathD& self, const PointI* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolylineF)(PathF& self, const PointF* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolylineD)(PathD& self, const PointD* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolygonI)(PathF& self, const PointI* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolygonI)(PathD& self, const PointI* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolygonF)(PathF& self, const PointF* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolygonD)(PathD& self, const PointD* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_Shape)(PathF& self, uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformF* tr);
  typedef err_t (FOG_CDECL *PathD_Shape)(PathD& self, uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformD* tr);

  typedef err_t (FOG_CDECL *PathF_AppendPathF)(PathF& self, const PathF& path, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendPathF)(PathD& self, const PathF& path, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendPathD)(PathD& self, const PathD& path, const Range* range);

  typedef err_t (FOG_CDECL *PathF_AppendTranslatedPathF)(PathF& self, const PathF& path, const PointF& pt, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTranslatedPathF)(PathD& self, const PathF& path, const PointD& pt, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTranslatedPathD)(PathD& self, const PathD& path, const PointD& pt, const Range* range);

  typedef err_t (FOG_CDECL *PathF_AppendTransformedPathF)(PathF& self, const PathF& path, const TransformF& tr, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTransformedPathF)(PathD& self, const PathF& path, const TransformD& tr, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTransformedPathD)(PathD& self, const PathD& path, const TransformD& tr, const Range* range);

  typedef err_t (FOG_CDECL *PathF_GetBoundingBox)(const PathF& self, BoxF* dst, const TransformF* transform);
  typedef err_t (FOG_CDECL *PathD_GetBoundingBox)(const PathD& self, BoxD* dst, const TransformD* transform);

  typedef bool (FOG_CDECL *PathF_HitTest)(const PathF& self, const PointF& pt, uint32_t fillRule);
  typedef bool (FOG_CDECL *PathD_HitTest)(const PathD& self, const PointD& pt, uint32_t fillRule);

  typedef err_t (FOG_CDECL *PathF_Translate)(PathF& self, const PointF& pt, const Range* range);
  typedef err_t (FOG_CDECL *PathD_Translate)(PathD& self, const PointD& pt, const Range* range);

  typedef err_t (FOG_CDECL *PathF_Transform)(PathF& self, const TransformF& tr, const Range* range);
  typedef err_t (FOG_CDECL *PathD_Transform)(PathD& self, const TransformD& tr, const Range* range);

  typedef err_t (FOG_CDECL *PathF_FitTo)(PathF& self, const RectF& r);
  typedef err_t (FOG_CDECL *PathD_FitTo)(PathD& self, const RectD& r);

  typedef err_t (FOG_CDECL *PathF_Scale)(PathF& self, const PointF& pt, bool keepStartPos);
  typedef err_t (FOG_CDECL *PathD_Scale)(PathD& self, const PointD& pt, bool keepStartPos);

  typedef err_t (FOG_CDECL *PathF_FlipX)(PathF& self, float x0, float x1);
  typedef err_t (FOG_CDECL *PathD_FlipX)(PathD& self, double x0, double x1);

  typedef err_t (FOG_CDECL *PathF_FlipY)(PathF& self, float y0, float y1);
  typedef err_t (FOG_CDECL *PathD_FlipY)(PathD& self, double y0, double y1);

  typedef err_t (FOG_CDECL *PathF_Flatten)(PathF& dst, const PathF& src, const PathFlattenParamsF& params, const Range* range);
  typedef err_t (FOG_CDECL *PathD_Flatten)(PathD& dst, const PathD& src, const PathFlattenParamsD& params, const Range* range);

  typedef bool (FOG_CDECL *PathF_Eq)(const PathF& a, const PathF& b);
  typedef bool (FOG_CDECL *PathD_Eq)(const PathD& a, const PathD& b);

  struct FOG_NO_EXPORT _FuncsPathF
  {
    PathF_Ctor ctor;
    PathF_CtorCopyF ctorCopyF;
    PathF_Dtor dtor;
    PathF_Detach detach;
    PathF_Reserve reserve;
    PathF_Squeeze squeeze;
    PathF_Prepare prepare;
    PathF_Add add;
    PathF_Clear clear;
    PathF_Reset reset;
    PathF_SetPathF setPathF;
    PathF_SetDeepF setDeepF;
    PathF_GetSubpathRange getSubpathRange;
    PathF_MoveTo moveTo;
    PathF_MoveTo moveToRel;
    PathF_LineTo lineTo;
    PathF_LineTo lineToRel;
    PathF_HLineTo hlineTo;
    PathF_HLineTo hlineToRel;
    PathF_VLineTo vlineTo;
    PathF_VLineTo vlineToRel;
    PathF_PolyTo polyTo;
    PathF_PolyTo polyToRel;
    PathF_QuadTo quadTo;
    PathF_QuadTo quadToRel;
    PathF_CubicTo cubicTo;
    PathF_CubicTo cubicToRel;
    PathF_SmoothQuadTo smoothQuadTo;
    PathF_SmoothQuadTo smoothQuadToRel;
    PathF_SmoothCubicTo smoothCubicTo;
    PathF_SmoothCubicTo smoothCubicToRel;
    PathF_ArcTo arcTo;
    PathF_ArcTo arcToRel;
    PathF_SvgArcTo svgArcTo;
    PathF_SvgArcTo svgArcToRel;
    PathF_Close close;
    PathF_BoxI boxI;
    PathF_BoxF boxF;
    PathF_RectI rectI;
    PathF_RectF rectF;
    PathF_BoxesI boxesI;
    PathF_BoxesF boxesF;
    PathF_RectsI rectsI;
    PathF_RectsF rectsF;
    PathF_Region region;
    PathF_PolylineI polylineI;
    PathF_PolylineF polylineF;
    PathF_PolygonI polygonI;
    PathF_PolygonF polygonF;
    PathF_Shape shape;
    PathF_AppendPathF appendPathF;
    PathF_AppendTranslatedPathF appendTranslatedPathF;
    PathF_AppendTransformedPathF appendTransformedPathF;
    PathF_UpdateFlat updateFlat;
    PathF_Flatten flatten;
    PathF_GetBoundingBox getBoundingBox;
    PathF_HitTest hitTest;
    PathF_Translate translate;
    PathF_Transform transform;
    PathF_FitTo fitTo;
    PathF_Scale scale;
    PathF_FlipX flipX;
    PathF_FlipY flipY;
    PathF_Eq eq;
  } pathf;

  struct FOG_NO_EXPORT _FuncsPathD
  {
    PathD_Ctor ctor;
    PathD_CtorCopyD ctorCopyD;
    PathD_Dtor dtor;
    PathD_Detach detach;
    PathD_Reserve reserve;
    PathD_Squeeze squeeze;
    PathD_Prepare prepare;
    PathD_Add add;
    PathD_Clear clear;
    PathD_Reset reset;
    PathD_SetPathD setPathD;
    PathD_SetPathF setPathF;
    PathD_SetDeepD setDeepD;
    PathD_GetSubpathRange getSubpathRange;
    PathD_MoveTo moveTo;
    PathD_MoveTo moveToRel;
    PathD_LineTo lineTo;
    PathD_LineTo lineToRel;
    PathD_HLineTo hlineTo;
    PathD_HLineTo hlineToRel;
    PathD_VLineTo vlineTo;
    PathD_VLineTo vlineToRel;
    PathD_PolyTo polyTo;
    PathD_PolyTo polyToRel;
    PathD_QuadTo quadTo;
    PathD_QuadTo quadToRel;
    PathD_CubicTo cubicTo;
    PathD_CubicTo cubicToRel;
    PathD_SmoothQuadTo smoothQuadTo;
    PathD_SmoothQuadTo smoothQuadToRel;
    PathD_SmoothCubicTo smoothCubicTo;
    PathD_SmoothCubicTo smoothCubicToRel;
    PathD_ArcTo arcTo;
    PathD_ArcTo arcToRel;
    PathD_SvgArcTo svgArcTo;
    PathD_SvgArcTo svgArcToRel;
    PathD_Close close;
    PathD_BoxI boxI;
    PathD_BoxF boxF;
    PathD_BoxD boxD;
    PathD_RectI rectI;
    PathD_RectF rectF;
    PathD_RectD rectD;
    PathD_BoxesI boxesI;
    PathD_BoxesF boxesF;
    PathD_BoxesD boxesD;
    PathD_RectsI rectsI;
    PathD_RectsF rectsF;
    PathD_RectsD rectsD;
    PathD_Region region;
    PathD_PolylineI polylineI;
    PathD_PolylineD polylineD;
    PathD_PolygonI polygonI;
    PathD_PolygonD polygonD;
    PathD_Shape shape;
    PathD_AppendPathF appendPathF;
    PathD_AppendPathD appendPathD;
    PathD_AppendTranslatedPathF appendTranslatedPathF;
    PathD_AppendTranslatedPathD appendTranslatedPathD;
    PathD_AppendTransformedPathF appendTransformedPathF;
    PathD_AppendTransformedPathD appendTransformedPathD;
    PathD_UpdateFlat updateFlat;
    PathD_Flatten flatten;
    PathD_GetBoundingBox getBoundingBox;
    PathD_HitTest hitTest;
    PathD_Translate translate;
    PathD_Transform transform;
    PathD_FitTo fitTo;
    PathD_Scale scale;
    PathD_FlipX flipX;
    PathD_FlipY flipY;
    PathD_Eq eq;
  } pathd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathClipper]
  // --------------------------------------------------------------------------

  typedef uint32_t (FOG_CDECL *PathClipperF_InitPath)(PathClipperF& self, const PathF& src);
  typedef uint32_t (FOG_CDECL *PathClipperD_InitPath)(PathClipperD& self, const PathD& src);

  typedef err_t (FOG_CDECL *PathClipperF_ContinuePath)(PathClipperF& self, PathF& dst, const PathF& src);
  typedef err_t (FOG_CDECL *PathClipperD_ContinuePath)(PathClipperD& self, PathD& dst, const PathD& src);

  typedef err_t (FOG_CDECL *PathClipperF_ContinueRaw)(PathClipperF& self, PathF& dst, const PointF* srcPts, const uint8_t* srcCmd, size_t srcLength);
  typedef err_t (FOG_CDECL *PathClipperD_ContinueRaw)(PathClipperD& self, PathD& dst, const PointD* srcPts, const uint8_t* srcCmd, size_t srcLength);

  typedef err_t (FOG_CDECL *PathClipperF_ClipPath)(PathClipperF& self, PathF& dst, const PathF& src, const TransformF* tr);
  typedef err_t (FOG_CDECL *PathClipperD_ClipPath)(PathClipperD& self, PathD& dst, const PathD& src, const TransformD* tr);

  struct FOG_NO_EXPORT _FuncsPathClipperF
  {
    PathClipperF_InitPath initPath;
    PathClipperF_ContinuePath continuePath;
    PathClipperF_ContinueRaw continueRaw;
    PathClipperF_ClipPath clipPath;
  } pathclipperf;

  struct FOG_NO_EXPORT _FuncsPathClipperD
  {
    PathClipperD_InitPath initPath;
    PathClipperD_ContinuePath continuePath;
    PathClipperD_ContinueRaw continueRaw;
    PathClipperD_ClipPath clipPath;
  } pathclipperd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Pie]
  // --------------------------------------------------------------------------

  typedef bool (FOG_CDECL *PieF_HitTest)(const PieF* self, const PointF* pt);
  typedef bool (FOG_CDECL *PieD_HitTest)(const PieD* self, const PointD* pt);

  struct FOG_NO_EXPORT _FuncsPieF
  {
    PieF_HitTest hitTest;
  } pief;

  struct FOG_NO_EXPORT _FuncsPieD
  {
    PieD_HitTest hitTest;
  } pied;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - QBezier]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *QBezierF_GetBoundingBox)(const PointF* self, BoxF* dst);
  typedef err_t (FOG_CDECL *QBezierD_GetBoundingBox)(const PointD* self, BoxD* dst);

  typedef err_t (FOG_CDECL *QBezierF_GetSplineBBox)(const PointF* self, size_t length, BoxF* dst);
  typedef err_t (FOG_CDECL *QBezierD_GetSplineBBox)(const PointD* self, size_t length, BoxD* dst);

  typedef void (FOG_CDECL *QBezierF_GetLength)(const PointF* self, float* length);
  typedef void (FOG_CDECL *QBezierD_GetLength)(const PointD* self, double* length);

  typedef err_t (FOG_CDECL *QBezierF_Flatten)(const PointF* self, PathF& dst, uint8_t initialCommand, float flatness);
  typedef err_t (FOG_CDECL *QBezierD_Flatten)(const PointD* self, PathD& dst, uint8_t initialCommand, double flatness);

  struct FOG_NO_EXPORT _FuncsQBezierF
  {
    QBezierF_GetBoundingBox getBoundingBox;
    QBezierF_GetSplineBBox getSplineBBox;
    QBezierF_GetLength getLength;
    QBezierF_Flatten flatten;
  } quadcurvef;

  struct FOG_NO_EXPORT _FuncsQBezierD
  {
    QBezierD_GetBoundingBox getBoundingBox;
    QBezierD_GetSplineBBox getSplineBBox;
    QBezierD_GetLength getLength;
    QBezierD_Flatten flatten;
  } quadcurved;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Round]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *RoundF_GetBoundingBox)(const RoundF* self, BoxF* dst, const TransformF* tr);
  typedef err_t (FOG_CDECL *RoundD_GetBoundingBox)(const RoundD* self, BoxD* dst, const TransformD* tr);

  typedef bool (FOG_CDECL *RoundF_HitTest)(const RoundF* self, const PointF* pt);
  typedef bool (FOG_CDECL *RoundD_HitTest)(const RoundD* self, const PointD* pt);

  struct FOG_NO_EXPORT _FuncsRoundF
  {
    RoundF_GetBoundingBox getBoundingBox;
    RoundF_HitTest hitTest;
  } roundf;

  struct FOG_NO_EXPORT _FuncsRoundD
  {
    RoundD_GetBoundingBox getBoundingBox;
    RoundD_HitTest hitTest;
  } roundd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Shape]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *ShapeF_GetBoundingBox)(uint32_t shapeType, const void* shapeData, BoxF* dst, const TransformF* transform);
  typedef err_t (FOG_CDECL *ShapeD_GetBoundingBox)(uint32_t shapeType, const void* shapeData, BoxD* dst, const TransformD* transform);

  typedef bool (FOG_CDECL *ShapeF_HitTest)(uint32_t shapeType, const void* shapeData, const PointF* pt);
  typedef bool (FOG_CDECL *ShapeD_HitTest)(uint32_t shapeType, const void* shapeData, const PointD* pt);

  struct FOG_NO_EXPORT _FuncsShapeF
  {
    ShapeF_GetBoundingBox getBoundingBox;
    ShapeF_HitTest hitTest;
  } shapef;

  struct FOG_NO_EXPORT _FuncsShapeD
  {
    ShapeD_GetBoundingBox getBoundingBox;
    ShapeD_HitTest hitTest;
  } shaped;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Transform]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *TransformF_Create)(TransformF& self, uint32_t createType, const void* params);
  typedef err_t (FOG_CDECL *TransformD_Create)(TransformD& self, uint32_t createType, const void* params);

  typedef uint32_t (FOG_CDECL *TransformF_Update)(const TransformF& self);
  typedef uint32_t (FOG_CDECL *TransformD_Update)(const TransformD& self);

  typedef err_t (FOG_CDECL *TransformF_Transform)(TransformF& self, uint32_t transformOp, const void* params);
  typedef err_t (FOG_CDECL *TransformD_Transform)(TransformD& self, uint32_t transformOp, const void* params);

  typedef TransformF (FOG_CDECL *TransformF_Transformed)(const TransformF& self, uint32_t transformOp, const void* params);
  typedef TransformD (FOG_CDECL *TransformD_Transformed)(const TransformD& self, uint32_t transformOp, const void* params);

  typedef void (FOG_CDECL *TransformF_Multiply)(TransformF& self, const TransformF& a, const TransformF& b);
  typedef void (FOG_CDECL *TransformD_Multiply)(TransformD& self, const TransformD& a, const TransformD& b);

  typedef bool (FOG_CDECL *TransformF_Invert)(TransformF& self, const TransformF& a);
  typedef bool (FOG_CDECL *TransformD_Invert)(TransformD& self, const TransformD& a);

  typedef void (FOG_CDECL *TransformF_MapPointF)(const TransformF& self, PointF& dst, const PointF& src);
  typedef void (FOG_CDECL *TransformD_MapPointD)(const TransformD& self, PointD& dst, const PointD& src);

  typedef void (FOG_CDECL *TransformF_MapPointsF)(const TransformF& self, PointF* dst, const PointF* src, size_t length);
  typedef void (FOG_CDECL *TransformD_MapPointsF)(const TransformD& self, PointD* dst, const PointF* src, size_t length);
  typedef void (FOG_CDECL *TransformD_MapPointsD)(const TransformD& self, PointD* dst, const PointD* src, size_t length);

  typedef err_t (FOG_CDECL *TransformF_MapPathF)(const TransformF& self, PathF& dst, const PathF& src, uint32_t cntOp);
  typedef err_t (FOG_CDECL *TransformD_MapPathF)(const TransformD& self, PathD& dst, const PathF& src, uint32_t cntOp);
  typedef err_t (FOG_CDECL *TransformD_MapPathD)(const TransformD& self, PathD& dst, const PathD& src, uint32_t cntOp);

  typedef err_t (FOG_CDECL *TransformF_MapPathDataF)(const TransformF& self, PathF& dst, const uint8_t* srcCmd, const PointF* srcPts, size_t length, uint32_t cntOp);
  typedef err_t (FOG_CDECL *TransformD_MapPathDataF)(const TransformD& self, PathD& dst, const uint8_t* srcCmd, const PointF* srcPts, size_t length, uint32_t cntOp);
  typedef err_t (FOG_CDECL *TransformD_MapPathDataD)(const TransformD& self, PathD& dst, const uint8_t* srcCmd, const PointD* srcPts, size_t length, uint32_t cntOp);

  typedef void (FOG_CDECL *TransformF_MapBoxF)(const TransformF& self, BoxF& dst, const BoxF& src);
  typedef void (FOG_CDECL *TransformD_MapBoxD)(const TransformD& self, BoxD& dst, const BoxD& src);

  typedef void (FOG_CDECL *TransformF_MapVectorF)(const TransformF& self, PointF& dst, const PointF& src);
  typedef void (FOG_CDECL *TransformD_MapVectorD)(const TransformD& self, PointD& dst, const PointD& src);

  typedef PointF (FOG_CDECL *TransformF_GetScaling)(const TransformF& self, bool absolute);
  typedef PointD (FOG_CDECL *TransformD_GetScaling)(const TransformD& self, bool absolute);

  typedef float (FOG_CDECL *TransformF_GetRotation)(const TransformF& self);
  typedef double (FOG_CDECL *TransformD_GetRotation)(const TransformD& self);

  typedef float (FOG_CDECL *TransformF_GetAverageScaling)(const TransformF& self);
  typedef double (FOG_CDECL *TransformD_GetAverageScaling)(const TransformD& self);

  struct FOG_NO_EXPORT _FuncsTransformF
  {
    TransformF_Create create;
    TransformF_Update update;
    TransformF_Transform transform;
    TransformF_Transformed transformed;
    TransformF_Multiply multiply;
    TransformF_Invert invert;
    TransformF_MapPointF mapPointF;
    TransformF_MapPointsF mapPointsF[TRANSFORM_TYPE_COUNT];
    TransformF_MapPathF mapPathF;
    TransformF_MapPathDataF mapPathDataF;
    TransformF_MapBoxF mapBoxF;
    TransformF_MapVectorF mapVectorF;
    TransformF_GetScaling getScaling;
    TransformF_GetRotation getRotation;
    TransformF_GetAverageScaling getAverageScaling;
  } transformf;

  struct FOG_NO_EXPORT _FuncsTransformD
  {
    TransformD_Create create;
    TransformD_Update update;
    TransformD_Transform transform;
    TransformD_Transformed transformed;
    TransformD_Multiply multiply;
    TransformD_Invert invert;
    TransformD_MapPointD mapPointD;
    TransformD_MapPointsF mapPointsF[TRANSFORM_TYPE_COUNT];
    TransformD_MapPointsD mapPointsD[TRANSFORM_TYPE_COUNT];
    TransformD_MapPathF mapPathF;
    TransformD_MapPathD mapPathD;
    TransformD_MapPathDataF mapPathDataF;
    TransformD_MapPathDataD mapPathDataD;
    TransformD_MapBoxD mapBoxD;
    TransformD_MapVectorD mapVectorD;
    TransformD_GetScaling getScaling;
    TransformD_GetRotation getRotation;
    TransformD_GetAverageScaling getAverageScaling;
  } transformd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Triangle]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *TriangleF_GetBoundingBox)(const PointF* self, BoxF* dst, const TransformF* tr);
  typedef err_t (FOG_CDECL *TriangleD_GetBoundingBox)(const PointD* self, BoxD* dst, const TransformD* tr);

  typedef bool (FOG_CDECL *TriangleF_HitTest)(const PointF* self, const PointF* pt);
  typedef bool (FOG_CDECL *TriangleD_HitTest)(const PointD* self, const PointD* pt);

  struct FOG_NO_EXPORT _FuncsTriangleF
  {
    TriangleF_GetBoundingBox getBoundingBox;
    TriangleF_HitTest hitTest;
  } trianglef;

  struct FOG_NO_EXPORT _FuncsTriangleD
  {
    TriangleD_GetBoundingBox getBoundingBox;
    TriangleD_HitTest hitTest;
  } triangled;

  // --------------------------------------------------------------------------
  // [G2d/Imaging - ImageFormatDescription]
  // --------------------------------------------------------------------------

  typedef uint32_t (FOG_CDECL *ImageFormatDescription_GetCompatibleFormat)(
    const ImageFormatDescription* self);

  typedef err_t (FOG_CDECL *ImageFormatDescription_CreateArgb)(
    ImageFormatDescription* self,
    uint32_t depth, uint32_t flags,
    uint64_t aMask, uint64_t rMask, uint64_t gMask, uint64_t bMask);

  struct FOG_NO_EXPORT _FuncsImageFormatDescription
  {
    ImageFormatDescription_GetCompatibleFormat getCompatibleFormat;
    ImageFormatDescription_CreateArgb createArgb;
  } imageformatdescription;

  // --------------------------------------------------------------------------
  // [G2d/Painting - Painter]
  // --------------------------------------------------------------------------

  typedef PaintEngine* (FOG_CDECL *Painter_GetNullEngine)();

  typedef err_t (FOG_CDECL *Painter_BeginImage)(Painter* self, Image& image, const RectI* rect, uint32_t initFlags);
  typedef err_t (FOG_CDECL *Painter_BeginIBits)(Painter* self, const ImageBits& imageBits, const RectI* rect, uint32_t initFlags);

  typedef err_t (FOG_CDECL *Painter_SwitchToImage)(Painter* self, Image& image, const RectI* rect);
  typedef err_t (FOG_CDECL *Painter_SwitchToIBits)(Painter* self, const ImageBits& imageBits, const RectI* rect);

  struct FOG_NO_EXPORT _FuncsPainter
  {
    Painter_GetNullEngine getNullEngine;
    Painter_BeginImage beginImage;
    Painter_BeginIBits beginIBits;
    Painter_SwitchToImage switchToImage;
    Painter_SwitchToIBits switchToIBits;
  } painter;

  // --------------------------------------------------------------------------
  // [G2d/Source - Color]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Color_Convert)(void* dst, const void* src);

  typedef err_t (FOG_CDECL *Color_SetModel)(Color& self, uint32_t model);
  typedef err_t (FOG_CDECL *Color_SetData)(Color& self, uint32_t modelExtended, const void* modelData);
  typedef err_t (FOG_CDECL *Color_Mix)(Color& self, uint32_t mixOp, uint32_t alphaOp, const Color& secondary, float mask);
  typedef err_t (FOG_CDECL *Color_Adjust)(Color& self, uint32_t adjustOp, float param);

  typedef err_t (FOG_CDECL *Color_ParseA)(Color& self, const Stub8& str, uint32_t flags);
  typedef err_t (FOG_CDECL *Color_ParseU)(Color& self, const Utf16& str, uint32_t flags);

  struct FOG_NO_EXPORT _FuncsColor
  {
    Color_Convert convert[_COLOR_MODEL_COUNT][_COLOR_MODEL_COUNT];

    Color_SetModel setModel;
    Color_SetData setData;

    Color_Mix mix;
    Color_Adjust adjust;

    Color_ParseA parseA;
    Color_ParseU parseU;
  } color;
};

extern FOG_API Api _api;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_API_H
