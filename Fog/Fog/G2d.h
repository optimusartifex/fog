// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

// [Guard]
#ifndef _FOG_G2D_H
#define _FOG_G2D_H

#if defined(FOG_DOXYGEN)

// ============================================================================
// [Fog/G2d - Documentation]
// ============================================================================

//! @defgroup Fog_G2d_Public Fog-Graphics
//!
//! Public classes and functions related to Fog-Graphics library.



//! @defgroup Fog_G2d_Constants Constants
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Geometry Geometry
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Imaging Imaging
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Painting Painting
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Text Fonts and Text
//! @ingroup Fog_G2d_Public



//! @defgroup Fog_G2d_Tools Tools
//! @ingroup Fog_G2d_Public

// ============================================================================
// [Fog/G2d - Namespaces]
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
//! When using compositing function and destination not supports alpha channel,
//! the result is premultiplied and alpha is set to 0xFF. This is like blitting
//! the result into black destination using a source over operator.

#endif // FOG_DOXYGEN

// ============================================================================
// [Fog/G2d Include Files]
// ============================================================================

#include <Fog/Core/Config/Config.h>

#include <Fog/G2d/Font/Font.h>
#include <Fog/G2d/Font/Glyph.h>
#include <Fog/G2d/Font/GlyphCache.h>
#include <Fog/G2d/Font/GlyphSet.h>
#include <Fog/G2d/Font/KerningPair.h>
#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/Chord.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/CubicCurve.h>
#include <Fog/G2d/Geometry/Ellipse.h>
#include <Fog/G2d/Geometry/Line.h>
#include <Fog/G2d/Geometry/Math2d.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/PathClipper.h>
#include <Fog/G2d/Geometry/PathEffect.h>
#include <Fog/G2d/Geometry/PathOnPath.h>
#include <Fog/G2d/Geometry/PathStroker.h>
#include <Fog/G2d/Geometry/Pie.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Polygon.h>
#include <Fog/G2d/Geometry/QuadCurve.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Shape.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Global/Api.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEffect.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Imaging/Effects/BlurEffect.h>
#include <Fog/G2d/Imaging/Effects/BorderEffect.h>
#include <Fog/G2d/Imaging/Effects/ColorMatrix.h>
#include <Fog/G2d/Imaging/Effects/ComponentTransferEffect.h>
#include <Fog/G2d/Imaging/Effects/ComponentTransferFunction.h>
#include <Fog/G2d/Imaging/Effects/ConvolveEffect.h>
#include <Fog/G2d/Imaging/Effects/EmbossEffect.h>
#include <Fog/G2d/Imaging/Effects/ShadowEffect.h>
#include <Fog/G2d/Imaging/Effects/SharpenEffect.h>
#include <Fog/G2d/Painting/Painter.h>
#include <Fog/G2d/Painting/PainterEngine.h>
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/PaintUtil.h>
#include <Fog/G2d/Shader/Shader.h>
#include <Fog/G2d/Source/Acmyk.h>
#include <Fog/G2d/Source/Ahsl.h>
#include <Fog/G2d/Source/Ahsv.h>
#include <Fog/G2d/Source/Argb.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Source/ColorStop.h>
#include <Fog/G2d/Source/ColorStopCache.h>
#include <Fog/G2d/Source/ColorStopList.h>
#include <Fog/G2d/Source/ColorUtil.h>
#include <Fog/G2d/Source/ConicalGradient.h>
#include <Fog/G2d/Source/Gradient.h>
#include <Fog/G2d/Source/LinearGradient.h>
#include <Fog/G2d/Source/Pattern.h>
#include <Fog/G2d/Source/RadialGradient.h>
#include <Fog/G2d/Source/RectangularGradient.h>
#include <Fog/G2d/Source/Texture.h>
#include <Fog/G2d/Text/TextIterator.h>
#include <Fog/G2d/Text/TextLayout.h>
#include <Fog/G2d/Text/TextRect.h>
#include <Fog/G2d/Tools/Dpi.h>
#include <Fog/G2d/Tools/Matrix.h>
#include <Fog/G2d/Tools/Region.h>

// [Guard]
#endif // _FOG_G2D_H
