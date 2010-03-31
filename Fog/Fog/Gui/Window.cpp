// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Std.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Gui/GuiEngine.h>
#include <Fog/Gui/Window.h>

#include <stdio.h>
#include <stdlib.h>

FOG_IMPLEMENT_OBJECT(Fog::Window)

namespace Fog {

// [Fog::Window]

Window::Window(uint32_t createFlags) :
Fog::Composite((createFlags & WINDOW_TYPE_MASK) != 0 ? createFlags : (createFlags | WINDOW_TYPE_DEFAULT))
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
