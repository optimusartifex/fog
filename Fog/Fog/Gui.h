// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSVC)
#pragma once
#endif // _MSVC

// [Guard]
#ifndef _FOG_GUI_H
#define _FOG_GUI_H

#if defined(_DOXYGEN)

// ============================================================================
// [Fog-Gui Groups]
// ============================================================================

//! @defgroup Fog_Gui Fog-Gui library API.
//!
//! Set of Fog-Gui library classes and their members.

//! @defgroup Fog_Gui_Win Fog-Gui Windows module API.
//!
//! This module contains specific part of UI system for Windows.

//! @defgroup Fog_Gui_X11 Fog-Gui X11 module API.
//!
//! This module contains specific part of UI system for X Window System (xlib).
#endif // _DOXYGEN

// ============================================================================
// [Fog-Gui Include Files]
// ============================================================================

#include <Fog/Build/Build.h>

#include <Fog/Gui/Button.h>
#include <Fog/Gui/ButtonBase.h>
#include <Fog/Gui/CheckBox.h>
#include <Fog/Gui/Composite.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/Event.h>
#include <Fog/Gui/GuiEngine.h>
#include <Fog/Gui/Layout/BorderLayout.h>
#include <Fog/Gui/Layout/BoxLayout.h>
#include <Fog/Gui/Layout/FlowLayout.h>
#include <Fog/Gui/Layout/GridLayout.h>
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Gui/Layout/LayoutHint.h>
#include <Fog/Gui/Layout/LayoutItem.h>
#include <Fog/Gui/Theme.h>
#include <Fog/Gui/Widget.h>
#include <Fog/Gui/Window.h>
#include <Fog/Gui/Animation.h>

// [Guard]
#endif // _FOG_GUI_H
