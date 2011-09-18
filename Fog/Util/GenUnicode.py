#!/usr/bin/env python

# Unicode Reports:
#
#   http://www.unicode.org/reports/tr44/
#   (Unicode Character Database)

# =============================================================================
# GenUnicode.py - Script that generates Fog-Unicode tables based on Unicode 6.0
#
# This script must be modified when new unicode standard is released. Currently
# it's tuned for Unicode 6.0, and it is possible to regenerate tables when only
# small additions are done to the UCD, but when enums are added, or some rules
# changed, it's needed to review the unicode reports and to modify the code. If
# there is something not expected in the unicode database then fatal error
# exception should be raised.
# =============================================================================

import os

# -----------------------------------------------------------------------------
# [UCD-Helpers]
# -----------------------------------------------------------------------------

def log(str):
  print(str)

# Convert integer to string representing unicode code-point "U+cp".  
def ucd(cp):
  if cp <= 0xFFFF:
    return "U+%04X" % cp
  else:
    return "U+%06X" % cp

def uch(cp):
  if cp <= 0xFFFF:
    return "0x%04X" % cp
  else:
    return "0x%06X" % cp

# Convert cp to UTF-16 surrogate pair.
def SurrogatePairOf(cp):
  assert cp >= 0x10000
  return (55232 + (cp >> 10), 56320 + (cp & 0x03FF))

# Split data using ';' as a separator and strip all leading/trailing whitespaces.    
def SplitData(line):
  data = line.split(';')
  for i in xrange(len(data)):
    data[i] = data[i].strip()
  return data

def GetCharRange(item):
  if ".." in item:
    items = item.split("..")
    first = int(items[0], 16)
    last = int(items[1], 16)
    return (first, last)
  else:
    code = int(item, 16)
    return (code, code)

# Line iterator, all comments and empty lines are removed.
def LineIterator(file):
  while True:
    line = file.readline()
    if len(line) == 0:
      break

    pos = line.find("#")
    if pos != -1:
      line = line[0:pos]
    line = line.strip(" \t\r\n")

    if len(line) != 0:
      yield line

def DownloadFile(url, targetName):
  import urllib
  dirName = os.path.dirname(targetName)
  if not os.path.isdir(dirName):
    os.makedirs(dirName)
  urllib.urlretrieve(url, targetName)

# -----------------------------------------------------------------------------
# [UCD-Files]
# -----------------------------------------------------------------------------

# The UCD files needed to run this script, downloaded on-demand.
UCD_FILES = [
  "ucd/auxiliary/GraphemeBreakProperty.txt",
  "ucd/auxiliary/SentenceBreakProperty.txt",
  "ucd/auxiliary/WordBreakProperty.txt",
  "ucd/ArabicShaping.txt",
  "ucd/BidiMirroring.txt",
  "ucd/Blocks.txt",
  "ucd/CaseFolding.txt",
  "ucd/CompositionExclusions.txt",
  "ucd/DerivedAge.txt",
  "ucd/DerivedNormalizationProps.txt",
  "ucd/EastAsianWidth.txt",
  "ucd/LineBreak.txt",
  "ucd/NormalizationCorrections.txt",
  "ucd/PropertyValueAliases.txt",
  "ucd/Scripts.txt",
  "ucd/ScriptExtensions.txt",
  "ucd/SpecialCasing.txt",
  "ucd/UnicodeData.txt"
]

# Unicode version.
UCD_VERSION="6.0.0"

# Local directory to store the files.
UCD_DIRECTORY = "tmp/"
# URL where to download if the files are missing (when check-outed).
UCD_PUBLIC_URL = "http://www.unicode.org/Public/" + UCD_VERSION

def PrepareFiles():
  log("-- Checking for unicode files")
  for name in UCD_FILES:
    targetName = UCD_DIRECTORY + name
    try:
      f = open(targetName, 'rb')
      f.close()
    except:
      log("   " + name + ": Not found, starting download...")
      DownloadFile(UCD_PUBLIC_URL + "/" + name, targetName)
      log("   " + name + ": Downloaded.")

# -----------------------------------------------------------------------------
# [UCD-Constants]
# -----------------------------------------------------------------------------

MAX_CODE_POINT = 0x10FFFF
NUM_CODE_POINTS = MAX_CODE_POINT + 1

# 17 bits, see Fog::CharProperty.
MAX_PAIRED_DIFF = 2**16 - 1

# http://www.unicode.org/Public/6.0.0/ucd/UnicodeData.txt
#
# The data index in UnicodeData.txt file, entries separated by ;
UCD_VALUE = 0
UCD_NAME = 1
UCD_CATEGORY = 2
UCD_COMBINING_CLASS = 3
UCD_BIDI = 4
UCD_DECOMPOSITION = 5
UCD_DECIMAL_DIGIT_VALUE = 6
UCD_DIGIT_VALUE = 7
UCD_NUMERIC_VALUE = 8
UCD_MIRRORED = 9
UCD_OLD_NAME = 10
UCD_COMMENT = 11
UCD_UPPERCASE = 12
UCD_LOWERCASE = 13
UCD_TITLECASE = 14

# Enumeration, mapping between the Unicode string name, internal ID value,
# and Fog constant name (without the "CHAR_XXX_" prefix).
class Enum(object):
  def __init__(self, name, default, data, alt={}):
    self._name = name
    self._map = {}
    self._arr = []
    self._default = default

    for i in xrange(len(data)):
      item = data[i]
      self._map[item[0]] = i
      self._arr.append(item[1])
      
    # Alternative map.
    for key in alt:
      self.addAlias(key, self._map[alt[key]])

  # Get id value from a given key.
  def get(self, key):
    if not key:
      return self._default

    if key in self._map:
      return self._map[key]

    raise KeyError(self._name + " - There is no key " + key)

  # Translate the id value to a Fog-Enum value.
  def fog(self, id):
    return self._arr[id]
    
  def addAlias(self, key, value):
    self._map[key] = value

  def generateEnum(self):
    s = ""
    for i in xrange(len(self._arr)):
      s += "  " + self._name + "_" + self._arr[i] + " = " + str(i)
      s += ",\n"
    s += "\n"
    s += "  " + self._name + "_COUNT" + " = " + str(len(self._arr)) + "\n"
    return s

# http://www.unicode.org/Public/6.0.0/ucd/UnicodeData.txt
#
# Bidi.
CHAR_BIDI = Enum(name="BIDI", default=0,
  data=[
    ["EN"             , "EN"                   ],
    ["ES"             , "ES"                   ],
    ["ET"             , "ET"                   ],
    ["AN"             , "AN"                   ],
    ["AL"             , "AL"                   ],
    ["WS"             , "WS"                   ],
    ["CS"             , "CS"                   ],
    ["B"              , "B"                    ],
    ["S"              , "S"                    ],
    ["L"              , "L"                    ],
    ["LRE"            , "LRE"                  ],
    ["LRO"            , "LRO"                  ],
    ["R"              , "R"                    ],
    ["RLE"            , "RLE"                  ],
    ["RLO"            , "RLO"                  ],
    ["PDF"            , "PDF"                  ],
    ["NSM"            , "NSM"                  ],
    ["BN"             , "BN"                   ],
    ["ON"             , "ON"                   ]
  ]
)

# http://www.unicode.org/Public/6.0.0/ucd/UnicodeData.txt
#
# Unicode character categories.
CHAR_CATEGORY = Enum(name="CATEGORY", default=0,
  data=[
    [""               , "NONE"                 ],
    ["Lu"             , "LU"                   ],
    ["Ll"             , "LL"                   ],
    ["Lt"             , "LT"                   ],
    ["Lm"             , "LM"                   ],
    ["Lo"             , "LO"                   ],

    ["Nd"             , "ND"                   ],
    ["Nl"             , "NL"                   ],
    ["No"             , "NO"                   ],

    ["Mc"             , "MC"                   ],
    ["Me"             , "ME"                   ],
    ["Mn"             , "MN"                   ],

    ["Zs"             , "ZS"                   ],
    ["Zl"             , "ZL"                   ],
    ["Zp"             , "ZP"                   ],

    ["Cc"             , "CC"                   ],
    ["Cf"             , "CF"                   ],
    ["Cs"             , "CS"                   ],
    ["Co"             , "CO"                   ],
    ["Cn"             , "CN"                   ],

    ["Pc"             , "PC"                   ],
    ["Pd"             , "PD"                   ],
    ["Ps"             , "PS"                   ],
    ["Pe"             , "PE"                   ],
    ["Pi"             , "PI"                   ],
    ["Pf"             , "PF"                   ],
    ["Po"             , "PO"                   ],

    ["Sm"             , "SM"                   ],
    ["Sc"             , "SC"                   ],
    ["Sk"             , "SK"                   ],
    ["So"             , "SO"                   ]
  ]
)

CHAR_CASE_FOLDING = Enum(name="CASE_FOLDING", default=0,
  data=[
    [""               , "NONE"                 ],
    ["F"              , "FULL"                 ],
    ["S"              , "SIMPLE"               ],
    ["C"              , "COMMON"               ],
    ["T"              , "SPECIAL"              ]
  ]
)

# Decomposition.
CHAR_DECOMPOSITION = Enum(name="DECOMPOSITION", default=0,
  data=[
    [""               , "NONE"                 ],
    ["<canonical>"    , "CANONICAL"            ],

    ["<font>"         , "FONT"                 ],
    ["<noBreak>"      , "NOBREAK"              ],

    # Arabic.
    ["<initial>"      , "INITIAL"              ],
    ["<medial>"       , "MEDIAL"               ],
    ["<final>"        , "FINAL"                ],
    ["<isolated>"     , "ISOLATED"             ],

    # Script form
    ["<circle>"       , "CIRCLE"               ],
    ["<super>"        , "SUPER"                ],
    ["<sub>"          , "SUB"                  ],

    ["<vertical>"     , "VERTICAL"             ],
    ["<wide>"         , "WIDE"                 ],
    ["<narrow>"       , "NARROW"               ],
    ["<small>"        , "SMALL"                ],
    ["<square>"       , "SQUARE"               ],
    ["<fraction>"     , "FRACTION"             ],
    ["<compat>"       , "COMPAT"               ]
  ]
)

# http://www.unicode.org/Public/6.0.0/ucd/auxiliary/GraphemeBreakProperty.txt
#
# Grapheme break properties.
CHAR_GRAPHEME_BREAK = Enum(name="GRAPHEME_BREAK", default=0,
  data=[
    [""               , "XX"                   ],
    ["CR"             , "CR"                   ],
    ["LF"             , "LF"                   ],
    ["Control"        , "CN"                   ],
    ["Extend"         , "EX"                   ],
    ["Prepend"        , "PP"                   ],
    ["SpacingMark"    , "SM"                   ],
    # Hungul jamo (L/V/T) and syllable (LV/LVT).
    ["L"              , "L"                    ],
    ["V"              , "V"                    ],
    ["T"              , "T"                    ],
    ["LV"             , "LV"                   ],
    ["LVT"            , "LVT"                  ]
  ]
)

# http://www.unicode.org/Public/6.0.0/ucd/auxiliary/WordBreakProperty.txt
#
# Work break properties.
CHAR_WORD_BREAK = Enum(name="WORD_BREAK", default=0,
  data=[
    [""               , "OTHER"                ],
    ["CR"             , "CR"                   ],
    ["LF"             , "LF"                   ],
    ["Newline"        , "NL"                   ],
    ["Extend"         , "EXTEND"               ],
    ["Format"         , "FO"                   ],
    ["Katakana"       , "KA"                   ],
    ["ALetter"        , "LE"                   ],
    ["MidLetter"      , "ML"                   ],
    ["MidNum"         , "MN"                   ],
    ["MidNumLet"      , "MB"                   ],
    ["Numeric"        , "NU"                   ],
    ["ExtendNumLet"   , "EX"                   ]
  ]
)

# http://www.unicode.org/Public/6.0.0/ucd/auxiliary/SentenceBreakProperty.txt
#
# Sentence break properties.
CHAR_SENTENCE_BREAK = Enum(name="SENTENCE_BREAK", default=0,
  data=[
    ["Other"          , "XX"                   ],
    ["CR"             , "CR"                   ],
    ["LF"             , "LF"                   ],
    ["Extend"         , "EX"                   ],
    ["Sep"            , "SE"                   ],
    ["Format"         , "FO"                   ],
    ["Sp"             , "SP"                   ],
    ["Lower"          , "LO"                   ],
    ["Upper"          , "UP"                   ],
    ["OLetter"        , "LE"                   ],
    ["Numeric"        , "NU"                   ],
    ["ATerm"          , "AT"                   ],
    ["STerm"          , "ST"                   ],
    ["Close"          , "CL"                   ],
    ["SContinue"      , "SC"                   ]
  ]
)

# http://www.unicode.org/Public/6.0.0/ucd/LineBreak.txt
#
# Line break properties.
CHAR_LINE_BREAK = Enum(name="LINE_BREAK", default=16,
  data=[
    ["BK"             , "BK"                   ],
    ["CR"             , "CR"                   ],
    ["LF"             , "LF"                   ],
    ["CM"             , "CM"                   ],
    ["SG"             , "SG"                   ],
    ["GL"             , "GL"                   ],
    ["CB"             , "CB"                   ],
    ["SP"             , "SP"                   ],
    ["ZW"             , "ZW"                   ],
    ["NL"             , "NL"                   ],
    ["WJ"             , "WJ"                   ],
    ["JL"             , "JL"                   ],
    ["JV"             , "JV"                   ],
    ["JT"             , "JT"                   ],
    ["H2"             , "H2"                   ],
    ["H3"             , "H3"                   ],

    ["XX"             , "XX"                   ],
    ["OP"             , "OP"                   ],
    ["CL"             , "CL"                   ],
    ["CP"             , "CP"                   ],
    ["QU"             , "QU"                   ],
    ["NS"             , "NS"                   ],
    ["EX"             , "EX"                   ],
    ["SY"             , "SY"                   ],
    ["IS"             , "IS"                   ],
    ["PR"             , "PR"                   ],
    ["PO"             , "PO"                   ],
    ["NU"             , "NU"                   ],
    ["AL"             , "AL"                   ],
    ["ID"             , "ID"                   ],
    ["IN"             , "IN"                   ],
    ["HY"             , "HY"                   ],
    ["BB"             , "BB"                   ],
    ["BA"             , "BA"                   ],
    ["SA"             , "SA"                   ],
    ["AI"             , "AI"                   ],
    ["B2"             , "B2"                   ]
  ]
)

# Joining.
CHAR_JOINING = Enum(name="JOINING", default=0,
  data=[
    ["U"              , "U"                    ],
    ["L"              , "L"                    ],
    ["R"              , "R"                    ],
    ["D"              , "D"                    ],
    ["C"              , "C"                    ],
    ["T"              , "T"                    ]
  ]
)

CHAR_EAW = Enum(name="CHAR_EAW", default=0,
  data=[
    ["A"              , "A"                    ],
    ["N"              , "N"                    ],
    ["F"              , "F"                    ],
    ["H"              , "H"                    ],
    ["Na"             , "NA"                   ],
    ["W"              , "W"                    ]
  ]
)

# Map type (Fog-Framework specific optimization).
CHAR_MAPPING = Enum(name="CHAR_MAPPING", default=0,
  data=[
    ["None"           , "NONE"                 ],
    ["Uppercase"      , "LOWERCASE"            ],
    ["Lowercase"      , "UPPERCASE"            ],
    ["Mirror"         , "MIRROR"               ],
    ["Digit"          , "DIGIT"                ],
    ["Special"        , "SPECIAL"              ]
  ]
)

CHAR_QUICK_CHECK = Enum(name="CHAR_QUICK_CHECK", default=0,
  data=[
    ["No"             , "NO"                   ],
    ["Yes"            , "YES"                  ],
    ["Maybe"          , "MAYBE"                ]
  ],
  alt={
    "N": "No",
    "Y": "Yes",
    "M": "Maybe"
  }
)

CHAR_VERSION = Enum(name="CHAR_VERSION", default=0,
  # Normal map, used by DerivedAge.txt.
  data=[
    [""               , "UNASSIGNED"           ],
    ["1.1"            , "V1_1"                 ],
    ["2.0"            , "V2_0"                 ],
    ["2.1"            , "V2_1"                 ],
    ["3.0"            , "V3_0"                 ],
    ["3.1"            , "V3_1"                 ],
    ["3.2"            , "V3_2"                 ],
    ["4.0"            , "V4_0"                 ],
    ["4.1"            , "V4_1"                 ],
    ["5.0"            , "V5_0"                 ],
    ["5.1"            , "V5_1"                 ],
    ["5.2"            , "V5_2"                 ],
    ["6.0"            , "V6_0"                 ]
  ],
  # Alternative map, used by NormalizationCorrections.txt.
  alt={
    "3.2.0": "3.2",
    "4.0.0": "4.0"
  }
)

TEXT_SCRIPT = Enum(name="TEXT_SCRIPT", default=0,
  data=[
    ["Unknown"               , "UNKNOWN"              ],
    ["Common"                , "COMMON"               ],
    ["Inherited"             , "INHERITED"            ],

    ["Latin"                 , "LATIN"                ],

    ["Arabic"                , "ARABIC"               ],
    ["Armenian"              , "ARMENIAN"             ],
    ["Avestan"               , "AVESTAN"              ],
    ["Balinese"              , "BALINESE"             ],
    ["Bamum"                 , "BAMUM"                ],
    ["Batak"                 , "BATAK"                ],
    ["Bengali"               , "BENGALI"              ],
    ["Bopomofo"              , "BOPOMOFO"             ],
    ["Brahmi"                , "BRAHMI"               ],
    ["Braille"               , "BRAILLE"              ],
    ["Buginese"              , "BUGINESE"             ],
    ["Buhid"                 , "BUHID"                ],
    ["Canadian_Aboriginal"   , "CANADIAN_ABORIGINAL"  ],
    ["Carian"                , "CARIAN"               ],
    ["Cham"                  , "CHAM"                 ],
    ["Cherokee"              , "CHEROKEE"             ],
    ["Coptic"                , "COPTIC"               ],
    ["Cuneiform"             , "CUNEIFORM"            ],
    ["Cypriot"               , "CYPRIOT"              ],
    ["Cyrillic"              , "CYRILLIC"             ],
    ["Devanagari"            , "DEVANAGARI"           ],
    ["Deseret"               , "DESERET"              ],
    ["Egyptian_Hieroglyphs"  , "EGYPTIAN_HIEROGLYPHS" ],
    ["Ethiopic"              , "ETHIOPIC"             ],
    ["Georgian"              , "GEORGIAN"             ],
    ["Glagolitic"            , "GLAGOLITIC"           ],
    ["Gothic"                , "GOTHIC"               ],
    ["Greek"                 , "GREEK"                ],
    ["Gujarati"              , "GUJARATI"             ],
    ["Gurmukhi"              , "GURMUKHI"             ],
    ["Han"                   , "HAN"                  ],
    ["Hangul"                , "HANGUL"               ],
    ["Hanunoo"               , "HANUNOO"              ],
    ["Hebrew"                , "HEBREW"               ],
    ["Hiragana"              , "HIRAGANA"             ],
    ["Imperial_Aramaic"      , "IMPERIAL_ARAMAIC"     ],
    ["Inscriptional_Pahlavi" , "INSCRIPTIONAL_PAHLAVI"],
    ["Inscriptional_Parthian", "INSCRIPTIONAL_PARTHIAN"],
    ["Javanese"              , "JAVANESE"             ],
    ["Kaithi"                , "KAITHI"               ],
    ["Kannada"               , "KANNADA"              ],
    ["Katakana"              , "KATAKANA"             ],
    ["Kayah_Li"              , "KAYAH_LI"             ],
    ["Kharoshthi"            , "KHAROSHTHI"           ],
    ["Khmer"                 , "KHMER"                ],
    ["Lao"                   , "LAO"                  ],
    ["Lepcha"                , "LEPCHA"               ],
    ["Limbu"                 , "LIMBU"                ],
    ["Linear_B"              , "LINEAR_B"             ],
    ["Lisu"                  , "LISU"                 ],
    ["Lycian"                , "LYCIAN"               ],
    ["Lydian"                , "LYDIAN"               ],
    ["Malayalam"             , "MALAYALAM"            ],
    ["Mandaic"               , "MANDAIC"              ],
    ["Meetei_Mayek"          , "MEETEI_MAYEK"         ],
    ["Mongolian"             , "MONGOLIAN"            ],
    ["Myanmar"               , "MAYANMAR"             ],
    ["New_Tai_Lue"           , "NEW_TAI_LUE"          ],
    ["Nko"                   , "NKO"                  ],
    ["Ogham"                 , "OGHAM"                ],
    ["Ol_Chiki"              , "OL_CHIKI"             ],
    ["Old_Italic"            , "OLD_ITALIC"           ],
    ["Old_Persian"           , "OLD_PERSIAN"          ],
    ["Old_South_Arabian"     , "OLD_SOUTH_ARABIAN"    ],
    ["Old_Turkic"            , "OLD_TURKIC"           ],
    ["Oriya"                 , "ORIYA"                ],
    ["Osmanya"               , "OSMANYA"              ],
    ["Phags_Pa"              , "PHAGS_PA"             ],
    ["Phoenician"            , "PHOENICIAN"           ],
    ["Rejang"                , "REJANG"               ],
    ["Runic"                 , "RUNIC"                ],
    ["Samaritan"             , "SAMARITAN"            ],
    ["Saurashtra"            , "SAURASHTRA"           ],
    ["Shavian"               , "SHAVIAN"              ],
    ["Sinhala"               , "SINHALA"              ],
    ["Sundanese"             , "SUNDANESE"            ],
    ["Syloti_Nagri"          , "SYLOTI_NAGRI"         ],
    ["Syriac"                , "SYRIAC"               ],
    ["Tagalog"               , "TAGALOG"              ],
    ["Tagbanwa"              , "TAGBANWA"             ],
    ["Tai_Le"                , "TAI_LE"               ],
    ["Tai_Tham"              , "TAI_THAM"             ],
    ["Tai_Viet"              , "TAI_VIET"             ],
    ["Tamil"                 , "TAMIL"                ],
    ["Telugu"                , "TELUGU"               ],
    ["Thaana"                , "THAANA"               ],
    ["Thai"                  , "THAI"                 ],
    ["Tibetan"               , "TIBETAN"              ],
    ["Tifinagh"              , "TIFINAGH"             ],
    ["Ugaritic"              , "UGARITIC"             ],
    ["Vai"                   , "VAI"                  ],
    ["Yi"                    , "YI"                   ]
  ]
)

# -----------------------------------------------------------------------------
# [UCD-CharProperty]
# -----------------------------------------------------------------------------

class CharProperty(object):
  used = 0
  codePoint = 0
  name = ""

  category = CHAR_CATEGORY.get("")
  combiningClass = 0
  bidi = CHAR_BIDI.get("L")
  joining = CHAR_JOINING.get("U")
  digitValue = -1

  decompositionType = CHAR_DECOMPOSITION.get("")
  decompositionData = None

  compositionExclusion = False
  nfdQC = CHAR_QUICK_CHECK.get("Yes")
  nfcQC = CHAR_QUICK_CHECK.get("Yes")
  nfkdQC = CHAR_QUICK_CHECK.get("Yes")
  nfkcQC = CHAR_QUICK_CHECK.get("Yes")

  fullCaseFolding = None
  simpleCaseFolding = 0
  specialCaseFolding = 0

  graphemeBreak = CHAR_GRAPHEME_BREAK.get("")
  wordBreak = CHAR_WORD_BREAK.get("")
  sentenceBreak = CHAR_SENTENCE_BREAK.get("")
  lineBreak = CHAR_LINE_BREAK.get("")

  lowercase = 0
  uppercase = 0
  titlecase = 0
  mirror = 0

  script = TEXT_SCRIPT.get("")
  eaw = CHAR_EAW.get("")
  version = CHAR_VERSION.get("")

  # Fog-specific memory optimization.
  mapping = CHAR_MAPPING.get("None")

  def __init__(self, **kw):
    self.decompositionData = []
    self.fullCaseFolding = []

    for prop in kw:
      setattr(self, prop, kw[prop])

  # Get the difference between the code-point and uppercase.
  @property
  def upperdiff(self):
    if self.uppercase != 0:
      return self.uppercase - self.codePoint
    else:
      return 0

  # Get the difference between the code-point and lowercase.
  @property
  def lowerdiff(self):
    if self.lowercase != 0:
      return self.lowercase - self.codePoint
    else:
      return 0

  # Get the difference between the code-point and titlecase.
  @property
  def titlediff(self):
    if self.titlecase != 0:
      return self.titlecase - self.codePoint
    else:
      return 0

  # Get the difference between the code-point and mirrored one.
  @property
  def mirrordiff(self):
    if self.mirror != 0:
      return self.mirror - self.codePoint
    else:
      return 0

  @property
  def isSpace(self):
    return self.category == CHAR_CATEGORY.get("Zs") or \
           self.category == CHAR_CATEGORY.get("Zl") or \
           self.category == CHAR_CATEGORY.get("Zp") or \
           (self.codePoint >= 9 and self.codePoint <= 13)

# -----------------------------------------------------------------------------
# [UCD-BlockProperty]
# -----------------------------------------------------------------------------

class BlockProperty(object):
  def __init__(self, **kw):
    self.first = 0
    self.last = 0
    self.name = ""

    for prop in kw:
      setattr(self, prop, kw[prop])

# -----------------------------------------------------------------------------
# [UCD-NormalizationCorrection]
# -----------------------------------------------------------------------------

class NormalizationCorrection(object):
  def __init__(self, **kw):
    self.code = 0
    self.original = 0
    self.corrected = 0
    self.version = CHAR_VERSION.get("")

    for prop in kw:
      setattr(self, prop, kw[prop])

# -----------------------------------------------------------------------------
# [UCD-Globals (Data readed from UCD)]
# -----------------------------------------------------------------------------

# Character to CharacterProperty mapping.
CharList = []
# Block list (from ucd/Blocks.txt").
BlockList = []
# Normalization corrections.
NormalizationCorrectionsList = []

def PrepareTables():
  log("-- Preparing...")
  for i in xrange(NUM_CODE_POINTS):
    CharList.append(CharProperty(codePoint=i))

# -----------------------------------------------------------------------------
# [UCD-Read]
# -----------------------------------------------------------------------------

def ReadPropertyValueAliases():
  log("-- Read ucd/PropertyValueAliases.txt")
  f = open(UCD_DIRECTORY + "ucd/PropertyValueAliases.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)

    # Just for safety
    if len(data) < 3:
      continue

    property = data[0]
    name = data[2]
    
    aliases = [data[1]]
    if len(data) > 3:
      for i in xrange(3, len(data)):
        aliases.append(data[i])

    # Sentence break aliases
    if property == "SB":
      for alias in aliases:
        CHAR_SENTENCE_BREAK.addAlias(alias, CHAR_SENTENCE_BREAK.get(name))

    # Script aliases.
    if property == "sc":
      # Ignore these.
      if name == "Katakana_Or_Hiragana":
        continue

      for alias in aliases:
        TEXT_SCRIPT.addAlias(alias, TEXT_SCRIPT.get(name))

  f.close()

def ReadUnicodeData():
  log("-- Read ucd/UnicodeData.txt")
  f = open(UCD_DIRECTORY + "ucd/UnicodeData.txt", 'r')

  first = -1
  last = -1

  for line in LineIterator(f):
    data = SplitData(line)
    code = int(data[UCD_VALUE], 16)
    name = data[UCD_NAME]

    # Parse a single code-point or range of code-points.
    if name.startswith("<") and name.find("First") != -1:
      first = code
      last = -1
      continue

    if name.startswith("<") and name.find("Last") != -1:
      assert(first != -1)
      last = code
    else:
      first = code
      last = code

    # Process the character (or range).
    for code in xrange(first, last+1):
      c = CharList[code]
      c.used = 1

      c.category = CHAR_CATEGORY.get(data[UCD_CATEGORY])
      c.combiningClass = int(data[UCD_COMBINING_CLASS])

      # Bidi-category.
      if data[UCD_BIDI]:
        c.bidi = CHAR_BIDI.get(data[UCD_BIDI])
      else:
        # Default BIDI property is related to the code-point.
        #
        # The unassigned code points that default to AL are in the ranges:
        #     [\u0600-\u07BF \uFB50-\uFDFF \uFE70-\uFEFF]
        #   - minus noncharacter code points.
        if code >= 0x0600 and code <= 0x07BF: c.bidi = CHAR_BIDI.get("AL")
        if code >= 0xFB50 and code <= 0xFDFF: c.bidi = CHAR_BIDI.get("AL")
        if code >= 0xFE70 and code <= 0xFEFF: c.bidi = CHAR_BIDI.get("AL")

        # The unassigned code points that default to R are in the ranges:
        #     [\u0590-\u05FF \u07C0-\u08FF \uFB1D-\uFB4F \U00010800-\U00010FFF \U0001E800-\U0001EFFF]
        if code >= 0x0590 and code <= 0x05FF: c.bidi = CHAR_BIDI.get("R")
        if code >= 0x07C0 and code <= 0x08FF: c.bidi = CHAR_BIDI.get("R")
        if code >= 0xFB1D and code <= 0xFB4F: c.bidi = CHAR_BIDI.get("R")

        if code >= 0x00010800 and code <= 0x00010FFF: c.bidi = CHAR_BIDI.get("R")
        if code >= 0x0001E800 and code <= 0x0001EFFF: c.bidi = CHAR_BIDI.get("R")

      # Digit value.
      if len(data[UCD_DIGIT_VALUE]):
        c.digitValue = int(data[UCD_DIGIT_VALUE], 10)

      # Uppercase/Lowercase/Titlecase - Assign the case change only if it
      # differs to the original code-point.
      if data[UCD_TITLECASE] == "":
        # If the TITLECASE is NULL, it has the value UPPERCASE.
        data[UCD_TITLECASE] = data[UCD_UPPERCASE]
      
      if data[UCD_UPPERCASE] and int(data[UCD_UPPERCASE], 16) != code:
        c.uppercase = int(data[UCD_UPPERCASE], 16)
      if data[UCD_LOWERCASE] and int(data[UCD_LOWERCASE], 16) != code:
        c.lowercase = int(data[UCD_LOWERCASE], 16)
      if data[UCD_TITLECASE] and int(data[UCD_TITLECASE], 16) != code:
        c.titlecase = int(data[UCD_TITLECASE], 16)

      # Joining (from Unicode 6.0):
      # - Those that not explicitly listed that are of General Category Mn, Me, or Cf
      #   have joining type T.
      # - All others not explicitly listed have joining type U.
      if c.category == CHAR_CATEGORY.get("Mn") or c.category == CHAR_CATEGORY.get("Me") or c.category == CHAR_CATEGORY.get("Cf"):
        c.joining = CHAR_JOINING.get("T")
      else:
        c.joining = CHAR_JOINING.get("U")

      # Decomposition.
      if data[UCD_DECOMPOSITION]:
        dec = data[UCD_DECOMPOSITION].split(' ')

        # Parse <decompositionType>.
        if dec[0].startswith('<'):
          c.decompositionType = CHAR_DECOMPOSITION.get(dec[0])
          dec = dec[1:]
        else:
          c.decompositionType = CHAR_DECOMPOSITION.get("<canonical>")

        # Decomposition character list.
        for d in dec:
          c.decompositionData.append(int(d, 16))

  f.close()

def ReadXBreakHelper(fileName, propertyName, MAP):
  log("-- Read " + fileName)
  f = open(UCD_DIRECTORY + fileName, 'rb')

  for line in LineIterator(f):
    data = SplitData(line)
    first, last = GetCharRange(data[0])

    for code in xrange(first, last + 1):
      c = CharList[code]
      setattr(c, propertyName, MAP.get(data[1]))

  f.close()

def ReadLineBreak():
  ReadXBreakHelper("ucd/LineBreak.txt", "lineBreak", CHAR_LINE_BREAK)

def ReadGraphemeBreak():
  ReadXBreakHelper("ucd/auxiliary/GraphemeBreakProperty.txt", "graphemeBreak", CHAR_GRAPHEME_BREAK)

def ReadSentenceBreak():
  ReadXBreakHelper("ucd/auxiliary/SentenceBreakProperty.txt", "sentenceBreak", CHAR_SENTENCE_BREAK)

def ReadWordBreak():
  ReadXBreakHelper("ucd/auxiliary/WordBreakProperty.txt", "wordBreak", CHAR_WORD_BREAK)

def ReadArabicShaping():
  log("-- Read ucd/ArabicShaping.txt")
  f = open(UCD_DIRECTORY + "ucd/ArabicShaping.txt", 'rb')

  # Note: Code points that are not explicitly listed in this file are
  # either of joining type T or U:
  #
  # - Those that not explicitly listed that are of General Category Mn, Me, or Cf
  #   have joining type T.
  # - All others not explicitly listed have joining type U.
  #
  # Note: U is default when CharProperty is created, need to setup the "T" joining
  # only.
  for code in xrange(NUM_CODE_POINTS):
    c = CharList[code]
    if c.category == CHAR_CATEGORY.get("Mn") or \
       c.category == CHAR_CATEGORY.get("Me") or \
       c.category == CHAR_CATEGORY.get("Cf"): c.joining = CHAR_JOINING.get("T")

  for line in LineIterator(f):
    data = SplitData(line)
    code = int(data[0], 16)

    c = CharList[code]
    c.joining = CHAR_JOINING.get(data[2])

  f.close()

def ReadBidiMirroring():
  log("-- Read ucd/BidiMirroring.txt")
  f = open(UCD_DIRECTORY + "ucd/BidiMirroring.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)
    code = int(data[0], 16)

    c = CharList[code]
    c.mirror = int(data[1], 16)

  f.close()

def ReadCaseFolding():
  log("-- Read ucd/CaseFolding.txt")
  f = open(UCD_DIRECTORY + "ucd/CaseFolding.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)

    code = int(data[0], 16)
    status = data[1]
    mapping = data[2].split(' ')
    for i in xrange(len(mapping)):
      mapping[i] = int(mapping[i].strip(), 16)

    c = CharList[code]

    # "C" - Common case-folding ("S" and "F" shared).
    if status == "C":
      assert len(mapping) == 1
      c.simpleCaseFolding = mapping[0]
      c.fullCaseFolding = mapping

    # "F" - Full case-folding.
    if status == "F":
      c.fullCaseFolding = mapping

    # "S" - Simple case-folding.
    if status == "S":
      c.simpleCaseFolding = mapping[0]

    # "T" - Special case-folding.
    if status == "T":
      c.specialCaseFolding = mapping[0]

  f.close()

def ReadSpecialCasing():
  log("-- Read ucd/SpecialCasing.txt")
  f = open(UCD_DIRECTORY + "ucd/SpecialCasing.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)

  # TODO
  # TODO
  # TODO
  # TODO
  # TODO
  # TODO

  f.close()

def ReadDerivedAge():
  log("-- Read ucd/DerivedAge.txt")
  f = open(UCD_DIRECTORY + "ucd/DerivedAge.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)
    first, last = GetCharRange(data[0])

    version = CHAR_VERSION.get(data[1])

    for code in xrange(first, last + 1):
      c = CharList[code]
      c.version = version

  f.close()

def ReadDerivedNormalizationProps():
  log("-- Read ucd/DerivedNormalizationProps.txt")
  f = open(UCD_DIRECTORY + "ucd/DerivedNormalizationProps.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)
    first, last = GetCharRange(data[0])

    for code in xrange(first, last + 1):
      c = CharList[code]
      prop = data[1]

      # FullCompositionExclusion.
      if prop == "Full_Composition_Exclusion":
        c.compositionExclusion = True

      # Quick-Check.
      if prop == "NFD_QC":
        c.nfdQC = CHAR_QUICK_CHECK.get(data[2])
      if prop == "NFC_QC":
        c.nfcQC = CHAR_QUICK_CHECK.get(data[2])
      if prop == "NFKD_QC":
        c.nfkdQC = CHAR_QUICK_CHECK.get(data[2])
      if prop == "NFKC_QC":
        c.nfkcQC = CHAR_QUICK_CHECK.get(data[2])

  f.close()

def ReadNormalizationCorrections():
  log("-- Read ucd/NormalizationCorrections.txt")
  f = open(UCD_DIRECTORY + "ucd/NormalizationCorrections.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)

    code = int(data[0], 16)
    orig = int(data[1], 16)
    corrected = int(data[2], 16)
    version = CHAR_VERSION.get(data[3])

    item = NormalizationCorrection(
      code=code,
      original=orig,
      corrected=corrected,
      version=version)
    NormalizationCorrectionsList.append(item)

  f.close()

def ReadEastAsianWidth():
  log("-- Read ucd/EastAsianWidth.txt")
  f = open(UCD_DIRECTORY + "ucd/EastAsianWidth.txt", 'rb')
  
  # The unassigned code points that default to "W" include ranges in the
  # following blocks:
  #     CJK Unified Ideographs Extension A:       U+3400..U+4DBF
  #     CJK Unified Ideographs:                   U+4E00..U+9FFF
  #     CJK Compatibility Ideographs:             U+F900..U+FAFF
  #     CJK Unified Ideographs Extension B:      U+20000..U+2A6DF 
  #     CJK Unified Ideographs Extension C:      U+2A700..U+2B73F
  #     CJK Unified Ideographs Extension D:      U+2B740..U+2B81F
  #     CJK Compatibility Ideographs Supplement: U+2F800..U+2FA1F
  #     and any other reserved code points on
  #         Planes 2 and 3:                      U+20000..U+2FFFD
  #                                              U+30000..U+3FFFD
  W = CHAR_EAW.get("W")
  for code in xrange(0x003400, 0x004DBF+1): CharList[code].eaw = W
  for code in xrange(0x004E00, 0x009FFF+1): CharList[code].eaw = W
  for code in xrange(0x00F900, 0x00FAFF+1): CharList[code].eaw = W
  for code in xrange(0x020000, 0x02A6DF+1): CharList[code].eaw = W
  for code in xrange(0x02A700, 0x02B73F+1): CharList[code].eaw = W
  for code in xrange(0x02B740, 0x02B81F+1): CharList[code].eaw = W
  for code in xrange(0x02F800, 0x02FA1F+1): CharList[code].eaw = W
  for code in xrange(0x020000, 0x02FFFD+1): CharList[code].eaw = W
  for code in xrange(0x030000, 0x03FFFD+1): CharList[code].eaw = W

  for line in LineIterator(f):
    data = SplitData(line)
    first, last = GetCharRange(data[0])
    eaw = CHAR_EAW.get(data[1])

    for code in xrange(first, last + 1):
      c = CharList[code]
      c.eaw = eaw

  f.close()

def ReadScriptsHelper(fileName):
  log("-- Read " + fileName)
  f = open(UCD_DIRECTORY + fileName, 'rb')

  for line in LineIterator(f):
    data = SplitData(line)
    first, last = GetCharRange(data[0])

    script = data[1]
    
    for code in xrange(first, last + 1):
      c = CharList[code]
      c.script = TEXT_SCRIPT.get(script)

  # TODO
  # TODO
  # TODO
  # TODO
  # TODO
  # TODO

  f.close()
  
def ReadScripts():
  ReadScriptsHelper("ucd/Scripts.txt")
  # ReadScriptsHelper("ucd/ScriptExtensions.txt")

def ReadBlocks():
  log("-- Read ucd/Blocks.txt")
  f = open(UCD_DIRECTORY + "ucd/Blocks.txt", 'rb')

  for line in LineIterator(f):
    data = SplitData(line)
    first, last = GetCharRange(data[0])
    name = data[1]

    BlockList.append(BlockProperty(first=first, last=last, name=name))

  f.close()

# -----------------------------------------------------------------------------
# [UCD-Check]
# -----------------------------------------------------------------------------

def Check():
  log("-- Checking data")
  
  maxCombiningClass = 0
  maxDecompositionChar = 0

  maxSimpleCaseFolding = 0
  maxSpecialCaseFolding = 0
  maxFullCaseFolding = 0
  maxSpace = 0

  for i in xrange(NUM_CODE_POINTS):
    c = CharList[i]
    
    if maxCombiningClass < c.combiningClass:
      maxCombiningClass = c.combiningClass
      
    if c.decompositionData:
      for d in c.decompositionData:
        if maxDecompositionChar < d:
          maxDecompositionChar = d
          
    if c.simpleCaseFolding:
      if maxSimpleCaseFolding < c.simpleCaseFolding:
        maxSimpleCaseFolding = c.simpleCaseFolding

    if c.specialCaseFolding:
      if maxSpecialCaseFolding < c.specialCaseFolding:
        maxSpecialCaseFolding = c.specialCaseFolding

    if c.fullCaseFolding:
      for d in c.fullCaseFolding:
        if maxFullCaseFolding < d:
          maxFullCaseFolding = d

    if c.isSpace:
      if maxSpace < i:
        maxSpace = i

    # Check whether the uppercase, lowercase, titlecase and mirror mapping 
    # doesn't exceed our limit (in this case Fog-API must be changed).
    if c.uppercase:
      if abs(c.uppercase - i) > MAX_PAIRED_DIFF:
        log("** FATAL: " + ucd(c.codePoint) + " and uppercase " + ucd(c.uppercase) + " characters are too far.")
      if (c.codePoint < 0x10000 and c.uppercase >=0x10000) or \
         (c.codePoint >=0x10000 and c.uppercase < 0x10000):
        log("** FATAL: Code point " + ucd(c.codePoint) + " and uppercase character " + ucd(c.uppercase) + "are in different plane (BMP/SMP combination)")

    if c.lowercase:
      if abs(c.lowercase - i) > MAX_PAIRED_DIFF:
        log("** FATAL: " + ucd(c.codePoint) + " and lowercase " + ucd(c.lowercase) + " characters are too far.")
      if (c.codePoint < 0x10000 and c.lowercase >=0x10000) or \
         (c.codePoint >=0x10000 and c.lowercase < 0x10000):
        log("** FATAL: Code point " + ucd(c.codePoint) + " and lowercase character " + ucd(c.lowercase) + "are in different plane (BMP/SMP combination)")

    if c.titlecase != 0:
      if abs(c.titlecase - i) > MAX_PAIRED_DIFF:
        log("** FATAL: " + ucd(c.codePoint) + " and titlecase " + ucd(c.titlecase) + " characters are too far.")
      if (c.codePoint < 0x10000 and c.titlecase >=0x10000) or \
         (c.codePoint >=0x10000 and c.titlecase < 0x10000):
        log("** FATAL: Code point " + ucd(c.codePoint) + " and titlecase character " + ucd(c.titlecase) + "are in different plane (BMP/SMP combination)")

    if c.mirror != 0:
      if abs(c.mirror - i) > MAX_PAIRED_DIFF:
        log("** FATAL: " + ucd(c.codePoint) + " and mirrored " + ucd(c.mirror) + " characters are too far.")
      if (c.codePoint < 0x10000 and c.mirror >=0x10000) or \
         (c.codePoint >=0x10000 and c.mirror < 0x10000):
        log("** FATAL: Code point " + ucd(c.codePoint) + " and mirror character " + ucd(c.mirror) + "are in different plane (BMP/SMP combination)")

    # Check if our assumption that "mirrored/digit character never has lowercase,
    # uppercase, or titlecase variant" is valid.
    if c.uppercase + c.lowercase + c.titlecase != 0 and (c.mirror != 0 or c.digitValue != -1):
      log("** FATAL: " + ucd(c.codePoint) + " - contains both, case and mirror mapping.")

    # Check another assumption that there is no character that maps to all
    # three categories (uppercase, lowercase, and titlecase). Mapping to itself
    # is never used.
    if c.uppercase != 0 and c.lowercase != 0 and c.titlecase != 0:
      log("** FATAL: " + ucd(c.codePoint) + " - contains upper("+ucd(c.uppercase)+")+lower(" + ucd(c.lowercase) + ")+title(" + ucd(c.titlecase) + ") mapping.")

  log("   MAX COMBINING CLASS     : " + str(maxCombiningClass))
  log("   MAX DECOMPOSED CHAR     : " + str(maxDecompositionChar))
  log("   MAX SIMPLE-CASE-FOLDING : " + str(maxSimpleCaseFolding))
  log("   MAX SPECIAL-CASE-FOLDING: " + str(maxSpecialCaseFolding))
  log("   MAX FULL-CASE-FOLDING   : " + str(maxFullCaseFolding))
  log("   MAX SPACE               : " + str(maxSpace))

def PrintTitleCase():
  log("-- Printing all titlecases characters")

  chars = {}

  for i in xrange(NUM_CODE_POINTS):
    c = CharList[i]

    if c.category == CHAR_CATEGORY.get("Lt"):
      chars[c.codePoint] = True

  for c in chars:
    c = CharList[c]
    log("   TC=" + ucd(c.codePoint) + " LC=" + ucd(c.lowercase) + " UC=" + ucd(c.uppercase))

# -----------------------------------------------------------------------------
# [UCD-Generate]
# -----------------------------------------------------------------------------

class Code(object):
  def __init__(self):
    # Use python list instead of string concatenation, it's many
    # times faster when concatenating a very large string.
    self.OffsetDecl = []
    self.OffsetRows = []

    self.PropertyClass = []
    self.PropertyDecl = []
    self.PropertyRows = []

    self.SpecialClass = []
    self.SpecialDecl = []
    self.SpecialRows = []

    self.InfoClass = []
    self.InfoDecl = []
    self.InfoRows = []

    self.DecompositionDecl = []
    self.DecompositionData = []

    self.MethodsInline = []
    self.TextScriptEnum = []

Code = Code()

# Please update when needed.
SIZE_OF_OFFSET_ROW = 1 # If number of blocks is smaller than 256, otherwise 2
SIZE_OF_PROPERTY_ROW = 8
SIZE_OF_INFO_ROW = 8
SIZE_OF_SPECIAL_ROW = 36

# Generator configuration (and some constants).
OUTPUT_BLOCK_SIZE = 128

# Whether to only test generator (output to stdout instead of writing to .h/.cpp).
TEST_ONLY = False

# The comment to include in auto-generated code.
AUTOGENERATED_BEGIN = "// --- Auto-generated by GenUnicode.py (Unicode " + UCD_VERSION + ") ---"
AUTOGENERATED_END   = "// --- Auto-generated by GenUnicode.py (Unicode " + UCD_VERSION + ") ---"

def GenerateSpecial():
  log("-- Generating special characters info (ligatures, case-folding, ...)")

  for i in xrange(NUM_CODE_POINTS):
    c = CharList[i]

    if c.titlecase == 0 and ((c.lowercase == 0 and c.mirror == 0) or \
                             (c.uppercase == 0 and c.mirror == 0) or \
                             (c.lowercase == 0 and c.uppercase == 0)):
      # Mark character as not special. This means that CHAR_MAPPING must contain one
      # of NONE, LOWERCASE, UPPERCASE or MIRROR.
      if c.uppercase != 0:
        c.mapping = CHAR_MAPPING.get("Uppercase")
      elif c.lowercase != 0:
        c.mapping = CHAR_MAPPING.get("Lowercase")
      elif c.mirror != 0:
        c.mapping = CHAR_MAPPING.get("Mirror")
      elif c.digitValue != -1:
        c.mapping = CHAR_MAPPING.get("Digit")
    else:
      # Special (complex) character mapping.
      c.mapping = CHAR_MAPPING.get("Special")

def GenerateClasses():
  pass

def GenerateTable():
  global SIZE_OF_OFFSET_ROW
  log("-- Generating tables")
  
  rowBlockHash = {}
  rowBlockList = []
  rowBlockData = ""
  rowBlockFirst = -1

  specialHash = {}
  specialList = []

  offsetList = []
  
  decompositionHash = {}
  decompositionList = []
  decompositionList.append(0)
  maxDecompositionLength = 0

  infoHash = {}
  infoList = []

  def GetOffsetShift(val):
    result = -1
    while val:
      result += 1
      val = val >> 1
    return result

  def GetOffsetMask(val):
    return val - 1

  # ---------------------------------------------------------------------------
  # Headers.
  # ---------------------------------------------------------------------------

  Code.PropertyClass.append("" + \
    "  " + AUTOGENERATED_BEGIN + "\n"        + \
    "\n"                                     + \
    "  // First 32-bit packed int.\n"        + \
    "  // - uint32_t _infoIndex : 12;\n"     + \
    "  // - uint32_t _mappingType : 3;\n"    + \
    "  // - int32_t _mappingData : 17;\n"    + \
    "  // Needed to pack to a signle integer, because of different types used.\n" + \
    "  int32_t _infoAndMapping;\n"           + \
    "\n"                                     + \
    "  // Second 32-bit packed int.\n"       + \
    "  uint32_t _category : 5;\n"            + \
    "  uint32_t _space : 1;\n"               + \
    "  uint32_t _decompositionType : 5;\n"   + \
    "  uint32_t _decompositionIndex : 15;\n" + \
    "  uint32_t _unused : 6;\n"              + \
    "\n"                                     + \
    "  " + AUTOGENERATED_END + "\n"          + \
    ""
  )

  Code.InfoClass.append("" + \
    "  " + AUTOGENERATED_BEGIN + "\n"        + \
    "\n"                                     + \
    "  // First 32-bit packed int.\n"        + \
    "  uint32_t _combiningClass : 8;\n"      + \
    "  uint32_t _script : 8;\n"              + \
    "  uint32_t _unicodeVersion : 4;\n"      + \
    "  uint32_t _graphemeBreak : 4;\n"       + \
    "  uint32_t _wordBreak : 4;\n"           + \
    "  uint32_t _sentenceBreak : 4;\n"       + \
    "\n"                                     + \
    "  // Second 32-bit packed int.\n"       + \
    "  uint32_t _lineBreak : 6;\n"           + \
    "  uint32_t _bidi : 5;\n"                + \
    "  uint32_t _joining : 3;\n"             + \
    "  uint32_t _eaw : 3;\n"                 + \
    "  uint32_t _compositionExclusion : 1;\n"+ \
    "  uint32_t _nfdQC : 1;\n"               + \
    "  uint32_t _nfcQC : 2;\n"               + \
    "  uint32_t _nfkdQC : 1;\n"              + \
    "  uint32_t _nfkcQC : 2;\n"              + \
    "  uint32_t _unused : 8;\n"              + \
    "\n"                                     + \
    "  " + AUTOGENERATED_END + "\n"          + \
    ""
  )

  Code.SpecialClass.append("" + \
    "  " + AUTOGENERATED_BEGIN + "\n"        + \
    "\n"                                     + \
    "  int32_t _upperCaseDiff : 22;\n"       + \
    "  int32_t _unused0 : 10;\n"             + \
    "\n"                                     + \
    "  int32_t _lowerCaseDiff : 22;\n"       + \
    "  int32_t _unused1 : 10;\n"             + \
    "\n"                                     + \
    "  int32_t _titleCaseDiff : 22;\n"       + \
    "  int32_t _unused2 : 10;\n"             + \
    "\n"                                     + \
    "  int32_t _mirrorDiff : 22;\n"          + \
    "  int32_t _digitValue : 10;\n"          + \
    "\n"                                     + \
    "  uint32_t _simpleCaseFolding;\n"       + \
    "  uint32_t _specialCaseFolding;\n"      + \
    "  uint32_t _fullCaseFolding[3];\n"      + \
    "\n"                                     + \
    "  " + AUTOGENERATED_END + "\n"          + \
    ""
  )

  # ---------------------------------------------------------------------------
  # Collect info and generate data which will be used to generate tables.
  # ---------------------------------------------------------------------------

  class RowBlock(object):
    def __init__(self, offset):
      self.offset = offset
      self.count = 1
      self.chars = []

  for i in xrange(NUM_CODE_POINTS):
    if rowBlockFirst == -1:
      rowBlockFirst = i
      rowBlockData = ""

    c = CharList[i]

    # Info.
    s = "    _CHAR_INFO("
    s += str(c.combiningClass) + ", "                      # CombiningClass
    s += TEXT_SCRIPT.fog(c.script) + ", "                  # Script
    s += CHAR_VERSION.fog(c.version) + ", "                # UnicodeVersion
    s += CHAR_GRAPHEME_BREAK.fog(c.graphemeBreak) + ", "   # GraphemeBreak
    s += CHAR_WORD_BREAK.fog(c.wordBreak) + ", "           # WordBreak
    s += CHAR_SENTENCE_BREAK.fog(c.sentenceBreak) + ", "   # SentenceBreak

    s += CHAR_LINE_BREAK.fog(c.lineBreak) + ", "           # LineBreak
    s += CHAR_BIDI.fog(c.bidi) + ", "                      # Bidi
    s += CHAR_JOINING.fog(c.joining) + ", "                # Joining
    s += CHAR_EAW.fog(c.eaw) + ", "                        # East-Asian Width.

    s += str(int(c.compositionExclusion)) + ", "           # CompositionExclusion.
    s += CHAR_QUICK_CHECK.fog(c.nfdQC) + ", "              # NFD_QC.
    s += CHAR_QUICK_CHECK.fog(c.nfcQC) + ", "              # NFC_QC.
    s += CHAR_QUICK_CHECK.fog(c.nfkdQC) + ", "             # NFKD_QC.
    s += CHAR_QUICK_CHECK.fog(c.nfkcQC)                    # NFKC_QC.

    s += ")"
    if not s in infoHash:
      infoHash[s] = len(infoList)
      infoList.append(s)

    infoIndex = infoHash[s]

    # Decomposition data.
    decompositionDataIndex = 0
    decompositionDataLength = 0

    s = ""
    if len(c.decompositionData) > 0:
      dt = [0]
      for cp in c.decompositionData:
        if cp > 0x10000:
          dt.extend(SurrogatePairOf(cp))
        else:
          dt.append(cp)
      dt[0] = len(dt) - 1

      if len(dt) > maxDecompositionLength:
        maxDecompositionLength = len(dt)

      s += ", ".join([str(item) for item in dt])
      if not s in decompositionHash:
        decompositionHash[s] = len(decompositionList)
        decompositionList.extend(dt)

      decompositionDataIndex = decompositionHash[s]
      decompositionDataLength = len(dt)

    # Mapping.
    mappingConstant = 0
    if c.mapping == CHAR_MAPPING.get("Uppercase"):
      mappingConstant = c.upperdiff

    if c.mapping == CHAR_MAPPING.get("Lowercase"):
      mappingConstant = c.lowerdiff

    if c.mapping == CHAR_MAPPING.get("Mirror"):
      mappingConstant = c.mirrordiff

    if c.mapping == CHAR_MAPPING.get("Digit"):
      mappingConstant = c.digitValue

    if c.mapping == CHAR_MAPPING.get("Special"):
      s = "    " + "{ "

      # UpperCaseDiff/Unused0.
      s += str(c.upperdiff) + ", "
      s += "0, "

      # LowerCaseDiff/Unused1.
      s += str(c.lowerdiff) + ", "
      s += "0, "

      # TitleCaseDiff/Unused2.
      s += str(c.titlediff) + ", "
      s += "0, "

      # MirrorDiff/DigitValue.
      s += str(c.mirrordiff) + ", "
      s += str(c.digitValue) + ", "

      # SimpleCaseFolding+FullCaseFolding.
      s += uch(c.simpleCaseFolding) + ", "
      s += uch(c.specialCaseFolding) + ", "
      s += "{ "
      if c.fullCaseFolding:
        s += ", ".join([uch(t) for t in c.fullCaseFolding])
      else:
        s += "0x0000"
      s += " } "

      s += "}"
      
      if s in specialHash:
        mappingConstant = specialHash[s]
      else:
        mappingConstant = len(specialList)
        specialHash[s] = mappingConstant
        specialList.append(s)

    s = "    _CHAR_PROPERTY("
    s += str(infoIndex) + ", "                              # Info Index.
    s += CHAR_MAPPING.fog(c.mapping) + ", "                 # MappingType.
    s += str(mappingConstant) + ", "                        # MappingData.

    s += CHAR_CATEGORY.fog(c.category) + ", "               # Category.
    s += str(int(c.isSpace)) + ", "                         # Space.
    s += CHAR_DECOMPOSITION.fog(c.decompositionType) + ", " # DecompositionType.
    s += str(decompositionDataIndex)                        # DecompositionIndex.
    s += ")"

    rowBlockData += s

    if i == rowBlockFirst + OUTPUT_BLOCK_SIZE - 1:
      if not rowBlockData in rowBlockHash:
        rowBlockHash[rowBlockData] = RowBlock(len(rowBlockList))
        rowBlockList.append(rowBlockData)
      else:
        rowBlockHash[rowBlockData].count += 1

      rowBlockHash[rowBlockData].chars.append(rowBlockFirst)
      rowBlockFirst = -1
      offsetList.append(rowBlockHash[rowBlockData].offset)
    else:
      rowBlockData += ",\n"

  # If number of row-blocks exceeded 255 then uint16_t must be 
  # used as an insex. It's larger, this is reason why the code
  # is tuned to output data in uint8_t.
  if len(rowBlockList) > 255:
    SIZE_OF_OFFSET_ROW = 2

  # ---------------------------------------------------------------------------
  # Generate info table.
  # ---------------------------------------------------------------------------

  numInfoRows = len(infoList)

  Code.InfoDecl.append("  " + AUTOGENERATED_BEGIN + "\n\n")
  Code.InfoDecl.append("  //! @brief Character info data (see @ref CharProperty::infoIndex).\n")
  Code.InfoDecl.append("  CharInfo info[" + str(numInfoRows) + "];\n\n")
  Code.InfoDecl.append("  " + AUTOGENERATED_END + "\n")

  Code.InfoRows.append("    " + AUTOGENERATED_BEGIN + "\n\n")
  Code.InfoRows.append("" + \
    "#   define _CHAR_INFO(_CombiningClass_, _Script_, _Version_, _GB_, _WB_, _SB_, _LB_, _Bidi_, _Joining_, _EAW_, _CompositionExclusion_, _NFDQC_, _NFCQC_, _NFKDQC_, _NFKCQC_) \\\n" + \
    "    { \\\n" + \
    "      _CombiningClass_,           \\\n" + \
    "      TEXT_SCRIPT_##_Script_,     \\\n" + \
    "      CHAR_UNICODE_##_Version_,   \\\n" + \
    "      CHAR_GRAPHEME_BREAK_##_GB_, \\\n" + \
    "      CHAR_WORD_BREAK_##_WB_,     \\\n" + \
    "      CHAR_SENTENCE_BREAK_##_SB_, \\\n" + \
    "      CHAR_LINE_BREAK_##_LB_,     \\\n" + \
    "      CHAR_BIDI_##_Bidi_,         \\\n" + \
    "      CHAR_JOINING_##_Joining_,   \\\n" + \
    "      CHAR_EAW_##_EAW_,           \\\n" + \
    "      _CompositionExclusion_,     \\\n" + \
    "      CHAR_QUICK_CHECK_##_NFDQC_, \\\n" + \
    "      CHAR_QUICK_CHECK_##_NFCQC_, \\\n" + \
    "      CHAR_QUICK_CHECK_##_NFKDQC_,\\\n" + \
    "      CHAR_QUICK_CHECK_##_NFKCQC_ \\\n" + \
    "    }\n"
  )
  
  for i in xrange(numInfoRows):
    Code.InfoRows.append(infoList[i])
    if i != numInfoRows-1:
      Code.InfoRows.append(",\n")
    else:
      Code.InfoRows.append("\n")

  Code.InfoRows.append("#   undef _CHAR_INFO\n")
  Code.InfoRows.append("\n")
  Code.InfoRows.append("    " + AUTOGENERATED_END + "\n")

  # ---------------------------------------------------------------------------
  # Generate offset table.
  # ---------------------------------------------------------------------------

  numOffsetRows = NUM_CODE_POINTS / OUTPUT_BLOCK_SIZE

  if SIZE_OF_OFFSET_ROW == 1:
    dataType = "uint8_t"
  else:
    dataType = "uint16_t"

  Code.OffsetDecl.append("  " + AUTOGENERATED_BEGIN + "\n\n")
  Code.OffsetDecl.append("  //! @brief Offset to the property block, see @c getCharProperty().\n")
  Code.OffsetDecl.append("  " + dataType + " offset[" + str(numOffsetRows) + "];\n\n")
  Code.OffsetDecl.append("  " + AUTOGENERATED_END + "\n")

  Code.OffsetRows.append("    " + AUTOGENERATED_BEGIN + "\n\n")

  for i in xrange(len(offsetList)):
    Code.OffsetRows.append("    " + str(offsetList[i]))

    if i != len(offsetList) - 1:
      Code.OffsetRows.append(",")
    else:
      Code.OffsetRows.append(" ")

    Code.OffsetRows.append(" // " + ucd(i * OUTPUT_BLOCK_SIZE) + ".." + ucd((i + 1) * OUTPUT_BLOCK_SIZE - 1))
    Code.OffsetRows.append("\n")

  Code.OffsetRows.append("\n")
  Code.OffsetRows.append("    " + AUTOGENERATED_END + "\n")

  # ---------------------------------------------------------------------------
  # Generate property table.
  # ---------------------------------------------------------------------------

  numPropertyRows = len(rowBlockList) * OUTPUT_BLOCK_SIZE

  Code.PropertyDecl.append("  " + AUTOGENERATED_BEGIN + "\n\n")
  Code.PropertyDecl.append("  //! @brief Unicode character properties, see @c getCharProperty().\n")
  Code.PropertyDecl.append("  CharProperty property[" + str(numPropertyRows) + "];\n\n")
  Code.PropertyDecl.append("  " + AUTOGENERATED_END + "\n")

  Code.PropertyRows.append("    " + AUTOGENERATED_BEGIN + "\n\n")
  Code.PropertyRows.append("" + \
    "#   define _CHAR_PROPERTY(_InfoIndex_, _MappingType_, _MappingData_, _Category_, _Space_, _DecompositionType_, _DecompositionIndex_) \\\n" + \
    "    { \\\n" + \
    "      ((int32_t)(_InfoIndex_)) |                        \\\n" + \
    "      ((int32_t)(CHAR_MAPPING_##_MappingType_) << 12) | \\\n" + \
    "      ((int32_t)(_MappingData_) << 15),                 \\\n" + \
    "                                                        \\\n" + \
    "      CHAR_CATEGORY_##_Category_,                       \\\n" + \
    "      _Space_,                                          \\\n" + \
    "      CHAR_DECOMPOSITION_##_DecompositionType_,         \\\n" + \
    "      _DecompositionIndex_                              \\\n" + \
    "    }\n"
  )
  Code.PropertyRows.append("\n")

  for i in xrange(len(rowBlockList)):
    rowBlockData = rowBlockList[i]

    # Print Range/Blocks comment (informative)
    chars = rowBlockHash[rowBlockData].chars
    first = -1
    for j in xrange(len(chars)):
      code = chars[j]
      if first == -1:
        first = code
      end = code + OUTPUT_BLOCK_SIZE
      if j != len(chars) - 1 and chars[j+1] == end:
        continue
      Code.PropertyRows.append("    // Range : " + ucd(first) + ".." + ucd(end - 1) + "\n")
      first = -1

    if rowBlockHash[rowBlockData].count > 1:
      Code.PropertyRows.append("    // Blocks: " + str(rowBlockHash[rowBlockData].count) + "\n")

    Code.PropertyRows.append(rowBlockData)

    if i < len(rowBlockList) - 1:
      Code.PropertyRows.append(",\n")
    else:
      Code.PropertyRows.append("\n")

  Code.PropertyRows.append("#   undef _CHAR_PROPERTY\n")
  Code.PropertyRows.append("\n")
  Code.PropertyRows.append("    " + AUTOGENERATED_END + "\n")

  # ---------------------------------------------------------------------------
  # Generate special table.
  # ---------------------------------------------------------------------------

  numSpecialRows = len(specialList)

  Code.SpecialDecl.append("  " + AUTOGENERATED_BEGIN + "\n\n")
  Code.SpecialDecl.append("  //! @brief Unicode special properties.\n")
  Code.SpecialDecl.append("  CharSpecial special[" + str(numSpecialRows) + "];\n\n")
  Code.SpecialDecl.append("  " + AUTOGENERATED_END + "\n")

  Code.SpecialRows.append("    " + AUTOGENERATED_BEGIN + "\n\n")

  for i in xrange(len(specialList)):
    Code.SpecialRows.append(specialList[i])
    if i != len(specialList) - 1:
      Code.SpecialRows.append(",")
    Code.SpecialRows.append("\n")

  Code.SpecialRows.append("\n")
  Code.SpecialRows.append("    " + AUTOGENERATED_END + "\n")

  # ---------------------------------------------------------------------------
  # Generate decomposition table.
  # ---------------------------------------------------------------------------
  
  Code.DecompositionDecl.append("  " + AUTOGENERATED_BEGIN + "\n\n")
  Code.DecompositionDecl.append("  //! @brief Decomposition data.\n")
  Code.DecompositionDecl.append("  uint16_t decomposition[" + str(len(decompositionList)) + "];\n\n")
  Code.DecompositionDecl.append("  " + AUTOGENERATED_END + "\n")

  Code.DecompositionData.append("    " + AUTOGENERATED_BEGIN + "\n\n")
  Code.DecompositionData.append("    ")
  for i in xrange(len(decompositionList)):
    Code.DecompositionData.append(uch(decompositionList[i]))
    if i != len(decompositionList) - 1:
      Code.DecompositionData.append(",")
      if (i > 1) and (i % 8 == 7):
        Code.DecompositionData.append("\n    ")
      else:
        Code.DecompositionData.append(" ")

  Code.DecompositionData.append("\n")
  Code.DecompositionData.append("\n")
  Code.DecompositionData.append("    " + AUTOGENERATED_END + "\n")

  # ---------------------------------------------------------------------------
  # Generate methods
  # ---------------------------------------------------------------------------

  Code.MethodsInline.append("  " + AUTOGENERATED_BEGIN + "\n\n")

  Code.MethodsInline.append("  enum\n")
  Code.MethodsInline.append("  {\n")
  Code.MethodsInline.append("    PO_BLOCK_SIZE = " + str(OUTPUT_BLOCK_SIZE) + ",\n")
  Code.MethodsInline.append("    PO_OFFSET_SHIFT = " + str(GetOffsetShift(OUTPUT_BLOCK_SIZE)) + ",\n")
  Code.MethodsInline.append("    PO_OFFSET_MASK  = " + str(GetOffsetMask(OUTPUT_BLOCK_SIZE)) + "\n")
  Code.MethodsInline.append("  };\n\n")

  Code.MethodsInline.append("  FOG_INLINE uint32_t getPropertyIndexUCS2(uint32_t ucs2) const\n")
  Code.MethodsInline.append("  {\n")
  Code.MethodsInline.append("    return offset[ucs2 >> PO_OFFSET_SHIFT] * PO_BLOCK_SIZE + (ucs2 & PO_OFFSET_MASK);\n")
  Code.MethodsInline.append("  }\n\n")

  Code.MethodsInline.append("  FOG_INLINE uint32_t getPropertyIndexUCS4(uint32_t ucs4) const\n")
  Code.MethodsInline.append("  {\n")
  Code.MethodsInline.append("    // Caller must ensure that ucs4 is in valid range.\n")
  Code.MethodsInline.append("    FOG_ASSERT(ucs4 <= UNICODE_MAX);\n")
  Code.MethodsInline.append("\n")
  Code.MethodsInline.append("    return offset[ucs4 >> PO_OFFSET_SHIFT] * PO_BLOCK_SIZE + (ucs4 & PO_OFFSET_MASK);\n")
  Code.MethodsInline.append("  }\n\n")

  Code.MethodsInline.append("  FOG_INLINE const CharProperty& getPropertyUCS2(uint32_t ucs2) const\n")
  Code.MethodsInline.append("  {\n")
  Code.MethodsInline.append("    return property[getPropertyIndexUCS2(ucs2)];\n")
  Code.MethodsInline.append("  }\n\n")

  Code.MethodsInline.append("  FOG_INLINE const CharProperty& getPropertyUCS4(uint32_t ucs4) const\n")
  Code.MethodsInline.append("  {\n")
  Code.MethodsInline.append("    return property[getPropertyIndexUCS4(ucs4)];\n")
  Code.MethodsInline.append("  }\n\n")

  Code.MethodsInline.append("  " + AUTOGENERATED_END + "\n")

  # ---------------------------------------------------------------------------
  # Log
  # ---------------------------------------------------------------------------

  #log("".join(Code.OffsetRows))
  #log("".join(Code.PropertyRows))
  #log("".join(Code.SpecialRows))

  log("OFFSET-ROWS   : " + str(numOffsetRows) + \
      " (" + str(numOffsetRows * SIZE_OF_OFFSET_ROW) + " bytes)")
  log("CHAR-ROWS: " + str(numPropertyRows) + \
      " (" + str(len(rowBlockList)) + " blocks, " + str(numPropertyRows * SIZE_OF_PROPERTY_ROW) + " bytes)")
  log("INFO-ROWS  : " + str(numInfoRows) + \
      " (" + str(numInfoRows * SIZE_OF_INFO_ROW) + " bytes)")
  log("SPECIAL-ROWS  : " + str(numSpecialRows) + \
      " (" + str(numSpecialRows * SIZE_OF_SPECIAL_ROW) + " bytes)")
  log("DECOMPOSITION : " + str(len(decompositionList)) + \
      " (" + str(2 * len(decompositionList)) + " bytes, max length=" + str(maxDecompositionLength - 1) + ")")

  log("")
  s = "BLOCK LIST    : "
  for row in rowBlockList:
    s += str(rowBlockHash[row].count) + " "
  log(s)

def GenerateEnums():
  # Generate TEXT_SCRIPT enum.
  Code.TextScriptEnum.append("  " + AUTOGENERATED_BEGIN + "\n")
  Code.TextScriptEnum.append("\n" + TEXT_SCRIPT.generateEnum() + "\n")
  Code.TextScriptEnum.append("  " + AUTOGENERATED_END + "\n")

def WriteDataToFile(name, m):
  log("-- Processing file " + name)
  name = "../" + name

  file = open(name, 'rb')
  data = file.read()
  file.close()
  
  for key in m:
    log("   - Replacing " + key)
    beginMark = "${" + key + ":BEGIN}"
    endMark = "${" + key + ":END}"
    
    beginMarkIndex = data.find(beginMark)
    endMarkIndex = data.find(endMark)

    beginMarkIndex = data.find('\n', beginMarkIndex) + 1
    endMarkIndex = data.rfind('\n', 0, endMarkIndex) + 1

    assert beginMarkIndex != -1
    assert endMarkIndex != -1
    assert beginMarkIndex <= endMarkIndex

    data = data[:beginMarkIndex] + m[key] + data[endMarkIndex:]

  if TEST_ONLY:
    log("-- Output file " + name)
    log(data)
  else:
    log("-- Writing file " + name)
    file = open(name, 'wb')
    file.truncate()
    file.write(data)
    file.close()

def Write():
  WriteDataToFile("Fog/Core/Tools/CharData.h", {
    "CHAR_OFFSET_DECL"       : "".join(Code.OffsetDecl),
    "CHAR_PROPERTY_CLASS"    : "".join(Code.PropertyClass),
    "CHAR_PROPERTY_DECL"     : "".join(Code.PropertyDecl),
    "CHAR_INFO_CLASS"        : "".join(Code.InfoClass),
    "CHAR_INFO_DECL"         : "".join(Code.InfoDecl),
    "CHAR_SPECIAL_CLASS"     : "".join(Code.SpecialClass),
    "CHAR_SPECIAL_DECL"      : "".join(Code.SpecialDecl),
    "CHAR_DECOMPOSITION_DECL": "".join(Code.DecompositionDecl),
    "CHAR_METHODS_INLINE"    : "".join(Code.MethodsInline)
  })
  
  WriteDataToFile("Fog/Core/Tools/CharData.cpp", {
    "CHAR_OFFSET_DATA"       : "".join(Code.OffsetRows),
    "CHAR_PROPERTY_DATA"     : "".join(Code.PropertyRows),
    "CHAR_INFO_DATA"         : "".join(Code.InfoRows),
    "CHAR_SPECIAL_DATA"      : "".join(Code.SpecialRows),
    "CHAR_DECOMPOSITION_DATA": "".join(Code.DecompositionData)
  })
  
  WriteDataToFile("Fog/Core/Global/EnumCore.h", {
    "TEXT_SCRIPT_ENUM"   : "".join(Code.TextScriptEnum)
  })

# -----------------------------------------------------------------------------
# [UCD-Main]
# -----------------------------------------------------------------------------

def main():
  PrepareFiles()
  PrepareTables()

  ReadPropertyValueAliases()
  ReadUnicodeData()

  ReadLineBreak()
  ReadGraphemeBreak()
  ReadSentenceBreak()
  ReadWordBreak()

  ReadArabicShaping()
  ReadBidiMirroring()
  ReadCaseFolding()
  ReadSpecialCasing()
  ReadDerivedAge()
  ReadDerivedNormalizationProps()
  ReadNormalizationCorrections()
  ReadEastAsianWidth()
  ReadScripts()
  ReadBlocks()

  Check()
  PrintTitleCase()

  GenerateSpecial()
  GenerateClasses()
  GenerateTable()
  GenerateEnums()

  Write()

  log("-- All done, quitting...")

main()
