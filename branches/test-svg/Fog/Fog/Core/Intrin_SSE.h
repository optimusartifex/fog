// [Fog/Core Library - C++ API]
//
// [Licence]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_INTRIN_SSE_H
#define _FOG_CORE_INTRIN_SSE_H

// [Dependencies]
#include <Fog/Build/Build.h>
#include <xmmintrin.h>

// MinGW workaround
#if defined(__MINGW32__)
// I can't understand why MinGW will not align data that uses FOG_ALIGN(16) attribute!
#define FOG_DECLARE_ALIGNED_VARIABLE(type, name, align) \
  int8_t name##_storage[sizeof(type) + align]; \
  type& name = *( (type *)(void *)( ((sysuint_t)(name##_storage) + align - 1) & ~(align - 1) ) )
#else
#define FOG_DECLARE_ALIGNED_VARIABLE(type, name, align) \
  type FOG_ALIGN(align) name
#endif

// [Guard]
#endif // _FOG_CORE_INTRIN_SSE_H
