// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

// [Guard]
#ifndef _FOG_GUI_H
#define _FOG_GUI_H

#if defined(FOG_DOXYGEN)

// ============================================================================
// [Fog-Gui Groups]
// ============================================================================

//! @defgroup Fog_Gui_Public Fog-Gui
//!
//! Public classes and functions related to Fog-Gui library.



//! @defgroup Fog_Gui_Constants Constants
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Engine Engine
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Layout Layout managers
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Theming Theming
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Widget Widgets
//! @ingroup Fog_Gui_Public



//! @defgroup Fog_Gui_Win Windows specific
//! @ingroup Fog_Gui_Public
//!
//! This module contains specific part of UI system for Windows.



//! @defgroup Fog_Gui_X11 X11 specific
//! @ingroup Fog_Gui_Public
//!
//! This module contains specific part of UI system for X Window System (xlib).



#endif // FOG_DOXYGEN

// ============================================================================
// [Fog-Gui Include Files]
// ============================================================================

#include <Fog/Core/Config/Config.h>

#include <Fog/Gui/Animation/Animation.h>
#include <Fog/Gui/Engine/GuiEngine.h>
#include <Fog/Gui/Global/Constants.h>
#include <Fog/Gui/Layout/BorderLayout.h>
#include <Fog/Gui/Layout/BoxLayout.h>
#include <Fog/Gui/Layout/FlowLayout.h>
#include <Fog/Gui/Layout/GridLayout.h>
#include <Fog/Gui/Layout/Layout.h>
#include <Fog/Gui/Layout/LayoutHint.h>
#include <Fog/Gui/Layout/LayoutItem.h>
#include <Fog/Gui/Layout/LayoutPolicy.h>
#include <Fog/Gui/Layout/LayoutUtil.h>
#include <Fog/Gui/Layout/Margin.h>
#include <Fog/Gui/Theming/ThemeEngine.h>
#include <Fog/Gui/Widget/Button.h>
#include <Fog/Gui/Widget/ButtonBase.h>
#include <Fog/Gui/Widget/CheckBox.h>
#include <Fog/Gui/Widget/Composite.h>
#include <Fog/Gui/Widget/Event.h>
#include <Fog/Gui/Widget/Frame.h>
#include <Fog/Gui/Widget/GroupBox.h>
#include <Fog/Gui/Widget/MenuBar.h>
#include <Fog/Gui/Widget/MenuItem.h>
#include <Fog/Gui/Widget/MenuSeparator.h>
#include <Fog/Gui/Widget/RadioButton.h>
#include <Fog/Gui/Widget/ScrollArea.h>
#include <Fog/Gui/Widget/ScrollBar.h>
#include <Fog/Gui/Widget/TextArea.h>
#include <Fog/Gui/Widget/TextField.h>
#include <Fog/Gui/Widget/ToolBar.h>
#include <Fog/Gui/Widget/ToolButton.h>
#include <Fog/Gui/Widget/Widget.h>
#include <Fog/Gui/Widget/Window.h>

// [Guard]
#endif // _FOG_GUI_H
