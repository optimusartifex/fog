// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/MemZoneAllocator.h>

namespace Fog {

// ============================================================================
// [Fog::MemZoneAllocator - Construction / Destruction]
// ============================================================================

static void FOG_CDECL MemZoneAllocator_ctor(MemZoneAllocator* self, size_t nodeSize)
{
  self->_nodeSize = nodeSize;

  // We need at least one node. The first node is here just to prevent checking
  // for null pointers in inlined methods.
  self->_first.prev = NULL;
  self->_first.next = NULL;
  self->_first.pos = self->_first.data;
  self->_first.end = self->_first.data;

  self->_current = &self->_first;
}

// ============================================================================
// [Fog::MemZoneAllocator - Alloc]
// ============================================================================

static void* FOG_CDECL MemZoneAllocator_alloc(MemZoneAllocator* self, size_t size)
{
  // First look into the next node.
  MemZoneNode* node = self->_current->next;

  // alloc() increased the position, we must decrease it.
  self->_current->pos -= size;

  if (node == NULL)
  {
    node = self->_allocChunk();
    if (node == NULL)
    {
      // Fix pos to never overflow if called zillion times.
      self->_current->pos = self->_current->data + self->_nodeSize;
      return NULL;
    }
    node->prev = self->_current;
    self->_current->next = node;

    node->next = NULL;
    node->end = node->data + self->_nodeSize;
  }

  self->_current = node;
  node->pos = reinterpret_cast<uint8_t*>(node->data) + size;
  return node->data;
}

// ============================================================================
// [Fog::MemZoneAllocator - Clear / Reset]
// ============================================================================

static void FOG_CDECL MemZoneAllocator_clear(MemZoneAllocator* self)
{
  self->_current = &self->_first;
}

static void FOG_CDECL MemZoneAllocator_reset(MemZoneAllocator* self)
{
  MemZoneNode* cur = self->_first.next;

  while (cur)
  {
    MemZoneNode* next = cur->next;
    MemMgr::free(cur);
    cur = next;
  }

  self->_first.next = NULL;
  self->_current = &self->_first;
}

// ============================================================================
// [Fog::MemZoneAllocator - Record / Revert]
// ============================================================================

static MemZoneRecord* FOG_CDECL MemZoneAllocator_record(MemZoneAllocator* self)
{
  MemZoneRecord* record = reinterpret_cast<MemZoneRecord*>(
    self->alloc(sizeof(MemZoneRecord)));

  record->current = self->_current;
  record->pos = self->_current->pos;

  return record;
}

static void FOG_CDECL MemZoneAllocator_revert(MemZoneAllocator* self, MemZoneRecord* record, bool keepRecord)
{
  self->_current = record->current;
  self->_current->pos = record->pos;

  if (keepRecord)
    self->alloc(sizeof(MemZoneRecord));
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemZoneAllocator_init(void)
{
  fog_api.memzoneallocator_ctor = MemZoneAllocator_ctor;
  fog_api.memzoneallocator_dtor = MemZoneAllocator_reset;
  fog_api.memzoneallocator_alloc = MemZoneAllocator_alloc;
  fog_api.memzoneallocator_clear = MemZoneAllocator_clear;
  fog_api.memzoneallocator_reset = MemZoneAllocator_reset;
  fog_api.memzoneallocator_record = MemZoneAllocator_record;
  fog_api.memzoneallocator_revert = MemZoneAllocator_revert;
}

} // Fog namespace
