// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUTITEM_H
#define _FOG_GUI_LAYOUTITEM_H

// [Dependencies]
#include <Fog/Core/Constants.h>
#include <Fog/Core/Object.h>
#include <Fog/Graphics/Geometry.h>
#include <Fog/Gui/Constants.h>
#include <Fog/Gui/LayoutPolicy.h>
#include <Fog/Gui/LayoutHint.h>
#include <Fog/Gui/Margin.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Forward Declarations]
// ============================================================================

struct Layout;
struct Widget;
struct SpacerItem;

// ============================================================================
// [Fog::LayoutItem - Macros]
// ============================================================================

// TODO: invalidateLayout.
#define FOG_LAYOUT_PROPERTY_CHANGED \
  _layout->_propertydirty = 1;

#define FOG_DECLARE_FLEX_PROPERTY() \
public: \
  FOG_INLINE bool hasFlex() const { return _flex > 0; } \
  FOG_INLINE int getFlex() const { return _flex; } \
  \
  FOG_INLINE void setFlex(int value) \
  { \
    if (value == _flex) return; \
    \
    _flex = value; \
    FOG_LAYOUT_PROPERTY_CHANGED \
  } \
  \
  FOG_INLINE void clear() \
  { \
    _flex = -1; \
    FOG_LAYOUT_PROPERTY_CHANGED \
  } \
private: \
  int _flex;

#define FOG_INIT_FLEX_PROPERTY() \
  _flex = -1;

#define FOG_DECLARE_PERCENT_HEIGHT_PROPERTY() \
public: \
  FOG_INLINE bool hasHeight() const { return _height != -1.f; } \
  FOG_INLINE float getHeight() const { return _height; } \
  \
  FOG_INLINE void setHeight(float value) \
  { \
    if (value == _height) return; \
    _height = ::Fog::Math::bound<float>(value, 0.0f, 1.0f); \
    \
    FOG_LAYOUT_PROPERTY_CHANGED \
  } \
  \
  FOG_INLINE void clearHeight() \
  { \
    if (_height == -1.0f) return; \
    _height = -1.0f; \
    \
    FOG_LAYOUT_PROPERTY_CHANGED \
  } \
private: \
  float _height;

#define FOG_INIT_PERCENT_HEIGHT_PROPERTY() \
  _height = 1.f;

#define FOG_DECLARE_PERCENT_WIDTH_PROPERTY()\
public: \
  FOG_INLINE bool hasWidth() const { return _width != -1.f; } \
  FOG_INLINE float getWidth() const { return _width; } \
  \
  FOG_INLINE void setWidth(float value) \
  { \
    if (value == _width) return; \
    _width = ::Fog::Math::bound<float>(value, 0.0f, 1.0f); \
    \
    FOG_LAYOUT_PROPERTY_CHANGED \
  } \
  \
  FOG_INLINE void clearWidth() \
  { \
    if (_width == -1.0f) return; \
    _width = -1.0f; \
    \
    FOG_LAYOUT_PROPERTY_CHANGED \
  } \
private: \
  float _width;

#define FOG_INIT_PERCENT_WIDTH_PROPERTY() \
  _width = 1.f;
       
#define FOG_DECLARE_PERCENT_SIZE_PROPERTY() \
  FOG_DECLARE_PERCENT_HEIGHT_PROPERTY() \
  FOG_DECLARE_PERCENT_WIDTH_PROPERTY()

#define FOG_INIT_PERCENT_SIZE_PROPERTY() \
  FOG_INIT_PERCENT_HEIGHT_PROPERTY() \
  FOG_INIT_PERCENT_WIDTH_PROPERTY()

// ============================================================================
// [Fog::LayoutItem]
// ============================================================================

//! @brief Base class used by @ref Widget, @ref Layout and @ref Spacer classes.
//!
//! Purpose of this class is to define interface shared between widget and
//! layout containers.
struct FOG_API LayoutItem : public Object
{
  FOG_DECLARE_OBJECT(LayoutItem, Object)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  LayoutItem(uint32_t alignment = 0);
  virtual ~LayoutItem();

  // --------------------------------------------------------------------------
  // [ContentMargins getter]
  // --------------------------------------------------------------------------

  FOG_INLINE Margin getContentMargin() const { return _contentmargin; }
  FOG_INLINE int getContentLeftMargin() const { return _contentmargin.left; }
  FOG_INLINE int getContentRightMargin() const { return _contentmargin.right; }
  FOG_INLINE int getContentTopMargin() const { return _contentmargin.top; }
  FOG_INLINE int getContentBottomMargin() const { return _contentmargin.bottom; }

  FOG_INLINE int getContentXMargins() const { return _contentmargin.left + _contentmargin.right; }
  FOG_INLINE int getContentYMargins() const { return _contentmargin.top + _contentmargin.bottom; }

  // --------------------------------------------------------------------------
  // [ContentMargins setter]
  // --------------------------------------------------------------------------

  //! @brief This method allows to write a margin filter.
  virtual int calcMargin(int margin, uint32_t location) const;

  // LAYOUT TODO: only invalidate if margin really changes after calcMargin
  void setContentMargin(const Margin& m);
  void setContentMargin(int left, int right, int top, int bottom);

  void setContentTopMargin(int m);
  void setContentRightMargin(int m);
  void setContentBottomMargin(int m);
  void setContentLeftMargin(int m);

  // --------------------------------------------------------------------------
  // [Height For Width]
  // --------------------------------------------------------------------------

  virtual bool hasLayoutHeightForWidth() const;
  virtual int getLayoutHeightForWidth(int width) const;
  virtual int getLayoutMinimumHeightForWidth(int width) const;

  // --------------------------------------------------------------------------
  // [LayoutHint]
  // --------------------------------------------------------------------------

  //! @brief Mark this layout for a need to repaint
  virtual void updateLayout();

  virtual void calculateLayoutHint(LayoutHint& hint) = 0;

  // LAYOUT TODO: maybe we can insert clearDirty() in special function, so that
  // it is only checked there, to have a real const getter!
  void clearDirty();
  const LayoutHint& getLayoutHint() const;

  FOG_INLINE const IntSize& getLayoutSizeHint() const { return getLayoutHint()._sizeHint; }
  FOG_INLINE const IntSize& getLayoutMinimumSize() const { return getLayoutHint()._minimumSize; }
  FOG_INLINE const IntSize& getLayoutMaximumSize() const { return getLayoutHint()._maximumSize; }

  // --------------------------------------------------------------------------
  // [Calculate SizeHint]
  // --------------------------------------------------------------------------

  IntSize calculateMinimumSize() const;
  IntSize calculateMaximumSize() const;

  // --------------------------------------------------------------------------
  // [Expanding Directions]
  // --------------------------------------------------------------------------

  virtual uint32_t getLayoutExpandingDirections() const = 0;

  // --------------------------------------------------------------------------
  // [Visibility]
  // --------------------------------------------------------------------------

  virtual bool isEmpty() const = 0;  

  // --------------------------------------------------------------------------
  // [Cache Handling]
  // --------------------------------------------------------------------------

  //virtual void invalidateLayout() { _dirty = 1; }

  // --------------------------------------------------------------------------
  // [Geometry]
  // --------------------------------------------------------------------------

  virtual void setLayoutGeometry(const IntRect&) = 0;
  // LAYOUT TODO: Move to .cpp
  virtual IntRect getLayoutGeometry() const { return IntRect(); }

  // --------------------------------------------------------------------------
  // [Alignment]
  // --------------------------------------------------------------------------

  FOG_INLINE uint32_t getLayoutAlignment() const { return _alignment; }
  // LAYOUT TODO: Update after set?
  void setLayoutAlignment(uint32_t a) { _alignment = a; }

  // --------------------------------------------------------------------------
  // [Flex variable support]
  // --------------------------------------------------------------------------
  
  void removeLayoutStruct();

  // --------------------------------------------------------------------------
  // [Members]
  // -------------------------------------------------------------------------- 

  template<typename T>
  FOG_INLINE typename T::PropertyType* getLayoutProperties()
  {
    if (!_layoutdata) return NULL;
    typename T::LayoutData* prop = ((typename T::LayoutData*)_layoutdata);
    return (typename T::PropertyType*)(&prop->_user);
  }

  template<typename T>
  FOG_INLINE T* getLayoutData()
  {
    return static_cast<T*>(_layoutdata);
  }

  // Struct for Calculation of Flex-Values in LayoutManager.
  struct LayoutData
  {
  };

  struct FlexLayoutData : public LayoutData
  {
    FlexLayoutData() : 
      _min(0),
      _max(0),
      _hint(0),
      _flex(-1),
      _potential(0),
      _offset(0),
      _next(NULL)
    {
    }

    //! @brief Minimum width or height.
    int _min;
    //! @brief Maximum width or height.
    int _max;
    //! @brief Width or height hint.
    int _hint;

    // Request/Response

    //! @brief the flex value for this Item
    float _flex;

    //! @internal
    int _potential;
    //! @internal
    int _offset;

    //! @brief For faster Flexible handling (could be different type).
    void* _next;

    FOG_INLINE void init(bool grow)
    {
      _offset = 0;
      _potential = grow ?_max - _hint : _hint - _min;
      _flex = grow ? _flex : (1 / _flex);
    }

    FOG_INLINE void prepare() { _potential = -INT_MAX; }
  };  

  LayoutData* _layoutdata;

  //! @brief Content margin.
  Margin _contentmargin;
  //! @brief Layout hint cache.
  LayoutHint _cache;
  //! @brief For fast identification of Layout, where this Item is inserted.
  //! 
  //! Maybe also used for layout pointer in widget.
  Layout* _withinLayout; 
  
  uint32_t _alignment : 2;
  //! @brief Layout hint is dirty -> call calculateLayoutHint() next time.
  uint32_t _dirty : 1;
  //! @brief Property values changes -> calculate.
  uint32_t _propertydirty : 1;
  //! @brief Reserved for future use.
  uint32_t _unused : 28;
};

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUTITEM_H
