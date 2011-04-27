// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_Core_p.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>

namespace Fog {

// ============================================================================
// [Fog::Numeric - Helpers]
// ============================================================================

static FOG_INLINE double mycbrt(double x)
{
  return (x > 0.0) ?  Math::pow( x, 1.0 / 3.0) :
         (x < 0.0) ? -Math::pow(-x, 1.0 / 3.0) : 0.0;
}

// ============================================================================
// [Fog::Numeric - SolveQuadraticFunction]
// ============================================================================

// I found one message on stackoverflow forum which noted that the standard
// equation to solve the quadratic function may be inaccurate. It's
// completely correct so I kept the message also here for developers who
// want to better understand the problem.
//
// URL to the problem:
//
//   http://stackoverflow.com/questions/4503849/quadratic-equation-in-ada/4504415#4504415
//
// The standard equation:
//
//   x0 = (-b + sqrt(delta)) / (2a)
//   x1 = (-b - sqrt(delta)) / (2a)
//
// When 4*a*c < b*b, computing x0 involves substracting close numbers, and
// makes you lose accuracy, so you use the following instead:
//
//   x0 = 2c / (-b - sqrt(delta))
//   x1 = 2c / (-b + sqrt(delta))
//
// Which yields a better x0, but whose x1 has the same problem as x0 had
// above. The correct way to compute the roots is therefore:
//
//   q  = -0.5 * (b + sign(b) * sqrt(delta))
//   x0 = q / a
//   x1 = c / q
template<typename Number>
static int FOG_CDECL _G2d_MathT_solveQuadraticFunction(Number* dst, const Number* src)
{
  double a = (double)src[0];
  double b = (double)src[1];
  double c = (double)src[2];

  // Catch the A and B near zero.
  if (Math::isFuzzyZero(a))
  {
    // A~=0 && B~=0.
    if (Math::isFuzzyZero(b)) return 0;

    dst[0] = (Number)(-c / b);
    return 1;
  }

  // The proposed solution.
  double d = b * b - 4.0 * a * c;
  if (d < 0.0) return 0;

  if (Math::isFuzzyPositiveZero(d))
  {
    dst[0] = (Number)(-b / (2.0 * a));
    return 1;
  }
  else
  {
    double s = Math::sqrt(d);
    double q = -0.5 * (b + ((b < 0.0) ? -s : s));

    dst[0] = (Number)(q / a);
    dst[1] = (Number)(c / q);

    // Sort.
    if (dst[0] > dst[1]) swap(dst[0], dst[1]);
    return 2;
  }
}

template<typename Number>
static int FOG_CDECL _G2d_MathT_solveQuadraticFunctionAt(Number* dst, const Number* src, Number tMin, Number tMax)
{
  double a = (double)src[0];
  double b = (double)src[1];
  double c = (double)src[2];
  double d;

  Number r0, r1;

  // Catch the A and B near zero.
  if (Math::isFuzzyZero(a))
  {
    // A~=0 && B~=0.
    if (Math::isFuzzyZero(b)) return 0;

    r0 = (Number)(-c / b);
    goto _OneRoot;
  }

  // The proposed solution.
  d = b * b - 4.0 * a * c;
  if (d < 0.0) return 0;

  if (Math::isFuzzyPositiveZero(d))
  {
    r0 = (Number)(-b / (2.0 * a));
    goto _OneRoot;
  }
  else
  {
    double s = Math::sqrt(d);
    double q = -0.5 * (b + ((b < 0.0) ? -s : s));

    r0 = (Number)(q / a);
    r1 = (Number)(c / q);
    if (r0 > r1) swap(r0, r1);

    if (r1 < tMin || r1 > tMax) goto _OneRoot;
    if (r0 < tMin || r0 > tMax) { r0 = r1; goto _OneRoot; }

    dst[0] = r0;
    dst[1] = r1;
    return 2;
  }

_OneRoot:
  if (r0 < tMin || r0 > tMax) return 0;

  dst[0] = r0;
  return 1;
}

// ============================================================================
// [Fog::Numeric - SolveCubicFunction]
// ============================================================================

// Roots3And4.c: Graphics Gems, original author Jochen Schwarze (schwarze@isa.de).
// See also the wiki article at http://en.wikipedia.org/wiki/Cubic_function for
// other equations.
template<typename Number>
static int FOG_CDECL _G2d_MathT_solveCubicFunction(Number* dst, const Number* src)
{
  if (Math::isFuzzyZero(src[0])) return Math::solveQuadraticFunction(dst, src + 1);

  // Convert to a normal form: x^3 + Ax^2 + Bx + C == 0.
  double _norm = (double)src[0];
  double a = (double)src[1] / _norm;
  double b = (double)src[2] / _norm;
  double c = (double)src[3] / _norm;

  // Substitute x = y - A/3 to eliminate quadric term:
  //
  //   x^3 + px + q = 0
  double sa = a * a;
  double p = (1.0 / 3.0) * ((-1.0 /  3.0) * sa + b);
  double q = (1.0 / 2.0) * (( 2.0 / 27.0) * sa * a - (1.0 / 3.0) * a * b + c);

  // Use Cardano's formula.
  double p3 = p * p * p;
  double d = q * q + p3;

  // Resubstitution constant.
  double sub = -(1.0 / 3.0) * a;

  if (Math::isFuzzyZero(d))
  {
    // One triple solution.
    if (Math::isFuzzyZero(q))
    {
      dst[0] = (Number)(sub);
      return 1;
    }
    // One single and one double solution.
    else
    {
      double u = mycbrt(-q);
      dst[0] = (Number)(sub + 2.0 * u);
      dst[1] = (Number)(sub - u      );

      // Sort.
      if (dst[0] > dst[1]) swap(dst[0], dst[1]);
      return 2;
    }
  }
  // Three real solutions.
  else if (d < 0.0)
  {
    double phi = (1.0 / 3.0) * Math::acos(-q / Math::sqrt(-p3));
    double t = 2.0 * sqrt(-p);

    dst[0] = (Number)(sub + t * Math::cos(phi)                   );
    dst[1] = (Number)(sub - t * Math::cos(phi + (MATH_PI / 3.0)) );
    dst[2] = (Number)(sub - t * Math::cos(phi - (MATH_PI / 3.0)) );

    // Sort.
    if (dst[0] > dst[1]) swap(dst[0], dst[1]);
    if (dst[1] > dst[2]) swap(dst[1], dst[2]);
    if (dst[0] > dst[1]) swap(dst[0], dst[1]);
    return 3;
  }
  // One real solution.
  else
  {
    double sqrt_d = sqrt(d);
    double u =  mycbrt(sqrt_d - q);
    double v = -mycbrt(sqrt_d + q);

    dst[0] = (Number)(sub + u + v);
    return 1;
  }
}

template<typename Number>
static int FOG_CDECL _G2d_MathT_solveCubicFunctionAt(Number* dst, const Number* src, Number tMin, Number tMax)
{
  Number tmp[3];
  int roots = _G2d_MathT_solveCubicFunction(tmp, src);
  int interestingRoots = 0;

  for (int i = 0; i < roots; i++)
  {
    if (tmp[i] < tMin || tmp[i] > tMax) continue;
    dst[interestingRoots++] = tmp[i];
  }

  return interestingRoots;
}

// ============================================================================
// [Fog::Core - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _core_math_init_solve(void)
{
  _core.mathf.solveQuadraticFunction = _G2d_MathT_solveQuadraticFunction<float>;
  _core.mathf.solveQuadraticFunctionAt = _G2d_MathT_solveQuadraticFunctionAt<float>;
  _core.mathf.solveCubicFunction = _G2d_MathT_solveCubicFunction<float>;
  _core.mathf.solveCubicFunctionAt = _G2d_MathT_solveCubicFunctionAt<float>;

  _core.mathd.solveQuadraticFunction = _G2d_MathT_solveQuadraticFunction<double>;
  _core.mathd.solveQuadraticFunctionAt = _G2d_MathT_solveQuadraticFunctionAt<double>;
  _core.mathd.solveCubicFunction = _G2d_MathT_solveCubicFunction<double>;
  _core.mathd.solveCubicFunctionAt = _G2d_MathT_solveCubicFunctionAt<double>;
}

} // Fog namespace
