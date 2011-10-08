// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Memory/MemBlockAllocator.h>

namespace Fog {

// ============================================================================
// [Fog::MemBlockAllocator - Constants]
// ============================================================================

//! @brief Size of one block (decreased by some value to always fit in
//! one/two pages). 96 bytes reserved for @c ::malloc() data and our
//! @c Block data.
enum { MEM_BLOCK_ALLOCATOR_NODE_SIZE = 8080 };

// ============================================================================
// [Fog::MemBlockAllocator - Construction / Destruction]
// ============================================================================

static void FOG_CDECL MemBlockAllocator_ctor(MemBlockAllocator* self)
{
  self->nodes = NULL;
}

// ============================================================================
// [Fog::MemBlockAllocator - Alloc / Free]
// ============================================================================

static void* FOG_CDECL MemBlockAllocator_alloc(MemBlockAllocator* self, size_t size)
{
  MemBlockNode* node = self->nodes;
  size += sizeof(MemBlockNode*);

  FOG_ASSERT(size <= MEM_BLOCK_ALLOCATOR_NODE_SIZE);

  if (FOG_UNLIKELY(node == NULL || (node->size - node->pos) < size))
  {
    // Traverse to previous blocks and try to find a completely freed one.
    MemBlockNode* prev = NULL;

    while (node)
    {
      if (node->used.get() == 0)
      {
        // Make it first.
        if (prev)
        {
          prev->next = node->next;
          node->next = self->nodes;
          self->nodes = node;
        }

        node->pos = 0;
        goto _AllocFromBlocks;
      }

      prev = node;
      node = node->next;
    }

    // If we are here, it's needed to allocate new node of memory.
    size_t nodeSize = MEM_BLOCK_ALLOCATOR_NODE_SIZE;
    node = (MemBlockNode*)MemMgr::alloc(MemBlockNode::getSizeOf(nodeSize));

    if (FOG_IS_NULL(node))
      return NULL;

    node->size = nodeSize;
    node->pos = 0;
    node->used.init(0);

    node->next = self->nodes;
    self->nodes = node;
  }

_AllocFromBlocks:
  // Initialize the link to the MemBlockNode.
  *reinterpret_cast<MemBlockNode**>(node->data + node->pos) = node;

  node->pos += size;
  node->used.inc();

  return reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(node) + sizeof(MemBlockNode**));
}

// ============================================================================
// [MemBlockAllocator - Clear / Reset]
// ============================================================================

static void FOG_CDECL MemBlockAllocator_reset(MemBlockAllocator* self)
{
  MemBlockNode* node = self->nodes;
  self->nodes = NULL;

  while (node)
  {
    // It's failure if MemBlockNode::used is not zero when resetting.
    FOG_ASSERT(node->used.get() == 0);
    MemBlockNode* next = node->next;

    MemMgr::free(node);
    node = next;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemBlockAllocator_init(void)
{
  _api.memblockallocator_ctor = MemBlockAllocator_ctor;
  _api.memblockallocator_dtor = MemBlockAllocator_reset;
  _api.memblockallocator_alloc = MemBlockAllocator_alloc;
  _api.memblockallocator_reset = MemBlockAllocator_reset;
}

} // Fog namespace
