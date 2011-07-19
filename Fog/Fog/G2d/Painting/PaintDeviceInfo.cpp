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
#include <Fog/Core/Memory/Alloc.h>
#include <Fog/G2d/Painting/PaintDeviceInfo.h>
#include <Fog/G2d/Text/Font.h>
#include <Fog/G2d/Text/FontHints.h>

namespace Fog {

// ============================================================================
// [Fog::PaintDeviceInfo - Statics]
// ============================================================================

static Static<PaintDeviceInfoData> PaintDeviceInfo_dnull;

// ============================================================================
// [Fog::PaintDeviceInfo - Helpers]
// ============================================================================

static FOG_INLINE PaintDeviceInfoData* PaintDeviceInfo_ref(PaintDeviceInfoData* d)
{
  d->refCount.inc();
  return d;
}

static FOG_INLINE void PaintDeviceInfo_deref(PaintDeviceInfoData* d)
{
  if (d->refCount.deref()) Memory::free(d);
}

static FOG_INLINE err_t PaintDeviceInfo_detach(PaintDeviceInfo* self)
{
  PaintDeviceInfoData* d = self->_d;

  if (d->refCount.get() != 1)
  {
    PaintDeviceInfoData* newd = reinterpret_cast<PaintDeviceInfoData*>(
      Memory::alloc(sizeof(PaintDeviceInfoData)));
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->refCount.init(1);
    newd->paintDevice = d->paintDevice;
    newd->fontKerning = d->fontKerning;
    newd->fontHinting = d->fontHinting;
    newd->fontAlignMode = d->fontAlignMode;
    newd->fontQuality = d->fontQuality;
    newd->lcdOrder    = d->lcdOrder;
    newd->dpiInfo     = d->dpiInfo;
    newd->scale       = d->scale;
    newd->aspectRatio = d->aspectRatio;
  }

  return ERR_OK;
}

// ============================================================================
// [Fog::PaintDeviceInfo - Construction / Destruction]
// ============================================================================

PaintDeviceInfo::PaintDeviceInfo() :
  _d(PaintDeviceInfo_ref(PaintDeviceInfo_dnull.instancep()))
{
}

PaintDeviceInfo::PaintDeviceInfo(const PaintDeviceInfo& other) :
  _d(PaintDeviceInfo_ref(other._d))
{
}

PaintDeviceInfo::~PaintDeviceInfo()
{
  PaintDeviceInfo_deref(_d);
}

// ============================================================================
// [Fog::PaintDeviceInfo - Reset]
// ============================================================================

void PaintDeviceInfo::reset()
{
  PaintDeviceInfo_deref(
    atomicPtrXchg(&_d, PaintDeviceInfo_ref(PaintDeviceInfo_dnull.instancep()))
  );
}

// ============================================================================
// [Fog::PaintDeviceInfo - Create]
// ============================================================================

err_t PaintDeviceInfo::create(uint32_t paintDevice,
  uint32_t fontKerning,
  uint32_t fontHinting,
  uint32_t fontAlignMode,
  uint32_t fontQuality,
  uint32_t lcdOrder,
  float dpi,
  const PointF& scale)
{
  if (paintDevice >= PAINT_DEVICE_COUNT ||
      fontKerning >= 2                  ||
      fontHinting >= 2                  ||
      fontQuality >= FONT_QUALITY_COUNT ||
      lcdOrder    >= LCD_ORDER_COUNT    ||
      dpi         <= 0.0f               ||
      scale.x     <= 0.0f               ||
      scale.y     <= 0.0f               )
  {
    return ERR_RT_INVALID_ARGUMENT;
  }

  FOG_RETURN_ON_ERROR(PaintDeviceInfo_detach(this));
  _d->paintDevice = paintDevice;
  _d->fontKerning = fontKerning;
  _d->fontHinting = fontHinting;
  _d->fontAlignMode = fontAlignMode;
  _d->fontQuality = fontQuality;
  _d->lcdOrder = lcdOrder;

  _d->dpiInfo.setDpi(dpi);
  _d->scale = scale;
  _d->aspectRatio.x = scale.x / scale.y;
  _d->aspectRatio.y = scale.y / scale.x;

  return ERR_OK;
}

// ============================================================================
// [Fog::PaintDeviceInfo - Methods]
// ============================================================================

err_t PaintDeviceInfo::makePhysicalFont(Font& physical, const Font& src)
{
  if (src.isPhysical())
  {
    physical = src;
    return ERR_OK;
  }

  FontData* sd = src._d;

  String family = src.getFamily();
  float height = _d->dpiInfo.toDeviceSpace(sd->metrics.getHeight(), sd->unit);
  FontHints hints = sd->hints;
  TransformF transform = sd->transform;

  if (hints.getKerning() == FONT_KERNING_DETECT) hints.setKerning(_d->fontKerning);
  if (hints.getHinting() == FONT_HINTING_DETECT) hints.setHinting(_d->fontHinting);
  if (hints.getAlignMode() == FONT_ALIGN_MODE_DETECT) hints.setAlignMode(_d->fontAlignMode);
  if (hints.getQuality() == FONT_QUALITY_DETECT) hints.setQuality(_d->fontQuality);

  FOG_RETURN_ON_ERROR(physical.create(family, height, UNIT_PX, hints, transform));
  FOG_ASSERT(physical.isPhysical() == true);

  return ERR_OK;
}

// ============================================================================
// [Fog::PaintDeviceInfo - Operator Overload]
// ============================================================================

PaintDeviceInfo& PaintDeviceInfo::operator=(const PaintDeviceInfo& other)
{
  PaintDeviceInfo_deref(
    atomicPtrXchg(&_d, PaintDeviceInfo_ref(other._d))
  );
  return *this;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void PaintDeviceInfo_init(void)
{
  PaintDeviceInfoData* d = PaintDeviceInfo_dnull.instancep();

  d->refCount.init(1);
  d->paintDevice = PAINT_DEVICE_UNKNOWN;
  d->fontKerning = FONT_KERNING_ENABLED;
  d->fontHinting = FONT_HINTING_DISABLED;
  d->fontAlignMode = FONT_ALIGN_MODE_NONE;
  d->fontQuality = FONT_QUALITY_GREY;
  d->lcdOrder = LCD_ORDER_NONE;
  d->scale.set(1.0f, 1.0f);
  d->aspectRatio.set(1.0f, 1.0f);
  d->dpiInfo.setDpi(90.0f);
}

} // Fog namespace
