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
#include <Fog/Core/Memory/MemGCAllocator.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>

namespace Fog {

// ============================================================================
// [Fog::MemGCLink]
// ============================================================================

struct FOG_NO_EXPORT MemGCLink
{
  MemGCLink* next;
};

// ============================================================================
// [Fog::MemGCAllocator - Helpers]
// ============================================================================

static FOG_INLINE size_t MemGCAllocator_align(size_t size, size_t n)
{
  return (size + n - 1) & ~(size_t)(n - 1);
}

// ============================================================================
// [Fog::MemGCAllocator - Data]
// ============================================================================

static Static <MemGCAllocatorData> MemGCAllocator_dNull;

static MemGCAllocatorData* MemGCAllocator_dCreate()
{
  MemGCAllocatorData* d = static_cast<MemGCAllocatorData*>(
    MemMgr::alloc(sizeof(MemGCAllocatorData)));

  if (FOG_IS_NULL(d))
    return NULL;

  d->chunkList.init();
  d->actualChunk = &d->fakeChunk;
  d->allocatedMemory = 0;
  d->usedMemory = 0;
  d->poolCounter = 0;
  d->baseChunkSize = 1024;

  // Initialize fake chunk.
  d->fakeChunk.pos = 0;
  d->fakeChunk.unused = 0;
  d->fakeChunk.length = 0;

  MemOps::zero(d->pool, sizeof(d->pool));
  return d;
}

static void MemGCAllocator_dFree(MemGCAllocatorData* d)
{
  d->chunkList.destroy();
  MemMgr::free(d);
}

// ============================================================================
// [Fog::MemGCAllocator - Construction / Destruction]
// ============================================================================

MemGCAllocator::MemGCAllocator(const MemGCFuncs* funcs)
{
  // Data.
  _d = &MemGCAllocator_dNull;
  
  // Scope/Queue links.
  _actualScope = &_fakeScope;
  _firstQueue = &_fakeQueue;
  _actualQueue = &_fakeQueue;

  // Initialize fake queue.
  _fakeQueue.prev = NULL;
  _fakeQueue.next = NULL;
  _fakeQueue.pos = 0;
  _fakeQueue.length = 1;
  _fakeQueue.data[0] = NULL;

  // Initialize fake scope.
  _fakeScope.prev = NULL;
  _fakeScope.queue = _actualQueue;
  _fakeScope.pos = 0;

  // Callbacks.
  _funcs = funcs;
}

MemGCAllocator::~MemGCAllocator()
{
  reset();
}

// ============================================================================
// [Fog::MemGCAllocator - Alloc]
// ============================================================================

void* MemGCAllocator::alloc(size_t size, size_t* realSize)
{
  MemGCAllocatorData* d = _d;

  if (FOG_UNLIKELY(d == &MemGCAllocator_dNull))
  {
    d = MemGCAllocator_dCreate();
    if (FOG_IS_NULL(d))
      return NULL;

    _d = d;
  }

  // If the requested size is too big, use standard C library allocator instead.
  if (size > MemGCAllocatorData::POOL_MAX_SIZE)
  {
    void* p = MemMgr::alloc(size);
    if (p != NULL)
    {
      d->allocatedMemory += size;
      d->usedMemory += size;
    
      if (realSize != NULL)
        *realSize = size;
    }
    return p;
  }

  // Align to pool granularity.
  size = MemGCAllocator_align(size, MemGCAllocatorData::POOL_GRANULARITY);

  // Okay, we have to first look whether there is a free block in a pool.
  size_t index = size >> MemGCAllocatorData::POOL_SHIFT;
  if (d->pool[index] != NULL)
  {
    void* p = d->pool[index];
    d->pool[index] = static_cast<MemGCLink*>(p)->next;

    d->usedMemory += size;
    d->poolCounter--;

    if (realSize)
      *realSize = size;

    return p;
  }

  // Try to allocate memory in actual chunk.
  if (d->actualChunk->pos + size <= d->actualChunk->length)
  {
    void* p = d->actualChunk->data + d->actualChunk->pos;

    d->actualChunk->pos += size;
    d->usedMemory += size;

    if (realSize)
      *realSize = size;

    return p;
  }

  // We are not lucky. We are now going to check whether there is memory in
  // other pools so we can split one block into two. But we are not going to
  // split the block which is immediately close to us, because we don't want
  // to have huge amount of small (possibly unused) objects in the pool. We 
  // also check pool counter whether it's profitable to traverse the pool.
  if (d->poolCounter >= 32)
  {
    size_t adjacentIndex = index + 4;

    if (adjacentIndex >= MemGCAllocatorData::POOL_LENGTH / 2 && 
        adjacentIndex < MemGCAllocatorData::POOL_LENGTH)
    {
      do {
        if (d->pool[adjacentIndex] != NULL)
        {
          // Split it.
          void* p = d->pool[adjacentIndex];

          void* secondP = static_cast<uint8_t*>(p) + size;
          size_t secondIndex = adjacentIndex - index;

          static_cast<MemGCLink*>(secondP)->next = static_cast<MemGCLink*>(d->pool[secondIndex]);
          d->pool[secondIndex] = secondP;

          // Because the block was split and we only return the first part, and
          // the second part put back into the pool, we can't advance the pool
          // counter. But we have to update count of used bytes.
          d->usedMemory += size;
          
          if (realSize)
            *realSize = size;
          
          return p;
        }
      } while (++adjacentIndex < MemGCAllocatorData::POOL_LENGTH);
    }
  }

  // If there is some memory in the actual chunk, pool it so it can be used
  // by another, but smaller, request.
  {
    void* p = d->actualChunk->data + d->actualChunk->pos;
    size_t remain = d->actualChunk->length - d->actualChunk->pos;

    index = remain >> MemGCAllocatorData::POOL_SHIFT;
    remain = index << MemGCAllocatorData::POOL_SHIFT;

    if (remain > 0)
    {
      static_cast<MemGCLink*>(p)->next = static_cast<MemGCLink*>(d->pool[index]);
      d->pool[index] = p;
    }
  }

  // Allocate new chunk. We slightly increase the chunk size if there are 
  // already chunks to delay another memory allocation as much as possible.
  size_t newChunkSize = d->baseChunkSize << Math::min<size_t>(d->chunkList->getLength(), 4);
  MemGCChunk* newChunk;

  // Do not try to alloc the exact page size, give some space to system memory
  // manager.
  size_t systemTolerance = sizeof(void*) * 4;

  for (;;)
  {
    newChunk = static_cast<MemGCChunk*>(MemMgr::alloc(newChunkSize - systemTolerance));

    if (FOG_LIKELY(newChunk != NULL))
      break;

    // Try to decrease chunk size on failure.
    if (newChunkSize == d->baseChunkSize)
      return NULL;

    newChunkSize >>= 1;
    continue;
  }

  void* p = newChunk->data;
  newChunk->pos = size;
  newChunk->unused = 0;
  newChunk->length = newChunkSize - systemTolerance - sizeof(MemGCChunk) + sizeof(void*);

  ListIterator<MemGCChunk*> it(d->chunkList());
  while (it.isValid())
  {
    if (it.getItem() > newChunk)
      break;
    it.next();
  }

  if (d->chunkList->insert(it.getIndex(), newChunk) != ERR_OK)
  {
    MemMgr::free(newChunk);
    return NULL;
  }

  d->allocatedMemory += newChunk->length;
  d->usedMemory += size;
  d->actualChunk = newChunk;

  if (realSize)
    *realSize = size;

  return p;
}

// ============================================================================
// [Fog::MemGCAllocator - Mark]
// ============================================================================

err_t MemGCAllocator::mark(void* p)
{
  MemGCAllocatorData* d = _d;

  // If we are marking anything the it had to go through MemGCAllocator::alloc(),
  // so 'd' have to be allocated at this time.
  FOG_ASSERT(d != &MemGCAllocator_dNull);

  // This MUST be checked before - this method is designed to mark object and
  // to put it into the GC queue.
  FOG_ASSERT(_funcs->getObjectMark(p) == false);

  // In the most cases this should pass. 
  size_t pos = _actualQueue->pos;
  if (FOG_LIKELY(pos < _actualQueue->length))
  {
    _actualQueue->data[pos] = p;
    _actualQueue->pos++;

    _funcs->setObjectMark(p, true);
    return ERR_OK;
  }

  // Okay, we have to use another queue chunk.
  //
  // If we are lucky then there is already allocated one.
  MemGCQueue* next = _actualQueue->next;
  if (next != NULL)
  {
    _actualQueue = next;
    _actualQueue->data[0] = p;
    _actualQueue->pos = 1;

    _funcs->setObjectMark(p, true);
    return ERR_OK;
  }

  // The worst case, and actually the only potential problematic case in which
  // we can get allocation failure. If we get allocation failure then we try
  // to decrease the requested size of queue, but of course this is not 
  // guaranteed to help.
  size_t queueLength = MemGCAllocatorData::QUEUE_LENGTH;
  for (;;)
  {
    next = static_cast<MemGCQueue*>(
      MemMgr::alloc(sizeof(MemGCQueue) + (queueLength - 1) * sizeof(void*)));

    if (FOG_LIKELY(next != NULL))
      break;

    if (queueLength < 32)
      return ERR_RT_OUT_OF_MEMORY;

    queueLength >>= 1;
  }

  // Just initialize and link to the existing queue list.
  _actualQueue->next = next;
  next->prev = _actualQueue;
  next->next = NULL;
  _actualQueue = next;

  next->pos = 1;
  next->length = queueLength;
  next->data[0] = p;

  _funcs->setObjectMark(p, true);
  return ERR_OK;
}

// ============================================================================
// [Fog::MemGCAllocator - Release]
// ============================================================================

void MemGCAllocator::release(void* p, size_t size)
{
  MemGCAllocatorData* d = _d;

  // If we are releasing anything the it had to go through MemGCAllocator::alloc(),
  // so 'd' have to be allocated at this time.
  FOG_ASSERT(d != &MemGCAllocator_dNull);

  // Free large using the same way as it has been allocated.
  if (size > MemGCAllocatorData::POOL_MAX_SIZE)
  {
    MemMgr::free(p);

    d->allocatedMemory -= size;
    d->usedMemory -= size;
    return;
  }

  // Align to pool granularity.
  size = MemGCAllocator_align(size, MemGCAllocatorData::POOL_GRANULARITY);

  // Pool small blocks.
  size_t index = size >> MemGCAllocatorData::POOL_SHIFT;
  d->usedMemory -= size;

  static_cast<MemGCLink*>(p)->next = static_cast<MemGCLink*>(d->pool[index]);
  d->pool[index] = p;
  d->poolCounter++;
}

// ============================================================================
// [Fog::MemGCAllocator - Collect]
// ============================================================================

static void MemGCAllocator_freeQueues(MemGCQueue* queue)
{
  while (queue != NULL)
  {
    MemGCQueue* next = queue->next;
    MemMgr::free(queue);
    queue = next;
  }
}

static void MemGCAllocator_freeChunks(MemGCAllocator* self)
{
  MemGCAllocatorData* d = self->_d;
  ListIterator<MemGCChunk*> it(d->chunkList());

  while (it.isValid())
  {
    MemMgr::free(it.getItem());
    it.next();
  }
}

static void MemGCAllocator_runGC(MemGCAllocator* self,
  MemGCQueue* fromQueue,
  size_t fromPos)
{
  // Internal method, ensure that we are in proper state.
  FOG_ASSERT(self->_d != &MemGCAllocator_dNull);

  MemGCQueue* lastQueue = self->_actualQueue;
  size_t lastPos = lastQueue->pos;

  MemGCQueue* curQueue = fromQueue;
  size_t curPos = fromPos;

  MemGCQueue* helpQueue = fromQueue;
  size_t helpPos = fromPos;

  const MemGCFuncs* funcs = self->_funcs;
  size_t destroyCounter = 0;

  for (;;)
  {
    void* p = curQueue->data[curPos];

    if (funcs->isObjectUsed(p))
    {
      // Object is used. We will move this object to helper queue so we can
      // check for all used objects before we return from collect(). This
      // means that the object will remain as "marked". We are going to
      // unmark used objects later.
      helpQueue->data[helpPos] = p;
      if (++helpPos == helpQueue->length)
      {
        helpQueue = helpQueue->next;
        FOG_ASSERT(helpQueue != NULL);
        helpPos = 0;
      }
    }
    else
    {
      // Object is not used so it can be safely destroyed. However, before
      // we destroy the object we need to get its size, because this allocator
      // doesn't remember that information.
      size_t objectSize = funcs->getObjectSize(p);
      funcs->destroyObject(p);

      self->release(p, objectSize);
      destroyCounter++;
    }

    // Advance.
    curPos++;

    if (curPos == lastPos && curQueue == lastQueue)
    {
      if (self->_actualQueue != lastQueue || self->_actualQueue->pos != lastPos)
      {
        // Extend lastPos and lastQueue if objects were added during the cycle.
        lastQueue = self->_actualQueue;
        lastPos = lastQueue->pos;
      }
      else
      {
        // We finished the garbage collection cycle. If no object has been
        // destroyed then we need to unmark all objects which survided.
        curQueue = fromQueue;
        curPos = fromPos;

        lastQueue = helpQueue;
        lastPos = helpPos;

        if (destroyCounter == 0 || (curQueue == helpQueue && curPos == helpPos))
          break;

        helpQueue = fromQueue;
        helpPos = fromPos;

        destroyCounter = 0;
        continue;
      }
    }

    if (curPos == curQueue->length)
    {
      curQueue = curQueue->next;
      FOG_ASSERT(curQueue != NULL);
      curPos = 0;
    }
  }
  
  for (;;)
  {
    size_t i;
    size_t n = lastPos;
    void** data = curQueue->data;

    if (curQueue != lastQueue)
      n = curQueue->length;

    for (i = curPos; i < n; i++)
      funcs->setObjectMark(data[i], false);

    if (curQueue != lastQueue)
    {
      curQueue = curQueue->next;
      FOG_ASSERT(curQueue != NULL);
      curPos = 0;
      continue;
    }

    break;
  }

  self->_actualQueue = fromQueue;
  self->_actualQueue->pos = fromPos;
}

void MemGCAllocator::collect()
{
  if (_d == &MemGCAllocator_dNull)
    return;

  MemGCScope* scope = _actualScope;

  if (scope->queue != _actualQueue || scope->pos != _actualQueue->pos)
  {
    // Run GC.
    MemGCAllocator_runGC(this, scope->queue, scope->pos);

    // Free GC queues.
    MemGCAllocator_freeQueues(_actualQueue->next);
    _actualQueue->next = NULL;
  }
}

// ============================================================================
// [Fog::MemGCAllocator - Scope]
// ============================================================================

void MemGCAllocator::enterScope(MemGCScope* scope)
{
  FOG_ASSERT(scope != NULL);

  scope->prev = _actualScope;
  scope->queue = _actualQueue;
  scope->pos = _actualQueue->pos;

  _actualScope = scope;
}

void MemGCAllocator::leaveScope(MemGCScope* scope)
{
  FOG_ASSERT(scope != NULL);
  FOG_ASSERT(scope == _actualScope);

  MemGCQueue* fromQueue = scope->queue;
  size_t fromPos = scope->pos;

  if (_d != &MemGCAllocator_dNull)
  {
    if (fromQueue != _actualQueue || fromPos != _actualQueue->pos)
    {
      // Run GC.
      MemGCAllocator_runGC(this, fromQueue, fromPos);

      // Free GC queues.
      MemGCAllocator_freeQueues(_actualQueue->next);
      _actualQueue->next = NULL;
    }
  }

  _actualScope = scope->prev;
}

// ============================================================================
// [Fog::MemGCAllocator - Reset]
// ============================================================================

void MemGCAllocator::reset()
{
  if (_d == &MemGCAllocator_dNull)
    return;

  // Reset can't be called somewhere in the middle of scoped code-flow.
  FOG_ASSERT(_actualScope == &_fakeScope);
  collect();

  FOG_ASSERT(_d->usedMemory == 0);
  MemGCAllocator_freeChunks(this);

  MemGCAllocator_dFree(_d);
  _d = &MemGCAllocator_dNull;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemGCAllocator_init(void)
{
  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  MemGCAllocatorData* d = &MemGCAllocator_dNull;

  d->chunkList.init();
  d->actualChunk = &d->fakeChunk;
  d->baseChunkSize = 1024;
}

} // Fog namespace
