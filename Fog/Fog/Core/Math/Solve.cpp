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
#include <Fog/Core/Memory/MemBufferTmp_p.h>
#include <Fog/Core/Tools/Algorithm.h>
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

    if (Math::isFuzzyPositiveZero(d))
    {
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
// [Fog::SolvePolynomialN - Eigen]
// ============================================================================

// Solve a Nth order polynomial function using eigenvalue method, see
//   "Numerical Recipes in C, The Art of Scientific Computing, Second Edition".

#define MAT(_Row_, _Col_) mat[(_Row_) * n + (_Col_)]

static void MathT_solvePolynomialN_Eigen_balance(double* mat, int n)
{
  for (;;)
  {
    bool last = true;
    for (int i = 0; i < n; i++)
    {
      // Calculate row and column norms.
      int j;
      double r = 0.0;
      double c = 0.0;
      
      for (j = 0; j < n; j++)
      {
        if (j != i)
        {
          c += Math::abs(MAT(j, i));
          r += Math::abs(MAT(i, j));
        }
      }

      if (c != 0.0 && r != 0.0)
      {
        double f = 1.0;
        double g = r / 2.0;
        double s = c + r;

        while (c < g)
        {
          f *= 2.0;
          c *= 4.0;
        }

        g = r * 2.0;
        while (c > g)
        {
          f /= 2.0;
          c /= 4.0;
        }

        if ((c + r) / f < 0.95 * s)
        {
          last = false;
          g = 1.0 / f;

          for (j = 0; j < n; j++) MAT(i, j) *= g;
          for (j = 0; j < n; j++) MAT(j, i) *= f;
        }
      }
    }

    if (last)
      break;
  }
}

// Finds all eigenvalues of an upper Hessenberg matrix a[1..n][1..n]. On input
// a can be exactly as output from elmhes §11.5; on output it is destroyed.
// The real and imaginary parts of the eigenvalues are returned in dst.
template<typename NumT>
static int MathT_solvePolynomialN_Eigen_hqr(NumT* dst, double* mat, int n)
{
  int roots = 0;
  int i, j, k, l, m;
  int nn;

  double x, y, z;
  double p, q, r, s;
  double v, w, u, t;
  double norm = 0.0;

  for (i = 0; i < n; i++)
  {
    for (j = Math::max(i - 1, 0); j < n; j++)
    {
      norm += Math::abs(MAT(i, j));
    }
  }

  // Gets changed only by an exceptional shift.
  t = 0.0;
  nn = n - 1;

  while (nn >= 0)
  { 
    // Begin search for next eigenvalue.
    int its = 0;
    do {
      for (l = nn; l >= 1; l--)
      {
        // Begin iteration: look for single small subdiagonal element.
        s = Math::abs(MAT(l - 1, l - 1)) + Math::abs(MAT(l, l));

        if (s == 0.0)
          s = norm;

        if ((Math::abs(MAT(l, l - 1)) + s) == s)
        {
          MAT(l, l - 1) = 0.0;
          break;
        }
      }

      x = MAT(nn, nn);
      
      if (l == nn)
      {
        // One root found.
        dst[roots++] = NumT(x + t);
        nn--;
      }
      else
      {
        y = MAT(nn - 1, nn - 1);
        w = MAT(nn    , nn - 1) * MAT(nn - 1, nn);
        
        if (l == (nn - 1))
        {
          // Two roots found...
          p = 0.5 * (y - x);
          q = p * p + w;
          x += t;
          
          if (q >= 0.0)
          {
            z = Math::sqrt(q);
            if (p < 0.0)
              z = -z;
            z += p;
            
            dst[roots++] = NumT(x + z);
            if (z != 0.0)
              dst[roots++] = NumT(x - w / z);
          }
          nn -= 2;
        }
        else
        {
          // No roots found. Continue iteration.
          if (its == 30)
            return roots;
          
          if (its == 10 || its == 20)
          {
            // Form exceptional shift.
            t += x;
            for (i = 0; i <= nn; i++)
              MAT(i, i) -= x;
            
            s = Math::abs(MAT(nn, nn - 1)) + Math::abs(MAT(nn - 1, nn - 2));
            x = 0.75 * s;
            y = x;
            w = -0.4375 * Math::pow2(s);
          }
          
          its++;
          
          for (m = nn - 2; m >= l; m--)
          {
            // Form shift and then look for 2 consecutive small subdiagonal elements.
            z = MAT(m, m);
            r = x - z;
            s = y - z;
            p = (r * s - w) / MAT(m + 1, m) + MAT(m, m + 1);
            
            // Equation (11.6.23).
            q = MAT(m + 1, m + 1) - z - r - s;
            r = MAT(m + 2, m + 1);
            
            // Scale to prevent overflow or underflow.
            s = Math::abs(p) + Math::abs(q) + Math::abs(r);
            p /= s;
            q /= s;
            r /= s;
            
            if (m == l)
              break;
            
            u = (Math::abs(MAT(m    , m - 1))) * (Math::abs(q) + Math::abs(r));
            v = (Math::abs(MAT(m - 1, m - 1)) + Math::abs(z) + Math::abs(MAT(m + 1, m + 1))) * Math::abs(p);
            
            // Equation (11.6.26).
            if (u + v == v)
              break;
          }
          
          for (i = m + 2; i <= nn; i++)
          {
            MAT(i, i - 2) = 0.0;
            if (i != m + 2)
              MAT(i, i - 3) = 0.0;
          }
          
          for (k = m; k <= nn - 1; k++)
          {
            // Double QR step on rows 0 to nn and columns m to nn.
            if (k != m)
            {
              // Begin setup of Householder vector.
              p = MAT(k    , k - 1);
              q = MAT(k + 1, k - 1);
              r = 0.0;
              
              if (k != nn - 1)
                r = MAT(k + 2, k - 1);
              
              x = Math::abs(p) + Math::abs(q) + Math::abs(r);
              if (x != 0.0)
              {
                // Scale to prevent overflow or underflow.
                p /= x;
                q /= x;
                r /= x;
              }
            }
            
            s = Math::sqrt(p * p + q * q + r * r);
            if (p < 0.0)
              s = -s;

            if (s != 0.0)
            {
              if (k == m)
              {
                if (l != m)
                  MAT(k, k - 1) = -MAT(k, k - 1);
              }
              else
              {
                MAT(k, k - 1) = -s * x;
              }
              
              // Equations (11.6.24).
              p += s;
              x = p / s;
              y = q / s;
              z = r / s;
              q /= p;
              r /= p;
              
              // Row modification.
              for (j = k; j <= nn; j++)
              {
                p = MAT(k, j) + q * MAT(k + 1, j);
                
                if (k != (nn-1))
                {
                  p += r * MAT(k + 2, j);
                  MAT(k + 2, j) -= p * z;
                }
                
                MAT(k + 1, j) -= p * y;
                MAT(k    , j) -= p * x;
              }
              
              // Column modification.
              int mmin = nn < k + 3 ? nn : k + 3;
              for (i = l; i <= mmin; i++)
              {
                p = x * MAT(i, k) + y * MAT(i, k + 1);

                if (k != (nn - 1))
                {
                  p += z * MAT(i, k + 2);
                  MAT(i, k + 2) -= p * r;
                }
                
                MAT(i, k + 1) -= p * q;
                MAT(i, k    ) -= p;
              }
            }
          }
        }
      }
    } while (l < nn - 1);
  }

  return roots;
}

// Find all the roots of a polynomial with real coefficients, given the degree
// "degree" and the coefficients mat[0...degree]. The method is to construct an
// upper Hessenberg matrix whose eigenvalues are the desired roots, and then use
// the routines balance and hqr. Complex roots are ignored, real roots are 
// returned in dst.
template<typename NumT>
static int MathT_solvePolynomialN_Eigen(NumT* dst, const NumT* polynomial, uint32_t degree, const NumT_(Interval)* interval)
{
  int n = (int)degree;
  if (n == 0)
    return 0;

  while (polynomial[0] == NumT(0.0))
  {
    polynomial++;
    if (--n <= 3)
      break;
  }

  if (n <= 3)
    return Math::solvePolynomial(dst, polynomial, n, interval);

  if (n > MATH_POLYNOMIAL_DEGREE_MAX_NUMERIC)
    return 0;

  MemBufferTmp<10 * 10 * sizeof(double)> matStorage;
  double* mat = reinterpret_cast<double*>(matStorage.alloc(n * n * sizeof(double)));
  double norm = double(-polynomial[0]);

  int i, j;

  // Construct the matrix.
  for (i = 0; i < n; i++)
  {
    MAT(0, i) = double(polynomial[i + 1]) / norm;

    for (j = 1; j < n; j++)
      MAT(j, i) = 0.0;

    if (i != n - 1)
      MAT(i + 1, i) = 1.0;
  }

  // Find its eigenvalues.
  MathT_solvePolynomialN_Eigen_balance(mat, n);
  int roots = MathT_solvePolynomialN_Eigen_hqr<NumT>(dst, mat, n);

  int interestingRoots = 0;
  for (i = 0; i < roots; i++)
  {
    if (dst[i] >= NumT(0.0) && dst[i] <= NumT(1.0))
      dst[interestingRoots++] = NumT(dst[i]);
  }
  roots = interestingRoots;

  Algorithm::isort_t<NumT>(dst, roots);
  return roots;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_solve(void)
{
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_NONE     ] = MathT_solvePolynomialA_Constant <float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_LINEAR   ] = MathT_solvePolynomialA_Linear   <float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUADRATIC] = MathT_solvePolynomialA_Quadratic<float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_CUBIC    ] = MathT_solvePolynomialA_Cubic    <float>;
  fog_api.mathf_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUARTIC  ] = MathT_solvePolynomialA_Quartic  <float>;
  fog_api.mathf_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_EIGEN     ] = MathT_solvePolynomialN_Eigen    <float>;

  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_NONE     ] = MathT_solvePolynomialA_Constant <double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_LINEAR   ] = MathT_solvePolynomialA_Linear   <double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUADRATIC] = MathT_solvePolynomialA_Quadratic<double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_CUBIC    ] = MathT_solvePolynomialA_Cubic    <double>;
  fog_api.mathd_solvePolynomialA[MATH_POLYNOMIAL_DEGREE_QUARTIC  ] = MathT_solvePolynomialA_Quartic  <double>;
  fog_api.mathd_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_EIGEN     ] = MathT_solvePolynomialN_Eigen    <double>;
}

} // Fog namespace
