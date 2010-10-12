// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_STD_H
#define _FOG_CORE_STD_H

// [Dependencies]
#include <Fog/Core/Build.h>

#include <stdio.h>

//! @addtogroup Fog_Core_Private
//! @{

FOG_CVAR_EXTERN FILE* fog_stdout;
FOG_CVAR_EXTERN FILE* fog_stderr;

FOG_CVAR_EXTERN uint32_t fog_failed;

FOG_CAPI_EXTERN void fog_stdout_msg(const char* namespaceStr, const char* methodStr, const char* format, ...);
FOG_CAPI_EXTERN void fog_stderr_msg(const char* namespaceStr, const char* methodStr, const char* format, ...);
FOG_CAPI_EXTERN void fog_stdxxx_vmsg(FILE* fp, const char* namespaceStr, const char* methodStr, const char* format, va_list ap);
FOG_CAPI_EXTERN bool fog_redirect_std_to_file(const char* fileName);

FOG_CAPI_EXTERN void fog_debug(const char* format, ...);
FOG_CAPI_EXTERN void FOG_NO_RETURN fog_fail(const char* format, ...);

FOG_CAPI_EXTERN void fog_sleep(uint32_t msecs);

typedef int (*fog_qsort_callback)(const void*, const void*);

FOG_CAPI_EXTERN void fog_qsort(void *a, sysuint_t n, sysuint_t es, fog_qsort_callback compar);
FOG_CAPI_EXTERN void *fog_bsearch(const void *key, const void *base, sysuint_t nmemb, sysuint_t size, fog_qsort_callback compar);

// [Out of memory]

FOG_CAPI_EXTERN bool fog_out_of_memory();
FOG_CAPI_EXTERN void FOG_NO_RETURN fog_out_of_memory_fatal();
FOG_CAPI_EXTERN void FOG_NO_RETURN fog_out_of_memory_fatal_format(
  const char* namespaceStr, 
  const char* methodStr,
  const char* format, ...);

namespace Fog {
namespace Std {

FOG_API sysuint_t calcOptimalCapacity(sysuint_t sizeof_d, sysuint_t sizeof_element, sysuint_t before, sysuint_t after);

//! @brief Checks if @a index and @a range are correct positions in @a length. @a range
//! can be modified. Returns @c true if range check is ok.
// TODO: DEPRECATED
FOG_INLINE bool checkRange(sysuint_t length, sysuint_t index, sysuint_t *range)
{
  if (index >= length) return false;

  sysuint_t max = length - index;
  if (*range > max) *range = max;
  return true;
}

// TODO: RENAME
FOG_INLINE bool checkRange_A(sysuint_t length, sysuint_t start, sysuint_t *end)
{
  if (start >= length) return false;
  if (*end > length) *end = length;
  return true;
}

} // Std namespace
} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_CORE_STD_H
