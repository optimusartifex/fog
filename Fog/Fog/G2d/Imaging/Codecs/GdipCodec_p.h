// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_G2D_IMAGING_CODECS_GDIPCODEC_P_H
#define _FOG_G2D_IMAGING_CODECS_GDIPCODEC_P_H

#include <Fog/Core/Config/Config.h>
#if !defined(FOG_OS_WINDOWS)
#error "Fog::GdiPlusCodecProvider can be included / compiled only under Windows"
#endif // FOG_OS_WINDOWS

// [Dependencies]
#include <Fog/Core/Global/Class.h>
#include <Fog/Core/Global/Constants.h>
#include <Fog/Core/Global/Static.h>
#include <Fog/Core/IO/Stream.h>
#include <Fog/Core/Library/Library.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Global/Init_G2d_p.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageConverter.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>
#include <Fog/G2d/Imaging/ImageEncoder.h>

#include <windows.h>

// ============================================================================
// [GdiPlus - Flat API]
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

// ============================================================================
// [Fog::WinGdiPlusLibrary]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT WinGdiPlusLibrary
{
  WinGdiPlusLibrary();
  ~WinGdiPlusLibrary();

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
  _FOG_CLASS_NO_COPY(WinGdiPlusLibrary)
};

// ============================================================================
// [Fog::GdiPlusJpegParams]
// ============================================================================

//! @internal
struct GdiPlusJpegParams
{
  int quality;
};

// ============================================================================
// [Fog::GdiPlusPngParams]
// ============================================================================

//! @internal
struct GdiPlusPngParams
{
  int dummy;
};

// ============================================================================
// [Fog::GdiPlusTiffParams]
// ============================================================================

//! @internal
struct GdiPlusTiffParams
{
  int dummy;
};

// ============================================================================
// [Fog::GdiPlusCommonParams]
// ============================================================================

//! @internal
union GdiPlusCommonParams
{
  GdiPlusJpegParams jpeg;
  GdiPlusPngParams png;
  GdiPlusTiffParams tiff;
};

// ============================================================================
// [Fog::GdiPlusCodecProvider]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT GdiPlusCodecProvider : public ImageCodecProvider
{
  GdiPlusCodecProvider(uint32_t streamType);
  virtual ~GdiPlusCodecProvider();

  virtual uint32_t checkSignature(const void* mem, size_t length) const;
  virtual err_t createCodec(uint32_t codecType, ImageCodec** codec) const;

  const WCHAR* _gdipMime;
};

// ============================================================================
// [Fog::GdiPlusDecoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT GdiPlusDecoder : public ImageDecoder
{
  FOG_DECLARE_OBJECT(GdiPlusDecoder, ImageDecoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GdiPlusDecoder(ImageCodecProvider* provider);
  virtual ~GdiPlusDecoder();

  // --------------------------------------------------------------------------
  // [Stream]
  // --------------------------------------------------------------------------

  virtual void attachStream(Stream& stream);
  virtual void detachStream();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual void reset();

  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief JPEG, PNG or TIFF parameters.
  GdiPlusCommonParams _params;

  //! @brief IStream bridge.
  IStream* _istream;

  //! @brief Gdi+ shadow image instance.
  GpImage* _gpImage;
};

// ============================================================================
// [Fog::GdiPlusEncoder]
// ============================================================================

//! @internal
struct FOG_NO_EXPORT GdiPlusEncoder : public ImageEncoder
{
  FOG_DECLARE_OBJECT(GdiPlusEncoder, ImageEncoder)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  GdiPlusEncoder(ImageCodecProvider* provider);
  virtual ~GdiPlusEncoder();

  // --------------------------------------------------------------------------
  // [Stream]
  // --------------------------------------------------------------------------

  virtual void attachStream(Stream& stream);
  virtual void detachStream();

  // --------------------------------------------------------------------------
  // [Virtuals]
  // --------------------------------------------------------------------------

  virtual void reset();
  virtual err_t writeImage(const Image& image);

  // --------------------------------------------------------------------------
  // [Properties]
  // --------------------------------------------------------------------------

  virtual err_t getProperty(const ManagedString& name, Value& value) const;
  virtual err_t setProperty(const ManagedString& name, const Value& value);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief JPEG, PNG or TIFF parameters.
  GdiPlusCommonParams _params;

  //! @brief IStream bridge.
  IStream* _istream;
};

} // Fog namespace

// [Guard]
#endif // _FOG_G2D_IMAGING_CODECS_GDIPCODEC_P_H
