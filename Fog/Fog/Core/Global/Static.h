// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_STATIC_H
#define _FOG_CORE_GLOBAL_STATIC_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [Fog::Static]
// ============================================================================

//! @brief Simple template that can create class using placement new and delete.
//!
//! This template should be used in situations where compiler can't guarante
//! order of static instantiation and you want full control about it.
//!
//! Example:
//! @verbatim
//! Fog::Static<Fog::Mutex> mutex;
//!
//! // Initialize instance, calls Mutex constructor internally.
//! mutex.init();
//!
//! // Do something, use -> overloaded operator() method.
//! mutex().lock();
//! mutex().unlock();
//!
//! // Destroy instance, calls Mutex destructor internally.
//! mutex.destroy();
//! @endverbatim
//!
//! This template is very efficient, because the memory is not allocated on the
//! heap, instead stack based allocation is used together with placement @c new
//! and @c delete operators.
template<typename Type>
struct Static
{
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  //! @brief Initializer (calls placement @c new operator).
  FOG_INLINE Type* init()
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) Type;
  }

  //! @brief Initializer with copy assignment (calls placement @c new operator).
  FOG_INLINE Type* init(const Static<Type>& t)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) Type(t());
  }

  //! @brief Initializer with copy assignment (calls placement @c new operator).
  FOG_INLINE Type* init(const Type& t)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) Type(t);
  }

  template<typename C1>
  FOG_INLINE Type* initCustom1(C1 t1)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) Type(t1);
  }

  template<typename C1, typename C2>
  FOG_INLINE Type* initCustom2(C1 t1, C2 t2)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) Type(t1, t2);
  }

  //! @brief Deinitializer (calls placement @c delete operator).
  FOG_INLINE void destroy()
  {
    p()->~Type();
  }

  // --------------------------------------------------------------------------
  // [Instance]
  // --------------------------------------------------------------------------

  //! @brief Returns pointer to instance of @c Type.
  FOG_INLINE Type* p() { return reinterpret_cast<Type*>(_storage); }
  //! @brief Returns const pointer to instance of @c Type.
  FOG_INLINE const Type* p() const { return reinterpret_cast<const Type*>(_storage); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Overridden Type& operator.
  FOG_INLINE operator Type&() { return *p(); }
  //! @brief Overridden const Type& operator.
  FOG_INLINE operator const Type&() const { return *p(); }

  //! @brief Overridden operator().
  FOG_INLINE Type& operator()() { return *p(); }
  //! @brief Overridden operator().
  FOG_INLINE const Type& operator()() const { return *p(); }

  //! @brief Overridden operator&().
  FOG_INLINE Type* operator&() { return p(); }
  //! @brief Overridden operator&().
  FOG_INLINE const Type* operator&() const { return p(); }

  //! @brief Overridden -> operator.
  FOG_INLINE Type* operator->() { return p(); }
  //! @brief Overridden const -> operator.
  FOG_INLINE Type const* operator->() const { return p(); }

private:
  //! @brief Stack based storage.
  char _storage[sizeof(Type)];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_STATIC_H
