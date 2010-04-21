CMake_Minimum_Required(VERSION 2.6)

Project(Fog C CXX)

# =============================================================================
# [Configuration]
# =============================================================================

# Whether to build examples.
# Set(FOG_BUILD_EXAMPLES TRUE)

# Whether to build support for X11.
# Set(FOG_BUILD_GUI_X11 TRUE)

# =============================================================================
# [Backward compatibility]
# =============================================================================

If(FOG_BUILD_UISYSTEM_X11)
  Set(FOG_BUILD_GUI_X11 ${FOG_BUILD_UISYSTEM_X11})
  Message("Warning: FOG_BUILD_UISYSTEM_X11 is deprecated, use FOG_BUILD_GUI_X11 instead!")
EndIf(FOG_BUILD_UISYSTEM_X11)

# =============================================================================
# [Basics]
# =============================================================================

Set(FOG_DIR ${CMAKE_SOURCE_DIR})
Set(FOG_LIBRARIES "")
Set(CMAKE_CXX_STANDARD_LIBRARIES "")

# =============================================================================
# [3rdparty Libraries]
# =============================================================================

# =============================================================================
# [Include cmake helpers we need]
# =============================================================================

Include(CheckIncludeFiles)
Include(CheckLibraryExists)
Include(CheckTypeSize)
Include(FindThreads)

# =============================================================================
# [Include Directories]
# =============================================================================

Include_Directories(${FOG_DIR})

# =============================================================================
# [CFlags]
# =============================================================================

Set(FOG_CFLAGS_OPTIMIZE "")
Set(FOG_CFLAGS_MMX "")
Set(FOG_CFLAGS_3DNOW "")
Set(FOG_CFLAGS_SSE "")
Set(FOG_CFLAGS_SSE2 "")
Set(FOG_CFLAGS_SSE3 "")
Set(FOG_CFLAGS_SSSE3 "")

# GCC flags we can try in future:
# * -fno-keep-static-consts
# * -fmerge-all-constants
# * -fpredictive-commoning

If(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  ## Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fvisibility-inlines-hidden -fstrict-aliasing -fno-math-errno -Winline -Wstrict-aliasing")
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -fvisibility-inlines-hidden -fstrict-aliasing -fno-math-errno -Winline")
  If(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    Set(FOG_CFLAGS_OPTIMIZE "-O2")
  EndIf(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
  Set(FOG_CFLAGS_MMX "${FOG_CFLAGS_OPTIMIZE} -mmmx")
  Set(FOG_CFLAGS_3DNOW "${FOG_CFLAGS_OPTIMIZE} -mmmx -m3dnow")
  Set(FOG_CFLAGS_SSE "${FOG_CFLAGS_OPTIMIZE} -msse")
  Set(FOG_CFLAGS_SSE2 "${FOG_CFLAGS_OPTIMIZE} -msse2")
  Set(FOG_CFLAGS_SSE3 "${FOG_CFLAGS_OPTIMIZE} -msse3")
  Set(FOG_CFLAGS_SSSE3 "${FOG_CFLAGS_OPTIMIZE} -mssse3")
EndIf(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

If(MSVC)
  Set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi /Oy /GF /GS- /GR-")
  Set(FOG_CFLAGS_OPTIMIZE "/Ot")
  Set(FOG_CFLAGS_MMX "${FOG_CFLAGS_OPTIMIZE}")
  Set(FOG_CFLAGS_3DNOW "${FOG_CFLAGS_OPTIMIZE}")
  Set(FOG_CFLAGS_SSE "${FOG_CFLAGS_OPTIMIZE} /arch:SSE")
  Set(FOG_CFLAGS_SSE2 "${FOG_CFLAGS_OPTIMIZE} /arch:SSE2")
  Set(FOG_CFLAGS_SSE3 "${FOG_CFLAGS_OPTIMIZE} /arch:SSE2")
  Set(FOG_CFLAGS_SSSE3 "${FOG_CFLAGS_OPTIMIZE} /arch:SSE2")
EndIf(MSVC)

If(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  Set(FOG_DEBUG TRUE)
EndIf(${CMAKE_BUILD_TYPE} MATCHES "Debug")

# =============================================================================
# [Endianness]
# =============================================================================

Include(TestBigEndian)
Test_Big_Endian(WORDS_BIGENDIAN)

If(WORDS_BIGENDIAN)
  Set(FOG_BYTE_ORDER 0)
Else(WORDS_BIGENDIAN)
  Set(FOG_BYTE_ORDER 1)
EndIf(WORDS_BIGENDIAN)

# =============================================================================
# [Size of POD types]
# =============================================================================

Check_Type_Size("void*" FOG_SIZEOF_VOID)
Check_Type_Size("short" FOG_SIZEOF_SHORT)
Check_Type_Size("int" FOG_SIZEOF_INT)
Check_Type_Size("long" FOG_SIZEOF_LONG)
Set(CMAKE_EXTRA_INCLUDE_FILES wchar.h)
Check_Type_Size("wchar_t" FOG_SIZEOF_WCHAR_T)
Set(CMAKE_EXTRA_INCLUDE_FILES)

# =============================================================================
# [Presence of headers]
# =============================================================================

If(NOT FOG_HAVE_FLOAT_H)
  Check_Include_Files(float.h FOG_HAVE_FLOAT_H)
EndIf(NOT FOG_HAVE_FLOAT_H)

If(NOT FOG_HAVE_LIMITS_H)
  Check_Include_Files(limits.h FOG_HAVE_LIMITS_H)
EndIf(NOT FOG_HAVE_LIMITS_H)

If(NOT FOG_HAVE_STDINT_H)
  Check_Include_Files(stdint.h FOG_HAVE_STDINT_H)
EndIf(NOT FOG_HAVE_STDINT_H)

If(UNIX)
  If(NOT FOG_HAVE_UNISTD_H)
    Check_Include_Files(unistd.h FOG_HAVE_UNISTD_H)
  EndIf(NOT FOG_HAVE_UNISTD_H)
  
  If(NOT FOG_HAVE_FENV_H)
    Check_Include_Files(fenv.h FOG_HAVE_FENV_H)
  EndIf(NOT FOG_HAVE_FENV_H)
EndIf(UNIX)

# =============================================================================
# [Windows specific setup]
# =============================================================================

# Unicode
If(WIN32)
  Add_Definitions(-D_UNICODE)
EndIf(WIN32)

If(WIN32)
  Set(FOG_LIBRARIES ${FOG_LIBRARIES} user32 gdi32 advapi32 winmm)
EndIf(WIN32)

# =============================================================================
# [Dynamic loadable library support]
# =============================================================================

If(WIN32)
  # Windows dll support
  Set(FOG_LIBRARY_WINDOWS 1)
Else(WIN32)
  # Unix libdl 
  Check_Include_Files(dlfcn.h FOG_HAVE_DLFCN_H)
  If(FOG_HAVE_DLFCN_H)
    Check_Library_Exists(dl dlopen "" FOG_LIBRARY_DL_EXISTS)
    If(FOG_LIBRARY_DL_EXISTS)
      Set(FOG_LIBRARY_DL 1)
      Set(FOG_LIBRARIES "${FOG_LIBRARIES} -ldl")
    EndIf(FOG_LIBRARY_DL_EXISTS)
  EndIf(FOG_HAVE_DLFCN_H)
EndIf(WIN32)

# =============================================================================
# [Threads support]
# =============================================================================

If(CMAKE_THREAD_LIBS_INIT)
  Set(FOG_LIBRARIES "${FOG_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}")
EndIf(CMAKE_THREAD_LIBS_INIT)

# =============================================================================
# [Include rt library under linux]
# =============================================================================

# Library rt is Linux specific (not included in BSD/MAC).
If(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  Set(FOG_LIBRARIES "${FOG_LIBRARIES} -lrt")
EndIf(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# =============================================================================
# [Fontconfig and Freetype2 support]
# =============================================================================

If(WIN32)
  Set(FOG_FONT_WINDOWS TRUE)
Else(WIN32)
  Find_File(HAVE_FREETYPE freetype-config)
  If(HAVE_FREETYPE)
    Exec_Program(freetype-config ARGS --cflags OUTPUT_VARIABLE FREETYPE_CONFIG_OUT RETURN_VALUE FREETYPE_CONFIG_RET)
    Set(FOG_FONT_FREETYPE TRUE)
    Set_Source_Files_Properties(${FOG_DIR}/Fog/Graphics/FontEngine/FreeType.cpp PROPERTIES COMPILE_FLAGS "${FREETYPE_CONFIG_OUT}")
  EndIf(HAVE_FREETYPE)

  If(NOT FOG_HAVE_FONTCONFIG)
    Check_Include_Files(fontconfig/fontconfig.h FOG_HAVE_FONTCONFIG)
  EndIf(NOT FOG_HAVE_FONTCONFIG)
EndIf(WIN32)

# =============================================================================
# [JPEG support]
# =============================================================================

If(NOT FOG_HAVE_LIBJPEG)
  Check_Include_Files(jconfig.h FOG_HAVE_LIBJPEG)
EndIf(NOT FOG_HAVE_LIBJPEG)

# =============================================================================
# [PNG support]
# =============================================================================

If(NOT FOG_HAVE_LIBPNG)
  Check_Include_Files(png.h FOG_HAVE_LIBPNG)
EndIf(NOT FOG_HAVE_LIBPNG)

# =============================================================================
# [Fog-Build]
# =============================================================================

Set(FOG_BUILD_SOURCES
)

Set(FOG_BUILD_HEADERS
  Fog/Build/Build.h
  Fog/Build/Config.h
)

SOURCE_GROUP(Build FILES
  ${FOG_BUILD_SOURCES} ${FOG_BUILD_HEADERS}
)

# =============================================================================
# [Fog-Core]
# =============================================================================

Set(FOG_CORE_LIB_SOURCES
  Fog/Core/Algorithms.cpp
  Fog/Core/Application.cpp
  Fog/Core/Assert.cpp
  Fog/Core/BitArray.cpp
  Fog/Core/Basics.cpp
  Fog/Core/Byte.cpp
  Fog/Core/ByteArray.cpp
  Fog/Core/ByteArrayFilter.cpp
  Fog/Core/ByteArrayMatcher.cpp
  Fog/Core/Char.cpp
  Fog/Core/CharUtil.cpp
  Fog/Core/CpuInfo.cpp
  Fog/Core/DirIterator.cpp
  Fog/Core/Event.cpp
  Fog/Core/EventLoop.cpp
  Fog/Core/FileSystem.cpp
  Fog/Core/FileUtil.cpp
  Fog/Core/Hash.cpp
  Fog/Core/HashUtil.cpp
  Fog/Core/Init.cpp
  Fog/Core/Lazy.cpp
  Fog/Core/Library.cpp
  Fog/Core/List.cpp
  Fog/Core/ListenerList.cpp
  Fog/Core/Locale.cpp
  Fog/Core/Lock.cpp
  Fog/Core/ManagedString.cpp
  Fog/Core/MapFile.cpp
  Fog/Core/Math.cpp
  Fog/Core/Memory.cpp
  Fog/Core/Misc.cpp
  Fog/Core/Object.cpp
  Fog/Core/OS.cpp
  Fog/Core/Std.cpp
  Fog/Core/Stream.cpp
  Fog/Core/String.cpp
  Fog/Core/StringFilter.cpp
  Fog/Core/StringMatcher.cpp
  Fog/Core/Strings.cpp
  Fog/Core/StringUtil.cpp
  Fog/Core/StringUtil_gen.cpp
  Fog/Core/Task.cpp
  Fog/Core/TextCodec.cpp
  Fog/Core/Thread.cpp
  Fog/Core/ThreadCondition.cpp
  Fog/Core/ThreadEvent.cpp
  Fog/Core/ThreadLocalStorage.cpp
  Fog/Core/ThreadPool.cpp
  Fog/Core/Time.cpp
  Fog/Core/Timer.cpp
  Fog/Core/UserInfo.cpp
  Fog/Core/Value.cpp
  Fog/Core/WinIStream.cpp
  Fog/Core/WinUtil.cpp
)

Set(FOG_CORE_LIB_HEADERS
  Fog/Core.h
  Fog/Core/Algorithms.h
  Fog/Core/Application.h
  Fog/Core/Assert.h
  Fog/Core/Atomic.h
  Fog/Core/AutoLock.h
  Fog/Core/AutoUnlock.h
  Fog/Core/Basics.h
  Fog/Core/BitArray.h
  Fog/Core/Byte.h
  Fog/Core/ByteArray.h
  Fog/Core/ByteArrayFilter.h
  Fog/Core/ByteArrayMatcher.h
  Fog/Core/Char.h
  Fog/Core/Constants.h
  Fog/Core/CpuInfo.h
  Fog/Core/CharUtil.h
  Fog/Core/Delegate.h
  Fog/Core/DirIterator.h
  Fog/Core/Event.h
  Fog/Core/EventLoop.h
  Fog/Core/FileSystem.h
  Fog/Core/FileUtil.h
  Fog/Core/Hash.h
  Fog/Core/HashUtil.h
  Fog/Core/Intrin_3dNow.h
  Fog/Core/Intrin_MMX.h
  Fog/Core/Intrin_SSE.h
  Fog/Core/Intrin_SSE2.h
  Fog/Core/Intrin_SSE3.h
  Fog/Core/Intrin_SSSE3.h
  Fog/Core/Lazy.h
  Fog/Core/Library.h
  Fog/Core/List.h
  Fog/Core/ListenerList.h
  Fog/Core/Locale.h
  Fog/Core/Lock.h
  Fog/Core/ManagedString.h
  Fog/Core/MapFile.h
  Fog/Core/Math.h
  Fog/Core/Memory.h
  Fog/Core/Misc.h
  Fog/Core/Object.h
  Fog/Core/OS.h
  Fog/Core/ScopedHandle.h
  Fog/Core/SequenceInfo.h
  Fog/Core/Static.h
  Fog/Core/Std.h
  Fog/Core/Stream.h
  Fog/Core/String.h
  Fog/Core/StringFilter.h
  Fog/Core/StringMatcher.h
  Fog/Core/Strings.h
  Fog/Core/StringUtil.h
  Fog/Core/Task.h
  Fog/Core/TextCodec.h
  Fog/Core/Thread.h
  Fog/Core/ThreadCondition.h
  Fog/Core/ThreadEvent.h
  Fog/Core/ThreadLocalStorage.h
  Fog/Core/ThreadPool.h
  Fog/Core/Time.h
  Fog/Core/Timer.h
  Fog/Core/TypeInfo.h
  Fog/Core/UserInfo.h
  Fog/Core/Value.h
  Fog/Core/WinIStream_p.h
  Fog/Core/WinUtil_p.h

  Fog/Core/Arch/Atomic_gcc_x86x64.h
  Fog/Core/Arch/Atomic_msc_intrin.h
  Fog/Core/Arch/Atomic_msc_x86.h

  Fog/Core/Compiler/PackByte.h
  Fog/Core/Compiler/PackWord.h
  Fog/Core/Compiler/PackDWord.h
  Fog/Core/Compiler/PackQWord.h
  Fog/Core/Compiler/PackRestore.h
)

# Source groups
SOURCE_GROUP(Core FILES ${FOG_CORE_LIB_SOURCES} ${FOG_CORE_LIB_HEADERS})

# =============================================================================
# [Fog-Graphics]
# =============================================================================

Set(FOG_GRAPHICS_LIB_SOURCES
  Fog/Graphics/Argb.cpp
  Fog/Graphics/ArgbAnalyzer.cpp
  Fog/Graphics/ArgbUtil.cpp
  Fog/Graphics/ColorLut.cpp
  Fog/Graphics/ColorMatrix.cpp
  Fog/Graphics/Constants.cpp
  Fog/Graphics/ConvolutionMatrix.cpp
  Fog/Graphics/DitherMatrix.cpp
  Fog/Graphics/Dpi.cpp
  Fog/Graphics/Font.cpp
  Fog/Graphics/Geometry.cpp
  Fog/Graphics/Glyph.cpp
  Fog/Graphics/GlyphCache.cpp
  Fog/Graphics/GlyphSet.cpp
  Fog/Graphics/Gradient.cpp
  Fog/Graphics/Image.cpp
  Fog/Graphics/ImageFilter.cpp
  Fog/Graphics/ImageFilterEngine.cpp
  Fog/Graphics/ImageFilterParams.cpp
  Fog/Graphics/ImageIO.cpp
  Fog/Graphics/Matrix.cpp
  Fog/Graphics/Painter.cpp
  Fog/Graphics/PaintEngine.cpp
  Fog/Graphics/PaintUtil.cpp
  Fog/Graphics/Palette.cpp
  Fog/Graphics/Path.cpp
  Fog/Graphics/PathTransform.cpp
  Fog/Graphics/PathUtil.cpp
  Fog/Graphics/PathUtil_SSE2.cpp
  Fog/Graphics/Pattern.cpp
  Fog/Graphics/RasterEngine.cpp
  Fog/Graphics/Rasterizer.cpp
  Fog/Graphics/Reduce.cpp
  Fog/Graphics/Region.cpp
  Fog/Graphics/RegionPoly.cpp
  Fog/Graphics/RegionRound.cpp
  Fog/Graphics/Scanline.cpp
  Fog/Graphics/Stroker.cpp
  Fog/Graphics/TextIterator.cpp
  Fog/Graphics/TextLayout.cpp
  Fog/Graphics/TextOnPath.cpp
)

Set(FOG_GRAPHICS_LIB_HEADERS
  Fog/Graphics.h
  Fog/Graphics/Argb.h
  Fog/Graphics/ArgbAnalyzer_p.h
  Fog/Graphics/ArgbUtil.h
  Fog/Graphics/ByteUtil_p.h
  Fog/Graphics/ColorLut.h
  Fog/Graphics/ColorMatrix.h
  Fog/Graphics/Constants.h
  Fog/Graphics/Constants_p.h
  Fog/Graphics/ConvolutionMatrix.h
  Fog/Graphics/DitherMatrix_p.h
  Fog/Graphics/Dpi.h
  Fog/Graphics/Font.h
  Fog/Graphics/Geometry.h
  Fog/Graphics/Glyph.h
  Fog/Graphics/GlyphCache.h
  Fog/Graphics/GlyphSet.h
  Fog/Graphics/Gradient.h
  Fog/Graphics/Image.h
  Fog/Graphics/ImageFilter.h
  Fog/Graphics/ImageFilterEngine.h
  Fog/Graphics/ImageFilterParams.h
  Fog/Graphics/ImageIO.h
  Fog/Graphics/Matrix.h
  Fog/Graphics/Painter.h
  Fog/Graphics/PaintEngine.h
  Fog/Graphics/PaintUtil.h
  Fog/Graphics/Palette.h
  Fog/Graphics/Path.h
  Fog/Graphics/PathUtil.h
  Fog/Graphics/PathTransform.h
  Fog/Graphics/Pattern.h
  Fog/Graphics/RasterEngine_p.h
  Fog/Graphics/Rasterizer_p.h
  Fog/Graphics/Reduce_p.h
  Fog/Graphics/Region.h
  Fog/Graphics/Scanline_p.h
  Fog/Graphics/Stroker.h
  Fog/Graphics/TextIterator.h
  Fog/Graphics/TextLayout.h
  Fog/Graphics/TextOnPath.h
)

Set(FOG_GRAPHICS_IMAGEIO_SOURCES
  Fog/Graphics/ImageIO/Bmp.cpp
  Fog/Graphics/ImageIO/GdiPlus.cpp
  Fog/Graphics/ImageIO/Gif.cpp
  Fog/Graphics/ImageIO/Ico.cpp
  Fog/Graphics/ImageIO/Jpeg.cpp
  Fog/Graphics/ImageIO/Pcx.cpp
  Fog/Graphics/ImageIO/Png.cpp
)

Set(FOG_GRAPHICS_IMAGEIO_HEADERS
  Fog/Graphics/ImageIO/Bmp_p.h
  Fog/Graphics/ImageIO/GdiPlus_p.h
  Fog/Graphics/ImageIO/Ico_p.h
  Fog/Graphics/ImageIO/Jpeg_p.h
  Fog/Graphics/ImageIO/Pcx_p.h
  Fog/Graphics/ImageIO/Png_p.h
)

Set(FOG_GRAPHICS_FONTENGINE_SOURCES
  Fog/Graphics/FontEngine/FreeType.cpp
  Fog/Graphics/FontEngine/Gdi.cpp
  Fog/Graphics/FontEngine/Null.cpp
)

Set(FOG_GRAPHICS_FONTENGINE_HEADERS
  Fog/Graphics/FontEngine/FreeType.h
  Fog/Graphics/FontEngine/Gdi.h
  Fog/Graphics/FontEngine/Null_p.h
)

Set(FOG_GRAPHICS_PAINTENGINE_SOURCES
  Fog/Graphics/PaintEngine/Null.cpp
  Fog/Graphics/PaintEngine/Raster.cpp
)

Set(FOG_GRAPHICS_PAINTENGINE_HEADERS
  Fog/Graphics/PaintEngine/Null_p.h
  Fog/Graphics/PaintEngine/Raster_p.h
)

Set(FOG_GRAPHICS_RASTERENGINE_SOURCES
  Fog/Graphics/RasterEngine/Init_C.cpp
  Fog/Graphics/RasterEngine/Init_MMX.cpp
  Fog/Graphics/RasterEngine/Init_MMX3dNow.cpp
  Fog/Graphics/RasterEngine/Init_MMXSSE.cpp
  Fog/Graphics/RasterEngine/Init_SSE2.cpp
  Fog/Graphics/RasterEngine/Init_SSSE3.cpp
)

Set(FOG_GRAPHICS_RASTERENGINE_HEADERS
  Fog/Graphics/RasterEngine/Composite_C_p.h
  Fog/Graphics/RasterEngine/Composite_MMX_p.h
  Fog/Graphics/RasterEngine/Composite_SSE2_p.h
  Fog/Graphics/RasterEngine/Composite_SSSE3_p.h

  Fog/Graphics/RasterEngine/Defs_C_p.h
  Fog/Graphics/RasterEngine/Defs_MMX_p.h
  Fog/Graphics/RasterEngine/Defs_SSE2_p.h
  Fog/Graphics/RasterEngine/Defs_SSSE3_p.h

  Fog/Graphics/RasterEngine/Dib_C_p.h
  Fog/Graphics/RasterEngine/Dib_MMX_p.h
  Fog/Graphics/RasterEngine/Dib_SSE2_p.h
  Fog/Graphics/RasterEngine/Dib_SSSE3_p.h

  Fog/Graphics/RasterEngine/Filters_C_p.h
  Fog/Graphics/RasterEngine/Filters_MMX_p.h
  Fog/Graphics/RasterEngine/Filters_SSE2_p.h
  Fog/Graphics/RasterEngine/Filters_SSSE3_p.h

  Fog/Graphics/RasterEngine/Interpolate_C_p.h
  Fog/Graphics/RasterEngine/Interpolate_MMX_p.h
  Fog/Graphics/RasterEngine/Interpolate_SSE2_p.h
  Fog/Graphics/RasterEngine/Interpolate_SSSE3_p.h

  Fog/Graphics/RasterEngine/Pattern_C_p.h
  Fog/Graphics/RasterEngine/Pattern_MMX_p.h
  Fog/Graphics/RasterEngine/Pattern_SSE2_p.h
  Fog/Graphics/RasterEngine/Pattern_SSSE3_p.h

  Fog/Graphics/RasterEngine/Scale_C_p.h
  Fog/Graphics/RasterEngine/Scale_MMX_p.h
  Fog/Graphics/RasterEngine/Scale_SSE2_p.h
  Fog/Graphics/RasterEngine/Scale_SSSE3_p.h

  Fog/Graphics/RasterEngine/Bresenham_p.h
  Fog/Graphics/RasterEngine/C_p.h
)

# Optimize specific files for speed
Set_Source_Files_Properties(Fog/Graphics/Image.cpp                             PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}")
Set_Source_Files_Properties(Fog/Graphics/PathUtil.cpp                          PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}")
Set_Source_Files_Properties(Fog/Graphics/PathUtil_SSE2.cpp                     PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE2}")
Set_Source_Files_Properties(Fog/Graphics/Rasterizer.cpp                        PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}")
Set_Source_Files_Properties(Fog/Graphics/Region.cpp                            PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}")
Set_Source_Files_Properties(Fog/Graphics/Scanline.cpp                          PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}")

Set_Source_Files_Properties(Fog/Graphics/RasterEngine/Init_C.cpp               PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}")
Set_Source_Files_Properties(Fog/Graphics/RasterEngine/Init_MMX.cpp             PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_MMX}")
Set_Source_Files_Properties(Fog/Graphics/RasterEngine/Init_MMX3dNow.cpp        PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_3DNOW}")
Set_Source_Files_Properties(Fog/Graphics/RasterEngine/Init_MMXSSE.cpp          PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE}")
Set_Source_Files_Properties(Fog/Graphics/RasterEngine/Init_SSE2.cpp            PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE2}")
Set_Source_Files_Properties(Fog/Graphics/RasterEngine/Init_SSSE3.cpp           PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSSE3}")

# Source groups
SOURCE_GROUP(Graphics               FILES ${FOG_GRAPHICS_LIB_SOURCES}          ${FOG_GRAPHICS_LIB_HEADERS})
SOURCE_GROUP(Graphics_ImageIO       FILES ${FOG_GRAPHICS_IMAGEIO_SOURCES}      ${FOG_GRAPHICS_IMAGEIO_HEADERS})
SOURCE_GROUP(Graphics_FontEngine    FILES ${FOG_GRAPHICS_FONTENGINE_SOURCES}   ${FOG_GRAPHICS_FONTENGINE_HEADERS})
SOURCE_GROUP(Graphics_PaintEngine   FILES ${FOG_GRAPHICS_PAINTENGINE_SOURCES}  ${FOG_GRAPHICS_PAINTENGINE_HEADERS})
SOURCE_GROUP(Graphics_RasterEngine  FILES ${FOG_GRAPHICS_RASTERENGINE_SOURCES} ${FOG_GRAPHICS_RASTERENGINE_HEADERS})

# =============================================================================
# [Fog-Xml]
# =============================================================================

Set(FOG_XML_LIB_SOURCES
  Fog/Xml/XmlDom.cpp
  Fog/Xml/XmlEntity.cpp
  Fog/Xml/XmlReader.cpp
  Fog/Xml/XmlWriter.cpp
)

Set(FOG_XML_LIB_HEADERS
  Fog/Xml.h
  Fog/Xml/Constants.h
  Fog/Xml/XmlDom.h
  Fog/Xml/XmlEntity_p.h
  Fog/Xml/XmlReader.h
  Fog/Xml/XmlWriter.h
)

SOURCE_GROUP(Xml FILES ${FOG_XML_LIB_SOURCES} ${FOG_XML_LIB_HEADERS})

# =============================================================================
# [Fog-Svg]
# =============================================================================

Set(FOG_SVG_LIB_SOURCES
  Fog/Svg/SvgBase.cpp
  Fog/Svg/SvgContext.cpp
  Fog/Svg/SvgDom.cpp
  Fog/Svg/SvgUtil.cpp
)

Set(FOG_SVG_LIB_HEADERS
  Fog/Svg.h
  Fog/Svg/Constants.h
  Fog/Svg/SvgBase.h
  Fog/Svg/SvgContext.h
  Fog/Svg/SvgDom.h
  Fog/Svg/SvgUtil.h
)

# Source groups
SOURCE_GROUP(Svg FILES ${FOG_SVG_LIB_SOURCES} ${FOG_SVG_LIB_HEADERS})

# =============================================================================
# [Fog-Gui]
# =============================================================================

Set(FOG_GUI_LIB_SOURCES
  Fog/Gui/Animation.cpp
  Fog/Gui/Button.cpp
  Fog/Gui/ButtonBase.cpp
  Fog/Gui/CheckBox.cpp
  Fog/Gui/Composite.cpp
  Fog/Gui/Event.cpp
  Fog/Gui/GuiEngine.cpp
  Fog/Gui/Layout/FlowLayout.cpp
  Fog/Gui/Layout/BoxLayout.cpp
  Fog/Gui/Layout/GridLayout.cpp
  Fog/Gui/Layout/Layout.cpp
  Fog/Gui/Layout/LayoutHint.cpp
  Fog/Gui/Layout/LayoutItem.cpp
  Fog/Gui/Layout/LayoutUtils.cpp
  Fog/Gui/Theme.cpp
  Fog/Gui/Widget.cpp
  Fog/Gui/Window.cpp
)

Set(FOG_GUI_LIB_HEADERS
  Fog/Gui/Animation.h
  Fog/Gui.h
  Fog/Gui/Button.h
  Fog/Gui/ButtonBase.h
  Fog/Gui/CheckBox.h
  Fog/Gui/Composite.h
  Fog/Gui/Constants.h
  Fog/Gui/Event.h
  Fog/Gui/GuiEngine.h
  Fog/Gui/Layout/BoxLayout.h
  Fog/Gui/Layout/GridLayout.h
  Fog/Gui/Layout/FlowLayout.h
  Fog/Gui/Layout/Layout.h
  Fog/Gui/Layout/LayoutHint.h
  Fog/Gui/Layout/LayoutItem.h
  Fog/Gui/Layout/LayoutPolicy.h
  Fog/Gui/Layout/LayoutUtils.h
  Fog/Gui/Theme.h
  Fog/Gui/Widget.h
  Fog/Gui/Widget_p.h
  Fog/Gui/Window.h
)

Set(FOG_GUI_ENGINE_BASE_SOURCES
  Fog/Gui/GuiEngine/Base.cpp
)

Set(FOG_GUI_ENGINE_BASE_HEADERS
  Fog/Gui/GuiEngine/Base.h
)

Set(FOG_GUI_ENGINE_WIN_SOURCES
  Fog/Gui/GuiEngine/Win.cpp
)

Set(FOG_GUI_ENGINE_WIN_HEADERS
  Fog/Gui/GuiEngine/Win.h
)

Set(FOG_GUI_ENGINE_X11_SOURCES
  Fog/Gui/GuiEngine/X11.cpp
)

Set(FOG_GUI_ENGINE_X11_HEADERS
  Fog/Gui/GuiEngine/X11.h
  Fog/Gui/GuiEngine/X11Headers.h
)

Set(FOG_GUI_THEME_SOURCES
  Fog/Gui/Theme/Default.cpp
)

Set(FOG_GUI_THEME_HEADERS
  Fog/Gui/Theme/Default.h
)

# Source groups
SOURCE_GROUP(Gui FILES
  ${FOG_GUI_LIB_SOURCES} ${FOG_GUI_LIB_HEADERS}
)

SOURCE_GROUP(Gui_GuiEngine FILES
  ${FOG_GUI_ENGINE_BASE_SOURCES} ${FOG_GUI_ENGINE_BASE_HEADERS}
  ${FOG_GUI_ENGINE_X11_SOURCES} ${FOG_GUI_ENGINE_X11_HEADERS}
  ${FOG_GUI_ENGINE_WIN_SOURCES} ${FOG_GUI_ENGINE_WIN_HEADERS}
)

SOURCE_GROUP(Gui_Theme FILES
  ${FOG_GUI_THEME_SOURCES} ${FOG_GUI_THEME_HEADERS}
)

# Gui backends.
If(WIN32)
  Set(FOG_GUI_WINDOWS 1)
Else(WIN32)
  Set(FOG_GUI_X11 1)
EndIf(WIN32)

# =============================================================================
# [Finalize]
# =============================================================================

Set(FOG_ALL_SOURCES
  ${FOG_BUILD_SOURCES}
  ${FOG_CORE_LIB_SOURCES}
  ${FOG_XML_LIB_SOURCES}
  ${FOG_GRAPHICS_LIB_SOURCES}
  ${FOG_GRAPHICS_IMAGEIO_SOURCES}
  ${FOG_GRAPHICS_FONTENGINE_SOURCES}
  ${FOG_GRAPHICS_PAINTENGINE_SOURCES}
  ${FOG_GRAPHICS_RASTERENGINE_SOURCES}
  ${FOG_SVG_LIB_SOURCES}
  ${FOG_GUI_LIB_SOURCES}
  ${FOG_GUI_ENGINE_BASE_SOURCES}
  ${FOG_GUI_THEME_SOURCES}
)

Set(FOG_ALL_HEADERS
  ${FOG_BUILD_HEADERS}
  ${FOG_CORE_LIB_HEADERS}
  ${FOG_XML_LIB_HEADERS}
  ${FOG_GRAPHICS_LIB_HEADERS}
  ${FOG_GRAPHICS_IMAGEIO_HEADERS}
  ${FOG_GRAPHICS_FONTENGINE_HEADERS}
  ${FOG_GRAPHICS_PAINTENGINE_HEADERS}
  ${FOG_GRAPHICS_RASTERENGINE_HEADERS}
  ${FOG_SVG_LIB_HEADERS}
  ${FOG_GUI_LIB_HEADERS}
  ${FOG_GUI_ENGINE_BASE_HEADERS}
  ${FOG_GUI_ENGINE_WIN_HEADERS}
  ${FOG_GUI_ENGINE_X11_HEADERS}
  ${FOG_GUI_THEME_HEADERS}
)

If(FOG_GUI_WINDOWS)
  Set(FOG_ALL_SOURCES ${FOG_ALL_SOURCES} ${FOG_GUI_ENGINE_WIN_SOURCES})
  Set(FOG_ALL_HEADERS ${FOG_ALL_HEADERS} ${FOG_GUI_ENGINE_WIN_HEADERS})
EndIf(FOG_GUI_WINDOWS)

# Write CMAKE_ #defines into Fog/Build/Config.h files
Configure_File(${FOG_DIR}/Fog/Build/Config.h.cmake ${FOG_DIR}/Fog/Build/Config.h)

# Trim libraries string (come compilers can complain if there are extra whitespaces).
String(STRIP "${FOG_LIBRARIES}" FOG_LIBRARIES)

# Install header files.
ForEach(i ${FOG_ALL_HEADERS})
  Get_Filename_Component(path ${i} PATH)
  Get_Filename_Component(name ${i} NAME)
  If(NOT "${name}" MATCHES "_p.h$")
    Install(FILES ${i} DESTINATION "include/${path}")
  EndIf(NOT "${name}" MATCHES "_p.h$")
EndForEach(i)

# Build Fog library.
Add_Library(Fog SHARED ${FOG_ALL_SOURCES} ${FOG_ALL_HEADERS})
Target_Link_Libraries(Fog ${FOG_LIBRARIES})
Install(TARGETS Fog DESTINATION lib)

# Build Fog_Gui_X11 plugin.
If(FOG_GUI_X11 AND FOG_BUILD_GUI_X11)
  Add_Library(Fog_Gui_X11 SHARED ${FOG_GUI_ENGINE_X11_SOURCES})
  Target_Link_Libraries(Fog_Gui_X11 Fog ${FOG_LIBRARIES})
  Install(TARGETS Fog_Gui_X11 DESTINATION lib)
EndIf(FOG_GUI_X11 AND FOG_BUILD_GUI_X11)

# Build examples.
If(FOG_BUILD_EXAMPLES)
  Set(FOG_EXAMPLE_SOURCES
    FogAnim
    FogBlur
    FogComposite
    FogGradients
    FogSvgView
    FogTest
  )
  ForEach(file ${FOG_EXAMPLE_SOURCES})
    Add_Executable(${file} WIN32 Examples/${file})
    Target_Link_Libraries(${file} Fog ${FOG_LIBRARIES})
  EndForEach(file)
EndIf(FOG_BUILD_EXAMPLES)

# Build benchmark.
If(FOG_BUILD_BENCH)
  If(NOT WIN32)
    Include_Directories(/usr/include/cairo)
  EndIf(NOT WIN32)

  Add_Executable(FogBench Examples/FogBench.cpp)
  Target_Link_Libraries(FogBench Fog ${FOG_LIBRARIES})

  If(NOT WIN32)
    Target_Link_Libraries(FogBench cairo)
  EndIf(NOT WIN32)
EndIf(FOG_BUILD_BENCH)

