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
#include <Fog/G2d/Imaging/ImageEncoder.h>

FOG_IMPLEMENT_OBJECT(Fog::ImageEncoder)

namespace Fog {

// ============================================================================
// [Fog::ImageEncoder - Construction / Destruction]
// ============================================================================

ImageEncoder::ImageEncoder(ImageCodecProvider* provider) :
  ImageCodec(provider),
  _headerDone(false),
  _writerDone(false)
{
  _codecType = IMAGE_CODEC_ENCODER;
}

ImageEncoder::~ImageEncoder()
{
}

// ============================================================================
// [Fog::ImageEncoder - Stream]
// ============================================================================

void ImageEncoder::detachStream()
{
  finalize();

  ImageCodec::detachStream();
}

// ============================================================================
// [Fog::ImageEncoder - Reset]
// ============================================================================

void ImageEncoder::reset()
{
  ImageCodec::reset();

  _headerDone = false;
  _writerDone = false;
}

// ============================================================================
// [Fog::ImageEncoder - Finalize]
// ============================================================================

void ImageEncoder::finalize()
{
  _headerDone = false;
  _writerDone = false;
}

} // Fog namespace
