// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_XMLENTITY_H
#define _FOG_XML_XMLENTITY_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/Char.h>
#include <Fog/Core/String.h>

namespace Fog {

//! @addtogroup Fog_Xml_Private
//! @{

// ============================================================================
// [Fog::XmlEntity]
// ============================================================================

//! @internal.
//!
//! @brief Static class that allows to encode and decode built-in xml named
//! entities.
struct FOG_HIDDEN XmlEntity
{
  //! @internal.
  //!
  //! @brief Xml entity structure, Entity <=> Character
  struct Pair
  {
    //! @brief Ascii entity name.
    char name[10];
    //! @brief Unicode representation (16 bit).
    uint16_t ch;
  };

  //! @internal.
  //!
  //! @brief Reversed pair structure.
  struct PairRev
  {
    //! @brief Character value.
    uint16_t ch;
    //! @brief Link to real entity position in @c Pair array.
    uint16_t link;
  };

  //! @brief Get all xml entity pairs defined by Fog library.
  static FOG_INLINE const Pair* pairs() { return _pairs; }

  //! @brief Get count of entity pairs returned by @c pairs() method.
  static FOG_INLINE sysuint_t pairsCount() { return _pairsCount; }

  //! @brief Decode named entity into unicode character.
  static Char decode(const char* entityName, sysuint_t entityLength = DETECT_LENGTH);
  //! @overload.
  static FOG_INLINE Char decode(const ByteArray& entityName) { return decode(entityName.getData(), entityName.getLength()); }

  //! @brief Decode named entity into unicode character.
  static Char decode(const Char* entityName, sysuint_t entityLength = DETECT_LENGTH);
  //! @overload.
  static FOG_INLINE Char decode(const String& entityName) { return decode(entityName.getData(), entityName.getLength()); }

  //! @brief Encode unicode character into named entity.
  //! @note Length of @a dest must be at least 16 characters.
  static sysuint_t encode(char* dst, Char ch);

  static const Pair* _pairs;
  static sysuint_t _pairsCount;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_XML_XMLENTITY_H
