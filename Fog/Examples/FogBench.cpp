#include <Fog/Build/Build.h>

#if defined(FOG_OS_WINDOWS)
#define FOG_BENCH_GDIPLUS
#else
#define FOG_BENCH_CAIRO
#endif

#define FOG_BENCH_DONT_SAVE

#if defined(FOG_BENCH_GDIPLUS)
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#endif // FOG_BENCH_HDIPLUS

#if defined(FOG_BENCH_CAIRO)
#include <cairo/cairo.h>
#endif // FOG_BENCH_CAIRO

#include <Fog/Core.h>
#include <Fog/Graphics.h>

#include <stdio.h>
#include <stdlib.h>

using namespace Fog;

// ============================================================================
// [Sprites]
// ============================================================================

#define NUM_SPRITES 4

static Image _sprite[NUM_SPRITES];

static void loadSprites()
{
  static const char* spriteNames[NUM_SPRITES] =
  {
    "babelfish.png",
    "blockdevice.png",
    "drop.png",
    "kweather.png"
  };

  bool spritesNotFound = false;
  int i;

  for (i = 0; i < NUM_SPRITES; i++)
  {
    if (_sprite[i].readFile(Ascii8(spriteNames[i])) != ERR_OK)
    {
      fog_debug("Warning: Can't load sprite './%s'", spriteNames[i]);
      spritesNotFound = true;
    }
    else
    {
      _sprite[i].premultiply();
    }
  }

  if (spritesNotFound)
  {
    fog_debug("\nDownload sprites from these locations and place them to 'FogBench' working directory:");
    for (i = 0; i < NUM_SPRITES; i++)
    {
      fog_debug("  http://kobalicek.com/data/fog/sprites/%s", spriteNames[i]);
    }
    fog_debug("\nAll image based tests will be incorrect!\n");
  }
}

// ============================================================================
// [Logging]
// ============================================================================

static void log(const String& s)
{
  ByteArray b;
  TextCodec::local8().fromUnicode(b, s);
  fprintf(stderr, "%s", b.getData());
}

// ============================================================================
// [Randomizer Tools]
// ============================================================================

static FOG_INLINE double randDouble()
{
  return (double)(rand() & (0xFFFF>>1)) / (double)(0xFFFF>>1);
}

static FOG_INLINE uint32_t randColor()
{
  return (rand() & 0xFFFF) | (rand() << 16);
}

static FOG_INLINE IntRect randRect(int w, int h, int rw, int rh)
{
  return IntRect(rand() % (w - rw), rand() % (h - rh), rw, rh);
}

static FOG_INLINE IntPoint randPoint(int w, int h)
{
  return IntPoint(rand() % w, rand() % h);
}

static FOG_INLINE DoublePoint randPointD(int w, int h)
{
  return DoublePoint(randDouble() * (double)w, randDouble() * (double)h);
}

// ============================================================================
// [Randomizer_Argb]
// ============================================================================

struct Randomizer_Argb
{
  Randomizer_Argb();
  ~Randomizer_Argb();

  void init(int quantity);
  void free();

  Argb* data;
};

Randomizer_Argb::Randomizer_Argb() : data(NULL) {}
Randomizer_Argb::~Randomizer_Argb() { free(); }

void Randomizer_Argb::init(int quantity)
{
  free();
  data = (Argb*)Memory::alloc(sizeof(Argb) * quantity);
  for (int a = 0; a < quantity; a++) data[a] = randColor();
}

void Randomizer_Argb::free()
{
  Memory::free(data);
  data = NULL;
}

// ============================================================================
// [Randomizer_Number]
// ============================================================================

struct Randomizer_Number
{
  Randomizer_Number();
  ~Randomizer_Number();

  void init(int quantity, int min_, int max_);
  void free();

  int* data;
};

Randomizer_Number::Randomizer_Number() : data(NULL) {}
Randomizer_Number::~Randomizer_Number() { free(); }

void Randomizer_Number::init(int quantity, int min_, int max_)
{
  free();
  data = (int*)Memory::alloc(sizeof(int) * quantity);

  for (int a = 0; a < quantity; a++)
  {
    data[a] = (int)(randDouble() * (double)(max_ - min_) + min_);
  }
}

void Randomizer_Number::free()
{
  Memory::free(data);
  data = NULL;
}

// ============================================================================
// [Randomizer_Rect]
// ============================================================================

struct Randomizer_Rect
{
  Randomizer_Rect();
  ~Randomizer_Rect();

  void init(int quantity, int w, int h, int sw, int sh);
  void free();

  IntRect* data;
};

Randomizer_Rect::Randomizer_Rect() : data(NULL) {}
Randomizer_Rect::~Randomizer_Rect() { free(); }

void Randomizer_Rect::init(int quantity, int w, int h, int sw, int sh)
{
  free();
  data = (IntRect*)Memory::alloc(sizeof(IntRect) * quantity);
  for (int a = 0; a < quantity; a++) data[a] = randRect(w, h, sw, sh);
}

void Randomizer_Rect::free()
{
  Memory::free(data);
  data = NULL;
}

// ============================================================================
// [Randomizer_Polygon]
// ============================================================================

struct Randomizer_Polygon
{
  Randomizer_Polygon();
  ~Randomizer_Polygon();

  void init(int quantity, int w, int h, int sw, int sh);
  void free();

  DoublePoint* data;
};

Randomizer_Polygon::Randomizer_Polygon() : data(NULL) {}
Randomizer_Polygon::~Randomizer_Polygon() { free(); }

void Randomizer_Polygon::init(int quantity, int w, int h, int sw, int sh)
{
  free();
  data = (DoublePoint*)Memory::alloc(sizeof(DoublePoint) * quantity * 10);

  double x = 0.0;
  double y = 0.0;

  for (int a = 0; a < quantity * 10; a++)
  {
    if ((a % 10) == 0)
    {
      x = randDouble() * (double)(w - sw);
      y = randDouble() * (double)(h - sh);
    }
    data[a] = randPointD(sw, sh).translated(x, y);
  }
}

void Randomizer_Polygon::free()
{
  Memory::free(data);
  data = NULL;
}

// ============================================================================
// [AbstractModule]
// ============================================================================

struct AbstractModule
{
  AbstractModule(int w, int h);
  virtual ~AbstractModule();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();
  virtual void bench(int quantity) = 0;

  virtual void save(int sw, int sh);

  virtual ByteArray getEngine() = 0;
  virtual ByteArray getType() = 0;
  
  virtual ByteArray getInfo();

  int w, h;

  Image screen;
  Image sprite[4];
};

AbstractModule::AbstractModule(int w, int h) : w(w), h(h)
{
  screen.create(w, h, PIXEL_FORMAT_PRGB32);
  screen.clear(0x00000000);

  for (int a = 0; a < NUM_SPRITES; a++) sprite[a] = _sprite[a];
}

AbstractModule::~AbstractModule()
{
}

void AbstractModule::prepare(int quantity, int sw, int sh) {}
void AbstractModule::finish() {}

void AbstractModule::save(int sw, int sh)
{
#if !defined(FOG_BENCH_DONT_SAVE)
  String fileName;
  ByteArray info = getInfo();

  fileName.set(Ascii8("Images/Bench - "));
  fileName.append(Ascii8(info.getData(), info.getLength()));
  if (sw && sh) fileName.appendFormat(" [%0.3dx%0.3d]", sw, sh);
  fileName.append(Ascii8(".bmp"));

  Image t(screen);
  t.forceFormat(PIXEL_FORMAT_XRGB32);
  t.writeFile(fileName);
#endif
}

ByteArray AbstractModule::getInfo()
{
  ByteArray i = getEngine();
  i.append(" - ");
  i.append(getType());
  return i;
}

// ============================================================================
// [FogModule]
// ============================================================================

struct FogModule : public AbstractModule
{
  FogModule(int w, int h);
  virtual ~FogModule();

  virtual ByteArray getEngine();
  void setEngine(int engine);

  virtual void configurePainter(Painter& p);

  int engine;
};

FogModule::FogModule(int w, int h) : AbstractModule(w, h)
{
  engine = PAINTER_ENGINE_RASTER_ST;
}

FogModule::~FogModule()
{
}

ByteArray FogModule::getEngine()
{
  ByteArray info;
  info.format("Fog-%s", 
    engine == PAINTER_ENGINE_RASTER_MT ? "mt" : "st");
  return info;
}

void FogModule::setEngine(int engine)
{
  this->engine = engine;
}

void FogModule::configurePainter(Painter& p)
{
  p.setEngine(engine);
}

// ============================================================================
// [FogModule_CreateDestroy]
// ============================================================================

struct FogModule_CreateDestroy : public FogModule
{
  FogModule_CreateDestroy();
  virtual ~FogModule_CreateDestroy();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

FogModule_CreateDestroy::FogModule_CreateDestroy() : FogModule(0, 0) {}
FogModule_CreateDestroy::~FogModule_CreateDestroy() {}

void FogModule_CreateDestroy::bench(int quantity)
{
  for (int a = 0; a < quantity; a++)
  {
    Painter p(screen, NO_FLAGS);
    configurePainter(p);
  }
}

ByteArray FogModule_CreateDestroy::getType()
{
  return ByteArray("Create-Destroy");
}

// ============================================================================
// [FogModule_FillRect]
// ============================================================================

struct FogModule_FillRect : public FogModule
{
  FogModule_FillRect(int w, int h);
  virtual ~FogModule_FillRect();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

FogModule_FillRect::FogModule_FillRect(int w, int h) : FogModule(w, h) {}
FogModule_FillRect::~FogModule_FillRect() {}

void FogModule_FillRect::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void FogModule_FillRect::finish()
{
  r_rect.free();
  r_argb.free();
}

void FogModule_FillRect::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  for (int a = 0; a < quantity; a++)
  {
    p.setSource(r_argb.data[a]);
    p.fillRect(r_rect.data[a]);
  }
}

ByteArray FogModule_FillRect::getType()
{
  return ByteArray("FillRect");
}

// ============================================================================
// [FogModule_FillRectAffine]
// ============================================================================

struct FogModule_FillRectAffine : public FogModule_FillRect
{
  FogModule_FillRectAffine(int w, int h);
  virtual ~FogModule_FillRectAffine();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

FogModule_FillRectAffine::FogModule_FillRectAffine(int w, int h) : FogModule_FillRect(w, h) {}
FogModule_FillRectAffine::~FogModule_FillRectAffine() {}

void FogModule_FillRectAffine::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  for (int a = 0; a < quantity; a++, rot += 0.01)
  {
    DoubleMatrix m;
    m.translate(cx, cy);
    m.rotate(rot);
    m.translate(-cx, -cy);

    p.setMatrix(m);
    p.setSource(r_argb.data[a]);
    p.fillRect(r_rect.data[a]);
  }
}

ByteArray FogModule_FillRectAffine::getType()
{
  return ByteArray("FillRectAffine");
}

// ============================================================================
// [FogModule_FillRound]
// ============================================================================

struct FogModule_FillRound : public FogModule_FillRect
{
  FogModule_FillRound(int w, int h);
  virtual ~FogModule_FillRound();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect randomizer;
};

FogModule_FillRound::FogModule_FillRound(int w, int h) : FogModule_FillRect(w, h) {}
FogModule_FillRound::~FogModule_FillRound() {}

void FogModule_FillRound::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  for (int a = 0; a < quantity; a++)
  {
    p.setSource(r_argb.data[a]);
    p.fillRound(r_rect.data[a], IntPoint(8, 8));
  }
}

ByteArray FogModule_FillRound::getType()
{
  return ByteArray("FillRound");
}

// ============================================================================
// [FogModule_FillPolygon]
// ============================================================================

struct FogModule_FillPolygon : public FogModule
{
  FogModule_FillPolygon(int w, int h);
  virtual ~FogModule_FillPolygon();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Polygon r_poly;
  Randomizer_Argb r_argb;
};

FogModule_FillPolygon::FogModule_FillPolygon(int w, int h) : FogModule(w, h) {}
FogModule_FillPolygon::~FogModule_FillPolygon() {}

void FogModule_FillPolygon::prepare(int quantity, int sw, int sh)
{
  r_poly.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void FogModule_FillPolygon::finish()
{
  r_poly.free();
  r_argb.free();
}

void FogModule_FillPolygon::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  p.setFillMode(FILL_EVEN_ODD);
  DoublePath path;

  for (int a = 0; a < quantity; a++)
  {
    const DoublePoint* polyData = &r_poly.data[a * 10];

    path.clear();
    for (int i = 0; i < 10; i++)
    {
      DoublePoint c0 = polyData[i];
      if (i == 0)
        path.moveTo(c0);
      else
        path.lineTo(c0);
    }

    p.setSource(r_argb.data[a]);
    p.fillPath(path);
  }
}

ByteArray FogModule_FillPolygon::getType()
{
  return ByteArray("FillPolygon");
}

// ============================================================================
// [FogModule_FillPattern]
// ============================================================================

struct FogModule_FillPattern : public FogModule
{
  FogModule_FillPattern(int w, int h);
  virtual ~FogModule_FillPattern();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  void setupPattern(int type);

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Pattern pattern;

  Randomizer_Rect r_rect;
};

FogModule_FillPattern::FogModule_FillPattern(int w, int h) : FogModule(w, h)
{
  setupPattern(PATTERN_LINEAR_GRADIENT);
}

FogModule_FillPattern::~FogModule_FillPattern()
{
}

void FogModule_FillPattern::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
}

void FogModule_FillPattern::finish()
{
  r_rect.free();
}

void FogModule_FillPattern::setupPattern(int type)
{
  pattern.setType(type);

  if (type == PATTERN_LINEAR_GRADIENT)
  {
    pattern.setPoints(DoublePoint(w/2.0, h/2.0), DoublePoint(30.0, 30.0));
  }
  else
  {
    pattern.setPoints(DoublePoint(w/2.0, h/2.0), DoublePoint(w/3.0, h/3.0));
    pattern.setRadius(w/4.0);
  }

  pattern.addStop(ArgbStop(0.0, Argb(0xFFFFFFFF)));
  pattern.addStop(ArgbStop(0.5, Argb(0xFFFFFF00)));
  pattern.addStop(ArgbStop(1.0, Argb(0xFF000000)));
}

void FogModule_FillPattern::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  p.setSource(pattern);

  for (int a = 0; a < quantity; a++)
  {
    p.fillRect(r_rect.data[a]);
  }
}

ByteArray FogModule_FillPattern::getType()
{
  const char* p = "";

  switch (pattern.getType())
  {
    case PATTERN_SOLID: p = "Solid"; break;
    case PATTERN_TEXTURE: p = "Texture"; break;
    case PATTERN_LINEAR_GRADIENT: p = "LinearGradient"; break;
    case PATTERN_RADIAL_GRADIENT: p = "RadialGradient"; break;
    case PATTERN_CONICAL_GRADIENT: p = "ConicalGradient"; break;
  }

  return ByteArray(p);
}

// ============================================================================
// [FogModule_Image]
// ============================================================================

struct FogModule_Image : public FogModule
{
  FogModule_Image(int w, int h);
  virtual ~FogModule_Image();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  Randomizer_Number r_numb;

  Image images[NUM_SPRITES];
};

FogModule_Image::FogModule_Image(int w, int h) : FogModule(w, h) {}
FogModule_Image::~FogModule_Image() {}

void FogModule_Image::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_numb.init(quantity, 0, NUM_SPRITES - 1);

  for (int a = 0; a < 4; a++) images[a] = sprite[a].scale(IntSize(sw, sh));
}

void FogModule_Image::finish()
{
  r_rect.free();
  r_numb.free();
}

void FogModule_Image::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  for (int a = 0; a < quantity; a++)
  {
    p.blitImage(r_rect.data[a].getPosition(), images[r_numb.data[a]]);
  }
}

ByteArray FogModule_Image::getType()
{
  return ByteArray("Image");
}

// ============================================================================
// [FogModule_ImageAffine]
// ============================================================================

struct FogModule_ImageAffine : public FogModule_Image
{
  FogModule_ImageAffine(int w, int h);
  virtual ~FogModule_ImageAffine();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

FogModule_ImageAffine::FogModule_ImageAffine(int w, int h) : FogModule_Image(w, h) {}
FogModule_ImageAffine::~FogModule_ImageAffine() {}

void FogModule_ImageAffine::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  for (int a = 0; a < quantity; a++, rot += 0.01)
  {
    DoubleMatrix m;
    m.translate(cx, cy);
    m.rotate(rot);
    m.translate(-cx, -cy);

    p.setMatrix(m);
    p.blitImage(r_rect.data[a].getPosition(), images[r_numb.data[a]]);
  }
}

ByteArray FogModule_ImageAffine::getType()
{
  return ByteArray("ImageAffine");
}

// ============================================================================
// [FogModule_RasterText]
// ============================================================================

struct FogModule_RasterText : public FogModule
{
  FogModule_RasterText(int w, int h);
  virtual ~FogModule_RasterText();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

FogModule_RasterText::FogModule_RasterText(int w, int h) : FogModule(w, h) {}
FogModule_RasterText::~FogModule_RasterText() {}

void FogModule_RasterText::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void FogModule_RasterText::finish()
{
  r_rect.free();
  r_argb.free();
}

void FogModule_RasterText::bench(int quantity)
{
  Painter p(screen, NO_FLAGS);
  configurePainter(p);

  String text(Ascii8("abcdef"));
  Font font;

  for (int a = 0; a < quantity; a++)
  {
    p.setSource(r_argb.data[a]);
    p.drawText(r_rect.data[a].getPosition(), text, font);
  }
}

ByteArray FogModule_RasterText::getType()
{
  return ByteArray("RasterText");
}

// ============================================================================
// [GdiPlusModule]
// ============================================================================

#if defined(FOG_BENCH_GDIPLUS)

struct GdiPlusModule : public AbstractModule
{
  GdiPlusModule(int w, int h);
  virtual ~GdiPlusModule();

  Gdiplus::Bitmap* screen_gdip;
  Gdiplus::Bitmap* sprite_gdip[4];

  virtual ByteArray getEngine();
};

GdiPlusModule::GdiPlusModule(int w, int h) : AbstractModule(w, h)
{
  screen_gdip = new Gdiplus::Bitmap(
    screen.getWidth(),
    screen.getHeight(), 
    screen.getStride(),
    PixelFormat32bppPARGB,
    (BYTE*)screen.getData());

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    sprite_gdip[a] = new Gdiplus::Bitmap(
      sprite[a].getWidth(),
      sprite[a].getHeight(), 
      sprite[a].getStride(),
      PixelFormat32bppPARGB,
      (BYTE*)sprite[a].getData());
  }
}

GdiPlusModule::~GdiPlusModule()
{
  delete screen_gdip;

  for (int a = 0; a < NUM_SPRITES; a++) delete sprite_gdip[a];
}

ByteArray GdiPlusModule::getEngine()
{
  return ByteArray("GDI+");
}

// ============================================================================
// [GdiPlusModule_CreateDestroy]
// ============================================================================

struct GdiPlusModule_CreateDestroy : public GdiPlusModule
{
  GdiPlusModule_CreateDestroy();
  virtual ~GdiPlusModule_CreateDestroy();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

GdiPlusModule_CreateDestroy::GdiPlusModule_CreateDestroy() : GdiPlusModule(0, 0) {}
GdiPlusModule_CreateDestroy::~GdiPlusModule_CreateDestroy() {}

void GdiPlusModule_CreateDestroy::bench(int quantity)
{
  for (int a = 0; a < quantity; a++)
  {
    Gdiplus::Graphics gr(screen_gdip);
  }
}

ByteArray GdiPlusModule_CreateDestroy::getType()
{
  return ByteArray("Create-Destroy");
}

// ============================================================================
// [GdiPlusModule_FillRect]
// ============================================================================

struct GdiPlusModule_FillRect : public GdiPlusModule
{
  GdiPlusModule_FillRect(int w, int h);
  virtual ~GdiPlusModule_FillRect();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

GdiPlusModule_FillRect::GdiPlusModule_FillRect(int w, int h) : GdiPlusModule(w, h) {}
GdiPlusModule_FillRect::~GdiPlusModule_FillRect() {}

void GdiPlusModule_FillRect::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void GdiPlusModule_FillRect::finish()
{
  r_rect.free();
  r_argb.free();
}

void GdiPlusModule_FillRect::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);

  for (int a = 0; a < quantity; a++)
  {
    IntRect r = r_rect.data[a];

    Gdiplus::Color c(r_argb.data[a]);
    Gdiplus::SolidBrush br(c);

    gr.FillRectangle((Gdiplus::Brush*)&br, r.x, r.y, r.w, r.h);
  }
}

ByteArray GdiPlusModule_FillRect::getType()
{
  return ByteArray("FillRect");
}

// ============================================================================
// [GdiPlusModule_FillRectAffine]
// ============================================================================

struct GdiPlusModule_FillRectAffine : public GdiPlusModule_FillRect
{
  GdiPlusModule_FillRectAffine(int w, int h);
  virtual ~GdiPlusModule_FillRectAffine();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

GdiPlusModule_FillRectAffine::GdiPlusModule_FillRectAffine(int w, int h) : GdiPlusModule_FillRect(w, h) {}
GdiPlusModule_FillRectAffine::~GdiPlusModule_FillRectAffine() {}

void GdiPlusModule_FillRectAffine::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);

  float cx = (float)w / 2.0f;
  float cy = (float)h / 2.0f;
  float rot = 0.0f;

  for (int a = 0; a < quantity; a++, rot += 0.01f)
  {
    IntRect r = r_rect.data[a];

    gr.ResetTransform();
    gr.TranslateTransform((Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
    gr.RotateTransform(Math::rad2deg(rot));
    gr.TranslateTransform((Gdiplus::REAL)-cx, (Gdiplus::REAL)-cy);

    Gdiplus::Color c(r_argb.data[a]);
    Gdiplus::SolidBrush br(c);

    gr.FillRectangle((Gdiplus::Brush*)&br, r.x, r.y, r.w, r.h);
  }
}

ByteArray GdiPlusModule_FillRectAffine::getType()
{
  return ByteArray("FillRectAffine");
}

// ============================================================================
// [GdiPlusModule_FillRound]
// ============================================================================

struct GdiPlusModule_FillRound : public GdiPlusModule_FillRect
{
  GdiPlusModule_FillRound(int w, int h);
  virtual ~GdiPlusModule_FillRound();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect randomizer;
};

GdiPlusModule_FillRound::GdiPlusModule_FillRound(int w, int h) : GdiPlusModule_FillRect(w, h) {}
GdiPlusModule_FillRound::~GdiPlusModule_FillRound() {}

void GdiPlusModule_FillRound::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  gr.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

  for (int a = 0; a < quantity; a++)
  {
    IntRect r = r_rect.data[a];

    Gdiplus::Color c(r_argb.data[a]);
    Gdiplus::SolidBrush br(c);

    Gdiplus::REAL r_x = (Gdiplus::REAL)r.x - (Gdiplus::REAL)0.5;
    Gdiplus::REAL r_y = (Gdiplus::REAL)r.y - (Gdiplus::REAL)0.5;
    Gdiplus::REAL r_w = (Gdiplus::REAL)r.w;
    Gdiplus::REAL r_h = (Gdiplus::REAL)r.h;
    Gdiplus::REAL d = 8;

    Gdiplus::GraphicsPath path;
    path.AddArc(r_x, r_y, d, d, 180.0f, 90.0f);
    path.AddArc(r_x + r_w - d, r_y, d, d, 270.0f, 90.0f);
    path.AddArc(r_x + r_w - d, r_y + r_h - d, d, d, 0.0f, 90.0f);
    path.AddArc(r_x, r_y + r_h - d, d, d, 90.0f, 90.0f);
    path.AddLine(r_x, r_y + r_h - d, r_x, r_y + d/2);
    gr.FillPath((Gdiplus::Brush*)&br, &path);
  }
}

ByteArray GdiPlusModule_FillRound::getType()
{
  return ByteArray("FillRound");
}

// ============================================================================
// [GdiPlusModule_FillPolygon]
// ============================================================================

struct GdiPlusModule_FillPolygon : public GdiPlusModule
{
  GdiPlusModule_FillPolygon(int w, int h);
  virtual ~GdiPlusModule_FillPolygon();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Polygon r_poly;
  Randomizer_Argb r_argb;
};

GdiPlusModule_FillPolygon::GdiPlusModule_FillPolygon(int w, int h) : GdiPlusModule(w, h) {}
GdiPlusModule_FillPolygon::~GdiPlusModule_FillPolygon() {}

void GdiPlusModule_FillPolygon::prepare(int quantity, int sw, int sh)
{
  r_poly.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void GdiPlusModule_FillPolygon::finish()
{
  r_poly.free();
  r_argb.free();
}

void GdiPlusModule_FillPolygon::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  gr.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

  for (int a = 0; a < quantity; a++)
  {
    const DoublePoint* polyData = &r_poly.data[a*10];

    Gdiplus::GraphicsPath path;
    Gdiplus::PointF lines[10];
    for (int i = 0; i < 10; i++)
    {
      lines[i].X = (Gdiplus::REAL)polyData[i].x - (Gdiplus::REAL)0.5;
      lines[i].Y = (Gdiplus::REAL)polyData[i].y - (Gdiplus::REAL)0.5;
    }
    path.AddLines(lines, 10);
    path.CloseFigure();

    Gdiplus::Color c(r_argb.data[a]);
    Gdiplus::SolidBrush br(c);

    gr.FillPath((Gdiplus::Brush*)&br, &path);
  }
}

ByteArray GdiPlusModule_FillPolygon::getType()
{
  return ByteArray("FillPolygon");
}

// ============================================================================
// [GdiPlusModule_FillPattern]
// ============================================================================

struct GdiPlusModule_FillPattern : public GdiPlusModule
{
  GdiPlusModule_FillPattern(int w, int h);
  virtual ~GdiPlusModule_FillPattern();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  void setupPattern(int type);

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  int type;
  Gdiplus::Brush* brush;
};

GdiPlusModule_FillPattern::GdiPlusModule_FillPattern(int w, int h) :
  GdiPlusModule(w, h), brush(NULL), type(PATTERN_NULL) {}
GdiPlusModule_FillPattern::~GdiPlusModule_FillPattern() { if (brush) delete brush; }

void GdiPlusModule_FillPattern::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
}

void GdiPlusModule_FillPattern::finish()
{
  r_rect.free();
}

void GdiPlusModule_FillPattern::setupPattern(int type_)
{
  if (brush) delete brush;

  type = type_;

  Gdiplus::Color clr[3];
  Gdiplus::REAL stops[3];

  stops[0] = 0.0; clr[0].SetValue(0xFFFFFFFF);
  stops[1] = 0.5; clr[1].SetValue(0xFFFFFF00);
  stops[2] = 1.0; clr[2].SetValue(0xFF000000);

  if (type_ == PATTERN_LINEAR_GRADIENT)
  {
    Gdiplus::LinearGradientBrush* mybrush = new Gdiplus::LinearGradientBrush(
      Gdiplus::PointF((Gdiplus::REAL)(w/2.0), (Gdiplus::REAL)(h/2.0)),
      Gdiplus::PointF(30.0f, 30.0f), 
      Gdiplus::Color(0xFFFFFFFF), Gdiplus::Color(0xFF000000));
    mybrush->SetInterpolationColors(clr, stops, 3);

    brush = mybrush;
  }
  else
  {
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float r = w / 4.0f;

    Gdiplus::GraphicsPath path;
    path.AddEllipse(Gdiplus::RectF(cx - r, cy - r, r * 2.0f, r * 2.0f));

    Gdiplus::PathGradientBrush* mybrush = new Gdiplus::PathGradientBrush(&path);
    mybrush->SetInterpolationColors(clr, stops, 3);
    mybrush->SetCenterPoint(Gdiplus::PointF(w / 3.0f, h / 3.0f));

    brush = mybrush;
  }
}

void GdiPlusModule_FillPattern::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);

  for (int a = 0; a < quantity; a++)
  {
    IntRect r = r_rect.data[a];
    gr.FillRectangle(brush, r.x, r.y, r.w, r.h);
  }
}

ByteArray GdiPlusModule_FillPattern::getType()
{
  const char* p = "";

  switch (type)
  {
    case PATTERN_SOLID: p = "Solid"; break;
    case PATTERN_TEXTURE: p = "Texture"; break;
    case PATTERN_LINEAR_GRADIENT: p = "LinearGradient"; break;
    case PATTERN_RADIAL_GRADIENT: p = "RadialGradient"; break;
    case PATTERN_CONICAL_GRADIENT: p = "ConicalGradient"; break;
  }

  return ByteArray(p);
}

// ============================================================================
// [GdiPlusModule_Image]
// ============================================================================

struct GdiPlusModule_Image : public GdiPlusModule
{
  GdiPlusModule_Image(int w, int h);
  virtual ~GdiPlusModule_Image();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  Randomizer_Number r_numb;

  Image images[NUM_SPRITES];
  Gdiplus::Bitmap* images_gdip[NUM_SPRITES];
};

GdiPlusModule_Image::GdiPlusModule_Image(int w, int h) : GdiPlusModule(w, h) {}
GdiPlusModule_Image::~GdiPlusModule_Image() {}

void GdiPlusModule_Image::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_numb.init(quantity, 0, NUM_SPRITES - 1);

  for (int a = 0; a < 4; a++)
  {
    images[a] = sprite[a].scale(IntSize(sw, sh));
    images_gdip[a] = new Gdiplus::Bitmap(
      images[a].getWidth(),
      images[a].getHeight(), 
      images[a].getStride(),
      PixelFormat32bppPARGB,
      (BYTE*)images[a].getData());
  }
}

void GdiPlusModule_Image::finish()
{
  r_rect.free();
  r_numb.free();

  for (int a = 0; a < 4; a++)
  {
    delete images_gdip[a];
  }
}

void GdiPlusModule_Image::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);

  for (int a = 0; a < quantity; a++)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;
    gr.DrawImage(images_gdip[r_numb.data[a]], x, y);
  }
}

ByteArray GdiPlusModule_Image::getType()
{
  return ByteArray("Image");
}

// ============================================================================
// [GdiPlusModule_ImageAffine]
// ============================================================================

struct GdiPlusModule_ImageAffine : public GdiPlusModule_Image
{
  GdiPlusModule_ImageAffine(int w, int h);
  virtual ~GdiPlusModule_ImageAffine();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

GdiPlusModule_ImageAffine::GdiPlusModule_ImageAffine(int w, int h) : GdiPlusModule_Image(w, h) {}
GdiPlusModule_ImageAffine::~GdiPlusModule_ImageAffine() {}

void GdiPlusModule_ImageAffine::bench(int quantity)
{
  Gdiplus::Graphics gr(screen_gdip);
  gr.SetInterpolationMode(Gdiplus::InterpolationModeBilinear);

  float cx = (float)w / 2.0f;
  float cy = (float)h / 2.0f;
  float rot = 0.0f;

  for (int a = 0; a < quantity; a++, rot += 0.01f)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;

    gr.ResetTransform();
    gr.TranslateTransform((Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
    gr.RotateTransform(Math::rad2deg(rot));
    gr.TranslateTransform((Gdiplus::REAL)-cx, (Gdiplus::REAL)-cy);

    gr.DrawImage(images_gdip[r_numb.data[a]], x, y);
  }
}

ByteArray GdiPlusModule_ImageAffine::getType()
{
  return ByteArray("ImageAffine");
}

#endif // FOG_BENCH_GDIPLUS

// ============================================================================
// [CairoModule]
// ============================================================================

#if defined(FOG_BENCH_CAIRO)

struct CairoModule : public FogModule
{
  CairoModule(int w, int h);
  virtual ~CairoModule();

  virtual ByteArray getEngine();

  cairo_surface_t* screen_cairo;
};

CairoModule::CairoModule(int w, int h) : FogModule(w, h)
{
  screen_cairo = cairo_image_surface_create_for_data(
    (unsigned char*)screen.getFirst(),
    CAIRO_FORMAT_ARGB32,
    screen.getWidth(),
    screen.getHeight(),
    screen.getStride());
}

CairoModule::~CairoModule()
{
  cairo_surface_destroy(screen_cairo);
}

ByteArray CairoModule::getEngine()
{
  return ByteArray("Cairo");
}

// ============================================================================
// [CairoModule_CreateDestroy]
// ============================================================================

struct CairoModule_CreateDestroy : public CairoModule
{
  CairoModule_CreateDestroy();
  virtual ~CairoModule_CreateDestroy();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

CairoModule_CreateDestroy::CairoModule_CreateDestroy() : CairoModule(0, 0) {}
CairoModule_CreateDestroy::~CairoModule_CreateDestroy() {}

void CairoModule_CreateDestroy::bench(int quantity)
{
  for (int a = 0; a < quantity; a++)
  {
    cairo_t* cr = cairo_create(screen_cairo);
    cairo_destroy(cr);
  }
}

ByteArray CairoModule_CreateDestroy::getType()
{
  return ByteArray("Create-Destroy");
}

// ============================================================================
// [CairoModule_FillRect]
// ============================================================================

struct CairoModule_FillRect : public CairoModule
{
  CairoModule_FillRect(int w, int h);
  virtual ~CairoModule_FillRect();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  Randomizer_Argb r_argb;
};

CairoModule_FillRect::CairoModule_FillRect(int w, int h) : CairoModule(w, h) {}
CairoModule_FillRect::~CairoModule_FillRect() {}

void CairoModule_FillRect::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void CairoModule_FillRect::finish()
{
  r_rect.free();
  r_argb.free();
}

void CairoModule_FillRect::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);

  for (int a = 0; a < quantity; a++)
  {
    IntRect r(r_rect.data[a]);
    Argb c(r_argb.data[a]);

    cairo_set_source_rgba(cr,
      (double)c.r / 255.0,
      (double)c.g / 255.0,
      (double)c.b / 255.0,
      (double)c.a / 255.0);
    cairo_rectangle(cr, r.x, r.y, r.w, r.h);
    cairo_fill(cr);
  }

  cairo_destroy(cr);
}

ByteArray CairoModule_FillRect::getType()
{
  return ByteArray("FillRect");
}

// ============================================================================
// [CairoModule_FillRectAffine]
// ============================================================================

struct CairoModule_FillRectAffine : public CairoModule_FillRect
{
  CairoModule_FillRectAffine(int w, int h);
  virtual ~CairoModule_FillRectAffine();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

CairoModule_FillRectAffine::CairoModule_FillRectAffine(int w, int h) : CairoModule_FillRect(w, h) {}
CairoModule_FillRectAffine::~CairoModule_FillRectAffine() {}

void CairoModule_FillRectAffine::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  for (int a = 0; a < quantity; a++, rot += 0.01)
  {
    IntRect r(r_rect.data[a]);
    Argb c(r_argb.data[a]);

    cairo_identity_matrix(cr);
    cairo_translate(cr, cx, cy);
    cairo_rotate(cr, rot);
    cairo_translate(cr, -cx, -cy);

    cairo_set_source_rgba(cr,
      (double)c.r / 255.0,
      (double)c.g / 255.0,
      (double)c.b / 255.0,
      (double)c.a / 255.0);
    cairo_rectangle(cr, r.x, r.y, r.w, r.h);
    cairo_fill(cr);
  }

  cairo_destroy(cr);
}

ByteArray CairoModule_FillRectAffine::getType()
{
  return ByteArray("FillRectAffine");
}

// ============================================================================
// [CairoModule_FillRound]
// ============================================================================

struct CairoModule_FillRound : public CairoModule_FillRect
{
  CairoModule_FillRound(int w, int h);
  virtual ~CairoModule_FillRound();

  virtual void bench(int quantity);
  ByteArray getType();

  void addRound(cairo_t* cr, IntRect rect, double radius);
};

CairoModule_FillRound::CairoModule_FillRound(int w, int h) : CairoModule_FillRect(w, h) {}
CairoModule_FillRound::~CairoModule_FillRound() {}

void CairoModule_FillRound::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);

  for (int a = 0; a < quantity; a++)
  {
    IntRect r(r_rect.data[a]);
    Argb c(r_argb.data[a]);

    cairo_set_source_rgba(cr,
      (double)c.r / 255.0,
      (double)c.g / 255.0,
      (double)c.b / 255.0,
      (double)c.a / 255.0);
    addRound(cr, r, 8);
    cairo_fill(cr);
  }

  cairo_destroy(cr);
}

ByteArray CairoModule_FillRound::getType()
{
  return ByteArray("FillRound");
}

void CairoModule_FillRound::addRound(cairo_t* cr, IntRect rect, double radius)
{
  double x0 = rect.x;
  double y0 = rect.y;
  double rect_width = rect.w;
  double rect_height = rect.h;

  double x1 = x0 + rect_width;
  double y1 = y0 + rect_height;

  radius *= 2.0;

  if (!rect_width || !rect_height) return;

  if (rect_width / 2 < radius)
  {
    if (rect_height / 2 < radius)
    {
      cairo_move_to(cr, x0, (y0 + y1)/2);
      cairo_curve_to(cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
      cairo_curve_to(cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
    }
    else
    {
      cairo_move_to(cr, x0, y0 + radius);
      cairo_curve_to(cr, x0 ,y0, x0, y0, (x0 + x1)/2, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, y0 + radius);
      cairo_line_to(cr, x1 , y1 - radius);
      cairo_curve_to(cr, x1, y1, x1, y1, (x1 + x0)/2, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, y1- radius);
    }
  }
  else
  {
    if (rect_height / 2 < radius)
    {
      cairo_move_to(cr, x0, (y0 + y1)/2);
      cairo_curve_to(cr, x0 , y0, x0 , y0, x0 + radius, y0);
      cairo_line_to(cr, x1 - radius, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, (y0 + y1)/2);
      cairo_curve_to(cr, x1, y1, x1, y1, x1 - radius, y1);
      cairo_line_to(cr, x0 + radius, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, (y0 + y1)/2);
    }
    else
    {
      cairo_move_to(cr, x0, y0 + radius);
      cairo_curve_to(cr, x0 , y0, x0 , y0, x0 + radius, y0);
      cairo_line_to(cr, x1 - radius, y0);
      cairo_curve_to(cr, x1, y0, x1, y0, x1, y0 + radius);
      cairo_line_to(cr, x1 , y1 - radius);
      cairo_curve_to(cr, x1, y1, x1, y1, x1 - radius, y1);
      cairo_line_to(cr, x0 + radius, y1);
      cairo_curve_to(cr, x0, y1, x0, y1, x0, y1- radius);
    }
  }
  cairo_close_path(cr);
}

// ============================================================================
// [CairoModule_FillPolygon]
// ============================================================================

struct CairoModule_FillPolygon : public CairoModule
{
  CairoModule_FillPolygon(int w, int h);
  virtual ~CairoModule_FillPolygon();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Polygon r_poly;
  Randomizer_Argb r_argb;
};

CairoModule_FillPolygon::CairoModule_FillPolygon(int w, int h) : CairoModule(w, h) {}
CairoModule_FillPolygon::~CairoModule_FillPolygon() {}

void CairoModule_FillPolygon::prepare(int quantity, int sw, int sh)
{
  r_poly.init(quantity, w, h, sw, sh);
  r_argb.init(quantity);
}

void CairoModule_FillPolygon::finish()
{
  r_poly.free();
  r_argb.free();
}

void CairoModule_FillPolygon::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);

  for (int a = 0; a < quantity; a++)
  {
    Argb c = r_argb.data[a];
    cairo_set_source_rgba(cr,
      (double)c.r / 255.0,
      (double)c.g / 255.0,
      (double)c.b / 255.0,
      (double)c.a / 255.0);

    const DoublePoint* polyData = &r_poly.data[a*10];
    for (int i = 0; i < 10; i++)
    {
      if (i == 0)
        cairo_move_to(cr, polyData[i].x, polyData[i].y);
      else
        cairo_line_to(cr, polyData[i].x, polyData[i].y);
    }
    cairo_close_path(cr);
    cairo_fill(cr);
  }

  cairo_destroy(cr);
}

ByteArray CairoModule_FillPolygon::getType()
{
  return ByteArray("FillPolygon");
}

// ============================================================================
// [AbstractModule_FillPattern]
// ============================================================================

struct CairoModule_FillPattern : public CairoModule
{
  CairoModule_FillPattern(int w, int h);
  virtual ~CairoModule_FillPattern();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  int type;
};

CairoModule_FillPattern::CairoModule_FillPattern(int w, int h) :
  CairoModule(w, h),
  type(0)
{
}

CairoModule_FillPattern::~CairoModule_FillPattern()
{
}

void CairoModule_FillPattern::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
}

void CairoModule_FillPattern::finish()
{
  r_rect.free();
}

void CairoModule_FillPattern::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);
  cairo_pattern_t *pat;

  if (type == 0)
  {
    pat = cairo_pattern_create_linear(w/2, h/2, 30.0, 30.0);
  }
  else if (type == 1)
  {
    pat = cairo_pattern_create_radial(w/2, h/2, 250.0, 30.0, 30.0, 1.0);
  }

  cairo_pattern_add_color_stop_rgba(pat, 0.0, 1, 1, 1, 1);
  cairo_pattern_add_color_stop_rgba(pat, 0.5, 1, 1, 0, 1);
  cairo_pattern_add_color_stop_rgba(pat, 1.0, 0, 0, 0, 1);
  cairo_set_source(cr, pat);

  for (int a = 0; a < quantity; a++)
  {
    IntRect r = r_rect.data[a];
    cairo_rectangle(cr, r.x, r.y, r.w, r.h);
    cairo_fill(cr);
  }

  cairo_destroy(cr);
  cairo_pattern_destroy(pat);
}

ByteArray CairoModule_FillPattern::getType()
{
  switch (type)
  {
    case 0: return ByteArray("LinearGradient");
    case 1: return ByteArray("RadialGradient");
    default: return ByteArray("");
  }
}

// ============================================================================
// [CairoModule_Image]
// ============================================================================

struct CairoModule_Image : public CairoModule
{
  CairoModule_Image(int w, int h);
  virtual ~CairoModule_Image();

  virtual void prepare(int quantity, int sw, int sh);
  virtual void finish();

  virtual void bench(int quantity);
  virtual ByteArray getType();

  Randomizer_Rect r_rect;
  Randomizer_Number r_numb;

  Image images[NUM_SPRITES];
  cairo_surface_t* images_cairo[NUM_SPRITES];
};

CairoModule_Image::CairoModule_Image(int w, int h) : CairoModule(w, h) {}
CairoModule_Image::~CairoModule_Image() {}

void CairoModule_Image::prepare(int quantity, int sw, int sh)
{
  r_rect.init(quantity, w, h, sw, sh);
  r_numb.init(quantity, 0, NUM_SPRITES - 1);

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    images[a] = sprite[a].scale(IntSize(sw, sh));
    images_cairo[a] = cairo_image_surface_create_for_data(
      (unsigned char*)images[a].getFirst(),
      CAIRO_FORMAT_ARGB32,
      images[a].getWidth(),
      images[a].getHeight(),
      images[a].getStride());
  }
}

void CairoModule_Image::finish()
{
  r_rect.free();
  r_numb.free();

  for (int a = 0; a < NUM_SPRITES; a++)
  {
    cairo_surface_destroy(images_cairo[a]);
  }
}

void CairoModule_Image::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);

  for (int a = 0; a < quantity; a++)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;
    cairo_set_source_surface(cr, images_cairo[r_numb.data[a]], x, y);
    cairo_paint(cr);
  }

  cairo_destroy(cr);
}

ByteArray CairoModule_Image::getType()
{
  return ByteArray("Image");
}

// ============================================================================
// [CairoModule_ImageAffine]
// ============================================================================

struct CairoModule_ImageAffine : public CairoModule_Image
{
  CairoModule_ImageAffine(int w, int h);
  virtual ~CairoModule_ImageAffine();

  virtual void bench(int quantity);
  virtual ByteArray getType();
};

CairoModule_ImageAffine::CairoModule_ImageAffine(int w, int h) : CairoModule_Image(w, h) {}
CairoModule_ImageAffine::~CairoModule_ImageAffine() {}

void CairoModule_ImageAffine::bench(int quantity)
{
  cairo_t* cr = cairo_create(screen_cairo);

  double cx = (double)w / 2.0;
  double cy = (double)h / 2.0;
  double rot = 0.0;

  for (int a = 0; a < quantity; a++, rot += 0.01)
  {
    int x = r_rect.data[a].x;
    int y = r_rect.data[a].y;

    cairo_identity_matrix(cr);
    cairo_translate(cr, cx, cy);
    cairo_rotate(cr, rot);
    cairo_translate(cr, -cx, -cy);

    cairo_set_source_surface(cr, images_cairo[r_numb.data[a]], x, y);
    cairo_paint(cr);
  }

  cairo_destroy(cr);
}

ByteArray CairoModule_ImageAffine::getType()
{
  return ByteArray("ImageAffine");
}

#endif // FOG_BENCH_CAIRO

// ============================================================================
// [Bench]
// ============================================================================

static void printBenchmarkHeader(const char* name)
{
  fog_debug("%-22s|Size      |Time", name);
  fog_debug("----------------------+----------+---------------");
}

static void printBenchmarkFooter(double t)
{
  fog_debug("---------------------------------+---------------");
  fog_debug("Summary                          |%10.3f [ms]", t);
  fog_debug("");
}

static TimeDelta bench(AbstractModule& mod, int sw, int sh, int quantity)
{
  // Clear random seed (so all tests will behave identically)
  srand(43);

  mod.prepare(quantity, sw, sh);
  TimeTicks ticks = TimeTicks::highResNow();
  mod.bench(quantity);
  TimeDelta delta =  TimeTicks::highResNow() - ticks;
  mod.finish();

  ByteArray type = mod.getType();
  ByteArray size;
  ByteArray time;

  if (sw != 0 && sh != 0) size.format("%dx%d", sw, sh);
  time.format("%10.3f [ms]", delta.inMillisecondsF());

  fog_debug("%-22s|%-10s|%s", type.getData(), size.getData(), time.getData());
  mod.save(sw, sh);

  return delta;
}

static void benchAll()
{
  int w = 640, h = 480;
  int quantity = 100000;

  IntSize sizes[] =
  {
    //Size(2, 2),
    //Size(4, 4),
    //Size(8, 8),
    
    IntSize(10, 10),
    IntSize(16, 16),
    IntSize(32, 32),
    IntSize(64, 64),
    IntSize(128, 128)
  };

  TimeDelta totalFog[3];

  // --------------------------------------------------------------------------
  // Header
  // --------------------------------------------------------------------------

  static const char* yesno[2] = { "no", "yes" };

  fog_debug("Surface  : %dx%d", w, h);
  fog_debug("Quantity : %d", quantity);
  fog_debug("");
  fog_debug("Processor: %s", cpuInfo->brand);
  fog_debug("Features1: MMX=%s, MMXExt=%s, 3dNow=%s, 3dNowExt=%s, SSE=%s",
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_MMX)],
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_MMXExt)],
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_3dNow)],
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_3dNowExt)],
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_SSE)]);
  fog_debug("Features2: SSE2=%s, SSE3=%s, SSSE3=%s",
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_SSE2)],
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_SSE3)],
    yesno[cpuInfo->hasFeature(CpuInfo::FEATURE_SSSE3)]);
  fog_debug("CPU count: %u", cpuInfo->numberOfProcessors);
  fog_debug("");

  // --------------------------------------------------------------------------
  // Working variables
  // --------------------------------------------------------------------------

  int engine;
  int s;

  // --------------------------------------------------------------------------
  // Fog
  // --------------------------------------------------------------------------

  static const char* fogEngineName[] =
  {
    "",
    "Fog (st)",
    "Fog (mt)"
  };

  for (engine = 1; engine < 3; engine++)
  {
    printBenchmarkHeader(fogEngineName[engine]);

    // Fog - CreateDestroy
    {
      FogModule_CreateDestroy mod;
      mod.setEngine(engine);
      totalFog[engine] += bench(mod, 0, 0, quantity);
    }

    for (s = 0; s < FOG_ARRAY_SIZE(sizes); s++)
    {
      // Fog - FillRect
      {
        FogModule_FillRect mod(w, h);
        mod.setEngine(engine);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
      }

      // Fog - FillRectAffine
      {
        FogModule_FillRectAffine mod(w, h);
        mod.setEngine(engine);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
      }

      // Fog - FillRound
      {
        FogModule_FillRound mod(w, h);
        mod.setEngine(engine);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
      }

      // Fog - FillPolygon
      {
        FogModule_FillPolygon mod(w, h);
        mod.setEngine(engine);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
      }

      // Fog - FillPattern
      {
        FogModule_FillPattern mod(w, h);
        mod.setEngine(engine);

        mod.setupPattern(PATTERN_LINEAR_GRADIENT);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
#if 0
        mod.setupPattern(PATTERN_RADIAL_GRADIENT);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
#endif
      }

      // Fog - Image
      {
        FogModule_Image mod(w, h);
        mod.setEngine(engine);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
      }

      // Fog - ImageAffine
      {
        FogModule_ImageAffine mod(w, h);
        mod.setEngine(engine);
        totalFog[engine] += bench(mod, sizes[s].w, sizes[s].h, quantity);
      }
    }
#if 0
    // Fog - RasterText
    {
      FogModule_RasterText mod(w, h);
      mod.setEngine(engine);
      totalFog[engine] += bench(mod, 0, 0, quantity);
    }
#endif
    printBenchmarkFooter(totalFog[engine].inMillisecondsF());
  }

  // --------------------------------------------------------------------------
  // GdiPlus
  // --------------------------------------------------------------------------

#if defined(FOG_BENCH_GDIPLUS)

  printBenchmarkHeader("Gdi+");
  TimeDelta totalGdiPlus;

  // GdiPlus - CreateDestroy
  {
    GdiPlusModule_CreateDestroy mod;
    totalGdiPlus += bench(mod, 0, 0, quantity);
  }

  for (s = 0; s < FOG_ARRAY_SIZE(sizes); s++)
  {
    // GdiPlus - FillRect
    {
      GdiPlusModule_FillRect mod(w, h);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // GdiPlus - FillRectAffine
    {
      GdiPlusModule_FillRectAffine mod(w, h);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // GdiPlus - FillRound
    {
      GdiPlusModule_FillRound mod(w, h);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // GdiPlus - FillPolygon
    {
      GdiPlusModule_FillPolygon mod(w, h);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // GdiPlus - FillPattern
    {
      GdiPlusModule_FillPattern mod(w, h);

      mod.setupPattern(PATTERN_LINEAR_GRADIENT);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);

#if 0
      mod.setupPattern(PATTERN_RADIAL_GRADIENT);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);
#endif
    }

    // GdiPlus - Image
    {
      GdiPlusModule_Image mod(w, h);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // GdiPlus - ImageAffine
    {
      GdiPlusModule_ImageAffine mod(w, h);
      totalGdiPlus += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }
  }

  printBenchmarkFooter(totalGdiPlus.inMillisecondsF());
#endif // FOG_BENCH_GDIPLUS

  // --------------------------------------------------------------------------
  // Cairo
  // --------------------------------------------------------------------------

#if defined(FOG_BENCH_CAIRO)

  printBenchmarkHeader("Cairo");
  TimeDelta totalCairo;

  // Cairo - CreateDestroy
  {
    CairoModule_CreateDestroy mod;
    totalCairo += bench(mod, 0, 0, quantity);
  }

  for (s = 0; s < FOG_ARRAY_SIZE(sizes); s++)
  {
    // Cairo - FillRect
    {
      CairoModule_FillRect mod(w, h);
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // Cairo - FillRectAffine
    {
      CairoModule_FillRectAffine mod(w, h);
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // Cairo - FillRound
    {
      CairoModule_FillRound mod(w, h);
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // Cairo - FillPolygon
    {
      CairoModule_FillPolygon mod(w, h);
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // Cairo - FillPattern
    {
      CairoModule_FillPattern mod(w, h);
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);

#if 0
      mod.type = 1;
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);
#endif
    }

    // Cairo - Image
    {
      CairoModule_Image mod(w, h);
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }

    // Cairo - ImageAffine
    {
      CairoModule_ImageAffine mod(w, h);
      totalCairo += bench(mod, sizes[s].w, sizes[s].h, quantity);
    }
  }

  printBenchmarkFooter(totalCairo.inMillisecondsF());
#endif // FOG_BENCH_CAIRO
}

// ============================================================================
// [MAIN]
// ============================================================================

#undef main
int main(int argc, char* argv[])
{
  fog_debug("Fog benchmark tool v0.2\n");

#if defined(FOG_BENCH_GDIPLUS)
	// Initialize GDI+
  ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif // FOG_BENCH_GDIPLUS

  loadSprites();
  benchAll();

#if defined(FOG_BENCH_GDIPLUS)
	// Shutdown GDI+
  Gdiplus::GdiplusShutdown(gdiplusToken);

  system("pause");
#endif // FOG_BENCH_GDIPLUS

  return 0;
}
