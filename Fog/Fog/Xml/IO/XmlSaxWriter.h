// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_IO_XMLSAXWRITER_H
#define _FOG_XML_IO_XMLSAXWRITER_H

// [Dependencies]
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Xml/Global/Constants.h>

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
#endif // _FOG_XML_IO_XMLSAXWRITER_H
