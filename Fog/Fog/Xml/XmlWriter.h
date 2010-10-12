// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_XMLWRITER_H
#define _FOG_XML_XMLWRITER_H

// [Dependencies]
#include <Fog/Core/Build.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Xml/Constants.h>

namespace Fog {

//! @addtogroup Fog_Xml_IO
//! @{

// ============================================================================
// [Fog::XmlWriter]
// ============================================================================

struct FOG_API XmlWriter
{
  XmlWriter();
  virtual ~XmlWriter();
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_XML_XMLWRITER_H
