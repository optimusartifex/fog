// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_SEQUENCEINFO_H
#define _FOG_CORE_GLOBAL_SEQUENCEINFO_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>

namespace Fog {

// ===========================================================================
// [Fog::SequenceInfo - helpers
// ===========================================================================

// helpers
struct SequenceInfo_NoInitFree
{
  static FOG_INLINE void init(void* _dest, sysuint_t count) { FOG_UNUSED(_dest); FOG_UNUSED(count); }
  static FOG_INLINE void free(void* _dest, sysuint_t count) { FOG_UNUSED(_dest); FOG_UNUSED(count); }
};

template<sysuint_t N>
struct SequenceInfo_MemMove
{
  static FOG_INLINE void move(void* _dest, const void* _src, sysuint_t count)
  { memmove(_dest, _src, N*count); }
};

template<sysuint_t N>
struct SequenceInfo_MemCopy
{
  static FOG_INLINE void copy(void* _dest, const void* _src, sysuint_t count)
  { memcpy(_dest, _src, N*count); }
};

template<typename T>
struct SequenceInfo_TypeInitFree
{
  static FOG_NO_INLINE void init(void* _dest, sysuint_t count)
  {
    T* dest = (T*)_dest;
    T* end = dest + count;
    while (dest != end) fog_new_p(dest++) T;
  }

  static FOG_NO_INLINE void free(void* _dest, sysuint_t count)
  {
    T* dest = (T*)_dest;
    T* end = dest + count;
    while (dest != end) dest++->~T();
  }
};

template<typename T>
struct SequenceInfo_TypeMove
{
  static FOG_NO_INLINE void move(void* _dest, const void* _src, sysuint_t count)
  {
    T* dest =(T*)_dest;
    T* end = dest;
    T* src = (T*)_src;

    if (dest < src)
    {
      end += count;
      while (dest != end) { fog_new_p(dest++) T(*src); src++->~T(); }
    }
    else
    {
      dest += count;
      src += count;
      while (dest != end) { fog_new_p(--dest) T(*(--src)); src->~T(); }
    }
  }
};

template<typename T>
struct SequenceInfo_TypeCopy
{
  static FOG_NO_INLINE void copy(void* _dest, const void* _src, sysuint_t count)
  {
    T* dest = (T*)_dest;
    T* end = dest + count;
    T* src = (T*)_src;
    while (dest != end) fog_new_p(dest++) T(*src++);
  }
};

// Primitive types.

template<typename T>
struct SequenceInfo_Primitive :
  public SequenceInfo_NoInitFree,
  public SequenceInfo_MemMove<sizeof(T)>,
  public SequenceInfo_MemCopy<sizeof(T)>
{
};

// Moveable types.

template<typename T>
struct SequenceInfo_Moveable :
  public SequenceInfo_TypeInitFree<T>,
  public SequenceInfo_TypeCopy<T>,
  public SequenceInfo_MemMove<sizeof(T)>
{
};

// Class types.

template<typename T>
struct SequenceInfo_Complex :
  public SequenceInfo_TypeInitFree<T>,
  public SequenceInfo_TypeCopy<T>,
  public SequenceInfo_TypeMove<T>
{
};

// ===========================================================================
// [Fog::SequenceInfo - virtual table
// ===========================================================================

// Sequence info vtable
struct SequenceInfoVTable
{
  // [Function Prototypes]

  typedef void (*InitFn)(void* _dest, sysuint_t count);
  typedef void (*FreeFn)(void* _dest, sysuint_t count);
  typedef void (*MoveFn)(void* _dest, const void* _src, sysuint_t count);
  typedef void (*CopyFn)(void* _dest, const void* _src, sysuint_t count);

  // [Data]

  //! @brief Size of T
  sysuint_t typeSize;

  //! @brief Init function (constructor of T)
  //!
  //! Calls constructors to data storage.
  InitFn init;
  //! @brief Free function (destructor of T)
  //!
  //! Calls destructors to data storage.
  FreeFn free;
  //! @brief Move function
  //!
  //! Moves data from one location to another, locations can overlap.
  MoveFn move;
  //! @brief Copy function
  //!
  //! Copies data from one location to another, locations can't overlap.
  //! New class instance is created by this function, but old instance
  //! is not freed!. Use move() function to move data or copy combined
  //! with free() function.
  CopyFn copy;
};

// ===========================================================================
// [Fog::SequenceInfo
// ===========================================================================

// 0 = PrimitiveType
// 1 = MoveableType
// 2 = ClassType
template<typename T, uint __TypeInfo__>
struct SequenceInfo_Wrapper {};

template<typename T>
struct SequenceInfo_Wrapper<T, 0> : public SequenceInfo_Primitive<T> {};
template<typename T>
struct SequenceInfo_Wrapper<T, 1> : public SequenceInfo_Moveable<T> {};
template<typename T>
struct SequenceInfo_Wrapper<T, 2> : public SequenceInfo_Complex<T> {};

template<typename T>
struct SequenceInfo : public SequenceInfo_Wrapper<T, TypeInfo<T>::TYPE>
{
  //! @brief Returns virtual table for non-specialized templates (default).
  static const SequenceInfoVTable seqvtable;
};

template<typename T>
const SequenceInfoVTable SequenceInfo<T>::seqvtable =
{
  sizeof(T),
  SequenceInfo<T>::init,
  SequenceInfo<T>::free,
  SequenceInfo<T>::move,
  SequenceInfo<T>::copy
};

} // Fog namespace

// ===========================================================================
// [Fog::SequenceInfo - macros
// ===========================================================================

#define FOG_DECLARE_SEQUENCEINFO(__symbol__, __vtable__) \
namespace Fog { \
  template <> \
  struct SequenceInfo <__symbol__> : public SequenceInfo_Wrapper< __symbol__, TypeInfo<T>::TYPE > \
  { \
    static FOG_INLINE const SequenceInfoVTable* vtable() \
    { return reinterpret_cast<const SequenceInfoVTable *>(__vtable__); } \
  }; \
}

// [Guard]
#endif // _FOG_CORE_GLOBAL_SEQUENCEINFO_H
