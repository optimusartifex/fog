// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/UI/Engine/GuiEngine.h>
#include <Fog/UI/Widget/Window.h>

FOG_IMPLEMENT_OBJECT(Fog::Window)

namespace Fog {

// ============================================================================
// [Fog::Window]
// ============================================================================

Window::Window(uint32_t createFlags) :
  Widget((createFlags & WINDOW_TYPE_MASK) != 0 ? createFlags : (createFlags | WINDOW_TYPE_DEFAULT))
{
  setFocusPolicy(FOCUS_STRONG);
}

Window::~Window()
{
}

void Window::onPaint(PaintEvent* e)
{
}

} // Fog namespace
