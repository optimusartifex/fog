// [Fog-Xml Library - Public API]
//
// [License]
// MIT, See COPYING file in package

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Xml/XmlEntity_p.h>

namespace Fog {

// ============================================================================
// [Fog::XmlEntity - Pairs]
// ============================================================================

// keep sorted order, this table is binary searched!
static const XmlEntity::Pair xmlentity_pairs[] =
{
  { "AElig",     198   },
  { "Aacute",    193   },
  { "Acirc",     194   },
  { "Agrave",    192   },
  { "Alpha",     913   },
  { "Aring",     197   },
  { "Atilde",    195   },
  { "Auml",      196   },
  { "Beta",      914   },
  { "Ccedil",    199   },
  { "Chi",       935   },
  { "Dagger",    8225  },
  { "Delta",     916   },
  { "Dstrok",    208   },
  { "ETH",       208   },
  { "Eacute",    201   },
  { "Ecirc",     202   },
  { "Egrave",    200   },
  { "Epsilon",   917   },
  { "Eta",       919   },
  { "Euml",      203   },
  { "Gamma",     915   },
  { "Iacute",    205   },
  { "Icirc",     206   },
  { "Igrave",    204   },
  { "Iota",      921   },
  { "Iuml",      207   },
  { "Kappa",     922   },
  { "Lambda",    923   },
  { "Mu",        924   },
  { "Ntilde",    209   },
  { "Nu",        925   },
  { "OElig",     338   },
  { "Oacute",    211   },
  { "Ocirc",     212   },
  { "Ograve",    210   },
  { "Omega",     937   },
  { "Omicron",   927   },
  { "Oslash",    216   },
  { "Otilde",    213   },
  { "Ouml",      214   },
  { "Phi",       934   },
  { "Pi",        928   },
  { "Prime",     8243  },
  { "Psi",       936   },
  { "Rho",       929   },
  { "Scaron",    352   },
  { "Sigma",     931   },
  { "THORN",     222   },
  { "Tau",       932   },
  { "Theta",     920   },
  { "Uacute",    218   },
  { "Ucirc",     219   },
  { "Ugrave",    217   },
  { "Upsilon",   933   },
  { "Uuml",      220   },
  { "Xi",        926   },
  { "Yacute",    221   },
  { "Yuml",      376   },
  { "Zeta",      918   },
  { "aacute",    225   },
  { "acirc",     226   },
  { "acute",     180   },
  { "aelig",     230   },
  { "agrave",    224   },
  { "alefsym",   8501  },
  { "alpha",     945   },
  { "amp",       '&'   },
  { "and",       8743  },
  { "ang",       8736  },
  // { "apos",      '\''  }, TODO: Add
  { "aring",     229   },
  { "asymp",     8776  },
  { "atilde",    227   },
  { "auml",      228   },
  { "bdquo",     8222  },
  { "beta",      946   },
  { "brkbar",    166   },
  { "brvbar",    166   },
  { "bull",      8226  },
  { "cap",       8745  },
  { "ccedil",    231   },
  { "cedil",     184   },
  { "cent",      162   },
  { "chi",       967   },
  { "circ",      710   },
  { "clubs",     9827  },
  { "cong",      8773  },
  { "copy",      169   },
  { "crarr",     8629  },
  { "cup",       8746  },
  { "curren",    164   },
  { "dArr",      8659  },
  { "dagger",    8224  },
  { "darr",      8595  },
  { "deg",       176   },
  { "delta",     948   },
  { "diams",     9830  },
  { "die",       168   },
  { "divide",    247   },
  { "eacute",    233   },
  { "ecirc",     234   },
  { "egrave",    232   },
  { "empty",     8709  },
  { "emsp",      8195  },
  { "ensp",      8194  },
  { "epsilon",   949   },
  { "equiv",     8801  },
  { "eta",       951   },
  { "eth",       240   },
  { "euml",      235   },
  { "euro",      8364  },
  { "exist",     8707  },
  { "fnof",      402   },
  { "forall",    8704  },
  { "frac12",    189   },
  { "frac14",    188   },
  { "frac34",    190   },
  { "frasl",     8260  },
  { "gamma",     947   },
  { "ge",        8805  },
  { "gt",        '>'   },
  { "hArr",      8660  },
  { "harr",      8596  },
  { "hearts",    9829  },
  { "hellip",    8230  },
  { "hibar",     175   },
  { "iacute",    237   },
  { "icirc",     238   },
  { "iexcl",     161   },
  { "igrave",    236   },
  { "image",     8465  },
  { "infin",     8734  },
  { "int",       8747  },
  { "iota",      953   },
  { "iquest",    191   },
  { "isin",      8712  },
  { "iuml",      239   },
  { "kappa",     954   },
  { "lArr",      8656  },
  { "lambda",    955   },
  { "lang",      9001  },
  { "laquo",     171   },
  { "larr",      8592  },
  { "lceil",     8968  },
  { "ldquo",     8220  },
  { "le",        8804  },
  { "lfloor",    8970  },
  { "lowast",    8727  },
  { "loz",       9674  },
  { "lrm",       8206  },
  { "lsaquo",    8249  },
  { "lsquo",     8216  },
  { "lt",        '<'   },
  { "macr",      175   },
  { "mdash",     8212  },
  { "micro",     181   },
  { "middot",    183   },
  { "minus",     8722  },
  { "mu",        956   },
  { "nabla",     8711  },
  { "nbsp",      160   },
  { "ndash",     8211  },
  { "ne",        8800  },
  { "ni",        8715  },
  { "not",       172   },
  { "notin",     8713  },
  { "nsub",      8836  },
  { "ntilde",    241   },
  { "nu",        957   },
  { "oacute",    243   },
  { "ocirc",     244   },
  { "oelig",     339   },
  { "ograve",    242   },
  { "oline",     8254  },
  { "omega",     969   },
  { "omicron",   959   },
  { "oplus",     8853  },
  { "or",        8744  },
  { "ordf",      170   },
  { "ordm",      186   },
  { "oslash",    248   },
  { "otilde",    245   },
  { "otimes",    8855  },
  { "ouml",      246   },
  { "para",      182   },
  { "part",      8706  },
  { "permil",    8240  },
  { "perp",      8869  },
  { "phi",       966   },
  { "pi",        960   },
  { "piv",       982   },
  { "plusmn",    177   },
  { "pound",     163   },
  { "prime",     8242  },
  { "prod",      8719  },
  { "prop",      8733  },
  { "psi",       968   },
  { "quot",      '\"'  },
  { "rArr",      8658  },
  { "radic",     8730  },
  { "rang",      9002  },
  { "raquo",     187   },
  { "rarr",      8594  },
  { "rceil",     8969  },
  { "rdquo",     8221  },
  { "real",      8476  },
  { "reg",       174   },
  { "rfloor",    8971  },
  { "rho",       961   },
  { "rlm",       8207  },
  { "rsaquo",    8250  },
  { "rsquo",     8217  },
  { "sbquo",     8218  },
  { "scaron",    353   },
  { "sdot",      8901  },
  { "sect",      167   },
  { "shy",       173   },
  { "sigma",     963   },
  { "sigmaf",    962   },
  { "sim",       8764  },
  { "spades",    9824  },
  { "sub",       8834  },
  { "sube",      8838  },
  { "sum",       8721  },
  { "sup",       8835  },
  { "sup1",      185   },
  { "sup2",      178   },
  { "sup3",      179   },
  { "supe",      8839  },
  { "szlig",     223   },
  { "tau",       964   },
  { "there4",    8756  },
  { "theta",     952   },
  { "thetasym",  977   },
  { "thinsp",    8201  },
  { "thorn",     254   },
  { "tilde",     732   },
  { "times",     215   },
  { "trade",     8482  },
  { "uArr",      8657  },
  { "uacute",    250   },
  { "uarr",      8593  },
  { "ucirc",     251   },
  { "ugrave",    249   },
  { "uml",       168   },
  { "upsih",     978   },
  { "upsilon",   965   },
  { "uuml",      252   },
  { "weierp",    8472  },
  { "xi",        958   },
  { "yacute",    253   },
  { "yen",       165   },
  { "yuml",      255   },
  { "zeta",      950   },
  { "zwj",       8205  },
  { "zwnj",      8204  }
};

// keep sorted order, this table is binary searched!
static const XmlEntity::PairRev xmlentity_pairs_rev[] =
{
  { 34, 197 },
  { 38, 67 },
  { 60, 152 },
  { 62, 120 },
  { 160, 160 },
  { 161, 128 },
  { 162, 82 },
  { 163, 192 },
  { 164, 90 },
  { 165, 251 },
  { 166, 76 },
  { 166, 77 },
  { 167, 215 },
  { 168, 97 },
  { 168, 244 },
  { 169, 87 },
  { 170, 178 },
  { 171, 141 },
  { 172, 164 },
  { 173, 216 },
  { 174, 206 },
  { 175, 125 },
  { 175, 153 },
  { 176, 94 },
  { 177, 191 },
  { 178, 226 },
  { 179, 227 },
  { 180, 62 },
  { 181, 155 },
  { 182, 184 },
  { 183, 156 },
  { 184, 81 },
  { 185, 225 },
  { 186, 179 },
  { 187, 201 },
  { 188, 115 },
  { 189, 114 },
  { 190, 116 },
  { 191, 134 },
  { 192, 3 },
  { 193, 1 },
  { 194, 2 },
  { 195, 6 },
  { 196, 7 },
  { 197, 5 },
  { 198, 0 },
  { 199, 9 },
  { 200, 17 },
  { 201, 15 },
  { 202, 16 },
  { 203, 20 },
  { 204, 24 },
  { 205, 22 },
  { 206, 23 },
  { 207, 26 },
  { 208, 13 },
  { 208, 14 },
  { 209, 30 },
  { 210, 35 },
  { 211, 33 },
  { 212, 34 },
  { 213, 39 },
  { 214, 40 },
  { 215, 237 },
  { 216, 38 },
  { 217, 53 },
  { 218, 51 },
  { 219, 52 },
  { 220, 55 },
  { 221, 57 },
  { 222, 48 },
  { 223, 229 },
  { 224, 64 },
  { 225, 60 },
  { 226, 61 },
  { 227, 72 },
  { 228, 73 },
  { 229, 70 },
  { 230, 63 },
  { 231, 80 },
  { 232, 101 },
  { 233, 99 },
  { 234, 100 },
  { 235, 109 },
  { 236, 129 },
  { 237, 126 },
  { 238, 127 },
  { 239, 136 },
  { 240, 108 },
  { 241, 167 },
  { 242, 172 },
  { 243, 169 },
  { 244, 170 },
  { 245, 181 },
  { 246, 183 },
  { 247, 98 },
  { 248, 180 },
  { 249, 243 },
  { 250, 240 },
  { 251, 242 },
  { 252, 247 },
  { 253, 250 },
  { 254, 235 },
  { 255, 252 },
  { 338, 32 },
  { 339, 171 },
  { 352, 46 },
  { 353, 213 },
  { 376, 58 },
  { 402, 112 },
  { 710, 84 },
  { 732, 236 },
  { 913, 4 },
  { 914, 8 },
  { 915, 21 },
  { 916, 12 },
  { 917, 18 },
  { 918, 59 },
  { 919, 19 },
  { 920, 50 },
  { 921, 25 },
  { 922, 27 },
  { 923, 28 },
  { 924, 29 },
  { 925, 31 },
  { 926, 56 },
  { 927, 37 },
  { 928, 42 },
  { 929, 45 },
  { 931, 47 },
  { 932, 49 },
  { 933, 54 },
  { 934, 41 },
  { 935, 10 },
  { 936, 44 },
  { 937, 36 },
  { 945, 66 },
  { 946, 75 },
  { 947, 118 },
  { 948, 95 },
  { 949, 105 },
  { 950, 253 },
  { 951, 107 },
  { 952, 232 },
  { 953, 133 },
  { 954, 137 },
  { 955, 139 },
  { 956, 158 },
  { 957, 168 },
  { 958, 249 },
  { 959, 175 },
  { 960, 189 },
  { 961, 208 },
  { 962, 218 },
  { 963, 217 },
  { 964, 230 },
  { 965, 246 },
  { 966, 188 },
  { 967, 83 },
  { 968, 196 },
  { 969, 174 },
  { 977, 233 },
  { 978, 245 },
  { 982, 190 },
  { 8194, 104 },
  { 8195, 103 },
  { 8201, 234 },
  { 8204, 255 },
  { 8205, 254 },
  { 8206, 149 },
  { 8207, 209 },
  { 8211, 161 },
  { 8212, 154 },
  { 8216, 151 },
  { 8217, 211 },
  { 8218, 212 },
  { 8220, 144 },
  { 8221, 204 },
  { 8222, 74 },
  { 8224, 92 },
  { 8225, 11 },
  { 8226, 78 },
  { 8230, 124 },
  { 8240, 186 },
  { 8242, 193 },
  { 8243, 43 },
  { 8249, 150 },
  { 8250, 210 },
  { 8254, 173 },
  { 8260, 117 },
  { 8364, 110 },
  { 8465, 130 },
  { 8472, 248 },
  { 8476, 205 },
  { 8482, 238 },
  { 8501, 65 },
  { 8592, 142 },
  { 8593, 241 },
  { 8594, 202 },
  { 8595, 93 },
  { 8596, 122 },
  { 8629, 88 },
  { 8656, 138 },
  { 8657, 239 },
  { 8658, 198 },
  { 8659, 91 },
  { 8660, 121 },
  { 8704, 113 },
  { 8706, 185 },
  { 8707, 111 },
  { 8709, 102 },
  { 8711, 159 },
  { 8712, 135 },
  { 8713, 165 },
  { 8715, 163 },
  { 8719, 194 },
  { 8721, 223 },
  { 8722, 157 },
  { 8727, 147 },
  { 8730, 199 },
  { 8733, 195 },
  { 8734, 131 },
  { 8736, 69 },
  { 8743, 68 },
  { 8744, 177 },
  { 8745, 79 },
  { 8746, 89 },
  { 8747, 132 },
  { 8756, 231 },
  { 8764, 219 },
  { 8773, 86 },
  { 8776, 71 },
  { 8800, 162 },
  { 8801, 106 },
  { 8804, 145 },
  { 8805, 119 },
  { 8834, 221 },
  { 8835, 224 },
  { 8836, 166 },
  { 8838, 222 },
  { 8839, 228 },
  { 8853, 176 },
  { 8855, 182 },
  { 8869, 187 },
  { 8901, 214 },
  { 8968, 143 },
  { 8969, 203 },
  { 8970, 146 },
  { 8971, 207 },
  { 9001, 140 },
  { 9002, 200 },
  { 9674, 148 },
  { 9824, 220 },
  { 9827, 85 },
  { 9829, 123 },
  { 9830, 96 }
};

// ============================================================================
// [Fog::XmlEntity]
// ============================================================================

const XmlEntity::Pair* XmlEntity::_pairs;
sysuint_t XmlEntity::_pairsCount;

template<typename T>
static FOG_INLINE int _compareEntities(const T* entity, sysuint_t length, const char* entityInTable)
{
  sysuint_t i = 0;
  while (true)
  {
    // end...
    if (i == length)
    {
      // How this works...? If entityInTable[i] is not zero terminator,
      // entityCompare returns @c true, but it means that entity is larger
      // and binary search shifts to right. Otherwise zero is returned and
      // this means MATCH.
      return entityInTable[i] != '\0';
    }
    // or end of entity string in table...?
    else if (entityInTable[i] == '\0')
    {
      // Result is negative and it means that entity is in larger than entity
      // in table
      return -1;
    }
    // character comparision
    else if (entity[i] != (uint8_t)entityInTable[i])
    {
      return (int)entity[i] - (int)(uint8_t)entityInTable[i];
    }
    i++;
  }
}

template<typename T>
static FOG_INLINE Char _decode(const T* entityName, sysuint_t entityLength)
{
  if (entityLength == DETECT_LENGTH) entityLength = StringUtil::len(entityName);
  if (entityLength == 0) return Char(0);

  // Numeric entity.
  if (entityName[0] == T('#'))
  {
    uint32_t base = 10;

    entityName++;
    entityLength--;

    if (entityLength > 0 && (entityName[0] == T('x') || entityName[0] == T('X')))
    {
      entityName++;
      entityLength--;

      base = 16;
    }

    uint16_t val;
    uint32_t flags;

    err_t err = StringUtil::atou16(entityName, entityLength, &val, base, NULL, &flags);

    // If conversion failed or parsed skipped some other characters, entity
    // is invalid.
    if (err || flags) val = 0;

    return Char(val);
  }

  // Named entity - binary search in entity table.
  else
  {
    const XmlEntity::Pair* base = xmlentity_pairs;
    const XmlEntity::Pair* basep;

    sysuint_t index;
    sysuint_t count = FOG_ARRAY_SIZE(xmlentity_pairs);

    for (index = count; index != 0; index >>= 1)
    {
      basep = base + (index >> 1);

      // Compare.
      int result = _compareEntities<T>(entityName, entityLength, basep->name);
      // Match.
      if (result == 0)
      {
        return Char(basep->ch);
      }
      // Larger... move right.
      else if (result > 0)
      {
        base = basep + 1;
        index--;
      }
      // else => move left
    }

    // Not found
    return Char(0);
  }
}

Char XmlEntity::decode(const char* entityName, sysuint_t entityLength)
{
  return _decode<char>(entityName, entityLength);
}

Char XmlEntity::decode(const Char* entityName, sysuint_t entityLength)
{
  return _decode<Char>(entityName, entityLength);
}

sysuint_t XmlEntity::encode(char* dst, Char _ch)
{
  // We first try to find named entity, it it fails, we will
  // generate hexadecimal character entity.
  if (_ch.ch() < 0xFFFF)
  {
    const XmlEntity::PairRev* base = xmlentity_pairs_rev;
    const XmlEntity::PairRev* basep;

    sysuint_t index;
    sysuint_t count = FOG_ARRAY_SIZE(xmlentity_pairs_rev);

    uint16_t ch = (uint16_t)_ch.ch();

    for (index = count; index != 0; index >>= 1)
    {
      basep = base + (index >> 1);

      // Match
      if (ch == basep->ch)
      {
        char* dstBegin = dst;
        const char* entityName = xmlentity_pairs[basep->link].name;

        *dst++ = '&';
        while (*entityName) { *dst++ = *entityName++; }
        *dst++ = ';';
        *dst = '\0';

        return (sysuint_t)(dst - dstBegin);
      }
      // Larger... move right
      else if (ch > basep->ch)
      {
        base = basep + 1;
        index--;
      }
      // else => move left
    }
  }

  if (_ch.ch() > 255)
    return (sysuint_t)sprintf(dst, "&#x%04X;", _ch.ch());
  else
    return (sysuint_t)sprintf(dst, "&#x%02X;", _ch.ch());
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_xmlentity_init(void)
{
  using namespace Fog;

  XmlEntity::_pairs = xmlentity_pairs;
  XmlEntity::_pairsCount = FOG_ARRAY_SIZE(xmlentity_pairs);

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_xmlentity_shutdown(void)
{
  using namespace Fog;
}
