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
#include <Fog/Core/Memory/MemDebug_p.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/Lock.h>

// [Dependencies - C]
#include <stdio.h>
#include <stdlib.h>

namespace Fog {

// ===========================================================================
// [Constants]
// ===========================================================================

// 00-31    - Maximum MemNode size (64-bit).
// 32-63    - Buffer underflow pattern.
// 63-XX    - Data.
// XX-XX+31 - Buffer overflow pattern.
enum { _MEMDBG_SIZE = 64 + 32 };

// ===========================================================================
// [Helpers]
// ===========================================================================

// Translate the user pointer to the memory debugger pointer.
static FOG_INLINE void* _DBG_FROM_USER(void* p)
{
  return reinterpret_cast<void*>(reinterpret_cast<char*>(p) - 64);
}

// Translate the memory debugger pointer to the user pointer.
static FOG_INLINE void* _USER_FROM_DBG(void* p)
{
  return reinterpret_cast<void*>(reinterpret_cast<char*>(p) + 64);
}

static const uint8_t MemMgr_protectedPattern[16] =
{
  '$', 'P', 'r', 'o', 't', 'e', 'c', 't', 'e', 'd', ' ', 'H', 'e', 'a', 'p', '$'
};

// ===========================================================================
// [Fog::MemNode]
// ===========================================================================

// 16 or 32 bytes.
struct FOG_NO_EXPORT MemNode
{
  MemNode* node[2];
  size_t red;
  size_t size;
};

// ===========================================================================
// [Fog::MemDbg]
// ===========================================================================

struct FOG_NO_EXPORT MemDbg
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  MemDbg();
  ~MemDbg();

  // --------------------------------------------------------------------------
  // [Management]
  // --------------------------------------------------------------------------

  void insert(void* p, size_t size);
  void remove(void* p);
  void check(void* p);

  void show();

  // --------------------------------------------------------------------------
  // [RB-Tree]
  // --------------------------------------------------------------------------

  static FOG_INLINE bool rbIsRed(MemNode* node)
  {
    return node != NULL && node->red;
  }

  static FOG_INLINE MemNode* rbRotateSingle(MemNode* root, int dir)
  {
    MemNode* save = root->node[!dir];

    root->node[!dir] = save->node[dir];
    save->node[ dir] = root;

    root->red = 1;
    save->red = 0;

    return save;
  }

  static FOG_INLINE MemNode* rbRotateDouble(MemNode* root, int dir)
  {
    root->node[!dir] = rbRotateSingle(root->node[!dir], !dir);
    return rbRotateSingle(root, dir);
  }

  void rbInsert(MemNode* node);
  bool rbRemove(MemNode* node);
  bool rbCheck(MemNode* node);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Lock lock;

  MemNode* rootNode;
  size_t blocksUsed;
  size_t bytesUsed;
};

// ===========================================================================
// [Fog::MemDbg - Construction / Destruction]
// ===========================================================================

MemDbg::MemDbg()
{
  rootNode = NULL;

  blocksUsed = 0;
  bytesUsed = 0;
}

MemDbg::~MemDbg()
{
}

// ===========================================================================
// [Fog::MemDbg - Management]
// ===========================================================================

void MemDbg::insert(void* p, size_t size)
{
  FOG_ASSERT(p != NULL);
  FOG_ASSERT(size > 0);

  MemNode* node = reinterpret_cast<MemNode*>(p);

  // Initialize.
  node->node[0] = NULL;
  node->node[1] = NULL;
  node->red = 0;
  node->size = size;

  // Copy the pattern to detect buffer underrun.
  memcpy(reinterpret_cast<char*>(p) + 32, MemMgr_protectedPattern, 16);
  memcpy(reinterpret_cast<char*>(p) + 48, MemMgr_protectedPattern, 16);

  // Copy the pattern to detect buffer overrun.
  memcpy(reinterpret_cast<char*>(p) + node->size + 64, MemMgr_protectedPattern, 16);
  memcpy(reinterpret_cast<char*>(p) + node->size + 80, MemMgr_protectedPattern, 16);

  rbInsert(node);

  blocksUsed++;
  bytesUsed += size;
}

void MemDbg::remove(void* p)
{
  FOG_ASSERT(p != NULL);

  MemNode* node = reinterpret_cast<MemNode*>(p);
  size_t size = node->size;

  if (!rbRemove(node))
  {
    Debug::failFunc("Fog::MemDbg", "remove", "Pointer %p not found.", _USER_FROM_DBG(p));
  }

  blocksUsed--;
  bytesUsed -= size;
}

void MemDbg::check(void* p)
{
  FOG_ASSERT(p != NULL);

  MemNode* node = reinterpret_cast<MemNode*>(p);

  if (!rbCheck(node))
  {
    Debug::failFunc("Fog::MemDbg", "check", "Pointer %p not found.", _USER_FROM_DBG(p));
  }

  // Check for the buffer underrun.
  if (memcmp(reinterpret_cast<char*>(p) + 32, MemMgr_protectedPattern, 16) != 0 ||
      memcmp(reinterpret_cast<char*>(p) + 48, MemMgr_protectedPattern, 16) != 0 )
  {
    Debug::failFunc("Fog::MemDbg", "check", "Detected buffer underrun (%p).", _USER_FROM_DBG(p));
  }

  // Check for the buffer overrun.
  if (memcmp(reinterpret_cast<char*>(p) + node->size + 64, MemMgr_protectedPattern, 16) != 0 ||
      memcmp(reinterpret_cast<char*>(p) + node->size + 80, MemMgr_protectedPattern, 16) != 0 )
  {
    Debug::failFunc("Fog::MemDbg", "check", "Detected buffer overrun (%p).", _USER_FROM_DBG(p));
  }
}

void MemDbg::show()
{
  // Correct state.
  if (blocksUsed == 0) return;

  Debug::dbgFunc("Fog::MemDbg", "show", "Detected %u memory leaks (%u bytes total)\n", (uint)blocksUsed, (uint)bytesUsed);
}

// ===========================================================================
// [Fog::MemDbg - RB-Tree]
// ===========================================================================

void MemDbg::rbInsert(MemNode* node)
{
  if (rootNode == NULL)
  {
    // Empty tree case.
    rootNode = node;
  }
  else
  {
    // False tree root.
    MemNode head = { { NULL, NULL }, 0, 0 };

    // Grandparent & parent.
    MemNode* g = NULL;
    MemNode* t = &head;

    // Iterator & parent.
    MemNode* p = NULL;
    MemNode* q = t->node[1] = rootNode;

    int dir = 0, last;

    // Search down the tree.
    for (;;)
    {
      if (q == NULL)
      {
        // Insert new node at the bottom.
        q = node;
        p->node[dir] = node;
      }
      else if (rbIsRed(q->node[0]) && rbIsRed(q->node[1]))
      {
        // Color flip.
        q->red = 1;
        q->node[0]->red = 0;
        q->node[1]->red = 0;
      }

      // Fix red violation.
      if (rbIsRed(q) && rbIsRed(p))
      {
        int dir2 = t->node[1] == g;
        t->node[dir2] = (q == p->node[last]) ? rbRotateSingle(g, !last) : rbRotateDouble(g, !last);
      }

      // Stop if found.
      if (q == node) break;

      last = dir;
      dir = q < node;

      // Update helpers.
      if (g != NULL) t = g;
      g = p;
      p = q;
      q = q->node[dir];
    }

    // Update root.
    rootNode = head.node[1];
  }

  // Make root black.
  rootNode->red = 0;
}

bool MemDbg::rbRemove(MemNode* node)
{
  // False tree root.
  MemNode head = { { NULL, NULL }, 0, 0 };

  // Helpers.
  MemNode* q = &head;
  MemNode* p = NULL;
  MemNode* g = NULL;

  // Found item.
  MemNode* f = NULL;
  int dir = 1;

  // Set up.
  q->node[1] = rootNode;

  // Search and push a red down.
  while (q->node[dir] != NULL)
  {
    int last = dir;

    // Update helpers.
    g = p;
    p = q;
    q = q->node[dir];
    dir = q < node;

    // Save found node.
    if (q == node) f = q;

    // Push the red node down.
    if (!rbIsRed(q) && !rbIsRed(q->node[dir]))
    {
      if (rbIsRed(q->node[!dir]))
      {
        p = p->node[last] = rbRotateSingle(q, dir);
      }
      else if (!rbIsRed(q->node[!dir]))
      {
        MemNode* s = p->node[!last];

        if (s != NULL)
        {
          if (!rbIsRed(s->node[!last]) && !rbIsRed(s->node[last]))
          {
            // Color flip.
            p->red = 0;
            s->red = 1;
            q->red = 1;
          }
          else
          {
            int dir2 = g->node[1] == p;

            if (rbIsRed(s->node[last]))
              g->node[dir2] = rbRotateDouble(p, last);
            else if (rbIsRed(s->node[!last]))
              g->node[dir2] = rbRotateSingle(p, last);

            // Ensure correct coloring.
            q->red = g->node[dir2]->red = 1;
            g->node[dir2]->node[0]->red = 0;
            g->node[dir2]->node[1]->red = 0;
          }
        }
      }
    }
  }

  if (f == NULL) return false;

  // Replace and remove.
  FOG_ASSERT(f != &head);
  FOG_ASSERT(q != &head);

  p->node[p->node[1] == q] = q->node[q->node[0] == NULL];

  // Update root and make it black.
  rootNode = head.node[1];
  if (rootNode != NULL) rootNode->red = 0;

  // TODO: This is wrong.

  // Original code was implemented to remove any node, simply copying the data
  // from the removed node. We need to remove the given 'node', not any ohter.
  if (q != node)
  {
    g = NULL;
    p = rootNode;

    for (;;)
    {
      if (p == node)
      {
        if (g)
          g->node[g < node] = q;
        else
          rootNode = q;

        q->node[0] = node->node[0];
        q->node[1] = node->node[1];
        q->red = node->red;
        break;
      }

      g = p;
      p = p->node[p < node];
      FOG_ASSERT(p != NULL);
    }
  }

  return true;
}

bool MemDbg::rbCheck(MemNode* node)
{
  MemNode* cur = rootNode;

  while (cur)
  {
    if (node < cur)
    {
      cur = cur->node[0];
      continue;
    }

    if (node > cur)
    {
      cur = cur->node[1];
      continue;
    }

    break;
  }

  return cur != NULL;
}

// ===========================================================================
// [Fog::MemDbg - Statics]
// ===========================================================================

static Static<MemDbg> memdbg;

// ===========================================================================
// [Fog::MemDbg - Alloc / Realloc / Free]
// ===========================================================================

static void* FOG_CDECL MemMgr_alloc_debug(size_t size)
{
  AutoLock locked(memdbg->lock);
  void* p = ::malloc(size + _MEMDBG_SIZE);

  if (FOG_IS_NULL(p) && size > 0)
  {
    MemMgr::cleanup(MEMMGR_CLEANUP_REASON_NO_MEMORY);
    p = ::malloc(size + _MEMDBG_SIZE);
  }

  if (p != NULL)
    memdbg->insert(p, size);

  return _USER_FROM_DBG(p);
}

static void* FOG_CDECL MemMgr_calloc_debug(size_t size)
{
  AutoLock locked(memdbg->lock);
  void* p = ::calloc(size + _MEMDBG_SIZE, 1);

  if (FOG_IS_NULL(p) && size > 0)
  {
    MemMgr::cleanup(MEMMGR_CLEANUP_REASON_NO_MEMORY);
    p = ::calloc(size + _MEMDBG_SIZE, 1);
  }

  if (p != NULL)
    memdbg->insert(p, size);

  return _USER_FROM_DBG(p);
}

static void* FOG_CDECL MemMgr_realloc_debug(void* p, size_t size)
{
  AutoLock locked(memdbg->lock);
  void* newp;

  if (FOG_IS_NULL(p))
  {
    return fog_api.memmgr_alloc(size);
  }

  p = _DBG_FROM_USER(p);
  memdbg->check(p);

  size_t oldSize = reinterpret_cast<MemNode*>(p)->size;
  memdbg->remove(p);

  if (FOG_UNLIKELY(size == 0))
  {
    fog_api.memmgr_free(p);
    return NULL;
  }

  newp = ::realloc(p, size + _MEMDBG_SIZE);
  if (FOG_IS_NULL(newp))
  {
    MemMgr::cleanup(MEMMGR_CLEANUP_REASON_NO_MEMORY);
    newp = ::realloc(p, size + _MEMDBG_SIZE);
  }

  if (newp == NULL)
    memdbg->insert(p, oldSize);
  else
    memdbg->insert(newp, size);

  return _USER_FROM_DBG(newp);
}

static void FOG_CDECL MemMgr_free_debug(void* p)
{
  AutoLock locked(memdbg->lock);
  if (p == NULL) return;
  p = _DBG_FROM_USER(p);

  memdbg->check(p);
  memdbg->remove(p);

  ::free(p);
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void MemDebug_init(void)
{
  memdbg.init();

  // Replace the default allocators debug versions.
  fog_api.memmgr_alloc = MemMgr_alloc_debug;
  fog_api.memmgr_calloc = MemMgr_calloc_debug;
  fog_api.memmgr_realloc = MemMgr_realloc_debug;
  fog_api.memmgr_free = MemMgr_free_debug;
}

FOG_NO_EXPORT void MemDebug_fini(void)
{
  memdbg->show();
  memdbg.destroy();
}

} // Fog namespace
