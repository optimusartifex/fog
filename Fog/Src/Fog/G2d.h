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

// ============================================================================
// [Fog/G2d]
// ============================================================================

//! @defgroup Fog_G2d Fog-G2d
//!
//! Public classes and functions related to Fog-G2d library.

#include <Fog/Core/C++/Base.h>

// ============================================================================
// [Fog/G2d/Geometry]
// ============================================================================

//! @defgroup Fog_G2d_Geometry Geometry classes, vector manipulation and math
//! @ingroup Fog_G2d

#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Chord.h>
#include <Fog/G2d/Geometry/Circle.h>
#include <Fog/G2d/Geometry/Coord.h>
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
#include <Fog/G2d/Geometry/PointArray.h>
#include <Fog/G2d/Geometry/Polygon.h>
#include <Fog/G2d/Geometry/QBezier.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/RectArray.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Shape.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Thickness.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Geometry/Triangle.h>

// ============================================================================
// [Fog/G2d/Imaging]
// ============================================================================

//! @defgroup Fog_G2d_Imaging Imaging, IO and effects
//! @ingroup Fog_G2d

#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
#include <Fog/G2d/Imaging/ImageFilterScale.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>
#include <Fog/G2d/Imaging/ImagePalette.h>
#include <Fog/G2d/Imaging/Filters/FeBase.h>
#include <Fog/G2d/Imaging/Filters/FeBlur.h>
#include <Fog/G2d/Imaging/Filters/FeBorder.h>
#include <Fog/G2d/Imaging/Filters/FeColorLut.h>
#include <Fog/G2d/Imaging/Filters/FeColorLutArray.h>
#include <Fog/G2d/Imaging/Filters/FeColorMatrix.h>
#include <Fog/G2d/Imaging/Filters/FeComponentFunction.h>
#include <Fog/G2d/Imaging/Filters/FeComponentTransfer.h>
#include <Fog/G2d/Imaging/Filters/FeConvolveMatrix.h>
#include <Fog/G2d/Imaging/Filters/FeConvolveSeparable.h>
#include <Fog/G2d/Imaging/Filters/FeMorphology.h>

// ============================================================================
// [Fog/G2d/OS]
// ============================================================================

//! @defgroup Fog_G2d_OS Access OS specific features
//! @ingroup Fog_G2d

#include <Fog/G2d/OS/OSUtil.h>

// ============================================================================
// [Fog/G2d/Painting]
// ============================================================================

//! @defgroup Fog_G2d_Painting Painting
//! @ingroup Fog_G2d

#include <Fog/G2d/Painting/PaintDevice.h>
#include <Fog/G2d/Painting/PaintDeviceInfo.h>
#include <Fog/G2d/Painting/PaintEngine.h>
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/PaintUtil.h>
#include <Fog/G2d/Painting/Painter.h>

// ============================================================================
// [Fog/G2d/Shader]
// ============================================================================

//! @defgroup Fog_G2d_Shader Shaders
//! @ingroup Fog_G2d

#include <Fog/G2d/Shader/Shader.h>

// ============================================================================
// [Fog/G2d/Source]
// ============================================================================

//! @defgroup Fog_G2d_Source Colors, gradients, and textures
//! @ingroup Fog_G2d

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

// ============================================================================
// [Fog/G2d/Svg]
// ============================================================================

#include <Fog/G2d/Svg/SvgDocument.h>
#include <Fog/G2d/Svg/SvgElement.h>
#include <Fog/G2d/Svg/SvgHitTest.h>
#include <Fog/G2d/Svg/SvgMeasure.h>
#include <Fog/G2d/Svg/SvgRender.h>
#include <Fog/G2d/Svg/SvgUtil.h>
#include <Fog/G2d/Svg/SvgVisitor.h>

// ============================================================================
// [Fog/G2d/Text]
// ============================================================================

//! @defgroup Fog_G2d_Text Fonts and text manipulation
//! @ingroup Fog_G2d

#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/FontFace.h>
#include <Fog/G2d/Text/FontFaceCache.h>
#include <Fog/G2d/Text/FontHints.h>
#include <Fog/G2d/Text/FontKerning.h>
#include <Fog/G2d/Text/FontManager.h>
#include <Fog/G2d/Text/FontMetrics.h>
#include <Fog/G2d/Text/FontProvider.h>
#include <Fog/G2d/Text/GlyphMetrics.h>
#include <Fog/G2d/Text/GlyphBitmap.h>
#include <Fog/G2d/Text/GlyphBitmapCache.h>
#include <Fog/G2d/Text/GlyphOutline.h>
#include <Fog/G2d/Text/GlyphOutlineCache.h>
#include <Fog/G2d/Text/TextLayout.h>
#include <Fog/G2d/Text/TextRect.h>

// ============================================================================
// [Fog/G2d/Tools]
// ============================================================================

//! @defgroup Fog_G2d_Tools Various tools used in 2d graphics
//! @ingroup Fog_G2d

#include <Fog/G2d/Tools/Dpi.h>
#include <Fog/G2d/Tools/Matrix.h>
#include <Fog/G2d/Tools/Region.h>

// [Guard]
#endif // _FOG_G2D_H
