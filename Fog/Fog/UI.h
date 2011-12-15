// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

// [Guard]
#ifndef _FOG_UI_H
#define _FOG_UI_H

// ============================================================================
// [Fog/UI]
// ============================================================================

//! @defgroup Fog_UI Fog/UI
//!
//! Public classes and functions related to Fog/UI module.

#include <Fog/Core/Global/Global.h>

// ============================================================================
// [Fog/UI/Engine]
// ============================================================================

//! @defgroup Fog_UI_Engine Engine
//! @ingroup Fog_UI

#include <Fog/UI/Engine/FbDisplayInfo.h>
#include <Fog/UI/Engine/FbEngine.h>
#include <Fog/UI/Engine/FbEvent.h>
#include <Fog/UI/Engine/FbKeyboardState.h>
#include <Fog/UI/Engine/FbMouseState.h>
#include <Fog/UI/Engine/FbPaletteInfo.h>
#include <Fog/UI/Engine/FbSecondary.h>
#include <Fog/UI/Engine/FbWindow.h>

#include <Fog/UI/Engine/GuiEngine.h>

// ============================================================================
// [Fog/UI/Layout]
// ============================================================================

//! @defgroup Fog_UI_Layout Layout management
//! @ingroup Fog_UI

#include <Fog/UI/Layout/BorderLayout.h>
#include <Fog/UI/Layout/BoxLayout.h>
#include <Fog/UI/Layout/FlowLayout.h>
#include <Fog/UI/Layout/GridLayout.h>
#include <Fog/UI/Layout/Layout.h>
#include <Fog/UI/Layout/LayoutHint.h>
#include <Fog/UI/Layout/LayoutItem.h>
#include <Fog/UI/Layout/LayoutPolicy.h>
#include <Fog/UI/Layout/LayoutUtil.h>
#include <Fog/UI/Layout/Margin.h>

// ============================================================================
// [Fog/UI/Tools]
// ============================================================================

//! @defgroup Fog_UI_Tools Tools
//! @ingroup Fog_UI

#include <Fog/UI/Tools/KeyUtil.h>

// ============================================================================
// [Fog/UI/Widget]
// ============================================================================

//! @defgroup Fog_UI_Widget Widgets
//! @ingroup Fog_UI

#include <Fog/UI/Widget/Event.h>
#include <Fog/UI/Widget/Widget.h>
#include <Fog/UI/Widget/Window.h>

// [Guard]
#endif // _FOG_UI_H
