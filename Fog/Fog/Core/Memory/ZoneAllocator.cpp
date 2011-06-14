// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/ZoneAllocator_p.h>

namespace Fog {

// ============================================================================
// [Fog::ZoneAllocator]
// ============================================================================

ZoneAllocator::ZoneAllocator(size_t chunkSize)
  : _chunkSize(chunkSize)
{
  // We need at least one chunk. The first chunk is here just to prevent
  // checking for null pointers in inlined methods.
  _first.prev = NULL;
  _first.next = NULL;
  _first.pos = _first.data;
  _first.end = _first.data;

  _current = &_first;
}

ZoneAllocator::~ZoneAllocator()
{
  reset();
}

void* ZoneAllocator::_alloc(size_t size)
{
  // First look into the next chunk.
  Chunk* chunk = _current->next;

  // alloc() increased the position, we must decrease it.
  _current->pos -= size;

  if (chunk == NULL)
  {
    chunk = _allocChunk();
    if (chunk == NULL)
    {
      // Fix pos to never overflow if called zillion times.
      _current->pos = _current->data + _chunkSize;
      return NULL;
    }
    chunk->prev = _current;
    _current->next = chunk;
    chunk->next = NULL;
    chunk->end = chunk->data + _chunkSize;
  }

  _current = chunk;
  chunk->pos = reinterpret_cast<uint8_t*>(chunk->data) + size;
  return chunk->data;
}

ZoneAllocator::Record* ZoneAllocator::record()
{
  Record* rec = reinterpret_cast<Record*>(alloc(sizeof(Record)));

  rec->current = _current;
  rec->pos = _current->pos;

  return rec;
}

void ZoneAllocator::revert(Record* record, bool keepRecord)
{
  _current = record->current;
  _current->pos = record->pos;

  if (keepRecord) alloc(sizeof(Record));
}

void ZoneAllocator::reset()
{
  _current = &_first;
}

void ZoneAllocator::free()
{
  Chunk* cur = _first.next;
  while (cur)
  {
    Chunk* next = cur->next;
    Memory::free(cur);
    cur = next;
  }

  _first.next = NULL;
  _current = &_first;
}

} // Fog namespace
