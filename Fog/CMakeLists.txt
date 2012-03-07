# =============================================================================
# [Project - CMakeLists.txt]
# =============================================================================

CMake_Minimum_Required(VERSION 2.8.4)

# =============================================================================
# [Project - Configuration]
# =============================================================================

# Whether to bypass project generator, using cmake Project() function(). This
# can be used in case that this CMakeLists.txt is included from different
# project which wants to build Fog statically.
# Set(FOG_BUILD_SLAVE FALSE)

# Whether to only setup project, but build nothing. Usable for cmake projects
# which are designed to just include Fog and Fog source files into the project
# and build all of this without making a static library.
# Set(FOG_BUILD_NOTHING FALSE)

# Whether to build static library (default FALSE).
# Set(FOG_BUILD_STATIC FALSE)

# Whether to build Fog/UI (default FALSE).
# Set(FOG_BUILD_UI FALSE)

# Whether to build Fog/UI-X11 support (default FALSE).
# Set(FOG_BUILD_UI_X11 FALSE)

# Whether to build Fog/UI-X11 module (default FALSE).
# Set(FOG_BUILD_UI_X11_MODULE FALSE)

# Whether to build FogBench benchmarking tool (default FALSE).
# Set(FOG_BUILD_BENCH FALSE)

# Whether to build FogExamples (default FALSE).
# Set(FOG_BUILD_EXAMPLES FALSE)

# Prefix of source files (the directory).
If (NOT FOG_SOURCE_PREFIX)
  Set(FOG_SOURCE_PREFIX "")
EndIf(NOT FOG_SOURCE_PREFIX)

# =============================================================================
# [Project - Build]
# =============================================================================

# Create Fog-Framework project. In case that FOG_BUILD_SLAVE is enabled, no
# project will be created, but the parent script is responsive for doing that.
If(NOT FOG_BUILD_SLAVE)
  Project(Fog C CXX)
  Message("-- Created Fog-Framework project (FOG_BUILD_SLAVE not enabled).")
Else()
  Message("-- Entering Fog-Framework slave build (FOG_BUILD_SLAVE enabled).")
EndIf()

# Always build static library when compiling for iOS or iOS simulator.
If(IOS)
  Set(FOG_BUILD_STATIC TRUE)
  Message("-- Forced to build a static library, because of iOS target.")
EndIf()

# Disable UI parts in case that UI is completely disabled.
If(NOT FOG_BUILD_UI AND FOG_BUILD_UI_X11)
  Set(FOG_BUILD_UI_X11 FALSE)
  Message("-- Disabled Fog/UI(X11), because of disabled Fog/UI.")
EndIf()

# Disable Bench/Examples in case that the UI is disabled.
If(NOT FOG_BUILD_UI AND FOG_BUILD_EXAMPLED)
  Set(FOG_BUILD_EXAMPLES FALSE)
  Message("-- Disabled Examples, because of disabled Fog/UI.")
EndIf()

If(NOT FOG_BUILD_STATIC)
  Message("")
  Message("== ====================================================")
  Message("== [Fog-Framework - Normal Build (${CMAKE_BUILD_TYPE})]")
  Message("== ====================================================")
  Message("")
Else()
  Message("")
  Message("== ====================================================")
  Message("== [Fog-Framework - Static Build (${CMAKE_BUILD_TYPE})]")
  Message("== ====================================================")
  Message("")
EndIf()

If(FOG_BUILD_SLAVE)
  Message("-- Build Slave - Enabled")
Else()
  Message("-- Build Slave - Disabled")
EndIf()

If(FOG_BUILD_NOTHING)
  Message("-- Build Nothing - Enabled")
Else()
  Message("-- Build Nothing - Disabled")
EndIf()

If(FOG_BUILD_STATIC)
  Message("-- Build Static - Enabled")
Else()
  Message("-- Build Static - Disabled")
EndIf()

If(FOG_BUILD_UI)
  Message("-- Build Fog/UI - Enabled")
Else()
  Message("-- Build Fog/UI - Disabled")
EndIf()

If(FOG_BUILD_UI_X11)
  Message("-- Build Fog/UI(X11) - Enabled")
Else()
  Message("-- Build Fog/UI(X11) - Disabled")
EndIf()

Message("")

# =============================================================================
# [Project - Directory]
# =============================================================================

If(NOT FOG_DIR)
  Set(FOG_DIR ${CMAKE_CURRENT_LIST_DIR})
  Message("-- Initializing FOG_DIR=${FOG_DIR}")
Else()
  Message("-- Using Custom FOG_DIR=${FOG_DIR}")
EndIf()

# =============================================================================
# [Project - Cleanup]
# =============================================================================

Set(CMAKE_CXX_STANDARD_LIBRARIES "")

# =============================================================================
# [Project - Libraries]
# =============================================================================

Set(FOG_LIBRARIES "")

# =============================================================================
# [Project - Include]
# =============================================================================

Include(CheckCXXCompilerFlag)
Include(CheckIncludeFiles)
Include(CheckLibraryExists)
Include(CheckTypeSize)

If(IOS)
  Set(CMAKE_THREAD_LIBS_INIT "-lpthread")
Else()
  Include(FindThreads)
EndIf()

Include_Directories(${FOG_DIR}/Src)

# =============================================================================
# [Project - Helpers]
# =============================================================================

Macro(FogAddSourceGroup group)
  Set(local_files "")

  ForEach(file ${ARGN})
    List(APPEND local_files "${FOG_SOURCE_PREFIX}${file}")
  EndForEach()

  Source_Group(${group} FILES ${local_files})
EndMacro()

# =============================================================================
# [OS - Detect]
# =============================================================================

# Detects both x86, and x64 version, and maybe ARM version (Win8+).
If(WIN32)
  Set(FOG_OS_WINDOWS TRUE)
EndIf()

# Detects both Mac OS X, and iOS.
If(APPLE)
  Set(FOG_OS_MAC TRUE)
EndIf()

If(IOS)
  Set(FOG_OS_IOS TRUE)
EndIf(IOS)

# Detects Mac OS X, Linux, and BSD like OSes.
If(APPLE OR LINUX OR UNIX)
  Set(FOG_OS_POSIX TRUE)
EndIf()

# =============================================================================
# [C++ Compiler - Architecture]
# =============================================================================

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
# [C++ Compiler - Fix]
# =============================================================================

# iOS support is a bit tricky. In order to successfully use it / compile it, we
# need to hardcode all checks to be able to generate the Xcode project.
If (FOG_OS_IOS)
  # Forces to disable any checking, we are hardcoding, because of cross-compilation.
  Set(DISABLE_COMPILER_CHECKS TRUE)

  # iOS forces to use little endian.
  Set(FOG_BYTE_ORDER     1)

  # iOS runs on a 32-bit platform.
  Set(FOG_SIZEOF_VOID    4)

  # Hardcode everything we need normally to check-for.
  Set(FOG_HAVE_DLFCN_H   TRUE)
  Set(FOG_HAVE_FENV_H    TRUE)
  Set(FOG_HAVE_FLOAT_H   TRUE)
  Set(FOG_HAVE_LIMITS_H  TRUE)
  Set(FOG_HAVE_STDARG_H  TRUE)
  Set(FOG_HAVE_STDINT_H  TRUE)
  Set(FOG_HAVE_UNISTD_H  TRUE)

  Set(FOG_LIBDL_EXISTS   TRUE)

  Set(FOG_CC_HAS_CPP0X   TRUE)
  Set(FOG_CC_HAS_WINLINE TRUE)
EndIf (FOG_OS_IOS)

# =============================================================================
# [C++ Compiler - MSC]
# =============================================================================

If(MSVC)
  Message("-- Detected Compiler - MSC")

  Set(FOG_CXX_FLAGS_RELEASE "/Oi /Oy /GF /GS- /GR-")
  Set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${FOG_CXX_FLAGS_RELEASE}")
  Set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${FOG_CXX_FLAGS_RELEASE}")
  Set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${FOG_CXX_FLAGS_RELEASE}")

  Set(FOG_CXX_FLAGS_OPTIMIZE "/Ot")
  Set(FOG_CXX_FLAGS_MMX "${FOG_CXX_FLAGS_OPTIMIZE} -DFOG_HARDCODE_MMX")
  Set(FOG_CXX_FLAGS_3DNOW "${FOG_CXX_FLAGS_OPTIMIZE}  -DFOG_HARDCODE_3DNOW")
  Set(FOG_CXX_FLAGS_SSE "${FOG_CXX_FLAGS_OPTIMIZE} -DFOG_HARDCODE_SSE /arch:SSE")
  Set(FOG_CXX_FLAGS_SSE2 "${FOG_CXX_FLAGS_OPTIMIZE} -DFOG_HARDCODE_SSE2 /arch:SSE2")
  Set(FOG_CXX_FLAGS_SSE3 "${FOG_CXX_FLAGS_OPTIMIZE} -DFOG_HARDCODE_SSE3 /arch:SSE2")
  Set(FOG_CXX_FLAGS_SSSE3 "${FOG_CXX_FLAGS_OPTIMIZE} -DFOG_HARDCODE_SSSE3 /arch:SSE2")

  # Enable multi-process compilation by default.
  If(MSVC80 OR MSVC90 OR MSVC10)
     Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  EndIf()
EndIf()

# =============================================================================
# [C++ Compiler - GCC]
# =============================================================================

If(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "g\\+\\+")
  Message("-- Detected Compiler - GNU")

  # Check for supported compiler flags.
  If(NOT FOG_CC_HAS_CPP0X)
    Check_CXX_Compiler_Flag("-std=c++0x" FOG_CC_HAS_CPP0X)
  EndIf()

  If(NOT FOG_CC_HAS_FLTO)
    Check_CXX_Compiler_Flag("-flto" FOG_CC_HAS_FLTO)
  EndIf()

  If(NOT FOG_CC_HAS_INLINE_FUNCTIONS)
    Check_CXX_Compiler_Flag("-finline-functions" FOG_CC_HAS_INLINE_FUNCTIONS)
  EndIf()

  If(NOT FOG_CC_HAS_FMERGE_ALL_CONSTANTS)
    Check_CXX_Compiler_Flag("-fmerge-all-constants" FOG_CC_HAS_FMERGE_ALL_CONSTANTS)
  EndIf()

  If(NOT FOG_CC_HAS_FNO_KEEP_STATIC_CONSTS)
    Check_CXX_Compiler_Flag("-fno-keep-static-consts" FOG_CC_HAS_FNO_KEEP_STATIC_CONSTS)
  EndIf()

  If(NOT FOG_CC_HAS_WINLINE)
    Check_CXX_Compiler_Flag("-Winline" FOG_CC_HAS_WINLINE)
  EndIf()

  If(NOT FOG_CC_HAS_FNO_ENUM_COMPARE)
    Check_CXX_Compiler_Flag("-Wno-enum-compare" FOG_CC_HAS_WNO_ENUM_COMPARE)
  EndIf()

  # Turn off exceptions (smaller code).
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

  # Hide all symbols by default. Exported symbols are declared by FOG_API.
  # When running iOS, the flags are set by default.
  If (NOT MINGW AND NOT FOG_OS_IOS)
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
  EndIf()

  # -std=c++0x
  If (${FOG_CC_HAS_CPP0X})
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
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

  # TODO: CMake - Disabled, not working properly!
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
  # are already optimized - there is nothing compiler can do. Another argument
  # to use -O2 is that -O3 produces larger binary without speed improvements.
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
  # Build type is set to "None", assuming that the project is generated for IDE
  # which supports multiple build options.
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
# [Architecture - Size of basic types and endianness]
# =============================================================================

# Standard checking procedure.
If (NOT DISABLE_COMPILER_CHECKS)
  Include(TestBigEndian)
  Test_Big_Endian(WORDS_BIGENDIAN)

  If(WORDS_BIGENDIAN)
    Set(FOG_BYTE_ORDER 0)
  Else()
    Set(FOG_BYTE_ORDER 1)
  EndIf()

  Check_Type_Size("void*" FOG_SIZEOF_VOID)
EndIf()

# =============================================================================
# [Architecture - Optimizations]
# =============================================================================

If (NOT FOG_OS_IOS)
  # Disable MMX/3dNow when building for Mac or AMD64.
  If (NOT FOG_OS_MAC AND ${FOG_SIZEOF_VOID} EQUAL 4)
    Set(FOG_OPTIMIZE_MMX TRUE)
    # Set(FOG_OPTIMIZE_3DNOW TRUE)
  EndIf()

  Set(FOG_OPTIMIZE_SSE TRUE)
  Set(FOG_OPTIMIZE_SSE2 TRUE)
  Set(FOG_OPTIMIZE_SSSE3 TRUE)
EndIf()

Macro(FogAddOptimizedSources dst optimization)
  Set(local_condition "FOG_OPTIMIZE_${optimization}")
  Set(local_cflags "FOG_CXX_FLAGS_${optimization}")

  If(${local_condition})
    Set(local_files "")
    ForEach(file ${ARGN})
      Message("   Added optimized file ${file} (${optimization})")
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

If(UNIX AND NOT FOG_HAVE_UNISTD_H)
  Check_Include_Files(unistd.h FOG_HAVE_UNISTD_H)
EndIf()

If(UNIX AND NOT FOG_HAVE_FENV_H)
  Check_Include_Files(fenv.h FOG_HAVE_FENV_H)
EndIf()

# =============================================================================
# [OS - Libraries]
# =============================================================================

If(FOG_OS_WINDOWS)
  List(APPEND FOG_LIBRARIES advapi32 gdi32 ole32 user32 uuid shell32 winmm)
EndIf()

If(FOG_OS_IOS)
  List(APPEND FOG_LIBRARIES "-framework CoreGraphics")
  List(APPEND FOG_LIBRARIES "-framework CoreText")
  List(APPEND FOG_LIBRARIES "-framework UIKit")
ElseIf(FOG_OS_MAC)
  List(APPEND FOG_LIBRARIES "-framework Cocoa")
EndIf()

# librt is Linux specific library (not available in BSD/MAC).
If(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  List(APPEND FOG_LIBRARIES "-lrt")
EndIf()

If(FOG_OS_POSIX)
  # libdl for dynamic-library opening.
  If (NOT FOG_HAVE_DLFCN_H)
    Check_Include_Files(dlfcn.h FOG_HAVE_DLFCN_H)
  EndIf()

  If(FOG_HAVE_DLFCN_H)
    If (NOT FOG_LIBDL_EXISTS)
      Check_Library_Exists(dl dlopen "" FOG_LIBDL_EXISTS)
    EndIf()
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
  Src/Fog/Core.h
)

# [Fog/Core/Acc]
Set(FOG_CORE_ACC_SOURCES
  Src/Fog/Core/Acc/Constants.cpp
)

Set(FOG_CORE_ACC_HEADERS
  Src/Fog/Core/Acc/Acc3dNow.h
  Src/Fog/Core/Acc/Acc3dNowExt.h
  Src/Fog/Core/Acc/AccC.h
  Src/Fog/Core/Acc/AccMmx.h
  Src/Fog/Core/Acc/AccMmxExt.h
  Src/Fog/Core/Acc/AccMmxSse.h
  Src/Fog/Core/Acc/AccMmxSse2.h
  Src/Fog/Core/Acc/AccMmxSsse3.h
  Src/Fog/Core/Acc/AccSse.h
  Src/Fog/Core/Acc/AccSse2.h
  Src/Fog/Core/Acc/AccSse3.h
  Src/Fog/Core/Acc/AccSsse3.h
  Src/Fog/Core/Acc/Constants.h
)


# [Fog/Core/C++]
Set(FOG_CORE_CPP_SOURCES
)

Set(FOG_CORE_CPP_HEADERS
  Src/Fog/Core/C++/Base.h
  Src/Fog/Core/C++/Compiler.h
  Src/Fog/Core/C++/CompilerBorland.h
  Src/Fog/Core/C++/CompilerClang.h
  Src/Fog/Core/C++/CompilerGnu.h
  Src/Fog/Core/C++/CompilerIde.h
  Src/Fog/Core/C++/CompilerMsc.h
  Src/Fog/Core/C++/ConfigCMake.h
  Src/Fog/Core/C++/Intrin3dNow.h
  Src/Fog/Core/C++/IntrinMmx.h
  Src/Fog/Core/C++/IntrinMmxExt.h
  Src/Fog/Core/C++/IntrinSse.h
  Src/Fog/Core/C++/IntrinSse2.h
  Src/Fog/Core/C++/IntrinSse3.h
  Src/Fog/Core/C++/IntrinSsse3.h
  Src/Fog/Core/C++/PackByte.h
  Src/Fog/Core/C++/PackWord.h
  Src/Fog/Core/C++/PackDWord.h
  Src/Fog/Core/C++/PackQWord.h
  Src/Fog/Core/C++/PackRestore.h
  Src/Fog/Core/C++/StdArch.h
  Src/Fog/Core/C++/StdDefs.h
  Src/Fog/Core/C++/StdHeaders.h
  Src/Fog/Core/C++/StdInt.h
  Src/Fog/Core/C++/StdNew.h
  Src/Fog/Core/C++/StdOS.h
)

# [Fog/Core/Dom]
Set(FOG_CORE_DOM_SOURCES
  Src/Fog/Core/Dom/Dom.cpp
  Src/Fog/Core/Dom/DomResourceManager.cpp
)

Set(FOG_CORE_DOM_HEADERS
  Src/Fog/Core/Dom/Dom.h
  Src/Fog/Core/Dom/DomResourceManager.h
)

# [Fog/Core/Global]
Set(FOG_CORE_GLOBAL_SOURCES
  Src/Fog/Core/Global/Api.cpp
  Src/Fog/Core/Global/Assert.cpp
  Src/Fog/Core/Global/BuildInfo.cpp
  Src/Fog/Core/Global/Init.cpp
)

Set(FOG_CORE_GLOBAL_HEADERS
  Src/Fog/Core/Global/Api.h
  Src/Fog/Core/Global/Assert.h
  Src/Fog/Core/Global/BuildInfo.h
  Src/Fog/Core/Global/Class.h
  Src/Fog/Core/Global/Enum.h
  Src/Fog/Core/Global/EnumCore.h
  Src/Fog/Core/Global/EnumEvent.h
  Src/Fog/Core/Global/EnumError.h
  Src/Fog/Core/Global/EnumG2d.h
  Src/Fog/Core/Global/EnumUI.h
  Src/Fog/Core/Global/Global.h
  Src/Fog/Core/Global/Init.h
  Src/Fog/Core/Global/Init_p.h
  Src/Fog/Core/Global/Main.h
  Src/Fog/Core/Global/Private.h
  Src/Fog/Core/Global/Static.h
  Src/Fog/Core/Global/TypeDefs.h
  Src/Fog/Core/Global/TypeInfo.h
  Src/Fog/Core/Global/Types.h
  Src/Fog/Core/Global/Uninitialized.h
)

# [Fog/Core/Kernel]
Set(FOG_CORE_KERNEL_SOURCES
  Src/Fog/Core/Kernel/Application.cpp
  Src/Fog/Core/Kernel/CoreObj.cpp
  Src/Fog/Core/Kernel/DefaultEventLoopImpl.cpp
  Src/Fog/Core/Kernel/Event.cpp
  Src/Fog/Core/Kernel/EventLoop.cpp
  Src/Fog/Core/Kernel/EventLoopImpl.cpp
  Src/Fog/Core/Kernel/EventLoopObserver.cpp
  Src/Fog/Core/Kernel/EventLoopObserverList.cpp
  Src/Fog/Core/Kernel/Object.cpp
  Src/Fog/Core/Kernel/Task.cpp
  Src/Fog/Core/Kernel/Timer.cpp
)

Set(FOG_CORE_KERNEL_HEADERS
  Src/Fog/Core/Kernel/Application.h
  Src/Fog/Core/Kernel/CoreObj.h
  Src/Fog/Core/Kernel/DefaultEventLoopImpl_p.h
  Src/Fog/Core/Kernel/Delegate.h
  Src/Fog/Core/Kernel/Event.h
  Src/Fog/Core/Kernel/EventLoop.h
  Src/Fog/Core/Kernel/EventLoopImpl.h
  Src/Fog/Core/Kernel/EventLoopObserver.h
  Src/Fog/Core/Kernel/EventLoopObserverList.h
  Src/Fog/Core/Kernel/Object.h
  Src/Fog/Core/Kernel/Property.h
  Src/Fog/Core/Kernel/Task.h
  Src/Fog/Core/Kernel/Timer.h
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_CORE_KERNEL_SOURCES
    Src/Fog/Core/Kernel/WinEventLoopImpl.cpp
  )
  List(APPEND FOG_CORE_KERNEL_HEADERS
    Src/Fog/Core/Kernel/WinEventLoopImpl.h
  )
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_CORE_KERNEL_SOURCES
    Src/Fog/Core/Kernel/MacEventLoopImpl.mm
  )
  List(APPEND FOG_CORE_KERNEL_HEADERS
    Src/Fog/Core/Kernel/MacEventLoopImpl.h
  )
EndIf()

# [Fog/Core/Math]
Set(FOG_CORE_MATH_SOURCES
  Src/Fog/Core/Math/Function.cpp
  Src/Fog/Core/Math/Fuzzy.cpp
  Src/Fog/Core/Math/Integrate.cpp
  Src/Fog/Core/Math/Math.cpp
  Src/Fog/Core/Math/MathVec.cpp
  Src/Fog/Core/Math/Solve.cpp
  Src/Fog/Core/Math/Solve_Polynomial_Analytic.cpp
  Src/Fog/Core/Math/Solve_Polynomial_Eigen.cpp
  Src/Fog/Core/Math/Solve_Polynomial_JenkinsTraub.cpp
)

Set(FOG_CORE_MATH_HEADERS
  Src/Fog/Core/Math/Cleanup.h
  Src/Fog/Core/Math/Constants.h
  Src/Fog/Core/Math/FloatControl.h
  Src/Fog/Core/Math/Function.h
  Src/Fog/Core/Math/Fuzzy.h
  Src/Fog/Core/Math/Integrate.h
  Src/Fog/Core/Math/Interval.h
  Src/Fog/Core/Math/Math.h
  Src/Fog/Core/Math/MathVec.h
  Src/Fog/Core/Math/Solve.h
)

# [Fog/Core/Memory]
Set(FOG_CORE_MEMORY_SOURCES
  Src/Fog/Core/Memory/MemDebug.cpp
  Src/Fog/Core/Memory/MemBlockAllocator.cpp
  Src/Fog/Core/Memory/MemGCAllocator.cpp
  Src/Fog/Core/Memory/MemMgr.cpp
  Src/Fog/Core/Memory/MemOps.cpp
  Src/Fog/Core/Memory/MemPool.cpp
  Src/Fog/Core/Memory/MemZoneAllocator.cpp
)

Set(FOG_CORE_MEMORY_HEADERS
  Src/Fog/Core/Memory/BSwap.h
  Src/Fog/Core/Memory/MemBlockAllocator.h
  Src/Fog/Core/Memory/MemBuffer.h
  Src/Fog/Core/Memory/MemBufferTmp_p.h
  Src/Fog/Core/Memory/MemDebug_p.h
  Src/Fog/Core/Memory/MemGCAllocator.h
  Src/Fog/Core/Memory/MemMgr.h
  Src/Fog/Core/Memory/MemOps.h
  Src/Fog/Core/Memory/MemPool.h
  Src/Fog/Core/Memory/MemZoneAllocator.h
)

# [Fog/Core/OS]
Set(FOG_CORE_OS_SOURCES
  Src/Fog/Core/OS/DirIterator.cpp
  Src/Fog/Core/OS/Environment.cpp
  Src/Fog/Core/OS/FileInfo.cpp
  Src/Fog/Core/OS/FileMapping.cpp
  Src/Fog/Core/OS/FilePath.cpp
  Src/Fog/Core/OS/FileUtil.cpp
  Src/Fog/Core/OS/Library.cpp
  Src/Fog/Core/OS/OSInfo.cpp
  Src/Fog/Core/OS/OSUtil.cpp
  Src/Fog/Core/OS/UserUtil.cpp
)

Set(FOG_CORE_OS_HEADERS
  Src/Fog/Core/OS/DirIterator.h
  Src/Fog/Core/OS/Environment.h
  Src/Fog/Core/OS/FileInfo.h
  Src/Fog/Core/OS/FileMapping.h
  Src/Fog/Core/OS/FilePath.h
  Src/Fog/Core/OS/FileUtil.h
  Src/Fog/Core/OS/Library.h
  Src/Fog/Core/OS/OSInfo.h
  Src/Fog/Core/OS/OSUtil.h
  Src/Fog/Core/OS/UserUtil.h
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_CORE_OS_SOURCES
    Src/Fog/Core/OS/WinCom.cpp
    Src/Fog/Core/OS/WinUtil.cpp
  )
  List(APPEND FOG_CORE_OS_HEADERS
    Src/Fog/Core/OS/WinCom.h
    Src/Fog/Core/OS/WinUtil.h
  )
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_CORE_OS_SOURCES
    Src/Fog/Core/OS/MacScopedAutoReleasePool.mm
    Src/Fog/Core/OS/MacUtil.mm
  )
  List(APPEND FOG_CORE_OS_HEADERS
    Src/Fog/Core/OS/MacDefs.h
    Src/Fog/Core/OS/MacScopedAutoReleasePool.h
    Src/Fog/Core/OS/MacUtil.h
  )
EndIf()

# [Fog/Core/Threading]
Set(FOG_CORE_THREADING_SOURCES
  Src/Fog/Core/Threading/Lock.cpp
  Src/Fog/Core/Threading/Thread.cpp
  Src/Fog/Core/Threading/ThreadCondition.cpp
  Src/Fog/Core/Threading/ThreadEvent.cpp
  Src/Fog/Core/Threading/ThreadLocal.cpp
  Src/Fog/Core/Threading/ThreadPool.cpp
)

Set(FOG_CORE_THREADING_HEADERS
  Src/Fog/Core/Threading/Atomic.h
  Src/Fog/Core/Threading/Atomic_gcc_intrin.h
  Src/Fog/Core/Threading/Atomic_gcc_x86x64.h
  Src/Fog/Core/Threading/Atomic_msc_intrin.h
  Src/Fog/Core/Threading/Atomic_msc_x86.h
  Src/Fog/Core/Threading/AtomicPadding.h
  Src/Fog/Core/Threading/Lock.h
  Src/Fog/Core/Threading/Thread.h
  Src/Fog/Core/Threading/ThreadCondition.h
  Src/Fog/Core/Threading/ThreadEvent.h
  Src/Fog/Core/Threading/ThreadLocal.h
  Src/Fog/Core/Threading/ThreadPool.h
)

# [Fog/Core/Tools]
Set(FOG_CORE_TOOLS_SOURCES
  Src/Fog/Core/Tools/Algorithm.cpp
  Src/Fog/Core/Tools/Char.cpp
  Src/Fog/Core/Tools/CharData.cpp
  Src/Fog/Core/Tools/CharUtil.cpp
  Src/Fog/Core/Tools/ContainerUtil.cpp
  Src/Fog/Core/Tools/Cpu.cpp
  Src/Fog/Core/Tools/Date.cpp
  Src/Fog/Core/Tools/Hash.cpp
  Src/Fog/Core/Tools/HashUtil.cpp
  Src/Fog/Core/Tools/InternedString.cpp
  Src/Fog/Core/Tools/List.cpp
  Src/Fog/Core/Tools/Locale.cpp
  Src/Fog/Core/Tools/Logger.cpp
  Src/Fog/Core/Tools/Random.cpp
  Src/Fog/Core/Tools/RegExp.cpp
  Src/Fog/Core/Tools/Stream.cpp
  Src/Fog/Core/Tools/String.cpp
  Src/Fog/Core/Tools/StringUtil.cpp
  Src/Fog/Core/Tools/StringUtil_dtoa.cpp
  Src/Fog/Core/Tools/Stub.cpp
  Src/Fog/Core/Tools/TextChunk.cpp
  Src/Fog/Core/Tools/TextCodec.cpp
  Src/Fog/Core/Tools/TextIterator.cpp
  Src/Fog/Core/Tools/Time.cpp
  Src/Fog/Core/Tools/Var.cpp
  Src/Fog/Core/Tools/XmlEntity.cpp
  Src/Fog/Core/Tools/XmlIO.cpp
)

Set(FOG_CORE_TOOLS_HEADERS
  Src/Fog/Core/Tools/Algorithm.h
  Src/Fog/Core/Tools/Char.h
  Src/Fog/Core/Tools/CharData.h
  Src/Fog/Core/Tools/CharUtil.h
  Src/Fog/Core/Tools/CharUtilPrivate_p.h
  Src/Fog/Core/Tools/ContainerUtil.h
  Src/Fog/Core/Tools/Cpu.h
  Src/Fog/Core/Tools/Date.h
  Src/Fog/Core/Tools/Hash.h
  Src/Fog/Core/Tools/HashString.h
  Src/Fog/Core/Tools/HashUInt.h
  Src/Fog/Core/Tools/HashUntyped.h
  Src/Fog/Core/Tools/HashUtil.h
  Src/Fog/Core/Tools/InternedString.h
  Src/Fog/Core/Tools/List.h
  Src/Fog/Core/Tools/ListReal.h
  Src/Fog/Core/Tools/ListString.h
  Src/Fog/Core/Tools/ListUntyped.h
  Src/Fog/Core/Tools/ListVar.h
  Src/Fog/Core/Tools/Locale.h
  Src/Fog/Core/Tools/Logger.h
  Src/Fog/Core/Tools/Random.h
  Src/Fog/Core/Tools/Range.h
  Src/Fog/Core/Tools/RegExp.h
  Src/Fog/Core/Tools/Stream.h
  Src/Fog/Core/Tools/String.h
  Src/Fog/Core/Tools/StringTmp_p.h
  Src/Fog/Core/Tools/StringUtil.h
  Src/Fog/Core/Tools/StringUtil_dtoa_p.h
  Src/Fog/Core/Tools/Stub.h
  Src/Fog/Core/Tools/Swap.h
  Src/Fog/Core/Tools/TextChunk.h
  Src/Fog/Core/Tools/TextCodec.h
  Src/Fog/Core/Tools/TextCodec_Page8Tables_p.h
  Src/Fog/Core/Tools/TextIterator.h
  Src/Fog/Core/Tools/Time.h
  Src/Fog/Core/Tools/Var.h
  Src/Fog/Core/Tools/VarId.h
  Src/Fog/Core/Tools/XmlEntity_p.h
  Src/Fog/Core/Tools/XmlIO.h
)

FogAddOptimizedSources(FOG_CORE_TOOLS_SOURCES SSE2
  Src/Fog/Core/Tools/List_SSE2.cpp
)

# Source groups.
FogAddSourceGroup("Fog/Core"                 ${FOG_CORE_LIB_SOURCES}             ${FOG_CORE_LIB_HEADERS}            )
FogAddSourceGroup("Fog/Core/Acc"             ${FOG_CORE_ACC_SOURCES}             ${FOG_CORE_ACC_HEADERS}            )
FogAddSourceGroup("Fog/Core/C++"             ${FOG_CORE_CPP_SOURCES}             ${FOG_CORE_CPP_HEADERS}            )
FogAddSourceGroup("Fog/Core/Dom"             ${FOG_CORE_DOM_SOURCES}             ${FOG_CORE_DOM_HEADERS}            )
FogAddSourceGroup("Fog/Core/Global"          ${FOG_CORE_GLOBAL_SOURCES}          ${FOG_CORE_GLOBAL_HEADERS}         )
FogAddSourceGroup("Fog/Core/Kernel"          ${FOG_CORE_KERNEL_SOURCES}          ${FOG_CORE_KERNEL_HEADERS}         )
FogAddSourceGroup("Fog/Core/Math"            ${FOG_CORE_MATH_SOURCES}            ${FOG_CORE_MATH_HEADERS}           )
FogAddSourceGroup("Fog/Core/Memory"          ${FOG_CORE_MEMORY_SOURCES}          ${FOG_CORE_MEMORY_HEADERS}         )
FogAddSourceGroup("Fog/Core/OS"              ${FOG_CORE_OS_SOURCES}              ${FOG_CORE_OS_HEADERS}             )
FogAddSourceGroup("Fog/Core/Threading"       ${FOG_CORE_THREADING_SOURCES}       ${FOG_CORE_THREADING_HEADERS}      )
FogAddSourceGroup("Fog/Core/Tools"           ${FOG_CORE_TOOLS_SOURCES}           ${FOG_CORE_TOOLS_HEADERS}          )

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
  Src/Fog/G2d.h
)

# [Fog/G2d/Acc]
Set(FOG_G2D_ACC_SOURCES
)

Set(FOG_G2D_ACC_HEADERS
  Src/Fog/G2d/Acc/AccC.h
  Src/Fog/G2d/Acc/AccMmx.h
  Src/Fog/G2d/Acc/AccMmxExt.h
  Src/Fog/G2d/Acc/AccSse.h
  Src/Fog/G2d/Acc/AccSse2.h
  Src/Fog/G2d/Acc/AccSse3.h
  Src/Fog/G2d/Acc/AccSsse3.h
)

# [Fog/G2d/Geometry]
Set(FOG_G2D_GEOMETRY_SOURCES
  Src/Fog/G2d/Geometry/Arc.cpp
  Src/Fog/G2d/Geometry/CBezier.cpp
  Src/Fog/G2d/Geometry/Chord.cpp
  Src/Fog/G2d/Geometry/Circle.cpp
  Src/Fog/G2d/Geometry/Ellipse.cpp
  Src/Fog/G2d/Geometry/Line.cpp
  Src/Fog/G2d/Geometry/Math2d.cpp
  Src/Fog/G2d/Geometry/Path.cpp
  Src/Fog/G2d/Geometry/PathClipper.cpp
  Src/Fog/G2d/Geometry/PathEffect.cpp
  Src/Fog/G2d/Geometry/PathInfo.cpp
  Src/Fog/G2d/Geometry/PathOnPath.cpp
  Src/Fog/G2d/Geometry/PathStroker.cpp
  Src/Fog/G2d/Geometry/Pie.cpp
  Src/Fog/G2d/Geometry/Point.cpp
  Src/Fog/G2d/Geometry/Polygon.cpp
  Src/Fog/G2d/Geometry/QBezier.cpp
  Src/Fog/G2d/Geometry/Round.cpp
  Src/Fog/G2d/Geometry/Shape.cpp
  Src/Fog/G2d/Geometry/Size.cpp
  Src/Fog/G2d/Geometry/Transform.cpp
  Src/Fog/G2d/Geometry/Triangle.cpp
)

Set(FOG_G2D_GEOMETRY_HEADERS
  Src/Fog/G2d/Geometry/Arc.h
  Src/Fog/G2d/Geometry/Box.h
  Src/Fog/G2d/Geometry/CBezier.h
  Src/Fog/G2d/Geometry/Chord.h
  Src/Fog/G2d/Geometry/Circle.h
  Src/Fog/G2d/Geometry/Coord.h
  Src/Fog/G2d/Geometry/Ellipse.h
  Src/Fog/G2d/Geometry/Internals_p.h
  Src/Fog/G2d/Geometry/Line.h
  Src/Fog/G2d/Geometry/Math2d.h
  Src/Fog/G2d/Geometry/Path.h
  Src/Fog/G2d/Geometry/PathClipper.h
  Src/Fog/G2d/Geometry/PathEffect.h
  Src/Fog/G2d/Geometry/PathInfo.h
  Src/Fog/G2d/Geometry/PathOnPath.h
  Src/Fog/G2d/Geometry/PathStroker.h
  Src/Fog/G2d/Geometry/PathTmp_p.h
  Src/Fog/G2d/Geometry/Pie.h
  Src/Fog/G2d/Geometry/Point.h
  Src/Fog/G2d/Geometry/PointArray.h
  Src/Fog/G2d/Geometry/Polygon.h
  Src/Fog/G2d/Geometry/PolygonTmp_p.h
  Src/Fog/G2d/Geometry/QBezier.h
  Src/Fog/G2d/Geometry/Rect.h
  Src/Fog/G2d/Geometry/RectArray.h
  Src/Fog/G2d/Geometry/Round.h
  Src/Fog/G2d/Geometry/Shape.h
  Src/Fog/G2d/Geometry/Size.h
  Src/Fog/G2d/Geometry/Thickness.h
  Src/Fog/G2d/Geometry/Transform.h
  Src/Fog/G2d/Geometry/Triangle.h
)

FogAddOptimizedSources(FOG_G2D_GEOMETRY_SOURCES 3DNOW
  Src/Fog/G2d/Geometry/Transform_3dNow.cpp
)

FogAddOptimizedSources(FOG_G2D_GEOMETRY_SOURCES SSE
  Src/Fog/G2d/Geometry/PathClipper_SSE.cpp
  Src/Fog/G2d/Geometry/Transform_SSE.cpp
)

FogAddOptimizedSources(FOG_G2D_GEOMETRY_SOURCES SSE2
  Src/Fog/G2d/Geometry/PathClipper_SSE2.cpp
  Src/Fog/G2d/Geometry/Transform_SSE2.cpp
)

# [Fog/G2d/Imaging]
Set(FOG_G2D_IMAGING_SOURCES
  Src/Fog/G2d/Imaging/Image.cpp
  Src/Fog/G2d/Imaging/ImageCodec.cpp
  Src/Fog/G2d/Imaging/ImageCodecProvider.cpp
  Src/Fog/G2d/Imaging/ImageConverter.cpp
  Src/Fog/G2d/Imaging/ImageDecoder.cpp
  Src/Fog/G2d/Imaging/ImageEncoder.cpp
  Src/Fog/G2d/Imaging/ImageFilter.cpp
  Src/Fog/G2d/Imaging/ImageFormatDescription.cpp
  Src/Fog/G2d/Imaging/ImagePalette.cpp
  Src/Fog/G2d/Imaging/ImageResize.cpp
)

Set(FOG_G2D_IMAGING_HEADERS
  Src/Fog/G2d/Imaging/Image.h
  Src/Fog/G2d/Imaging/ImageBits.h
  Src/Fog/G2d/Imaging/ImageCodec.h
  Src/Fog/G2d/Imaging/ImageCodecProvider.h
  Src/Fog/G2d/Imaging/ImageConverter.h
  Src/Fog/G2d/Imaging/ImageDecoder.h
  Src/Fog/G2d/Imaging/ImageEncoder.h
  Src/Fog/G2d/Imaging/ImageFilter.h
  Src/Fog/G2d/Imaging/ImageFilterScale.h
  Src/Fog/G2d/Imaging/ImageFormatDescription.h
  Src/Fog/G2d/Imaging/ImagePalette.h
  Src/Fog/G2d/Imaging/ImageResize_p.h
)

Set_Source_Files_Properties(
  Src/Fog/G2d/Imaging/Image.cpp
  Src/Fog/G2d/Imaging/ImageResize.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CXX_FLAGS_OPTIMIZE}"
)

FogAddOptimizedSources(FOG_G2D_IMAGING_SOURCES SSE2
  Src/Fog/G2d/Imaging/ImageResize_SSE2.cpp
)

Set(FOG_G2D_IMAGING_CODECS_SOURCES
  Src/Fog/G2d/Imaging/Codecs/BmpCodec.cpp
  Src/Fog/G2d/Imaging/Codecs/GifCodec.cpp
  Src/Fog/G2d/Imaging/Codecs/IcoCodec.cpp
  Src/Fog/G2d/Imaging/Codecs/JpegCodec.cpp
  Src/Fog/G2d/Imaging/Codecs/MacCGCodec.cpp
  Src/Fog/G2d/Imaging/Codecs/PcxCodec.cpp
  Src/Fog/G2d/Imaging/Codecs/PngCodec.cpp
  Src/Fog/G2d/Imaging/Codecs/WinGdipCodec.cpp
)

Set(FOG_G2D_IMAGING_CODECS_HEADERS
  Src/Fog/G2d/Imaging/Codecs/BmpCodec_p.h
  Src/Fog/G2d/Imaging/Codecs/IcoCodec_p.h
  Src/Fog/G2d/Imaging/Codecs/JpegCodec_p.h
  Src/Fog/G2d/Imaging/Codecs/MacCGCodec_p.h
  Src/Fog/G2d/Imaging/Codecs/PcxCodec_p.h
  Src/Fog/G2d/Imaging/Codecs/PngCodec_p.h
  Src/Fog/G2d/Imaging/Codecs/WinGdipCodec_p.h
)

Set(FOG_G2D_IMAGING_FILTERS_SOURCES
  Src/Fog/G2d/Imaging/Filters/FeBlur.cpp
  Src/Fog/G2d/Imaging/Filters/FeColorLut.cpp
  Src/Fog/G2d/Imaging/Filters/FeColorLutArray.cpp
  Src/Fog/G2d/Imaging/Filters/FeColorMatrix.cpp
  Src/Fog/G2d/Imaging/Filters/FeComponentFunction.cpp
  Src/Fog/G2d/Imaging/Filters/FeComponentTransfer.cpp
  Src/Fog/G2d/Imaging/Filters/FeCompositingFunction.cpp
  Src/Fog/G2d/Imaging/Filters/FeConvolveMatrix.cpp
  Src/Fog/G2d/Imaging/Filters/FeConvolveSeparable.cpp
  Src/Fog/G2d/Imaging/Filters/FeMorphology.cpp
  Src/Fog/G2d/Imaging/Filters/FeTurbulence.cpp
)

Set(FOG_G2D_IMAGING_FILTERS_HEADERS
  Src/Fog/G2d/Imaging/Filters/FeBorder.h
  Src/Fog/G2d/Imaging/Filters/FeBase.h
  Src/Fog/G2d/Imaging/Filters/FeBlur.h
  Src/Fog/G2d/Imaging/Filters/FeColorLut.h
  Src/Fog/G2d/Imaging/Filters/FeColorLutArray.h
  Src/Fog/G2d/Imaging/Filters/FeColorMatrix.h
  Src/Fog/G2d/Imaging/Filters/FeComponentFunction.h
  Src/Fog/G2d/Imaging/Filters/FeComponentTransfer.h
  Src/Fog/G2d/Imaging/Filters/FeCompositingFunction.h
  Src/Fog/G2d/Imaging/Filters/FeConvolveMatrix.h
  Src/Fog/G2d/Imaging/Filters/FeConvolveSeparable.h
  Src/Fog/G2d/Imaging/Filters/FeMorphology.h
  Src/Fog/G2d/Imaging/Filters/FeTurbulence.h
)

FogAddOptimizedSources(FOG_G2D_IMAGING_FILTERS_SOURCES SSE
  Src/Fog/G2d/Imaging/Filters/FeColorMatrix_SSE.cpp
)

If(NOT FOG_HAVE_LIBJPEG)
  Check_Include_Files(jconfig.h FOG_HAVE_LIBJPEG)
EndIf()

If(NOT FOG_HAVE_LIBPNG)
  Check_Include_Files(png.h FOG_HAVE_LIBPNG)
EndIf()

# [Fog/G2d/OS]
Set(FOG_G2D_OS_SOURCES
)

Set(FOG_G2D_OS_HEADERS
  Src/Fog/G2d/OS/OSUtil.h
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_G2D_OS_SOURCES
    Src/Fog/G2d/OS/WinGdiPlus.cpp
    Src/Fog/G2d/OS/WinImage.cpp
    Src/Fog/G2d/OS/WinRegion.cpp
    Src/Fog/G2d/OS/WinUtil.cpp
    Src/Fog/G2d/OS/WinUxTheme.cpp
  )
  List(APPEND FOG_G2D_OS_HEADERS
    Src/Fog/G2d/OS/WinGdiPlus.h
    Src/Fog/G2d/OS/WinUtil.h
    Src/Fog/G2d/OS/WinUxTheme.h
  )
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_G2D_OS_SOURCES
    Src/Fog/G2d/OS/MacImage.mm
    Src/Fog/G2d/OS/MacUtil.mm
  )
  List(APPEND FOG_G2D_OS_HEADERS
    Src/Fog/G2d/OS/MacUtil.h
  )
EndIf()

# [Fog/G2d/Painting]
Set(FOG_G2D_PAINTING_SOURCES
  Src/Fog/G2d/Painting/NullPaintEngine.cpp
  Src/Fog/G2d/Painting/PaintDevice.cpp
  Src/Fog/G2d/Painting/PaintDeviceInfo.cpp
  Src/Fog/G2d/Painting/PaintEngine.cpp
  Src/Fog/G2d/Painting/PaintParams.cpp
  Src/Fog/G2d/Painting/PaintUtil.cpp
  Src/Fog/G2d/Painting/Painter.cpp
  Src/Fog/G2d/Painting/RasterApi.cpp
  Src/Fog/G2d/Painting/RasterConstants.cpp
  Src/Fog/G2d/Painting/RasterInit.cpp
  Src/Fog/G2d/Painting/RasterInit_C.cpp
  Src/Fog/G2d/Painting/RasterPaintContext.cpp
  Src/Fog/G2d/Painting/RasterPaintEngine.cpp
  Src/Fog/G2d/Painting/RasterPaintEngineDoGroup.cpp
  Src/Fog/G2d/Painting/RasterPaintEngineDoRender.cpp
  Src/Fog/G2d/Painting/RasterScanline.cpp
  Src/Fog/G2d/Painting/Rasterizer.cpp
)

Set(FOG_G2D_PAINTING_HEADERS
  Src/Fog/G2d/Painting/NullPaintEngine_p.h
  Src/Fog/G2d/Painting/PaintDevice.h
  Src/Fog/G2d/Painting/PaintDeviceInfo.h
  Src/Fog/G2d/Painting/PaintEngine.h
  Src/Fog/G2d/Painting/PaintParams.h
  Src/Fog/G2d/Painting/PaintUtil.h
  Src/Fog/G2d/Painting/Painter.h
  Src/Fog/G2d/Painting/RasterApi_p.h
  Src/Fog/G2d/Painting/RasterConstants_p.h
  Src/Fog/G2d/Painting/RasterInit_p.h
  Src/Fog/G2d/Painting/RasterPaintCmd_p.h
  Src/Fog/G2d/Painting/RasterPaintContext_p.h
  Src/Fog/G2d/Painting/RasterPaintEngine_p.h
  Src/Fog/G2d/Painting/RasterPaintStructs_p.h
  Src/Fog/G2d/Painting/RasterScanline_p.h
  Src/Fog/G2d/Painting/RasterSpan_p.h
  Src/Fog/G2d/Painting/RasterStructs_p.h
  Src/Fog/G2d/Painting/RasterUtil_p.h
  Src/Fog/G2d/Painting/Rasterizer_p.h
)

Set_Source_Files_Properties(
  Src/Fog/G2d/Painting/Rasterizer.cpp
  PROPERTIES COMPILE_FLAGS "${FOG_CXX_FLAGS_OPTIMIZE}"
)

FogAddOptimizedSources(FOG_G2D_PAINTING_SOURCES SSE2
  Src/Fog/G2d/Painting/RasterPaintEngine_SSE2.cpp
)

# [Fog/G2d/Painting/RasterOps_C]
Set(FOG_G2D_PAINTING_RASTEROPS_C_SOURCES
)

Set(FOG_G2D_PAINTING_RASTEROPS_C_HEADERS
  Src/Fog/G2d/Painting/RasterOps_C/BaseAccess_p.h
  Src/Fog/G2d/Painting/RasterOps_C/BaseConvert_p.h
  Src/Fog/G2d/Painting/RasterOps_C/BaseDefs_p.h
  Src/Fog/G2d/Painting/RasterOps_C/BaseHelpers_p.h
  Src/Fog/G2d/Painting/RasterOps_C/CompositeBase_p.h
  Src/Fog/G2d/Painting/RasterOps_C/CompositeClear_p.h
  Src/Fog/G2d/Painting/RasterOps_C/CompositeExt_p.h
  Src/Fog/G2d/Painting/RasterOps_C/CompositeFunc_p.h
  Src/Fog/G2d/Painting/RasterOps_C/CompositeNop_p.h
  Src/Fog/G2d/Painting/RasterOps_C/CompositeSrc_p.h
  Src/Fog/G2d/Painting/RasterOps_C/CompositeSrcOver_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterBase_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterBlur_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterColorLut_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterColorMatrix_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterComponentTransfer_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterConvolveMatrix_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterConvolveSeparable_p.h
  Src/Fog/G2d/Painting/RasterOps_C/FilterMorphology_p.h
  Src/Fog/G2d/Painting/RasterOps_C/GradientBase_p.h
  Src/Fog/G2d/Painting/RasterOps_C/GradientConical_p.h
  Src/Fog/G2d/Painting/RasterOps_C/GradientLinear_p.h
  Src/Fog/G2d/Painting/RasterOps_C/GradientRadial_p.h
  Src/Fog/G2d/Painting/RasterOps_C/GradientRectangular_p.h
  Src/Fog/G2d/Painting/RasterOps_C/TextureAffine_p.h
  Src/Fog/G2d/Painting/RasterOps_C/TextureBase_p.h
  Src/Fog/G2d/Painting/RasterOps_C/TextureProjection_p.h
  Src/Fog/G2d/Painting/RasterOps_C/TextureScale_p.h
  Src/Fog/G2d/Painting/RasterOps_C/TextureSimple_p.h
)

# [Fog/G2d/Source]
Set(FOG_G2D_SOURCE_SOURCES
  Src/Fog/G2d/Source/Color.cpp
  Src/Fog/G2d/Source/ColorBase.cpp
  Src/Fog/G2d/Source/ColorStop.cpp
  Src/Fog/G2d/Source/ColorStopCache.cpp
  Src/Fog/G2d/Source/ColorStopList.cpp
  Src/Fog/G2d/Source/ConicalGradient.cpp
  Src/Fog/G2d/Source/Gradient.cpp
  Src/Fog/G2d/Source/LinearGradient.cpp
  Src/Fog/G2d/Source/Pattern.cpp
  Src/Fog/G2d/Source/RadialGradient.cpp
  Src/Fog/G2d/Source/RectangularGradient.cpp
  Src/Fog/G2d/Source/Texture.cpp
)

FogAddOptimizedSources(FOG_G2D_SOURCE_SOURCES SSE2
  Src/Fog/G2d/Source/Color_SSE2.cpp
)

Set(FOG_G2D_SOURCE_HEADERS
  Src/Fog/G2d/Source/Acmyk.h
  Src/Fog/G2d/Source/Ahsl.h
  Src/Fog/G2d/Source/Ahsv.h
  Src/Fog/G2d/Source/Argb.h
  Src/Fog/G2d/Source/Color.h
  Src/Fog/G2d/Source/ColorBase.h
  Src/Fog/G2d/Source/ColorStop.h
  Src/Fog/G2d/Source/ColorStopCache.h
  Src/Fog/G2d/Source/ColorStopList.h
  Src/Fog/G2d/Source/ColorUtil.h
  Src/Fog/G2d/Source/ConicalGradient.h
  Src/Fog/G2d/Source/Gradient.h
  Src/Fog/G2d/Source/LinearGradient.h
  Src/Fog/G2d/Source/Pattern.h
  Src/Fog/G2d/Source/RadialGradient.h
  Src/Fog/G2d/Source/RectangularGradient.h
  Src/Fog/G2d/Source/Texture.h
)

# [Fog/G2d/Svg]
Set(FOG_G2D_SVG_SOURCES
  Src/Fog/G2d/Svg/SvgContext.cpp
  Src/Fog/G2d/Svg/SvgDom.cpp
  Src/Fog/G2d/Svg/SvgUtil.cpp
)

Set(FOG_G2D_SVG_HEADERS
  Src/Fog/G2d/Svg/SvgContext.h
  Src/Fog/G2d/Svg/SvgDom.h
  Src/Fog/G2d/Svg/SvgUtil.h
)

# [Fog/G2d/Text]
Set(FOG_G2D_TEXT_SOURCES
  Src/Fog/G2d/Text/Font.cpp
  Src/Fog/G2d/Text/TextDocument.cpp
  Src/Fog/G2d/Text/TextLayout.cpp
)

Set(FOG_G2D_TEXT_HEADERS
  Src/Fog/G2d/Text/Font.h
  Src/Fog/G2d/Text/TextDocument.h
  Src/Fog/G2d/Text/TextLayout.h
  Src/Fog/G2d/Text/TextRect.h
)

# [Fog/G2d/Text - Windows]
Set(FOG_G2D_TEXT_SOURCES_WIN
  Src/Fog/G2d/Text/WinFont.cpp
)

Set(FOG_G2D_TEXT_HEADERS_WIN
  Src/Fog/G2d/Text/WinFont.h
)

# [Fog/G2d/Text - Mac]
Set(FOG_G2D_TEXT_SOURCES_MAC
  Src/Fog/G2d/Text/MacFont.mm
)
Set(FOG_G2D_TEXT_HEADERS_MAC
  Src/Fog/G2d/Text/MacFont.h
)

# [Fog/G2d/Text - FreeType]
Set(FOG_G2D_TEXT_SOURCES_FREETYPE
  Src/Fog/G2d/Text/FTFont.cpp
)

Set(FOG_G2D_TEXT_HEADERS_FREETYPE
  Src/Fog/G2d/Text/FTFont.h
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

# [Fog/G2d/Text/OpenType]
Set(FOG_G2D_TEXT_OPENTYPE_SOURCES
  Src/Fog/G2d/Text/OpenType/OTApi.cpp
  Src/Fog/G2d/Text/OpenType/OTCMap.cpp
  Src/Fog/G2d/Text/OpenType/OTFace.cpp
  Src/Fog/G2d/Text/OpenType/OTTypes.cpp
)

Set(FOG_G2D_TEXT_OPENTYPE_HEADERS
  Src/Fog/G2d/Text/OpenType/OTApi.h
  Src/Fog/G2d/Text/OpenType/OTCMap.h
  Src/Fog/G2d/Text/OpenType/OTEnum.h
  Src/Fog/G2d/Text/OpenType/OTFace.h
  Src/Fog/G2d/Text/OpenType/OTTypes.h
)

# [Fog/G2d/Tools]
Set(FOG_G2D_TOOLS_SOURCES
  Src/Fog/G2d/Tools/ColorAnalyzer.cpp
  Src/Fog/G2d/Tools/DitherTable.cpp
  Src/Fog/G2d/Tools/Dpi.cpp
  Src/Fog/G2d/Tools/Matrix.cpp
  Src/Fog/G2d/Tools/Reduce.cpp
  Src/Fog/G2d/Tools/Region.cpp
  Src/Fog/G2d/Tools/RegionUtil.cpp
)

Set(FOG_G2D_TOOLS_HEADERS
  Src/Fog/G2d/Tools/ColorAnalyzer_p.h
  Src/Fog/G2d/Tools/DitherTable_p.h
  Src/Fog/G2d/Tools/Dpi.h
  Src/Fog/G2d/Tools/Matrix.h
  Src/Fog/G2d/Tools/Reduce_p.h
  Src/Fog/G2d/Tools/Region.h
  Src/Fog/G2d/Tools/RegionTmp_p.h
  Src/Fog/G2d/Tools/RegionUtil_p.h
)

FogAddOptimizedSources(FOG_G2D_TOOLS_SOURCES SSE2
  Src/Fog/G2d/Tools/Region_SSE2.cpp
)

# Source groups.
FogAddSourceGroup("Fog/G2d"                  ${FOG_G2D_LIB_SOURCES}              ${FOG_G2D_LIB_HEADERS}             )
FogAddSourceGroup("Fog/G2d/Acc"              ${FOG_G2D_ACC_SOURCES}              ${FOG_G2D_ACC_HEADERS}             )
FogAddSourceGroup("Fog/G2d/Geometry"         ${FOG_G2D_GEOMETRY_SOURCES}         ${FOG_G2D_GEOMETRY_HEADERS}        )
FogAddSourceGroup("Fog/G2d/Imaging"          ${FOG_G2D_IMAGING_SOURCES}          ${FOG_G2D_IMAGING_HEADERS}         )
FogAddSourceGroup("Fog/G2d/Imaging/Codecs"   ${FOG_G2D_IMAGING_CODECS_SOURCES}   ${FOG_G2D_IMAGING_CODECS_HEADERS}  )
FogAddSourceGroup("Fog/G2d/Imaging/Filters"  ${FOG_G2D_IMAGING_FILTERS_SOURCES}  ${FOG_G2D_IMAGING_FILTERS_HEADERS} )
FogAddSourceGroup("Fog/G2d/OS"               ${FOG_G2D_OS_SOURCES}               ${FOG_G2D_OS_HEADERS}              )
FogAddSourceGroup("Fog/G2d/Painting"         ${FOG_G2D_PAINTING_SOURCES}         ${FOG_G2D_PAINTING_HEADERS}        )
FogAddSourceGroup("Fog/G2d/Source"           ${FOG_G2D_SOURCE_SOURCES}           ${FOG_G2D_SOURCE_HEADERS}          )
FogAddSourceGroup("Fog/G2d/Svg"              ${FOG_G2D_SVG_SOURCES}              ${FOG_G2D_SVG_HEADERS}             )
FogAddSourceGroup("Fog/G2d/Text"             ${FOG_G2D_TEXT_SOURCES}             ${FOG_G2D_TEXT_HEADERS}            )
FogAddSourceGroup("Fog/G2d/Text/OpenType"    ${FOG_G2D_TEXT_OPENTYPE_SOURCES}    ${FOG_G2D_TEXT_OPENTYPE_HEADERS}   )
FogAddSourceGroup("Fog/G2d/Tools"            ${FOG_G2D_TOOLS_SOURCES}            ${FOG_G2D_TOOLS_HEADERS}           )

FogAddSourceGroup("Fog/G2d/Painting/RasterOps_C"
  ${FOG_G2D_PAINTING_RASTEROPS_C_SOURCES}
  ${FOG_G2D_PAINTING_RASTEROPS_C_HEADERS}
)

# =============================================================================
# [Fog/UI]
# =============================================================================

If(FOG_BUILD_UI)
Message("-- Configuring Fog/UI")

# =============================================================================
# [Fog/UI - Files]
# =============================================================================

# [Fog/UI]
Set(FOG_UI_LIB_SOURCES
)

Set(FOG_UI_LIB_HEADERS
  Src/Fog/UI.h
)

# [Fog/UI/Engine]
Set(FOG_UI_ENGINE_SOURCES
  Src/Fog/UI/Engine/UIEngine.cpp
  Src/Fog/UI/Engine/UIEngineEvent.cpp
  Src/Fog/UI/Engine/UIEngineWindow.cpp
)

Set(FOG_UI_ENGINE_HEADERS
  Src/Fog/UI/Engine/UIEngine.h
  Src/Fog/UI/Engine/UIEngineDisplayInfo.h
  Src/Fog/UI/Engine/UIEngineEvent.h
  Src/Fog/UI/Engine/UIEngineKeyboardInfo.h
  Src/Fog/UI/Engine/UIEngineKeyboardState.h
  Src/Fog/UI/Engine/UIEngineMouseInfo.h
  Src/Fog/UI/Engine/UIEngineMouseState.h
  Src/Fog/UI/Engine/UIEnginePaletteInfo.h
  Src/Fog/UI/Engine/UIEngineSecondaryFB.h
  Src/Fog/UI/Engine/UIEngineWindow.h
)

Set(FOG_UI_ENGINE_MAC_SOURCES
  Src/Fog/UI/Engine/MacUIEngine.mm
  Src/Fog/UI/Engine/MacUIEngineWindow.mm
  Src/Fog/UI/Engine/MacUIEventLoopImpl.mm
)

Set(FOG_UI_ENGINE_MAC_HEADERS
  Src/Fog/UI/Engine/MacUIEngine.h
  Src/Fog/UI/Engine/MacUIEngineWindow.h
  Src/Fog/UI/Engine/MacUIEventLoopImpl_p.h
)

Set(FOG_UI_ENGINE_WIN_SOURCES
  Src/Fog/UI/Engine/WinUIEngine.cpp
  Src/Fog/UI/Engine/WinUIEngineWindow.cpp
  Src/Fog/UI/Engine/WinUIEventLoopImpl.cpp
)

Set(FOG_UI_ENGINE_WIN_HEADERS
  Src/Fog/UI/Engine/WinUIEngine.h
  Src/Fog/UI/Engine/WinUIEngineWindow.h
  Src/Fog/UI/Engine/WinUIEventLoopImpl_p.h
)

Set(FOG_UI_ENGINE_X11_SOURCES
  Src/Fog/UI/Engine/X11UIEngine.cpp
  Src/Fog/UI/Engine/X11UIEngineWindow.cpp
  Src/Fog/UI/Engine/X11UIEventLoopImpl.cpp
)

Set(FOG_UI_ENGINE_X11_HEADERS
  Src/Fog/UI/Engine/X11UIEngine.h
  Src/Fog/UI/Engine/X11UIEngineConfig.h
  Src/Fog/UI/Engine/X11UIEngineWindow.h
  Src/Fog/UI/Engine/X11UIEngineXlib.h
  Src/Fog/UI/Engine/X11UIEventLoopImpl_p.h
)

# [Fog/UI/Tools]
Set(FOG_UI_TOOLS_SOURCES
)

Set(FOG_UI_TOOLS_HEADERS
  Src/Fog/UI/Tools/UIUtil.h
)

# Source groups.
FogAddSourceGroup("Fog/UI"                   ${FOG_UI_LIB_SOURCES}              ${FOG_UI_LIB_HEADERS}             )
FogAddSourceGroup("Fog/UI/Engine"            ${FOG_UI_ENGINE_SOURCES}           ${FOG_UI_ENGINE_HEADERS}
                                             ${FOG_UI_ENGINE_MAC_SOURCES}       ${FOG_UI_ENGINE_MAC_HEADERS}
                                             ${FOG_UI_ENGINE_X11_SOURCES}       ${FOG_UI_ENGINE_X11_HEADERS}
                                             ${FOG_UI_ENGINE_WIN_SOURCES}       ${FOG_UI_ENGINE_WIN_HEADERS}      )
FogAddSourceGroup("Fog/UI/Tools"             ${FOG_UI_TOOLS_SOURCES}            ${FOG_UI_TOOLS_HEADERS}           )

EndIf(FOG_BUILD_UI)

# =============================================================================
# [Fog-All]
# =============================================================================

Set(FOG_ALL_SOURCES
  # Fog/Core.
  ${FOG_CORE_LIB_SOURCES}
  ${FOG_CORE_ACC_SOURCES}
  ${FOG_CORE_CPP_SOURCES}
  ${FOG_CORE_DOM_SOURCES}
  ${FOG_CORE_GLOBAL_SOURCES}
  ${FOG_CORE_KERNEL_SOURCES}
  ${FOG_CORE_MATH_SOURCES}
  ${FOG_CORE_MEMORY_SOURCES}
  ${FOG_CORE_OS_SOURCES}
  ${FOG_CORE_THREADING_SOURCES}
  ${FOG_CORE_TOOLS_SOURCES}
  # Fog/G2d.
  ${FOG_G2D_LIB_SOURCES}
  ${FOG_G2D_ACC_SOURCES}
  ${FOG_G2D_GEOMETRY_SOURCES}
  ${FOG_G2D_IMAGING_SOURCES}
  ${FOG_G2D_IMAGING_CODECS_SOURCES}
  ${FOG_G2D_IMAGING_FILTERS_SOURCES}
  ${FOG_G2D_OS_SOURCES}
  ${FOG_G2D_PAINTING_SOURCES}
  ${FOG_G2D_PAINTING_RASTEROPS_C_SOURCES}
  ${FOG_G2D_SOURCE_SOURCES}
  ${FOG_G2D_SVG_SOURCES}
  ${FOG_G2D_TEXT_SOURCES}
  ${FOG_G2D_TEXT_OPENTYPE_SOURCES}
  ${FOG_G2D_TOOLS_SOURCES}
)

Set(FOG_ALL_HEADERS
  # Fog/Core.
  ${FOG_CORE_LIB_HEADERS}
  ${FOG_CORE_ACC_HEADERS}
  ${FOG_CORE_CPP_HEADERS}
  ${FOG_CORE_DOM_HEADERS}
  ${FOG_CORE_GLOBAL_HEADERS}
  ${FOG_CORE_KERNEL_HEADERS}
  ${FOG_CORE_MATH_HEADERS}
  ${FOG_CORE_MEMORY_HEADERS}
  ${FOG_CORE_OS_HEADERS}
  ${FOG_CORE_THREADING_HEADERS}
  ${FOG_CORE_TOOLS_HEADERS}
  # Fog/G2d.
  ${FOG_G2D_LIB_HEADERS}
  ${FOG_G2D_ACC_HEADERS}
  ${FOG_G2D_GEOMETRY_HEADERS}
  ${FOG_G2D_IMAGING_HEADERS}
  ${FOG_G2D_IMAGING_CODECS_HEADERS}
  ${FOG_G2D_IMAGING_FILTERS_HEADERS}
  ${FOG_G2D_OS_HEADERS}
  ${FOG_G2D_PAINTING_HEADERS}
  ${FOG_G2D_PAINTING_RASTEROPS_C_HEADERS}
  ${FOG_G2D_GEOMETRY_HEADERS}
  ${FOG_G2D_SOURCE_HEADERS}
  ${FOG_G2D_SVG_HEADERS}
  ${FOG_G2D_TEXT_HEADERS}
  ${FOG_G2D_TEXT_OPENTYPE_HEADERS}
  ${FOG_G2D_TOOLS_HEADERS}
)

If(FOG_OS_WINDOWS)
  List(APPEND FOG_ALL_SOURCES ${FOG_G2D_WIN_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_G2D_WIN_HEADERS})
EndIf()

If(FOG_OS_MAC)
  List(APPEND FOG_ALL_SOURCES ${FOG_G2D_MAC_SOURCES})
  List(APPEND FOG_ALL_HEADERS ${FOG_G2D_MAC_HEADERS})
EndIf()

If(FOG_BUILD_UI)
  # Fog/UI.
  List(APPEND FOG_ALL_SOURCES
    ${FOG_UI_LIB_SOURCES}
    ${FOG_UI_ENGINE_SOURCES}
    ${FOG_UI_TOOLS_SOURCES}
  )

  List(APPEND FOG_ALL_HEADERS
    ${FOG_UI_LIB_HEADERS}
    ${FOG_UI_ENGINE_HEADERS}
    ${FOG_UI_ENGINE_X11_HEADERS}
    ${FOG_UI_ENGINE_WIN_HEADERS}
    ${FOG_UI_TOOLS_HEADERS}
  )

  If(FOG_OS_WINDOWS)
    List(APPEND FOG_ALL_SOURCES ${FOG_UI_ENGINE_WIN_SOURCES})
    List(APPEND FOG_ALL_HEADERS ${FOG_UI_ENGINE_WIN_HEADERS})
  EndIf()

  If(FOG_OS_MAC)
    List(APPEND FOG_ALL_SOURCES ${FOG_UI_ENGINE_MAC_SOURCES})
    List(APPEND FOG_ALL_HEADERS ${FOG_UI_ENGINE_MAC_HEADERS})
  EndIf()

  If(FOG_BUILD_UI_X11 AND NOT FOG_BUILD_UI_X11_MODULE)
    List(APPEND FOG_ALL_SOURCES ${FOG_UI_ENGINE_X11_SOURCES})
    List(APPEND FOG_ALL_HEADERS ${FOG_UI_ENGINE_X11_HEADERS})
  EndIf()
EndIf()

# Corrected Sources/Headers.
Set(FOG_ALL_SOURCES_CORRECTED "")
Set(FOG_ALL_HEADERS_CORRECTED "")

ForEach(i ${FOG_ALL_SOURCES})
  List(APPEND FOG_ALL_SOURCES_CORRECTED ${FOG_DIR}/${i})
EndForEach()

ForEach(i ${FOG_ALL_HEADERS})
  List(APPEND FOG_ALL_HEADERS_CORRECTED ${FOG_DIR}/${i})
EndForEach()

# =============================================================================
# [Setup - Src/Fog/Core/C++/ConfigCMake.h]
# =============================================================================

# Write CMAKE_ #defines into Src/Fog/Core/C++/ConfigCMake.h files
Configure_File(${CMAKE_CURRENT_LIST_DIR}/Src/Fog/Core/C++/ConfigCMake.h.cmake
               ${CMAKE_CURRENT_LIST_DIR}/Src/Fog/Core/C++/ConfigCMake.h)

# =============================================================================
# [Setup - Fix]
# =============================================================================

# TODO: CMake - Quick hack, do not open freetype dynamically, we can link to it.
If(FOG_FONT_FREETYPE)
  Include_Directories(/usr/include/freetype2)
EndIf()

# Trim libraries string (come compilers can complain if there are extra whitespaces).
String(STRIP "${FOG_LIBRARIES}" FOG_LIBRARIES)

# =============================================================================
# [Setup - Install Header Files]
# =============================================================================

If(NOT FOG_BUILD_NOTHING)
  If(NOT FOG_BUILD_STATIC)
    ForEach(i ${FOG_ALL_HEADERS})
      Get_Filename_Component(path ${i} PATH)
      Get_Filename_Component(name ${i} NAME)
      If(NOT "${name}" MATCHES "Src/ThirdParty/")
        If(NOT "${name}" MATCHES "_p.h$")
          Install(FILES ${i} DESTINATION "include/${path}")
        EndIf()
      EndIf()
    EndForEach()
  EndIf()
EndIf()

# =============================================================================
# [Setup - Lib - Fog]
# =============================================================================

If(NOT FOG_BUILD_NOTHING)
  If(NOT FOG_BUILD_STATIC)
    # Shared library.
    Add_Library(Fog SHARED ${FOG_ALL_SOURCES_CORRECTED} ${FOG_ALL_HEADERS_CORRECTED})
    Target_Link_Libraries(Fog ${FOG_LIBRARIES})
    Install(TARGETS Fog DESTINATION lib)
  Else()
    # Static library.
    Add_Library(Fog STATIC
      ${FOG_ALL_SOURCES_CORRECTED}
      ${FOG_ALL_HEADERS_CORRECTED})
    Target_Link_Libraries(Fog ${FOG_LIBRARIES})
  EndIf()
EndIf()

# =============================================================================
# [Setup - Lib - FogUI_X11]
# =============================================================================

If(NOT FOG_BUILD_NOTHING)
  If(NOT FOG_BUILD_STATIC AND FOG_BUILD_UI_X11 AND FOG_BUILD_UI_X11_MODULE)
    Add_Library(FogUI_X11 SHARED ${FOG_UI_ENGINE_X11_SOURCES} ${FOG_UI_ENGINE_X11_HEADERS})
    Target_Link_Libraries(FogUI_X11 Fog ${FOG_LIBRARIES})
    Install(TARGETS FogUI_X11 DESTINATION lib)
  EndIf()
EndIf()

# =============================================================================
# [Setup - App - Samples]
# =============================================================================

If(NOT FOG_BUILD_NOTHING)
  If(NOT FOG_BUILD_STATIC AND FOG_BUILD_EXAMPLES)
    Set(FOG_SAMPLE_SOURCES
      FogBlend
      FogGroup
      FogTest
      FogSvg
    )

    ForEach(file ${FOG_SAMPLE_SOURCES})
      Add_Executable(${file} WIN32 MACOSX_BUNDLE
        Src/App/Sample/${file}.cpp
        Src/App/Sample/Resources.h)
      Target_Link_Libraries(${file} Fog ${FOG_LIBRARIES})
    EndForEach(file)

    Add_Executable(FogSizeOf Src/App/Sample/FogSizeOf.cpp)
    Target_Link_Libraries(FogSizeOf Fog ${FOG_LIBRARIES})
  EndIf()
EndIf()

# =============================================================================
# [Setup - App - Bench]
# =============================================================================

If(NOT FOG_BUILD_NOTHING)
  If(NOT FOG_BUILD_STATIC AND FOG_BUILD_BENCH)
    Set(FOG_BENCH_LIBRARIES "")
    Set(FOG_BENCH_INCLUDE "")

    # Platform independent libraries to benchmark.
    
    # Set(FOG_BENCH_CAIRO True)
    # Set(FOG_BENCH_QT4 True)

    # Platform specific libraries to benchmark.
    If(FOG_OS_WINDOWS)
      # Set(FOG_BENCH_GDIPLUS True)
    EndIf()

    If(FOG_OS_MAC)
      # Set(FOG_BENCH_CG True)
    EndIf(FOG_OS_MAC)


    Configure_File(
      ${CMAKE_CURRENT_LIST_DIR}/Src/App/Bench/BenchConfig.h.cmake
      ${CMAKE_CURRENT_LIST_DIR}/Src/App/Bench/BenchConfig.h)

    Set(FOG_BENCH_FILES
      Src/App/Bench/BenchApp.cpp
      Src/App/Bench/BenchApp.h
      Src/App/Bench/BenchCairo.cpp
      Src/App/Bench/BenchCairo.h
      Src/App/Bench/BenchCG.cpp
      Src/App/Bench/BenchCG.h
      Src/App/Bench/BenchConfig.h
      Src/App/Bench/BenchFog.cpp
      Src/App/Bench/BenchFog.h
      Src/App/Bench/BenchGdiPlus.cpp
      Src/App/Bench/BenchGdiPlus.h
      Src/App/Bench/BenchQt4.cpp
      Src/App/Bench/BenchQt4.h
    )

    If(FOG_BENCH_CAIRO)
      Include_Directories(/usr/include/cairo)
      List(APPEND FOG_BENCH_LIBRARIES cairo)
    EndIf()

    If(FOG_BENCH_QT4)
      Find_Package(Qt4 REQUIRED)
      Include(${QT_USE_FILE})
      List(APPEND FOG_BENCH_LIBRARIES ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
    EndIf()

    Add_Executable(FogBench ${FOG_BENCH_FILES})
    Target_Link_Libraries(FogBench Fog ${FOG_LIBRARIES} ${FOG_BENCH_LIBRARIES})
  EndIf()
EndIf()
