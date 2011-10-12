// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_API_H
#define _FOG_CORE_GLOBAL_API_H

// [Dependencies]
#include <Fog/Core/Global/EnumCore.h>
#include <Fog/Core/Global/EnumG2d.h>
#include <Fog/Core/Global/EnumUI.h>
#include <Fog/Core/Global/TypeDefs.h>

#if defined(FOG_OS_WINDOWS)
#include <objidl.h>
#endif // FOG_OS_WINDOWS

// ============================================================================
// [Fog::Api]
// ============================================================================

#define FOG_CAPI_CTOR(_Name_) void (FOG_CDECL* _Name_)
#define FOG_CAPI_DTOR(_Name_) void (FOG_CDECL* _Name_)
#define FOG_CAPI_METHOD(_Ret_, _Name_) _Ret_ (FOG_CDECL* _Name_)
#define FOG_CAPI_STATIC(_Ret_, _Name_) _Ret_ (FOG_CDECL* _Name_)

namespace Fog {

//! @internal
//!
//! @brief Fog-Framework C-API.
struct FOG_NO_EXPORT Api
{
  // --------------------------------------------------------------------------
  // [Core/Math - Math]
  // --------------------------------------------------------------------------

  typedef err_t (FOG_CDECL *MathF_IntegrateFunc)(float* dst, const FunctionF* func, const IntervalF* interval, uint32_t steps);
  typedef err_t (FOG_CDECL *MathD_IntegrateFunc)(double* dst, const FunctionD* func, const IntervalD* interval, uint32_t steps);

  typedef int (FOG_CDECL *MathF_SolveFunc)(float* dst, const float* func);
  typedef int (FOG_CDECL *MathD_SolveFunc)(double* dst, const double* func);

  typedef int (FOG_CDECL *MathF_SolveAtFunc)(float* dst, const float* func, const IntervalF* interval);
  typedef int (FOG_CDECL *MathD_SolveAtFunc)(double* dst, const double* func, const IntervalD* interval);

  MathF_IntegrateFunc mathf_integrate[MATH_INTEGRATION_METHOD_COUNT];
  MathF_SolveFunc mathf_solve[MATH_SOLVE_COUNT];
  MathF_SolveAtFunc mathf_solveAt[MATH_SOLVE_COUNT];
  FOG_CAPI_STATIC(void, mathf_vecFloatFromDouble)(float* dst, const double* src, size_t length);

  MathD_IntegrateFunc mathd_integrate[MATH_INTEGRATION_METHOD_COUNT];
  MathD_SolveFunc mathd_solve[MATH_SOLVE_COUNT];
  MathD_SolveAtFunc mathd_solveAt[MATH_SOLVE_COUNT];
  FOG_CAPI_STATIC(void, mathd_vecDoubleFromFloat)(double* dst, const float* src, size_t length);

  // --------------------------------------------------------------------------
  // [Core/Memory - MemBlockAllocator]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(memblockallocator_ctor)(MemBlockAllocator* self);
  FOG_CAPI_DTOR(memblockallocator_dtor)(MemBlockAllocator* self);

  FOG_CAPI_METHOD(void*, memblockallocator_alloc)(MemBlockAllocator* self, size_t size);
  FOG_CAPI_METHOD(void, memblockallocator_reset)(MemBlockAllocator* self);

  // --------------------------------------------------------------------------
  // [Core/Memory - MemMgr]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(void*, memmgr_alloc)(size_t size);
  FOG_CAPI_STATIC(void*, memmgr_calloc)(size_t size);
  FOG_CAPI_STATIC(void*, memmgr_realloc)(void* ptr, size_t size);
  FOG_CAPI_STATIC(void, memmgr_free)(void* ptr);

  FOG_CAPI_STATIC(void, memmgr_cleanup)(uint32_t reason);
  FOG_CAPI_STATIC(err_t, memmgr_registerCleanupFunc)(MemCleanupFunc func, void* closure);
  FOG_CAPI_STATIC(err_t, memmgr_unregisterCleanupFunc)(MemCleanupFunc func, void* closure);

  FOG_CAPI_STATIC(uint64_t, memmgr_getAmountOfPhysicalMemory)(void);
  FOG_CAPI_STATIC(uint32_t, memmgr_getAmountOfPhysicalMemoryMB)(void);

  // --------------------------------------------------------------------------
  // [Core/Memory - MemOps]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(void*, memops_copy)(void* dst, const void* src, size_t size);
  FOG_CAPI_STATIC(void*, memops_move)(void* dst, const void* src, size_t size);
  FOG_CAPI_STATIC(void*, memops_zero)(void* dst, size_t size);
  FOG_CAPI_STATIC(void*, memops_set)(void* dst, uint val, size_t size);

  FOG_CAPI_STATIC(void*, memops_copynt)(void* dst, const void* src, size_t size);
  FOG_CAPI_STATIC(void*, memops_zeront)(void* dst, size_t size);
  FOG_CAPI_STATIC(void*, memops_setnt)(void* dst, uint val, size_t size);

  FOG_CAPI_STATIC(void, memops_xchg)(void* a, void* b, size_t size);
  FOG_CAPI_STATIC(void*, memops_eq)(const void* a, const void* b, size_t size);

  // --------------------------------------------------------------------------
  // [Core/Memory - MemPool]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(mempool_ctor)(MemPool* self);
  FOG_CAPI_DTOR(mempool_dtor)(MemPool* self);

  FOG_CAPI_METHOD(size_t, mempool_getCapacity)(const MemPool* self);

  FOG_CAPI_METHOD(void, mempool_reset)(MemPool* self);
  FOG_CAPI_METHOD(void*, mempool_save)(MemPool* self);

  FOG_CAPI_METHOD(err_t, mempool_prealloc)(MemPool* self, size_t szItem, size_t count);
  FOG_CAPI_METHOD(void*, mempool_alloc)(MemPool* self, size_t szItem);

  FOG_CAPI_METHOD(void, mempool_freeSaved)(void* ptr);

  // --------------------------------------------------------------------------
  // [Core/Memory - MemZoneAllocator]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(memzoneallocator_ctor)(MemZoneAllocator* self, size_t nodeSize);
  FOG_CAPI_DTOR(memzoneallocator_dtor)(MemZoneAllocator* self);

  FOG_CAPI_METHOD(void*, memzoneallocator_alloc)(MemZoneAllocator* self, size_t size);

  FOG_CAPI_METHOD(void, memzoneallocator_clear)(MemZoneAllocator* self);
  FOG_CAPI_METHOD(void, memzoneallocator_reset)(MemZoneAllocator* self);

  FOG_CAPI_METHOD(MemZoneRecord*, memzoneallocator_record)(MemZoneAllocator* self);
  FOG_CAPI_METHOD(void, memzoneallocator_revert)(MemZoneAllocator* self, MemZoneRecord* record, bool keepRecord);

  // --------------------------------------------------------------------------
  // [Core/OS - DirIterator]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(diriterator_ctor)(DirIterator* self);
  FOG_CAPI_CTOR(diriterator_ctorString)(DirIterator* self, const StringW* path, bool skipParent);
  FOG_CAPI_DTOR(diriterator_dtor)(DirIterator* self);

  FOG_CAPI_METHOD(err_t, diriterator_open)(DirIterator* self, const StringW* path, bool skipParent);
  FOG_CAPI_METHOD(void, diriterator_close)(DirIterator* self);

  FOG_CAPI_METHOD(bool, diriterator_readFileInfo)(DirIterator* self, FileInfo* dirEntry);
  FOG_CAPI_METHOD(bool, diriterator_readFileName)(DirIterator* self, StringW* fileName);

  FOG_CAPI_METHOD(err_t, diriterator_rewind)(DirIterator* self);
  FOG_CAPI_METHOD(int64_t, diriterator_tell)(DirIterator* self);

  // --------------------------------------------------------------------------
  // [Core/OS - Environment]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(err_t, environment_getValueStubA)(const StubA* key, StringW* value);
  FOG_CAPI_STATIC(err_t, environment_getValueStringW)(const StringW* key, StringW* value);

  FOG_CAPI_STATIC(err_t, environment_setValueStubA)(const StubA* key, const StringW* value);
  FOG_CAPI_STATIC(err_t, environment_setValueStringW)(const StringW* key, const StringW* value);

  // --------------------------------------------------------------------------
  // [Core/OS - FileInfo]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(fileinfo_ctor)(FileInfo* self);
  FOG_CAPI_CTOR(fileinfo_ctorCopy)(FileInfo* self, const FileInfo* other);
  FOG_CAPI_DTOR(fileinfo_dtor)(FileInfo* self);

  FOG_CAPI_METHOD(err_t, fileinfo_copy)(FileInfo* self, const FileInfo* other);
  FOG_CAPI_METHOD(err_t, fileinfo_detach)(FileInfo* self);

  FOG_CAPI_METHOD(err_t, fileinfo_fromFile)(FileInfo* self, const StringW* path);
  FOG_CAPI_METHOD(err_t, fileinfo_fromFileEx)(FileInfo* self, const StringW* filePath, const StringW* fileName);

#if defined(FOG_OS_WINDOWS)
  FOG_CAPI_METHOD(err_t, fileinfo_fromWinFileAttributeData)(FileInfo* self, const StringW* filePath, const StringW* fileName, const void* wfad);
  FOG_CAPI_METHOD(err_t, fileinfo_fromWinFindData)(FileInfo* self, const StringW* filePath, const void* wfd);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_POSIX)
  FOG_CAPI_METHOD(err_t, fileinfo_fromStat)(FileInfo* self, const StringW* filePath, const StringW* fileName, const void* s);
#endif // FOG_OS_POSIX

  FOG_CAPI_STATIC(FileInfoData*, fileinfo_dCreate)(const StringW* fileName);
  FOG_CAPI_STATIC(void, fileinfo_dFree)(FileInfoData* d);

  FileInfo* fileinfo_oNull;

  // --------------------------------------------------------------------------
  // [Core/OS - FileMapping]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(filemapping_ctor)(FileMapping* self);
  FOG_CAPI_CTOR(filemapping_ctorCopy)(FileMapping* self, const FileMapping* other);
  FOG_CAPI_DTOR(filemapping_dtor)(FileMapping* self);

  FOG_CAPI_METHOD(err_t, filemapping_open)(FileMapping* self, const StringW* fileName, uint32_t flags);
  FOG_CAPI_METHOD(void, filemapping_close)(FileMapping* self);

  FOG_CAPI_METHOD(void, filemapping_copy)(FileMapping* self, const FileMapping* other);

  FOG_CAPI_STATIC(void, filemapping_dRelease)(FileMappingData* d);

  // --------------------------------------------------------------------------
  // [Core/OS - FilePath]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(err_t, filepath_join)(StringW* dst, const StringW* base, const StringW* part);

  FOG_CAPI_STATIC(err_t, filepath_extractFile)(StringW* dst, const StringW* path);
  FOG_CAPI_STATIC(err_t, filepath_extractExtension)(StringW* dst, const StringW* path);
  FOG_CAPI_STATIC(err_t, filepath_extractDirectory)(StringW* dst, const StringW* path);

  FOG_CAPI_STATIC(bool, filepath_containsFile)(const StringW* path, const StringW* file, uint cs);
  FOG_CAPI_STATIC(bool, filepath_containsExtension)(const StringW* path, const StringW* extension, uint cs);
  FOG_CAPI_STATIC(bool, filepath_containsDirectory)(const StringW* path, const StringW* directory, uint cs);

  FOG_CAPI_STATIC(err_t, filepath_normalize)(StringW* dst, const StringW* path);
  FOG_CAPI_STATIC(bool, filepath_isNormalized)(const StringW* path);

  FOG_CAPI_STATIC(bool, filepath_isRoot)(const StringW* path);

  FOG_CAPI_STATIC(err_t, filepath_toAbsolute)(StringW* dst, const StringW* path, const StringW* base);
  FOG_CAPI_STATIC(bool, filepath_isAbsolute)(const StringW* path);

  FOG_CAPI_STATIC(err_t, filepath_substituteEnvironmentVars)(StringW* dst, const StringW* path, uint32_t format);

  // --------------------------------------------------------------------------
  // [Core/OS - FileUtil]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(uint32_t, fileutil_test)(const StringW* path, uint32_t flags);
  FOG_CAPI_STATIC(bool, fileutil_testLocalName)(const StringW* path);
  FOG_CAPI_STATIC(bool, fileutil_findFile)(StringW* dst, const StringW* fileName, const List<StringW>* paths);
  FOG_CAPI_STATIC(err_t, fileutil_createDirectory)(const StringW* dir, bool recursive);
  FOG_CAPI_STATIC(err_t, fileutil_deleteDirectory)(const StringW* dir);

#if defined(FOG_OS_POSIX)
  FOG_CAPI_STATIC(int, fileutil_stat)(void* dst, const StringW* fileName);
#endif // FOG_OS_POSIX

  // --------------------------------------------------------------------------
  // [Core/OS - Library]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(library_ctor)(Library* self);
  FOG_CAPI_CTOR(library_ctorCopy)(Library* self, const Library* other);
  FOG_CAPI_DTOR(library_dtor)(Library* self);

  FOG_CAPI_METHOD(void, library_setLibrary)(Library* self, const Library* other);
  FOG_CAPI_METHOD(err_t, library_openLibrary)(Library* self, const StringW* fileName, uint32_t flags);
  FOG_CAPI_METHOD(err_t, library_openPlugin)(Library* self, const StringW* category, const StringW* name);
  FOG_CAPI_METHOD(void, library_close)(Library* self);

  FOG_CAPI_METHOD(void*, library_getSymbolStubA)(const Library* self, const StubA* sym);
  FOG_CAPI_METHOD(void*, library_getSymbolStringW)(const Library* self, const StringW* sym);
  FOG_CAPI_METHOD(size_t, library_getSymbols)(const Library* self, void** target, const char* symbolsData, size_t symbolsLength, size_t symbolsCount, char** fail);

  FOG_CAPI_STATIC(LibraryData*, library_dCreate)(void* handle);
  FOG_CAPI_STATIC(void, library_dFree)(LibraryData* d);

  FOG_CAPI_STATIC(err_t, library_getSystemPrefix)(StringW* dst);
  FOG_CAPI_STATIC(err_t, library_getSystemExtensions)(List<StringW>* dst);

  FOG_CAPI_STATIC(err_t, library_getLibraryPaths)(List<StringW>* dst);
  FOG_CAPI_STATIC(err_t, library_addLibraryPath)(const StringW* path, uint32_t mode);
  FOG_CAPI_STATIC(err_t, library_removeLibraryPath)(const StringW* path);
  FOG_CAPI_STATIC(bool, library_hasLibraryPath)(const StringW* path);

  LibraryData* library_dNone;

  // --------------------------------------------------------------------------
  // [Core/OS - OSInfo]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(err_t, osinfo_getName)(StringW* dst);

  // --------------------------------------------------------------------------
  // [Core/OS - OSUtil]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(err_t, osutil_getErrFromLibCErrorCode)(int code);
  FOG_CAPI_STATIC(err_t, osutil_getErrFromLibCErrno)(void);

  FOG_CAPI_STATIC(err_t, osutil_getErrFromOSErrorCode)(int code);
  FOG_CAPI_STATIC(err_t, osutil_getErrFromOSLastError)(void);

  // --------------------------------------------------------------------------
  // [Core/OS - UserUtil]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(uint32_t, userutil_getUid)(void);
  FOG_CAPI_STATIC(uint32_t, userutil_getGid)(void);
  FOG_CAPI_STATIC(err_t, userutil_getUserDirectory)(StringW* dst, uint32_t directoryId);

#if defined(FOG_OS_WINDOWS)

  // --------------------------------------------------------------------------
  // [Core/OS - WinCOM]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(err_t, wincom_makeIStream)(IStream** dst, Stream* src);

  // --------------------------------------------------------------------------
  // [Core/OS - WinUtil]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(err_t, winutil_getErrFromWinErrorCode)(DWORD code);
  FOG_CAPI_STATIC(err_t, winutil_getErrFromWinLastError)(void);

  FOG_CAPI_STATIC(uint32_t, winutil_getWinVersion)(WinVersion* dst);
  FOG_CAPI_STATIC(err_t, winutil_getWinDirectory)(StringW* dst);
  FOG_CAPI_STATIC(err_t, winutil_getModuleFileName)(StringW* dst, HMODULE hModule);
  FOG_CAPI_STATIC(err_t, winutil_makeWinPath)(StringW* dst, const StringW* src);

#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Core/Threading - ThreadEvent]
  // --------------------------------------------------------------------------

  // TODO:

  // --------------------------------------------------------------------------
  // [Core/Threading - ThreadLocal]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(err_t, threadlocal_create)(uint32_t* slot, void* dtor);
  FOG_CAPI_STATIC(err_t, threadlocal_destroy)(uint32_t slot);
  FOG_CAPI_STATIC(void*, threadlocal_get)(uint32_t slot);
  FOG_CAPI_STATIC(err_t, threadlocal_set)(uint32_t slot, void* val);

  // --------------------------------------------------------------------------
  // [Core/Tools - Cpu]
  // --------------------------------------------------------------------------

  Cpu* cpu_oInstance;

  // --------------------------------------------------------------------------
  // [Core/Tools - Date]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(int, date_getValue)(const Date* self, uint32_t key);
  FOG_CAPI_METHOD(err_t, date_setValue)(Date* self, uint32_t key, int value);
  FOG_CAPI_METHOD(err_t, date_addValue)(Date* self, uint32_t key, int64_t value);

  FOG_CAPI_METHOD(err_t, date_setYMD)(Date* self, int year, int month, int day);
  FOG_CAPI_METHOD(err_t, date_setHMS)(Date* self, int hour, int minute, int second, int us);
  FOG_CAPI_METHOD(err_t, date_fromTime)(Date* self, const Time* time);

  FOG_CAPI_METHOD(bool, date_isLeapYear)(const Date* self, int year);
  FOG_CAPI_METHOD(int, date_getNumberOfDaysInYear)(const Date* self, int year);
  FOG_CAPI_METHOD(int, date_getNumberOfDaysInMonth)(const Date* self, int year, int month);

  FOG_CAPI_METHOD(err_t, date_convert)(Date* dst, const Date* src, uint32_t zone);

  FOG_CAPI_STATIC(bool, date_eq)(const Date* a, const Date* b);
  FOG_CAPI_STATIC(int, date_compare)(const Date* a, const Date* b);

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<Unknown, Unknown>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(hash_unknown_unknown_ctor)(HashUntyped* self);
  FOG_CAPI_CTOR(hash_unknown_unknown_ctorCopy)(HashUntyped* self, const HashUntyped* other);
  FOG_CAPI_CTOR(hash_unknown_unknown_dtor)(HashUntyped* self, const HashUntypedVTable* v);

  FOG_CAPI_METHOD(err_t, hash_unknown_unknown_detach)(HashUntyped* self, const HashUntypedVTable* v);
  FOG_CAPI_METHOD(err_t, hash_unknown_unknown_rehash)(HashUntyped* self, const HashUntypedVTable* v, size_t capacity);
  FOG_CAPI_METHOD(err_t, hash_unknown_unknown_reserve)(HashUntyped* self, const HashUntypedVTable* v, size_t capacity);
  FOG_CAPI_METHOD(void, hash_unknown_unknown_squeeze)(HashUntyped* self, const HashUntypedVTable* v);

  FOG_CAPI_METHOD(void, hash_unknown_unknown_clear)(HashUntyped* self, const HashUntypedVTable* v);
  FOG_CAPI_METHOD(void, hash_unknown_unknown_reset)(HashUntyped* self, const HashUntypedVTable* v);

  FOG_CAPI_METHOD(const void*, hash_unknown_unknown_get)(const HashUntyped* self, const HashUntypedVTable* v, const void* key);
  FOG_CAPI_METHOD(void*, hash_unknown_unknown_use)(HashUntyped* self, const HashUntypedVTable* v, const void* key);
  FOG_CAPI_METHOD(err_t, hash_unknown_unknown_put)(HashUntyped* self, const HashUntypedVTable* v, const void* key, const void* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_unknown_unknown_remove)(HashUntyped* self, const HashUntypedVTable* v, const void* key);

  FOG_CAPI_METHOD(void, hash_unknown_unknown_copy)(HashUntyped* self, const HashUntypedVTable* v, const HashUntyped* other);
  FOG_CAPI_METHOD(bool, hash_unknown_unknown_eq)(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc);

  FOG_CAPI_METHOD(HashUntypedData*, hash_unknown_unknown_dCreate)(size_t capacity);
  FOG_CAPI_METHOD(void, hash_unknown_unknown_dFree)(HashUntypedData* d, const HashUntypedVTable* v);

  HashUntyped* hash_unknown_unknown_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<Int32, Unknown>]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(const void*, hash_int32_unknown_get)(const HashUntyped* self, const HashUntypedVTable* v, int32_t key);
  FOG_CAPI_METHOD(void*, hash_int32_unknown_use)(HashUntyped* self, const HashUntypedVTable* v, int32_t key);
  FOG_CAPI_METHOD(err_t, hash_int32_unknown_put)(HashUntyped* self, const HashUntypedVTable* v, int32_t key, const void* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_int32_unknown_remove)(HashUntyped* self, const HashUntypedVTable* v, int32_t key);

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<Int64, Unknown>]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(const void*, hash_int64_unknown_get)(const HashUntyped* self, const HashUntypedVTable* v, int64_t key);
  FOG_CAPI_METHOD(void*, hash_int64_unknown_use)(HashUntyped* self, const HashUntypedVTable* v, int64_t key);
  FOG_CAPI_METHOD(err_t, hash_int64_unknown_put)(HashUntyped* self, const HashUntypedVTable* v, int64_t key, const void* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_int64_unknown_remove)(HashUntyped* self, const HashUntypedVTable* v, int64_t key);

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringA, Unknown>]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(const void*, hash_stringa_unknown_getStubA)(const HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  FOG_CAPI_METHOD(const void*, hash_stringa_unknown_getStringA)(const HashUntyped* self, const HashUntypedVTable* v, const StringA* key);

  FOG_CAPI_METHOD(void*, hash_stringa_unknown_useStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  FOG_CAPI_METHOD(void*, hash_stringa_unknown_useStringA)(HashUntyped* self, const HashUntypedVTable* v, const StringA* key);

  FOG_CAPI_METHOD(err_t, hash_stringa_unknown_putStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key, const void* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringa_unknown_putStringA)(HashUntyped* self, const HashUntypedVTable* v, const StringA* key, const void* item, bool replace);

  FOG_CAPI_METHOD(err_t, hash_stringa_unknown_removeStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  FOG_CAPI_METHOD(err_t, hash_stringa_unknown_removeStringA)(HashUntyped* self, const HashUntypedVTable* v, const StringA* key);

  FOG_CAPI_STATIC(bool, hash_stringa_unknown_eq)(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc);

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringA, StringA>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(hash_stringa_stringa_ctor)(HashUntyped* self);
  FOG_CAPI_DTOR(hash_stringa_stringa_dtor)(HashUntyped* self);

  FOG_CAPI_METHOD(const StringA*, hash_stringa_stringa_getStubA)(const HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(const StringA*, hash_stringa_stringa_getStringA)(const HashUntyped* self, const StringA* key);

  FOG_CAPI_METHOD(StringA*, hash_stringa_stringa_useStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(StringA*, hash_stringa_stringa_useStringA)(HashUntyped* self, const StringA* key);

  FOG_CAPI_METHOD(err_t, hash_stringa_stringa_putStubA)(HashUntyped* self, const StubA* key, const StringA* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringa_stringa_putStringA)(HashUntyped* self, const StringA* key, const StringA* item, bool replace);

  FOG_CAPI_METHOD(err_t, hash_stringa_stringa_removeStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(err_t, hash_stringa_stringa_removeStringA)(HashUntyped* self, const StringA* key);

  FOG_CAPI_METHOD(void, hash_stringa_stringa_dFree)(HashUntypedData* d);

  HashUntypedVTable* hash_stringa_stringa_vTable;
  HashUntyped* hash_stringa_stringa_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringA, Var>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(hash_stringa_var_ctor)(HashUntyped* self);
  FOG_CAPI_DTOR(hash_stringa_var_dtor)(HashUntyped* self);

  FOG_CAPI_METHOD(const Var*, hash_stringa_var_getStubA)(const HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(const Var*, hash_stringa_var_getStringA)(const HashUntyped* self, const StringA* key);

  FOG_CAPI_METHOD(Var*, hash_stringa_var_useStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(Var*, hash_stringa_var_useStringA)(HashUntyped* self, const StringA* key);

  FOG_CAPI_METHOD(err_t, hash_stringa_var_putStubA)(HashUntyped* self, const StubA* key, const Var* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringa_var_putStringA)(HashUntyped* self, const StringA* key, const Var* item, bool replace);

  FOG_CAPI_METHOD(err_t, hash_stringa_var_removeStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(err_t, hash_stringa_var_removeStringA)(HashUntyped* self, const StringA* key);

  FOG_CAPI_METHOD(void, hash_stringa_var_dFree)(HashUntypedData* d);

  HashUntypedVTable* hash_stringa_var_vTable;
  HashUntyped* hash_stringa_var_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringW, Unknown>]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(const void*, hash_stringw_unknown_getStubA)(const HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  FOG_CAPI_METHOD(const void*, hash_stringw_unknown_getStubW)(const HashUntyped* self, const HashUntypedVTable* v, const StubW* key);
  FOG_CAPI_METHOD(const void*, hash_stringw_unknown_getStringW)(const HashUntyped* self, const HashUntypedVTable* v, const StringW* key);

  FOG_CAPI_METHOD(void*, hash_stringw_unknown_useStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  FOG_CAPI_METHOD(void*, hash_stringw_unknown_useStubW)(HashUntyped* self, const HashUntypedVTable* v, const StubW* key);
  FOG_CAPI_METHOD(void*, hash_stringw_unknown_useStringW)(HashUntyped* self, const HashUntypedVTable* v, const StringW* key);

  FOG_CAPI_METHOD(err_t, hash_stringw_unknown_putStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key, const void* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringw_unknown_putStubW)(HashUntyped* self, const HashUntypedVTable* v, const StubW* key, const void* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringw_unknown_putStringW)(HashUntyped* self, const HashUntypedVTable* v, const StringW* key, const void* item, bool replace);

  FOG_CAPI_METHOD(err_t, hash_stringw_unknown_removeStubA)(HashUntyped* self, const HashUntypedVTable* v, const StubA* key);
  FOG_CAPI_METHOD(err_t, hash_stringw_unknown_removeStubW)(HashUntyped* self, const HashUntypedVTable* v, const StubW* key);
  FOG_CAPI_METHOD(err_t, hash_stringw_unknown_removeStringW)(HashUntyped* self, const HashUntypedVTable* v, const StringW* key);

  FOG_CAPI_METHOD(bool, hash_stringw_unknown_eq)(const HashUntyped* a, const HashUntyped* b, const HashUntypedVTable* v, EqFunc itemEqFunc);

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringW, StringW>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(hash_stringw_stringw_ctor)(HashUntyped* self);
  FOG_CAPI_DTOR(hash_stringw_stringw_dtor)(HashUntyped* self);

  FOG_CAPI_METHOD(const StringW*, hash_stringw_stringw_getStubA)(const HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(const StringW*, hash_stringw_stringw_getStubW)(const HashUntyped* self, const StubW* key);
  FOG_CAPI_METHOD(const StringW*, hash_stringw_stringw_getStringW)(const HashUntyped* self, const StringW* key);

  FOG_CAPI_METHOD(StringW*, hash_stringw_stringw_useStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(StringW*, hash_stringw_stringw_useStubW)(HashUntyped* self, const StubW* key);
  FOG_CAPI_METHOD(StringW*, hash_stringw_stringw_useStringW)(HashUntyped* self, const StringW* key);

  FOG_CAPI_METHOD(err_t, hash_stringw_stringw_putStubA)(HashUntyped* self, const StubA* key, const StringW* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringw_stringw_putStubW)(HashUntyped* self, const StubW* key, const StringW* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringw_stringw_putStringW)(HashUntyped* self, const StringW* key, const StringW* item, bool replace);

  FOG_CAPI_METHOD(err_t, hash_stringw_stringw_removeStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(err_t, hash_stringw_stringw_removeStubW)(HashUntyped* self, const StubW* key);
  FOG_CAPI_METHOD(err_t, hash_stringw_stringw_removeStringW)(HashUntyped* self, const StringW* key);

  FOG_CAPI_METHOD(void, hash_stringw_stringw_dFree)(HashUntypedData* d);

  HashUntypedVTable* hash_stringw_stringw_vTable;
  HashUntyped* hash_stringw_stringw_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - Hash<StringW, Var>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(hash_stringw_var_ctor)(HashUntyped* self);
  FOG_CAPI_DTOR(hash_stringw_var_dtor)(HashUntyped* self);

  FOG_CAPI_METHOD(const Var*, hash_stringw_var_getStubA)(const HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(const Var*, hash_stringw_var_getStubW)(const HashUntyped* self, const StubW* key);
  FOG_CAPI_METHOD(const Var*, hash_stringw_var_getStringW)(const HashUntyped* self, const StringW* key);

  FOG_CAPI_METHOD(Var*, hash_stringw_var_useStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(Var*, hash_stringw_var_useStubW)(HashUntyped* self, const StubW* key);
  FOG_CAPI_METHOD(Var*, hash_stringw_var_useStringW)(HashUntyped* self, const StringW* key);

  FOG_CAPI_METHOD(err_t, hash_stringw_var_putStubA)(HashUntyped* self, const StubA* key, const Var* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringw_var_putStubW)(HashUntyped* self, const StubW* key, const Var* item, bool replace);
  FOG_CAPI_METHOD(err_t, hash_stringw_var_putStringW)(HashUntyped* self, const StringW* key, const Var* item, bool replace);

  FOG_CAPI_METHOD(err_t, hash_stringw_var_removeStubA)(HashUntyped* self, const StubA* key);
  FOG_CAPI_METHOD(err_t, hash_stringw_var_removeStubW)(HashUntyped* self, const StubW* key);
  FOG_CAPI_METHOD(err_t, hash_stringw_var_removeStringW)(HashUntyped* self, const StringW* key);

  FOG_CAPI_METHOD(void, hash_stringw_var_dFree)(HashUntypedData* d);

  HashUntypedVTable* hash_stringw_var_vTable;
  HashUntyped* hash_stringw_var_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - HashHelper]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(size_t, hashhelper_calcExpandCapacity)(size_t capacity);
  FOG_CAPI_STATIC(size_t, hashhelper_calcShrinkCapacity)(size_t capacity);

  // --------------------------------------------------------------------------
  // [Core/Tools - HashIterator<...>]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(bool, hashiterator_start)(HashUntypedIterator* i);
  FOG_CAPI_METHOD(bool, hashiterator_next)(HashUntypedIterator* i);

  // --------------------------------------------------------------------------
  // [Core/Tools - HashUtil]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(uint32_t, hashutil_hashBinary)(const void* src, size_t length);
  FOG_CAPI_STATIC(uint32_t, hashutil_hashVectorD)(const void* src, size_t length);
  FOG_CAPI_STATIC(uint32_t, hashutil_hashVectorQ)(const void* src, size_t length);

  FOG_CAPI_STATIC(uint32_t, hashutil_hashStubA)(const StubA* item);
  FOG_CAPI_STATIC(uint32_t, hashutil_hashStubW)(const StubW* item);

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Untyped>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(list_untyped_ctor)(ListUntyped* self);
  FOG_CAPI_DTOR(list_untyped_ctorCopy)(ListUntyped* self, const ListUntyped* other);

  FOG_CAPI_METHOD(size_t, list_untyped_indexOf_4B)(const ListUntyped* self, const Range* range, const void* item);
  FOG_CAPI_METHOD(size_t, list_untyped_indexOf_8B)(const ListUntyped* self, const Range* range, const void* item);
  FOG_CAPI_METHOD(size_t, list_untyped_indexOf_16B)(const ListUntyped* self, const Range* range, const void* item);
  FOG_CAPI_METHOD(size_t, list_untyped_lastIndexOf_4B)(const ListUntyped* self, const Range* range, const void* item);
  FOG_CAPI_METHOD(size_t, list_untyped_lastIndexOf_8B)(const ListUntyped* self, const Range* range, const void* item);
  FOG_CAPI_METHOD(size_t, list_untyped_lastIndexOf_16B)(const ListUntyped* self, const Range* range, const void* item);
  FOG_CAPI_METHOD(bool, list_untyped_binaryEq)(const ListUntyped* a, const ListUntyped* b, size_t szItemT);
  FOG_CAPI_METHOD(bool, list_untyped_customEq)(const ListUntyped* a, const ListUntyped* b, size_t szItemT, EqFunc eqFunc);

  FOG_CAPI_STATIC(ListUntypedData*, list_untyped_dCreate)(size_t szItemT, size_t capacity);

  ListUntyped* list_untyped_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Simple>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(list_simple_ctorSlice)(ListUntyped* self, size_t szItemT, const ListUntyped* other, const Range* range);
  FOG_CAPI_CTOR(list_simple_dtor)(ListUntyped* self);

  FOG_CAPI_METHOD(err_t, list_simple_detach)(ListUntyped* self, size_t szItemT);
  FOG_CAPI_METHOD(err_t, list_simple_reserve)(ListUntyped* self, size_t szItemT, size_t capacity);
  FOG_CAPI_METHOD(void*, list_simple_prepare)(ListUntyped* self, size_t szItemT, uint32_t cntOp, size_t length);
  FOG_CAPI_METHOD(err_t, list_simple_growBoth)(ListUntyped* self, size_t szItemT, size_t left, size_t right);
  FOG_CAPI_METHOD(err_t, list_simple_growLeft)(ListUntyped* self, size_t szItemT, size_t length);
  FOG_CAPI_METHOD(err_t, list_simple_growRight)(ListUntyped* self, size_t szItemT, size_t length);
  FOG_CAPI_METHOD(void, list_simple_squeeze)(ListUntyped* self, size_t szItemT);
  FOG_CAPI_METHOD(err_t, list_simple_setAt)(ListUntyped* self, size_t szItemT, size_t index, const void* item);
  FOG_CAPI_METHOD(err_t, list_simple_setAt_4x)(ListUntyped* self, size_t szItemT, size_t index, const void* item);
  FOG_CAPI_METHOD(void, list_simple_clear)(ListUntyped* self);
  FOG_CAPI_METHOD(void, list_simple_reset)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_simple_opList)(ListUntyped* self, size_t szItemT, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_simple_opData)(ListUntyped* self, size_t szItemT, uint32_t cntOp, const void* data, size_t dataLength);
  FOG_CAPI_METHOD(err_t, list_simple_appendItem)(ListUntyped* self, size_t szItemT, const void* item);
  FOG_CAPI_METHOD(err_t, list_simple_appendItem_4x)(ListUntyped* self, size_t szItemT, const void* item);
  FOG_CAPI_METHOD(err_t, list_simple_insertItem)(ListUntyped* self, size_t szItemT, size_t index, const void* item);
  FOG_CAPI_METHOD(err_t, list_simple_insertItem_4x)(ListUntyped* self, size_t szItemT, size_t index, const void* item);
  FOG_CAPI_METHOD(err_t, list_simple_remove)(ListUntyped* self, size_t szItemT, const Range* range);
  FOG_CAPI_METHOD(err_t, list_simple_replace)(ListUntyped* self, size_t szItemT, const Range* range, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_simple_slice)(ListUntyped* self, size_t szItemT, const Range* range);
  FOG_CAPI_METHOD(err_t, list_simple_sort)(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareFunc compareFunc);
  FOG_CAPI_METHOD(err_t, list_simple_sortEx)(ListUntyped* self, size_t szItemT, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  FOG_CAPI_METHOD(err_t, list_simple_swapItems)(ListUntyped* self, size_t szItemT, size_t index1, size_t index2);
  FOG_CAPI_METHOD(err_t, list_simple_swapItems_4x)(ListUntyped* self, size_t szItemT, size_t index1, size_t index2);
  FOG_CAPI_METHOD(void, list_simple_copy)(ListUntyped* self, const ListUntyped* other);

  FOG_CAPI_STATIC(void, list_simple_dRelease)(ListUntypedData* d);
  FOG_CAPI_STATIC(void, list_simple_dFree)(ListUntypedData* d);

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Movable>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(list_unknown_ctorSlice)(ListUntyped* self, const ListUntypedVTable* v, const ListUntyped* other, const Range* range);
  FOG_CAPI_DTOR(list_unknown_dtor)(ListUntyped* self, const ListUntypedVTable* v);

  FOG_CAPI_METHOD(err_t, list_unknown_detach)(ListUntyped* self, const ListUntypedVTable* v);
  FOG_CAPI_METHOD(err_t, list_unknown_reserve)(ListUntyped* self, const ListUntypedVTable* v, size_t capacity);
  FOG_CAPI_METHOD(void*, list_unknown_prepare)(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, size_t length);
  FOG_CAPI_METHOD(void*, list_unknown_prepareAppendItem)(ListUntyped* self, const ListUntypedVTable* v);
  FOG_CAPI_METHOD(void*, list_unknown_prepareInsertItem)(ListUntyped* self, const ListUntypedVTable* v, size_t index);
  FOG_CAPI_METHOD(err_t, list_unknown_growBoth)(ListUntyped* self, const ListUntypedVTable* v, size_t left, size_t right);
  FOG_CAPI_METHOD(err_t, list_unknown_growLeft)(ListUntyped* self, const ListUntypedVTable* v, size_t length);
  FOG_CAPI_METHOD(err_t, list_unknown_growRight)(ListUntyped* self, const ListUntypedVTable* v, size_t length);
  FOG_CAPI_METHOD(void, list_unknown_squeeze)(ListUntyped* self, const ListUntypedVTable* v);
  FOG_CAPI_METHOD(void, list_unknown_clear)(ListUntyped* self, const ListUntypedVTable* v);
  FOG_CAPI_METHOD(void, list_unknown_reset)(ListUntyped* self, const ListUntypedVTable* v);
  FOG_CAPI_METHOD(err_t, list_unknown_opList)(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_unknown_opData)(ListUntyped* self, const ListUntypedVTable* v, uint32_t cntOp, const void* data, size_t dataLength);
  FOG_CAPI_METHOD(err_t, list_unknown_remove)(ListUntyped* self, const ListUntypedVTable* v, const Range* range);
  FOG_CAPI_METHOD(err_t, list_unknown_replace)(ListUntyped* self, const ListUntypedVTable* v, const Range* range, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_unknown_slice)(ListUntyped* self, const ListUntypedVTable* v, const Range* range);
  FOG_CAPI_METHOD(err_t, list_unknown_sort)(ListUntyped* self, const ListUntypedVTable* v, uint32_t sortOrder, CompareFunc compareFunc);
  FOG_CAPI_METHOD(err_t, list_unknown_sortEx)(ListUntyped* self, const ListUntypedVTable* v, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  FOG_CAPI_METHOD(void, list_unknown_copy)(ListUntyped* self, const ListUntypedVTable* v, const ListUntyped* other);

  FOG_CAPI_STATIC(void, list_unknown_dRelease)(ListUntypedData* d, const ListUntypedVTable* v);
  FOG_CAPI_STATIC(void, list_unknown_dFree)(ListUntypedData* d, const ListUntypedVTable* v);

  // --------------------------------------------------------------------------
  // [Core/Tools - List<float>]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, list_float_opListD)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_float_opDataD)(ListUntyped* self, uint32_t cntOp, const void* data, size_t dataLength);

  // --------------------------------------------------------------------------
  // [Core/Tools - List<double>]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, list_double_opListF)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_double_opDataF)(ListUntyped* self, uint32_t cntOp, const void* data, size_t dataLength);

  // --------------------------------------------------------------------------
  // [Core/Tools - List<StringA>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(list_stringa_ctor)(ListUntyped* self);
  FOG_CAPI_CTOR(list_stringa_ctorSlice)(ListUntyped* self, const ListUntyped* other, const Range* range);
  FOG_CAPI_DTOR(list_stringa_dtor)(ListUntyped* self);

  FOG_CAPI_METHOD(err_t, list_stringa_detach)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_stringa_reserve)(ListUntyped* self, size_t capacity);
  FOG_CAPI_METHOD(err_t, list_stringa_growBoth)(ListUntyped* self, size_t left, size_t right);
  FOG_CAPI_METHOD(err_t, list_stringa_growLeft)(ListUntyped* self, size_t length);
  FOG_CAPI_METHOD(err_t, list_stringa_growRight)(ListUntyped* self, size_t length);
  FOG_CAPI_METHOD(void, list_stringa_squeeze)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_stringa_setAtStubA)(ListUntyped* self, size_t index, const StubA* item);
  FOG_CAPI_METHOD(err_t, list_stringa_setAtStringA)(ListUntyped* self, size_t index, const StringA* item);
  FOG_CAPI_METHOD(void, list_stringa_clear)(ListUntyped* self);
  FOG_CAPI_METHOD(void, list_stringa_reset)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_stringa_opList)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_stringa_appendStubA)(ListUntyped* self, const StubA* item);
  FOG_CAPI_METHOD(err_t, list_stringa_appendStringA)(ListUntyped* self, const StringA* item);
  FOG_CAPI_METHOD(err_t, list_stringa_insertStubA)(ListUntyped* self, size_t index, const StubA* item);
  FOG_CAPI_METHOD(err_t, list_stringa_insertStringA)(ListUntyped* self, size_t index, const StringA* item);
  FOG_CAPI_METHOD(err_t, list_stringa_remove)(ListUntyped* self, const Range* range);
  FOG_CAPI_METHOD(err_t, list_stringa_replace)(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_stringa_slice)(ListUntyped* self, const Range* range);
  FOG_CAPI_METHOD(size_t, list_stringa_indexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  FOG_CAPI_METHOD(size_t, list_stringa_indexOfStringA)(const ListUntyped* self, const Range* range, const StringA* item);
  FOG_CAPI_METHOD(size_t, list_stringa_lastIndexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  FOG_CAPI_METHOD(size_t, list_stringa_lastIndexOfStringA)(const ListUntyped* self, const Range* range, const StringA* item);
  FOG_CAPI_METHOD(err_t, list_stringa_sort)(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc);
  FOG_CAPI_METHOD(err_t, list_stringa_sortEx)(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  FOG_CAPI_METHOD(err_t, list_stringa_swapItems)(ListUntyped* self, size_t index1, size_t index2);

  ListUntypedVTable *list_stringa_vTable;
  ListUntyped* list_stringa_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - List<StringW>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(list_stringw_ctor)(ListUntyped* self);
  FOG_CAPI_CTOR(list_stringw_ctorSlice)(ListUntyped* self, const ListUntyped* other, const Range* range);
  FOG_CAPI_DTOR(list_stringw_dtor)(ListUntyped* self);

  FOG_CAPI_METHOD(err_t, list_stringw_detach)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_stringw_reserve)(ListUntyped* self, size_t capacity);
  FOG_CAPI_METHOD(err_t, list_stringw_growBoth)(ListUntyped* self, size_t left, size_t right);
  FOG_CAPI_METHOD(err_t, list_stringw_growLeft)(ListUntyped* self, size_t length);
  FOG_CAPI_METHOD(err_t, list_stringw_growRight)(ListUntyped* self, size_t length);
  FOG_CAPI_METHOD(void, list_stringw_squeeze)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_stringw_setAtStubA)(ListUntyped* self, size_t index, const StubA* item);
  FOG_CAPI_METHOD(err_t, list_stringw_setAtStubW)(ListUntyped* self, size_t index, const StubW* item);
  FOG_CAPI_METHOD(err_t, list_stringw_setAtStringW)(ListUntyped* self, size_t index, const StringW* item);
  FOG_CAPI_METHOD(void, list_stringw_clear)(ListUntyped* self);
  FOG_CAPI_METHOD(void, list_stringw_reset)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_stringw_opList)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_stringw_appendStubA)(ListUntyped* self, const StubA* item);
  FOG_CAPI_METHOD(err_t, list_stringw_appendStubW)(ListUntyped* self, const StubW* item);
  FOG_CAPI_METHOD(err_t, list_stringw_appendStringW)(ListUntyped* self, const StringW* item);
  FOG_CAPI_METHOD(err_t, list_stringw_insertStubA)(ListUntyped* self, size_t index, const StubA* item);
  FOG_CAPI_METHOD(err_t, list_stringw_insertStubW)(ListUntyped* self, size_t index, const StubW* item);
  FOG_CAPI_METHOD(err_t, list_stringw_insertStringW)(ListUntyped* self, size_t index, const StringW* item);
  FOG_CAPI_METHOD(err_t, list_stringw_remove)(ListUntyped* self, const Range* range);
  FOG_CAPI_METHOD(err_t, list_stringw_replace)(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_stringw_slice)(ListUntyped* self, const Range* range);
  FOG_CAPI_METHOD(size_t, list_stringw_indexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  FOG_CAPI_METHOD(size_t, list_stringw_indexOfStubW)(const ListUntyped* self, const Range* range, const StubW* item);
  FOG_CAPI_METHOD(size_t, list_stringw_indexOfStringW)(const ListUntyped* self, const Range* range, const StringW* item);
  FOG_CAPI_METHOD(size_t, list_stringw_lastIndexOfStubA)(const ListUntyped* self, const Range* range, const StubA* item);
  FOG_CAPI_METHOD(size_t, list_stringw_lastIndexOfStubW)(const ListUntyped* self, const Range* range, const StubW* item);
  FOG_CAPI_METHOD(size_t, list_stringw_lastIndexOfStringW)(const ListUntyped* self, const Range* range, const StringW* item);
  FOG_CAPI_METHOD(err_t, list_stringw_sort)(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc);
  FOG_CAPI_METHOD(err_t, list_stringw_sortEx)(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  FOG_CAPI_METHOD(err_t, list_stringw_swapItems)(ListUntyped* self, size_t index1, size_t index2);

  ListUntypedVTable *list_stringw_vTable;
  ListUntyped* list_stringw_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - List<Var>]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(list_var_ctor)(ListUntyped* self);
  FOG_CAPI_CTOR(list_var_ctorSlice)(ListUntyped* self, const ListUntyped* other, const Range* range);
  FOG_CAPI_DTOR(list_var_dtor)(ListUntyped* self);

  FOG_CAPI_METHOD(err_t, list_var_detach)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_var_reserve)(ListUntyped* self, size_t capacity);
  FOG_CAPI_METHOD(err_t, list_var_growBoth)(ListUntyped* self, size_t left, size_t right);
  FOG_CAPI_METHOD(err_t, list_var_growLeft)(ListUntyped* self, size_t length);
  FOG_CAPI_METHOD(err_t, list_var_growRight)(ListUntyped* self, size_t length);
  FOG_CAPI_METHOD(void, list_var_squeeze)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_var_setAt)(ListUntyped* self, size_t index, const Var* item);
  FOG_CAPI_METHOD(void, list_var_clear)(ListUntyped* self);
  FOG_CAPI_METHOD(void, list_var_reset)(ListUntyped* self);
  FOG_CAPI_METHOD(err_t, list_var_opList)(ListUntyped* self, uint32_t cntOp, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_var_append)(ListUntyped* self, const Var* item);
  FOG_CAPI_METHOD(err_t, list_var_insert)(ListUntyped* self, size_t index, const Var* item);
  FOG_CAPI_METHOD(err_t, list_var_remove)(ListUntyped* self, const Range* range);
  FOG_CAPI_METHOD(err_t, list_var_replace)(ListUntyped* self, const Range* range, const ListUntyped* src, const Range* srcRange);
  FOG_CAPI_METHOD(err_t, list_var_slice)(ListUntyped* self, const Range* range);
  FOG_CAPI_METHOD(size_t, list_var_indexOf)(const ListUntyped* self, const Range* range, const Var* item);
  FOG_CAPI_METHOD(size_t, list_var_lastIndexOf)(const ListUntyped* self, const Range* range, const Var* item);
  FOG_CAPI_METHOD(err_t, list_var_sort)(ListUntyped* self, uint32_t sortOrder, CompareFunc compareFunc);
  FOG_CAPI_METHOD(err_t, list_var_sortEx)(ListUntyped* self, uint32_t sortOrder, CompareExFunc compareFunc, const void* data);
  FOG_CAPI_METHOD(err_t, list_var_swapItems)(ListUntyped* self, size_t index1, size_t index2);

  ListUntypedVTable *list_var_vTable;
  ListUntyped* list_var_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - Locale]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(locale_ctor)(Locale* self);
  FOG_CAPI_CTOR(locale_ctorCopy)(Locale* self, const Locale* other);
  FOG_CAPI_CTOR(locale_ctorString)(Locale* self, const StringW* name);
  FOG_CAPI_DTOR(locale_dtor)(Locale* self);

  FOG_CAPI_METHOD(err_t, locale_detach)(Locale* self);
  FOG_CAPI_METHOD(void, locale_reset)(Locale* self);
  FOG_CAPI_METHOD(err_t, locale_setChar)(Locale* self, uint32_t id, uint16_t ch);
  FOG_CAPI_METHOD(err_t, locale_create)(Locale* self, const StringW* name);
  FOG_CAPI_METHOD(err_t, locale_copy)(Locale* self, const Locale* other);

  FOG_CAPI_STATIC(bool, locale_eq)(const Locale* a, const Locale* b);
  FOG_CAPI_STATIC(LocaleData*, locale_dCreate)(void);
  FOG_CAPI_STATIC(void, locale_dFree)(LocaleData* d);

  Locale* locale_oPosix;
  Locale* locale_oUser;

  // --------------------------------------------------------------------------
  // [Core/Tools - RegExpA]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(regexpa_ctor)(RegExpA* self);
  FOG_CAPI_CTOR(regexpa_ctorCopy)(RegExpA* self, const RegExpA* other);
  FOG_CAPI_DTOR(regexpa_dtor)(RegExpA* self);

  FOG_CAPI_METHOD(void, regexpa_reset)(RegExpA* self);
  FOG_CAPI_METHOD(void, regexpa_copy)(RegExpA* self, const RegExpA* other);
  FOG_CAPI_METHOD(err_t, regexpa_createStubA)(RegExpA* self, const StubA* stub, uint32_t type, uint32_t cs);
  FOG_CAPI_METHOD(err_t, regexpa_createStringA)(RegExpA* self, const StringA* stub, uint32_t type, uint32_t cs);
  FOG_CAPI_METHOD(bool, regexpa_indexIn)(const RegExpA* self, const char* sData, size_t sLength, const Range* sRange, Range* out);
  FOG_CAPI_METHOD(bool, regexpa_lastIndexIn)(const RegExpA* self, const char* sData, size_t sLength, const Range* sRange, Range* out);

  RegExpA* regexpa_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - RegExpW]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(regexpw_ctor)(RegExpW* self);
  FOG_CAPI_CTOR(regexpw_ctorCopy)(RegExpW* self, const RegExpW* other);
  FOG_CAPI_DTOR(regexpw_dtor)(RegExpW* self);

  FOG_CAPI_METHOD(void, regexpw_reset)(RegExpW* self);
  FOG_CAPI_METHOD(void, regexpw_copy)(RegExpW* self, const RegExpW* other);
  FOG_CAPI_METHOD(err_t, regexpw_createStubA)(RegExpW* self, const StubA* stub, uint32_t type, uint32_t cs);
  FOG_CAPI_METHOD(err_t, regexpw_createStubW)(RegExpW* self, const StubW* stub, uint32_t type, uint32_t cs);
  FOG_CAPI_METHOD(err_t, regexpw_createStringW)(RegExpW* self, const StringW* stub, uint32_t type, uint32_t cs);
  FOG_CAPI_METHOD(bool, regexpw_indexIn)(const RegExpW* self, const CharW* sData, size_t sLength, const Range* sRange, Range* out);
  FOG_CAPI_METHOD(bool, regexpw_lastIndexIn)(const RegExpW* self, const CharW* sData, size_t sLength, const Range* sRange, Range* out);

  RegExpW* regexpw_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - StringA]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(stringa_ctor)(StringA* self);
  FOG_CAPI_CTOR(stringa_ctorStubA)(StringA* self, const StubA* stub);
  FOG_CAPI_CTOR(stringa_ctorStubA2)(StringA* self, const StubA* a, const StubA* b);
  FOG_CAPI_CTOR(stringa_ctorCopyA)(StringA* self, const StringA* other);
  FOG_CAPI_CTOR(stringa_ctorCopyA2)(StringA* self, const StringA* a, const StringA* b);
  FOG_CAPI_CTOR(stringa_ctorSubstr)(StringA* self, const StringA* other, const Range* range);
  FOG_CAPI_CTOR(stringa_ctorU32)(StringA* self, uint32_t n, bool isUnsigned);
  FOG_CAPI_CTOR(stringa_ctorU64)(StringA* self, uint64_t n, bool isUnsigned);
  FOG_CAPI_CTOR(stringa_ctorDouble)(StringA* self, double d);
  FOG_CAPI_DTOR(stringa_dtor)(StringA* self);

  FOG_CAPI_METHOD(err_t, stringa_detach)(StringA* self);
  FOG_CAPI_METHOD(err_t, stringa_reserve)(StringA* self, size_t capacity);
  FOG_CAPI_METHOD(err_t, stringa_resize)(StringA* self, size_t length);
  FOG_CAPI_METHOD(err_t, stringa_truncate)(StringA* self, size_t length);
  FOG_CAPI_METHOD(void, stringa_squeeze)(StringA* self);
  FOG_CAPI_METHOD(char*, stringa_prepare)(StringA* self, uint32_t cntOp, size_t length);
  FOG_CAPI_METHOD(char*, stringa_add)(StringA* self, size_t length);
  FOG_CAPI_METHOD(void, stringa_clear)(StringA* self);
  FOG_CAPI_METHOD(void, stringa_reset)(StringA* self);
  FOG_CAPI_METHOD(uint32_t, stringa_getHashCode)(const StringA* self);
  FOG_CAPI_METHOD(err_t, stringa_setStubA)(StringA* self, const StubA* stub);
  FOG_CAPI_METHOD(err_t, stringa_setStringA)(StringA* self, const StringA* other);
  FOG_CAPI_METHOD(err_t, stringa_setStringExA)(StringA* self, const StringA* other, const Range* range);
  FOG_CAPI_METHOD(err_t, stringa_setDeep)(StringA* self, const StringA* other);
  FOG_CAPI_METHOD(err_t, stringa_appendStubA)(StringA* self, const StubA* stub);
  FOG_CAPI_METHOD(err_t, stringa_appendStringA)(StringA* self, const StringA* other);
  FOG_CAPI_METHOD(err_t, stringa_appendStringExA)(StringA* self, const StringA* other, const Range* range);
  FOG_CAPI_METHOD(err_t, stringa_opFill)(StringA* self, uint32_t cntOp, char ch, size_t len);
  FOG_CAPI_METHOD(err_t, stringa_opBool)(StringA* self, uint32_t cntOp, bool b);
  FOG_CAPI_METHOD(err_t, stringa_opI32)(StringA* self, uint32_t cntOp, int32_t n);
  FOG_CAPI_METHOD(err_t, stringa_opI32Ex)(StringA* self, uint32_t cntOp, int32_t n, const FormatInt* fmt);
  FOG_CAPI_METHOD(err_t, stringa_opU32)(StringA* self, uint32_t cntOp, uint32_t n);
  FOG_CAPI_METHOD(err_t, stringa_opU32Ex)(StringA* self, uint32_t cntOp, uint32_t n, const FormatInt* fmt);
  FOG_CAPI_METHOD(err_t, stringa_opI64)(StringA* self, uint32_t cntOp, int64_t n);
  FOG_CAPI_METHOD(err_t, stringa_opI64Ex)(StringA* self, uint32_t cntOp, int64_t n, const FormatInt* fmt);
  FOG_CAPI_METHOD(err_t, stringa_opU64)(StringA* self, uint32_t cntOp, uint64_t n);
  FOG_CAPI_METHOD(err_t, stringa_opU64Ex)(StringA* self, uint32_t cntOp, uint64_t n, const FormatInt* fmt);
  FOG_CAPI_METHOD(err_t, stringa_opDouble)(StringA* self, uint32_t cntOp, double d);
  FOG_CAPI_METHOD(err_t, stringa_opDoubleEx)(StringA* self, uint32_t cntOp, double d, const FormatReal* fmt);
  FOG_CAPI_METHOD(err_t, stringa_opVFormatStubA)(StringA* self, uint32_t cntOp, const StubA* fmt, const TextCodec* tc, va_list ap);
  FOG_CAPI_METHOD(err_t, stringa_opVFormatStringA)(StringA* self, uint32_t cntOp, const StringA* fmt, const TextCodec* tc, va_list ap);
  FOG_CAPI_METHOD(err_t, stringa_opZFormatStubA)(StringA* self, uint32_t cntOp, const StubA* fmt, char lex, const StringA* args, size_t argsLength);
  FOG_CAPI_METHOD(err_t, stringa_opZFormatStringA)(StringA* self, uint32_t cntOp, const StringA* fmt, char lex, const StringA* args, size_t argsLength);
  FOG_CAPI_METHOD(err_t, stringa_opNormalizeSlashesA)(StringA* self, uint32_t cntOp, const StringA* other, const Range* range, uint32_t slashForm);
  FOG_CAPI_METHOD(err_t, stringa_prependChars)(StringA* self, char ch, size_t len);
  FOG_CAPI_METHOD(err_t, stringa_prependStubA)(StringA* self, const StubA* stub);
  FOG_CAPI_METHOD(err_t, stringa_prependStringA)(StringA* self, const StringA* other);
  FOG_CAPI_METHOD(err_t, stringa_insertChars)(StringA* self, size_t index, char ch, size_t length);
  FOG_CAPI_METHOD(err_t, stringa_insertStubA)(StringA* self, size_t index, const StubA* stub);
  FOG_CAPI_METHOD(err_t, stringa_insertStringA)(StringA* self, size_t index, const StringA* other);
  FOG_CAPI_METHOD(err_t, stringa_removeRange)(StringA* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringa_removeRangeList)(StringA* self, const Range* range, size_t rangeLength);
  FOG_CAPI_METHOD(err_t, stringa_removeChar)(StringA* self, const Range* range, char ch, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringa_removeStubA)(StringA* self, const Range* range, const StubA* stub, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringa_removeStringA)(StringA* self, const Range* range, const StringA* other, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringa_removeRegExpA)(StringA* self, const Range* range, const RegExpA* re);
  FOG_CAPI_METHOD(err_t, stringa_replaceRangeStubA)(StringA* self, const Range* range, const StubA* replacement);
  FOG_CAPI_METHOD(err_t, stringa_replaceRangeStringA)(StringA* self, const Range* range, const StringA* replacement);
  FOG_CAPI_METHOD(err_t, stringa_replaceRangeListStubA)(StringA* self, const Range* range, size_t rangeLength, const StubA* replacement);
  FOG_CAPI_METHOD(err_t, stringa_replaceRangeListStringA)(StringA* self, const Range* range, size_t rangeLength, const StringA* replacement);
  FOG_CAPI_METHOD(err_t, stringa_replaceChar)(StringA* self, const Range* range, char before, char after, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringa_replaceStringA)(StringA* self, const Range* range, const StringA* pattern, const StringA* replacement, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringa_replaceRegExpA)(StringA* self, const Range* range, const RegExpA* re, const StringA* replacement);
  FOG_CAPI_METHOD(err_t, stringa_lower)(StringA* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringa_upper)(StringA* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringa_trim)(StringA* self);
  FOG_CAPI_METHOD(err_t, stringa_simplify)(StringA* self);
  FOG_CAPI_METHOD(err_t, stringa_justify)(StringA* self, size_t n, char ch, uint32_t flags);
  FOG_CAPI_METHOD(err_t, stringa_splitChar)(List<StringA>* dst, uint32_t cntOp, const StringA* src, const Range* range, char ch, uint32_t splitBehavior, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringa_splitStringA)(List<StringA>* dst, uint32_t cntOp, const StringA* src, const Range* range, const StringA* pattern, uint32_t splitBehavior, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringa_splitRegExpA)(List<StringA>* dst, uint32_t cntOp, const StringA* src, const Range* range, const RegExpA* re, uint32_t splitBehavior);
  FOG_CAPI_METHOD(err_t, stringa_slice)(StringA* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringa_joinChar)(StringA* self, const StringA* list, size_t listLength, char sep);
  FOG_CAPI_METHOD(err_t, stringa_joinStringA)(StringA* self, const StringA* list, size_t listLength, const StringA* sep);
  FOG_CAPI_METHOD(err_t, stringa_parseBool)(const StringA* self, bool* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseI8)(const StringA* self, int8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseU8)(const StringA* self, uint8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseI16)(const StringA* self, int16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseU16)(const StringA* self, uint16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseI32)(const StringA* self, int32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseU32)(const StringA* self, uint32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseI64)(const StringA* self, int64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseU64)(const StringA* self, uint64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseFloat)(const StringA* self, float* dst, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringa_parseDouble)(const StringA* self, double* dst, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(size_t, stringa_countOfChar)(const StringA* self, const Range* range, char ch, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_countOfStubA)(const StringA* self, const Range* range, const StubA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_countOfStringA)(const StringA* self, const Range* range, const StringA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_countOfRegExpA)(const StringA* self, const Range* range, const RegExpA* re);
  FOG_CAPI_METHOD(size_t, stringa_indexOfChar)(const StringA* self, const Range* range, char ch, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_indexOfStubA)(const StringA* self, const Range* range, const StubA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_indexOfStringA)(const StringA* self, const Range* range, const StringA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_indexOfRegExpA)(const StringA* self, const Range* range, const RegExpA* re);
  FOG_CAPI_METHOD(size_t, stringa_indexOfAnyCharA)(const StringA* self, const Range* range, const char* charArray, size_t charLength, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_lastIndexOfChar)(const StringA* self, const Range* range, char ch, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_lastIndexOfStubA)(const StringA* self, const Range* range, const StubA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_lastIndexOfStringA)(const StringA* self, const Range* range, const StringA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringa_lastIndexOfRegExpA)(const StringA* self, const Range* range, const RegExpA* re);
  FOG_CAPI_METHOD(size_t, stringa_lastIndexOfAnyCharA)(const StringA* self, const Range* range, const char* charArray, size_t charLength, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringa_startsWithChar)(const StringA* self, char ch, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringa_startsWithStubA)(const StringA* self, const StubA* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringa_startsWithStringA)(const StringA* self, const StringA* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringa_startsWithRegExpA)(const StringA* self, const RegExpA* re);
  FOG_CAPI_METHOD(bool, stringa_endsWithChar)(const StringA* self, char ch, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringa_endsWithStubA)(const StringA* self, const StubA* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringa_endsWithStringA)(const StringA* self, const StringA* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringa_endsWithRegExpA)(const StringA* self, const RegExpA* re);
  FOG_CAPI_METHOD(bool, stringa_eqStubA)(const StringA* a, const StubA* b);

  FOG_CAPI_STATIC(bool, stringa_eqStringA)(const StringA* a, const StringA* b);
  FOG_CAPI_STATIC(bool, stringa_eqStubExA)(const StringA* a, const StubA* b, uint32_t cs);
  FOG_CAPI_STATIC(bool, stringa_eqStringExA)(const StringA* a, const StringA* b, uint32_t cs);
  FOG_CAPI_STATIC(int, stringa_compareStubA)(const StringA* a, const StubA* b);
  FOG_CAPI_STATIC(int, stringa_compareStringA)(const StringA* a, const StringA* b);
  FOG_CAPI_STATIC(int, stringa_compareStubExA)(const StringA* a, const StubA* b, uint32_t cs);
  FOG_CAPI_STATIC(int, stringa_compareStringExA)(const StringA* a, const StringA* b, uint32_t cs);

  FOG_CAPI_METHOD(err_t, stringa_validateUtf8)(const StringA* self, size_t* invalid);
  FOG_CAPI_METHOD(err_t, stringa_getUcsLength)(const StringA* self, size_t* ucsLength);
  FOG_CAPI_METHOD(err_t, stringa_hexDecode)(StringA* dst, uint32_t cntOp, const StringA* src);
  FOG_CAPI_METHOD(err_t, stringa_hexEncode)(StringA* dst, uint32_t cntOp, const StringA* src, uint32_t textCase);
  FOG_CAPI_METHOD(err_t, stringa_base64DecodeStringA)(StringA* dst, uint32_t cntOp, const StringA* src);
  FOG_CAPI_METHOD(err_t, stringa_base64DecodeStringW)(StringA* dst, uint32_t cntOp, const StringW* src);
  FOG_CAPI_METHOD(err_t, stringa_base64DecodeDataA)(StringA* dst, uint32_t cntOp, const char* src, size_t srcLength);
  FOG_CAPI_METHOD(err_t, stringa_base64DecodeDataW)(StringA* dst, uint32_t cntOp, const CharW* src, size_t srcLength);
  FOG_CAPI_METHOD(err_t, stringa_base64EncodeStringA)(StringA* dst, uint32_t cntOp, const StringA* src);
  FOG_CAPI_METHOD(err_t, stringa_base64EncodeDataA)(StringA* dst, uint32_t cntOp, const char* src, size_t srcLength);

  FOG_CAPI_STATIC(StringDataA*, stringa_dCreate)(size_t capacity);
  FOG_CAPI_STATIC(StringDataA*, stringa_dCreateStubA)(size_t capacity, const StubA* stub);
  FOG_CAPI_STATIC(StringDataA*, stringa_dAdopt)(void* address, size_t capacity);
  FOG_CAPI_STATIC(StringDataA*, stringa_dAdoptStubA)(void* address, size_t capacity, const StubA* stub);
  FOG_CAPI_STATIC(StringDataA*, stringa_dRealloc)(StringDataA* d, size_t capacity);
  FOG_CAPI_STATIC(void, stringa_dFree)(StringDataA* d);

  StringA* stringa_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - StringW]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(stringw_ctor)(StringW* self);
  FOG_CAPI_CTOR(stringw_ctorStubA)(StringW* self, const StubA* stub);
  FOG_CAPI_CTOR(stringw_ctorStubW)(StringW* self, const StubW* stub);
  FOG_CAPI_CTOR(stringw_ctorCodec)(StringW* self, const StubA* stub, const TextCodec* tc);
  FOG_CAPI_CTOR(stringw_ctorStubA2)(StringW* self, const StubA* a, const StubA* b);
  FOG_CAPI_CTOR(stringw_ctorStubW2)(StringW* self, const StubW* a, const StubW* b);
  FOG_CAPI_CTOR(stringw_ctorCopyW)(StringW* self, const StringW* other);
  FOG_CAPI_CTOR(stringw_ctorCopyW2)(StringW* self, const StringW* a, const StringW* b);
  FOG_CAPI_CTOR(stringw_ctorSubstr)(StringW* self, const StringW* other, const Range* range);
  FOG_CAPI_CTOR(stringw_ctorU32)(StringW* self, uint32_t n, bool isUnsigned);
  FOG_CAPI_CTOR(stringw_ctorU64)(StringW* self, uint64_t n, bool isUnsigned);
  FOG_CAPI_CTOR(stringw_ctorDouble)(StringW* self, double d);
  FOG_CAPI_DTOR(stringw_dtor)(StringW* self);

  FOG_CAPI_METHOD(err_t, stringw_detach)(StringW* self);
  FOG_CAPI_METHOD(err_t, stringw_reserve)(StringW* self, size_t capacity);
  FOG_CAPI_METHOD(err_t, stringw_resize)(StringW* self, size_t length);
  FOG_CAPI_METHOD(err_t, stringw_truncate)(StringW* self, size_t length);
  FOG_CAPI_METHOD(void, stringw_squeeze)(StringW* self);
  FOG_CAPI_METHOD(CharW*, stringw_prepare)(StringW* self, uint32_t cntOp, size_t length);
  FOG_CAPI_METHOD(CharW*, stringw_add)(StringW* self, size_t length);
  FOG_CAPI_METHOD(void, stringw_clear)(StringW* self);
  FOG_CAPI_METHOD(void, stringw_reset)(StringW* self);
  FOG_CAPI_METHOD(uint32_t, stringw_getHashCode)(const StringW* self);
  FOG_CAPI_METHOD(err_t, stringw_setStubA)(StringW* self, const StubA* stub, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_setStubW)(StringW* self, const StubW* stub);
  FOG_CAPI_METHOD(err_t, stringw_setStringA)(StringW* self, const StringA* other, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_setStringW)(StringW* self, const StringW* other);
  FOG_CAPI_METHOD(err_t, stringw_setStringExW)(StringW* self, const StringW* other, const Range* range);
  FOG_CAPI_METHOD(err_t, stringw_setDeep)(StringW* self, const StringW* other);
  FOG_CAPI_METHOD(err_t, stringw_appendStubA)(StringW* self, const StubA* stub, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_appendStubW)(StringW* self, const StubW* stub);
  FOG_CAPI_METHOD(err_t, stringw_appendStringA)(StringW* self, const StringA* other, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_appendStringW)(StringW* self, const StringW* other);
  FOG_CAPI_METHOD(err_t, stringw_appendStringExW)(StringW* self, const StringW* other, const Range* range);
  FOG_CAPI_METHOD(err_t, stringw_opFill)(StringW* self, uint32_t cntOp, uint16_t ch, size_t len);
  FOG_CAPI_METHOD(err_t, stringw_opBool)(StringW* self, uint32_t cntOp, bool b);
  FOG_CAPI_METHOD(err_t, stringw_opI32)(StringW* self, uint32_t cntOp, int32_t n);
  FOG_CAPI_METHOD(err_t, stringw_opI32Ex)(StringW* self, uint32_t cntOp, int32_t n, const FormatInt* fmt, const Locale* locale);
  FOG_CAPI_METHOD(err_t, stringw_opU32)(StringW* self, uint32_t cntOp, uint32_t n);
  FOG_CAPI_METHOD(err_t, stringw_opU32Ex)(StringW* self, uint32_t cntOp, uint32_t n, const FormatInt* fmt, const Locale* locale);
  FOG_CAPI_METHOD(err_t, stringw_opI64)(StringW* self, uint32_t cntOp, int64_t n);
  FOG_CAPI_METHOD(err_t, stringw_opI64Ex)(StringW* self, uint32_t cntOp, int64_t n, const FormatInt* fmt, const Locale* locale);
  FOG_CAPI_METHOD(err_t, stringw_opU64)(StringW* self, uint32_t cntOp, uint64_t n);
  FOG_CAPI_METHOD(err_t, stringw_opU64Ex)(StringW* self, uint32_t cntOp, uint64_t n, const FormatInt* fmt, const Locale* locale);
  FOG_CAPI_METHOD(err_t, stringw_opDouble)(StringW* self, uint32_t cntOp, double d);
  FOG_CAPI_METHOD(err_t, stringw_opDoubleEx)(StringW* self, uint32_t cntOp, double d, const FormatReal* fmt, const Locale* locale);
  FOG_CAPI_METHOD(err_t, stringw_opVFormatStubA)(StringW* self, uint32_t cntOp, const StubA* fmt, const TextCodec* tc, const Locale* locale, va_list ap);
  FOG_CAPI_METHOD(err_t, stringw_opVFormatStubW)(StringW* self, uint32_t cntOp, const StubW* fmt, const TextCodec* tc, const Locale* locale, va_list ap);
  FOG_CAPI_METHOD(err_t, stringw_opVFormatStringW)(StringW* self, uint32_t cntOp, const StringW* fmt, const TextCodec* tc, const Locale* locale, va_list ap);
  FOG_CAPI_METHOD(err_t, stringw_opZFormatStubW)(StringW* self, uint32_t cntOp, const StubW* fmt, uint16_t lex, const StringW* args, size_t argsLength);
  FOG_CAPI_METHOD(err_t, stringw_opZFormatStringW)(StringW* self, uint32_t cntOp, const StringW* fmt, uint16_t lex, const StringW* args, size_t argsLength);
  FOG_CAPI_METHOD(err_t, stringw_opNormalizeSlashesW)(StringW* self, uint32_t cntOp, const StringW* other, const Range* range, uint32_t slashForm);
  FOG_CAPI_METHOD(err_t, stringw_prependChars)(StringW* self, uint16_t ch, size_t len);
  FOG_CAPI_METHOD(err_t, stringw_prependStubA)(StringW* self, const StubA* stub, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_prependStubW)(StringW* self, const StubW* stub);
  FOG_CAPI_METHOD(err_t, stringw_prependStringA)(StringW* self, const StringA* other, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_prependStringW)(StringW* self, const StringW* other);
  FOG_CAPI_METHOD(err_t, stringw_insertChars)(StringW* self, size_t index, uint16_t ch, size_t length);
  FOG_CAPI_METHOD(err_t, stringw_insertStubA)(StringW* self, size_t index, const StubA* stub, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_insertStubW)(StringW* self, size_t index, const StubW* stub);
  FOG_CAPI_METHOD(err_t, stringw_insertStringA)(StringW* self, size_t index, const StringA* other, const TextCodec* tc);
  FOG_CAPI_METHOD(err_t, stringw_insertStringW)(StringW* self, size_t index, const StringW* other);
  FOG_CAPI_METHOD(err_t, stringw_removeRange)(StringW* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringw_removeRangeList)(StringW* self, const Range* range, size_t rangeLength);
  FOG_CAPI_METHOD(err_t, stringw_removeChar)(StringW* self, const Range* range, uint16_t ch, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_removeStubA)(StringW* self, const Range* range, const StubA* stub, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_removeStubW)(StringW* self, const Range* range, const StubW* stub, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_removeStringW)(StringW* self, const Range* range, const StringW* other, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_removeRegExpW)(StringW* self, const Range* range, const RegExpW* re);
  FOG_CAPI_METHOD(err_t, stringw_replaceRangeStubW)(StringW* self, const Range* range, const StubW* replacement);
  FOG_CAPI_METHOD(err_t, stringw_replaceRangeStringW)(StringW* self, const Range* range, const StringW* replacement);
  FOG_CAPI_METHOD(err_t, stringw_replaceRangeListStubW)(StringW* self, const Range* range, size_t rangeLength, const StubW* replacement);
  FOG_CAPI_METHOD(err_t, stringw_replaceRangeListStringW)(StringW* self, const Range* range, size_t rangeLength, const StringW* replacement);
  FOG_CAPI_METHOD(err_t, stringw_replaceChar)(StringW* self, const Range* range, uint16_t before, uint16_t after, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_replaceStringW)(StringW* self, const Range* range, const StringW* pattern, const StringW* replacement, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_replaceRegExpW)(StringW* self, const Range* range, const RegExpW* re, const StringW* replacement);
  FOG_CAPI_METHOD(err_t, stringw_lower)(StringW* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringw_upper)(StringW* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringw_trim)(StringW* self);
  FOG_CAPI_METHOD(err_t, stringw_simplify)(StringW* self);
  FOG_CAPI_METHOD(err_t, stringw_justify)(StringW* self, size_t n, uint16_t ch, uint32_t flags);
  FOG_CAPI_METHOD(err_t, stringw_splitChar)(List<StringW>* dst, uint32_t cntOp, const StringW* src, const Range* range, uint16_t ch, uint32_t splitBehavior, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_splitStringW)(List<StringW>* dst, uint32_t cntOp, const StringW* src, const Range* range, const StringW* pattern, uint32_t splitBehavior, uint32_t cs);
  FOG_CAPI_METHOD(err_t, stringw_splitRegExpW)(List<StringW>* dst, uint32_t cntOp, const StringW* src, const Range* range, const RegExpW* re, uint32_t splitBehavior);
  FOG_CAPI_METHOD(err_t, stringw_slice)(StringW* self, const Range* range);
  FOG_CAPI_METHOD(err_t, stringw_joinChar)(StringW* self, const StringW* list, size_t listLength, uint16_t sep);
  FOG_CAPI_METHOD(err_t, stringw_joinStringW)(StringW* self, const StringW* list, size_t listLength, const StringW* sep);
  FOG_CAPI_METHOD(err_t, stringw_parseBool)(const StringW* self, bool* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseI8)(const StringW* self, int8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseU8)(const StringW* self, uint8_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseI16)(const StringW* self, int16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseU16)(const StringW* self, uint16_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseI32)(const StringW* self, int32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseU32)(const StringW* self, uint32_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseI64)(const StringW* self, int64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseU64)(const StringW* self, uint64_t* dst, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseFloat)(const StringW* self, float* dst, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(err_t, stringw_parseDouble)(const StringW* self, double* dst, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_METHOD(size_t, stringw_countOfChar)(const StringW* self, const Range* range, uint16_t ch, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_countOfStubA)(const StringW* self, const Range* range, const StubA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_countOfStubW)(const StringW* self, const Range* range, const StubW* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_countOfStringW)(const StringW* self, const Range* range, const StringW* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_countOfRegExpW)(const StringW* self, const Range* range, const RegExpW* re);
  FOG_CAPI_METHOD(size_t, stringw_indexOfChar)(const StringW* self, const Range* range, uint16_t ch, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_indexOfStubA)(const StringW* self, const Range* range, const StubA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_indexOfStubW)(const StringW* self, const Range* range, const StubW* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_indexOfStringW)(const StringW* self, const Range* range, const StringW* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_indexOfRegExpW)(const StringW* self, const Range* range, const RegExpW* re);
  FOG_CAPI_METHOD(size_t, stringw_indexOfAnyCharW)(const StringW* self, const Range* range, const CharW* charArray, size_t charLength, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_lastIndexOfChar)(const StringW* self, const Range* range, uint16_t ch, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_lastIndexOfStubA)(const StringW* self, const Range* range, const StubA* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_lastIndexOfStubW)(const StringW* self, const Range* range, const StubW* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_lastIndexOfStringW)(const StringW* self, const Range* range, const StringW* pattern, uint32_t cs);
  FOG_CAPI_METHOD(size_t, stringw_lastIndexOfRegExpW)(const StringW* self, const Range* range, const RegExpW* re);
  FOG_CAPI_METHOD(size_t, stringw_lastIndexOfAnyCharW)(const StringW* self, const Range* range, const CharW* charArray, size_t charLength, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_startsWithChar)(const StringW* self, uint16_t ch, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_startsWithStubA)(const StringW* self, const StubA* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_startsWithStubW)(const StringW* self, const StubW* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_startsWithStringW)(const StringW* self, const StringW* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_startsWithRegExpW)(const StringW* self, const RegExpW* re);
  FOG_CAPI_METHOD(bool, stringw_endsWithChar)(const StringW* self, uint16_t ch, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_endsWithStubA)(const StringW* self, const StubA* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_endsWithStubW)(const StringW* self, const StubW* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_endsWithStringW)(const StringW* self, const StringW* str, uint32_t cs);
  FOG_CAPI_METHOD(bool, stringw_endsWithRegExpW)(const StringW* self, const RegExpW* re);

  FOG_CAPI_STATIC(bool, stringw_eqStubA)(const StringW* a, const StubA* b);
  FOG_CAPI_STATIC(bool, stringw_eqStubW)(const StringW* a, const StubW* b);
  FOG_CAPI_STATIC(bool, stringw_eqStringW)(const StringW* a, const StringW* b);
  FOG_CAPI_STATIC(bool, stringw_eqStubExA)(const StringW* a, const StubA* b, uint32_t cs);
  FOG_CAPI_STATIC(bool, stringw_eqStubExW)(const StringW* a, const StubW* b, uint32_t cs);
  FOG_CAPI_STATIC(bool, stringw_eqStringExW)(const StringW* a, const StringW* b, uint32_t cs);
  FOG_CAPI_STATIC(int, stringw_compareStubA)(const StringW* a, const StubA* b);
  FOG_CAPI_STATIC(int, stringw_compareStubW)(const StringW* a, const StubW* b);
  FOG_CAPI_STATIC(int, stringw_compareStringW)(const StringW* a, const StringW* b);
  FOG_CAPI_STATIC(int, stringw_compareStubExA)(const StringW* a, const StubA* b, uint32_t cs);
  FOG_CAPI_STATIC(int, stringw_compareStubExW)(const StringW* a, const StubW* b, uint32_t cs);
  FOG_CAPI_STATIC(int, stringw_compareStringExW)(const StringW* a, const StringW* b, uint32_t cs);

  FOG_CAPI_METHOD(err_t, stringw_validateUtf16)(const StringW* self, size_t* invalid);
  FOG_CAPI_METHOD(err_t, stringw_getUcsLength)(const StringW* self, size_t* ucsLength);
  FOG_CAPI_METHOD(err_t, stringw_base64EncodeStringA)(StringW* dst, uint32_t cntOp, const StringA* src);
  FOG_CAPI_METHOD(err_t, stringw_base64EncodeDataA)(StringW* dst, uint32_t cntOp, const char* src, size_t srcLength);
  FOG_CAPI_METHOD(err_t, stringw_bswap)(StringW* self);

  FOG_CAPI_STATIC(StringDataW*, stringw_dCreate)(size_t capacity);
  FOG_CAPI_STATIC(StringDataW*, stringw_dCreateStubA)(size_t capacity, const StubA* stub);
  FOG_CAPI_STATIC(StringDataW*, stringw_dCreateStubW)(size_t capacity, const StubW* stub);
  FOG_CAPI_STATIC(StringDataW*, stringw_dAdopt)(void* address, size_t capacity);
  FOG_CAPI_STATIC(StringDataW*, stringw_dAdoptStubA)(void* address, size_t capacity, const StubA* stub);
  FOG_CAPI_STATIC(StringDataW*, stringw_dAdoptStubW)(void* address, size_t capacity, const StubW* stub);
  FOG_CAPI_STATIC(StringDataW*, stringw_dRealloc)(StringDataW* d, size_t capacity);
  FOG_CAPI_STATIC(void, stringw_dFree)(StringDataW* d);

  StringW* stringw_oEmpty;

  // --------------------------------------------------------------------------
  // [Core/Tools - StringUtil]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(void, stringutil_copyA)(char* dst, const char* src, size_t length);
  FOG_CAPI_STATIC(void, stringutil_copyW)(CharW* dst, const CharW* src, size_t length);

  FOG_CAPI_STATIC(err_t, stringutil_latinFromUnicode)(char* dst, const CharW* src, size_t length);
  FOG_CAPI_STATIC(void, stringutil_unicodeFromLatin)(CharW* dst, const char* src, size_t length);

  FOG_CAPI_STATIC(void, stringutil_moveA)(char* dst, const char* src, size_t length);
  FOG_CAPI_STATIC(void, stringutil_moveW)(CharW* dst, const CharW* src, size_t length);

  FOG_CAPI_STATIC(void, stringutil_fillA)(char* dst, char ch, size_t length);
  FOG_CAPI_STATIC(void, stringutil_fillW)(CharW* dst, uint16_t ch, size_t length);

  FOG_CAPI_STATIC(size_t, stringutil_lenA)(const char* src);
  FOG_CAPI_STATIC(size_t, stringutil_lenW)(const CharW* src);

  FOG_CAPI_STATIC(size_t, stringutil_nLenA)(const char* src, size_t max);
  FOG_CAPI_STATIC(size_t, stringutil_nLenW)(const CharW* src, size_t max);

  typedef bool (FOG_CDECL* StringUtil_EqA)(const char* a, const char* b, size_t length);
  typedef bool (FOG_CDECL* StringUtil_EqW)(const CharW* a, const CharW* b, size_t length);
  typedef bool (FOG_CDECL* StringUtil_EqMixed)(const CharW* a, const char* b, size_t length);

  StringUtil_EqA stringutil_eqA[CASE_SENSITIVITY_COUNT];
  StringUtil_EqW stringutil_eqW[CASE_SENSITIVITY_COUNT];
  StringUtil_EqMixed stringutil_eqMixed[CASE_SENSITIVITY_COUNT];

  typedef size_t (FOG_CDECL* StringUtil_CountOfA)(const char* str, size_t length, char ch);
  typedef size_t (FOG_CDECL* StringUtil_CountOfW)(const CharW* str, size_t length, uint16_t ch);

  StringUtil_CountOfA stringutil_countOfA[CASE_SENSITIVITY_COUNT];
  StringUtil_CountOfW stringutil_countOfW[CASE_SENSITIVITY_COUNT];

  typedef size_t (FOG_CDECL* StringUtil_IndexOfCharA)(const char* str, size_t length, char ch);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfCharW)(const CharW* str, size_t length, uint16_t ch);

  StringUtil_IndexOfCharA stringutil_indexOfCharA[CASE_SENSITIVITY_COUNT];
  StringUtil_IndexOfCharW stringutil_indexOfCharW[CASE_SENSITIVITY_COUNT];

  typedef size_t (FOG_CDECL* StringUtil_IndexOfStringA)(const char* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfStringWA)(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfStringW)(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength);

  StringUtil_IndexOfStringA stringutil_indexOfStringA[CASE_SENSITIVITY_COUNT];
  StringUtil_IndexOfStringWA stringutil_indexOfStringWA[CASE_SENSITIVITY_COUNT];
  StringUtil_IndexOfStringW stringutil_indexOfStringW[CASE_SENSITIVITY_COUNT];

  typedef size_t (FOG_CDECL* StringUtil_IndexOfAnyA)(const char* str, size_t length, const char* charArray, size_t charLength);
  typedef size_t (FOG_CDECL* StringUtil_IndexOfAnyW)(const CharW* str, size_t length, const CharW* charArray, size_t charLength);

  StringUtil_IndexOfAnyA stringutil_indexOfAnyA[CASE_SENSITIVITY_COUNT];
  StringUtil_IndexOfAnyW stringutil_indexOfAnyW[CASE_SENSITIVITY_COUNT];

  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfCharA)(const char* str, size_t length, char ch);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfCharW)(const CharW* str, size_t length, uint16_t ch);

  StringUtil_LastIndexOfCharA stringutil_lastIndexOfCharA[CASE_SENSITIVITY_COUNT];
  StringUtil_LastIndexOfCharW stringutil_lastIndexOfCharW[CASE_SENSITIVITY_COUNT];

  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfStringA)(const char* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfStringWA)(const CharW* aStr, size_t aLength, const char* bStr, size_t bLength);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfStringW)(const CharW* aStr, size_t aLength, const CharW* bStr, size_t bLength);

  StringUtil_LastIndexOfStringA stringutil_lastIndexOfStringA[CASE_SENSITIVITY_COUNT];
  StringUtil_LastIndexOfStringWA stringutil_lastIndexOfStringWA[CASE_SENSITIVITY_COUNT];
  StringUtil_LastIndexOfStringW stringutil_lastIndexOfStringW[CASE_SENSITIVITY_COUNT];

  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfAnyA)(const char* str, size_t length, const char* charArray, size_t charLength);
  typedef size_t (FOG_CDECL* StringUtil_LastIndexOfAnyW)(const CharW* str, size_t length, const CharW* charArray, size_t charLength);

  StringUtil_LastIndexOfAnyA stringutil_lastIndexOfAnyA[CASE_SENSITIVITY_COUNT];
  StringUtil_LastIndexOfAnyW stringutil_lastIndexOfAnyW[CASE_SENSITIVITY_COUNT];

  FOG_CAPI_STATIC(err_t, stringutil_validateUtf8)(const char* data, size_t length, size_t* invalid);
  FOG_CAPI_STATIC(err_t, stringutil_validateUtf16)(const CharW* data, size_t length, size_t* invalid);

  FOG_CAPI_STATIC(err_t, stringutil_ucsFromUtf8Length)(const char* data, size_t length, size_t* ucsLength);
  FOG_CAPI_STATIC(err_t, stringutil_ucsFromUtf16Length)(const CharW* data, size_t length, size_t* ucsLength);

  FOG_CAPI_STATIC(void, stringutil_itoa)(NTOAContext* ctx, int64_t n, uint32_t base, uint32_t textCase);
  FOG_CAPI_STATIC(void, stringutil_utoa)(NTOAContext* ctx, uint64_t n, uint32_t base, uint32_t textCase);
  FOG_CAPI_STATIC(void, stringutil_dtoa)(NTOAContext* ctx, double d, uint32_t mode, int nDigits);

  FOG_CAPI_STATIC(err_t, stringutil_parseBoolA)(bool* dst, const char* src, size_t length, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseBoolW)(bool* dst, const CharW* src, size_t length, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseI8A)(int8_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseI8W)(int8_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseU8A)(uint8_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseU8W)(uint8_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseI16A)(int16_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseI16W)(int16_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseU16A)(uint16_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseU16W)(uint16_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseI32A)(int32_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseI32W)(int32_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseU32A)(uint32_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseU32W)(uint32_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseI64A)(int64_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseI64W)(int64_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseU64A)(uint64_t* dst, const char* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseU64W)(uint64_t* dst, const CharW* src, size_t length, uint32_t base, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseFloatA)(float* dst, const char* src, size_t length, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseFloatW)(float* dst, const CharW* src, size_t length, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);

  FOG_CAPI_STATIC(err_t, stringutil_parseDoubleA)(double* dst, const char* src, size_t length, char decimalPoint, size_t* pEnd, uint32_t* pFlags);
  FOG_CAPI_STATIC(err_t, stringutil_parseDoubleW)(double* dst, const CharW* src, size_t length, uint16_t decimalPoint, size_t* pEnd, uint32_t* pFlags);

  // --------------------------------------------------------------------------
  // [Core/Tools - TextCodec]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(textcodec_ctor)(TextCodec* self);
  FOG_CAPI_CTOR(textcodec_ctorCopy)(TextCodec* self, const TextCodec* other);
  FOG_CAPI_DTOR(textcodec_dtor)(TextCodec* self);

  FOG_CAPI_METHOD(err_t, textcodec_createFromCode)(TextCodec* self, uint32_t code);
  FOG_CAPI_METHOD(err_t, textcodec_createFromMimeStubA)(TextCodec* self, const StubA* mime);
  FOG_CAPI_METHOD(err_t, textcodec_createFromMimeStringW)(TextCodec* self, const StringW* mime);
  FOG_CAPI_METHOD(err_t, textcodec_createFromBom)(TextCodec* self, const void* data, size_t length);
  FOG_CAPI_METHOD(void, textcodec_reset)(TextCodec* self);
  FOG_CAPI_METHOD(err_t, textcodec_copy)(TextCodec* self, const TextCodec* other);
  FOG_CAPI_METHOD(err_t, textcodec_encodeStubW)(const TextCodec* self, StringA* dst, const StubW* src, TextCodecState* state, TextCodecHandler* handler, uint32_t cntOp);
  FOG_CAPI_METHOD(err_t, textcodec_encodeStringW)(const TextCodec* self, StringA* dst, const StringW* src, TextCodecState* state, TextCodecHandler* handler, uint32_t cntOp);
  FOG_CAPI_METHOD(err_t, textcodec_decodeStubA)(const TextCodec* self, StringW* dst, const StubA* src, TextCodecState* state, uint32_t cntOp);
  FOG_CAPI_METHOD(err_t, textcodec_decodeStringA)(const TextCodec* self, StringW* dst, const StringA* src, TextCodecState* state, uint32_t cntOp);

  TextCodec* textcodec_oCache[TEXT_CODEC_CACHE_COUNT];

  // --------------------------------------------------------------------------
  // [Core/Tools - Time]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(int64_t, time_now)(void);

  FOG_CAPI_STATIC(err_t, time_fromTimeT)(int64_t* us, const time_t* src);
  FOG_CAPI_STATIC(err_t, time_toTimeT)(int64_t us, time_t* dst);

#if defined(FOG_OS_WINDOWS)
  FOG_CAPI_STATIC(err_t, time_fromFILETIME)(int64_t* us, const FILETIME* src);
  FOG_CAPI_STATIC(err_t, time_toFILETIME)(int64_t us, FILETIME* dst);
#endif // FOG_OS_WINDOWS

  // --------------------------------------------------------------------------
  // [Core/Tools - TimeTicks]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(int64_t, timeticks_now)(uint32_t ticksPrecision);

  // --------------------------------------------------------------------------
  // [Core/Tools - Var]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(var_ctor)(Var* self);
  FOG_CAPI_CTOR(var_ctorCopy)(Var* self, const Var* other);
  FOG_CAPI_CTOR(var_ctorType)(Var* self, uint32_t vType, const void* vData);
  FOG_CAPI_DTOR(var_dtor)(Var* self);

  FOG_CAPI_METHOD(size_t, var_getReference)(const Var* self);
  FOG_CAPI_METHOD(uint32_t, var_getVarType)(const Var* self);
  FOG_CAPI_METHOD(void, var_reset)(Var* self);
  FOG_CAPI_METHOD(void, var_copy)(Var* self, const Var* other);
  FOG_CAPI_METHOD(err_t, var_getI32)(const Var* self, int32_t* dst);
  FOG_CAPI_METHOD(err_t, var_getI32Bound)(const Var* self, int32_t* dst, int32_t min, int32_t max);
  FOG_CAPI_METHOD(err_t, var_getU32)(const Var* self, uint32_t* dst);
  FOG_CAPI_METHOD(err_t, var_getU32Bound)(const Var* self, uint32_t* dst, uint32_t min, uint32_t max);
  FOG_CAPI_METHOD(err_t, var_getI64)(const Var* self, int64_t* dst);
  FOG_CAPI_METHOD(err_t, var_getI64Bound)(const Var* self, int64_t* dst, int64_t min, int64_t max);
  FOG_CAPI_METHOD(err_t, var_getU64)(const Var* self, uint64_t* dst);
  FOG_CAPI_METHOD(err_t, var_getU64Bound)(const Var* self, uint64_t* dst, uint64_t min, uint64_t max);
  FOG_CAPI_METHOD(err_t, var_getFloat)(const Var* self, float* dst);
  FOG_CAPI_METHOD(err_t, var_getFloatBound)(const Var* self, float* dst, float min, float max);
  FOG_CAPI_METHOD(err_t, var_getDouble)(const Var* self, double* dst);
  FOG_CAPI_METHOD(err_t, var_getDoubleBound)(const Var* self, double* dst, double min, double max);
  FOG_CAPI_METHOD(err_t, var_getType)(const Var* self, uint32_t vType, void* vData);
  FOG_CAPI_METHOD(err_t, var_setType)(Var* self, uint32_t vType, const void* vData);
  FOG_CAPI_METHOD(bool, var_eq)(const Var* a, const Var* b);
  FOG_CAPI_METHOD(int, var_compare)(const Var* a, const Var* b);

  FOG_CAPI_STATIC(VarData*, var_dCreate)(size_t dataSize);
  FOG_CAPI_STATIC(VarData*, var_dAddRef)(VarData* d);
  FOG_CAPI_STATIC(void, var_dRelease)(VarData* d);

  Var* var_oNull;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - ArcF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, arcf_getBoundingBox)(const ArcF* self, BoxF* dst, const TransformF* tr, bool includeCenterPoint);
  FOG_CAPI_METHOD(uint, arcf_toCSpline)(const ArcF* self, PointF* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - ArcD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, arcd_getBoundingBox)(const ArcD* self, BoxD* dst, const TransformD* tr, bool includeCenterPoint);
  FOG_CAPI_METHOD(uint, arcd_toCSpline)(const ArcD* self, PointD* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - CBezierF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, cbezierf_getBoundingBox)(const PointF* self, BoxF* dst);
  FOG_CAPI_METHOD(err_t, cbezierf_getSplineBBox)(const PointF* self, size_t length, BoxF* dst);
  FOG_CAPI_METHOD(void, cbezierf_getLength)(const PointF* self, float* length);
  FOG_CAPI_METHOD(int, cbezierf_getInflectionPoints)(const PointF* self, float*  t);
  FOG_CAPI_METHOD(int, cbezierf_simplifyForProcessing)(const PointF* self, PointF* pts);
  FOG_CAPI_METHOD(err_t, cbezierf_flatten)(const PointF* self, PathF* dst, uint8_t initialCommand, float flatness);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - CBezierD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, cbezierd_getBoundingBox)(const PointD* self, BoxD* dst);
  FOG_CAPI_METHOD(err_t, cbezierd_getSplineBBox)(const PointD* self, size_t length, BoxD* dst);
  FOG_CAPI_METHOD(void, cbezierd_getLength)(const PointD* self, double* length);
  FOG_CAPI_METHOD(int, cbezierd_getInflectionPoints)(const PointD* self, double* t);
  FOG_CAPI_METHOD(int, cbezierd_simplifyForProcessing)(const PointD* self, PointD* pts);
  FOG_CAPI_METHOD(err_t, cbezierd_flatten)(const PointD* self, PathD* dst, uint8_t initialCommand, double flatness);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - ChordF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(bool, chordf_hitTest)(const ChordF* self, const PointF* pt);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - ChordD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(bool, chordd_hitTest)(const ChordD* self, const PointD* pt);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - CircleF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, circlef_getBoundingBox)(const CircleF* self, BoxF* dst, const TransformF* tr);
  FOG_CAPI_METHOD(bool, circlef_hitTest)(const CircleF* self, const PointF* pt);
  FOG_CAPI_METHOD(uint, circlef_toCSpline)(const CircleF* self, PointF* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - CircleD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, circled_getBoundingBox)(const CircleD* self, BoxD* dst, const TransformD* tr);
  FOG_CAPI_METHOD(bool, circled_hitTest)(const CircleD* self, const PointD* pt);
  FOG_CAPI_METHOD(uint, circled_toCSpline)(const CircleD* self, PointD* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - EllipseF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, ellipsef_getBoundingBox)(const EllipseF* self, BoxF* dst, const TransformF* tr);
  FOG_CAPI_METHOD(bool, ellipsef_hitTest)(const EllipseF* self, const PointF* pt);
  FOG_CAPI_METHOD(uint, ellipsef_toCSpline)(const EllipseF* self, PointF* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - EllipseD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, ellipsed_getBoundingBox)(const EllipseD* self, BoxD* dst, const TransformD* tr);
  FOG_CAPI_METHOD(bool, ellipsed_hitTest)(const EllipseD* self, const PointD* pt);
  FOG_CAPI_METHOD(uint, ellipsed_toCSpline)(const EllipseD* self, PointD* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - LineF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(uint32_t, linef_intersect)(PointF* dst, const PointF* lineA, const PointF* lineB);
  FOG_CAPI_METHOD(float, linef_polyAngle)(const PointF* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - LineD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(uint32_t, lined_intersect)(PointD* dst, const PointD* lineA, const PointD* lineB);
  FOG_CAPI_METHOD(double, lined_polyAngle)(const PointD* pts);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathF]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(pathf_ctor)(PathF* self);
  FOG_CAPI_CTOR(pathf_ctorCopyF)(PathF* self, const PathF* other);
  FOG_CAPI_DTOR(pathf_dtor)(PathF* self);

  FOG_CAPI_METHOD(err_t, pathf_detach)(PathF* self);
  FOG_CAPI_METHOD(err_t, pathf_reserve)(PathF* self, size_t capacity);
  FOG_CAPI_METHOD(void, pathf_squeeze)(PathF* self);
  FOG_CAPI_METHOD(size_t, pathf_prepare)(PathF* self, uint32_t cntOp, size_t count);
  FOG_CAPI_METHOD(size_t, pathf_add)(PathF* self, size_t count);
  FOG_CAPI_METHOD(void, pathf_updateFlat)(const PathF* self);
  FOG_CAPI_METHOD(void, pathf_clear)(PathF* self);
  FOG_CAPI_METHOD(void, pathf_reset)(PathF* self);
  FOG_CAPI_METHOD(err_t, pathf_setPathF)(PathF* self, const PathF* other);
  FOG_CAPI_METHOD(err_t, pathf_setDeepF)(PathF* self, const PathF* other);
  FOG_CAPI_METHOD(err_t, pathf_getLastVertex)(const PathF* self, PointF* dst);
  FOG_CAPI_METHOD(err_t, pathf_setVertex)(PathF* self, size_t index, const PointF* pt);
  FOG_CAPI_METHOD(err_t, pathf_getSubpathRange)(const PathF* self, Range* dst, size_t index);
  FOG_CAPI_METHOD(err_t, pathf_moveTo)(PathF* self, const PointF* pt0);
  FOG_CAPI_METHOD(err_t, pathf_moveToRel)(PathF* self, const PointF* pt0);
  FOG_CAPI_METHOD(err_t, pathf_lineTo)(PathF* self, const PointF* pt0);
  FOG_CAPI_METHOD(err_t, pathf_lineToRel)(PathF* self, const PointF* pt0);
  FOG_CAPI_METHOD(err_t, pathf_hlineTo)(PathF* self, float x);
  FOG_CAPI_METHOD(err_t, pathf_hlineToRel)(PathF* self, float x);
  FOG_CAPI_METHOD(err_t, pathf_vlineTo)(PathF* self, float y);
  FOG_CAPI_METHOD(err_t, pathf_vlineToRel)(PathF* self, float y);
  FOG_CAPI_METHOD(err_t, pathf_polyTo)(PathF* self, const PointF* pts, size_t count);
  FOG_CAPI_METHOD(err_t, pathf_polyToRel)(PathF* self, const PointF* pts, size_t count);
  FOG_CAPI_METHOD(err_t, pathf_quadTo)(PathF* self, const PointF* pt1, const PointF* pt2);
  FOG_CAPI_METHOD(err_t, pathf_quadToRel)(PathF* self, const PointF* pt1, const PointF* pt2);
  FOG_CAPI_METHOD(err_t, pathf_cubicTo)(PathF* self, const PointF* pt1, const PointF* pt2, const PointF* pt3);
  FOG_CAPI_METHOD(err_t, pathf_cubicToRel)(PathF* self, const PointF* pt1, const PointF* pt2, const PointF* pt3);
  FOG_CAPI_METHOD(err_t, pathf_smoothQuadTo)(PathF* self, const PointF* pt2);
  FOG_CAPI_METHOD(err_t, pathf_smoothQuadToRel)(PathF* self, const PointF* pt2);
  FOG_CAPI_METHOD(err_t, pathf_smoothCubicTo)(PathF* self, const PointF* pt1, const PointF* pt2);
  FOG_CAPI_METHOD(err_t, pathf_smoothCubicToRel)(PathF* self, const PointF* pt1, const PointF* pt2);
  FOG_CAPI_METHOD(err_t, pathf_arcTo)(PathF* self, const PointF* cp, const PointF* rp, float start, float sweep, bool startPath);
  FOG_CAPI_METHOD(err_t, pathf_arcToRel)(PathF* self, const PointF* cp, const PointF* rp, float start, float sweep, bool startPath);
  FOG_CAPI_METHOD(err_t, pathf_svgArcTo)(PathF* self, const PointF* rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF* pt);
  FOG_CAPI_METHOD(err_t, pathf_svgArcToRel)(PathF* self, const PointF* rp, float angle, bool largeArcFlag, bool sweepFlag, const PointF* pt);
  FOG_CAPI_METHOD(err_t, pathf_close)(PathF* self);
  FOG_CAPI_METHOD(err_t, pathf_boxI)(PathF* self, const BoxI* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_boxF)(PathF* self, const BoxF* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_rectI)(PathF* self, const RectI* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_rectF)(PathF* self, const RectF* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_boxesI)(PathF* self, const BoxI* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_boxesF)(PathF* self, const BoxF* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_rectsI)(PathF* self, const RectI* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_rectsF)(PathF* self, const RectF* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_region)(PathF* self, const Region* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_polylineI)(PathF* self, const PointI* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_polylineF)(PathF* self, const PointF* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_polygonI)(PathF* self, const PointI* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_polygonF)(PathF* self, const PointF* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathf_shape)(PathF* self, uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformF* tr);
  FOG_CAPI_METHOD(err_t, pathf_appendPathF)(PathF* self, const PathF* path, const Range* range);
  FOG_CAPI_METHOD(err_t, pathf_appendTranslatedPathF)(PathF* self, const PathF* path, const PointF* pt, const Range* range);
  FOG_CAPI_METHOD(err_t, pathf_appendTransformedPathF)(PathF* self, const PathF* path, const TransformF* tr, const Range* range);
  FOG_CAPI_METHOD(err_t, pathf_getBoundingBox)(const PathF* self, BoxF* dst, const TransformF* transform);
  FOG_CAPI_METHOD(bool, pathf_hitTest)(const PathF* self, const PointF* pt, uint32_t fillRule);
  FOG_CAPI_METHOD(size_t, pathf_getClosestVertex)(const PathF* self, const PointF* pt, float maxDistance, float* distance);
  FOG_CAPI_METHOD(err_t, pathf_translate)(PathF* self, const PointF* pt, const Range* range);
  FOG_CAPI_METHOD(err_t, pathf_transform)(PathF* self, const TransformF* tr, const Range* range);
  FOG_CAPI_METHOD(err_t, pathf_fitTo)(PathF* self, const RectF* r);
  FOG_CAPI_METHOD(err_t, pathf_scale)(PathF* self, const PointF* pt, bool keepStartPos);
  FOG_CAPI_METHOD(err_t, pathf_flipX)(PathF* self, float x0, float x1);
  FOG_CAPI_METHOD(err_t, pathf_flipY)(PathF* self, float y0, float y1);
  FOG_CAPI_METHOD(err_t, pathf_flatten)(PathF* dst, const PathF* src, const PathFlattenParamsF* params, const Range* range);
  FOG_CAPI_METHOD(const PathInfoF*, pathf_getPathInfo)(const PathF* self);

  FOG_CAPI_STATIC(bool, pathf_eq)(const PathF* a, const PathF* b);
  FOG_CAPI_STATIC(PathDataF*, pathf_dCreate)(size_t capacity);
  FOG_CAPI_STATIC(PathDataF*, pathf_dAdopt)(void* address, size_t capacity);
  FOG_CAPI_STATIC(void, pathf_dFree)(PathDataF* d);

  PathF* pathf_oEmpty;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathD]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(pathd_ctor)(PathD* self);
  FOG_CAPI_CTOR(pathd_ctorCopyD)(PathD* self, const PathD* other);
  FOG_CAPI_DTOR(pathd_dtor)(PathD* self);

  FOG_CAPI_METHOD(err_t, pathd_detach)(PathD* self);
  FOG_CAPI_METHOD(err_t, pathd_reserve)(PathD* self, size_t capacity);
  FOG_CAPI_METHOD(void, pathd_squeeze)(PathD* self);
  FOG_CAPI_METHOD(size_t, pathd_prepare)(PathD* self, uint32_t cntOp, size_t count);
  FOG_CAPI_METHOD(size_t, pathd_add)(PathD* self, size_t count);
  FOG_CAPI_METHOD(void, pathd_updateFlat)(const PathD* self);
  FOG_CAPI_METHOD(void, pathd_clear)(PathD* self);
  FOG_CAPI_METHOD(void, pathd_reset)(PathD* self);
  FOG_CAPI_METHOD(err_t, pathd_setPathF)(PathD* self, const PathF* other);
  FOG_CAPI_METHOD(err_t, pathd_setPathD)(PathD* self, const PathD* other);
  FOG_CAPI_METHOD(err_t, pathd_setDeepD)(PathD* self, const PathD* other);
  FOG_CAPI_METHOD(err_t, pathd_getLastVertex)(const PathD* self, PointD* dst);
  FOG_CAPI_METHOD(err_t, pathd_setVertex)(PathD* self, size_t index, const PointD* pt);
  FOG_CAPI_METHOD(err_t, pathd_getSubpathRange)(const PathD* self, Range* dst, size_t index);
  FOG_CAPI_METHOD(err_t, pathd_moveTo)(PathD* self, const PointD* pt0);
  FOG_CAPI_METHOD(err_t, pathd_moveToRel)(PathD* self, const PointD* pt0);
  FOG_CAPI_METHOD(err_t, pathd_lineTo)(PathD* self, const PointD* pt0);
  FOG_CAPI_METHOD(err_t, pathd_lineToRel)(PathD* self, const PointD* pt0);
  FOG_CAPI_METHOD(err_t, pathd_hlineTo)(PathD* self, double x);
  FOG_CAPI_METHOD(err_t, pathd_hlineToRel)(PathD* self, double x);
  FOG_CAPI_METHOD(err_t, pathd_vlineTo)(PathD* self, double y);
  FOG_CAPI_METHOD(err_t, pathd_vlineToRel)(PathD* self, double y);
  FOG_CAPI_METHOD(err_t, pathd_polyTo)(PathD* self, const PointD* pts, size_t count);
  FOG_CAPI_METHOD(err_t, pathd_polyToRel)(PathD* self, const PointD* pts, size_t count);
  FOG_CAPI_METHOD(err_t, pathd_quadTo)(PathD* self, const PointD* pt1, const PointD* pt2);
  FOG_CAPI_METHOD(err_t, pathd_quadToRel)(PathD* self, const PointD* pt1, const PointD* pt2);
  FOG_CAPI_METHOD(err_t, pathd_cubicTo)(PathD* self, const PointD* pt1, const PointD* pt2, const PointD* pt3);
  FOG_CAPI_METHOD(err_t, pathd_cubicToRel)(PathD* self, const PointD* pt1, const PointD* pt2, const PointD* pt3);
  FOG_CAPI_METHOD(err_t, pathd_smoothQuadTo)(PathD* self, const PointD* pt2);
  FOG_CAPI_METHOD(err_t, pathd_smoothQuadToRel)(PathD* self, const PointD* pt2);
  FOG_CAPI_METHOD(err_t, pathd_smoothCubicTo)(PathD* self, const PointD* pt1, const PointD* pt2);
  FOG_CAPI_METHOD(err_t, pathd_smoothCubicToRel)(PathD* self, const PointD* pt1, const PointD* pt2);
  FOG_CAPI_METHOD(err_t, pathd_arcTo)(PathD* self, const PointD* cp, const PointD* rp, double start, double sweep, bool startPath);
  FOG_CAPI_METHOD(err_t, pathd_arcToRel)(PathD* self, const PointD* cp, const PointD* rp, double start, double sweep, bool startPath);
  FOG_CAPI_METHOD(err_t, pathd_svgArcTo)(PathD* self, const PointD* rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD* pt);
  FOG_CAPI_METHOD(err_t, pathd_svgArcToRel)(PathD* self, const PointD* rp, double angle, bool largeArcFlag, bool sweepFlag, const PointD* pt);
  FOG_CAPI_METHOD(err_t, pathd_close)(PathD* self);
  FOG_CAPI_METHOD(err_t, pathd_boxI)(PathD* self, const BoxI* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_boxF)(PathD* self, const BoxF* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_boxD)(PathD* self, const BoxD* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_rectI)(PathD* self, const RectI* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_rectF)(PathD* self, const RectF* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_rectD)(PathD* self, const RectD* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_boxesI)(PathD* self, const BoxI* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_boxesF)(PathD* self, const BoxF* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_boxesD)(PathD* self, const BoxD* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_rectsI)(PathD* self, const RectI* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_rectsF)(PathD* self, const RectF* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_rectsD)(PathD* self, const RectD* r, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_region)(PathD* self, const Region* r, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_polylineI)(PathD* self, const PointI* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_polylineD)(PathD* self, const PointD* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_polygonI)(PathD* self, const PointI* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_polygonD)(PathD* self, const PointD* pts, size_t count, uint32_t direction);
  FOG_CAPI_METHOD(err_t, pathd_shape)(PathD* self, uint32_t shapeType, const void* shapeData, uint32_t direction, const TransformD* tr);
  FOG_CAPI_METHOD(err_t, pathd_appendPathF)(PathD* self, const PathF* path, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_appendPathD)(PathD* self, const PathD* path, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_appendTranslatedPathF)(PathD* self, const PathF* path, const PointD* pt, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_appendTranslatedPathD)(PathD* self, const PathD* path, const PointD* pt, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_appendTransformedPathF)(PathD* self, const PathF* path, const TransformD* tr, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_appendTransformedPathD)(PathD* self, const PathD* path, const TransformD* tr, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_getBoundingBox)(const PathD* self, BoxD* dst, const TransformD* transform);
  FOG_CAPI_METHOD(bool, pathd_hitTest)(const PathD* self, const PointD* pt, uint32_t fillRule);
  FOG_CAPI_METHOD(size_t, pathd_getClosestVertex)(const PathD* self, const PointD* pt, double maxDistance, double* distance);
  FOG_CAPI_METHOD(err_t, pathd_translate)(PathD* self, const PointD* pt, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_transform)(PathD* self, const TransformD* tr, const Range* range);
  FOG_CAPI_METHOD(err_t, pathd_fitTo)(PathD* self, const RectD* r);
  FOG_CAPI_METHOD(err_t, pathd_scale)(PathD* self, const PointD* pt, bool keepStartPos);
  FOG_CAPI_METHOD(err_t, pathd_flipX)(PathD* self, double x0, double x1);
  FOG_CAPI_METHOD(err_t, pathd_flipY)(PathD* self, double y0, double y1);
  FOG_CAPI_METHOD(err_t, pathd_flatten)(PathD* dst, const PathD* src, const PathFlattenParamsD* params, const Range* range);
  FOG_CAPI_METHOD(const PathInfoD*, pathd_getPathInfo)(const PathD* self);

  FOG_CAPI_STATIC(bool, pathd_eq)(const PathD* a, const PathD* b);
  FOG_CAPI_STATIC(PathDataD*, pathd_dCreate)(size_t capacity);
  FOG_CAPI_STATIC(PathDataD*, pathd_dAdopt)(void* address, size_t capacity);
  FOG_CAPI_STATIC(void, pathd_dFree)(PathDataD* d);

  PathD* pathd_oEmpty;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathClipperF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(uint32_t, pathclipperf_measurePath)(PathClipperF* self, const PathF* src);
  FOG_CAPI_METHOD(err_t, pathclipperf_continuePath)(PathClipperF* self, PathF* dst, const PathF* src);
  FOG_CAPI_METHOD(err_t, pathclipperf_continuePathData)(PathClipperF* self, PathF* dst, const PointF* srcPts, const uint8_t* srcCmd, size_t srcLength);
  FOG_CAPI_METHOD(err_t, pathclipperf_clipPath)(PathClipperF* self, PathF* dst, const PathF* src, const TransformF* tr);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathClipperD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(uint32_t, pathclipperd_measurePath)(PathClipperD* self, const PathD* src);
  FOG_CAPI_METHOD(err_t, pathclipperd_continuePath)(PathClipperD* self, PathD* dst, const PathD* src);
  FOG_CAPI_METHOD(err_t, pathclipperd_continuePathData)(PathClipperD* self, PathD* dst, const PointD* srcPts, const uint8_t* srcCmd, size_t srcLength);
  FOG_CAPI_METHOD(err_t, pathclipperd_clipPath)(PathClipperD* self, PathD* dst, const PathD* src, const TransformD* tr);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathInfoF]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(PathInfoF*, pathinfof_generate)(const PathF* path);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathInfoD]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(PathInfoD*, pathinfod_generate)(const PathD* path);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathStrokerF]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(pathstrokerf_ctor)(PathStrokerF* self);
  FOG_CAPI_CTOR(pathstrokerf_ctorCopy)(PathStrokerF* self, const PathStrokerF* other);
  FOG_CAPI_CTOR(pathstrokerf_ctorParams)(PathStrokerF* self, const PathStrokerParamsF* params, const TransformF* tr, const BoxF* clipBox);
  FOG_CAPI_DTOR(pathstrokerf_dtor)(PathStrokerF* self);

  FOG_CAPI_METHOD(void, pathstrokerf_setParams)(PathStrokerF* self, const PathStrokerParamsF* params);
  FOG_CAPI_METHOD(void, pathstrokerf_setOther)(PathStrokerF* self, const PathStrokerF* other);
  FOG_CAPI_METHOD(err_t, pathstrokerf_strokeShape)(const PathStrokerF* self, PathF* dst, uint32_t shapeType, const void* shapeData);
  FOG_CAPI_METHOD(err_t, pathstrokerf_strokePath)(const PathStrokerF* self, PathF* dst, const PathF* src);
  FOG_CAPI_METHOD(void, pathstrokerf_update)(PathStrokerF* self);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PathStrokerD]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(pathstrokerd_ctor)(PathStrokerD* self);
  FOG_CAPI_CTOR(pathstrokerd_ctorCopy)(PathStrokerD* self, const PathStrokerD* other);
  FOG_CAPI_CTOR(pathstrokerd_ctorParams)(PathStrokerD* self, const PathStrokerParamsD* params, const TransformD* tr, const BoxD* clipBox);
  FOG_CAPI_DTOR(pathstrokerd_dtor)(PathStrokerD* self);

  FOG_CAPI_METHOD(void, pathstrokerd_setParams)(PathStrokerD* self, const PathStrokerParamsD* params);
  FOG_CAPI_METHOD(void, pathstrokerd_setOther)(PathStrokerD* self, const PathStrokerD* other);
  FOG_CAPI_METHOD(err_t, pathstrokerd_strokeShape)(const PathStrokerD* self, PathD* dst, uint32_t shapeType, const void* shapeData);
  FOG_CAPI_METHOD(err_t, pathstrokerd_strokePath)(const PathStrokerD* self, PathD* dst, const PathD* src);
  FOG_CAPI_METHOD(void, pathstrokerd_update)(PathStrokerD* self);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PieF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(bool, pief_hitTest)(const PieF* self, const PointF* pt);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - PieD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(bool, pied_hitTest)(const PieD* self, const PointD* pt);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - QBezierF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, qbezierf_getBoundingBox)(const PointF* self, BoxF* dst);
  FOG_CAPI_METHOD(err_t, qbezierf_getSplineBBox)(const PointF* self, size_t length, BoxF* dst);
  FOG_CAPI_METHOD(void, qbezierf_getLength)(const PointF* self, float* length);
  FOG_CAPI_METHOD(err_t, qbezierf_flatten)(const PointF* self, PathF* dst, uint8_t initialCommand, float flatness);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - QBezierD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, qbezierd_getBoundingBox)(const PointD* self, BoxD* dst);
  FOG_CAPI_METHOD(err_t, qbezierd_getSplineBBox)(const PointD* self, size_t length, BoxD* dst);
  FOG_CAPI_METHOD(void, qbezierd_getLength)(const PointD* self, double* length);
  FOG_CAPI_METHOD(err_t, qbezierd_flatten)(const PointD* self, PathD* dst, uint8_t initialCommand, double flatness);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - Round]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, roundf_getBoundingBox)(const RoundF* self, BoxF* dst, const TransformF* tr);
  FOG_CAPI_METHOD(bool, roundf_hitTest)(const RoundF* self, const PointF* pt);

  FOG_CAPI_METHOD(err_t, roundd_getBoundingBox)(const RoundD* self, BoxD* dst, const TransformD* tr);
  FOG_CAPI_METHOD(bool, roundd_hitTest)(const RoundD* self, const PointD* pt);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - ShapeF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, shapef_getBoundingBox)(uint32_t shapeType, const void* shapeData, BoxF* dst, const TransformF* transform);
  FOG_CAPI_METHOD(bool, shapef_hitTest)(uint32_t shapeType, const void* shapeData, const PointF* pt);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - ShapeD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(bool, shaped_hitTest)(uint32_t shapeType, const void* shapeData, const PointD* pt);
  FOG_CAPI_METHOD(err_t, shaped_getBoundingBox)(uint32_t shapeType, const void* shapeData, BoxD* dst, const TransformD* transform);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - TransformF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, transformf_create)(TransformF* self, uint32_t createType, const void* params);
  FOG_CAPI_METHOD(uint32_t, transformf_update)(const TransformF* self);
  FOG_CAPI_METHOD(err_t, transformf_transform)(TransformF* self, uint32_t transformOp, const void* params);
  FOG_CAPI_STATIC(err_t, transformf_transform2)(TransformF* dst, const TransformF* src, uint32_t transformOp, const void* params);
  FOG_CAPI_METHOD(void, transformf_multiply)(TransformF* self, const TransformF* a, const TransformF* b);
  FOG_CAPI_METHOD(bool, transformf_invert)(TransformF* self, const TransformF* a);
  FOG_CAPI_METHOD(void, transformf_mapPointF)(const TransformF* self, PointF* dst, const PointF* src);

  typedef void (FOG_CDECL* TransformF_MapPointsF)(const TransformF* self, PointF* dst, const PointF* src, size_t length);
  TransformF_MapPointsF transformf_mapPointsF[TRANSFORM_TYPE_COUNT];

  FOG_CAPI_METHOD(err_t, transformf_mapPathF)(const TransformF* self, PathF* dst, const PathF* src, uint32_t cntOp);
  FOG_CAPI_METHOD(err_t, transformf_mapPathDataF)(const TransformF* self, PathF* dst, const uint8_t* srcCmd, const PointF* srcPts, size_t length, uint32_t cntOp);
  FOG_CAPI_METHOD(void, transformf_mapBoxF)(const TransformF* self, BoxF* dst, const BoxF* src);
  FOG_CAPI_METHOD(void, transformf_mapVectorF)(const TransformF* self, PointF* dst, const PointF* src);
  FOG_CAPI_METHOD(err_t, transformf_getScaling)(const TransformF* self, PointF* dst, bool absolute);
  FOG_CAPI_METHOD(float, transformf_getRotation)(const TransformF* self);
  FOG_CAPI_METHOD(float, transformf_getAverageScaling)(const TransformF* self);

  TransformF* transformf_oIdentity;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - TransformD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, transformd_create)(TransformD* self, uint32_t createType, const void* params);
  FOG_CAPI_METHOD(uint32_t, transformd_update)(const TransformD* self);
  FOG_CAPI_METHOD(err_t, transformd_transform)(TransformD* self, uint32_t transformOp, const void* params);
  FOG_CAPI_STATIC(err_t, transformd_transform2)(TransformD* dst, const TransformD* src, uint32_t transformOp, const void* params);
  FOG_CAPI_METHOD(void, transformd_multiply)(TransformD* self, const TransformD* a, const TransformD* b);
  FOG_CAPI_METHOD(bool, transformd_invert)(TransformD* self, const TransformD* a);
  FOG_CAPI_METHOD(void, transformd_mapPointD)(const TransformD* self, PointD* dst, const PointD* src);

  typedef void (FOG_CDECL* TransformD_MapPointsF)(const TransformD* self, PointD* dst, const PointF* src, size_t length);
  typedef void (FOG_CDECL* TransformD_MapPointsD)(const TransformD* self, PointD* dst, const PointD* src, size_t length);

  TransformD_MapPointsF transformd_mapPointsF[TRANSFORM_TYPE_COUNT];
  TransformD_MapPointsD transformd_mapPointsD[TRANSFORM_TYPE_COUNT];

  FOG_CAPI_METHOD(err_t, transformd_mapPathF)(const TransformD* self, PathD* dst, const PathF* src, uint32_t cntOp);
  FOG_CAPI_METHOD(err_t, transformd_mapPathD)(const TransformD* self, PathD* dst, const PathD* src, uint32_t cntOp);
  FOG_CAPI_METHOD(err_t, transformd_mapPathDataF)(const TransformD* self, PathD* dst, const uint8_t* srcCmd, const PointF* srcPts, size_t length, uint32_t cntOp);
  FOG_CAPI_METHOD(err_t, transformd_mapPathDataD)(const TransformD* self, PathD* dst, const uint8_t* srcCmd, const PointD* srcPts, size_t length, uint32_t cntOp);
  FOG_CAPI_METHOD(void, transformd_mapBoxD)(const TransformD* self, BoxD* dst, const BoxD* src);
  FOG_CAPI_METHOD(void, transformd_mapVectorD)(const TransformD* self, PointD* dst, const PointD* src);
  FOG_CAPI_METHOD(err_t, transformd_getScaling)(const TransformD* self, PointD* dst, bool absolute);
  FOG_CAPI_METHOD(double, transformd_getRotation)(const TransformD* self);
  FOG_CAPI_METHOD(double, transformd_getAverageScaling)(const TransformD* self);

  TransformD* transformd_oIdentity;

  // --------------------------------------------------------------------------
  // [G2d/Geometry - TriangleF]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, trianglef_getBoundingBox)(const PointF* self, BoxF* dst, const TransformF* tr);
  FOG_CAPI_METHOD(bool, trianglef_hitTest)(const PointF* self, const PointF* pt);

  // --------------------------------------------------------------------------
  // [G2d/Geometry - TriangleD]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, triangled_getBoundingBox)(const PointD* self, BoxD* dst, const TransformD* tr);
  FOG_CAPI_METHOD(bool, triangled_hitTest)(const PointD* self, const PointD* pt);

  // --------------------------------------------------------------------------
  // [G2d/Imaging - Image]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(image_ctor)(Image* self);
  FOG_CAPI_CTOR(image_ctorCopy)(Image* self, const Image* other);
  FOG_CAPI_CTOR(image_ctorCreate)(Image* self, const SizeI* size, uint32_t imageFormat, uint32_t imageType);
  FOG_CAPI_DTOR(image_dtor)(Image* self);

  FOG_CAPI_METHOD(err_t, image_detach)(Image* self);

  FOG_CAPI_METHOD(uint32_t, image_getAlphaDistribution)(const Image* self);
  FOG_CAPI_METHOD(void, image_modified)(Image* self);

  FOG_CAPI_METHOD(void, image_reset)(Image* self);

  FOG_CAPI_METHOD(err_t, image_create)(Image* self, const SizeI* size, uint32_t format, uint32_t type);
  FOG_CAPI_METHOD(err_t, image_adopt)(Image* self, const ImageBits* bits, uint32_t adoptFlags);

  FOG_CAPI_METHOD(err_t, image_copy)(Image* self, const Image* other);
  FOG_CAPI_METHOD(err_t, image_copyDeep)(Image* self, const Image* other);
  FOG_CAPI_METHOD(err_t, image_setImage)(Image* self, const Image* other, const RectI* area);

  FOG_CAPI_METHOD(err_t, image_convert)(Image* self, uint32_t format);
  FOG_CAPI_METHOD(err_t, image_forceFormat)(Image* self, uint32_t format);

  FOG_CAPI_METHOD(err_t, image_convertTo8BitDepth)(Image* self);
  FOG_CAPI_METHOD(err_t, image_convertTo8BitDepthPalette)(Image* self, const ImagePalette* palette);

  FOG_CAPI_METHOD(err_t, image_setPalette)(Image* self, const ImagePalette* palette);
  FOG_CAPI_METHOD(err_t, image_setPaletteData)(Image* self, const Range* range, const Argb32* data);

  FOG_CAPI_METHOD(err_t, image_clearArgb32)(Image* self, const Argb32* c0);
  FOG_CAPI_METHOD(err_t, image_clearColor)(Image* self, const Color* c0);

  FOG_CAPI_METHOD(err_t, image_fillRectArgb32)(Image* self, const RectI* r, const Argb32* c0, uint32_t compositingOperator, float opacity);
  FOG_CAPI_METHOD(err_t, image_fillRectColor)(Image* self, const RectI* r, const Color* c0, uint32_t compositingOperator, float opacity);

  FOG_CAPI_METHOD(err_t, image_blitImageAt)(Image* self, const PointI* pt, const Image* i, const RectI* iFragment, uint32_t compositingOperator, float opacity);

  FOG_CAPI_METHOD(err_t, image_scroll)(Image* self, const PointI* pt, const RectI* area);

  FOG_CAPI_METHOD(err_t, image_readFromFile)(Image* self, const StringW* fileName);
  FOG_CAPI_METHOD(err_t, image_readFromStream)(Image* self, Stream* stream, const StringW* ext);
  FOG_CAPI_METHOD(err_t, image_readFromBufferStringA)(Image* self, const StringA* buffer, const StringW* ext);
  FOG_CAPI_METHOD(err_t, image_readFromBufferRaw)(Image* self, const void* buffer, size_t size, const StringW* ext);

  FOG_CAPI_METHOD(err_t, image_writeToFile)(const Image* self, const StringW* fileName, const Hash<StringW, Var>* options);
  FOG_CAPI_METHOD(err_t, image_writeToStream)(const Image* self, Stream* stream, const StringW* ext, const Hash<StringW, Var>* options);
  FOG_CAPI_METHOD(err_t, image_writeToBuffer)(const Image* self, StringA* buffer, uint32_t cntOp, const StringW* ext, const Hash<StringW, Var>* options);

#if defined(FOG_OS_WINDOWS)
  FOG_CAPI_METHOD(err_t, image_toWinBitmap)(const Image* self, HBITMAP* hBitmap);
  FOG_CAPI_METHOD(err_t, image_fromWinBitmap)(Image* self, HBITMAP hBitmap);

  FOG_CAPI_METHOD(err_t, image_getDC)(Image* self, HDC* hDC);
  FOG_CAPI_METHOD(err_t, image_releaseDC)(Image* self, HDC hDC);
#endif // FOG_OS_WINDOWS

  FOG_CAPI_STATIC(bool, image_eq)(const Image* a, const Image* b);

  FOG_CAPI_STATIC(ssize_t, image_getStrideFromWidth)(int width, uint32_t depth);

  FOG_CAPI_STATIC(err_t, image_glyphFromPathF)(Image* dst, PointI* dstOffset, const PathF* path, uint32_t fillRule, uint32_t precision);
  FOG_CAPI_STATIC(err_t, image_glyphFromPathD)(Image* dst, PointI* dstOffset, const PathD* path, uint32_t fillRule, uint32_t precision);

  FOG_CAPI_STATIC(err_t, image_invert)(Image* dst, const Image* src, const RectI* area, uint32_t channels);
  FOG_CAPI_STATIC(err_t, image_mirror)(Image* dst, const Image* src, const RectI* area, uint32_t mirrorMode);
  FOG_CAPI_STATIC(err_t, image_rotate)(Image* dst, const Image* src, const RectI* area, uint32_t rotateMode);

  FOG_CAPI_STATIC(ImageData*, image_dAddRef)(ImageData* d);
  FOG_CAPI_STATIC(void, image_dRelease)(ImageData* d);

  const ImageVTable* image_vTable[4];

  Image* image_oEmpty;

  // --------------------------------------------------------------------------
  // [G2d/Imaging - ImageConverter]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(imageconverter_ctor)(ImageConverter* self);
  FOG_CAPI_CTOR(imageconverter_ctorCopy)(ImageConverter* self, const ImageConverter* other);
  FOG_CAPI_CTOR(imageconverter_ctorCreate)(ImageConverter* self,
    const ImageFormatDescription* dstFormatDescription,
    const ImageFormatDescription* srcFormatDescription,
    uint32_t dither,
    const ImagePalette* dstPalette,
    const ImagePalette* srcPalette);
  FOG_CAPI_DTOR(imageconverter_dtor)(ImageConverter* self);

  FOG_CAPI_METHOD(err_t, imageconverter_create)(ImageConverter* self,
    const ImageFormatDescription* dstFormatDescription,
    const ImageFormatDescription* srcFormatDescription,
    uint32_t dither,
    const ImagePalette* dstPalette,
    const ImagePalette* srcPalette);

  FOG_CAPI_METHOD(err_t, imageconverter_createDithered8)(ImageConverter* self,
    const ImageDither8Params* dstParams,
    const ImageFormatDescription* srcFormatDescription,
    const ImagePalette* dstPalette,
    const ImagePalette* srcPalette);

  FOG_CAPI_METHOD(void, imageconverter_reset)(ImageConverter* self);

  FOG_CAPI_METHOD(void, imageconverter_blitLine)(const ImageConverter* self,
    void* dst, const void* src, int w, const PointI* ditherOrigin);
  FOG_CAPI_METHOD(void, imageconverter_blitRect)(const ImageConverter* self,
    void* dst, size_t dstStride, const void* src, size_t srcStride, int w, int h, const PointI* ditherOrigin);

  FOG_CAPI_STATIC(ImageConverterData*, imageconverter_dCreate)(void);
  FOG_CAPI_STATIC(void, imageconverter_dFree)(ImageConverterData* d);

  ImageConverter* imageconverter_oNull;

  // --------------------------------------------------------------------------
  // [G2d/Imaging - ImageFormatDescription]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(uint32_t, imageformatdescription_getCompatibleFormat)(
    const ImageFormatDescription* self);
  FOG_CAPI_METHOD(err_t, imageformatdescription_createArgb)(
    ImageFormatDescription* self,
    uint32_t depth, uint32_t flags,
    uint64_t aMask, uint64_t rMask, uint64_t gMask, uint64_t bMask);

  // --------------------------------------------------------------------------
  // [G2d/Imaging - ImagePalette]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(imagepalette_ctor)(ImagePalette* self);
  FOG_CAPI_CTOR(imagepalette_ctorCopy)(ImagePalette* self, const ImagePalette* other);
  FOG_CAPI_DTOR(imagepalette_dtor)(ImagePalette* self);

  FOG_CAPI_METHOD(err_t, imagepalette_detach)(ImagePalette* self);
  FOG_CAPI_METHOD(err_t, imagepalette_setData)(ImagePalette* self, const Range* range, const Argb32* data);
  FOG_CAPI_METHOD(err_t, imagepalette_setDeep)(ImagePalette* self, const ImagePalette* other);
  FOG_CAPI_METHOD(err_t, imagepalette_setLength)(ImagePalette* self, size_t length);
  FOG_CAPI_METHOD(void, imagepalette_clear)(ImagePalette* self);
  FOG_CAPI_METHOD(void, imagepalette_reset)(ImagePalette* self);
  FOG_CAPI_METHOD(err_t, imagepalette_copy)(ImagePalette* self, const ImagePalette* other);
  FOG_CAPI_METHOD(bool, imagepalette_eq)(const ImagePalette* a, const ImagePalette* b);

  FOG_CAPI_STATIC(ImagePaletteData*, imagepalette_dCreate)(void);
  FOG_CAPI_STATIC(ImagePaletteData*, imagepalette_dCreateGreyscale)(uint32_t length);
  FOG_CAPI_STATIC(ImagePaletteData*, imagepalette_dCreateColorCube)(uint32_t r, uint32_t g, uint32_t b);
  FOG_CAPI_STATIC(void, imagepalette_dFree)(ImagePaletteData* d);
  FOG_CAPI_STATIC(bool, imagepalette_isGreyscale)(const Argb32* data, size_t length);

  ImagePalette* imagepalette_oEmpty;

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeBlur]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(feblur_ctor)(FeBlur* self);
  FOG_CAPI_CTOR(feblur_ctorCopy)(FeBlur* self, const FeBlur* other);

  FOG_CAPI_METHOD(void, feblur_reset)(FeBlur* self);
  FOG_CAPI_METHOD(err_t, feblur_copy)(FeBlur* self, const FeBlur* other);
  FOG_CAPI_STATIC(bool, feblur_eq)(const FeBlur* a, const FeBlur* b);

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeColorLut]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(fecolorlut_ctor)(FeColorLut* self);
  FOG_CAPI_CTOR(fecolorlut_ctorCopy)(FeColorLut* self, const FeColorLut* other);
  FOG_CAPI_CTOR(fecolorlut_dtor)(FeColorLut* self);

  FOG_CAPI_METHOD(void, fecolorlut_reset)(FeColorLut* self);
  FOG_CAPI_METHOD(err_t, fecolorlut_copy)(FeColorLut* self, const FeColorLut* other);
  FOG_CAPI_STATIC(bool, fecolorlut_eq)(const FeColorLut* a, const FeColorLut* b);

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeColorLutArray]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(fecolorlutarray_ctor)(FeColorLutArray* self);
  FOG_CAPI_CTOR(fecolorlutarray_ctorCopy)(FeColorLutArray* self, const FeColorLutArray* other);
  FOG_CAPI_CTOR(fecolorlutarray_dtor)(FeColorLutArray* self);

  FOG_CAPI_METHOD(err_t, fecolorlutarray_detach)(FeColorLutArray* self);
  FOG_CAPI_METHOD(err_t, fecolorlutarray_setAt)(FeColorLutArray* self, size_t index, uint8_t value);
  FOG_CAPI_METHOD(err_t, fecolorlutarray_setFromComponentFunction)(FeColorLutArray* self, const FeComponentFunction* func);

  FOG_CAPI_METHOD(void, fecolorlutarray_reset)(FeColorLutArray* self);
  FOG_CAPI_METHOD(err_t, fecolorlutarray_copy)(FeColorLutArray* self, const FeColorLutArray* other);
  FOG_CAPI_STATIC(bool, fecolorlutarray_eq)(const FeColorLutArray* a, const FeColorLutArray* b);

  FOG_CAPI_STATIC(FeColorLutArrayData*, fecolorlutarray_dCreate)(void);
  FOG_CAPI_STATIC(void, fecolorlutarray_dFree)(FeColorLutArrayData* d);

  FOG_CAPI_STATIC(void, fecolorlutarray_setIdentity)(uint8_t* data);
  FOG_CAPI_STATIC(bool, fecolorlutarray_isIdentity)(const uint8_t* data);

  FeColorLutArray* fecolorlutarray_oIdentity;

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeColorMatrix]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(fecolormatrix_ctor)(FeColorMatrix* self);
  FOG_CAPI_METHOD(uint32_t, fecolormatrix_getType)(const FeColorMatrix* self);

  FOG_CAPI_METHOD(err_t, fecolormatrix_addMatrix)(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b);
  FOG_CAPI_METHOD(err_t, fecolormatrix_addScalar)(FeColorMatrix* dst, const FeColorMatrix* a, const RectI* rect, float s);

  FOG_CAPI_METHOD(err_t, fecolormatrix_subtractMatrix)(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b);
  FOG_CAPI_METHOD(err_t, fecolormatrix_subtractScalar)(FeColorMatrix* dst, const FeColorMatrix* a, const RectI* rect, float s);

  FOG_CAPI_METHOD(err_t, fecolormatrix_multiplyOther)(FeColorMatrix* dst, const FeColorMatrix* other, uint32_t order);
  FOG_CAPI_METHOD(err_t, fecolormatrix_multiplyMatrix)(FeColorMatrix* dst, const FeColorMatrix* a, const FeColorMatrix* b);
  FOG_CAPI_METHOD(err_t, fecolormatrix_multiplyScalar)(FeColorMatrix* dst, const FeColorMatrix* a, const RectI* rect, float s);

  FOG_CAPI_METHOD(err_t, fecolormatrix_simplifiedPremultiply)(FeColorMatrix* self, const FeColorMatrix* other);

  FOG_CAPI_METHOD(err_t, fecolormatrix_translateArgb)(FeColorMatrix* self, float a, float r, float g, float b, uint32_t order);
  FOG_CAPI_METHOD(err_t, fecolormatrix_scaleArgb)(FeColorMatrix* self, float a, float r, float g, float b, uint32_t order);
  FOG_CAPI_METHOD(err_t, fecolormatrix_scaleTint)(FeColorMatrix* self, float phi, float amount);
  FOG_CAPI_METHOD(err_t, fecolormatrix_saturate)(FeColorMatrix* self, float s, uint32_t order);
  FOG_CAPI_METHOD(err_t, fecolormatrix_rotateColor)(FeColorMatrix* self, int x, int y, float phi, uint32_t order);
  FOG_CAPI_METHOD(err_t, fecolormatrix_rotateHue)(FeColorMatrix* self, float phi);
  FOG_CAPI_METHOD(err_t, fecolormatrix_shearColor)(FeColorMatrix* self, int x, int y0, float c0, int y1, float c1, uint32_t order);

  FOG_CAPI_METHOD(void, fecolormatrix_mapArgb32)(const FeColorMatrix* self, Argb32* dst, const Argb32* src);
  FOG_CAPI_METHOD(void, fecolormatrix_mapArgb64)(const FeColorMatrix* self, Argb64* dst, const Argb64* src);
  FOG_CAPI_METHOD(void, fecolormatrix_mapArgbF)(const FeColorMatrix* self, ArgbF* dst, const ArgbF* src);

  FOG_CAPI_STATIC(void, fecolormatrix_copy)(float* dst, const float* src);
  FOG_CAPI_STATIC(bool, fecolormatrix_eq)(const FeColorMatrix* a, const FeColorMatrix* b);

  const FeColorMatrix* fecolormatrix_oIdentity;
  const FeColorMatrix* fecolormatrix_oZero;
  const FeColorMatrix* fecolormatrix_oGreyscale;
  const FeColorMatrix* fecolormatrix_oPreHue;
  const FeColorMatrix* fecolormatrix_oPostHue;

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeComponentFunction]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(fecomponentfunction_ctor)(FeComponentFunction* self);
  FOG_CAPI_CTOR(fecomponentfunction_ctorCopy)(FeComponentFunction* self, const FeComponentFunction* other);
  FOG_CAPI_CTOR(fecomponentfunction_dtor)(FeComponentFunction* self);

  FOG_CAPI_METHOD(bool, fecomponentfunction_resultsInIdentity)(const FeComponentFunction* self);

  FOG_CAPI_METHOD(err_t, fecomponentfunction_getData)(const FeComponentFunction* self, uint32_t functionType, void* functionData);
  FOG_CAPI_METHOD(err_t, fecomponentfunction_setData)(FeComponentFunction* self, uint32_t functionType, const void* functionData);

  FOG_CAPI_METHOD(void, fecomponentfunction_reset)(FeComponentFunction* self);
  FOG_CAPI_METHOD(err_t, fecomponentfunction_copy)(FeComponentFunction* self, const FeComponentFunction* other);
  FOG_CAPI_STATIC(bool, fecomponentfunction_eq)(const FeComponentFunction* a, const FeComponentFunction* b);

  FOG_CAPI_STATIC(FeComponentFunctionData*, fecomponentfunction_dCreate)(uint32_t functionType, const void* functionData);
  FOG_CAPI_STATIC(void, fecomponentfunction_dFree)(FeComponentFunctionData* d);

  FeComponentFunction* fecomponentfunction_oIdentity;

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeComponentTransfer]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(fecomponenttransfer_ctor)(FeComponentTransfer* self);
  FOG_CAPI_CTOR(fecomponenttransfer_ctorCopy)(FeComponentTransfer* self, const FeComponentTransfer* other);
  FOG_CAPI_CTOR(fecomponenttransfer_dtor)(FeComponentTransfer* self);

  FOG_CAPI_METHOD(void, fecomponenttransfer_reset)(FeComponentTransfer* self);
  FOG_CAPI_METHOD(err_t, fecomponenttransfer_copy)(FeComponentTransfer* self, const FeComponentTransfer* other);
  FOG_CAPI_STATIC(bool, fecomponenttransfer_eq)(const FeComponentTransfer* a, const FeComponentTransfer* b);

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeConvolveMatrix]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(feconvolvematrix_ctor)(FeConvolveMatrix* self);
  FOG_CAPI_CTOR(feconvolvematrix_ctorCopy)(FeConvolveMatrix* self, const FeConvolveMatrix* other);
  FOG_CAPI_CTOR(feconvolvematrix_dtor)(FeConvolveMatrix* self);

  FOG_CAPI_METHOD(void, feconvolvematrix_reset)(FeConvolveMatrix* self);
  FOG_CAPI_METHOD(err_t, feconvolvematrix_copy)(FeConvolveMatrix* self, const FeConvolveMatrix* other);
  FOG_CAPI_STATIC(bool, feconvolvematrix_eq)(const FeConvolveMatrix* a, const FeConvolveMatrix* b);

  // --------------------------------------------------------------------------
  // [G2d/Imaging/Filters - FeMorphology]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(femorphology_ctor)(FeMorphology* self);
  FOG_CAPI_CTOR(femorphology_ctorCopy)(FeMorphology* self, const FeMorphology* other);
  FOG_CAPI_CTOR(femorphology_dtor)(FeMorphology* self);

  FOG_CAPI_METHOD(void, femorphology_reset)(FeMorphology* self);
  FOG_CAPI_METHOD(err_t, femorphology_copy)(FeMorphology* self, const FeMorphology* other);
  FOG_CAPI_STATIC(bool, femorphology_eq)(const FeMorphology* a, const FeMorphology* b);

  // --------------------------------------------------------------------------
  // [G2d/Painting - Painter]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(err_t, painter_beginImage)(Painter* self, Image* image, const RectI* rect, uint32_t initFlags);
  FOG_CAPI_METHOD(err_t, painter_beginIBits)(Painter* self, const ImageBits* imageBits, const RectI* rect, uint32_t initFlags);
  FOG_CAPI_METHOD(err_t, painter_switchToImage)(Painter* self, Image* image, const RectI* rect);
  FOG_CAPI_METHOD(err_t, painter_switchToIBits)(Painter* self, const ImageBits* imageBits, const RectI* rect);
  FOG_CAPI_STATIC(PaintEngine*, painter_getNullEngine)();

  // --------------------------------------------------------------------------
  // [G2d/Source - Color]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL* Color_Convert)(void* dst, const void* src);
  Color_Convert color_convert[_COLOR_MODEL_COUNT][_COLOR_MODEL_COUNT];

  FOG_CAPI_METHOD(err_t, color_setModel)(Color* self, uint32_t model);
  FOG_CAPI_METHOD(err_t, color_setData)(Color* self, uint32_t modelExtended, const void* modelData);
  FOG_CAPI_METHOD(err_t, color_mix)(Color* self, uint32_t mixOp, uint32_t alphaOp, const Color* secondary, float mask);
  FOG_CAPI_METHOD(err_t, color_adjust)(Color* self, uint32_t adjustOp, float param);

  FOG_CAPI_METHOD(err_t, color_parseA)(Color* self, const StubA* str, uint32_t flags);
  FOG_CAPI_METHOD(err_t, color_parseU)(Color* self, const StubW* str, uint32_t flags);

  // --------------------------------------------------------------------------
  // [G2d/Source - ColorStopList]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(colorstoplist_ctor)(ColorStopList* self);
  FOG_CAPI_CTOR(colorstoplist_ctorCopy)(ColorStopList* self, const ColorStopList* other);
  FOG_CAPI_DTOR(colorstoplist_dtor)(ColorStopList* self);

  FOG_CAPI_METHOD(err_t, colorstoplist_reserve)(ColorStopList* self, size_t n);
  FOG_CAPI_METHOD(void, colorstoplist_squeeze)(ColorStopList* self);
  FOG_CAPI_METHOD(err_t, colorstoplist_setData)(ColorStopList* self, const ColorStop* stops, size_t length);
  FOG_CAPI_METHOD(bool, colorstoplist_isOpaque)(const ColorStopList* self);
  FOG_CAPI_METHOD(bool, colorstoplist_isOpaqueARGB32)(const ColorStopList* self);
  FOG_CAPI_METHOD(void, colorstoplist_clear)(ColorStopList* self);
  FOG_CAPI_METHOD(void, colorstoplist_reset)(ColorStopList* self);
  FOG_CAPI_METHOD(err_t, colorstoplist_addArgb32)(ColorStopList* self, float offset, const Argb32* argb32);
  FOG_CAPI_METHOD(err_t, colorstoplist_addColor)(ColorStopList* self, float offset, const Color* color);
  FOG_CAPI_METHOD(err_t, colorstoplist_addColorStop)(ColorStopList* self, const ColorStop* stop);
  FOG_CAPI_METHOD(err_t, colorstoplist_removeOffset)(ColorStopList* self, float offset);
  FOG_CAPI_METHOD(err_t, colorstoplist_removeStop)(ColorStopList* self, const ColorStop* stop);
  FOG_CAPI_METHOD(err_t, colorstoplist_removeAt)(ColorStopList* self, size_t index);
  FOG_CAPI_METHOD(err_t, colorstoplist_removeRange)(ColorStopList* self, const Range* range);
  FOG_CAPI_METHOD(err_t, colorstoplist_removeInterval)(ColorStopList* self, const IntervalF* interval);
  FOG_CAPI_METHOD(size_t, colorstoplist_indexOfOffset)(const ColorStopList* self, float offset);
  FOG_CAPI_METHOD(err_t, colorstoplist_copy)(ColorStopList* self, const ColorStopList* other);

  FOG_CAPI_STATIC(bool, colorstoplist_eq)(const ColorStopList* a, const ColorStopList* b);
  FOG_CAPI_STATIC(ColorStopListData*, colorstoplist_dCreate)(size_t capacity);
  FOG_CAPI_STATIC(void, colorstoplist_dFree)(ColorStopListData* d);

  ColorStopList* colorstoplist_oEmpty;

  // --------------------------------------------------------------------------
  // [G2d/Source - GradientF]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(gradientf_ctor)(GradientF* self, uint32_t gradientType);
  FOG_CAPI_CTOR(gradientf_ctorCopyF)(GradientF* self, const GradientF* other);
  FOG_CAPI_CTOR(gradientf_ctorCopyD)(GradientF* self, const GradientD* other);
  FOG_CAPI_DTOR(gradientf_dtor)(GradientF* self);

  FOG_CAPI_METHOD(err_t, gradientf_copyF)(GradientF* self, const GradientF* other);
  FOG_CAPI_METHOD(err_t, gradientf_copyD)(GradientF* self, const GradientD* other);
  FOG_CAPI_METHOD(void, gradientf_reset)(GradientF* self);

  FOG_CAPI_STATIC(bool, gradientf_eq)(const GradientF* a, const GradientF* b);

  // --------------------------------------------------------------------------
  // [G2d/Source - GradientD]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(gradientd_ctor)(GradientD* self, uint32_t gradientType);
  FOG_CAPI_CTOR(gradientd_ctorCopyF)(GradientD* self, const GradientF* other);
  FOG_CAPI_CTOR(gradientd_ctorCopyD)(GradientD* self, const GradientD* other);
  FOG_CAPI_DTOR(gradientd_dtor)(GradientD* self);

  FOG_CAPI_METHOD(err_t, gradientd_copyF)(GradientD* self, const GradientF* other);
  FOG_CAPI_METHOD(err_t, gradientd_copyD)(GradientD* self, const GradientD* other);
  FOG_CAPI_METHOD(void, gradientd_reset)(GradientD* self);

  FOG_CAPI_STATIC(bool, gradientd_eq)(const GradientD* a, const GradientD* b);

  // --------------------------------------------------------------------------
  // [G2d/Source - Pattern]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(pattern_ctor)(Pattern* self);
  FOG_CAPI_CTOR(pattern_ctorCopy)(Pattern* self, const Pattern* other);
  FOG_CAPI_CTOR(pattern_ctorArgb32)(Pattern* self, const ArgbBase32* argb32);
  FOG_CAPI_CTOR(pattern_ctorColor)(Pattern* self, const Color* color);
  FOG_CAPI_CTOR(pattern_ctorTextureF)(Pattern* self, const Texture* texture, const TransformF* tr);
  FOG_CAPI_CTOR(pattern_ctorTextureD)(Pattern* self, const Texture* texture, const TransformD* tr);
  FOG_CAPI_CTOR(pattern_ctorGradientF)(Pattern* self, const GradientF* gradient, const TransformF* tr);
  FOG_CAPI_CTOR(pattern_ctorGradientD)(Pattern* self, const GradientD* gradient, const TransformD* tr);
  FOG_CAPI_DTOR(pattern_dtor)(Pattern* self);

  FOG_CAPI_METHOD(err_t, pattern_detach)(Pattern* self);
  FOG_CAPI_METHOD(err_t, pattern_getArgb32)(const Pattern* self, ArgbBase32* argb32);
  FOG_CAPI_METHOD(err_t, pattern_getColor)(const Pattern* self, Color* color);
  FOG_CAPI_METHOD(err_t, pattern_getTexture)(const Pattern* self, Texture* texture);
  FOG_CAPI_METHOD(err_t, pattern_getGradientF)(const Pattern* self, uint32_t targetType, GradientF* gradient);
  FOG_CAPI_METHOD(err_t, pattern_getGradientD)(const Pattern* self, uint32_t targetType, GradientD* gradient);
  FOG_CAPI_METHOD(err_t, pattern_getTransformF)(const Pattern* self, TransformF* tr);
  FOG_CAPI_METHOD(err_t, pattern_getTransformD)(const Pattern* self, TransformD* tr);
  FOG_CAPI_METHOD(err_t, pattern_createArgb32)(Pattern* self, const ArgbBase32* argb32);
  FOG_CAPI_METHOD(err_t, pattern_createColor)(Pattern* self, const Color* color);
  FOG_CAPI_METHOD(err_t, pattern_createTextureF)(Pattern* self, const Texture* texture, const TransformF* tr);
  FOG_CAPI_METHOD(err_t, pattern_createTextureD)(Pattern* self, const Texture* texture, const TransformD* tr);
  FOG_CAPI_METHOD(err_t, pattern_createGradientF)(Pattern* self, const GradientF* gradient, const TransformF* tr);
  FOG_CAPI_METHOD(err_t, pattern_createGradientD)(Pattern* self, const GradientD* gradient, const TransformD* tr);
  FOG_CAPI_METHOD(err_t, pattern_setTransformF)(Pattern* self, const TransformF* tr);
  FOG_CAPI_METHOD(err_t, pattern_setTransformD)(Pattern* self, const TransformD* tr);
  FOG_CAPI_METHOD(err_t, pattern_applyTransform)(Pattern* self, uint32_t transformOp, const void* params);
  FOG_CAPI_METHOD(err_t, pattern_resetTransform)(Pattern* self);
  FOG_CAPI_METHOD(void, pattern_reset)(Pattern* self);
  FOG_CAPI_METHOD(err_t, pattern_copy)(Pattern* self, const Pattern* other);

  FOG_CAPI_STATIC(bool, pattern_eq)(const Pattern* a, const Pattern* b);
  FOG_CAPI_STATIC(PatternData*, pattern_dCreate)(size_t size);
  FOG_CAPI_STATIC(void, pattern_dFree)(PatternData* d);

  Pattern* pattern_oNull;

  // --------------------------------------------------------------------------
  // [G2d/Text - Font]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(font_ctor)(Font* self);
  FOG_CAPI_CTOR(font_ctorCopy)(Font* self, const Font* other);
  FOG_CAPI_DTOR(font_dtor)(Font* self);

  FOG_CAPI_METHOD(err_t, font_detach)(Font* self);
  FOG_CAPI_METHOD(err_t, font_setHeight)(Font* self, float height, uint32_t unit);
  FOG_CAPI_METHOD(err_t, font_setLetterSpacing)(Font* self, float spacing, uint32_t spacingMode);
  FOG_CAPI_METHOD(err_t, font_setWordSpacing)(Font* self, float spacing, uint32_t spacingMode);
  FOG_CAPI_METHOD(err_t, font_setHints)(Font* self, const FontHints* hints);
  FOG_CAPI_METHOD(err_t, font_setStyle)(Font* self, uint32_t style);
  FOG_CAPI_METHOD(err_t, font_setWeight)(Font* self, uint32_t weight);
  FOG_CAPI_METHOD(err_t, font_setVariant)(Font* self, uint32_t variant);
  FOG_CAPI_METHOD(err_t, font_setDecoration)(Font* self, uint32_t decoration);
  FOG_CAPI_METHOD(err_t, font_setKerning)(Font* self, uint32_t kerning);
  FOG_CAPI_METHOD(err_t, font_setHinting)(Font* self, uint32_t hinting);
  FOG_CAPI_METHOD(err_t, font_setAlignMode)(Font* self, uint32_t alignMode);
  FOG_CAPI_METHOD(err_t, font_setTransform)(Font* self, const TransformF* transform);
  FOG_CAPI_METHOD(err_t, font_setForceCaching)(Font* self, bool val);
  FOG_CAPI_METHOD(void, font_reset)(Font* self);
  FOG_CAPI_METHOD(err_t, font_create)(Font* self, const StringW* family, float height, uint32_t unit);
  FOG_CAPI_METHOD(err_t, font_createEx)(Font* self, const StringW* family, float height, uint32_t unit,
    const FontHints* hints, const TransformF* transform);
  FOG_CAPI_METHOD(err_t, font_fromFace)(Font* self, FontFace* face, float height, uint32_t unit);
  FOG_CAPI_METHOD(err_t, font_getTextOutlineFStubW)(const Font* self, PathF* dst, uint32_t cntOp, const PointF* pt, const StubW* str);
  FOG_CAPI_METHOD(err_t, font_getTextOutlineFStringW)(const Font* self, PathF* dst, uint32_t cntOp, const PointF* pt, const StringW* str);
  FOG_CAPI_METHOD(err_t, font_getTextOutlineDStubW)(const Font* self, PathD* dst, uint32_t cntOp, const PointD* pt, const StubW* str);
  FOG_CAPI_METHOD(err_t, font_getTextOutlineDStringW)(const Font* self, PathD* dst, uint32_t cntOp, const PointD* pt, const StringW* str);
  FOG_CAPI_METHOD(err_t, font_getTextExtentsFStubW)(const Font* self, TextExtentsF* extents, const StubW* str);
  FOG_CAPI_METHOD(err_t, font_getTextExtentsFStringW)(const Font* self, TextExtentsF* extents, const StringW* str);
  FOG_CAPI_METHOD(err_t, font_getTextExtentsDStubW)(const Font* self, TextExtentsD* extents, const StubW* str);
  FOG_CAPI_METHOD(err_t, font_getTextExtentsDStringW)(const Font* self, TextExtentsD* extents, const StringW* str);
  FOG_CAPI_METHOD(err_t, font_copy)(Font* self, const Font* other);

  FOG_CAPI_STATIC(bool, font_eq)(const Font* a, const Font* b);
  FOG_CAPI_STATIC(FontData*, font_dCreate)(void);
  FOG_CAPI_STATIC(void, font_dFree)(FontData* d);

  // --------------------------------------------------------------------------
  // [G2d/Tools - Dpi]
  // --------------------------------------------------------------------------

  FOG_CAPI_METHOD(void, dpi_reset)(Dpi* self);
  FOG_CAPI_METHOD(err_t, dpi_setDpi)(Dpi* self, float dpi);
  FOG_CAPI_METHOD(err_t, dpi_setDpiEmEx)(Dpi* self, float dpi, float em, float ex);
  FOG_CAPI_METHOD(void, dpi_copy)(Dpi* self, const Dpi* other);

  // --------------------------------------------------------------------------
  // [G2d/Tools - MatrixF]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(matrixf_ctor)(MatrixF* self);
  FOG_CAPI_CTOR(matrixf_ctorCopy)(MatrixF* self, const MatrixF* other);
  FOG_CAPI_CTOR(matrixf_ctorCreate)(MatrixF* self, const SizeI* size, const float* data);
  FOG_CAPI_DTOR(matrixf_dtor)(MatrixF* self);

  FOG_CAPI_METHOD(err_t, matrixf_detach)(MatrixF* self);
  FOG_CAPI_METHOD(err_t, matrixf_create)(MatrixF* self, const SizeI* size, const float* data);
  FOG_CAPI_METHOD(err_t, matrixf_resize)(MatrixF* self, const SizeI* size, float value);
  FOG_CAPI_METHOD(float, matrixf_getCell)(const MatrixF* self, int x, int y);
  FOG_CAPI_METHOD(err_t, matrixf_setCell)(MatrixF* self, int x, int y, float value);
  FOG_CAPI_METHOD(err_t, matrixf_fill)(MatrixF* self, const RectI* area, float value);
  FOG_CAPI_METHOD(void, matrixf_reset)(MatrixF* self);
  FOG_CAPI_METHOD(void, matrixf_copy)(MatrixF* self, const MatrixF* other);

  FOG_CAPI_STATIC(bool, matrixf_eq)(const MatrixF* a, const MatrixF* b);
  FOG_CAPI_STATIC(MatrixDataF*, matrixf_dCreate)(const SizeI* size, const float* data);
  FOG_CAPI_STATIC(void, matrixf_dFree)(MatrixDataF* d);

  MatrixF* matrixf_oEmpty;

  // --------------------------------------------------------------------------
  // [G2d/Tools - MatrixD]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(matrixd_ctor)(MatrixD* self);
  FOG_CAPI_CTOR(matrixd_ctorCopy)(MatrixD* self, const MatrixD* other);
  FOG_CAPI_CTOR(matrixd_ctorCreate)(MatrixD* self, const SizeI* size, const double* data);
  FOG_CAPI_DTOR(matrixd_dtor)(MatrixD* self);

  FOG_CAPI_METHOD(err_t, matrixd_detach)(MatrixD* self);
  FOG_CAPI_METHOD(err_t, matrixd_create)(MatrixD* self, const SizeI* size, const double* data);
  FOG_CAPI_METHOD(err_t, matrixd_resize)(MatrixD* self, const SizeI* size, double value);

  FOG_CAPI_METHOD(double, matrixd_getCell)(const MatrixD* self, int x, int y);
  FOG_CAPI_METHOD(err_t, matrixd_setCell)(MatrixD* self, int x, int y, double value);
  FOG_CAPI_METHOD(err_t, matrixd_fill)(MatrixD* self, const RectI* area, double value);
  FOG_CAPI_METHOD(void, matrixd_reset)(MatrixD* self);
  FOG_CAPI_METHOD(void, matrixd_copy)(MatrixD* self, const MatrixD* other);

  FOG_CAPI_STATIC(bool, matrixd_eq)(const MatrixD* a, const MatrixD* b);
  FOG_CAPI_STATIC(MatrixDataD*, matrixd_dCreate)(const SizeI* size, const double* data);
  FOG_CAPI_STATIC(void, matrixd_dFree)(MatrixDataD* d);

  MatrixD* matrixd_oEmpty;

  // --------------------------------------------------------------------------
  // [G2d/Tools - Region]
  // --------------------------------------------------------------------------

  FOG_CAPI_CTOR(region_ctor)(Region* self);
  FOG_CAPI_CTOR(region_ctorRegion)(Region* self, const Region* other);
  FOG_CAPI_CTOR(region_ctorBox)(Region* self, const BoxI* box);
  FOG_CAPI_CTOR(region_ctorRect)(Region* self, const RectI* rect);
  FOG_CAPI_DTOR(region_dtor)(Region* self);

  FOG_CAPI_METHOD(err_t, region_detach)(Region* self);
  FOG_CAPI_METHOD(err_t, region_reserve)(Region* self, size_t capacity);
  FOG_CAPI_METHOD(void, region_squeeze)(Region* self);
  FOG_CAPI_METHOD(err_t, region_prepare)(Region* self, size_t count);

  FOG_CAPI_METHOD(uint32_t, region_getType)(const Region* self);

  FOG_CAPI_METHOD(void, region_clear)(Region* self);
  FOG_CAPI_METHOD(void, region_reset)(Region* self);

  FOG_CAPI_METHOD(err_t, region_setRegion)(Region* self, const Region* other);
  FOG_CAPI_METHOD(err_t, region_setDeep)(Region* self, const Region* other);
  FOG_CAPI_METHOD(err_t, region_setBox)(Region* self, const BoxI* box);
  FOG_CAPI_METHOD(err_t, region_setRect)(Region* self, const RectI* rect);

  FOG_CAPI_METHOD(err_t, region_setBoxList)(Region* self, const BoxI* data, size_t length);
  FOG_CAPI_METHOD(err_t, region_setRectList)(Region* self, const RectI* data, size_t length);

  FOG_CAPI_METHOD(err_t, region_combineRegionRegion)(Region* dst, const Region* a, const Region* b, uint32_t combineOp);
  FOG_CAPI_METHOD(err_t, region_combineRegionBox)(Region* dst, const Region* a, const BoxI* b, uint32_t combineOp);
  FOG_CAPI_METHOD(err_t, region_combineBoxRegion)(Region* dst, const BoxI* a, const Region* b, uint32_t combineOp);
  FOG_CAPI_METHOD(err_t, region_combineBoxBox)(Region* dst, const BoxI* a, const BoxI* b, uint32_t combineOp);

  FOG_CAPI_METHOD(err_t, region_translate)(Region* dst, const Region* src, const PointI* pt);
  FOG_CAPI_METHOD(err_t, region_translateAndClip)(Region* dst, const Region* src, const PointI* pt, const BoxI* clipBox);
  FOG_CAPI_METHOD(err_t, region_intersectAndClip)(Region* dst, const Region* a, const Region* b, const BoxI* clipBox);

  FOG_CAPI_METHOD(uint32_t, region_hitTestPoint)(const Region* self, const PointI* pt);
  FOG_CAPI_METHOD(uint32_t, region_hitTestBox)(const Region* self, const BoxI* box);
  FOG_CAPI_METHOD(uint32_t, region_hitTestRect)(const Region* self, const RectI* rect);

  FOG_CAPI_METHOD(bool, region_eq)(const Region* a, const Region* b);

#if defined(FOG_OS_WINDOWS)
  FOG_CAPI_METHOD(err_t, region_hrgnFromRegion)(HRGN* dst, const Region* src);
  FOG_CAPI_METHOD(err_t, region_regionFromHRGN)(Region* dst, HRGN src);
#endif // FOG_OS_WINDOWS

  FOG_CAPI_STATIC(RegionData*, region_dCreate)(size_t capacity);
  FOG_CAPI_STATIC(RegionData*, region_dCreateBox)(size_t capacity, const BoxI* box);
  FOG_CAPI_STATIC(RegionData*, region_dCreateRegion)(size_t capacity, const BoxI* data, size_t count, const BoxI* extent);
  FOG_CAPI_STATIC(RegionData*, region_dAdopt)(void* address, size_t capacity);
  FOG_CAPI_STATIC(RegionData*, region_dAdoptBox)(void* address, size_t capacity, const BoxI* box);
  FOG_CAPI_STATIC(RegionData*, region_dAdoptRegion)(void* address, size_t capacity, const BoxI* data, size_t count, const BoxI* bbox);
  FOG_CAPI_STATIC(RegionData*, region_dRealloc)(RegionData* d, size_t capacity);
  FOG_CAPI_STATIC(RegionData*, region_dCopy)(const RegionData* d);
  FOG_CAPI_STATIC(void, region_dFree)(RegionData* d);

  Region* region_oEmpty;
  Region* region_oInfinite;

  // --------------------------------------------------------------------------
  // [G2d/Tools - RegionUtil]
  // --------------------------------------------------------------------------

  FOG_CAPI_STATIC(const BoxI*, regionutil_getClosestBox)(const BoxI* data, size_t length, int y);
  FOG_CAPI_STATIC(bool, regionutil_isBoxListSorted)(const BoxI* data, size_t length);
  FOG_CAPI_STATIC(bool, regionutil_isRectListSorted)(const RectI* data, size_t length);
};

extern FOG_API Api _api;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_API_H
