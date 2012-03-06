// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_CPP_INTRINSSE3_H
#define _FOG_CORE_CPP_INTRINSSE3_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>
#include <Fog/Core/C++/IntrinSse2.h>

#if defined(_MSC_VER)
# include <intrin.h>
#else
# include <pmmintrin.h>
#endif

// [Guard]
#endif // _FOG_CORE_CPP_INTRINSSE3_H
