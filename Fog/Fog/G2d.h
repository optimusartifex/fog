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

#include <Fog/Core/C++/Base.h>

#include <Fog/G2d/Geometry/Arc.h>
#include <Fog/G2d/Geometry/Box.h>
#include <Fog/G2d/Geometry/CBezier.h>
#include <Fog/G2d/Geometry/Chord.h>
#include <Fog/G2d/Geometry/Circle.h>
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
#include <Fog/G2d/Geometry/QBezier.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Round.h>
#include <Fog/G2d/Geometry/Shape.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Geometry/Triangle.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>
#include <Fog/G2d/Imaging/ImageFilter.h>
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
#include <Fog/G2d/Painting/PaintDevice.h>
#include <Fog/G2d/Painting/PaintDeviceInfo.h>
#include <Fog/G2d/Painting/PaintEngine.h>
#include <Fog/G2d/Painting/PaintParams.h>
#include <Fog/G2d/Painting/PaintUtil.h>
#include <Fog/G2d/Painting/Painter.h>
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
#include <Fog/G2d/Tools/Dpi.h>
#include <Fog/G2d/Tools/Matrix.h>
#include <Fog/G2d/Tools/Region.h>

// [Guard]
#endif // _FOG_G2D_H
