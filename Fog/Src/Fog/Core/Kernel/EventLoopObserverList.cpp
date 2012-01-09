// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/EventLoopObserverList.h>

namespace Fog {

// ============================================================================
// [Fog::EventLoopObserverListBase - Constants]
// ============================================================================

#define FOG_EVENT_LOOP_OBSERVER_LIST_COMPACT_BIT \
  ((size_t)1 << (sizeof(size_t) * 8 - 1))

// ============================================================================
// [Fog::EventLoopObserverListBase - Constructor / Destructor]
// ============================================================================

static void FOG_CDECL EventLoopObserverList_ctor(EventLoopObserverListBase* self)
{
  self->_list.init();
  self->_depth = 0;
}

static void FOG_CDECL EventLoopObserverList_dtor(EventLoopObserverListBase* self)
{
  self->_list.destroy();
}

// ============================================================================
// [Fog::EventLoopObserverListBase - Add / Release]
// ============================================================================

static err_t FOG_CDECL EventLoopObserverList_add(EventLoopObserverListBase* self, void* obj)
{
  if (self->_list().indexOf(obj) != INVALID_INDEX)
    return ERR_RT_OBJECT_ALREADY_EXISTS;
  else
    return self->_list().append(obj);
}

static err_t FOG_CDECL EventLoopObserverList_remove(EventLoopObserverListBase* self, void* obj)
{
  List<void*> list = self->_list();
  size_t idx = list.indexOf(obj);

  if (idx == INVALID_INDEX)
    return ERR_RT_OBJECT_NOT_FOUND;

  if (self->_depth == 0)
    return list.removeAt(idx);

  // Fill the last bit in depth so we know to call _compact().
  self->_depth |= FOG_EVENT_LOOP_OBSERVER_LIST_COMPACT_BIT;
  return list.setAt(idx, NULL);
}

static bool FOG_CDECL EventLoopObserverList_contains(const EventLoopObserverListBase* self, void* obj)
{
  return self->_list().contains(obj);
}

// ============================================================================
// [Fog::EventLoopObserverListBase - Compact]
// ============================================================================

static void FOG_CDECL EventLoopObserverList_compact(const EventLoopObserverListBase* self)
{
  FOG_ASSERT(self->_depth == FOG_EVENT_LOOP_OBSERVER_LIST_COMPACT_BIT);

  self->_depth &= FOG_EVENT_LOOP_OBSERVER_LIST_COMPACT_BIT;

  List<void*> list = self->_list();
  size_t listLength = list.getLength(); 

  if (listLength == 0)
    return;

  // We disallow copy of EventLoopObserverList instances, this means that list is
  // never shared with another container. So getDataX() should be always safe!
  void** listData = list.getDataX();

  size_t dstIndex = 0;
  size_t srcIndex = 0;

  for (;;)
  {
    if (listData[srcIndex] == NULL)
      break;
    
    if (++srcIndex == listLength)
      return;
  }

  dstIndex = srcIndex;
  while (++srcIndex < listLength)
  {
    if (listData[srcIndex] != NULL)
      listData[dstIndex++] = listData[srcIndex];
  }

  // TODO: Quite hacky, some method implemented in List<> would be better than
  // this black-magic.
  list._d->length = dstIndex;
  list._d->end = list._d->start + dstIndex;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void EventLoopObserverList_init(void)
{
  // --------------------------------------------------------------------------
  // [Funcs]
  // --------------------------------------------------------------------------

  fog_api.eventloopobserverlist_ctor = EventLoopObserverList_ctor;
  fog_api.eventloopobserverlist_dtor = EventLoopObserverList_dtor;
  fog_api.eventloopobserverlist_add = EventLoopObserverList_add;
  fog_api.eventloopobserverlist_remove = EventLoopObserverList_remove;
  fog_api.eventloopobserverlist_contains = EventLoopObserverList_contains;
  fog_api.eventloopobserverlist_compact = EventLoopObserverList_compact;
}

} // Fog namespace
