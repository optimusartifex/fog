// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLSAXWRITER_H
#define _FOG_CORE_XML_XMLSAXWRITER_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextCodec.h>

namespace Fog {

//! @addtogroup Fog_Xml_IO
//! @{

// ============================================================================
// [Fog::XmlSaxWriter]
// ============================================================================

struct FOG_API XmlSaxWriter
{
  XmlSaxWriter();
  virtual ~XmlSaxWriter();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_XML_XMLSAXWRITER_H
