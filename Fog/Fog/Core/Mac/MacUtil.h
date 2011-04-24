// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MACUTIL_H
#define _FOG_CORE_MACUTIL_H

#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_MAC)

// [Dependencies]
#include <Fog/Core/Tools/String.h>

#ifdef __OBJC__
@class NSAutoreleasePool;
@class NSString;
#else
class NSAutoreleasePool;
class NSString;
#endif

namespace Fog {

//! @brief ScopedAutoreleasePool allocates an NSAutoreleasePool on instatiation and
//! releases it on destruction.
struct ScopedAutoreleasePool
{
  ScopedAutoreleasePool();
  ~ScopedAutoreleasePool();

  //! @brief Get cleaned up and start new.
  void recycle();

private:
  FOG_DISABLE_COPY(ScopedAutoreleasePool)

  NSAutoreleasePool* _pool;
};

// =================================================================================
// [Cocoa String Conversions]
// =================================================================================

String fromNSString(NSString* str);
NSString* toNSString(const String& str);

} // Fog namespace

#else
#warning "Fog::MacUtil - Mac support not enabled, but header file included!"
#endif // FOG_OS_MAC

// [Guard]
#endif // _FOG_CORE_MACUTIL_H
