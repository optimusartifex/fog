// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_CONSTANTS_H
#define _FOG_CORE_MATH_CONSTANTS_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

namespace Fog {

// ============================================================================
// [Fog::Math - Constants]
// ============================================================================

//! @addtogroup Fog_Core_Math
//! @{

//! @brief e.
static const double MATH_E = 2.7182818284590452354;

//! @brief log_2(e).
static const double MATH_LOG2E = 1.4426950408889634074;

//! @brief log_10(e).
static const double MATH_LOG10E = 0.43429448190325182765;

//! @brief log_e(2).
static const double MATH_LN2 = 0.69314718055994530942;

//! @brief log_e(10).
static const double MATH_LN10 = 2.30258509299404568402;

//! @brief Pi.
static const double MATH_PI = 3.14159265358979323846;

//! @brief pi / 2.
static const double MATH_PI_DIV_2 = 1.57079632679489661923;

//! @brief pi / 4.
static const double MATH_PI_DIV_4 = 0.78539816339744830962;

//! @brief 1 / pi.
static const double MATH_1_DIV_PI = 0.31830988618379067154;

//! @brief 2 / pi.
static const double MATH_2_DIV_PI = 0.63661977236758134308;

//! @brief 2 / sqrt(pi).
static const double MATH_2_DIV_SQRTPI = 1.12837916709551257390;

//! @brief sqrt(2).
static const double MATH_SQRT2 = 1.41421356237309504880;

//! @brief 1/sqrt(2).
static const double MATH_1_DIV_SQRT2 = 0.70710678118654752440;

//! @brief Default epsilon used in math for 32-bit floats.
static const float MATH_EPSILON_F = 1e-8f;

//! @brief Default epsilon used in math for 64-bit floats.
static const double MATH_EPSILON_D = 1e-14;

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_CONSTANTS_H
