// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/OS/WinCom.h>
#include <Fog/Core/OS/WinUtil.h>

namespace Fog {

// ============================================================================
// [Fog::WinIStream]
// ============================================================================

class FOG_NO_EXPORT WinIStream : public IStream
{
public:
  WinIStream(Stream& stream);
  ~WinIStream();

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

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

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Stream _stream;
  LONG _reference;
};

// ============================================================================
// [Fog::WinIStream - Construction / Destruction]
// ============================================================================

// Default IStream implementation can be found in the MSDN:
// - http://msdn.microsoft.com/en-us/library/ms752876%28VS.85%29.aspx .
//
// But the function IStream::Stat() from the link is wrong. The Stat
// filled pStatstg->cbSize, but nothing else. We are using IStream to
// provide streaming for Gdi+ and it expects that all members are clear.

WinIStream::WinIStream(Stream& stream) :
  _stream(stream),
  _reference(1)
{
}

WinIStream::~WinIStream()
{
}

// ============================================================================
// [Fog::WinIStream - IUnknown]
// ============================================================================

HRESULT STDMETHODCALLTYPE WinIStream::QueryInterface(REFIID iid, void** ppvObject)
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

ULONG STDMETHODCALLTYPE WinIStream::AddRef(void)
{
  return (ULONG)InterlockedIncrement(&_reference);
}

ULONG STDMETHODCALLTYPE WinIStream::Release(void)
{
  ULONG res = (ULONG)InterlockedDecrement(&_reference);

  if (res == 0)
    fog_delete(this);

  return res;
}

// ============================================================================
// [Fog::WinIStream - ISequentialStream]
// ============================================================================

HRESULT STDMETHODCALLTYPE WinIStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
  size_t cbRead = _stream.read(pv, cb);
  if (pcbRead) *pcbRead = (ULONG)cbRead;

  if (cbRead == cb)
    return S_OK;
  else
    return S_FALSE;
}

HRESULT STDMETHODCALLTYPE WinIStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
  size_t cbWritten = _stream.write(pv, cb);
  if (pcbWritten) *pcbWritten = (ULONG)cbWritten;

  if (cbWritten == cb)
    return S_OK;
  else
    return STG_E_MEDIUMFULL;
}

// ============================================================================
// [Fog::WinIStream - IStream]
// ============================================================================

HRESULT STDMETHODCALLTYPE WinIStream::SetSize(ULARGE_INTEGER libNewSize)
{
  err_t err = _stream.setSize((int64_t)libNewSize.QuadPart);
  if (FOG_IS_ERROR(err))
    return E_FAIL;
  else
    return S_OK;
}

HRESULT STDMETHODCALLTYPE WinIStream::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
  // TODO.
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinIStream::Commit(DWORD grfCommitFlags)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinIStream::Revert(void)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinIStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinIStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinIStream::Clone(IStream** ppstm)
{
  // TODO.
  return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinIStream::Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer)
{
  // Win32 STREAM_SEEK is compatible with Fog::STREAM_SEEK_MODE.
  if (dwOrigin > 2)
    return STG_E_INVALIDFUNCTION;

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

HRESULT STDMETHODCALLTYPE WinIStream::Stat(STATSTG* pStatstg, DWORD grfStatFlag)
{
  ZeroMemory(pStatstg, sizeof(*pStatstg));
  _stream.getSize((int64_t*)&pStatstg->cbSize);
  return S_OK;
}

// ============================================================================
// [Fog::WinCOM]
// ============================================================================

static err_t FOG_CDECL WinCOM_makeIStream(IStream** dst, Stream* src)
{
  IStream* s = fog_new WinIStream(*src);

  if (FOG_IS_NULL(s))
  {
    *dst = NULL;
    return ERR_RT_OUT_OF_MEMORY;
  }
  else
  {
    *dst = s;
    return ERR_OK;
  }
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void WinCOM_init(void)
{
  _api.wincom_makeIStream = WinCOM_makeIStream;
}

} // Fog namespace
