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
#include <Fog/Core/Memory/MemMgr.h>
#include <Fog/G2d/Painting/PaintDeviceInfo.h>
#include <Fog/G2d/Text/Font.h>

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
  d->reference.inc();
  return d;
}

static FOG_INLINE void PaintDeviceInfo_deref(PaintDeviceInfoData* d)
{
  if (d->reference.deref()) MemMgr::free(d);
}

static FOG_INLINE err_t PaintDeviceInfo_detach(PaintDeviceInfo* self)
{
  PaintDeviceInfoData* d = self->_d;

  if (d->reference.get() != 1)
  {
    PaintDeviceInfoData* newd = reinterpret_cast<PaintDeviceInfoData*>(
      MemMgr::alloc(sizeof(PaintDeviceInfoData)));
    if (FOG_IS_NULL(newd)) return ERR_RT_OUT_OF_MEMORY;

    newd->reference.init(1);
    newd->paintDevice = d->paintDevice;
    newd->fontKerning = d->fontKerning;
    newd->fontHinting = d->fontHinting;
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
  _d(PaintDeviceInfo_ref(&PaintDeviceInfo_dnull))
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
  PaintDeviceInfo_deref(atomicPtrXchg(&_d, PaintDeviceInfo_ref(&PaintDeviceInfo_dnull)));
}

// ============================================================================
// [Fog::PaintDeviceInfo - Create]
// ============================================================================

err_t PaintDeviceInfo::create(uint32_t paintDevice,
  uint32_t fontKerning,
  uint32_t fontHinting,
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
  _d->fontQuality = fontQuality;
  _d->lcdOrder = lcdOrder;

  _d->dpiInfo.setDpi(dpi);
  _d->scale = scale;
  _d->aspectRatio.x = scale.x / scale.y;
  _d->aspectRatio.y = scale.y / scale.x;

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
  PaintDeviceInfoData* d = &PaintDeviceInfo_dnull;

  d->reference.init(1);
  d->paintDevice = PAINT_DEVICE_NULL;
  d->fontKerning = FONT_KERNING_ENABLED;
  d->fontHinting = FONT_HINTING_DISABLED;
  d->fontQuality = FONT_QUALITY_GREY;
  d->lcdOrder = LCD_ORDER_NONE;
  d->scale.set(1.0f, 1.0f);
  d->aspectRatio.set(1.0f, 1.0f);
  d->dpiInfo.setDpi(90.0f);
}

} // Fog namespace
