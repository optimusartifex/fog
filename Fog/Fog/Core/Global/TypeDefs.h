// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_TYPEDEFS_H
#define _FOG_CORE_GLOBAL_TYPEDEFS_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// ============================================================================
// [TypeDefs - Classes]
// ============================================================================

// Fog/Core/Kernel.
struct Application;
struct Event;
struct EventLoop;
struct MetaClass;
struct Object;
struct ObjectExtra;
struct Task;
struct Timer;

// Fog/Core/Math.
union  DoubleBits;
union  FloatBits;
struct FunctionF;
struct FunctionD;
struct IntervalF;
struct IntervalD;

// Fog/Core/Memory.
struct MemBlockAllocator;
struct MemBlockNode;
struct MemBuffer;
struct MemPool;
struct MemZoneAllocator;
struct MemZoneNode;
struct MemZoneRecord;

// CFog/ore/OS.
struct DirIterator;
struct FileInfo;
struct FileInfoData;
struct FileMapping;
struct FileMappingData;
struct Library;
struct LibraryData;

#if defined(FOG_OS_WINDOWS)
struct WinVersion;
#endif // FOG_OS_WINDOWS

// Fog/Core/Threading.
template<typename T> struct Atomic;
struct Lock;
struct Thread;
struct ThreadCondition;
struct ThreadEvent;
struct ThreadLocal;
struct ThreadPool;
struct ThreadPoolEntry;

// Fog/Core/Tools.
struct Ascii8;
struct Local8;
struct Utf8;

struct ListUntyped;
struct ListUntypedData;
struct ListUntypedVTable;
template<typename ItemT> struct List;

struct HashUntyped;
struct HashUntypedData;
struct HashUntypedIterator;
struct HashUntypedNode;
struct HashUntypedVTable;
template<typename KeyT, typename ValueT> struct Hash;

struct CharA;
struct CharW;
struct Cpu;
struct Date;
struct DateDelta;
struct FormatInt;
struct FormatReal;
struct Locale;
struct LocaleData;
struct ManagedStringW;
struct ManagedStringCacheW;
struct NTOAContext;
struct Range;
struct RegExpA;
struct RegExpW;
struct RegExpDataA;
struct RegExpDataW;
struct Stream;
struct StringA;
struct StringW;
struct StringDataA;
struct StringDataW;
struct StubA;
struct StubW;
struct TextCodec;
struct TextCodecData;
struct TextCodecHandler;
struct TextCodecState;
struct Time;
struct TimeDelta;
struct TimeTicks;
struct Var;
struct VarData;

// Fog/Core/Xml
struct XmlDocument;
struct XmlElement;

// Fog/G2d/Geometry.
struct ArcF;
struct ArcD;
struct BoxI;
struct BoxF;
struct BoxD;
struct CBezierF;
struct CBezierD;
struct ChordF;
struct ChordD;
struct CircleF;
struct CircleD;
struct EllipseF;
struct EllipseD;
struct LineF;
struct LineD;
struct PathF;
struct PathD;
struct PathClipperF;
struct PathClipperD;
struct PathDataF;
struct PathDataD;
struct PathFlattenParamsF;
struct PathFlattenParamsD;
struct PathInfoF;
struct PathInfoD;
struct PathInfoFigureF;
struct PathInfoFigureD;
struct PathStrokerF;
struct PathStrokerD;
union  PathStrokerHints;
struct PathStrokerParamsF;
struct PathStrokerParamsD;
struct PieF;
struct PieD;
struct PointI;
struct PointF;
struct PointD;
struct PolygonF;
struct PolygonD;
struct QBezierF;
struct QBezierD;
struct RectI;
struct RectF;
struct RectD;
struct RoundF;
struct RoundD;
struct ShapeF;
struct ShapeD;
struct SizeI;
struct SizeF;
struct SizeD;
struct TransformF;
struct TransformD;
struct TriangleF;
struct TriangleD;

// Fog/G2d/Imaging.
struct Image;
struct ImageBits;
struct ImageCodec;
struct ImageCodecProvider;
struct ImageConverter;
struct ImageConverterClosure;
struct ImageConverterData;
struct ImageData;
struct ImageDecoder;
struct ImageDither8Params;
struct ImageEncoder;
struct ImageFilter;
struct ImageFilterData;
struct ImageFormatDescription;
struct ImagePalette;
struct ImagePaletteData;
struct ImageVTable;

// Fog/G2d/Imaging/Filters.
struct FeBase;
struct FeBlur;
struct FeBorder;
struct FeColorLut;
struct FeColorLutArray;
struct FeColorLutArrayData;
struct FeColorMatrix;
struct FeComponentTransfer;
struct FeComponentFunction;
struct FeComponentFunctionData;
struct FeComponentFunctionGamma;
struct FeComponentFunctionLinear;
struct FeConvolveMatrix;
struct FeConvolveSeparable;
struct FeMorphology;

// Fog/G2d/Painting.
struct Painter;
struct PaintDevice;
struct PaintDeviceInfo;
struct PaintEngine;
struct PaintParamsF;
struct PaintParamsD;

// Fog/G2d/Source.
struct AcmykF;
struct AcmykBaseF;
struct AhslF;
struct AhslBaseF;
struct AhsvF;
struct AhsvBaseF;
struct Argb32;
struct ArgbBase32;
struct Argb64;
struct ArgbBase64;
struct ArgbF;
struct ArgbBaseF;
struct Color;
struct ColorBase;
struct ColorStop;
struct ColorStopCache;
struct ColorStopList;
struct ColorStopListData;
struct ConicalGradientF;
struct ConicalGradientD;
struct GradientF;
struct GradientD;
struct LinearGradientF;
struct LinearGradientD;
struct Pattern;
struct PatternData;
struct PatternColorData;
struct PatternGradientDataF;
struct PatternGradientDataD;
struct PatternTextureDataF;
struct PatternTextureDataD;
struct RadialGradientF;
struct RadialGradientD;
struct RectangularGradientF;
struct RectangularGradientD;
struct Texture;

// Fog/G2d/Text
struct Font;
struct FontData;
struct FontFace;
struct FontHints;
union  FontKerningChars;
struct FontKerningPairI;
struct FontKerningPairF;
struct FontKerningTableI;
struct FontKerningTableF;
struct FontMetricsF;
struct FontManager;
struct FontManagerData;
struct FontProvider;
struct FontProviderData;
struct GlyphBitmap;
struct GlyphOutline;
struct TextExtentsI;
struct TextExtentsF;
struct TextExtentsD;
struct TextLayout;
struct TextRectI;
struct TextRectF;
struct TextRectD;

// Fog/G2d/Tools.
struct Dpi;
struct MatrixF;
struct MatrixD;
struct MatrixDataF;
struct MatrixDataD;
struct Region;
struct RegionData;

// Fog/UI/Layout
struct Layout;

// Fog/UI/Widget
struct Widget;

// ============================================================================
// [TypeDefs - Template-Specialization]
// ============================================================================

// Template specialization must be specified here, because if we use for
// example List<StringW> in the Fog C-API (Core/Global/Api.h) then the template
// is instantiated within the compiler and the compile error is show when our
// instantiation is defined.

template<> struct List<float>;
template<> struct List<double>;
template<> struct List<StringA>;
template<> struct List<StringW>;
template<> struct List<Var>;

template<> struct Hash<StringA, StringA>;
template<> struct Hash<StringA, Var>;
template<> struct Hash<StringW, StringW>;
template<> struct Hash<StringW, Var>;

template<typename ItemT> struct Hash<StringA, ItemT>;
template<typename ItemT> struct Hash<StringW, ItemT>;

// ============================================================================
// [TypeDefs - Functions]
// ============================================================================

typedef void (FOG_CDECL *MemCleanupFunc)(void* closure, uint32_t reason);

typedef uint32_t (FOG_CDECL *HashFunc)(const void* key);
typedef bool (FOG_CDECL *EqFunc)(const void* a, const void* b);
typedef int (FOG_CDECL *CompareFunc)(const void* a, const void* b);
typedef int (FOG_CDECL *CompareExFunc)(const void* a, const void* b, const void* data);

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_TYPEDEFS_H
