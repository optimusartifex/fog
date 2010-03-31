// [Fog-Svg Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_SVGUTIL_H
#define _FOG_SVG_SVGUTIL_H

// [Dependencies]
#include <Fog/Core/Char.h>
#include <Fog/Core/List.h>
#include <Fog/Core/String.h>
#include <Fog/Graphics/Argb.h>
#include <Fog/Graphics/Matrix.h>
#include <Fog/Graphics/Path.h>
#include <Fog/Svg/SvgBase.h>

namespace Fog {

//! @addtogroup Fog_Svg
//! @{

// ============================================================================
// [Fog::SvgUtil]
// ============================================================================

namespace SvgUtil {

// ============================================================================
// [Fog::SvgUtil - Color & Opacity]
// ============================================================================

//! @brief Parse SVG color and save it to @a dst. This method can also check
//! whether a given string is 'none' or URI to pattern definition.
//!
//! @return One of @c SVG_SOURCE_TYPE enumeration value is returned.
FOG_API int parseColor(const String& str, Argb* dst);

//! @brief Serialize SVG color using #RRGGBB notation.
FOG_API err_t serializeColor(String& dst, Argb color);

//! @brief Parse SVG opacity value: 0.0 to 1.0 or 0% to 100% and clamp it if.
//! needed.
FOG_API err_t parseOpacity(const String& str, double* dst);

// ============================================================================
// [Fog::SvgUtil - Matrix]
// ============================================================================

//! @brief Parse SVG matrix and save it to @a dst.
FOG_API err_t parseMatrix(const String& str, DoubleMatrix* dst);

// ============================================================================
// [Fog::SvgUtil - Coordinates]
// ============================================================================

//! @brief Parse SVG coorinate and return it.
FOG_API SvgCoord parseCoord(const String& str);

//! @brief Serialize SVG coordinate to string.
FOG_API err_t serializeCoord(String& dst, const SvgCoord& coord);

// ============================================================================
// [Fog::SvgUtil - Paths]
// ============================================================================

FOG_API DoublePath parsePoints(const String& str);

FOG_API DoublePath parsePath(const String& str);

} // SvgUtil namespace

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_SVGUTIL_H
