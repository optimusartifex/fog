# =============================================================================
# [Project]
# =============================================================================

CMake_Minimum_Required(VERSION 2.8.4)

If(NOT FOG_BUILD_STATIC)
  # Create Fog-Framework project.
  Project(Fog C CXX)

  Message("== ====================================================")
  Message("== [Fog-Framework - Normal Build (${CMAKE_BUILD_TYPE})]")
  Message("== ====================================================")
Else()
  Message("== ====================================================")
  Message("== [Fog-Framework - Static Build (${CMAKE_BUILD_TYPE})]")
  Message("== ====================================================")
EndIf()

# =============================================================================
# [Configuration]
# =============================================================================

# Whether to build static library (default FALSE).
# Set(FOG_BUILD_STATIC TRUE)

# Whether to build FogBench benchmarking tool (default FALSE).
# Set(FOG_BUILD_BENCH TRUE)

# Whether to build FogExamples (default FALSE).
# Set(FOG_BUILD_EXAMPLES TRUE)

# Whether to build X11 module for Fog/UI (default FALSE).
# Set(FOG_BUILD_UI_X11 TRUE)

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

Set(FOG_CXX_FLAGS_OPTIMIZE "")
Set(FOG_CXX_FLAGS_MMX "")
Set(FOG_CXX_FLAGS_3DNOW "")
Set(FOG_CXX_FLAGS_SSE "")
Set(FOG_CXX_FLAGS_SSE2 "")
Set(FOG_CXX_FLAGS_SSE3 "")
Set(FOG_CXX_FLAGS_SSSE3 "")

# =============================================================================
# [C++ Compiler - GCC Support]
# =============================================================================

If(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Check for supported compiler flags.
  Check_CXX_Compiler_Flag("-flto" FOG_CC_HAS_FLTO)
  Check_CXX_Compiler_Flag("-finline-functions" FOG_CC_HAS_INLINE_FUNCTIONS)
  Check_CXX_Compiler_Flag("-fmerge-all-constants" FOG_CC_HAS_FMERGE_ALL_CONSTANTS)
  Check_CXX_Compiler_Flag("-fno-keep-static-consts" FOG_CC_HAS_FNO_KEEP_STATIC_CONSTS)

  Check_CXX_Compiler_Flag("-Winline" FOG_CC_HAS_WINLINE)
  Check_CXX_Compiler_Flag("-Wno-enum-compare" FOG_CC_HAS_WNO_ENUM_COMPARE)

  # Turn off exceptions (smaller code).
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

  # Hide all symbols by default. Exported symbols are declared by FOG_API.
  If (NOT MINGW)
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
  EndIf()

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

  # Inline functions.
  If(${FOG_CC_HAS_INLINE_FUNCTIONS})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finline-functions")
  EndIf()

  # Strict aliasing, Fog-Framework should be well written.
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing")

  # We are not interested in math-errno.
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-math-errno")

  # Disabled by default for binary compatibility with projects which
  # use Fog-Framework. Enabled only for testing.
  # Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

  # -flto
  #
  # Use link-time optimization.

  # TODO: Disabled, not working properly!
  # If (${FOG_CC_HAS_FLTO})
  #   Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
  # EndIf()

  # -Winline:
  #
  # We want to see all functions which are not inlined. This could help
  # when to identify critical functions in code that were't inlined.
  If (${FOG_CC_HAS_WINLINE})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
  EndIf()

  # -Wno-enum-compare.
  If (${FOG_CC_HAS_WNO_ENUM_COMPARE})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-enum-compare")
  EndIf()

  # -O3 doesn't improve the performance, because all inner loops used by Fog
  # Use -O2 as a standard optimization level when running release builds. The
  # are already optimized - there is nothing compiler can do. Another argument
  # to use -O2 is that -O3 produces larger binaries without speed improvements.
  Set(FOG_CXX_FLAGS_OPTIMIZE "-O2")

  # Setup FOG_CFLAGS specific to GCC.
  Set(FOG_CXX_FLAGS_MMX "${FOG_CXX_FLAGS_OPTIMIZE} -mmmx")
  Set(FOG_CXX_FLAGS_3DNOW "${FOG_CXX_FLAGS_OPTIMIZE} -mmmx -m3dnow")
  Set(FOG_CXX_FLAGS_SSE "${FOG_CXX_FLAGS_OPTIMIZE} -mmmx -msse")
  Set(FOG_CXX_FLAGS_SSE2 "${FOG_CXX_FLAGS_OPTIMIZE} -msse -msse2")
  Set(FOG_CXX_FLAGS_SSE3 "${FOG_CXX_FLAGS_OPTIMIZE} -msse -msse2 -msse3")
  Set(FOG_CXX_FLAGS_SSSE3 "${FOG_CXX_FLAGS_OPTIMIZE} -msse -msse2 -msse3 -mssse3")
EndIf()

# =============================================================================
# [C++ Compiler - MSVC Support]
# =============================================================================

If(MSVC)
  Set(FOG_CXX_FLAGS_RELEASE "/Oi /Oy /GF /GS- /GR-")
  Set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${FOG_CXX_FLAGS_RELEASE}")
  Set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${FOG_CXX_FLAGS_RELEASE}")
  Set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${FOG_CXX_FLAGS_RELEASE}")

  Set(FOG_CXX_FLAGS_OPTIMIZE "/Ot")
  Set(FOG_CXX_FLAGS_MMX "${FOG_CXX_FLAGS_OPTIMIZE}")
  Set(FOG_CXX_FLAGS_3DNOW "${FOG_CXX_FLAGS_OPTIMIZE}")
  Set(FOG_CXX_FLAGS_SSE "${FOG_CXX_FLAGS_OPTIMIZE} /arch:SSE")
  Set(FOG_CXX_FLAGS_SSE2 "${FOG_CXX_FLAGS_OPTIMIZE} /arch:SSE2")
  Set(FOG_CXX_FLAGS_SSE3 "${FOG_CXX_FLAGS_OPTIMIZE} /arch:SSE2")
  Set(FOG_CXX_FLAGS_SSSE3 "${FOG_CXX_FLAGS_OPTIMIZE} /arch:SSE2")

  # Enable multi-process compilation by default.
  If(MSVC80 OR MSVC90 OR MSVC10)
     Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  EndIf()
EndIf()

# =============================================================================
# [C++ Compiler - Build-Type]
# =============================================================================

# According to CMake documentation, these CMAKE variables are used to control
# the build type using a single-configuration generator:
#
#   None           - CMAKE_C_FLAGS               , CMAKE_CXX_FLAGS
#   Debug          - CMAKE_C_FLAGS_DEBUG         , CMAKE_CXX_FLAGS_DEBUG
#   Release        - CMAKE_C_FLAGS_RELEASE       , CMAKE_CXX_FLAGS_RELEASE
#   RelWithDebInfo - CMAKE_C_FLAGS_RELWITHDEBINFO, CMAKE_CXX_FLAGS_RELWITHDEBINFO
#   MinSizeRel     - CMAKE_C_FLAGS_MINSIZEREL    , CMAKE_CXX_FLAGS_MINSIZEREL

If(CMAKE_BUILD_TYPE)
  # Build type is "Debug".
  If(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    Set(FOG_DEBUG TRUE)
  EndIf()

  # Build type is "Release".
  If(${CMAKE_BUILD_TYPE} MATCHES "Release")
  EndIf()
Else()
  # Build type is "None". set, assuming that the project is generated for IDE which
  # supports multiple build options.
  Set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFOG_DEBUG")
  Set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DFOG_NO_DEBUG")
  Set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DFOG_NO_DEBUG")
  Set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DFOG_NO_DEBUG")
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

Macro(AddOptimizedSources dst optimization)
  Set(local_condition "FOG_OPTIMIZE_${optimization}")
  Set(local_cflags "FOG_CXX_FLAGS_${optimization}")

  If(${local_condition})
    Set(local_files "")
    ForEach(file ${ARGN})
      Message("   Adding optimized file ${file} (${optimization})")
      List(APPEND local_files ${file})
    EndForEach()
    List(APPEND ${dst} ${local_files})
    Set_Source_Files_Properties(${local_files} PROPERTIES COMPILE_FLAGS "${${local_cflags}}")
  EndIf()
EndMacro()

# =============================================================================
# [Environment - Header Files]
# =============================================================================

If(NOT FOG_HAVE_FLOAT_H)
  Check_Include_Files(float.h FOG_HAVE_FLOAT_H)
EndIf()

If(NOT FOG_HAVE_LIMITS_H)
  Check_Include_Files(limits.h FOG_HAVE_LIMITS_H)
EndIf()

If(NOT FOG_HAVE_STDARG_H)
  Check_Include_Files(stdint.h FOG_HAVE_STDARG_H)
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
  List(APPEND FOG_LIBRARIES advapi32 gdi32 ole32 user32 uuid shell32 winmm)
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
# [Fog/Core]
# =============================================================================

Message("-- Configuring Fog/Core")

# =============================================================================
# [Fog/Core - Files]
# =============================================================================

# [Fog/Core]
Set(FOG_CORE_LIB_SOURCES
)

Set(FOG_CORE_LIB_HEADERS
  Fog/Core.h
)

# [Fog/Core/C++]
Set(FOG_CORE_CPP_SOURCES
)

Set(FOG_CORE_CPP_HEADERS
  Fog/Core/C++/Base.h
  Fog/Core/C++/Compiler.h
  Fog/Core/C++/CompilerBorland.h
  Fog/Core/C++/CompilerClang.h
  Fog/Core/C++/CompilerGnu.h
  Fog/Core/C++/CompilerIde.h
  Fog/Core/C++/CompilerMsc.h
  Fog/Core/C++/ConfigCMake.h
  Fog/Core/C++/Intrin3dNow.h
  Fog/Core/C++/IntrinMMX.h
  Fog/Core/C++/IntrinMMXExt.h
  Fog/Core/C++/IntrinSSE.h
  Fog/Core/C++/IntrinSSE2.h
  Fog/Core/C++/IntrinSSE3.h
  Fog/Core/C++/IntrinSSSE3.h
  Fog/Core/C++/PackByte.h
  Fog/Core/C++/PackWord.h
  Fog/Core/C++/PackDWord.h
  Fog/Core/C++/PackQWord.h
  Fog/Core/C++/PackRestore.h
  Fog/Core/C++/StdArch.h
  Fog/Core/C++/StdDefs.h
  Fog/Core/C++/StdHeaders.h
  Fog/Core/C++/StdInt.h
  Fog/Core/C++/StdNew.h
  Fog/Core/C++/StdOS.h
)

# [Fog/Core/Collection]
Set(FOG_CORE_COLLECTION_SOURCES
  Fog/Core/Collection/Util.cpp
)

Set(FOG_CORE_COLLECTION_HEADERS
  Fog/Core/Collection/StackP.h
  Fog/Core/Collection/Util.h
)

# [Fog/Core/Face]
Set(FOG_CORE_FACE_SOURCES
  Fog/Core/Face/Constants.cpp
)

Set(FOG_CORE_FACE_HEADERS
  Fog/Core/Face/Constants.h
  Fog/Core/Face/Face3dNow.h
  Fog/Core/Face/Face3dNowExt.h
  Fog/Core/Face/FaceC.h
  Fog/Core/Face/FaceC_Asm.h
  Fog/Core/Face/FaceC_P32.h
  Fog/Core/Face/FaceC_P64.h
  Fog/Core/Face/FaceC_Types.h
  Fog/Core/Face/FaceMMX.h
  Fog/Core/Face/FaceMMXExt.h
  Fog/Core/Face/FaceMMXSSE.h
  Fog/Core/Face/FaceMMXSSE2.h
  Fog/Core/Face/FaceMMXSSSE3.h
  Fog/Core/Face/FaceSSE.h
  Fog/Core/Face/FaceSSE2.h
  Fog/Core/Face/FaceSSE3.h
  Fog/Core/Face/FaceSSSE3.h
  Fog/Core/Face/Features.h
)

# [Fog/Core/Global]
Set(FOG_CORE_GLOBAL_SOURCES
  Fog/Core/Global/Api.cpp
  Fog/Core/Global/Assert.cpp
  Fog/Core/Global/Debug.cpp
  Fog/Core/Global/Init.cpp
)

Set(FOG_CORE_GLOBAL_HEADERS
  Fog/Core/Global/Api.h
  Fog/Core/Global/Assert.h
  Fog/Core/Global/Class.h
  Fog/Core/Global/Enum.h
  Fog/Core/Global/EnumCore.h
  Fog/Core/Global/EnumEvent.h
  Fog/Core/Global/EnumError.h
  Fog/Core/Global/EnumG2d.h
  Fog/Core/Global/EnumSvg.h
  Fog/Core/Global/EnumUI.h
  Fog/Core/Global/Debug.h
  Fog/Core/Global/Global.h
  Fog/Core/Global/Init.h
  Fog/Core/Global/Init_p.h
  Fog/Core/Global/Main.h
  Fog/Core/Global/Private.h
  Fog/Core/Global/Static.h
  Fog/Core/Global/TypeDefs.h
  Fog/Core/Global/TypeInfo.h
  Fog/Core/Global/Types.h
  Fog/Core/Global/Uninitialized.h
)

# [Fog/Core/Kernel]
Set(FOG_CORE_KERNEL_SOURCES
  Fog/Core/Kernel/Application.cpp
  Fog/Core/Kernel/Event.cpp
  Fog/Core/Kernel/EventLoop.cpp
  Fog/Core/Kernel/ListenerList.cpp
  Fog/Core/Kernel/Object.cpp
  Fog/Core/Kernel/Task.cpp
  Fog/Core/Kernel/Timer.cpp
)

Set(FOG_CORE_KERNEL_HEADERS
  Fog/Core/Kernel/Application.h
  Fog/Core/Kernel/Delegate.h
  Fog/Core/Kernel/Event.h
  Fog/Core/Kernel/EventLoop.h
  Fog/Core/Kernel/ListenerList.h
  Fog/Core/Kernel/Object.h
  Fog/Core/Kernel/Task.h
  Fog/Core/Kernel/Timer.h
)

# [Fog/Core/Math]
Set(FOG_CORE_MATH_SOURCES
  Fog/Core/Math/Function.cpp
  Fog/Core/Math/Fuzzy.cpp
  Fog/Core/Math/Integrate.cpp
  Fog/Core/Math/Math.cpp
  Fog/Core/Math/MathVec.cpp
  Fog/Core/Math/Solve.cpp
)

Set(FOG_CORE_MATH_HEADERS
  Fog/Core/Math/Cleanup.h
  Fog/Core/Math/Constants.h
  Fog/Core/Math/FloatBits.h
  Fog/Core/Math/FloatControl.h
  Fog/Core/Math/Function.h
  Fog/Core/Math/Fuzzy.h
  Fog/Core/Math/Integrate.h
  Fog/Core/Math/Interval.h
  Fog/Core/Math/Math.h
  Fog/Core/Math/MathVec.h
  Fog/Core/Math/Solve.h
)

# [Fog/Core/Memory]
Set(FOG_CORE_MEMORY_SOURCES
  Fog/Core/Memory/MemDebug.cpp
  Fog/Core/Memory/MemBlockAllocator.cpp
  Fog/Core/Memory/MemMgr.cpp
  Fog/Core/Memory/MemOps.cpp
  Fog/Core/Memory/MemPool.cpp
  Fog/Core/Memory/MemZoneAllocator.cpp
)

Set(FOG_CORE_MEMORY_HEADERS
  Fog/Core/Memory/BSwap.h
  Fog/Core/Memory/BSwap_gcc_pcc.h
  Fog/Core/Memory/BSwap_gcc_x86x64.h
  Fog/Core/Memory/BSwap_generic.h
  Fog/Core/Memory/BSwap_msc_intrin.h
  Fog/Core/Memory/BSwap_msc_x86.h
  Fog/Core/Memory/MemBlockAllocator.h
  Fog/Core/Memory/MemBuffer.h
  Fog/Core/Memory/MemBufferTmp_p.h
  Fog/Core/Memory/MemDebug_p.h
  Fog/Core/Memory/MemMgr.h
  Fog/Core/Memory/MemOps.h
  Fog/Core/Memory/MemPool.h
  Fog/Core/Memory/MemZoneAllocator.h
)

# [Fog/Core/OS]
Set(FOG_CORE_OS_SOURCES
  Fog/Core/OS/DirIterator.cpp
  Fog/Core/OS/Environment.cpp
  Fog/Core/OS/FileInfo.cpp
  Fog/Core/OS/FileMapping.cpp
  Fog/Core/OS/FilePath.cpp
  Fog/Core/OS/FileUtil.cpp
  Fog/Core/OS/Library.cpp
  Fog/Core/OS/OSInfo.cpp
  Fog/Core/OS/OSUtil.cpp
  Fog/Core/OS/UserUtil.cpp
)

Set(FOG_CORE_OS_HEADERS
  Fog/Core/OS/DirIterator.h
  Fog/Core/OS/Environment.h
  Fog/Core/OS/FileInfo.h
  Fog/Core/OS/FileMapping.h
  Fog/Core/OS/FilePath.h
  Fog/Core/OS/FileUtil.h
  Fog/Core/OS/Library.h
  Fog/Core/OS/OSInfo.h
  Fog/Core/OS/OSUtil.h
  Fog/Core/OS/UserUtil.h
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_CORE_OS_SOURCES
    Fog/Core/OS/WinCom.cpp
    Fog/Core/OS/WinUtil.cpp
  )
  List(APPEND FOG_CORE_OS_HEADERS
    Fog/Core/OS/WinCom.h
    Fog/Core/OS/WinUtil.h
  )
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_CORE_OS_SOURCES
    Fog/Core/OS/MacScopedAutoReleasePool.mm
    Fog/Core/OS/MacUtil.mm
  )
  List(APPEND FOG_CORE_OS_HEADERS
    Fog/Core/OS/MacScopedAutoReleasePool.h
    Fog/Core/OS/MacUtil.h
  )
EndIf()

# [Fog/Core/Threading]
Set(FOG_CORE_THREADING_SOURCES
  Fog/Core/Threading/Lock.cpp
  Fog/Core/Threading/Thread.cpp
  Fog/Core/Threading/ThreadCondition.cpp
  Fog/Core/Threading/ThreadEvent.cpp
  Fog/Core/Threading/ThreadLocal.cpp
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
  Fog/Core/Threading/ThreadLocal.h
  Fog/Core/Threading/ThreadPool.h
)

# [Fog/Core/Tools]
Set(FOG_CORE_TOOLS_SOURCES
  Fog/Core/Tools/Algorithm.cpp
  Fog/Core/Tools/Char.cpp
  Fog/Core/Tools/CharData.cpp
  Fog/Core/Tools/Cpu.cpp
  Fog/Core/Tools/Date.cpp
  Fog/Core/Tools/Hash.cpp
  Fog/Core/Tools/HashUtil.cpp
  Fog/Core/Tools/List.cpp
  Fog/Core/Tools/Locale.cpp
  Fog/Core/Tools/Logger.cpp
  Fog/Core/Tools/ManagedString.cpp
  Fog/Core/Tools/Range.cpp
  Fog/Core/Tools/RegExp.cpp
  Fog/Core/Tools/Stream.cpp
  Fog/Core/Tools/String.cpp
  Fog/Core/Tools/StringUtil.cpp
  Fog/Core/Tools/StringUtil_dtoa.cpp
  Fog/Core/Tools/Stub.cpp
  Fog/Core/Tools/TextChunk.cpp
  Fog/Core/Tools/TextCodec.cpp
  Fog/Core/Tools/TextIterator.cpp
  Fog/Core/Tools/Time.cpp
  Fog/Core/Tools/Var.cpp
)

Set(FOG_CORE_TOOLS_HEADERS
  Fog/Core/Tools/Algorithm.h
  Fog/Core/Tools/Char.h
  Fog/Core/Tools/CharData.h
  Fog/Core/Tools/Cpu.h
  Fog/Core/Tools/Date.h
  Fog/Core/Tools/Hash.h
  Fog/Core/Tools/HashString.h
  Fog/Core/Tools/HashUntyped.h
  Fog/Core/Tools/HashUtil.h
  Fog/Core/Tools/List.h
  Fog/Core/Tools/ListReal.h
  Fog/Core/Tools/ListString.h
  Fog/Core/Tools/ListUntyped.h
  Fog/Core/Tools/ListVar.h
  Fog/Core/Tools/Locale.h
  Fog/Core/Tools/Logger.h
  Fog/Core/Tools/ManagedString.h
  Fog/Core/Tools/Range.h
  Fog/Core/Tools/RegExp.h
  Fog/Core/Tools/Stream.h
  Fog/Core/Tools/String.h
  Fog/Core/Tools/StringTmp_p.h
  Fog/Core/Tools/StringUtil.h
  Fog/Core/Tools/StringUtil_dtoa_p.h
  Fog/Core/Tools/Stub.h
  Fog/Core/Tools/Swap.h
  Fog/Core/Tools/TextChunk.h
  Fog/Core/Tools/TextCodec.h
  Fog/Core/Tools/TextCodec_Page8Tables_p.h
  Fog/Core/Tools/TextIterator.h
  Fog/Core/Tools/Time.h
  Fog/Core/Tools/Var.h
  Fog/Core/Tools/VarId.h
)

AddOptimizedSources(FOG_CORE_TOOLS_SOURCES SSE2
  Fog/Core/Tools/List_SSE2.cpp
)

# [Fog/Core/Xml]
Set(FOG_CORE_XML_SOURCES
  Fog/Core/Xml/XmlAttribute.cpp
  Fog/Core/Xml/XmlCDATA.cpp
  Fog/Core/Xml/XmlComment.cpp
  Fog/Core/Xml/XmlDocument.cpp
  Fog/Core/Xml/XmlDomReader.cpp
  Fog/Core/Xml/XmlDomWriter.cpp
  Fog/Core/Xml/XmlElement.cpp
  Fog/Core/Xml/XmlEntity.cpp
  Fog/Core/Xml/XmlIdAttribute.cpp
  Fog/Core/Xml/XmlIdManager.cpp
  Fog/Core/Xml/XmlNoText.cpp
  Fog/Core/Xml/XmlPI.cpp
  Fog/Core/Xml/XmlSaxReader.cpp
  Fog/Core/Xml/XmlSaxWriter.cpp
  Fog/Core/Xml/XmlText.cpp
)

Set(FOG_CORE_XML_HEADERS
  Fog/Core/Xml/XmlAttribute.h
  Fog/Core/Xml/XmlCDATA.h
  Fog/Core/Xml/XmlComment.h
  Fog/Core/Xml/XmlDocument.h
  Fog/Core/Xml/XmlDomReader.h
  Fog/Core/Xml/XmlDomWriter.h
  Fog/Core/Xml/XmlElement.h
  Fog/Core/Xml/XmlEntity_p.h
  Fog/Core/Xml/XmlIdAttribute_p.h
  Fog/Core/Xml/XmlIdManager.h
  Fog/Core/Xml/XmlNoText.h
  Fog/Core/Xml/XmlPI.h
  Fog/Core/Xml/XmlSaxReader.h
  Fog/Core/Xml/XmlSaxWriter.h
  Fog/Core/Xml/XmlText.h
)

# Source groups.
Source_Group("Core"                 FILES ${FOG_CORE_LIB_SOURCES}             ${FOG_CORE_LIB_HEADERS}            )
Source_Group("Core/C++"             FILES ${FOG_CORE_CPP_SOURCES}             ${FOG_CORE_CPP_HEADERS}            )
Source_Group("Core/Collection"      FILES ${FOG_CORE_COLLECTION_SOURCES}      ${FOG_CORE_COLLECTION_HEADERS}     )
Source_Group("Core/Face"            FILES ${FOG_CORE_FACE_SOURCES}            ${FOG_CORE_FACE_HEADERS}           )
Source_Group("Core/Global"          FILES ${FOG_CORE_GLOBAL_SOURCES}          ${FOG_CORE_GLOBAL_HEADERS}         )
Source_Group("Core/Kernel"          FILES ${FOG_CORE_KERNEL_SOURCES}          ${FOG_CORE_KERNEL_HEADERS}         )
Source_Group("Core/Math"            FILES ${FOG_CORE_MATH_SOURCES}            ${FOG_CORE_MATH_HEADERS}           )
Source_Group("Core/Memory"          FILES ${FOG_CORE_MEMORY_SOURCES}          ${FOG_CORE_MEMORY_HEADERS}         )
Source_Group("Core/OS"              FILES ${FOG_CORE_OS_SOURCES}              ${FOG_CORE_OS_HEADERS}             )
Source_Group("Core/Threading"       FILES ${FOG_CORE_THREADING_SOURCES}       ${FOG_CORE_THREADING_HEADERS}      )
Source_Group("Core/Tools"           FILES ${FOG_CORE_TOOLS_SOURCES}           ${FOG_CORE_TOOLS_HEADERS}          )
Source_Group("Core/Xml"             FILES ${FOG_CORE_XML_SOURCES}             ${FOG_CORE_XML_HEADERS}            )

# =============================================================================
# [Fog/G2d]
# =============================================================================

Message("-- Configuring Fog/G2d")

# =============================================================================
# [Fog/G2d - Files]
# =============================================================================

# [Fog/G2d]
Set(FOG_G2D_LIB_SOURCES
)

Set(FOG_G2D_LIB_HEADERS
  Fog/G2d.h
)

# [Fog/G2d/Face]
Set(FOG_G2D_FACE_SOURCES
)

Set(FOG_G2D_FACE_HEADERS
  Fog/G2d/Face/Face_Raster_C.h
  Fog/G2d/Face/Face_Raster_MMX.h
  Fog/G2d/Face/Face_Raster_MMX_Ext.h
  Fog/G2d/Face/Face_Raster_SSE.h
  Fog/G2d/Face/Face_Raster_SSE2.h
  Fog/G2d/Face/Face_Raster_SSE3.h
  Fog/G2d/Face/Face_Raster_SSSE3.h
)

# [Fog/G2d/Geometry]
Set(FOG_G2D_GEOMETRY_SOURCES
  Fog/G2d/Geometry/Arc.cpp
  Fog/G2d/Geometry/Box.cpp
  Fog/G2d/Geometry/CBezier.cpp
  Fog/G2d/Geometry/Chord.cpp
  Fog/G2d/Geometry/Circle.cpp
  Fog/G2d/Geometry/Ellipse.cpp
  Fog/G2d/Geometry/Line.cpp
  Fog/G2d/Geometry/Math2d.cpp
  Fog/G2d/Geometry/Path.cpp
  Fog/G2d/Geometry/PathClipper.cpp
  Fog/G2d/Geometry/PathEffect.cpp
  Fog/G2d/Geometry/PathInfo.cpp
  Fog/G2d/Geometry/PathOnPath.cpp
  Fog/G2d/Geometry/PathStroker.cpp
  Fog/G2d/Geometry/Pie.cpp
  Fog/G2d/Geometry/Point.cpp
  Fog/G2d/Geometry/Polygon.cpp
  Fog/G2d/Geometry/QBezier.cpp
  Fog/G2d/Geometry/Rect.cpp
  Fog/G2d/Geometry/Round.cpp
  Fog/G2d/Geometry/Shape.cpp
  Fog/G2d/Geometry/Size.cpp
  Fog/G2d/Geometry/Size.cpp
  Fog/G2d/Geometry/Transform.cpp
  Fog/G2d/Geometry/Triangle.cpp
)

Set(FOG_G2D_GEOMETRY_HEADERS
  Fog/G2d/Geometry/Arc.h
  Fog/G2d/Geometry/Box.h
  Fog/G2d/Geometry/CBezier.h
  Fog/G2d/Geometry/Chord.h
  Fog/G2d/Geometry/Circle.h
  Fog/G2d/Geometry/Ellipse.h
  Fog/G2d/Geometry/Internals_p.h
  Fog/G2d/Geometry/Line.h
  Fog/G2d/Geometry/Math2d.h
  Fog/G2d/Geometry/Path.h
  Fog/G2d/Geometry/PathClipper.h
  Fog/G2d/Geometry/PathEffect.h
  Fog/G2d/Geometry/PathInfo.h
  Fog/G2d/Geometry/PathOnPath.h
  Fog/G2d/Geometry/PathStroker.h
  Fog/G2d/Geometry/PathTmp_p.h
  Fog/G2d/Geometry/Pie.h
  Fog/G2d/Geometry/Point.h
  Fog/G2d/Geometry/Polygon.h
  Fog/G2d/Geometry/PolygonTmp_p.h
  Fog/G2d/Geometry/QBezier.h
  Fog/G2d/Geometry/Rect.h
  Fog/G2d/Geometry/Round.h
  Fog/G2d/Geometry/Shape.h
  Fog/G2d/Geometry/Size.h
  Fog/G2d/Geometry/Transform.h
  Fog/G2d/Geometry/Triangle.h
)

AddOptimizedSources(FOG_G2D_GEOMETRY_SOURCES 3DNOW
  Fog/G2d/Geometry/Transform_3dNow.cpp
)

AddOptimizedSources(FOG_G2D_GEOMETRY_SOURCES SSE
  Fog/G2d/Geometry/PathClipper_SSE.cpp
  Fog/G2d/Geometry/Transform_SSE.cpp
)

AddOptimizedSources(FOG_G2D_GEOMETRY_SOURCES SSE2
  Fog/G2d/Geometry/PathClipper_SSE2.cpp
  Fog/G2d/Geometry/Transform_SSE2.cpp
)

# [Fog/G2d/Imaging]
Set(FOG_G2D_IMAGING_SOURCES
  Fog/G2d/Imaging/Image.cpp
  Fog/G2d/Imaging/ImageCodec.cpp
  Fog/G2d/Imaging/ImageCodecProvider.cpp
  Fog/G2d/Imaging/ImageConverter.cpp
  Fog/G2d/Imaging/ImageDecoder.cpp
  Fog/G2d/Imaging/ImageEncoder.cpp
  Fog/G2d/Imaging/ImageFilter.cpp
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
  Fog/G2d/Imaging/ImageEncoder.h
  Fog/G2d/Imaging/ImageFilter.h
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

Set(FOG_G2D_IMAGING_FILTERS_SOURCES
  Fog/G2d/Imaging/Filters/FeBlur.cpp
  Fog/G2d/Imaging/Filters/FeColorLut.cpp
  Fog/G2d/Imaging/Filters/FeColorLutArray.cpp
  Fog/G2d/Imaging/Filters/FeColorMatrix.cpp
  Fog/G2d/Imaging/Filters/FeComponentFunction.cpp
  Fog/G2d/Imaging/Filters/FeComponentTransfer.cpp
  Fog/G2d/Imaging/Filters/FeConvolveMatrix.cpp
  Fog/G2d/Imaging/Filters/FeConvolveSeparable.cpp
  Fog/G2d/Imaging/Filters/FeMorphology.cpp
)

Set(FOG_G2D_IMAGING_FILTERS_HEADERS
  Fog/G2d/Imaging/Filters/FeBorder.h
  Fog/G2d/Imaging/Filters/FeBase.h
  Fog/G2d/Imaging/Filters/FeBlur.h
  Fog/G2d/Imaging/Filters/FeColorLut.h
  Fog/G2d/Imaging/Filters/FeColorLutArray.h
  Fog/G2d/Imaging/Filters/FeColorMatrix.h
  Fog/G2d/Imaging/Filters/FeComponentFunction.h
  Fog/G2d/Imaging/Filters/FeComponentTransfer.h
  Fog/G2d/Imaging/Filters/FeConvolveMatrix.h
  Fog/G2d/Imaging/Filters/FeConvolveSeparable.h
  Fog/G2d/Imaging/Filters/FeMorphology.h
)

AddOptimizedSources(FOG_G2D_IMAGING_FILTERS_SOURCES SSE
  Fog/G2d/Imaging/Filters/FeColorMatrix_SSE.cpp
)

If(NOT FOG_HAVE_LIBJPEG)
  Check_Include_Files(jconfig.h FOG_HAVE_LIBJPEG)
EndIf()

If(NOT FOG_HAVE_LIBPNG)
  Check_Include_Files(png.h FOG_HAVE_LIBPNG)
EndIf()

Set_Source_Files_Properties(
  Fog/G2d/Imaging/Image.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CXX_FLAGS_OPTIMIZE}"
)

# [Fog/G2d/Painting]
Set(FOG_G2D_PAINTING_SOURCES
  Fog/G2d/Painting/NullPaintEngine.cpp
  Fog/G2d/Painting/PaintDevice.cpp
  Fog/G2d/Painting/PaintDeviceInfo.cpp
  Fog/G2d/Painting/PaintEngine.cpp
  Fog/G2d/Painting/PaintParams.cpp
  Fog/G2d/Painting/PaintUtil.cpp
  Fog/G2d/Painting/Painter.cpp
  Fog/G2d/Painting/RasterApi.cpp
  Fog/G2d/Painting/RasterConstants.cpp
  Fog/G2d/Painting/RasterInit.cpp
  Fog/G2d/Painting/RasterInit_C.cpp
  Fog/G2d/Painting/RasterPaintContext.cpp
  Fog/G2d/Painting/RasterPaintEngine.cpp
  Fog/G2d/Painting/RasterPaintWork.cpp
  Fog/G2d/Painting/RasterScanline.cpp
  Fog/G2d/Painting/Rasterizer.cpp
)

Set(FOG_G2D_PAINTING_HEADERS
  Fog/G2d/Painting/NullPaintEngine_p.h
  Fog/G2d/Painting/PaintDevice.h
  Fog/G2d/Painting/PaintDeviceInfo.h
  Fog/G2d/Painting/PaintEngine.h
  Fog/G2d/Painting/PaintParams.h
  Fog/G2d/Painting/PaintUtil.h
  Fog/G2d/Painting/Painter.h
  Fog/G2d/Painting/RasterApi_p.h
  Fog/G2d/Painting/RasterConstants_p.h
  Fog/G2d/Painting/RasterInit_p.h
  Fog/G2d/Painting/RasterPaintContext_p.h
  Fog/G2d/Painting/RasterPaintEngine_p.h
  Fog/G2d/Painting/RasterPaintStructs_p.h
  Fog/G2d/Painting/RasterPaintWork_p.h
  Fog/G2d/Painting/RasterScanline_p.h
  Fog/G2d/Painting/RasterSpan_p.h
  Fog/G2d/Painting/RasterState_p.h
  Fog/G2d/Painting/RasterStructs_p.h
  Fog/G2d/Painting/RasterUtil_p.h
  Fog/G2d/Painting/Rasterizer_p.h
)

Set_Source_Files_Properties(
  Fog/G2d/Painting/Rasterizer.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CXX_FLAGS_OPTIMIZE}"
)

AddOptimizedSources(FOG_G2D_PAINTING_SOURCES SSE2
  Fog/G2d/Painting/RasterPaintEngine_SSE2.cpp
)

# [Fog/G2d/Painting/RasterOps_C]
Set(FOG_G2D_PAINTING_RASTEROPS_C_SOURCES
)

Set(FOG_G2D_PAINTING_RASTEROPS_C_HEADERS
  Fog/G2d/Painting/RasterOps_C/BaseAccess_p.h
  Fog/G2d/Painting/RasterOps_C/BaseConvert_p.h
  Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h
  Fog/G2d/Painting/RasterOps_C/BaseHelpers_p.h
  Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h
  Fog/G2d/Painting/RasterOps_C/CompositeClear_p.h
  Fog/G2d/Painting/RasterOps_C/CompositeNop_p.h
  Fog/G2d/Painting/RasterOps_C/CompositeSrc_p.h
  Fog/G2d/Painting/RasterOps_C/CompositeSrcOver_p.h
  Fog/G2d/Painting/RasterOps_C/GradientBase_p.h
  Fog/G2d/Painting/RasterOps_C/GradientConical_p.h
  Fog/G2d/Painting/RasterOps_C/GradientLinear_p.h
  Fog/G2d/Painting/RasterOps_C/GradientRadial_p.h
  Fog/G2d/Painting/RasterOps_C/GradientRectangular_p.h
  Fog/G2d/Painting/RasterOps_C/TextureAffine_p.h
  Fog/G2d/Painting/RasterOps_C/TextureBase_p.h
  Fog/G2d/Painting/RasterOps_C/TextureProjection_p.h
  Fog/G2d/Painting/RasterOps_C/TextureScale_p.h
  Fog/G2d/Painting/RasterOps_C/TextureSimple_p.h
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

AddOptimizedSources(FOG_G2D_SOURCE_SOURCES SSE2
  Fog/G2d/Source/Color_SSE2.cpp
)

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
  Fog/G2d/Text/Font.cpp
  Fog/G2d/Text/FontFace.cpp
  Fog/G2d/Text/FontFaceCache.cpp
  Fog/G2d/Text/FontKerning.cpp
  Fog/G2d/Text/FontManager.cpp
  Fog/G2d/Text/FontProvider.cpp
  Fog/G2d/Text/GlyphBitmap.cpp
  Fog/G2d/Text/GlyphBitmapCache.cpp
  Fog/G2d/Text/GlyphOutline.cpp
  Fog/G2d/Text/GlyphOutlineCache.cpp
  Fog/G2d/Text/NullFontFace.cpp
  Fog/G2d/Text/TextExtents.cpp
  Fog/G2d/Text/TextLayout.cpp
  Fog/G2d/Text/TextRect.cpp
)

Set(FOG_G2D_TEXT_HEADERS
  Fog/G2d/Text/Font.h
  Fog/G2d/Text/FontFace.h
  Fog/G2d/Text/FontFaceCache.h
  Fog/G2d/Text/FontHints.h
  Fog/G2d/Text/FontKerning.h
  Fog/G2d/Text/FontManager.h
  Fog/G2d/Text/FontMetrics.h
  Fog/G2d/Text/FontProvider.h
  Fog/G2d/Text/GlyphBitmap.h
  Fog/G2d/Text/GlyphBitmapCache.h
  Fog/G2d/Text/GlyphMetrics.h
  Fog/G2d/Text/GlyphOutline.h
  Fog/G2d/Text/GlyphOutlineCache.h
  Fog/G2d/Text/NullFontFace_p.h
  Fog/G2d/Text/TextExtents.h
  Fog/G2d/Text/TextHints.h
  Fog/G2d/Text/TextLayout.h
  Fog/G2d/Text/TextRect.h
)

# [Fog/G2d/Text - Windows]
Set(FOG_G2D_TEXT_SOURCES_WIN
  Fog/G2d/Text/WinFontFace.cpp
  Fog/G2d/Text/WinFontMaster.cpp
  Fog/G2d/Text/WinFontProvider.cpp
  Fog/G2d/Text/WinFontUtil.cpp
)

Set(FOG_G2D_TEXT_HEADERS_WIN
  Fog/G2d/Text/WinFontFace.h
  Fog/G2d/Text/WinFontMaster.h
  Fog/G2d/Text/WinFontProvider.h
  Fog/G2d/Text/WinFontUtil_p.h
)

# [Fog/G2d/Text - Mac]
Set(FOG_G2D_TEXT_SOURCES_MAC
  Fog/G2d/Text/MacFontFace.mm
  Fog/G2d/Text/MacFontMaster.mm
  Fog/G2d/Text/MacFontProvider.mm
)
Set(FOG_G2D_TEXT_HEADERS_MAC
  Fog/G2d/Text/MacFontFace.h
  Fog/G2d/Text/MacFontMaster.h
  Fog/G2d/Text/MacFontProvider.h
)

# [Fog/G2d/Text - FreeType]
Set(FOG_G2D_TEXT_SOURCES_FREETYPE
  Fog/G2d/Text/FTFontFace.cpp
  Fog/G2d/Text/FTFontMaster.cpp
  Fog/G2d/Text/FTFontProvider.cpp
)

Set(FOG_G2D_TEXT_HEADERS_FREETYPE
  Fog/G2d/Text/FTFontFace.h
  Fog/G2d/Text/FTFontMaster.h
  Fog/G2d/Text/FTFontProvider.h
)

# [Fog/G2d/Text - Detect]
If(FOG_OS_WINDOWS)
  Set(FOG_FONT_WINDOWS TRUE)
  List(APPEND FOG_G2D_TEXT_SOURCES ${FOG_G2D_TEXT_SOURCES_WIN})
  List(APPEND FOG_G2D_TEXT_HEADERS ${FOG_G2D_TEXT_HEADERS_WIN})
ElseIf(FOG_OS_MAC)
  Set(FOG_FONT_MAC TRUE)
  List(APPEND FOG_G2D_TEXT_SOURCES ${FOG_G2D_TEXT_SOURCES_MAC})
  List(APPEND FOG_G2D_TEXT_HEADERS ${FOG_G2D_TEXT_HEADERS_MAC})
Else()
  Find_File(HAVE_FREETYPE freetype-config)
  If(HAVE_FREETYPE)
    Exec_Program(freetype-config ARGS --cflags OUTPUT_VARIABLE FREETYPE_CONFIG_OUT RETURN_VALUE FREETYPE_CONFIG_RET)
    Set(FOG_FONT_FREETYPE TRUE)
  EndIf()

  If(NOT FOG_HAVE_FONTCONFIG)
    Check_Include_Files(fontconfig/fontconfig.h FOG_HAVE_FONTCONFIG)
  EndIf()

  List(APPEND FOG_G2D_TEXT_SOURCES ${FOG_G2D_TEXT_SOURCES_FREETYPE})
  List(APPEND FOG_G2D_TEXT_HEADERS ${FOG_G2D_TEXT_HEADERS_FREETYPE})
  Set_Source_Files_Properties(${FOG_G2d_TEXT_SOURCES_FREETYPE} PROPERTIES COMPILE_FLAGS "${FREETYPE_CONFIG_OUT}")
EndIf()

# [Fog/G2d/Tools]
Set(FOG_G2D_TOOLS_SOURCES
  Fog/G2d/Tools/ColorAnalyzer.cpp
  Fog/G2d/Tools/DitherTable.cpp
  Fog/G2d/Tools/Dpi.cpp
  Fog/G2d/Tools/Matrix.cpp
  Fog/G2d/Tools/Reduce.cpp
  Fog/G2d/Tools/Region.cpp
  Fog/G2d/Tools/RegionUtil.cpp
)

Set(FOG_G2D_TOOLS_HEADERS
  Fog/G2d/Tools/ColorAnalyzer_p.h
  Fog/G2d/Tools/DitherTable_p.h
  Fog/G2d/Tools/Dpi.h
  Fog/G2d/Tools/Matrix.h
  Fog/G2d/Tools/Reduce_p.h
  Fog/G2d/Tools/Region.h
  Fog/G2d/Tools/RegionTmp_p.h
  Fog/G2d/Tools/RegionUtil_p.h
)

AddOptimizedSources(FOG_G2D_TOOLS_SOURCES SSE2
  Fog/G2d/Tools/Region_SSE2.cpp
)

# [Fog/G2d/Win]
If(FOG_OS_WINDOWS)
  Set(FOG_G2D_WIN_SOURCES
    Fog/G2d/Win/GdipLibrary.cpp
    Fog/G2d/Win/UxThemeLibrary.cpp
  )
  Set(FOG_G2D_WIN_HEADERS
    Fog/G2d/Win/GdipLibrary.h
    Fog/G2d/Win/UxThemeLibrary.h
  )
EndIf()

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
Source_Group("G2d/Face"             FILES ${FOG_G2D_FACE_SOURCES}             ${FOG_G2D_FACE_HEADERS}            )
Source_Group("G2d/Geometry"         FILES ${FOG_G2D_GEOMETRY_SOURCES}         ${FOG_G2D_GEOMETRY_HEADERS}        )
Source_Group("G2d/Imaging"          FILES ${FOG_G2D_IMAGING_SOURCES}          ${FOG_G2D_IMAGING_HEADERS}         )
Source_Group("G2d/Imaging/Codecs"   FILES ${FOG_G2D_IMAGING_CODECS_SOURCES}   ${FOG_G2D_IMAGING_CODECS_HEADERS}  )
Source_Group("G2d/Imaging/Filters"  FILES ${FOG_G2D_IMAGING_FILTERS_SOURCES}  ${FOG_G2D_IMAGING_FILTERS_HEADERS} )
Source_Group("G2d/Painting"         FILES ${FOG_G2D_PAINTING_SOURCES}         ${FOG_G2D_PAINTING_HEADERS}        )
Source_Group("G2d/Source"           FILES ${FOG_G2D_SOURCE_SOURCES}           ${FOG_G2D_SOURCE_HEADERS}          )
Source_Group("G2d/Text"             FILES ${FOG_G2D_TEXT_SOURCES}             ${FOG_G2D_TEXT_HEADERS}            )
Source_Group("G2d/Tools"            FILES ${FOG_G2D_TOOLS_SOURCES}            ${FOG_G2D_TOOLS_HEADERS}           )

Source_Group("G2d/Painting/RasterOps_C" FILES
  ${FOG_G2D_PAINTING_RASTEROPS_C_SOURCES}
  ${FOG_G2D_PAINTING_RASTEROPS_C_HEADERS}
)

If(FOG_OS_WINDOWS)
Source_Group("G2d/Win"              FILES ${FOG_G2D_WIN_SOURCES}              ${FOG_G2D_WIN_HEADERS}             )
EndIf()

If(FOG_OS_MAC)
Source_Group("G2d/Mac"              FILES ${FOG_G2D_MAC_SOURCES}              ${FOG_G2D_MAC_HEADERS}             )
EndIf()

# =============================================================================
# [Fog/UI]
# =============================================================================

Message("-- Configuring Fog/UI")

# =============================================================================
# [Fog/UI - Files]
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
Source_Group("Gui/Layout"           FILES ${FOG_GUI_LAYOUT_SOURCES}           ${FOG_GUI_LAYOUT_HEADERS}          )
Source_Group("Gui/Theming"          FILES ${FOG_GUI_THEMING_SOURCES}          ${FOG_GUI_THEMING_HEADERS}         )
Source_Group("Gui/Widget"           FILES ${FOG_GUI_WIDGET_SOURCES}           ${FOG_GUI_WIDGET_HEADERS}          )

# =============================================================================
# [Fog/Svg]
# =============================================================================

Message("-- Configuring Fog/Svg")

# =============================================================================
# [Fog-Svg]
# =============================================================================

Set(FOG_SVG_LIB_SOURCES
)

Set(FOG_SVG_LIB_HEADERS
  Fog/Svg.h
)

Set(FOG_SVG_DOM_SOURCES
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
  Fog/Svg/Dom/SvgRootElement.cpp
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
  Fog/Svg/Dom/SvgViewBoxAttribute.cpp
)

Set(FOG_SVG_DOM_HEADERS
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
  Fog/Svg/Dom/SvgRootElement_p.h
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
  Fog/Svg/Dom/SvgViewBoxAttribute_p.h
)

Set(FOG_SVG_VISIT_SOURCES
  Fog/Svg/Visit/SvgHitTest.cpp
  Fog/Svg/Visit/SvgMeasure.cpp
  Fog/Svg/Visit/SvgRender.cpp
  Fog/Svg/Visit/SvgVisitor.cpp
)

Set(FOG_SVG_VISIT_HEADERS
  Fog/Svg/Visit/SvgHitTest.h
  Fog/Svg/Visit/SvgMeasure.h
  Fog/Svg/Visit/SvgRender.h
  Fog/Svg/Visit/SvgVisitor.h
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
Source_Group("Svg/Tools"            FILES ${FOG_SVG_TOOLS_SOURCES}            ${FOG_SVG_TOOLS_HEADERS}           )
Source_Group("Svg/Visit"            FILES ${FOG_SVG_VISIT_SOURCES}            ${FOG_SVG_VISIT_HEADERS}           )

# =============================================================================
# [Fog-All]
# =============================================================================

Set(FOG_ALL_SOURCES
  # Fog/Core.
  ${FOG_CORE_LIB_SOURCES}
  ${FOG_CORE_CPP_SOURCES}
  ${FOG_CORE_COLLECTION_SOURCES}
  ${FOG_CORE_FACE_SOURCES}
  ${FOG_CORE_GLOBAL_SOURCES}
  ${FOG_CORE_KERNEL_SOURCES}
  ${FOG_CORE_MATH_SOURCES}
  ${FOG_CORE_MEMORY_SOURCES}
  ${FOG_CORE_OS_SOURCES}
  ${FOG_CORE_THREADING_SOURCES}
  ${FOG_CORE_TOOLS_SOURCES}
  ${FOG_CORE_XML_SOURCES}
  # Fog/G2d.
  ${FOG_G2D_LIB_SOURCES}
  ${FOG_G2D_FACE_SOURCES}
  ${FOG_G2D_GEOMETRY_SOURCES}
  ${FOG_G2D_IMAGING_SOURCES}
  ${FOG_G2D_IMAGING_CODECS_SOURCES}
  ${FOG_G2D_IMAGING_FILTERS_SOURCES}
  ${FOG_G2D_PAINTING_SOURCES}
  ${FOG_G2D_PAINTING_RASTEROPS_C_SOURCES}
  ${FOG_G2D_SOURCE_SOURCES}
  ${FOG_G2D_TEXT_SOURCES}
  ${FOG_G2D_TOOLS_SOURCES}
  # Fog/Gui.
  ${FOG_GUI_LIB_SOURCES}
  ${FOG_GUI_ANIMATION_SOURCES}
  ${FOG_GUI_CANVAS_SOURCES}
  ${FOG_GUI_ENGINE_SOURCES}
  ${FOG_GUI_LAYOUT_SOURCES}
  ${FOG_GUI_THEMING_SOURCES}
  ${FOG_GUI_WIDGET_SOURCES}
  # Fog/Svg.
  ${FOG_SVG_LIB_SOURCES}
  ${FOG_SVG_DOM_SOURCES}
  ${FOG_SVG_TOOLS_SOURCES}
  ${FOG_SVG_VISIT_SOURCES}
)

Set(FOG_ALL_HEADERS
  # Fog/Core.
  ${FOG_CORE_LIB_HEADERS}
  ${FOG_CORE_CPP_HEADERS}
  ${FOG_CORE_COLLECTION_HEADERS}
  ${FOG_CORE_FACE_HEADERS}
  ${FOG_CORE_GLOBAL_HEADERS}
  ${FOG_CORE_KERNEL_HEADERS}
  ${FOG_CORE_MATH_HEADERS}
  ${FOG_CORE_MEMORY_HEADERS}
  ${FOG_CORE_OS_HEADERS}
  ${FOG_CORE_THREADING_HEADERS}
  ${FOG_CORE_TOOLS_HEADERS}
  ${FOG_CORE_XML_HEADERS}
  # Fog/G2d.
  ${FOG_G2D_LIB_HEADERS}
  ${FOG_G2D_FACE_HEADERS}
  ${FOG_G2D_GEOMETRY_HEADERS}
  ${FOG_G2D_IMAGING_HEADERS}
  ${FOG_G2D_IMAGING_CODECS_HEADERS}
  ${FOG_G2D_IMAGING_FILTERS_HEADERS}
  ${FOG_G2D_PAINTING_HEADERS}
  ${FOG_G2D_PAINTING_RASTEROPS_C_HEADERS}
  ${FOG_G2D_GEOMETRY_HEADERS}
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
  ${FOG_GUI_LAYOUT_HEADERS}
  ${FOG_GUI_THEMING_HEADERS}
  ${FOG_GUI_WIDGET_HEADERS}
  # Fog/Svg.
  ${FOG_SVG_LIB_HEADERS}
  ${FOG_SVG_DOM_HEADERS}
  ${FOG_SVG_TOOLS_HEADERS}
  ${FOG_SVG_VISIT_HEADERS}
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_ALL_SOURCES ${FOG_G2D_WIN_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_G2D_WIN_HEADERS})
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_ALL_SOURCES ${FOG_G2D_MAC_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_G2D_MAC_HEADERS})
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
# [Setup - Fog/Core/C++/ConfigCMake.h]
# =============================================================================

# Write CMAKE_ #defines into Fog/Core/C++/ConfigCMake.h files
Configure_File(${CMAKE_CURRENT_LIST_DIR}/Fog/Core/C++/ConfigCMake.h.cmake
               ${CMAKE_CURRENT_LIST_DIR}/Fog/Core/C++/ConfigCMake.h)

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
# [Setup - Binary - Fog/UI-X11]
# =============================================================================

If(NOT FOG_BUILD_STATIC AND FOG_GUI_X11 AND FOG_BUILD_UI_X11)
  Add_Library(Fog_UI_X11 SHARED
    ${FOG_GUI_ENGINE_X11_SOURCES}
    ${FOG_GUI_ENGINE_X11_HEADERS})
  Target_Link_Libraries(Fog_UI_X11 Fog ${FOG_LIBRARIES})
  Install(TARGETS Fog_UI_X11 DESTINATION lib)
EndIf()

# =============================================================================
# [Setup - Binary - FogBench]
# =============================================================================

If(NOT FOG_BUILD_STATIC AND FOG_BUILD_BENCH)
  Set(BENCH_LIBRARIES "")
  Set(BENCH_INCLUDE "")

  If(FOG_OS_WINDOWS)
    # Set(FOG_BENCH_GDIPLUS True)
  Else()
    # Set(FOG_BENCH_CAIRO True)
    # Set(FOG_BENCH_QT4 True)
  EndIf()

  Configure_File(
    ${CMAKE_CURRENT_LIST_DIR}/Bench/BenchConfig.h.cmake
    ${CMAKE_CURRENT_LIST_DIR}/Bench/BenchConfig.h)

  Set(FOG_BENCH_FILES
    Bench/BenchApp.cpp
    Bench/BenchApp.h
    Bench/BenchCairo.cpp
    Bench/BenchCairo.h
    Bench/BenchConfig.h
    Bench/BenchFog.cpp
    Bench/BenchFog.h
    Bench/BenchGdiPlus.cpp
    Bench/BenchGdiPlus.h
    Bench/BenchQt4.cpp
    Bench/BenchQt4.h
  )

  If(FOG_BENCH_CAIRO)
    Include_Directories(/usr/include/cairo)
    List(APPEND BENCH_LIBRARIES cairo)
  EndIf(FOG_BENCH_CAIRO)

  If(FOG_BENCH_QT4)
    Include_Directories(/usr/include/qt4)
    Link_Directories(/usr/lib/qt4)
    List(APPEND BENCH_LIBRARIES QtCore QtGui)
  EndIf(FOG_BENCH_QT4)

  Add_Executable(FogBench ${FOG_BENCH_FILES})
  Target_Link_Libraries(FogBench Fog ${FOG_LIBRARIES} ${BENCH_LIBRARIES})
EndIf()

# =============================================================================
# [Setup - Binary - Examples]
# =============================================================================

If(NOT FOG_BUILD_STATIC AND FOG_BUILD_EXAMPLES)
  Set(FOG_EXAMPLE_SOURCES
    FogSvgView
    FogTest
    FogTestBezierOffset
    FogTestPolyOffset
    FogTestRegion
#   FogTestRle
  )

  ForEach(file ${FOG_EXAMPLE_SOURCES})
    Add_Executable(${file} WIN32 MACOSX_BUNDLE
      Examples/${file}.cpp
      Examples/Helpers.cpp
      Examples/Helpers.h)
    Target_Link_Libraries(${file} Fog ${FOG_LIBRARIES})
  EndForEach(file)

  Add_Executable(FogTestSizeOf
    Examples/FogTestSizeOf.cpp)
  Target_Link_Libraries(FogTestSizeOf Fog ${FOG_LIBRARIES})
EndIf()
