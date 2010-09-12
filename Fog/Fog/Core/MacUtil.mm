// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Build.h>

#if defined(FOG_OS_MAC)

#include <Fog/Core/MacUtil.h>

#import <Cocoa/Cocoa.h>

namespace Fog {

AutoNSAutoreleasePool::AutoNSAutoreleasePool()
{
    _pool = [[NSAutoreleasePool alloc] init];
}

AutoNSAutoreleasePool::~AutoNSAutoreleasePool()
{
    [_pool release];
}


} // Fog namespace

#endif // defined(FOG_OS_MAC)
