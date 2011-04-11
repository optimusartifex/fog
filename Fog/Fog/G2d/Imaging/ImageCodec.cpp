// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Core/Tools/Strings.h>
#include <Fog/G2d/Global/Constants.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageCodec.h>
#include <Fog/G2d/Imaging/ImageCodecProvider.h>

FOG_IMPLEMENT_OBJECT(Fog::ImageCodec)

namespace Fog {

// ============================================================================
// [Fog::ImageCodec]
// ============================================================================

ImageCodec::ImageCodec(ImageCodecProvider* provider) :
  _provider(provider),
  _streamType(provider->getStreamType()),
  _codecType(IMAGE_CODEC_NONE),
  _attachedOffset(FOG_UINT64_C(0)),
  _stream(),
  _size(0, 0),
  _depth(0),
  _planes(0),
  _progress(0.0f),
  _actualFrame(0),
  _framesCount(0)
{
}

ImageCodec::~ImageCodec()
{
}

// ============================================================================
// [Fog::ImageCodec - Properties]
// ============================================================================

err_t ImageCodec::getProperty(const ManagedString& name, Value& value) const
{
  if (name == fog_strings->getString(STR_G2D_CODEC_width      )) return value.setInt32(_size.w);
  if (name == fog_strings->getString(STR_G2D_CODEC_height     )) return value.setInt32(_size.h);
  if (name == fog_strings->getString(STR_G2D_CODEC_depth      )) return value.setInt32(_depth);
  if (name == fog_strings->getString(STR_G2D_CODEC_planes     )) return value.setInt32(_planes);
  if (name == fog_strings->getString(STR_G2D_CODEC_actualFrame)) return value.setInt32(_actualFrame);
  if (name == fog_strings->getString(STR_G2D_CODEC_framesCount)) return value.setInt32(_framesCount);
  if (name == fog_strings->getString(STR_G2D_CODEC_progress   )) return value.setDouble(_progress);

  return base::getProperty(name, value);
}

err_t ImageCodec::setProperty(const ManagedString& name, const Value& value)
{
  if (name == fog_strings->getString(STR_G2D_CODEC_width) ||
      name == fog_strings->getString(STR_G2D_CODEC_height) ||
      name == fog_strings->getString(STR_G2D_CODEC_depth) ||
      name == fog_strings->getString(STR_G2D_CODEC_planes) ||
      name == fog_strings->getString(STR_G2D_CODEC_actualFrame) ||
      name == fog_strings->getString(STR_G2D_CODEC_framesCount) ||
      name == fog_strings->getString(STR_G2D_CODEC_progress))
  {
    return ERR_OBJECT_READ_ONLY_PROPERTY;
  }

  return base::setProperty(name, value);
}

// ============================================================================
// [Fog::ImageCodec - Progress]
// ============================================================================

void ImageCodec::updateProgress(float value)
{
  _progress = value;
}

void ImageCodec::updateProgress(uint32_t y, uint32_t height)
{
  updateProgress((float)( (double)y / (double)height ));
}

// ============================================================================
// [Fog::ImageCodec - Image Size]
// ============================================================================

bool ImageCodec::checkImageSize() const 
{
  return ((_size.w > 0) & (_size.w <= IMAGE_MAX_WIDTH ) &
          (_size.h > 0) & (_size.h <= IMAGE_MAX_HEIGHT) );
}

// ============================================================================
// [Fog::ImageCodec - Stream]
// ============================================================================

void ImageCodec::attachStream(Stream& stream)
{
  // detach current stream first
  if (_stream.isOpen()) detachStream();

  _stream = stream;
  _attachedOffset = (uint64_t)_stream.tell();
}

void ImageCodec::detachStream()
{
  if (_stream.isOpen())
  {
    reset();
    _stream.close();
  }
}

// ============================================================================
// [Fog::ImageCodec - Reset]
// ============================================================================

void ImageCodec::reset()
{
  _attachedOffset = FOG_UINT64_C(0);

  _size.reset();
  _depth = 0;
  _planes = 0;

  _actualFrame = 0;
  _framesCount = 0;

  _format = IMAGE_FORMAT_NULL;
  _progress = 0.0f;

  _palette.reset();
  _comment.reset();
}

} // Fog namespace
