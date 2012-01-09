// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_INIT_P_H
#define _FOG_CORE_GLOBAL_INIT_P_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

// ============================================================================
// [Fog::Core - Init / Fini]
// ============================================================================

namespace Fog {

// [Fog/Core/Math]
FOG_NO_EXPORT void Math_init(void);

// [Fog/Core/Memory]
FOG_NO_EXPORT void MemMgr_init(void);
FOG_NO_EXPORT void MemMgr_fini(void);

FOG_NO_EXPORT void MemBlockAllocator_init(void);
FOG_NO_EXPORT void MemOps_init(void);
FOG_NO_EXPORT void MemPool_init(void);
FOG_NO_EXPORT void MemZoneAllocator_init(void);

// [Fog/Core/Kernel]
FOG_NO_EXPORT void Application_init(void);
FOG_NO_EXPORT void Application_fini(void);

FOG_NO_EXPORT void EventLoop_init(void);
FOG_NO_EXPORT void EventLoopObserverList_init(void);

FOG_NO_EXPORT void Object_init(void);
FOG_NO_EXPORT void Object_fini(void);

// [Fog/Core/OS]
FOG_NO_EXPORT void DirIterator_init(void);
FOG_NO_EXPORT void Environment_init(void);
FOG_NO_EXPORT void FileInfo_init(void);
FOG_NO_EXPORT void FileMapping_init(void);
FOG_NO_EXPORT void FilePath_init(void);
FOG_NO_EXPORT void FileUtil_init(void);

FOG_NO_EXPORT void Library_init(void);
FOG_NO_EXPORT void Library_fini(void);

FOG_NO_EXPORT void OSInfo_init(void);
FOG_NO_EXPORT void OSUtil_init(void);
FOG_NO_EXPORT void UserUtil_init(void);

#if defined(FOG_OS_WINDOWS)
FOG_NO_EXPORT void WinCOM_init(void);
FOG_NO_EXPORT void WinUtil_init(void);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_MAC)
FOG_NO_EXPORT void MacUtil_init(void);
#endif // FOG_OS_MAC

// [Fog/Core/Threading]
FOG_NO_EXPORT void Lock_init(void);
FOG_NO_EXPORT void Lock_fini(void);

FOG_NO_EXPORT void ThreadLocal_init(void);
FOG_NO_EXPORT void ThreadLocal_fini(void);

FOG_NO_EXPORT void ThreadEvent_init(void);
FOG_NO_EXPORT void ThreadCondition_init(void);

FOG_NO_EXPORT void Thread_init(void);
FOG_NO_EXPORT void Thread_fini(void);

FOG_NO_EXPORT void ThreadPool_init(void);
FOG_NO_EXPORT void ThreadPool_fini(void);

// [Fog/Core/Tools]
FOG_NO_EXPORT void CharUtil_init(void);

FOG_NO_EXPORT void Cpu_init(void);

FOG_NO_EXPORT void Hash_init(void);
FOG_NO_EXPORT void HashUtil_init(void);
FOG_NO_EXPORT void List_init(void);

FOG_NO_EXPORT void Locale_init(void);
FOG_NO_EXPORT void Locale_fini(void);

FOG_NO_EXPORT void Logger_init(void);
FOG_NO_EXPORT void Logger_fini(void);

FOG_NO_EXPORT void ManagedString_init(void);
FOG_NO_EXPORT void ManagedString_fini(void);

FOG_NO_EXPORT void Stream_init(void);
FOG_NO_EXPORT void Stream_fini(void);

FOG_NO_EXPORT void String_init(void);
FOG_NO_EXPORT void StringUtil_init(void);
FOG_NO_EXPORT void StringUtil_init_dtoa(void);

FOG_NO_EXPORT void Random_init(void);

FOG_NO_EXPORT void RegExp_init(void);

FOG_NO_EXPORT void TextCodec_init(void);
FOG_NO_EXPORT void TextCodec_fini(void);

FOG_NO_EXPORT void Date_init(void);
FOG_NO_EXPORT void Time_init(void);

FOG_NO_EXPORT void Var_init(void);

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
FOG_NO_EXPORT void PathStroker_init(void);
FOG_NO_EXPORT void PathInfo_init(void);

// [Fog/G2d/Imaging]
FOG_NO_EXPORT void Image_init(void);

#if defined(FOG_OS_WINDOWS)
FOG_NO_EXPORT void Image_init_win(void);
#endif // FOG_OS_WINDOWS

#if defined(FOG_OS_MAC)
FOG_NO_EXPORT void Image_init_mac(void);
#endif // FOG_OS_MAC

FOG_NO_EXPORT void ImageResize_init(void);
FOG_NO_EXPORT void ImagePalette_init(void);
FOG_NO_EXPORT void ImageConverter_init(void);
FOG_NO_EXPORT void ImageFilter_init(void);
FOG_NO_EXPORT void ImageFormatDescription_init(void);

FOG_NO_EXPORT void ImageCodecProvider_init(void);
FOG_NO_EXPORT void ImageCodecProvider_fini(void);

FOG_NO_EXPORT void FeBlur_init(void);
FOG_NO_EXPORT void FeColorLut_init(void);
FOG_NO_EXPORT void FeColorLutArray_init(void);
FOG_NO_EXPORT void FeColorMatrix_init(void);
FOG_NO_EXPORT void FeComponentFunction_init(void);
FOG_NO_EXPORT void FeComponentTransfer_init(void);
FOG_NO_EXPORT void FeConvolveMatrix_init(void);
FOG_NO_EXPORT void FeConvolveSeparable_init(void);
FOG_NO_EXPORT void FeMorphology_init(void);

// [Fog/G2d/Painting]
FOG_NO_EXPORT void NullPaintEngine_init(void);
FOG_NO_EXPORT void PaintDeviceInfo_init(void);
FOG_NO_EXPORT void RasterPaintEngine_init(void);
FOG_NO_EXPORT void RasterOps_init(void);
FOG_NO_EXPORT void Rasterizer_init(void);

// [Fog/G2d/Source]
FOG_NO_EXPORT void Color_init(void);
FOG_NO_EXPORT void ColorStopList_init(void);
FOG_NO_EXPORT void Gradient_init(void);
FOG_NO_EXPORT void Pattern_init(void);

// [Fog/G2d/Tools]
FOG_NO_EXPORT void Dpi_init(void);
FOG_NO_EXPORT void Matrix_init(void);

FOG_NO_EXPORT void Region_init(void);

#if defined(FOG_OS_WINDOWS)
FOG_NO_EXPORT void Region_init_win(void);
#endif // FOG_OS_WINDOWS

FOG_NO_EXPORT void RegionUtil_init(void);

// [Fog/G2d/Text]
FOG_NO_EXPORT void GlyphBitmap_init(void);
FOG_NO_EXPORT void GlyphBitmap_fini(void);

FOG_NO_EXPORT void GlyphOutline_init(void);
FOG_NO_EXPORT void GlyphOutline_fini(void);

FOG_NO_EXPORT void Font_init(void);
FOG_NO_EXPORT void Font_fini(void);

FOG_NO_EXPORT void FontManager_init(void);
FOG_NO_EXPORT void FontManager_fini(void);

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_INIT_P_H
