// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_WIDGET_WIDGET_P_H
#define _FOG_UI_WIDGET_WIDGET_P_H

// [Dependencies]
#include <Fog/UI/Widget/Widget.h>

namespace Fog {

// ============================================================================
// [Private Widget Iterator used in Fog-Gui library]
//
// These macros are used internally, so don't use them outside if Fog library.
// ============================================================================

#define FOG_WIDGET_TREE_ITERATOR(_Name_, _Base_, _Cond_, _BeforeTraverse_, _AfterTraverse_) \
  FOG_WIDGET_TREE_ITERATOR_EX(_Name_, _Base_, _Cond_, _BeforeTraverse_, _AfterTraverse_, {}, {})

#define FOG_WIDGET_TREE_ITERATOR_EX(_Name_, _Base_, _Cond_, _BeforeTraverse_, _AfterTraverse_, _Push_, _Pop_) \
  if ((_Base_)->_objectExtra->_children.getLength()) \
  { \
    ::Fog::StackP<512> stack; \
    ::Fog::Widget** childCur = (::Fog::Widget** )( _Base_->_objectExtra->_children.getData() ); \
    ::Fog::Widget** childEnd = childCur + ( _Base_->_objectExtra->_children.getLength() ); \
    ::Fog::Widget* child; \
    \
    for (;;) \
    { \
      child = *childCur; \
      \
      if (!((::Fog::Object*)child)->isWidget()) \
        goto _Name_##_next; \
      \
      _BeforeTraverse_ \
      \
      if (child->_objectExtra->_children.getLength() && (_Cond_)) \
      { \
        \
        _Push_ \
        \
        stack.push(childCur); \
        stack.push(childEnd); \
        \
        childCur = (::Fog::Widget** )child->_objectExtra->_children.getData(); \
        childEnd = childCur + child->_objectExtra->_children.getLength(); \
        \
        continue; \
      } \
      \
_Name_##_after: \
      \
      _AfterTraverse_ \
      \
_Name_##_next: \
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
        _Pop_ \
        \
        goto _Name_##_after; \
      } \
    } \
  }

#define FOG_WIDGET_TREE_ITERATOR_NEXT(_Name_) goto _Name_##_next

} // Fog namespace

// [Guard]
#endif // _FOG_UI_WIDGET_WIDGET_P_H
