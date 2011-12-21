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
// [Fog::_StaticTypeForSize]
// ============================================================================

//! @internal
//!
//! @brief Helper template for Static<Type> to determine the correct type which
//! should be used with the template.
//!
//! There is an interesting problem related to @c Static<Type>, and it is that
//! there is basically no alignment control - this means that the Static<Type>
//! can have different alignment than Type, and we need to avoid such issue (
//! because some hardware doesn't allow to access unaligned memory).
template<size_t SizeT>
struct _StaticTypeForSize
{
  typedef intptr_t Type;
};

template<> struct _StaticTypeForSize<1> { typedef uint8_t  Type; };
template<> struct _StaticTypeForSize<2> { typedef uint16_t Type; };
template<> struct _StaticTypeForSize<3> { typedef uint32_t Type; };
template<> struct _StaticTypeForSize<4> { typedef uint32_t Type; };

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
template<typename TypeT>
struct Static
{
  // --------------------------------------------------------------------------
  // [Init / Destroy]
  // --------------------------------------------------------------------------

  //! @brief Initializer (calls placement @c new operator).
  FOG_INLINE TypeT* init()
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) TypeT;
  }

  //! @brief Initializer with copy assignment (calls placement @c new operator).
  FOG_INLINE TypeT* init(const Static<TypeT>& t)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) TypeT(t());
  }

  //! @brief Initializer with copy assignment (calls placement @c new operator).
  FOG_INLINE TypeT* init(const TypeT& t)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) TypeT(t);
  }

  template<typename C1>
  FOG_INLINE TypeT* initCustom1(C1 t1)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) TypeT(t1);
  }

  template<typename C1, typename C2>
  FOG_INLINE TypeT* initCustom2(C1 t1, C2 t2)
  {
    return fog_new_p(reinterpret_cast<void*>(_storage)) TypeT(t1, t2);
  }

  //! @brief Deinitializer (calls placement @c delete operator).
  FOG_INLINE void destroy()
  {
    p()->~TypeT();
  }

  // --------------------------------------------------------------------------
  // [Instance]
  // --------------------------------------------------------------------------

  //! @brief Returns pointer to instance of @c TypeT.
  FOG_INLINE TypeT* p() { return reinterpret_cast<TypeT*>(_storage); }
  //! @brief Returns const pointer to instance of @c TypeT.
  FOG_INLINE const TypeT* p() const { return reinterpret_cast<const TypeT*>(_storage); }

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  //! @brief Overridden TypeT& operator.
  FOG_INLINE operator TypeT&() { return *p(); }
  //! @brief Overridden const TypeT& operator.
  FOG_INLINE operator const TypeT&() const { return *p(); }

  //! @brief Overridden operator().
  FOG_INLINE TypeT& operator()() { return *p(); }
  //! @brief Overridden operator().
  FOG_INLINE const TypeT& operator()() const { return *p(); }

  //! @brief Overridden operator&().
  FOG_INLINE TypeT* operator&() { return p(); }
  //! @brief Overridden operator&().
  FOG_INLINE const TypeT* operator&() const { return p(); }

  //! @brief Overridden -> operator.
  FOG_INLINE TypeT* operator->() { return p(); }
  //! @brief Overridden const -> operator.
  FOG_INLINE TypeT const* operator->() const { return p(); }

private:
  //! @brief Type determined to use for _storage.
  typedef typename _StaticTypeForSize<sizeof(TypeT)>::Type StorageType;

  //! @brief Stack based storage.
  //!
  //! Because there is no aligment required by char, we decided to use
  //! StorageType for data unit. C++ compiler should automatically use
  //! the correct alignment for that type.
  StorageType _storage[(sizeof(TypeT) + sizeof(StorageType) - 1) / sizeof(StorageType)];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_STATIC_H
