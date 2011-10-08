// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Global/Private.h>
#include <Fog/Core/Math/Function.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Interval.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/Solve.h>

namespace Fog {

// ============================================================================
// [Fog::Math - Integrate - Gauss]
// ============================================================================

template<typename NumT>
static err_t FOG_CDECL MathT_integrate_GaussLegendre(NumT* dst, const NumT_(Function)* f, const NumT_(Interval)* interval, uint32_t steps)
{
  if (!interval->isValid())
    return ERR_RT_INVALID_ARGUMENT;

  if (steps == 0)
    return ERR_RT_INVALID_ARGUMENT;

  static const double x[] =
  {
    0.1488743389816312108848260,
    0.4333953941292471907992659,
    0.6794095682990244062343274,
    0.8650633666889845107320967,
    0.9739065285171717200779640
  };

  static const double w[] =
  {
    0.2955242247147528701738930,
    0.2692667193099963550912269,
    0.2190863625159820439955349,
    0.1494513491505805931457763,
    0.0666713443086881375935688
  };

  double a = interval->getMin();
  double b = interval->getMax();
  double advance = (b - a) / double(steps);
  double result = 0.0;

  b = a + advance;

  steps--;

  for (uint step = 0; step <= steps; step++)
  {
    // There is some small error when incrementing advance to 'b', if
    // this is the last step then it's good to use the 'b' from the
    // given interval.
    if (step == steps)
      b = interval->getMax();

    double xm = 0.5 * (b + a);
    double xr = 0.5 * (b - a);
    double sum = 0.0;

    for (uint i = 0; i < 5; i++)
    {
      double d = x[i] * xr;

      NumT r[2];
      FOG_RETURN_ON_ERROR(f->evaluate(&r[0], NumT(xm + d)));
      FOG_RETURN_ON_ERROR(f->evaluate(&r[1], NumT(xm - d)));

      sum += w[i] * (double(r[0]) + double(r[1]));
    }

    a += advance;
    b += advance;
    result += sum * xr;
  }

  *dst = NumT(result);
  return ERR_OK;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Math_init_integrate(void)
{
  _api.mathf_integrate[MATH_INTEGRATION_METHOD_GAUSS] = MathT_integrate_GaussLegendre<float>;
  _api.mathd_integrate[MATH_INTEGRATION_METHOD_GAUSS] = MathT_integrate_GaussLegendre<double>;
}

} // Fog namespace
