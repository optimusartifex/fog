// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLIDMANAGER_H
#define _FOG_CORE_XML_XMLIDMANAGER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Xml_Dom
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct XmlElement;

// ============================================================================
// [Fog::XmlIdManager]
// ============================================================================

//! @internal
//!
//! @brief Hash table to store elemens ID used by the @c XmlDocument. Do not
//! use directly.
struct FOG_API XmlIdManager
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  XmlIdManager();
  ~XmlIdManager();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  void add(XmlElement* e);
  void remove(XmlElement* e);
  XmlElement* get(const StringW& id) const;
  XmlElement* get(const CharW* idStr, size_t idLen) const;

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

private:
  void _rehash(size_t capacity);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Count of buckets.
  size_t _capacity;
  //! @brief Count of nodes.
  size_t _length;

  //! @brief Count of buckets we will expand to if length exceeds _expandLength.
  size_t _expandCapacity;
  //! @brief Count of nodes to grow.
  size_t _expandLength;

  //! @brief Count of buckeds we will shrink to if length gets _shinkLength.
  size_t _shrinkCapacity;
  //! @brief Count of nodes to shrink.
  size_t _shrinkLength;

  //! @brief Buckets.
  XmlElement** _buckets;
  //! @brief Initial buckets up to 16 elements with id attribute
  //! (for very small documents).
  XmlElement* _bucketsBuffer[16];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLIDMANAGER_H
