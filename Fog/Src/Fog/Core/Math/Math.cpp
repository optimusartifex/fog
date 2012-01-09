// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {

// ============================================================================
// [Math::Besj]
// ============================================================================

// Function BESJ calculates Bessel function of first kind of order n
// Arguments:
//     n - an integer (>=0), the order
//     x - value at which the Bessel function is required
//--------------------
// C++ Mathematical Library
// Convereted from equivalent FORTRAN library
// Converetd by Gareth Walker for use by course 392 computational project
// All functions tested and yield the same results as the corresponding
// FORTRAN versions.
//
// If you have any problems using these functions please report them to
// M.Muldoon@UMIST.ac.uk
//
// Documentation available on the web
// http://www.ma.umist.ac.uk/mrm/Teaching/392/libs/392.html
// Version 1.0   8/98
// 29 October, 1999
//
// Adapted for use in AGG library by Andy Wilk <castor.vulgaris@gmail.com>
// Adapted for use in Fog library by Petr Kobalicek <kobalicek.petr@gmail.com>
template<typename NumT>
static NumT Math_besj(NumT x, int n)
{
  if (n < 0)
    return NumT(0);

  NumT d = NumT(1e-6);
  NumT b = NumT(0);

  if (Math::abs(x) <= d)
    return (n != 0) ? NumT(0) : NumT(1);

  // b1 is the value from the previous iteration.
  NumT b1 = NumT(0);

  // Set up a starting order for recurrence.
  int m1, m2;

  if (Math::abs(x) > NumT(5))
    m1 = (int)(Math::abs(1.4 * x + 60.0 / x));
  else
    m1 = (int)Math::abs(x) + 6;

  m2 = ((int)Math::abs(x) / 4) + 2 + n;
  if (m1 > m2) m2 = m1;
    
  // Apply recurrence down from curent max order
  for (;;) 
  {
    NumT c3 = NumT(0);
    NumT c2 = MathConstant<NumT>::getEpsilon();
    NumT c4 = NumT(0);

    int m8 = 1;
    if (m2 / 2 * 2 == m2) m8 = -1;

    int imax = m2 - 2;
    for (int i = 1; i <= imax; i++) 
    {
      NumT c6 = 2 * (m2 - i) * c2 / x - c3;
      c3 = c2;
      c2 = c6;

      if (m2 - i - 1 == n)
        b = c6;

      m8 = -1 * m8;
      if (m8 > 0)
        c4 = c4 + 2 * c6;
    }

    NumT c6 = NumT(2) * c2 / x - c3;
    if (n == 0) b = c6;

    c4 += c6;
    b /= c4;

    if (Math::abs(b - b1) < d)
      return b;

    b1 = b;
    m2 += 3;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_integrate(void);
FOG_NO_EXPORT void Math_init_solve(void);
FOG_NO_EXPORT void Math_init_vec(void);

FOG_NO_EXPORT void Math_init(void)
{
  fog_api.mathf_besj = Math_besj<float>;
  fog_api.mathd_besj = Math_besj<double>;

  Math_init_vec();
  Math_init_integrate();
  Math_init_solve();
}

} // Fog namespace
