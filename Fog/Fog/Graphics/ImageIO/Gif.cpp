// [Fog-Graphics Library - Public API]
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
#include <Fog/Core/ManagedString.h>
#include <Fog/Core/Math.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Misc.h>
#include <Fog/Core/Stream.h>
#include <Fog/Core/String.h>
#include <Fog/Core/Strings.h>
#include <Fog/Graphics/Constants.h>
#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/ImageIO.h>
#include <Fog/Graphics/RasterEngine_p.h>

#include <string.h>

// [giflib]

/******************************************************************************
 *   "Gif-Lib" - Yet another gif library.
 *
 * Written by:  Gershon Elber            IBM PC Ver 1.1,    Aug. 1990
 ******************************************************************************
 * The kernel of the GIF Decoding process can be found here.
 ******************************************************************************
 * History:
 * 16 Jun 89 - Version 1.0 by Gershon Elber.
 *  3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89, Unique names).
 *****************************************************************************/

// The GIFLIB distribution is Copyright (c) 1997  Eric S. Raymond
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// [giflib hash table]

int _GifError;

#define HT_SIZE 8192       // 12bits = 4096 or twice as big!
#define HT_KEY_MASK 0x1FFF // 13bits keys.
#define HT_KEY_NUM_BITS 13 // 13bits keys.
#define HT_MAX_KEY 8191    // 13bits - 1, maximal code possible.
#define HT_MAX_CODE 4095   // Biggest code possible in 12 bits.

// The 32 bits of the long are divided into two parts for the key & code:
// 1. The code is 12 bits as our compression algorithm is limited to 12bits
// 2. The key is 12 bits Prefix code + 8 bit new char or 20 bits.
// The key is the upper 20 bits.  The code is the lower 12.
#define HT_GET_KEY(l) (l >> 12)
#define HT_GET_CODE(l) (l & 0x0FFF)
#define HT_PUT_KEY(l) (l << 12)
#define HT_PUT_CODE(l) (l & 0x0FFF)

struct GifHashTableType
{
  uint32_t HTable[HT_SIZE];
};

static GifHashTableType *_InitHashTable(void);
static void _ClearHashTable(GifHashTableType *HashTable);
static void _InsertHashTable(GifHashTableType *HashTable, uint32_t Key, int Code);
static int _ExistsHashTable(GifHashTableType *HashTable, uint32_t Key);
static int KeyItem(uint32_t Item);

// Initialize HashTable - allocate the memory needed and clear it.
static GifHashTableType *_InitHashTable(void)
{
  GifHashTableType *HashTable;

  if ((HashTable = (GifHashTableType *) Fog::Memory::alloc(sizeof(GifHashTableType))) == NULL)
  {
    return NULL;
  }

  _ClearHashTable(HashTable);
  return HashTable;
}

// Routine to clear the HashTable to an empty state.
// This part is a little machine depended. Use the commented part otherwise.
static void _ClearHashTable(GifHashTableType *HashTable)
{
  memset(HashTable -> HTable, 0xFF, HT_SIZE * sizeof(uint32_t));
}

// Routine to insert a new Item into the HashTable. The data is assumed to be
// new one.
static void _InsertHashTable(GifHashTableType *HashTable, uint32_t Key, int Code)
{
  int HKey = KeyItem(Key);
  uint32_t *HTable = HashTable -> HTable;

  while (HT_GET_KEY(HTable[HKey]) != 0xFFFFFL)
  {
    HKey = (HKey + 1) & HT_KEY_MASK;
  }
  HTable[HKey] = HT_PUT_KEY(Key) | HT_PUT_CODE(Code);
}

// Routine to test if given Key exists in HashTable and if so returns its code
// Returns the Code if key was found, -1 if not.
static int _ExistsHashTable(GifHashTableType *HashTable, uint32_t Key)
{
  int HKey = KeyItem(Key);
  uint32_t *HTable = HashTable -> HTable, HTKey;

  while ((HTKey = HT_GET_KEY(HTable[HKey])) != 0xFFFFFL)
  {
    if (Key == HTKey) return HT_GET_CODE(HTable[HKey]);
    HKey = (HKey + 1) & HT_KEY_MASK;
  }

  return -1;
}

// Routine to generate an HKey for the hashtable out of the given unique key.
// The given Key is assumed to be 20 bits as follows: lower 8 bits are the
// new postfix character, while the upper 12 bits are the prefix code.
// Because the average hit ratio is only 2 (2 hash references per entry),
// evaluating more complex keys (such as twin prime keys) does not worth it!
static int KeyItem(uint32_t item)
{
  return ((item >> 12) ^ item) & HT_KEY_MASK;
}

// [giflib - header]

#define GIF_ERROR 0
#define GIF_OK 1

#define GIF_STAMP "GIFVER"                   /* First chars in file - GIF stamp.  */
#define GIF_STAMP_LEN (sizeof(GIF_STAMP) - 1)
#define GIF_VERSION_POS 3                    /* Version first character in stamp. */
#define GIF87_STAMP "GIF87a"                 /* First chars in file - GIF stamp.  */
#define GIF89_STAMP "GIF89a"                 /* First chars in file - GIF stamp.  */

#define GIF_FILE_BUFFER_SIZE 16384           /* Files uses bigger buffers than usual. */

// GIF89 extension function codes
#define COMMENT_EXT_FUNC_CODE     0xfe       /* comment */
#define GRAPHICS_EXT_FUNC_CODE    0xf9       /* graphics control */
#define PLAINTEXT_EXT_FUNC_CODE   0x01       /* plaintext */
#define APPLICATION_EXT_FUNC_CODE 0xff       /* application block */

#define LZ_MAX_CODE         4095             /* Biggest code possible in 12 bits. */
#define LZ_BITS             12

#define FLUSH_OUTPUT        4096             /* Impossible code, to signal flush. */
#define FIRST_CODE          4097             /* Impossible code, to signal first. */
#define NO_SUCH_CODE        4098             /* Impossible code, to signal empty. */

#define FILE_STATE_WRITE    0x01
#define FILE_STATE_SCREEN   0x02
#define FILE_STATE_IMAGE    0x04
#define FILE_STATE_READ     0x08

#define IS_READABLE(GifFile) (GifFile->FileState & FILE_STATE_READ)
#define IS_WRITEABLE(GifFile) (GifFile->FileState & FILE_STATE_WRITE)

typedef int GifBooleanType;
typedef uint8_t GifPixelType;
typedef uint8_t *GifRowType;
typedef uint16_t GifPrefixType;
typedef int GifWord;

#include <Fog/Core/Compiler/PackByte.h>
struct GifColorType
{
  uint8_t Red;
  uint8_t Green;
  uint8_t Blue;
};
#include <Fog/Core/Compiler/PackRestore.h>

struct ColorMapObject
{
  int ColorCount;
  int BitsPerPixel;
  GifColorType *Colors;        /* allocated on the heap */
};

struct GifImageDesc
{
  GifWord Left;                /* Current image dimensions. */
  GifWord Top;
  GifWord Width;
  GifWord Height;
  GifWord Interlace;           /* Sequential/Interlaced lines. */
  ColorMapObject *ColorMap;    /* The local color map */
};

/* This is the in-core version of an extension record */
struct ExtensionBlock
{
  int ByteCount;
  char *Bytes;                 /* on heap */
  int Function;                /* Holds the type of the Extension block. */
};

/* This holds an image header, its unpacked raster bits, and extensions */
struct SavedImage
{
  GifImageDesc ImageDesc;
  unsigned char *RasterBits;   /* on heap */
  int Function;                /* DEPRECATED: Use ExtensionBlocks[x].Function instead */
  int ExtensionBlockCount;
  ExtensionBlock *ExtensionBlocks; /* on heap */
};

enum GifRecordType
{
  UNDEFINED_RECORD_TYPE,
  SCREEN_DESC_RECORD_TYPE,
  IMAGE_DESC_RECORD_TYPE,      /* Begin with ',' */
  EXTENSION_RECORD_TYPE,       /* Begin with '!' */
  TERMINATE_RECORD_TYPE        /* Begin with ';' */
};

struct GifFileType
{
  GifWord SWidth;
  GifWord SHeight;             /* Screen dimensions. */
  GifWord SColorResolution;    /* How many colors can we generate? */
  GifWord SBackGroundColor;    /* I hope you understand this one... */
  ColorMapObject *SColorMap;   /* NULL if not exists. */
  int ImageCount;              /* Number of current image */
  GifImageDesc Image;          /* Block describing current image */
  SavedImage *SavedImages;     /* Use this to accumulate file state */

  // Private members
  Fog::Stream* stream;
  GifWord FileState;           /* Where all this data goes to! */
  GifWord BitsPerPixel;        /* Bits per pixel (Codes uses at least this + 1). */
  GifWord ClearCode;           /* The CLEAR LZ code. */
  GifWord EOFCode;             /* The EOF LZ code. */
  GifWord RunningCode;         /* The next code algorithm can generate. */
  GifWord RunningBits;         /* The number of bits required to represent RunningCode. */
  GifWord MaxCode1;            /* 1 bigger than max. possible code, in RunningBits bits. */
  GifWord LastCode;            /* The code before the current code. */
  GifWord CrntCode;            /* Current algorithm code. */
  GifWord StackPtr;            /* For character stack (see below). */
  GifWord CrntShiftState;      /* Number of bits in CrntShiftDWord. */
  unsigned long CrntShiftDWord;/* For bytes decomposition into codes. */
  unsigned long PixelCount;    /* Number of pixels in image. */
  uint8_t Buf[256];            /* Compressed input is buffered here. */
  uint8_t Stack[LZ_MAX_CODE];  /* Decoded pixels are stacked here. */
  uint8_t Suffix[LZ_MAX_CODE + 1]; /* So we can trace the codes. */
  GifPrefixType Prefix[LZ_MAX_CODE + 1];
  GifHashTableType *HashTable;
};

// [giflib function prototypes]

static GifFileType *DGifOpen(Fog::Stream* stream);

static int DGifGetScreenDesc(GifFileType * GifFile);
static int DGifGetRecordType(GifFileType * GifFile, GifRecordType * GifType);
static int DGifGetImageDesc(GifFileType * GifFile);
static int DGifGetLine(GifFileType * GifFile, GifPixelType * GifLine, int GifLineLen);
static int DGifGetPixel(GifFileType * GifFile, GifPixelType GifPixel);
static int DGifGetExtension(GifFileType * GifFile, int *GifExtCode, uint8_t ** GifExtension);
static int DGifGetExtensionNext(GifFileType * GifFile, uint8_t ** GifExtension);
static int DGifGetCode(GifFileType * GifFile, int *GifCodeSize, uint8_t ** GifCodeBlock);
static int DGifGetCodeNext(GifFileType * GifFile, uint8_t ** GifCodeBlock);
static int DGifGetLZCodes(GifFileType * GifFile, int *GifCode);
static int DGifCloseFile(GifFileType * GifFile);

#define D_GIF_ERR_OPEN_FAILED    101    /* And DGif possible errors. */
#define D_GIF_ERR_READ_FAILED    102
#define D_GIF_ERR_NOT_GIF_FILE   103
#define D_GIF_ERR_NO_SCRN_DSCR   104
#define D_GIF_ERR_NO_IMAG_DSCR   105
#define D_GIF_ERR_NO_COLOR_MAP   106
#define D_GIF_ERR_WRONG_RECORD   107
#define D_GIF_ERR_DATA_TOO_BIG   108
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_CLOSE_FAILED   110
#define D_GIF_ERR_NOT_READABLE   111
#define D_GIF_ERR_IMAGE_DEFECT   112
#define D_GIF_ERR_EOF_TOO_SOON   113

static GifFileType *EGifOpen(Fog::Stream* stream);

static int EGifSpew(GifFileType * GifFile);
static void EGifSetGifVersion(const char *Version);
static int EGifPutScreenDesc(GifFileType * GifFile, int GifWidth, int GifHeight, int GifColorRes, int GifBackGround, const ColorMapObject * GifColorMap);
static int EGifPutImageDesc(GifFileType * GifFile, int GifLeft, int GifTop, int Width, int GifHeight, int GifInterlace, const ColorMapObject * GifColorMap);
static int EGifPutLine(GifFileType * GifFile, GifPixelType * GifLine, int GifLineLen);
static int EGifPutPixel(GifFileType * GifFile, GifPixelType GifPixel);
static int EGifPutComment(GifFileType * GifFile, const char *GifComment);
static int EGifPutExtensionFirst(GifFileType * GifFile, int GifExtCode, int GifExtLen, const void* GifExtension);
static int EGifPutExtensionNext(GifFileType * GifFile, int GifExtCode, int GifExtLen, const void* GifExtension);
static int EGifPutExtensionLast(GifFileType * GifFile, int GifExtCode, int GifExtLen, const void* GifExtension);
static int EGifPutExtension(GifFileType * GifFile, int GifExtCode, int GifExtLen, const void* GifExtension);
static int EGifPutCode(GifFileType * GifFile, int GifCodeSize, const uint8_t * GifCodeBlock);
static int EGifPutCodeNext(GifFileType * GifFile, const uint8_t * GifCodeBlock);
static int EGifCloseFile(GifFileType * GifFile);

#define E_GIF_ERR_OPEN_FAILED    1    /* And EGif possible errors. */
#define E_GIF_ERR_WRITE_FAILED   2

#define E_GIF_ERR_HAS_SCRN_DSCR  3
#define E_GIF_ERR_HAS_IMAG_DSCR  4
#define E_GIF_ERR_NO_COLOR_MAP   5
#define E_GIF_ERR_DATA_TOO_BIG   6
#define E_GIF_ERR_NOT_ENOUGH_MEM 7
#define E_GIF_ERR_DISK_IS_FULL   8
#define E_GIF_ERR_CLOSE_FAILED   9
#define E_GIF_ERR_NOT_WRITEABLE  10

static int QuantizeBuffer(unsigned int Width, unsigned int Height,
  int *ColorMapSize, uint8_t * RedInput,
  uint8_t * GreenInput, uint8_t * BlueInput,
  uint8_t * OutputBuffer,
  GifColorType * OutputColorMap);

// Support for the in-core structures allocation (slurp mode).
static void ApplyTranslation(SavedImage * Image, GifPixelType Translation[]);
static void MakeExtension(SavedImage * New, int Function);
static int AddExtensionBlock(SavedImage * New, int Len, unsigned char ExtData[]);
static void FreeExtension(SavedImage * Image);
static SavedImage *MakeSavedImage(GifFileType * GifFile, const SavedImage * CopyFrom);
static void FreeSavedImages(GifFileType * GifFile);

// Return smallest bitfield size n will fit in.
static int BitSize(int n)
{
  int i;
  for (i = 1; i <= 8; i++) if ((1 << i) >= n) break;
  return i;
}

// [giflib - ColorMap object functions]

// Allocate a color map of given size; initialize with contents of
// ColorMap if that pointer is non-NULL.
static ColorMapObject *MakeMapObject(int ColorCount, const GifColorType * ColorMap)
{
  ColorMapObject *Object;

  /*** FIXME: Our ColorCount has to be a power of two.  Is it necessary to
   * make the user know that or should we automatically round up instead? */
  if (ColorCount != (1 << BitSize(ColorCount))) return NULL;

  Object = (ColorMapObject *)Fog::Memory::alloc(sizeof(ColorMapObject));
  if (Object == (ColorMapObject *) NULL) return NULL;

  Object->Colors = (GifColorType *)Fog::Memory::calloc(ColorCount * sizeof(GifColorType));
  if (Object->Colors == (GifColorType *) NULL) return NULL;

  Object->ColorCount = ColorCount;
  Object->BitsPerPixel = BitSize(ColorCount);

  if (ColorMap) {
    memcpy((char *)Object->Colors, (char *)ColorMap, ColorCount * sizeof(GifColorType));
  }

  return (Object);
}

// Free a color map object
static void FreeMapObject(ColorMapObject * Object)
{
  if (Object != NULL)
  {
    Fog::Memory::free(Object->Colors);
    Fog::Memory::free(Object);
    /*** FIXME:
     * When we are willing to break API we need to make this function
     * FreeMapObject(ColorMapObject **Object)
     * and do this assignment to NULL here:
     * *Object = NULL;
     */
  }
}

// Compute the union of two given color maps and return it.  If result can't 
// fit into 256 colors, NULL is returned, the allocated union otherwise.
// ColorIn1 is copied as is to ColorUnion, while colors from ColorIn2 are
// copied iff they didn't exist before.  ColorTransIn2 maps the old
// ColorIn2 into ColorUnion color map table.
static ColorMapObject *UnionColorMap(
  const ColorMapObject * ColorIn1,
  const ColorMapObject * ColorIn2,
  GifPixelType ColorTransIn2[])
{
  int i, j, CrntSlot, RoundUpTo, NewBitSize;
  ColorMapObject *ColorUnion;

  /* 
   * Allocate table which will hold the result for sure.
   */
  ColorUnion = MakeMapObject(Fog::Math::max(ColorIn1->ColorCount,
                 ColorIn2->ColorCount) * 2, NULL);

  if (ColorUnion == NULL)
    return (NULL);

  /* Copy ColorIn1 to ColorUnionSize; */
  /*** FIXME: What if there are duplicate entries into the colormap to begin
   * with? */
  for (i = 0; i < ColorIn1->ColorCount; i++)
    ColorUnion->Colors[i] = ColorIn1->Colors[i];
  CrntSlot = ColorIn1->ColorCount;

  /* 
   * Potentially obnoxious hack:
   *
   * Back CrntSlot down past all contiguous {0, 0, 0} slots at the end
   * of table 1.  This is very useful if your display is limited to
   * 16 colors.
   */
  while (ColorIn1->Colors[CrntSlot - 1].Red == 0
      && ColorIn1->Colors[CrntSlot - 1].Green == 0
      && ColorIn1->Colors[CrntSlot - 1].Blue == 0) CrntSlot--;

  /* Copy ColorIn2 to ColorUnionSize (use old colors if they exist): */
  for (i = 0; i < ColorIn2->ColorCount && CrntSlot <= 256; i++) {
    /* Let's see if this color already exists: */
    /*** FIXME: Will it ever occur that ColorIn2 will contain duplicate
     * entries?  So we should search from 0 to CrntSlot rather than
     * ColorIn1->ColorCount?
     */
    for (j = 0; j < ColorIn1->ColorCount; j++)
      if (memcmp (&ColorIn1->Colors[j], &ColorIn2->Colors[i], 
            sizeof(GifColorType)) == 0)
        break;

    if (j < ColorIn1->ColorCount)
      ColorTransIn2[i] = j;    /* color exists in Color1 */
    else {
      /* Color is new - copy it to a new slot: */
      ColorUnion->Colors[CrntSlot] = ColorIn2->Colors[i];
      ColorTransIn2[i] = CrntSlot++;
    }
  }

  if (CrntSlot > 256) {
    FreeMapObject(ColorUnion);
    return ((ColorMapObject *) NULL);
  }

  NewBitSize = BitSize(CrntSlot);
  RoundUpTo = (1 << NewBitSize);

  if (RoundUpTo != ColorUnion->ColorCount)
  {
    GifColorType *Map = ColorUnion->Colors;

    /* 
     * Zero out slots up to next power of 2.
     * We know these slots exist because of the way ColorUnion's
     * start dimension was computed.
     */
    for (j = CrntSlot; j < RoundUpTo; j++)
      Map[j].Red = Map[j].Green = Map[j].Blue = 0;

    /* perhaps we can shrink the map? */
    if (RoundUpTo < ColorUnion->ColorCount)
      ColorUnion->Colors = (GifColorType *)Fog::Memory::realloc(Map,
                 sizeof(GifColorType) * RoundUpTo);
  }

  ColorUnion->ColorCount = RoundUpTo;
  ColorUnion->BitsPerPixel = NewBitSize;

  return (ColorUnion);
}

// Apply a given color translation to the raster bits of an image
static void ApplyTranslation(SavedImage * Image, GifPixelType Translation[])
{
  int i;
  int RasterSize = Image->ImageDesc.Height * Image->ImageDesc.Width;

  for (i = 0; i < RasterSize; i++) Image->RasterBits[i] = Translation[Image->RasterBits[i]];
}

// [giflib extension record functions]

static void MakeExtension(SavedImage * New, int Function)
{
  New->Function = Function;
  /*** FIXME:
   * Someday we might have to deal with multiple extensions.
   * ??? Was this a note from Gershon or from me?  Does the multiple
   * extension blocks solve this or do we need multiple Functions?  Or is
   * this an obsolete function?  (People should use AddExtensionBlock
   * instead?)
   * Looks like AddExtensionBlock needs to take the int Function argument
   * then it can take the place of this function.  Right now people have to
   * use both.  Fix AddExtensionBlock and add this to the deprecation list.
   */
}

static int AddExtensionBlock(SavedImage * New, int Len, unsigned char ExtData[])
{
  ExtensionBlock *ep;

  if (New->ExtensionBlocks == NULL)
    New->ExtensionBlocks=(ExtensionBlock *)Fog::Memory::alloc(sizeof(ExtensionBlock));
  else
    New->ExtensionBlocks = (ExtensionBlock *)Fog::Memory::realloc(New->ExtensionBlocks,
      sizeof(ExtensionBlock) *
      (New->ExtensionBlockCount + 1));

  if (New->ExtensionBlocks == NULL)
    return (GIF_ERROR);

  ep = &New->ExtensionBlocks[New->ExtensionBlockCount++];

  ep->ByteCount=Len;
  ep->Bytes = (char *)Fog::Memory::alloc(ep->ByteCount);
  if (ep->Bytes == NULL)
    return (GIF_ERROR);

  if (ExtData) {
    memcpy(ep->Bytes, ExtData, Len);
    ep->Function = New->Function;
  }

  return (GIF_OK);
}

static void FreeExtension(SavedImage *Image)
{
  ExtensionBlock *ep;

  if ((Image == NULL) || (Image->ExtensionBlocks == NULL)) return;

  for (ep = Image->ExtensionBlocks; ep < (Image->ExtensionBlocks + Image->ExtensionBlockCount); ep++)
  {
    Fog::Memory::free((char *)ep->Bytes);
  }

  Fog::Memory::free((char *)Image->ExtensionBlocks);
  Image->ExtensionBlocks = NULL;
}

// [giflib - image block allocation functions]

// Private Function:
// Frees the last image in the GifFile->SavedImages array
static void FreeLastSavedImage(GifFileType *GifFile)
{
  SavedImage *sp;

  if ((GifFile == NULL) || (GifFile->SavedImages == NULL)) return;

  /* Remove one SavedImage from the GifFile */
  GifFile->ImageCount--;
  sp = &GifFile->SavedImages[GifFile->ImageCount];

  /* Deallocate its Colormap */
  if (sp->ImageDesc.ColorMap)
  {
      FreeMapObject(sp->ImageDesc.ColorMap);
      sp->ImageDesc.ColorMap = NULL;
  }

  /* Deallocate the image data */
  if (sp->RasterBits) Fog::Memory::free((char *)sp->RasterBits);

  /* Deallocate any extensions */
  if (sp->ExtensionBlocks) FreeExtension(sp);

  /*** FIXME: We could Fog::Memory::realloc the GifFile->SavedImages structure but is
   * there a point to it? Saves some memory but we'd have to do it every
   * time.  If this is used in FreeSavedImages then it would be inefficient
   * (The whole array is going to be deallocated.)  If we just use it when
   * we want to Fog::Memory::free the last Image it's convenient to do it here.
   */
}

// Append an image block to the SavedImages array  
static SavedImage *MakeSavedImage(GifFileType * GifFile, const SavedImage * CopyFrom)
{
  SavedImage *sp;

  if (GifFile->SavedImages == NULL)
    GifFile->SavedImages = (SavedImage *)Fog::Memory::alloc(sizeof(SavedImage));
  else
    GifFile->SavedImages = (SavedImage *)Fog::Memory::realloc(GifFile->SavedImages,
                           sizeof(SavedImage) * (GifFile->ImageCount + 1));

  if (GifFile->SavedImages == NULL) return ((SavedImage *)NULL);

  sp = &GifFile->SavedImages[GifFile->ImageCount++];
  memset((char *)sp, '\0', sizeof(SavedImage));

  if (CopyFrom)
  {
    memcpy((char *)sp, CopyFrom, sizeof(SavedImage));

    /*
     * Make our own allocated copies of the heap fields in the
     * copied record.  This guards against potential aliasing
     * problems.
     */

    /* first, the local color map */
    if (sp->ImageDesc.ColorMap) {
      sp->ImageDesc.ColorMap = MakeMapObject(
                               CopyFrom->ImageDesc.ColorMap->ColorCount,
                               CopyFrom->ImageDesc.ColorMap->Colors);
      if (sp->ImageDesc.ColorMap == NULL) {
        FreeLastSavedImage(GifFile);
        return (SavedImage *)(NULL);
      }
    }

    /* next, the raster */
    sp->RasterBits = (unsigned char *)Fog::Memory::alloc(sizeof(GifPixelType) *
                                           CopyFrom->ImageDesc.Height *
                                           CopyFrom->ImageDesc.Width);
    if (sp->RasterBits == NULL) {
      FreeLastSavedImage(GifFile);
      return (SavedImage *)(NULL);
    }
    memcpy(sp->RasterBits, CopyFrom->RasterBits,
      sizeof(GifPixelType) * CopyFrom->ImageDesc.Height *
      CopyFrom->ImageDesc.Width);

    /* finally, the extension blocks */
    if (sp->ExtensionBlocks) {
      sp->ExtensionBlocks = (ExtensionBlock *)Fog::Memory::alloc(
                            sizeof(ExtensionBlock) *
                            CopyFrom->ExtensionBlockCount);
      if (sp->ExtensionBlocks == NULL) {
        FreeLastSavedImage(GifFile);
        return (SavedImage *)(NULL);
      }
      memcpy(sp->ExtensionBlocks, CopyFrom->ExtensionBlocks,
             sizeof(ExtensionBlock) * CopyFrom->ExtensionBlockCount);

      /*
       * For the moment, the actual blocks can take their
       * chances with Fog::Memory::free().  We'll fix this later.
       *** FIXME: [Better check this out... Toshio]
       * 2004 May 27: Looks like this was an ESR note.
       * It means the blocks are shallow copied from InFile to
       * OutFile.  However, I don't see that in this code....
       * Did ESR fix it but never remove this note (And other notes
       * in gifspnge?)
       */
    }
  }

  return sp;
}

static void FreeSavedImages(GifFileType * GifFile)
{
  SavedImage *sp;

  if ((GifFile == NULL) || (GifFile->SavedImages == NULL)) return;

  for (sp = GifFile->SavedImages; sp < GifFile->SavedImages + GifFile->ImageCount; sp++)
  {
    if (sp->ImageDesc.ColorMap) {
      FreeMapObject(sp->ImageDesc.ColorMap);
      sp->ImageDesc.ColorMap = NULL;
    }

    if (sp->RasterBits) Fog::Memory::free((char *)sp->RasterBits);
    if (sp->ExtensionBlocks) FreeExtension(sp);
  }
  Fog::Memory::free((char *)GifFile->SavedImages);
  GifFile->SavedImages=NULL;
}

// [giflib - dgif]

#define READ(_gif, _buf, _len) (_gif->stream->read(_buf, _len))

static int DGifGetWord(GifFileType *GifFile, GifWord *Word);
static int DGifSetupDecompress(GifFileType *GifFile);
static int DGifDecompressLine(GifFileType *GifFile, GifPixelType *Line, int LineLen);
static int DGifGetPrefixChar(GifPrefixType *Prefix, int Code, int ClearCode);
static int DGifDecompressInput(GifFileType *GifFile, int *Code);
static int DGifBufferedInput(GifFileType *GifFile, uint8_t *Buf, uint8_t *NextByte);

/******************************************************************************
 * GifFileType constructor with user supplied input function (TVT)
 *****************************************************************************/
static GifFileType *DGifOpen(Fog::Stream* stream)
{
  uint8_t Buf[GIF_STAMP_LEN + 1];
  GifFileType *GifFile;

  GifFile = (GifFileType *)Fog::Memory::calloc(sizeof(GifFileType));
  if (GifFile == NULL)
  {
    _GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
    return NULL;
  }

  GifFile->FileState = FILE_STATE_READ;
  GifFile->stream = stream;

  /* Lets see if this is a GIF file: */
  if (READ(GifFile, Buf, GIF_STAMP_LEN) != GIF_STAMP_LEN)
  {
    _GifError = D_GIF_ERR_READ_FAILED;
    Fog::Memory::free((char *)GifFile);
    return NULL;
  }

  /* The GIF Version number is ignored at this time. Maybe we should do
   * something more useful with it. */
  Buf[GIF_STAMP_LEN] = 0;
  if (strncmp(GIF_STAMP, (const char*)Buf, GIF_VERSION_POS) != 0)
  {
    _GifError = D_GIF_ERR_NOT_GIF_FILE;
    Fog::Memory::free((char *)GifFile);
    return NULL;
  }

  if (DGifGetScreenDesc(GifFile) == GIF_ERROR)
  {
    Fog::Memory::free((char *)GifFile);
    return NULL;
  }

  _GifError = 0;
  return GifFile;
}

/******************************************************************************
 * This routine should be called before any other DGif calls. Note that
 * this routine is called automatically from DGif file open routines.
 *****************************************************************************/
static int DGifGetScreenDesc(GifFileType * GifFile)
{
  int i, BitsPerPixel;
  uint8_t Buf[3];

  if (!IS_READABLE(GifFile))
  {
    /// This file was NOT open for reading:
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  // Put the screen descriptor into the file:
  if (DGifGetWord(GifFile, &GifFile->SWidth) == GIF_ERROR ||
    DGifGetWord(GifFile, &GifFile->SHeight) == GIF_ERROR)
  {
    return GIF_ERROR;
  }

  if (READ(GifFile, Buf, 3) != 3)
  {
    _GifError = D_GIF_ERR_READ_FAILED;
    return GIF_ERROR;
  }
  GifFile->SColorResolution = (((Buf[0] & 0x70) + 1) >> 4) + 1;
  BitsPerPixel = (Buf[0] & 0x07) + 1;
  GifFile->SBackGroundColor = Buf[1];

  if (Buf[0] & 0x80)
  {
    // Do we have global color map?
    GifFile->SColorMap = MakeMapObject(1 << BitsPerPixel, NULL);
    if (GifFile->SColorMap == NULL)
    {
      _GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
      return GIF_ERROR;
    }

    // Get the global color map:
    for (i = 0; i < GifFile->SColorMap->ColorCount; i++)
    {
      if (READ(GifFile, Buf, 3) != 3)
      {
        FreeMapObject(GifFile->SColorMap);
        GifFile->SColorMap = NULL;
        _GifError = D_GIF_ERR_READ_FAILED;
        return GIF_ERROR;
      }
      GifFile->SColorMap->Colors[i].Red = Buf[0];
      GifFile->SColorMap->Colors[i].Green = Buf[1];
      GifFile->SColorMap->Colors[i].Blue = Buf[2];
    }
  }
  else
  {
    GifFile->SColorMap = NULL;
  }

  return GIF_OK;
}

/******************************************************************************
 * This routine should be called before any attempt to read an image.
 *****************************************************************************/
static int DGifGetRecordType(GifFileType * GifFile, GifRecordType * Type)
{
  uint8_t Buf;

  if (!IS_READABLE(GifFile)) {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  if (READ(GifFile, &Buf, 1) != 1) {
    _GifError = D_GIF_ERR_READ_FAILED;
    return GIF_ERROR;
  }

  switch (Buf) {
    case ',':
      *Type = IMAGE_DESC_RECORD_TYPE;
      break;
    case '!':
      *Type = EXTENSION_RECORD_TYPE;
      break;
    case ';':
      *Type = TERMINATE_RECORD_TYPE;
      break;
    default:
      *Type = UNDEFINED_RECORD_TYPE;
      _GifError = D_GIF_ERR_WRONG_RECORD;
      return GIF_ERROR;
  }

  return GIF_OK;
}

/******************************************************************************
 * This routine should be called before any attempt to read an image.
 * Note it is assumed the Image desc. header (',') has been read.
 *****************************************************************************/
static int DGifGetImageDesc(GifFileType * GifFile)
{
  int i, BitsPerPixel;
  uint8_t Buf[3];
  SavedImage *sp;

  if (!IS_READABLE(GifFile))
  {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  if (DGifGetWord(GifFile, &GifFile->Image.Left) == GIF_ERROR ||
      DGifGetWord(GifFile, &GifFile->Image.Top) == GIF_ERROR ||
      DGifGetWord(GifFile, &GifFile->Image.Width) == GIF_ERROR ||
      DGifGetWord(GifFile, &GifFile->Image.Height) == GIF_ERROR)
      return GIF_ERROR;
  if (READ(GifFile, Buf, 1) != 1) {
      _GifError = D_GIF_ERR_READ_FAILED;
      return GIF_ERROR;
  }
  BitsPerPixel = (Buf[0] & 0x07) + 1;
  GifFile->Image.Interlace = (Buf[0] & 0x40);
  if (Buf[0] & 0x80)\
  {
    /* Does this image have local color map? */

    /*** FIXME: Why do we check both of these in order to do this?
     * Why do we have both Image and SavedImages? */
    if (GifFile->Image.ColorMap && GifFile->SavedImages == NULL)
      FreeMapObject(GifFile->Image.ColorMap);

    GifFile->Image.ColorMap = MakeMapObject(1 << BitsPerPixel, NULL);
    if (GifFile->Image.ColorMap == NULL) {
      _GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
      return GIF_ERROR;
    }

    /* Get the image local color map: */
    for (i = 0; i < GifFile->Image.ColorMap->ColorCount; i++) {
      if (READ(GifFile, Buf, 3) != 3) {
        FreeMapObject(GifFile->Image.ColorMap);
        _GifError = D_GIF_ERR_READ_FAILED;
        GifFile->Image.ColorMap = NULL;
        return GIF_ERROR;
      }
      GifFile->Image.ColorMap->Colors[i].Red = Buf[0];
      GifFile->Image.ColorMap->Colors[i].Green = Buf[1];
      GifFile->Image.ColorMap->Colors[i].Blue = Buf[2];
    }
  }
  else if (GifFile->Image.ColorMap)
  {
    FreeMapObject(GifFile->Image.ColorMap);
    GifFile->Image.ColorMap = NULL;
  }

  if (GifFile->SavedImages) {
    if ((GifFile->SavedImages = (SavedImage *)Fog::Memory::realloc(GifFile->SavedImages,
                                sizeof(SavedImage) *
                                (GifFile->ImageCount + 1))) == NULL) {
      _GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
      return GIF_ERROR;
    }
  }
  else
  {
    if ((GifFile->SavedImages = (SavedImage *) Fog::Memory::alloc(sizeof(SavedImage))) == NULL)
    {
      _GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
      return GIF_ERROR;
    }
  }

  sp = &GifFile->SavedImages[GifFile->ImageCount];
  memcpy(&sp->ImageDesc, &GifFile->Image, sizeof(GifImageDesc));
  if (GifFile->Image.ColorMap != NULL) {
    sp->ImageDesc.ColorMap = MakeMapObject(
                             GifFile->Image.ColorMap->ColorCount,
                             GifFile->Image.ColorMap->Colors);
    if (sp->ImageDesc.ColorMap == NULL) {
      _GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
      return GIF_ERROR;
    }
  }
  sp->RasterBits = (unsigned char *)NULL;
  sp->ExtensionBlockCount = 0;
  sp->ExtensionBlocks = (ExtensionBlock *) NULL;

  GifFile->ImageCount++;
  GifFile->PixelCount = (long)GifFile->Image.Width * (long)GifFile->Image.Height;

  DGifSetupDecompress(GifFile);  /* Reset decompress algorithm parameters. */

  return GIF_OK;
}

/******************************************************************************
 * Get one full scanned line (Line) of length LineLen from GIF file.
 *****************************************************************************/
static int DGifGetLine(GifFileType * GifFile, GifPixelType * Line, int LineLen)
{
  uint8_t *Dummy;

  if (!IS_READABLE(GifFile))
  {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  if (!LineLen) LineLen = GifFile->Image.Width;

  if ((GifFile->PixelCount -= LineLen) > 0xffff0000) {
    _GifError = D_GIF_ERR_DATA_TOO_BIG;
    return GIF_ERROR;
  }

  if (DGifDecompressLine(GifFile, Line, LineLen) == GIF_OK) {
    if (GifFile->PixelCount == 0) {
      /* We probably would not be called any more, so lets clean
       * everything before we return: need to flush out all rest of
       * image until empty block (size 0) detected. We use GetCodeNext. */
      do {
        if (DGifGetCodeNext(GifFile, &Dummy) == GIF_ERROR)
          return GIF_ERROR;
      } while (Dummy != NULL) ;
    }
    return GIF_OK;
  } else
    return GIF_ERROR;
}

/******************************************************************************
 * Put one pixel (Pixel) into GIF file.
 *****************************************************************************/
static int DGifGetPixel(GifFileType * GifFile, GifPixelType Pixel)
{
  uint8_t *Dummy;

  if (!IS_READABLE(GifFile)) {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }
  if (--GifFile->PixelCount > 0xffff0000)
  {
    _GifError = D_GIF_ERR_DATA_TOO_BIG;
    return GIF_ERROR;
  }

  if (DGifDecompressLine(GifFile, &Pixel, 1) == GIF_OK) {
    if (GifFile->PixelCount == 0) {
      /* We probably would not be called any more, so lets clean
       * everything before we return: need to flush out all rest of
       * image until empty block (size 0) detected. We use GetCodeNext. */
      do
        if (DGifGetCodeNext(GifFile, &Dummy) == GIF_ERROR)
          return GIF_ERROR;
      while (Dummy != NULL) ;
    }
    return GIF_OK;
  } else
    return GIF_ERROR;
}

/******************************************************************************
 * Get an extension block (see GIF manual) from gif file. This routine only
 * returns the first data block, and DGifGetExtensionNext should be called
 * after this one until NULL extension is returned.
 * The Extension should NOT be freed by the user (not dynamically allocated).
 * Note it is assumed the Extension desc. header ('!') has been read.
 *****************************************************************************/
static int DGifGetExtension(GifFileType * GifFile, int *ExtCode, uint8_t ** Extension)
{
  uint8_t Buf;

  if (!IS_READABLE(GifFile)) {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  if (READ(GifFile, &Buf, 1) != 1) {
    _GifError = D_GIF_ERR_READ_FAILED;
    return GIF_ERROR;
  }
  *ExtCode = Buf;

  return DGifGetExtensionNext(GifFile, Extension);
}

/******************************************************************************
 * Get a following extension block (see GIF manual) from gif file. This
 * routine should be called until NULL Extension is returned.
 * The Extension should NOT be freed by the user (not dynamically allocated).
 *****************************************************************************/
static int DGifGetExtensionNext(GifFileType * GifFile, uint8_t ** Extension)
{
  uint8_t Buf;

  if (READ(GifFile, &Buf, 1) != 1) {
    _GifError = D_GIF_ERR_READ_FAILED;
    return GIF_ERROR;
  }
  if (Buf > 0) {
    *Extension = GifFile->Buf;    /* Use private unused buffer. */
    (*Extension)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
    if (READ(GifFile, &((*Extension)[1]), Buf) != Buf) {
      _GifError = D_GIF_ERR_READ_FAILED;
      return GIF_ERROR;
    }
  } else
    *Extension = NULL;

  return GIF_OK;
}

/******************************************************************************
 * This routine should be called last, to close the GIF file.
 *****************************************************************************/
static int DGifCloseFile(GifFileType * GifFile)
{
  if (!IS_READABLE(GifFile))
  {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  if (GifFile->Image.ColorMap)
  {
    FreeMapObject(GifFile->Image.ColorMap);
    GifFile->Image.ColorMap = NULL;
  }

  if (GifFile->SColorMap)
  {
    FreeMapObject(GifFile->SColorMap);
    GifFile->SColorMap = NULL;
  }

  if (GifFile->SavedImages)
  {
    FreeSavedImages(GifFile);
    GifFile->SavedImages = NULL;
  }

  Fog::Memory::free(GifFile);
  return GIF_OK;
}

/******************************************************************************
 * Get 2 bytes (word) from the given file:
 *****************************************************************************/
static int DGifGetWord(GifFileType * GifFile, GifWord *Word)
{
  unsigned char c[2];

  if (READ(GifFile, c, 2) != 2) {
    _GifError = D_GIF_ERR_READ_FAILED;
    return GIF_ERROR;
  }

  *Word = (((unsigned int)c[1]) << 8) + c[0];
  return GIF_OK;
}

/******************************************************************************
 * Get the image code in compressed form.  This routine can be called if the
 * information needed to be piped out as is. Obviously this is much faster
 * than decoding and encoding again. This routine should be followed by calls
 * to DGifGetCodeNext, until NULL block is returned.
 * The block should NOT be freed by the user (not dynamically allocated).
 *****************************************************************************/
static int DGifGetCode(GifFileType * GifFile, int *CodeSize, uint8_t ** CodeBlock)
{
  if (!IS_READABLE(GifFile))
  {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  *CodeSize = GifFile->BitsPerPixel;

  return DGifGetCodeNext(GifFile, CodeBlock);
}

/******************************************************************************
 * Continue to get the image code in compressed form. This routine should be
 * called until NULL block is returned.
 * The block should NOT be freed by the user (not dynamically allocated).
 *****************************************************************************/
static int DGifGetCodeNext(GifFileType * GifFile, uint8_t ** CodeBlock)
{
  uint8_t Buf;

  if (READ(GifFile, &Buf, 1) != 1)
  {
    _GifError = D_GIF_ERR_READ_FAILED;
    return GIF_ERROR;
  }

  if (Buf > 0)
  {
    *CodeBlock = GifFile->Buf;    /* Use private unused buffer. */
    (*CodeBlock)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
    if (READ(GifFile, &((*CodeBlock)[1]), Buf) != Buf) {
      _GifError = D_GIF_ERR_READ_FAILED;
      return GIF_ERROR;
    }
  }
  else
  {
    *CodeBlock = NULL;
    GifFile->Buf[0] = 0;    /* Make sure the buffer is empty! */
    GifFile->PixelCount = 0;    /* And local info. indicate image read. */
  }

  return GIF_OK;
}

/******************************************************************************
 * Setup the LZ decompression for this image:
 *****************************************************************************/
static int DGifSetupDecompress(GifFileType * GifFile)
{
  int i, BitsPerPixel;
  uint8_t CodeSize;
  GifPrefixType *Prefix;

  READ(GifFile, &CodeSize, 1);    /* Read Code size from file. */
  BitsPerPixel = CodeSize;

  GifFile->Buf[0] = 0;    /* Input Buffer empty. */
  GifFile->BitsPerPixel = BitsPerPixel;
  GifFile->ClearCode = (1 << BitsPerPixel);
  GifFile->EOFCode = GifFile->ClearCode + 1;
  GifFile->RunningCode = GifFile->EOFCode + 1;
  GifFile->RunningBits = BitsPerPixel + 1;    /* Number of bits per code. */
  GifFile->MaxCode1 = 1 << GifFile->RunningBits;    /* Max. code + 1. */
  GifFile->StackPtr = 0;    /* No pixels on the pixel stack. */
  GifFile->LastCode = NO_SUCH_CODE;
  GifFile->CrntShiftState = 0;    /* No information in CrntShiftDWord. */
  GifFile->CrntShiftDWord = 0;

  Prefix = GifFile->Prefix;
  for (i = 0; i <= LZ_MAX_CODE; i++) Prefix[i] = NO_SUCH_CODE;

  return GIF_OK;
}

/******************************************************************************
 * The LZ decompression routine:
 * This version decompress the given gif file into Line of length LineLen.
 * This routine can be called few times (one per scan line, for example), in
 * order the complete the whole image.
 *****************************************************************************/
static int DGifDecompressLine(GifFileType * GifFile, GifPixelType * Line, int LineLen)
{
  int i = 0;
  int j, CrntCode, EOFCode, ClearCode, CrntPrefix, LastCode, StackPtr;
  uint8_t *Stack, *Suffix;
  GifPrefixType *Prefix;

  StackPtr = GifFile->StackPtr;
  Prefix = GifFile->Prefix;
  Suffix = GifFile->Suffix;
  Stack = GifFile->Stack;
  EOFCode = GifFile->EOFCode;
  ClearCode = GifFile->ClearCode;
  LastCode = GifFile->LastCode;

  if (StackPtr != 0) {
    /* Let pop the stack off before continueing to read the gif file: */
    while (StackPtr != 0 && i < LineLen)
      Line[i++] = Stack[--StackPtr];
  }

  while (i < LineLen)
  {
    /* Decode LineLen items. */
    if (DGifDecompressInput(GifFile, &CrntCode) == GIF_ERROR) return GIF_ERROR;

    if (CrntCode == EOFCode)
    {
      /* Note however that usually we will not be here as we will stop
       * decoding as soon as we got all the pixel, or EOF code will
       * not be read at all, and DGifGetLine/Pixel clean everything.  */
      if (i != LineLen - 1 || GifFile->PixelCount != 0) {
        _GifError = D_GIF_ERR_EOF_TOO_SOON;
        return GIF_ERROR;
      }
      i++;
    }
    else if (CrntCode == ClearCode)
    {
      /* We need to start over again: */
      for (j = 0; j <= LZ_MAX_CODE; j++) Prefix[j] = NO_SUCH_CODE;
      GifFile->RunningCode = GifFile->EOFCode + 1;
      GifFile->RunningBits = GifFile->BitsPerPixel + 1;
      GifFile->MaxCode1 = 1 << GifFile->RunningBits;
      LastCode = GifFile->LastCode = NO_SUCH_CODE;
    }
    else
    {
      /* Its regular code - if in pixel range simply add it to output
       * stream, otherwise trace to codes linked list until the prefix
       * is in pixel range: */
      if (CrntCode < ClearCode) {
        /* This is simple - its pixel scalar, so add it to output: */
        Line[i++] = CrntCode;
      }
      else
      {
        /* Its a code to needed to be traced: trace the linked list
         * until the prefix is a pixel, while pushing the suffix
         * pixels on our stack. If we done, pop the stack in reverse
         * (thats what stack is good for!) order to output.  */
        if (Prefix[CrntCode] == NO_SUCH_CODE)
        {
          /* Only allowed if CrntCode is exactly the running code:
           * In that case CrntCode = XXXCode, CrntCode or the
           * prefix code is last code and the suffix char is
           * exactly the prefix of last code! */
          if (CrntCode == GifFile->RunningCode - 2)
          {
            CrntPrefix = LastCode;
            Suffix[GifFile->RunningCode - 2] =
               Stack[StackPtr++] = DGifGetPrefixChar(Prefix, LastCode, ClearCode);
          } else {
            _GifError = D_GIF_ERR_IMAGE_DEFECT;
            return GIF_ERROR;
          }
        }
        else
        {
          CrntPrefix = CrntCode;
        }

        /* Now (if image is O.K.) we should not get an NO_SUCH_CODE
         * During the trace. As we might loop forever, in case of
         * defective image, we count the number of loops we trace
         * and stop if we got LZ_MAX_CODE. obviously we can not
         * loop more than that.  */
        j = 0;
        while (j++ <= LZ_MAX_CODE && CrntPrefix > ClearCode && CrntPrefix <= LZ_MAX_CODE) {
          Stack[StackPtr++] = Suffix[CrntPrefix];
          CrntPrefix = Prefix[CrntPrefix];
        }

        if (j >= LZ_MAX_CODE || CrntPrefix > LZ_MAX_CODE) {
          _GifError = D_GIF_ERR_IMAGE_DEFECT;
          return GIF_ERROR;
        }
        /* Push the last character on stack: */
        Stack[StackPtr++] = CrntPrefix;

        /* Now lets pop all the stack into output: */
        while (StackPtr != 0 && i < LineLen) Line[i++] = Stack[--StackPtr];
      }

      if (LastCode != NO_SUCH_CODE)
      {
        Prefix[GifFile->RunningCode - 2] = LastCode;

        if (CrntCode == GifFile->RunningCode - 2) {
          /* Only allowed if CrntCode is exactly the running code:
           * In that case CrntCode = XXXCode, CrntCode or the
           * prefix code is last code and the suffix char is
           * exactly the prefix of last code! */
          Suffix[GifFile->RunningCode - 2] = DGifGetPrefixChar(Prefix, LastCode, ClearCode);
        }
        else
        {
          Suffix[GifFile->RunningCode - 2] = DGifGetPrefixChar(Prefix, CrntCode, ClearCode);
        }
      }
      LastCode = CrntCode;
    }
  }

  GifFile->LastCode = LastCode;
  GifFile->StackPtr = StackPtr;

  return GIF_OK;
}

/******************************************************************************
 * Routine to trace the Prefixes linked list until we get a prefix which is
 * not code, but a pixel value (less than ClearCode). Returns that pixel value.
 * If image is defective, we might loop here forever, so we limit the loops to
 * the maximum possible if image O.k. - LZ_MAX_CODE times.
 *****************************************************************************/
static int DGifGetPrefixChar(GifPrefixType *Prefix, int Code, int ClearCode)
{
  int i = 0;
  while (Code > ClearCode && i++ <= LZ_MAX_CODE) Code = Prefix[Code];
  return Code;
}

/******************************************************************************
 * Interface for accessing the LZ codes directly. Set Code to the real code
 * (12bits), or to -1 if EOF code is returned.
 *****************************************************************************/
static int DGifGetLZCodes(GifFileType * GifFile, int *Code)
{
  uint8_t *CodeBlock;

  if (!IS_READABLE(GifFile))
  {
    /* This file was NOT open for reading: */
    _GifError = D_GIF_ERR_NOT_READABLE;
    return GIF_ERROR;
  }

  if (DGifDecompressInput(GifFile, Code) == GIF_ERROR) return GIF_ERROR;

  if (*Code == GifFile->EOFCode) {
    /* Skip rest of codes (hopefully only NULL terminating block): */
    do {
      if (DGifGetCodeNext(GifFile, &CodeBlock) == GIF_ERROR)
        return GIF_ERROR;
    } while (CodeBlock != NULL) ;

    *Code = -1;
  }
  else if (*Code == GifFile->ClearCode)
  {
    /* We need to start over again: */
    GifFile->RunningCode = GifFile->EOFCode + 1;
    GifFile->RunningBits = GifFile->BitsPerPixel + 1;
    GifFile->MaxCode1 = 1 << GifFile->RunningBits;
  }

  return GIF_OK;
}

/******************************************************************************
 * The LZ decompression input routine:
 * This routine is responsable for the decompression of the bit stream from
 * 8 bits (bytes) packets, into the real codes.
 * Returns GIF_OK if read succesfully.
 *****************************************************************************/
static int DGifDecompressInput(GifFileType * GifFile, int *Code)
{
  static unsigned short CodeMasks[] = {
    0x0000, 0x0001, 0x0003, 0x0007,
    0x000f, 0x001f, 0x003f, 0x007f,
    0x00ff, 0x01ff, 0x03ff, 0x07ff,
    0x0fff
  };

  uint8_t NextByte;

  /* The image can't contain more than LZ_BITS per code. */
  if (GifFile->RunningBits > LZ_BITS) {
    _GifError = D_GIF_ERR_IMAGE_DEFECT;
    return GIF_ERROR;
  }

  while (GifFile->CrntShiftState < GifFile->RunningBits) {
    /* Needs to get more bytes from input stream for next code: */
    if (DGifBufferedInput(GifFile, GifFile->Buf, &NextByte) == GIF_ERROR) {
      return GIF_ERROR;
    }
    GifFile->CrntShiftDWord |= ((unsigned long)NextByte) << GifFile->CrntShiftState;
    GifFile->CrntShiftState += 8;
  }
  *Code = GifFile->CrntShiftDWord & CodeMasks[GifFile->RunningBits];

  GifFile->CrntShiftDWord >>= GifFile->RunningBits;
  GifFile->CrntShiftState -= GifFile->RunningBits;

  /* If code cannot fit into RunningBits bits, must raise its size. Note
   * however that codes above 4095 are used for special signaling.
   * If we're using LZ_BITS bits already and we're at the max code, just
   * keep using the table as it is, don't increment GifFile->RunningCode.
   */
  if (GifFile->RunningCode < LZ_MAX_CODE + 2 &&
    ++GifFile->RunningCode > GifFile->MaxCode1 &&
    GifFile->RunningBits < LZ_BITS)
  {
    GifFile->MaxCode1 <<= 1;
    GifFile->RunningBits++;
  }
  return GIF_OK;
}

/******************************************************************************
 * This routines read one gif data block at a time and buffers it internally
 * so that the decompression routine could access it.
 * The routine returns the next byte from its internal buffer (or read next
 * block in if buffer empty) and returns GIF_OK if succesful.
 *****************************************************************************/
static int DGifBufferedInput(GifFileType * GifFile, uint8_t * Buf, uint8_t * NextByte)
{
  if (Buf[0] == 0)
  {
    /* Needs to read the next buffer - this one is empty: */
    if (READ(GifFile, Buf, 1) != 1) {
      _GifError = D_GIF_ERR_READ_FAILED;
      return GIF_ERROR;
    }
    /* There shouldn't be any empty data blocks here as the LZW spec
     * says the LZW termination code should come first.  Therefore we
     * shouldn't be inside this routine at that point.
     */
    if (Buf[0] == 0) {
      _GifError = D_GIF_ERR_IMAGE_DEFECT;
      return GIF_ERROR;
    }
    if (READ(GifFile, &Buf[1], Buf[0]) != Buf[0]) {
      _GifError = D_GIF_ERR_READ_FAILED;
      return GIF_ERROR;
    }
    *NextByte = Buf[1];
    Buf[1] = 2;    /* We use now the second place as last char read! */
    Buf[0]--;
  }
  else
  {
    *NextByte = Buf[Buf[1]++];
    Buf[0]--;
  }

  return GIF_OK;
}
#ifndef _GBA_NO_FILEIO

/******************************************************************************
 * This routine reads an entire GIF into core, hanging all its state info off
 * the GifFileType pointer.  Call DGifOpenFileName() or DGifOpenFileHandle()
 * first to initialize I/O.  Its inverse is EGifSpew().
 ******************************************************************************/
static int DGifSlurp(GifFileType * GifFile)
{
  int ImageSize;
  GifRecordType RecordType;
  SavedImage *sp;
  uint8_t *ExtData;
  SavedImage temp_save;

  temp_save.ExtensionBlocks = NULL;
  temp_save.ExtensionBlockCount = 0;

  do {
    if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) return (GIF_ERROR);

    switch (RecordType) {
      case IMAGE_DESC_RECORD_TYPE:
        if (DGifGetImageDesc(GifFile) == GIF_ERROR) return (GIF_ERROR);

        sp = &GifFile->SavedImages[GifFile->ImageCount - 1];
        ImageSize = sp->ImageDesc.Width * sp->ImageDesc.Height;

        sp->RasterBits = (unsigned char *)Fog::Memory::alloc(ImageSize * sizeof(GifPixelType));

        if (sp->RasterBits == NULL) return GIF_ERROR;
        if (DGifGetLine(GifFile, sp->RasterBits, ImageSize) == GIF_ERROR) return (GIF_ERROR);
        
        if (temp_save.ExtensionBlocks)
        {
          sp->ExtensionBlocks = temp_save.ExtensionBlocks;
          sp->ExtensionBlockCount = temp_save.ExtensionBlockCount;

          temp_save.ExtensionBlocks = NULL;
          temp_save.ExtensionBlockCount = 0;

          /* FIXME: The following is wrong.  It is left in only for
           * backwards compatibility.  Someday it should go away. Use
           * the sp->ExtensionBlocks->Function variable instead. */
          sp->Function = sp->ExtensionBlocks[0].Function;
        }
        break;

      case EXTENSION_RECORD_TYPE:
        if (DGifGetExtension(GifFile, &temp_save.Function, &ExtData) == GIF_ERROR) return (GIF_ERROR);
        
        while (ExtData != NULL)
        {
          /* Create an extension block with our data */
          if (AddExtensionBlock(&temp_save, ExtData[0], &ExtData[1]) == GIF_ERROR) return (GIF_ERROR);
          if (DGifGetExtensionNext(GifFile, &ExtData) == GIF_ERROR) return (GIF_ERROR);
          temp_save.Function = 0;
        }
        break;

      case TERMINATE_RECORD_TYPE:
        break;

      default:    /* Should be trapped by DGifGetRecordType */
        break;
    }
  } while (RecordType != TERMINATE_RECORD_TYPE);

  /* Just in case the Gif has an extension block without an associated
   * image... (Should we save this into a savefile structure with no image
   * instead? Have to check if the present writing code can handle that as
   * well.... */
  if (temp_save.ExtensionBlocks) FreeExtension(&temp_save);

  return (GIF_OK);
}
#endif /* _GBA_NO_FILEIO */

// ---------------------------------------------------------------------------
// giflib - egif
// ---------------------------------------------------------------------------

// Masks given codes to BitsPerPixel, to make sure all codes are in range:
static GifPixelType CodeMask[] =
{
  0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

static char GifVersionPrefix[GIF_STAMP_LEN + 1] = GIF87_STAMP;

#define WRITE(_gif, _buf, _len) (_gif->stream->write(_buf, _len))

static int EGifPutWord(int Word, GifFileType * GifFile);
static int EGifSetupCompress(GifFileType * GifFile);
static int EGifCompressLine(GifFileType * GifFile, GifPixelType * Line, int LineLen);
static int EGifCompressOutput(GifFileType * GifFile, int Code);
static int EGifBufferedOutput(GifFileType * GifFile, uint8_t * Buf, int c);

/******************************************************************************
 * Output constructor that takes user supplied output function.
 * Basically just a copy of EGifOpenFileHandle. (MRB)
 *****************************************************************************/
static GifFileType *EGifOpen(Fog::Stream* stream)
{
  GifFileType *GifFile;

  GifFile = (GifFileType *)Fog::Memory::calloc(sizeof(GifFileType));
  if (GifFile == NULL)
  {
    _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
    return NULL;
  }

  GifFile->HashTable = _InitHashTable();
  if (GifFile->HashTable == NULL)
  {
    Fog::Memory::free(GifFile);
    _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
    return NULL;
  }

  GifFile->stream = stream;
  GifFile->FileState = FILE_STATE_WRITE;

  _GifError = 0;

  return GifFile;
}

/******************************************************************************
 * Routine to set current GIF version. All files open for write will be
 * using this version until next call to this routine. Version consists of
 * 3 characters as "87a" or "89a". No test is made to validate the version.
 *****************************************************************************/
static void EGifSetGifVersion(const char *Version)
{
  strncpy(GifVersionPrefix + GIF_VERSION_POS, Version, 3);
}

/******************************************************************************
 * This routine should be called before any other EGif calls, immediately
 * follows the GIF file openning.
 *****************************************************************************/
static int EGifPutScreenDesc(GifFileType * GifFile,
  int Width,
  int Height,
  int ColorRes,
  int BackGround,
  const ColorMapObject * ColorMap)
{
  int i;
  uint8_t Buf[3];

  if (GifFile->FileState & FILE_STATE_SCREEN) {
    /* If already has screen descriptor - something is wrong! */
    _GifError = E_GIF_ERR_HAS_SCRN_DSCR;
    return GIF_ERROR;
  }
  if (!IS_WRITEABLE(GifFile)) {
    /* This file was NOT open for writing: */
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

/* First write the version prefix into the file. */
#ifndef DEBUG_NO_PREFIX
  if (WRITE(GifFile, (unsigned char *)GifVersionPrefix,
    strlen(GifVersionPrefix)) != strlen(GifVersionPrefix))
  {
    _GifError = E_GIF_ERR_WRITE_FAILED;
    return GIF_ERROR;
  }
#endif /* DEBUG_NO_PREFIX */

  GifFile->SWidth = Width;
  GifFile->SHeight = Height;
  GifFile->SColorResolution = ColorRes;
  GifFile->SBackGroundColor = BackGround;
  if (ColorMap) {
    GifFile->SColorMap = MakeMapObject(ColorMap->ColorCount,
                                       ColorMap->Colors);
    if (GifFile->SColorMap == NULL) {
      _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
      return GIF_ERROR;
    }
  }
  else
  {
    GifFile->SColorMap = NULL;
  }

  /*
   * Put the logical screen descriptor into the file:
   */
  /* Logical Screen Descriptor: Dimensions */
  EGifPutWord(Width, GifFile);
  EGifPutWord(Height, GifFile);

  /* Logical Screen Descriptor: Packed Fields */
  /* Note: We have actual size of the color table default to the largest
   * possible size (7+1 == 8 bits) because the decoder can use it to decide
   * how to display the files.
   */
  Buf[0] = (ColorMap ? 0x80 : 0x00) | /* Yes/no global colormap */
           ((ColorRes - 1) << 4) | /* Bits allocated to each primary color */
           (ColorMap ? ColorMap->BitsPerPixel - 1 : 0x07 ); /* Actual size of the color table. */
  Buf[1] = BackGround;    /* Index into the ColorTable for background color */
  Buf[2] = 0;             /* Pixel Aspect Ratio */
#ifndef DEBUG_NO_PREFIX
  WRITE(GifFile, Buf, 3);
#endif /* DEBUG_NO_PREFIX */

  /* If we have Global color map - dump it also: */
#ifndef DEBUG_NO_PREFIX
  if (ColorMap != NULL)
  {
    for (i = 0; i < ColorMap->ColorCount; i++) {
      /* Put the ColorMap out also: */
      Buf[0] = ColorMap->Colors[i].Red;
      Buf[1] = ColorMap->Colors[i].Green;
      Buf[2] = ColorMap->Colors[i].Blue;
      if (WRITE(GifFile, Buf, 3) != 3) {
        _GifError = E_GIF_ERR_WRITE_FAILED;
        return GIF_ERROR;
      }
    }
  }
#endif /* DEBUG_NO_PREFIX */

  /* Mark this file as has screen descriptor, and no pixel written yet: */
  GifFile->FileState |= FILE_STATE_SCREEN;

  return GIF_OK;
}

/******************************************************************************
 * This routine should be called before any attempt to dump an image - any
 * call to any of the pixel dump routines.
 *****************************************************************************/
static int EGifPutImageDesc(GifFileType * GifFile,
  int Left, int Top, int Width, int Height,
  int Interlace, const ColorMapObject * ColorMap)
{
  int i;
  uint8_t Buf[3];

  if (GifFile->FileState & FILE_STATE_IMAGE && GifFile->PixelCount > 0xffff0000)
  {
    /* If already has active image descriptor - something is wrong! */
    _GifError = E_GIF_ERR_HAS_IMAG_DSCR;
    return GIF_ERROR;
  }
  if (!IS_WRITEABLE(GifFile)) {
      /* This file was NOT open for writing: */
      _GifError = E_GIF_ERR_NOT_WRITEABLE;
      return GIF_ERROR;
  }

  GifFile->Image.Left = Left;
  GifFile->Image.Top = Top;
  GifFile->Image.Width = Width;
  GifFile->Image.Height = Height;
  GifFile->Image.Interlace = Interlace;
  if (ColorMap) {
    GifFile->Image.ColorMap = MakeMapObject(ColorMap->ColorCount,
                                            ColorMap->Colors);
    if (GifFile->Image.ColorMap == NULL) {
      _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
      return GIF_ERROR;
    }
  } else {
    GifFile->Image.ColorMap = NULL;
  }

  /* Put the image descriptor into the file: */
  Buf[0] = ',';    /* Image seperator character. */
#ifndef DEBUG_NO_PREFIX
  WRITE(GifFile, Buf, 1);
#endif /* DEBUG_NO_PREFIX */
  EGifPutWord(Left, GifFile);
  EGifPutWord(Top, GifFile);
  EGifPutWord(Width, GifFile);
  EGifPutWord(Height, GifFile);
  Buf[0] = (ColorMap ? 0x80 : 0x00) |
           (Interlace ? 0x40 : 0x00) |
           (ColorMap ? ColorMap->BitsPerPixel - 1 : 0);
#ifndef DEBUG_NO_PREFIX
  WRITE(GifFile, Buf, 1);
#endif /* DEBUG_NO_PREFIX */

  /* If we have Global color map - dump it also: */
#ifndef DEBUG_NO_PREFIX
  if (ColorMap != NULL)
  {
    for (i = 0; i < ColorMap->ColorCount; i++) {
      /* Put the ColorMap out also: */
      Buf[0] = ColorMap->Colors[i].Red;
      Buf[1] = ColorMap->Colors[i].Green;
      Buf[2] = ColorMap->Colors[i].Blue;
      if (WRITE(GifFile, Buf, 3) != 3) {
        _GifError = E_GIF_ERR_WRITE_FAILED;
        return GIF_ERROR;
      }
    }
  }
#endif /* DEBUG_NO_PREFIX */
  if (GifFile->SColorMap == NULL && GifFile->Image.ColorMap == NULL) {
    _GifError = E_GIF_ERR_NO_COLOR_MAP;
    return GIF_ERROR;
  }

  /* Mark this file as has screen descriptor: */
  GifFile->FileState |= FILE_STATE_IMAGE;
  GifFile->PixelCount = (long)Width *(long)Height;

  EGifSetupCompress(GifFile);    /* Reset compress algorithm parameters. */

  return GIF_OK;
}

/******************************************************************************
 * Put one full scanned line (Line) of length LineLen into GIF file.
 *****************************************************************************/
static int EGifPutLine(GifFileType * GifFile, GifPixelType * Line, int LineLen)
{
  int i;
  GifPixelType Mask;

  if (!IS_WRITEABLE(GifFile)) {
    /* This file was NOT open for writing: */
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  if (!LineLen) LineLen = GifFile->Image.Width;

  if (GifFile->PixelCount < (unsigned)LineLen)
  {
    _GifError = E_GIF_ERR_DATA_TOO_BIG;
    return GIF_ERROR;
  }
  GifFile->PixelCount -= LineLen;

  /* Make sure the codes are not out of bit range, as we might generate
   * wrong code (because of overflow when we combine them) in this case: */
  Mask = CodeMask[GifFile->BitsPerPixel];
  for (i = 0; i < LineLen; i++) Line[i] &= Mask;

  return EGifCompressLine(GifFile, Line, LineLen);
}

/******************************************************************************
 * Put one pixel (Pixel) into GIF file.
 *****************************************************************************/
static int EGifPutPixel(GifFileType * GifFile, GifPixelType Pixel)
{
  if (!IS_WRITEABLE(GifFile)) {
    /* This file was NOT open for writing: */
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  if (GifFile->PixelCount == 0) {
    _GifError = E_GIF_ERR_DATA_TOO_BIG;
    return GIF_ERROR;
  }
  --GifFile->PixelCount;

  /* Make sure the code is not out of bit range, as we might generate
   * wrong code (because of overflow when we combine them) in this case: */
  Pixel &= CodeMask[GifFile->BitsPerPixel];

  return EGifCompressLine(GifFile, &Pixel, 1);
}

/******************************************************************************
 * Put a comment into GIF file using the GIF89 comment extension block.
 *****************************************************************************/

static int EGifPutComment(GifFileType * GifFile, const char *Comment)
{
  unsigned int length = strlen(Comment);
  char *buf;

  if (length <= 255)
  {
    return EGifPutExtension(GifFile, COMMENT_EXT_FUNC_CODE, length, Comment);
  }
  else
  {
    buf = (char *)Comment;
    if (EGifPutExtensionFirst(GifFile, COMMENT_EXT_FUNC_CODE, 255, buf) == GIF_ERROR) return GIF_ERROR;
    length -= 255;
    buf = buf + 255;

    // Break the comment into 255 byte sub blocks.
    while (length > 255)
    {
      if (EGifPutExtensionNext(GifFile, 0, 255, buf) == GIF_ERROR) return GIF_ERROR;
      buf = buf + 255;
      length -= 255;
    }

    // Output any partial block and the clear code.
    if (length > 0)
    {
      if (EGifPutExtensionLast(GifFile, 0, length, buf) == GIF_ERROR) return GIF_ERROR;
    }
    else
    {
      if (EGifPutExtensionLast(GifFile, 0, 0, NULL) == GIF_ERROR) return GIF_ERROR;
    }
  }

  return GIF_OK;
}

/******************************************************************************
 * Put a first extension block (see GIF manual) into gif file.  Here more
 * extensions can be dumped using EGifPutExtensionNext until
 * EGifPutExtensionLast is invoked.
 *****************************************************************************/
static int EGifPutExtensionFirst(GifFileType * GifFile, int ExtCode, int ExtLen, const void* Extension)
{
  uint8_t Buf[3];

  if (!IS_WRITEABLE(GifFile))
  {
    // This file was NOT open for writing.
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  if (ExtCode == 0)
  {
    WRITE(GifFile, (uint8_t *)&ExtLen, 1);
  }
  else
  {
    Buf[0] = '!';
    Buf[1] = ExtCode;
    Buf[2] = ExtLen;
    WRITE(GifFile, Buf, 3);
  }

  WRITE(GifFile, Extension, ExtLen);

  return GIF_OK;
}

/******************************************************************************
 * Put a middle extension block (see GIF manual) into gif file.
 *****************************************************************************/
static int EGifPutExtensionNext(GifFileType * GifFile, int ExtCode, int ExtLen, const void* Extension)
{
  uint8_t Buf;

  if (!IS_WRITEABLE(GifFile))
  {
    // This file was NOT open for writing.
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  Buf = ExtLen;
  WRITE(GifFile, &Buf, 1);
  WRITE(GifFile, Extension, ExtLen);

  return GIF_OK;
}

/******************************************************************************
 * Put a last extension block (see GIF manual) into gif file.
 *****************************************************************************/
static int EGifPutExtensionLast(GifFileType * GifFile, int ExtCode, int ExtLen, const void* Extension) 
{
  uint8_t Buf;

  if (!IS_WRITEABLE(GifFile))
  {
    // This file was NOT open for writing.
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  // If we are given an extension sub-block output it now.
  if (ExtLen > 0) {
    Buf = ExtLen;
    WRITE(GifFile, &Buf, 1);
    WRITE(GifFile, Extension, ExtLen);
  }

  // Write the block terminator.
  Buf = 0;
  WRITE(GifFile, &Buf, 1);

  return GIF_OK;
}

/******************************************************************************
 * Put an extension block (see GIF manual) into gif file.
 * Warning: This function is only useful for Extension blocks that have at
 * most one subblock.  Extensions with more than one subblock need to use the
 * EGifPutExtension{First,Next,Last} functions instead.
 *****************************************************************************/
static int EGifPutExtension(GifFileType * GifFile, int ExtCode, int ExtLen, const void* Extension)
{
  uint8_t Buf[3];

  if (!IS_WRITEABLE(GifFile))
  {
    // This file was NOT open for writing.
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  if (ExtCode == 0)
  {
    WRITE(GifFile, (uint8_t *)&ExtLen, 1);
  }
  else
  {
    Buf[0] = '!';       /* Extension Introducer 0x21 */
    Buf[1] = ExtCode;   /* Extension Label */
    Buf[2] = ExtLen;    /* Extension length */
    WRITE(GifFile, Buf, 3);
  }
  WRITE(GifFile, Extension, ExtLen);
  Buf[0] = 0;
  WRITE(GifFile, Buf, 1);

  return GIF_OK;
}

/******************************************************************************
 * Put the image code in compressed form. This routine can be called if the
 * information needed to be piped out as is. Obviously this is much faster
 * than decoding and encoding again. This routine should be followed by calls
 * to EGifPutCodeNext, until NULL block is given.
 * The block should NOT be freed by the user (not dynamically allocated).
 *****************************************************************************/
static int EGifPutCode(GifFileType * GifFile, int CodeSize, const uint8_t * CodeBlock)
{
  if (!IS_WRITEABLE(GifFile))
  {
    // This file was NOT open for writing.
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  /* No need to dump code size as Compression set up does any for us: */
  /*
   * Buf = CodeSize;
   * if (WRITE(GifFile, &Buf, 1) != 1) {
   *      _GifError = E_GIF_ERR_WRITE_FAILED;
   *      return GIF_ERROR;
   * }
   */
  return EGifPutCodeNext(GifFile, CodeBlock);
}

/******************************************************************************
 * Continue to put the image code in compressed form. This routine should be
 * called with blocks of code as read via DGifGetCode/DGifGetCodeNext. If
 * given buffer pointer is NULL, empty block is written to mark end of code.
 *****************************************************************************/
static int EGifPutCodeNext(GifFileType* GifFile, const uint8_t* CodeBlock)
{
  uint8_t Buf;

  if (CodeBlock != NULL)
  {
    if (WRITE(GifFile, CodeBlock, CodeBlock[0] + 1) != (unsigned)(CodeBlock[0] + 1))
    {
      _GifError = E_GIF_ERR_WRITE_FAILED;
      return GIF_ERROR;
    }
  }
  else
  {
    Buf = 0;
    if (WRITE(GifFile, &Buf, 1) != 1)
    {
      _GifError = E_GIF_ERR_WRITE_FAILED;
      return GIF_ERROR;
    }
    // And local info. indicate image read.
    GifFile->PixelCount = 0;
  }

  return GIF_OK;
}

/******************************************************************************
 * This routine should be called last, to close GIF file.
 *****************************************************************************/

static int EGifCloseFile(GifFileType* GifFile)
{
  uint8_t Buf;

  if (GifFile == NULL) return GIF_ERROR;

  if (!IS_WRITEABLE(GifFile))
  {
    // This file was NOT open for writing.
    _GifError = E_GIF_ERR_NOT_WRITEABLE;
    return GIF_ERROR;
  }

  Buf = ';';
  WRITE(GifFile, &Buf, 1);

  if (GifFile->Image.ColorMap)
  {
    FreeMapObject(GifFile->Image.ColorMap);
    GifFile->Image.ColorMap = NULL;
  }

  if (GifFile->SColorMap)
  {
    FreeMapObject(GifFile->SColorMap);
    GifFile->SColorMap = NULL;
  }

  if (GifFile->HashTable)
  {
    Fog::Memory::free(GifFile->HashTable);
  }

  Fog::Memory::free(GifFile);
  return GIF_OK;
}

/******************************************************************************
 * Put 2 bytes (word) into the given file:
 *****************************************************************************/
static int EGifPutWord(int Word, GifFileType * GifFile)
{
  unsigned char c[2];

  c[0] = Word & 0xff;
  c[1] = (Word >> 8) & 0xff;
#ifndef DEBUG_NO_PREFIX
  if (WRITE(GifFile, c, 2) == 2)
    return GIF_OK;
  else
    return GIF_ERROR;
#else
  return GIF_OK;
#endif /* DEBUG_NO_PREFIX */
}

/******************************************************************************
 * Setup the LZ compression for this image:
 *****************************************************************************/
static int EGifSetupCompress(GifFileType * GifFile)
{
  int BitsPerPixel;
  uint8_t Buf;

  // Test and see what color map to use, and from it # bits per pixel.
  if (GifFile->Image.ColorMap)
    BitsPerPixel = GifFile->Image.ColorMap->BitsPerPixel;
  else if (GifFile->SColorMap)
    BitsPerPixel = GifFile->SColorMap->BitsPerPixel;
  else {
    _GifError = E_GIF_ERR_NO_COLOR_MAP;
    return GIF_ERROR;
  }

  Buf = BitsPerPixel = (BitsPerPixel < 2 ? 2 : BitsPerPixel);
  WRITE(GifFile, &Buf, 1);    /* Write the Code size to file. */

  GifFile->Buf[0] = 0;    /* Nothing was output yet. */
  GifFile->BitsPerPixel = BitsPerPixel;
  GifFile->ClearCode = (1 << BitsPerPixel);
  GifFile->EOFCode = GifFile->ClearCode + 1;
  GifFile->RunningCode = GifFile->EOFCode + 1;
  GifFile->RunningBits = BitsPerPixel + 1;    /* Number of bits per code. */
  GifFile->MaxCode1 = 1 << GifFile->RunningBits;    /* Max. code + 1. */
  GifFile->CrntCode = FIRST_CODE;    /* Signal that this is first one! */
  GifFile->CrntShiftState = 0;    /* No information in CrntShiftDWord. */
  GifFile->CrntShiftDWord = 0;

  /* Clear hash table and send Clear to make sure the decoder do the same. */
  _ClearHashTable(GifFile->HashTable);

  if (EGifCompressOutput(GifFile, GifFile->ClearCode) == GIF_ERROR) {
    _GifError = E_GIF_ERR_DISK_IS_FULL;
    return GIF_ERROR;
  }
  return GIF_OK;
}

/******************************************************************************
 * The LZ compression routine:
 * This version compresses the given buffer Line of length LineLen.
 * This routine can be called a few times (one per scan line, for example), in
 * order to complete the whole image.
******************************************************************************/
static int EGifCompressLine(GifFileType * GifFile, GifPixelType * Line, int LineLen)
{
  int i = 0, CrntCode, NewCode;
  unsigned long NewKey;
  GifPixelType Pixel;
  GifHashTableType *HashTable;

  HashTable = GifFile->HashTable;

  if (GifFile->CrntCode == FIRST_CODE)    /* Its first time! */
    CrntCode = Line[i++];
  else
    CrntCode = GifFile->CrntCode;    /* Get last code in compression. */

  while (i < LineLen) {   /* Decode LineLen items. */
    Pixel = Line[i++];  /* Get next pixel from stream. */
    /* Form a new unique key to search hash table for the code combines 
     * CrntCode as Prefix string with Pixel as postfix char.
     */
    NewKey = (((uint32_t) CrntCode) << 8) + Pixel;
    if ((NewCode = _ExistsHashTable(HashTable, NewKey)) >= 0) {
      /* This Key is already there, or the string is old one, so
       * simple take new code as our CrntCode:
       */
      CrntCode = NewCode;
    } else {
      /* Put it in hash table, output the prefix code, and make our
       * CrntCode equal to Pixel.
       */
      if (EGifCompressOutput(GifFile, CrntCode) == GIF_ERROR) {
        _GifError = E_GIF_ERR_DISK_IS_FULL;
        return GIF_ERROR;
      }
      CrntCode = Pixel;

      /* If however the HashTable if full, we send a clear first and
       * Clear the hash table.
       */
      if (GifFile->RunningCode >= LZ_MAX_CODE) {
        /* Time to do some clearance: */
        if (EGifCompressOutput(GifFile, GifFile->ClearCode)
            == GIF_ERROR) {
          _GifError = E_GIF_ERR_DISK_IS_FULL;
          return GIF_ERROR;
        }
        GifFile->RunningCode = GifFile->EOFCode + 1;
        GifFile->RunningBits = GifFile->BitsPerPixel + 1;
        GifFile->MaxCode1 = 1 << GifFile->RunningBits;
        _ClearHashTable(HashTable);
      } else {
        /* Put this unique key with its relative Code in hash table: */
        _InsertHashTable(HashTable, NewKey, GifFile->RunningCode++);
      }
    }

  }

  /* Preserve the current state of the compression algorithm: */
  GifFile->CrntCode = CrntCode;

  if (GifFile->PixelCount == 0) {
    /* We are done - output last Code and flush output buffers: */
    if (EGifCompressOutput(GifFile, CrntCode) == GIF_ERROR) {
      _GifError = E_GIF_ERR_DISK_IS_FULL;
      return GIF_ERROR;
    }
    if (EGifCompressOutput(GifFile, GifFile->EOFCode) == GIF_ERROR) {
      _GifError = E_GIF_ERR_DISK_IS_FULL;
      return GIF_ERROR;
    }
    if (EGifCompressOutput(GifFile, FLUSH_OUTPUT) == GIF_ERROR) {
      _GifError = E_GIF_ERR_DISK_IS_FULL;
      return GIF_ERROR;
    }
  }

  return GIF_OK;
}

/******************************************************************************
 * The LZ compression output routine:
 * This routine is responsible for the compression of the bit stream into
 * 8 bits (bytes) packets.
 * Returns GIF_OK if written succesfully.
 *****************************************************************************/
static int EGifCompressOutput(GifFileType * GifFile, int Code)
{
  int retval = GIF_OK;

  if (Code == FLUSH_OUTPUT)
  {
    while (GifFile->CrntShiftState > 0)
    {
      /* Get Rid of what is left in DWord, and flush it. */
      if (EGifBufferedOutput(GifFile, GifFile->Buf, GifFile->CrntShiftDWord & 0xff) == GIF_ERROR)
        retval = GIF_ERROR;
      GifFile->CrntShiftDWord >>= 8;
      GifFile->CrntShiftState -= 8;
    }
    GifFile->CrntShiftState = 0;    /* For next time. */
    if (EGifBufferedOutput(GifFile, GifFile->Buf, FLUSH_OUTPUT) == GIF_ERROR)
      retval = GIF_ERROR;
  }
  else
  {
    GifFile->CrntShiftDWord |= ((long)Code) << GifFile->CrntShiftState;
    GifFile->CrntShiftState += GifFile->RunningBits;
    while (GifFile->CrntShiftState >= 8)
    {
      /* Dump out full bytes: */
      if (EGifBufferedOutput(GifFile, GifFile->Buf, GifFile->CrntShiftDWord & 0xff) == GIF_ERROR)
          retval = GIF_ERROR;
      GifFile->CrntShiftDWord >>= 8;
      GifFile->CrntShiftState -= 8;
    }
  }

  /* If code cannt fit into RunningBits bits, must raise its size. Note */
  /* however that codes above 4095 are used for special signaling.      */
  if (GifFile->RunningCode >= GifFile->MaxCode1 && Code <= 4095) {
     GifFile->MaxCode1 = 1 << ++GifFile->RunningBits;
  }

  return retval;
}

/******************************************************************************
 * This routines buffers the given characters until 255 characters are ready
 * to be output. If Code is equal to -1 the buffer is flushed (EOF).
 * The buffer is Dumped with first byte as its size, as GIF format requires.
 * Returns GIF_OK if written succesfully.
 *****************************************************************************/
static int EGifBufferedOutput(GifFileType * GifFile, uint8_t * Buf, int c)
{
  if (c == FLUSH_OUTPUT) {
    /* Flush everything out. */
    if (Buf[0] != 0 && WRITE(GifFile, Buf, Buf[0] + 1) != (unsigned)(Buf[0] + 1))
    {
      _GifError = E_GIF_ERR_WRITE_FAILED;
      return GIF_ERROR;
    }
    /* Mark end of compressed data, by an empty block (see GIF doc): */
    Buf[0] = 0;
    if (WRITE(GifFile, Buf, 1) != 1)
    {
      _GifError = E_GIF_ERR_WRITE_FAILED;
      return GIF_ERROR;
    }
  }
  else
  {
    if (Buf[0] == 255) {
      /* Dump out this buffer - it is full: */
      if (WRITE(GifFile, Buf, Buf[0] + 1) != (unsigned)(Buf[0] + 1))
      {
        _GifError = E_GIF_ERR_WRITE_FAILED;
        return GIF_ERROR;
      }
      Buf[0] = 0;
    }
    Buf[++Buf[0]] = c;
  }

  return GIF_OK;
}

/******************************************************************************
 * This routine writes to disk an in-core representation of a GIF previously
 * created by DGifSlurp().
 *****************************************************************************/
static int EGifSpew(GifFileType * GifFileOut)
{
  int i, j, gif89 = false;
  int bOff;   /* Block Offset for adding sub blocks in Extensions */
  char SavedStamp[GIF_STAMP_LEN + 1];

  for (i = 0; i < GifFileOut->ImageCount; i++) {
      for (j = 0; j < GifFileOut->SavedImages[i].ExtensionBlockCount; j++) {
          int function =
             GifFileOut->SavedImages[i].ExtensionBlocks[j].Function;

          if (function == COMMENT_EXT_FUNC_CODE
              || function == GRAPHICS_EXT_FUNC_CODE
              || function == PLAINTEXT_EXT_FUNC_CODE
              || function == APPLICATION_EXT_FUNC_CODE)
              gif89 = true;
      }
  }

  strncpy(SavedStamp, GifVersionPrefix, GIF_STAMP_LEN);
  if (gif89) {
      strncpy(GifVersionPrefix, GIF89_STAMP, GIF_STAMP_LEN);
  } else {
      strncpy(GifVersionPrefix, GIF87_STAMP, GIF_STAMP_LEN);
  }
  if (EGifPutScreenDesc(GifFileOut,
                        GifFileOut->SWidth,
                        GifFileOut->SHeight,
                        GifFileOut->SColorResolution,
                        GifFileOut->SBackGroundColor,
                        GifFileOut->SColorMap) == GIF_ERROR) {
      strncpy(GifVersionPrefix, SavedStamp, GIF_STAMP_LEN);
      return GIF_ERROR;
  }
  strncpy(GifVersionPrefix, SavedStamp, GIF_STAMP_LEN);

  for (i = 0; i < GifFileOut->ImageCount; i++)
  {
    SavedImage *sp = &GifFileOut->SavedImages[i];
    int SavedHeight = sp->ImageDesc.Height;
    int SavedWidth = sp->ImageDesc.Width;
    ExtensionBlock *ep;

    /* this allows us to delete images by nuking their rasters */
    if (sp->RasterBits == NULL)
        continue;

    if (sp->ExtensionBlocks) {
        for (j = 0; j < sp->ExtensionBlockCount; j++) {
            ep = &sp->ExtensionBlocks[j];
            if (j == sp->ExtensionBlockCount - 1 || (ep+1)->Function != 0) {
                /*** FIXME: Must check whether outputting
                 * <ExtLen><Extension> is ever valid or if we should just
                 * drop anything with a 0 for the Function.  (And whether
                 * we should drop here or in EGifPutExtension)
                 */
                if (EGifPutExtension(GifFileOut,
                                     (ep->Function != 0) ? ep->Function : '\0',
                                     ep->ByteCount,
                                     ep->Bytes) == GIF_ERROR) {
                    return (GIF_ERROR);
                }
            } else {
                EGifPutExtensionFirst(GifFileOut, ep->Function, ep->ByteCount, ep->Bytes);
                for (bOff = j+1; bOff < sp->ExtensionBlockCount; bOff++) {
                    ep = &sp->ExtensionBlocks[bOff];
                    if (ep->Function != 0) {
                        break;
                    }
                    EGifPutExtensionNext(GifFileOut, 0,
                            ep->ByteCount, ep->Bytes);
                }
                EGifPutExtensionLast(GifFileOut, 0, 0, NULL);
                j = bOff-1;
            }
        }
    }

    if (EGifPutImageDesc(GifFileOut,
      sp->ImageDesc.Left,
      sp->ImageDesc.Top,
      SavedWidth,
      SavedHeight,
      sp->ImageDesc.Interlace,
      sp->ImageDesc.ColorMap) == GIF_ERROR)
    {
      return GIF_ERROR;
    }

    for (j = 0; j < SavedHeight; j++) {
      if (EGifPutLine(GifFileOut, sp->RasterBits + j * SavedWidth, SavedWidth) == GIF_ERROR)
        return GIF_ERROR;
    }
  }

  if (EGifCloseFile(GifFileOut) == GIF_ERROR) return GIF_ERROR;

  return GIF_OK;
}

namespace Fog {
namespace ImageIO {

// ============================================================================
// [Fog::ImageIO::GifDecoderDevice]
// ============================================================================

struct FOG_HIDDEN GifDecoderDevice : public DecoderDevice
{
  FOG_DECLARE_OBJECT(GifDecoderDevice, DecoderDevice)

  GifDecoderDevice(Provider* provider);
  virtual ~GifDecoderDevice();

  virtual void reset();
  virtual err_t readHeader();
  virtual err_t readImage(Image& image);

private:
  GifFileType* _context;

  bool openGif();
  void closeGif();
};

// ============================================================================
// [Fog::ImageIO::GifEncoderDevice]
// ============================================================================

struct FOG_HIDDEN GifEncoderDevice : public EncoderDevice
{
  FOG_DECLARE_OBJECT(GifEncoderDevice, EncoderDevice)

  GifEncoderDevice(Provider* provider);
  virtual ~GifEncoderDevice();

  virtual err_t writeImage(const Image& image);
};

// ============================================================================
// [Fog::ImageIO::GifProvider]
// ============================================================================

struct GifProvider : public Provider
{
  GifProvider();
  virtual ~GifProvider();

  virtual uint32_t checkSignature(const void* mem, sysuint_t length) const;
  virtual err_t createDevice(uint32_t deviceType, BaseDevice** device) const;
};


GifProvider::GifProvider()
{
  // Name of ImageIO Provider.
  _name = fog_strings->getString(STR_GRAPHICS_GIF);

  // File type.
  _fileType = IMAGE_IO_FILE_GIF;

  // Supported devices.
  _deviceType = IMAGE_IO_DEVICE_DECODER;

  // Supported extensions.
  _imageExtensions.reserve(1);
  _imageExtensions.append(fog_strings->getString(STR_GRAPHICS_gif));
}

GifProvider::~GifProvider()
{
}

uint32_t GifProvider::checkSignature(const void* mem, sysuint_t length) const
{
  if (!mem || length < 3) return 0;

  const uint8_t* m = (const uint8_t*)mem;
  if (memcmp(m, "GIF", 3) != 0) return 0;

  if (length < 6) return 75;
  if (memcmp(m + 3, "87a", 3) != 0) return 0;
  if (memcmp(m + 3, "89a", 3) != 0) return 0;

  return 90;
}

err_t GifProvider::createDevice(uint32_t deviceType, BaseDevice** device) const
{
  BaseDevice* d = NULL;

  switch (deviceType)
  {
    case IMAGE_IO_DEVICE_DECODER:
      d = new(std::nothrow) GifDecoderDevice(const_cast<GifProvider*>(this));
      break;
    case IMAGE_IO_DEVICE_ENCODER:
      return ERR_IMAGEIO_NO_ENCODER;
    default:
      return ERR_RT_INVALID_ARGUMENT;
  }

  if (!d) return ERR_RT_OUT_OF_MEMORY;

  *device = d;
  return ERR_OK;
}

// ============================================================================
// [Fog::ImageIO::GifDecoderDevice]
// ============================================================================

GifDecoderDevice::GifDecoderDevice(Provider* provider) :
  DecoderDevice(provider),
  _context(NULL)
{
}

GifDecoderDevice::~GifDecoderDevice()
{
  closeGif();
}

void GifDecoderDevice::reset()
{
  closeGif();
  DecoderDevice::reset();
}

bool GifDecoderDevice::openGif()
{
  return 
    (_context != NULL) || 
    (getStream().isOpen() && (_context = DGifOpen(&_stream)) != NULL);
}

void GifDecoderDevice::closeGif()
{
  if (_context) DGifCloseFile(_context);
  _context = NULL;
}

err_t GifDecoderDevice::readHeader()
{
  // Don't read header more than once.
  if (isHeaderDone()) return _headerResult;

  // Mark header as done.
  _headerDone = true;

  if (!openGif()) return (_headerResult = ERR_IMAGEIO_INTERNAL_ERROR);

  _width = _context->SWidth;
  _height = _context->SHeight;
  _depth = _context->BitsPerPixel;
  _planes = 1;
  _actualFrame = 0;
  _framesCount = 0xFFFFFFFF;

  _format = PIXEL_FORMAT_I8;

  // Success.
  return (_headerResult = ERR_OK);
}

err_t GifDecoderDevice::readImage(Image& image)
{
  // read gif header
  if (readHeader() != ERR_OK) return getHeaderResult();

  uint32_t *ptr;
  GifRowType *rows = NULL;
  GifRecordType rec;
  ColorMapObject *cmap;
  int i, j, done = 0, bg, r, g, b, w = 0, h = 0;
  int intoffset[] = { 0, 4, 2, 1 };
  int intjump[] = { 8, 8, 4, 2 };
  int transp = -1;
  err_t error = ERR_OK;
  int format = PIXEL_FORMAT_XRGB32;

  do {
    if (DGifGetRecordType(_context, &rec) == GIF_ERROR)
    {
      // ERROR...
      rec = TERMINATE_RECORD_TYPE;
    }
    if ((rec == IMAGE_DESC_RECORD_TYPE) && (!done))
    {
      if (DGifGetImageDesc(_context) == GIF_ERROR)
      {
        // ERROR...
        rec = TERMINATE_RECORD_TYPE;
      }
      w = _context->Image.Width;
      h = _context->Image.Height;
      rows = (GifRowType*)Fog::Memory::calloc(h * sizeof(GifRowType *));
      if (!rows)
      {
        error = ERR_RT_OUT_OF_MEMORY;
        goto end;
      }
      for (i = 0; i < h; i++)
      {
        rows[i] = (GifRowType)Fog::Memory::alloc(w * sizeof(GifPixelType));
        if (!rows[i])
        {
          error = ERR_RT_OUT_OF_MEMORY;
          goto end;
        }
      }
      if (_context->Image.Interlace)
      {
        for (i = 0; i < 4; i++)
        {
          for (j = intoffset[i]; j < h; j += intjump[i])
          {
            DGifGetLine(_context, rows[j], w);
          }
        }
      }
      else
      {
        for (i = 0; i < h; i++)
        {
          DGifGetLine(_context, rows[i], w);
        }
      }
      done = 1;
    }
    else if (rec == EXTENSION_RECORD_TYPE)
    {
      int ext_code;
      uint8_t* ext = NULL;

      DGifGetExtension(_context, &ext_code, &ext);
      while (ext)
      {
        if ((ext_code == 0xf9) && (ext[1] & 1) && (transp < 0))
        {
          transp = (int)ext[4];
        }
        ext = NULL;
        DGifGetExtensionNext(_context, &ext);
      }
    }
  } while (rec != TERMINATE_RECORD_TYPE && !done);

  if (transp >= 0) format = PIXEL_FORMAT_ARGB32;
  if ((error = image.create(w, h, format))) goto end;

  bg = _context->SBackGroundColor;
  cmap = (_context->Image.ColorMap ? _context->Image.ColorMap : _context->SColorMap);

  ptr = (uint32_t*)image.getXFirst();
  for (i = 0; i < h; i++)
  {
    for (j = 0; j < w; j++)
    {
      if (rows[i][j] == transp)
      {
        r = cmap->Colors[bg].Red;
        g = cmap->Colors[bg].Green;
        b = cmap->Colors[bg].Blue;
        *ptr++ = 0x00ffffff & ((r << 16) | (g << 8) | b);
      }
      else
      {
        r = cmap->Colors[rows[i][j]].Red;
        g = cmap->Colors[rows[i][j]].Green;
        b = cmap->Colors[rows[i][j]].Blue;
        *ptr++ = (0xff << 24) | (r << 16) | (g << 8) | b;
      }
    }
  }

end:
  if (rows)
  {
    for (i = 0; i < h; i++)
    {
      if (rows[i]) Fog::Memory::free(rows[i]);
    }
    Fog::Memory::free(rows);
  }
  return error;
}

// ============================================================================
// [Fog::ImageIO::GifEncoderDevice]
// ============================================================================

// TODO
GifEncoderDevice::GifEncoderDevice(Provider* provider) :
  EncoderDevice(provider)
{
}

GifEncoderDevice::~GifEncoderDevice()
{
}

err_t GifEncoderDevice::writeImage(const Image& image)
{
  // TODO
  return 0;
}

} // ImageIO namespace
} // Fog namespace

FOG_IMPLEMENT_OBJECT(Fog::ImageIO::GifDecoderDevice)
FOG_IMPLEMENT_OBJECT(Fog::ImageIO::GifEncoderDevice)

// ============================================================================
// [CAPI]
// ============================================================================

FOG_INIT_DECLARE void fog_imageio_init_gif(void)
{
  using namespace Fog;
  ImageIO::addProvider(IMAGE_IO_DEVICE_DECODER, new(std::nothrow) ImageIO::GifProvider());
}
