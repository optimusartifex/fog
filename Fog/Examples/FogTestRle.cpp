#include "Helpers.h"

#include <Fog/Core/Face/FaceC.h>
#include <Fog/Core/Face/FaceSSE2.h>

#include <Fog/G2d/Face/Face_Raster_C.h>
#include <Fog/G2d/Face/Face_Raster_SSE2.h>

// ============================================================================
// [FogTest]
// ============================================================================

using namespace Fog;

// ============================================================================
// [Fog::RLE_IMAGE]
// ============================================================================

enum RLE_IMAGE
{
  RLE_IMAGE_CMD_STOP = 0,
  RLE_IMAGE_CMD_DATA = 1,
  RLE_IMAGE_CMD_FILL = 2,
  RLE_IMAGE_CMD_SKIP = 3,

  RLE_IMAGE_CMD_MASK = 3,
  RLE_IMAGE_MAX_LEN = 63
};

// ============================================================================
// [Fog::RleImageData]
// ============================================================================

struct RleImageData
{
  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  FOG_INLINE RleImageData* addRef() const
  {
    reference.inc();
    return const_cast<RleImageData*>(this);
  }

  FOG_INLINE void release()
  {
    if (reference.deref())
      MemMgr::free(this);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  mutable Atomic<size_t> reference;
  uint32_t vType;
  uint32_t rleLength;

  SizeI size;
  uint8_t* data;
};

// ============================================================================
// [Fog::RleImage]
// ============================================================================

struct RleImage
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  RleImage();
  RleImage(const RleImage& other);
  ~RleImage();

  err_t create(const Image& image);
  void reset();

  RleImage& operator=(const RleImage& other);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  _FOG_CLASS_D(RleImageData)
};

// ============================================================================
// [Fog::RleImage - Global]
// ============================================================================

static Static<RleImageData> RleImage_dEmpty;

// ============================================================================
// [Fog::RleImage - Construction / Destruction]
// ============================================================================

RleImage::RleImage()
{
  _d = RleImage_dEmpty->addRef();
}

RleImage::RleImage(const RleImage& other)
{
  _d = other._d->addRef();
}

RleImage::~RleImage()
{
  _d->release();
}

// ============================================================================
// [Fog::RleImage - Create]
// ============================================================================

template<typename Target>
struct RleProcess : public Target
{
  void process(const uint8_t* sData, const SizeI& size, ssize_t stride)
  {
    uint32_t w = size.w;
    uint32_t h = size.h;

    for (uint32_t y = 0; y < h; y++, sData += stride)
    {
      const uint8_t* sPtr = sData;
      const uint8_t* sEnd = sData + w;

      uint32_t x = 0;
      uint32_t cmd = 0xFF;

      do {
        const uint8_t* sMark = sPtr;
        uint8_t p = sPtr[0];

        if (p > 0x00 && p < 0xFF)
        {
          do {
            if (++sPtr == sEnd)
              break;
          } while (sPtr[0] > 0x00 && sPtr[0] < 0xFF);

          size_t spanLen = (size_t)(sPtr - sMark);
          Target::span(spanLen, sMark);
        }
        else
        {
          do {
            if (++sPtr == sEnd)
              break;
          } while (sPtr[0] == p);

          if (sPtr == sEnd)
            break;

          size_t spanLen = (size_t)(sPtr - sMark);
          if (p == 0xFF)
            Target::fill(spanLen);
          else
            Target::skip(spanLen);
        }
      } while (sPtr != sEnd);
      Target::stop();
    }
  }
};

struct RleAnalyze
{
  FOG_INLINE RleAnalyze() : total(0) {}

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  FOG_INLINE void stop()
  {
    total++;
  }

  FOG_INLINE void span(size_t spanLen, const uint8_t* spanMask)
  {
    do {
      size_t rleLen = Math::min<size_t>(spanLen, RLE_IMAGE_MAX_LEN);
      total += 1 + rleLen;
      spanLen -= rleLen;
    } while (spanLen > 0);
  }

  FOG_INLINE void fill(size_t spanLen)
  {
    do {
      size_t rleLen = Math::min<size_t>(spanLen, RLE_IMAGE_MAX_LEN);
      total++;
      spanLen -= rleLen;
    } while (spanLen > 0);
  }

  FOG_INLINE void skip(size_t spanLen)
  {
    do {
      size_t rleLen = Math::min<size_t>(spanLen, RLE_IMAGE_MAX_LEN);
      total++;
      spanLen -= rleLen;
    } while (spanLen > 0);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  size_t total;
};

struct RleCompress
{
  FOG_INLINE RleCompress() : dData(NULL) {}

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  FOG_INLINE void stop()
  {
    dData[0] = RLE_IMAGE_CMD_STOP;
    dData++;
  }

  FOG_INLINE void span(size_t spanLen, const uint8_t* spanMask)
  {
    do {
      size_t rleLen = Math::min<size_t>(spanLen, RLE_IMAGE_MAX_LEN);

      dData[0] = (uint8_t)(rleLen << 2) | RLE_IMAGE_CMD_DATA;
      dData++;

      memcpy(dData, spanMask, rleLen);
      dData += rleLen;

      spanLen -= rleLen;
    } while (spanLen > 0);
  }

  FOG_INLINE void fill(size_t spanLen)
  {
    do {
      size_t rleLen = Math::min<size_t>(spanLen, RLE_IMAGE_MAX_LEN);

      dData[0] = (uint8_t)(rleLen << 2) | RLE_IMAGE_CMD_FILL;
      dData++;

      spanLen -= rleLen;
    } while (spanLen > 0);
  }

  FOG_INLINE void skip(size_t spanLen)
  {
    do {
      size_t rleLen = Math::min<size_t>(spanLen, RLE_IMAGE_MAX_LEN);

      dData[0] = (uint8_t)(rleLen << 2) | RLE_IMAGE_CMD_SKIP;
      dData++;

      spanLen -= rleLen;
    } while (spanLen > 0);
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint8_t* dData;
};

err_t RleImage::create(const Image& image)
{
  SizeI size = image.getSize();

  if (size.w == 0 || image.getFormat() != IMAGE_FORMAT_A8)
  {
    reset();
    return ERR_OK;
  }

  const uint8_t* sData = image.getFirst();
  ssize_t stride = image.getStride();

  // Analyze.
  RleProcess<RleAnalyze> analyzer;
  analyzer.process(sData, size, stride);

  RleImageData* d = reinterpret_cast<RleImageData*>(MemMgr::alloc(sizeof(RleImageData) + analyzer.total));
  if (FOG_IS_NULL(d))
  {
    reset();
    return ERR_RT_OUT_OF_MEMORY;
  }

  d->reference.init(1);
  d->vType = 0;
  d->rleLength = (uint32_t)analyzer.total;
  d->size = size;
  d->data = reinterpret_cast<uint8_t*>(d) + sizeof(RleImageData);

  RleProcess<RleCompress> compressor;
  compressor.dData = d->data;
  compressor.process(sData, size, stride);

  atomicPtrXchg(&_d, d)->release();
  return ERR_OK;
}

// ============================================================================
// [RleImage - Reset]
// ============================================================================

void RleImage::reset()
{
  atomicPtrXchg(&this->_d, RleImage_dEmpty->addRef())->release();
}

// ============================================================================
// [RleImage - Copy]
// ============================================================================

RleImage& RleImage::operator=(const RleImage& other)
{
  atomicPtrXchg(&this->_d, other._d->addRef())->release();
  return *this;
}

// ============================================================================
// [Draw]
// ============================================================================

struct Draw
{
  void clear(const Argb32& color);

  void glyph(const PointI& pt, const Image& glyph, const Argb32& color);
  void glyph(const PointI& pt, const RleImage& glyph, const Argb32& color);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Image image;
};

void Draw::clear(const Argb32& color)
{
  image.clear(Color(color));
}

void Draw::glyph(const PointI& pt, const Image& glyph, const Argb32& color)
{
  ImageData* d = image._d;
  ImageData* s = glyph._d;

  int x0 = pt.x;
  int y0 = pt.y;

  int x1 = x0 + s->size.w;
  int y1 = y0 + s->size.h;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;

  if (x1 > d->size.w) x1 = d->size.w;
  if (y1 > d->size.h) y1 = d->size.h;

  if (y0 >= y1 || x0 >= x1)
    return;

  int w = x1 - x0;

  ssize_t dStride = d->stride;
  ssize_t sStride = s->stride;

  uint8_t* dData = d->first + (y0       ) * dStride + (x0       ) * 4;
  uint8_t* sData = s->first + (y0 - pt.y) * sStride + (x0 - pt.x) * 1;

  dStride -= w * 4;
  sStride -= w * 1;

  Face::p32 sro0p;
  Face::p32 sro0p_20, sro0p_31;

  Face::p32PRGB32FromARGB32(sro0p, color.p32);
  Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

  do {
    for (int i = 0; i < w; i++)
    {
      Face::p32 dst0p;
      Face::p32 src0p;
      Face::p32 msk0p;

      Face::p32Load1b(msk0p, sData);
      if (msk0p == 0x00) goto _A8_Glyph_Skip;

      Face::p32Copy(src0p, sro0p);
      if (msk0p == 0xFF) goto _A8_Glyph_Fill;

      Face::p32Load4a(dst0p, dData);

      Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
      Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);

      Face::p32Negate256SBW(msk0p, msk0p);
      Face::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
      Face::p32Add(src0p, src0p, dst0p);

_A8_Glyph_Fill:
      Face::p32Store4a(dData, src0p);

_A8_Glyph_Skip:
      dData += 4;
      sData += 1;
    }

    dData += dStride;
    sData += sStride;
  } while (++y0 != y1);
}

#if 0
void Draw::glyph(const PointI& pt, const RleImage& glyph, const Argb32& color)
{
  ImageData* d = image._d;
  RleImageData* s = glyph._d;

  int x0 = pt.x;
  int y0 = pt.y;

  int x1 = x0 + s->size.w;
  int y1 = y0 + s->size.h;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;

  if (x1 > d->size.w) x1 = d->size.w;
  if (y1 > d->size.h) y1 = d->size.h;

  if (y0 >= y1 || x0 >= x1)
    return;

  int w = x1 - x0;

  ssize_t dStride = d->stride;
  uint8_t* dBase = d->first + (y0) * dStride + (x0) * 4;
  uint8_t* sData = s->data;

  Face::p32 sro0p;
  Face::p32 sro0p_20, sro0p_31;

  Face::p32PRGB32FromARGB32(sro0p, color.p32);
  Face::p32UnpackPBWFromPBB_2031(sro0p_20, sro0p_31, sro0p);

  uint8_t* dData = dBase;

  for (;;)
  {
    uint32_t cmd = sData[0];
    uint32_t len = cmd >> 2;

    sData += 1;

    switch (cmd & RLE_IMAGE_CMD_MASK)
    {
      case RLE_IMAGE_CMD_DATA:
        do {
          Face::p32 dst0p;
          Face::p32 src0p;
          Face::p32 msk0p;

          Face::p32Load1b(msk0p, sData);
          Face::p32Load4a(dst0p, dData);

          Face::p32Cvt256SBWFrom255SBW(msk0p, msk0p);
          Face::p32MulDiv256PBW_SBW_2x_Pack_2031(src0p, sro0p_20, msk0p, sro0p_31, msk0p);

          Face::p32Negate256SBW(msk0p, msk0p);
          Face::p32MulDiv256PBB_SBW(dst0p, dst0p, msk0p);
          Face::p32Add(src0p, src0p, dst0p);

          Face::p32Store4a(dData, src0p);
          dData += 4;
          sData += 1;
        } while (--len);
        break;

      case RLE_IMAGE_CMD_FILL:
        do {
          Face::p32Store4a(dData, sro0p);
          dData += 4;
        } while (--len);
        break;

      case RLE_IMAGE_CMD_SKIP:
        dData += len * 4;
        break;

      case RLE_IMAGE_CMD_STOP:
        if (++y0 == y1)
          return;

        dBase += dStride;
        dData = dBase;
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }
}
#endif

#if 1
void Draw::glyph(const PointI& pt, const RleImage& glyph, const Argb32& color)
{
  ImageData* d = image._d;
  RleImageData* s = glyph._d;

  int x0 = pt.x;
  int y0 = pt.y;

  int x1 = x0 + s->size.w;
  int y1 = y0 + s->size.h;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;

  if (x1 > d->size.w) x1 = d->size.w;
  if (y1 > d->size.h) y1 = d->size.h;

  if (y0 >= y1 || x0 >= x1)
    return;

  int w = x1 - x0;

  ssize_t dStride = d->stride;
  uint8_t* dBase = d->first + (y0) * dStride + (x0) * 4;
  uint8_t* sData = s->data;

  Face::prefetchT0(dBase);
  Face::prefetchT0(sData);

  Face::m128i sro0;
  Face::m128i sru0;
  Face::m128iLoad4(sro0, &color.p32);

  Face::m128iUnpackPI16FromPI8Lo(sro0, sro0);
  Face::m128iShufflePI16Lo<3, 3, 3, 3>(sru0, sro0);
  Face::m128iFillPBWi<3>(sro0, sro0);

  Face::m128iMulDiv255PI16(sro0, sro0, sru0);
  Face::m128iPackPU8FromPU16(sro0, sro0);
  Face::m128iShufflePI32<0, 0, 0, 0>(sro0, sro0);

  Face::m128iUnpackPI16FromPI8Lo(sru0, sro0);

  uint8_t* dData = dBase;
  Face::prefetchT0(dBase + dStride);

  for (;;)
  {
    uint32_t cmd = sData[0];
    uint32_t len = cmd >> 2;

    sData += 1;

    switch (cmd & RLE_IMAGE_CMD_MASK)
    {
      case RLE_IMAGE_CMD_DATA:
        if (len >= 4)
        {
          if (((size_t)dData & 0xF) != 0)
          {
            Face::m128i dst0, dst1;
            Face::m128i src0, src1;
            Face::m128i msk0, msk1;

            Face::m128iLoad4(msk0, sData);
            Face::m128iLoad8(dst0, dData + 0);
            Face::m128iLoad8(dst1, dData + 8);

            Face::m128iUnpackPI16FromPI8Lo(msk0, msk0);
            Face::m128iUnpackPI16FromPI8Lo(dst0, dst0);

            Face::m128iUnpackPI32FromPI16Lo(msk0, msk0, msk0);
            Face::m128iUnpackPI16FromPI8Lo(dst1, dst1);

            msk0 = _mm_slli_epi16(msk0, 4);

            Face::m128iCopy(src0, sru0);
            Face::m128iShufflePI32<3, 3, 2, 2>(msk1, msk0);

            Face::m128iCopy(src1, sru0);
            Face::m128iShufflePI32<1, 1, 0, 0>(msk0, msk0);

            Face::m128iSubPI16(src0, src0, dst0);
            Face::m128iSubPI16(src1, src1, dst1);

            src0 = _mm_slli_epi16(src0, 4);
            src1 = _mm_slli_epi16(src1, 4);

            src0 = _mm_mulhi_epi16(src0, msk0);
            src1 = _mm_mulhi_epi16(src1, msk1);

            Face::m128iAddPI16(dst0, dst0, src0);
            Face::m128iAddPI16(dst1, dst1, src1);

            Face::m128iPackPU8FromPU16(dst0, dst0, dst1);

            uint32_t i = (uint32_t)(16 - ((size_t)dData & 0xF)) >> 2;
            FOG_ASSERT(i < 4);

            len -= i;
            sData += i;

            do {
              Face::m128iStore4(dData, dst0);
              Face::m128iRShiftSU128<32>(dst0, dst0);
              dData += 4;
            } while (--i);
          }

          while (len >= 4)
          {
            Face::m128i dst0, dst1;
            Face::m128i src0, src1;
            Face::m128i msk0, msk1;

            Face::m128iLoad4(msk0, sData);
            Face::m128iLoad16a(dst0, dData);

            Face::m128iUnpackPI16FromPI8Lo(msk0, msk0);
            Face::m128iUnpackPI16FromPI8Hi(dst1, dst0);

            Face::m128iUnpackPI32FromPI16Lo(msk0, msk0, msk0);
            Face::m128iUnpackPI16FromPI8Lo(dst0, dst0);

            msk0 = _mm_slli_epi16(msk0, 4);

            Face::m128iCopy(src0, sru0);
            Face::m128iShufflePI32<3, 3, 2, 2>(msk1, msk0);

            Face::m128iCopy(src1, sru0);
            Face::m128iShufflePI32<1, 1, 0, 0>(msk0, msk0);

            Face::m128iSubPI16(src0, src0, dst0);
            Face::m128iSubPI16(src1, src1, dst1);

            src0 = _mm_slli_epi16(src0, 4);
            src1 = _mm_slli_epi16(src1, 4);

            src0 = _mm_mulhi_epi16(src0, msk0);
            src1 = _mm_mulhi_epi16(src1, msk1);

            Face::m128iAddPI16(dst0, dst0, src0);
            Face::m128iAddPI16(dst1, dst1, src1);

            Face::m128iPackPU8FromPU16(dst0, dst0, dst1);
            Face::m128iStore16a(dData, dst0);

            dData += 16;
            sData += 4;
            len -= 4;
          }

_Small:
          if (!len) break;
        }

        do {
          Face::m128i dst0;
          Face::m128i src0;
          Face::m128i msk0;

          Face::m128iLoad1(msk0, sData);
          Face::m128iLoad4(dst0, dData);

          msk0 = _mm_slli_epi16(msk0, 4);
          Face::m128iUnpackPI16FromPI8Lo(dst0, dst0);

          Face::m128iExtendPI16FromSI16Lo(msk0, msk0);
          Face::m128iCopy(src0, sru0);

          Face::m128iSubPI16(src0, src0, dst0);
          src0 = _mm_slli_epi16(src0, 4);
          src0 = _mm_mulhi_epi16(src0, msk0);

          Face::m128iAddPI16(dst0, dst0, src0);

          Face::m128iPackPU8FromPU16(dst0, dst0);
          Face::m128iStore4(dData, dst0);

          dData += 4;
          sData += 1;
        } while (--len);
        break;

      case RLE_IMAGE_CMD_FILL:
        if (len >= 20)
        {
          uint8_t* dEnd = dData + len * 4;

          Face::m128iStore16u(dData, sro0);
          dData = (uint8_t*)( ((size_t)dData + 16) & ~(size_t)15 );

          dEnd -= 64;

          while (dData <= dEnd)
          {
            Face::m128iStore16a(dData +  0, sro0);
            Face::m128iStore16a(dData + 16, sro0);
            Face::m128iStore16a(dData + 32, sro0);
            Face::m128iStore16a(dData + 48, sro0);

            dData += 64;
          }

          dEnd += 48;
          while (dData <= dEnd)
          {
            Face::m128iStore16a(dData +  0, sro0);
            dData += 16;
          }

          dData = dEnd + 16;
          Face::m128iStore16u(dEnd, sro0);
        }
        else
        {
          do {
            Face::m128iStore4(dData, sro0);
            dData += 4;
          } while (--len);
        }
        break;

      case RLE_IMAGE_CMD_SKIP:
        dData += len * 4;
        break;

      case RLE_IMAGE_CMD_STOP:
        if (++y0 == y1)
          return;

        dBase += dStride;
        dData = dBase;
        Face::prefetchT0(dBase + dStride);
        break;

      default:
        FOG_ASSERT_NOT_REACHED();
    }
  }
}
#endif

// ============================================================================
// [MyWindow]
// ============================================================================

struct Item
{
  PointI pos;
  PointI adv;
  Argb32 color;

  Image glyph;
  RleImage rle;
};
_FOG_TYPE_DECLARE(Item, TYPE_CATEGORY_MOVABLE)

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onKey(KeyEvent* e);
  virtual void onMouse(MouseEvent* e);
  virtual void onTimer(TimerEvent* e);
  virtual void onPaint(PaintEvent* e);

  // RLE.
  void generate();
  void advance();
  void updateTitle();
  void doPaint();

  List<Item> items;
  Draw draw;
  bool _useRLE;

  uint32_t imageSize;
  uint32_t rleSize;

  // FPS...
  Timer timer;
  float fps;
  float fpsCounter;
  Time fpsTime;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  timer.setInterval(TimeDelta::fromMilliseconds(0));
  timer.addListener(EVENT_TIMER, this, &MyWindow::onTimer);

  draw.image.create(SizeI(600, 600), IMAGE_FORMAT_XRGB32);
  _useRLE = false;

  fps = 0.0f;
  fpsCounter = 0.0f;

  setWindowTitle(StringW::fromAscii8("Rle-Text"));
  generate();
  doPaint();
}

MyWindow::~MyWindow()
{
}

void MyWindow::onKey(KeyEvent* e)
{
  if (e->getCode() == EVENT_KEY_PRESS)
  {
    switch (e->getKey())
    {
      case KEY_SPACE:
        if (timer.isRunning())
        {
          timer.stop();
        }
        else
        {
          timer.start();
          fps = 0.0f;
          fpsCounter = 0.0f;
          fpsTime = Time::now();
        }
        break;

      case KEY_ENTER:
        _useRLE ^= 1;

        doPaint();
        update(WIDGET_UPDATE_ALL);
        updateTitle();
        break;
    }
  }

  base::onKey(e);
}

void MyWindow::onMouse(MouseEvent* e)
{
  base::onMouse(e);
}

void MyWindow::onTimer(TimerEvent* e)
{
  advance();
  doPaint();

  update(WIDGET_UPDATE_PAINT);
}

void MyWindow::onPaint(PaintEvent* e)
{
  Painter* p = e->getPainter();

  //p->setSource(Argb32(0xFFFFFFFF));
  //p->fillAll();

  p->blitImage(PointI(0, 0), draw.image);
}

void MyWindow::generate()
{
  size_t length = 10000;

  imageSize = 0;
  rleSize = 0;

  for (size_t i = 0; i < length; i++)
  {
    Font font = getFont();
    font.setHeight(100, UNIT_PX);

    CharW c('A' + rand() % 26);

    PathD path;
    font.getTextOutline(path, CONTAINER_OP_APPEND, PointD(0.0, 0.0), StubW(&c, 1));
    PathStrokerD stroker;

    PathD pathS;
    stroker.strokePath(pathS, path);

    PointI offset;
    Item item;

    item.pos.set(rand() % 200, rand() % 200);
    item.adv.set((rand() % 7) - 3, rand() % 7 - 3);

    if (item.adv.x == 0) item.adv.x++;
    if (item.adv.y == 0) item.adv.y++;

    Image::glyphFromPath(item.glyph, offset, path, FILL_RULE_NON_ZERO, IMAGE_PRECISION_BYTE);
    item.rle.create(item.glyph);

    item.color.p32 = Argb32::fromAhsv(AhsvF(
      1.0f,
      float((rand() & 0xFFFF)) / 65535.0f,
      float((rand() & 0xFFFF)) / 65535.0f,
      1.0f
    ));

    imageSize += item.glyph.getWidth() * item.glyph.getHeight();
    rleSize += item.rle._d->rleLength;

    items.append(item);
  }
}

void MyWindow::advance()
{
  SizeI size = draw.image.getSize();
  size_t length = items.getLength();

  for (size_t i = 0; i < length; i++)
  {
    Item& item = (Item&)items[i];
    item.pos += item.adv;

    if (item.pos.x < 0 || item.pos.x + item.glyph.getWidth() > size.w)
    {
      item.adv.x *= -1;
      item.pos.x += item.adv.x;
    }

    if (item.pos.y < 0 || item.pos.y + item.glyph.getHeight() > size.h)
    {
      item.adv.y *= -1;
      item.pos.y += item.adv.y;
    }

    if (rand() % 100 == 0)
    {
      item.adv.set((rand() % 7) - 3, rand() % 7 - 3);
      if (item.adv.x == 0) item.adv.x++;
      if (item.adv.y == 0) item.adv.y++;
    }
  }
}

void MyWindow::doPaint()
{
  draw.clear(Argb32(0xFF000000));
  Time startTime = Time::now();

  if (_useRLE)
  {
    size_t length = items.getLength();
    for (size_t i = 0; i < length; i++)
    {
      draw.glyph(items[i].pos, items[i].rle, items[i].color);
    }
  }
  else
  {
    size_t length = items.getLength();
    for (size_t i = 0; i < length; i++)
    {
      draw.glyph(items[i].pos, items[i].glyph, items[i].color);
    }
  }

  Time lastTime = Time::now();

  TimeDelta frameDelta = lastTime - startTime;
  TimeDelta fpsDelta = lastTime - fpsTime;

  if (fpsDelta.getMillisecondsD() >= 1000.0f)
  {
    fps = fpsCounter;
    fpsCounter = 0.0f;
    fpsTime = lastTime;

    updateTitle();
  }
  else
  {
    fpsCounter++;
  }
}

void MyWindow::updateTitle()
{
  StringW text;
  text.format("FPS: %d (%s) == %d glyphs/s | Image: %uB | RLE: %uB.",
    (int)fps,
    _useRLE ? "RLE-Image" : "Raster-Image",
    (int)(fps * items.getLength()),
    imageSize,
    rleSize);
  setWindowTitle(text);
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_UI_MAIN()
{
  RleImage_dEmpty->reference.init(1);
  RleImage_dEmpty->vType = 0;
  RleImage_dEmpty->size.reset();
  RleImage_dEmpty->data = NULL;

  //Api* api = &_api;
  //printf("%p", api);

  Application app(StringW::fromAscii8("UI"));
  MyWindow window(WINDOW_TYPE_DEFAULT);

  window.addListener(EVENT_CLOSE, &app, &Application::quit);
  window.setSize(SizeI(window.draw.image.getWidth() + 20, window.draw.image.getHeight() + 40));
  window.show();

  return app.run();
}
















#if 0


enum SPAN_CMD
{
  // --------------------------------------------------------------------------
  // [Control]
  // --------------------------------------------------------------------------

  //! @brief RLE skip.
  //!
  //! This command describes how many pixels to skip.
  SPAN_CMD_SKIP = 0,

  // --------------------------------------------------------------------------
  // [Span-CMask]
  // --------------------------------------------------------------------------

  //! @brief Const-alpha mask.
  SPAN_CMD_CONST = 1,

  // --------------------------------------------------------------------------
  // [Span-VMask]
  // --------------------------------------------------------------------------

  //! @brief Variable-alpha mask (8-bit A8).
  //!
  //! @note Must be always implemented.
  SPAN_CMD_A8 = 2,

  //! @brief Variable-alpha mask (8-bit extended A8 stored as 16-bit).
  //!
  //! @note Must be always implemented.
  SPAN_CMD_A8X = 3,

  //! @brief Variable-alpha mask (16-bit A16).
  //!
  //! @note Forbidden when blending on 8-bit surfaces.
  SPAN_CMD_A16 = 4,

  //! @brief Variable-alpha mask (32-bit ARGB32).
  //!
  //! @note Must be always implemented.
  SPAN_CMD_ARGB32 = 5,

  //! @brief Variable-alpha mask (32-bit extended ARGB32 stored as 64-bit).
  //!
  //! @note Must be always implemented.
  SPAN_CMD_ARGB32X = 6,

  //! @brief Variable-alpha mask (64-bit ARGB64).
  //!
  //! @note Forbidden when blending on 8-bit surfaces.
  SPAN_CMD_ARGB64 = 7,

  //! @brief Count of RLE commands.
  SPAN_CMD_COUNT = 8,

#if FOG_ARCH_BITS >= 64
  RLE_MAX_LENGTH = 0xFFFFFFFF
#else
  RLE_MAX_LENGTH = 0x1FFFFFFF
#endif
};

struct FOG_NO_EXPORT Span
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getCmd() const { return _cmd; }
  FOG_INLINE uint32_t getLen() const { return _len; }

  FOG_INLINE uint32_t getCMask() const { return (uint32_t_cMask; }
  FOG_INLINE uint8_t* getVMask() const { return _vMask; }

  FOG_INLINE void setCmd(uint32_t cmd) { _cmd = cmd; }
  FOG_INLINE void setLen(uint32_t len) { _len = len; }

  FOG_INLINE void setCMask(uint32_t m) { _cMask = m; }
  FOG_INLINE void setVMask(uint8_t* m) { _vMask = m; }

  FOG_INLINE void setCSpan(uint32_t cmd, uint32_t len, uint32_t m)
  {
    _cmd = cmd;
    _len = len;
    _cMask = m;
  }

  FOG_INLINE void setVSpan(uint32_t cmd, uint32_t len, uint8_t* m)
  {
    _cmd = cmd;
    _len = len;
    _vMask = m;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

#if FOG_ARCH_BITS >= 64
  uint32_t _cmd;
  uint32_t _len;
#else
  uint32_t _cmd : 3;
  uint32_t _len : 29;
#endif

  union
  {
    size_t _cMask;
    uint8_t* _vMask;
  };
};

struct FOG_NO_EXPORT Scanline
{
  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getX() const { return _x; }
  FOG_INLINE uint32_t getCount() const { return _count; }

  FOG_INLINE Span* getData() { return _data; }
  FOG_INLINE const Span* getData() const { return _data; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  uint32_t _x;
  uint32_t _count;
  Span _data[1];
};

void test(const Scanline *sl, uint8_t** pDst)
{
  uint32_t x = sl->getX();
  uint32_t i = sl->getCount();
  const Span* span = sl->getData();

  do {
    if (span->getCmd() == SPAN_CMD_SKIP)
    {
    }
    else
    {
    }

    span++;
  } while (--i);
}
#endif
