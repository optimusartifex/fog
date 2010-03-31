// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Std.h>

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(FOG_OS_POSIX)
#include <unistd.h>
#endif // FOG_OS_POSIX

#include <stdarg.h>

// private
static FILE* fog_stdfile;

// [Fog::Std]

FOG_CVAR_DECLARE FILE* fog_stdout;
FOG_CVAR_DECLARE FILE* fog_stderr;

FOG_CVAR_DECLARE uint32_t fog_failed;

FOG_CAPI_DECLARE void fog_stdout_msg(const char* namespaceStr, const char* methodStr, const char* format, ...)
{
  va_list ap;
  va_start(ap, format);

  fog_stdxxx_vmsg(fog_stdout, namespaceStr, methodStr, format, ap);

  va_end(ap);
}

FOG_CAPI_DECLARE void fog_stderr_msg(const char* namespaceStr, const char* methodStr, const char* format, ...)
{
  va_list ap;
  va_start(ap, format);

  fog_stdxxx_vmsg(fog_stderr, namespaceStr, methodStr, format, ap);

  va_end(ap);
}

FOG_CAPI_DECLARE void fog_stdxxx_vmsg(FILE* fp, const char* namespaceStr, const char* methodStr, const char* format, va_list ap)
{
  if (namespaceStr)
  {
    fputs(namespaceStr, fp);
    fputs("::", fp);
  }

  if (methodStr)
  {
    fputs(methodStr, fp);
    fputs("() ", fp);
  }

  vfprintf(fp, format, ap);
  fputc('\n', fp);
  fflush(fp);
}

FOG_CAPI_DECLARE bool fog_redirect_std_to_file(const char* fileName)
{
  FILE* file = fopen(fileName, "w");

  if (fog_stdfile) fclose(fog_stdfile);

  if (file)
  {
    fog_stdfile = file;
    fog_stderr = file;
    fog_stdout = file;
  }
  else
  {
    fog_stdfile = NULL;
  }

  return fog_stdfile != NULL;
}

FOG_CAPI_DECLARE void fog_debug(const char* format, ...)
{
  va_list ap;
  va_start(ap, format);

  fog_stdxxx_vmsg(fog_stderr, NULL, NULL, format, ap);

  va_end(ap);
}

FOG_CAPI_DECLARE void FOG_NO_RETURN fog_fail(const char* format, ...)
{
  va_list ap;
  va_start(ap, format);

  fog_stdxxx_vmsg(fog_stderr, NULL, NULL, format, ap);

  va_end(ap);

  fog_failed = 1;
  exit(EXIT_FAILURE);
}

FOG_CAPI_DECLARE void fog_sleep(uint32_t msecs)
{
#if defined(FOG_OS_WINDOWS)
  Sleep(msecs);
#elif defined(FOG_OS_POSIX)
  usleep((useconds_t)msecs * (useconds_t)1000);
#else
#error "fog_sleep - Not implemented"
#endif
}

// Copyright (c) 1992, 1993
// The Regents of the University of California. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".

static FOG_INLINE char* fog_qsort_med3(char* a, char* b, char* c, fog_qsort_callback compar)
{
  return compar(a, b) < 0
    ? (compar(b, c) < 0 ? b : (compar(a, c) < 0 ? c : a ))
    : (compar(b, c) > 0 ? b : (compar(a, c) < 0 ? a : c ));
}

#define fog_qsort_swapcode(TYPE, parmi, parmj, n) \
{ \
  sysint_t i = (n) / sizeof (TYPE); \
  TYPE *pi = (TYPE *) (parmi); \
  TYPE *pj = (TYPE *) (parmj); \
  do { \
    TYPE t = *pi; \
    *pi++ = *pj; \
    *pj++ = t; \
  } while (--i > 0); \
}

#define fog_qsort_swapinit(a, es) \
  swaptype = ((char *)a - (char *)0) % sizeof(sysint_t) || \
  es % sizeof(sysint_t) ? 2 : es == sizeof(sysint_t)? 0 : 1;

static FOG_INLINE void fog_qsort_swapfunc(char *a, char *b, sysint_t n, sysint_t swaptype)
{
  if (swaptype <= 1)
    fog_qsort_swapcode(sysint_t, a, b, n)
  else
    fog_qsort_swapcode(char, a, b, n)
}

#define fog_qsort_swap(a, b) \
  if (swaptype == 0) \
  { \
    sysint_t t = *(sysint_t *)(a); \
    *(sysint_t *)(a) = *(sysint_t *)(b); \
    *(sysint_t *)(b) = t; \
  } \
  else \
    fog_qsort_swapfunc((char*)(a), (char*)(b), es, swaptype) /* ; */

#define fog_qsort_vecswap(a, b, n) \
  if ((n) > 0) \
    fog_qsort_swapfunc((char*)(a), (char*)(b), n, swaptype)

FOG_CAPI_DECLARE void fog_qsort(void *a, sysuint_t n, sysuint_t es, fog_qsort_callback compar)
{
  char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
  sysint_t d, r, swaptype, swap_cnt;

loop:
  fog_qsort_swapinit(a, es);
  swap_cnt = 0;
  if (n < 7) {
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
      for (pl = pm; pl > (char *) a && compar(pl - es, pl) > 0; pl -= es)
        fog_qsort_swap(pl, pl - es);
    return;
  }
  pm = (char *) a + (n / 2) * es;
  if (n > 7)
  {
    pl = (char *)a;
    pn = (char *)a + (n - 1) * es;
    if (n > 40)
    {
      d = (n / 8) * es;
      pl = fog_qsort_med3(pl, pl + d, pl + 2 * d, compar);
      pm = fog_qsort_med3(pm - d, pm, pm + d, compar);
      pn = fog_qsort_med3(pn - 2 * d, pn - d, pn, compar);
    }
    pm = fog_qsort_med3(pl, pm, pn, compar);
  }
  fog_qsort_swap(a, pm);
  pa = pb = (char *) a + es;

  pc = pd = (char *) a + (n - 1) * es;
  for (;;)
  {
    while (pb <= pc && (r = compar(pb, a)) <= 0)
    {
      if (r == 0)
      {
        swap_cnt = 1;
        fog_qsort_swap(pa, pb);
        pa += es;
      }
      pb += es;
    }
    while (pb <= pc && (r = compar(pc, a)) >= 0)
    {
      if (r == 0)
      {
        swap_cnt = 1;
        fog_qsort_swap(pc, pd);
        pd -= es;
      }
      pc -= es;
    }
    if (pb > pc) break;
    fog_qsort_swap(pb, pc);
    swap_cnt = 1;
    pb += es;
    pc -= es;
  }

  // Switch to insertion sort.
  if (swap_cnt == 0) {
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
      for (pl = pm; pl > (char *) a && compar(pl - es, pl) > 0; pl -= es)
        fog_qsort_swap(pl, pl - es);
    return;
  }

  pn = (char *) a + n * es;
  r = Fog::Math::min(pa - (char *)a, pb - pa);
  fog_qsort_vecswap(a, pb - r, r);
  r = Fog::Math::min((sysint_t)(pd - pc), (sysint_t)(pn - pd) - (sysint_t)es);
  fog_qsort_vecswap(pb, pn - r, r);
  if ((sysuint_t)(r = pb - pa) > es) fog_qsort(a, (sysuint_t)r / es, es, compar);
  if ((sysuint_t)(r = pd - pc) > es)
  {
    // Iterate rather than recurse to save stack space.
    a = pn - r;
    n = r / es;
    goto loop;
  }
}

FOG_CAPI_DECLARE void *fog_bsearch(const void *key, const void *base, sysuint_t nmemb, sysuint_t size, fog_qsort_callback compar)
{
  const char *_base = (const char*)base;
  sysuint_t lim;
  int cmp;
  const void *cur;

  for (lim = nmemb; lim != 0; lim >>= 1)
  {
    cur = _base + (lim >> 1) * size;
    cmp = compar(key, cur);
    if (cmp == 0)
    {
      return ((void *)cur);
    }
    if (cmp > 0)
    {
      _base = (char *)cur + size;
      lim--;
    }
  }
  return NULL;
}

// [Out of memory]

FOG_CAPI_DECLARE bool fog_out_of_memory()
{
  return false;
}

FOG_CAPI_DECLARE void FOG_NO_RETURN fog_out_of_memory_fatal()
{
  fog_debug("Out of memory!\n");
  fog_fail("Exit!\n");
}

FOG_CAPI_DECLARE void FOG_NO_RETURN fog_out_of_memory_fatal_format(
  const char* namespaceStr, 
  const char* methodStr,
  const char* format, ...)
{
  va_list ap;
  va_start(ap, format);

  fog_debug("Out of Memory\n"
             "-------------\n");
  fog_stdxxx_vmsg(fog_stderr, namespaceStr, methodStr, format, ap);

  va_end(ap);
  
  fog_fail("");
}

namespace Fog {
namespace Std {

sysuint_t calcOptimalCapacity(sysuint_t sizeof_d, sysuint_t sizeof_element, sysuint_t before, sysuint_t after)
{
  // Threshold for excessive growing. If data size in memory is larger than this,
  // grow will not be excessive but constant.
  const sysuint_t minThreshold = 128;
  const sysuint_t maxThreshold = 1024 * 1024 * 8;

  FOG_ASSERT(before < after);

  sysuint_t beforeSize = sizeof_d + before * sizeof_element;
  sysuint_t afterSize = sizeof_d + after * sizeof_element;
  sysuint_t recommend;

  if (afterSize < minThreshold)
  {
    recommend = minThreshold;
  }
  else if (beforeSize < maxThreshold && afterSize < maxThreshold)
  {
    recommend = beforeSize;
    while (recommend < afterSize) recommend += ((recommend + 2) >> 1);
  }
  else
  {
    recommend = Math::max(beforeSize + maxThreshold, afterSize);
  }

  recommend = (recommend + 15) & ~15;
  recommend = ((recommend - sizeof_d) / sizeof_element);

  FOG_ASSERT(recommend >= after);
  return recommend;
}

} // Std namespace
} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_std_init(void)
{
  using namespace Fog;

  fog_stdfile = NULL;
  fog_stdout = stdout;
  fog_stderr = stderr;
  fog_failed = 0;

  setlocale(LC_ALL, "");

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_std_shutdown(void)
{
  using namespace Fog;

  if (fog_stdfile) fclose(fog_stdfile);
}
