// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/UI/Engine/FbEngine.h>
#include <Fog/UI/Engine/FbWindow.h>

namespace Fog {

// ============================================================================
// [Fog::FbWindowData]
// ============================================================================

// ============================================================================
// [Fog::FbWindow]
// ============================================================================

// ============================================================================
// [Fog::FbWindow - Window Stack]
// ============================================================================

void FbWindow::moveToTop(FbWindow* w)
{
  void* handle = NULL;

  if (w != NULL)
    handle = w->getHandle();
  
  return _d->moveToTop(handle);
}

void FbWindow::moveToBottom(FbWindow* w)
{
  void* handle = NULL;

  if (w != NULL)
    handle = w->getHandle();
  
  return _d->moveToBottom(handle);
}

// ============================================================================
// [Fog::FbWindow - Window Coordinates]
// ============================================================================

err_t FbWindow::worldToClient(PointI& pt) const
{
  return _d->worldToClient(pt);
}

err_t FbWindow::clientToWorld(PointI& pt) const
{
  return _d->clientToWorld(pt);
}

// ============================================================================
// [Fog::FbWindow - Events]
// ============================================================================
 
void FbWindow::onFbEvent(FbEvent* ev)
{
  // Nothing to do here.
}

} // Fog namespace
