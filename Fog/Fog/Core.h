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

#include <Fog/Core/C++/Base.h>

#include <Fog/Core/Collection/StackP.h>
#include <Fog/Core/Collection/Util.h>
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/IO/DirEntry.h>
#include <Fog/Core/IO/DirIterator.h>
#include <Fog/Core/IO/FileSystem.h>
#include <Fog/Core/IO/MapFile.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Kernel/Delegate.h>
#include <Fog/Core/Kernel/Event.h>
#include <Fog/Core/Kernel/EventLoop.h>
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/Math/Convert.h>
#include <Fog/Core/Math/FloatBits.h>
#include <Fog/Core/Math/FloatControl.h>
#include <Fog/Core/Math/Function.h>
#include <Fog/Core/Math/Fuzzy.h>
#include <Fog/Core/Math/Integrate.h>
#include <Fog/Core/Math/Interval.h>
#include <Fog/Core/Math/Math.h>
#include <Fog/Core/Math/MathVec.h>
#include <Fog/Core/Math/Solve.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/Core/Memory/MemBuffer.h>
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/Core/Memory/MemOps.h>
#include <Fog/Core/Memory/MemPool.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/OS/System.h>
#include <Fog/Core/OS/UserInfo.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Threading/AtomicPadding.h>
#include <Fog/Core/Threading/Lock.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadCondition.h>
#include <Fog/Core/Threading/ThreadEvent.h>
#include <Fog/Core/Threading/ThreadLocal.h>
#include <Fog/Core/Threading/ThreadPool.h>
#include <Fog/Core/Tools/Algorithm.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharData.h>
#include <Fog/Core/Tools/Cpu.h>
#include <Fog/Core/Tools/Date.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/HashString.h>
#include <Fog/Core/Tools/HashUntyped.h>
#include <Fog/Core/Tools/HashUtil.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/ListReal.h>
#include <Fog/Core/Tools/ListString.h>
#include <Fog/Core/Tools/ListUntyped.h>
#include <Fog/Core/Tools/ListVar.h>
#include <Fog/Core/Tools/Locale.h>
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/Core/Tools/Range.h>
#include <Fog/Core/Tools/RegExp.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/StringUtil.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/Core/Tools/Stub.h>
#include <Fog/Core/Tools/Swap.h>
#include <Fog/Core/Tools/TextCodec.h>
#include <Fog/Core/Tools/TextIterator.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/Core/Tools/Var.h>
#include <Fog/Core/Tools/VarId.h>

// [Guard]
#endif // _FOG_CORE_H
