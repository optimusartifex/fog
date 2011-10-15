// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemPool.h>

namespace Fog {

// ===========================================================================
// [Fog::MemPool - Configuration]
// ===========================================================================

enum
{
  MEMPOOL_CAPACITY_INITIAL = 8,
  MEMPOOL_CAPACITY_THRESHOLD = 1024
};

// ===========================================================================
// [Fog::MemPool - Helpers]
// ===========================================================================

static MemPool::Chunk* MemPool_allocChunk(size_t szItem, size_t capacity)
{
  MemPool::Chunk* chunk = reinterpret_cast<MemPool::Chunk*>(
    MemMgr::alloc(sizeof(MemPool::Chunk) + capacity * szItem));

  if (FOG_IS_NULL(chunk))
    return NULL;

  chunk->next = NULL;
  chunk->capacity = capacity;

  return chunk;
}

static MemPool::Link* MemPool_createLinks(MemPool::Chunk* chunk, size_t szItem, MemPool::Link* lastLink = NULL)
{
  size_t i = 0;
  size_t capacity = chunk->capacity;

  MemPool::Link* first = reinterpret_cast<MemPool::Link*>(
    reinterpret_cast<uint8_t*>(chunk) + sizeof(MemPool::Chunk));
  MemPool::Link* link = first;

  while (++i < capacity)
  {
    MemPool::Link* next = reinterpret_cast<MemPool::Link*>(
      reinterpret_cast<uint8_t*>(link) + szItem);

    link->next = next;
    link = next;
  }

  link->next = lastLink;
  return first;
}

// ===========================================================================
// [Fog::MemPool - Construction / Destruction]
// ===========================================================================

static void FOG_CDECL MemPool_ctor(MemPool* self)
{
  self->unused = NULL;
  self->chunk = NULL;
}

static void FOG_CDECL MemPool_dtor(MemPool* self)
{
  fog_api.mempool_reset(self);
}

// ===========================================================================
// [Fog::MemPool - Accessors]
// ===========================================================================

static size_t FOG_CDECL MemPool_getCapacity(const MemPool* self)
{
  size_t capacity = 0;
  const MemPool::Chunk* chunk = self->chunk;

  while (chunk)
  {
    capacity += chunk->capacity;
    chunk = chunk->next;
  }

  return capacity;
}

// ===========================================================================
// [Fog::MemPool - Reset / Save]
// ===========================================================================

static void FOG_CDECL MemPool_reset(MemPool* self)
{
  if (self->chunk == NULL)
    return;

  fog_api.mempool_freeSaved(self->chunk);

  self->unused = NULL;
  self->chunk = NULL;
}

static void* FOG_CDECL MemPool_save(MemPool* self)
{
  void* p = self->chunk;

  self->unused = NULL;
  self->chunk = NULL;

  return p;
}

// ===========================================================================
// [Fog::MemPool - Alloc / Free]
// ===========================================================================

static err_t FOG_CDECL MemPool_prealloc(MemPool* self, size_t szItem, size_t count)
{
  if (count == 0)
    return ERR_OK;

  MemPool::Chunk* chunk = MemPool_allocChunk(szItem, count);
  if (chunk == NULL)
    return ERR_RT_OUT_OF_MEMORY;

  MemPool::Link* p = MemPool_createLinks(chunk, szItem, self->unused);

  chunk->next = self->chunk;
  self->chunk = chunk;
  self->unused = p;

  return ERR_OK;
}

static void* FOG_CDECL MemPool_alloc(MemPool* self, size_t szItem)
{
  FOG_ASSERT(self->unused == NULL);
  MemPool::Chunk* chunk = self->chunk;

  // If this is the first chunk, we use very small initial capacity to ensure
  // that the memory is saved in case that won't be too much allocations.
  size_t capacity = MEMPOOL_CAPACITY_INITIAL;

  if (chunk != NULL)
  {
    capacity = chunk->capacity;

    if (capacity < MEMPOOL_CAPACITY_THRESHOLD)
      capacity = capacity * 2;

    if (capacity > MEMPOOL_CAPACITY_THRESHOLD)
      capacity = MEMPOOL_CAPACITY_THRESHOLD;
  }

  FOG_ASSERT(capacity > 0);

  chunk = MemPool_allocChunk(szItem, capacity);
  if (chunk == NULL)
    return NULL;

  MemPool::Link* p = MemPool_createLinks(chunk, szItem, self->unused);
  chunk->next = self->chunk;

  self->chunk = chunk;
  self->unused = p->next;

  return p;
}

// ===========================================================================
// [Fog::MemPool - Statics]
// ===========================================================================

static void FOG_CDECL MemPool_freeSaved(void* p)
{
  MemPool::Chunk* chunk = reinterpret_cast<MemPool::Chunk*>(p);

  while (chunk)
  {
    MemPool::Chunk* next = chunk->next;

    MemMgr::free(chunk);
    chunk = next;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemPool_init(void)
{
  fog_api.mempool_ctor = MemPool_ctor;
  fog_api.mempool_dtor = MemPool_dtor;
  fog_api.mempool_getCapacity = MemPool_getCapacity;
  fog_api.mempool_reset = MemPool_reset;
  fog_api.mempool_save = MemPool_save;
  fog_api.mempool_prealloc = MemPool_prealloc;
  fog_api.mempool_alloc = MemPool_alloc;
  fog_api.mempool_freeSaved = MemPool_freeSaved;
}

} // Fog namespace
