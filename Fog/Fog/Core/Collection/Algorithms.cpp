// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Collection/Algorithms.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Memory/Ops.h>

namespace Fog {
namespace Algorithms {

// ============================================================================
// [Fog::Algorithms::QSort]
// ============================================================================

template<size_t Size>
struct QSortCompare
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    return _cmp(_a, _b);
  }

  FOG_INLINE void _swap(void* _a, void* _b)
  {
    Memory::xchg_s<Size>(_a, _b);
  }

  CompareFn _cmp;
  enum { _size = Size };
};

template<size_t Size>
struct QSortCompareEx
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    return _cmp(_self, _a, _b);
  }

  FOG_INLINE void _swap(void* _a, void* _b)
  {
    Memory::xchg_s<Size>(_a, _b);
  }

  CompareEx _cmp;
  const void* _self;
  enum { _size = Size };
};

template<typename T>
struct QSortGeneric
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    return _cmp(_a, _b);
  }

  FOG_INLINE void _swap(void* _a, void* _b)
  {
    size_t i;
    T* a = reinterpret_cast<T*>(_a);
    T* b = reinterpret_cast<T*>(_b);

    for (i = 0; i < _size / sizeof(T); i++)
    {
      T tmp = a[i];
      a[i] = b[i];
      b[i] = tmp;
    }
  }

  CompareFn _cmp;
  size_t _size;
};

template<typename T>
struct QSortGenericEx
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    return _cmp(_self, _a, _b);
  }

  FOG_INLINE void _swap(void* _a, void* _b)
  {
    size_t i;
    T* a = reinterpret_cast<T*>(_a);
    T* b = reinterpret_cast<T*>(_b);

    for (i = 0; i < _size / sizeof(T); i++)
    {
      T tmp = a[i];
      a[i] = b[i];
      b[i] = tmp;
    }
  }

  CompareEx _cmp;
  const void* _self;
  size_t _size;
};

// ============================================================================
// [Fog::Algorithms::QSort - API]
// ============================================================================

void qsort(void* base, size_t nmemb, size_t size, CompareFn compar)
{
#define FOG_QSORT_DEFINE(_Size_) \
  case _Size_: \
  { \
    QSortCore< QSortCompare<_Size_> > context; \
    \
    context._cmp = compar; \
    context._sort(reinterpret_cast<uint8_t*>(base), nmemb); \
    break; \
  }

#define FOG_QSORT_GENERIC() \
  default: \
  { \
    if (((size_t)base & (sizeof(size_t)-1)) == 0 && size % sizeof(size_t) == 0) \
    { \
      QSortCore< QSortGeneric<size_t> > context; \
      \
      context._cmp = compar; \
      context._size = size; \
      \
      context._sort(reinterpret_cast<uint8_t*>(base), nmemb); \
    } \
    else \
    { \
      QSortCore< QSortGeneric<uint8_t> > context; \
      \
      context._cmp = compar; \
      context._size = size; \
      \
      context._sort(reinterpret_cast<uint8_t*>(base), nmemb); \
    } \
    break; \
  }

  switch (size)
  {
    FOG_QSORT_DEFINE(2)
    FOG_QSORT_DEFINE(4)
    FOG_QSORT_DEFINE(8)
    FOG_QSORT_DEFINE(12)
    FOG_QSORT_DEFINE(16)
    FOG_QSORT_DEFINE(24)
    FOG_QSORT_DEFINE(32)
    FOG_QSORT_GENERIC()
  }

#undef FOG_QSORT_GENERIC
#undef FOG_QSORT_DEFINE
}

void qsort(void* base, size_t nmemb, size_t size, CompareEx compar, const void* self)
{
#define FOG_QSORT_DEFINE(_Size_) \
  case _Size_: \
  { \
    QSortCore< QSortCompareEx<_Size_> > context; \
    \
    context._cmp = compar; \
    context._self = self; \
    context._sort(reinterpret_cast<uint8_t*>(base), nmemb); \
    break; \
  }

#define FOG_QSORT_GENERIC() \
  default: \
  { \
    if (((size_t)base & (sizeof(size_t)-1)) == 0 && size % sizeof(size_t) == 0) \
    { \
      QSortCore< QSortGenericEx<size_t> > context; \
      \
      context._cmp = compar; \
      context._self = self; \
      context._size = size; \
      \
      context._sort(reinterpret_cast<uint8_t*>(base), nmemb); \
    } \
    else \
    { \
      QSortCore< QSortGenericEx<uint8_t> > context; \
      \
      context._cmp = compar; \
      context._self = self; \
      context._size = size; \
      \
      context._sort(reinterpret_cast<uint8_t*>(base), nmemb); \
    } \
    break; \
  }

  switch (size)
  {
    FOG_QSORT_DEFINE(2)
    FOG_QSORT_DEFINE(4)
    FOG_QSORT_DEFINE(8)
    FOG_QSORT_DEFINE(12)
    FOG_QSORT_DEFINE(16)
    FOG_QSORT_DEFINE(24)
    FOG_QSORT_DEFINE(32)
    FOG_QSORT_GENERIC()
  }

#undef FOG_QSORT_GENERIC
#undef FOG_QSORT_DEFINE
}

// ============================================================================
// [Fog::Algorithms::BSearch]
// ============================================================================

struct BSearchGeneric
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    return _cmp(_a, _b);
  }

  CompareFn _cmp;
  size_t _size;
};

struct BSearchGenericEx
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    return _cmp(_self, _a, _b);
  }

  CompareEx _cmp;
  const void* _self;
  size_t _size;
};

const void* bsearch(const void* _base, size_t nmemb, size_t size, const void* key, CompareFn compar)
{
  BSearchCore<BSearchGeneric> context;

  context._cmp = compar;
  context._size = size;
  return context._search(
    reinterpret_cast<const uint8_t*>(_base), nmemb,
    reinterpret_cast<const uint8_t*>(key));
}

const void* bsearch(const void* _base, size_t nmemb, size_t size, const void* key, CompareEx compar, const void* self)
{
  BSearchCore<BSearchGenericEx> context;

  context._cmp = compar;
  context._self = self;
  context._size = size;
  return context._search(
    reinterpret_cast<const uint8_t*>(_base), nmemb,
    reinterpret_cast<const uint8_t*>(key));
}

} // Algorithms namespace
} // Fog namespace
