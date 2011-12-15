// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

// [Guard]
#ifndef _FOG_SVG_H
#define _FOG_SVG_H

// ============================================================================
// [Fog/Svg]
// ============================================================================

//! @defgroup Fog_Svg Fog-Svg
//!
//! Public classes and functions related to Fog-Svg library.

// ============================================================================
// [Fog/Svg/Global]
// ============================================================================

//! @defgroup Fog_Svg_Global SVG global definitions
//! @ingroup Fog_Svg

#include <Fog/Core/Global/Global.h>

// ============================================================================
// [Fog/Svg/Dom]
// ============================================================================

//! @defgroup Fog_Svg_Dom SVG-DOM
//! @ingroup Fog_Svg

#include <Fog/Svg/Dom/SvgDocument.h>
#include <Fog/Svg/Dom/SvgElement.h>

// ============================================================================
// [Fog/Svg/Tools]
// ============================================================================

//! @defgroup Fog_Svg_Tools SVG tools (parsing and serializing)
//! @ingroup Fog_Svg

#include <Fog/Svg/Tools/SvgUtil.h>

// ============================================================================
// [Fog/Svg/Visit]
// ============================================================================

//! @defgroup Fog_Svg_Visit SVG visitors
//! @ingroup Fog_Svg

#include <Fog/Svg/Visit/SvgHitTest.h>
#include <Fog/Svg/Visit/SvgMeasure.h>
#include <Fog/Svg/Visit/SvgRender.h>
#include <Fog/Svg/Visit/SvgVisitor.h>

// [Guard]
#endif // _FOG_SVG_H
