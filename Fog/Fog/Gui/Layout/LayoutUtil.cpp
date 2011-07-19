// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Math/Math.h>
#include <Fog/Gui/Layout/LayoutUtil.h>
#include <Fog/Gui/Layout/GridLayout.h>

namespace Fog {
namespace LayoutUtil {

template<class T>
static FOG_INLINE void calculateFlexOffsetsT(T* head, int avail, int& used)
{
  if (head == NULL) return;

  bool grow = avail > used;
  int remaining = Math::abs(avail - used);

  FOG_ASSERT(remaining != 0);

  bool set = false;

  while (remaining != 0)
  {
    bool breakfrom = true;
    float flexStep = FLOAT_MAX;
    T* item = head;
    float flexSum = 0;

    // Calculate sum of all flexes and initialize variables.
    // LAYOUT TODO: can be done on setter so we can save this loop!
    while (item)
    {
      LayoutItem::FlexLayoutData* child = static_cast<LayoutItem::FlexLayoutData*>(item->_layoutdata);

      if (!set)
      {
        child->init(grow);
      }

      if (child->_potential > 0)
      {
        flexSum += child->_flex;
        flexStep = Math::min<float>(flexStep, child->_potential / child->_flex);
      }

      item = (T*)child->_next;
    }

    // Should not happen, but just to be sure :-)
    if (flexSum == 0)
      break;

    set = true;

    // Never take more space than the parent has!
    flexStep = Math::min<float>((float)remaining, flexStep * flexSum) / flexSum;

    // Start with the calculation.
    item = head;

    while (item)
    {
      LayoutItem::FlexLayoutData* child = static_cast<LayoutItem::FlexLayoutData*>(item->_layoutdata);

      if (child->_potential > 0)
      {
        breakfrom = false;
        // Compute offset for this step.
        int currentOffset = Math::min<int>(remaining, child->_potential, (int)::ceil(flexStep * child->_flex));
        // Update child status.
        child->_potential -= currentOffset;

        child->_offset += grow? currentOffset : -currentOffset;
        used += child->_offset;

        // Update parent status.
        remaining -= currentOffset;
      }

      item = (T*)child->_next;
    }

    if (breakfrom)
      break;
  }
}

void calculateFlexOffsets(LayoutItem* head, int avail, int& used)
{
  return calculateFlexOffsetsT<LayoutItem>(head, avail, used);
}

void calculateFlexOffsets(GridLayout::Column* head, int avail, int& used)
{
  return calculateFlexOffsetsT<GridLayout::Column>(head, avail, used);
}

void calculateFlexOffsets(GridLayout::Row* head, int avail, int& used)
{
  return calculateFlexOffsetsT<GridLayout::Row>(head, avail, used);
}

#define CALC(VALUE) \
  if (VALUE < 0) \
  { \
    min = Math::min(min, VALUE); \
  } \
  else if (VALUE > 0) \
  { \
    max = Math::max(max, VALUE); \
  }

int collapseMargins(int margin1, int margin2)
{
  int min = 0;
  int max = 0;

  CALC(margin1);
  CALC(margin2);

  return min + max;
}

int collapseMargins(int margin1, int margin2, int margin3)
{
  int min = 0;
  int max = 0;

  CALC(margin1);
  CALC(margin2);
  CALC(margin3);

  return min + max;
}

#undef CALC

} // LayoutUtil namespace
} // Fog namespace
