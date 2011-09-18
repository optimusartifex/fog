// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_API_H
#define _FOG_CORE_GLOBAL_API_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

#include <Fog/Core/Global/EnumCore.h>
#include <Fog/Core/Global/EnumG2d.h>
#include <Fog/Core/Global/TypeDefs.h>
#include <Fog/Core/Global/TypeInfo.h>

// ============================================================================
// [Fog::Api]
// ============================================================================

namespace Fog {

//! @internal
//!
//! @brief Fog-Framework C-API.
struct FOG_NO_EXPORT Api
{
  // --------------------------------------------------------------------------
  // [Core/IO - DirEntry]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *DirEntry_Ctor)(DirEntry* self);
  typedef void (FOG_CDECL *DirEntry_CtorCopy)(DirEntry* self, const DirEntry* other);
  typedef void (FOG_CDECL *DirEntry_Dtor)(DirEntry* self);

  typedef void (FOG_CDECL *DirEntry_SetDirEntry)(DirEntry* self, const DirEntry* other);

  struct FOG_NO_EXPORT _Api_DirEntry
  {
    DirEntry_Ctor ctor;
    DirEntry_CtorCopy ctorCopy;
    DirEntry_Dtor dtor;
    DirEntry_SetDirEntry setDirEntry;
  } direntry;

  // --------------------------------------------------------------------------
  // [Core/IO - DirIterator]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *DirIterator_Ctor)(DirIterator* self);
  typedef void (FOG_CDECL *DirIterator_CtorString)(DirIterator* self, const StringW* path);
  typedef void (FOG_CDECL *DirIterator_Dtor)(DirIterator* self);

  typedef err_t (FOG_CDECL *DirIterator_Open)(DirIterator* self, const StringW* path);
  typedef void (FOG_CDECL *DirIterator_Close)(DirIterator* self);

  typedef bool (FOG_CDECL *DirIterator_ReadDirEntry)(DirIterator* self, DirEntry* dirEntry);
  typedef bool (FOG_CDECL *DirIterator_ReadString)(DirIterator* self, StringW* fileName);

  typedef err_t (FOG_CDECL *DirIterator_Rewind)(DirIterator* self);
  typedef int64_t (FOG_CDECL *DirIterator_Tell)(DirIterator* self);

  struct FOG_NO_EXPORT _Api_DirIterator
  {
    DirIterator_Ctor ctor;
    DirIterator_CtorString ctorString;
    DirIterator_Dtor dtor;
    DirIterator_Open open;
    DirIterator_Close close;
    DirIterator_ReadDirEntry readDirEntry;
    DirIterator_ReadString readString;
    DirIterator_Rewind rewind;
    DirIterator_Tell tell;
  } diriterator;

  // --------------------------------------------------------------------------
  // [Core/Math - Math]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL* MathF_Integrate)(float* dst, const FunctionF* func, const IntervalF* interval, uint32_t steps);
  typedef err_t (FOG_CDECL* MathD_Integrate)(double* dst, const FunctionD* func, const IntervalD* interval, uint32_t steps);

  typedef int (FOG_CDECL* MathF_Solve)(float* dst, const float* func);
  typedef int (FOG_CDECL* MathD_Solve)(double* dst, const double* func);

  typedef int (FOG_CDECL* MathF_SolveAt)(float* dst, const float* func, const IntervalF* interval);
  typedef int (FOG_CDECL* MathD_SolveAt)(double* dst, const double* func, const IntervalD* interval);

  typedef void (FOG_CDECL* MathF_VecFloatFromDouble)(float* dst, const double* src, size_t length);
  typedef void (FOG_CDECL* MathD_VecDoubleFromFloat)(double* dst, const float* src, size_t length);

  struct FOG_NO_EXPORT _Api_MathF
  {
    MathF_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];

    MathF_Solve solve[MATH_SOLVE_COUNT];
    MathF_SolveAt solveAt[MATH_SOLVE_COUNT];

    MathF_VecFloatFromDouble vecFloatFromDouble;
  } mathf;

  struct FOG_NO_EXPORT _Api_MathD
  {
    MathD_Integrate integrate[MATH_INTEGRATION_METHOD_COUNT];

    MathD_Solve solve[MATH_SOLVE_COUNT];
    MathD_SolveAt solveAt[MATH_SOLVE_COUNT];

    MathD_VecDoubleFromFloat vecDoubleFromFloat;
  } mathd;

  // --------------------------------------------------------------------------
  // [Core/Memory - MemMgr]
  // --------------------------------------------------------------------------

  typedef void* (FOG_CDECL *MemMgr_Alloc)(size_t size);
  typedef void* (FOG_CDECL *MemMgr_Calloc)(size_t size);
  typedef void* (FOG_CDECL *MemMgr_Realloc)(void* ptr, size_t size);
  typedef void (FOG_CDECL *MemMgr_Free)(void* ptr);

  typedef void (FOG_CDECL *MemMgr_Cleanup)(void);
  typedef err_t (FOG_CDECL *MemMgr_RegisterCleanupFunc)(MemCleanupFunc func, void* closure);
  typedef err_t (FOG_CDECL *MemMgr_UnregisterCleanupFunc)(MemCleanupFunc func, void* closure);

  struct FOG_NO_EXPORT _Api_Memory
  {
    // These functions are prefixed by _m_, because some memory leak detectors
    // use macros to replace malloc(), realloc(), free(), etc... This prevents
    // to be messed with them.
    MemMgr_Alloc _m_alloc;
    MemMgr_Calloc _m_calloc;
    MemMgr_Realloc _m_realloc;
    MemMgr_Free _m_free;

    MemMgr_Cleanup cleanup;
    MemMgr_RegisterCleanupFunc registerCleanupFunc;
    MemMgr_UnregisterCleanupFunc unregisterCleanupFunc;
  } memmgr;

  // --------------------------------------------------------------------------
  // [Core/Memory - MemOps]
  // --------------------------------------------------------------------------

  typedef void* (FOG_CDECL *MemOps_Copy)(void* dst, const void* src, size_t size);
  typedef void* (FOG_CDECL *MemOps_Move)(void* dst, const void* src, size_t size);
  typedef void* (FOG_CDECL *MemOps_Zero)(void* dst, size_t size);
  typedef void* (FOG_CDECL *MemOps_Set)(void* dst, uint val, size_t size);

  typedef void (FOG_CDECL *MemOps_Xchg)(void* a, void* b, size_t size);
  typedef void* (FOG_CDECL *MemOps_Eq)(const void* a, const void* b, size_t size);

  struct FOG_NO_EXPORT _Api_MemOps
  {
    MemOps_Copy copy;
    MemOps_Move move;
    MemOps_Zero zero;
    MemOps_Set set;

    MemOps_Copy copy_nt;
    MemOps_Zero zero_nt;
    MemOps_Set set_nt;

    MemOps_Xchg xchg;
    MemOps_Eq eq;
  } memops;

  // --------------------------------------------------------------------------
  // [Core/Memory - MemPool]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *MemPool_Ctor)(MemPool* self);
  typedef void (FOG_CDECL *MemPool_Dtor)(MemPool* self);

  typedef size_t (FOG_CDECL *MemPool_GetCapacity)(const MemPool* self);

  typedef void (FOG_CDECL *MemPool_Reset)(MemPool* self);
  typedef void* (FOG_CDECL *MemPool_Save)(MemPool* self);

  typedef err_t (FOG_CDECL *MemPool_Prealloc)(MemPool* self, size_t szItem, size_t count);
  typedef void* (FOG_CDECL *MemPool_Alloc)(MemPool* self, size_t szItem);

  typedef void (FOG_CDECL *MemPool_FreeSaved)(void* ptr);

  struct FOG_NO_EXPORT _Api_MemPool
  {
    MemPool_Ctor ctor;
    MemPool_Dtor dtor;

    MemPool_GetCapacity getCapacity;

    MemPool_Reset reset;
    MemPool_Save save;

    MemPool_Prealloc prealloc;
    MemPool_Alloc alloc;

    MemPool_FreeSaved freeSaved;
  } mempool;

  // --------------------------------------------------------------------------
  // [Core/OS - Library]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL* Library_Ctor)(Library* self);
  typedef void (FOG_CDECL* Library_CtorCopy)(Library* self, const Library* other);
  typedef void (FOG_CDECL* Library_Dtor)(Library* self);

  typedef void (FOG_CDECL* Library_SetLibrary)(Library* self, const Library* other);
  typedef err_t (FOG_CDECL* Library_OpenLibrary)(Library* self, const StringW* fileName, uint32_t flags);
  typedef err_t (FOG_CDECL* Library_OpenPlugin)(Library* self, const StringW* category, const StringW* name);
  typedef void (FOG_CDECL* Library_Close)(Library* self);

  typedef void* (FOG_CDECL* Library_GetSymbolStubA)(const Library* self, const StubA* sym);
  typedef void* (FOG_CDECL* Library_GetSymbolStringW)(const Library* self, const StringW* sym);
  typedef size_t (FOG_CDECL* Library_GetSymbols)(const Library* self, void** target, const char* symbolsData, size_t symbolsLength, size_t symbolsCount, char** fail);

  typedef LibraryData* (FOG_CDECL* Library_DCreate)(void* handle);
  typedef void (FOG_CDECL* Library_DFree)(LibraryData* d);

  typedef err_t (FOG_CDECL* Library_GetSystemPrefix)(StringW* dst);
  typedef err_t (FOG_CDECL* Library_GetSystemExtensions)(List<StringW>* dst);

  typedef err_t (FOG_CDECL* Library_GetLibraryPaths)(List<StringW>* dst);
  typedef err_t (FOG_CDECL* Library_AddLibraryPath)(const StringW* path, uint32_t mode);
  typedef err_t (FOG_CDECL* Library_RemoveLibraryPath)(const StringW* path);
  typedef bool (FOG_CDECL* Library_HasLibraryPath)(const StringW* path);

  struct FOG_NO_EXPORT _Api_Library
  {
    Library_Ctor ctor;
    Library_CtorCopy ctorCopy;
    Library_Ctor dtor;
    Library_SetLibrary setLibrary;
    Library_OpenLibrary openLibrary;
    Library_OpenPlugin openPlugin;
    Library_Close close;
    Library_GetSymbolStubA getSymbolStubA;
    Library_GetSymbolStringW getSymbolStringW;
    Library_GetSymbols getSymbols;

    Library_DCreate dCreate;
    Library_DFree dFree;

    Library_GetSystemPrefix getSystemPrefix;
    Library_GetSystemExtensions getSystemExtensions;
    Library_GetLibraryPaths getLibraryPaths;
    Library_AddLibraryPath addLibraryPath;
    Library_RemoveLibraryPath removeLibraryPath;
    Library_HasLibraryPath hasLibraryPath;

    LibraryData* dNone;
  } library;

  // --------------------------------------------------------------------------
  // [Core/OS - System]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *System_GetSystemName)(StringW* dst);
  typedef err_t (FOG_CDECL *System_GetSystemVersion)(StringW* dst);

#if defined(FOG_OS_WINDOWS)
  typedef uint32_t (FOG_CDECL *System_GetWindowsVersion)(void);
  typedef err_t (FOG_CDECL *System_GetWindowsDirectory)(StringW* dst);
#endif // FOG_OS_WINDOWS

  typedef uint64_t (FOG_CDECL *System_GetAmountOfPhysicalMemory)(void);
  typedef uint32_t (FOG_CDECL *System_GetAmountOfPhysicalMemoryMB)(void);

  typedef err_t (FOG_CDECL* System_GetEnvironmentStubA)(const StubA* key, StringW* value);
  typedef err_t (FOG_CDECL* System_GetEnvironmentStringW)(const StringW* key, StringW* value);

  typedef err_t (FOG_CDECL* System_SetEnvironmentStubA)(const StubA* key, const StringW* value);
  typedef err_t (FOG_CDECL* System_SetEnvironmentStringW)(const StringW* key, const StringW* value);

  typedef err_t (FOG_CDECL *System_ErrorFromOSErrorCode)(int code);
  typedef err_t (FOG_CDECL *System_ErrorFromOSLastError)(void);

#if defined(FOG_OS_WINDOWS)
  typedef err_t (FOG_CDECL *System_GetModuleFileName)(StringW* dst, HMODULE hModule);
  typedef err_t (FOG_CDECL *System_MakeWindowsPath)(StringW* dst, const StringW* src);
#endif // FOG_OS_WINDOWS

  struct FOG_NO_EXPORT _Api_System
  {
    System_GetSystemName getSystemName;
    System_GetSystemVersion getSystemVersion;

#if defined(FOG_OS_WINDOWS)
    System_GetWindowsVersion getWindowsVersion;
    System_GetWindowsDirectory getWindowsDirectory;
#endif // FOG_OS_WINDOWS

    System_GetAmountOfPhysicalMemory getAmountOfPhysicalMemory;
    System_GetAmountOfPhysicalMemoryMB getAmountOfPhysicalMemoryMB;

    System_GetEnvironmentStubA getEnvironmentStubA;
    System_GetEnvironmentStringW getEnvironmentStringW;

    System_SetEnvironmentStubA setEnvironmentStubA;
    System_SetEnvironmentStringW setEnvironmentStringW;

    System_ErrorFromOSErrorCode errorFromOSErrorCode;
    System_ErrorFromOSLastError errorFromOSLastError;

#if defined(FOG_OS_WINDOWS)
    System_GetModuleFileName getModuleFileName;
    System_MakeWindowsPath makeWindowsPath;
#endif // FOG_OS_WINDOWS
  } system;

  // --------------------------------------------------------------------------
  // [Core/Threading - ThreadLocal]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL* ThreadLocal_Create)(uint32_t* slot, void* dtor);
  typedef err_t (FOG_CDECL* ThreadLocal_Destroy)(uint32_t slot);
  typedef void* (FOG_CDECL* ThreadLocal_Get)(uint32_t slot);
  typedef err_t (FOG_CDECL* ThreadLocal_Set)(uint32_t slot, void* val);

  struct FOG_NO_EXPORT _Api_ThreadLocal
  {
    ThreadLocal_Create create;
    ThreadLocal_Destroy destroy;
    ThreadLocal_Get get;
    ThreadLocal_Set set;
  } threadlocal;

  // --------------------------------------------------------------------------
  // [Core/Tools - Cpu]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _Api_Cpu
  {
    Cpu* instance;
  } cpu;

  // --------------------------------------------------------------------------
  // [Core/Tools - Date]
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

  typedef bool (FOG_CDECL* Date_Eq)(const Date* a, const Date* b);
  typedef int (FOG_CDECL* Date_Compare)(const Date* a, const Date* b);

  struct FOG_NO_EXPORT _Api_Date
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

    Date_Eq eq;
    Date_Compare compare;
  } date;

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash - Helpers]
  // --------------------------------------------------------------------------

  typedef size_t (FOG_CDECL *HashHelper_CalcExpandCapacity)(size_t capacity);
  typedef size_t (FOG_CDECL *HashHelper_CalcShrinkCapacity)(size_t capacity);

  struct FOG_NO_EXPORT _Api_Hash_Helper
  {
    HashHelper_CalcExpandCapacity calcExpandCapacity;
    HashHelper_CalcShrinkCapacity calcShrinkCapacity;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<Unknown, Unknown>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Hash_Unknown_Unknown_Ctor)(HashUntyped* self);
  typedef void (FOG_CDECL *Hash_Unknown_Unknown_CtorCopy)(HashUntyped* self, const HashUntyped* other);
  typedef void (FOG_CDECL *Hash_Unknown_Unknown_Dtor)(HashUntyped* self, const HashUntypedVTable* v);

  typedef err_t (FOG_CDECL *Hash_Unknown_Unknown_Detach)(HashUntyped* self, const HashUntypedVTable* v);
  typedef err_t (FOG_CDECL *Hash_Unknown_Unknown_Rehash)(HashUntyped* self, const HashUntypedVTable* v, size_t capacity);
  typedef err_t (FOG_CDECL *Hash_Unknown_Unknown_Reserve)(HashUntyped* self, const HashUntypedVTable* v, size_t capacity);
  typedef void (FOG_CDECL *Hash_Unknown_Unknown_Squeeze)(HashUntyped* self, const HashUntypedVTable* v);

  typedef void (FOG_CDECL *Hash_Unknown_Unknown_Clear)(HashUntyped* self, const HashUntypedVTable* v);
  typedef void (FOG_CDECL *Hash_Unknown_Unknown_Reset)(HashUntyped* self, const HashUntypedVTable* v);

  typedef const void* (FOG_CDECL *Hash_Unknown_Unknown_Get)(const HashUntyped* self, const HashUntypedVTable* v, const void* key);
  typedef void* (FOG_CDECL *Hash_Unknown_Unknown_Use)(HashUntyped* self, const HashUntypedVTable* v, const void* key);
  typedef err_t (FOG_CDECL *Hash_Unknown_Unknown_Put)(HashUntyped* self, const HashUntypedVTable* v, const void* key, const void* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_Unknown_Unknown_Remove)(HashUntyped* self, const HashUntypedVTable* v, const void* key);

  typedef void (FOG_CDECL *Hash_Unknown_Unknown_Copy)(HashUntyped* self, const HashUntypedVTable* v, const HashUntyped* other);
  typedef bool (FOG_CDECL *Hash_Unknown_Unknown_Eq)(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc);

  typedef HashUntypedData* (FOG_CDECL *Hash_Unknown_Unknown_DCreate)(size_t capacity);
  typedef void (FOG_CDECL *Hash_Unknown_Unknown_DFree)(HashUntypedData* d, const HashUntypedVTable* v);

  struct FOG_NO_EXPORT _Api_Hash_Unknown_Unknown
  {
    Hash_Unknown_Unknown_Ctor ctor;
    Hash_Unknown_Unknown_CtorCopy ctorCopy;
    Hash_Unknown_Unknown_Dtor dtor;

    Hash_Unknown_Unknown_Rehash rehash;
    Hash_Unknown_Unknown_Detach detach;
    Hash_Unknown_Unknown_Reserve reserve;
    Hash_Unknown_Unknown_Squeeze squeeze;

    Hash_Unknown_Unknown_Clear clear;
    Hash_Unknown_Unknown_Reset reset;

    Hash_Unknown_Unknown_Get get;
    Hash_Unknown_Unknown_Use use;
    Hash_Unknown_Unknown_Put put;
    Hash_Unknown_Unknown_Remove remove;

    Hash_Unknown_Unknown_Copy copy;
    Hash_Unknown_Unknown_Eq eq;

    Hash_Unknown_Unknown_DCreate dCreate;
    Hash_Unknown_Unknown_DFree dFree;

    HashUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<Int32, Unknown>]
  // --------------------------------------------------------------------------

  typedef const void* (FOG_CDECL *Hash_Int32_Unknown_Get)(const HashUntyped* self, const HashUntypedVTable* v, int32_t key);
  typedef void* (FOG_CDECL *Hash_Int32_Unknown_Use)(HashUntyped* self, const HashUntypedVTable* v, int32_t key);
  typedef err_t (FOG_CDECL *Hash_Int32_Unknown_Put)(HashUntyped* self, const HashUntypedVTable* v, int32_t key, const void* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_Int32_Unknown_Remove)(HashUntyped* self, const HashUntypedVTable* v, int32_t key);

  struct FOG_NO_EXPORT _Api_Hash_Int32_Unknown
  {
    Hash_Int32_Unknown_Get get;
    Hash_Int32_Unknown_Use use;
    Hash_Int32_Unknown_Put put;
    Hash_Int32_Unknown_Remove remove;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<Int64, Unknown>]
  // --------------------------------------------------------------------------

  typedef const void* (FOG_CDECL *Hash_Int64_Unknown_Get)(const HashUntyped* self, const HashUntypedVTable* v, int64_t key);
  typedef void* (FOG_CDECL *Hash_Int64_Unknown_Use)(HashUntyped* self, const HashUntypedVTable* v, int64_t key);
  typedef err_t (FOG_CDECL *Hash_Int64_Unknown_Put)(HashUntyped* self, const HashUntypedVTable* v, int64_t key, const void* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_Int64_Unknown_Remove)(HashUntyped* self, const HashUntypedVTable* v, int64_t key);

  struct FOG_NO_EXPORT _Api_Hash_Int64_Unknown
  {
    Hash_Int64_Unknown_Get get;
    Hash_Int64_Unknown_Use use;
    Hash_Int64_Unknown_Put put;
    Hash_Int64_Unknown_Remove remove;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringA, Unknown>]
  // --------------------------------------------------------------------------

  typedef const void* (FOG_CDECL *Hash_StringA_Unknown_GetStubA)(const HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  typedef const void* (FOG_CDECL *Hash_StringA_Unknown_GetStringA)(const HashUntyped* self, const HashUntypedVTable* v, const StringA* key);

  typedef void* (FOG_CDECL *Hash_StringA_Unknown_UseStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  typedef void* (FOG_CDECL *Hash_StringA_Unknown_UseStringA)(HashUntyped* self, const HashUntypedVTable* v, const StringA* key);
  
  typedef err_t (FOG_CDECL *Hash_StringA_Unknown_PutStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key, const void* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringA_Unknown_PutStringA)(HashUntyped* self, const HashUntypedVTable* v, const StringA* key, const void* item, bool replace);
  
  typedef err_t (FOG_CDECL *Hash_StringA_Unknown_RemoveStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  typedef err_t (FOG_CDECL *Hash_StringA_Unknown_RemoveStringA)(HashUntyped* self, const HashUntypedVTable* v, const StringA* key);

  typedef bool (FOG_CDECL *Hash_StringA_Unknown_Eq)(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc);

  struct FOG_NO_EXPORT _Api_Hash_StringA_Unknown
  {
    Hash_StringA_Unknown_GetStubA getStubA;
    Hash_StringA_Unknown_GetStringA getStringA;
    
    Hash_StringA_Unknown_UseStubA useStubA;
    Hash_StringA_Unknown_UseStringA useStringA;
    
    Hash_StringA_Unknown_PutStubA putStubA;
    Hash_StringA_Unknown_PutStringA putStringA;
    
    Hash_StringA_Unknown_RemoveStubA removeStubA;
    Hash_StringA_Unknown_RemoveStringA removeStringA;

    Hash_StringA_Unknown_Eq eq;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringA, StringA>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Hash_StringA_StringA_Ctor)(HashUntyped* self);
  typedef void (FOG_CDECL *Hash_StringA_StringA_Dtor)(HashUntyped* self);

  typedef const StringA* (FOG_CDECL *Hash_StringA_StringA_GetStubA)(const HashUntyped* self, const StubA* key);
  typedef const StringA* (FOG_CDECL *Hash_StringA_StringA_GetStringA)(const HashUntyped* self, const StringA* key);

  typedef StringA* (FOG_CDECL *Hash_StringA_StringA_UseStubA)(HashUntyped* self, const StubA* key);
  typedef StringA* (FOG_CDECL *Hash_StringA_StringA_UseStringA)(HashUntyped* self, const StringA* key);

  typedef err_t (FOG_CDECL *Hash_StringA_StringA_PutStubA)(HashUntyped* self, const StubA* key, const StringA* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringA_StringA_PutStringA)(HashUntyped* self, const StringA* key, const StringA* item, bool replace);

  typedef err_t (FOG_CDECL *Hash_StringA_StringA_RemoveStubA)(HashUntyped* self, const StubA* key);
  typedef err_t (FOG_CDECL *Hash_StringA_StringA_RemoveStringA)(HashUntyped* self, const StringA* key);

  typedef void (FOG_CDECL *Hash_StringA_StringA_DFree)(HashUntypedData* d);

  struct FOG_NO_EXPORT _Api_Hash_StringA_StringA
  {
    Hash_StringA_StringA_Ctor ctor;
    Hash_StringA_StringA_Dtor dtor;

    Hash_StringA_StringA_GetStubA getStubA;
    Hash_StringA_StringA_GetStringA getStringA;
    
    Hash_StringA_StringA_UseStubA useStubA;
    Hash_StringA_StringA_UseStringA useStringA;
    
    Hash_StringA_StringA_PutStubA putStubA;
    Hash_StringA_StringA_PutStringA putStringA;
    
    Hash_StringA_StringA_RemoveStubA removeStubA;
    Hash_StringA_StringA_RemoveStringA removeStringA;

    Hash_StringA_StringA_DFree dFree;

    HashUntypedVTable* vTable;
    HashUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringA, Var>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Hash_StringA_Var_Ctor)(HashUntyped* self);
  typedef void (FOG_CDECL *Hash_StringA_Var_Dtor)(HashUntyped* self);

  typedef const Var* (FOG_CDECL *Hash_StringA_Var_GetStubA)(const HashUntyped* self, const StubA* key);
  typedef const Var* (FOG_CDECL *Hash_StringA_Var_GetStringA)(const HashUntyped* self, const StringA* key);

  typedef Var* (FOG_CDECL *Hash_StringA_Var_UseStubA)(HashUntyped* self, const StubA* key);
  typedef Var* (FOG_CDECL *Hash_StringA_Var_UseStringA)(HashUntyped* self, const StringA* key);

  typedef err_t (FOG_CDECL *Hash_StringA_Var_PutStubA)(HashUntyped* self, const StubA* key, const Var* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringA_Var_PutStringA)(HashUntyped* self, const StringA* key, const Var* item, bool replace);

  typedef err_t (FOG_CDECL *Hash_StringA_Var_RemoveStubA)(HashUntyped* self, const StubA* key);
  typedef err_t (FOG_CDECL *Hash_StringA_Var_RemoveStringA)(HashUntyped* self, const StringA* key);

  typedef void (FOG_CDECL *Hash_StringA_Var_DFree)(HashUntypedData* d);

  struct FOG_NO_EXPORT _Api_Hash_StringA_Var
  {
    Hash_StringA_Var_Ctor ctor;
    Hash_StringA_Var_Ctor dtor;

    Hash_StringA_Var_GetStubA getStubA;
    Hash_StringA_Var_GetStringA getStringA;
    
    Hash_StringA_Var_UseStubA useStubA;
    Hash_StringA_Var_UseStringA useStringA;
    
    Hash_StringA_Var_PutStubA putStubA;
    Hash_StringA_Var_PutStringA putStringA;
    
    Hash_StringA_Var_RemoveStubA removeStubA;
    Hash_StringA_Var_RemoveStringA removeStringA;

    Hash_StringA_Var_DFree dFree;

    HashUntypedVTable* vTable;
    HashUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringW, Unknown>]
  // --------------------------------------------------------------------------

  typedef const void* (FOG_CDECL *Hash_StringW_Unknown_GetStubA)(const HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  typedef const void* (FOG_CDECL *Hash_StringW_Unknown_GetStubW)(const HashUntyped* self, const HashUntypedVTable* v, const StubW* key);
  typedef const void* (FOG_CDECL *Hash_StringW_Unknown_GetStringW)(const HashUntyped* self, const HashUntypedVTable* v, const StringW* key);

  typedef void* (FOG_CDECL *Hash_StringW_Unknown_UseStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  typedef void* (FOG_CDECL *Hash_StringW_Unknown_UseStubW)(HashUntyped* self, const HashUntypedVTable* v, const StubW* key);
  typedef void* (FOG_CDECL *Hash_StringW_Unknown_UseStringW)(HashUntyped* self, const HashUntypedVTable* v, const StringW* key);
  
  typedef err_t (FOG_CDECL *Hash_StringW_Unknown_PutStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key, const void* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringW_Unknown_PutStubW)(HashUntyped* self, const HashUntypedVTable* v, const StubW* key, const void* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringW_Unknown_PutStringW)(HashUntyped* self, const HashUntypedVTable* v, const StringW* key, const void* item, bool replace);
  
  typedef err_t (FOG_CDECL *Hash_StringW_Unknown_RemoveStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  typedef err_t (FOG_CDECL *Hash_StringW_Unknown_RemoveStubW)(HashUntyped* self, const HashUntypedVTable* v, const StubW* key);
  typedef err_t (FOG_CDECL *Hash_StringW_Unknown_RemoveStringW)(HashUntyped* self, const HashUntypedVTable* v, const StringW* key);

  typedef bool (FOG_CDECL *Hash_StringW_Unknown_Eq)(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc);

  struct FOG_NO_EXPORT _Api_Hash_StringW_Unknown
  {
    Hash_StringW_Unknown_GetStubA getStubA;
    Hash_StringW_Unknown_GetStubW getStubW;
    Hash_StringW_Unknown_GetStringW getStringW;
    
    Hash_StringW_Unknown_UseStubA useStubA;
    Hash_StringW_Unknown_UseStubW useStubW;
    Hash_StringW_Unknown_UseStringW useStringW;
    
    Hash_StringW_Unknown_PutStubA putStubA;
    Hash_StringW_Unknown_PutStubW putStubW;
    Hash_StringW_Unknown_PutStringW putStringW;
    
    Hash_StringW_Unknown_RemoveStubA removeStubA;
    Hash_StringW_Unknown_RemoveStubW removeStubW;
    Hash_StringW_Unknown_RemoveStringW removeStringW;

    Hash_StringW_Unknown_Eq eq;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringW, StringW>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Hash_StringW_StringW_Ctor)(HashUntyped* self);
  typedef void (FOG_CDECL *Hash_StringW_StringW_Dtor)(HashUntyped* self);

  typedef const StringW* (FOG_CDECL *Hash_StringW_StringW_GetStubA)(const HashUntyped* self, const StubA* key);
  typedef const StringW* (FOG_CDECL *Hash_StringW_StringW_GetStubW)(const HashUntyped* self, const StubW* key);
  typedef const StringW* (FOG_CDECL *Hash_StringW_StringW_GetStringW)(const HashUntyped* self, const StringW* key);

  typedef StringW* (FOG_CDECL *Hash_StringW_StringW_UseStubA)(HashUntyped* self, const StubA* key);
  typedef StringW* (FOG_CDECL *Hash_StringW_StringW_UseStubW)(HashUntyped* self, const StubW* key);
  typedef StringW* (FOG_CDECL *Hash_StringW_StringW_UseStringW)(HashUntyped* self, const StringW* key);

  typedef err_t (FOG_CDECL *Hash_StringW_StringW_PutStubA)(HashUntyped* self, const StubA* key, const StringW* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringW_StringW_PutStubW)(HashUntyped* self, const StubW* key, const StringW* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringW_StringW_PutStringW)(HashUntyped* self, const StringW* key, const StringW* item, bool replace);

  typedef err_t (FOG_CDECL *Hash_StringW_StringW_RemoveStubA)(HashUntyped* self, const StubA* key);
  typedef err_t (FOG_CDECL *Hash_StringW_StringW_RemoveStubW)(HashUntyped* self, const StubW* key);
  typedef err_t (FOG_CDECL *Hash_StringW_StringW_RemoveStringW)(HashUntyped* self, const StringW* key);

  typedef void (FOG_CDECL *Hash_StringW_StringW_DFree)(HashUntypedData* d);

  struct FOG_NO_EXPORT _Api_Hash_StringW_StringW
  {
    Hash_StringW_StringW_Ctor ctor;
    Hash_StringW_StringW_Ctor dtor;

    Hash_StringW_StringW_GetStubA getStubA;
    Hash_StringW_StringW_GetStubW getStubW;
    Hash_StringW_StringW_GetStringW getStringW;
    
    Hash_StringW_StringW_UseStubA useStubA;
    Hash_StringW_StringW_UseStubW useStubW;
    Hash_StringW_StringW_UseStringW useStringW;
    
    Hash_StringW_StringW_PutStubA putStubA;
    Hash_StringW_StringW_PutStubW putStubW;
    Hash_StringW_StringW_PutStringW putStringW;
    
    Hash_StringW_StringW_RemoveStubA removeStubA;
    Hash_StringW_StringW_RemoveStubW removeStubW;
    Hash_StringW_StringW_RemoveStringW removeStringW;

    Hash_StringW_StringW_DFree dFree;

    HashUntypedVTable* vTable;
    HashUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringW, Var>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Hash_StringW_Var_Ctor)(HashUntyped* self);
  typedef void (FOG_CDECL *Hash_StringW_Var_Dtor)(HashUntyped* self);

  typedef const Var* (FOG_CDECL *Hash_StringW_Var_GetStubA)(const HashUntyped* self, const StubA* key);
  typedef const Var* (FOG_CDECL *Hash_StringW_Var_GetStubW)(const HashUntyped* self, const StubW* key);
  typedef const Var* (FOG_CDECL *Hash_StringW_Var_GetStringW)(const HashUntyped* self, const StringW* key);

  typedef Var* (FOG_CDECL *Hash_StringW_Var_UseStubA)(HashUntyped* self, const StubA* key);
  typedef Var* (FOG_CDECL *Hash_StringW_Var_UseStubW)(HashUntyped* self, const StubW* key);
  typedef Var* (FOG_CDECL *Hash_StringW_Var_UseStringW)(HashUntyped* self, const StringW* key);

  typedef err_t (FOG_CDECL *Hash_StringW_Var_PutStubA)(HashUntyped* self, const StubA* key, const Var* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringW_Var_PutStubW)(HashUntyped* self, const StubW* key, const Var* item, bool replace);
  typedef err_t (FOG_CDECL *Hash_StringW_Var_PutStringW)(HashUntyped* self, const StringW* key, const Var* item, bool replace);

  typedef err_t (FOG_CDECL *Hash_StringW_Var_RemoveStubA)(HashUntyped* self, const StubA* key);
  typedef err_t (FOG_CDECL *Hash_StringW_Var_RemoveStubW)(HashUntyped* self, const StubW* key);
  typedef err_t (FOG_CDECL *Hash_StringW_Var_RemoveStringW)(HashUntyped* self, const StringW* key);

  typedef void (FOG_CDECL *Hash_StringW_Var_DFree)(HashUntypedData* d);

  struct FOG_NO_EXPORT _Api_Hash_StringW_Var
  {
    Hash_StringW_Var_Ctor ctor;
    Hash_StringW_Var_Dtor dtor;

    Hash_StringW_Var_GetStubA getStubA;
    Hash_StringW_Var_GetStubW getStubW;
    Hash_StringW_Var_GetStringW getStringW;

    Hash_StringW_Var_UseStubA useStubA;
    Hash_StringW_Var_UseStubW useStubW;
    Hash_StringW_Var_UseStringW useStringW;

    Hash_StringW_Var_PutStubA putStubA;
    Hash_StringW_Var_PutStubW putStubW;
    Hash_StringW_Var_PutStringW putStringW;

    Hash_StringW_Var_RemoveStubA removeStubA;
    Hash_StringW_Var_RemoveStubW removeStubW;
    Hash_StringW_Var_RemoveStringW removeStringW;

    Hash_StringW_Var_DFree dFree;

    HashUntypedVTable* vTable;
    HashUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<...>]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _Api_Hash
  {
    _Api_Hash_Helper helper;
    _Api_Hash_Unknown_Unknown unknown_unknown;

    _Api_Hash_Int32_Unknown int32_unknown;
    _Api_Hash_Int64_Unknown int64_unknown;

    _Api_Hash_StringA_Unknown stringa_unknown;
    _Api_Hash_StringA_StringA stringa_stringa;
    _Api_Hash_StringA_Var stringa_var;

    _Api_Hash_StringW_Unknown stringw_unknown;
    _Api_Hash_StringW_StringW stringw_stringw;
    _Api_Hash_StringW_Var stringw_var;
  } hash;

  // --------------------------------------------------------------------------
  // [Core/Tools - HashIterator<...>]
  // --------------------------------------------------------------------------

  typedef bool (FOG_CDECL *HashIterator_Start)(HashUntypedIterator* i);
  typedef bool (FOG_CDECL *HashIterator_Next)(HashUntypedIterator* i);

  struct FOG_NO_EXPORT _Api_HashIterator
  {
    HashIterator_Start start;
    HashIterator_Next next;
  } hashiterator;

  // --------------------------------------------------------------------------
  // [Core/Tools - HashUtil]
  // --------------------------------------------------------------------------

  typedef uint32_t (FOG_CDECL *HashUtil_HashBinary)(const void* src, size_t length);
  typedef uint32_t (FOG_CDECL *HashUtil_HashVectorD)(const void* src, size_t length);
  typedef uint32_t (FOG_CDECL *HashUtil_HashVectorQ)(const void* src, size_t length);

  typedef uint32_t (FOG_CDECL *HashUtil_HashStubA)(const StubA* item);
  typedef uint32_t (FOG_CDECL *HashUtil_HashStubW)(const StubW* item);

  struct FOG_NO_EXPORT _Api_HashUtil
  {
    HashUtil_HashBinary binary;
    HashUtil_HashVectorD vectorD;
    HashUtil_HashVectorQ vectorQ;

    HashUtil_HashStubA stubA;
    HashUtil_HashStubW stubW;
  } hashutil;

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Untyped>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *List_Untyped_Ctor)(ListUntyped* self);
  typedef void (FOG_CDECL *List_Untyped_CtorCopy)(ListUntyped* self, const ListUntyped* other);

  typedef size_t (FOG_CDECL *List_Untyped_IndexOf)(const ListUntyped* self, const Range* range, const void* item);
  typedef size_t (FOG_CDECL *List_Untyped_LastIndexOf)(const ListUntyped* self, const Range* range, const void* item);

  typedef bool (FOG_CDECL *List_Untyped_BinaryEq)(const ListUntyped* a, const ListUntyped* b, size_t szItemT);
  typedef bool (FOG_CDECL *List_Untyped_CustomEq)(const ListUntyped* a, const ListUntyped* b, size_t szItemT, EqFunc eqFunc);

  typedef ListUntypedData* (FOG_CDECL *List_Untyped_DCreate)(size_t szItemT, size_t capacity);

  struct FOG_NO_EXPORT _Api_List_Untyped
  {
    List_Untyped_Ctor ctor;
    List_Untyped_CtorCopy ctorCopy;

    List_Untyped_IndexOf indexOf_4B;
    List_Untyped_IndexOf indexOf_8B;
    List_Untyped_IndexOf indexOf_16B;

    List_Untyped_LastIndexOf lastIndexOf_4B;
    List_Untyped_LastIndexOf lastIndexOf_8B;
    List_Untyped_LastIndexOf lastIndexOf_16B;

    List_Untyped_BinaryEq binaryEq;
    List_Untyped_CustomEq customEq;

    List_Untyped_DCreate dCreate;

    ListUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Simple>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *List_Simple_CtorSlice)(ListUntyped* self, size_t szItemT, const ListUntyped* other, const Range* range);
  typedef void (FOG_CDECL *List_Simple_Dtor)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_Simple_Detach)(ListUntyped* self, size_t szItemT);
  typedef err_t (FOG_CDECL *List_Simple_Reserve)(ListUntyped* self, size_t szItemT, size_t capacity);
  typedef void* (FOG_CDECL *List_Simple_Prepare)(ListUntyped* self, size_t szItemT, uint32_t cntOp, size_t length);
  typedef err_t (FOG_CDECL *List_Simple_GrowBoth)(ListUntyped* self, size_t szItemT, size_t left, size_t right);
  typedef err_t (FOG_CDECL *List_Simple_GrowSide)(ListUntyped* self, size_t szItemT, size_t length);
  typedef void (FOG_CDECL *List_Simple_Squeeze)(ListUntyped* self, size_t szItemT);

  typedef err_t (FOG_CDECL *List_Simple_SetAt)(ListUntyped* self, size_t szItemT, size_t index, const void* item);
  typedef void (FOG_CDECL *List_Simple_Clear)(ListUntyped* self);
  typedef void (FOG_CDECL *List_Simple_Reset)(ListUntyped* self);
  typedef err_t (FOG_CDECL *List_Simple_OpList)(ListUntyped* self, size_t szItemT, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_Simple_OpData)(ListUntyped* self, size_t szItemT, uint32_t cntOp, const void* data, size_t dataLength);
  typedef err_t (FOG_CDECL *List_Simple_AppendItem)(ListUntyped* self, size_t szItemT, const void* item);
  typedef err_t (FOG_CDECL *List_Simple_InsertItem)(ListUntyped* self, size_t szItemT, size_t index, const void* item);
  typedef err_t (FOG_CDECL *List_Simple_Remove)(ListUntyped* self, size_t szItemT, const Range* range);
  typedef err_t (FOG_CDECL *List_Simple_Replace)(ListUntyped* self, size_t szItemT, const Range* range, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_Simple_Slice)(ListUntyped* self, size_t szItemT, const Range* range);

  typedef err_t (FOG_CDECL *List_Simple_Sort)(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareFunc compareFunc);
  typedef err_t (FOG_CDECL *List_Simple_SortEx)(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  typedef err_t (FOG_CDECL *List_Simple_SwapItems)(ListUntyped* self, size_t szItemT, size_t index1, size_t index2);

  typedef void (FOG_CDECL *List_Simple_Copy)(ListUntyped* self, const ListUntyped* other);

  typedef void (FOG_CDECL *List_Simple_DRelease)(ListUntypedData* d);
  typedef void (FOG_CDECL *List_Simple_DFree)(ListUntypedData* d);

  struct FOG_NO_EXPORT _Api_List_Simple
  {
    List_Simple_CtorSlice ctorSlice;
    List_Simple_Dtor dtor;

    List_Simple_Detach detach;
    List_Simple_Reserve reserve;
    List_Simple_Prepare prepare;
    List_Simple_GrowSide growLeft;
    List_Simple_GrowSide growRight;
    List_Simple_Squeeze squeeze;

    List_Simple_SetAt setAt;
    List_Simple_SetAt setAt_4x;

    List_Simple_Clear clear;
    List_Simple_Reset reset;

    List_Simple_OpList opList;
    List_Simple_OpData opData;

    List_Simple_AppendItem appendItem;
    List_Simple_AppendItem appendItem_4x;

    List_Simple_InsertItem insertItem;
    List_Simple_InsertItem insertItem_4x;

    List_Simple_Remove remove;
    List_Simple_Replace replace;
    List_Simple_Slice slice;

    List_Simple_Sort sort;
    List_Simple_SortEx sortEx;

    List_Simple_SwapItems swapItems;
    List_Simple_SwapItems swapItems_4x;

    List_Simple_Copy copy;

    List_Simple_DRelease dRelease;
    List_Simple_DFree dFree;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Movable>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *List_Unknown_CtorSlice)(ListUntyped* self, const ListUntypedVTable* v, const ListUntyped* other, const Range* range);
  typedef void (FOG_CDECL *List_Unknown_Dtor)(ListUntyped* self, const ListUntypedVTable* v);

  typedef err_t (FOG_CDECL *List_Unknown_Detach)(ListUntyped* self, const ListUntypedVTable* v);
  typedef err_t (FOG_CDECL *List_Unknown_Reserve)(ListUntyped* self, const ListUntypedVTable* v, size_t capacity);
  typedef void* (FOG_CDECL *List_Unknown_Prepare)(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, size_t length);
  typedef void* (FOG_CDECL *List_Unknown_PrepareAppendItem)(ListUntyped* self, const ListUntypedVTable* v);
  typedef void* (FOG_CDECL *List_Unknown_PrepareInsertItem)(ListUntyped* self, const ListUntypedVTable* v, size_t index);
  typedef err_t (FOG_CDECL *List_Unknown_GrowBoth)(ListUntyped* self, const ListUntypedVTable* v, size_t left, size_t right);
  typedef err_t (FOG_CDECL *List_Unknown_GrowSide)(ListUntyped* self, const ListUntypedVTable* v, size_t length);
  typedef void (FOG_CDECL *List_Unknown_Squeeze)(ListUntyped* self, const ListUntypedVTable* v);

  typedef void (FOG_CDECL *List_Unknown_Clear)(ListUntyped* self, const ListUntypedVTable* v);
  typedef void (FOG_CDECL *List_Unknown_Reset)(ListUntyped* self, const ListUntypedVTable* v);
  typedef err_t (FOG_CDECL *List_Unknown_OpList)(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_Unknown_OpData)(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, const void* data, size_t dataLength);
  typedef err_t (FOG_CDECL *List_Unknown_Remove)(ListUntyped* self, const ListUntypedVTable* v, const Range* range);
  typedef err_t (FOG_CDECL *List_Unknown_Replace)(ListUntyped* self, const ListUntypedVTable* v, const Range* range, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_Unknown_Slice)(ListUntyped* self, const ListUntypedVTable* v, const Range* range);

  typedef err_t (FOG_CDECL *List_Unknown_Sort)(ListUntyped* self, const ListUntypedVTable* v, uint32_t sortOrder, CompareFunc compareFunc);
  typedef err_t (FOG_CDECL *List_Unknown_SortEx)(ListUntyped* self, const ListUntypedVTable* v, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);

  typedef void (FOG_CDECL *List_Unknown_Copy)(ListUntyped* self, const ListUntypedVTable* v, const ListUntyped* other);

  typedef void (FOG_CDECL *List_Unknown_DRelease)(ListUntypedData* d, const ListUntypedVTable* v);
  typedef void (FOG_CDECL *List_Unknown_DFree)(ListUntypedData* d, const ListUntypedVTable* v);

  struct FOG_NO_EXPORT _Api_List_Unknown
  {
    List_Unknown_CtorSlice ctorSlice;
    List_Unknown_Dtor dtor;

    List_Unknown_Detach detach;
    List_Unknown_Reserve reserve;
    List_Unknown_Prepare prepare;
    List_Unknown_PrepareAppendItem prepareAppendItem;
    List_Unknown_PrepareInsertItem prepareInsertItem;
    List_Unknown_GrowSide growLeft;
    List_Unknown_GrowSide growRight;
    List_Unknown_Squeeze squeeze;

    List_Unknown_Clear clear;
    List_Unknown_Reset reset;
    List_Unknown_OpList opList;
    List_Unknown_OpData opData;
    List_Unknown_Remove remove;
    List_Unknown_Replace replace;
    List_Unknown_Slice slice;

    List_Unknown_Sort sort;
    List_Unknown_SortEx sortEx;

    List_Unknown_Copy copy;

    List_Unknown_DRelease dRelease;
    List_Unknown_DFree dFree;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - List<float>, List<double>]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *List_Float_OpListD)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_Float_OpDataD)(ListUntyped* self, uint32_t cntOp, const void* data, size_t dataLength);

  typedef err_t (FOG_CDECL *List_Double_OpListF)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_Double_OpDataF)(ListUntyped* self, uint32_t cntOp, const void* data, size_t dataLength);

  struct FOG_NO_EXPORT _Api_List_Float
  {
    List_Float_OpListD opListD;
    List_Float_OpDataD opDataD;
  };

  struct FOG_NO_EXPORT _Api_List_Double
  {
    List_Double_OpListF opListF;
    List_Double_OpDataF opDataF;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - List<StringA>, List<StringW>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *List_StringA_Ctor)(ListUntyped* self);
  typedef void (FOG_CDECL *List_StringW_Ctor)(ListUntyped* self);

  typedef void (FOG_CDECL *List_StringA_CtorSlice)(ListUntyped* self, const ListUntyped* other, const Range* range);
  typedef void (FOG_CDECL *List_StringW_CtorSlice)(ListUntyped* self, const ListUntyped* other, const Range* range);

  typedef void (FOG_CDECL *List_StringA_Dtor)(ListUntyped* self);
  typedef void (FOG_CDECL *List_StringW_Dtor)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_StringA_Detach)(ListUntyped* self);
  typedef err_t (FOG_CDECL *List_StringW_Detach)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_StringA_Reserve)(ListUntyped* self, size_t capacity);
  typedef err_t (FOG_CDECL *List_StringW_Reserve)(ListUntyped* self, size_t capacity);

  typedef err_t (FOG_CDECL *List_StringA_GrowBoth)(ListUntyped* self, size_t left, size_t right);
  typedef err_t (FOG_CDECL *List_StringW_GrowBoth)(ListUntyped* self, size_t left, size_t right);

  typedef err_t (FOG_CDECL *List_StringA_GrowSide)(ListUntyped* self, size_t length);
  typedef err_t (FOG_CDECL *List_StringW_GrowSide)(ListUntyped* self, size_t length);

  typedef void (FOG_CDECL *List_StringA_Squeeze)(ListUntyped* self);
  typedef void (FOG_CDECL *List_StringW_Squeeze)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_StringA_SetAtStubA)(ListUntyped* self, size_t index, const StubA* item);
  typedef err_t (FOG_CDECL *List_StringW_SetAtStubA)(ListUntyped* self, size_t index, const StubA* item);
  typedef err_t (FOG_CDECL *List_StringW_SetAtStubW)(ListUntyped* self, size_t index, const StubW* item);

  typedef err_t (FOG_CDECL *List_StringA_SetAtStringA)(ListUntyped* self, size_t index, const StringA* item);
  typedef err_t (FOG_CDECL *List_StringW_SetAtStringW)(ListUntyped* self, size_t index, const StringW* item);

  typedef void (FOG_CDECL *List_StringA_Clear)(ListUntyped* self);
  typedef void (FOG_CDECL *List_StringW_Clear)(ListUntyped* self);

  typedef void (FOG_CDECL *List_StringA_Reset)(ListUntyped* self);
  typedef void (FOG_CDECL *List_StringW_Reset)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_StringA_OpList)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_StringW_OpList)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);

  typedef err_t (FOG_CDECL *List_StringA_AppendStubA)(ListUntyped* self, const StubA* item);
  typedef err_t (FOG_CDECL *List_StringW_AppendStubA)(ListUntyped* self, const StubA* item);
  typedef err_t (FOG_CDECL *List_StringW_AppendStubW)(ListUntyped* self, const StubW* item);

  typedef err_t (FOG_CDECL *List_StringA_AppendStringA)(ListUntyped* self, const StringA* item);
  typedef err_t (FOG_CDECL *List_StringW_AppendStringW)(ListUntyped* self, const StringW* item);

  typedef err_t (FOG_CDECL *List_StringA_InsertStubA)(ListUntyped* self, size_t index, const StubA* item);
  typedef err_t (FOG_CDECL *List_StringW_InsertStubA)(ListUntyped* self, size_t index, const StubA* item);
  typedef err_t (FOG_CDECL *List_StringW_InsertStubW)(ListUntyped* self, size_t index, const StubW* item);

  typedef err_t (FOG_CDECL *List_StringA_InsertStringA)(ListUntyped* self, size_t index, const StringA* item);
  typedef err_t (FOG_CDECL *List_StringW_InsertStringW)(ListUntyped* self, size_t index, const StringW* item);

  typedef err_t (FOG_CDECL *List_StringA_Remove)(ListUntyped* self, const Range* range);
  typedef err_t (FOG_CDECL *List_StringW_Remove)(ListUntyped* self, const Range* range);

  typedef err_t (FOG_CDECL *List_StringA_Replace)(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_StringW_Replace)(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange);

  typedef err_t (FOG_CDECL *List_StringA_Slice)(ListUntyped* self, const Range* range);
  typedef err_t (FOG_CDECL *List_StringW_Slice)(ListUntyped* self, const Range* range);

  typedef size_t (FOG_CDECL *List_StringA_IndexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  typedef size_t (FOG_CDECL *List_StringW_IndexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  typedef size_t (FOG_CDECL *List_StringW_IndexOfStubW)(const ListUntyped* self, const Range* range, const StubW* item);

  typedef size_t (FOG_CDECL *List_StringA_IndexOfStringA)(const ListUntyped* self, const Range* range, const StringA* item);
  typedef size_t (FOG_CDECL *List_StringW_IndexOfStringW)(const ListUntyped* self, const Range* range, const StringW* item);

  typedef size_t (FOG_CDECL *List_StringA_LastIndexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  typedef size_t (FOG_CDECL *List_StringW_LastIndexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  typedef size_t (FOG_CDECL *List_StringW_LastIndexOfStubW)(const ListUntyped* self, const Range* range, const StubW* item);

  typedef size_t (FOG_CDECL *List_StringA_LastIndexOfStringA)(const ListUntyped* self, const Range* range, const StringA* item);
  typedef size_t (FOG_CDECL *List_StringW_LastIndexOfStringW)(const ListUntyped* self, const Range* range, const StringW* item);

  typedef err_t (FOG_CDECL *List_StringA_Sort)(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc);
  typedef err_t (FOG_CDECL *List_StringW_Sort)(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc);

  typedef err_t (FOG_CDECL *List_StringA_SortEx)(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  typedef err_t (FOG_CDECL *List_StringW_SortEx)(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);

  typedef err_t (FOG_CDECL *List_StringA_SwapItems)(ListUntyped* self, size_t index1, size_t index2);
  typedef err_t (FOG_CDECL *List_StringW_SwapItems)(ListUntyped* self, size_t index1, size_t index2);

  struct FOG_NO_EXPORT _Api_List_StringA
  {
    List_StringA_Ctor ctor;
    List_StringA_CtorSlice ctorSlice;
    List_StringA_Dtor dtor;

    List_StringA_Detach detach;
    List_StringA_Reserve reserve;
    List_StringA_GrowSide growLeft;
    List_StringA_GrowSide growRight;
    List_StringA_Squeeze squeeze;

    List_StringA_SetAtStubA setAtStubA;
    List_StringA_SetAtStringA setAtStringA;

    List_StringA_Clear clear;
    List_StringA_Reset reset;

    List_StringA_OpList opList;

    List_StringA_AppendStubA appendStubA;
    List_StringA_AppendStringA appendStringA;

    List_StringA_InsertStubA insertStubA;
    List_StringA_InsertStringA insertStringA;

    List_StringA_Remove remove;
    List_StringA_Replace replace;
    List_StringA_Slice slice;

    List_StringA_IndexOfStubA indexOfStubA;
    List_StringA_IndexOfStringA indexOfStringA;

    List_StringA_LastIndexOfStubA lastIndexOfStubA;
    List_StringA_LastIndexOfStringA lastIndexOfStringA;

    List_StringA_Sort sort;
    List_StringA_SortEx sortEx;
    List_StringA_SwapItems swapItems;

    ListUntypedVTable *vTable;
    ListUntyped* oEmpty;
  };

  struct FOG_NO_EXPORT _Api_List_StringW
  {
    List_StringW_Ctor ctor;
    List_StringW_CtorSlice ctorSlice;
    List_StringW_Dtor dtor;

    List_StringW_Detach detach;
    List_StringW_Reserve reserve;
    List_StringW_GrowSide growLeft;
    List_StringW_GrowSide growRight;
    List_StringW_Squeeze squeeze;

    List_StringW_SetAtStubA setAtStubA;
    List_StringW_SetAtStubW setAtStubW;
    List_StringW_SetAtStringW setAtStringW;

    List_StringW_Clear clear;
    List_StringW_Reset reset;

    List_StringW_OpList opList;

    List_StringW_AppendStubA appendStubA;
    List_StringW_AppendStubW appendStubW;
    List_StringW_AppendStringW appendStringW;

    List_StringW_InsertStubA insertStubA;
    List_StringW_InsertStubW insertStubW;
    List_StringW_InsertStringW insertStringW;

    List_StringW_Remove remove;
    List_StringW_Replace replace;
    List_StringW_Slice slice;

    List_StringW_IndexOfStubA indexOfStubA;
    List_StringW_IndexOfStubW indexOfStubW;
    List_StringW_IndexOfStringW indexOfStringW;

    List_StringW_LastIndexOfStubA lastIndexOfStubA;
    List_StringW_LastIndexOfStubW lastIndexOfStubW;
    List_StringW_LastIndexOfStringW lastIndexOfStringW;

    List_StringW_Sort sort;
    List_StringW_SortEx sortEx;
    List_StringW_SwapItems swapItems;

    ListUntypedVTable *vTable;
    ListUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Var>]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *List_Var_Ctor)(ListUntyped* self);
  typedef void (FOG_CDECL *List_Var_CtorSlice)(ListUntyped* self, const ListUntyped* other, const Range* range);
  typedef void (FOG_CDECL *List_Var_Dtor)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_Var_Detach)(ListUntyped* self);
  typedef err_t (FOG_CDECL *List_Var_Reserve)(ListUntyped* self, size_t capacity);
  typedef err_t (FOG_CDECL *List_Var_GrowBoth)(ListUntyped* self, size_t left, size_t right);
  typedef err_t (FOG_CDECL *List_Var_GrowSide)(ListUntyped* self, size_t length);
  typedef void (FOG_CDECL *List_Var_Squeeze)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_Var_SetAt)(ListUntyped* self, size_t index, const Var* item);

  typedef void (FOG_CDECL *List_Var_Clear)(ListUntyped* self);
  typedef void (FOG_CDECL *List_Var_Reset)(ListUntyped* self);

  typedef err_t (FOG_CDECL *List_Var_OpList)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);

  typedef err_t (FOG_CDECL *List_Var_Append)(ListUntyped* self, const Var* item);
  typedef err_t (FOG_CDECL *List_Var_Insert)(ListUntyped* self, size_t index, const Var* item);

  typedef err_t (FOG_CDECL *List_Var_Remove)(ListUntyped* self, const Range* range);
  typedef err_t (FOG_CDECL *List_Var_Replace)(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange);
  typedef err_t (FOG_CDECL *List_Var_Slice)(ListUntyped* self, const Range* range);

  typedef size_t (FOG_CDECL *List_Var_IndexOf)(const ListUntyped* self, const Range* range, const Var* item);
  typedef size_t (FOG_CDECL *List_Var_LastIndexOf)(const ListUntyped* self, const Range* range, const Var* item);

  typedef err_t (FOG_CDECL *List_Var_Sort)(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc);
  typedef err_t (FOG_CDECL *List_Var_SortEx)(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  typedef err_t (FOG_CDECL *List_Var_SwapItems)(ListUntyped* self, size_t index1, size_t index2);

  struct FOG_NO_EXPORT _Api_ListVar
  {
    List_Var_Ctor ctor;
    List_Var_CtorSlice ctorSlice;
    List_Var_Dtor dtor;

    List_Var_Detach detach;
    List_Var_Reserve reserve;
    List_Var_GrowSide growLeft;
    List_Var_GrowSide growRight;
    List_Var_Squeeze squeeze;

    List_Var_SetAt setAt;

    List_Var_Clear clear;
    List_Var_Reset reset;

    List_Var_OpList opList;

    List_Var_Append append;
    List_Var_Insert insert;

    List_Var_Remove remove;
    List_Var_Replace replace;
    List_Var_Slice slice;

    List_Var_IndexOf indexOf;
    List_Var_LastIndexOf lastIndexOf;

    List_Var_Sort sort;
    List_Var_SortEx sortEx;
    List_Var_SwapItems swapItems;

    ListUntypedVTable *vTable;
    ListUntyped* oEmpty;
  };

  // --------------------------------------------------------------------------
  // [Core/Tools - List<...>]
  // --------------------------------------------------------------------------

  struct FOG_NO_EXPORT _Api_List
  {
    _Api_List_Untyped untyped;

    _Api_List_Simple simple;
    _Api_List_Unknown unknown;

    _Api_List_Float pod_float;
    _Api_List_Double pod_double;

    _Api_List_StringA stringa;
    _Api_List_StringW stringw;

    _Api_ListVar var;
  } list;

  // --------------------------------------------------------------------------
  // [Core/Tools - Locale]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Locale_Ctor)(Locale* self);
  typedef void (FOG_CDECL *Locale_CtorCopy)(Locale* self, const Locale* other);
  typedef void (FOG_CDECL *Locale_CtorString)(Locale* self, const StringW* name);
  typedef void (FOG_CDECL *Locale_Dtor)(Locale* self);

  typedef err_t (FOG_CDECL *Locale_Detach)(Locale* self);
  typedef void (FOG_CDECL *Locale_Reset)(Locale* self);

  typedef void (FOG_CDECL *Locale_SetLocale)(Locale* self, const Locale* other);
  typedef err_t (FOG_CDECL *Locale_SetChar)(Locale* self, uint32_t id, uint16_t ch);

  typedef err_t (FOG_CDECL *Locale_Create)(Locale* self, const StringW* name);

  typedef LocaleData* (FOG_CDECL *Locale_DCreate)(void);
  typedef void (FOG_CDECL *Locale_DFree)(LocaleData* d);

  struct FOG_NO_EXPORT _Api_Locale
  {
    Locale_Ctor ctor;
    Locale_CtorCopy ctorCopy;
    Locale_CtorString ctorString;
    Locale_Dtor dtor;
    Locale_Detach detach;
    Locale_Reset reset;
    Locale_Create create;
    Locale_SetLocale setLocale;
    Locale_SetChar setChar;

    Locale_DCreate dCreate;
    Locale_DFree dFree;

    Locale* oPosix;
    Locale* oUser;
  } locale;

  // --------------------------------------------------------------------------
  // [Core/Tools - RegExp]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *RegExpA_Ctor)(RegExpA* self);
  typedef void (FOG_CDECL *RegExpW_Ctor)(RegExpW* self);

  typedef void (FOG_CDECL *RegExpA_CtorCopy)(RegExpA* self, const RegExpA* other);
  typedef void (FOG_CDECL *RegExpW_CtorCopy)(RegExpW* self, const RegExpW* other);

  typedef void (FOG_CDECL *RegExpA_Dtor)(RegExpA* self);
  typedef void (FOG_CDECL *RegExpW_Dtor)(RegExpW* self);

  typedef void (FOG_CDECL *RegExpA_Reset)(RegExpA* self);
  typedef void (FOG_CDECL *RegExpW_Reset)(RegExpW* self);

  typedef void (FOG_CDECL *RegExpA_Copy)(RegExpA* self, const RegExpA* other);
  typedef void (FOG_CDECL *RegExpW_Copy)(RegExpW* self, const RegExpW* other);

  typedef err_t (FOG_CDECL *RegExpA_CreateStubA)(RegExpA* self, const StubA* stub, uint32_t type, uint32_t cs);
  typedef err_t (FOG_CDECL *RegExpW_CreateStubA)(RegExpW* self, const StubA* stub, uint32_t type, uint32_t cs);
  typedef err_t (FOG_CDECL *RegExpW_CreateStubW)(RegExpW* self, const StubW* stub, uint32_t type, uint32_t cs);

  typedef err_t (FOG_CDECL *RegExpA_CreateStringA)(RegExpA* self, const StringA* stub, uint32_t type, uint32_t cs);
  typedef err_t (FOG_CDECL *RegExpW_CreateStringW)(RegExpW* self, const StringW* stub, uint32_t type, uint32_t cs);

  typedef bool (FOG_CDECL *RegExpA_IndexIn)(const RegExpA* self, const char* sData, size_t sLength, const Range* sRange, Range* out);
  typedef bool (FOG_CDECL *RegExpW_IndexIn)(const RegExpW* self, const CharW* sData, size_t sLength, const Range* sRange, Range* out);

  typedef bool (FOG_CDECL *RegExpA_LastIndexIn)(const RegExpA* self, const char* sData, size_t sLength, const Range* sRange, Range* out);
  typedef bool (FOG_CDECL *RegExpW_LastIndexIn)(const RegExpW* self, const CharW* sData, size_t sLength, const Range* sRange, Range* out);

  struct FOG_NO_EXPORT _Api_StringMactherA
  {
    RegExpA_Ctor ctor;
    RegExpA_CtorCopy ctorCopy;
    RegExpA_Dtor dtor;
    RegExpA_Reset reset;
    RegExpA_Copy copy;
    RegExpA_CreateStubA createStubA;
    RegExpA_CreateStringA createStringA;
    RegExpA_IndexIn indexIn;
    RegExpA_LastIndexIn lastIndexIn;

    RegExpA* oEmpty;
  } regexpa;

  struct FOG_NO_EXPORT _Api_StringMactherW
  {
    RegExpW_Ctor ctor;
    RegExpW_CtorCopy ctorCopy;
    RegExpW_Dtor dtor;
    RegExpW_Reset reset;
    RegExpW_Copy copy;
    RegExpW_CreateStubA createStubA;
    RegExpW_CreateStubW createStubW;
    RegExpW_CreateStringW createStringW;
    RegExpW_IndexIn indexIn;
    RegExpW_LastIndexIn lastIndexIn;

    RegExpW* oEmpty;
  } regexpw;

  // --------------------------------------------------------------------------
  // [Core/Tools - String]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *StringA_Ctor)(StringA* self);
  typedef void (FOG_CDECL *StringW_Ctor)(StringW* self);

  typedef void (FOG_CDECL *StringA_CtorStubA)(StringA* self, const StubA* stub);
  typedef void (FOG_CDECL *StringW_CtorStubA)(StringW* self, const StubA* stub);
  typedef void (FOG_CDECL *StringW_CtorStubW)(StringW* self, const StubW* stub);
  typedef void (FOG_CDECL *StringW_CtorCodec)(StringW* self, const StubA* stub, const TextCodec* tc);

  typedef void (FOG_CDECL *StringA_CtorStubA2)(StringA* self, const StubA* a, const StubA* b);
  typedef void (FOG_CDECL *StringW_CtorStubA2)(StringW* self, const StubA* a, const StubA* b);
  typedef void (FOG_CDECL *StringW_CtorStubW2)(StringW* self, const StubW* a, const StubW* b);

  typedef void (FOG_CDECL *StringA_CtorCopyA)(StringA* self, const StringA* other);
  typedef void (FOG_CDECL *StringW_CtorCopyW)(StringW* self, const StringW* other);

  typedef void (FOG_CDECL *StringA_CtorCopyA2)(StringA* self, const StringA* a, const StringA* b);
  typedef void (FOG_CDECL *StringW_CtorCopyW2)(StringW* self, const StringW* a, const StringW* b);

  typedef void (FOG_CDECL *StringA_CtorSubstr)(StringA* self, const StringA* other, const Range* range);
  typedef void (FOG_CDECL *StringW_CtorSubstr)(StringW* self, const StringW* other, const Range* range);

  typedef void (FOG_CDECL *StringA_CtorU32)(StringA* self, uint32_t n, bool isUnsigned);
  typedef void (FOG_CDECL *StringW_CtorU32)(StringW* self, uint32_t n, bool isUnsigned);

  typedef void (FOG_CDECL *StringA_CtorU64)(StringA* self, uint64_t n, bool isUnsigned);
  typedef void (FOG_CDECL *StringW_CtorU64)(StringW* self, uint64_t n, bool isUnsigned);

  typedef void (FOG_CDECL *StringA_CtorDouble)(StringA* self, double d);
  typedef void (FOG_CDECL *StringW_CtorDouble)(StringW* self, double d);

  typedef void (FOG_CDECL *StringA_Dtor)(StringA* self);
  typedef void (FOG_CDECL *StringW_Dtor)(StringW* self);

  typedef err_t (FOG_CDECL *StringA_Detach)(StringA* self);
  typedef err_t (FOG_CDECL *StringW_Detach)(StringW* self);

  typedef err_t (FOG_CDECL *StringA_Reserve)(StringA* self, size_t capacity);
  typedef err_t (FOG_CDECL *StringW_Reserve)(StringW* self, size_t capacity);

  typedef err_t (FOG_CDECL *StringA_Resize)(StringA* self, size_t length);
  typedef err_t (FOG_CDECL *StringW_Resize)(StringW* self, size_t length);

  typedef err_t (FOG_CDECL *StringA_Truncate)(StringA* self, size_t length);
  typedef err_t (FOG_CDECL *StringW_Truncate)(StringW* self, size_t length);

  typedef void (FOG_CDECL *StringA_Squeeze)(StringA* self);
  typedef void (FOG_CDECL *StringW_Squeeze)(StringW* self);

  typedef char* (FOG_CDECL *StringA_Prepare)(StringA* self, uint32_t cntOp, size_t length);
  typedef CharW* (FOG_CDECL *StringW_Prepare)(StringW* self, uint32_t cntOp, size_t length);

  typedef char* (FOG_CDECL *StringA_Add)(StringA* self, size_t length);
  typedef CharW* (FOG_CDECL *StringW_Add)(StringW* self, size_t length);

  typedef void (FOG_CDECL *StringA_Clear)(StringA* self);
  typedef void (FOG_CDECL *StringW_Clear)(StringW* self);

  typedef void (FOG_CDECL *StringA_Reset)(StringA* self);
  typedef void (FOG_CDECL *StringW_Reset)(StringW* self);

  typedef uint32_t (FOG_CDECL *StringA_GetHashCode)(const StringA* self);
  typedef uint32_t (FOG_CDECL *StringW_GetHashCode)(const StringW* self);

  typedef err_t (FOG_CDECL *StringA_SetStubA)(StringA* self, const StubA* stub);
  typedef err_t (FOG_CDECL *StringW_SetStubA)(StringW* self, const StubA* stub, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_SetStubW)(StringW* self, const StubW* stub);

  typedef err_t (FOG_CDECL *StringA_SetStringA)(StringA* self, const StringA* other);
  typedef err_t (FOG_CDECL *StringW_SetStringA)(StringW* self, const StringA* other, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_SetStringW)(StringW* self, const StringW* other);

  typedef err_t (FOG_CDECL *StringA_SetStringExA)(StringA* self, const StringA* other, const Range* range);
  typedef err_t (FOG_CDECL *StringW_SetStringExW)(StringW* self, const StringW* other, const Range* range);

  typedef err_t (FOG_CDECL *StringA_SetDeep)(StringA* self, const StringA* other);
  typedef err_t (FOG_CDECL *StringW_SetDeep)(StringW* self, const StringW* other);

  typedef err_t (FOG_CDECL *StringA_AppendStubA)(StringA* self, const StubA* stub);
  typedef err_t (FOG_CDECL *StringW_AppendStubA)(StringW* self, const StubA* stub, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_AppendStubW)(StringW* self, const StubW* stub);

  typedef err_t (FOG_CDECL *StringA_AppendStringA)(StringA* self, const StringA* other);
  typedef err_t (FOG_CDECL *StringW_AppendStringA)(StringW* self, const StringA* other, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_AppendStringW)(StringW* self, const StringW* other);

  typedef err_t (FOG_CDECL *StringA_AppendStringExA)(StringA* self, const StringA* other, const Range* range);
  typedef err_t (FOG_CDECL *StringW_AppendStringExW)(StringW* self, const StringW* other, const Range* range);

  typedef err_t (FOG_CDECL *StringA_OpFill)(StringA* self, uint32_t cntOp, char ch, size_t len);
  typedef err_t (FOG_CDECL *StringW_OpFill)(StringW* self, uint32_t cntOp, uint16_t ch, size_t len);

  typedef err_t (FOG_CDECL *StringA_OpBool)(StringA* self, uint32_t cntOp, bool b);
  typedef err_t (FOG_CDECL *StringW_OpBool)(StringW* self, uint32_t cntOp, bool b);

  typedef err_t (FOG_CDECL *StringA_OpI32)(StringA* self, uint32_t cntOp, int32_t n);
  typedef err_t (FOG_CDECL *StringW_OpI32)(StringW* self, uint32_t cntOp, int32_t n);

  typedef err_t (FOG_CDECL *StringA_OpI32Ex)(StringA* self, uint32_t cntOp, int32_t n, const FormatInt* fmt);
  typedef err_t (FOG_CDECL *StringW_OpI32Ex)(StringW* self, uint32_t cntOp, int32_t n, const FormatInt* fmt, const Locale* locale);

  typedef err_t (FOG_CDECL *StringA_OpU32)(StringA* self, uint32_t cntOp, uint32_t n);
  typedef err_t (FOG_CDECL *StringW_OpU32)(StringW* self, uint32_t cntOp, uint32_t n);

  typedef err_t (FOG_CDECL *StringA_OpU32Ex)(StringA* self, uint32_t cntOp, uint32_t n, const FormatInt* fmt);
  typedef err_t (FOG_CDECL *StringW_OpU32Ex)(StringW* self, uint32_t cntOp, uint32_t n, const FormatInt* fmt, const Locale* locale);

  typedef err_t (FOG_CDECL *StringA_OpI64)(StringA* self, uint32_t cntOp, int64_t n);
  typedef err_t (FOG_CDECL *StringW_OpI64)(StringW* self, uint32_t cntOp, int64_t n);

  typedef err_t (FOG_CDECL *StringA_OpI64Ex)(StringA* self, uint32_t cntOp, int64_t n, const FormatInt* fmt);
  typedef err_t (FOG_CDECL *StringW_OpI64Ex)(StringW* self, uint32_t cntOp, int64_t n, const FormatInt* fmt, const Locale* locale);

  typedef err_t (FOG_CDECL *StringA_OpU64)(StringA* self, uint32_t cntOp, uint64_t n);
  typedef err_t (FOG_CDECL *StringW_OpU64)(StringW* self, uint32_t cntOp, uint64_t n);

  typedef err_t (FOG_CDECL *StringA_OpU64Ex)(StringA* self, uint32_t cntOp, uint64_t n, const FormatInt* fmt);
  typedef err_t (FOG_CDECL *StringW_OpU64Ex)(StringW* self, uint32_t cntOp, uint64_t n, const FormatInt* fmt, const Locale* locale);

  typedef err_t (FOG_CDECL *StringA_OpDouble)(StringA* self, uint32_t cntOp, double d);
  typedef err_t (FOG_CDECL *StringW_OpDouble)(StringW* self, uint32_t cntOp, double d);

  typedef err_t (FOG_CDECL *StringA_OpDoubleEx)(StringA* self, uint32_t cntOp, double d, const FormatReal* fmt);
  typedef err_t (FOG_CDECL *StringW_OpDoubleEx)(StringW* self, uint32_t cntOp, double d, const FormatReal* fmt, const Locale* locale);

  typedef err_t (FOG_CDECL *StringA_OpVFormatStubA)(StringA* self, uint32_t cntOp, const StubA* fmt, const TextCodec* tc, va_list ap);
  typedef err_t (FOG_CDECL *StringW_OpVFormatStubA)(StringW* self, uint32_t cntOp, const StubA* fmt, const TextCodec* tc, const Locale* locale, va_list ap);
  typedef err_t (FOG_CDECL *StringW_OpVFormatStubW)(StringW* self, uint32_t cntOp, const StubW* fmt, const TextCodec* tc, const Locale* locale, va_list ap);

  typedef err_t (FOG_CDECL *StringA_OpVFormatStringA)(StringA* self, uint32_t cntOp, const StringA* fmt, const TextCodec* tc, va_list ap);
  typedef err_t (FOG_CDECL *StringW_OpVFormatStringW)(StringW* self, uint32_t cntOp, const StringW* fmt, const TextCodec* tc, const Locale* locale, va_list ap);

  typedef err_t (FOG_CDECL *StringA_OpZFormatStubA)(StringA* self, uint32_t cntOp, const StubA* fmt, char lex, const StringA* args, size_t argsLength);
  typedef err_t (FOG_CDECL *StringW_OpZFormatStubW)(StringW* self, uint32_t cntOp, const StubW* fmt, uint16_t lex, const StringW* args, size_t argsLength);

  typedef err_t (FOG_CDECL *StringA_OpZFormatStringA)(StringA* self, uint32_t cntOp, const StringA* fmt, char lex, const StringA* args, size_t argsLength);
  typedef err_t (FOG_CDECL *StringW_OpZFormatStringW)(StringW* self, uint32_t cntOp, const StringW* fmt, uint16_t lex, const StringW* args, size_t argsLength);

  typedef err_t (FOG_CDECL *StringA_OpNormalizeSlashesA)(StringA* self, uint32_t cntOp, const StringA* other, uint32_t slashForm);
  typedef err_t (FOG_CDECL *StringW_OpNormalizeSlashesW)(StringW* self, uint32_t cntOp, const StringW* other, uint32_t slashForm);

  typedef err_t (FOG_CDECL *StringA_PrependChars)(StringA* self, char ch, size_t len);
  typedef err_t (FOG_CDECL *StringW_PrependChars)(StringW* self, uint16_t ch, size_t len);

  typedef err_t (FOG_CDECL *StringA_PrependStubA)(StringA* self, const StubA* stub);
  typedef err_t (FOG_CDECL *StringW_PrependStubA)(StringW* self, const StubA* stub, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_PrependStubW)(StringW* self, const StubW* stub);

  typedef err_t (FOG_CDECL *StringA_PrependStringA)(StringA* self, const StringA* other);
  typedef err_t (FOG_CDECL *StringW_PrependStringA)(StringW* self, const StringA* other, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_PrependStringW)(StringW* self, const StringW* other);

  typedef err_t (FOG_CDECL *StringA_InsertChars)(StringA* self, size_t index, char ch, size_t length);
  typedef err_t (FOG_CDECL *StringW_InsertChars)(StringW* self, size_t index, uint16_t ch, size_t length);

  typedef err_t (FOG_CDECL *StringA_InsertStubA)(StringA* self, size_t index, const StubA* stub);
  typedef err_t (FOG_CDECL *StringW_InsertStubA)(StringW* self, size_t index, const StubA* stub, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_InsertStubW)(StringW* self, size_t index, const StubW* stub);

  typedef err_t (FOG_CDECL *StringA_InsertStringA)(StringA* self, size_t index, const StringA* other);
  typedef err_t (FOG_CDECL *StringW_InsertStringA)(StringW* self, size_t index, const StringA* other, const TextCodec* tc);
  typedef err_t (FOG_CDECL *StringW_InsertStringW)(StringW* self, size_t index, const StringW* other);

  typedef err_t (FOG_CDECL *StringA_RemoveRange)(StringA* self, const Range* range);
  typedef err_t (FOG_CDECL *StringW_RemoveRange)(StringW* self, const Range* range);

  typedef err_t (FOG_CDECL *StringA_RemoveRangeList)(StringA* self, const Range* range, size_t rangeLength);
  typedef err_t (FOG_CDECL *StringW_RemoveRangeList)(StringW* self, const Range* range, size_t rangeLength);

  typedef err_t (FOG_CDECL *StringA_RemoveChar)(StringA* self, const Range* range, char ch, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_RemoveChar)(StringW* self, const Range* range, uint16_t ch, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_RemoveStubA)(StringA* self, const Range* range, const StubA* stub, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_RemoveStubA)(StringW* self, const Range* range, const StubA* stub, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_RemoveStubW)(StringW* self, const Range* range, const StubW* stub, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_RemoveStringA)(StringA* self, const Range* range, const StringA* other, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_RemoveStringW)(StringW* self, const Range* range, const StringW* other, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_RemoveRegExpA)(StringA* self, const Range* range, const RegExpA* re);
  typedef err_t (FOG_CDECL *StringW_RemoveRegExpW)(StringW* self, const Range* range, const RegExpW* re);

  typedef err_t (FOG_CDECL *StringA_ReplaceRangeStubA)(StringA* self, const Range* range, const StubA* replacement);
  typedef err_t (FOG_CDECL *StringW_ReplaceRangeStubW)(StringW* self, const Range* range, const StubW* replacement);

  typedef err_t (FOG_CDECL *StringA_ReplaceRangeStringA)(StringA* self, const Range* range, const StringA* replacement);
  typedef err_t (FOG_CDECL *StringW_ReplaceRangeStringW)(StringW* self, const Range* range, const StringW* replacement);

  typedef err_t (FOG_CDECL *StringA_ReplaceRangeListStubA)(StringA* self, const Range* range, size_t rangeLength, const StubA* replacement);
  typedef err_t (FOG_CDECL *StringW_ReplaceRangeListStubW)(StringW* self, const Range* range, size_t rangeLength, const StubW* replacement);

  typedef err_t (FOG_CDECL *StringA_ReplaceRangeListStringA)(StringA* self, const Range* range, size_t rangeLength, const StringA* replacement);
  typedef err_t (FOG_CDECL *StringW_ReplaceRangeListStringW)(StringW* self, const Range* range, size_t rangeLength, const StringW* replacement);

  typedef err_t (FOG_CDECL *StringA_ReplaceChar)(StringA* self, const Range* range, char before, char after, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_ReplaceChar)(StringW* self, const Range* range, uint16_t before, uint16_t after, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_ReplaceStringA)(StringA* self, const Range* range, const StringA* pattern, const StringA* replacement, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_ReplaceStringW)(StringW* self, const Range* range, const StringW* pattern, const StringW* replacement, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_ReplaceRegExpA)(StringA* self, const Range* range, const RegExpA* re, const StringA* replacement);
  typedef err_t (FOG_CDECL *StringW_ReplaceRegExpW)(StringW* self, const Range* range, const RegExpW* re, const StringW* replacement);

  typedef err_t (FOG_CDECL *StringA_Lower)(StringA* self, const Range* range);
  typedef err_t (FOG_CDECL *StringW_Lower)(StringW* self, const Range* range);

  typedef err_t (FOG_CDECL *StringA_Upper)(StringA* self, const Range* range);
  typedef err_t (FOG_CDECL *StringW_Upper)(StringW* self, const Range* range);

  typedef err_t (FOG_CDECL *StringA_Trim)(StringA* self);
  typedef err_t (FOG_CDECL *StringW_Trim)(StringW* self);

  typedef err_t (FOG_CDECL *StringA_Simplify)(StringA* self);
  typedef err_t (FOG_CDECL *StringW_Simplify)(StringW* self);

  typedef err_t (FOG_CDECL *StringA_Justify)(StringA* self, size_t n, char ch, uint32_t flags);
  typedef err_t (FOG_CDECL *StringW_Justify)(StringW* self, size_t n, uint16_t ch, uint32_t flags);

  typedef err_t (FOG_CDECL *StringA_SplitChar)(List<StringA>* dst, uint32_t cntOp, const StringA* src, const Range* range, char ch, uint32_t splitBehavior, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_SplitChar)(List<StringW>* dst, uint32_t cntOp, const StringW* src, const Range* range, uint16_t ch, uint32_t splitBehavior, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_SplitStringA)(List<StringA>* dst, uint32_t cntOp, const StringA* src, const Range* range, const StringA* pattern, uint32_t splitBehavior, uint32_t cs);
  typedef err_t (FOG_CDECL *StringW_SplitStringW)(List<StringW>* dst, uint32_t cntOp, const StringW* src, const Range* range, const StringW* pattern, uint32_t splitBehavior, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_SplitRegExpA)(List<StringA>* dst, uint32_t cntOp, const StringA* src, const Range* range, const RegExpA* re, uint32_t splitBehavior);
  typedef err_t (FOG_CDECL *StringW_SplitRegExpW)(List<StringW>* dst, uint32_t cntOp, const StringW* src, const Range* range, const RegExpW* re, uint32_t splitBehavior);

  typedef err_t (FOG_CDECL *StringA_Slice)(StringA* self, const Range* range);
  typedef err_t (FOG_CDECL *StringW_Slice)(StringW* self, const Range* range);

  typedef err_t (FOG_CDECL *StringA_JoinChar)(StringA* self, const StringA* list, size_t listLength, char sep);
  typedef err_t (FOG_CDECL *StringW_JoinChar)(StringW* self, const StringW* list, size_t listLength, uint16_t sep);

  typedef err_t (FOG_CDECL *StringA_JoinStringA)(StringA* self, const StringA* list, size_t listLength, const StringA* sep);
  typedef err_t (FOG_CDECL *StringW_JoinStringW)(StringW* self, const StringW* list, size_t listLength, const StringW* sep);

  typedef err_t (FOG_CDECL *StringA_ParseBool)(const StringA* self, bool* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseBool)(const StringW* self, bool* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseI8)(const StringA* self, int8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseI8)(const StringW* self, int8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseU8)(const StringA* self, uint8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseU8)(const StringW* self, uint8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseI16)(const StringA* self, int16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseI16)(const StringW* self, int16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseU16)(const StringA* self, uint16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseU16)(const StringW* self, uint16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseI32)(const StringA* self, int32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseI32)(const StringW* self, int32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseU32)(const StringA* self, uint32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseU32)(const StringW* self, uint32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseI64)(const StringA* self, int64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseI64)(const StringW* self, int64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseU64)(const StringA* self, uint64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseU64)(const StringW* self, uint64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseFloat)(const StringA* self, float* dst, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseFloat)(const StringW* self, float* dst, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringA_ParseDouble)(const StringA* self, double* dst, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringW_ParseDouble)(const StringW* self, double* dst, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);

  typedef size_t (FOG_CDECL *StringA_CountOfChar)(const StringA* self, const Range* range, char ch, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_CountOfChar)(const StringW* self, const Range* range, uint16_t ch, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_CountOfStubA)(const StringA* self, const Range* range, const StubA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_CountOfStubA)(const StringW* self, const Range* range, const StubA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_CountOfStubW)(const StringW* self, const Range* range, const StubW* pattern, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_CountOfStringA)(const StringA* self, const Range* range, const StringA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_CountOfStringW)(const StringW* self, const Range* range, const StringW* pattern, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_CountOfRegExpA)(const StringA* self, const Range* range, const RegExpA* re);
  typedef size_t (FOG_CDECL *StringW_CountOfRegExpW)(const StringW* self, const Range* range, const RegExpW* re);

  typedef size_t (FOG_CDECL *StringA_IndexOfChar)(const StringA* self, const Range* range, char ch, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_IndexOfChar)(const StringW* self, const Range* range, uint16_t ch, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_IndexOfStubA)(const StringA* self, const Range* range, const StubA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_IndexOfStubA)(const StringW* self, const Range* range, const StubA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_IndexOfStubW)(const StringW* self, const Range* range, const StubW* pattern, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_IndexOfStringA)(const StringA* self, const Range* range, const StringA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_IndexOfStringW)(const StringW* self, const Range* range, const StringW* pattern, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_IndexOfRegExpA)(const StringA* self, const Range* range, const RegExpA* re);
  typedef size_t (FOG_CDECL *StringW_IndexOfRegExpW)(const StringW* self, const Range* range, const RegExpW* re);

  typedef size_t (FOG_CDECL *StringA_IndexOfAnyCharA)(const StringA* self, const Range* range, const char* charArray, size_t charLength, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_IndexOfAnyCharW)(const StringW* self, const Range* range, const CharW* charArray, size_t charLength, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_LastIndexOfChar)(const StringA* self, const Range* range, char ch, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_LastIndexOfChar)(const StringW* self, const Range* range, uint16_t ch, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_LastIndexOfStubA)(const StringA* self, const Range* range, const StubA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_LastIndexOfStubA)(const StringW* self, const Range* range, const StubA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_LastIndexOfStubW)(const StringW* self, const Range* range, const StubW* pattern, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_LastIndexOfStringA)(const StringA* self, const Range* range, const StringA* pattern, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_LastIndexOfStringW)(const StringW* self, const Range* range, const StringW* pattern, uint32_t cs);

  typedef size_t (FOG_CDECL *StringA_LastIndexOfRegExpA)(const StringA* self, const Range* range, const RegExpA* re);
  typedef size_t (FOG_CDECL *StringW_LastIndexOfRegExpW)(const StringW* self, const Range* range, const RegExpW* re);

  typedef size_t (FOG_CDECL *StringA_LastIndexOfAnyCharA)(const StringA* self, const Range* range, const char* charArray, size_t charLength, uint32_t cs);
  typedef size_t (FOG_CDECL *StringW_LastIndexOfAnyCharW)(const StringW* self, const Range* range, const CharW* charArray, size_t charLength, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_StartsWithChar)(const StringA* self, char ch, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_StartsWithChar)(const StringW* self, uint16_t ch, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_StartsWithStubA)(const StringA* self, const StubA* str, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_StartsWithStubA)(const StringW* self, const StubA* str, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_StartsWithStubW)(const StringW* self, const StubW* str, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_StartsWithStringA)(const StringA* self, const StringA* str, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_StartsWithStringW)(const StringW* self, const StringW* str, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_StartsWithRegExpA)(const StringA* self, const RegExpA* re);
  typedef bool (FOG_CDECL *StringW_StartsWithRegExpW)(const StringW* self, const RegExpW* re);

  typedef bool (FOG_CDECL *StringA_EndsWithChar)(const StringA* self, char ch, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_EndsWithChar)(const StringW* self, uint16_t ch, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_EndsWithStubA)(const StringA* self, const StubA* str, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_EndsWithStubA)(const StringW* self, const StubA* str, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_EndsWithStubW)(const StringW* self, const StubW* str, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_EndsWithStringA)(const StringA* self, const StringA* str, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_EndsWithStringW)(const StringW* self, const StringW* str, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_EndsWithRegExpA)(const StringA* self, const RegExpA* re);
  typedef bool (FOG_CDECL *StringW_EndsWithRegExpW)(const StringW* self, const RegExpW* re);

  typedef bool (FOG_CDECL *StringA_EqStubA)(const StringA* a, const StubA* b);
  typedef bool (FOG_CDECL *StringW_EqStubA)(const StringW* a, const StubA* b);
  typedef bool (FOG_CDECL *StringW_EqStubW)(const StringW* a, const StubW* b);

  typedef bool (FOG_CDECL *StringA_EqStringA)(const StringA* a, const StringA* b);
  typedef bool (FOG_CDECL *StringW_EqStringW)(const StringW* a, const StringW* b);

  typedef bool (FOG_CDECL *StringA_EqStubExA)(const StringA* a, const StubA* b, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_EqStubExA)(const StringW* a, const StubA* b, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_EqStubExW)(const StringW* a, const StubW* b, uint32_t cs);

  typedef bool (FOG_CDECL *StringA_EqStringExA)(const StringA* a, const StringA* b, uint32_t cs);
  typedef bool (FOG_CDECL *StringW_EqStringExW)(const StringW* a, const StringW* b, uint32_t cs);

  typedef int (FOG_CDECL *StringA_CompareStubA)(const StringA* a, const StubA* b);
  typedef int (FOG_CDECL *StringW_CompareStubA)(const StringW* a, const StubA* b);
  typedef int (FOG_CDECL *StringW_CompareStubW)(const StringW* a, const StubW* b);

  typedef int (FOG_CDECL *StringA_CompareStringA)(const StringA* a, const StringA* b);
  typedef int (FOG_CDECL *StringW_CompareStringW)(const StringW* a, const StringW* b);

  typedef int (FOG_CDECL *StringA_CompareStubExA)(const StringA* a, const StubA* b, uint32_t cs);
  typedef int (FOG_CDECL *StringW_CompareStubExA)(const StringW* a, const StubA* b, uint32_t cs);
  typedef int (FOG_CDECL *StringW_CompareStubExW)(const StringW* a, const StubW* b, uint32_t cs);

  typedef int (FOG_CDECL *StringA_CompareStringExA)(const StringA* a, const StringA* b, uint32_t cs);
  typedef int (FOG_CDECL *StringW_CompareStringExW)(const StringW* a, const StringW* b, uint32_t cs);

  typedef err_t (FOG_CDECL *StringA_ValidateUtf8)(const StringA* self, size_t* invalid);
  typedef err_t (FOG_CDECL *StringW_ValidateUtf16)(const StringW* self, size_t* invalid);

  typedef err_t (FOG_CDECL *StringA_GetUcsLength)(const StringA* self, size_t* ucsLength);
  typedef err_t (FOG_CDECL *StringW_GetUcsLength)(const StringW* self, size_t* ucsLength);

  typedef err_t (FOG_CDECL *StringA_HexDecode)(StringA* dst, uint32_t cntOp, const StringA* src);
  typedef err_t (FOG_CDECL *StringA_HexEncode)(StringA* dst, uint32_t cntOp, const StringA* src, uint32_t textCase);

  typedef err_t (FOG_CDECL *StringA_Base64DecodeStringA)(StringA* dst, uint32_t cntOp, const StringA* src);
  typedef err_t (FOG_CDECL *StringA_Base64DecodeStringW)(StringA* dst, uint32_t cntOp, const StringW* src);

  typedef err_t (FOG_CDECL *StringA_Base64DecodeDataA)(StringA* dst, uint32_t cntOp, const char* src, size_t srcLength);
  typedef err_t (FOG_CDECL *StringA_Base64DecodeDataW)(StringA* dst, uint32_t cntOp, const CharW* src, size_t srcLength);

  typedef err_t (FOG_CDECL *StringA_Base64EncodeStringA)(StringA* dst, uint32_t cntOp, const StringA* src);
  typedef err_t (FOG_CDECL *StringW_Base64EncodeStringA)(StringW* dst, uint32_t cntOp, const StringA* src);

  typedef err_t (FOG_CDECL *StringA_Base64EncodeDataA)(StringA* dst, uint32_t cntOp, const char* src, size_t srcLength);
  typedef err_t (FOG_CDECL *StringW_Base64EncodeDataA)(StringW* dst, uint32_t cntOp, const char* src, size_t srcLength);

  typedef err_t (FOG_CDECL *StringW_BSwap)(StringW* self);

  typedef StringDataA* (FOG_CDECL *StringA_DCreate)(size_t capacity);
  typedef StringDataW* (FOG_CDECL *StringW_DCreate)(size_t capacity);

  typedef StringDataA* (FOG_CDECL *StringA_DCreateStubA)(size_t capacity, const StubA* stub);
  typedef StringDataW* (FOG_CDECL *StringW_DCreateStubA)(size_t capacity, const StubA* stub);
  typedef StringDataW* (FOG_CDECL *StringW_DCreateStubW)(size_t capacity, const StubW* stub);

  typedef StringDataA* (FOG_CDECL *StringA_DAdopt)(void* address, size_t capacity);
  typedef StringDataW* (FOG_CDECL *StringW_DAdopt)(void* address, size_t capacity);

  typedef StringDataA* (FOG_CDECL *StringA_DAdoptStubA)(void* address, size_t capacity, const StubA* stub);
  typedef StringDataW* (FOG_CDECL *StringW_DAdoptStubA)(void* address, size_t capacity, const StubA* stub);
  typedef StringDataW* (FOG_CDECL *StringW_DAdoptStubW)(void* address, size_t capacity, const StubW* stub);

  typedef StringDataA* (FOG_CDECL *StringA_DRealloc)(StringDataA* d, size_t capacity);
  typedef StringDataW* (FOG_CDECL *StringW_DRealloc)(StringDataW* d, size_t capacity);

  typedef void (FOG_CDECL *StringA_DFree)(StringDataA* d);
  typedef void (FOG_CDECL *StringW_DFree)(StringDataW* d);

  struct FOG_NO_EXPORT _Api_StringA
  {
    StringA_Ctor ctor;
    StringA_CtorStubA ctorStubA;
    StringA_CtorStubA2 ctorStubA2;
    StringA_CtorCopyA ctorCopyA;
    StringA_CtorCopyA2 ctorCopyA2;
    StringA_CtorSubstr ctorSubstr;
    StringA_CtorU32 ctorU32;
    StringA_CtorU64 ctorU64;
    StringA_CtorDouble ctorDouble;
    StringA_Dtor dtor;

    StringA_Detach detach;
    StringA_Reserve reserve;
    StringA_Resize resize;
    StringA_Truncate truncate;
    StringA_Squeeze squeeze;
    StringA_Prepare prepare;
    StringA_Add add;

    StringA_Clear clear;
    StringA_Reset reset;

    StringA_GetHashCode getHashCode;

    StringA_SetStubA setStubA;
    StringA_SetStringA setStringA;
    StringA_SetStringExA setStringExA;

    StringA_SetDeep setDeep;

    StringA_AppendStubA appendStubA;
    StringA_AppendStringA appendStringA;
    StringA_AppendStringExA appendStringExA;

    StringA_OpFill opFill;
    StringA_OpBool opBool;
    StringA_OpI32 opI32;
    StringA_OpI32Ex opI32Ex;
    StringA_OpU32 opU32;
    StringA_OpU32Ex opU32Ex;
    StringA_OpI64 opI64;
    StringA_OpI64Ex opI64Ex;
    StringA_OpU64 opU64;
    StringA_OpU64Ex opU64Ex;
    StringA_OpDouble opDouble;
    StringA_OpDoubleEx opDoubleEx;

    StringA_OpVFormatStubA opVFormatStubA;
    StringA_OpVFormatStringA opVFormatStringA;

    StringA_OpZFormatStubA opZFormatStubA;
    StringA_OpZFormatStringA opZFormatStringA;

    StringA_OpNormalizeSlashesA opNormalizeSlashesA;

    StringA_PrependChars prependChars;
    StringA_PrependStubA prependStubA;
    StringA_PrependStringA prependStringA;

    StringA_InsertChars insertChars;
    StringA_InsertStubA insertStubA;
    StringA_InsertStringA insertStringA;

    StringA_RemoveRange removeRange;
    StringA_RemoveRangeList removeRangeList;

    StringA_RemoveChar removeChar;
    StringA_RemoveStubA removeStubA;
    StringA_RemoveStringA removeStringA;
    StringA_RemoveRegExpA removeRegExpA;

    StringA_ReplaceRangeStubA replaceRangeStubA;
    StringA_ReplaceRangeStringA replaceRangeStringA;

    StringA_ReplaceRangeListStubA replaceRangeListStubA;
    StringA_ReplaceRangeListStringA replaceRangeListStringA;

    StringA_ReplaceChar replaceChar;
    StringA_ReplaceStringA replaceStringA;
    StringA_ReplaceRegExpA replaceRegExpA;

    StringA_Lower lower;
    StringA_Upper upper;

    StringA_Trim trim;
    StringA_Simplify simplify;
    StringA_Justify justify;

    StringA_SplitChar splitChar;
    StringA_SplitStringA splitStringA;
    StringA_SplitRegExpA splitRegExpA;

    StringA_Slice slice;

    StringA_JoinChar joinChar;
    StringA_JoinStringA joinStringA;

    StringA_ParseBool parseBool;
    StringA_ParseI8 parseI8;
    StringA_ParseU8 parseU8;
    StringA_ParseI16 parseI16;
    StringA_ParseU16 parseU16;
    StringA_ParseI32 parseI32;
    StringA_ParseU32 parseU32;
    StringA_ParseI64 parseI64;
    StringA_ParseU64 parseU64;
    StringA_ParseFloat parseFloat;
    StringA_ParseDouble parseDouble;

    StringA_CountOfChar countOfChar;
    StringA_CountOfStubA countOfStubA;
    StringA_CountOfStringA countOfStringA;
    StringA_CountOfRegExpA countOfRegExpA;

    StringA_IndexOfChar indexOfChar;
    StringA_IndexOfStubA indexOfStubA;
    StringA_IndexOfStringA indexOfStringA;
    StringA_IndexOfRegExpA indexOfRegExpA;
    StringA_IndexOfAnyCharA indexOfAnyCharA;

    StringA_LastIndexOfChar lastIndexOfChar;
    StringA_LastIndexOfStubA lastIndexOfStubA;
    StringA_LastIndexOfStringA lastIndexOfStringA;
    StringA_LastIndexOfRegExpA lastIndexOfRegExpA;
    StringA_LastIndexOfAnyCharA lastIndexOfAnyCharA;

    StringA_StartsWithChar startsWithChar;
    StringA_StartsWithStubA startsWithStubA;
    StringA_StartsWithStringA startsWithStringA;
    StringA_StartsWithRegExpA startsWithRegExpA;

    StringA_EndsWithChar endsWithChar;
    StringA_EndsWithStubA endsWithStubA;
    StringA_EndsWithStringA endsWithStringA;
    StringA_EndsWithRegExpA endsWithRegExpA;

    StringA_EqStubA eqStubA;
    StringA_EqStringA eqStringA;

    StringA_EqStubExA eqStubExA;
    StringA_EqStringExA eqStringExA;

    StringA_CompareStubA compareStubA;
    StringA_CompareStringA compareStringA;

    StringA_CompareStubExA compareStubExA;
    StringA_CompareStringExA compareStringExA;

    StringA_ValidateUtf8 validateUtf8;
    StringA_GetUcsLength getUcsLength;

    StringA_HexDecode hexDecode;
    StringA_HexEncode hexEncode;

    StringA_Base64DecodeStringA base64DecodeStringA;
    StringA_Base64DecodeStringW base64DecodeStringW;
    StringA_Base64DecodeDataA base64DecodeDataA;
    StringA_Base64DecodeDataW base64DecodeDataW;
    StringA_Base64EncodeStringA base64EncodeStringA;
    StringA_Base64EncodeDataA base64EncodeDataA;

    StringA_DCreate dCreate;
    StringA_DCreateStubA dCreateStubA;
    StringA_DAdopt dAdopt;
    StringA_DAdoptStubA dAdoptStubA;
    StringA_DRealloc dRealloc;
    StringA_DFree dFree;

    StringA* oEmpty;
  } stringa;

  struct FOG_NO_EXPORT _Api_StringW
  {
    StringW_Ctor ctor;
    StringW_CtorStubA ctorStubA;
    StringW_CtorStubW ctorStubW;
    StringW_CtorStubA2 ctorStubA2;
    StringW_CtorStubW2 ctorStubW2;
    StringW_CtorCopyW ctorCopyW;
    StringW_CtorCopyW2 ctorCopyW2;
    StringW_CtorCodec ctorCodec;
    StringW_CtorSubstr ctorSubstr;
    StringW_CtorU32 ctorU32;
    StringW_CtorU64 ctorU64;
    StringW_CtorDouble ctorDouble;
    StringW_Dtor dtor;

    StringW_Detach detach;
    StringW_Reserve reserve;
    StringW_Resize resize;
    StringW_Truncate truncate;
    StringW_Squeeze squeeze;
    StringW_Prepare prepare;
    StringW_Add add;

    StringW_Clear clear;
    StringW_Reset reset;

    StringW_GetHashCode getHashCode;

    StringW_SetStubA setStubA;
    StringW_SetStubW setStubW;
    StringW_SetStringA setStringA;
    StringW_SetStringW setStringW;
    StringW_SetStringExW setStringExW;

    StringW_SetDeep setDeep;

    StringW_AppendStubA appendStubA;
    StringW_AppendStubW appendStubW;
    StringW_AppendStringA appendStringA;
    StringW_AppendStringW appendStringW;
    StringW_AppendStringExW appendStringExW;

    StringW_OpFill opFill;
    StringW_OpBool opBool;
    StringW_OpI32 opI32;
    StringW_OpI32Ex opI32Ex;
    StringW_OpU32 opU32;
    StringW_OpU32Ex opU32Ex;
    StringW_OpI64 opI64;
    StringW_OpI64Ex opI64Ex;
    StringW_OpU64 opU64;
    StringW_OpU64Ex opU64Ex;
    StringW_OpDouble opDouble;
    StringW_OpDoubleEx opDoubleEx;

    StringW_OpVFormatStubA opVFormatStubA;
    StringW_OpVFormatStubW opVFormatStubW;
    StringW_OpVFormatStringW opVFormatStringW;

    StringW_OpZFormatStubW opZFormatStubW;
    StringW_OpZFormatStringW opZFormatStringW;

    StringW_OpNormalizeSlashesW opNormalizeSlashesW;

    StringW_PrependChars prependChars;
    StringW_PrependStubA prependStubA;
    StringW_PrependStubW prependStubW;
    StringW_PrependStringA prependStringA;
    StringW_PrependStringW prependStringW;

    StringW_InsertChars insertChars;
    StringW_InsertStubA insertStubA;
    StringW_InsertStubW insertStubW;
    StringW_InsertStringA insertStringA;
    StringW_InsertStringW insertStringW;

    StringW_RemoveRange removeRange;
    StringW_RemoveRangeList removeRangeList;

    StringW_RemoveChar removeChar;
    StringW_RemoveStubA removeStubA;
    StringW_RemoveStubW removeStubW;
    StringW_RemoveStringW removeStringW;
    StringW_RemoveRegExpW removeRegExpW;

    StringW_ReplaceRangeStubW replaceRangeStubW;
    StringW_ReplaceRangeStringW replaceRangeStringW;

    StringW_ReplaceRangeListStubW replaceRangeListStubW;
    StringW_ReplaceRangeListStringW replaceRangeListStringW;

    StringW_ReplaceChar replaceChar;
    StringW_ReplaceStringW replaceStringW;
    StringW_ReplaceRegExpW replaceRegExpW;

    StringW_Lower lower;
    StringW_Upper upper;

    StringW_Trim trim;
    StringW_Simplify simplify;
    StringW_Justify justify;

    StringW_SplitChar splitChar;
    StringW_SplitStringW splitStringW;
    StringW_SplitRegExpW splitRegExpW;

    StringW_Slice slice;

    StringW_JoinChar joinChar;
    StringW_JoinStringW joinStringW;

    StringW_ParseBool parseBool;
    StringW_ParseI8 parseI8;
    StringW_ParseU8 parseU8;
    StringW_ParseI16 parseI16;
    StringW_ParseU16 parseU16;
    StringW_ParseI32 parseI32;
    StringW_ParseU32 parseU32;
    StringW_ParseI64 parseI64;
    StringW_ParseU64 parseU64;
    StringW_ParseFloat parseFloat;
    StringW_ParseDouble parseDouble;

    StringW_CountOfChar countOfChar;
    StringW_CountOfStubA countOfStubA;
    StringW_CountOfStubW countOfStubW;
    StringW_CountOfStringW countOfStringW;
    StringW_CountOfRegExpW countOfRegExpW;

    StringW_IndexOfChar indexOfChar;
    StringW_IndexOfStubA indexOfStubA;
    StringW_IndexOfStubW indexOfStubW;
    StringW_IndexOfStringW indexOfStringW;
    StringW_IndexOfRegExpW indexOfRegExpW;
    StringW_IndexOfAnyCharW indexOfAnyCharW;

    StringW_LastIndexOfChar lastIndexOfChar;
    StringW_LastIndexOfStubA lastIndexOfStubA;
    StringW_LastIndexOfStubW lastIndexOfStubW;
    StringW_LastIndexOfStringW lastIndexOfStringW;
    StringW_LastIndexOfRegExpW lastIndexOfRegExpW;
    StringW_LastIndexOfAnyCharW lastIndexOfAnyCharW;

    StringW_StartsWithChar startsWithChar;
    StringW_StartsWithStubA startsWithStubA;
    StringW_StartsWithStubW startsWithStubW;
    StringW_StartsWithStringW startsWithStringW;
    StringW_StartsWithRegExpW startsWithRegExpW;

    StringW_EndsWithChar endsWithChar;
    StringW_EndsWithStubA endsWithStubA;
    StringW_EndsWithStubW endsWithStubW;
    StringW_EndsWithStringW endsWithStringW;
    StringW_EndsWithRegExpW endsWithRegExpW;

    StringW_EqStubA eqStubA;
    StringW_EqStubW eqStubW;
    StringW_EqStringW eqStringW;

    StringW_EqStubExA eqStubExA;
    StringW_EqStubExW eqStubExW;
    StringW_EqStringExW eqStringExW;

    StringW_CompareStubA compareStubA;
    StringW_CompareStubW compareStubW;
    StringW_CompareStringW compareStringW;

    StringW_CompareStubExA compareStubExA;
    StringW_CompareStubExW compareStubExW;
    StringW_CompareStringExW compareStringExW;

    StringW_ValidateUtf16 validateUtf16;
    StringW_GetUcsLength getUcsLength;

    StringW_Base64EncodeStringA base64EncodeStringA;
    StringW_Base64EncodeDataA base64EncodeDataA;

    StringW_BSwap bswap;

    StringW_DCreate dCreate;
    StringW_DCreateStubA dCreateStubA;
    StringW_DCreateStubW dCreateStubW;
    StringW_DAdopt dAdopt;
    StringW_DAdoptStubA dAdoptStubA;
    StringW_DAdoptStubW dAdoptStubW;
    StringW_DRealloc dRealloc;
    StringW_DFree dFree;

    StringW* oEmpty;
  } stringw;

  // --------------------------------------------------------------------------
  // [Core/Tools - StringUtil]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *StringUtil_CopyA)(char* dst, const char* src, size_t length);
  typedef void (FOG_CDECL *StringUtil_CopyW)(CharW* dst, const CharW* src, size_t length);

  typedef err_t (FOG_CDECL *StringUtil_LatinFromUnicode)(char* dst, const CharW* src, size_t length);
  typedef void (FOG_CDECL *StringUtil_UnicodeFromLatin)(CharW* dst, const char* src, size_t length);

  typedef void (FOG_CDECL *StringUtil_MoveA)(char* dst, const char* src, size_t length);
  typedef void (FOG_CDECL *StringUtil_MoveW)(CharW* dst, const CharW* src, size_t length);

  typedef void (FOG_CDECL *StringUtil_FillA)(char* dst, char ch, size_t length);
  typedef void (FOG_CDECL *StringUtil_FillW)(CharW* dst, uint16_t ch, size_t length);

  typedef size_t (FOG_CDECL* StringUtil_LenA)(const char* src);
  typedef size_t (FOG_CDECL* StringUtil_LenW)(const CharW* src);

  typedef size_t (FOG_CDECL* StringUtil_NLenA)(const char* src, size_t max);
  typedef size_t (FOG_CDECL* StringUtil_NLenW)(const CharW* src, size_t max);

  typedef bool (FOG_CDECL* StringUtil_EqA)(const char* a, const char* b, size_t length);
  typedef bool (FOG_CDECL* StringUtil_EqW)(const CharW* a, const CharW* b, size_t length);
  typedef bool (FOG_CDECL* StringUtil_EqMixed)(const CharW* a, const char* b, size_t length);

  typedef size_t (FOG_CDECL* StringUtil_CountOfA)(const char* str, size_t length, char ch);
  typedef size_t (FOG_CDECL* StringUtil_CountOfW)(const CharW* str, size_t length, uint16_t ch);

  typedef size_t (FOG_CDECL* StringUtil_IndexOfCharA)(const char* str, size_t length, char ch);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfCharW)(const CharW* str, size_t length, uint16_t ch);

  typedef size_t (FOG_CDECL* StringUtil_IndexOfStringA)(const char* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfStringWA)(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfStringW)(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength);

  typedef size_t (FOG_CDECL* StringUtil_IndexOfAnyA)(const char* str, size_t length, const char* charArray, size_t charLength);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfAnyW)(const CharW* str, size_t length, const CharW* charArray, size_t charLength);

  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfCharA)(const char* str, size_t length, char ch);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfCharW)(const CharW* str, size_t length, uint16_t ch);

  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfStringA)(const char* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfStringWA)(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfStringW)(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength);

  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfAnyA)(const char* str, size_t length, const char* charArray, size_t charLength);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfAnyW)(const CharW* str, size_t length, const CharW* charArray, size_t charLength);

  typedef err_t (FOG_CDECL *StringUtil_ValidateUtf8)(const char* data, size_t length, size_t* invalid);
  typedef err_t (FOG_CDECL *StringUtil_ValidateUtf16)(const CharW* data, size_t length, size_t* invalid);

  typedef err_t (FOG_CDECL *StringUtil_UcsFromUtf8Length)(const char* data, size_t length, size_t* ucsLength);
  typedef err_t (FOG_CDECL *StringUtil_UcsFromUtf16Length)(const CharW* data, size_t length, size_t* ucsLength);

  typedef void (FOG_CDECL *StringUtil_Itoa)(NTOAContext* ctx, int64_t n, uint32_t base, uint32_t textCase);
  typedef void (FOG_CDECL *StringUtil_Utoa)(NTOAContext* ctx, uint64_t n, uint32_t base, uint32_t textCase);
  typedef void (FOG_CDECL *StringUtil_Dtoa)(NTOAContext* ctx, double d, uint32_t mode, int nDigits);

  typedef err_t (FOG_CDECL *StringUtil_ParseBoolA)(bool* dst, const char* src, size_t length, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseBoolW)(bool* dst, const CharW* src, size_t length, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseI8A)(int8_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseI8W)(int8_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseU8A)(uint8_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseU8W)(uint8_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseI16A)(int16_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseI16W)(int16_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseU16A)(uint16_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseU16W)(uint16_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseI32A)(int32_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseI32W)(int32_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseU32A)(uint32_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseU32W)(uint32_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseI64A)(int64_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseI64W)(int64_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseU64A)(uint64_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseU64W)(uint64_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseFloatA)(float* dst, const char* src, size_t length, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseFloatW)(float* dst, const CharW* src, size_t length, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);

  typedef err_t (FOG_CDECL *StringUtil_ParseDoubleA)(double* dst, const char* src, size_t length, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  typedef err_t (FOG_CDECL *StringUtil_ParseDoubleW)(double* dst, const CharW* src, size_t length, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);

  struct FOG_NO_EXPORT _Api_StringUtil
  {
    StringUtil_CopyA copyA;
    StringUtil_CopyW copyW;

    StringUtil_LatinFromUnicode latinFromUnicode;
    StringUtil_UnicodeFromLatin unicodeFromLatin;

    StringUtil_MoveA moveA;
    StringUtil_MoveW moveW;

    StringUtil_FillA fillA;
    StringUtil_FillW fillW;

    StringUtil_LenA lenA;
    StringUtil_LenW lenW;

    StringUtil_NLenA nLenA;
    StringUtil_NLenW nLenW;

    StringUtil_EqA eqA[CASE_SENSITIVITY_COUNT];
    StringUtil_EqW eqW[CASE_SENSITIVITY_COUNT];
    StringUtil_EqMixed eqMixed[CASE_SENSITIVITY_COUNT];

    StringUtil_CountOfA countOfA[CASE_SENSITIVITY_COUNT];
    StringUtil_CountOfW countOfW[CASE_SENSITIVITY_COUNT];

    StringUtil_IndexOfCharA indexOfCharA[CASE_SENSITIVITY_COUNT];
    StringUtil_IndexOfCharW indexOfCharW[CASE_SENSITIVITY_COUNT];

    StringUtil_IndexOfStringA indexOfStringA[CASE_SENSITIVITY_COUNT];
    StringUtil_IndexOfStringWA indexOfStringWA[CASE_SENSITIVITY_COUNT];
    StringUtil_IndexOfStringW indexOfStringW[CASE_SENSITIVITY_COUNT];

    StringUtil_IndexOfAnyA indexOfAnyA[CASE_SENSITIVITY_COUNT];
    StringUtil_IndexOfAnyW indexOfAnyW[CASE_SENSITIVITY_COUNT];

    StringUtil_LastIndexOfCharA lastIndexOfCharA[CASE_SENSITIVITY_COUNT];
    StringUtil_LastIndexOfCharW lastIndexOfCharW[CASE_SENSITIVITY_COUNT];

    StringUtil_LastIndexOfStringA lastIndexOfStringA[CASE_SENSITIVITY_COUNT];
    StringUtil_LastIndexOfStringWA lastIndexOfStringWA[CASE_SENSITIVITY_COUNT];
    StringUtil_LastIndexOfStringW lastIndexOfStringW[CASE_SENSITIVITY_COUNT];

    StringUtil_LastIndexOfAnyA lastIndexOfAnyA[CASE_SENSITIVITY_COUNT];
    StringUtil_LastIndexOfAnyW lastIndexOfAnyW[CASE_SENSITIVITY_COUNT];

    StringUtil_ValidateUtf8 validateUtf8;
    StringUtil_ValidateUtf16 validateUtf16;

    StringUtil_UcsFromUtf8Length ucsFromUtf8Length;
    StringUtil_UcsFromUtf16Length ucsFromUtf16Length;

    StringUtil_Itoa itoa;
    StringUtil_Utoa utoa;
    StringUtil_Dtoa dtoa;

    StringUtil_ParseBoolA parseBoolA;
    StringUtil_ParseBoolW parseBoolW;

    StringUtil_ParseI8A parseI8A;
    StringUtil_ParseI8W parseI8W;

    StringUtil_ParseU8A parseU8A;
    StringUtil_ParseU8W parseU8W;

    StringUtil_ParseI16A parseI16A;
    StringUtil_ParseI16W parseI16W;

    StringUtil_ParseU16A parseU16A;
    StringUtil_ParseU16W parseU16W;

    StringUtil_ParseI32A parseI32A;
    StringUtil_ParseI32W parseI32W;

    StringUtil_ParseU32A parseU32A;
    StringUtil_ParseU32W parseU32W;

    StringUtil_ParseI64A parseI64A;
    StringUtil_ParseI64W parseI64W;

    StringUtil_ParseU64A parseU64A;
    StringUtil_ParseU64W parseU64W;

    StringUtil_ParseFloatA parseFloatA;
    StringUtil_ParseFloatW parseFloatW;

    StringUtil_ParseDoubleA parseDoubleA;
    StringUtil_ParseDoubleW parseDoubleW;
  } stringutil;

  // --------------------------------------------------------------------------
  // [Core/Tools - TextCodec]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *TextCodec_Ctor)(TextCodec* self);
  typedef void (FOG_CDECL *TextCodec_CtorCopy)(TextCodec* self, const TextCodec* other);
  typedef void (FOG_CDECL *TextCodec_Dtor)(TextCodec* self);

  typedef err_t (FOG_CDECL *TextCodec_CreateFromCode)(TextCodec* self, uint32_t code);
  typedef err_t (FOG_CDECL *TextCodec_CreateFromMimeStubA)(TextCodec* self, const StubA* mime);
  typedef err_t (FOG_CDECL *TextCodec_CreateFromMimeStringW)(TextCodec* self, const StringW* mime);
  typedef err_t (FOG_CDECL *TextCodec_CreateFromBom)(TextCodec* self, const void* data, size_t length);

  typedef void (FOG_CDECL *TextCodec_Reset)(TextCodec* self);
  typedef err_t (FOG_CDECL *TextCodec_Copy)(TextCodec* self, const TextCodec* other);

  typedef err_t (FOG_CDECL *TextCodec_EncodeStubW)(const TextCodec* self, StringA* dst, const StubW* src, TextCodecState* state, TextCodecHandler* handler, uint32_t cntOp);
  typedef err_t (FOG_CDECL *TextCodec_EncodeStringW)(const TextCodec* self, StringA* dst, const StringW* src, TextCodecState* state, TextCodecHandler* handler, uint32_t cntOp);

  typedef err_t (FOG_CDECL *TextCodec_DecodeStubA)(const TextCodec* self, StringW* dst, const StubA* src, TextCodecState* state, uint32_t cntOp);
  typedef err_t (FOG_CDECL *TextCodec_DecodeStringA)(const TextCodec* self, StringW* dst, const StringA* src, TextCodecState* state, uint32_t cntOp);

  struct FOG_NO_EXPORT _Api_TextCodec
  {
    TextCodec_Ctor ctor;
    TextCodec_CtorCopy ctorCopy;
    TextCodec_Dtor dtor;

    TextCodec_CreateFromCode createFromCode;
    TextCodec_CreateFromMimeStubA createFromMimeStubA;
    TextCodec_CreateFromMimeStringW createFromMimeStringW;
    TextCodec_CreateFromBom createFromBom;

    TextCodec_Reset reset;
    TextCodec_Copy copy;

    TextCodec_EncodeStubW encodeStubW;
    TextCodec_EncodeStringW encodeStringW;

    TextCodec_DecodeStubA decodeStubA;
    TextCodec_DecodeStringA decodeStringA;

    TextCodec* oCache[TEXT_CODEC_CACHE_COUNT];
  } textcodec;

  // --------------------------------------------------------------------------
  // [Core/Tools - Time]
  // --------------------------------------------------------------------------

  typedef int64_t (FOG_CDECL* Time_Now)(void);
  typedef time_t (FOG_CDECL* Time_ToTimeT)(int64_t us);
  typedef int64_t (FOG_CDECL* Time_FromTimeT)(time_t t);

  struct FOG_NO_EXPORT _Api_Time
  {
    Time_Now now;
    Time_ToTimeT toTimeT;
    Time_FromTimeT fromTimeT;
  } time;

  // --------------------------------------------------------------------------
  // [Core/Tools - TimeTicks]
  // --------------------------------------------------------------------------

  typedef int64_t (FOG_CDECL* TimeTicks_Now)(uint32_t ticksPrecision);

  struct FOG_NO_EXPORT _Api_TimeTicks
  {
    TimeTicks_Now now;
  } timeticks;

  // --------------------------------------------------------------------------
  // [Core/Tools - Var]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Var_Ctor)(Var* self);
  typedef void (FOG_CDECL *Var_CtorCopy)(Var* self, const Var* other);
  typedef void (FOG_CDECL *Var_CtorType)(Var* self, uint32_t vType, const void* vData);
  typedef void (FOG_CDECL *Var_Dtor)(Var* self);

  typedef size_t (FOG_CDECL *Var_GetReference)(const Var* self);
  typedef uint32_t (FOG_CDECL *Var_GetTypeId)(const Var* self);

  typedef void (FOG_CDECL *Var_Reset)(Var* self);
  typedef void (FOG_CDECL *Var_Copy)(Var* self, const Var* other);

  typedef err_t (FOG_CDECL *Var_GetI32)(const Var* self, int32_t* dst);
  typedef err_t (FOG_CDECL *Var_GetI32Bound)(const Var* self, int32_t* dst, int32_t min, int32_t max);

  typedef err_t (FOG_CDECL *Var_GetU32)(const Var* self, uint32_t* dst);
  typedef err_t (FOG_CDECL *Var_GetU32Bound)(const Var* self, uint32_t* dst, uint32_t min, uint32_t max);

  typedef err_t (FOG_CDECL *Var_GetI64)(const Var* self, int64_t* dst);
  typedef err_t (FOG_CDECL *Var_GetI64Bound)(const Var* self, int64_t* dst, int64_t min, int64_t max);

  typedef err_t (FOG_CDECL *Var_GetU64)(const Var* self, uint64_t* dst);
  typedef err_t (FOG_CDECL *Var_GetU64Bound)(const Var* self, uint64_t* dst, uint64_t min, uint64_t max);

  typedef err_t (FOG_CDECL *Var_GetFloat)(const Var* self, float* dst);
  typedef err_t (FOG_CDECL *Var_GetFloatBound)(const Var* self, float* dst, float min, float max);

  typedef err_t (FOG_CDECL *Var_GetDouble)(const Var* self, double* dst);
  typedef err_t (FOG_CDECL *Var_GetDoubleBound)(const Var* self, double* dst, double min, double max);

  typedef err_t (FOG_CDECL *Var_GetType)(const Var* self, uint32_t vType, void* vData);
  typedef err_t (FOG_CDECL *Var_SetType)(Var* self, uint32_t vType, const void* vData);

  typedef bool (FOG_CDECL *Var_Eq)(const Var* a, const Var* b);
  typedef int (FOG_CDECL *Var_Compare)(const Var* a, const Var* b);

  typedef VarData* (FOG_CDECL *Var_DCreate)(size_t dataSize);
  typedef VarData* (FOG_CDECL *Var_DAddRef)(VarData* d);
  typedef void (FOG_CDECL *Var_DRelease)(VarData* d);

  struct FOG_NO_EXPORT _Api_Var
  {
    Var_Ctor ctor;
    Var_CtorCopy ctorCopy;
    Var_CtorType ctorType;
    Var_Dtor dtor;

    Var_GetReference getReference;
    Var_GetTypeId getTypeId;

    Var_Reset reset;
    Var_Copy copy;

    Var_GetI32 getI32;
    Var_GetI32Bound getI32Bound;

    Var_GetU32 getU32;
    Var_GetU32Bound getU32Bound;

    Var_GetI64 getI64;
    Var_GetI64Bound getI64Bound;

    Var_GetU64 getU64;
    Var_GetU64Bound getU64Bound;

    Var_GetFloat getFloat;
    Var_GetFloatBound getFloatBound;

    Var_GetDouble getDouble;
    Var_GetDoubleBound getDoubleBound;

    Var_GetType getType;
    Var_SetType setType;

    Var_Eq eq;
    Var_Compare compare;

    Var_DCreate dCreate;
    Var_DAddRef dAddRef;
    Var_DRelease dRelease;

    Var* oNull;
  } var;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Arc]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *ArcF_GetBoundingBox)(const ArcF* self, BoxF* dst, const TransformF* tr, bool includeCenterPoint);
  typedef err_t (FOG_CDECL *ArcD_GetBoundingBox)(const ArcD* self, BoxD* dst, const TransformD* tr, bool includeCenterPoint);

  typedef uint (FOG_CDECL *ArcF_ToCSpline)(const ArcF* self, PointF* pts);
  typedef uint (FOG_CDECL *ArcD_ToCSpline)(const ArcD* self, PointD* pts);

  struct FOG_NO_EXPORT _Api_ArcF
  {
    ArcF_GetBoundingBox getBoundingBox;
    ArcF_ToCSpline toCSpline;
  } arcf;

  struct FOG_NO_EXPORT _Api_ArcD
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

  typedef int (FOG_CDECL *CBezierF_SimplifyForProcessing)(const PointF* self, PointF* pts);
  typedef int (FOG_CDECL *CBezierD_SimplifyForProcessing)(const PointD* self, PointD* pts);

  typedef err_t (FOG_CDECL *CBezierF_Flatten)(const PointF* self, PathF& dst, uint8_t initialCommand, float flatness);
  typedef err_t (FOG_CDECL *CBezierD_Flatten)(const PointD* self, PathD& dst, uint8_t initialCommand, double flatness);

  struct FOG_NO_EXPORT _Api_CBezierF
  {
    CBezierF_GetBoundingBox getBoundingBox;
    CBezierF_GetSplineBBox getSplineBBox;
    CBezierF_GetLength getLength;
    CBezierF_GetInflectionPoints getInflectionPoints;
    CBezierF_SimplifyForProcessing simplifyForProcessing;
    CBezierF_Flatten flatten;
  } cbezierf;

  struct FOG_NO_EXPORT _Api_CBezierD
  {
    CBezierD_GetBoundingBox getBoundingBox;
    CBezierD_GetSplineBBox getSplineBBox;
    CBezierD_GetLength getLength;
    CBezierD_GetInflectionPoints getInflectionPoints;
    CBezierD_SimplifyForProcessing simplifyForProcessing;
    CBezierD_Flatten flatten;
  } cbezierd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Chord]
  // --------------------------------------------------------------------------

  typedef bool (FOG_CDECL *ChordF_HitTest)(const ChordF* self, const PointF* pt);
  typedef bool (FOG_CDECL *ChordD_HitTest)(const ChordD* self, const PointD* pt);

  struct FOG_NO_EXPORT _Api_ChordF
  {
    ChordF_HitTest hitTest;
  } chordf;

  struct FOG_NO_EXPORT _Api_ChordD
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

  struct FOG_NO_EXPORT _Api_CircleF
  {
    CircleF_GetBoundingBox getBoundingBox;
    CircleF_HitTest hitTest;
    CircleF_ToCSpline toCSpline;
  } circlef;

  struct FOG_NO_EXPORT _Api_CircleD
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

  struct FOG_NO_EXPORT _Api_EllipseF
  {
    EllipseF_GetBoundingBox getBoundingBox;
    EllipseF_HitTest hitTest;
    EllipseF_ToCSpline toCSpline;
  } ellipsef;

  struct FOG_NO_EXPORT _Api_EllipseD
  {
    EllipseD_GetBoundingBox getBoundingBox;
    EllipseD_HitTest hitTest;
    EllipseD_ToCSpline toCSpline;
  } ellipsed;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Line]
  // --------------------------------------------------------------------------

  typedef uint32_t (FOG_CDECL *LineF_Intersect)(PointF* dst, const PointF* lineA, const PointF* lineB);
  typedef uint32_t (FOG_CDECL *LineD_Intersect)(PointD* dst, const PointD* lineA, const PointD* lineB);

  typedef float (FOG_CDECL* LineF_PolyAngle)(const PointF* pts);
  typedef double (FOG_CDECL* LineD_PolyAngle)(const PointD* pts);

  struct FOG_NO_EXPORT _Api_LineF
  {
    LineF_Intersect intersect;
    LineF_PolyAngle polyAngle;
  } linef;

  struct FOG_NO_EXPORT _Api_LineD
  {
    LineD_Intersect intersect;
    LineD_PolyAngle polyAngle;
  } lined;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Path]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *PathF_Ctor)(PathF* self);
  typedef void (FOG_CDECL *PathD_Ctor)(PathD* self);

  typedef void (FOG_CDECL *PathF_CtorCopyF)(PathF* self, const PathF* other);
  typedef void (FOG_CDECL *PathD_CtorCopyD)(PathD* self, const PathD* other);

  typedef void (FOG_CDECL *PathF_Dtor)(PathF* self);
  typedef void (FOG_CDECL *PathD_Dtor)(PathD* self);

  typedef err_t (FOG_CDECL *PathF_Detach)(PathF* self);
  typedef err_t (FOG_CDECL *PathD_Detach)(PathD* self);

  typedef err_t (FOG_CDECL *PathF_Reserve)(PathF* self, size_t capacity);
  typedef err_t (FOG_CDECL *PathD_Reserve)(PathD* self, size_t capacity);

  typedef void (FOG_CDECL *PathF_Squeeze)(PathF* self);
  typedef void (FOG_CDECL *PathD_Squeeze)(PathD* self);

  typedef size_t (FOG_CDECL *PathF_Prepare)(PathF* self, uint32_t cntOp, size_t count);
  typedef size_t (FOG_CDECL *PathD_Prepare)(PathD* self, uint32_t cntOp, size_t count);

  typedef size_t (FOG_CDECL *PathF_Add)(PathF* self, size_t count);
  typedef size_t (FOG_CDECL *PathD_Add)(PathD* self, size_t count);

  typedef void (FOG_CDECL *PathF_UpdateFlat)(const PathF* self);
  typedef void (FOG_CDECL *PathD_UpdateFlat)(const PathD* self);

  typedef void (FOG_CDECL *PathF_Clear)(PathF* self);
  typedef void (FOG_CDECL *PathD_Clear)(PathD* self);

  typedef void (FOG_CDECL *PathF_Reset)(PathF* self);
  typedef void (FOG_CDECL *PathD_Reset)(PathD* self);

  typedef err_t (FOG_CDECL *PathF_SetPathF)(PathF* self, const PathF* other);
  typedef err_t (FOG_CDECL *PathD_SetPathF)(PathD* self, const PathF* other);
  typedef err_t (FOG_CDECL *PathD_SetPathD)(PathD* self, const PathD* other);

  typedef err_t (FOG_CDECL *PathF_SetDeepF)(PathF* self, const PathF* other);
  typedef err_t (FOG_CDECL *PathD_SetDeepD)(PathD* self, const PathD* other);

  typedef err_t (FOG_CDECL *PathF_GetLastVertex)(const PathF* self, PointF* dst);
  typedef err_t (FOG_CDECL *PathD_GetLastVertex)(const PathD* self, PointD* dst);

  typedef err_t (FOG_CDECL *PathF_SetVertex)(PathF* self, size_t index, const PointF* pt);
  typedef err_t (FOG_CDECL *PathD_SetVertex)(PathD* self, size_t index, const PointD* pt);

  typedef Range (FOG_CDECL *PathF_GetSubpathRange)(const PathF* self, size_t index);
  typedef Range (FOG_CDECL *PathD_GetSubpathRange)(const PathD* self, size_t index);

  typedef err_t (FOG_CDECL *PathF_MoveTo)(PathF* self, const PointF* pt0);
  typedef err_t (FOG_CDECL *PathD_MoveTo)(PathD* self, const PointD* pt0);

  typedef err_t (FOG_CDECL *PathF_LineTo)(PathF* self, const PointF* pt0);
  typedef err_t (FOG_CDECL *PathD_LineTo)(PathD* self, const PointD* pt0);

  typedef err_t (FOG_CDECL *PathF_HLineTo)(PathF* self, float x);
  typedef err_t (FOG_CDECL *PathD_HLineTo)(PathD* self, double x);

  typedef err_t (FOG_CDECL *PathF_VLineTo)(PathF* self, float y);
  typedef err_t (FOG_CDECL *PathD_VLineTo)(PathD* self, double y);

  typedef err_t (FOG_CDECL *PathF_PolyTo)(PathF* self, const PointF* pts, size_t count);
  typedef err_t (FOG_CDECL *PathD_PolyTo)(PathD* self, const PointD* pts, size_t count);

  typedef err_t (FOG_CDECL *PathF_QuadTo)(PathF* self, const PointF* pt1, const PointF* pt2);
  typedef err_t (FOG_CDECL *PathD_QuadTo)(PathD* self, const PointD* pt1, const PointD* pt2);

  typedef err_t (FOG_CDECL *PathF_CubicTo)(PathF* self, const PointF* pt1, const PointF* pt2, const PointF* pt3);
  typedef err_t (FOG_CDECL *PathD_CubicTo)(PathD* self, const PointD* pt1, const PointD* pt2, const PointD* pt3);

  typedef err_t (FOG_CDECL *PathF_SmoothQuadTo)(PathF* self, const PointF* pt2);
  typedef err_t (FOG_CDECL *PathD_SmoothQuadTo)(PathD* self, const PointD* pt2);

  typedef err_t (FOG_CDECL *PathF_SmoothCubicTo)(PathF* self, const PointF* pt1, const PointF* pt2);
  typedef err_t (FOG_CDECL *PathD_SmoothCubicTo)(PathD* self, const PointD* pt1, const PointD* pt2);

  typedef err_t (FOG_CDECL *PathF_ArcTo)(PathF* self, const PointF* cp, const PointF* rp, float start, float sweep, bool startPath);
  typedef err_t (FOG_CDECL *PathD_ArcTo)(PathD* self, const PointD* cp, const PointD* rp, double start, double sweep, bool startPath);

  typedef err_t (FOG_CDECL *PathF_SvgArcTo)(PathF* self, const PointF* rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF* pt);
  typedef err_t (FOG_CDECL *PathD_SvgArcTo)(PathD* self, const PointD* rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD* pt);

  typedef err_t (FOG_CDECL *PathF_Close)(PathF* self);
  typedef err_t (FOG_CDECL *PathD_Close)(PathD* self);

  typedef err_t (FOG_CDECL *PathF_BoxI)(PathF* self, const BoxI* r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxI)(PathD* self, const BoxI* r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_BoxF)(PathF* self, const BoxF* r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxF)(PathD* self, const BoxF* r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxD)(PathD* self, const BoxD* r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectI)(PathF* self, const RectI* r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectI)(PathD* self, const RectI* r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectF)(PathF* self, const RectF* r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectF)(PathD* self, const RectF* r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectD)(PathD* self, const RectD* r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_BoxesI)(PathF* self, const BoxI* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxesI)(PathD* self, const BoxI* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_BoxesF)(PathF* self, const BoxF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxesF)(PathD* self, const BoxF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_BoxesD)(PathD* self, const BoxD* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectsI)(PathF* self, const RectI* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectsI)(PathD* self, const RectI* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_RectsF)(PathF* self, const RectF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectsF)(PathD* self, const RectF* r, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_RectsD)(PathD* self, const RectD* r, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_Region)(PathF* self, const Region* r, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_Region)(PathD* self, const Region* r, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolylineI)(PathF* self, const PointI* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolylineI)(PathD* self, const PointI* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolylineF)(PathF* self, const PointF* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolylineD)(PathD* self, const PointD* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolygonI)(PathF* self, const PointI* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolygonI)(PathD* self, const PointI* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_PolygonF)(PathF* self, const PointF* pts, size_t count, uint32_t direction);
  typedef err_t (FOG_CDECL *PathD_PolygonD)(PathD* self, const PointD* pts, size_t count, uint32_t direction);

  typedef err_t (FOG_CDECL *PathF_Shape)(PathF* self, uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformF* tr);
  typedef err_t (FOG_CDECL *PathD_Shape)(PathD* self, uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformD* tr);

  typedef err_t (FOG_CDECL *PathF_AppendPathF)(PathF* self, const PathF* path, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendPathF)(PathD* self, const PathF* path, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendPathD)(PathD* self, const PathD* path, const Range* range);

  typedef err_t (FOG_CDECL *PathF_AppendTranslatedPathF)(PathF* self, const PathF* path, const PointF* pt, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTranslatedPathF)(PathD* self, const PathF* path, const PointD* pt, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTranslatedPathD)(PathD* self, const PathD* path, const PointD* pt, const Range* range);

  typedef err_t (FOG_CDECL *PathF_AppendTransformedPathF)(PathF* self, const PathF* path, const TransformF* tr, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTransformedPathF)(PathD* self, const PathF* path, const TransformD* tr, const Range* range);
  typedef err_t (FOG_CDECL *PathD_AppendTransformedPathD)(PathD* self, const PathD* path, const TransformD* tr, const Range* range);

  typedef err_t (FOG_CDECL *PathF_GetBoundingBox)(const PathF* self, BoxF* dst, const TransformF* transform);
  typedef err_t (FOG_CDECL *PathD_GetBoundingBox)(const PathD* self, BoxD* dst, const TransformD* transform);

  typedef bool (FOG_CDECL *PathF_HitTest)(const PathF* self, const PointF* pt, uint32_t fillRule);
  typedef bool (FOG_CDECL *PathD_HitTest)(const PathD* self, const PointD* pt, uint32_t fillRule);

  typedef size_t (FOG_CDECL *PathF_GetClosestVertex)(const PathF* self, const PointF* pt, float maxDistance, float* distance);
  typedef size_t (FOG_CDECL *PathD_GetClosestVertex)(const PathD* self, const PointD* pt, double maxDistance, double* distance);

  typedef err_t (FOG_CDECL *PathF_Translate)(PathF* self, const PointF* pt, const Range* range);
  typedef err_t (FOG_CDECL *PathD_Translate)(PathD* self, const PointD* pt, const Range* range);

  typedef err_t (FOG_CDECL *PathF_Transform)(PathF* self, const TransformF* tr, const Range* range);
  typedef err_t (FOG_CDECL *PathD_Transform)(PathD* self, const TransformD* tr, const Range* range);

  typedef err_t (FOG_CDECL *PathF_FitTo)(PathF* self, const RectF* r);
  typedef err_t (FOG_CDECL *PathD_FitTo)(PathD* self, const RectD* r);

  typedef err_t (FOG_CDECL *PathF_Scale)(PathF* self, const PointF* pt, bool keepStartPos);
  typedef err_t (FOG_CDECL *PathD_Scale)(PathD* self, const PointD* pt, bool keepStartPos);

  typedef err_t (FOG_CDECL *PathF_FlipX)(PathF* self, float x0, float x1);
  typedef err_t (FOG_CDECL *PathD_FlipX)(PathD* self, double x0, double x1);

  typedef err_t (FOG_CDECL *PathF_FlipY)(PathF* self, float y0, float y1);
  typedef err_t (FOG_CDECL *PathD_FlipY)(PathD* self, double y0, double y1);

  typedef err_t (FOG_CDECL *PathF_Flatten)(PathF* dst, const PathF* src, const PathFlattenParamsF* params, const Range* range);
  typedef err_t (FOG_CDECL *PathD_Flatten)(PathD* dst, const PathD* src, const PathFlattenParamsD* params, const Range* range);

  typedef const PathInfoF* (FOG_CDECL *PathF_GetPathInfo)(const PathF* self);
  typedef const PathInfoD* (FOG_CDECL *PathD_GetPathInfo)(const PathD* self);

  typedef bool (FOG_CDECL *PathF_Eq)(const PathF* a, const PathF* b);
  typedef bool (FOG_CDECL *PathD_Eq)(const PathD* a, const PathD* b);

  typedef PathDataF* (FOG_CDECL *PathF_DCreate)(size_t capacity);
  typedef PathDataD* (FOG_CDECL *PathD_DCreate)(size_t capacity);

  typedef PathDataF* (FOG_CDECL *PathF_DAdopt)(void* address, size_t capacity);
  typedef PathDataD* (FOG_CDECL *PathD_DAdopt)(void* address, size_t capacity);

  typedef void (FOG_CDECL *PathF_DFree)(PathDataF* d);
  typedef void (FOG_CDECL *PathD_DFree)(PathDataD* d);

  struct FOG_NO_EXPORT _Api_PathF
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
    PathF_GetLastVertex getLastVertex;
    PathF_SetVertex setVertex;
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
    PathF_GetClosestVertex getClosestVertex;
    PathF_Translate translate;
    PathF_Transform transform;
    PathF_FitTo fitTo;
    PathF_Scale scale;
    PathF_FlipX flipX;
    PathF_FlipY flipY;
    PathF_GetPathInfo getPathInfo;
    PathF_Eq eq;

    PathF_DCreate dCreate;
    PathF_DAdopt dAdopt;
    PathF_DFree dFree;

    PathF* oEmpty;
  } pathf;

  struct FOG_NO_EXPORT _Api_PathD
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
    PathD_GetLastVertex getLastVertex;
    PathD_SetVertex setVertex;
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
    PathD_GetClosestVertex getClosestVertex;
    PathD_Translate translate;
    PathD_Transform transform;
    PathD_FitTo fitTo;
    PathD_Scale scale;
    PathD_FlipX flipX;
    PathD_FlipY flipY;
    PathD_GetPathInfo getPathInfo;
    PathD_Eq eq;

    PathD_DCreate dCreate;
    PathD_DAdopt dAdopt;
    PathD_DFree dFree;

    PathD* oEmpty;
  } pathd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathClipper]
  // --------------------------------------------------------------------------

  typedef uint32_t (FOG_CDECL *PathClipperF_MeasurePath)(PathClipperF* self, const PathF* src);
  typedef uint32_t (FOG_CDECL *PathClipperD_MeasurePath)(PathClipperD* self, const PathD* src);

  typedef err_t (FOG_CDECL *PathClipperF_ContinuePath)(PathClipperF* self, PathF* dst, const PathF* src);
  typedef err_t (FOG_CDECL *PathClipperD_ContinuePath)(PathClipperD* self, PathD* dst, const PathD* src);

  typedef err_t (FOG_CDECL *PathClipperF_ContinuePathData)(PathClipperF* self, PathF* dst, const PointF* srcPts, const uint8_t* srcCmd, size_t srcLength);
  typedef err_t (FOG_CDECL *PathClipperD_ContinuePathData)(PathClipperD* self, PathD* dst, const PointD* srcPts, const uint8_t* srcCmd, size_t srcLength);

  typedef err_t (FOG_CDECL *PathClipperF_ClipPath)(PathClipperF* self, PathF* dst, const PathF* src, const TransformF* tr);
  typedef err_t (FOG_CDECL *PathClipperD_ClipPath)(PathClipperD* self, PathD* dst, const PathD* src, const TransformD* tr);

  struct FOG_NO_EXPORT _Api_PathClipperF
  {
    PathClipperF_MeasurePath measurePath;
    PathClipperF_ContinuePath continuePath;
    PathClipperF_ContinuePathData continuePathData;
    PathClipperF_ClipPath clipPath;
  } pathclipperf;

  struct FOG_NO_EXPORT _Api_PathClipperD
  {
    PathClipperD_MeasurePath measurePath;
    PathClipperD_ContinuePath continuePath;
    PathClipperD_ContinuePathData continuePathData;
    PathClipperD_ClipPath clipPath;
  } pathclipperd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathInfo]
  // --------------------------------------------------------------------------

  typedef PathInfoF* (FOG_CDECL *PathInfoF_Generate)(const PathF* path);
  typedef PathInfoD* (FOG_CDECL *PathInfoD_Generate)(const PathD* path);

  struct FOG_NO_EXPORT _Api_PathInfoF
  {
    PathInfoF_Generate generate;
  } pathinfof;

  struct FOG_NO_EXPORT _Api_PathInfoD
  {
    PathInfoD_Generate generate;
  } pathinfod;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathStroker]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *PathStrokerF_Ctor)(PathStrokerF* self);
  typedef void (FOG_CDECL *PathStrokerD_Ctor)(PathStrokerD* self);

  typedef void (FOG_CDECL *PathStrokerF_CtorParams)(PathStrokerF* self, const PathStrokerParamsF* params, const TransformF* tr, const BoxF* clipBox);
  typedef void (FOG_CDECL *PathStrokerD_CtorParams)(PathStrokerD* self, const PathStrokerParamsD* params, const TransformD* tr, const BoxD* clipBox);

  typedef void (FOG_CDECL *PathStrokerF_CtorOther)(PathStrokerF* self, const PathStrokerF* other);
  typedef void (FOG_CDECL *PathStrokerD_CtorOther)(PathStrokerD* self, const PathStrokerD* other);

  typedef void (FOG_CDECL *PathStrokerF_Dtor)(PathStrokerF* self);
  typedef void (FOG_CDECL *PathStrokerD_Dtor)(PathStrokerD* self);

  typedef void (FOG_CDECL *PathStrokerF_SetParams)(PathStrokerF* self, const PathStrokerParamsF* params);
  typedef void (FOG_CDECL *PathStrokerD_SetParams)(PathStrokerD* self, const PathStrokerParamsD* params);

  typedef void (FOG_CDECL *PathStrokerF_SetOther)(PathStrokerF* self, const PathStrokerF* other);
  typedef void (FOG_CDECL *PathStrokerD_SetOther)(PathStrokerD* self, const PathStrokerD* other);

  typedef err_t (FOG_CDECL *PathStrokerF_StrokeShape)(const PathStrokerF* self, PathF* dst, uint32_t shapeType, const void* shapeData);
  typedef err_t (FOG_CDECL *PathStrokerD_StrokeShape)(const PathStrokerD* self, PathD* dst, uint32_t shapeType, const void* shapeData);

  typedef err_t (FOG_CDECL *PathStrokerF_StrokePath)(const PathStrokerF* self, PathF* dst, const PathF* src);
  typedef err_t (FOG_CDECL *PathStrokerD_StrokePath)(const PathStrokerD* self, PathD* dst, const PathD* src);

  typedef void (FOG_CDECL *PathStrokerF_Update)(PathStrokerF* self);
  typedef void (FOG_CDECL *PathStrokerD_Update)(PathStrokerD* self);

  struct FOG_NO_EXPORT _Api_PathStrokerF
  {
    PathStrokerF_Ctor ctor;
    PathStrokerF_CtorParams ctorParams;
    PathStrokerF_CtorOther ctorOther;
    PathStrokerF_Dtor dtor;
    PathStrokerF_SetParams setParams;
    PathStrokerF_SetOther setOther;
    PathStrokerF_StrokeShape strokeShape;
    PathStrokerF_StrokePath strokePath;
    PathStrokerF_Update update;
  } pathstrokerf;

  struct FOG_NO_EXPORT _Api_PathStrokerD
  {
    PathStrokerD_Ctor ctor;
    PathStrokerD_CtorParams ctorParams;
    PathStrokerD_CtorOther ctorOther;
    PathStrokerD_Dtor dtor;
    PathStrokerD_SetParams setParams;
    PathStrokerD_SetOther setOther;
    PathStrokerD_StrokeShape strokeShape;
    PathStrokerD_StrokePath strokePath;
    PathStrokerD_Update update;
  } pathstrokerd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Pie]
  // --------------------------------------------------------------------------

  typedef bool (FOG_CDECL *PieF_HitTest)(const PieF* self, const PointF* pt);
  typedef bool (FOG_CDECL *PieD_HitTest)(const PieD* self, const PointD* pt);

  struct FOG_NO_EXPORT _Api_PieF
  {
    PieF_HitTest hitTest;
  } pief;

  struct FOG_NO_EXPORT _Api_PieD
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

  struct FOG_NO_EXPORT _Api_QBezierF
  {
    QBezierF_GetBoundingBox getBoundingBox;
    QBezierF_GetSplineBBox getSplineBBox;
    QBezierF_GetLength getLength;
    QBezierF_Flatten flatten;
  } qbezierf;

  struct FOG_NO_EXPORT _Api_QBezierD
  {
    QBezierD_GetBoundingBox getBoundingBox;
    QBezierD_GetSplineBBox getSplineBBox;
    QBezierD_GetLength getLength;
    QBezierD_Flatten flatten;
  } qbezierd;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Round]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *RoundF_GetBoundingBox)(const RoundF* self, BoxF* dst, const TransformF* tr);
  typedef err_t (FOG_CDECL *RoundD_GetBoundingBox)(const RoundD* self, BoxD* dst, const TransformD* tr);

  typedef bool (FOG_CDECL *RoundF_HitTest)(const RoundF* self, const PointF* pt);
  typedef bool (FOG_CDECL *RoundD_HitTest)(const RoundD* self, const PointD* pt);

  struct FOG_NO_EXPORT _Api_RoundF
  {
    RoundF_GetBoundingBox getBoundingBox;
    RoundF_HitTest hitTest;
  } roundf;

  struct FOG_NO_EXPORT _Api_RoundD
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

  struct FOG_NO_EXPORT _Api_ShapeF
  {
    ShapeF_GetBoundingBox getBoundingBox;
    ShapeF_HitTest hitTest;
  } shapef;

  struct FOG_NO_EXPORT _Api_ShapeD
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

  struct FOG_NO_EXPORT _Api_TransformF
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

  struct FOG_NO_EXPORT _Api_TransformD
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

  struct FOG_NO_EXPORT _Api_TriangleF
  {
    TriangleF_GetBoundingBox getBoundingBox;
    TriangleF_HitTest hitTest;
  } trianglef;

  struct FOG_NO_EXPORT _Api_TriangleD
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

  struct FOG_NO_EXPORT _Api_ImageFormatDescription
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

  struct FOG_NO_EXPORT _Api_Painter
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

  typedef err_t (FOG_CDECL *Color_ParseA)(Color& self, const StubA& str, uint32_t flags);
  typedef err_t (FOG_CDECL *Color_ParseU)(Color& self, const StubW& str, uint32_t flags);

  struct FOG_NO_EXPORT _Api_Color
  {
    Color_Convert convert[_COLOR_MODEL_COUNT][_COLOR_MODEL_COUNT];

    Color_SetModel setModel;
    Color_SetData setData;

    Color_Mix mix;
    Color_Adjust adjust;

    Color_ParseA parseA;
    Color_ParseU parseU;
  } color;

  // --------------------------------------------------------------------------
  // [G2d/Source - ColorStopList]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *ColorStopList_Ctor)(ColorStopList* self);
  typedef void (FOG_CDECL *ColorStopList_CtorCopy)(ColorStopList* self, const ColorStopList* other);
  typedef void (FOG_CDECL *ColorStopList_Dtor)(ColorStopList* self);

  typedef err_t (FOG_CDECL *ColorStopList_Reserve)(ColorStopList* self, size_t n);
  typedef void (FOG_CDECL *ColorStopList_Squeeze)(ColorStopList* self);
  typedef err_t (FOG_CDECL *ColorStopList_SetData)(ColorStopList* self, const ColorStop* stops, size_t length);
  typedef bool (FOG_CDECL *ColorStopList_IsOpaque)(const ColorStopList* self);
  typedef bool (FOG_CDECL *ColorStopList_IsOpaque_ARGB32)(const ColorStopList* self);
  typedef void (FOG_CDECL *ColorStopList_Clear)(ColorStopList* self);
  typedef void (FOG_CDECL *ColorStopList_Reset)(ColorStopList* self);

  typedef err_t (FOG_CDECL *ColorStopList_AddStop)(ColorStopList* self, const ColorStop* stop);
  typedef err_t (FOG_CDECL *ColorStopList_RemoveOffset)(ColorStopList* self, float offset);
  typedef err_t (FOG_CDECL *ColorStopList_RemoveStop)(ColorStopList* self, const ColorStop* stop);
  typedef err_t (FOG_CDECL *ColorStopList_RemoveAt)(ColorStopList* self, size_t index);
  typedef err_t (FOG_CDECL *ColorStopList_RemoveRange)(ColorStopList* self, const Range* range);
  typedef err_t (FOG_CDECL *ColorStopList_RemoveInterval)(ColorStopList* self, const IntervalF* interval);
  typedef size_t (FOG_CDECL *ColorStopList_IndexOfOffset)(const ColorStopList* self, float offset);

  typedef err_t (FOG_CDECL *ColorStopList_Copy)(ColorStopList* self, const ColorStopList* other);
  typedef bool (FOG_CDECL *ColorStopList_Eq)(const ColorStopList* a, const ColorStopList* b);

  typedef ColorStopListData* (FOG_CDECL *ColorStopList_DCreate)(size_t capacity);
  typedef void (FOG_CDECL *ColorStopList_DFree)(ColorStopListData* d);

  struct FOG_NO_EXPORT _Api_ColorStopList
  {
    ColorStopList_Ctor ctor;
    ColorStopList_CtorCopy ctorCopy;
    ColorStopList_Dtor dtor;
    ColorStopList_Reserve reserve;
    ColorStopList_Squeeze squeeze;
    ColorStopList_SetData setData;
    ColorStopList_IsOpaque isOpaque;
    ColorStopList_IsOpaque_ARGB32 isOpaque_ARGB32;
    ColorStopList_Clear clear;
    ColorStopList_Reset reset;

    ColorStopList_AddStop addStop;
    ColorStopList_RemoveOffset removeOffset;
    ColorStopList_RemoveStop removeStop;
    ColorStopList_RemoveAt removeAt;
    ColorStopList_RemoveRange removeRange;
    ColorStopList_RemoveInterval removeInterval;
    ColorStopList_IndexOfOffset indexOfOffset;

    ColorStopList_Copy copy;
    ColorStopList_Eq eq;

    ColorStopList_DCreate dCreate;
    ColorStopList_DFree dFree;

    ColorStopList* oEmpty;
  } colorstoplist;

  // --------------------------------------------------------------------------
  // [G2d/Tools - Dpi]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Dpi_Reset)(Dpi* self);
  typedef err_t (FOG_CDECL *Dpi_SetDpi)(Dpi* self, float dpi);
  typedef err_t (FOG_CDECL *Dpi_SetDpiEmEx)(Dpi* self, float dpi, float em, float ex);
  typedef void (FOG_CDECL *Dpi_Copy)(Dpi* self, const Dpi* other);

  struct FOG_NO_EXPORT _Api_Dpi
  {
    Dpi_Reset reset;
    Dpi_SetDpi setDpi;
    Dpi_SetDpiEmEx setDpiEmEx;
    Dpi_Copy copy;
  } dpi;

  // --------------------------------------------------------------------------
  // [G2d/Tools - Matrix]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *MatrixF_Ctor)(MatrixF* self);
  typedef void (FOG_CDECL *MatrixD_Ctor)(MatrixD* self);

  typedef void (FOG_CDECL *MatrixF_CtorCopy)(MatrixF* self, const MatrixF* other);
  typedef void (FOG_CDECL *MatrixD_CtorCopy)(MatrixD* self, const MatrixD* other);

  typedef void (FOG_CDECL *MatrixF_CtorCreate)(MatrixF* self, const SizeI* size, const float* data);
  typedef void (FOG_CDECL *MatrixD_CtorCreate)(MatrixD* self, const SizeI* size, const double* data);

  typedef void (FOG_CDECL *MatrixF_Dtor)(MatrixF* self);
  typedef void (FOG_CDECL *MatrixD_Dtor)(MatrixD* self);

  typedef err_t (FOG_CDECL *MatrixF_Detach)(MatrixF* self);
  typedef err_t (FOG_CDECL *MatrixD_Detach)(MatrixD* self);

  typedef err_t (FOG_CDECL *MatrixF_Create)(MatrixF* self, const SizeI* size, const float* data);
  typedef err_t (FOG_CDECL *MatrixD_Create)(MatrixD* self, const SizeI* size, const double* data);

  typedef err_t (FOG_CDECL *MatrixF_Resize)(MatrixF* self, const SizeI* size, float value);
  typedef err_t (FOG_CDECL *MatrixD_Resize)(MatrixD* self, const SizeI* size, double value);

  typedef float (FOG_CDECL *MatrixF_GetCell)(const MatrixF* self, int x, int y);
  typedef double (FOG_CDECL *MatrixD_GetCell)(const MatrixD* self, int x, int y);

  typedef err_t (FOG_CDECL *MatrixF_SetCell)(MatrixF* self, int x, int y, float value);
  typedef err_t (FOG_CDECL *MatrixD_SetCell)(MatrixD* self, int x, int y, double value);

  typedef err_t (FOG_CDECL *MatrixF_Fill)(MatrixF* self, const RectI* area, float value);
  typedef err_t (FOG_CDECL *MatrixD_Fill)(MatrixD* self, const RectI* area, double value);

  typedef void (FOG_CDECL *MatrixF_Reset)(MatrixF* self);
  typedef void (FOG_CDECL *MatrixD_Reset)(MatrixD* self);

  typedef void (FOG_CDECL *MatrixF_Copy)(MatrixF* self, const MatrixF* other);
  typedef void (FOG_CDECL *MatrixD_Copy)(MatrixD* self, const MatrixD* other);

  typedef bool (FOG_CDECL *MatrixF_Eq)(const MatrixF* a, const MatrixF* b);
  typedef bool (FOG_CDECL *MatrixD_Eq)(const MatrixD* a, const MatrixD* b);

  typedef MatrixDataF* (FOG_CDECL *MatrixF_DCreate)(const SizeI* size, const float* data);
  typedef MatrixDataD* (FOG_CDECL *MatrixD_DCreate)(const SizeI* size, const double* data);

  typedef void (FOG_CDECL *MatrixF_DFree)(MatrixDataF* d);
  typedef void (FOG_CDECL *MatrixD_DFree)(MatrixDataD* d);

  struct FOG_NO_EXPORT _Api_MatrixF
  {
    MatrixF_Ctor ctor;
    MatrixF_CtorCopy ctorCopy;
    MatrixF_CtorCreate ctorCreate;
    MatrixF_Dtor dtor;
    MatrixF_Detach detach;
    MatrixF_Create create;
    MatrixF_Resize resize;
    MatrixF_GetCell getCell;
    MatrixF_SetCell setCell;
    MatrixF_Fill fill;
    MatrixF_Reset reset;
    MatrixF_Copy copy;
    MatrixF_Eq eq;

    MatrixF_DCreate dCreate;
    MatrixF_DFree dFree;

    MatrixF* oEmpty;
  } matrixf;

  struct FOG_NO_EXPORT _Api_MatrixD
  {
    MatrixD_Ctor ctor;
    MatrixD_CtorCopy ctorCopy;
    MatrixD_CtorCreate ctorCreate;
    MatrixD_Dtor dtor;
    MatrixD_Detach detach;
    MatrixD_Create create;
    MatrixD_Resize resize;
    MatrixD_GetCell getCell;
    MatrixD_SetCell setCell;
    MatrixD_Fill fill;
    MatrixD_Reset reset;
    MatrixD_Copy copy;
    MatrixD_Eq eq;

    MatrixD_DCreate dCreate;
    MatrixD_DFree dFree;

    MatrixD* oEmpty;
  } matrixd;

  // --------------------------------------------------------------------------
  // [G2d/Tools - Region]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Region_Ctor)(Region* self);
  typedef void (FOG_CDECL *Region_CtorRegion)(Region* self, const Region* other);
  typedef void (FOG_CDECL *Region_CtorBox)(Region* self, const BoxI* box);
  typedef void (FOG_CDECL *Region_CtorRect)(Region* self, const RectI* rect);
  typedef void (FOG_CDECL *Region_Dtor)(Region* self);

  typedef err_t (FOG_CDECL *Region_Detach)(Region* self);
  typedef err_t (FOG_CDECL *Region_Reserve)(Region* self, size_t capacity);
  typedef void (FOG_CDECL *Region_Squeeze)(Region* self);
  typedef err_t (FOG_CDECL *Region_Prepare)(Region* self, size_t count);

  typedef uint32_t (FOG_CDECL *Region_GetType)(const Region* self);

  typedef void (FOG_CDECL *Region_Clear)(Region* self);
  typedef void (FOG_CDECL *Region_Reset)(Region* self);

  typedef err_t (FOG_CDECL *Region_SetRegion)(Region* self, const Region* other);
  typedef err_t (FOG_CDECL *Region_SetDeep)(Region* self, const Region* other);
  typedef err_t (FOG_CDECL *Region_SetBox)(Region* self, const BoxI* box);
  typedef err_t (FOG_CDECL *Region_SetRect)(Region* self, const RectI* rect);

  typedef err_t (FOG_CDECL *Region_SetBoxList)(Region* self, const BoxI* data, size_t length);
  typedef err_t (FOG_CDECL *Region_SetRectList)(Region* self, const RectI* data, size_t length);

  typedef err_t (FOG_CDECL *Region_CombineRegionRegion)(Region* dst, const Region* a, const Region* b, uint32_t combineOp);
  typedef err_t (FOG_CDECL *Region_CombineRegionBox)(Region* dst, const Region* a, const BoxI* b, uint32_t combineOp);
  typedef err_t (FOG_CDECL *Region_CombineBoxRegion)(Region* dst, const BoxI* a, const Region* b, uint32_t combineOp);
  typedef err_t (FOG_CDECL *Region_CombineBoxBox)(Region* dst, const BoxI* a, const BoxI* b, uint32_t combineOp);

  typedef err_t (FOG_CDECL *Region_Translate)(Region* dst, const Region* src, const PointI* pt);
  typedef err_t (FOG_CDECL *Region_TranslateAndClip)(Region* dst, const Region* src, const PointI* pt, const BoxI* clipBox);
  typedef err_t (FOG_CDECL *Region_IntersectAndClip)(Region* dst, const Region* a, const Region* b, const BoxI* clipBox);

  typedef uint32_t (FOG_CDECL *Region_HitTestPoint)(const Region* self, const PointI* pt);
  typedef uint32_t (FOG_CDECL *Region_HitTestBox)(const Region* self, const BoxI* box);
  typedef uint32_t (FOG_CDECL *Region_HitTestRect)(const Region* self, const RectI* rect);

  typedef bool (FOG_CDECL *Region_Eq)(const Region* a, const Region* b);

#if defined(FOG_OS_WINDOWS)
  typedef err_t (FOG_CDECL *Region_HRGNFromRegion)(HRGN* dst, const Region* src);
  typedef err_t (FOG_CDECL *Region_RegionFromHRGN)(Region* dst, HRGN src);
#endif // FOG_OS_WINDOWS

  typedef RegionData* (FOG_CDECL *Region_DCreate)(size_t capacity);
  typedef RegionData* (FOG_CDECL *Region_DCreateBox)(size_t capacity, const BoxI* box);
  typedef RegionData* (FOG_CDECL *Region_DCreateRegion)(size_t capacity, const BoxI* data, size_t count, const BoxI* extent);

  typedef RegionData* (FOG_CDECL *Region_DAdopt)(void* address, size_t capacity);
  typedef RegionData* (FOG_CDECL *Region_DAdoptBox)(void* address, size_t capacity, const BoxI* box);
  typedef RegionData* (FOG_CDECL *Region_DAdoptRegion)(void* address, size_t capacity, const BoxI* data, size_t count, const BoxI* bbox);

  typedef RegionData* (FOG_CDECL *Region_DRealloc)(RegionData* d, size_t capacity);
  typedef RegionData* (FOG_CDECL *Region_DCopy)(const RegionData* d);

  typedef void (FOG_CDECL *Region_DFree)(RegionData* d);

  struct FOG_NO_EXPORT _Api_Region
  {
    Region_Ctor ctor;
    Region_CtorRegion ctorRegion;
    Region_CtorBox ctorBox;
    Region_CtorRect ctorRect;
    Region_Dtor dtor;

    Region_Detach detach;
    Region_Reserve reserve;
    Region_Squeeze squeeze;
    Region_Prepare prepare;

    Region_GetType getType;

    Region_Clear clear;
    Region_Reset reset;

    Region_SetRegion setRegion;
    Region_SetDeep setDeep;
    Region_SetBox setBox;
    Region_SetRect setRect;

    Region_SetBoxList setBoxList;
    Region_SetRectList setRectList;

    Region_CombineRegionRegion combineRegionRegion;
    Region_CombineRegionBox combineRegionBox;
    Region_CombineBoxRegion combineBoxRegion;
    Region_CombineBoxBox combineBoxBox;

    Region_Translate translate;
    Region_TranslateAndClip translateAndClip;
    Region_IntersectAndClip intersectAndClip;

    Region_HitTestPoint hitTestPoint;
    Region_HitTestBox hitTestBox;
    Region_HitTestRect hitTestRect;

    Region_Eq eq;

#if defined(FOG_OS_WINDOWS)
    Region_HRGNFromRegion hrgnFromRegion;
    Region_RegionFromHRGN regionFromHRGN;
#endif // FOG_OS_WINDOWS

    Region_DCreate dCreate;
    Region_DCreateBox dCreateBox;
    Region_DCreateRegion dCreateRegion;

    Region_DAdopt dAdopt;
    Region_DAdoptBox dAdoptBox;
    Region_DAdoptRegion dAdoptRegion;

    Region_DRealloc dRealloc;
    Region_DCopy dCopy;

    Region_DFree dFree;

    Region* oEmpty;
    Region* oInfinite;
  } region;

  // --------------------------------------------------------------------------
  // [G2d/Tools - RegionUtil]
  // --------------------------------------------------------------------------

  typedef const BoxI* (FOG_CDECL *RegionUtil_GetClosestBox)(const BoxI* data, size_t length, int y);
  typedef bool (FOG_CDECL *RegionUtil_IsBoxListSorted)(const BoxI* data, size_t length);
  typedef bool (FOG_CDECL *RegionUtil_IsRectListSorted)(const RectI* data, size_t length);

  struct FOG_NO_EXPORT _Api_RegionUtil
  {
    RegionUtil_GetClosestBox getClosestBox;
    RegionUtil_IsBoxListSorted isBoxListSorted;
    RegionUtil_IsRectListSorted isRectListSorted;
  } regionutil;
};

extern FOG_API Api _api;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_API_H
