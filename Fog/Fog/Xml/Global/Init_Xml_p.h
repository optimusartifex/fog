// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_XML_GLOBAL_INIT_P_H
#define _FOG_XML_GLOBAL_INIT_P_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

// ============================================================================
// [Fog::Xml - Init / Fini]
// ============================================================================

namespace Fog {

// [Fog/Xml]
FOG_NO_EXPORT void _xml_init(void);
FOG_NO_EXPORT void _xml_fini(void);

// [Fog/Xml/Util]
FOG_NO_EXPORT void _xml_xmlentity_init(void);

} // Fog namespace

// [Guard]
#endif // _FOG_XML_GLOBAL_INIT_P_H
