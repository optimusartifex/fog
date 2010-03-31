// [Fog-Xml Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_XMLWRITER_H
#define _FOG_XML_XMLWRITER_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/TextCodec.h>
#include <Fog/Xml/Constants.h>

//! @addtogroup Fog_Xml
//! @{

namespace Fog {

// ============================================================================
// [Fog::XmlWriter]
// ============================================================================

struct FOG_API XmlWriter
{
  XmlWriter();
  virtual ~XmlWriter();
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_XML_XMLWRITER_H
