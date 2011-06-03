// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_COLLECTION_UTIL_H
#define _FOG_CORE_COLLECTION_UTIL_H

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>

namespace Fog {
namespace Util {

//! @addtogroup Fog_Core_Collection
//! @{

// ============================================================================
// [Fog::Util]
// ============================================================================

size_t getGrowCapacity(size_t headerSize, size_t elementSize, size_t before, size_t after);

//! @}

} // Util namespace
} // Fog namespace

// [Guard]
#endif // _FOG_CORE_COLLECTION_UTIL_H
