// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COLLECTION_PSTACK_H
#define _FOG_CORE_COLLECTION_PSTACK_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Memory/Memory.h>

namespace Fog {

//! @addtogroup Fog_Core_Collection
//! @{

// ============================================================================
// Fog::PStack<>
// ============================================================================

//! @brief Fast and secure stack implementation for performance-critical
//! operations.
template<sysuint_t N>
struct PStack
{
  // --------------------------------------------------------------------------
  // [Node]
  // --------------------------------------------------------------------------

  struct Node
  {
    //! @brief Pointer to current position in buffer for this node.
    uint8_t* cur;
    //! @brief Pointer to next node.
    Node* next;
    //! @brief Pointer to previous node.
    Node* prev;
    //! @brief Remaining bytes in bufer.
    sysuint_t remain;
    //! @brief Node data.
    uint8_t buffer[N];
  };

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  FOG_INLINE PStack()
  {
    _current = &_first;
    _first.prev = NULL;
    _first.next = NULL;
    _first.cur = _first.buffer;
    _first.remain = N;
  }

  FOG_INLINE ~PStack()
  {
    Node* node = _first.next;
    while (node)
    {
      Node* next = node->next;
      Memory::free(node);
      node = next;
    }
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const
  {
    return (_current == &_first && _first.cur == _first.buffer);
  }

  // --------------------------------------------------------------------------
  // [Push / Pop]
  // --------------------------------------------------------------------------

  template<class T>
  FOG_INLINE err_t push(T& data)
  {
    if (FOG_UNLIKELY(_current->remain < sizeof(T)))
    {
      if (_current->next == NULL)
      {
        Node* node = (Node*)Memory::alloc(sizeof(Node) - N + (1024*32));
        if (FOG_UNLIKELY(node == NULL)) return ERR_RT_OUT_OF_MEMORY;

        node->cur = node->buffer;
        node->prev = _current;
        node->next = NULL;
        node->remain = 1024*32;
        _current->next = node;
        _current = node;
      }
      else
      {
        _current = _current->next;
      }
    }

    *(T *)(_current->cur) = data;
    _current->cur += sizeof(T);
    _current->remain -= sizeof(T);

    return ERR_OK;
  }

  template<class T>
  FOG_INLINE void pop(T& data)
  {
    if (FOG_UNLIKELY(_current->cur == _current->buffer))
    {
      _current = _current->prev;
    }

    FOG_ASSERT(_current->cur != _current->buffer);
    _current->cur -= sizeof(T);
    _current->remain += sizeof(T);
    data = *(T *)(_current->cur);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  Node* _current;
  Node _first;

private:
  _FOG_CLASS_NO_COPY(PStack)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_COLLECTION_PSTACK_H
