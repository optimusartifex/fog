// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_SVG_SVGUTIL_H
#define _FOG_G2D_SVG_SVGUTIL_H

// [Dependencies]
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Geometry/Coord.h>
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/G2d/Geometry/Transform.h>
#include <Fog/G2d/Source/Color.h>

namespace Fog {
namespace SvgUtil {

//! @addtogroup Fog_G2d_Svg
//! @{

// ============================================================================
// [Fog::SvgUtil - Parse]
// ============================================================================

//! @brief Parse SVG color and save it to @a dst. This method can also check
//! whether a given string is 'none' or URI to pattern definition.
//!
//! @return One of @c SVG_SOURCE_TYPE enumeration value is returned.
FOG_API uint32_t parseColor(Color& dst, const StringW& src);

//! @overload.
FOG_API uint32_t parseColor(Argb32& dst, const StringW& src);

//! @brief Parse SVG offset value: 0.0 to 1.0 or 0% to 100% and clamp if needed.
FOG_API err_t parseOffset(float& dst, const StringW& src);

//! @brief Parse SVG opacity value: 0.0 to 1.0 or 0% to 100% and clamp if needed.
FOG_API err_t parseOpacity(float& dst, const StringW& src);

//! @brief Parse SVG matrix and save it to @a dst.
FOG_API err_t parseTransform(TransformF& dst, const StringW& src);

//! @brief Parse SVG coordinate and return it.
FOG_API err_t parseCoord(CoordF& coord, const StringW& src);

FOG_API err_t parseViewBox(BoxF& box, const StringW& src);

FOG_API err_t parsePoints(PathF& dst, const StringW& src, bool closePath);
FOG_API err_t parsePath(PathF& dst, const StringW& src);

FOG_API err_t parseCSSStyle(const StringW& src, CSSStyleHandlerFunc func, void* ctx);

// ============================================================================
// [Fog::SvgUtil - Serialize]
// ============================================================================

//! @brief Serialize SVG color using #RRGGBB notation.
FOG_API err_t serializeColor(StringW& dst, const Color& src);
//! @overload
FOG_API err_t serializeColor(StringW& dst, const Argb32& src);

//! @brief Serialize SVG opacity to string.
FOG_API err_t serializeOffset(StringW& dst, float src);

//! @brief Serialize SVG opacity to string.
FOG_API err_t serializeOpacity(StringW& dst, float src);

//! @brief Serialize SVG coordinate to string.
FOG_API err_t serializeCoord(StringW& dst, const CoordF& src);

//! @brief Serialize SVG view-box to string.
FOG_API err_t serializeViewBox(StringW& dst, const BoxF& src);

//! @brief Serialize SVG path to string.
FOG_API err_t serializePath(StringW& dst, const PathF& src);

//! @brief Serialize SVG transform to string.
FOG_API err_t serializeTransform(StringW& dst, const TransformF& src);

//! @}

} // SvgUtil namespace
} // Fog namespace

// [Guard]
#endif // _FOG_G2D_SVG_SVGUTIL_H
