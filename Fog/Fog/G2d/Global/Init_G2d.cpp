// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Global/Init_G2d_p.h>

namespace Fog {

// ============================================================================
// [Fog::G2d - Initialization / Finalization]
// ============================================================================

FOG_NO_EXPORT void _g2d_init(void)
{
  // [Fog/G2d/Geometry]
  _g2d_line_init();
  _g2d_qbezier_init();
  _g2d_cbezier_init();
  _g2d_arc_init();
  _g2d_circle_init();
  _g2d_ellipse_init();
  _g2d_chord_init();
  _g2d_pie_init();
  _g2d_round_init();
  _g2d_triangle_init();

  _g2d_path_init();
  _g2d_shape_init();

  _g2d_transform_init();
  _g2d_pathclipper_init();

  // [Fog/G2d/Render]
  _g2d_render_init();

  // [Fog/G2d/Source]
  _g2d_color_init();
  _g2d_colorstoplist_init();
  _g2d_pattern_init();

  // [Fog/G2d/Imaging]
  _g2d_imagepalette_init();
  _g2d_imageformatdescription_init();
  _g2d_image_init();
  _g2d_imageconverter_init();
  _g2d_imagecodecprovider_init();
  _g2d_imageeffect_init();

  // [Fog/G2d/Tools]
  _g2d_region_init();
  _g2d_genericmatrix_init();

  // [Fog/G2d/Painter]
  _g2d_paintdeviceinfo_init();
  _g2d_painter_init_null();
  _g2d_painter_init_raster();

  // [Fog/G2d/Text]
  _g2d_glyphbitmap_init();
  _g2d_glyphoutline_init();
  _g2d_fontmanager_init();
}

FOG_NO_EXPORT void _g2d_fini(void)
{
  // [Fog/G2d/Text]
  _g2d_fontmanager_fini();
  _g2d_glyphoutline_fini();
  _g2d_glyphbitmap_fini();

  // [Fog/G2d/Painter]
  _g2d_painter_fini_raster();
  _g2d_painter_fini_null();
  _g2d_paintdeviceinfo_fini();

  // [Fog/G2d/Tools]
  _g2d_genericmatrix_fini();
  _g2d_region_fini();

  // [Fog/G2d/Imaging]
  _g2d_imageeffect_fini();
  _g2d_imagecodecprovider_fini();
  _g2d_imageconverter_fini();
  _g2d_image_fini();
  _g2d_imageformatdescription_fini();
  _g2d_imagepalette_fini();

  // [Fog/G2d/Source]
  _g2d_pattern_fini();
  _g2d_colorstoplist_fini();

  // [Fog/G2d/Render]
  _g2d_render_fini();

  // [Fog/G2d/Geometry]
  _g2d_shape_fini();
  _g2d_path_fini();
}

} // Fog namespace
