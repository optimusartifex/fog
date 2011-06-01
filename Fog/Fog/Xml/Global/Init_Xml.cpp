// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Xml/Global/Init_Xml_p.h>

namespace Fog {

// ============================================================================
// [Fog::Xml - Library Initializers]
// ============================================================================

FOG_NO_EXPORT void _xml_init(void)
{
  _xml_xmlentity_init();
}

FOG_NO_EXPORT void _xml_fini(void)
{
}

} // Fog namespace
