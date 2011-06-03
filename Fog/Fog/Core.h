// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

// [Guard]
#ifndef _FOG_CORE_H
#define _FOG_CORE_H

// ============================================================================
// [Fog-Core]
// ============================================================================

#include <Fog/Core/Config/Config.h>

#include <Fog/Core/Collection/Algorithms.h>
#include <Fog/Core/Collection/Hash.h>
#include <Fog/Core/Collection/HashP.h>
#include <Fog/Core/Collection/HashUtil.h>
#include <Fog/Core/Collection/List.h>
#include <Fog/Core/Collection/ListP.h>
#include <Fog/Core/Collection/BufferP.h>
#include <Fog/Core/Collection/StackP.h>
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Cpu/Cpu.h>
#include <Fog/Core/Cpu/Initializer.h>
#include <Fog/Core/DateTime/Time.h>
#include <Fog/Core/DateTime/TimeDelta.h>
#include <Fog/Core/DateTime/TimeTicks.h>
#include <Fog/Core/Global/Api.h>
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Error.h>
#include <Fog/Core/Global/Init.h>
#include <Fog/Core/Global/SequenceInfo.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/Global/Swap.h>
#include <Fog/Core/Global/TypeInfo.h>
#include <Fog/Core/Global/TypeVariant.h>
#include <Fog/Core/Global/Uninitialized.h>
#include <Fog/Core/IO/DirEntry.h>
#include <Fog/Core/IO/DirIterator.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/Math/Fixed.h>
#include <Fog/Core/Math/FloatBits.h>
#include <Fog/Core/Math/Function.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Integrate.h>
#include <Fog/Core/Math/Interval.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/MathVec.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/Memory.h>
#include <Fog/Core/Memory/MemoryBuffer.h>
#include <Fog/Core/Memory/MemoryManager.h>
#include <Fog/Core/OS/OS.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/System/Application.h>
#include <Fog/Core/System/Delegate.h>
#include <Fog/Core/System/Event.h>
#include <Fog/Core/System/EventLoop.h>
#include <Fog/Core/System/Object.h>
#include <Fog/Core/System/Timer.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/AtomicPadding.h>
#include <Fog/Core/Threading/Lazy.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Threading/ThreadLocalStorage.h>
#include <Fog/Core/Threading/ThreadPool.h>
#include <Fog/Core/Tools/Byte.h>
#include <Fog/Core/Tools/ByteArray.h>
#include <Fog/Core/Tools/ByteArrayFilter.h>
#include <Fog/Core/Tools/ByteArrayMatcher.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharUtil.h>
#include <Fog/Core/Tools/Format.h>
#include <Fog/Core/Tools/Locale.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringFilter.h>
#include <Fog/Core/Tools/StringMatcher.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Core/Tools/Stub.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Core/Tools/TextIterator.h>
#include <Fog/Core/Tools/Unicode.h>
#include <Fog/Core/Variant/Var.h>

// [Guard]
#endif // _FOG_CORE_H
