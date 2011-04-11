// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/Gui/Engine/GuiEngine.h>
#include <Fog/Gui/Widget/Window.h>

#include <stdio.h>
#include <stdlib.h>

FOG_IMPLEMENT_OBJECT(Fog::Window)

namespace Fog {

// ============================================================================
// [Fog::Window]
// ============================================================================

Window::Window(uint32_t createFlags) :
  Composite((createFlags & WINDOW_TYPE_MASK) != 0 ? createFlags : (createFlags | WINDOW_TYPE_DEFAULT))
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
