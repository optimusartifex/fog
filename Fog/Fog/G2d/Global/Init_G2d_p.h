// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_GLOBAL_INIT_P_H
#define _FOG_G2D_GLOBAL_INIT_P_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

namespace Fog {

// [Fog/G2d]
FOG_NO_EXPORT void _g2d_init(void);
FOG_NO_EXPORT void _g2d_fini(void);

// [Fog/G2d/Geometry]
FOG_NO_EXPORT void _g2d_line_init(void);
FOG_NO_EXPORT void _g2d_qbezier_init(void);
FOG_NO_EXPORT void _g2d_cbezier_init(void);
FOG_NO_EXPORT void _g2d_arc_init(void);

FOG_NO_EXPORT void _g2d_triangle_init(void);
FOG_NO_EXPORT void _g2d_circle_init(void);
FOG_NO_EXPORT void _g2d_ellipse_init(void);
FOG_NO_EXPORT void _g2d_chord_init(void);
FOG_NO_EXPORT void _g2d_pie_init(void);
FOG_NO_EXPORT void _g2d_round_init(void);

FOG_NO_EXPORT void _g2d_path_init(void);
FOG_NO_EXPORT void _g2d_path_fini(void);

FOG_NO_EXPORT void _g2d_shape_init(void);
FOG_NO_EXPORT void _g2d_shape_fini(void);

FOG_NO_EXPORT void _g2d_transform_init(void);
FOG_NO_EXPORT void _g2d_pathclipper_init(void);

// [Fog/G2d/Imaging]
FOG_NO_EXPORT void _g2d_imagepalette_init(void);
FOG_NO_EXPORT void _g2d_imagepalette_fini(void);

FOG_NO_EXPORT void _g2d_imageeffect_init(void);
FOG_NO_EXPORT void _g2d_imageeffect_fini(void);

FOG_NO_EXPORT void _g2d_imageformatdescription_init(void);
FOG_NO_EXPORT void _g2d_imageformatdescription_fini(void);

FOG_NO_EXPORT void _g2d_image_init(void);
FOG_NO_EXPORT void _g2d_image_fini(void);

FOG_NO_EXPORT void _g2d_imageconverter_init(void);
FOG_NO_EXPORT void _g2d_imageconverter_fini(void);

FOG_NO_EXPORT void _g2d_imagecodecprovider_init(void);
FOG_NO_EXPORT void _g2d_imagecodecprovider_fini(void);

// [Fog/G2d/Painting]
FOG_NO_EXPORT void _g2d_paintdeviceinfo_init(void);
FOG_NO_EXPORT void _g2d_paintdeviceinfo_fini(void);

FOG_NO_EXPORT void _g2d_painter_init_null(void);
FOG_NO_EXPORT void _g2d_painter_fini_null(void);

FOG_NO_EXPORT void _g2d_painter_init_raster(void);
FOG_NO_EXPORT void _g2d_painter_fini_raster(void);

// [Fog/G2d/Render]
FOG_NO_EXPORT void _g2d_render_init(void);
FOG_NO_EXPORT void _g2d_render_fini(void);

// [Fog/G2d/Source]
FOG_NO_EXPORT void _g2d_color_init(void);

FOG_NO_EXPORT void _g2d_colorstoplist_init(void);
FOG_NO_EXPORT void _g2d_colorstoplist_fini(void);

FOG_NO_EXPORT void _g2d_pattern_init(void);
FOG_NO_EXPORT void _g2d_pattern_fini(void);

// [Fog/G2d/Tools]
FOG_NO_EXPORT void _g2d_genericmatrix_init(void);
FOG_NO_EXPORT void _g2d_genericmatrix_fini(void);

FOG_NO_EXPORT void _g2d_region_init(void);
FOG_NO_EXPORT void _g2d_region_fini(void);

// [Fog/G2d/Text]
FOG_NO_EXPORT void _g2d_glyphbitmap_init(void);
FOG_NO_EXPORT void _g2d_glyphbitmap_fini(void);

FOG_NO_EXPORT void _g2d_glyphoutline_init(void);
FOG_NO_EXPORT void _g2d_glyphoutline_fini(void);

FOG_NO_EXPORT void _g2d_fontmanager_init(void);
FOG_NO_EXPORT void _g2d_fontmanager_fini(void);

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_GLOBAL_INIT_P_H
