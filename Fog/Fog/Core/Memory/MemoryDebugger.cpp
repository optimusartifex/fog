#if 0
#if defined(FOG_DEBUG_MEMORY)
  fog_memdbg_state = 0;
  fog_memdbg_init();
#endif // FOG_DEBUG_MEMORY

#if defined(FOG_DEBUG_MEMORY)
  fog_memdbg_fini();
#endif // FOG_DEBUG_MEMORY
#endif

#if 0
FOG_CAPI_DECLARE void* fog_memory_alloc(sysuint_t size)
{
  if (FOG_LIKELY(size))
  {
    void* addr = malloc(size);

    // If out of memory returns true, try malloc again.
    if (FOG_IS_NULL(addr) && fog_out_of_memory()) addr = malloc(size);

#if defined(FOG_DEBUG_MEMORY)
    if (addr) fog_memdbg_add(addr, size);
#endif // FOG_DEBUG_MEMORY

    return addr;
  }
  else
    return NULL;
}

FOG_CAPI_DECLARE void* fog_memory_calloc(sysuint_t size)
{
  if (FOG_LIKELY(size))
  {
    void* addr = calloc(1, size);

    // If out of memory returns true, try calloc again
    if (FOG_IS_NULL(addr) && fog_out_of_memory()) addr = calloc(1, size);

#if defined(FOG_DEBUG_MEMORY)
    if (addr) fog_memdbg_add(addr, size);
#endif // FOG_DEBUG_MEMORY

    return addr;
  }
  else
    return NULL;
}

FOG_CAPI_DECLARE void* fog_memory_realloc(void* addr, sysuint_t size)
{
  if (FOG_LIKELY(addr != NULL))
  {
    if (FOG_LIKELY(size))
    {
      void* r_addr = realloc(addr, size);
      // If out of memory returns true, try realloc again
      if (FOG_IS_NULL(r_addr) && fog_out_of_memory()) r_addr = realloc(addr, size);

#if defined(FOG_DEBUG_MEMORY)
      if (r_addr)
      {
        fog_memdbg_remove(addr);
        fog_memdbg_add(r_addr, size);
      }
#endif // FOG_DEBUG_MEMORY

      return r_addr;
    }
    else
    {
      free(addr);
      return NULL;
    }
  }
  else
    return fog_memory_alloc(size);
}

FOG_CAPI_DECLARE void* fog_memory_reallocf(void* addr, sysuint_t size)
{
  void* r_addr = fog_memory_realloc(addr, size);
  // If out of memory returns true, try realloc again
  if (FOG_IS_NULL(r_addr) && fog_out_of_memory()) r_addr = realloc(addr, size);

  if (FOG_UNLIKELY(r_addr == NULL))
  {
    fog_memory_free(addr);
  }

  return r_addr;
}

FOG_CAPI_DECLARE void fog_memory_free(void* addr)
{
  if (FOG_LIKELY(addr != NULL))
  {
#if defined(FOG_DEBUG_MEMORY)
    fog_memdbg_remove(addr);
#endif // FOG_DEBUG_MEMORY
    free(addr);
  }
}
#endif
















// ===========================================================================
// [Fog::MemDbg]
// ===========================================================================
#if 0
// Should be defined by cmake if needed.
// #define FOG_DEBUG_MEMORY

#if defined(FOG_DEBUG_MEMORY)
struct MemDbgNode
{
  void* addr;
  sysuint_t size;
  MemDbgNode* next;
};

static Fog::Static<Fog::Lock> fog_memdbg_lock;

static uint fog_memdbg_state;

static uint64_t fog_memdbg_blocks_current;
static uint64_t fog_memdbg_blocks_total;
static uint64_t fog_memdbg_blocks_max;

static uint64_t fog_memdbg_heapalloc_current;
static uint64_t fog_memdbg_heapalloc_total;
static uint64_t fog_memdbg_heapalloc_max;

// This is small non-growing hash table, it's more better than single linked lists
static MemDbgNode** fog_memdbg_nodes;

static MemDbgNode* fog_memdbg_find(void* addr);
static void fog_memdbg_leaks(void);
static void fog_memdbg_dump(void* addr, sysuint_t size);

#define FOG_MEMDBG_TABLE_SIZE 47431

static FOG_INLINE uint32_t fog_memdbg_hash(void* addr)
{
  // Pointers are usually aligned to 4, 8 or 16 bytes (depending to arch)
#if FOG_ARCH_BITS == 32
  return ((uint32_t)addr >> 2) % FOG_MEMDBG_TABLE_SIZE;
#else
  return (((uint32_t)(sysuint_t)addr ^ (uint32_t)((sysuint_t)addr >> 32)) >> 3) % FOG_MEMDBG_TABLE_SIZE;
#endif
}

static void fog_memdbg_clear_statistics(void)
{
  fog_memdbg_blocks_current = 0;
  fog_memdbg_blocks_total = 0;
  fog_memdbg_blocks_max = 0;

  fog_memdbg_heapalloc_current = 0;
  fog_memdbg_heapalloc_total = 0;
  fog_memdbg_heapalloc_total = 0;
}

static void fog_memdbg_init(void)
{
  fog_memdbg_lock.init();

  fog_memdbg_state = 1;
  fog_memdbg_clear_statistics();

  fog_memdbg_nodes = (MemDbgNode**)calloc(1, sizeof(void*) * FOG_MEMDBG_TABLE_SIZE);
  if (fog_memdbg_nodes == NULL)
  {
    fog_out_of_memory_fatal_format("Fog::MemDbg", "init", "Couldn't allocate memory for memory debugger");
  }
}

static void fog_memdbg_fini(void)
{
  fog_stderr_msg("Fog::MemDbg", "shutdown", "Total memory allocations: %llu [Count].", (uint64_t)fog_memdbg_blocks_total);
  fog_stderr_msg("Fog::MemDbg", "shutdown", "Total allocated size: %llu [Bytes].", (uint64_t)fog_memdbg_heapalloc_total);

  fog_stderr_msg("Fog::MemDbg", "shutdown", "Maximum memory allocations at time: %llu [Count].", (uint64_t)fog_memdbg_blocks_max);
  fog_stderr_msg("Fog::MemDbg", "shutdown", "Maximum memory size at time: %llu [Bytes].", (uint64_t)fog_memdbg_heapalloc_max);

  // only show debug information if application wasn't fail.
  if (fog_failed == 0) fog_memdbg_leaks();

  free((void*)fog_memdbg_nodes);

  fog_memdbg_state = 0;
  fog_memdbg_clear_statistics();

  fog_memdbg_lock.destroy();
}

static void fog_memdbg_add(void* addr, sysuint_t size)
{
  MemDbgNode* node = (MemDbgNode*)malloc(sizeof(MemDbgNode));

  if (FOG_LIKELY(node != NULL))
  {
    fog_memdbg_lock->lock();

    node->addr = addr;
    node->size = size;

    node->next = fog_memdbg_nodes[fog_memdbg_hash(addr)];
    fog_memdbg_nodes[fog_memdbg_hash(addr)] = node;

    fog_memdbg_blocks_current++;
    fog_memdbg_blocks_total++;
    fog_memdbg_blocks_max = Fog::Math::max(fog_memdbg_blocks_max, fog_memdbg_blocks_current);

    fog_memdbg_heapalloc_current += size;
    fog_memdbg_heapalloc_total += size;
    fog_memdbg_heapalloc_max = Fog::Math::max(fog_memdbg_heapalloc_max, fog_memdbg_heapalloc_current);

    fog_memdbg_lock->unlock();
  }
  else
  {
    fog_out_of_memory_fatal_format("Fog::MemDbg", "add", "Couldn't allocate memory for debugger hash node");
  }
}

static void fog_memdbg_remove(void* addr)
{
  fog_memdbg_lock->lock();

  MemDbgNode* node = fog_memdbg_nodes[fog_memdbg_hash(addr)];
  MemDbgNode* prev = 0;

  while (node)
  {
    if (FOG_UNLIKELY(node->addr == addr))
    {
      if (prev)
        prev->next = node->next;
      else
        fog_memdbg_nodes[fog_memdbg_hash(addr)] = node->next;

      fog_memdbg_blocks_current--;
      fog_memdbg_heapalloc_current -= node->size;

      free(node);
      fog_memdbg_lock->unlock();
      return;
    }

    prev = node;
    node = node->next;
  }

  fog_memdbg_lock->unlock();

  fog_stderr_msg("Fog::MemDbg", "remove", "Memory address not found");

  // Cause segfault rather than exit
  *(uint*)NULL = 0;

  exit(1);
}

static void fog_memdbg_leaks(void)
{
  fog_memdbg_lock->lock();

  if (fog_memdbg_blocks_current > 0)
  {
    fog_stderr_msg("Fog::MemDbg", "leaks", "Detected %llu memory leak(s), size:%llu bytes.", fog_memdbg_blocks_current, fog_memdbg_heapalloc_current);

    sysuint_t i;
    MemDbgNode* node;

    for (i = 0; i != FOG_MEMDBG_TABLE_SIZE; i++)
    {
      for (node = fog_memdbg_nodes[i]; node; node = node->next)
      {
        bool doDump = true;

        fprintf(fog_stderr, "At %p of %u bytes", node->addr, (uint)node->size);

        // Fog uses implicit sharing that usually only needs to alloc
        // memory of pointer size in case that implicit shared object
        // was created by new operator (this is normal situation, because
        // objects allocated on the stack will not be here)
        if (node->size == sizeof(void*))
        {
          void *link = *(void **)node->addr;

          // Find if its a pointer.
          if (fog_memdbg_find(link))
          {
            fprintf(fog_stderr, " -> %p (pointer found)", link);
            doDump = false;
          }
          else
          {
            fprintf(fog_stderr, " -> %p (pointer not found)", link);
          }
        }

        fprintf(fog_stderr, "\n");
        if (doDump) fog_memdbg_dump(node->addr, (node->size < 512) ? node->size : 512);
      }
    }
    fflush(fog_stderr);
  }

  fog_memdbg_lock->unlock();
}

static MemDbgNode* fog_memdbg_find(void* addr)
{
  /* don't lock mutex, this method is only called from MemoryDebugger::leaks() */
  MemDbgNode* node = fog_memdbg_nodes[fog_memdbg_hash(addr)];
  while (node)
  {
    if (node->addr == addr) return node;
    node = node->next;
  }

  return NULL;
}

static void fog_memdbg_dump(void* addr, sysuint_t size)
{
  const uint8_t* addr_c = (const uint8_t*)addr;
  sysuint_t a = 0, i;

  while (a < size)
  {
    sysuint_t width = 24;
    if (a + width > size) width = size - a;

    for (i = 0; i != width; i++)
    {
      int c = addr_c[i];
      fprintf(fog_stderr, "%02X", c);
    }
    while (i++ < 24) fprintf(fog_stderr, "  ");

    for (i = 0; i != width; i++)
    {
      int c = addr_c[i];
      if (c >= ' ' && c < 128)
        fprintf(fog_stderr, "%c", c);
      else
        fprintf(fog_stderr, ".");
    }
    fprintf(fog_stderr, "\n");

    a += width;
    addr_c += width;
  }
}
#endif // FOG_DEBUG_MEMORY
#endif
