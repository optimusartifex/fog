#include <Fog/Core.h>
#include <Fog/G2d.h>
#include <Fog/Gui.h>

// ============================================================================
// [FogTestSizeOf]
// ============================================================================

using namespace Fog;

#define PRINT_PART(_Part_) \
  FOG_MACRO_BEGIN \
    printf("[Fog/%s]\n", _Part_); \
  FOG_MACRO_END

#define PRINT_TYPE(_Type_) \
  FOG_MACRO_BEGIN \
    printf("sizeof(" #_Type_ ") == %d\n", (int)sizeof(_Type_)); \
  FOG_MACRO_END

int main(int argc, char* argv[])
{
  // [Core/Kernel]
  PRINT_PART("Core/Kernel");
  PRINT_TYPE(Application);
  PRINT_TYPE(Event);
  PRINT_TYPE(Object);
  PRINT_TYPE(Task);
  PRINT_TYPE(Timer);

  // [Core/Math]
  PRINT_PART("Core/Math");
  PRINT_TYPE(FunctionF);
  PRINT_TYPE(FunctionD);

  // [Core/Memory]
  PRINT_PART("Core/Memory");
  PRINT_TYPE(MemBuffer);
  PRINT_TYPE(MemPool);

  // [Core/Threading]
  PRINT_PART("Core/Threading");
  PRINT_TYPE(Lock);
  PRINT_TYPE(Thread);
  PRINT_TYPE(ThreadCondition);
  PRINT_TYPE(ThreadEvent);
  PRINT_TYPE(ThreadLocal);
  PRINT_TYPE(ThreadPool);

  // [Core/Tools]
  PRINT_PART("Core/Tools");
  PRINT_TYPE(CharA);
  PRINT_TYPE(CharW);
  PRINT_TYPE(CharInfo);
  PRINT_TYPE(CharProperty);
  PRINT_TYPE(CharSpecial);
  PRINT_TYPE(Date);
  PRINT_TYPE(HashUntyped);
  PRINT_TYPE(HashUntypedData);
  PRINT_TYPE(ListUntyped);
  PRINT_TYPE(ListUntypedData);
  PRINT_TYPE(Locale);
  PRINT_TYPE(LocaleData);
  PRINT_TYPE(ManagedString);
  PRINT_TYPE(Range);
  PRINT_TYPE(RegExpA);
  PRINT_TYPE(RegExpW);
  PRINT_TYPE(StringA);
  PRINT_TYPE(StringW);
  PRINT_TYPE(StringDataA);
  PRINT_TYPE(StringDataW);
  PRINT_TYPE(StubA);
  PRINT_TYPE(StubW);
  PRINT_TYPE(TextCodec);
  PRINT_TYPE(Time);
  PRINT_TYPE(Var);
  PRINT_TYPE(VarData);

  // [G2d/Geometry]
  PRINT_PART("G2d/Geometry");
  PRINT_TYPE(ArcF);
  PRINT_TYPE(ArcD);
  PRINT_TYPE(BoxI);
  PRINT_TYPE(BoxF);
  PRINT_TYPE(BoxD);
  PRINT_TYPE(CBezierF);
  PRINT_TYPE(CBezierD);
  PRINT_TYPE(ChordF);
  PRINT_TYPE(ChordD);
  PRINT_TYPE(CircleF);
  PRINT_TYPE(CircleD);
  PRINT_TYPE(EllipseF);
  PRINT_TYPE(EllipseD);
  PRINT_TYPE(LineF);
  PRINT_TYPE(LineD);
  PRINT_TYPE(PathF);
  PRINT_TYPE(PathD);
  PRINT_TYPE(PathClipperF);
  PRINT_TYPE(PathClipperD);
  PRINT_TYPE(PathDataF);
  PRINT_TYPE(PathDataD);
  PRINT_TYPE(PathInfoF);
  PRINT_TYPE(PathInfoD);
  PRINT_TYPE(PieF);
  PRINT_TYPE(PieD);
  PRINT_TYPE(PointI);
  PRINT_TYPE(PointF);
  PRINT_TYPE(PointD);
  PRINT_TYPE(QBezierF);
  PRINT_TYPE(QBezierD);
  PRINT_TYPE(RectI);
  PRINT_TYPE(RectF);
  PRINT_TYPE(RectD);
  PRINT_TYPE(RoundF);
  PRINT_TYPE(RoundD);
  PRINT_TYPE(SizeI);
  PRINT_TYPE(SizeF);
  PRINT_TYPE(SizeD);
  PRINT_TYPE(TransformF);
  PRINT_TYPE(TransformD);

  // [G2d/Imaging]
  PRINT_PART("G2d/Imaging");
  PRINT_TYPE(Image);
  PRINT_TYPE(ImageBits);
  PRINT_TYPE(ImageConverter);
  PRINT_TYPE(ImageData);
  PRINT_TYPE(ImageDecoder);
  PRINT_TYPE(ImageEncoder);
  PRINT_TYPE(ImageFormatDescription);
  PRINT_TYPE(ImagePalette);
  PRINT_TYPE(ColorMatrix);

  // [G2d/Painting]
  PRINT_PART("G2d/Painting");
  PRINT_TYPE(Painter);
  PRINT_TYPE(PaintDeviceInfo);
  PRINT_TYPE(PaintDeviceInfoData);
  PRINT_TYPE(PaintParamsF);
  PRINT_TYPE(PaintParamsD);

  // [G2d/Source]
  PRINT_PART("G2d/Source");
  PRINT_TYPE(AcmykF);
  PRINT_TYPE(AhslF);
  PRINT_TYPE(AhsvF);
  PRINT_TYPE(Argb32);
  PRINT_TYPE(Argb64);
  PRINT_TYPE(ArgbF);
  PRINT_TYPE(Color);
  PRINT_TYPE(ColorStop);
  PRINT_TYPE(ColorStopCache);
  PRINT_TYPE(ColorStopList);
  PRINT_TYPE(ColorStopListData);
  PRINT_TYPE(ConicalGradientF);
  PRINT_TYPE(ConicalGradientD);
  PRINT_TYPE(GradientF);
  PRINT_TYPE(GradientD);
  PRINT_TYPE(LinearGradientF);
  PRINT_TYPE(LinearGradientD);
  PRINT_TYPE(Pattern);
  PRINT_TYPE(PatternData);
  PRINT_TYPE(PatternColorData);
  PRINT_TYPE(PatternGradientDataF);
  PRINT_TYPE(PatternGradientDataD);
  PRINT_TYPE(PatternTextureDataF);
  PRINT_TYPE(PatternTextureDataD);
  PRINT_TYPE(RadialGradientF);
  PRINT_TYPE(RadialGradientD);
  PRINT_TYPE(RectangularGradientF);
  PRINT_TYPE(RectangularGradientD);
  PRINT_TYPE(Texture);

  // [G2d/Text]
  PRINT_PART("G2d/Font");
  PRINT_TYPE(Font);
  PRINT_TYPE(FontData);

  // [G2d/Tools]
  PRINT_PART("G2d/Tools");
  PRINT_TYPE(MatrixF);
  PRINT_TYPE(MatrixD);
  PRINT_TYPE(MatrixDataF);
  PRINT_TYPE(MatrixDataD);
  PRINT_TYPE(Region);
  PRINT_TYPE(RegionData);

  return 0;
}
