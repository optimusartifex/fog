// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COLLECTION_ALGORITHMS_H
#define _FOG_CORE_COLLECTION_ALGORITHMS_H

// [Dependencies]
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Math/Math.h>

namespace Fog {
namespace Algorithms {

//! @addtogroup Fog_Core_Collection
//! @{

// ============================================================================
// [Function Prototypes]
// ============================================================================

typedef int (*CompareFn)(const void* a, const void* b);
typedef int (*CompareEx)(const void* self, const void* a, const void* b);

// ============================================================================
// [Fog::Algorithms::QSortCore<>]
// ============================================================================

template<typename T>
struct QSortCore : public T
{
  FOG_INLINE uint8_t* _med3(uint8_t* a, uint8_t* b, uint8_t* c);
  FOG_NO_INLINE void _sort(uint8_t* base, size_t nmemb);
};

template<typename T>
uint8_t* QSortCore<T>::_med3(uint8_t* a, uint8_t* b, uint8_t* c)
{
  int cmp_ab = T::_compare(a, b);
  int cmp_bc = T::_compare(b, c);

  return cmp_ab < 0 ? (cmp_bc < 0 ? b : (T::_compare(a, c) < 0 ? c : a))
                    : (cmp_bc > 0 ? b : (T::_compare(a, c) < 0 ? a : c));
}

template<typename T>
void QSortCore<T>::_sort(uint8_t* base, size_t nmemb)
{
  uint8_t* pa; uint8_t* pb; uint8_t* pc; uint8_t* pd;
  uint8_t* pl; uint8_t* pm; uint8_t* pn;

  sysint_t d, r;
  uint swapFlag;

_Repeat:
  swapFlag = 0;

  // Insertion sort.
  if (nmemb < 7)
  {
    for (pm = base + T::_size; pm < base + nmemb * T::_size; pm += T::_size)
      for (pl = pm; pl > base && T::_compare(pl - T::_size, pl) > 0; pl -= T::_size)
        T::_swap(pl, pl - T::_size);
    return;
  }

  pm = base + (nmemb >> 1) * T::_size;
  if (nmemb > 7)
  {
    pl = base;
    pn = base + (nmemb - 1) * T::_size;
    if (nmemb > 40)
    {
      d = (nmemb / 8) * T::_size;
      pl = _med3(pl        , pl + d, pl + 2 * d);
      pm = _med3(pm - d    , pm    , pm + d    );
      pn = _med3(pn - 2 * d, pn - d, pn        );
    }
    pm = _med3(pl, pm, pn);
  }

  T::_swap(base, pm);

  pa = pb = base + T::_size;
  pc = pd = base + (nmemb - 1) * T::_size;

  for (;;)
  {
    while (pb <= pc && (r = T::_compare(pb, base)) <= 0)
    {
      if (r == 0)
      {
        swapFlag = 1;
        T::_swap(pa, pb);
        pa += T::_size;
      }
      pb += T::_size;
    }

    while (pb <= pc && (r = T::_compare(pc, base)) >= 0)
    {
      if (r == 0)
      {
        swapFlag = 1;
        T::_swap(pc, pd);
        pd -= T::_size;
      }
      pc -= T::_size;
    }
    if (pb > pc) break;

    swapFlag = 1;
    T::_swap(pb, pc);
    pb += T::_size;
    pc -= T::_size;
  }

  if (swapFlag)
  {
    uint8_t* swpA;
    uint8_t* swpB;

    pn = base + nmemb * T::_size;

    // Step 1.
    r = Math::min((size_t)(pa - base), (size_t)(pb - pa));
    swpA = base;
    swpB = pb - r;

    for (;;)
    {
      while (r > 0)
      {
        T::_swap(swpA, swpB);
        swpA += T::_size;
        swpB += T::_size;
        r -= T::_size;
      }

      if (swapFlag == 0) break;
      swapFlag--;

      // Step 2.
      r = Math::min((sysint_t)(pd - pc), (sysint_t)(pn - pd) - (sysint_t)T::_size);
      swpA = pb;
      swpB = pn - r;
    }

    if ((r = (size_t)(pb - pa) > T::_size))
    {
      // Recurse.
      _sort(base, (size_t)r / T::_size);
    }

    if ((r = (size_t)(pd - pc) > T::_size))
    {
      // Iterate.
      base = pn - r;
      nmemb = r / T::_size;
      goto _Repeat;
    }
  }
  else
  {
    // Insertion sort.
    for (pm = base + T::_size; pm < base + nmemb * T::_size; pm += T::_size)
      for (pl = pm; pl > base && T::_compare(pl - T::_size, pl) > 0; pl -= T::_size)
        T::_swap(pl, pl - T::_size);
  }
}

// ============================================================================
// [Fog::Algorithms::QSortType<>]
// ============================================================================

template<typename T>
struct QSortType
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    const T* a = reinterpret_cast<const T*>(_a);
    const T* b = reinterpret_cast<const T*>(_b);

    return TypeCmp<T>::compare(a, b);
  }

  FOG_INLINE void _swap(void* _a, void* _b)
  {
    T* a = reinterpret_cast<T*>(_a);
    T* b = reinterpret_cast<T*>(_b);

    T t(*a);
    *a = *b;
    *b = t;
  }

  enum { _size = sizeof(T) };
};

// ============================================================================
// [Fog::Algorithms::BSearchCore<>]
// ============================================================================

template<typename T>
struct BSearchCore : public T
{
  FOG_NO_INLINE const uint8_t* _search(const uint8_t* base, size_t nmemb, const uint8_t* key);
};

template<typename T>
const uint8_t* BSearchCore<T>::_search(const uint8_t* base, size_t nmemb, const uint8_t* key)
{
  size_t lim;
  for (lim = nmemb; lim != 0; lim >>= 1)
  {
    const uint8_t* cur = base + (lim >> 1) * T::_size;
    int result = T::_compare(key, cur);

    if (result > 0)
    {
      base = cur + T::_size;
      lim--;
      continue;
    }
    else if (result < 0)
    {
      continue;
    }
    else
    {
      return cur;
    }
  }

  return NULL;
}

// ============================================================================
// [Fog::Algorithms::BSearchType<>]
// ============================================================================

template<typename T>
struct BSearchType
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    const T* a = reinterpret_cast<const T*>(_a);
    const T* b = reinterpret_cast<const T*>(_b);

    return TypeCmp<T>::compare(a, b);
  }

  enum { _size = sizeof(T) };
};

// ============================================================================
// [Fog::Algorithms::QSort]
// ============================================================================

FOG_API void qsort(void* base, size_t nmemb, size_t size, CompareFn compar);
FOG_API void qsort(void* base, size_t nmemb, size_t size, CompareEx compar, const void* self);

template<typename T>
static FOG_INLINE void qsort_t(T* base, size_t nmemb)
{
  QSortCore< QSortType<T> > context;
  context._sort(base, nmemb);
}

// ============================================================================
// [Fog::Algorithms::ISort]
// ============================================================================

template<typename T>
static FOG_INLINE void isort_t(T* base, size_t nmemb)
{
  T* pm;
  T* pl;

  for (pm = base + 1; pm < base + nmemb; pm++)
    for (pl = pm; pl > base && pl[-1] > pl[0]; pl--)
      swap(pl[-1], pl[0]);
}

// ============================================================================
// [Fog::Algorithms::BSearch]
// ============================================================================

FOG_API const void* bsearch(const void* base, size_t nmemb, size_t size, const void* key, CompareFn compar);
FOG_API const void* bsearch(const void* base, size_t nmemb, size_t size, const void* key, CompareEx compar, const void* self);

template<typename T>
const T* bsearch_t(const T* base, size_t nmemb, const T* key)
{
  BSearchCore< BSearchType<T> > context;

  return context._search(
    reinterpret_cast<const uint8_t*>(base), nmemb,
    reinterpret_cast<const uint8_t*>(key));
}

//! @}

} // Algorithms namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_COLLECTION_ALGORITHMS_H
