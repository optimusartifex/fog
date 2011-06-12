// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Guard]
#include <Fog/Core/Config/Config.h>
#if defined(FOG_OS_WINDOWS)

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Win/Com.h>
#include <Fog/Core/Win/ComStream_p.h>

namespace Fog {

// Default IStream implementation can be found in the MSDN:
// - http://msdn.microsoft.com/en-us/library/ms752876%28VS.85%29.aspx .
//
// But the function IStream::Stat() from the link is wrong. The Stat
// filled pStatstg->cbSize, but nothing else. We are using IStream to
// provide streaming for Gdi+ and it expects that all members are clear.

ComStream::ComStream(Stream& stream) :
  _stream(stream),
  _refCount(1)
{
}

ComStream::~ComStream()
{
}

HRESULT STDMETHODCALLTYPE ComStream::QueryInterface(REFIID iid, void** ppvObject)
{
  if (iid == __uuidof(IUnknown) ||
      iid == __uuidof(IStream) ||
      iid == __uuidof(ISequentialStream))
  {
    *ppvObject = static_cast<IStream*>(this);
    AddRef();
    return S_OK;
  }
  else
  {
    return E_NOINTERFACE;
  }
}

ULONG STDMETHODCALLTYPE ComStream::AddRef(void)
{
  return (ULONG)InterlockedIncrement(&_refCount);
}

ULONG STDMETHODCALLTYPE ComStream::Release(void)
{
  ULONG res = (ULONG)InterlockedDecrement(&_refCount);

  if (res == 0)
    fog_delete(this);

  return res;
}

// ISequentialStream Interface
HRESULT STDMETHODCALLTYPE ComStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
  size_t cbRead = _stream.read(pv, cb);
  if (pcbRead) *pcbRead = (ULONG)cbRead;

  if (cbRead == cb)
    return S_OK;
  else
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE ComStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
  size_t cbWritten = _stream.write(pv, cb);
  if (pcbWritten) *pcbWritten = (ULONG)cbWritten;

  if (cbWritten == cb)
    return S_OK;
  else
    return STG_E_MEDIUMFULL;
}

// IStream Interface
HRESULT STDMETHODCALLTYPE ComStream::SetSize(ULARGE_INTEGER libNewSize)
{
  err_t err = _stream.setSize((int64_t)libNewSize.QuadPart);
  if (FOG_IS_ERROR(err))
    return E_FAIL;
  else
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ComStream::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
  // TODO.
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ComStream::Commit(DWORD grfCommitFlags)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ComStream::Revert(void)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ComStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ComStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ComStream::Clone(IStream** ppstm)
{
  // TODO.
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE ComStream::Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer)
{
  // Win32 STREAM_SEEK is compatible to Fog::STREAM_SEEK_MODE.
  if (dwOrigin > 2) return STG_E_INVALIDFUNCTION;
  int64_t np = _stream.seek((int64_t)liDistanceToMove.QuadPart, dwOrigin);

  if (np != -1)
  {
    if (lpNewFilePointer) lpNewFilePointer->QuadPart = (uint64_t)np;
    return S_OK;
  }
  else
  {
    if (lpNewFilePointer) lpNewFilePointer->QuadPart = 0;
    return STG_E_INVALIDPOINTER;
  }
}

HRESULT STDMETHODCALLTYPE ComStream::Stat(STATSTG* pStatstg, DWORD grfStatFlag)
{
  ZeroMemory(pStatstg, sizeof(*pStatstg));
  _stream.getSize((int64_t*)&pStatstg->cbSize);
  return S_OK;
}

} // Fog namespace

// [Guard]
#endif // FOG_OS_WINDOWS
