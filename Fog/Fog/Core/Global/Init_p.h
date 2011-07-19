// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_INIT_P_H
#define _FOG_CORE_GLOBAL_INIT_P_H

// [Dependencies]
#include <Fog/Core/Global/Constants.h>

// ============================================================================
// [Fog::Core - Init / Fini]
// ============================================================================

namespace Fog {

// [Fog/Core/Collection]
FOG_NO_EXPORT void List_init(void);
FOG_NO_EXPORT void Hash_init(void);

// [Fog/Core/Cpu]
FOG_NO_EXPORT void Cpu_init(void);

// [Fog/Core/DateTime]
FOG_NO_EXPORT void Date_init(void);
FOG_NO_EXPORT void Time_init(void);

// [Fog/Core/Global]
FOG_NO_EXPORT void Error_init(void);

// [Fog/Core/IO]
FOG_NO_EXPORT void Stream_init(void);
FOG_NO_EXPORT void Stream_fini(void);

// [Fog/Core/Library]
FOG_NO_EXPORT void Library_init(void);
FOG_NO_EXPORT void Library_fini(void);

// [Fog/Core/Math]
FOG_NO_EXPORT void Math_init(void);

// [Fog/Core/Memory]
FOG_NO_EXPORT void MemoryOps_init(void);
FOG_NO_EXPORT void MemoryManager_init(void);

FOG_NO_EXPORT void MemoryAlloc_init(void);
FOG_NO_EXPORT void MemoryAlloc_fini(void);

FOG_NO_EXPORT void MemoryCleanup_init(void);

// [Fog/Core/OS]
FOG_NO_EXPORT void OS_init(void);
FOG_NO_EXPORT void OS_fini(void);

// [Fog/Core/System]
FOG_NO_EXPORT void Application_init(void);
FOG_NO_EXPORT void Application_fini(void);

FOG_NO_EXPORT void Object_init(void);
FOG_NO_EXPORT void Object_fini(void);

// [Fog/Core/Threading]
FOG_NO_EXPORT void Lock_init(void);
FOG_NO_EXPORT void Lock_fini(void);

FOG_NO_EXPORT void ThreadLocal_init(void);
FOG_NO_EXPORT void ThreadLocal_fini(void);

FOG_NO_EXPORT void Thread_init(void);
FOG_NO_EXPORT void Thread_fini(void);

FOG_NO_EXPORT void ThreadPool_init(void);
FOG_NO_EXPORT void ThreadPool_fini(void);

// [Fog/Core/Tools]
FOG_NO_EXPORT void ByteArray_init(void);
FOG_NO_EXPORT void Locale_init(void);

FOG_NO_EXPORT void ManagedString_init(void);
FOG_NO_EXPORT void ManagedString_fini(void);

FOG_NO_EXPORT void String_init(void);
FOG_NO_EXPORT void Strings_init(void);

FOG_NO_EXPORT void TextCodec_init(void);
FOG_NO_EXPORT void TextCodec_fini(void);

// [Fog/Core/Variant]
FOG_NO_EXPORT void Var_init(void);
FOG_NO_EXPORT void Var_fini(void);

// [Fog/Core/Xml]
FOG_NO_EXPORT void XmlEntity_init(void);

// [Fog/G2d/Geometry]
FOG_NO_EXPORT void Line_init(void);
FOG_NO_EXPORT void QBezier_init(void);
FOG_NO_EXPORT void CBezier_init(void);
FOG_NO_EXPORT void Arc_init(void);

FOG_NO_EXPORT void Triangle_init(void);
FOG_NO_EXPORT void Circle_init(void);
FOG_NO_EXPORT void Ellipse_init(void);
FOG_NO_EXPORT void Chord_init(void);
FOG_NO_EXPORT void Pie_init(void);
FOG_NO_EXPORT void Round_init(void);
FOG_NO_EXPORT void Path_init(void);
FOG_NO_EXPORT void Shape_init(void);

FOG_NO_EXPORT void Transform_init(void);
FOG_NO_EXPORT void PathClipper_init(void);

// [Fog/G2d/Imaging]
FOG_NO_EXPORT void Image_init(void);
FOG_NO_EXPORT void ImagePalette_init(void);

FOG_NO_EXPORT void ImageConverter_init(void);
FOG_NO_EXPORT void ImageEffect_init(void);
FOG_NO_EXPORT void ImageFormatDescription_init(void);

FOG_NO_EXPORT void ImageCodecProvider_init(void);
FOG_NO_EXPORT void ImageCodecProvider_fini(void);

// [Fog/G2d/Painting]
FOG_NO_EXPORT void PaintDeviceInfo_init(void);
FOG_NO_EXPORT void Painter_initNull(void);
FOG_NO_EXPORT void Painter_initRaster(void);

// [Fog/G2d/Render]
FOG_NO_EXPORT void Render_init(void);

// [Fog/G2d/Source]
FOG_NO_EXPORT void Color_init(void);
FOG_NO_EXPORT void ColorStopList_init(void);
FOG_NO_EXPORT void Pattern_init(void);

// [Fog/G2d/Tools]
FOG_NO_EXPORT void Matrix_init(void);
FOG_NO_EXPORT void Region_init(void);

// [Fog/G2d/Text]
FOG_NO_EXPORT void GlyphBitmap_init(void);
FOG_NO_EXPORT void GlyphBitmap_fini(void);

FOG_NO_EXPORT void GlyphOutline_init(void);
FOG_NO_EXPORT void GlyphOutline_fini(void);

FOG_NO_EXPORT void FontManager_init(void);
FOG_NO_EXPORT void FontManager_fini(void);

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_INIT_P_H
