// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Generator]
// - CHAR_TYPE - Char type (char, Char)
// - CHAR_SIZE - Char size (1, 2, )
#if defined(__G_GENERATE)

namespace Fog {
namespace StringUtil {

// ============================================================================
// [Fog::StringUtil::atoi, atou]
// ============================================================================

err_t atob(const CHAR_TYPE* str, size_t length, bool* dst, size_t* parserEnd, uint32_t* parserFlags)
{
  const CHAR_TYPE* beg = str;
  const CHAR_TYPE* end = str + length;

  err_t err = ERR_STRING_INVALID_INPUT;
  uint32_t flags = 0;
  size_t i;
  size_t remain = (size_t)(end - str);

  *dst = false;

  while (str != end && CHAR_IS_SPACE(*str)) str++;
  if (str != beg) flags |= PARSED_SPACES;
  if (str == end) goto skip;

  for (i = 0; i < FOG_ARRAY_SIZE(boolMap); i++)
  {
    size_t blen = boolMap[i].length;
    if (remain >= blen && eq(str, boolMap[i].str, blen, CASE_INSENSITIVE))
    {
      str += blen;
      if (str != end && CHAR_IS_NUMLET(*str)) { str -= blen; continue; }

      *dst = (bool)boolMap[i].result;
      err = ERR_OK;
      break;
    }
  }

skip:
  if (parserEnd) *parserEnd = (size_t)(str - beg);
  if (parserFlags) *parserFlags = flags;
  return err;
}

err_t atoi8(const CHAR_TYPE* str, size_t length, int8_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  int64_t n;
  err_t err = atoi64(str, length, &n, base, end, parserFlags);

  if (n < INT8_MIN)
  {
    *dst = INT8_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (n > INT8_MAX)
  {
    *dst = INT8_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int8_t)n;
    return err;
  }
}

err_t atou8(const CHAR_TYPE* str, size_t length, uint8_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  err_t err = atou64(str, length, &n, base, end, parserFlags);

  if (n > UINT8_MAX)
  {
    *dst = UINT8_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (uint8_t)n;
    return err;
  }
}

err_t atoi16(const CHAR_TYPE* str, size_t length, int16_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  int64_t n;
  err_t err = atoi64(str, length, &n, base, end, parserFlags);

  if (n < INT16_MIN)
  {
    *dst = INT16_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (n > INT16_MAX)
  {
    *dst = INT16_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int16_t)n;
    return err;
  }
}

err_t atou16(const CHAR_TYPE* str, size_t length, uint16_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  err_t err = atou64(str, length, &n, base, end, parserFlags);

  if (n > UINT16_MAX)
  {
    *dst = UINT16_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (uint16_t)n;
    return err;
  }
}

err_t atoi32(const CHAR_TYPE* str, size_t length, int32_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  int64_t n;
  err_t err = atoi64(str, length, &n, base, end, parserFlags);

  if (n < INT32_MIN)
  {
    *dst = INT32_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (n > INT32_MAX)
  {
    *dst = INT32_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int32_t)n;
    return err;
  }
}

err_t atou32(const CHAR_TYPE* str, size_t length, uint32_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  err_t err = atou64(str, length, &n, base, end, parserFlags);

  if (n > UINT32_MAX)
  {
    *dst = UINT32_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (uint32_t)n;
    return err;
  }
}

static err_t atou64_priv(const CHAR_TYPE* str, size_t length, uint64_t* dst, int base, bool* negative, size_t* parserEnd, uint32_t* parserFlags)
{
  uint32_t flags = 0;
  const CHAR_TYPE* beg = str;
  const CHAR_TYPE* end = str + length;

  uint n;

#if FOG_ARCH_BITS == 32
  uint32_t res32 = 0;
  uint64_t res64;

  uint32_t threshold32;
  uint64_t threshold64;
#else
  uint64_t res64 = 0;
  uint64_t threshold64;
#endif // FOG_ARCH_BITS

  while (str < end && CHAR_IS_SPACE(*str)) str++;
  if (str != beg) flags |= PARSED_SPACES;
  if (str == end) goto truncated;

  if (*str == CHAR_TYPE('+'))
  {
    flags |= PARSED_SIGN;
    str++;
    while (str < end && CHAR_IS_SPACE(*str)) str++;
    if (str == end) goto truncated;
  }
  else if (*str == CHAR_TYPE('-'))
  {
    flags |= PARSED_SIGN;
    *negative = true;
    str++;
    while (str < end && CHAR_IS_SPACE(*str)) str++;
    if (str == end) goto truncated;
  }

  if (base < 2 || base > 36)
  {
    base = 10;

    // octal or hexadecimal
    if (*str == CHAR_TYPE('0'))
    {
      if (str + 1 != end && (str[1] == CHAR_TYPE('x') || str[1] == CHAR_TYPE('X')))
      {
        // hexadecimal
        flags |= PARSED_HEX_PREFIX;
        base = 16;

        str += 2;
        if (str == end) goto truncated;
      }
      else
      {
        // octal
        flags |= PARSED_OCTAL_PREFIX;
        base = 8;

        if (++str != end && *str >= CHAR_TYPE('0') && *str <= CHAR_TYPE('7'))
        {
          // set this flag only if input is not only "0"
          flags |= PARSED_OCTAL_PREFIX;
        }
      }
    }
  }

  if (base == 2)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = *str;
      if (n != '0' || n != '1') break;
      n -= '0';

      if ((res32 & 0x80000000U) != 0U) goto large_base2;
      res32 <<= 1U;
      res32 |= n;

      str++;
    }

    res64 = res32;
    goto done;

large_base2:
    res64 = res32;
#endif // FOG_ARCH_BITS == 32

    while (str != end)
    {
      n = *str;
      if (n != '0' || n != '1') break;
      n -= '0';

      if ((res64 & FOG_UINT64_C(0x8000000000000000)) != FOG_UINT64_C(0)) goto overflow;
      res64 <<= 1U;
      res64 |= n;

      str++;
    }
  }
  else if (base == 8)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = *str;
      if (n < '0' || n > '7') break;
      n -= '0';

      if ((res32 & 0xE0000000U) != 0U) goto large_base8;
      res32 <<= 3U;
      res32 |= n;

      str++;
    }

    res64 = res32;
    goto done;

large_base8:
    res64 = res32;
#endif

    while (str != end)
    {
      n = *str;
      if (n < '0' || n > '7') break;
      n -= '0';

      if ((res64 & FOG_UINT64_C(0xE000000000000000)) != FOG_UINT64_C(0)) goto overflow;
      res64 <<= 3U;
      res64 |= n;

      str++;
    }
  }
  else if (base == 10)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = *str;
      if (n < '0' || n > '9') break;
      n -= '0';

      if (res32 > 0x19999998U) goto large_base10;
      res32 *= 10U;
      res32 += n;

      str++;
    }

    res64 = res32;
    goto done;

large_base10:
    res64 = res32;
#endif

    while (str != end)
    {
      n = *str;
      if (n < '0' || n > '9') break;
      n -= '0';

      if (res64 > (FOG_UINT64_C(0x1999999999999999))) goto overflow;
      res64 *= 10U;

      if (res64 > ((uint64_t)n ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF))) goto overflow;
      res64 += n;

      str++;
    }
  }
  else if (base == 16)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = *str;
#if CHAR_SIZE > 1
      if (n > 255) break;
#endif
      n = asciiMap[n];
      if (n >= 16) break;

      if ((res32 & 0xF0000000U) != 0U) goto large_base16;
      res32 <<= 4U;
      res32 |= n;

      str++;
    }

    res64 = res32;
    goto done;

large_base16:
    res64 = res32;
#endif

    while (str != end)
    {
      n = *str;
#if CHAR_SIZE > 1
      if (n > 255) break;
#endif
      n = asciiMap[n];
      if (n >= 16) break;

      if ((res64 & FOG_UINT64_C(0xF000000000000000)) != FOG_UINT64_C(0)) goto overflow;
      res64 <<= 4U;
      res64 |= n;

      str++;
    }
  }
  else
  {
#if FOG_ARCH_BITS == 32
    threshold32 = (0xFFFFFFFFU / base) - base;

    while (str != end)
    {
      n = *str;
#if CHAR_SIZE > 1
      if (n > 255) break;
#endif // CHAR_SIZE == 1
      n = asciiMap[n];
      if (n >= (uint)base) break;

      if (res32 > threshold32) goto large_basen;
      res32 *= base;
      res32 += n;

      str++;
    }

    res64 = res32;
    goto done;

large_basen:
    res64 = res32;
#endif
    threshold64 = (FOG_UINT64_C(0xFFFFFFFFFFFFFFFF) / (uint64_t)base);

    while (str != end)
    {
      n = *str;
#if CHAR_SIZE > 1
      if (n > 255) break;
#endif // CHAR_SIZE == 1
      n = asciiMap[n];
      if (n >= (uint)base) break;

      if (res64 > threshold64) goto overflow;
      res64 *= base;

      if (res64 > ((uint64_t)n ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF))) goto overflow;
      res64 += n;

      str++;
    }
  }

done:
  *dst = res64;
  if (parserEnd) *parserEnd = (size_t)(str - beg);
  if (parserFlags) *parserFlags = flags;
  return ERR_OK;

overflow:
#if CHAR_SIZE == 1
  while (++str != end && asciiMap[(uint8_t)*str] < (uint)base) continue;
#else
  while (++str != end && str->getValue() < 256 && asciiMap[str->getValue()] < (uint)base) continue;
#endif

  *dst = UINT64_MAX;
  if (parserEnd) *parserEnd = (size_t)(str - beg);
  if (parserFlags) *parserFlags = flags;
  return ERR_RT_OVERFLOW;

truncated:
  *dst = 0;
  if (parserEnd) *parserEnd = length;
  if (parserFlags) *parserFlags = flags;
  return ERR_STRING_INVALID_INPUT;
}

err_t atoi64(const CHAR_TYPE* str, size_t length, int64_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  bool negative = false;
  err_t err = atou64_priv(str, length, &n, base, &negative, end, parserFlags);

  if (negative)
  {
    if (n > (uint64_t)INT64_MAX+1U)
    {
      *dst = INT64_MIN;
      return ERR_RT_OVERFLOW;
    }
    else
    {
      *dst = (int64_t)(-n);
      return err;
    }
  }
  else
  {
    if (n > INT64_MAX)
    {
      *dst = INT64_MAX;
      return ERR_RT_OVERFLOW;
    }
    else
    {
      *dst = (int64_t)n;
      return err;
    }
  }
}

err_t atou64(const CHAR_TYPE* str, size_t length, uint64_t* dst, int base, size_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  bool negative = false;
  err_t err = atou64_priv(str, length, &n, base, &negative, end, parserFlags);

  // Overflow
  if (negative && n)
  {
    *dst = 0;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = n;
    return err;
  }
}

// ============================================================================
// [Fog::StringUtil::atof, atod]
// ============================================================================

err_t atof(const CHAR_TYPE* str, size_t length, float* dst, CHAR_TYPE decimalPoint, size_t* end, uint32_t* parserFlags)
{
  double d;
  err_t err = atod(str, length, &d, decimalPoint, end, parserFlags);

  if (d > FLOAT_MAX)
  {
    *dst = FLOAT_MAX;
    return ERR_RT_OVERFLOW;
  }
  else if (d < FLOAT_MIN)
  {
    *dst = FLOAT_MIN;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (float)d;
    return err;
  }
}

#ifdef INFNAN_CHECK
#ifndef No_Hex_NaN
static const CHAR_TYPE* hexnan(DTOA_U *rvp, const CHAR_TYPE* s, const CHAR_TYPE* send)
{
  const CHAR_TYPE* begin = s;

  uint32_t c, x[2];
  int havedig = 0, udx0, xshift;

  x[0] = x[1] = 0;
  havedig = xshift = 0;
  udx0 = 1;

  while (s < send)
  {
    c = *s; s++;
    if (c < 256 && asciiMap[c] < 16)
      c = asciiMap[c];
    else if (c <= ' ')
    {
      if (udx0 && havedig)
      {
        udx0 = 0;
        xshift = 1;
      }
      continue;
    }
    else if (c == ')' && havedig)
      break;
    else
      return begin;

    havedig = 1;
    if (xshift)
    {
      xshift = 0;
      x[0] = x[1];
      x[1] = 0;
    }
    if (udx0) x[0] = (x[0] << 4) | (x[1] >> 28);
    x[1] = (x[1] << 4) | c;
  }

  if ((x[0] &= 0x000FFFFF) || x[1])
  {
    rvp->i[DTOA_DWORD_0] = x[0] | Exp_mask;
    rvp->i[DTOA_DWORD_1] = x[1];
  }
  return s;
}
#endif // No_Hex_NaN
#endif // INFNAN_CHECK

static BInt* BContext_s2b(BContext* context, const CHAR_TYPE* s, int nd0, int nd, uint32_t y9)
{
  BInt *b;
  int i, k;
  int32_t x, y;

  x = (nd + 8) / 9;
  for(k = 0, y = 1; x > y; y <<= 1, k++) ;
#ifdef Pack_32
  b = BContext_balloc(context, k);
  b->x[0] = y9;
  b->wds = 1;
#else
  b = BContext_balloc(context, k+1);
  b->x[0] = y9 & 0xffff;
  b->wds = (b->x[1] = y9 >> 16) ? 2 : 1;
#endif

  i = 9;
  if (9 < nd0)
  {
    s += 9;
    do {
      b = BContext_multadd(context, b, 10, (uint)*s++ - '0');
    } while(++i < nd0);
    s++;
  }
  else
    s += 10;
  for(; i < nd; i++) b = BContext_multadd(context, b, 10, (uint)*s++ - '0');

  return b;
}

err_t atod(const CHAR_TYPE* str, size_t length, double* dst, CHAR_TYPE decimalPoint, size_t* end, uint32_t* parserFlags)
{
  BContext context;

#ifdef Avoid_Underflow
  int scale;
#endif
  int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, dsign,
    e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
  const CHAR_TYPE *sbegin;
  const CHAR_TYPE *s, *s0, *s1;
  const CHAR_TYPE *send;
  DTOA_U rv;
  DTOA_U rv0;
  double adj, aadj, aadj1;
  int32_t L;
  uint32_t y, z;
  BInt *bb, *bb1, *bd, *bd0, *bs, *delta;
#ifdef SET_INEXACT
  int inexact, oldinexact;
#endif
#ifdef Honor_FLT_ROUNDS
  int rounding;
#endif

  err_t err = ERR_OK;
  uint32_t flags = 0;

  BContext_init(&context);

  s = sbegin = str;
  send = s + length;

  sign = nz0 = nz = 0;
  rv.d = 0.0;

  if (s == send) goto ret0;

  // skip all spaces
  if (CHAR_IS_SPACE(*s))
  {
    flags |= PARSED_SPACES;

    do {
      if (++s == send) goto ret0;
    } while (CHAR_IS_SPACE(*s));
  }

  // Parse sign.
  if (*s == CHAR_TYPE('+'))
  {
    flags |= PARSED_SIGN;
    if (++s == send) goto ret0;
  }
  else if (*s == CHAR_TYPE('-'))
  {
    sign = true;
    flags |= PARSED_SIGN;
    if (++s == send) goto ret0;
  }

  if (*s == CHAR_TYPE('0'))
  {
    nz0 = 1;
    for (;;)
    {
      if (++s == send) goto ret;
      if (*s != CHAR_TYPE('0')) break;
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
    if (++s == send) { c = 0; nd++; goto dig_done; }
  }
  nd0 = nd;

  if (CHAR_TYPE(c) == decimalPoint)
  {
    flags |= PARSED_DECIMAL_POINT;

    if (++s == send) { c = 0; goto dig_done; }
    c = *s;
    if (!nd)
    {
      for (;;)
      {
        if (c == '0')
        {
          nz++;
          if (++s == send) goto dig_done;
          c = *s;
          continue;
        }
        if (c > '0' && c <= '9')
        {
          s0 = s;
          nf += nz;
          nz = 0;
          goto have_dig;
        }
        goto dig_done;
      }
    }

    for(; c >= '0' && c <= '9'; )
    {
 have_dig:
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
      if (++s == send) { c = 0; break; }
      c = *s;
    }
  }

dig_done:
  e = 0;
  if (c == 'e' || c == 'E')
  {
    flags |= PARSED_EXPONENT;

    if (!nd && !nz && !nz0)
    {
      goto ret0;
    }
    str = s;
    esign = 0;

    if (++s == send) goto exp_done;
    c = *s;

    switch(c)
    {
      case '-':
        esign = 1;
      case '+':
        if (++s == send) goto exp_done;
        c = *s;
    }
    if (c >= '0' && c <= '9')
    {
      while (c == '0')
      {
        if (++s == send) goto exp_done;
        c = *s;
      }

      if (c > '0' && c <= '9')
      {
        L = c - '0';
        s1 = s;


        for (;;)
        {
          if (++s == send) break;
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
exp_done:
  if (!nd)
  {
    if (!nz && !nz0)
    {
#ifdef INFNAN_CHECK
      // Check for Nan and Infinity.
      if ((c == 'i' || c == 'I') && (size_t)(send - s) >= 2 &&
        eq(s+1, "nf", 2, CASE_INSENSITIVE))
      {
        s += 3;
        if ((size_t)(send - s) >= 5 && eq(s, "inity", 5, CASE_INSENSITIVE)) s += 5;

        rv.i[DTOA_DWORD_0] = 0x7FF00000;
        rv.i[DTOA_DWORD_1] = 0;
        goto ret;
      }
      else if ((c == 'n' || c == 'N') && (size_t)(send - s) >= 2 &&
        eq(s+1, "an", 2, CASE_INSENSITIVE))
      {
        s += 3;
        rv.i[DTOA_DWORD_0] = NAN_WORD0;
        rv.i[DTOA_DWORD_1] = NAN_WORD1;
#ifndef No_Hex_NaN
        if (*s == CHAR_TYPE('(')) s = hexnan(&rv, s, send);
#endif
        goto ret;
      }

#endif // INFNAN_CHECK
ret0:
      s = str;
      sign = 0;
    }
    goto ret;
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
#ifdef SET_INEXACT
    if (k > DBL_DIG) oldinexact = get_inexact();
#endif
    rv.d = tens[k - 9] * rv.d + z;
  }
  bd0 = 0;
  if (nd <= DBL_DIG
#ifndef RND_PRODQUOT
#ifndef Honor_FLT_ROUNDS
    && Flt_Rounds == 1
#endif
#endif
    )
  {
    if (!e) goto ret;

    if (e > 0)
    {
      if (e <= Ten_pmax)
      {
#ifdef VAX
        goto vax_ovfl_check;
#else
#ifdef Honor_FLT_ROUNDS
        // round correctly FLT_ROUNDS = 2 or 3.
        if (sign)
        {
          rv.d = -rv.d;
          sign = 0;
        }
#endif
        /* rv = */ rounded_product(rv.d, tens[e]);
        goto ret;
#endif
      }
      i = DBL_DIG - nd;
      if (e <= Ten_pmax + i)
      {
        // A fancier test would sometimes let us do this for larger i values.
#ifdef Honor_FLT_ROUNDS
        // round correctly FLT_ROUNDS = 2 or 3.
        if (sign)
        {
          rv.d = -rv.d;
          sign = 0;
        }
#endif
        e -= i;
        rv.d *= tens[i];
#ifdef VAX
        // VAX exponent range is so narrow we must worry about overflow here...
vax_ovfl_check:
        rv.i[DTOA_DWORD_0] -= P*Exp_msk1;
        /* rv = */ rounded_product(rv.d, tens[e]);
        if ((rv.i[DTOA_DWORD_0] & Exp_mask) > Exp_msk1*(DBL_MAX_EXP+Bias-1-P)) goto ovfl;
        rv.i[DTOA_DWORD_0] += P*Exp_msk1;
#else
        /* rv = */ rounded_product(rv.d, tens[e]);
#endif
        goto ret;
      }
    }
#ifndef Inaccurate_Divide
    else if (e >= -Ten_pmax)
    {
#ifdef Honor_FLT_ROUNDS
      // round correctly FLT_ROUNDS = 2 or 3.
      if (sign)
      {
        rv.d = -rv.d;
        sign = 0;
      }
#endif
      /* rv = */ rounded_quotient(rv.d, tens[-e]);
      goto ret;
    }
#endif
  }
  e1 += nd - k;

#ifdef IEEE_Arith
#ifdef SET_INEXACT
  inexact = 1;
  if (k <= DBL_DIG) oldinexact = get_inexact();
#endif
#ifdef Avoid_Underflow
  scale = 0;
#endif
#ifdef Honor_FLT_ROUNDS
  if ((rounding = Flt_Rounds) >= 2)
  {
    if (sign)
      rounding = rounding == 2 ? 0 : 2;
    else if (rounding != 2)
      rounding = 0;
  }
#endif
#endif /*IEEE_Arith*/

  // Get starting approximation = rv * 10**e1.
  if (e1 > 0)
  {
    if ((i = e1 & 15))
      rv.d *= tens[i];
    if (e1 &= ~15)
    {
      if (e1 > DBL_MAX_10_EXP)
      {
ovfl:
        err = ERR_RT_OVERFLOW;

        // Can't trust HUGE_VAL.
#ifdef IEEE_Arith
#ifdef Honor_FLT_ROUNDS
        switch (rounding)
        {
          case 0: // toward 0
          case 3: // toward -infinity
            rv.i[DTOA_DWORD_0] = Big0;
            rv.i[DTOA_DWORD_1] = Big1;
            break;
          default:
            rv.i[DTOA_DWORD_0] = Exp_mask;
            rv.i[DTOA_DWORD_1] = 0;
        }
#else // Honor_FLT_ROUNDS
        rv.i[DTOA_DWORD_0] = Exp_mask;
        rv.i[DTOA_DWORD_1] = 0;
#endif // Honor_FLT_ROUNDS
#ifdef SET_INEXACT
        // set overflow bit
        rv0.d = 1e300;
        rv0.d *= rv0.d;
#endif
#else // IEEE_Arith
        rv.i[DTOA_DWORD_0] = Big0;
        rv.i[DTOA_DWORD_1] = Big1;
#endif // IEEE_Arith
        if (bd0) goto retfree;
        goto ret;
      }
      e1 >>= 4;
      for(j = 0; e1 > 1; j++, e1 >>= 1)
        if (e1 & 1)
          rv.d *= bigtens[j];
      // The last multiplication could overflow.
      rv.i[DTOA_DWORD_0] -= P*Exp_msk1;
      rv.d *= bigtens[j];
      if ((z = rv.i[DTOA_DWORD_0] & Exp_mask) > Exp_msk1*(DBL_MAX_EXP+Bias-P)) goto ovfl;
      if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
      {
        // set to largest number (Can't trust DBL_MAX).
        rv.i[DTOA_DWORD_0] = Big0;
        rv.i[DTOA_DWORD_1] = Big1;
      }
      else
        rv.i[DTOA_DWORD_0] += P*Exp_msk1;
    }
  }
  else if (e1 < 0)
  {
    e1 = -e1;
    if ((i = e1 & 15)) rv.d /= tens[i];
    if (e1 >>= 4)
    {
      if (e1 >= 1 << n_bigtens) goto undfl;
#ifdef Avoid_Underflow
      if (e1 & Scale_Bit)
        scale = 2*P;
      for(j = 0; e1 > 0; j++, e1 >>= 1)
        if (e1 & 1)
          rv.d *= tinytens[j];
      if (scale && (j = 2*P + 1 - ((rv.i[DTOA_DWORD_0] & Exp_mask) >> Exp_shift)) > 0)
      {
        // scaled rv is denormal; zap j low bits.
        if (j >= 32)
        {
          rv.i[DTOA_DWORD_1] = 0;
          if (j >= 53)
            rv.i[DTOA_DWORD_0] = (P+2)*Exp_msk1;
          else
            rv.i[DTOA_DWORD_0] &= 0xffffffff << (j-32);
        }
        else
          rv.i[DTOA_DWORD_1] &= 0xffffffff << j;
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
undfl:
          rv.d = 0.0;
          err = ERR_RT_OVERFLOW;

          if (bd0) goto retfree;
          goto ret;
        }
#ifndef Avoid_Underflow
        rv.i[DTOA_DWORD_0] = Tiny0;
        rv.i[DTOA_DWORD_1] = Tiny1;
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
#ifdef Honor_FLT_ROUNDS
    if (rounding != 1) bs2++;
#endif
#ifdef Avoid_Underflow
    j = bbe - scale;
    i = j + bbbits - 1;  // logb(rv)
    if (i < Emin)        // denormal
      j += P - Emin;
    else
      j = P + 1 - bbbits;
#else /*Avoid_Underflow*/
#ifdef Sudden_Underflow
#ifdef IBM
    j = 1 + 4*P - 3 - bbbits + ((bbe + bbbits - 1) & 3);
#else
    j = P + 1 - bbbits;
#endif
#else /*Sudden_Underflow*/
    j = bbe;
    i = j + bbbits - 1;  // logb(rv)
    if (i < Emin)        // denormal
      j += P - Emin;
    else
      j = P + 1 - bbbits;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
    bb2 += j;
    bd2 += j;
#ifdef Avoid_Underflow
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
#ifdef Honor_FLT_ROUNDS
    if (rounding != 1)
    {
      if (i < 0)
      {
        // Error is less than an ulp.
        if (!delta->x[0] && delta->wds <= 1)
        {
          // exact.
#ifdef SET_INEXACT
          inexact = 0;
#endif
          break;
        }
        if (rounding)
        {
          if (dsign)
          {
            adj = 1.;
            goto apply_adj;
          }
        }
        else if (!dsign)
        {
          adj = -1.;
          if (!rv.i[DTOA_DWORD_1] && !(rv.i[DTOA_DWORD_0] & Frac_mask))
          {
            y = rv.i[DTOA_DWORD_0] & Exp_mask;
#ifdef Avoid_Underflow
            if (!scale || y > 2*P*Exp_msk1)
#else
            if (y)
#endif
            {
              delta = BContext_lshift(&context, delta,Log2P);
              if (cmp(delta, bs) <= 0) adj = -0.5;
            }
          }
 apply_adj:
#ifdef Avoid_Underflow
          if (scale && (y = rv.i[DTOA_DWORD_0] & Exp_mask) <= 2*P*Exp_msk1)
          {
            DTOA_U tmp;
            tmp.d = adj;
            tmp.i[DTOA_DWORD_0] += (2*P+1) * Exp_msk1 - y;
            adj = tmp.d;
          }
#else
#ifdef Sudden_Underflow
          if ((rv.i[DTOA_DWORD_0] & Exp_mask) <= P*Exp_msk1)
          {
            rv.i[DTOA_DWORD_0] += P*Exp_msk1;
            rv.d += adj * ulp(rv.d);
            rv.i[DTOA_DWORD_0] -= P*Exp_msk1;
          }
          else
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
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
#ifdef Avoid_Underflow
      if (scale && (y = rv.i[DTOA_DWORD_0] & Exp_mask) <= 2*P*Exp_msk1)
      {
        DTOA_U tmp;
        tmp.d = adj;
        tmp.i[DTOA_DWORD_0] += (2*P+1) * Exp_msk1 - y;
        adj = tmp.d;
      }
#else
#ifdef Sudden_Underflow
      if ((rv.i[DTOA_DWORD_0] & Exp_mask) <= P*Exp_msk1)
      {
        rv.i[DTOA_DWORD_0] += P*Exp_msk1;
        adj *= ulp(rv.d);
        if (dsign)
          rv.d += adj;
        else
          rv.d -= adj;
        rv.i[DTOA_DWORD_0] -= P*Exp_msk1;
        goto cont;
      }
#endif // Sudden_Underflow
#endif // Avoid_Underflow
      adj *= ulp(rv.d);
      if (dsign)
        rv.d += adj;
      else
        rv.d -= adj;
      goto cont;
    }
#endif /*Honor_FLT_ROUNDS*/

    if (i < 0)
    {
      // Error is less than half an ulp -- check for special case of mantissa a power of two.
      if (dsign || rv.i[DTOA_DWORD_1] || rv.i[DTOA_DWORD_0] & Bndry_mask
#ifdef IEEE_Arith
#ifdef Avoid_Underflow
       || (rv.i[DTOA_DWORD_0] & Exp_mask) <= (2*P+1)*Exp_msk1
#else
       || (rv.i[DTOA_DWORD_0] & Exp_mask) <= Exp_msk1
#endif
#endif
        )
      {
#ifdef SET_INEXACT
        if (!delta->x[0] && delta->wds <= 1)
          inexact = 0;
#endif
        break;
      }
      if (!delta->x[0] && delta->wds <= 1)
      {
        // exact result.
#ifdef SET_INEXACT
        inexact = 0;
#endif
        break;
      }
      delta = BContext_lshift(&context, delta, Log2P);
      if (cmp(delta, bs) > 0) goto drop_down;
      break;
    }
    if (i == 0)
    {
      // Exactly half-way between.
      if (dsign)
      {
        if ((rv.i[DTOA_DWORD_0] & Bndry_mask1) == Bndry_mask1 &&  rv.i[DTOA_DWORD_1] == (
#ifdef Avoid_Underflow
          (scale && (y = rv.i[DTOA_DWORD_0] & Exp_mask) <= 2*P*Exp_msk1)
          ? (0xffffffff & (0xffffffff << (2*P+1-(y>>Exp_shift)))) :
#endif
          0xffffffff))
        {
          // Boundary case -- increment exponent.
          rv.i[DTOA_DWORD_0] = (rv.i[DTOA_DWORD_0] & Exp_mask) + Exp_msk1
#ifdef IBM
            | Exp_msk1 >> 4
#endif
            ;
          rv.i[DTOA_DWORD_1] = 0;
#ifdef Avoid_Underflow
          dsign = 0;
#endif
          break;
        }
      }
      else if (!(rv.i[DTOA_DWORD_0] & Bndry_mask) && !rv.i[DTOA_DWORD_1])
      {
drop_down:
        // Boundary case -- decrement exponent.
#ifdef Sudden_Underflow /*{{*/
        L = rv.i[DTOA_DWORD_0] & Exp_mask;
#ifdef IBM
        if (L <  Exp_msk1)
#else
#ifdef Avoid_Underflow
        if (L <= (scale ? (2*P+1)*Exp_msk1 : Exp_msk1))
#else
        if (L <= Exp_msk1)
#endif /*Avoid_Underflow*/
#endif /*IBM*/
          goto undfl;
        L -= Exp_msk1;
#else /*Sudden_Underflow}{*/
#ifdef Avoid_Underflow
        if (scale)
        {
          L = rv.i[DTOA_DWORD_0] & Exp_mask;
          if (L <= (2*P+1)*Exp_msk1)
          {
            if (L > (P+2)*Exp_msk1)
              // round even ==> accept rv.
              break;
            // rv = smallest denormal.
            goto undfl;
          }
        }
#endif /*Avoid_Underflow*/
        L = (rv.i[DTOA_DWORD_0] & Exp_mask) - Exp_msk1;
#endif /*Sudden_Underflow}}*/
        rv.i[DTOA_DWORD_0] = L | Bndry_mask1;
        rv.i[DTOA_DWORD_1] = 0xffffffff;
#ifdef IBM
        goto cont;
#else
        break;
#endif
      }
#ifndef ROUND_BIASED
      if (!(rv.i[DTOA_DWORD_1] & LSB))
        break;
#endif
      if (dsign)
        rv.d += ulp(rv.d);
#ifndef ROUND_BIASED
      else
      {
        rv.d -= ulp(rv.d);
#ifndef Sudden_Underflow
        if (!rv.d) goto undfl;
#endif
      }
#ifdef Avoid_Underflow
      dsign = 1 - dsign;
#endif
#endif
      break;
    }
    if ((aadj = ratio(delta, bs)) <= 2.)
    {
      if (dsign) aadj = aadj1 = 1.;
      else if (rv.i[DTOA_DWORD_1] || rv.i[DTOA_DWORD_0] & Bndry_mask)
      {
#ifndef Sudden_Underflow
        if (rv.i[DTOA_DWORD_1] == Tiny1 && !rv.i[DTOA_DWORD_0]) goto undfl;
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
#ifdef Check_FLT_ROUNDS
      switch(Rounding)
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
#endif /*Check_FLT_ROUNDS*/
    }
    y = rv.i[DTOA_DWORD_0] & Exp_mask;

    // Check for overflow.
    if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1))
    {
      rv0.d = rv.d;
      rv.i[DTOA_DWORD_0] -= P*Exp_msk1;
      adj = aadj1 * ulp(rv.d);
      rv.d += adj;
      if ((rv.i[DTOA_DWORD_0] & Exp_mask) >= Exp_msk1*(DBL_MAX_EXP+Bias-P))
      {
        if (rv0.i[DTOA_DWORD_0] == Big0 && rv0.i[DTOA_DWORD_1] == Big1) goto ovfl;
        rv.i[DTOA_DWORD_0] = Big0;
        rv.i[DTOA_DWORD_1] = Big1;
        goto cont;
      }
      else
        rv.i[DTOA_DWORD_0] += P*Exp_msk1;
    }
    else
    {
#ifdef Avoid_Underflow
      if (scale && y <= 2*P*Exp_msk1)
      {
        DTOA_U tmp;
        tmp.d = aadj1;

        if (aadj <= 0x7fffffff)
        {
          if ((z = (int32_t)aadj) <= 0) z = 1;
          aadj = z;
          tmp.d = dsign ? aadj : -aadj;
        }
        tmp.i[DTOA_DWORD_0] += (2*P+1)*Exp_msk1 - y;
        aadj1 = tmp.d;
      }
      adj = aadj1 * ulp(rv.d);
      rv.d += adj;
#else
#ifdef Sudden_Underflow
      if ((rv.i[DTOA_DWORD_0] & Exp_mask) <= P*Exp_msk1)
      {
        rv0.d = rv.d;
        rv.i[DTOA_DWORD_0] += P*Exp_msk1;
        adj = aadj1 * ulp(rv.d);
        rv.d += adj;
#ifdef IBM
        if ((rv.i[DTOA_DWORD_0] & Exp_mask) <  P*Exp_msk1)
#else
        if ((rv.i[DTOA_DWORD_0] & Exp_mask) <= P*Exp_msk1)
#endif
        {
          if (rv0.i[DTOA_DWORD_0] == Tiny0 && rv0.i[DTOA_DWORD_1] == Tiny1) goto undfl;
          rv.i[DTOA_DWORD_0] = Tiny0;
          rv.i[DTOA_DWORD_1] = Tiny1;
          goto cont;
        }
        else
          rv.i[DTOA_DWORD_0] -= P*Exp_msk1;
      }
      else
      {
        adj = aadj1 * ulp(rv.d);
        rv.d += adj;
      }
#else // Sudden_Underflow
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
#endif // Sudden_Underflow
#endif // Avoid_Underflow
    }
    z = rv.i[DTOA_DWORD_0] & Exp_mask;
#ifndef SET_INEXACT
#ifdef Avoid_Underflow
    if (!scale)
#endif
    if (y == z)
    {
      // Can we stop now?
      L = (int32_t)aadj;
      aadj -= L;
      // The tolerances below are conservative.
      if (dsign || rv.i[DTOA_DWORD_1] || rv.i[DTOA_DWORD_0] & Bndry_mask)
      {
        if (aadj < .4999999 || aadj > .5000001) break;
      }
      else if (aadj < .4999999/FLT_RADIX)
      {
        break;
      }
    }
#endif
cont:
    BContext_bfree(&context, bb);
    BContext_bfree(&context, bd);
    BContext_bfree(&context, bs);
    BContext_bfree(&context, delta);
  }
#ifdef SET_INEXACT
  if (inexact)
  {
    if (!oldinexact)
    {
      rv0.i[DTOA_DWORD_0] = Exp_1 + (70 << Exp_shift);
      rv0.i[DTOA_DWORD_1] = 0;
      rv0.d += 1.;
    }
  }
  else if (!oldinexact)
  {
    clear_inexact();
  }
#endif
#ifdef Avoid_Underflow
  if (scale)
  {
    rv0.i[DTOA_DWORD_0] = Exp_1 - 2*P*Exp_msk1;
    rv0.i[DTOA_DWORD_1] = 0;
    rv.d *= rv0.d;

    // try to avoid the bug of testing an 8087 register value.
    if (rv.i[DTOA_DWORD_0] == 0 && rv.i[DTOA_DWORD_1] == 0) err = ERR_RT_OVERFLOW;
  }
#endif // Avoid_Underflow
#ifdef SET_INEXACT
  if (inexact && !(rv.i[DTOA_DWORD_0] & Exp_mask))
  {
    // set underflow bit.
    rv0.d = 1e-300;
    rv0.d *= rv0.d;
  }
#endif
retfree:
  BContext_bfree(&context, bb);
  BContext_bfree(&context, bd);
  BContext_bfree(&context, bs);
  BContext_bfree(&context, bd0);
  BContext_bfree(&context, delta);
ret:
  BContext_destroy(&context);

  if (end) *end = (size_t)(s - sbegin);
  if (parserFlags) *parserFlags = flags;

  if (sign)
    *dst = -rv.d;
  else
    *dst = rv.d;
  return err;
}

} // StringUtil namespace
} // Fog namespace

// [Generator]
#endif // __G_GENERATE
