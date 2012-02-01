// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_XMLENTITY_P_H
#define _FOG_CORE_TOOLS_XMLENTITY_P_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/String.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::XmlEntity]
// ============================================================================

//! @internal.
//!
//! @brief Static class that allows to encode and decode built-in xml named
//! entities.
struct FOG_NO_EXPORT XmlEntity
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
  static FOG_INLINE size_t pairsCount() { return _pairsCount; }

  //! @brief Decode named entity into unicode character.
  static CharW decode(const char* entityName, size_t entityLength = DETECT_LENGTH);
  //! @overload.
  static FOG_INLINE CharW decode(const StringA& entityName) { return decode(entityName.getData(), entityName.getLength()); }

  //! @brief Decode named entity into unicode character.
  static CharW decode(const CharW* entityName, size_t entityLength = DETECT_LENGTH);
  //! @overload.
  static FOG_INLINE CharW decode(const StringW& entityName) { return decode(entityName.getData(), entityName.getLength()); }

  //! @brief Encode unicode character into named entity.
  //! @note Length of @a dest must be at least 16 characters.
  static size_t encode(char* dst, CharW ch);

  static const Pair* _pairs;
  static size_t _pairsCount;
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_XMLENTITY_P_H
