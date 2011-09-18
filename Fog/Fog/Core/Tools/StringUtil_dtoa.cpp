// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/FloatControl.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharData.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>

// [Dependencies - C]
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#if defined(FOG_HAVE_FLOAT_H)
# include <float.h>
#endif // FOG_HAVE_FLOAT_H

#if defined(FOG_HAVE_FENV_H)
# include <fenv.h>
#endif // FOG_HAVE_FENV_H

namespace Fog {

// ============================================================================
// [Fog::StringUtil - dtoa]
// ============================================================================

// The author of this software is David M. Gay.
//
// Copyright (c) 1991, 2000, 2001 by Lucent Technologies.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose without fee is hereby granted, provided that this entire notice
// is included in all copies of any software which is or includes a copy
// or modification of this software and in all copies of the supporting
// documentation for such software.
//
// THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR LUCENT MAKES ANY
// REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
// OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
//
// Please send bug reports to David M. Gay
// (dmg at acm dot org, with " at " changed at "@" and " dot " changed to ".").

// On a machine with IEEE extended-precision registers, it is necessary to
// specify double-precision (53-bit) rounding precision before invoking atod
// or dtoa. If the machine uses (the equivalent of) Intel 80x87 arithmetic, the
// call to _control87(PC_53, MCW_PC) does this with many compilers. Whether
// this or another call is appropriate depends on the compiler; for this to
// work, it may be necessary to #include "float.h" or another system-dependent
// header file.

// atod() for IEEE-, VAX-, and IBM-arithmetic machines.
//
// This atod() returns a nearest machine number to the input decimal string (or
// sets errno to ERANGE). With IEEE arithmetic, ties are broken by the IEEE
// round-even rule. Otherwise ties are broken by biased rounding (add half and
// chop).
//
// Inspired loosely by William D. Clinger's paper "How to Read Floating Point
// Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 92-101].
//
// Modifications:
//
//  1. We only require IEEE, IBM, or VAX double-precision arithmetic.
//
//  2. We get by with floating-point arithmetic in a case that Clinger missed
//     -- when we're computing d * 10^n for a small integer d and the integer
//        N is not too much larger than 22 (the maximum integer K for which we
//        can represent 10^k exactly), we may be able to compute
//
//          (d*10^k) * 10^(e-k)
//
//        with just one _RoundOff.
//
//  3. Rather than a bit-at-a-time adjustment of the binary result in the hard
//     case, we use floating-point arithmetic to determine the adjustment to
//     within one bit; only in really hard cases do we need to compute a second
//     residual.
//
//  4. Because of 3., we don't need a large table of powers of 10 for ten-to-e
//     (just some small tables, e.g. of 10^k for 0 <= k <= 22).

// #define DTOA_IEEE for IEEE-arithmetic machines.
// #define DTOA_IBM for IBM mainframe-style floating-point arithmetic.
// #define DTOA_VAX for VAX-style floating-point arithmetic (D_floating).
//
// #define DTOA_NO_LEFTRIGHT to omit left-right logic in fast floating-point
//   computation of dtoa.
//
// #define DTOA_HONOR_FLOAT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3
//   and atod and dtoa should round accordingly.
//
// #define DTOA_CHECK_FLOAT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3
//   and DTOA_HONOR_FLOAT_ROUNDS is not #defined.
//
// #define DTOA_RND_PRODQUOT to use rnd_prod and rnd_quot (assembly routines
//   that use extended-precision instructions to compute rounded
//   products and quotients) with IBM.
//
// #define DTOA_ROUND_BIASED for IEEE-format with biased rounding.
//
// #define DTOA_INACCURATE_DIVIDE for IEEE-format with correctly rounded
//   products but inaccurate quotients, e.g., for Intel i860.
//
// #define DTOA_INFNAN_CHECK on IEEE systems to cause atod to check for
//   Infinity and NaN (case insensitively).
//
// #define DTOA_NO_IEEE_SCALE to disable new (Feb. 1997) logic in atod that
//   avoids underflows on inputs whose result does not underflow.
//   If you #define DTOA_NO_IEEE_SCALE on a machine that uses IEEE-format
//   floating-point numbers and flushes underflows to zero rather
//   than implementing gradual underflow, then you must also #define
//   DTOA_SUDDEN_OVERFLOW.
//
// #define DTOA_SET_INEXACT if IEEE arithmetic is being used and extra
//   computation should be done to set the inexact flag when the
//   result is inexact and avoid setting inexact when the result
//   is exact.  In this case, dtoa.c must be compiled in
//   an environment, perhaps provided by #include "dtoa.c" in a
//   suitable wrapper, that defines two functions,
//     int get_inexact(void);
//     void clear_inexact(void);
//   such that get_inexact() returns a nonzero value if the
//   inexact bit is already set, and clear_inexact() sets the
//   inexact bit to 0.  When DTOA_SET_INEXACT is #defined, atod
//   also does extra computations to set the underflow and overflow
//   flags when appropriate (i.e., when the result is tiny and
//   inexact or when it is a numeric value rounded to +-infinity).

// IEEE is the default.
#if !defined(DTOA_IEEE) && !defined(DTOA_VAX) && !defined(DTOA_IBM)
# define DTOA_IEEE
#endif

#if defined(DTOA_IEEE) + defined(DTOA_VAX) + defined(DTOA_IBM) != 1
# error "Exactly one of DTOA_IEEE, DTOA_VAX, or DTOA_IBM should be defined."
#endif

#if !defined(FOG_HAVE_FLOAT_H) && defined(DTOA_IEEE)
# define DBL_DIG 15
# define DBL_MAX_10_EXP 308
# define DBL_MAX_EXP 1024
# define FLT_RADIX 2
#endif // !FOG_HAVE_FLOAT_H && DTOA_IEEE

#if !defined(FOG_HAVE_FLOAT_H) && defined(DTOA_IBM)
# define DBL_DIG 16
# define DBL_MAX_10_EXP 75
# define DBL_MAX_EXP 63
# define FLT_RADIX 16
# define DBL_MAX 7.2370055773322621e+75
#endif // !FOG_HAVE_FLOAT_H && DTOA_IBM

#if !defined(FOG_HAVE_FLOAT_H) && defined(DTOA_VAX)
# define DBL_DIG 16
# define DBL_MAX_10_EXP 38
# define DBL_MAX_EXP 127
# define FLT_RADIX 2
# define DBL_MAX 1.7014118346046923e+38
#endif // !FOG_HAVE_FLOAT_H && DTOA_VAX

// TODO: Fix

// The following definition of Storeinc is appropriate for MIPS processors.
// An alternative that may be better on some machines is
// #define Storeinc(a,b,c) (*a++ = b << 16 | c & 0xffff)
#if defined(DTOA_IEEE) || defined(DTOA_VAX)
# define Storeinc(a, b, c) \
   (((unsigned short *)a)[1] = (unsigned short)b, ((unsigned short *)a)[0] = (unsigned short)c, a++)
#else
# define Storeinc(a, b, c) \
  (((unsigned short *)a)[0] = (unsigned short)b, ((unsigned short *)a)[1] = (unsigned short)c, a++)
#endif

// #define P DBL_MANT_DIG
// Ten_pmax = floor(P*log(2)/log(5))
// Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16
// Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1)
// Int_max = floor(P*log(FLT_RADIX)/log(10) - 1)

#define DTOA_INFNAN_CHECK
#define DTOA_HONOR_FLOAT_ROUNDS

#if defined(DTOA_IEEE)
# define Exp_shift  20
# define Exp_shift1 20
# define Exp_msk1    0x100000
# define Exp_msk11   0x100000
# define Exp_mask  0x7FF00000
# define P 53
# define Bias 1023
# define Emin (-1022)
# define Exp_1  0x3FF00000
# define Exp_11 0x3FF00000
# define Ebits 11
# define Frac_mask  0xFFFFF
# define Frac_mask1 0xFFFFF
# define Ten_pmax 22
# define Bletch 0x10
# define Bndry_mask  0xFFFFF
# define Bndry_mask1 0xFFFFF
# define LSB 1
# define Sign_bit 0x80000000
# define Log2P 1
# define Tiny0 0
# define Tiny1 1
# define Quick_max 14
# define Int_max 14
# ifndef DTOA_NO_IEEE_SCALE
#  define DTOA_AVOID_UNDERFLOW
#  ifdef Flush_Denorm  /* debugging option */
#   undef DTOA_SUDDEN_OVERFLOW
#  endif
# endif

# ifndef Flt_Rounds
#  ifdef FLT_ROUNDS
#   define Flt_Rounds FLT_ROUNDS
#  else
#   define Flt_Rounds 1
#  endif
# endif // Flt_Rounds

# ifdef DTOA_HONOR_FLOAT_ROUNDS
#  define Rounding rounding
#  undef DTOA_CHECK_FLOAT_ROUNDS
#  define DTOA_CHECK_FLOAT_ROUNDS
# else
#  define Rounding Flt_Rounds
# endif

#else // !DTOA_IEEE
# undef DTOA_CHECK_FLOAT_ROUNDS
# undef DTOA_HONOR_FLOAT_ROUNDS
# undef DTOA_SET_INEXACT
# undef  DTOA_SUDDEN_OVERFLOW
# define DTOA_SUDDEN_OVERFLOW
# ifdef DTOA_IBM
#  undef Flt_Rounds
#  define Flt_Rounds 0
#  define Exp_shift  24
#  define Exp_shift1 24
#  define Exp_msk1   0x1000000
#  define Exp_msk11  0x1000000
#  define Exp_mask  0x7F000000
#  define P 14
#  define Bias 65
#  define Exp_1  0x41000000
#  define Exp_11 0x41000000
#  define Ebits 8 // Exponent has 7 bits, but 8 is the right value in b2d.
#  define Frac_mask  0xFFFFFF
#  define Frac_mask1 0xFFFFFF
#  define Bletch 4
#  define Ten_pmax 22
#  define Bndry_mask  0xEFFFFF
#  define Bndry_mask1 0xFFFFFF
#  define LSB 1
#  define Sign_bit 0x80000000
#  define Log2P 4
#  define Tiny0 0x100000
#  define Tiny1 0
#  define Quick_max 14
#  define Int_max 15
# else // DTOA_VAX
#  undef Flt_Rounds
#  define Flt_Rounds 1
#  define Exp_shift  23
#  define Exp_shift1 7
#  define Exp_msk1    0x80
#  define Exp_msk11   0x800000
#  define Exp_mask  0x7F80
#  define P 56
#  define Bias 129
#  define Exp_1  0x40800000
#  define Exp_11 0x4080
#  define Ebits 8
#  define Frac_mask  0x7FFFFF
#  define Frac_mask1 0xFFFF007F
#  define Ten_pmax 24
#  define Bletch 2
#  define Bndry_mask  0xFFFF007F
#  define Bndry_mask1 0xFFFF007F
#  define LSB 0x10000
#  define Sign_bit 0x8000
#  define Log2P 1
#  define Tiny0 0x80
#  define Tiny1 0
#  define Quick_max 15
#  define Int_max 15
# endif // DTOA_IBM || DTOA_VAX
#endif // DTOA_IEEE

#ifndef DTOA_IEEE
# define DTOA_ROUND_BIASED
#endif

#if defined(DTOA_RND_PRODQUOT)
extern "C" double rnd_prod(double, double);
extern "C" double rnd_quot(double, double);
# define rounded_product(a, b) a = rnd_prod(a, b)
# define rounded_quotient(a, b) a = rnd_quot(a, b)
#else
# define rounded_product(a, b) a *= b
# define rounded_quotient(a, b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1 * (DBL_MAX_EXP + Bias - 1))
#define Big1 0xFFFFFFFF

struct BInt
{
  BInt* next;
  int k, maxwds, sign, wds;
  uint32_t x[1];
};

struct BContext
{
  BInt* freelist[16];
  BInt* p5s;
  char* memoryNext;
  uint remain;
  uint dynamic;

  // Where to escape if allocation failed.
  jmp_buf escape;

  // This memory was 5124 bytes * sizeof(double). It's 40kB of memory and
  // it's much more actually needed for common numbers. Decreased to 2kB.
  char memory[2048];
};

static void BContext_init(BContext* context)
{
  size_t i;

  for (i = 0; i != FOG_ARRAY_SIZE(context->freelist); i++)
  {
    context->freelist[i] = NULL;
  }
  context->p5s = NULL;
  context->memoryNext = context->memory;
  context->remain = FOG_ARRAY_SIZE(context->memory);
  context->dynamic = 0;
}

static void BContext_destroy(BContext* context)
{
  // Only free if there are dynamic block(s).
  if (context->dynamic)
  {
    BInt* bi;
    BInt* next;
    size_t i;

    for (i = 0; i != FOG_ARRAY_SIZE(context->freelist); i++)
    {
      for (bi = context->freelist[i]; bi; bi = next)
      {
        next = bi->next;
        if ((char*)bi < context->memory || (char*)bi >= context->memory + FOG_ARRAY_SIZE(context->memory))
        {
          MemMgr::free((void*)bi);
        }
      }
    }
  }
}

static BInt* BContext_balloc(BContext* context, int k)
{
  BInt* rv;
  int x;
  uint len;

  if ((rv = context->freelist[k]))
  {
    context->freelist[k] = rv->next;
  }
  else
  {
    x = 1 << k;
    len = ((sizeof(BInt) + (x-1) * sizeof(uint32_t) + sizeof(double) - 1) / sizeof(double)) * sizeof(double);
    if (context->remain > len)
    {
      rv = (BInt*)context->memoryNext;
      context->memoryNext += len;
      context->remain -= len;
    }
    else
    {
      rv = (BInt*)MemMgr::alloc(len);
      if (FOG_IS_NULL(rv)) longjmp(context->escape, 1);
      context->dynamic += len;
    }
    rv->k = k;
    rv->maxwds = x;
  }
  rv->sign = rv->wds = 0;
  return rv;
}

static void BContext_bfree(BContext* context, BInt* v)
{
  if (v)
  {
    v->next = context->freelist[v->k];
    context->freelist[v->k] = v;
  }
}

#define Bcopy(x, y) memcpy((char *)&x->sign, (char *)&y->sign, y->wds*sizeof(int32_t) + 2*sizeof(int))

// Multiply by m and add a.
static BInt* BContext_madd(BContext* context, BInt* b, int m, int a)
{
  int i, wds;
#if FOG_ARCH_BITS >= 64
  uint32_t *x;
  uint64_t carry, y;
#else
  uint32_t carry, *x, y;
  uint32_t xi, z;
#endif
  BInt* b1;

  wds = b->wds;
  x = b->x;
  i = 0;
  carry = a;
  do {
#if FOG_ARCH_BITS >= 64
    y = *x * (uint64_t)m + carry;
    carry = y >> 32;
    *x++ = y & 0xFFFFFFFF;
#else
    xi = *x;
    y = (xi & 0xffff) * m + carry;
    z = (xi >> 16) * m + (y >> 16);
    carry = z >> 16;
    *x++ = (z << 16) + (y & 0xffff);
#endif
  } while (++i < wds);

  if (carry)
  {
    if (wds >= b->maxwds)
    {
      b1 = BContext_balloc(context, b->k+1);
      Bcopy(b1, b);
      BContext_bfree(context, b);
      b = b1;
    }
    b->x[wds++] = (uint32_t)carry;
    b->wds = wds;
  }
  return b;
}

static int hi0bits(uint32_t x)
{
  int k = 0;

  if (!(x & 0xffff0000))
  {
    k = 16;
    x <<= 16;
  }
  if (!(x & 0xff000000))
  {
    k += 8;
    x <<= 8;
  }
  if (!(x & 0xf0000000))
  {
    k += 4;
    x <<= 4;
  }
  if (!(x & 0xc0000000))
  {
    k += 2;
    x <<= 2;
  }
  if (!(x & 0x80000000))
  {
    k++;
    if (!(x & 0x40000000)) return 32;
  }
  return k;
}

static int lo0bits(uint32_t *y)
{
  int k;
  uint32_t x = *y;

  if (x & 7)
  {
    if (x & 1)
    {
      return 0;
    }
    if (x & 2)
    {
      *y = x >> 1;
      return 1;
    }
    *y = x >> 2;
    return 2;
  }
  k = 0;
  if (!(x & 0xffff))
  {
    k = 16;
    x >>= 16;
  }
  if (!(x & 0xff))
  {
    k += 8;
    x >>= 8;
  }
  if (!(x & 0xf))
  {
    k += 4;
    x >>= 4;
  }
  if (!(x & 0x3))
  {
    k += 2;
    x >>= 2;
  }
  if (!(x & 1))
  {
    k++;
    x >>= 1;
    if (!x) return 32;
  }
  *y = x;
  return k;
}

static BInt* BContext_i2b(BContext* context, int i)
{
  BInt* b = BContext_balloc(context, 1);
  b->x[0] = i;
  b->wds = 1;
  return b;
}

static BInt* BContext_mult(BContext* context, BInt* a, BInt* b)
{
  BInt* c;
  int k, wa, wb, wc;
  uint32_t *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
  uint32_t y;
#if FOG_ARCH_BITS >= 64
  uint64_t carry, z;
#else
  uint32_t carry, z;
  uint32_t z2;
#endif

  if (a->wds < b->wds)
  {
    c = a;
    a = b;
    b = c;
  }

  k = a->k;
  wa = a->wds;
  wb = b->wds;
  wc = wa + wb;

  if (wc > a->maxwds)
    k++;

  c = BContext_balloc(context, k);
  for (x = c->x, xa = x + wc; x < xa; x++)
    *x = 0;

  xa = a->x;
  xae = xa + wa;
  xb = b->x;
  xbe = xb + wb;
  xc0 = c->x;

#if FOG_ARCH_BITS >= 64
  for (; xb < xbe; xc0++)
  {
    if ((y = *xb++))
    {
      x = xa;
      xc = xc0;
      carry = 0;
      do {
        z = *x++ * (uint64_t)y + *xc + carry;
        carry = z >> 32;
        *xc++ = z & 0xFFFFFFFF;
      } while (x < xae);
      *xc = (uint32_t)carry;
    }
  }
#else
  for (; xb < xbe; xb++, xc0++)
  {
    if ((y = *xb & 0xffff))
    {
      x = xa;
      xc = xc0;
      carry = 0;
      do {
        z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
        carry = z >> 16;
        z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
        carry = z2 >> 16;
        Storeinc(xc, z2, z);
      } while (x < xae);
      *xc = (uint32_t)carry;
    }
    if ((y = *xb >> 16))
    {
      x = xa;
      xc = xc0;
      carry = 0;
      z2 = *xc;
      do {
        z = (*x & 0xffff) * y + (*xc >> 16) + carry;
        carry = z >> 16;
        Storeinc(xc, z, z2);
        z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
        carry = z2 >> 16;
      } while (x < xae);
      *xc = (uint32_t)z2;
    }
  }
#endif
  for (xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
  c->wds = wc;
  return c;
}

static BInt* BContext_pow5mult(BContext* context, BInt* b, int k)
{
  BInt* b1, *p5, *p51;
  int i;
  static int p05[3] = { 5, 25, 125 };

  if ((i = k & 3)) b = BContext_madd(context, b, p05[i-1], 0);

  if (!(k >>= 2)) return b;
  if (!(p5 = context->p5s))
  {
    p5 = context->p5s = BContext_i2b(context, 625);
    p5->next = 0;
  }
  for (;;)
  {
    if (k & 1)
    {
      b1 = BContext_mult(context, b, p5);
      BContext_bfree(context, b);
      b = b1;
    }
    if (!(k >>= 1)) break;
    if (!(p51 = p5->next))
    {
      p51 = p5->next = BContext_mult(context, p5, p5);
      p51->next = 0;
    }
    p5 = p51;
  }
  return b;
}

static BInt* BContext_lshift(BContext* context, BInt* b, int k)
{
  int i, k1, n, n1;
  BInt* b1;
  uint32_t *x, *x1, *xe, z;

  n = k >> 5;
  k1 = b->k;
  n1 = n + b->wds + 1;
  for (i = b->maxwds; n1 > i; i <<= 1) k1++;
  b1 = BContext_balloc(context, k1);
  x1 = b1->x;
  for (i = 0; i < n; i++) *x1++ = 0;
  x = b->x;
  xe = x + b->wds;
  if (k &= 0x1f)
  {
    k1 = 32 - k;
    z = 0;
    do {
      *x1++ = *x << k | z;
      z = *x++ >> k1;
    } while (x < xe);
    if ((*x1 = z)) ++n1;
  }
  else
  {
    do {
      *x1++ = *x++;
    } while (x < xe);
  }
  b1->wds = n1 - 1;
  BContext_bfree(context, b);
  return b1;
}

static int cmp(BInt* a, BInt* b)
{
  uint32_t *xa, *xa0, *xb, *xb0;
  int i, j;

  i = a->wds;
  j = b->wds;

  FOG_ASSERT_X(i <= 1 || a->x[i - 1] != 0, "Fog::StringUtil::dtoa() - cmp called with a->x[a->wds-1] == 0");
  FOG_ASSERT_X(j <= 1 || a->x[j - 1] != 0, "Fog::StringUtil::dtoa() - cmp called with b->x[b->wds-1] == 0");

  if (i -= j) return i;
  xa0 = a->x;
  xa = xa0 + j;
  xb0 = b->x;
  xb = xb0 + j;
  for (;;)
  {
    if (*--xa != *--xb)
      return *xa < *xb ? -1 : 1;
    if (xa <= xa0)
      break;
  }
  return 0;
}

static BInt* BContext_diff(BContext* context, BInt* a, BInt* b)
{
  BInt* c;
  int i, wa, wb;
  uint32_t *xa, *xae, *xb, *xbe, *xc;
#if FOG_ARCH_BITS >= 64
  uint64_t borrow, y;
#else
  uint32_t borrow, y;
  uint32_t z;
#endif

  i = cmp(a,b);
  if (!i)
  {
    c = BContext_balloc(context, 0);
    c->wds = 1;
    c->x[0] = 0;
    return c;
  }
  if (i < 0)
  {
    c = a;
    a = b;
    b = c;
    i = 1;
  }
  else
    i = 0;

  c = BContext_balloc(context, a->k);
  c->sign = i;
  wa = a->wds;
  xa = a->x;
  xae = xa + wa;
  wb = b->wds;
  xb = b->x;
  xbe = xb + wb;
  xc = c->x;
  borrow = 0;

#if FOG_ARCH_BITS >= 64
  do {
    y = (uint64_t)*xa++ - *xb++ - borrow;
    borrow = y >> 32 & (uint32_t)1;
    *xc++ = y & 0xFFFFFFFF;
  } while (xb < xbe);

  while (xa < xae)
  {
    y = *xa++ - borrow;
    borrow = y >> 32 & (uint32_t)1;
    *xc++ = y & 0xFFFFFFFF;
  }
#else
  do {
    y = (*xa & 0xffff) - (*xb & 0xffff) - borrow;
    borrow = (y & 0x10000) >> 16;
    z = (*xa++ >> 16) - (*xb++ >> 16) - borrow;
    borrow = (z & 0x10000) >> 16;
    Storeinc(xc, z, y);
  } while (xb < xbe);

  while (xa < xae)
  {
    y = (*xa & 0xffff) - borrow;
    borrow = (y & 0x10000) >> 16;
    z = (*xa++ >> 16) - borrow;
    borrow = (z & 0x10000) >> 16;
    Storeinc(xc, z, y);
  }
#endif
  while (!*--xc) wa--;
  c->wds = wa;
  return c;
}

static double ulp(double _x)
{
  DoubleBits a, x;
  int32_t L;

  x.d = _x;

  L = (x.u32Hi & Exp_mask) - (P-1) * Exp_msk1;
#if !defined(DTOA_AVOID_UNDERFLOW)
#if !defined(DTOA_SUDDEN_OVERFLOW)
  if (L > 0)
  {
#endif
#endif
#if defined(DTOA_IBM)
    L |= Exp_msk1 >> 4;
#endif // DTOA_IBM
    a.u32Hi = L;
    a.u32Lo = 0;
#if !defined(DTOA_AVOID_UNDERFLOW)
#if !defined(DTOA_SUDDEN_OVERFLOW)
  }
  else
  {
    L = -L >> Exp_shift;
    if (L < Exp_shift)
    {
      a.u32Hi = 0x80000 >> L;
      a.u32Lo = 0;
    }
    else
    {
      L -= Exp_shift;
      a.u32Hi = 0;
      a.u32Lo = L >= 31 ? 1 : 1 << 31 - L;
    }
  }
#endif
#endif
  return a.d;
}

static double b2d(BInt* a, int *e)
{
  uint32_t *xa, *xa0, w, y, z;
  int k;
  DoubleBits d;

#if defined(DTOA_VAX)
  uint32_t d0;
  uint32_t d1;
#else
#define d0 d.u32Hi
#define d1 d.u32Lo
#endif // DTOA_VAX

  xa0 = a->x;
  xa = xa0 + a->wds;

  y = *--xa;
  FOG_ASSERT_X(y != 0, "Fog::StringUtil::dtoa() - Zero y in b2d.");

  k = hi0bits(y);
  *e = 32 - k;

  if (k < Ebits)
  {
    d.u32Hi = Exp_1 | (y >> (Ebits - k));
    w = xa > xa0 ? *--xa : 0;
    d.u32Lo = (y << (32-Ebits + k)) | (w >> (Ebits - k));
    goto _Ret;
  }
  z = xa > xa0 ? *--xa : 0;
  if (k -= Ebits)
  {
    d.u32Hi = Exp_1 | y << k | (z >> (32 - k));
    y = xa > xa0 ? *--xa : 0;
    d.u32Lo = (z << k) | (y >> (32 - k));
  }
  else
  {
    d.u32Hi = Exp_1 | y;
    d.u32Lo = z;
  }

_Ret:
#if defined(DTOA_VAX)
  d.u32Hi = (d.u32Hi >> 16) | (d.u32Hi << 16);
  d.u32Lo = (d.u32Lo >> 16) | (d.u32Lo << 16);
#endif // DTOA_VAX

  return d.d;
}

static BInt* BContext_d2b(BContext* context, double _d, int *e, int *bits)
{
  DoubleBits d;
  d.d = _d;

  BInt* b;
  int de, k;
  uint32_t *x, y, z;

#if !defined(DTOA_SUDDEN_OVERFLOW)
  int i;
#endif // !DTOA_SUDDEN_OVERFLOW

#if defined(DTOA_VAX)
  d.u32Hi = (d.u32Hi >> 16) | (d.u32Hi << 16);
  d.u32Lo = (d.u32Lo >> 16) | (d.u32Lo << 16);
#endif // DTOA_VAX

  b = BContext_balloc(context, 1);
  x = b->x;
  z = d.u32Hi & Frac_mask;
  // Clear sign bit, which we ignore.
  d.u32Hi &= 0x7FFFFFFF;

#if defined(DTOA_SUDDEN_OVERFLOW)
  de = (int)(d.u32Hi >> Exp_shift);
#if !defined(DTOA_IBM)
  z |= Exp_msk11;
#endif // !DTOA_IBM
#else
  if ((de = (int)(d.u32Hi >> Exp_shift))) z |= Exp_msk1;
#endif // DTOA_SUDDEN_OVERFLOW

  if ((y = d.u32Lo))
  {
    if ((k = lo0bits(&y)))
    {
      x[0] = y | (z << (32 - k));
      z >>= k;
    }
    else
      x[0] = y;
#if !defined(DTOA_SUDDEN_OVERFLOW)
    i =
#endif // !DTOA_SUDDEN_OVERFLOW
        b->wds = (x[1] = z) ? 2 : 1;
  }
  else
  {
    FOG_ASSERT_X(z != 0, "Fog::StringUtil::dtoa() - Zero passed to d2b.");
    k = lo0bits(&z);
    x[0] = z;
#if !defined(DTOA_SUDDEN_OVERFLOW)
    i =
#endif // !DTOA_SUDDEN_OVERFLOW
        b->wds = 1;
    k += 32;
  }
#if !defined(DTOA_SUDDEN_OVERFLOW)
  if (de)
  {
#endif // !DTOA_SUDDEN_OVERFLOW
#if defined(DTOA_IBM)
    *e = (de - Bias - (P-1) << 2) + k;
    *bits = 4*P + 8 - k - hi0bits(d.u32Hi & Frac_mask);
#else
    *e = de - Bias - (P-1) + k;
    *bits = P - k;
#endif // DTOA_IBM
#if !defined(DTOA_SUDDEN_OVERFLOW)
  }
  else
  {
    *e = de - Bias - (P-1) + 1 + k;
    *bits = 32*i - hi0bits(x[i-1]);
  }
#endif // !DTOA_SUDDEN_OVERFLOW

  return b;
}

static double ratio(BInt* a, BInt* b)
{
  DoubleBits da, db;
  int k, ka, kb;

  da.d = b2d(a, &ka);
  db.d = b2d(b, &kb);

  k = ka - kb + 32*(a->wds - b->wds);

#if defined(DTOA_IBM)
  if (k > 0)
  {
    da.u32Hi += (k >> 2) * Exp_msk1;
    if (k &= 3) da.d *= 1 << k;
  }
  else
  {
    k = -k;
    db.u32Hi += (k >> 2) * Exp_msk1;
    if (k &= 3) db.d *= 1 << k;
  }
#else
  if (k > 0)
  {
    da.u32Hi += k * Exp_msk1;
  }
  else
  {
    k = -k;
    db.u32Hi += k * Exp_msk1;
  }
#endif

  return da.d / db.d;
}

static const double tens[] =
{
  1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
  1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
  1e20, 1e21, 1e22
#if defined(DTOA_VAX)
  , 1e23, 1e24
#endif // DTOA_VAX
};

#if defined(DTOA_IEEE)
static const double bigtens[] = { 1e16, 1e32, 1e64, 1e128, 1e256 };
static const double tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128,
#if defined(DTOA_AVOID_UNDERFLOW)
  9007199254740992.*9007199254740992.e-256
  // = 2^106 * 1e-53
#else
  1e-256
#endif // DTOA_AVOID_UNDERFLOW
};

// The factor of 2^53 in tinytens[4] helps us avoid setting the underflow
// flag unnecessarily.  It leads to a song and dance at the end of atod.
#define Scale_Bit 0x10
#endif // DTOA_IEEE

#if defined(DTOA_IBM)
static const double bigtens[] = { 1e16, 1e32, 1e64 };
static const double tinytens[] = { 1e-16, 1e-32, 1e-64 };
#undef DTOA_INFNAN_CHECK
#endif // DTOA_IBM

#if defined(DTOA_VAX)
static const double bigtens[] = { 1e16, 1e32 };
static const double tinytens[] = { 1e-16, 1e-32 };
#undef DTOA_INFNAN_CHECK
#endif // DTOA_VAX

FOG_NO_EXPORT double _mprec_log10(int dig)
{
  double v = 1.0;

  if (dig < FOG_ARRAY_SIZE(tens))
    return tens[dig];

  while (dig > 0)
  {
    v *= 10;
    dig--;
  }

  return v;
}

static int quorem(BInt* b, BInt* S)
{
  int n;
  uint32_t *bx, *bxe, q, *sx, *sxe;

#if FOG_ARCH_BITS >= 64
  uint64_t borrow, carry, y, ys;
#else
  uint32_t borrow, carry, y, ys;
  uint32_t si, z, zs;
#endif

  n = S->wds;
  FOG_ASSERT_X(b->wds <= n, "Fog::StringUtil::dtoa() - Oversize b in quorem.");

  if (b->wds < n)
    return 0;

  sx = S->x;
  sxe = sx + --n;
  bx = b->x;
  bxe = bx + n;

  // Ensure q <= true quotient.
  q = *bxe / (*sxe + 1);
  FOG_ASSERT_X(q <= 9, "Fog::StringUtil::dtoa() - Oversized quotient in quorem.");

  if (q)
  {
    borrow = 0;
    carry = 0;
    do {
#if FOG_ARCH_BITS >= 64
      ys = *sx++ * (uint64_t)q + carry;
      carry = ys >> 32;
      y = *bx - (ys & 0xFFFFFFFF) - borrow;
      borrow = y >> 32 & (uint32_t)1;
      *bx++ = y & 0xFFFFFFFF;
#else
      si = *sx++;
      ys = (si & 0xffff) * q + carry;
      zs = (si >> 16) * q + (ys >> 16);
      carry = zs >> 16;
      y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
      borrow = (y & 0x10000) >> 16;
      z = (*bx >> 16) - (zs & 0xffff) - borrow;
      borrow = (z & 0x10000) >> 16;
      Storeinc(bx, z, y);
#endif
    } while (sx <= sxe);
    if (!*bxe)
    {
      bx = b->x;
      while (--bxe > bx && !*bxe) --n;
      b->wds = n;
    }
  }

  if (cmp(b, S) >= 0)
  {
    q++;
    borrow = 0;
    carry = 0;
    bx = b->x;
    sx = S->x;
    do {
#if FOG_ARCH_BITS >= 64
      ys = *sx++ + carry;
      carry = ys >> 32;
      y = *bx - (ys & 0xFFFFFFFF) - borrow;
      borrow = y >> 32 & (uint32_t)1;
      *bx++ = y & 0xFFFFFFFF;
#else
      si = *sx++;
      ys = (si & 0xffff) + carry;
      zs = (si >> 16) + (ys >> 16);
      carry = zs >> 16;
      y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
      borrow = (y & 0x10000) >> 16;
      z = (*bx >> 16) - (zs & 0xffff) - borrow;
      borrow = (z & 0x10000) >> 16;
      Storeinc(bx, z, y);
#endif
    } while (sx <= sxe);

    bx = b->x;
    bxe = bx + n;

    if (!*bxe)
    {
      while (--bxe > bx && !*bxe)
        --n;
      b->wds = n;
    }
  }
  return q;
}

// dtoa() for IEEE arithmetic (dmg): convert double to ASCII string.
//
// Inspired by "How to Print Floating-Point Numbers Accurately" by
// Guy L. Steele, Jr. and Jon L. White [Proc. ACM SIGPLAN '90, pp. 112-126].
//
// Modifications:
//
//  1. Rather than iterating, we use a simple numeric overestimate to determine
//     k = floor(log10(d)).  We scale relevant quantities using O(log2(k))
//     rather than O(k) multiplications.
//  2. For some modes > 2 (corresponding to ecvt and fcvt), we don't try to
//     generate digits strictly left to right.  Instead, we compute with fewer
//     bits and propagate the carry if necessary when rounding the final digit
//     up. This is often faster.
//  3. Under the assumption that input will be rounded nearest, mode 0 renders
//     1e23 as 1e23 rather than 9.999999999999999e22. That is, we allow
//     equality in stopping tests when the round-nearest rule will give the
//     same floating-point value as would satisfaction of the stopping test
//     with strict inequality.
//  4. We remove common factors of powers of 2 from relevant quantities.
//  5. When converting floating-point integers less than 1e16, we use
//     floating-point arithmetic rather than resorting to multiple-precision
//     integers.
//  6. When asked to produce fewer than 15 digits, we first try to get by with
//     floating-point arithmetic; we resort to multiple-precision integer
//     arithmetic only if we cannot guarantee that the floating-point
//     calculation has given the correctly rounded result. For k requested
//     digits and "uniformly" distributed input, the probability is something
//     like 10^(k-15) that we must resort to the integer calculation.
//
// Arguments ndigits, decpt, sign are similar to those of ecvt and fcvt;
// trailing zeros are suppressed from the returned string. If not null, *rve
// is set to point to the end of the return value.  If d is +-Infinity or NaN,
// then decpt is set to 9999.
//
// Form:
//    0 ==> shortest string that yields d when read in and rounded to nearest.
//    1 ==> like 0, but with Steele & White stopping rule; e.g. with IEEE P754
//      arithmetic , mode 0 gives 1e23 whereas mode 1 gives 9.999999999999999e22.
//    2 ==> max(1, ndigits) significant digits.  This gives a return value
//      similar to that of ecvt, except that trailing zeros are suppressed.
//    3 ==> through ndigits past the decimal point.  This gives a return value
//      similar to that from fcvt, except that trailing zeros are suppressed,
//      and ndigits can be negative.
//    4,5 ==> similar to 2 and 3, respectively, but (in round-nearest mode) with
//      the tests of mode 0 to possibly return a shorter string that rounds to d.
//      With IEEE arithmetic and compilation with -DDTOA_HONOR_FLOAT_ROUNDS,
//      modes 4 and 5 behave the same as modes 2 and 3 when FLT_ROUNDS != 1.
//    6-9 ==> Debugging modes similar to mode - 4:  don't try fast floating-point
//      estimate (if applicable).
//
//    Values of mode other than 0-9 are treated as mode 0.
static void FOG_CDECL StringUtil_dtoa(NTOAContext* ctx, double _d, uint32_t mode, int ndigits)
{
  FOG_CONTROL87_BEGIN();

  BContext context;

  DoubleBits d;
  DoubleBits d2, ds, eps;

  BInt* b, *b1, *delta, *mlo, *mhi, *S;
  char *s, *s0;

  int negative = 0;

  int bbits, b2, b5, be, dig, i, ieps, ilim, ilim0, ilim1,
    j, j1, k, k0, k_check, leftright, m2, m5, s2, s5,
    spec_case, try_quick;
  int32_t L;

#if !defined(DTOA_SUDDEN_OVERFLOW)
  int denorm;
  uint32_t x;
#endif // !DTOA_SUDDEN_OVERFLOW

#if defined(DTOA_HONOR_FLOAT_ROUNDS)
  int rounding;
#endif // DTOA_HONOR_FLOAT_ROUNDS

#if defined(DTOA_SET_INEXACT)
  int inexact, oldinexact;
#endif // DTOA_SET_INEXACT

  d.d = _d;

  ctx->result = ctx->buffer;
  s = s0 = (char*)ctx->buffer;

  if (d.u32Hi & Sign_bit)
  {
    /* set sign for everything, including 0's and NaNs */
    negative = 1;
    d.u32Hi &= ~Sign_bit;  /* clear sign bit */
  }

#if defined(DTOA_IEEE) || defined(DTOA_VAX)
#if defined(DTOA_IEEE)
  if ((d.u32Hi & Exp_mask) == Exp_mask)
#else
  if (d.u32Hi == 0x00008000)
#endif
  {
    FOG_CONTROL87_END();

    // Infinity or NaN
    ctx->decpt = 9999;
    ctx->negative = negative;

#if defined(DTOA_IEEE)
    if (d.u32Lo == 0 && (d.u32Hi & 0x000FFFFF) == 0)
    {
      memcpy(s, "Infinity", 8);
      ctx->length = 8;
    }
    else
#endif // DTOA_IEEE
    {
      memcpy(s, "NaN", 3);
      ctx->length = 3;
    }
    return;
  }
#endif // DTOA_IEEE || DTOA_VAX

#if defined(DTOA_IBM)
  // Normalize.
  d.d += 0;
#endif // DTOA_IBM

  if (!d.d)
  {
    FOG_CONTROL87_END();

    *s = '0';
    ctx->length = 1;
    ctx->decpt = 1;
    ctx->negative = negative;

    return;
  }

  BContext_init(&context);
  if (setjmp(context.escape))
  {
    ctx->result = ctx->buffer;
    ctx->result[0] = '0';
    ctx->result[1] = '\0';
    ctx->length = 1;
    ctx->decpt = 1;

    BContext_destroy(&context);
    return;
  }

#if defined(DTOA_SET_INEXACT)
  try_quick = oldinexact = get_inexact();
  inexact = 1;
#endif // DTOA_SET_INEXACT

#if defined(DTOA_HONOR_FLOAT_ROUNDS)
  if ((rounding = Flt_Rounds) >= 2)
  {
    if (negative)
      rounding = rounding == 2 ? 0 : 2;
    else
      if (rounding != 2) rounding = 0;
  }
#endif // DTOA_HONOR_FLOAT_ROUNDS

  b = BContext_d2b(&context, d.d, &be, &bbits);
#if defined(DTOA_SUDDEN_OVERFLOW)
  i = (int)(d.u32Hi >> Exp_shift1 & (Exp_mask>>Exp_shift1));
#else // DTOA_SUDDEN_OVERFLOW

  if ((i = (int)(d.u32Hi >> Exp_shift1 & (Exp_mask>>Exp_shift1))))
  {
#endif
    d2.d = d.d;
    d2.u32Hi &= Frac_mask1;
    d2.u32Hi |= Exp_11;

#if defined(DTOA_IBM)
    if (j = 11 - hi0bits(d2.u32Hi & Frac_mask))
      d2.d /= 1 << j;
#endif

    // log(x)  ~=~ log(1.5) + (x-1.5)/1.5
    // log10(x)   =  log(x) / log(10)
    //    ~=~ log(1.5)/log(10) + (x-1.5)/(1.5*log(10))
    // log10(d) = (i-Bias)*log(2)/log(10) + log10(d2)
    //
    // This suggests computing an approximation k to log10(d) by
    //
    // k = (i - Bias)*0.301029995663981
    //  + ( (d2-1.5)*0.289529654602168 + 0.176091259055681 );
    //
    // We want k to be too large rather than too small.
    // The error in the first-order Taylor series approximation
    // is in our favor, so we just round up the constant enough
    // to compensate for any error in the multiplication of
    // (i - Bias) by 0.301029995663981; since |i - Bias| <= 1077,
    // and 1077 * 0.30103 * 2^-52 ~=~ 7.2e-14,
    // adding 1e-13 to the constant term more than suffices.
    // Hence we adjust the constant term to 0.1760912590558.
    // (We could get a more accurate k by invoking log10,
    // but this is probably not worthwhile.)

    i -= Bias;
#if defined(DTOA_IBM)
    i <<= 2;
    i += j;
#endif
#if !defined(DTOA_SUDDEN_OVERFLOW)
    denorm = 0;
  }
  else
  {
    // d is denormalized

    i = bbits + be + (Bias + (P-1) - 1);
    x = (i > 32)
      ? (d.u32Hi << (64 - i)) | (d.u32Hi >> (i - 32))
      : (d.u32Lo << (32 - i));
    d2.d = x;
    d2.u32Hi -= 31 * Exp_msk1; // adjust exponent
    i -= (Bias + (P-1) - 1) + 1;
    denorm = 1;
  }
#endif
  ds.d = (d2.d - 1.5) * 0.289529654602168 + 0.1760912590558 + i * 0.301029995663981;
  k = (int)ds.d;
  if (ds.d < 0. && ds.d != k)
    k--; // want k = floor(ds.d)
  k_check = 1;
  if (k >= 0 && k <= Ten_pmax)
  {
    if (d.d < tens[k]) k--;
    k_check = 0;
  }
  j = bbits - i - 1;
  if (j >= 0)
  {
    b2 = 0;
    s2 = j;
  }
  else
  {
    b2 = -j;
    s2 = 0;
  }
  if (k >= 0)
  {
    b5 = 0;
    s5 = k;
    s2 += k;
  }
  else
  {
    b2 -= k;
    b5 = -k;
    s5 = 0;
  }

  if (mode < 0 || mode > 9)
    mode = 0;

#ifndef DTOA_SET_INEXACT
#if defined(DTOA_CHECK_FLOAT_ROUNDS)
  try_quick = Rounding == 1;
#else
  try_quick = 1;
#endif
#endif /*DTOA_SET_INEXACT*/

  if (mode > 5)
  {
    mode -= 4;
    try_quick = 0;
  }

  leftright = 1;
  switch (mode)
  {
    case 0:
    case 1:
      ilim = ilim1 = -1;
      i = 18;
      ndigits = 0;
      break;
    case 2:
      leftright = 0;
      /* no break */
    case 4:
      if (ndigits <= 0)
        ndigits = 1;
      ilim = ilim1 = i = ndigits;
      break;
    case 3:
      leftright = 0;
      /* no break */
    case 5:
      i = ndigits + k + 1;
      ilim = i;
      ilim1 = i - 1;
      if (i <= 0)
        i = 1;
  }

#if defined(DTOA_HONOR_FLOAT_ROUNDS)
  if (mode > 1 && rounding != 1) leftright = 0;
#endif

  if (ilim >= 0 && ilim <= Quick_max && try_quick)
  {
    // Try to get by with floating-point arithmetic.
    i = 0;
    d2.d = d.d;
    k0 = k;
    ilim0 = ilim;
    ieps = 2; /* conservative */
    if (k > 0)
    {
      ds.d = tens[k&0xf];
      j = k >> 4;
      if (j & Bletch)
      {
        /* prevent overflows */
        j &= Bletch - 1;
        d.d /= bigtens[FOG_ARRAY_SIZE(bigtens) - 1];
        ieps++;
      }
      for (; j; j >>= 1, i++)
      {
        if (j & 1)
        {
          ieps++;
          ds.d *= bigtens[i];
        }
      }
      d.d /= ds.d;
    }
    else if ((j1 = -k))
    {
      d.d *= tens[j1 & 0xf];
      for (j = j1 >> 4; j; j >>= 1, i++)
      {
        if (j & 1)
        {
          ieps++;
          d.d *= bigtens[i];
        }
      }
    }

    if (k_check && d.d < 1. && ilim > 0)
    {
      if (ilim1 <= 0)
        goto _FastFailed;

      ilim = ilim1;
      k--;
      d.d *= 10.0;
      ieps++;
    }

    eps.d = ieps * d.d + 7.;
    eps.u32Hi -= (P-1) * Exp_msk1;
    if (ilim == 0)
    {
      S = mhi = 0;
      d.d -= 5.;

      if (d.d > eps.d)
        goto _OneDigit;
      else if (d.d < -eps.d)
        goto _NoDigits;
      else
        goto _FastFailed;
    }
#ifndef DTOA_NO_LEFTRIGHT
    if (leftright)
    {
      /* Use Steele & White method of only
       * generating digits needed.
       */
      eps.d = 0.5/tens[ilim-1] - eps.d;
      for (i = 0;;)
      {
        L = (int32_t)d.d;
        d.d -= L;
        *s++ = '0' + (int)L;

        if (d.d < eps.d)
          goto _Ret1;
        if (1. - d.d < eps.d)
          goto _BumpUp;
        if (++i >= ilim)
          break;

        eps.d *= 10.;
        d.d *= 10.;
      }
    }
    else
    {
#endif
      /* Generate ilim digits, then fix them up. */
      eps.d *= tens[ilim-1];
      for (i = 1;; i++, d.d *= 10.)
      {
        L = (int32_t)(d.d);
        if (!(d.d -= L)) ilim = i;
        *s++ = '0' + (int)L;
        if (i == ilim)
        {
          if (d.d > 0.5 + eps.d)
          {
            goto _BumpUp;
          }
          else if (d.d < 0.5 - eps.d)
          {
            while (*--s == '0')
              continue;
            s++;
            goto _Ret1;
          }
          break;
        }
      }
#ifndef DTOA_NO_LEFTRIGHT
    }
#endif
_FastFailed:
    s = s0;
    d.d = d2.d;
    k = k0;
    ilim = ilim0;
  }

  // Do we have a "small" integer?
  if (be >= 0 && k <= Int_max)
  {
    // Yes.
    ds.d = tens[k];
    if (ndigits < 0 && ilim <= 0)
    {
      S = mhi = 0;
      if (ilim < 0 || d.d <= 5 * ds.d)
        goto _NoDigits;
      else
        goto _OneDigit;
    }
    for (i = 1;; i++, d.d *= 10.)
    {
      L = (int32_t)(d.d / ds.d);
      d.d -= L * ds.d;
#if defined(DTOA_CHECK_FLOAT_ROUNDS)
      /* If FLT_ROUNDS == 2, L will usually be high by 1 */
      if (d.d < 0)
      {
        L--;
        d.d += ds.d;
      }
#endif
      *s++ = '0' + (int)L;
      if (!d.d)
      {
#if defined(DTOA_SET_INEXACT)
        inexact = 0;
#endif
        break;
      }
      if (i == ilim)
      {
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
        if (mode > 1)
        {
          switch (rounding)
          {
            case 0: goto _Ret1;
            case 2: goto _BumpUp;
          }
        }
#endif
        d.d += d.d;
        if (d.d > ds.d || d.d == ds.d && L & 1)
        {
_BumpUp:
          while (*--s == '9')
          {
            if (s == s0)
            {
              k++;
              *s = '0';
              break;
            }
          }
          ++*s++;
        }
        break;
      }
    }
    goto _Ret1;
  }

  m2 = b2;
  m5 = b5;
  mhi = mlo = 0;
  if (leftright)
  {
    i =
#if !defined(DTOA_SUDDEN_OVERFLOW)
      denorm ? be + (Bias + (P-1) - 1 + 1) :
#endif
#if defined(DTOA_IBM)
      1 + 4*P - 3 - bbits + ((bbits + be - 1) & 3);
#else
      1 + P - bbits;
#endif
    b2 += i;
    s2 += i;
    mhi = BContext_i2b(&context, 1);
  }
  if (m2 > 0 && s2 > 0)
  {
    i = m2 < s2 ? m2 : s2;
    b2 -= i;
    m2 -= i;
    s2 -= i;
  }
  if (b5 > 0)
  {
    if (leftright)
    {
      if (m5 > 0)
      {
        mhi = BContext_pow5mult(&context, mhi, m5);
        b1 = BContext_mult(&context, mhi, b);
        BContext_bfree(&context, b);
        b = b1;
      }
      if ((j = b5 - m5))
        b = BContext_pow5mult(&context, b, j);
    }
    else
      b = BContext_pow5mult(&context, b, b5);
  }
  S = BContext_i2b(&context, 1);
  if (s5 > 0) S = BContext_pow5mult(&context, S, s5);

  // Check for special case that d is a normalized power of 2.

  spec_case = 0;
  if ((mode < 2 || leftright)
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
      && rounding == 1
#endif
    )
  {
    if (!d.u32Lo && !(d.u32Hi & Bndry_mask)
#if !defined(DTOA_SUDDEN_OVERFLOW)
     && d.u32Hi & (Exp_mask & ~Exp_msk1)
#endif
      )
    {
      // The special case.
      b2 += Log2P;
      s2 += Log2P;
      spec_case = 1;
    }
  }

  // Arrange for convenient computation of quotients:
  // shift left if necessary so divisor has 4 leading 0 bits.
  //
  // Perhaps we should just compute leading 28 bits of S once
  // and for all and pass them and a shift to quorem, so it
  // can do shifts and ors to compute the numerator for q.
  if ((i = ((s5 ? 32 - hi0bits(S->x[S->wds-1]) : 1) + s2) & 0x1f))
    i = 32 - i;
  if (i > 4)
  {
    i -= 4;
    b2 += i;
    m2 += i;
    s2 += i;
  }
  else if (i < 4)
  {
    i += 28;
    b2 += i;
    m2 += i;
    s2 += i;
  }
  if (b2 > 0)
    b = BContext_lshift(&context, b, b2);
  if (s2 > 0)
    S = BContext_lshift(&context, S, s2);
  if (k_check)
  {
    if (cmp(b,S) < 0)
    {
      k--;
      b = BContext_madd(&context, b, 10, 0); /* we botched the k estimate */
      if (leftright)
        mhi = BContext_madd(&context, mhi, 10, 0);
      ilim = ilim1;
    }
  }

  if (ilim <= 0 && (mode == 3 || mode == 5))
  {
    if (ilim < 0 || cmp(b,S = BContext_madd(&context, S, 5, 0)) <= 0)
    {
      // No digits, fcvt style.
_NoDigits:
      k = -1 - ndigits;
      goto _Ret;
    }
_OneDigit:
    *s++ = '1';
    k++;
    goto _Ret;
  }

  if (leftright)
  {
    if (m2 > 0) mhi = BContext_lshift(&context, mhi, m2);

    // Compute mlo -- check for special case
    // that d is a normalized power of 2.

    mlo = mhi;
    if (spec_case)
    {
      mhi = BContext_balloc(&context, mhi->k);
      Bcopy(mhi, mlo);
      mhi = BContext_lshift(&context, mhi, Log2P);
    }

    for (i = 1;; i++)
    {
      dig = quorem(b,S) + '0';
      // Do we yet have the shortest decimal string
      // that will round to d?
      j = cmp(b, mlo);
      delta = BContext_diff(&context, S, mhi);
      j1 = delta->sign ? 1 : cmp(b, delta);
      BContext_bfree(&context, delta);
#ifndef DTOA_ROUND_BIASED
      if (j1 == 0 && mode != 1 && !(d.u32Lo & 1)
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
        && rounding >= 1
#endif
        )
      {
        if (dig == '9')
          goto _Round9Up;

        if (j > 0)
          dig++;
#if defined(DTOA_SET_INEXACT)
        else if (!b->x[0] && b->wds <= 1)
          inexact = 0;
#endif
        *s++ = dig;
        goto _Ret;
      }
#endif
      if (j < 0 || j == 0 && mode != 1
#ifndef DTOA_ROUND_BIASED
        && !(d.u32Lo & 1)
#endif
        )
      {
        if (!b->x[0] && b->wds <= 1)
        {
#if defined(DTOA_SET_INEXACT)
          inexact = 0;
#endif
          goto _AcceptDigit;
        }
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
        if (mode > 1)
        {
          switch (rounding)
          {
            case 0: goto _AcceptDigit;
            case 2: goto _KeepDigit;
          }
        }
#endif // DTOA_HONOR_FLOAT_ROUNDS
        if (j1 > 0)
        {
          b = BContext_lshift(&context, b, 1);
          j1 = cmp(b, S);
          if ((j1 > 0 || j1 == 0 && dig & 1) && dig++ == '9')
            goto _Round9Up;
        }
_AcceptDigit:
        *s++ = dig;
        goto _Ret;
      }
      if (j1 > 0)
      {
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
        if (!rounding) goto _AcceptDigit;
#endif
        // Possible if i == 1.
        if (dig == '9')
        {
_Round9Up:
          *s++ = '9';
          goto _RoundOff;
        }
        *s++ = dig + 1;
        goto _Ret;
      }

#if defined(DTOA_HONOR_FLOAT_ROUNDS)
_KeepDigit:
#endif
      *s++ = dig;
      if (i == ilim) break;
      b = BContext_madd(&context, b, 10, 0);
      if (mlo == mhi)
        mlo = mhi = BContext_madd(&context, mhi, 10, 0);
      else
      {
        mlo = BContext_madd(&context, mlo, 10, 0);
        mhi = BContext_madd(&context, mhi, 10, 0);
      }
    }
  }
  else
  {
    for (i = 1;; i++)
    {
      *s++ = dig = quorem(b,S) + '0';
      if (!b->x[0] && b->wds <= 1)
      {
#if defined(DTOA_SET_INEXACT)
        inexact = 0;
#endif
        goto _Ret;
      }
      if (i >= ilim) break;
      b = BContext_madd(&context, b, 10, 0);
    }
  }

  // Round off last digit.

#if defined(DTOA_HONOR_FLOAT_ROUNDS)
  switch (rounding)
  {
    case 0: goto _TrimZeros;
    case 2: goto _RoundOff;
  }
#endif
  b = BContext_lshift(&context, b, 1);
  j = cmp(b, S);
  if (j > 0 || j == 0 && dig & 1)
  {
_RoundOff:
    while (*--s == '9')
      if (s == s0)
      {
        k++;
        *s++ = '1';
        goto _Ret;
      }
    ++*s++;
  }
  else
  {
_TrimZeros:
    while (*--s == '0') ;
    s++;
  }

_Ret:
  BContext_bfree(&context, S);
  if (mhi)
  {
    if (mlo && mlo != mhi) BContext_bfree(&context, mlo);
    BContext_bfree(&context, mhi);
  }
 _Ret1:
#if defined(DTOA_SET_INEXACT)
  if (inexact)
  {
    if (!oldinexact)
    {
      d.u32Hi = Exp_1 + (70 << Exp_shift);
      d.u32Lo = 0;
      d.d += 1.0;
    }
  }
  else if (!oldinexact)
  {
    clear_inexact();
  }
#endif
  BContext_bfree(&context, b);

  ctx->length = (uint32_t)(size_t)(s - (char*)ctx->buffer);
  ctx->decpt = k + 1;
  ctx->negative = negative;

  BContext_destroy(&context);
  FOG_CONTROL87_END();
}

// ============================================================================
// [Fog::StringUtil - ParseFloat / ParseDouble]
// ============================================================================

template<typename CharT>
static err_t FOG_CDECL StringUtil_parseFloat(float* dst, const CharT* str, size_t length, CharT_Type decimalPoint, size_t* pEnd, uint32_t* pFlags)
{
  double d;
  err_t err = StringUtil::parseReal(&d, str, length, CharT(decimalPoint), pEnd, pFlags);

  if (d < double(FLOAT_MIN))
  {
    *dst = FLOAT_MIN;
    return ERR_RT_OVERFLOW;
  }

  if (d > double(FLOAT_MAX))
  {
    *dst = FLOAT_MAX;
    return ERR_RT_OVERFLOW;
  }

  *dst = (float)d;
  return err;
}

template<typename CharT>
static BInt* BContext_s2b(BContext* context, const CharT* s, int nd0, int nd, uint32_t y9)
{
  BInt* b;
  int i, k;
  int32_t x, y;

  x = (nd + 8) / 9;
  for (k = 0, y = 1; x > y; y <<= 1, k++)
    continue;

  b = BContext_balloc(context, k);
  b->x[0] = y9;
  b->wds = 1;

  i = 9;
  if (9 < nd0)
  {
    s += 9;
    do {
      b = BContext_madd(context, b, 10, (uint)*s++ - '0');
    } while(++i < nd0);
    s++;
  }
  else
  {
    s += 10;
  }

  for (; i < nd; i++)
    b = BContext_madd(context, b, 10, (uint)*s++ - '0');

  return b;
}

template<typename CharT>
static FOG_CDECL err_t StringUtil_parseDouble(double* dst, const CharT* str, size_t length, CharT_Type decimalPoint, size_t* pEnd, uint32_t* pFlags)
{
  BContext context;

  int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, dsign,
    e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;

  const CharT* sBegin;
  const CharT* s;
  const CharT* s0;
  const CharT* s1;
  const CharT* sEnd;

  DoubleBits rv;
  DoubleBits rv0;

  double adj, aadj, aadj1;
  int32_t L;
  uint32_t y, z;
  BInt* bb, *bb1, *bd, *bd0, *bs, *delta;

#if defined(DTOA_AVOID_UNDERFLOW)
  int scale;
#endif

#if defined(DTOA_SET_INEXACT)
  int inexact, oldinexact;
#endif

#if defined(DTOA_HONOR_FLOAT_ROUNDS)
  int rounding;
#endif

  err_t err = ERR_OK;
  uint32_t flags = 0;

  BContext_init(&context);

  s = sBegin = str;
  sEnd = s + length;

  sign = nz0 = nz = 0;
  rv.d = 0.0;

  if (s == sEnd)
    goto _Ret0;

  // skip all spaces
  if (CharT_Func::isSpace(*s))
  {
    flags |= STRING_PARSED_SPACES;

    do {
      if (++s == sEnd)
        goto _Ret0;
    } while (CharT_Func::isSpace(*s));
  }

  // Parse sign.
  if (*s == CharT('+'))
  {
    flags |= STRING_PARSED_SIGN;
    if (++s == sEnd)
      goto _Ret0;
  }
  else if (*s == CharT('-'))
  {
    sign = true;
    flags |= STRING_PARSED_SIGN;
    if (++s == sEnd)
      goto _Ret0;
  }

  if (*s == CharT('0'))
  {
    nz0 = 1;
    for (;;)
    {
      if (++s == sEnd) goto _Ret;
      if (*s != CharT('0')) break;
    }
  }

  s0 = s;
  y = z = 0;
  nd = nf = 0;
  nd0 = 0;

  for(; (c = *s) >= '0' && c <= '9'; nd++)
  {
    if (nd < 9)
      y = 10*y + (int)c - '0';
    else if (nd < 16)
      z = 10*z + (int)c - '0';

    if (++s == sEnd)
    {
      c = 0; nd++;
      goto _DigitDone;
    }
  }
  nd0 = nd;

  if (CharT(c) == decimalPoint)
  {
    flags |= STRING_PARSED_DECIMAL_POINT;

    if (++s == sEnd)
    {
      c = 0;
      goto _DigitDone;
    }

    c = *s;
    if (!nd)
    {
      for (;;)
      {
        if (c == '0')
        {
          nz++;
          if (++s == sEnd) goto _DigitDone;
          c = *s;
          continue;
        }

        if (c > '0' && c <= '9')
        {
          s0 = s;
          nf += nz;
          nz = 0;
          goto _HaveDigit;
        }

        goto _DigitDone;
      }
    }

    for(; c >= '0' && c <= '9'; )
    {
 _HaveDigit:
      nz++;
      c -= '0';

      if (c)
      {
        nf += nz;
        for(i = 1; i < nz; i++)
        {
          if (nd++ < 9)
            y *= 10;
          else if (nd <= DBL_DIG + 1)
            z *= 10;
        }

        if (nd++ < 9)
          y = 10*y + c;
        else if (nd <= DBL_DIG + 1)
          z = 10*z + c;

        nz = 0;
      }

      if (++s == sEnd)
      {
        c = 0;
        break;
      }

      c = *s;
    }
  }

_DigitDone:
  e = 0;
  if (c == 'e' || c == 'E')
  {
    flags |= STRING_PARSED_EXPONENT;

    if (!nd && !nz && !nz0)
    {
      goto _Ret0;
    }
    str = s;
    esign = 0;

    if (++s == sEnd)
      goto _ExponentDone;
    c = *s;

    switch (c)
    {
      case '-':
        esign = 1;
      case '+':
        if (++s == sEnd)
          goto _ExponentDone;
        c = *s;
    }
    if (c >= '0' && c <= '9')
    {
      while (c == '0')
      {
        if (++s == sEnd) goto _ExponentDone;
        c = *s;
      }

      if (c > '0' && c <= '9')
      {
        L = c - '0';
        s1 = s;


        for (;;)
        {
          if (++s == sEnd) break;
          c = *s;
          if (c >= '0' && c <= '9')
          {
            L = 10*L + c - '0';
            continue;
          }
          else
            break;
        }

        if (s - s1 > 8 || L > 19999)
          // Avoid confusion from exponents so large that e might overflow.
          e = 19999; // safe for 16 bit ints.
        else
          e = (int)L;
        if (esign) e = -e;
      }
      else
        e = 0;
    }
    else
      s = str;
  }
_ExponentDone:
  if (!nd)
  {
    if (!nz && !nz0)
    {
#if defined(DTOA_INFNAN_CHECK)
      // Check for Nan and Infinity.
      if ((c == 'i' || c == 'I') && (size_t)(sEnd - s) >= 2 && StringUtil::eq(s+1, "nf", 2, CASE_INSENSITIVE))
      {
        s += 3;
        if ((size_t)(sEnd - s) >= 5 && StringUtil::eq(s, "inity", 5, CASE_INSENSITIVE)) s += 5;

        rv.u32Hi = 0x7FF00000;
        rv.u32Lo = 0;
        goto _Ret;
      }
      else if ((c == 'n' || c == 'N') && (size_t)(sEnd - s) >= 2 && StringUtil::eq(s+1, "an", 2, CASE_INSENSITIVE))
      {
        s += 3;
        rv.d = Math::getQNanD();
        goto _Ret;
      }

#endif // DTOA_INFNAN_CHECK
_Ret0:
      s = str;
      sign = 0;
    }
    goto _Ret;
  }
  e1 = e -= nf;

  // Now we have nd0 digits, starting at s0, followed by a
  // decimal point, followed by nd-nd0 digits.  The number we're
  // after is the integer represented by those digits times
  // 10**e

  if (!nd0) nd0 = nd;
  k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
  rv.d = y;
  if (k > 9)
  {
#if defined(DTOA_SET_INEXACT)
    if (k > DBL_DIG) oldinexact = get_inexact();
#endif
    rv.d = tens[k - 9] * rv.d + z;
  }
  bd0 = 0;
  if (nd <= DBL_DIG
#ifndef DTOA_RND_PRODQUOT
#ifndef DTOA_HONOR_FLOAT_ROUNDS
    && Flt_Rounds == 1
#endif
#endif
    )
  {
    if (!e) goto _Ret;

    if (e > 0)
    {
      if (e <= Ten_pmax)
      {
#if defined(DTOA_VAX)
        goto _VaxOverflowCheck;
#else
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
        // round correctly FLT_ROUNDS = 2 or 3.
        if (sign)
        {
          rv.d = -rv.d;
          sign = 0;
        }
#endif
        /* rv = */ rounded_product(rv.d, tens[e]);
        goto _Ret;
#endif
      }
      i = DBL_DIG - nd;
      if (e <= Ten_pmax + i)
      {
        // A fancier test would sometimes let us do this for larger i values.
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
        // round correctly FLT_ROUNDS = 2 or 3.
        if (sign)
        {
          rv.d = -rv.d;
          sign = 0;
        }
#endif
        e -= i;
        rv.d *= tens[i];
#if defined(DTOA_VAX)
        // VAX exponent range is so narrow we must worry about overflow here...
_VaxOverflowCheck:
        rv.u32Hi -= P*Exp_msk1;
        rounded_product(rv.d, tens[e]);
        if ((rv.u32Hi & Exp_mask) > Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
          goto _Overflow;
        rv.u32Hi += P*Exp_msk1;
#else
        rounded_product(rv.d, tens[e]);
#endif
        goto _Ret;
      }
    }
#ifndef DTOA_INACCURATE_DIVIDE
    else if (e >= -Ten_pmax)
    {
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
      // round correctly FLT_ROUNDS = 2 or 3.
      if (sign)
      {
        rv.d = -rv.d;
        sign = 0;
      }
#endif
      rounded_quotient(rv.d, tens[-e]);
      goto _Ret;
    }
#endif
  }
  e1 += nd - k;

#if defined(DTOA_IEEE)
#if defined(DTOA_SET_INEXACT)
  inexact = 1;
  if (k <= DBL_DIG) oldinexact = get_inexact();
#endif
#if defined(DTOA_AVOID_UNDERFLOW)
  scale = 0;
#endif
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
  if ((rounding = Flt_Rounds) >= 2)
  {
    if (sign)
      rounding = rounding == 2 ? 0 : 2;
    else if (rounding != 2)
      rounding = 0;
  }
#endif
#endif /*DTOA_IEEE*/

  // Get starting approximation = rv * 10**e1.
  if (e1 > 0)
  {
    if ((i = e1 & 15))
      rv.d *= tens[i];
    if (e1 &= ~15)
    {
      if (e1 > DBL_MAX_10_EXP)
      {
_Overflow:
        err = ERR_RT_OVERFLOW;

        // Can't trust HUGE_VAL.
#if defined(DTOA_IEEE)
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
        switch (rounding)
        {
          case 0: // Toward 0
          case 3: // Toward -INF.
            rv.u32Hi = Big0;
            rv.u32Lo = Big1;
            break;
          default:
            rv.u32Hi = Exp_mask;
            rv.u32Lo = 0;
        }
#else // DTOA_HONOR_FLOAT_ROUNDS
        rv.u32Hi = Exp_mask;
        rv.u32Lo = 0;
#endif // DTOA_HONOR_FLOAT_ROUNDS
#if defined(DTOA_SET_INEXACT)
        // Set overflow bit.
        rv0.d = 1e300;
        rv0.d *= rv0.d;
#endif
#else // DTOA_IEEE
        rv.u32Hi = Big0;
        rv.u32Lo = Big1;
#endif // DTOA_IEEE
        if (bd0)
          goto _RetFree;
        else
          goto _Ret;
      }
      e1 >>= 4;
      for(j = 0; e1 > 1; j++, e1 >>= 1)
        if (e1 & 1)
          rv.d *= bigtens[j];

      // The last multiplication could overflow.
      rv.u32Hi -= P*Exp_msk1;
      rv.d *= bigtens[j];

      if ((z = rv.u32Hi & Exp_mask) > Exp_msk1*(DBL_MAX_EXP+Bias-P))
        goto _Overflow;

      if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
      {
        // Set to largest number (Can't trust DBL_MAX).
        rv.u32Hi = Big0;
        rv.u32Lo = Big1;
      }
      else
        rv.u32Hi += P*Exp_msk1;
    }
  }
  else if (e1 < 0)
  {
    e1 = -e1;
    if ((i = e1 & 15)) rv.d /= tens[i];
    if (e1 >>= 4)
    {
      if (e1 >= 1 << FOG_ARRAY_SIZE(bigtens)) goto _Underflow;
#if defined(DTOA_AVOID_UNDERFLOW)
      if (e1 & Scale_Bit)
        scale = 2*P;
      for(j = 0; e1 > 0; j++, e1 >>= 1)
        if (e1 & 1)
          rv.d *= tinytens[j];
      if (scale && (j = 2*P + 1 - ((rv.u32Hi & Exp_mask) >> Exp_shift)) > 0)
      {
        // scaled rv is denormal; zap j low bits.
        if (j >= 32)
        {
          rv.u32Lo = 0;
          if (j >= 53)
            rv.u32Hi = (P+2)*Exp_msk1;
          else
            rv.u32Hi &= 0xffffffff << (j-32);
        }
        else
          rv.u32Lo &= 0xffffffff << j;
      }
#else
      for(j = 0; e1 > 1; j++, e1 >>= 1)
        if (e1 & 1)
          rv.d *= tinytens[j];

      // The last multiplication could underflow.
      rv0.d = rv.d;
      rv.d *= tinytens[j];
      if (!rv.d)
      {
        rv.d = 2. * rv0.d;
        rv.d *= tinytens[j];
#endif
        if (!rv.d)
        {
_Underflow:
          rv.d = 0.0;
          err = ERR_RT_OVERFLOW;

          if (bd0)
            goto _RetFree;
          else
            goto _Ret;
        }
#if !defined(DTOA_AVOID_UNDERFLOW)
        rv.u32Hi = Tiny0;
        rv.u32Lo = Tiny1;
        // The refinement below will clean this approximation up.
      }
#endif
    }
  }

  // Now the hard part -- adjusting rv to the correct value.

  // Put digits into bd: true value = bd * 10^e.
  bd0 = BContext_s2b(&context, s0, nd0, nd, y);

  for(;;)
  {
    bd = BContext_balloc(&context, bd0->k);
    Bcopy(bd, bd0);
    bb = BContext_d2b(&context, rv.d, &bbe, &bbbits); // rv = bb * 2^bbe
    bs = BContext_i2b(&context, 1);

    if (e >= 0)
    {
      bb2 = bb5 = 0;
      bd2 = bd5 = e;
    }
    else
    {
      bb2 = bb5 = -e;
      bd2 = bd5 = 0;
    }
    if (bbe >= 0)
      bb2 += bbe;
    else
      bd2 -= bbe;
    bs2 = bb2;
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
    if (rounding != 1) bs2++;
#endif
#if defined(DTOA_AVOID_UNDERFLOW)
    j = bbe - scale;
    i = j + bbbits - 1;  // logb(rv)
    if (i < Emin)        // denormal
      j += P - Emin;
    else
      j = P + 1 - bbbits;
#else /*DTOA_AVOID_UNDERFLOW*/
#if defined(DTOA_SUDDEN_OVERFLOW)
#if defined(DTOA_IBM)
    j = 1 + 4*P - 3 - bbbits + ((bbe + bbbits - 1) & 3);
#else
    j = P + 1 - bbbits;
#endif
#else /*DTOA_SUDDEN_OVERFLOW*/
    j = bbe;
    i = j + bbbits - 1;  // logb(rv)
    if (i < Emin)        // denormal
      j += P - Emin;
    else
      j = P + 1 - bbbits;
#endif /*DTOA_SUDDEN_OVERFLOW*/
#endif /*DTOA_AVOID_UNDERFLOW*/
    bb2 += j;
    bd2 += j;
#if defined(DTOA_AVOID_UNDERFLOW)
    bd2 += scale;
#endif
    i = bb2 < bd2 ? bb2 : bd2;
    if (i > bs2) i = bs2;
    if (i > 0)
    {
      bb2 -= i;
      bd2 -= i;
      bs2 -= i;
    }
    if (bb5 > 0)
    {
      bs = BContext_pow5mult(&context, bs, bb5);
      bb1 = BContext_mult(&context, bs, bb);
      BContext_bfree(&context, bb);
      bb = bb1;
    }
    if (bb2 > 0)
      bb = BContext_lshift(&context, bb, bb2);
    if (bd5 > 0)
      bd = BContext_pow5mult(&context, bd, bd5);
    if (bd2 > 0)
      bd = BContext_lshift(&context, bd, bd2);
    if (bs2 > 0)
      bs = BContext_lshift(&context, bs, bs2);
    delta = BContext_diff(&context, bb, bd);
    dsign = delta->sign;
    delta->sign = 0;
    i = cmp(delta, bs);
#if defined(DTOA_HONOR_FLOAT_ROUNDS)
    if (rounding != 1)
    {
      if (i < 0)
      {
        // Error is less than an ulp.
        if (!delta->x[0] && delta->wds <= 1)
        {
          // exact.
#if defined(DTOA_SET_INEXACT)
          inexact = 0;
#endif
          break;
        }
        if (rounding)
        {
          if (dsign)
          {
            adj = 1.;
            goto _ApplyAdj;
          }
        }
        else if (!dsign)
        {
          adj = -1.;
          if (!rv.u32Lo && !(rv.u32Hi & Frac_mask))
          {
            y = rv.u32Hi & Exp_mask;
#if defined(DTOA_AVOID_UNDERFLOW)
            if (!scale || y > 2*P*Exp_msk1)
#else
            if (y)
#endif
            {
              delta = BContext_lshift(&context, delta,Log2P);
              if (cmp(delta, bs) <= 0) adj = -0.5;
            }
          }
 _ApplyAdj:
#if defined(DTOA_AVOID_UNDERFLOW)
          if (scale && (y = rv.u32Hi & Exp_mask) <= 2*P*Exp_msk1)
          {
            DoubleBits tmp;
            tmp.d = adj;
            tmp.u32Hi += (2 * P + 1) * Exp_msk1 - y;
            adj = tmp.d;
          }
#else
#if defined(DTOA_SUDDEN_OVERFLOW)
          if ((rv.u32Hi & Exp_mask) <= P*Exp_msk1)
          {
            rv.u32Hi += P*Exp_msk1;
            rv.d += adj * ulp(rv.d);
            rv.u32Hi -= P*Exp_msk1;
          }
          else
#endif /*DTOA_SUDDEN_OVERFLOW*/
#endif /*DTOA_AVOID_UNDERFLOW*/
            rv.d += adj * ulp(rv.d);
        }
        break;
      }
      adj = ratio(delta, bs);
      if (adj < 1.)
        adj = 1.;
      if (adj <= 0x7ffffffe)
      {
        // adj = rounding ? ceil(adj) : floor(adj);
        y = (uint32_t)adj;
        if (y != adj)
        {
          if (!((rounding>>1) ^ dsign))
            y++;
          adj = y;
        }
      }
#if defined(DTOA_AVOID_UNDERFLOW)
      if (scale && (y = rv.u32Hi & Exp_mask) <= 2*P*Exp_msk1)
      {
        DoubleBits tmp;
        tmp.d = adj;
        tmp.u32Hi += (2*P+1) * Exp_msk1 - y;
        adj = tmp.d;
      }
#else
#if defined(DTOA_SUDDEN_OVERFLOW)
      if ((rv.u32Hi & Exp_mask) <= P*Exp_msk1)
      {
        rv.u32Hi += P*Exp_msk1;
        adj *= ulp(rv.d);
        if (dsign)
          rv.d += adj;
        else
          rv.d -= adj;
        rv.u32Hi -= P*Exp_msk1;
        goto _Continue;
      }
#endif // DTOA_SUDDEN_OVERFLOW
#endif // DTOA_AVOID_UNDERFLOW
      adj *= ulp(rv.d);
      if (dsign)
        rv.d += adj;
      else
        rv.d -= adj;
      goto _Continue;
    }
#endif /*DTOA_HONOR_FLOAT_ROUNDS*/

    if (i < 0)
    {
      // Error is less than half an ulp -- check for special case of mantissa a power of two.
      if (dsign || rv.u32Lo || rv.u32Hi & Bndry_mask
#if defined(DTOA_IEEE)
#if defined(DTOA_AVOID_UNDERFLOW)
       || (rv.u32Hi & Exp_mask) <= (2*P+1)*Exp_msk1
#else
       || (rv.u32Hi & Exp_mask) <= Exp_msk1
#endif
#endif
        )
      {
#if defined(DTOA_SET_INEXACT)
        if (!delta->x[0] && delta->wds <= 1)
          inexact = 0;
#endif
        break;
      }
      if (!delta->x[0] && delta->wds <= 1)
      {
        // exact result.
#if defined(DTOA_SET_INEXACT)
        inexact = 0;
#endif
        break;
      }

      delta = BContext_lshift(&context, delta, Log2P);
      if (cmp(delta, bs) > 0)
        goto _DropDown;
      break;
    }
    if (i == 0)
    {
      // Exactly half-way between.
      if (dsign)
      {
        if ((rv.u32Hi & Bndry_mask1) == Bndry_mask1 &&  rv.u32Lo == (
#if defined(DTOA_AVOID_UNDERFLOW)
          (scale && (y = rv.u32Hi & Exp_mask) <= 2*P*Exp_msk1)
          ? (0xffffffff & (0xffffffff << (2*P+1-(y>>Exp_shift)))) :
#endif
          0xffffffff))
        {
          // Boundary case -- increment exponent.
          rv.u32Hi = (rv.u32Hi & Exp_mask) + Exp_msk1
#if defined(DTOA_IBM)
            | Exp_msk1 >> 4
#endif
            ;
          rv.u32Lo = 0;
#if defined(DTOA_AVOID_UNDERFLOW)
          dsign = 0;
#endif
          break;
        }
      }
      else if (!(rv.u32Hi & Bndry_mask) && !rv.u32Lo)
      {
_DropDown:
        // Boundary case -- decrement exponent.
#if defined(DTOA_SUDDEN_OVERFLOW)
        L = rv.u32Hi & Exp_mask;
#if defined(DTOA_IBM)
        if (L <  Exp_msk1)
#else
#if defined(DTOA_AVOID_UNDERFLOW)
        if (L <= (scale ? (2*P+1)*Exp_msk1 : Exp_msk1))
#else
        if (L <= Exp_msk1)
#endif // DTOA_AVOID_UNDERFLOW
#endif // DTOA_IBM
          goto _Underflow;
        L -= Exp_msk1;
#else /*DTOA_SUDDEN_OVERFLOW}{*/
#if defined(DTOA_AVOID_UNDERFLOW)
        if (scale)
        {
          L = rv.u32Hi & Exp_mask;
          if (L <= (2*P+1)*Exp_msk1)
          {
            if (L > (P+2)*Exp_msk1)
              // round even ==> accept rv.
              break;
            // rv = smallest denormal.
            goto _Underflow;
          }
        }
#endif // DTOA_AVOID_UNDERFLOW
        L = (rv.u32Hi & Exp_mask) - Exp_msk1;
#endif // DTOA_SUDDEN_OVERFLOW
        rv.u32Hi = L | Bndry_mask1;
        rv.u32Lo = 0xffffffff;
#if defined(DTOA_IBM)
        goto _Continue;
#else
        break;
#endif
      }
#ifndef DTOA_ROUND_BIASED
      if (!(rv.u32Lo & LSB))
        break;
#endif
      if (dsign)
        rv.d += ulp(rv.d);
#ifndef DTOA_ROUND_BIASED
      else
      {
        rv.d -= ulp(rv.d);
#if !defined(DTOA_SUDDEN_OVERFLOW)
        if (!rv.d)
          goto _Underflow;
#endif
      }
#if defined(DTOA_AVOID_UNDERFLOW)
      dsign = 1 - dsign;
#endif
#endif
      break;
    }
    if ((aadj = ratio(delta, bs)) <= 2.)
    {
      if (dsign) aadj = aadj1 = 1.;
      else if (rv.u32Lo || rv.u32Hi & Bndry_mask)
      {
#if !defined(DTOA_SUDDEN_OVERFLOW)
        if (rv.u32Lo == Tiny1 && !rv.u32Hi)
          goto _Underflow;
#endif
        aadj = 1.;
        aadj1 = -1.;
      }
      else
      {
        // special case -- power of FLT_RADIX to be rounded down...
        if (aadj < 2./FLT_RADIX)
          aadj = 1./FLT_RADIX;
        else
          aadj *= 0.5;
        aadj1 = -aadj;
      }
    }
    else
    {
      aadj *= 0.5;
      aadj1 = dsign ? aadj : -aadj;
#if defined(DTOA_CHECK_FLOAT_ROUNDS)
      switch (Rounding)
      {
        case 2: // towards +infinity
          aadj1 -= 0.5;
          break;
        case 0: // towards 0
        case 3: // towards -infinity
          aadj1 += 0.5;
      }
#else
      if (Flt_Rounds == 0) aadj1 += 0.5;
#endif /*DTOA_CHECK_FLOAT_ROUNDS*/
    }
    y = rv.u32Hi & Exp_mask;

    // Check for overflow.
    if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1))
    {
      rv0.d = rv.d;
      rv.u32Hi -= P*Exp_msk1;
      adj = aadj1 * ulp(rv.d);
      rv.d += adj;
      if ((rv.u32Hi & Exp_mask) >= Exp_msk1*(DBL_MAX_EXP+Bias-P))
      {
        if (rv0.u32Hi == Big0 && rv0.u32Lo == Big1)
          goto _Overflow;

        rv.u32Hi = Big0;
        rv.u32Lo = Big1;
        goto _Continue;
      }
      else
        rv.u32Hi += P*Exp_msk1;
    }
    else
    {
#if defined(DTOA_AVOID_UNDERFLOW)
      if (scale && y <= 2*P*Exp_msk1)
      {
        DoubleBits tmp;
        tmp.d = aadj1;

        if (aadj <= 0x7fffffff)
        {
          if ((z = (int32_t)aadj) <= 0) z = 1;
          aadj = z;
          tmp.d = dsign ? aadj : -aadj;
        }
        tmp.u32Hi += (2*P+1)*Exp_msk1 - y;
        aadj1 = tmp.d;
      }
      adj = aadj1 * ulp(rv.d);
      rv.d += adj;
#else
#if defined(DTOA_SUDDEN_OVERFLOW)
      if ((rv.u32Hi & Exp_mask) <= P*Exp_msk1)
      {
        rv0.d = rv.d;
        rv.u32Hi += P*Exp_msk1;
        adj = aadj1 * ulp(rv.d);
        rv.d += adj;
#if defined(DTOA_IBM)
        if ((rv.u32Hi & Exp_mask) <  P*Exp_msk1)
#else
        if ((rv.u32Hi & Exp_mask) <= P*Exp_msk1)
#endif
        {
          if (rv0.u32Hi == Tiny0 && rv0.u32Lo == Tiny1)
            goto _Underflow;

          rv.u32Hi = Tiny0;
          rv.u32Lo = Tiny1;
          goto _Continue;
        }
        else
          rv.u32Hi -= P*Exp_msk1;
      }
      else
      {
        adj = aadj1 * ulp(rv.d);
        rv.d += adj;
      }
#else // DTOA_SUDDEN_OVERFLOW
      // Compute adj so that the IEEE rounding rules will
      // correctly round rv + adj in some half-way cases.
      // If rv * ulp(rv) is denormalized (i.e.,
      // y <= (P-1)*Exp_msk1), we must adjust aadj to avoid
      // trouble from bits lost to denormalization;
      // example: 1.2e-307 .
      //
      if (y <= (P-1)*Exp_msk1 && aadj > 1.)
      {
        aadj1 = (double)(int)(aadj + 0.5);
        if (!dsign) aadj1 = -aadj1;
      }
      adj = aadj1 * ulp(rv.d);
      rv.d += adj;
#endif // DTOA_SUDDEN_OVERFLOW
#endif // DTOA_AVOID_UNDERFLOW
    }
    z = rv.u32Hi & Exp_mask;
#ifndef DTOA_SET_INEXACT
#if defined(DTOA_AVOID_UNDERFLOW)
    if (!scale)
#endif
    if (y == z)
    {
      // Can we stop now?
      L = (int32_t)aadj;
      aadj -= L;
      // The tolerances below are conservative.
      if (dsign || rv.u32Lo || rv.u32Hi & Bndry_mask)
      {
        if (aadj < .4999999 || aadj > .5000001) break;
      }
      else if (aadj < .4999999/FLT_RADIX)
      {
        break;
      }
    }
#endif

_Continue:
    BContext_bfree(&context, bb);
    BContext_bfree(&context, bd);
    BContext_bfree(&context, bs);
    BContext_bfree(&context, delta);
  }

#if defined(DTOA_SET_INEXACT)
  if (inexact)
  {
    if (!oldinexact)
    {
      rv0.u32Hi = Exp_1 + (70 << Exp_shift);
      rv0.u32Lo = 0;
      rv0.d += 1.;
    }
  }
  else if (!oldinexact)
  {
    clear_inexact();
  }
#endif

#if defined(DTOA_AVOID_UNDERFLOW)
  if (scale)
  {
    rv0.u32Hi = Exp_1 - 2*P*Exp_msk1;
    rv0.u32Lo = 0;
    rv.d *= rv0.d;

    // Try to avoid the bug of testing an 8087 register value.
    if (rv.u32Hi == 0 && rv.u32Lo == 0)
      err = ERR_RT_OVERFLOW;
  }
#endif // DTOA_AVOID_UNDERFLOW

#if defined(DTOA_SET_INEXACT)
  if (inexact && !(rv.u32Hi & Exp_mask))
  {
    // set underflow bit.
    rv0.d = 1e-300;
    rv0.d *= rv0.d;
  }
#endif

_RetFree:
  BContext_bfree(&context, bb);
  BContext_bfree(&context, bd);
  BContext_bfree(&context, bs);
  BContext_bfree(&context, bd0);
  BContext_bfree(&context, delta);

_Ret:
  BContext_destroy(&context);

  if (pEnd) *pEnd = (size_t)(s - sBegin);
  if (pFlags) *pFlags = flags;

  if (sign)
    *dst = -rv.d;
  else
    *dst = rv.d;
  return err;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void StringUtil_init_dtoa(void)
{
  _api.stringutil.dtoa = StringUtil_dtoa;

  _api.stringutil.parseFloatA = StringUtil_parseFloat<char>;
  _api.stringutil.parseFloatW = StringUtil_parseFloat<CharW>;

  _api.stringutil.parseDoubleA = StringUtil_parseDouble<char>;
  _api.stringutil.parseDoubleW = StringUtil_parseDouble<CharW>;
}

} // Fog namespace
