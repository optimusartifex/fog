// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_ALGORITHM_H
#define _FOG_CORE_TOOLS_ALGORITHM_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Tools/Swap.h>

namespace Fog {
namespace Algorithm {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::Algorithm::QSortImpl<TypeT>]
// ============================================================================

template<typename TypeT>
struct QSortImpl : public TypeT
{
  FOG_INLINE uint8_t* _med3(uint8_t* a, uint8_t* b, uint8_t* c);
  FOG_NO_INLINE void _sort(uint8_t* base, size_t nmemb);
};

template<typename TypeT>
uint8_t* QSortImpl<TypeT>::_med3(uint8_t* a, uint8_t* b, uint8_t* c)
{
  int cmp_ab = TypeT::_compare(a, b);
  int cmp_bc = TypeT::_compare(b, c);

  return cmp_ab < 0 ? (cmp_bc < 0 ? b : (TypeT::_compare(a, c) < 0 ? c : a))
                    : (cmp_bc > 0 ? b : (TypeT::_compare(a, c) < 0 ? a : c));
}

template<typename TypeT>
void QSortImpl<TypeT>::_sort(uint8_t* base, size_t nmemb)
{
  uint8_t* pa; uint8_t* pb; uint8_t* pc; uint8_t* pd;
  uint8_t* pl; uint8_t* pm; uint8_t* pn;

  ssize_t d, r;
  uint swapFlag;

_Repeat:
  swapFlag = 0;

  // Insertion sort.
  if (nmemb < 7)
  {
    for (pm = base + TypeT::_size; pm < base + nmemb * TypeT::_size; pm += TypeT::_size)
    {
      for (pl = pm; pl > base && TypeT::_compare(pl - TypeT::_size, pl) > 0; pl -= TypeT::_size)
      {
        TypeT::_swap(pl, pl - TypeT::_size);
      }
    }
    return;
  }

  pm = base + (nmemb >> 1) * TypeT::_size;
  if (nmemb > 7)
  {
    pl = base;
    pn = base + (nmemb - 1) * TypeT::_size;
    if (nmemb > 40)
    {
      d = (nmemb / 8) * TypeT::_size;
      pl = _med3(pl        , pl + d, pl + 2 * d);
      pm = _med3(pm - d    , pm    , pm + d    );
      pn = _med3(pn - 2 * d, pn - d, pn        );
    }
    pm = _med3(pl, pm, pn);
  }

  TypeT::_swap(base, pm);

  pa = pb = base + TypeT::_size;
  pc = pd = base + (nmemb - 1) * TypeT::_size;

  for (;;)
  {
    while (pb <= pc && (r = TypeT::_compare(pb, base)) <= 0)
    {
      if (r == 0)
      {
        swapFlag = 1;
        TypeT::_swap(pa, pb);
        pa += TypeT::_size;
      }
      pb += TypeT::_size;
    }

    while (pb <= pc && (r = TypeT::_compare(pc, base)) >= 0)
    {
      if (r == 0)
      {
        swapFlag = 1;
        TypeT::_swap(pc, pd);
        pd -= TypeT::_size;
      }
      pc -= TypeT::_size;
    }
    if (pb > pc) break;

    swapFlag = 1;
    TypeT::_swap(pb, pc);
    pb += TypeT::_size;
    pc -= TypeT::_size;
  }

  if (swapFlag)
  {
    uint8_t* swpA;
    uint8_t* swpB;

    pn = base + nmemb * TypeT::_size;

    // Step 1.
    r = Math::min((size_t)(pa - base), (size_t)(pb - pa));
    swpA = base;
    swpB = pb - r;

    for (;;)
    {
      while (r > 0)
      {
        TypeT::_swap(swpA, swpB);
        swpA += TypeT::_size;
        swpB += TypeT::_size;
        r -= TypeT::_size;
      }

      if (swapFlag == 0) break;
      swapFlag--;

      // Step 2.
      r = Math::min((ssize_t)(pd - pc), (ssize_t)(pn - pd) - (ssize_t)TypeT::_size);
      swpA = pb;
      swpB = pn - r;
    }

    if ((r = (size_t)(pb - pa) > TypeT::_size))
    {
      // Recurse.
      _sort(base, (size_t)r / TypeT::_size);
    }

    if ((r = (size_t)(pd - pc) > TypeT::_size))
    {
      // Iterate.
      base = pn - r;
      nmemb = r / TypeT::_size;
      goto _Repeat;
    }
  }
  else
  {
    // Insertion sort.
    for (pm = base + TypeT::_size; pm < base + nmemb * TypeT::_size; pm += TypeT::_size)
    {
      for (pl = pm; pl > base && TypeT::_compare(pl - TypeT::_size, pl) > 0; pl -= TypeT::_size)
      {
        TypeT::_swap(pl, pl - TypeT::_size);
      }
    }
  }
}

// ============================================================================
// [Fog::Algorithm::QSortType<TypeT>]
// ============================================================================

template<typename TypeT>
struct QSortType
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    const TypeT* a = reinterpret_cast<const TypeT*>(_a);
    const TypeT* b = reinterpret_cast<const TypeT*>(_b);

    return TypeFunc<TypeT>::compare(a, b);
  }

  FOG_INLINE void _swap(void* _a, void* _b)
  {
    TypeT* a = reinterpret_cast<TypeT*>(_a);
    TypeT* b = reinterpret_cast<TypeT*>(_b);

    TypeT t(*a);
    *a = *b;
    *b = t;
  }

  enum { _size = sizeof(TypeT) };
};

// ============================================================================
// [Fog::Algorithm::BSearchImpl<>]
// ============================================================================

template<typename TypeT>
struct BSearchImpl : public TypeT
{
  FOG_NO_INLINE const uint8_t* _search(const uint8_t* base, size_t nmemb, const uint8_t* key);
};

template<typename TypeT>
const uint8_t* BSearchImpl<TypeT>::_search(const uint8_t* base, size_t nmemb, const uint8_t* key)
{
  size_t lim;
  for (lim = nmemb; lim != 0; lim >>= 1)
  {
    const uint8_t* cur = base + (lim >> 1) * TypeT::_size;
    int result = TypeT::_compare(key, cur);

    if (result > 0)
    {
      base = cur + TypeT::_size;
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
// [Fog::Algorithm::BSearchType<TypeT>]
// ============================================================================

template<typename TypeT>
struct BSearchType
{
  FOG_INLINE int _compare(const void* _a, const void* _b) const
  {
    const TypeT* a = reinterpret_cast<const TypeT*>(_a);
    const TypeT* b = reinterpret_cast<const TypeT*>(_b);

    return TypeFunc<TypeT>::compare(a, b);
  }

  enum { _size = sizeof(TypeT) };
};

// ============================================================================
// [Fog::Algorithm::QSort]
// ============================================================================

FOG_API void qsort(void* base, size_t nmemb, size_t size, CompareFunc compareFunc);
FOG_API void qsort(void* base, size_t nmemb, size_t size, CompareExFunc compareFunc, const void* data);

template<typename TypeT>
static FOG_INLINE void qsort_t(TypeT* base, size_t nmemb)
{
  QSortImpl< QSortType<TypeT> > context;
  context._sort(reinterpret_cast<uint8_t*>(base), nmemb);
}

// ============================================================================
// [Fog::Algorithm::ISort]
// ============================================================================

template<typename TypeT>
static FOG_INLINE void isort_t(TypeT* base, size_t nmemb)
{
  TypeT* pm;
  TypeT* pl;

  for (pm = base + 1; pm < base + nmemb; pm++)
    for (pl = pm; pl > base && pl[-1] > pl[0]; pl--)
      swap(pl[-1], pl[0]);
}

// ============================================================================
// [Fog::Algorithm::BSearch]
// ============================================================================

FOG_API const void* bsearch(const void* base, size_t nmemb, size_t size, const void* key, CompareFunc compar);
FOG_API const void* bsearch(const void* base, size_t nmemb, size_t size, const void* key, CompareExFunc compar, const void* data);

template<typename TypeT>
const TypeT* bsearch_t(const TypeT* base, size_t nmemb, const TypeT* key)
{
  BSearchImpl< BSearchType<TypeT> > context;

  return context._search(
    reinterpret_cast<const uint8_t*>(base), nmemb,
    reinterpret_cast<const uint8_t*>(key));
}

//! @}

} // Algorithm namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_ALGORITHM_H
