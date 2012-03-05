// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_KERNEL_EVENTLOOPOBSERVERLIST_H
#define _FOG_CORE_KERNEL_EVENTLOOPOBSERVERLIST_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>

namespace Fog {

// ============================================================================
// [Fog::EventLoopObserverListBase]
// ============================================================================

//! @brief Event loop observer list (base class for template).
struct FOG_NO_EXPORT EventLoopObserverListBase
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE EventLoopObserverListBase()
  {
    fog_api.eventloopobserverlist_ctor(this);
  }

  FOG_INLINE ~EventLoopObserverListBase()
  {
    fog_api.eventloopobserverlist_dtor(this);
  }

  // --------------------------------------------------------------------------
  // [List Management]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t _add(void* obj)
  {
    FOG_ASSERT(obj != NULL);
    return fog_api.eventloopobserverlist_add(this, obj);
  }

  FOG_INLINE err_t _remove(void* obj)
  {
    FOG_ASSERT(obj != NULL);
    return fog_api.eventloopobserverlist_remove(this, obj);
  }
  
  FOG_INLINE bool _contains(void* obj) const
  {
    FOG_ASSERT(obj != NULL);
    return fog_api.eventloopobserverlist_contains(this, obj);
  }

  // --------------------------------------------------------------------------
  // [Helpers]
  // --------------------------------------------------------------------------

  FOG_INLINE void _compact() const
  {
    fog_api.eventloopobserverlist_compact(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Event listeners.
  //!
  //! In case that the event listener is removed inside the dispatching process
  //! (@ref FOG_EVENT_LOOP_OBSERVER_LIST_EACH) then it's not removed from the
  //! list, but it's set to @c NULL. All @c NULL items are removed by @ref 
  //! _compact().
  mutable Static< List<void*> > _list;

  //! @brief Notify depth.
  mutable size_t _depth;

private:
  FOG_NO_COPY(EventLoopObserverListBase)
};

// ============================================================================
// [Fog::EventListenerList<ItemT>]
// ============================================================================

//! @brief Event loop observer list.
template <typename ItemT>
struct EventLoopObserverList : public EventLoopObserverListBase
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE EventLoopObserverList() {}
  FOG_INLINE ~EventLoopObserverList() {}

  // --------------------------------------------------------------------------
  // [List Management]
  // --------------------------------------------------------------------------

  //! @brief Get length (how many event listeners are in the list).
  FOG_INLINE size_t getLength() const
  {
    return EventLoopObserverListBase::_list().getLength();
  }

  //! @brief Get event listener at @a index.
  FOG_INLINE ItemT* getAt(size_t index) const
  {
    return reinterpret_cast<ItemT*>(EventLoopObserverListBase::_list().getAt(index));
  }

  //! @brief Get @c List<EventListenerT*> instance.
  FOG_INLINE const List<ItemT*>& getList() const
  {
    return reinterpret_cast<const List<ItemT*>&>(EventLoopObserverListBase::_list());
  }

  //! @brief Add an event listener to the list.
  FOG_INLINE err_t add(ItemT* obj)
  {
    return EventLoopObserverListBase::_add(static_cast<void*>(obj));
  }

  //! @brief Remove the event listener from the list.
  FOG_INLINE err_t remove(ItemT* obj)
  {
    return EventLoopObserverListBase::_remove(static_cast<void*>(obj));
  }
  
  FOG_INLINE bool contains(ItemT* obj) const
  {
    return EventLoopObserverListBase::_contains(static_cast<void*>(obj));
  }
  
  // --------------------------------------------------------------------------
  // [Statics]
  // --------------------------------------------------------------------------

  //! @brief Used internally to convert the type of listener into ItemT.
  static FOG_INLINE ItemT* _getItemAsListener(void* obj)
  {
    return reinterpret_cast<ItemT*>(obj);
  }

private:
  FOG_NO_COPY(EventLoopObserverList)
};

// ============================================================================
// [Fog::EventListenerList - Helpers]
// ============================================================================

#define FOG_EVENT_LOOP_OBSERVER_LIST_ENTER(_Self_) \
  FOG_MACRO_BEGIN \
    _Self_._depth++; \
  FOG_MACRO_END

#define FOG_EVENT_LOOP_OBSERVER_LIST_EACH(_Self_, _Action_) \
  FOG_MACRO_BEGIN \
    const ::Fog::List<void*>& _forEachList = _Self_._list(); \
    \
    size_t _forEachIndex = 0; \
    size_t _forEachLength = _forEachList.getLength(); \
    \
    while (_forEachIndex < _forEachLength) \
    { \
      void* _forEachItem = _forEachList.getAt(_forEachIndex); \
      \
      if (_forEachItem != NULL) \
        _Self_._getItemAsListener(_forEachItem)->_Action_; \
      \
      _forEachIndex++; \
    } \
  FOG_MACRO_END

#define FOG_EVENT_LOOP_OBSERVER_LIST_LEAVE(_Self_) \
  FOG_MACRO_BEGIN \
    _Self_._depth--; \
    if (_Self_._depth == ((size_t)1 << (sizeof(size_t) * 8 - 1))) \
      _Self_._compact(); \
  FOG_MACRO_END

} // Fog namespace

// [Guard]
#endif  // _FOG_CORE_KERNEL_EVENTLOOPOBSERVERLIST_H
