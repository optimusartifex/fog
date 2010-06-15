// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSVC)
#pragma once
#endif // _MSVC

// [Guard]
#ifndef _FOG_GRAPHICS_H
#define _FOG_GRAPHICS_H

#if defined(FOG_DOXYGEN)

// ============================================================================
// [Fog - Graphics - Documentation]
// ============================================================================

//! @defgroup Fog_Graphics_Public Fog-Graphics
//!
//! Public classes and functions related to Fog-Graphics library.

//! @internal
//! @defgroup Fog_Graphics_Private Fog-Graphics (private)
//!
//! Private classes and functions related to Fog-Graphics library.



//! @defgroup Fog_Graphics_Constants Constants
//! @ingroup Fog_Graphics_Public



//! @defgroup Fog_Graphics_Effects Effects
//! @ingroup Fog_Graphics_Public



//! @defgroup Fog_Graphics_Geometry Geometry classes and primitives
//! @ingroup Fog_Graphics_Public



//! @defgroup Fog_Graphics_Imaging Imaging
//! @ingroup Fog_Graphics_Public



//! @defgroup Fog_Graphics_Painting Painting and related classes
//! @ingroup Fog_Graphics_Public



//! @defgroup Fog_Graphics_Text Text and font-management
//! @ingroup Fog_Graphics_Public



//! @defgroup Fog_Graphics_Other Non-categorized classes and functions
//! @ingroup Fog_Graphics_Public

// ============================================================================
// [Fog - Graphics - Namespaces]
// ============================================================================

//! @namespace Fog::RasterEngine
//! @internal
//! @brief Low level raster based functions namespace.
//!
//! RasterEngine namespace contains lowest level pixel manipulation library that
//! is used in Fog library.
//!
//! @section Pixel format conversion and definitions.
//!
//! Fog library contains few pixel formats that can be usually combined together.
//! When usung compositing function and destination not supports alpha channel,
//! the result is premultiplied and alpha is set to 0xFF. This is like blitting
//! the result into black destination using source over operator.

#endif // FOG_DOXYGEN

// ============================================================================
// [Fog-Graphics Include Files]
// ============================================================================

#include <Fog/Core/Build.h>

#include <Fog/Graphics/Color.h>
#include <Fog/Graphics/ColorLut.h>
#include <Fog/Graphics/ColorMatrix.h>
#include <Fog/Graphics/ColorUtil.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/ConvolutionMatrix.h>
#include <Fog/Graphics/Font.h>
#include <Fog/Graphics/Glyph.h>
#include <Fog/Graphics/GlyphCache.h>
#include <Fog/Graphics/GlyphSet.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageFilter.h>
#include <Fog/Graphics/ImageFilterEngine.h>
#include <Fog/Graphics/ImageFilterParams.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Painter.h>
#include <Fog/Graphics/PaintEngine.h>
#include <Fog/Graphics/PaintUtil.h>
#include <Fog/Graphics/Palette.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Graphics/PathStroker.h>
#include <Fog/Graphics/PathTransform.h>
#include <Fog/Graphics/PathUtil.h>
#include <Fog/Graphics/Pattern.h>
#include <Fog/Graphics/Region.h>
#include <Fog/Graphics/TextLayout.h>
#include <Fog/Graphics/TextOnPath.h>

// [Guard]
#endif // _FOG_GRAPHICS_H
