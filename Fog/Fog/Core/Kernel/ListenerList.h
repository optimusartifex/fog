// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_KERNEL_LISTENERLIST_H
#define _FOG_CORE_KERNEL_LISTENERLIST_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/List.h>

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
  mutable size_t _notifyDepth;

private:
  _FOG_NO_COPY(ListenerListBase)
};

template <typename ListenerT>
struct ListenerList : public ListenerListBase
{
  FOG_INLINE ListenerList() {}
  FOG_INLINE ~ListenerList() {}

  FOG_INLINE const List<ListenerT*>& getList() const
  { return reinterpret_cast<const List<ListenerT*>&>(_listeners); }

  FOG_INLINE size_t getLength() const
  { return _listeners.getLength(); }

  FOG_INLINE ListenerT* _at(size_t index) const
  { return reinterpret_cast<ListenerT*>(_listeners.getAt(index)); }

  //! @brief Add an listener to the list.
  FOG_INLINE bool add(ListenerT* listener)
  { return ListenerListBase::_add(static_cast<void*>(listener)); }

  //! @brief  Remove an listener from the list.
  FOG_INLINE bool remove(ListenerT* listener)
  { return ListenerListBase::_remove(static_cast<void*>(listener)); }

  //! @brief Scope, used by @c FOG_LISTENER_FOR_EACH() macro defined below.
  struct Scope
  {
    FOG_INLINE Scope(const ListenerList<ListenerT>& listenerList) :
      _list(listenerList)
    {
      _list._notifyDepth++;
    }

    FOG_INLINE ~Scope()
    {
      if (--_list._notifyDepth == 0) _list._compact();
    }

  //private:
    const ListenerList<ListenerT>& _list;
  };

//private:
//  friend struct ListenerList<ListenerT>::Scope;

  _FOG_NO_COPY(ListenerList)
};

#define FOG_LISTENER_FOR_EACH(ListenerT, _listenerList, _func) \
  FOG_MACRO_BEGIN \
    size_t _ForEachIndex = 0; \
    size_t _ForEachLength = (_listenerList).getList().getLength(); \
    \
    for (_ForEachIndex = 0; _ForEachIndex < _ForEachLength; _ForEachIndex++) \
    { \
      ListenerT* __for_each_listener = _listenerList.getList().getAt(_ForEachIndex); \
      if (__for_each_listener) __for_each_listener->_func; \
    } \
  FOG_MACRO_END

} // Fog namespace

// [Guard]
#endif  // _FOG_CORE_KERNEL_LISTENERLIST_H
