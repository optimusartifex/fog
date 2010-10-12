// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/MemoryAllocator_p.h>

namespace Fog {

// ============================================================================
// [Fog::BlockMemoryAllocator]
// ============================================================================

BlockMemoryAllocator::BlockMemoryAllocator() :
  blocks(NULL)
{
}

BlockMemoryAllocator::~BlockMemoryAllocator()
{
  reset();
}

void* BlockMemoryAllocator::alloc(sysuint_t size)
{
  size += sizeof(Header);

  FOG_ASSERT(size <= BLOCK_SIZE);

  if (FOG_UNLIKELY(!blocks || (blocks->size - blocks->pos) < size))
  {
    // Traverse to previous blocks and try to find complete free one.
    Block* cur = blocks;
    Block* prev = NULL;

    while (cur)
    {
      if (cur->used.get() == 0)
      {
        // Make it first.
        if (prev)
        {
          prev->next = cur->next;
          cur->next = blocks;
          blocks = cur;
        }

        cur->pos = 0;
        goto allocFromBlocks;
      }

      prev = cur;
      cur = cur->next;
    }

    // If we are here, it's needed to allocate new chunk of memory.
    cur = (Block*)Memory::alloc(sizeof(Block));
    if (!cur) return NULL;

    cur->size = BLOCK_SIZE;
    cur->pos = 0;
    cur->used.init(0);

    cur->next = blocks;
    blocks = cur;
  }

allocFromBlocks:
  Header* header = reinterpret_cast<Header*>(blocks->memory + blocks->pos);

  header->block = blocks;
  header->size = size;

  blocks->pos += size;
  blocks->used.add(size);

  return reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(header) + sizeof(Header));
}

void BlockMemoryAllocator::reset()
{
  Block* cur = blocks;
  blocks = NULL;

  while (cur)
  {
    // It's failure if Block::used is not zero when resetting.
    FOG_ASSERT(cur->used.get() == 0);

    Block* next = cur->next;
    Memory::free(cur);
    cur = next;
  }
}

// ============================================================================
// [Fog::ZoneMemoryAllocator]
// ============================================================================

ZoneMemoryAllocator::ZoneMemoryAllocator(sysuint_t chunkSize)
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

ZoneMemoryAllocator::~ZoneMemoryAllocator()
{
  free();
}

void* ZoneMemoryAllocator::_alloc(sysuint_t size)
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

ZoneMemoryAllocator::Record* ZoneMemoryAllocator::record()
{
  Record* rec = reinterpret_cast<Record*>(alloc(sizeof(Record)));

  rec->current = _current;
  rec->pos = _current->pos;

  return rec;
}

void ZoneMemoryAllocator::revert(Record* record, bool keepRecord)
{
  _current = record->current;
  _current->pos = record->pos;

  if (keepRecord) alloc(sizeof(Record));
}

void ZoneMemoryAllocator::reset()
{
  _current = &_first;
}

void ZoneMemoryAllocator::free()
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
