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

// Fog/Core/Dom.
struct DomAttr;
struct DomCDATASection;
struct DomCharacterData;
struct DomContainer;
struct DomDocument;
struct DomDocumentFragment;
struct DomDocumentType;
struct DomElement;
struct DomGCScope;
struct DomNode;
struct DomNodeList;
struct DomObj;
struct DomProcessingInstruction;
struct DomResourceItem;
struct DomResourceManager;
struct DomText;

// Fog/Core/Kernel.
struct Application;
struct CoreObj;
struct Event;
struct EventLoop;
struct EventLoopImpl;
struct EventLoopObserver;
struct EventLoopObserverListBase;
struct EventLoopPendingTask;
struct MetaClass;
struct Object;
struct ObjectExtra;
struct PropertyInfo;
struct Task;
struct Timer;

// Fog/Core/Math.
union  DoubleBits;
union  FloatBits;
struct IntervalF;
struct IntervalD;
struct MathFunctionF;
struct MathFunctionD;

// Fog/Core/Memory.
struct MemBlockAllocator;
struct MemBlockNode;
struct MemBuffer;
struct MemPool;
struct MemZoneAllocator;
struct MemZoneNode;
struct MemZoneRecord;

// Fog/Core/OS.
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
struct InternedStringW;
struct InternedStringCacheW;
struct Locale;
struct LocaleData;
struct LoggerRecord;
struct NTOAContext;
struct Random;
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
struct CoordF;
struct CoordD;
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
struct PointArrayI;
struct PointArrayF;
struct PointArrayD;
struct PolygonF;
struct PolygonD;
struct QBezierF;
struct QBezierD;
struct RectI;
struct RectF;
struct RectD;
struct RectArrayI;
struct RectArrayF;
struct RectArrayD;
struct RoundF;
struct RoundD;
struct ShapeF;
struct ShapeD;
struct SizeI;
struct SizeF;
struct SizeD;
struct ThicknessI;
struct ThicknessF;
struct ThicknessD;
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
struct ImageFilterScaleF;
struct ImageFilterScaleD;
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
struct FeCompositingFunction;
struct FeCompositingFunctionData;
struct FeConvolveMatrix;
struct FeConvolveSeparable;
struct FeMorphology;
struct FeTurbulence;

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

// These are not different types, but we like to mark whether the
// color is premultiplied or not in the API, it's more clear.
typedef Argb32 Prgb32;
typedef ArgbBase32 Prgb32Base;
typedef Argb64 Prgb64;
typedef ArgbBase64 PrgbBase64;
typedef ArgbF PrgbF;
typedef ArgbBaseF PrgbBaseF;

// Fog/G2d/Svg.
struct SvgAElement;
struct SvgCircleElement;
struct SvgContext;
struct SvgContextExtension;
struct SvgContextGState;
struct SvgContextSource;
struct SvgDefsElement;
struct SvgDocument;
struct SvgElement;
struct SvgEllipseElement;
struct SvgGElement;
struct SvgGradientElement;
struct SvgImageElement;
struct SvgLineElement;
struct SvgLinearGradientElement;
struct SvgPathElement;
struct SvgPatternElement;
struct SvgPolygonElement;
struct SvgPolylineElement;
struct SvgRadialGradientElement;
struct SvgRectElement;
struct SvgRootElement;
struct SvgSolidColorElement;
struct SvgStopElement;
struct SvgStylableElement;
struct SvgSymbolElement;
struct SvgTextElement;
struct SvgTextPositioningElement;
struct SvgTSpanElement;
struct SvgUseElement;
struct SvgViewElement;

// Fog/G2d/Text.
struct Face;
struct FaceFeatures;
struct FaceInfo;
struct FaceInfoData;
struct FaceVTable;
struct Font;
struct FaceCollection;
struct FaceCollectionData;
struct FontData;
struct FontEngine;
struct FontEngineVTable;
struct FontFeatures;
struct FontMatrix;
struct FontMetrics;
struct FontSpacing;
struct GlyphItem;
struct GlyphPosition;
struct GlyphRun;
struct TextLayoutRectI;
struct TextLayoutRectF;
struct TextLayoutRectD;

// Fog/G2d/Tools.
struct Dpi;
struct MatrixF;
struct MatrixD;
struct MatrixDataF;
struct MatrixDataD;
struct Region;
struct RegionData;

#if defined(FOG_BUILD_UI)
// Fog/UI/Engine.
struct UIEngine;
struct UIEngineDisplayInfo;
struct UIEngineEvent;
struct UIEngineKeyboardInfo;
struct UIEngineKeyboardState;
struct UIEngineMouseInfo;
struct UIEngineMouseState;
struct UIEnginePaletteInfo;
struct UIEngineWindow;
struct UIEngineWindowImpl;
#endif // FOG_BUILD_UI

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
// [TypeDefs - Functions - Fog/Core]
// ============================================================================

typedef void (FOG_CDECL *MemCleanupFunc)(void* closure, uint32_t reason);

typedef uint32_t (FOG_CDECL *HashFunc)(const void* key);
typedef bool (FOG_CDECL *EqFunc)(const void* a, const void* b);
typedef int (FOG_CDECL *CompareFunc)(const void* a, const void* b);
typedef int (FOG_CDECL *CompareExFunc)(const void* a, const void* b, const void* data);

typedef EventLoopImpl* (*EventLoopConstructor)(void);

// ============================================================================
// [TypeDefs - Functions - Fog/G2d]
// ============================================================================

typedef void (FOG_CDECL* FeCompositingFunctionPrgb32Func)(
  const FeCompositingFunctionData* self, Prgb32* dst, const Prgb32* a, const Prgb32* b, size_t length);

typedef void (FOG_CDECL* FeCompositingFunctionPrgb64Func)(
  const FeCompositingFunctionData* self, Prgb64* dst, const Prgb64* a, const Prgb64* b, size_t length);

typedef err_t (FOG_CDECL *CSSStyleHandlerFunc)(
  void* ctx, const StringW* name, const StringW* value);

// ============================================================================
// [TypeDefs - Functions - Fog/UI]
// ============================================================================

#if defined(FOG_BUILD_UI)
//! @brief Constructor used to create native @ref UIEngine instance.
typedef UIEngine* (*UIEngineConstructor)(void);
#endif // FOG_BUILD_UI

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_TYPEDEFS_H
