// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Mac/MacScopedAutoReleasePool.h>

// [Cocoa]
#import <Cocoa/Cocoa.h>

namespace Fog {
  
// ============================================================================
// [Fog::MacScopedAutoReleasePool]
// ============================================================================

MacScopedAutoReleasePool::MacScopedAutoReleasePool()
{
  _pool = [[NSAutoreleasePool alloc] init];
}

MacScopedAutoReleasePool::~MacScopedAutoReleasePool()
{
  [_pool drain];
}

void MacScopedAutoReleasePool::recycle()
{
  [_pool drain];
  _pool = [[NSAutoreleasePool alloc] init];
}
  
} // Fog namespace
