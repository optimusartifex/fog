// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_DOM_XMLIDMANAGER_H
#define _FOG_XML_DOM_XMLIDMANAGER_H

// [Dependencies]
#include <Fog/Core/Tools/String.h>
#include <Fog/Xml/Global/Constants.h>

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
  XmlElement* get(const String& id) const;
  XmlElement* get(const Char* idStr, sysuint_t idLen) const;

  // --------------------------------------------------------------------------
  // [Internal]
  // --------------------------------------------------------------------------

private:
  void _rehash(sysuint_t capacity);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

protected:
  //! @brief Count of buckets.
  sysuint_t _capacity;
  //! @brief Count of nodes.
  sysuint_t _length;

  //! @brief Count of buckets we will expand to if length exceeds _expandLength.
  sysuint_t _expandCapacity;
  //! @brief Count of nodes to grow.
  sysuint_t _expandLength;

  //! @brief Count of buckeds we will shrink to if length gets _shinkLength.
  sysuint_t _shrinkCapacity;
  //! @brief Count of nodes to shrink.
  sysuint_t _shrinkLength;

  //! @brief Buckets.
  XmlElement** _buckets;
  //! @brief Initial buckets up to 16 elements with id attribute
  //! (for very small documents).
  XmlElement* _bucketsBuffer[16];
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_XML_DOM_XMLIDMANAGER_H
