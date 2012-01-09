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
// [Fog::SolvePolynomialN - Jenkins-Traub]
// ============================================================================

// The code adapted from:
//
//   rpoly.cpp -- Jenkins-Traub real polynomial root finder.
//   (C) 2002, C. Bond. All rights reserved.
//
// Translation of TOMS493 from FORTRAN to C. This implementation of Jenkins-Traub
// partially adapts the original code to a C environment by restruction many of 
// the 'goto' controls to better fit a block structured form. It also eliminates
// the global memory allocation in favor of local, dynamic memory management.
//
// Translated to use C++ by Petr Kobalicek <kobalicek.petr@gmail.com>

#define JT_BASE 2.0
#define JT_ETA 2.22e-16
#define JT_INF 3.4e38
#define JT_SMALL 1.2e-38

struct FOG_NO_EXPORT SolveJenkinsTraub
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SolveJenkinsTraub(const float* polynomial, int degree);
  SolveJenkinsTraub(const double* polynomial, int degree);
  bool init(int degree);

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  void quad(double a, double b1, double c, double* sr, double* si, double* lr, double* li);
  void fxshfr(int l2, int* nz);
  void quadit(double* uu, double* vv, int* nz);
  void realit(double sss, int* nz, int* iflag);
  void calcsc(int* type);
  void nextk(int* type);
  void newest(int type, double* uu, double* vv);
  void quadsd(int n, double* u, double* v, double* p, double* q, double* a, double* b);

  // --------------------------------------------------------------------------
  // [Solve]
  // --------------------------------------------------------------------------

  // Called by MathT_solvePolynomialJenkinsTraub().
  int solve();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  MemBufferTmp<2048> memBuffer;
  double* temp;
  double* pt;
  double* p;
  double* qp;
  double* k;
  double* qk;
  double* svk;
  double* zeror;
  double* zeroi;

  double sr, si, u, v, a, b, c, d, a1, a2;
  double a3, a6, a7, e, f, g, h, szr, szi, lzr, lzi;
  double are, mre;

  int degree, n, nn, nmi, zerok;
  int itercnt;
};

SolveJenkinsTraub::SolveJenkinsTraub(const float* polynomial, int degree)
{
  // Alloc memory and initialize pointers.
  if (!init(degree))
    return;

  // Copy the polynomial.
  for (int i = 0; i <= degree; i++)
    p[i] = double(polynomial[i]);
}

SolveJenkinsTraub::SolveJenkinsTraub(const double* polynomial, int degree)
{
  // Alloc memory and initialize pointers.
  if (!init(degree))
    return;

  // Copy the polynomial.
  for (int i = 0; i <= degree; i++)
    p[i] = polynomial[i];
}

bool SolveJenkinsTraub::init(int degree)
{
  temp = reinterpret_cast<double*>(memBuffer.alloc((degree + 1) * 9 * sizeof(double)));

  if (temp == NULL)
    return false;

  pt    = temp + (degree + 1);
  p     = pt   + (degree + 1);
  qp    = p    + (degree + 1);
  k     = qp   + (degree + 1);
  qk    = k    + (degree + 1);
  svk   = qk   + (degree + 1);
  zeror = svk  + (degree + 1);
  zeroi = zeror+ (degree + 1);

  this->n = degree;
  this->degree = degree;

  return true;
}

// Computes up to L2 fixed shift k-polynomials, testing for convergence in the
// linear or quadratic case. Initiates one of the variable shift iterations and
// returns with the number of zeros found.
void SolveJenkinsTraub::fxshfr(int l2, int* nz)
{
  double svu, svv, ui, vi, s;
  double betas, betav, oss, ovv, ss, vv, ts, tv;
  double ots, otv, tvv, tss;
  int type, i, j, iflag, vpass, spass, vtry, stry;

  *nz = 0;
  betav = 0.25;
  betas = 0.25;
  oss = sr;
  ovv = v;

  // Evaluate polynomial by synthetic division.
  quadsd(n,&u,&v,p,qp,&a,&b);
  calcsc(&type);

  for (j = 0; j < l2; j++)
  {
    // Calculate next k polynomial and estimate v.
    nextk(&type);
    calcsc(&type);
    newest(type,&ui,&vi);
    vv = vi;

    // Estimate s.
    ss = k[n-1] == 0.0 ? 0.0 : -p[n] / k[n - 1];
    tv = 1.0;
    ts = 1.0;
    if (j == 0 || type == 3)
      goto _70;
    
    // Compute relative measures of convergence of s and v sequences.
    if (vv != 0.0) tv = Math::abs((vv - ovv) / vv);
    if (ss != 0.0) ts = Math::abs((ss - oss) / ss);

    // If decreaMath::sing, multiply two most recent convergence measures.
    tvv = tv < otv ? tv * otv : 1.0;
    tss = ts < ots ? ts * ots : 1.0;

    // Compare with convergence criteria.
    vpass = (tvv < betav);
    spass = (tss < betas);
    if (!(spass || vpass))
      goto _70;

    // At least one sequence has passed the convergence test. Store variables 
    // before iterating.
    svu = u;
    svv = v;
    for (i = 0; i < n; i++)
      svk[i] = k[i];
    s = ss;

    // Choose iteration according to the fastest converging sequence.
    vtry = 0;
    stry = 0;
    if (spass && (!vpass || tss < tvv))
      goto _40;

_20:
    quadit(&ui, &vi, nz);
    if (*nz > 0)
      return;

    // Quadratic iteration has failed. Flag that it has been tried and decrease 
    // the convergence criterion.
    vtry = 1;
    betav *= 0.25;

    // Try linear iteration if it has not been tried and the S sequence is converging.
    if (stry || !spass) goto _50;
    for (i = 0; i < n; i++)
      k[i] = svk[i];

_40:
    realit(s, nz, &iflag);
    if (*nz > 0)
      return;

    // Linear iteration has failed. Flag that it has been tried and decrease 
    // the convergence criterion.
    stry = 1;
    betas *=0.25;
    if (iflag == 0)
      goto _50;

    // If linear iteration signals an almost double real zero attempt quadratic 
    // iteration.
    ui = -(s+s);
    vi = s*s;
    goto _20;

    // Restore variables.
_50:
    u = svu;
    v = svv;
    for (i = 0; i < n; i++)
      k[i] = svk[i];

    // Try quadratic iteration if it has not been tried and the V sequence is
    // converging.
    if (vpass && !vtry)
      goto _20;

    // Recompute QP and scalar values to continue the second stage.
    quadsd(n, &u, &v, p, qp, &a, &b);
    calcsc(&type);

_70:
    ovv = vv;
    oss = ss;
    otv = tv;
    ots = ts;
  }
}

// Variable-shift k-polynomial iteration for a quadratic factor converges only
// if the zeros are equimodular or nearly so.
//
// uu, vv - coefficients of starting quadratic.
// nz     - number of zeros found.
void SolveJenkinsTraub::quadit(double* uu, double* vv, int* nz)
{
  double ui, vi;
  double mp, omp, ee, relstp, t, zm;
  int type, i, j, tried;

  *nz = 0;
  tried = 0;
  u = *uu;
  v = *vv;
  j = 0;

  // Main loop.
  for (;;)
  {
    itercnt++;
    quad(1.0, u, v, &szr, &szi, &lzr, &lzi);

    // Return if roots of the quadratic are real and not close to multiple or 
    // nearly equal and of opposite sign.
    if (Math::abs(Math::abs(szr)-Math::abs(lzr)) > 0.01 * Math::abs(lzr))
      return;

    // Evaluate polynomial by quadratic synthetic division.
    quadsd(n, &u, &v, p, qp, &a, &b);
    mp = Math::abs(a - szr * b) + Math::abs(szi * b);

    // Compute a rigorous bound on the rounding error in evaluating p.
    zm = Math::sqrt(Math::abs(v));
    ee = 2.0 * Math::abs(qp[0]);
    t = -szr*b;

    for (i = 1; i < n; i++)
      ee = ee * zm + Math::abs(qp[i]);

    ee = ee * zm + Math::abs(a+t);
    ee *= (5.0 * mre + 4.0 * are);
    ee = ee - (5.0 * mre + 2.0 * are) * (Math::abs(a + t) + Math::abs(b) * zm);
    ee = ee + 2.0*are*Math::abs(t);

    // Iteration has converged sufficiently if the polynomial value is less than
    // 20 times this bound.
    if (mp <= 20.0 * ee)
    {
      *nz = 2;
      return;
    }

    // Stop iteration after 20 steps.
    if (++j > 20)
      return;

    if (!(j < 2 || relstp > 0.01 || mp < omp || tried))
    {
      // A cluster appears to be stalling the convergence. Five fixed shift steps
      // are taken with a u,v close to the cluster.
      if (relstp < JT_ETA)
        relstp = JT_ETA;

      relstp = Math::sqrt(relstp);
      u = u - u*relstp;
      v = v + v*relstp;
      quadsd(n, &u, &v, p, qp, &a, &b);

      for (i = 0; i < 5; i++)
      {
        calcsc(&type);
        nextk(&type);
      }

      tried = 1;
      j = 0;
    }

    omp = mp;

    // Calculate next k polynomial and new u and v.
    calcsc(&type);
    nextk(&type);
    calcsc(&type);
    newest(type,&ui,&vi);

    // If vi is zero the iteration is not converging.
    if (vi == 0.0)
      return;

    relstp = Math::abs((vi - v) / vi);
    u = ui;
    v = vi;
  }
}

// Variable-shift H polynomial iteration for a real zero.
//
// sss - starting iterate
// nz  - number of zeros found
// iflag - flag to indicate a pair of zeros near real axis.
void SolveJenkinsTraub::realit(double sss, int* nz, int* iflag)
{
  double pv, kv, t, s;
  double ms, mp, omp, ee;
  int i, j;

  *nz = 0;
  *iflag = 0;

  s = sss;
  j = 0;

  for (;;)
  {
    itercnt++;
    pv = p[0];

    // Evaluate p at s.
    qp[0] = pv;

    for (i = 1; i <= n; i++)
    {
      pv = pv*s + p[i];
      qp[i] = pv;
    }
    mp = Math::abs(pv);

    // Compute a rigorous bound on the error in evaluating p.
    ms = Math::abs(s);
    ee = (mre / (are + mre)) * Math::abs(qp[0]);

    for (i = 1; i <= n; i++)
      ee = ee*ms + Math::abs(qp[i]);

    // Iteration has converged sufficiently if the polynomial value is less
    // than 20 times this bound.
    if (mp <= 20.0*((are+mre)*ee-mre*mp))
    {
      *nz = 1;
      szr = s;
      szi = 0.0;
      return;
    }
    j++;

    // Stop iteration after 10 steps.
    if (j > 10) return;
    if (j < 2) goto _50;
    if (Math::abs(t) > 0.001*Math::abs(s-t) || mp < omp) goto _50;
    
    // A cluster of zeros near the real axis has been encountered. Return with
    // iflag set to initiate a quadratic iteration.
    *iflag = 1;
    sss = s; 
    return;
    
_50:
    omp = mp;

    // Compute t, the next polynomial, and the new iterate.
    kv = k[0];
    qk[0] = kv;

    for (i = 1; i < n; i++)
    {
      kv = kv*s + k[i];
      qk[i] = kv;
    }

    // HVE n -> n-1
    if (Math::abs(kv) <= Math::abs(k[n-1])*10.0*JT_ETA)
    {
      // Use the unscaled form.
      k[0] = 0.0;
      for (i = 1; i < n; i++)
        k[i] = qk[i-1];
    }
    else
    {
      // Use the scaled form of the recurrence if the value of k at s is nonzero.
      t = -pv / kv;
      k[0] = qp[0];
      for (i = 1; i < n; i++)
        k[i] = t*qk[i-1] + qp[i];
    }

    kv = k[0];
    for (i = 1; i < n; i++)
      kv = kv*s + k[i];

    t = 0.0;
    if (Math::abs(kv) > Math::abs(k[n-1] * 10.0 * JT_ETA))
      t = -pv/kv;

    s += t;
  }
}

// This routine calculates scalar quantities used to compute the next k 
// polynomial and new estimates of the quadratic coefficients.
//
// type - integer variable set here indicating how the calculations are 
//        normalized to avoid overflow.
void SolveJenkinsTraub::calcsc(int* type)
{
  // Synthetic division of k by the quadratic 1, u, v.
  quadsd(n - 1, &u, &v, k, qk, &c, &d);

  if (Math::abs(c) > Math::abs(k[n-1] * 100.0 * JT_ETA)) goto _10;
  if (Math::abs(d) > Math::abs(k[n-2] * 100.0 * JT_ETA)) goto _10;

  // Type=3 indicates the quadratic is almost a factor of k.
  *type = 3;
  return;

_10:
  if (Math::abs(d) < Math::abs(c))
  {
    // Type=1 indicates that all formulas are divided by c.
    *type = 1;
    e = a / c;
    f = d / c;
    g = u * e;
    h = v * b;

    a3 = a * e + b * (h / c + g);
    a1 = b - a * (d / c);
    a7 = a + g * d + h * f;
    return;
  }
  else
  {
    // Type=2 indicates that all formulas are divided by d.
    *type = 2;
    e = a / d;
    f = c / d;
    g = u * b;
    h = v * b;

    a3 = (a + g) * e + h * (b / d);
    a1 = b * f - a;
    a7 = (f + u) * a + h;
  }
}

// Computes the next k polynomials using scalars computed in calcsc.
void SolveJenkinsTraub::nextk(int* type)
{
  double temp;
  int i;

  if (*type == 3)
  {
    // Use unscaled form of the recurrence if type is 3.
    k[0] = 0.0;
    k[1] = 0.0;

    for (i = 2; i < n; i++)
      k[i] = qk[i-2];
  }
  else
  {
    temp = a;
    if (*type == 1) temp = b;

    if (Math::abs(a1) <= Math::abs(temp) * 10.0 * JT_ETA)
    {
      // If a1 is nearly zero then use a special form of the recurrence.
      k[0] = 0.0;
      k[1] = -a7 * qp[0];

      for (i = 2; i < n; i++)
        k[i] = a3 * qk[i-2] - a7 * qp[i-1];
    }
    else
    {
      // Use scaled form of the recurrence.
      a7 /= a1;
      a3 /= a1;
      k[0] = qp[0];
      k[1] = qp[1] - a7*qp[0];

      for (i = 2; i < n; i++)
        k[i] = a3 * qk[i-2] - a7 * qp[i-1] + qp[i];
    }
  }
}

// Compute new estimates of the quadratic coefficients using the scalars 
// computed in calcsc.
void SolveJenkinsTraub::newest(int type, double* uu, double* vv)
{
  // Use formulas appropriate to setting of type.
  if (type == 3)
  {
    // If type=3 the quadratic is zeroed.
    *uu = 0.0;
    *vv = 0.0;
    return;
  }

  double a4, a5;
  if (type == 2)
  {
    a4 = (a + g) * f + h;
    a5 = (f + u) * c + v * d;
  }
  else
  {
    a4 = a + u * b + h * f;
    a5 = c + d * (u + v * f);
  }

  // Evaluate new quadratic coefficients.
  double b1 = -k[n-1] / p[n];
  double b2 = -(k[n-2] + b1 * p[n-1]) / p[n];
  double c1 = v * b2 * a1;
  double c2 = b1 * a7;
  double c3 = b1 * b1 * a3;
  double c4 = c1 - c2 - c3;

  double t = a5 + b1 * a4 - c4;
  if (t == 0.0)
  {
    *uu = 0.0;
    *vv = 0.0;
  }
  else
  {
    *uu = u - (u * (c3 + c2) + v * (b1 * a1 + b2 * a7)) / t;
    *vv = v * (1.0 + c4 / t);
  }
}

// Divides p by the quadratic 1,u,v placing the quotient in q and the remainder in a, b.
void SolveJenkinsTraub::quadsd(int nn, double* u, double* v, double* p, double* q, double* a, double* b)
{
  double c;
  int i;

  *b = p[0];
  q[0] = *b;
  *a = p[1] - (*b)*(*u);
  q[1] = *a;

  for (i = 2; i <= nn; i++)
  {
    c = p[i] - (*a)*(*u) - (*b)*(*v);
    q[i] = c;
    *b = *a;
    *a = c;
  }	
}

// Calculate the zeros of the quadratic a*z^2 + b1*z + c. The quadratic formula,
// modified to avoid overflow, is used to find the larger zero if the zeros are
// real and both are complex. The smaller real zero is found directly from  the
// product of the zeros c/a.
void SolveJenkinsTraub::quad(double a, double b1, double c, double* sr, double* si, double* lr, double* li)
{
  double b, d, e;

  if (a == 0.0)
  {
    // Less than two roots.
    if (b1 != 0.0)
      *sr = -c/b1;
    else
      *sr = 0.0;

    *lr = 0.0;
    *si = 0.0;
    *li = 0.0;
    return;
  }

  if (c == 0.0)
  {
    // one real root, one zero root.
    *sr = 0.0;
    *lr = -b1 / a;
    *si = 0.0;
    *li = 0.0;
    return;
  }

  // Compute discriminant avoiding overflow.
  b = b1 / 2.0;
  if (Math::abs(b) < Math::abs(c))
  {
    e = c >= 0.0 ? a : -a;
    e = b * (b / Math::abs(c)) - e;
    d = Math::sqrt(Math::abs(e)) * Math::sqrt(Math::abs(c));
  }
  else
  {
    e = 1.0 - (a/b)*(c/b);
    d = Math::sqrt(Math::abs(e)) * Math::abs(b);
  }

  if (e < 0.0)
  {
    // Complex conjugate zeros.
    *sr = -b / a;
    *lr = *sr;
    *si = Math::abs(d/a);
    *li = -(*si);
  }
  else
  {
    // Real zeros.
    if (b >= 0.0)
      d = -d;
    *lr = (d - b)/a;
    *sr = 0.0;

    if (*lr != 0.0)
      *sr = (c / *lr) / a;
    *si = 0.0;
    *li = 0.0;
  }
}

int SolveJenkinsTraub::solve()
{
  double t, aa, bb, cc, factor;
  double lo, max, min, xx, yy, cosr, sinr, xxx, x, sc, bnd;
  double xm, ff, df, dx;
  int cnt, nz, i, j, jj, l, nm1, zerok;

  // Algorithm fails of the leading coefficient is zero.
  FOG_ASSERT(p[0] != 0.0);
  FOG_ASSERT(n > 0);

  are = JT_ETA;
  mre = JT_ETA;
  lo = JT_SMALL / JT_ETA;

  // Initialization of constants for shift rotation.
  xx = MATH_SQRT_HALF;            // Math::sqrt(0.5).
  yy = -xx;                       //-Math::sqrt(0.5).
  sinr =  0.99756405025982424761; // Math::sin(94 * MATH_PI / 180).
  cosr = -0.06975647374412530078; // Math::cos(94 * MATH_PI / 180).

  // Start the algorithm for one zero.
_40:        
  itercnt = 0;

  if (n == 1)
  {
    zeror[degree-1] = -p[1] / p[0];
    zeroi[degree-1] = 0.0;
    n -= 1;
    goto _99;
  }
  
  // Calculate the final zero or pair of zeros.
  if (n == 2)
  {
    quad(p[0], p[1], p[2], &zeror[degree-2], &zeroi[degree-2], &zeror[degree-1], &zeroi[degree-1]);
    n -= 2;
    goto _99;
  }

  // Find largest and smallest moduli of coefficients.
  min = JT_INF;
  max = 0.0;

  for (i = 0; i <= n; i++)
  {
    x = Math::abs(p[i]);

    if (x > max)
      max = x;
    if (x != 0.0 && x < min)
      min = x;
  }

  // Scale if there are large or very small coefficients. Computes a scale 
  // factor to multiply the coefficients of the polynomial. The scaling is
  // done to avoid overflow and to avoid undetected underflow interfering
  // with the convergence criterion. The factor is a power of the JT_BASE.
  sc = lo / min;
  if (sc > 1.0 && max > JT_INF / sc)
    goto _110;

  if (sc <= 1.0)
  {
    if (max < 10.0)
      goto _110;

    if (sc == 0.0)
      sc = JT_SMALL;
  }

  // 1.44269504088896340736 == 1 / log(JT_BASE)
  l = Math::iround(1.44269504088896340736 * Math::log(sc));

  // Scale polynomial.
  factor = pow(JT_BASE, l);
  if (factor != 1.0)
  {
    for (i=0;i<=n;i++) 
      p[i] = factor*p[i];
  }

_110:
  // Compute lower bound on moduli of roots.
  for (i = 0; i <= n; i++)
    pt[i] = (Math::abs(p[i]));
  pt[n] = - pt[n];

  // Compute upper estimate of bound.
  x = Math::exp((Math::log(-pt[n]) - Math::log(pt[0])) / (double)n);

  // If Newton step at the origin is better, use it.
  if (pt[n - 1] != 0.0)
  {
    xm = -pt[n] / pt[n - 1];
    if (xm < x)  x = xm;
  }

  // Chop the interval (0,x) until ff <= 0.
  for (;;)
  {
    xm = x * 0.1;
    ff = pt[0];

    for (i = 1; i <= n; i++)
      ff = ff*xm + pt[i];

    if (ff <= 0.0)
      break;
    x = xm;
  }
  dx = x;

  // Do Newton interation until x converges to two decimal places. 
  while (Math::abs(dx/x) > 0.005)
  {
    ff = pt[0];
    df = ff;

    for (i = 1; i < n; i++)
    {
      ff = ff*x + pt[i];
      df = df*x + ff;
    }

    ff = ff*x + pt[n];
    dx = ff/df;
    x -= dx;
    itercnt++;
  }
  bnd = x;

  // Compute the derivative as the initial k polynomial and do 5 steps with 
  // no shift.
  nm1 = n - 1;

  for (i=1;i<n;i++)
    k[i] = (double)(n-i)*p[i]/(double)n;
  k[0] = p[0];

  aa = p[n];
  bb = p[n-1];
  zerok = (k[n-1] == 0);

  for (jj = 0; jj < 5; jj++)
  {
    itercnt++;
    cc = k[n-1];

    if (!zerok)
    {
      // Use a scaled form of recurrence if value of k at 0 is nonzero.
      t = -aa/cc;
      for (i=0;i<nm1;i++) {
        j = n-i-1;
        k[j] = t*k[j-1]+p[j];
      }
      k[0] = p[0];
      zerok = (Math::abs(k[n-1]) <= Math::abs(bb) * JT_ETA * 10.0);
    }
    else
    {
      // Use unscaled form of recurrence.
      for (i = 0; i < nm1; i++)
      {
        j = n-i-1;
        k[j] = k[j-1];
      }
      k[0] = 0.0;
      zerok = (k[n-1] == 0.0);
    }
  }

  // Save k for restarts with new shifts.
  for (i=0;i<n;i++) 
    temp[i] = k[i];

  // Loop to select the quadratic corresponding to each new shift.
  for (cnt = 0; cnt < 20; cnt++)
  {
    // Quadratic corresponds to a double shift to a non-real point and its
    // complex conjugate. The point has modulus bnd and amplitude rotated
    // by 94 degrees from the previous shift.
    xxx = cosr * xx - sinr * yy;
    yy = sinr * xx + cosr * yy;
    xx = xxx;
    sr = bnd * xx;
    si = bnd * yy;
    u = -2.0 * sr;
    v = bnd;
    fxshfr(20 * (cnt + 1), &nz);

    if (nz != 0)
    {
      // The second stage jumps directly to one of the third stage iterations
      // and returns here if successful. Deflate the polynomial, store the 
      // zero or zeros and return to the main algorithm.
      j = degree - n;
      zeror[j] = szr;
      zeroi[j] = szi;
      n -= nz;

      for (i = 0; i <= n; i++)
        p[i] = qp[i];

      if (nz != 1)
      {
        zeror[j+1] = lzr;
        zeroi[j+1] = lzi;
      }
      goto _40;
    }

    // If the iteration is unsuccessful, another quadratic is chosen after
    // restoring k.
    for (i = 0; i < n; i++)
      k[i] = temp[i];
  } 

  // Return with failure if no convergence after 20 shifts.
_99:
  return degree - n;
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

template<typename NumT>
static int MathT_solvePolynomialN_JenkinsTraub(NumT* dst, const NumT* polynomial, uint32_t degree, const NumT_(Interval)* interval)
{
  int i;

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

  SolveJenkinsTraub jenkinsTraub(polynomial, n);
  int roots = jenkinsTraub.solve();

  if (zeros)
    dst[roots++] = NumT(0.0);

  if (interval == NULL)
  {
    int interestingRoots = 0;
    for (i = 0; i < roots; i++)
    {
      if (Math::isFuzzyZero(jenkinsTraub.zeroi[i]))
      {
        NumT r = NumT(jenkinsTraub.zeror[i]);
        dst[interestingRoots++] = r;
      }
    }
    roots = interestingRoots;
  }
  else
  {
    NumT tMin = interval->getMin();
    NumT tMax = interval->getMax();

    int interestingRoots = 0;
    for (i = 0; i < roots; i++)
    {
      if (Math::isFuzzyZero(jenkinsTraub.zeroi[i]))
      {
        NumT r = NumT(jenkinsTraub.zeror[i]);
        if (r >= tMin && r <= tMax)
          dst[interestingRoots++] = r;
      }
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

FOG_NO_EXPORT void Math_init_solvePolynomialJenkinsTraub(void)
{
  fog_api.mathf_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_JENKINS_TRAUB] = MathT_solvePolynomialN_JenkinsTraub<float>;
  fog_api.mathd_solvePolynomialN[MATH_POLYNOMIAL_SOLVE_JENKINS_TRAUB] = MathT_solvePolynomialN_JenkinsTraub<double>;
}

} // Fog namespace
