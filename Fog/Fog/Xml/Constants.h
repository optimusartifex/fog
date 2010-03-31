// [Fog-Xml Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_CONSTANTS_H
#define _FOG_XML_CONSTANTS_H

// [Dependencies]
#include <Fog/Build/Build.h>

//! @addtogroup Fog_Xml
//! @{

namespace Fog {

// ============================================================================
// [Fog::XML_ELEMENT_TYPE]
// ============================================================================

//! @brief Element type id.
enum XML_ELEMENT_TYPE
{
  XML_ELEMENT_BASE = 0x01,
  XML_ELEMENT_TEXT = 0x03,
  XML_ELEMENT_CDATA = 0x04,
  XML_ELEMENT_PI = 0x07,
  XML_ELEMENT_COMMENT = 0x08,
  XML_ELEMENT_DOCUMENT = 0x09,

  XML_ELEMENT_MASK = 0x0F,

  // Svg support.
  SVG_ELEMENT_MASK = 0x10,
  SVG_ELEMENT_BASE = XML_ELEMENT_BASE | SVG_ELEMENT_MASK,
  SVG_ELEMENT_DOCUMENT = XML_ELEMENT_DOCUMENT | SVG_ELEMENT_MASK
};

// ============================================================================
// [Fog::XML_ELEMENT_FLAGS]
// ============================================================================

enum XML_FLAGS
{
  //! @brief Whether element can be manipulated (DOM).
  XML_ALLOWED_DOM_MANIPULATION = 0x01,
  //! @brief Whether element tag name can be changed.
  XML_ALLOWED_TAG = 0x02,
  //! @brief Whether element supports attributes.
  XML_ALLOWED_ATTRIBUTES = 0x04
};

// ============================================================================
// [Fog::ERR_XML]
// ============================================================================

//! @brief Error codes used in Fog-Xml.
enum ERR_XML_ENUM
{
  // Errors Range.
  ERR_XML_START = 0x00011000,
  ERR_XML_LAST  = 0x000110FF,

  ERR_XML_INTERNAL = ERR_XML_START,

  // XmlDom Errors.
  ERR_XML_DOCUMENT_INVALID_CHILD,
  ERR_XML_MANUPULATION_NOT_ALLOWED,
  ERR_XML_TAG_CHANGE_NOT_ALLOWED,
  ERR_XML_ATTRIBUTES_NOT_ALLOWED,
  ERR_XML_NOT_A_TEXT_NODE,
  ERR_XML_ATTRIBUTE_NOT_EXISTS,
  ERR_XML_ATTRIBUTE_CANT_BE_REMOVED,
  ERR_XML_INVALID_ATTRIBUTE,
  ERR_XML_CYCLIC,
  ERR_XML_INVALID_CHILD,
  ERR_XML_INVALID_TAG_NAME,
  ERR_XML_DOCUMENT_HAS_ALREADY_ROOT,

  // XmlReader Errors.
  ERR_XML_NO_DOCUMENT,
  ERR_XML_MISSING_ROOT_TAG,
  ERR_XML_MISSING_TAG,
  ERR_XML_MISSING_ATTRIBUTE,
  ERR_XML_UNMATCHED_CLOSING_TAG,
  ERR_XML_UNCLOSED_CDATA,
  ERR_XML_UNCLOSED_PI,
  ERR_XML_UNCLOSED_COMMENT,
  ERR_XML_UNCLOSED_DOCTYPE,
  ERR_XML_SYNTAX_ERROR,

  ERR_XML_INVALID_CLOSING_TAG,
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_XML_CONSTANTS_H
