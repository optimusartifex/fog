// [Fog-Core Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// [Guard]
#ifndef _FOG_CORE_SCOPEDHANDLE_H
#define _FOG_CORE_SCOPEDHANDLE_H

// [Dependencies]
#include <Fog/Build/Build.h>

// [Windows Specific]
#if defined(FOG_OS_WINDOWS)

#include <windows.h>

namespace Fog {

// Used so we always remember to close the handle. Example:
//   Fog::ScopedHANDLE hfile(CreateFile(...));
//   if (!hfile.get())
//     ...process error
//   ReadFile(hfile.get(), ...);
//
// To sqirrel the handle away somewhere else:
//   secret_handle_ = hfile.take();
//
// To explicitly close the handle:
//   CloseHandle(hfile.take());
struct ScopedHANDLE
{
public:
  FOG_INLINE ScopedHANDLE() : _handle(NULL) {}
  FOG_INLINE explicit ScopedHANDLE(HANDLE h) : _handle(h) { normalize(); }
  FOG_INLINE ~ScopedHANDLE() { close(); }

  // Use this instead of comparing to INVALID_HANDLE_VALUE to pick up our NULL
  // usage for errors.
  FOG_INLINE bool isValid() const { return _handle != NULL; }

  FOG_INLINE HANDLE get() { return _handle; }
  FOG_INLINE HANDLE take() { HANDLE r = _handle; _handle = 0; return r; }
  FOG_INLINE void free() { close(); _handle = 0; }
  FOG_INLINE void set(HANDLE h) { close(); _handle = h; normalize(); }
  FOG_INLINE operator HANDLE() { return _handle; }

private:
  FOG_INLINE void close()
  {
    if (_handle)
    {
      if (!::CloseHandle(_handle))
      {
        FOG_ASSERT_NOT_REACHED();
      }
    }
  }

  FOG_INLINE void normalize()
  {
    // Windows is inconsistent about invalid handles, so we always use NULL
    if (_handle == INVALID_HANDLE_VALUE) _handle = NULL;
  }

  HANDLE _handle;
  FOG_DISABLE_COPY(ScopedHANDLE)
};

} // Fog namespace

// [Windows Specific]
#endif // FOG_OS_WINDOWS

// [Guard]
#endif // _FOG_CORE_SCOPEDHANDLE_H
