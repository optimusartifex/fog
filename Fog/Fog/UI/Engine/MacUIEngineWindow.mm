// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/OS/OSUtil.h>
#include <Fog/UI/Engine/UIEngineEvent.h>
#include <Fog/UI/Engine/MacUIEngine.h>
#include <Fog/UI/Engine/MacUIEngineWindow.h>

// [Dependencies - Mac]
#import <Foundation/Foundation.h>

#if defined(FOG_OS_IOS)
# import <UIKit/UIKit.h>
#else
# import <AppKit/AppKit.h>
# import <Cocoa/Cocoa.h>
#endif // FOG_OS_IOS

// ==============================================================================
// [Util]
// ==============================================================================

namespace Fog {

static FOG_INLINE Fog::RectI RectI_fromNSRect(const NSRect& nsRect)
{
  return RectI(int(nsRect.origin.x),
               int(nsRect.origin.y),
               int(nsRect.size.width),
               int(nsRect.size.height));
}

static FOG_INLINE Fog::PointI PointI_fromNSPoint(const NSPoint& nsPoint)
{
  return PointI(int(nsPoint.x), int(nsPoint.y));
}

} // Fog namespace

// ==============================================================================
// [Fog_MacNSView]
// ==============================================================================

@interface Fog_MacNSView : NSView
{
@public
  Fog::MacUIEngineWindowImpl* _d;
}
@end

@implementation Fog_MacNSView

- (id)init: (Fog::MacUIEngineWindowImpl*)d
{
  if (self = [super init])
  {
    _d = d;
  }

  return self;
}

- (void)drawRect:(NSRect)rect
{
  CGImageRef cgImage = static_cast<CGImageRef>(_d->_bufferImage.getHandle());
  CGContextRef ctx = static_cast<CGContextRef>(
    [[NSGraphicsContext currentContext] graphicsPort]);

  CGContextDrawImage(ctx, NSRectToCGRect(rect), cgImage);
}
@end

// ==============================================================================
// [Fog_MacNSWindow]
// ==============================================================================

@interface Fog_MacNSWindow : NSWindow
{
@public
  Fog::MacUIEngineWindowImpl* _d;
}
- (Fog_MacNSView*) contentView;
@end

@implementation Fog_MacNSWindow

- (id)init       : (Fog::MacUIEngineWindowImpl*)d
      frame      : (NSRect)frame
      styleMask  : (NSUInteger)style
      contentView: (NSView*)view
{
  if (self = [super initWithContentRect: frame styleMask: style backing: NSBackingStoreBuffered defer: NO])
  {
    [self setContentView:view];

    // Initialize notifications.
    NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
    
    [center addObserver:self selector:@selector(windowBecameKey:)
            name:NSWindowDidBecomeKeyNotification object:self];

    [center addObserver:self selector:@selector(windowMoved:)
            name:NSWindowDidMoveNotification object:self];
    
    [center addObserver:self selector:@selector(windowResized:)
            name:NSWindowDidResizeNotification object:self];
    
    _d = d;
  }
  return self;
}

- (void)windowBecameKey:(NSNotification*)notification
{
  Fog::MacUIEngine* engine = reinterpret_cast<Fog::MacUIEngine*>(_d->_engine);

  engine->doStateAction(_d->_window,
    Fog::UI_ENGINE_EVENT_SHOW,
    Fog::WINDOW_STATE_NORMAL);
}

- (void)windowMoved:(NSNotification*)notification
{
  Fog::MacUIEngine* engine = reinterpret_cast<Fog::MacUIEngine*>(_d->_engine);

  NSRect wr = [self frame];
  NSRect cr = [self contentRectForFrameRect: wr];

  engine->doGeometryAction(_d->_window,
    Fog::UI_ENGINE_EVENT_GEOMETRY,
    Fog::ORIENTATION_HORIZONTAL,
    Fog::RectI_fromNSRect(wr),
    Fog::RectI_fromNSRect(cr));
}

- (void)windowResized:(NSNotification*)notification
{
  Fog::MacUIEngine* engine = reinterpret_cast<Fog::MacUIEngine*>(_d->_engine);

  NSRect wr = [self frame];
  NSRect cr = [self contentRectForFrameRect: wr];

  engine->doGeometryAction(_d->_window,
    Fog::UI_ENGINE_EVENT_GEOMETRY,
    Fog::ORIENTATION_HORIZONTAL,
    Fog::RectI_fromNSRect(wr),
    Fog::RectI_fromNSRect(cr));
}

- (void)keyDown:(NSEvent*)event
{
  // TODO:
}

- (void)keyUp:(NSEvent*)event
{
  // TODO:
}

- (void)mouseMoved:(NSEvent*)event
{
  Fog::MacUIEngine* engine = reinterpret_cast<Fog::MacUIEngine*>(_d->_engine);
  
  uint32_t mId = 0;
  Fog::UIEngineMouseState* mState = &engine->_mouseState[mId];
  
  NSPoint pos = [[self contentView] convertPoint:[event locationInWindow] fromView:nil];

  engine->doMouseAction(_d->_window,
    Fog::UI_ENGINE_EVENT_MOUSE_MOVE,
    0,
    Fog::PointI_fromNSPoint(pos),
    Fog::BUTTON_NONE,
    mState->getButtonMask());
}

- (void)mouseUp:(NSEvent*)event
{
}

- (void)mouseDown:(NSEvent*)event
{
}

- (void)rightMouseUp:(NSEvent*)event
{
}

- (void)rightMouseDown:(NSEvent*)event
{
}

- (Fog_MacNSView*)contentView
{
  return (Fog_MacNSView*)[super contentView];
}

@end

// ============================================================================
// [Fog::]
// ============================================================================

// We can't use namespace for Objective-C declaration, so it's used from here.
namespace Fog {

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Construction / Destruction]
// ============================================================================

MacUIEngineWindowImpl::MacUIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window) :
  UIEngineWindowImpl(engine, window)
{
}

MacUIEngineWindowImpl::~MacUIEngineWindowImpl()
{
  freeDoubleBuffer();
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Create / Destroy]
// ============================================================================

err_t MacUIEngineWindowImpl::create(uint32_t hints)
{
  /*
  NSUInteger nsStyleMask = NSBorderlessWindowMask |
      ( (hints & WINDOW_HINT_HAS_MINIMIZE) ? NSMiniaturizableWindowMask : 0) |
      ( (hints & WINDOW_HINT_HAS_MAXIMIZE) ? NSResizableWindowMask      : 0) |
      ( (hints & WINDOW_HINT_HAS_CLOSE   ) ? NSClosableWindowMask       : 0) |
      (!(hints & WINDOW_HINT_FRAMELESS   ) ? NSTitledWindowMask         : 0) ;
  */
  NSUInteger nsStyleMask =
    NSTitledWindowMask         |
    NSMiniaturizableWindowMask |
    NSResizableWindowMask      |
    NSClosableWindowMask       ;
  NSRect nsFrame = NSMakeRect(0, 0, 500, 500);    

  Fog_MacNSWindow* window = [[Fog_MacNSWindow alloc] init: this
    frame      : nsFrame
    styleMask  : nsStyleMask
    contentView: [[Fog_MacNSView alloc] init: this]
  ];

  _handle = window;
  return ERR_OK;
}

err_t MacUIEngineWindowImpl::destroy()
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);

  // Destroy mapping.
  _engine->removeHandle(_handle);

  // Destroy Window.
  [window performClose: nil];

  // Clear all variables.
  _handle = NULL;

  _isEnabledToParent = false;
  _isVisibleToParent = false;
  _isDirty = false;
  _hasFocus = false;
  _isWindowOpaque = true;

  _shouldUpdate = false;
  _shouldPaint = false;
  _shouldBlit = false;
  
  _resetHints = true;
  _windowDepth = 0;
  _windowState = WINDOW_STATE_NORMAL;
  _windowHints = NO_FLAGS;
  _windowScreen = 0;
  _windowOpacity = 1.0f;
  _windowGeometry.reset();
  _clientGeometry.reset();

  return ERR_OK;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Enabled / Disabled]
// ============================================================================

err_t MacUIEngineWindowImpl::setEnabled(bool enabled)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  // TODO:
  return ERR_OK;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Focus]
// ============================================================================

err_t MacUIEngineWindowImpl::focus()
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  // TODO:
  return ERR_OK;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Window State]
// ============================================================================

err_t MacUIEngineWindowImpl::setState(uint32_t state)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);

  switch (state)
  {
    case WINDOW_STATE_NORMAL:
      [window makeKeyAndOrderFront: nil];
      return ERR_OK;

    case WINDOW_STATE_HIDDEN:
      [window orderOut: nil];
      return ERR_OK;
      
    default:
      // TODO:
      return ERR_RT_NOT_IMPLEMENTED;
  }
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Window Geometry]
// ============================================================================

err_t MacUIEngineWindowImpl::setWindowPosition(const PointI& pos)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);
  [window setFrameTopLeftPoint: NSMakePoint(CGFloat(pos.x), CGFloat(pos.y))];

  return ERR_OK;
}

err_t MacUIEngineWindowImpl::setWindowSize(const SizeI& size)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  NSRect nsFrame = NSMakeRect(
    CGFloat(_windowGeometry.x),
    CGFloat(_windowGeometry.y),
    CGFloat(size.w),
    CGFloat(size.h));

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);
  [window setFrame: nsFrame display: NO];

  return ERR_OK;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Window Stack]
// ============================================================================

err_t MacUIEngineWindowImpl::moveToTop(void* targetHandle)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

err_t MacUIEngineWindowImpl::moveToBottom(void* targetHandle)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  // TODO:
  return ERR_RT_NOT_IMPLEMENTED;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Window Coordinates]
// ============================================================================

err_t MacUIEngineWindowImpl::worldToClient(PointI& pt) const
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);
  NSPoint result = [window convertScreenToBase: NSMakePoint(CGFloat(pt.x), CGFloat(pt.y))];
  
  pt.set(int(result.x), int(result.y));
  return ERR_OK;
}

err_t MacUIEngineWindowImpl::clientToWorld(PointI& pt) const
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_STATE;

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);
  NSPoint result = [window convertBaseToScreen: NSMakePoint(CGFloat(pt.x), CGFloat(pt.y))];
  
  pt.set(int(result.x), int(result.y));
  return ERR_OK;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Window Opacity]
// ============================================================================

err_t MacUIEngineWindowImpl::setOpacity(float opacity)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_HANDLE;

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);
  int opFixed = Math::iround(opacity * 255.0f);

  if (opFixed >= 255)
  {
    [window setOpaque: YES];
    [window setAlphaValue: 1.0f];
  }
  else
  {
    [window setOpaque: NO];
    [window setAlphaValue: opacity];
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Window Title]
// ============================================================================

err_t MacUIEngineWindowImpl::setWindowTitle(const StringW& title)
{
  if (FOG_IS_NULL(_handle))
    return ERR_RT_INVALID_HANDLE;

  Fog_MacNSWindow* window = static_cast<Fog_MacNSWindow*>(_handle);
  NSString* nsTitle;

  FOG_RETURN_ON_ERROR(title.toNSString(&nsTitle));

  [window setTitle: nsTitle];
  [nsTitle release];

  _windowTitle = title;
  return ERR_OK;
}

// ============================================================================
// [Fog::MacUIEngineWindowImpl - Window Frame-Buffer]
// ============================================================================

err_t MacUIEngineWindowImpl::allocDoubleBuffer(const SizeI& size)
{
  uint32_t format = (_windowHints & WINDOW_HINT_COMPOSITE)
    ? IMAGE_FORMAT_PRGB32
    : IMAGE_FORMAT_XRGB32;

  FOG_RETURN_ON_ERROR(_bufferImage.create(size, format, IMAGE_TYPE_MAC_CG));
  _bufferData.setRaw(_bufferImage.getDataX(),
    _bufferImage.getSize(),
    _bufferImage.getFormat(),
    _bufferImage.getStride());
  _bufferType = UI_ENGINE_BUFFER_WIN_DIB;

  return ERR_OK;
}

err_t MacUIEngineWindowImpl::freeDoubleBuffer()
{
  _bufferImage.reset();

  _bufferData.reset();
  _bufferType = UI_ENGINE_BUFFER_NONE;

  _bufferCacheSize.reset();
  _bufferCacheCreated.reset();
  _bufferCacheExpire.reset();

  return ERR_OK;
}

} // Fog namespace
