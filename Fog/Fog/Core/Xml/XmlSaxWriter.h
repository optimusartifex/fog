// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_XML_XMLSAXWRITER_H
#define _FOG_CORE_XML_XMLSAXWRITER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextCodec.h>

namespace Fog {

//! @addtogroup Fog_Core_Xml
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
