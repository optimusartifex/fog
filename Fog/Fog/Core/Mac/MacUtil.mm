// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Build.h>

#if defined(FOG_OS_MAC)

#include <Fog/Core/MacUtil.h>
#include <Fog/Core/String.h>
#include <Fog/Core/ByteArray.h>
#include <Fog/Core/TextCodec.h>

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

// ============================================================================

// TODO: Why UTF8.
NSString* toNSString(const String& str)
{
  ByteArray tmp;
  TextCodec::local8().fromUnicode(tmp, str);
  return [[NSString alloc] initWithCString:tmp.getData() encoding:NSUTF8StringEncoding];
}

// TODO: Why UTF8.
String fromNSString(NSString* str) 
{
	return (Char*)[str UTF8String];
}

} // Fog namespace

#endif // defined(FOG_OS_MAC)
