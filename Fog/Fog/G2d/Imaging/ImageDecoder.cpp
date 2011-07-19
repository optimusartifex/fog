// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>
#include <Fog/G2d/Imaging/ImageDecoder.h>

FOG_IMPLEMENT_OBJECT(Fog::ImageDecoder)

namespace Fog {

// ============================================================================
// [Fog::ImageDecoder - Construction / Destruction]
// ============================================================================

ImageDecoder::ImageDecoder(ImageCodecProvider* provider) :
  ImageCodec(provider),
  _headerDone(false),
  _readerDone(false),
  _headerResult(ERR_OK),
  _readerResult(ERR_OK)
{
  _codecType = IMAGE_CODEC_DECODER;
}

ImageDecoder::~ImageDecoder()
{
}

// ============================================================================
// [Fog::ImageDecoder - Reset]
// ============================================================================

void ImageDecoder::reset()
{
  ImageCodec::reset();

  _headerDone = false;
  _readerDone = false;

  _headerResult = ERR_OK;
  _readerResult = ERR_OK;
}

} // Fog namespace
