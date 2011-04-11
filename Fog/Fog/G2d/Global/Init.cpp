// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Global/Init_p.h>

namespace Fog {

// ============================================================================
// [Fog::G2d - Initialization / Finalization]
// ============================================================================

FOG_NO_EXPORT void _g2d_init(void)
{
  // [Fog/G2d/Geometry]
  _g2d_arc_init();
  _g2d_line_init();
  _g2d_quadcurve_init();
  _g2d_cubiccurve_init();

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
  _g2d_imageeffect_init();
  _g2d_imageformatdescription_init();
  _g2d_image_init();
  _g2d_imageconverter_init();
  _g2d_imagecodecprovider_init();

  // [Fog/G2d/Tools]
  _g2d_region_init();
  _g2d_genericmatrix_init();

  // [Fog/G2d/Font]
  _g2d_glyph_init();
  _g2d_glyphset_init();
  _g2d_font_init();

  // [Fog/G2d/Painter]
  _g2d_painter_init_null();
  _g2d_painter_init_raster();
}

FOG_NO_EXPORT void _g2d_fini(void)
{
  // [Fog/G2d/Painter]
  _g2d_painter_fini_raster();
  _g2d_painter_fini_null();

  // [Fog/G2d/Font]
  _g2d_font_fini();
  _g2d_glyphset_fini();
  _g2d_glyph_fini();

  // [Fog/G2d/Tools]
  _g2d_genericmatrix_fini();
  _g2d_region_fini();

  // [Fog/G2d/Imaging]
  _g2d_imagecodecprovider_fini();
  _g2d_imageconverter_fini();
  _g2d_image_fini();
  _g2d_imageformatdescription_fini();
  _g2d_imageeffect_fini();
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
