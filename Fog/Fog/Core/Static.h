// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_STATIC_H
#define _FOG_CORE_STATIC_H

// [Dependencies]
#include <Fog/Core/Build.h>

namespace Fog {

//! @addtogroup Fog_Core_Other
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
//! // initialize class, call new Mutex
//! mutex.init();
//!
//! // do something, use -> operator or instance() method
//! mutex->lock();
//! mutex->unlock();
//!
//! mutex.instance().lock();
//! mutex.instance().unlock();
//!
//! // destroy class, call delete Mutex
//! mutex.destroy();
//! @endverbatim
//!
//! This template is very effective, because the memory is not allocated on the
//! heap, instead stack based allocation is used together with placement @c new
//! and @c delete operators.
template<typename Type>
struct Static
{
public:
  //! @brief Initializer (calls placement @c new operator).
  FOG_INLINE void init() { fog_new_p(reinterpret_cast<void*>(_storage)) Type; }
  //! @brief Initializer with copy assignment (calls placement @c new operator).
  FOG_INLINE void init(const Static<Type>& t) { fog_new_p(reinterpret_cast<void*>(_storage)) Type(t.instance()); }
  //! @brief Initializer with copy assignment (calls placement @c new operator).
  FOG_INLINE void init(const Type& t) { fog_new_p(reinterpret_cast<void*>(_storage)) Type(t); }

  template<typename C1>
  FOG_INLINE void initCustom1(C1 t1) { fog_new_p(reinterpret_cast<void*>(_storage)) Type(t1); }

  template<typename C1, typename C2>
  FOG_INLINE void initCustom2(C1 t1, C2 t2) { fog_new_p(reinterpret_cast<void*>(_storage)) Type(t1, t2); }

  //! @brief Deinitializer (calls placement @c delete operator).
  FOG_INLINE void destroy() { getStorage()->~Type(); }

  //! @brief Returns instance of @c Type.
  FOG_INLINE Type& instance() { return *getStorage(); }
  //! @brief Returns const instance of @c Type.
  FOG_INLINE const Type& instance() const { return *getStorage(); }

  //! @brief Returns pointer to instance of @c Type.
  FOG_INLINE Type* instancep() { return getStorage(); }
  //! @brief Returns const pointer to instance of @c Type.
  FOG_INLINE const Type* instancep() const { return getStorage(); }

  //! @brief Overriden Type* operator.
  FOG_INLINE operator Type*() { return getStorage(); }
  //! @brief Overriden const Type* operator.
  FOG_INLINE operator const Type*() const { return getStorage(); }

  //! @brief Overriden -> operator.
  FOG_INLINE Type* operator->() { return getStorage(); }
  //! @brief Overriden const -> operator.
  FOG_INLINE Type const* operator->() const { return getStorage(); }

  FOG_INLINE Type* getStorage() { return reinterpret_cast<Type*>(_storage); }
  FOG_INLINE const Type* getStorage() const { return reinterpret_cast<const Type*>(_storage); }

private:
  //! @brief Stack based storage.
  char _storage[sizeof(Type)];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_STATIC_H
