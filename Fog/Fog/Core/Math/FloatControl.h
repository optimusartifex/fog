// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_FLOATCONTROL_H
#define _FOG_CORE_MATH_FLOATCONTROL_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// [Dependencies - C]
#include <math.h>
#if defined(FOG_HAVE_FLOAT_H)
# include <float.h>
#endif // FOG_HAVE_FLOAT_H

// ============================================================================
// [Fog::Math - Control87]
// ============================================================================

// According to MSDN, the _control87() is x86 specific, while the _controlfp()
// is portable across more platforms.
//
// /---------------------------------------------------------------------------
// | _MCW_DN (0x03000000) - Denormal control
// |   - _DN_SAVE       (0x00000000)
// |   - _DN_FLUSH      (0x01000000)
// |
// | Default: Flush (ignored on x86/amd64).
// |
// |---------------------------------------------------------------------------
// |
// | _MCW_EM (0x0008001F) - Interrupt exception mask
// |   - _EM_INEXACT    (0x00000001)
// |   - _EM_UNDERFLOW  (0x00000002)
// |   - _EM_OVERFLOW   (0x00000004)
// |   - _EM_ZERODIVIDE (0x00000008)
// |   - _EM_INVALID    (0x00000010)
// |   - _EM_DENORMAL   (0x00080000)
// |
// | Default: No exceptions (all masked).
// |
// |---------------------------------------------------------------------------
// |
// | _MCW_IC (0x00040000) - Infinity control
// |   - _IC_PROJECTIVE (0x00000000)
// |   - _IC_AFFINE     (0x00040000)
// |
// | Default: Projective.
// |
// |---------------------------------------------------------------------------
// |
// | _MCW_RC (0x00000300) - Rounding control
// |   - _RC_NEAR       (0x00000000)
// |   - _RC_DOWN       (0x00000100)
// |   - _RC_UP         (0x00000200)
// |   - _RC_CHOP       (0x00000300)
// |
// | Default: Nearest.
// |
// |---------------------------------------------------------------------------
// |
// | _MCW_PC (0x00030000) - Precision control
// |   - _PC_64         (0x00000000)
// |   - _PC_53         (0x00010000)
// |   - _PC_24         (0x00020000)
// |
// | Default: 53 bits.
// |
// \---------------------------------------------------------------------------

#if defined(FOG_OS_WINDOWS) && defined(FOG_CC_MSC)
# define FOG_CONTROL87_BEGIN() uint _control87_old = _control87(0x9001F, _MCW_DN | _MCW_EM | _MCW_RC)
# define FOG_CONTROL87_END() _control87(_control87_old, _MCW_DN | _MCW_EM | _MCW_RC)
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
