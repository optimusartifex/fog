# =============================================================================
# [Project]
# =============================================================================

CMake_Minimum_Required(VERSION 2.8)

Message("== ========================================")
Message("== [Fog-Framework]")
Message("== ========================================")

If(NOT FOG_BUILD_STATIC)
  Project(Fog C CXX)
  Message("-- Fog cmake-project created")
Else()
  Message("-- Fog included from existing list-file")
EndIf()

# =============================================================================
# [Configuration]
# =============================================================================

# Whether to build static library (default FALSE)
# Set(FOG_BUILD_STATIC TRUE)

# Whether to build examples (default FALSE).
# Set(FOG_BUILD_EXAMPLES TRUE)

# Whether to build support for X11 (default FALSE)
# Set(FOG_BUILD_GUI_X11 TRUE)

# =============================================================================
# [Basics]
# =============================================================================

If(NOT FOG_DIR)
  Set(FOG_DIR ${CMAKE_CURRENT_LIST_DIR})
  Message("-- Initializing FOG_DIR=${FOG_DIR}")
Else()
  Message("-- Using Custom FOG_DIR=${FOG_DIR}")
EndIf()

Set(FOG_LIBRARIES "")
Set(CMAKE_CXX_STANDARD_LIBRARIES "")

Include(CheckCXXCompilerFlag)
Include(CheckIncludeFiles)
Include(CheckLibraryExists)
Include(CheckTypeSize)
Include(FindThreads)

# =============================================================================
# [Include Directories]
# =============================================================================

Include_Directories(${FOG_DIR})

# =============================================================================
# [OS - Detect]
# =============================================================================

If(WIN32)
  Set(FOG_OS_WINDOWS TRUE)
ElseIf(APPLE)
  Set(FOG_OS_MAC TRUE)
Else()
  Set(FOG_OS_POSIX TRUE)
EndIf()

# =============================================================================
# [C++ Compiler - Clear Flags]
# =============================================================================

Set(FOG_CFLAGS_OPTIMIZE "")
Set(FOG_CFLAGS_MMX "")
Set(FOG_CFLAGS_3DNOW "")
Set(FOG_CFLAGS_SSE "")
Set(FOG_CFLAGS_SSE2 "")
Set(FOG_CFLAGS_SSE3 "")
Set(FOG_CFLAGS_SSSE3 "")

# =============================================================================
# [C++ Compiler - GCC Support]
# =============================================================================

If(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Check for supported compiler flags.
  Check_CXX_Compiler_Flag("-fmerge-all-constants" FOG_CC_HAS_FMERGE_ALL_CONSTANTS)
  Check_CXX_Compiler_Flag("-fno-keep-static-consts" FOG_CC_HAS_FNO_KEEP_STATIC_CONSTS)
  Check_CXX_Compiler_Flag("-flto" FOG_CC_HAS_FLTO)

  Check_CXX_Compiler_Flag("-Winline" FOG_CC_HAS_WINLINE)
  Check_CXX_Compiler_Flag("-Wno-enum-compare" FOG_CC_HAS_WNO_ENUM_COMPARE)

  # Turn off exceptions (smaller code).
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

  # Hide all symbols by default. Exported symbols are declared by FOG_API.
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

  # -fmerge-all-constants / -fno-keep-static-consts
  #
  # Decrease code size by merging all constants (standard violation) and 
  # removing unused ones. It's against standard so it must be turned-on
  # explicitly.
  If (${FOG_CC_HAS_FMERGE_ALL_CONSTANTS})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmerge-all-constants")
  EndIf()
  If (${FOG_CC_HAS_FNO_KEEP_STATIC_CONSTS})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-keep-static-consts")
  EndIf()

  # Strict aliasing, Fog-Framework should be well written.
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing")

  # We are not interested in math-errno.
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno")

  # -flto
  #
  # Use link-time optimization.
  If (${FOG_CC_HAS_FLTO})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
  EndIf()

  # -Winline:
  #
  # I want to see all functions which are not inlined. This could help when
  # looking for non-inlined function in Fog::G2d inner loops.
  If (${FOG_CC_HAS_WINLINE})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
  EndIf()

  # -Wno-enum-compare.
  If (${FOG_CC_HAS_WNO_ENUM_COMPARE})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-enum-compare")
  EndIf()

  # Use -O2 as a standard optimization level when running release builds. The
  # -O3 not improve the performance, because all inner loops used by Fog are
  # generally optimized - there is no that compiler can do. Another argument
  # to use -O2 is that -O3 produces larger binaries with no speed improvement.
  If(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    Set(FOG_CFLAGS_OPTIMIZE "-O2")
  EndIf()

  # Setup FOG_CFLAGS specific to GCC.
  Set(FOG_CFLAGS_MMX "${FOG_CFLAGS_OPTIMIZE} -mmmx")
  Set(FOG_CFLAGS_3DNOW "${FOG_CFLAGS_OPTIMIZE} -mmmx -m3dnow")
  Set(FOG_CFLAGS_SSE "${FOG_CFLAGS_OPTIMIZE} -mmmx -msse")
  Set(FOG_CFLAGS_SSE2 "${FOG_CFLAGS_OPTIMIZE} -msse2")
  Set(FOG_CFLAGS_SSE3 "${FOG_CFLAGS_OPTIMIZE} -msse3")
  Set(FOG_CFLAGS_SSSE3 "${FOG_CFLAGS_OPTIMIZE} -mssse3")
EndIf()

# =============================================================================
# [C++ Compiler - MSVC Support]
# =============================================================================

If(MSVC)
  Set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi /Oy /GF /GS- /GR-")

  Set(FOG_CFLAGS_OPTIMIZE "/Ot")
  Set(FOG_CFLAGS_MMX "${FOG_CFLAGS_OPTIMIZE}")
  Set(FOG_CFLAGS_3DNOW "${FOG_CFLAGS_OPTIMIZE}")
  Set(FOG_CFLAGS_SSE "${FOG_CFLAGS_OPTIMIZE} /arch:SSE")
  Set(FOG_CFLAGS_SSE2 "${FOG_CFLAGS_OPTIMIZE} /arch:SSE2")
  Set(FOG_CFLAGS_SSE3 "${FOG_CFLAGS_OPTIMIZE} /arch:SSE2")
  Set(FOG_CFLAGS_SSSE3 "${FOG_CFLAGS_OPTIMIZE} /arch:SSE2")
EndIf()

If(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  Set(FOG_DEBUG TRUE)
EndIf()

# =============================================================================
# [C++ Compiler - Windows Specific Flags]
# =============================================================================

# Always use UNICODE when building for Windows.
If(FOG_OS_WINDOWS)
  Add_Definitions(-D_UNICODE)
EndIf()

# =============================================================================
# [Architecture - Endianness]
# =============================================================================

Include(TestBigEndian)
Test_Big_Endian(WORDS_BIGENDIAN)

If(WORDS_BIGENDIAN)
  Set(FOG_BYTE_ORDER 0)
Else()
  Set(FOG_BYTE_ORDER 1)
EndIf()

# =============================================================================
# [Architecture - Size of POD types]
# =============================================================================

Check_Type_Size("void*" FOG_SIZEOF_VOID)
Check_Type_Size("int" FOG_SIZEOF_INT)
Check_Type_Size("long" FOG_SIZEOF_LONG)
Set(CMAKE_EXTRA_INCLUDE_FILES wchar.h)
Check_Type_Size("wchar_t" FOG_SIZEOF_WCHAR_T)
Set(CMAKE_EXTRA_INCLUDE_FILES)

# =============================================================================
# [Architecture - Enable/Disable Optimizations]
# =============================================================================

# Disable MMX/3dNow while building for AMD64.
If (${FOG_SIZEOF_VOID} EQUAL 4)
  Set(FOG_OPTIMIZE_MMX TRUE)
  # Set(FOG_OPTIMIZE_3DNOW TRUE)
EndIf()

Set(FOG_OPTIMIZE_SSE TRUE)
Set(FOG_OPTIMIZE_SSE2 TRUE)
Set(FOG_OPTIMIZE_SSSE3 TRUE)

# =============================================================================
# [Environment - Header Files]
# =============================================================================

If(NOT FOG_HAVE_FLOAT_H)
  Check_Include_Files(float.h FOG_HAVE_FLOAT_H)
EndIf()

If(NOT FOG_HAVE_LIMITS_H)
  Check_Include_Files(limits.h FOG_HAVE_LIMITS_H)
EndIf()

If(NOT FOG_HAVE_STDINT_H)
  Check_Include_Files(stdint.h FOG_HAVE_STDINT_H)
EndIf()

If(UNIX)
  If(NOT FOG_HAVE_UNISTD_H)
    Check_Include_Files(unistd.h FOG_HAVE_UNISTD_H)
  EndIf()

  If(NOT FOG_HAVE_FENV_H)
    Check_Include_Files(fenv.h FOG_HAVE_FENV_H)
  EndIf()
EndIf()

# =============================================================================
# [OS - Libraries]
# =============================================================================

If(FOG_OS_WINDOWS)
  List(APPEND FOG_LIBRARIES user32 gdi32 advapi32 winmm uuid)
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_LIBRARIES "-framework Cocoa")
EndIf()

If(FOG_OS_POSIX)
  # librt is Linux specific library (not available in BSD/MAC).
  If(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    List(APPEND FOG_LIBRARIES "-lrt")
  EndIf()

  # libdl for dynamic-library opening.
  Check_Include_Files(dlfcn.h FOG_HAVE_DLFCN_H)
  If(FOG_HAVE_DLFCN_H)
    Check_Library_Exists(dl dlopen "" FOG_LIBDL_EXISTS)
    If(FOG_LIBDL_EXISTS)
      List(APPEND FOG_LIBRARIES "-ldl")
    EndIf()
  EndIf()
EndIf()

If(CMAKE_THREAD_LIBS_INIT)
  List(APPEND FOG_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
EndIf()

# =============================================================================
# [Fog/Core - Files]
# =============================================================================

# [Fog/Core]
Set(FOG_CORE_LIB_SOURCES
)

Set(FOG_CORE_LIB_HEADERS
  Fog/Core.h
)

# [Fog/Core/Collection]
Set(FOG_CORE_COLLECTION_SOURCES
  Fog/Core/Collection/Algorithms.cpp
  Fog/Core/Collection/List.cpp
  Fog/Core/Collection/Hash.cpp
  Fog/Core/Collection/HashUtil.cpp
  Fog/Core/Collection/PBuffer.cpp
  Fog/Core/Collection/PHash.cpp
  Fog/Core/Collection/PList.cpp
  Fog/Core/Collection/PStack.cpp
  Fog/Core/Collection/Util.cpp
)

Set(FOG_CORE_COLLECTION_HEADERS
  Fog/Core/Collection/Algorithms.h
  Fog/Core/Collection/List.h
  Fog/Core/Collection/Hash.h
  Fog/Core/Collection/HashUtil.h
  Fog/Core/Collection/PBuffer.h
  Fog/Core/Collection/PHash.h
  Fog/Core/Collection/PList.h
  Fog/Core/Collection/PStack.h
  Fog/Core/Collection/Util.h
)

# [Fog/Core/Config]
Set(FOG_CORE_CONFIG_SOURCES
)

Set(FOG_CORE_CONFIG_HEADERS
  Fog/Core/Config/Config.h
  Fog/Core/Config/ConfigArch.h
  Fog/Core/Config/ConfigCC.h
  Fog/Core/Config/ConfigCC_gcc_compat.h
  Fog/Core/Config/ConfigCC_msc_compat.h
  Fog/Core/Config/ConfigCC_ide.h
  Fog/Core/Config/ConfigHeaders.h
  Fog/Core/Config/ConfigMacros.h
  Fog/Core/Config/ConfigMain.h
  Fog/Core/Config/ConfigNew.h
  Fog/Core/Config/ConfigOS.h
  Fog/Core/Config/ConfigStdInt.h
  Fog/Core/Config/Defs/ConfigCMake.h
)

# [Fog/Core/Cpu]
Set(FOG_CORE_CPU_SOURCES
  Fog/Core/Cpu/Cpu.cpp
)

Set(FOG_CORE_CPU_HEADERS
  Fog/Core/Cpu/Cpu.h
  Fog/Core/Cpu/Initializer.h
  Fog/Core/Cpu/Intrin_3dNow.h
  Fog/Core/Cpu/Intrin_MMX.h
  Fog/Core/Cpu/Intrin_MMXExt.h
  Fog/Core/Cpu/Intrin_SSE.h
  Fog/Core/Cpu/Intrin_SSE2.h
  Fog/Core/Cpu/Intrin_SSE3.h
  Fog/Core/Cpu/Intrin_SSSE3.h
)

# [Fog/Core/Data]
Set(FOG_CORE_DATA_SOURCES
  Fog/Core/Data/Value.cpp
)

Set(FOG_CORE_DATA_HEADERS
  Fog/Core/Data/Value.h
)

# [Fog/Core/DateTime]
Set(FOG_CORE_DATETIME_SOURCES
  Fog/Core/DateTime/Time.cpp
  Fog/Core/DateTime/TimeDelta.cpp
  Fog/Core/DateTime/TimeTicks.cpp
)

Set(FOG_CORE_DATETIME_HEADERS
  Fog/Core/DateTime/Time.h
  Fog/Core/DateTime/TimeDelta.h
  Fog/Core/DateTime/TimeTicks.h
)

# [Fog/Core/Face]
Set(FOG_CORE_FACE_SOURCES
  Fog/Core/Face/Face_Constants.cpp
)

Set(FOG_CORE_FACE_HEADERS
  Fog/Core/Face/Face_3dNow.h
  Fog/Core/Face/Face_3dNowExt.h
  Fog/Core/Face/Face_C.h
  Fog/Core/Face/Face_C_F32.h
  Fog/Core/Face/Face_C_G2d.h
  Fog/Core/Face/Face_C_P32.h
  Fog/Core/Face/Face_C_P64.h
  Fog/Core/Face/Face_C_Types.h
  Fog/Core/Face/Face_Constants.h
  Fog/Core/Face/Face_MMX.h
  Fog/Core/Face/Face_MMX_G2d.h
  Fog/Core/Face/Face_MMXExt.h
  Fog/Core/Face/Face_MMXExt_G2d.h
  Fog/Core/Face/Face_SSE.h
  Fog/Core/Face/Face_SSE_G2d.h
  Fog/Core/Face/Face_SSE2.h
  Fog/Core/Face/Face_SSE2_G2d.h
  Fog/Core/Face/Face_SSE3.h
  Fog/Core/Face/Face_SSE3_G2d.h
  Fog/Core/Face/Face_SSSE3.h
  Fog/Core/Face/Face_SSSE3_G2d.h
  Fog/Core/Face/Features.h
)

# [Fog/Core/Global]
Set(FOG_CORE_GLOBAL_SOURCES
  Fog/Core/Global/Api.cpp
  Fog/Core/Global/Assert.cpp
  Fog/Core/Global/Debug.cpp
  Fog/Core/Global/Error.cpp
  Fog/Core/Global/Init.cpp
  Fog/Core/Global/Init_Core.cpp
  Fog/Core/Global/Uninitialized.cpp
)

Set(FOG_CORE_GLOBAL_HEADERS
  Fog/Core/Global/Api.h
  Fog/Core/Global/Assert.h
  Fog/Core/Global/Constants.h
  Fog/Core/Global/Debug.h
  Fog/Core/Global/Error.h
  Fog/Core/Global/Init.h
  Fog/Core/Global/Init_Core_p.h
  Fog/Core/Global/Static.h
  Fog/Core/Global/SequenceInfo.h
  Fog/Core/Global/Swap.h
  Fog/Core/Global/TypeInfo.h
  Fog/Core/Global/TypeVariant.h
  Fog/Core/Global/Uninitialized.h
)

# [Fog/Core/IO]
Set(FOG_CORE_IO_SOURCES
  Fog/Core/IO/DirEntry.cpp
  Fog/Core/IO/DirIterator.cpp
  Fog/Core/IO/FileSystem.cpp
  Fog/Core/IO/MapFile.cpp
  Fog/Core/IO/Stream.cpp
)

Set(FOG_CORE_IO_HEADERS
  Fog/Core/IO/DirEntry.h
  Fog/Core/IO/DirIterator.h
  Fog/Core/IO/FileSystem.h
  Fog/Core/IO/MapFile.h
  Fog/Core/IO/Stream.h
)

# [Fog/Core/Library]
Set(FOG_CORE_LIBRARY_SOURCES
  Fog/Core/Library/Library.cpp
)

Set(FOG_CORE_LIBRARY_HEADERS
  Fog/Core/Library/Library.h
)

# [Fog/Core/Math]
Set(FOG_CORE_MATH_SOURCES
  Fog/Core/Math/Cleanup.h
  Fog/Core/Math/Constants.h
  Fog/Core/Math/Fixed.h
  Fog/Core/Math/FloatBits.h
  Fog/Core/Math/FloatControl.h
  Fog/Core/Math/Fuzzy.h
  Fog/Core/Math/Math.h
  Fog/Core/Math/Solve.h
)

Set(FOG_CORE_MATH_HEADERS
  Fog/Core/Math/Fixed.cpp
  Fog/Core/Math/Fuzzy.cpp
  Fog/Core/Math/Math.cpp
  Fog/Core/Math/Solve.cpp
)

# [Fog/Core/Memory]
Set(FOG_CORE_MEMORY_SOURCES
  Fog/Core/Memory/BlockMemoryAllocator.cpp
  Fog/Core/Memory/Memory.cpp
  Fog/Core/Memory/MemoryBuffer.cpp
  Fog/Core/Memory/MemoryDebugger.cpp
  Fog/Core/Memory/MemoryManager.cpp
  Fog/Core/Memory/ZoneMemoryAllocator.cpp
)

Set(FOG_CORE_MEMORY_HEADERS
  Fog/Core/Memory/BlockMemoryAllocator_p.h
  Fog/Core/Memory/BSwap.h
  Fog/Core/Memory/BSwap_gcc_pcc.h
  Fog/Core/Memory/BSwap_gcc_x86x64.h
  Fog/Core/Memory/BSwap_generic.h
  Fog/Core/Memory/BSwap_msc_intrin.h
  Fog/Core/Memory/BSwap_msc_x86.h
  Fog/Core/Memory/Memory.h
  Fog/Core/Memory/MemoryBuffer.h
  Fog/Core/Memory/MemoryDebugger_p.h
  Fog/Core/Memory/MemoryManager.h
  Fog/Core/Memory/MemoryOps.h
  Fog/Core/Memory/ZoneMemoryAllocator_p.h
)

# [Fog/Core/OS]
Set(FOG_CORE_OS_SOURCES
  Fog/Core/OS/OS.cpp
  Fog/Core/OS/UserInfo.cpp
)

Set(FOG_CORE_OS_HEADERS
  Fog/Core/OS/OS.h
  Fog/Core/OS/UserInfo.h
)

# [Fog/Core/Pack]
Set(FOG_CORE_PACK_SOURCES
)

Set(FOG_CORE_PACK_HEADERS
  Fog/Core/Pack/PackByte.h
  Fog/Core/Pack/PackWord.h
  Fog/Core/Pack/PackDWord.h
  Fog/Core/Pack/PackQWord.h
  Fog/Core/Pack/PackRestore.h
)

# [Fog/Core/System]
Set(FOG_CORE_SYSTEM_SOURCES
  Fog/Core/System/Application.cpp
  Fog/Core/System/Event.cpp
  Fog/Core/System/EventLoop.cpp
  Fog/Core/System/ListenerList.cpp
  Fog/Core/System/Object.cpp
  Fog/Core/System/Task.cpp
  Fog/Core/System/Timer.cpp
)

Set(FOG_CORE_SYSTEM_HEADERS
  Fog/Core/System/Application.h
  Fog/Core/System/Delegate.h
  Fog/Core/System/Event.h
  Fog/Core/System/EventLoop.h
  Fog/Core/System/ListenerList.h
  Fog/Core/System/Object.h
  Fog/Core/System/Task.h
  Fog/Core/System/Timer.h
)

# [Fog/Core/Threading]
Set(FOG_CORE_THREADING_SOURCES
  Fog/Core/Threading/Lock.cpp
  Fog/Core/Threading/Thread.cpp
  Fog/Core/Threading/ThreadCondition.cpp
  Fog/Core/Threading/ThreadEvent.cpp
  Fog/Core/Threading/ThreadLocalStorage.cpp
  Fog/Core/Threading/ThreadPool.cpp
)

Set(FOG_CORE_THREADING_HEADERS
  Fog/Core/Threading/Atomic.h
  Fog/Core/Threading/Atomic_gcc_x86x64.h
  Fog/Core/Threading/Atomic_gcc_intrin.h
  Fog/Core/Threading/Atomic_msc_intrin.h
  Fog/Core/Threading/Atomic_msc_x86.h
  Fog/Core/Threading/AtomicPadding.h
  Fog/Core/Threading/Lock.h
  Fog/Core/Threading/Thread.h
  Fog/Core/Threading/ThreadCondition.h
  Fog/Core/Threading/ThreadEvent.h
  Fog/Core/Threading/ThreadLocalStorage.h
  Fog/Core/Threading/ThreadPool.h
)

# [Fog/Core/Tools]
Set(FOG_CORE_TOOLS_SOURCES
  Fog/Core/Tools/Byte.cpp
  Fog/Core/Tools/ByteArray.cpp
  Fog/Core/Tools/ByteArrayFilter.cpp
  Fog/Core/Tools/ByteArrayMatcher.cpp
  Fog/Core/Tools/Char.cpp
  Fog/Core/Tools/CharUtil.cpp
  Fog/Core/Tools/Format.cpp
  Fog/Core/Tools/Lazy.cpp
  Fog/Core/Tools/Locale.cpp
  Fog/Core/Tools/ManagedString.cpp
  Fog/Core/Tools/Range.cpp
  Fog/Core/Tools/String.cpp
  Fog/Core/Tools/StringFilter.cpp
  Fog/Core/Tools/StringMatcher.cpp
  Fog/Core/Tools/Strings.cpp
  Fog/Core/Tools/StringUtil.cpp
  Fog/Core/Tools/StringUtil_gen.cpp
  Fog/Core/Tools/Stub.cpp
  Fog/Core/Tools/TextCodec.cpp
)

Set(FOG_CORE_TOOLS_HEADERS
  Fog/Core/Tools/Byte.h
  Fog/Core/Tools/ByteArray.h
  Fog/Core/Tools/ByteArrayFilter.h
  Fog/Core/Tools/ByteArrayMatcher.h
  Fog/Core/Tools/Char.h
  Fog/Core/Tools/CharUtil.h
  Fog/Core/Tools/Format.h
  Fog/Core/Tools/Lazy.h
  Fog/Core/Tools/Locale.h
  Fog/Core/Tools/ManagedString.h
  Fog/Core/Tools/Range.h
  Fog/Core/Tools/String.h
  Fog/Core/Tools/StringFilter.h
  Fog/Core/Tools/StringMatcher.h
  Fog/Core/Tools/Strings.h
  Fog/Core/Tools/StringUtil.h
  Fog/Core/Tools/Stub.h
  Fog/Core/Tools/TextCodec.h
)

# [Fog/Core/Win]
If(FOG_OS_WINDOWS)
  Set(FOG_CORE_WIN_SOURCES 
    Fog/Core/Win/WinCom.cpp
    Fog/Core/Win/WinComStream.cpp
    Fog/Core/Win/WinGdipLibrary.cpp
    Fog/Core/Win/WinUtil.cpp
    Fog/Core/Win/WinUxThemeLibrary.cpp
  )
  Set(FOG_CORE_WIN_HEADERS
    Fog/Core/Win/WinCom.h
    Fog/Core/Win/WinComStream_p.h
    Fog/Core/Win/WinGdipLibrary.h
    Fog/Core/Win/WinUtil_p.h
    Fog/Core/Win/WinUxThemeLibrary.h
  )
EndIf()

# [Fog/Core/Mac]
If(FOG_OS_MAC)
  Set(FOG_CORE_MAC_SOURCES 
    Fog/Core/Mac/MacUtil.mm
  )
  Set(FOG_CORE_MAC_HEADERS
    Fog/Core/Mac/MacUtil.h
  )
EndIf()

# Source groups.
Source_Group("Core"                 FILES ${FOG_CORE_LIB_SOURCES}             ${FOG_CORE_LIB_HEADERS}            )
Source_Group("Core/Collection"      FILES ${FOG_CORE_COLLECTION_SOURCES}      ${FOG_CORE_COLLECTION_HEADERS}     )
Source_Group("Core/Config"          FILES ${FOG_CORE_CONFIG_SOURCES}          ${FOG_CORE_CONFIG_HEADERS}         )
Source_Group("Core/Cpu"             FILES ${FOG_CORE_CPU_SOURCES}             ${FOG_CORE_CPU_HEADERS}            )
Source_Group("Core/Data"            FILES ${FOG_CORE_DATA_SOURCES}            ${FOG_CORE_DATA_HEADERS}           )
Source_Group("Core/DateTime"        FILES ${FOG_CORE_DATETIME_SOURCES}        ${FOG_CORE_DATETIME_HEADERS}       )
Source_Group("Core/Face"            FILES ${FOG_CORE_FACE_SOURCES}            ${FOG_CORE_FACE_HEADERS}           )
Source_Group("Core/Global"          FILES ${FOG_CORE_GLOBAL_SOURCES}          ${FOG_CORE_GLOBAL_HEADERS}         )
Source_Group("Core/IO"              FILES ${FOG_CORE_IO_SOURCES}              ${FOG_CORE_IO_HEADERS}             )
Source_Group("Core/Library"         FILES ${FOG_CORE_LIBRARY_SOURCES}         ${FOG_CORE_LIBRARY_HEADERS}        )
Source_Group("Core/Math"            FILES ${FOG_CORE_MATH_SOURCES}            ${FOG_CORE_MATH_HEADERS}           )
Source_Group("Core/Memory"          FILES ${FOG_CORE_MEMORY_SOURCES}          ${FOG_CORE_MEMORY_HEADERS}         )
Source_Group("Core/Object"          FILES ${FOG_CORE_OBJECT_SOURCES}          ${FOG_CORE_OBJECT_HEADERS}         )
Source_Group("Core/OS"              FILES ${FOG_CORE_OS_SOURCES}              ${FOG_CORE_OS_HEADERS}             )
Source_Group("Core/Pack"            FILES ${FOG_CORE_PACK_SOURCES}            ${FOG_CORE_PACK_HEADERS}           )
Source_Group("Core/System"          FILES ${FOG_CORE_SYSTEM_SOURCES}          ${FOG_CORE_SYSTEM_HEADERS}         )
Source_Group("Core/Threading"       FILES ${FOG_CORE_THREADING_SOURCES}       ${FOG_CORE_THREADING_HEADERS}      )
Source_Group("Core/Tools"           FILES ${FOG_CORE_TOOLS_SOURCES}           ${FOG_CORE_TOOLS_HEADERS}          )

If(FOG_OS_WINDOWS)
Source_Group("Core/Win"             FILES ${FOG_CORE_WIN_SOURCES}             ${FOG_CORE_WIN_HEADERS}            )
EndIf()

If(FOG_OS_MAC)
Source_Group("Core/Mac"             FILES ${FOG_CORE_MAC_SOURCES}             ${FOG_CORE_MAC_HEADERS}            )
EndIf()

# =============================================================================
# [Fog/G2d - Files]
# =============================================================================

# [Fog/G2d]
Set(FOG_G2D_LIB_SOURCES
)

Set(FOG_G2D_LIB_HEADERS
  Fog/G2d.h
)

# [Fog/G2d/Font]
Set(FOG_G2D_FONT_SOURCES
  Fog/G2d/Font/Font.cpp
  Fog/G2d/Font/FTFontEngine.cpp
  Fog/G2d/Font/Glyph.cpp
  Fog/G2d/Font/GlyphCache.cpp
  Fog/G2d/Font/GlyphSet.cpp
  Fog/G2d/Font/KerningPair.cpp
  Fog/G2d/Font/NullFontEngine.cpp
)

Set(FOG_G2D_FONT_HEADERS
  Fog/G2d/Font/Font.h
  Fog/G2d/Font/FTFontEngine.h
  Fog/G2d/Font/Glyph.h
  Fog/G2d/Font/GlyphCache.h
  Fog/G2d/Font/GlyphSet.h
  Fog/G2d/Font/KerningPair.h
  Fog/G2d/Font/NullFontEngine_p.h
)

If(FOG_OS_WINDOWS)
  Set(FOG_FONT_WINDOWS TRUE)
ElseIf(FOG_OS_MAC)
  Set(FOG_FONT_MAC TRUE)
Else()
  Find_File(HAVE_FREETYPE freetype-config)
  If(HAVE_FREETYPE)
    Exec_Program(freetype-config ARGS --cflags OUTPUT_VARIABLE FREETYPE_CONFIG_OUT RETURN_VALUE FREETYPE_CONFIG_RET)
    Set(FOG_FONT_FREETYPE TRUE)
  EndIf()

  If(NOT FOG_HAVE_FONTCONFIG)
    Check_Include_Files(fontconfig/fontconfig.h FOG_HAVE_FONTCONFIG)
  EndIf()
EndIf()

If(FOG_FONT_WINDOWS)
  List(APPEND FOG_G2D_FONT_SOURCES Fog/G2d/Font/WinFontEngine.cpp)
  List(APPEND FOG_G2D_FONT_HEADERS Fog/G2d/Font/WinFontEngine.h)
EndIf()

If(FOG_FONT_FREETYPE)
  Set_Source_Files_Properties(
    Fog/G2d/Font/FTFontEngine.cpp
    PROPERTIES COMPILE_FLAGS "${FREETYPE_CONFIG_OUT}"
  )
EndIf()

If(FOG_FONT_MAC)
  List(APPEND FOG_G2D_FONT_SOURCES Fog/G2d/Font/MacFontEngine.mm)
  List(APPEND FOG_G2D_FONT_HEADERS Fog/G2d/Font/MacFontEngine.h)
EndIf()

# [Fog/G2d/Geometry]
Set(FOG_G2D_GEOMETRY_SOURCES
  Fog/G2d/Geometry/Arc.cpp
  Fog/G2d/Geometry/Box.cpp
  Fog/G2d/Geometry/Chord.cpp
  Fog/G2d/Geometry/Circle.cpp
  Fog/G2d/Geometry/CubicCurve.cpp
  Fog/G2d/Geometry/Ellipse.cpp
  Fog/G2d/Geometry/Line.cpp
  Fog/G2d/Geometry/Math2d.cpp
  Fog/G2d/Geometry/Path.cpp
  Fog/G2d/Geometry/PathClipper.cpp
  Fog/G2d/Geometry/PathEffect.cpp
  Fog/G2d/Geometry/PathOnPath.cpp
  Fog/G2d/Geometry/PathStroker.cpp
  Fog/G2d/Geometry/Pie.cpp
  Fog/G2d/Geometry/Point.cpp
  Fog/G2d/Geometry/Polygon.cpp
  Fog/G2d/Geometry/QuadCurve.cpp
  Fog/G2d/Geometry/Rect.cpp
  Fog/G2d/Geometry/Round.cpp
  Fog/G2d/Geometry/Shape.cpp
  Fog/G2d/Geometry/Size.cpp
  Fog/G2d/Geometry/Transform.cpp
)

Set(FOG_G2D_GEOMETRY_HEADERS
  Fog/G2d/Geometry/Arc.h
  Fog/G2d/Geometry/Box.h
  Fog/G2d/Geometry/Chord.h
  Fog/G2d/Geometry/Circle.h
  Fog/G2d/Geometry/CubicCurve.h
  Fog/G2d/Geometry/Ellipse.h
  Fog/G2d/Geometry/Internals_p.h
  Fog/G2d/Geometry/Line.h
  Fog/G2d/Geometry/Math2d.h
  Fog/G2d/Geometry/Path.h
  Fog/G2d/Geometry/PathClipper.h
  Fog/G2d/Geometry/PathEffect.h
  Fog/G2d/Geometry/PathOnPath.h
  Fog/G2d/Geometry/PathStroker.h
  Fog/G2d/Geometry/Pie.h
  Fog/G2d/Geometry/Point.h
  Fog/G2d/Geometry/Polygon.h
  Fog/G2d/Geometry/QuadCurve.h
  Fog/G2d/Geometry/Rect.h
  Fog/G2d/Geometry/Round.h
  Fog/G2d/Geometry/Shape.h
  Fog/G2d/Geometry/Size.h
  Fog/G2d/Geometry/Transform.h
)

Set_Source_Files_Properties(
  Fog/G2d/Geometry/Transform.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}"
)

If(FOG_OPTIMIZE_3DNOW)
  List(APPEND FOG_G2D_GEOMETRY_SOURCES 
    Fog/G2d/Geometry/Transform_3dNow.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Geometry/Transform_3dNow.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_3DNOW}"
  )
EndIf()

If(FOG_OPTIMIZE_SSE)
  List(APPEND FOG_G2D_GEOMETRY_SOURCES
    Fog/G2d/Geometry/PathClipper_SSE.cpp
    Fog/G2d/Geometry/Transform_SSE.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Geometry/PathClipper_SSE.cpp
    Fog/G2d/Geometry/Transform_SSE.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE}"
  )
EndIf()

If(FOG_OPTIMIZE_SSE2)
  List(APPEND FOG_G2D_GEOMETRY_SOURCES
    Fog/G2d/Geometry/PathClipper_SSE2.cpp
    Fog/G2d/Geometry/Transform_SSE2.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Geometry/PathClipper_SSE2.cpp
    Fog/G2d/Geometry/Transform_SSE2.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE2}"
  )
EndIf()

# [Fog/G2d/Global]
Set(FOG_G2D_GLOBAL_SOURCES
  Fog/G2d/Global/Api.cpp
  Fog/G2d/Global/Constants.cpp
  Fog/G2d/Global/Init_G2d.cpp
)

Set(FOG_G2D_GLOBAL_HEADERS
  Fog/G2d/Global/Api.h
  Fog/G2d/Global/Constants.h
  Fog/G2d/Global/Init_G2d_p.h
)

# [Fog/G2d/Imaging]
Set(FOG_G2D_IMAGING_SOURCES
  Fog/G2d/Imaging/Image.cpp
  Fog/G2d/Imaging/ImageBits.cpp
  Fog/G2d/Imaging/ImageCodec.cpp
  Fog/G2d/Imaging/ImageCodecProvider.cpp
  Fog/G2d/Imaging/ImageConverter.cpp
  Fog/G2d/Imaging/ImageDecoder.cpp
  Fog/G2d/Imaging/ImageEffect.cpp
  Fog/G2d/Imaging/ImageEncoder.cpp
  Fog/G2d/Imaging/ImageFormatDescription.cpp
  Fog/G2d/Imaging/ImagePalette.cpp
)

Set(FOG_G2D_IMAGING_HEADERS
  Fog/G2d/Imaging/Image.h
  Fog/G2d/Imaging/ImageBits.h
  Fog/G2d/Imaging/ImageCodec.h
  Fog/G2d/Imaging/ImageCodecProvider.h
  Fog/G2d/Imaging/ImageConverter.h
  Fog/G2d/Imaging/ImageDecoder.h
  Fog/G2d/Imaging/ImageEffect.h
  Fog/G2d/Imaging/ImageEncoder.h
  Fog/G2d/Imaging/ImageFormatDescription.h
  Fog/G2d/Imaging/ImagePalette.h
)

Set(FOG_G2D_IMAGING_CODECS_SOURCES
  Fog/G2d/Imaging/Codecs/BmpCodec.cpp
  Fog/G2d/Imaging/Codecs/GdipCodec.cpp
  Fog/G2d/Imaging/Codecs/GifCodec.cpp
  Fog/G2d/Imaging/Codecs/IcoCodec.cpp
  Fog/G2d/Imaging/Codecs/JpegCodec.cpp
  Fog/G2d/Imaging/Codecs/PcxCodec.cpp
  Fog/G2d/Imaging/Codecs/PngCodec.cpp
)

Set(FOG_G2D_IMAGING_CODECS_HEADERS
  Fog/G2d/Imaging/Codecs/BmpCodec_p.h
  Fog/G2d/Imaging/Codecs/GdipCodec_p.h
  Fog/G2d/Imaging/Codecs/IcoCodec_p.h
  Fog/G2d/Imaging/Codecs/JpegCodec_p.h
  Fog/G2d/Imaging/Codecs/PcxCodec_p.h
  Fog/G2d/Imaging/Codecs/PngCodec_p.h
)

Set(FOG_G2D_IMAGING_EFFECTS_SOURCES
  Fog/G2d/Imaging/Effects/BlurEffect.cpp
  Fog/G2d/Imaging/Effects/BorderEffect.cpp
  Fog/G2d/Imaging/Effects/ColorMatrix.cpp
  Fog/G2d/Imaging/Effects/ComponentTransferEffect.cpp
  Fog/G2d/Imaging/Effects/ComponentTransferFunction.cpp
  Fog/G2d/Imaging/Effects/ConvolveEffect.cpp
  Fog/G2d/Imaging/Effects/EmbossEffect.cpp
  Fog/G2d/Imaging/Effects/ShadowEffect.cpp
  Fog/G2d/Imaging/Effects/SharpenEffect.cpp
)

Set(FOG_G2D_IMAGING_EFFECTS_HEADERS
  Fog/G2d/Imaging/Effects/BlurEffect.h
  Fog/G2d/Imaging/Effects/BorderEffect.h
  Fog/G2d/Imaging/Effects/ColorMatrix.h
  Fog/G2d/Imaging/Effects/ComponentTransferEffect.h
  Fog/G2d/Imaging/Effects/ComponentTransferFunction.h
  Fog/G2d/Imaging/Effects/ConvolveEffect.h
  Fog/G2d/Imaging/Effects/EmbossEffect.h
  Fog/G2d/Imaging/Effects/ShadowEffect.h
  Fog/G2d/Imaging/Effects/SharpenEffect.h
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_G2D_IMAGING_SOURCES Fog/G2d/Imaging/WinImage.cpp)
  List(APPEND FOG_G2D_IMAGING_HEADERS Fog/G2d/Imaging/WinImage_p.h)
EndIf()

If(NOT FOG_HAVE_LIBJPEG)
  Check_Include_Files(jconfig.h FOG_HAVE_LIBJPEG)
EndIf()

If(NOT FOG_HAVE_LIBPNG)
  Check_Include_Files(png.h FOG_HAVE_LIBPNG)
EndIf()

Set_Source_Files_Properties(
  Fog/G2d/Imaging/Image.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}"
)

# [Fog/G2d/Painting]
Set(FOG_G2D_PAINTING_SOURCES
  Fog/G2d/Painting/Painter.cpp
  Fog/G2d/Painting/PainterEngine.cpp
  Fog/G2d/Painting/PainterParams.cpp
  Fog/G2d/Painting/PainterUtil.cpp
)

Set(FOG_G2D_PAINTING_HEADERS
  Fog/G2d/Painting/Painter.h
  Fog/G2d/Painting/PainterEngine.h
  Fog/G2d/Painting/PainterParams.h
  Fog/G2d/Painting/PainterUtil.h
)

Set(FOG_G2D_PAINTING_NULL_SOURCES
  Fog/G2d/Painting/Null/NullEngine.cpp
)

Set(FOG_G2D_PAINTING_NULL_HEADERS
  Fog/G2d/Painting/Null/NullEngine_p.h
)

Set(FOG_G2D_PAINTING_RASTER_SOURCES
  Fog/G2d/Painting/Raster/RasterAction.cpp
  Fog/G2d/Painting/Raster/RasterContext.cpp
  Fog/G2d/Painting/Raster/RasterEngine.cpp
  Fog/G2d/Painting/Raster/RasterFuncs.cpp
  Fog/G2d/Painting/Raster/RasterRender.cpp
  Fog/G2d/Painting/Raster/RasterState.cpp
  Fog/G2d/Painting/Raster/RasterWorker.cpp
)

Set(FOG_G2D_PAINTING_RASTER_HEADERS
  Fog/G2d/Painting/Raster/RasterEngine_DoMask_p.h
  Fog/G2d/Painting/Raster/RasterEngine_DoPaint_p.h
  Fog/G2d/Painting/Raster/RasterAction_p.h
  Fog/G2d/Painting/Raster/RasterConstants_p.h
  Fog/G2d/Painting/Raster/RasterContext_p.h
  Fog/G2d/Painting/Raster/RasterEngine_p.h
  Fog/G2d/Painting/Raster/RasterFuncs_p.h
  Fog/G2d/Painting/Raster/RasterRender_p.h
  Fog/G2d/Painting/Raster/RasterState_p.h
  Fog/G2d/Painting/Raster/RasterStructs_p.h
  Fog/G2d/Painting/Raster/RasterUtil_p.h
  Fog/G2d/Painting/Raster/RasterWorker_p.h
)

# [Fog/G2d/Rasterizer]
Set(FOG_G2D_RASTERIZER_SOURCES
  Fog/G2d/Rasterizer/Rasterizer.cpp
  Fog/G2d/Rasterizer/Rasterizer_Line.cpp
  Fog/G2d/Rasterizer/Rasterizer_Path.cpp
  Fog/G2d/Rasterizer/Rasterizer_Rect.cpp
  Fog/G2d/Rasterizer/Rasterizer_RectXorRect.cpp
  Fog/G2d/Rasterizer/Rasterizer_Round.cpp
  Fog/G2d/Rasterizer/Rasterizer_RoundXorRound.cpp
  Fog/G2d/Rasterizer/Scanline.cpp
  Fog/G2d/Rasterizer/Span.cpp
)

Set(FOG_G2D_RASTERIZER_HEADERS
  Fog/G2d/Rasterizer/Rasterizer_p.h
  Fog/G2d/Rasterizer/Scanline_p.h
  Fog/G2d/Rasterizer/Span_p.h
)

Set_Source_Files_Properties(
  Fog/G2d/Rasterizer/Rasterizer.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}"
)

# [Fog/G2d/Render]
Set(FOG_G2D_RENDER_SOURCES
  Fog/G2d/Render/RenderApi.cpp
  Fog/G2d/Render/RenderConstants.cpp
  Fog/G2d/Render/RenderInit.cpp
  Fog/G2d/Render/RenderInit_C.cpp
  Fog/G2d/Render/RenderUtil.cpp
)

If(FOG_OPTIMIZE_MMX)
  List(APPEND FOG_G2D_RENDER_SOURCES 
    Fog/G2d/Render/RenderInit_MMX.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Render/RenderInit_MMX.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_MMX}"
  )
EndIf(FOG_OPTIMIZE_MMX)

If(FOG_OPTIMIZE_3DNOW)
  List(APPEND FOG_G2D_RENDER_SOURCES 
    Fog/G2d/Render/RenderInit_MMX3dNow.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Render/RenderInit_MMX3dNow.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_3DNOW}"
  )
EndIf(FOG_OPTIMIZE_3DNOW)

If(FOG_OPTIMIZE_SSE)
  List(APPEND FOG_G2D_RENDER_SOURCES
    Fog/G2d/Render/RenderInit_MMXSSE.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Render/RenderInit_MMXSSE.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE}"
  )
EndIf(FOG_OPTIMIZE_SSE)

If(FOG_OPTIMIZE_SSE2)
  List(APPEND FOG_G2D_RENDER_SOURCES
    Fog/G2d/Render/RenderInit_SSE2.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Render/RenderInit_SSE2.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE2}"
  )
EndIf(FOG_OPTIMIZE_SSE2)

If(FOG_OPTIMIZE_SSSE3)
  List(APPEND FOG_G2D_RENDER_SOURCES
    Fog/G2d/Render/RenderInit_SSSE3.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Render/RenderInit_SSSE3.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSSE3}"
  )
EndIf(FOG_OPTIMIZE_SSSE3)

Set(FOG_G2D_RENDER_HEADERS
  Fog/G2d/Render/RenderApi_p.h
  Fog/G2d/Render/RenderConstants_p.h
  Fog/G2d/Render/RenderFuncs_p.h
  Fog/G2d/Render/RenderInit_p.h
  Fog/G2d/Render/RenderStructs_p.h
  Fog/G2d/Render/RenderUtil_p.h
)

Set(FOG_G2D_RENDER_C_HEADERS
  Fog/G2d/Render/Render_C/Access_p.h
  Fog/G2d/Render/Render_C/CBase_p.h
  Fog/G2d/Render/Render_C/CClear_p.h
  Fog/G2d/Render/Render_C/CConvert_p.h
  Fog/G2d/Render/Render_C/CNop_p.h
  Fog/G2d/Render/Render_C/CSrc_p.h
  Fog/G2d/Render/Render_C/CSrcOver_p.h
  Fog/G2d/Render/Render_C/Defs_p.h
  Fog/G2d/Render/Render_C/Helpers_p.h
  Fog/G2d/Render/Render_C/PGradientBase_p.h
  Fog/G2d/Render/Render_C/PGradientConical_p.h
  Fog/G2d/Render/Render_C/PGradientLinear_p.h
  Fog/G2d/Render/Render_C/PGradientRadial_p.h
  Fog/G2d/Render/Render_C/PGradientRectangular_p.h
  Fog/G2d/Render/Render_C/PTextureAffine_p.h
  Fog/G2d/Render/Render_C/PTextureBase_p.h
  Fog/G2d/Render/Render_C/PTextureProjection_p.h
  Fog/G2d/Render/Render_C/PTextureScale_p.h
  Fog/G2d/Render/Render_C/PTextureSimple_p.h
)

Set(FOG_G2D_RENDER_MMX_HEADERS
)

Set(FOG_G2D_RENDER_SSE2_HEADERS
)

# [Fog/G2d/Shader]
Set(FOG_G2D_SHADER_SOURCES
  Fog/G2d/Shader/Shader.cpp
)

Set(FOG_G2D_SHADER_HEADERS
  Fog/G2d/Shader/Shader.h
)

# [Fog/G2d/Source]
Set(FOG_G2D_SOURCE_SOURCES
  Fog/G2d/Source/Color.cpp
  Fog/G2d/Source/ColorBase.cpp
  Fog/G2d/Source/ColorStop.cpp
  Fog/G2d/Source/ColorStopCache.cpp
  Fog/G2d/Source/ColorStopList.cpp
  Fog/G2d/Source/ColorUtil.cpp
  Fog/G2d/Source/ConicalGradient.cpp
  Fog/G2d/Source/Gradient.cpp
  Fog/G2d/Source/LinearGradient.cpp
  Fog/G2d/Source/Pattern.cpp
  Fog/G2d/Source/RadialGradient.cpp
  Fog/G2d/Source/RectangularGradient.cpp
  Fog/G2d/Source/Texture.cpp
)

If(FOG_OPTIMIZE_SSE2)
  List(APPEND FOG_G2D_SOURCE_SOURCES
    Fog/G2d/Source/Color_SSE2.cpp
  )
  Set_Source_Files_Properties(
    Fog/G2d/Source/Color_SSE2.cpp
    PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_SSE2}"
  )
EndIf(FOG_OPTIMIZE_SSE2)

Set(FOG_G2D_SOURCE_HEADERS
  Fog/G2d/Source/Acmyk.h
  Fog/G2d/Source/Ahsl.h
  Fog/G2d/Source/Ahsv.h
  Fog/G2d/Source/Argb.h
  Fog/G2d/Source/Color.h
  Fog/G2d/Source/ColorBase.h
  Fog/G2d/Source/ColorStop.h
  Fog/G2d/Source/ColorStopCache.h
  Fog/G2d/Source/ColorStopList.h
  Fog/G2d/Source/ColorUtil.h
  Fog/G2d/Source/ConicalGradient.h
  Fog/G2d/Source/Gradient.h
  Fog/G2d/Source/LinearGradient.h
  Fog/G2d/Source/Pattern.h
  Fog/G2d/Source/RadialGradient.h
  Fog/G2d/Source/RectangularGradient.h
  Fog/G2d/Source/Texture.h
)

# [Fog/G2d/Text]
Set(FOG_G2D_TEXT_SOURCES
  Fog/G2d/Text/TextIterator.cpp
  Fog/G2d/Text/TextLayout.cpp
  Fog/G2d/Text/TextRect.cpp
)

Set(FOG_G2D_TEXT_HEADERS
  Fog/G2d/Text/TextIterator.h
  Fog/G2d/Text/TextLayout.h
  Fog/G2d/Text/TextRect.h
)

# [Fog/G2d/Tools]
Set(FOG_G2D_TOOLS_SOURCES
  Fog/G2d/Tools/ColorAnalyzer.cpp
  Fog/G2d/Tools/DitherTable.cpp
  Fog/G2d/Tools/Dpi.cpp
  Fog/G2d/Tools/Matrix.cpp
  Fog/G2d/Tools/Reduce.cpp
  Fog/G2d/Tools/Region.cpp
)

Set(FOG_G2D_TOOLS_HEADERS
  Fog/G2d/Tools/ColorAnalyzer_p.h
  Fog/G2d/Tools/DitherTable_p.h
  Fog/G2d/Tools/Dpi.h
  Fog/G2d/Tools/Matrix.h
  Fog/G2d/Tools/Reduce_p.h
  Fog/G2d/Tools/Region.h
)

Set_Source_Files_Properties(
  Fog/G2d/Tools/Region.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CFLAGS_OPTIMIZE}"
)

# [Fog/G2d/Mac]
If(FOG_OS_MAC)
  Set(FOG_G2D_MAC_SOURCES 
    Fog/G2d/Mac/MacUtil.mm
  )
  Set(FOG_G2D_MAC_HEADERS
    Fog/G2d/Mac/MacUtil.h
  )
EndIf()

# Source groups.
Source_Group("G2d"                  FILES ${FOG_G2D_LIB_SOURCES}              ${FOG_G2D_LIB_HEADERS}             )
Source_Group("G2d/Font"             FILES ${FOG_G2D_FONT_SOURCES}             ${FOG_G2D_FONT_HEADERS}            )
Source_Group("G2d/Geometry"         FILES ${FOG_G2D_GEOMETRY_SOURCES}         ${FOG_G2D_GEOMETRY_HEADERS}        )
Source_Group("G2d/Global"           FILES ${FOG_G2D_GLOBAL_SOURCES}           ${FOG_G2D_GLOBAL_HEADERS}          )
Source_Group("G2d/Imaging"          FILES ${FOG_G2D_IMAGING_SOURCES}          ${FOG_G2D_IMAGING_HEADERS}         )
Source_Group("G2d/Imaging/Codecs"   FILES ${FOG_G2D_IMAGING_CODECS_SOURCES}   ${FOG_G2D_IMAGING_CODECS_HEADERS}  )
Source_Group("G2d/Imaging/Effects"  FILES ${FOG_G2D_IMAGING_EFFECTS_SOURCES}  ${FOG_G2D_IMAGING_EFFECTS_HEADERS} )
Source_Group("G2d/Painting"         FILES ${FOG_G2D_PAINTING_SOURCES}         ${FOG_G2D_PAINTING_HEADERS}        )
Source_Group("G2d/Painting/Null"    FILES ${FOG_G2D_PAINTING_NULL_SOURCES}    ${FOG_G2D_PAINTING_NULL_HEADERS}   )
Source_Group("G2d/Painting/Raster"  FILES ${FOG_G2D_PAINTING_RASTER_SOURCES}  ${FOG_G2D_PAINTING_RASTER_HEADERS} )
Source_Group("G2d/Rasterizer"       FILES ${FOG_G2D_RASTERIZER_SOURCES}       ${FOG_G2D_RASTERIZER_HEADERS}      )
Source_Group("G2d/Render"           FILES ${FOG_G2D_RENDER_SOURCES}           ${FOG_G2D_RENDER_HEADERS}          )
Source_Group("G2d/Render/C"         FILES                                     ${FOG_G2D_RENDER_C_HEADERS}        )
Source_Group("G2d/Render/MMX"       FILES                                     ${FOG_G2D_RENDER_MMX_HEADERS}      )
Source_Group("G2d/Render/SSE2"      FILES                                     ${FOG_G2D_RENDER_SSE2_HEADERS}     )
Source_Group("G2d/Shader"           FILES ${FOG_G2D_SHADER_SOURCES}           ${FOG_G2D_SHADER_HEADERS}          )
Source_Group("G2d/Source"           FILES ${FOG_G2D_SOURCE_SOURCES}           ${FOG_G2D_SOURCE_HEADERS}          )
Source_Group("G2d/Text"             FILES ${FOG_G2D_TEXT_SOURCES}             ${FOG_G2D_TEXT_HEADERS}            )
Source_Group("G2d/Tools"            FILES ${FOG_G2D_TOOLS_SOURCES}            ${FOG_G2D_TOOLS_HEADERS}           )

If(FOG_OS_MAC)
Source_Group("G2d/Mac"              FILES ${FOG_G2D_MAC_SOURCES}              ${FOG_G2D_MAC_HEADERS}             )
EndIf()

# =============================================================================
# [Fog-Gui]
# =============================================================================

# Gui backends.
If(FOG_OS_WINDOWS)
  Set(FOG_GUI_WINDOWS 1)
ElseIf(FOG_OS_MAC)
  Set(FOG_GUI_MAC 1)
Else()
  Set(FOG_GUI_X11 1)
EndIf()

# [Fog/Gui]
Set(FOG_GUI_LIB_SOURCES
)

Set(FOG_GUI_LIB_HEADERS
  Fog/Gui.h
)

# [Fog/Gui/Animation]
Set(FOG_GUI_ANIMATION_SOURCES
  Fog/Gui/Animation/Animation.cpp
)

Set(FOG_GUI_ANIMATION_HEADERS
  Fog/Gui/Animation/Animation.h
)

# [Fog/Gui/Canvas]
Set(FOG_GUI_CANVAS_SOURCES
  Fog/Gui/Canvas/CanvasDelegate.cpp
  Fog/Gui/Canvas/CanvasElement.cpp
  Fog/Gui/Canvas/CanvasView.cpp
)

Set(FOG_GUI_CANVAS_HEADERS
  Fog/Gui/Canvas/CanvasDelegate.h
  Fog/Gui/Canvas/CanvasElement.h
  Fog/Gui/Canvas/CanvasView.h
)

# [Fog/Gui/Global]
Set(FOG_GUI_GLOBAL_SOURCES
  Fog/Gui/Global/Init_UI.cpp
)

Set(FOG_GUI_GLOBAL_HEADERS
  Fog/Gui/Global/Constants.h
  Fog/Gui/Global/Init_UI_p.h
)

# [Fog/Gui/Engine]
Set(FOG_GUI_ENGINE_SOURCES
  Fog/Gui/Engine/GuiEngine.cpp
)

Set(FOG_GUI_ENGINE_HEADERS
  Fog/Gui/Engine/GuiEngine.h
)

Set(FOG_GUI_ENGINE_MAC_SOURCES 
  Fog/Gui/Engine/MacGuiEngine.mm
)

Set(FOG_GUI_ENGINE_MAC_HEADERS 
  Fog/Gui/Engine/MacGuiEngine.h
)

Set(FOG_GUI_ENGINE_WIN_SOURCES
  Fog/Gui/Engine/WinGuiEngine.cpp
)

Set(FOG_GUI_ENGINE_WIN_HEADERS
  Fog/Gui/Engine/WinGuiEngine.h
)

Set(FOG_GUI_ENGINE_X11_SOURCES
  Fog/Gui/Engine/X11GuiEngine.cpp
)

Set(FOG_GUI_ENGINE_X11_HEADERS
  Fog/Gui/Engine/X11GuiEngine.h
  Fog/Gui/Engine/X11Headers.h
)

# [Fog/Gui/Layout]
Set(FOG_GUI_LAYOUT_SOURCES
  Fog/Gui/Layout/BorderLayout.cpp
  Fog/Gui/Layout/BoxLayout.cpp
  Fog/Gui/Layout/FlowLayout.cpp
  Fog/Gui/Layout/GridLayout.cpp
  Fog/Gui/Layout/Layout.cpp
  Fog/Gui/Layout/LayoutHint.cpp
  Fog/Gui/Layout/LayoutItem.cpp
  Fog/Gui/Layout/LayoutUtil.cpp
  Fog/Gui/Layout/Margin.cpp
)

Set(FOG_GUI_LAYOUT_HEADERS
  Fog/Gui/Layout/BorderLayout.h
  Fog/Gui/Layout/BoxLayout.h
  Fog/Gui/Layout/FlowLayout.h
  Fog/Gui/Layout/GridLayout.h
  Fog/Gui/Layout/Layout.h
  Fog/Gui/Layout/LayoutHint.h
  Fog/Gui/Layout/LayoutItem.h
  Fog/Gui/Layout/LayoutPolicy.h
  Fog/Gui/Layout/LayoutUtil.h
  Fog/Gui/Layout/Margin.h
)

# [Fog/Gui/Theming]
Set(FOG_GUI_THEMING_SOURCES
  Fog/Gui/Theming/ThemeEngine.cpp
  Fog/Gui/Theming/DefaultThemeEngine.cpp
  Fog/Gui/Theming/WinThemeEngine.cpp
)

Set(FOG_GUI_THEMING_HEADERS
  Fog/Gui/Theming/ThemeEngine.h
  Fog/Gui/Theming/DefaultThemeEngine_p.h
  Fog/Gui/Theming/WinThemeEngine_p.h
)

# [Fog/Gui/Widget]
Set(FOG_GUI_WIDGET_SOURCES
  Fog/Gui/Widget/Button.cpp
  Fog/Gui/Widget/ButtonBase.cpp
  Fog/Gui/Widget/CheckBox.cpp
  Fog/Gui/Widget/Composite.cpp
  Fog/Gui/Widget/Event.cpp
  Fog/Gui/Widget/Frame.cpp
  Fog/Gui/Widget/GroupBox.cpp
  Fog/Gui/Widget/MenuBar.cpp
  Fog/Gui/Widget/MenuItem.cpp
  Fog/Gui/Widget/MenuSeparator.cpp
  Fog/Gui/Widget/RadioButton.cpp
  Fog/Gui/Widget/ScrollArea.cpp
  Fog/Gui/Widget/ScrollBar.cpp
  Fog/Gui/Widget/TextArea.cpp
  Fog/Gui/Widget/TextField.cpp
  Fog/Gui/Widget/ToolBar.cpp
  Fog/Gui/Widget/ToolButton.cpp
  Fog/Gui/Widget/Widget.cpp
  Fog/Gui/Widget/Window.cpp
)

Set(FOG_GUI_WIDGET_HEADERS
  Fog/Gui/Widget/Button.h
  Fog/Gui/Widget/ButtonBase.h
  Fog/Gui/Widget/CheckBox.h
  Fog/Gui/Widget/Composite.h
  Fog/Gui/Widget/Event.h
  Fog/Gui/Widget/Frame.h
  Fog/Gui/Widget/GroupBox.h
  Fog/Gui/Widget/MenuBar.h
  Fog/Gui/Widget/MenuItem.h
  Fog/Gui/Widget/MenuSeparator.h
  Fog/Gui/Widget/RadioButton.h
  Fog/Gui/Widget/ScrollArea.h
  Fog/Gui/Widget/ScrollBar.h
  Fog/Gui/Widget/TextArea.h
  Fog/Gui/Widget/TextField.h
  Fog/Gui/Widget/ToolBar.h
  Fog/Gui/Widget/ToolButton.h
  Fog/Gui/Widget/Widget.h
  Fog/Gui/Widget/Widget_p.h
  Fog/Gui/Widget/Window.h
)

# Source groups.
Source_Group("Gui"                  FILES ${FOG_GUI_LIB_SOURCES}              ${FOG_GUI_LIB_HEADERS}             )
Source_Group("Gui/Animation"        FILES ${FOG_GUI_ANIMATION_SOURCES}        ${FOG_GUI_ANIMATION_HEADERS}       )
Source_Group("Gui/Canvas"           FILES ${FOG_GUI_CANVAS_SOURCES}           ${FOG_GUI_CANVAS_HEADERS}          )
Source_Group("Gui/Engine"           FILES ${FOG_GUI_ENGINE_SOURCES}           ${FOG_GUI_ENGINE_HEADERS}
                                          ${FOG_GUI_ENGINE_MAC_SOURCES}       ${FOG_GUI_ENGINE_MAC_SOURCES}
                                          ${FOG_GUI_ENGINE_X11_SOURCES}       ${FOG_GUI_ENGINE_X11_HEADERS}
                                          ${FOG_GUI_ENGINE_WIN_SOURCES}       ${FOG_GUI_ENGINE_WIN_HEADERS}      )
Source_Group("Gui/Global"           FILES ${FOG_GUI_GLOBAL_SOURCES}           ${FOG_GUI_GLOBAL_HEADERS}          )
Source_Group("Gui/Layout"           FILES ${FOG_GUI_LAYOUT_SOURCES}           ${FOG_GUI_LAYOUT_HEADERS}          )
Source_Group("Gui/Theming"          FILES ${FOG_GUI_THEMING_SOURCES}          ${FOG_GUI_THEMING_HEADERS}         )
Source_Group("Gui/Widget"           FILES ${FOG_GUI_WIDGET_SOURCES}           ${FOG_GUI_WIDGET_HEADERS}          )

# =============================================================================
# [Fog-Svg]
# =============================================================================

Set(FOG_SVG_LIB_SOURCES
)

Set(FOG_SVG_LIB_HEADERS
  Fog/Svg.h
)

Set(FOG_SVG_DOM_SOURCES
  Fog/Svg/Dom/SvgObject.cpp

  Fog/Svg/Dom/SvgCoordAttribute.cpp
  Fog/Svg/Dom/SvgEnumAttribute.cpp
  Fog/Svg/Dom/SvgEnumItem.cpp
  Fog/Svg/Dom/SvgImageLinkAttribute.cpp
  Fog/Svg/Dom/SvgOffsetAttribute.cpp
  Fog/Svg/Dom/SvgPathAttribute.cpp
  Fog/Svg/Dom/SvgPointsAttribute.cpp
  Fog/Svg/Dom/SvgStyleAttribute.cpp
  Fog/Svg/Dom/SvgTransformAttribute.cpp

  Fog/Svg/Dom/SvgAbstractGradientElement.cpp
  Fog/Svg/Dom/SvgAElement.cpp
  Fog/Svg/Dom/SvgCircleElement.cpp
  Fog/Svg/Dom/SvgClipPathElement.cpp
  Fog/Svg/Dom/SvgDefsElement.cpp
  Fog/Svg/Dom/SvgDescElement.cpp
  Fog/Svg/Dom/SvgDocument.cpp
  Fog/Svg/Dom/SvgElement.cpp
  Fog/Svg/Dom/SvgEllipseElement.cpp
  Fog/Svg/Dom/SvgGElement.cpp
  Fog/Svg/Dom/SvgImageElement.cpp
  Fog/Svg/Dom/SvgLinearGradientElement.cpp
  Fog/Svg/Dom/SvgLineElement.cpp
  Fog/Svg/Dom/SvgMarkerElement.cpp
  Fog/Svg/Dom/SvgMaskElement.cpp
  Fog/Svg/Dom/SvgPathElement.cpp
  Fog/Svg/Dom/SvgPatternElement.cpp
  Fog/Svg/Dom/SvgPolygonElement.cpp
  Fog/Svg/Dom/SvgPolylineElement.cpp
  Fog/Svg/Dom/SvgRadialGradientElement.cpp
  Fog/Svg/Dom/SvgRectElement.cpp
  Fog/Svg/Dom/SvgSolidColorElement.cpp
  Fog/Svg/Dom/SvgStopElement.cpp
  Fog/Svg/Dom/SvgStyledElement.cpp
  Fog/Svg/Dom/SvgSymbolElement.cpp
  Fog/Svg/Dom/SvgTextElement.cpp
  Fog/Svg/Dom/SvgTextPathElement.cpp
  Fog/Svg/Dom/SvgTitleElement.cpp
  Fog/Svg/Dom/SvgTRefElement.cpp
  Fog/Svg/Dom/SvgTSpanElement.cpp
  Fog/Svg/Dom/SvgUseElement.cpp
  Fog/Svg/Dom/SvgViewElement.cpp
)

Set(FOG_SVG_DOM_HEADERS
  Fog/Svg/Dom/SvgObject.h

  Fog/Svg/Dom/SvgCoordAttribute_p.h
  Fog/Svg/Dom/SvgEnumAttribute_p.h
  Fog/Svg/Dom/SvgEnumItem_p.h
  Fog/Svg/Dom/SvgImageLinkAttribute_p.h
  Fog/Svg/Dom/SvgOffsetAttribute_p.h
  Fog/Svg/Dom/SvgPathAttribute_p.h
  Fog/Svg/Dom/SvgPointsAttribute_p.h
  Fog/Svg/Dom/SvgStyleAttribute_p.h
  Fog/Svg/Dom/SvgTransformAttribute_p.h

  Fog/Svg/Dom/SvgAbstractGradientElement_p.h
  Fog/Svg/Dom/SvgAElement_p.h
  Fog/Svg/Dom/SvgCircleElement_p.h
  Fog/Svg/Dom/SvgClipPathElement_p.h
  Fog/Svg/Dom/SvgDefsElement_p.h
  Fog/Svg/Dom/SvgDescElement_p.h
  Fog/Svg/Dom/SvgDocument.h
  Fog/Svg/Dom/SvgElement.h
  Fog/Svg/Dom/SvgEllipseElement_p.h
  Fog/Svg/Dom/SvgGElement_p.h
  Fog/Svg/Dom/SvgImageElement_p.h
  Fog/Svg/Dom/SvgLinearGradientElement_p.h
  Fog/Svg/Dom/SvgLineElement_p.h
  Fog/Svg/Dom/SvgMarkerElement_p.h
  Fog/Svg/Dom/SvgMaskElement_p.h
  Fog/Svg/Dom/SvgPathElement_p.h
  Fog/Svg/Dom/SvgPatternElement_p.h
  Fog/Svg/Dom/SvgPolygonElement_p.h
  Fog/Svg/Dom/SvgPolylineElement_p.h
  Fog/Svg/Dom/SvgRadialGradientElement_p.h
  Fog/Svg/Dom/SvgRectElement_p.h
  Fog/Svg/Dom/SvgSolidColorElement_p.h
  Fog/Svg/Dom/SvgStopElement_p.h
  Fog/Svg/Dom/SvgStyledElement_p.h
  Fog/Svg/Dom/SvgSymbolElement_p.h
  Fog/Svg/Dom/SvgTextElement_p.h
  Fog/Svg/Dom/SvgTextPathElement_p.h
  Fog/Svg/Dom/SvgTitleElement_p.h
  Fog/Svg/Dom/SvgTRefElement_p.h
  Fog/Svg/Dom/SvgTSpanElement_p.h
  Fog/Svg/Dom/SvgUseElement_p.h
  Fog/Svg/Dom/SvgViewElement_p.h
)

Set(FOG_SVG_GLOBAL_SOURCES
  Fog/Svg/Global/Init_Svg.cpp
)

Set(FOG_SVG_GLOBAL_HEADERS
  Fog/Svg/Global/Constants.h
  Fog/Svg/Global/Init_Svg_p.h
)

Set(FOG_SVG_RENDER_SOURCES
  Fog/Svg/Render/SvgRender.cpp
)

Set(FOG_SVG_RENDER_HEADERS
  Fog/Svg/Render/SvgRender.h
)

Set(FOG_SVG_TOOLS_SOURCES
  Fog/Svg/Tools/SvgCoord.cpp
  Fog/Svg/Tools/SvgUtil.cpp
)

Set(FOG_SVG_TOOLS_HEADERS
  Fog/Svg/Tools/SvgCoord.h
  Fog/Svg/Tools/SvgUtil.h
)

# Source groups.
Source_Group("Svg"                  FILES ${FOG_SVG_LIB_SOURCES}              ${FOG_SVG_LIB_HEADERS}             )
Source_Group("Svg/Dom"              FILES ${FOG_SVG_DOM_SOURCES}              ${FOG_SVG_DOM_HEADERS}             )
Source_Group("Svg/Global"           FILES ${FOG_SVG_GLOBAL_SOURCES}           ${FOG_SVG_GLOBAL_HEADERS}          )
Source_Group("Svg/Render"           FILES ${FOG_SVG_RENDER_SOURCES}           ${FOG_SVG_RENDER_HEADERS}          )
Source_Group("Svg/Tools"            FILES ${FOG_SVG_TOOLS_SOURCES}            ${FOG_SVG_TOOLS_HEADERS}           )

# =============================================================================
# [Fog-Xml]
# =============================================================================

Set(FOG_XML_LIB_SOURCES
)

Set(FOG_XML_LIB_HEADERS
  Fog/Xml.h
)

Set(FOG_XML_DOM_SOURCES
  Fog/Xml/Dom/XmlAttribute.cpp
  Fog/Xml/Dom/XmlCDATA.cpp
  Fog/Xml/Dom/XmlComment.cpp
  Fog/Xml/Dom/XmlDocument.cpp
  Fog/Xml/Dom/XmlElement.cpp
  Fog/Xml/Dom/XmlIdAttribute.cpp
  Fog/Xml/Dom/XmlIdManager.cpp
  Fog/Xml/Dom/XmlNoText.cpp
  Fog/Xml/Dom/XmlPI.cpp
  Fog/Xml/Dom/XmlText.cpp
)

Set(FOG_XML_DOM_HEADERS
  Fog/Xml/Dom/XmlAttribute.h
  Fog/Xml/Dom/XmlCDATA.h
  Fog/Xml/Dom/XmlComment.h
  Fog/Xml/Dom/XmlDocument.h
  Fog/Xml/Dom/XmlElement.h
  Fog/Xml/Dom/XmlIdAttribute_p.h
  Fog/Xml/Dom/XmlIdManager.h
  Fog/Xml/Dom/XmlNoText.h
  Fog/Xml/Dom/XmlPI.h
  Fog/Xml/Dom/XmlText.h
)

Set(FOG_XML_GLOBAL_SOURCES
  Fog/Xml/Global/Init_Xml.cpp
)

Set(FOG_XML_GLOBAL_HEADERS
  Fog/Xml/Global/Constants.h
  Fog/Xml/Global/Init_Xml_p.h
)

Set(FOG_XML_IO_SOURCES
  Fog/Xml/IO/XmlDomReader.cpp
  Fog/Xml/IO/XmlDomWriter.cpp
  Fog/Xml/IO/XmlSaxReader.cpp
  Fog/Xml/IO/XmlSaxWriter.cpp
)

Set(FOG_XML_IO_HEADERS
  Fog/Xml/IO/XmlDomReader.h
  Fog/Xml/IO/XmlDomWriter.h
  Fog/Xml/IO/XmlSaxReader.h
  Fog/Xml/IO/XmlSaxWriter.h
)

Set(FOG_XML_TOOLS_SOURCES
  Fog/Xml/Tools/XmlEntity.cpp
)

Set(FOG_XML_TOOLS_HEADERS
  Fog/Xml/Tools/XmlEntity_p.h
)

# Source groups.
Source_Group("Xml"                  FILES ${FOG_XML_LIB_SOURCES}              ${FOG_XML_LIB_HEADERS}             )
Source_Group("Xml/Dom"              FILES ${FOG_XML_DOM_SOURCES}              ${FOG_XML_DOM_HEADERS}             )
Source_Group("Xml/Global"           FILES ${FOG_XML_GLOBAL_SOURCES}           ${FOG_XML_GLOBAL_HEADERS}          )
Source_Group("Xml/IO"               FILES ${FOG_XML_IO_SOURCES}               ${FOG_XML_IO_HEADERS}              )
Source_Group("Xml/Tools"            FILES ${FOG_XML_TOOLS_SOURCES}            ${FOG_XML_TOOLS_HEADERS}           )

# =============================================================================
# [Fog-All]
# =============================================================================

Set(FOG_ALL_SOURCES
  # Fog/Core.
  ${FOG_CORE_LIB_SOURCES}
  ${FOG_CORE_COLLECTION_SOURCES}
  ${FOG_CORE_CONFIG_SOURCES}
  ${FOG_CORE_CPU_SOURCES}
  ${FOG_CORE_DATA_SOURCES}
  ${FOG_CORE_DATETIME_SOURCES}
  ${FOG_CORE_FACE_SOURCES}
  ${FOG_CORE_GLOBAL_SOURCES}
  ${FOG_CORE_IO_SOURCES}
  ${FOG_CORE_LIBRARY_SOURCES}
  ${FOG_CORE_MATH_SOURCES}
  ${FOG_CORE_MEMORY_SOURCES}
  ${FOG_CORE_OS_SOURCES}
  ${FOG_CORE_PACK_SOURCES}
  ${FOG_CORE_SYSTEM_SOURCES}
  ${FOG_CORE_THREADING_SOURCES}
  ${FOG_CORE_TOOLS_SOURCES}
  # Fog/G2d.
  ${FOG_G2D_LIB_SOURCES}
  ${FOG_G2D_FONT_SOURCES}
  ${FOG_G2D_GEOMETRY_SOURCES}
  ${FOG_G2D_GLOBAL_SOURCES}
  ${FOG_G2D_IMAGING_SOURCES}
  ${FOG_G2D_IMAGING_CODECS_SOURCES}
  ${FOG_G2D_IMAGING_EFFECTS_SOURCES}
  ${FOG_G2D_PAINTING_SOURCES}
  ${FOG_G2D_PAINTING_NULL_SOURCES}
  ${FOG_G2D_PAINTING_RASTER_SOURCES}
  ${FOG_G2D_RASTERIZER_SOURCES}
  ${FOG_G2D_RENDER_SOURCES}
  ${FOG_G2D_SHADER_SOURCES}
  ${FOG_G2D_SOURCE_SOURCES}
  ${FOG_G2D_TEXT_SOURCES}
  ${FOG_G2D_TOOLS_SOURCES}
  # Fog/Gui.
  ${FOG_GUI_LIB_SOURCES}
  ${FOG_GUI_ANIMATION_SOURCES}
  ${FOG_GUI_CANVAS_SOURCES}
  ${FOG_GUI_ENGINE_SOURCES}
  ${FOG_GUI_GLOBAL_SOURCES}
  ${FOG_GUI_LAYOUT_SOURCES}
  ${FOG_GUI_THEMING_SOURCES}
  ${FOG_GUI_WIDGET_SOURCES}
  # Fog/Svg.
  ${FOG_SVG_LIB_SOURCES}
  ${FOG_SVG_DOM_SOURCES}
  ${FOG_SVG_GLOBAL_SOURCES}
  ${FOG_SVG_RENDER_SOURCES}
  ${FOG_SVG_TOOLS_SOURCES}
  # Fog/Xml.
  ${FOG_XML_LIB_SOURCES}
  ${FOG_XML_DOM_SOURCES}
  ${FOG_XML_GLOBAL_SOURCES}
  ${FOG_XML_IO_SOURCES}
  ${FOG_XML_TOOLS_SOURCES}
)

Set(FOG_ALL_HEADERS
  # Fog/Core.
  ${FOG_CORE_LIB_HEADERS}
  ${FOG_CORE_COLLECTION_HEADERS}
  ${FOG_CORE_CONFIG_HEADERS}
  ${FOG_CORE_CPU_HEADERS}
  ${FOG_CORE_DATA_HEADERS}
  ${FOG_CORE_DATETIME_HEADERS}
  ${FOG_CORE_FACE_HEADERS}
  ${FOG_CORE_GLOBAL_HEADERS}
  ${FOG_CORE_IO_HEADERS}
  ${FOG_CORE_LIBRARY_HEADERS}
  ${FOG_CORE_MATH_HEADERS}
  ${FOG_CORE_MEMORY_HEADERS}
  ${FOG_CORE_OS_HEADERS}
  ${FOG_CORE_PACK_HEADERS}
  ${FOG_CORE_SYSTEM_HEADERS}
  ${FOG_CORE_THREADING_HEADERS}
  ${FOG_CORE_TOOLS_HEADERS}
  # Fog/G2d.
  ${FOG_G2D_LIB_HEADERS}
  ${FOG_G2D_FONT_HEADERS}
  ${FOG_G2D_GEOMETRY_HEADERS}
  ${FOG_G2D_GLOBAL_HEADERS}
  ${FOG_G2D_IMAGING_HEADERS}
  ${FOG_G2D_IMAGING_CODECS_HEADERS}
  ${FOG_G2D_IMAGING_EFFECTS_HEADERS}
  ${FOG_G2D_PAINTING_HEADERS}
  ${FOG_G2D_PAINTING_NULL_HEADERS}
  ${FOG_G2D_PAINTING_RASTER_HEADERS}
  ${FOG_G2D_GEOMETRY_HEADERS}
  ${FOG_G2D_RASTERIZER_HEADERS}
  ${FOG_G2D_RENDER_HEADERS}
  ${FOG_G2D_RENDER_C_HEADERS}
  ${FOG_G2D_RENDER_MMX_HEADERS}
  ${FOG_G2D_RENDER_SSE2_HEADERS}
  ${FOG_G2D_SHADER_HEADERS}
  ${FOG_G2D_SOURCE_HEADERS}
  ${FOG_G2D_TEXT_HEADERS}
  ${FOG_G2D_TOOLS_HEADERS}
  # Fog/Gui.
  ${FOG_GUI_LIB_HEADERS}
  ${FOG_GUI_ANIMATION_HEADERS}
  ${FOG_GUI_CANVAS_HEADERS}
  ${FOG_GUI_ENGINE_HEADERS}
  ${FOG_GUI_ENGINE_X11_HEADERS}
  ${FOG_GUI_ENGINE_WIN_HEADERS}
  ${FOG_GUI_GLOBAL_HEADERS}
  ${FOG_GUI_LAYOUT_HEADERS}
  ${FOG_GUI_THEMING_HEADERS}
  ${FOG_GUI_WIDGET_HEADERS}
  # Fog/Svg.
  ${FOG_SVG_LIB_HEADERS}
  ${FOG_SVG_DOM_HEADERS}
  ${FOG_SVG_GLOBAL_HEADERS}
  ${FOG_SVG_RENDER_HEADERS}
  ${FOG_SVG_TOOLS_HEADERS}
  # Fog/Xml.
  ${FOG_XML_LIB_HEADERS}
  ${FOG_XML_DOM_HEADERS}
  ${FOG_XML_GLOBAL_HEADERS}
  ${FOG_XML_IO_HEADERS}
  ${FOG_XML_TOOLS_HEADERS}
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_ALL_SOURCES ${FOG_CORE_WIN_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_CORE_WIN_HEADERS})
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_ALL_SOURCES ${FOG_CORE_MAC_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_CORE_MAC_HEADERS})
EndIf()

If(FOG_GUI_WINDOWS)
  List(APPEND FOG_ALL_SOURCES ${FOG_GUI_ENGINE_WIN_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_GUI_ENGINE_WIN_HEADERS})
EndIf()

If(FOG_GUI_MAC)
  List(APPEND FOG_ALL_SOURCES ${FOG_GUI_ENGINE_MAC_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_GUI_ENGINE_MAC_HEADERS})
EndIf()

Set(FOG_ALL_SOURCES_CORRECTED "")
ForEach(i ${FOG_ALL_SOURCES})
  List(APPEND FOG_ALL_SOURCES_CORRECTED ${FOG_DIR}/${i})
EndForEach()

Set(FOG_ALL_HEADERS_CORRECTED "")
ForEach(i ${FOG_ALL_HEADERS})
  List(APPEND FOG_ALL_HEADERS_CORRECTED ${FOG_DIR}/${i})
EndForEach()

# =============================================================================
# [Setup - Fog/Core/Config/ConfigCMake.h]
# =============================================================================

# Write CMAKE_ #defines into Fog/Core/Config/Defs/ConfigCMake.h files
Configure_File(
  ${CMAKE_CURRENT_SOURCE_DIR}/${FOG_DIR}/Fog/Core/Config/Defs/ConfigCMake.h.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/${FOG_DIR}/Fog/Core/Config/Defs/ConfigCMake.h)

# =============================================================================
# [Setup - Fix]
# =============================================================================

# TODO: Quick hack, do not open freetype dynamically, we can link to it.
If(FOG_FONT_FREETYPE)
  Include_Directories(/usr/include/freetype2)
EndIf()

# Trim libraries string (come compilers can complain if there are extra whitespaces).
String(STRIP "${FOG_LIBRARIES}" FOG_LIBRARIES)

# =============================================================================
# [Setup - Install Header Files]
# =============================================================================

If(NOT FOG_BUILD_STATIC)
  ForEach(i ${FOG_ALL_HEADERS})
    Get_Filename_Component(path ${i} PATH)
    Get_Filename_Component(name ${i} NAME)
    If(NOT "${name}" MATCHES "_p.h$")
      Install(FILES ${i} DESTINATION "include/${path}")
    EndIf()
  EndForEach()
EndIf()

# =============================================================================
# [Setup - Binary - Fog]
# =============================================================================

If(NOT FOG_BUILD_STATIC)
  Add_Library(Fog SHARED ${FOG_ALL_SOURCES_CORRECTED} ${FOG_ALL_HEADERS_CORRECTED})
  Target_Link_Libraries(Fog ${FOG_LIBRARIES})
  Install(TARGETS Fog DESTINATION lib)
Else()
  Add_Library(Fog STATIC ${FOG_ALL_SOURCES_CORRECTED} ${FOG_ALL_HEADERS_CORRECTED})
  Target_Link_Libraries(Fog ${FOG_LIBRARIES})
EndIf()

# =============================================================================
# [Setup - Binary - Fog_Gui_X11]
# =============================================================================

If(NOT FOG_BUILD_STATIC AND FOG_GUI_X11 AND FOG_BUILD_GUI_X11)
  Add_Library(Fog_Gui_X11 SHARED ${FOG_GUI_ENGINE_X11_SOURCES} ${FOG_GUI_ENGINE_X11_HEADERS})
  Target_Link_Libraries(Fog_Gui_X11 Fog ${FOG_LIBRARIES})
  Install(TARGETS Fog_Gui_X11 DESTINATION lib)
EndIf()

# =============================================================================
# [Setup - Binary - Examples]
# =============================================================================

If(NOT FOG_BUILD_STATIC AND FOG_BUILD_EXAMPLES)
  Set(FOG_EXAMPLE_SOURCES
    FogSvgView
    FogTest
  )
  ForEach(file ${FOG_EXAMPLE_SOURCES})
    Add_Executable(${file} WIN32 MACOSX_BUNDLE Examples/${file})
    Target_Link_Libraries(${file} Fog ${FOG_LIBRARIES})
  EndForEach(file)
EndIf()
