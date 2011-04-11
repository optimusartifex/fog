// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_WIDGET_WIDGET_P_H
#define _FOG_GUI_WIDGET_WIDGET_P_H

// [Dependencies]
#include <Fog/Gui/Widget/Widget.h>

namespace Fog {

// ============================================================================
// [Private Widget Iterator used in Fog-Gui library]
//
// These macros are used internally, so don't use them outside if Fog library.
// ============================================================================

#define FOG_WIDGET_TREE_ITERATOR(__name__, __basewidget__, __conditional__, __before_traverse__, __after_traverse__) \
  FOG_WIDGET_TREE_ITERATOR_EX(__name__, __basewidget__, __conditional__, __before_traverse__, __after_traverse__, {}, {})

#define FOG_WIDGET_TREE_ITERATOR_EX(__name__, __basewidget__, __conditional__, __before_traverse__, __after_traverse__, __push__, __pop__) \
  if ((__basewidget__)->_children.getLength()) \
  { \
    ::Fog::PStack<512> stack; \
    ::Fog::Widget** childCur = (::Fog::Widget** )( __basewidget__->_children.getData() ); \
    ::Fog::Widget** childEnd = childCur + ( __basewidget__->_children.getLength() ); \
    ::Fog::Widget* child; \
    \
    for (;;) \
    { \
      child = *childCur; \
      \
      if (!((::Fog::Object*)child)->isWidget()) \
        goto __name__##_next; \
      \
      __before_traverse__ \
      \
      if (child->_children.getLength() && (__conditional__)) \
      { \
        \
        __push__ \
        \
        stack.push(childCur); \
        stack.push(childEnd); \
        \
        childCur = (::Fog::Widget** )child->_children.getData(); \
        childEnd = childCur + child->_children.getLength(); \
        \
        continue; \
      } \
      \
__name__##_after: \
      \
      __after_traverse__ \
      \
__name__##_next: \
      \
      childCur++; \
      \
      if (FOG_UNLIKELY(childCur == childEnd)) \
      { \
        if (FOG_UNLIKELY(stack.isEmpty())) break; \
        \
        stack.pop(childEnd); \
        stack.pop(childCur); \
        \
        __pop__ \
        \
        goto __name__##_after; \
      } \
    } \
  }

#define FOG_WIDGET_TREE_ITERATOR_NEXT(__name__) goto __name__##_next

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_WIDGET_WIDGET_P_H
