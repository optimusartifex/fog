// [Fog-Xml]
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

// Keep sorted order, this table is binary searched!
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
  { "amp",       '&'   }, // & == 38
  { "and",       8743  },
  { "ang",       8736  },
  { "apos",      '\''  }, // ' == 39
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
  { "gt",        '>'   }, // > == 62
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
  { "lt",        '<'   }, // < == 60
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
  { "quot",      '\"'  }, // " == 34
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

// Keep sorted order, this table is binary searched!
static const XmlEntity::PairRev xmlentity_pairs_rev[] =
{
  { 34, 198 },
  { 38, 67 },
  { 39, 70 },
  { 60, 153 },
  { 62, 121 },
  { 160, 161 },
  { 161, 129 },
  { 162, 83 },
  { 163, 193 },
  { 164, 91 },
  { 165, 252 },
  { 166, 77 },
  { 166, 77 },
  { 167, 216 },
  { 168, 98 },
  { 168, 98 },
  { 169, 88 },
  { 170, 179 },
  { 171, 142 },
  { 172, 165 },
  { 173, 217 },
  { 174, 207 },
  { 175, 126 },
  { 175, 126 },
  { 176, 95 },
  { 177, 192 },
  { 178, 227 },
  { 179, 228 },
  { 180, 62 },
  { 181, 156 },
  { 182, 185 },
  { 183, 157 },
  { 184, 82 },
  { 185, 226 },
  { 186, 180 },
  { 187, 202 },
  { 188, 116 },
  { 189, 115 },
  { 190, 117 },
  { 191, 135 },
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
  { 208, 13 },
  { 209, 30 },
  { 210, 35 },
  { 211, 33 },
  { 212, 34 },
  { 213, 39 },
  { 214, 40 },
  { 215, 238 },
  { 216, 38 },
  { 217, 53 },
  { 218, 51 },
  { 219, 52 },
  { 220, 55 },
  { 221, 57 },
  { 222, 48 },
  { 223, 230 },
  { 224, 64 },
  { 225, 60 },
  { 226, 61 },
  { 227, 73 },
  { 228, 74 },
  { 229, 71 },
  { 230, 63 },
  { 231, 81 },
  { 232, 102 },
  { 233, 100 },
  { 234, 101 },
  { 235, 110 },
  { 236, 130 },
  { 237, 127 },
  { 238, 128 },
  { 239, 137 },
  { 240, 109 },
  { 241, 168 },
  { 242, 173 },
  { 243, 170 },
  { 244, 171 },
  { 245, 182 },
  { 246, 184 },
  { 247, 99 },
  { 248, 181 },
  { 249, 244 },
  { 250, 241 },
  { 251, 243 },
  { 252, 248 },
  { 253, 251 },
  { 254, 236 },
  { 255, 253 },
  { 338, 32 },
  { 339, 172 },
  { 352, 46 },
  { 353, 214 },
  { 376, 58 },
  { 402, 113 },
  { 710, 85 },
  { 732, 237 },
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
  { 946, 76 },
  { 947, 119 },
  { 948, 96 },
  { 949, 106 },
  { 950, 254 },
  { 951, 108 },
  { 952, 233 },
  { 953, 134 },
  { 954, 138 },
  { 955, 140 },
  { 956, 159 },
  { 957, 169 },
  { 958, 250 },
  { 959, 176 },
  { 960, 190 },
  { 961, 209 },
  { 962, 219 },
  { 963, 218 },
  { 964, 231 },
  { 965, 247 },
  { 966, 189 },
  { 967, 84 },
  { 968, 197 },
  { 969, 175 },
  { 977, 234 },
  { 978, 246 },
  { 982, 191 },
  { 8194, 105 },
  { 8195, 104 },
  { 8201, 235 },
  { 8204, 256 },
  { 8205, 255 },
  { 8206, 150 },
  { 8207, 210 },
  { 8211, 162 },
  { 8212, 155 },
  { 8216, 152 },
  { 8217, 212 },
  { 8218, 213 },
  { 8220, 145 },
  { 8221, 205 },
  { 8222, 75 },
  { 8224, 93 },
  { 8225, 11 },
  { 8226, 79 },
  { 8230, 125 },
  { 8240, 187 },
  { 8242, 194 },
  { 8243, 43 },
  { 8249, 151 },
  { 8250, 211 },
  { 8254, 174 },
  { 8260, 118 },
  { 8364, 111 },
  { 8465, 131 },
  { 8472, 249 },
  { 8476, 206 },
  { 8482, 239 },
  { 8501, 65 },
  { 8592, 143 },
  { 8593, 242 },
  { 8594, 203 },
  { 8595, 94 },
  { 8596, 123 },
  { 8629, 89 },
  { 8656, 139 },
  { 8657, 240 },
  { 8658, 199 },
  { 8659, 92 },
  { 8660, 122 },
  { 8704, 114 },
  { 8706, 186 },
  { 8707, 112 },
  { 8709, 103 },
  { 8711, 160 },
  { 8712, 136 },
  { 8713, 166 },
  { 8715, 164 },
  { 8719, 195 },
  { 8721, 224 },
  { 8722, 158 },
  { 8727, 148 },
  { 8730, 200 },
  { 8733, 196 },
  { 8734, 132 },
  { 8736, 69 },
  { 8743, 68 },
  { 8744, 178 },
  { 8745, 80 },
  { 8746, 90 },
  { 8747, 133 },
  { 8756, 232 },
  { 8764, 220 },
  { 8773, 87 },
  { 8776, 72 },
  { 8800, 163 },
  { 8801, 107 },
  { 8804, 146 },
  { 8805, 120 },
  { 8834, 222 },
  { 8835, 225 },
  { 8836, 167 },
  { 8838, 223 },
  { 8839, 229 },
  { 8853, 177 },
  { 8855, 183 },
  { 8869, 188 },
  { 8901, 215 },
  { 8968, 144 },
  { 8969, 204 },
  { 8970, 147 },
  { 8971, 208 },
  { 9001, 141 },
  { 9002, 201 },
  { 9674, 149 },
  { 9824, 221 },
  { 9827, 86 },
  { 9829, 124 },
  { 9830, 97 }
};

/*
// If regeneration of secondary table is needed then uncomment this piece of
// code.
struct RegenerateXmlEntityTable
{
  RegenerateXmlEntityTable()
  {
    const uint32_t count = FOG_ARRAY_SIZE(xmlentity_pairs);

    XmlEntity::Pair* pairs = reinterpret_cast<XmlEntity::Pair*>(
      malloc(sizeof(XmlEntity::Pair) * count));

    memcpy(pairs, xmlentity_pairs, sizeof(XmlEntity::Pair) * count);
    qsort(pairs, count, sizeof(XmlEntity::Pair), cmp);

    printf("static const XmlEntity::PairRev xmlentity_pairs_rev[] =\n");
    printf("{\n");

    for (uint32_t i = 0; i < count; i++)
    {
      printf("  { %u, %u }", pairs[i].ch, findEntityPos(pairs[i].ch));
      if (i != count - 1) printf(",\n");
    }

    printf("\n};\n");
    free(pairs);
  }

  static int cmp(const void* _a, const void* _b)
  {
    const XmlEntity::Pair* a = reinterpret_cast<const XmlEntity::Pair*>(_a);
    const XmlEntity::Pair* b = reinterpret_cast<const XmlEntity::Pair*>(_b);

    return (int)a->ch - (int)b->ch;
  }

  static uint32_t findEntityPos(uint32_t ch)
  {
    const uint32_t count = FOG_ARRAY_SIZE(xmlentity_pairs);

    for (uint32_t i = 0; i < count; i++)
    {
      if (xmlentity_pairs[i].ch == ch) return i;
    }

    FOG_ASSERT_NOT_REACHED();
    return 0xFFFFFFFF;
  }
};

static RegenerateXmlEntityTable regenerateXmlEntityTable;
*/

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
    // End...
    if (i == length)
    {
      // How this works...? If entityInTable[i] is not zero terminator,
      // entityCompare returns @c true, but it means that entity is larger
      // and binary search shifts to right. Otherwise zero is returned and
      // this means MATCH.
      return entityInTable[i] != '\0';
    }
    // Or end of entity string in table...?
    else if (entityInTable[i] == '\0')
    {
      // Result is negative and it means that entity is in larger than entity
      // in table
      return -1;
    }
    // Character comparision.
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
      int result = _compareEntities<T>(entityName, entityLength, basep->name);

      // Match.
      if (result == 0)
      {
        return Char(basep->ch);
      }
      // Larger, move right.
      else if (result > 0)
      {
        base = basep + 1;
        index--;
      }
      // Else, move left.
    }

    // Not found.
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

      // Match.
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
      // Larger, move right.
      else if (ch > basep->ch)
      {
        base = basep + 1;
        index--;
      }
      // Else, move left.
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
