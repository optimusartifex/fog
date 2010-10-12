// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_ALGORITHMS_H
#define _FOG_CORE_ALGORITHMS_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/TypeInfo.h>

namespace Fog {

//! @addtogroup Fog_Core_Other
//! @{

// ============================================================================
// [Fog::QSort<T>]
// ============================================================================

//! @brief Quick sort algorithm implemented as C++ template.
template<typename T>
struct QSort
{
  typedef int (*CompareFn)(const T* a, const T* b);

  static FOG_NO_INLINE void sort(T* a, sysuint_t n, CompareFn compar = TypeCmp<T>::compare);

private:
  static FOG_INLINE T* med3(T* a, T* b, T* c, CompareFn compar);
  static FOG_INLINE void swap(T* a, T* b);
  static FOG_INLINE void swap(T* a, T* b, sysuint_t n);
};

template<typename T>
FOG_INLINE T* QSort<T>::med3(T* a, T* b, T* c, CompareFn compar)
{
  int cmp_ab = compar(a, b);
  int cmp_bc = compar(b, c);
  int cmp_ac = compar(a, c);

  return cmp_ab < 0 ? (cmp_bc < 0 ? b : (cmp_ac < 0 ? c : a))
                    : (cmp_bc > 0 ? b : (cmp_ac < 0 ? a : c));
}

template<typename T>
FOG_INLINE void QSort<T>::swap(T* a, T* b)
{
  T t = *a; *a = *b; *b = t;
}

template<typename T>
FOG_INLINE void QSort<T>::swap(T* a, T* b, sysuint_t n)
{
  T t;
  while (n--) { t = *a; *a++ = *b; *b++ = t; }
}

template<typename T>
void QSort<T>::sort(T* a, sysuint_t n, CompareFn compar)
{
  T *pa, *pb, *pc, *pd, *pl, *pm, *pn;
  sysint_t d, r, swap_cnt;

loop:
  swap_cnt = 0;

  // Insertion sort.
  if (n < 7)
  {
    for (pm = a + 1; pm < a + n; pm++)
      for (pl = pm; pl > a && compar(pl - 1, pl) > 0; pl--)
        swap(pl, pl - 1);
    return;
  }

  pm = a + (n >> 1);

  if (n > 7)
  {
    pl = a;
    pn = a + (n - 1);
    if (n > 40)
    {
      d = (n / 8);
      pl = med3(pl, pl + d, pl + 2 * d, compar);
      pm = med3(pm - d, pm, pm + d, compar);
      pn = med3(pn - 2 * d, pn - d, pn, compar);
    }
    pm = med3(pl, pm, pn, compar);
  }

  swap(a, pm);

  pa = pb = a + 1;
  pc = pd = a + (n - 1);

  for (;;)
  {
    while (pb <= pc && (r = compar(pb, a)) <= 0)
    {
      if (r == 0)
      {
        swap_cnt = 1;
        swap(pa, pb);
        pa++;
      }
      pb++;
    }
    while (pb <= pc && (r = compar(pc, a)) >= 0)
    {
      if (r == 0)
      {
        swap_cnt = 1;
        swap(pc, pd);
        pd--;
      }
      pc--;
    }
    if (pb > pc) break;

    swap_cnt = 1;
    swap(pb, pc);

    pb++;
    pc--;
  }

  // Insertion sort.
  if (swap_cnt == 0)
  {
    for (pm = a + 1; pm < a + n; pm++)
      for (pl = pm; pl > a && compar(pl - 1, pl) > 0; pl--)
        swap(pl, pl - 1);
    return;
  }

  pn = a + n;

  r = Math::min((sysint_t)(pa - a), (sysint_t)(pb - pa));
  swap(a, pb - r, r);

  r = Math::min((sysint_t)(pd - pc), (sysint_t)(pn - pd) - (sysint_t)1);
  swap(pb, pn - r, r);

  if ((r = (sysint_t)(pb - pa)) > 1) sort(a, (sysuint_t)r, compar);
  if ((r = (sysint_t)(pd - pc)) > 1)
  {
    // Iterate rather than recurse to save stack space.
    a = pn - r;
    n = r;
    goto loop;
  }
}

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_ALGORITHMS_H
