// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

#ifndef _FOG_CORE_CPP_STDNEW_H
#define _FOG_CORE_CPP_STDNEW_H

#if !defined(FOG_IDE) && !defined(_FOG_CORE_CPP_BASE_H)
#error "Fog/Core/C++/StdNew.h can be only included by Fog/Core/C++/Base.h"
#endif // _FOG_CORE_CPP_BASE_H

// ============================================================================
// [Fog::Core::C++ - New]
// ============================================================================

#define FOG_USE_STDNEW

#if defined(FOG_USE_STDNEW)

#include <new>

#define fog_new new(::std::nothrow)
#define fog_new_p(_Ptr_) new(_Ptr_)

#define fog_delete(_Ptr_) delete _Ptr_

#else

namespace Fog {
namespace MemMgr {

// Implemented-Later.
static FOG_INLINE void* alloc(size_t size);
static FOG_INLINE void free(void* p);

} // MemMgr namespace
} // Fog namespace

struct __FogMemMgr {};
// Implemented in Fog/Core/Memory/MemMgr.cpp
FOG_CVAR_EXTERN __FogMemMgr fog_memmgr;

// Standard new/delete implementation (using Fog::MemMgr backend and no exceptions).
FOG_INLINE void* operator new(size_t size, const __FogMemMgr&) FOG_NOTHROW { return Fog::MemMgr::alloc(size); }
FOG_INLINE void* operator new[](size_t size, const __FogMemMgr&) FOG_NOTHROW { return Fog::MemMgr::alloc(size); }

FOG_INLINE void operator delete(void* p, const __FogMemMgr&) FOG_NOTHROW { return Fog::MemMgr::free(p); }
FOG_INLINE void operator delete[](void* p, const __FogMemMgr&) FOG_NOTHROW { return Fog::MemMgr::free(p); }

// Placement new/delete implementation (using fog_memmgr to distinguist between
// STD/FOG implementations).
FOG_INLINE void* operator new(size_t, const __FogMemMgr&, void* p) FOG_NOTHROW { return p; }
FOG_INLINE void* operator new[](size_t, const __FogMemMgr&, void* p) FOG_NOTHROW { return p; }

FOG_INLINE void operator delete(void*, const __FogMemMgr&, void*) FOG_NOTHROW {}
FOG_INLINE void operator delete[](void*, const __FogMemMgr&, void*) FOG_NOTHROW {}

template<typename T>
void __fog_delete(T* inst)
{
  inst->~T();
  Fog::MemMgr::free(inst);
}

// Fog macros to make memory allocation more friendly and compatible with std
// version.
#define fog_new new(fog_memmgr)
#define fog_new_p(_Ptr_) new(fog_memmgr, _Ptr_)
#define fog_delete(_Ptr_) __fog_delete(_Ptr_);

#endif // FOG_USE_STDNEW
#endif // _FOG_CORE_CPP_STDNEW_H
