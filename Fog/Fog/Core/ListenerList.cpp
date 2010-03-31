// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

#include <Fog/Core/ListenerList.h>

namespace Fog {

// ============================================================================
// [Fog::ListenerListBase]
// ============================================================================

ListenerListBase::ListenerListBase()
  : _notifyDepth(0)
{
}

ListenerListBase::~ListenerListBase()
{
}

bool ListenerListBase::_add(void* listener)
{
  // Listeners can only be added once!
  if (_listeners.indexOf(listener) != INVALID_INDEX) return false;
  return (_listeners.append(listener) == ERR_OK);
}

bool ListenerListBase::_remove(void* listener)
{
  sysuint_t i = _listeners.indexOf(listener);

  if (i != INVALID_INDEX)
  {
    if (_notifyDepth)
      return _listeners.set(i, NULL) == ERR_OK;
    else
      return _listeners.removeAt(i) == ERR_OK;
  }
  else
  {
    return false;
  }
}

bool ListenerListBase::_has(void* listener)
{
  return _listeners.contains(listener);
}

void ListenerListBase::_compact() const
{
  sysuint_t i = 0;

  while (i < _listeners.getLength())
  {
    if (_listeners.at(i) == NULL)
      _listeners.removeAt(i);
    else
      i++;
  }
}

} // Fog namespace