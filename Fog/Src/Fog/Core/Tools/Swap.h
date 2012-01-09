// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_SWAP_H
#define _FOG_CORE_TOOLS_SWAP_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::Swap]
// ============================================================================

// Generic swap implementation.
template<typename Type, int IsMovable>
struct SwapHelper
{
  static FOG_INLINE void swap(Type& a, Type& b)
  {
    Type t(a);

    a = b;
    b = t;
  }
};

// Movable swap implementation (instead of creating new Type instance we can
// just swap bytes in memory.
template<typename Type>
struct SwapHelper<Type, 1>
{
  static FOG_INLINE void swap(Type& a, Type& b)
  {
    char buf[sizeof(Type)];

    MemOps::copy_t<Type>(reinterpret_cast<Type*>(buf), &a);
    MemOps::copy_t<Type>(&a, &b);
    MemOps::copy_t<Type>(&b, reinterpret_cast<Type*>(buf));
  }
};

// Implicit 'D' swap implementation.
template<typename Type>
struct SwapHelper<Type, 2>
{
  static FOG_INLINE void swap(Type& a, Type& b)
  {
    typename Type::_D* t = a._d;

    a._d = b._d;
    b._d = t;
  }
};

//! @brief Swap @a a and @a b.
template<typename Type>
FOG_INLINE void swap(Type& a, Type& b)
{
  SwapHelper< Type,
    TypeInfo<Type>::IS_IMPLICIT ? 2 :
    /*TypeInfo<Type>::IS_MOVABLE  ? 1 :*/ -1
  >::swap(a, b);
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_SWAP_H
