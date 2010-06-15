// [Fog-Core Library - Private API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_WINISTREAM_P_H
#define _FOG_CORE_WINISTREAM_P_H

#include <Fog/Core/Build.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Stream.h>
#include <objidl.h>

namespace Fog {

//! @addtogroup Fog_Core_Private
//! @{

class FOG_HIDDEN WinIStream : public IStream
{
public:
  WinIStream(Stream& stream);
private:
  ~WinIStream();

public:
  // IUnknown Interface.
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
  virtual ULONG   STDMETHODCALLTYPE AddRef(void);
  virtual ULONG   STDMETHODCALLTYPE Release(void);

  // ISequentialStream Interface.
  virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead);
  virtual HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten);

  // IStream Interface.
  virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize);
  virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten);
  virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
  virtual HRESULT STDMETHODCALLTYPE Revert(void);
  virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
  virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
  virtual HRESULT STDMETHODCALLTYPE Clone(IStream** ppstm);
  virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer);
  virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg, DWORD grfStatFlag);

private:
  Stream _stream;
  LONG _refCount;
};

//! @}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
#endif // _FOG_CORE_WINISTREAM_P_H
