// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FLOATCONTROL_H
#define _FOG_CORE_MATH_FLOATCONTROL_H

// [Dependencies]
#include <Fog/Core/Config/Config.h>

#include <math.h>

#if defined(FOG_HAVE_FLOAT_H)
#include <float.h>
#endif // FOG_HAVE_FLOAT_H

// ============================================================================
// [Fog::Math - Control87]
// ============================================================================

#if defined(FOG_OS_WINDOWS) && defined(FOG_CC_MSC)
# define FOG_CONTROL87_BEGIN() uint _control87_old = _control87(0x9001F, FOG_ARCH_32_64(_MCW_DN|_MCW_EM|_MCW_RC, 0xFFFF))
# define FOG_CONTROL87_END() _control87(_control87_old, FOG_ARCH_32_64(_MCW_DN|_MCW_EM|_MCW_RC, 0xFFFF))
#elif defined(FOG_OS_WINDOWS)
# define FOG_CONTROL87_BEGIN() _control87(0, 0)
# define FOG_CONTROL87_END() _clear87()
#elif defined(FOG_HAVE_FENV_H)
# define FOG_CONTROL87_BEGIN() fenv_t _control87_env; feholdexcept(&_control87_env)
# define FOG_CONTROL87_END() fesetenv(&_control87_env)
#else
# define FOG_CONTROL87_BEGIN FOG_NOP
# define FOG_CONTROL87_END FOG_NOP
#endif

// [Guard]
#endif // _FOG_CORE_MATH_FLOATCONTROL_H
