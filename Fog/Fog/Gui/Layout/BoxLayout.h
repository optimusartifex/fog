// [Fog-Gui]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_GUI_LAYOUT_BOXLAYOUT_H
#define _FOG_GUI_LAYOUT_BOXLAYOUT_H

// [Dependencies]
#include <Fog/Gui/Layout/Layout.h>

namespace Fog {

//! @addtogroup Fog_Gui_Layout
//! @{

// ============================================================================
// [Fog::BoxLayout]
// ============================================================================

//! @brief Base class used by @ref HBoxLayout and @ref VBoxLayout.
struct FOG_API BoxLayout : public Layout
{
  FOG_DECLARE_OBJECT(BoxLayout, Layout)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  BoxLayout(Widget *parent, int margin = -1, int spacing = -1);
  BoxLayout(int margin = -1, int spacing = -1);
  virtual ~BoxLayout();

  // --------------------------------------------------------------------------
  // [Property / Data]
  // --------------------------------------------------------------------------

  struct LayoutProperty
  {
    LayoutProperty(Layout* layout) :
      _layout(layout),
      _nextpercent(0)
    {
      FOG_INIT_FLEX_PROPERTY();
      FOG_INIT_PERCENT_SIZE_PROPERTY();
    }

    Layout* _layout;
    LayoutItem* _nextpercent;

    FOG_DECLARE_FLEX_PROPERTY()
    FOG_DECLARE_PERCENT_SIZE_PROPERTY()
  };

  struct LayoutData : LayoutItem::FlexLayoutData
  {
    LayoutData(Layout* layout) : _user(layout) {}

    FOG_INLINE bool hasFlex() const { return _user.hasFlex(); }
    FOG_INLINE int getFlex() const { return _user.getFlex(); }
    FOG_INLINE void setFlex(int flex) { return _user.setFlex(flex); }

    LayoutProperty _user;
  };
  typedef LayoutProperty PropertyType;

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  void addItem(LayoutItem *item, int flex = -1);
  virtual uint32_t getLayoutExpandingDirections() const;

  FOG_INLINE uint32_t getDirection() const { return _direction; }   
  FOG_INLINE void setDirection(uint32_t d) { _direction = d; }

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

protected:
  FOG_INLINE bool isForward() const { return _direction == 0; }

  virtual void setLayoutGeometry(const RectI &rect);    
  virtual int doLayout(const RectI &rect) = 0;

  int calculateHorizontalGaps(bool collapse = true);
  int calculateVerticalGaps(bool collapse = true);
  
  LayoutItem* _flexibles;
  uint32_t _allocated : 31;

private:
  uint32_t _direction : 1;
};

// ============================================================================
// [Fog::HBoxLayout]
// ============================================================================

//! @brief Horizontal box layout.
struct FOG_API HBoxLayout : public BoxLayout
{
  FOG_DECLARE_OBJECT(HBoxLayout, BoxLayout)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  HBoxLayout(Widget *parent, int margin = -1, int spacing = -1);
  HBoxLayout(int margin = -1, int spacing = -1);
  virtual ~HBoxLayout();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  virtual int doLayout(const RectI &rect);
  virtual void calculateLayoutHint(LayoutHint& hint);
};

// ============================================================================
// [Fog::VBoxLayout]
// ============================================================================

//! @brief Vertical box layout.
struct FOG_API VBoxLayout : public BoxLayout
{
  FOG_DECLARE_OBJECT(VBoxLayout, BoxLayout)

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  VBoxLayout(Widget *parent, int margin = -1, int spacing = -1);
  VBoxLayout(int margin = -1, int spacing = -1);
  virtual ~VBoxLayout();

  // --------------------------------------------------------------------------
  // [Implementation]
  // --------------------------------------------------------------------------

  // LAYOUT TODO: Was private!
  virtual int doLayout(const RectI &rect);
  virtual void calculateLayoutHint(LayoutHint& hint);
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_GUI_LAYOUT_BOXLAYOUT_H
