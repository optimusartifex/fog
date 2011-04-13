// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_MAC)

#include <Fog/Core/Mac/MacUtil.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/TextCodec.h>

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
