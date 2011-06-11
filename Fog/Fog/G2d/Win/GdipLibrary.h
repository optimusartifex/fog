// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_WIN_GDIPLIBRARY_H
#define _FOG_G2D_WIN_GDIPLIBRARY_H

// [Dependencies]
#include <Fog/Core/Library/Library.h>

// [Dependencies - Windows]
#include <windows.h>

// ============================================================================
// [Fog::GdipLibrary - Defs]
// ============================================================================

#include <Fog/Core/Pack/PackQWord.h>

typedef uint32_t GpARGB;
typedef int GpPixelFormat;

struct GpBitmap;
struct GpBitmapData;
struct GpEncoderParameter;
struct GpEncoderParameters;
struct GpGdiplusStartupInput;
struct GpGdiplusStartupOutput;
struct GpGraphics;
struct GpImage;
struct GpImageCodecInfo;
struct GpPropertyItem;
struct GpRect;

enum GpPixelFormatEnum
{
  GpPixelFormatUndefined      = 0x00000000,
  GpPixelFormatDontCare       = 0x00000000,

  GpPixelFormatIndexed        = 0x00010000,
  GpPixelFormatGDI            = 0x00020000,
  GpPixelFormatAlpha          = 0x00040000,
  GpPixelFormatPAlpha         = 0x00080000,
  GpPixelFormatExtended       = 0x00100000,
  GpPixelFormatCanonical      = 0x00200000,

  GpPixelFormat1bppIndexed    = (1  | ( 1 << 8) | GpPixelFormatGDI     | GpPixelFormatIndexed   ),
  GpPixelFormat4bppIndexed    = (2  | ( 4 << 8) | GpPixelFormatGDI     | GpPixelFormatIndexed   ),
  GpPixelFormat8bppIndexed    = (3  | ( 8 << 8) | GpPixelFormatGDI     | GpPixelFormatIndexed   ),
  GpPixelFormat16bppGrayScale = (4  | (16 << 8) | GpPixelFormatExtended),
  GpPixelFormat16bppRGB555    = (5  | (16 << 8) | GpPixelFormatGDI     ),
  GpPixelFormat16bppRGB565    = (6  | (16 << 8) | GpPixelFormatGDI     ),
  GpPixelFormat16bppARGB1555  = (7  | (16 << 8) | GpPixelFormatGDI     | GpPixelFormatAlpha     ),
  GpPixelFormat24bppRGB       = (8  | (24 << 8) | GpPixelFormatGDI     ),
  GpPixelFormat32bppRGB       = (9  | (32 << 8) | GpPixelFormatGDI     ),
  GpPixelFormat32bppARGB      = (10 | (32 << 8) | GpPixelFormatGDI     | GpPixelFormatAlpha     | GpPixelFormatCanonical ),
  GpPixelFormat32bppPARGB     = (11 | (32 << 8) | GpPixelFormatGDI     | GpPixelFormatAlpha     | GpPixelFormatPAlpha    ),
  GpPixelFormat48bppRGB       = (12 | (48 << 8) | GpPixelFormatExtended),
  GpPixelFormat64bppARGB      = (13 | (64 << 8) | GpPixelFormatExtended| GpPixelFormatAlpha     | GpPixelFormatCanonical ),
  GpPixelFormat64bppPARGB     = (14 | (64 << 8) | GpPixelFormatExtended| GpPixelFormatAlpha     | GpPixelFormatPAlpha    ),
  GpPixelFormatMax            = (15)
};

enum GpCompositingMode
{
  GpCompositingModeSourceOver              = 0,
  GpCompositingModeSourceCopy              = 1
};

enum GpEncoderParameterValueType
{
  GpEncoderParameterValueTypeByte          = 1,
  GpEncoderParameterValueTypeASCII         = 2,
  GpEncoderParameterValueTypeShort         = 3,
  GpEncoderParameterValueTypeLong          = 4,
  GpEncoderParameterValueTypeRational      = 5,
  GpEncoderParameterValueTypeLongRange     = 6,
  GpEncoderParameterValueTypeUndefined     = 7,
  GpEncoderParameterValueTypeRationalRange = 8,
  GpEncoderParameterValueTypePointer       = 9
};

enum GpFlushIntention
{
  GpFlushIntentionFlush                    = 0,
  GpFlushIntentionSync                     = 1
};

enum GpImageFlags
{
  GpImageFlagsNone                         = 0x00000000,
  GpImageFlagsScalable                     = 0x00000001,
  GpImageFlagsHasAlpha                     = 0x00000002,
  GpImageFlagsHasTranslucent               = 0x00000004,
  GpImageFlagsPartiallyScalable            = 0x00000008,
  GpImageFlagsColorSpaceRGB                = 0x00000010,
  GpImageFlagsColorSpaceCMYK               = 0x00000020,
  GpImageFlagsColorSpaceGRAY               = 0x00000040,
  GpImageFlagsColorSpaceYCBCR              = 0x00000080,
  GpImageFlagsColorSpaceYCCK               = 0x00000100,
  GpImageFlagsHasRealDPI                   = 0x00001000,
  GpImageFlagsHasRealPixelSize             = 0x00002000,
  GpImageFlagsReadOnly                     = 0x00010000,
  GpImageFlagsCaching                      = 0x00020000
};

enum GpImageLockMode
{
  GpImageLockModeRead                      = 0x00000001,
  GpImageLockModeWrite                     = 0x00000002,
  GpImageLockModeUserInputBuf              = 0x00000004
};

enum GpImageType
{
  GpImageTypeUnknown                       = 0,
  GpImageTypeBitmap                        = 1,
  GpImageTypeMetafile                      = 2
};

enum GpStatus
{
  GpOk                                     = 0,
  GpGenericError                           = 1,
  GpInvalidParameter                       = 2,
  GpOutOfMemory                            = 3,
  GpObjectBusy                             = 4,
  GpInsufficientBuffer                     = 5,
  GpNotImplemented                         = 6,
  GpWin32Error                             = 7,
  GpWrongState                             = 8,
  GpAborted                                = 9,
  GpFileNotFound                           = 10,
  GpValueOverflow                          = 11,
  GpAccessDenied                           = 12,
  GpUnknownImageFormat                     = 13,
  GpFontFamilyNotFound                     = 14,
  GpFontStyleNotFound                      = 15,
  GpNotTrueTypeFont                        = 16,
  GpUnsupportedGdiplusVersion              = 17,
  GpGdiplusNotInitialized                  = 18,
  GpPropertyNotFound                       = 19,
  GpPropertyNotSupported                   = 20,
  GpProfileNotFound                        = 21
};

struct GpBitmapData
{
  UINT Width;
  UINT Height;
  INT Stride;
  GpPixelFormat PixelFormat;
  VOID* Scan0;
  UINT_PTR Reserved;
};

struct GpEncoderParameter
{
  GUID Guid;
  ULONG NumberOfValues;
  ULONG Type;
  void* Value;
};

struct GpEncoderParameters
{
  UINT Count;
  GpEncoderParameter Parameter[1];
};

struct GpGdiplusStartupInput
{
  UINT32 GdiplusVersion;
  void* DebugEventCallback;
  BOOL SuppressBackgroundThread;
  BOOL SuppressExternalCodecs;
};

struct GpGdiplusStartupOutput
{
  GpStatus (FOG_STDCALL* NotificationHookProc)(ULONG_PTR* token);
  void (FOG_STDCALL* NotificationUnhookProc)(ULONG_PTR token);
};

struct GpImageCodecInfo
{
  CLSID Clsid;
  GUID FormatID;
  const WCHAR* CodecName;
  const WCHAR* DllName;
  const WCHAR* FormatDescription;
  const WCHAR* FilenameExtension;
  const WCHAR* MimeType;
  DWORD Flags;
  DWORD Version;
  DWORD SigCount;
  DWORD SigSize;
  const BYTE* SigPattern;
  const BYTE* SigMask;
};

struct GpPropertyItem
{
  ULONG Id;
  ULONG Length;
  WORD Type;
  void* Value;
};

struct GpRect
{
  int X;
  int Y;
  int Width;
  int Height;
};

#include <Fog/Core/Pack/PackRestore.h>

namespace Fog {

//! @addtogroup Fog_G2d_Win
//! @{

// ============================================================================
// [Fog::GdipLibrary]
// ============================================================================

//! @brief GdiPlus library interface.
struct FOG_API GdipLibrary
{
  GdipLibrary();
  ~GdipLibrary();

  err_t prepare();
  err_t init();
  void close();

  enum { NUM_SYMBOLS = 20 };
  union
  {
    struct
    {
      GpStatus (FOG_STDCALL* pGdiplusStartup)(ULONG_PTR* token, const GpGdiplusStartupInput* input, GpGdiplusStartupOutput *output);
      void (FOG_STDCALL* pGdiplusShutdown)(ULONG_PTR token);
      GpStatus (FOG_STDCALL* pGdipLoadImageFromStream)(IStream* stream, GpImage** image);
      GpStatus (FOG_STDCALL* pGdipSaveImageToStream)(GpImage* image, IStream* stream, const CLSID* clsidEncoder, const GpEncoderParameters* encoderParams);
      GpStatus (FOG_STDCALL* pGdipDisposeImage)(GpImage* image);
      GpStatus (FOG_STDCALL* pGdipGetImageType)(GpImage* image, UINT* type);
      GpStatus (FOG_STDCALL* pGdipGetImageWidth)(GpImage* image, UINT* width);
      GpStatus (FOG_STDCALL* pGdipGetImageHeight)(GpImage* image, UINT* height);
      GpStatus (FOG_STDCALL* pGdipGetImageFlags)(GpImage* image, UINT* flags);
      GpStatus (FOG_STDCALL* pGdipGetImagePixelFormat)(GpImage* image, GpPixelFormat* format);
      GpStatus (FOG_STDCALL* pGdipGetImageGraphicsContext)(GpImage* image, GpGraphics** graphics);
      GpStatus (FOG_STDCALL* pGdipImageGetFrameCount)(GpImage* image, const GUID* dimensionID, UINT* count);
      GpStatus (FOG_STDCALL* pGdipImageSelectActiveFrame)(GpImage* image, const GUID* dimensionID, UINT frameIndex);
      GpStatus (FOG_STDCALL* pGdipCreateBitmapFromScan0)(INT width, INT height, INT stride, GpPixelFormat format, BYTE* scan0, GpBitmap** bitmap);
      GpStatus (FOG_STDCALL* pGdipSetCompositingMode)(GpGraphics* graphics, GpCompositingMode compositingMode);
      GpStatus (FOG_STDCALL* pGdipDrawImageI)(GpGraphics* graphics, GpImage* image, INT x, INT y);
      GpStatus (FOG_STDCALL* pGdipFlush)(GpGraphics* graphics, GpFlushIntention intention);
      GpStatus (FOG_STDCALL* pGdipDeleteGraphics)(GpGraphics* graphics);

      GpStatus (FOG_STDCALL* pGdipGetImageEncoders)(UINT numEncoders, UINT size, GpImageCodecInfo* encoders);
      GpStatus (FOG_STDCALL* pGdipGetImageEncodersSize)(UINT *numEncoders, UINT *size);
    };
    void* addr[NUM_SYMBOLS];
  };

  Library dll;
  volatile err_t err;

  ULONG_PTR gdiplusToken;

private:
  _FOG_CLASS_NO_COPY(GdipLibrary)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_WIN_GDIPLIBRARY_H
