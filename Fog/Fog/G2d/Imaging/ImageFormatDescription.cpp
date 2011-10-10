// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Memory/BSwap.h>
#include <Fog/G2d/Imaging/ImageFormatDescription.h>

namespace Fog {

// ============================================================================
// [Fog::ImageFormatDescription - Helpers]
// ============================================================================

static uint32_t ImageFormatDescription_detectId(ImageFormatDescription* self)
{
  uint32_t format;
  if (!self->isValid()) return IMAGE_FORMAT_NULL;

  for (format = 0; format < IMAGE_FORMAT_COUNT; format++)
  {
    const ImageFormatDescription* other = &ImageFormatDescription_list[format];

    if (self->_depth == other->_depth && MemOps::eq_s<sizeof(ImageFormatDescription) - 4>(
      reinterpret_cast<const uint8_t*>(self) + 4,
      reinterpret_cast<const uint8_t*>(other) + 4))
    {
      self->_format = format;
      break;
    }
  }

  return self->_format;
}

static err_t ImageFormatDescription_detectIntegerFormatValues(ImageFormatDescription* self)
{
  uint32_t c;

  // Detect alpha component.
  if (self->_aMask != 0)
  {
    self->_componentMask |= IMAGE_COMPONENT_ALPHA;
  }

  // Detect R.G.B components.
  if ((self->_rMask | self->_gMask | self->_bMask) != 0)
  {
    self->_componentMask |= IMAGE_COMPONENT_RGB;

    // Unused compnents?
    if (self->_rMask == 0 || self->_gMask == 0 || self->_bMask == 0)
      goto _Fail;
  }

  // No components?
  if (self->_componentMask == 0)
  {
    goto _Fail;
  }

  // Overlapped components?
  if ((self->_aMask & self->_rMask) != 0 ||
      (self->_gMask & self->_bMask) != 0 ||
      ((self->_aMask | self->_rMask) & (self->_gMask | self->_bMask)) != 0)
  {
    goto _Fail;
  }

  switch (self->_depth)
  {
    case 8:
    {
      break;
    }

    case 16:
    {
      if (self->_aMask > 0x0000FFFFU ||
          self->_rMask > 0x0000FFFFU ||
          self->_gMask > 0x0000FFFFU ||
          self->_bMask > 0x0000FFFFU)
      {
        goto _Fail;
      }
      break;
    }

    case 24:
    {
      if (self->_aMask > 0x00FFFFFFU ||
          self->_rMask > 0x00FFFFFFU ||
          self->_gMask > 0x00FFFFFFU ||
          self->_bMask > 0x00FFFFFFU)
      {
        goto _Fail;
      }
      // ... Fall through ...
    }

    case 32:
    {
      if (self->_aMask > 0xFFFFFFFFU ||
          self->_rMask > 0xFFFFFFFFU ||
          self->_gMask > 0xFFFFFFFFU ||
          self->_bMask > 0xFFFFFFFFU)
      {
        goto _Fail;
      }

      uint32_t am = (uint32_t)self->_aMask;
      uint32_t rm = (uint32_t)self->_rMask;
      uint32_t gm = (uint32_t)self->_gMask;
      uint32_t bm = (uint32_t)self->_bMask;

      self->_hasAlignedComponents =
        (rm == 0xFF000000 || rm == 0x00FF0000 || rm == 0x0000FF00 || rm == 0x000000FF) &&
        (gm == 0xFF000000 || gm == 0x00FF0000 || gm == 0x0000FF00 || gm == 0x000000FF) &&
        (bm == 0xFF000000 || bm == 0x00FF0000 || bm == 0x0000FF00 || bm == 0x000000FF) &&
        (am == 0xFF000000 || am == 0x00FF0000 || am == 0x0000FF00 || am == 0x000000FF || am == 0);
      break;
    }

    case 48:
    {
      if (self->_aMask > FOG_UINT64_C(0x0000FFFFFFFFFFFF) ||
          self->_rMask > FOG_UINT64_C(0x0000FFFFFFFFFFFF) ||
          self->_gMask > FOG_UINT64_C(0x0000FFFFFFFFFFFF) ||
          self->_bMask > FOG_UINT64_C(0x0000FFFFFFFFFFFF))
      {
        goto _Fail;
      }
      // ... Fall through ...
    }

    case 64:
    {
      uint64_t am = self->_aMask;
      uint64_t rm = self->_rMask;
      uint64_t gm = self->_gMask;
      uint64_t bm = self->_bMask;

      self->_hasAlignedComponents =
        (rm == FOG_UINT64_C(0xFFFF000000000000) || rm == FOG_UINT64_C(0x0000FFFF00000000) ||
         rm == FOG_UINT64_C(0x00000000FFFF0000) || rm == FOG_UINT64_C(0x000000000000FFFF)  ) &&
        (gm == FOG_UINT64_C(0xFFFF000000000000) || gm == FOG_UINT64_C(0x0000FFFF00000000) ||
         gm == FOG_UINT64_C(0x00000000FFFF0000) || gm == FOG_UINT64_C(0x000000000000FFFF)  ) &&
        (bm == FOG_UINT64_C(0xFFFF000000000000) || bm == FOG_UINT64_C(0x0000FFFF00000000) ||
         bm == FOG_UINT64_C(0x00000000FFFF0000) || bm == FOG_UINT64_C(0x000000000000FFFF)  ) &&
        (am == FOG_UINT64_C(0xFFFF000000000000) || am == FOG_UINT64_C(0x0000FFFF00000000) ||
         am == FOG_UINT64_C(0x00000000FFFF0000) || am == FOG_UINT64_C(0x000000000000FFFF) || am == 0);
      break;
    }

    default:
    {
      goto _Fail;
    }
  }

  // Try to avoid BSwap when using 24-bit and 32-bit pixel formats. Note that
  // this technique can't be used for 48-bit and 64-bit formats, because it
  // didn't solve the byte-swapping of individual components.
  if (self->_depth == 24 && self->_isByteSwapped && self->_hasAlignedComponents)
  {
    self->_isByteSwapped = 0;
    self->_rMask = Memory::bswap32((uint32_t)self->_rMask) >> 8;
    self->_gMask = Memory::bswap32((uint32_t)self->_gMask) >> 8;
    self->_bMask = Memory::bswap32((uint32_t)self->_bMask) >> 8;
  }

  if (self->_depth == 32 && self->_isByteSwapped && self->_hasAlignedComponents)
  {
    self->_isByteSwapped = 0;

    self->_aMask = Memory::bswap32((uint32_t)self->_aMask);
    self->_rMask = Memory::bswap32((uint32_t)self->_rMask);
    self->_gMask = Memory::bswap32((uint32_t)self->_gMask);
    self->_bMask = Memory::bswap32((uint32_t)self->_bMask);
  }

  for (c = 0; c < 4; c++)
  {
    uint64_t mask = (&self->_aMask)[c];
    uint32_t pos = 0;
    uint32_t size = 0;

    if (mask)
    {
      // Optimization. Maximum allowed mask size is 16-bits per pixel. We can
      // simply skip non-interesting parts of mask by using these conditions.
      if ((mask & FOG_UINT64_C(0x00FFFFFF)) == 0) { mask >>= 24; pos += 24; }
      if ((mask & FOG_UINT64_C(0x0000FFFF)) == 0) { mask >>= 16; pos += 16; }
      if ((mask & FOG_UINT64_C(0x000000FF)) == 0) { mask >>=  8; pos +=  8; }

      // Max 16-bits per mask.
      if (mask > 0xFFFF)
        goto _Fail;

      // 32-bit arithmetic is faster when running on 32-bit.
      uint32_t m = (uint32_t)mask;

      // Get rid off all zero bits.
      while ((m & 0x1) == 0x0) { m >>= 1; pos++; }
      // Calculate ones, this gives us the precision of the component.
      while ((m & 0x1) == 0x1) { m >>= 1; size++; }

      // Detect non-continuous mask, for example [110011] is an invalid mask.
      if (m)
        goto _Fail;

      if (size > 8) self->_precision = IMAGE_PRECISION_WORD;
    }

    (&self->_aPos)[c] = (uint8_t)pos;
    (&self->_aSize)[c] = (uint8_t)size;
  }

  if (self->_isPremultiplied && self->_aSize == 0) self->_isPremultiplied = 0;

  ImageFormatDescription_detectId(self);
  return ERR_OK;

_Fail:
  self->reset();
  return ERR_IMAGE_INVALID_FORMAT;
}

// ============================================================================
// [Fog::ImageFormatDescription - Init]
// ============================================================================

static uint32_t FOG_CDECL ImageFormatDescription_getCompatibleFormat(
  const ImageFormatDescription* self)
{
  if (!self->isValid()) return IMAGE_FORMAT_COUNT;
  if (self->_format < IMAGE_FORMAT_COUNT) return self->_format;

  uint32_t i;

  // First try to match an alternative pixel format (different mask, etc...)
  for (i = 0; i < IMAGE_FORMAT_COUNT; i++)
  {
    const ImageFormatDescription* f = &ImageFormatDescription_list[i];

    if (self->_aSize == f->_aSize &&
        self->_rSize == f->_rSize &&
        self->_gSize == f->_bSize &&
        self->_bSize == f->_bSize &&
        self->_isPremultiplied == f->_isPremultiplied)
    {
      return i;
    }
  }

  const ImageFormatDescription* best = NULL;
  int score = 0;

  // Alternatively, pick the format where there is no loss of information.
  for (i = 0; i < IMAGE_FORMAT_COUNT; i++)
  {
    const ImageFormatDescription* f = &ImageFormatDescription_list[i];

    if (self->_isPremultiplied == f->_isPremultiplied &&
        (f->_aSize > 0) == (self->_aSize > 0) &&
        f->_aSize >= self->_aSize &&
        f->_rSize >= self->_rSize &&
        f->_gSize >= self->_gSize &&
        f->_bSize >= self->_bSize)
    {
      int fscore = f->_aSize - self->_aSize + f->_rSize - self->_rSize +
                   f->_gSize - self->_gSize + f->_bSize - self->_bSize ;

      if ((best == NULL) ||
          (score > fscore) ||
          (score == fscore && f->_depth < best->_depth))
      {
        best = f;
        score = fscore;
      }
    }
  }

  return best ? best->getFormat() : IMAGE_FORMAT_COUNT;
}

static err_t FOG_CDECL ImageFormatDescription_createArgb(
  ImageFormatDescription* self,
  uint32_t depth, uint32_t flags,
  uint64_t aMask, uint64_t rMask, uint64_t gMask, uint64_t bMask)
{
  self->_format = IMAGE_FORMAT_NULL;
  self->_depth = (uint16_t)depth;

  self->_bytesPerPixel = depth / 8;
  self->_precision = IMAGE_PRECISION_BYTE;
  self->_componentMask = IMAGE_COMPONENT_NONE;

  self->_isPremultiplied = aMask ? (flags & IMAGE_FD_IS_PREMULTIPLIED) != 0 : 0;
  self->_isIndexed = 0;
  self->_isByteSwapped = (flags & IMAGE_FD_IS_BYTESWAPPED) != 0;
  self->_hasAlignedComponents = 0;
  self->_reservedBits = 0;

  self->_aMask = aMask;
  self->_rMask = rMask;
  self->_gMask = gMask;
  self->_bMask = bMask;

  return ImageFormatDescription_detectIntegerFormatValues(self);
}

// ============================================================================
// [Fog::ImageFormatDescription - Statics]
// ============================================================================

// ----------------------------------------------------------------------------
// Depth | LITTLE ENDIAN                     | BIG ENGIAN
//       |                                   |
//     8 | [Bi.Gi.Ri.Ai]                     | [Ai.Ri.Gi.Bi]
//    16 | [Bi.Bi.Gi.Gi.Ri.Ri.Ai.Ai]         | [Ai.Ai.Ri.Ri.Gi.Gi.Bi.Bi]
// ----------------------------------------------------------------------------

#define __FOG_IMAGE_FORMAT_INTEGER(_format_, _Depth_, _Premultiplied_, _AlignedComponents_, _FillUnusedBits_, _APos_, _RPos_, _GPos_, _BPos_, _ASize_, _RSize_, _GSize_, _BSize_) { \
  _format_, \
  \
  _Depth_, \
  (_Depth_ + 7) / 8, \
  (_ASize_ <= 8 && _RSize_ <= 8 && _GSize_ <= 8 && _BSize_ <= 8) \
    ? IMAGE_PRECISION_BYTE \
    : IMAGE_PRECISION_WORD, \
  ( (_ASize_ > 0 ? IMAGE_COMPONENT_ALPHA : 0) | \
    (_RSize_ > 0 && _GSize_ > 0 && _BSize_ > 0 ? IMAGE_COMPONENT_RGB : 0) \
  ), \
  \
  _Premultiplied_, \
  0, \
  0, \
  _AlignedComponents_, \
  0, \
  _FillUnusedBits_, \
  \
  _APos_, \
  _RPos_, \
  _GPos_, \
  _BPos_, \
  \
  _ASize_, \
  _RSize_, \
  _GSize_, \
  _BSize_, \
  \
  ((uint64_t)((1 << _ASize_) - 1) << _APos_), \
  ((uint64_t)((1 << _RSize_) - 1) << _RPos_), \
  ((uint64_t)((1 << _GSize_) - 1) << _GPos_), \
  ((uint64_t)((1 << _BSize_) - 1) << _BPos_) \
}

#define __FOG_IMAGE_FORMAT_INDEXED(_format_, _Depth_) { \
  _format_, \
  \
  _Depth_, \
  (_Depth_ + 7) / 8, \
  IMAGE_PRECISION_BYTE, \
  IMAGE_COMPONENT_RGB, \
  \
  0, \
  0, \
  0, \
  0, \
  0, \
  0, \
  \
  0, \
  0, \
  0, \
  0, \
  \
  0, \
  0, \
  0, \
  0, \
  \
  (uint64_t)0, \
  (uint64_t)0, \
  (uint64_t)0, \
  (uint64_t)0 \
}

// ${IMAGE_FORMAT:BEGIN}
const ImageFormatDescription ImageFormatDescription_list[IMAGE_FORMAT_COUNT + 1] =
{
  //                        |Image Format           |Dpt|P|A|F| Position  |   Size    |
  __FOG_IMAGE_FORMAT_INTEGER(IMAGE_FORMAT_PRGB32    , 32,1,1,0,24,16, 8, 0, 8, 8, 8, 8),
  __FOG_IMAGE_FORMAT_INTEGER(IMAGE_FORMAT_XRGB32    , 32,0,1,1, 0,16, 8, 0, 0, 8, 8, 8),
  __FOG_IMAGE_FORMAT_INTEGER(IMAGE_FORMAT_RGB24     , 24,0,1,0, 0,16, 8, 0, 0, 8, 8, 8),

  __FOG_IMAGE_FORMAT_INTEGER(IMAGE_FORMAT_A8        ,  8,0,0,0, 0, 0, 0, 0, 8, 0, 0, 0),
  __FOG_IMAGE_FORMAT_INDEXED(IMAGE_FORMAT_I8        ,  8                              ),

  //                        |Image Format           |Dpt|P|A|F| Position  |   Size    |
  __FOG_IMAGE_FORMAT_INTEGER(IMAGE_FORMAT_PRGB64    , 64,1,0,0,48,24,16, 0,16,16,16,16),
  __FOG_IMAGE_FORMAT_INTEGER(IMAGE_FORMAT_RGB48     , 48,0,0,0, 0,24,16, 0, 0,16,16,16),
  __FOG_IMAGE_FORMAT_INTEGER(IMAGE_FORMAT_A16       , 16,0,0,0, 0, 0, 0, 0,16, 0, 0, 0),

  //
  {
    IMAGE_FORMAT_NULL,
    0,

    0,
    IMAGE_PRECISION_BYTE,
    IMAGE_COMPONENT_NONE,

    0, 0, 0, 0, 0, 0,

    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  }
};
// ${IMAGE_FORMAT:END}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void ImageFormatDescription_init(void)
{
  _api.imageformatdescription_getCompatibleFormat = ImageFormatDescription_getCompatibleFormat;
  _api.imageformatdescription_createArgb = ImageFormatDescription_createArgb;
}

} // Fog namespace
