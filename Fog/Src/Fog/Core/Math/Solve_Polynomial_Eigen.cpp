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

// Inject root into array.
template<typename NumT>
static int MathT_injectRoot(NumT* arr, int n, NumT value)
{
  int i, j;

  for (i = 0; i < n; i++)
  {
    if (arr[i] < NumT(0.0))
      continue;
    if (arr[i] == NumT(0.0))
      return n;
    break;
  }

  for (j = n; j != i; j++)
  {
    arr[j] = arr[j - 1];
  }

  arr[i] = value;
  return n + 1;
}

// Find all the roots of a polynomial with real coefficients, given the degree
// "degree" and the coefficients polynomial[0...degree]. The method is to construct
// an upper Hessenberg matrix whose eigenvalues are the desired roots, and then use
// the routines balance and hqr. Complex roots are ignored, real roots are 
// returned in dst.
template<typename NumT>
static int MathT_solvePolynomialN_Eigen(NumT* dst, const NumT* polynomial, uint32_t degree, const NumT_(Interval)* interval)
{
  int i, j;

  int n = (int)degree;
  int zeros = 0;

  // Decrease degree of polynomial if the highest degree coefficient is zero.
  if (n == 0)
    return 0;

  while (polynomial[0] == NumT(0.0))
  {
    polynomial++;
    if (--n <= 3)
      break;
  }

  // Remove the zeros at the origin, if any.
  if (n == 0)
    return 0;

  while (polynomial[n] == 0.0)
  {
    zeros++;
    if (--n <= 3)
      break;
  }

  // Use an analytic method if the degree was decreased to 3.
  if (n <= 3)
  {
    int roots = Math::solvePolynomial(dst, polynomial, n, interval);

    if (zeros != 0 && interval == NULL && interval->hasValue(NumT(0.0)))
      return MathT_injectRoot(dst, roots, NumT(0.0));
    else
      return roots;
  }

  // Limit the maximum polynomial degree.
  if (n > MATH_POLYNOMIAL_DEGREE_MAX_NUMERIC)
    return 0;

  MemBufferTmp<10 * 10 * sizeof(double)> matStorage;
  double* mat = reinterpret_cast<double*>(matStorage.alloc(n * n * sizeof(double)));
  double norm = double(-polynomial[0]);

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

  if (zeros)
    dst[roots++] = NumT(0.0);

  if (interval != NULL)
  {
    NumT tMin = interval->getMin();
    NumT tMax = interval->getMax();

    int interestingRoots = 0;
    for (i = 0; i < roots; i++)
    {
      if (dst[i] >= tMin && dst[i] <= tMax)
        dst[interestingRoots++] = NumT(dst[i]);
    }
    roots = interestingRoots;
  }

  if (roots > 1)
    Algorithm::qsort_t<NumT>(dst, roots);

  return roots;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_solvePolynomialEigen(void)
{
  fog_api.mathf_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_EIGEN] = MathT_solvePolynomialN_Eigen<float>;
  fog_api.mathd_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_EIGEN] = MathT_solvePolynomialN_Eigen<double>;
}

} // Fog namespace
