// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_MATH_CONSTANTS_H
#define _FOG_CORE_MATH_CONSTANTS_H

// [Dependencies]
#include <Fog/Core/C++/Base.h>

namespace Fog {

// ============================================================================
// [Fog::Math - Documentation]
// ============================================================================

// The numbers in 60-digit precision; can be used as a reference.
//
// +----------+---------------------------------------------------------------+
// |Constant  |Number                                                         |
// +----------+---------------------------------------------------------------+
// |e         | 2.718281828459045235360287471352662497757247093699959574966968|
// +----------+---------------------------------------------------------------+
// |log2(e)   | 1.442695040888963407359924681001892137426645954152985934135449|
// |log10(e)  | 0.434294481903251827651128918916605082294397005803666566114454|
// |ln(2)     | 0.693147180559945309417232121458176568075500134360255254120680|
// |ln(10)    | 2.302585092994045684017991454684364207601101488628772976033328|
// +----------+---------------------------------------------------------------+
// |pi        | 3.141592653589793238462643383279502884197169399375105820974945|
// +----------+---------------------------------------------------------------+
// |pi*1.5    | 4.712388980384689857693965074919254326295754099062658731462417|
// |pi*2      | 6.283185307179586476925286766559005768394338798750211641949889|
// |pi*2.5    | 7.853981633974483096156608458198757210492923498437764552437361|
// |pi*3      | 9.424777960769379715387930149838508652591508198125317462924834|
// |pi*3.5    |10.995574287564276334619251841478260094690092897812870373412306|
// |pi*4      |12.566370614359172953850573533118011536788677597500423283899778|
// |pi*4.5    |14.137166941154069573081895224757762978887262297187976194387251|
// |pi/2      | 1.570796326794896619231321691639751442098584699687552910487472|
// |pi/3      | 1.047197551196597746154214461093167628065723133125035273658315|
// |pi/4      | 0.785398163397448309615660845819875721049292349843776455243736|
// |1/(pi)    | 0.318309886183790671537767526745028724068919291480912897495335|
// |1/(pi*2)  | 0.159154943091895335768883763372514362034459645740456448747667|
// |1/(pi*3)  | 0.106103295394596890512589175581676241356306430493637632498445|
// |1/(pi*4)  | 0.079577471545947667884441881686257181017229822870228224373834|
// |2/(pi)    | 0.636619772367581343075535053490057448137838582961825794990669|
// |2/(pi*3)  | 0.212206590789193781025178351163352482712612860987275264996890|
// |3/(pi)    | 0.954929658551372014613302580235086172206757874442738692486004|
// |3/(pi*2)  | 0.477464829275686007306651290117543086103378937221369346243002|
// |3/(pi*4)  | 0.238732414637843003653325645058771543051689468610684673121501|
// |1/sqrt(pi)| 0.564189583547756286948079451560772585844050629328998856844086|
// |2/sqrt(pi)| 1.128379167095512573896158903121545171688101258657997713688171|
// |3/sqrt(pi)| 1.692568750643268860844238354682317757532151887986996570532257|
// +----------+---------------------------------------------------------------+
// |pi/180    | 0.017453292519943295769236907684886127134428718885417254560972|
// |180/pi    |57.295779513082320876798154814105170332405472466564321549160244|
// +----------+---------------------------------------------------------------+
// |sqrt(2)   | 1.414213562373095048801688724209698078569671875376948073176680|
// |sqrt(3)   | 1.732050807568877293527446341505872366942805253810380628055807|
// |1/sqrt(2) | 0.707106781186547524400844362104849039284835937688474036588340|
// |1/sqrt(3) | 0.577350269189625764509148780501957455647601751270126876018602|
// +----------+---------------------------------------------------------------+
// |kappa     | (4/3) * (Sqrt(2) - 1)                                         |
// |          | 0.552284749830793398402251632279597438092895833835930764235573|
// |1-kappa   | 0.447715250169206601597748367720402561907104166164069235764427|
// +----------+---------------------------------------------------------------+
// |gld-ratio | 1.618033988749894848204586834365638117720309179805762862135449|
// +----------+---------------------------------------------------------------+
// | TODO:    | 1/255    1/256                                                |
// |          | 1/65535  1/65536                                              |
// +----------+---------------------------------------------------------------+

// ============================================================================
// [Fog::Math - Constants]
// ============================================================================

//! @addtogroup Fog_Core_Math
//! @{

//! @brief e.
static const double MATH_E              = 2.71828182845904523536;
//! @brief log2(e).
static const double MATH_LOG2_E         = 1.44269504088896340736;
//! @brief log10(e).
static const double MATH_LOG10_E        = 0.43429448190325182765;
//! @brief logE(2).
static const double MATH_LN_2           = 0.69314718055994530942;
//! @brief logE(10).
static const double MATH_LN_10          = 2.30258509299404568402;

//! @brief pi.
static const double MATH_PI             = 3.14159265358979323846;

//! @brief pi * 1.5.
static const double MATH_ONE_HALF_PI    = 4.71238898038468985769;

//! @brief pi * 2.
static const double MATH_TWO_PI         = 6.28318530717958647692;
//! @brief pi * 2.5.
static const double MATH_TWO_HALF_PI    = 7.85398163397448309616;
//! @brief pi * 3.
static const double MATH_THREE_PI       = 9.42477796076937971539;
//! @brief pi * 3.5
static const double MATH_THREE_HALF_PI  =10.99557428756427633462;
//! @brief pi * 4.
static const double MATH_FOUR_PI        =12.56637061435917295385;
//! @brief pi * 4.5
static const double MATH_FOUR_HALF_PI   =14.13716694115406957308;

//! @brief pi / 2.
static const double MATH_HALF_PI        = 1.57079632679489661923;
//! @brief pi / 3.
static const double MATH_THIRD_PI       = 1.04719755119659774615;
//! @brief pi / 4.
static const double MATH_QUARTER_PI     = 0.78539816339744830962;

//! @brief 1 / pi.
static const double MATH_1_DIV_PI       = 0.31830988618379067154;
//! @brief 1 / (2 * pi).
static const double MATH_1_DIV_TWO_PI   = 0.15915494309189533577;
//! @brief 1 / (3 * pi).
static const double MATH_1_DIV_THREE_PI = 0.10610329539459689051;
//! @brief 1 / (4 * pi).
static const double MATH_1_DIV_FOUR_PI  = 0.07957747154594766788;

//! @brief 2 / pi.
static const double MATH_2_DIV_PI       = 0.63661977236758134308;
//! @brief 2 / (3 * pi).
static const double MATH_2_DIV_THREE_PI = 0.21220659078919378103;
//! @brief 2 / (4 * pi).
static const double MATH_2_DIV_FOUR_PI  = 0.15915494309189533577;

//! @brief 3 / pi.
static const double MATH_3_DIV_PI       = 0.95492965855137201461;
//! @brief 3 / (2 * pi).
static const double MATH_3_DIV_TWO_PI   = 0.47746482927568600731;
//! @brief 3 / (4 * pi).
static const double MATH_3_DIV_FOUR_PI  = 0.23873241463784300365;

//! @brief 1 / sqrt(pi).
static const double MATH_1_DIV_SQRT_PI  = 0.56418958354775628695;
//! @brief 2 / sqrt(pi).
static const double MATH_2_DIV_SQRT_PI  = 1.12837916709551257390;
//! @brief 3 / sqrt(pi).
static const double MATH_3_DIV_SQRT_PI  = 1.69256875064326886084;

//! @brief sqrt(2).
static const double MATH_SQRT_2         = 1.41421356237309504880;
//! @brief sqrt(3).
static const double MATH_SQRT_3         = 1.73205080756887729353;

//! @brief 1 / sqrt(2).
static const double MATH_1_DIV_SQRT_2   = 0.70710678118654752440;
//! @brief 1 / sqrt(3).
static const double MATH_1_DIV_SQRT_3   = 0.57735026918962576451;

//! @brief 4/3 * (sqrt(2) - 1).
static const double MATH_KAPPA          = 0.55228474983079339840;
//! @brief 1 - (4/3 * (sqrt(2) - 1)).
static const double MATH_1_MINUS_KAPPA  = 0.44771525016920660160;

//! @brief Golden ratio.
static const double MATH_GOLDEN_RATIO   = 1.61803398874989484820;

//! @brief Constant to convert radians from degrees.
static const double MATH_RAD_FROM_DEG   = 0.017453292519943295769;
//! @brief Constant to convert degrees from radians.
static const double MATH_DEG_FROM_RAD   =57.29577951308232087680;

//! @brief 1 / 3.
static const double MATH_1_DIV_3         = 1.0 / 3.0;

//! @brief 1 / 255.
static const double MATH_1_DIV_255       = 1.0 / 255.0;
//! @brief 1 / 256.
static const double MATH_1_DIV_256       = 1.0 / 256.0;

//! @brief 1 / 65535.
static const double MATH_1_DIV_65535     = 1.0 / 65535.0;
//! @brief 1 / 65536.
static const double MATH_1_DIV_65536     = 1.0 / 65536.0;

//! @brief Default epsilon used in math for 32-bit floats.
static const float MATH_EPSILON_F = 1e-8f;

//! @brief Default epsilon used in math for 64-bit floats.
static const double MATH_EPSILON_D = 1e-14;

// ============================================================================
// [Fog::MathConstant]
// ============================================================================

//! @brief Math constants structure.
template<typename NumT>
struct MathConstant {};

template<>
struct MathConstant<float>
{
  //! @brief Get math epsilon (float).
  static FOG_INLINE float getEpsilon() { return MATH_EPSILON_F; }

  //! @brief Get default flatness (float).
  static FOG_INLINE float getDefaultFlatness() { return 0.20f; }

  //! @brief Get intersection epsilon (float).
  static FOG_INLINE float getIntersectionEpsilon() { return 1.0e-8f; }

  static FOG_INLINE float getCollinearityEpsilon() { return 1e-8f; }

  //! @brief Get coinciding points maximal distance (float).
  static FOG_INLINE float getDistanceEpsilon() { return 1.0e-6f; }
};

template<>
struct MathConstant<double>
{
  //! @brief Get math epsilon (double).
  static FOG_INLINE double getEpsilon() { return MATH_EPSILON_D; }

  //! @brief Get default flatness (double).
  static FOG_INLINE double getDefaultFlatness() { return 0.20; }

  //! @brief Get intersection epsilon (double).
  static FOG_INLINE double getIntersectionEpsilon() { return 1.0e-30; }

  static FOG_INLINE double getCollinearityEpsilon() { return 1e-30; }

  //! @brief Get coinciding points maximal distance (double).
  static FOG_INLINE double getDistanceEpsilon() { return 1.0e-14; }
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_MATH_CONSTANTS_H
