// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Source/Gradient.h>

namespace Fog {

// ============================================================================
// [Fog::GradientF]
// ============================================================================

GradientF::GradientF(uint32_t gradientType) :
  _gradientType(gradientType),
  _gradientSpread(GRADIENT_SPREAD_DEFAULT)
{
  _pts[0].reset();
  _pts[1].reset();
  _pts[2].reset();
}

GradientF::GradientF(const GradientF& other) :
  _gradientType(other._gradientType),
  _gradientSpread(other._gradientSpread),
  _stops(other._stops)
{
  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];
}

GradientF::GradientF(const GradientD& other) :
  _gradientType(other._gradientType),
  _gradientSpread(other._gradientSpread),
  _stops(other._stops)
{
  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];
}

GradientF::~GradientF()
{
}

err_t GradientF::setGradient(const GradientF& other)
{
  _gradientType = other._gradientType;
  _gradientSpread = other._gradientSpread;
  _stops = other._stops;

  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];

  return ERR_OK;
}

err_t GradientF::setGradient(const GradientD& other)
{
  _gradientType = other._gradientType;
  _gradientSpread = other._gradientSpread;
  _stops = other._stops;

  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];

  return ERR_OK;
}

void GradientF::reset()
{
  _gradientType = GRADIENT_TYPE_INVALID;
  _gradientSpread = GRADIENT_SPREAD_DEFAULT;
  _stops.reset();

  _pts[0].reset();
  _pts[1].reset();
  _pts[2].reset();
}

// ============================================================================
// [Fog::GradientD]
// ============================================================================

GradientD::GradientD(uint32_t gradientType) :
  _gradientType(gradientType),
  _gradientSpread(GRADIENT_SPREAD_DEFAULT)
{
  _pts[0].reset();
  _pts[1].reset();
  _pts[2].reset();
}

GradientD::GradientD(const GradientF& other) :
  _gradientType(other._gradientType),
  _gradientSpread(other._gradientSpread),
  _stops(other._stops)
{
  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];
}

GradientD::GradientD(const GradientD& other) :
  _gradientType(other._gradientType),
  _gradientSpread(other._gradientSpread),
  _stops(other._stops)
{
  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];
}

GradientD::~GradientD()
{
}

err_t GradientD::setGradient(const GradientF& other)
{
  _gradientType = other._gradientType;
  _gradientSpread = other._gradientSpread;
  _stops = other._stops;

  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];

  return ERR_OK;
}

err_t GradientD::setGradient(const GradientD& other)
{
  _gradientType = other._gradientType;
  _gradientSpread = other._gradientSpread;
  _stops = other._stops;

  _pts[0] = other._pts[0];
  _pts[1] = other._pts[1];
  _pts[2] = other._pts[2];

  return ERR_OK;
}

void GradientD::reset()
{
  _gradientType = GRADIENT_TYPE_INVALID;
  _gradientSpread = GRADIENT_SPREAD_DEFAULT;
  _stops.reset();

  _pts[0].reset();
  _pts[1].reset();
  _pts[2].reset();
}

} // Fog namespace
