// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_LISTENERLIST_H
#define _FOG_CORE_LISTENERLIST_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/List.h>

namespace Fog {

//! @brief A container for a list of listeners. 
//!
//! Unlike a normal STL vector or list, this container can be modified during
//! iteration without invalidating the iterator. So, it safely handles the
//! case of an listener removing itself or other listeners from the list while
//! listeners are being notified.
struct FOG_API ListenerListBase
{
  ListenerListBase();
  ~ListenerListBase();

protected:
  bool _add(void* listener);
  bool _remove(void* listener);
  bool _has(void* listener);

  void _compact() const;

  // These are marked mutable to facilitate having NotifyAll be const.
  mutable List<void*> _listeners;
  mutable sysuint_t _notifyDepth;

private:
  FOG_DISABLE_COPY(ListenerListBase)
};

template <class ListenerType>
struct ListenerList : public ListenerListBase
{
  FOG_INLINE ListenerList() {}
  FOG_INLINE ~ListenerList() {}

  FOG_INLINE const List<ListenerType*>& getList() const
  { return reinterpret_cast<const List<ListenerType*>&>(_listeners); }

  FOG_INLINE sysuint_t getLength() const
  { return _listeners.getLength(); }

  FOG_INLINE ListenerType* _at(sysuint_t index) const
  { return reinterpret_cast<ListenerType*>(_listeners.at(index)); }

  //! @brief Add an listener to the list.
  FOG_INLINE bool add(ListenerType* listener)
  { return ListenerListBase::_add(static_cast<void*>(listener)); }

  //! @brief  Remove an listener from the list.
  FOG_INLINE bool remove(ListenerType* listener)
  { return ListenerListBase::_remove(static_cast<void*>(listener)); }

  //! @brief Scope, used by @c FOG_LISTENER_FOR_EACH() macro defined below.
  struct Scope
  {
    FOG_INLINE Scope(const ListenerList<ListenerType>& listenerList) :
      _list(listenerList)
    {
      _list._notifyDepth++;
    }

    FOG_INLINE ~Scope()
    {
      if (--_list._notifyDepth == 0) _list._compact();
    }

  private:
    const ListenerList<ListenerType>& _list;
  };

private:
  friend struct ListenerList<ListenerType>::Scope;

  FOG_DISABLE_COPY(ListenerList)
};

#define FOG_LISTENER_FOR_EACH(ListenerType, _listenerList, _func) \
  FOG_BEGIN_MACRO \
    sysuint_t __for_each_index = 0; \
    sysuint_t __for_each_length = (_listenerList).getList().getLength(); \
    \
    for (__for_each_index = 0; __for_each_index < __for_each_length; __for_each_index++) \
    { \
      ListenerType* __for_each_listener = _listenerList.getList().at(__for_each_index); \
      if (__for_each_listener) __for_each_listener->_func; \
    } \
  FOG_END_MACRO

} // Fog namespace

// [Guard]
#endif  // _FOG_CORE_LISTENERLIST_H
