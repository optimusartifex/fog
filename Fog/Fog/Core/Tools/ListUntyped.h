// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LISTUNTYPED_H
#define _FOG_CORE_TOOLS_LISTUNTYPED_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::ListUntypedVTable]
// ============================================================================

struct ListUntypedVTable
{
  // --------------------------------------------------------------------------
  // [Typedefs]
  // --------------------------------------------------------------------------

  typedef void (FOG_CDECL *Ctor)(void* dst, const void* src, size_t length);
  typedef void (FOG_CDECL *Dtor)(void* dst, size_t length);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t szItemT;
  Ctor ctor;
  Dtor dtor;
};
// ===========================================================================
// [Fog::ListUntypedData]
// ===========================================================================

struct FOG_NO_EXPORT ListUntypedData
{
  // -------------------------------------------------------------------------
  // [AddRef]
  // -------------------------------------------------------------------------

  FOG_INLINE ListUntypedData* addRef() const
  {
    reference.inc();
    return const_cast<ListUntypedData*>(this);
  }

  // -------------------------------------------------------------------------
  // [Accessors]
  // -------------------------------------------------------------------------

  //! @brief Get const data pointer which points to the raw start of list-data.
  FOG_INLINE const char* getArray() const
  {
    return reinterpret_cast<const char*>(this) + sizeof(ListUntypedData);
  }

  //! @brief Get mutable data pointer which points to the raw start of list-data.
  FOG_INLINE char* getArray()
  {
    return reinterpret_cast<char*>(this) + sizeof(ListUntypedData);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  // ${VAR:BEGIN}
  //
  // This data-object is binary compatible with the VarData header in the second
  // form called - "implicitly shared container". The members must be binary
  // compatible with the header below:
  //
  // +==============+============+============================================+
  // | Size         | Name       | Description / Purpose                      |
  // +==============+============+============================================+
  // | size_t       | reference  | Atomic reference count, can be managed by  |
  // |              |            | VarData without calling container specific |
  // |              |            | methods.                                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | vType      | Variable type and flags.                   |
  // +--------------+------------+--------------------------------------------+
  // | uint32_t     | padding0_32| Not used by the Var. This member is only   |
  // |              |            | defined for 64-bit compilation to pad      |
  // |              |            | other members!                             |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | capacity   | Capacity of the container (items).         |
  // +--------------+------------+--------------------------------------------+
  // | size_t       | length     | Length of the container (items).           |
  // +==============+============+============================================+
  //
  // List of possible variable types:
  //
  // +===========================+============================================+
  // | VAR_TYPE_...              | Description / Purpose                      |
  // +===========================+============================================+
  // | UNKNOWN                   | List<?>, where the type is unknown or not  |
  // |                           | supported by Var. Instance of this list    |
  // |                           | can't be casted to the Var or List<Var>.   |
  // +---------------------------+--------------------------------------------+
  // | LIST_STRINGA              | List<StringA>, typed list compatible with  |
  // |                           | Var and List<Var>.                         |
  // +---------------------------+--------------------------------------------+
  // | LIST_STRINGW              | List<StringW>, typed list compatible with  |
  // |                           | Var and List<Var>.                         |
  // +---------------------------+--------------------------------------------+
  // | LIST_VAR                  | List<Var>, typed list compatible with Var. |
  // +===========================+============================================+
  //
  // ${VAR:END}

  //! @brief Reference count.
  mutable Atomic<size_t> reference;

  //! @brief Variable type and flags.
  uint32_t vType;

#if FOG_ARCH_BITS >= 64
  //! @brief Padding (0.32).
  uint32_t padding0_32;
#endif // FOG_ARCH_BITS >= 64

  //! @brief Capacity.
  size_t capacity;
  //! @brief Length.
  size_t length;

  // --------------------------------------------------------------------------
  // [...]
  // --------------------------------------------------------------------------

  //! @brief The index of the first item in the list.
  size_t start;
  //! @brief The index of the last item in the list + 1.
  size_t end;

  //! @brief Pointer to the first item (getArray() + start * sizeof(ItemT)).
  char* data;

#if FOG_ARCH_BITS >= 64
  //! @brief Padding (1.64).
  uint64_t padding1_64;
#else
  //! @brief Padding (1.32).
  uint32_t padding1_32;
#endif // FOG_ARCH_BITS >= 64
};

// ===========================================================================
// [Fog::ListUntyped]
// ===========================================================================

struct FOG_NO_EXPORT ListUntyped
{
  // --------------------------------------------------------------------------
  // [Sharing]
  // --------------------------------------------------------------------------

  //! @copydoc Doxygen::Implicit::getReference().
  FOG_INLINE size_t getReference() const { return _d->reference.get(); }
  //! @copydoc Doxygen::Implicit::isDetached().
  FOG_INLINE bool isDetached() const { return getReference() == 1; }

  // --------------------------------------------------------------------------
  // [Container]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getCapacity() const { return _d->capacity; }
  FOG_INLINE size_t getLength() const { return _d->length; }

  FOG_INLINE bool isEmpty() const { return _d->length == 0; }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const void* _getUntypedData() const
  {
    return _d->data;
  }

  // -------------------------------------------------------------------------
  // [Members]
  // -------------------------------------------------------------------------

  _FOG_CLASS_D(ListUntypedData)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_LISTUNTYPED_H
