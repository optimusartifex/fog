#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/UI.h>

// ============================================================================
// [FogTestSizeOf]
// ============================================================================

#define PRINT_PART(_Part_) \
  FOG_MACRO_BEGIN \
    printf("[%s]\n", _Part_); \
  FOG_MACRO_END

#define PRINT_TYPE(_Type_) \
  FOG_MACRO_BEGIN \
    printf("sizeof(" #_Type_ ") == %d\n", (int)sizeof(_Type_)); \
  FOG_MACRO_END

int main(int argc, char* argv[])
{
  // [Fog/Core/Kernel]
  PRINT_PART("Fog/Core/Kernel");
  PRINT_TYPE(Fog::Application);
  PRINT_TYPE(Fog::Event);
  PRINT_TYPE(Fog::Object);
  PRINT_TYPE(Fog::Task);
  PRINT_TYPE(Fog::Timer);

  // [Fog/Core/Math]
  PRINT_PART("Fog/Core/Math");
  PRINT_TYPE(Fog::IntervalF);
  PRINT_TYPE(Fog::IntervalD);
  PRINT_TYPE(Fog::MathFunctionF);
  PRINT_TYPE(Fog::MathFunctionD);

  // [Fog/Core/Memory]
  PRINT_PART("Fog/Core/Memory");
  PRINT_TYPE(Fog::MemBuffer);
  PRINT_TYPE(Fog::MemPool);

  // [Fog/Core/Threading]
  PRINT_PART("Fog/Core/Threading");
  PRINT_TYPE(Fog::Lock);
  PRINT_TYPE(Fog::Thread);
  PRINT_TYPE(Fog::ThreadCondition);
  PRINT_TYPE(Fog::ThreadEvent);
  PRINT_TYPE(Fog::ThreadLocal);
  PRINT_TYPE(Fog::ThreadPool);

  // [Fog/Core/Tools]
  PRINT_PART("Fog/Core/Tools");
  PRINT_TYPE(Fog::CharA);
  PRINT_TYPE(Fog::CharW);
  PRINT_TYPE(Fog::CharInfo);
  PRINT_TYPE(Fog::CharProperty);
  PRINT_TYPE(Fog::CharSpecial);
  PRINT_TYPE(Fog::Date);
  PRINT_TYPE(Fog::HashUntyped);
  PRINT_TYPE(Fog::HashUntypedData);
  PRINT_TYPE(Fog::ListUntyped);
  PRINT_TYPE(Fog::ListUntypedData);
  PRINT_TYPE(Fog::Locale);
  PRINT_TYPE(Fog::LocaleData);
  PRINT_TYPE(Fog::ManagedStringW);
  PRINT_TYPE(Fog::Range);
  PRINT_TYPE(Fog::RegExpA);
  PRINT_TYPE(Fog::RegExpW);
  PRINT_TYPE(Fog::StringA);
  PRINT_TYPE(Fog::StringW);
  PRINT_TYPE(Fog::StringDataA);
  PRINT_TYPE(Fog::StringDataW);
  PRINT_TYPE(Fog::StubA);
  PRINT_TYPE(Fog::StubW);
  PRINT_TYPE(Fog::TextCodec);
  PRINT_TYPE(Fog::Time);
  PRINT_TYPE(Fog::Var);
  PRINT_TYPE(Fog::VarData);

  // [Fog/G2d/Geometry]
  PRINT_PART("Fog/G2d/Geometry");
  PRINT_TYPE(Fog::ArcF);
  PRINT_TYPE(Fog::ArcD);
  PRINT_TYPE(Fog::BoxI);
  PRINT_TYPE(Fog::BoxF);
  PRINT_TYPE(Fog::BoxD);
  PRINT_TYPE(Fog::CBezierF);
  PRINT_TYPE(Fog::CBezierD);
  PRINT_TYPE(Fog::ChordF);
  PRINT_TYPE(Fog::ChordD);
  PRINT_TYPE(Fog::CircleF);
  PRINT_TYPE(Fog::CircleD);
  PRINT_TYPE(Fog::EllipseF);
  PRINT_TYPE(Fog::EllipseD);
  PRINT_TYPE(Fog::LineF);
  PRINT_TYPE(Fog::LineD);
  PRINT_TYPE(Fog::PathF);
  PRINT_TYPE(Fog::PathD);
  PRINT_TYPE(Fog::PathClipperF);
  PRINT_TYPE(Fog::PathClipperD);
  PRINT_TYPE(Fog::PathDataF);
  PRINT_TYPE(Fog::PathDataD);
  PRINT_TYPE(Fog::PathInfoF);
  PRINT_TYPE(Fog::PathInfoD);
  PRINT_TYPE(Fog::PieF);
  PRINT_TYPE(Fog::PieD);
  PRINT_TYPE(Fog::PointI);
  PRINT_TYPE(Fog::PointF);
  PRINT_TYPE(Fog::PointD);
  PRINT_TYPE(Fog::QBezierF);
  PRINT_TYPE(Fog::QBezierD);
  PRINT_TYPE(Fog::RectI);
  PRINT_TYPE(Fog::RectF);
  PRINT_TYPE(Fog::RectD);
  PRINT_TYPE(Fog::RoundF);
  PRINT_TYPE(Fog::RoundD);
  PRINT_TYPE(Fog::SizeI);
  PRINT_TYPE(Fog::SizeF);
  PRINT_TYPE(Fog::SizeD);
  PRINT_TYPE(Fog::TransformF);
  PRINT_TYPE(Fog::TransformD);

  // [Fog/G2d/Imaging]
  PRINT_PART("Fog/G2d/Imaging");
  PRINT_TYPE(Fog::Image);
  PRINT_TYPE(Fog::ImageBits);
  PRINT_TYPE(Fog::ImageCodec);
  PRINT_TYPE(Fog::ImageCodecProvider);
  PRINT_TYPE(Fog::ImageConverter);
  PRINT_TYPE(Fog::ImageData);
  PRINT_TYPE(Fog::ImageDecoder);
  PRINT_TYPE(Fog::ImageEncoder);
  PRINT_TYPE(Fog::ImageFilter);
  PRINT_TYPE(Fog::ImageFilterData);
  PRINT_TYPE(Fog::ImageFormatDescription);
  PRINT_TYPE(Fog::ImagePalette);
  PRINT_TYPE(Fog::ImagePaletteData);
  PRINT_TYPE(Fog::FeBase);
  PRINT_TYPE(Fog::FeBlur);
  PRINT_TYPE(Fog::FeBorder);
  PRINT_TYPE(Fog::FeColorLut);
  PRINT_TYPE(Fog::FeColorLutArray);
  PRINT_TYPE(Fog::FeColorLutArrayData);
  PRINT_TYPE(Fog::FeColorMatrix);
  PRINT_TYPE(Fog::FeComponentFunction);
  PRINT_TYPE(Fog::FeComponentFunctionData);
  PRINT_TYPE(Fog::FeComponentFunctionGamma);
  PRINT_TYPE(Fog::FeComponentFunctionLinear);
  PRINT_TYPE(Fog::FeComponentTransfer);
  PRINT_TYPE(Fog::FeConvolveMatrix);
  // PRINT_TYPE(FeConvolveSeparable);

  // [Fog/G2d/Painting]
  PRINT_PART("Fog/G2d/Painting");
  PRINT_TYPE(Fog::Painter);
  PRINT_TYPE(Fog::PaintDeviceInfo);
  PRINT_TYPE(Fog::PaintDeviceInfoData);
  PRINT_TYPE(Fog::PaintParamsF);
  PRINT_TYPE(Fog::PaintParamsD);

  // [Fog/G2d/Source]
  PRINT_PART("Fog/G2d/Source");
  PRINT_TYPE(Fog::AcmykF);
  PRINT_TYPE(Fog::AhslF);
  PRINT_TYPE(Fog::AhsvF);
  PRINT_TYPE(Fog::Argb32);
  PRINT_TYPE(Fog::Argb64);
  PRINT_TYPE(Fog::ArgbF);
  PRINT_TYPE(Fog::Color);
  PRINT_TYPE(Fog::ColorStop);
  PRINT_TYPE(Fog::ColorStopCache);
  PRINT_TYPE(Fog::ColorStopList);
  PRINT_TYPE(Fog::ColorStopListData);
  PRINT_TYPE(Fog::ConicalGradientF);
  PRINT_TYPE(Fog::ConicalGradientD);
  PRINT_TYPE(Fog::GradientF);
  PRINT_TYPE(Fog::GradientD);
  PRINT_TYPE(Fog::LinearGradientF);
  PRINT_TYPE(Fog::LinearGradientD);
  PRINT_TYPE(Fog::Pattern);
  PRINT_TYPE(Fog::PatternData);
  PRINT_TYPE(Fog::PatternColorData);
  PRINT_TYPE(Fog::PatternGradientDataF);
  PRINT_TYPE(Fog::PatternGradientDataD);
  PRINT_TYPE(Fog::PatternTextureDataF);
  PRINT_TYPE(Fog::PatternTextureDataD);
  PRINT_TYPE(Fog::RadialGradientF);
  PRINT_TYPE(Fog::RadialGradientD);
  PRINT_TYPE(Fog::RectangularGradientF);
  PRINT_TYPE(Fog::RectangularGradientD);
  PRINT_TYPE(Fog::Texture);

  // [Fog/G2d/Text]
  PRINT_PART("Fog/G2d/Text");
  PRINT_TYPE(Fog::Font);
  PRINT_TYPE(Fog::FontData);

  // [Fog/G2d/Tools]
  PRINT_PART("Fog/G2d/Tools");
  PRINT_TYPE(Fog::MatrixF);
  PRINT_TYPE(Fog::MatrixD);
  PRINT_TYPE(Fog::MatrixDataF);
  PRINT_TYPE(Fog::MatrixDataD);
  PRINT_TYPE(Fog::Region);
  PRINT_TYPE(Fog::RegionData);

  return 0;
}
