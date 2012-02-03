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

static void FOG_CDECL MemZoneAllocator_ctor(MemZoneAllocator* self, uint32_t nodeSize)
{
  self->_pos = self->_first.data;
  self->_end = self->_first.data;

  self->_current = &self->_first;
  self->_nodeSize = nodeSize;
  self->_firstSize = 0;

  // We need at least one node. The first node is here just to prevent checking
  // for null pointers in inlined methods.
  self->_first.prev = NULL;
  self->_first.next = NULL;
}

// ============================================================================
// [Fog::MemZoneAllocator - Alloc]
// ============================================================================

static void* FOG_CDECL MemZoneAllocator_alloc(MemZoneAllocator* self, size_t size)
{
  // First look into the next node.
  MemZoneNode* node = self->_current->next;

  // alloc() increased the position, we must decrease it.
  self->_pos -= size;

  if (node == NULL)
  {
    node = self->_allocChunk();

    if (node == NULL)
    {
      // Fix pos to never overflow if failed multiple times.
      self->_pos = self->_current->data + self->_nodeSize;
      return NULL;
    }

    node->prev = self->_current;
    self->_current->next = node;

    node->next = NULL;
  }

  self->_current = node;
  self->_pos = reinterpret_cast<uint8_t*>(node->data) + size;
  self->_end = node->data + self->_nodeSize;
  return node->data;
}

// ============================================================================
// [Fog::MemZoneAllocator - Clear / Reset]
// ============================================================================

static void FOG_CDECL MemZoneAllocator_clear(MemZoneAllocator* self)
{
  self->_current = &self->_first;

  self->_pos = self->_first.data;
  self->_end = self->_first.data + self->_firstSize;
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

  self->_pos = self->_first.data;
  self->_end = self->_first.data + self->_firstSize;
}

// ============================================================================
// [Fog::MemZoneAllocator - Record / Revert]
// ============================================================================

static MemZoneRecord* FOG_CDECL MemZoneAllocator_record(MemZoneAllocator* self)
{
  MemZoneRecord* record = static_cast<MemZoneRecord*>(
    self->alloc(sizeof(MemZoneRecord)));

  if (record != NULL)
    record->current = self->_current;

  return record;
}

static void FOG_CDECL MemZoneAllocator_revert(MemZoneAllocator* self, MemZoneRecord* record, bool keepRecord)
{
  self->_current = record->current;
  self->_pos = reinterpret_cast<uint8_t*>(record);

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
