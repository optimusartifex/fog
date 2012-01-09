// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_OS_MACDEFS_H
#define _FOG_CORE_OS_MACDEFS_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// [Dependencies - Mac]
#include <CoreFoundation/CoreFoundation.h>

#if defined(FOG_OS_IOS)
# include <CoreGraphics/CoreGraphics.h>
# include <CoreText/CoreText.h>
#else
# include <ApplicationServices/ApplicationServices.h>
#endif // FOG_OS_IOS

// Please don't include here another Fog-Header files, because this file is 
// included from Fog/Core/Global/Api.h in case that we are building for MAC.
// Maybe we should think about moving it to the Fog/Core/Global subdirectory
// later.

// ============================================================================
// [_FOG_OBJC_CLASS]
// ============================================================================

//! @def _FOG_OBJC_CLASS(_Name_)
//!
//! @brief Forward define an ObjectiveC class which can be referenced by both,
//! C++ code, and ObjectiveC code.
#if defined(__OBJC__)
# define _FOG_OBJC_CLASS(_Name_) @class _Name_
#else
# define _FOG_OBJC_CLASS(_Name_) class _Name_
#endif // __OBJC__

// ============================================================================
// [Forward Declarations]
// ============================================================================

_FOG_OBJC_CLASS(NSAutoreleasePool);
_FOG_OBJC_CLASS(NSEvent);
_FOG_OBJC_CLASS(NSRunLoop);
_FOG_OBJC_CLASS(NSString);

// [Guard]
#endif // _FOG_CORE_OS_MACDEFS_H
