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
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/Core/Tools/Swap.h>

namespace Fog {

// ============================================================================
// [Fog::SolvePolynomialA - Constant]
// ============================================================================

template<typename NumT>
static int FOG_CDECL MathT_solvePolynomialA_Constant(NumT* dst, const NumT* polynomial, const NumT_(Interval)* interval)
{
  FOG_UNUSED(dst);
  FOG_UNUSED(polynomial);
  FOG_UNUSED(interval);

  return 0;
}

// ============================================================================
// [Fog::SolvePolynomialA - Linear]
// ============================================================================

// x = -b / a.
template<typename NumT>
static int FOG_CDECL MathT_solvePolynomialA_Linear(NumT* dst, const NumT* polynomial, const NumT_(Interval)* interval)
{
  double a = double(polynomial[0]);
  double b = double(polynomial[1]);

  if (Math::isFuzzyZero(a))
    return 0;

  NumT x = NumT(-b / a);

  if (interval != NULL)
  {
    if (x < interval->getMin() || x > interval->getMax())
      return 0;
  }

  *dst = x;
  return 1;
}

// ============================================================================
// [Fog::SolvePolynomialA - Quadratic]
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
//   x0 = (-b + sqrt(delta)) / 2a
//   x1 = (-b - sqrt(delta)) / 2a
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
template<typename NumT>
static int FOG_CDECL MathT_solvePolynomialA_Quadratic(NumT* dst, const NumT* polynomial, const NumT_(Interval)* interval)
{
  double a = double(polynomial[0]);
  double b = double(polynomial[1]);
  double c = double(polynomial[2]);

  // Catch the A and B near zero.
  if (Math::isFuzzyZero(a))
  {
    // A~=0 && B~=0.
    if (Math::isFuzzyZero(b))
      return 0;

    NumT x0 = NumT(-c / b);
    if (interval != NULL && (x0 < interval->getMin() || x0 > interval->getMax()))
      return 0;

    dst[0] = x0;
    return 1;
  }
  else
  {
    // The proposed solution.
    double d = b * b - 4.0 * a * c;
    if (d < 0.0)
      return 0;

    // D~=0
    if (Math::isFuzzyPositiveZero(d))
    {
      // D~=0 && B~=0.
      if (Math::isFuzzyZero(b))
        return 0;

      NumT x0 = NumT(-c / b);

      if (interval != NULL && (x0 < interval->getMin() || x0 > interval->getMax()))
        return 0;

      dst[0] = x0;
      return 1;
    }
    else
    {
      double s = Math::sqrt(d);
      double q = -0.5 * (b + ((b < 0.0) ? -s : s));

      NumT x0 = NumT(q / a);
      NumT x1 = NumT(c / q);

      // Sort.
      if (x0 > x1)
        swap(x0, x1);

      if (interval != NULL)
      {
        NumT tMin = interval->getMin();
        NumT tMax = interval->getMax();

        int roots = 0;
        if (x0 >= tMin && x0 <= tMax)
          dst[roots++] = x0;
        if (x1 >= tMin && x1 <= tMax)
          dst[roots++] = x1;
        return roots;
      }
      else
      {
        dst[0] = x0;
        dst[1] = x1;
        return 2;
      }
    }
  }
}

// ============================================================================
// [Fog::SolvePolynomialA - Cubic]
// ============================================================================

// Roots3And4.c: Graphics Gems, original author Jochen Schwarze (schwarze@isa.de).
// See also the wiki article at http://en.wikipedia.org/wiki/Cubic_function for
// other equations.
template<typename NumT>
static int FOG_CDECL MathT_solvePolynomialA_Cubic(NumT* dst, const NumT* polynomial, const NumT_(Interval)* interval)
{
  int roots = 0;
  static const double _2_DIV_27 = 2.0 / 27.0;

  if (polynomial[0] == NumT(0.0))
    return Math::solvePolynomial(dst, polynomial + 1, MATH_POLYNOMIAL_DEGREE_QUADRATIC, interval);

  // Convert to a normal form: x^3 + Ax^2 + Bx + C == 0.
  double _norm = (double)polynomial[0];
  double a = (double)polynomial[1] / _norm;
  double b = (double)polynomial[2] / _norm;
  double c = (double)polynomial[3] / _norm;

  // Substitute x = y - A/3 to eliminate quadric term:
  //
  //   x^3 + px + q = 0
  double sa = a * a;
  double p = -MATH_1_DIV_9 * sa + MATH_1_DIV_3 * b;
  double q = 0.5 * ((_2_DIV_27 * sa - MATH_1_DIV_3 * b) * a + c);

  // Use Cardano's formula.
  double p3 = p * p * p;
  double d  = q * q + p3;

  // Resubstitution constant.
  double sub = -MATH_1_DIV_3 * a;

  if (Math::isFuzzyZero(d))
  {
    // One triple solution.
    if (Math::isFuzzyZero(q))
    {
      roots = 1;
      dst[0] = NumT(sub);
    }
    // One single and one double solution.
    else
    {
      double u = Math::cbrt(-q);

      roots = 2;
      dst[0] = NumT(sub + 2.0 * u);
      dst[1] = NumT(sub - u      );

      // Sort.
      if (dst[0] > dst[1])
        swap(dst[0], dst[1]);
    }
  }
  // Three real solutions.
  else if (d < 0.0)
  {
    double phi = MATH_1_DIV_3 * Math::acos(-q / Math::sqrt(-p3));
    double t = 2.0 * sqrt(-p);

    roots = 3;
    dst[0] = NumT(sub + t * Math::cos(phi                ));
    dst[1] = NumT(sub - t * Math::cos(phi + MATH_THIRD_PI));
    dst[2] = NumT(sub - t * Math::cos(phi - MATH_THIRD_PI));

    // Sort.
    if (dst[0] > dst[1])
      swap(dst[0], dst[1]);
    if (dst[1] > dst[2])
      swap(dst[1], dst[2]);
    if (dst[0] > dst[1])
      swap(dst[0], dst[1]);
  }
  // One real solution.
  else
  {
    double sqrt_d = sqrt(d);
    double u =  Math::cbrt(sqrt_d - q);
    double v = -Math::cbrt(sqrt_d + q);

    roots = 1;
    dst[0] = NumT(sub + u + v);
  }

  if (interval != NULL)
  {
    NumT tMin = interval->getMin();
    NumT tMax = interval->getMax();

    int interestingRoots = 0;
    for (int i = 0; i < roots; i++)
    {
      if (dst[i] < tMin || dst[i] > tMax)
        continue;
      dst[interestingRoots++] = dst[i];
    }
    return interestingRoots;
  }

  return roots;
}

// ============================================================================
// [Fog::SolvePolynomialA - Quartic]
// ============================================================================

template<typename NumT>
static int FOG_CDECL MathT_solvePolynomialA_Quartic(NumT* dst, const NumT* polynomial, const NumT_(Interval)* interval)
{
  if (Math::isFuzzyZero(polynomial[0]))
    Math::solvePolynomial(dst, polynomial + 1, MATH_POLYNOMIAL_DEGREE_CUBIC);

  double _norm = polynomial[0];
  double a = double(polynomial[1]) / _norm;
  double b = double(polynomial[2]) / _norm;
  double c = double(polynomial[3]) / _norm;
  double d = double(polynomial[4]) / _norm;

  double _aa = a * a;
  double _2b = b * 2.0;
  double _cc = c * c;
  double _4d = d * 4.0;

  double base = a * (-0.25);
  double q0 = _aa * 0.75 - _2b;
  double q1;

  {
    double cFunction[4];
    double cRoots[4];

    cFunction[0] = 1.0;
    cFunction[1] =-b;
    cFunction[2] =-_4d + a * c;
    cFunction[3] = _4d * b - _cc - _aa * d;

    uint cRootsCount = Math::solvePolynomial(cRoots, cFunction, MATH_POLYNOMIAL_DEGREE_CUBIC);
    if (cRootsCount == 0)
      return 0;

    double x = cRoots[cRootsCount - 1];
    double w = _aa * 0.25 - b + x;

    if (w > MATH_EPSILON_D)
    {
      double r = Math::sqrt(w);
      base += 0.5 * r;

      q0 -= w;
      w = (a * (b - _aa * 0.25) - c * 2.0) / r;
    }
    else
    {
      w = 2.0 * Math::sqrt(x * x - _4d);
    }

    q1 = q0 + w;
    q0 = q0 - w;

    if (q0 < 0.0 || (q0 > q1 && q1 >= 0.0))
      swap(q0, q1);
  }

  int roots = 0;
  if (q0 >= 0.0)
  {
    double v = Math::sqrt(q0) * 0.5;

    if (v >= MATH_EPSILON_D)
    {
      dst[roots++] = NumT(base - v);
      dst[roots++] = NumT(base + v);
    }
    else
    {
      dst[roots++] = NumT(base);
    }
  }

  if (q1 >= 0.0 && !Math::isFuzzyEq(q0, q1))
  {
    double v = Math::sqrt(q1) * 0.5;

    if (roots == 2)
      dst[2] = dst[1];

    dst[1] = dst[0];
    roots++;

    if (v >= MATH_EPSILON_D)
    {
      dst[0] = NumT(base - v);
      dst[roots++] = NumT(base + v);
    }
    else
    {
      dst[0] = NumT(base);
    }
  }

  if (interval != NULL)
  {
    NumT tMin = interval->getMin();
    NumT tMax = interval->getMax();

    int interestingRoots = 0;
    for (int i = 0; i < roots; i++)
    {
      if (dst[i] < tMin || dst[i] > tMax)
        continue;
      dst[interestingRoots++] = dst[i];
    }
    return interestingRoots;
  }

  return roots;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_solvePolynomialAnalytic(void)
{
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_NONE     ] = MathT_solvePolynomialA_Constant <float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_LINEAR   ] = MathT_solvePolynomialA_Linear   <float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUADRATIC] = MathT_solvePolynomialA_Quadratic<float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_CUBIC    ] = MathT_solvePolynomialA_Cubic    <float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUARTIC  ] = MathT_solvePolynomialA_Quartic  <float>;

  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_NONE     ] = MathT_solvePolynomialA_Constant <double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_LINEAR   ] = MathT_solvePolynomialA_Linear   <double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUADRATIC] = MathT_solvePolynomialA_Quadratic<double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_CUBIC    ] = MathT_solvePolynomialA_Cubic    <double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUARTIC  ] = MathT_solvePolynomialA_Quartic  <double>;
}

} // Fog namespace
