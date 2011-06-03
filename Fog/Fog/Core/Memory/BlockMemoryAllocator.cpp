// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/BlockMemoryAllocator_p.h>

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

void* BlockMemoryAllocator::alloc(size_t size)
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
    if (FOG_IS_NULL(cur)) return NULL;

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

} // Fog namespace
