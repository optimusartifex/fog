// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
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
  _codecType(IMAGE_CODEC_NONE),
  _streamType(provider->getStreamType()),
  _attachedOffset(FOG_UINT64_C(0)),
  _stream(),
  _size(0, 0),
  _depth(0),
  _planes(0),
  _actualFrame(0),
  _framesCount(0),
  _progress(0.0f)
{
}

ImageCodec::~ImageCodec()
{
}

// ============================================================================
// [Fog::ImageCodec - Properties]
// ============================================================================

err_t ImageCodec::_getProperty(const ManagedStringW& name, Var& dst) const
{
  if (name == FOG_STR_(IMAGE_CODEC_width      )) return dst.setInt(_size.w);
  if (name == FOG_STR_(IMAGE_CODEC_height     )) return dst.setInt(_size.h);
  if (name == FOG_STR_(IMAGE_CODEC_depth      )) return dst.setInt(_depth);
  if (name == FOG_STR_(IMAGE_CODEC_planes     )) return dst.setInt(_planes);
  if (name == FOG_STR_(IMAGE_CODEC_actualFrame)) return dst.setInt(_actualFrame);
  if (name == FOG_STR_(IMAGE_CODEC_framesCount)) return dst.setInt(_framesCount);
  if (name == FOG_STR_(IMAGE_CODEC_progress   )) return dst.setReal(_progress);

  return base::_getProperty(name, dst);
}

err_t ImageCodec::_setProperty(const ManagedStringW& name, const Var& src)
{
  if (name == FOG_STR_(IMAGE_CODEC_width) ||
      name == FOG_STR_(IMAGE_CODEC_height) ||
      name == FOG_STR_(IMAGE_CODEC_depth) ||
      name == FOG_STR_(IMAGE_CODEC_planes) ||
      name == FOG_STR_(IMAGE_CODEC_actualFrame) ||
      name == FOG_STR_(IMAGE_CODEC_framesCount) ||
      name == FOG_STR_(IMAGE_CODEC_progress))
  {
    return ERR_PROPERTY_READ_ONLY;
  }

  return base::_setProperty(name, src);
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
