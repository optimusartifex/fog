// [Fog-Graphics Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GRAPHICS_CONVOLUTIONMATRIX_H
#define _FOG_GRAPHICS_CONVOLUTIONMATRIX_H

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Atomic.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/Static.h>
#include <Fog/Graphics/Constants.h>

//! @addtogroup Fog_Graphics
//! @{

namespace Fog {

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct IntRect;

// ============================================================================
// [Fog::ConvolutionMatrixI]
// ============================================================================

struct FOG_API ConvolutionMatrixI
{
  // --------------------------------------------------------------------------
  // [Value Type]
  // --------------------------------------------------------------------------

  typedef int ValueType;

  // --------------------------------------------------------------------------
  // [Data]
  // --------------------------------------------------------------------------

  struct FOG_API Data
  {
    // [Ref / Deref]

    FOG_INLINE Data* ref() const
    {
      refCount.inc();
      return const_cast<Data*>(this);
    }

    FOG_INLINE void deref()
    {
      if (refCount.deref()) Memory::free(this);
    }

    // [Statics]

    static Data* alloc(int w, int h);
    static void copy(Data* dst, int dstX, int dstY, Data* src, int srcX, int srcY, int w, int h);

    // [Members]

    mutable Atomic<sysuint_t> refCount;
    int width;
    int height;
    ValueType m[1];
  };

  static Static<Data> sharedNull;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  ConvolutionMatrixI();
  ConvolutionMatrixI(const ConvolutionMatrixI& other);

  FOG_INLINE explicit ConvolutionMatrixI(Data* d) : _d(d) {}

  ~ConvolutionMatrixI();

  static ConvolutionMatrixI fromData(int w, int h, const ValueType* data);

  // --------------------------------------------------------------------------
  // [Implicit Sharing]
  // --------------------------------------------------------------------------

  FOG_INLINE sysuint_t refCount() const { return _d->refCount.get(); }
  FOG_INLINE bool isDetached() const { return _d->refCount.get() == 1; }
  FOG_INLINE err_t detach() { return _d->refCount.get() == 1 ? ERR_OK : _detach(); }

  err_t _detach();

  // --------------------------------------------------------------------------
  // [Flags]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isEmpty() const { return _d->width == 0; }
  FOG_INLINE bool isNull() const { return _d == sharedNull.instancep(); }

  // --------------------------------------------------------------------------
  // [Width / Height]
  // --------------------------------------------------------------------------

  FOG_INLINE int getWidth() const { return _d->width; }
  FOG_INLINE int getHeight() const { return _d->height; }

  // --------------------------------------------------------------------------
  // [Manipulation]
  // --------------------------------------------------------------------------

  FOG_INLINE const ValueType* getData() const { return _d->m; }
  FOG_INLINE ValueType* getMData() { return detach() ? NULL : _d->m; }

  FOG_INLINE ValueType* getXData()
  {
    FOG_ASSERT_X(isDetached(), "Fog::ConvolutionMatrixI::getXData() - Not detached.");
    return _d->m;
  }

  err_t setData(int width, int height, const ValueType* data);

  err_t create(int width, int height);
  err_t extend(int width, int height, ValueType value = 0);

  int getCell(int x, int y) const;
  err_t setCell(int x, int y, int val);

  err_t fill(const IntRect& rect, ValueType value);

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  ConvolutionMatrixI& operator=(const ConvolutionMatrixI& other);

  ValueType* operator[](int y);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  FOG_DECLARE_D(Data)
};

// ============================================================================
// [Fog::ConvolutionMatrix<>]
// ============================================================================

template<typename T>
struct ConvolutionMatrix {};

template<>
struct ConvolutionMatrix<int> : public ConvolutionMatrixI
{
  FOG_INLINE ConvolutionMatrix() :
    ConvolutionMatrixI()
  {
  }

  FOG_INLINE ConvolutionMatrix(const ConvolutionMatrixI& other) :
    ConvolutionMatrixI(other)
  {
  }

  FOG_INLINE ~ConvolutionMatrix()
  {
  }

  FOG_INLINE ConvolutionMatrix& operator=(const ConvolutionMatrix& other)
  {
    // Cast back to ConvolutionMatrix<int>.
    return reinterpret_cast<ConvolutionMatrix&>(ConvolutionMatrixI::operator=(other));
  }
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_GRAPHICS_CONVOLUTIONMATRIX_H
