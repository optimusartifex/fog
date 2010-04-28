#include <Fog/Core.h>
#include <Fog/Graphics.h>
#include <Fog/Gui.h>
#include <Fog/Svg.h>
#include <Fog/Xml.h>

// This is for MY testing:)

//#include <Windows.h>
#include <uxtheme.h>
#include <Tmschema.h>

using namespace Fog;

struct MyPopUp : public Widget {
  MyPopUp() : Widget(WINDOW_INLINE_POPUP) {

  }

  virtual void onPaint(PaintEvent* e)
  {
    Painter* p = e->getPainter();
    p->setOperator(OPERATOR_SRC_OVER);

    // Clear everything to white.
    p->setSource(Argb(0xAAFFFFFFF));
    p->clear();

    p->setSource(Argb(0xFF000000));
    p->drawText(IntPoint(0,0),Ascii8("TEXT TEXT"),getFont());
  }
};

struct UxTheme {
  UxTheme() : _theme(0) {
    if(!_dll) {
      _dll = LoadLibrary(L"UxTheme.dll");
    }

    _pOpenThemeData = (PFNOPENTHEMEDATA)getProc("OpenThemeData");
    _pCloseThemeData = (PFNCLOSETHEMEDATA)getProc("CloseThemeData");
    _pDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)getProc("DrawThemeBackground");
    _pDrawThemeText = (PFNDRAWTHEMETEXT)getProc("DrawThemeText");
    _pGetThemeRegion = (PFNGETTHEMEBACKGROUNDREGION) getProc("GetThemeBackgroundRegion");
    _pIsThemeBackgroundPartiallyTransparent = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT) getProc("IsThemeBackgroundPartiallyTransparent");
  }

  ~UxTheme() {
    close();
  }

  void openTheme(LPCWSTR cls) {
    _theme = _pOpenThemeData(0,cls);
  }

  void close() {
    _pCloseThemeData(_theme);
    _theme = 0;
  }

  BOOL drawThemeBackground(HDC hdc, int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect) {
    return _pDrawThemeBackground(_theme,hdc,iPartId,iStateId,pRect,pClipRect) == S_OK;
  }

  BOOL drawThemeText(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect) {
      return (*_pDrawThemeText)(_theme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect) == S_OK;
  }

  BOOL getThemeBackgroundRegion(HDC hdc, int iPartId, int iStateId, const RECT *pRect,  HRGN *pRegion) {
    return (*_pGetThemeRegion)(_theme, hdc, iPartId, iStateId, pRect, pRegion) == S_OK;
  }

  BOOL IsThemeBackgroundPartiallyTransparent(int iPartId, int iStateId) {
    return (*_pIsThemeBackgroundPartiallyTransparent)(_theme, iPartId, iStateId);
  }

  static bool EnableTheming(bool b) {
    if(!_dll) {
      _dll = LoadLibrary(L"UxTheme.dll");
    }
    typedef HRESULT (__stdcall *PFNENABLETHEMING)(BOOL fEnable);
    PFNENABLETHEMING pfn = (PFNENABLETHEMING)getProc("EnableTheming");
    return (*pfn)(b) == S_OK;
  }

  static bool isAppThemed() {
    if(!_dll) {
      _dll = LoadLibrary(L"UxTheme.dll");
    }
    typedef HRESULT (__stdcall *PFNENABLETHEMING)();
    PFNENABLETHEMING pfn = (PFNENABLETHEMING)getProc("IsAppThemed");
    return (*pfn)();
  }

  


private:

  static void* getProc(LPCSTR szProc) {
    if (_dll != NULL)
      return GetProcAddress(_dll, szProc);

    return 0;
  }

  typedef HTHEME(__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
  typedef HRESULT(__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
  typedef HRESULT(__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect,  const RECT *pClipRect);
  typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect);
  typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDREGION)(HTHEME hTheme,  HDC hdc, int iPartId, int iStateId, const RECT *pRect,  HRGN *pRegion);
  typedef BOOL (__stdcall *PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME hTheme, int iPartId, int iStateId);


  PFNOPENTHEMEDATA _pOpenThemeData;
  PFNCLOSETHEMEDATA _pCloseThemeData;
  PFNDRAWTHEMEBACKGROUND _pDrawThemeBackground;
  PFNDRAWTHEMETEXT _pDrawThemeText;
  PFNGETTHEMEBACKGROUNDREGION _pGetThemeRegion;
  PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT _pIsThemeBackgroundPartiallyTransparent;

  static HMODULE _dll;
  HTHEME _theme;
};

HMODULE UxTheme::_dll = 0;

//TABP_TABITEM
//BP_PUSHBUTTON

struct XPButton : public ButtonBase {
  XPButton() : _hBitmap(0), _hdc(0), _default(false), TYPE(TABP_TABITEM), _rgn(0), isbutton(true) {
    _theme.openTheme(L"Button");
  }

  void setDefault(bool def) {
    _default = def;
  }

  virtual void onConfigure(ConfigureEvent* e) {
    int width = _geometry.getWidth();
    int height = _geometry.getHeight();
  }

  FOG_INLINE int calcState() const {
    int state = 0;

    if(isbutton) {
      state = isMouseOver() ? PBS_HOT : (_default ? PBS_DEFAULTED : PBS_NORMAL);
      state = isMouseDown() ? PBS_PRESSED : state;
      state = !isEnabled() ? PBS_DISABLED : state;
    } else {
      state = isMouseOver() ? TIS_HOT : TIS_NORMAL;
      state = isMouseDown() ? TIS_SELECTED : state;
      state = !isEnabled() ? TIS_DISABLED : state;
    }

    return state;
  }

  void createBitmap(int width, int height) {
    int targetBPP = 32;
    // Define bitmap attributes.
    BITMAPINFO bmi;

    if(_hdc)
      DeleteDC(_hdc);

    if(_hBitmap)
      DeleteObject(_hBitmap);

    _hdc = CreateCompatibleDC(NULL);
    if (_hdc == NULL) {
      return;
    }

    //Fog::Memory::zero();
    Fog::Memory::zero(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = targetBPP;
    bmi.bmiHeader.biCompression = BI_RGB;

    unsigned char* pixels = NULL;

    _hBitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
    if (_hBitmap == NULL) 
    {
      DeleteDC(_hdc);
      return;
    }

    // Select bitmap to DC.
    SelectObject(_hdc, _hBitmap);

    DIBSECTION info;
    GetObjectW(_hBitmap, sizeof(DIBSECTION), &info);

    _buffer.data = pixels;
    _buffer.width = width;
    _buffer.height = height;
    _buffer.stride = info.dsBm.bmWidthBytes;

    if(_theme.IsThemeBackgroundPartiallyTransparent(TYPE,calcState())) {
      _buffer.format = PIXEL_FORMAT_PRGB32;
    } else {
      _buffer.format = PIXEL_FORMAT_XRGB32;
    }

    _image.adopt(_buffer);
  }

  virtual void onPaint(PaintEvent* e) {    
    IntRect r = getClientContentGeometry(); //for easy margin support
    RECT rect;
    rect.left = r.x;
    rect.right = r.getWidth();
    rect.top = r.y;
    rect.bottom = r.getHeight();

    createBitmap(r.getWidth(), r.getHeight());

    int state = calcState();

    if(_theme.IsThemeBackgroundPartiallyTransparent(TYPE,state)) {
      //button
      //_image.clear(Argb(0x00000000));
      //_theme.getThemeBackgroundRegion(_hdc,TYPE,state,&rect,&_rgn);
      //SelectClipRgn(_hdc, _rgn);
    } else {
      //tab
      _image.clear(Argb(0xFFFFFFFF));
    }    

    //Clip on Region to support Transparency! ... needed?

    BOOL result = _theme.drawThemeBackground(_hdc, TYPE,	state, &rect, NULL);

    //draws with a really ugly font... why?? (also if I set the font within window -> maybe because I open them with null hwnd)
    //BOOL ret = _theme.drawThemeText(_hdc,BP_PUSHBUTTON,state,(wchar_t*)_text.getData(),_text.getLength(),DT_CENTER|DT_VCENTER|DT_SINGLELINE,0,&rect);     
    e->getPainter()->blitImage(IntPoint(0,0),_image);
    e->getPainter()->drawText(r, _text, _font, TEXT_ALIGN_CENTER);
  }


private:
  UxTheme _theme;
  HBITMAP _hBitmap;
  HRGN _rgn;
  HDC _hdc;

  ImageBuffer _buffer;

  Image _image;
  bool _default;

  const int TYPE;

  const int isbutton;
};


struct MyModalWindow : public Window {
  MyModalWindow(int x) : Window(WINDOW_TYPE_DEFAULT), _x(x) {
    Button* button5 = new Button();
    add(button5);
    button5->setGeometry(IntRect(40, 200, 100, 20));
    button5->setText(Ascii8("Test ModalWindow"));
    button5->show();  
    button5->addListener(EVENT_CLICK, this, &MyModalWindow::onModalTestClick);
  }

  void onModalTestClick(MouseEvent* e) {
    _modalwindow = new MyModalWindow(_x+1);
    _modalwindow->setSize(IntSize(200,300));
    _modalwindow->addListener(EVENT_CLOSE, this, &MyModalWindow::onModalClose);
    _modalwindow->showModal(getGuiWindow());
  }

  void onModalClose() {
    _modalwindow->hide();
    //_modalwindow->destroy();
    _modalwindow = 0;
  }

  virtual void onPaint(PaintEvent* e)
  {
    Painter* p = e->getPainter();
    p->setOperator(OPERATOR_SRC);
    // Clear everything to white.
    p->setSource(Argb(0xFF000000));
    p->clear();

    p->setSource(Argb(0xFFFFFFFF));
    String s;
    s.format("MODAL DIALOG NO: %i", _x);
    p->drawText(IntPoint(0,0),s,getFont());
  }

  int _x;
  MyModalWindow* _modalwindow;
};

struct MyWindow : public Window
{
  // [Fog Object System]
  FOG_DECLARE_OBJECT(MyWindow, Window)

  // [Construction / Destruction]
  MyWindow(uint32_t createFlags = 0);
  virtual ~MyWindow();

  // [Event Handlers]
  virtual void onPaint(PaintEvent* e);

  void createButtons(int count) {
    _buttons.clear();
    _buttons.reserve(count);

    for(int i=0;i<count;++i) {
      XPButton* buttonx1 = new XPButton();
      add(buttonx1);
      //button4->setGeometry(IntRect(40, 160, 100, 20));
      String str;
      str.format("XButton%i", i);
      buttonx1->setText(str);
      buttonx1->show(); 

      buttonx1->setMinimumSize(IntSize(40,40));
      _buttons.append(buttonx1);
    }

  }

  void testBorderLayout(Layout* parent=0) {
    createButtons(6);
    BorderLayout* layout;
    if(parent) {
      layout = new BorderLayout();      
    } else {
      layout = new BorderLayout(this);
    }
    
    layout->addItem(_buttons.at(0), BorderLayout::SOUTH);
    layout->addItem(_buttons.at(1), BorderLayout::WEST);
    layout->addItem(_buttons.at(2), BorderLayout::CENTER);
    layout->addItem(_buttons.at(3), BorderLayout::EAST);
    layout->addItem(_buttons.at(4), BorderLayout::NORTH);
    layout->addItem(_buttons.at(5), BorderLayout::NORTH);
  }

  void testGridLayout(Layout* parent=0) {
    createButtons(8);

    GridLayout* layout;
    if(parent) {
      layout = new GridLayout();      
    } else {
      layout = new GridLayout(this);
    }

    layout->addItem(_buttons.at(0),0,0);
    layout->addItem(_buttons.at(1),0,1);
    layout->addItem(_buttons.at(2),0,2);
    layout->addItem(_buttons.at(3),1,0,1,3);
    layout->addItem(_buttons.at(4),2,0,2);
    layout->addItem(_buttons.at(5),2,1);
    layout->addItem(_buttons.at(6),3,1);
    layout->addItem(_buttons.at(6),3,2);

    layout->setRowFlex(2, 1);
    layout->setColumnFlex(2, 1);
  }

  void testVBoxLayout(Layout* parent = 0) {
    const int COUNT = 8;
    createButtons(COUNT);

    VBoxLayout* layout;
    if(parent) {
      layout = new VBoxLayout(0,0);      
    } else {
      layout = new VBoxLayout(this,0,0);
    }

    for(int i=0;i<COUNT;++i) {
      layout->addItem(_buttons.at(i));
    }

    _buttons.at(3)->setFlex(1);
    _buttons.at(2)->setFlex(2);
  }

  void testHBoxLayout(Layout* parent = 0) {
    const int COUNT = 8;
    createButtons(COUNT);
    HBoxLayout* layout;
    if(parent) {
      layout = new HBoxLayout(0,0);      
    } else {
      layout = new HBoxLayout(this,0,0);
    }

    for(int i=0;i<COUNT;++i) {
      layout->addItem(_buttons.at(i));
    }

    _buttons.at(3)->setFlex(1);
    _buttons.at(2)->setFlex(2);
  }

  void testNestedLayout() {

  }

  void onModalClose() {
    _modalwindow->hide();
    _modalwindow->destroy();
    _modalwindow = 0;
  }

  virtual void onClose(CloseEvent* e) {
    this->destroyWindow();
  }

  void onModalTestClick(MouseEvent* e) {
    _modalwindow = new MyModalWindow(_mcount);
    _modalwindow->setSize(IntSize(200,300));
    _modalwindow->addListener(EVENT_CLOSE, this, &MyWindow::onModalClose);
    _modalwindow->showModal(getGuiWindow());
  }

  void onPopUpClick(MouseEvent* e) {
    if(_popup->getVisibility() < WIDGET_VISIBLE) {
      _popup->show();
    }
  }

  void onFrameTestClick(MouseEvent* e) {
    if(getWindowFlags() & WINDOW_NATIVE) {
      setWindowFlags(WINDOW_TYPE_TOOL|WINDOW_TRANSPARENT);
    } else {
      setWindowFlags(WINDOW_TYPE_DEFAULT|WINDOW_TRANSPARENT);
    }
  }

  void onFullScreenClick(MouseEvent* e) {
    if(isFullScreen()) {
      show();
    } else {      
      show(WIDGET_VISIBLE_FULLSCREEN);
    }
  }

  void onTransparencyClick(MouseEvent* e) {
    WidgetOpacityAnimation* anim = new WidgetOpacityAnimation(this);
    anim->setDuration(TimeDelta::fromMilliseconds(1000));

    anim->setStartOpacity(0.0f);
    anim->setEndOpacity(1.0f);
    
    if(getTransparency() == 1.0) {
      anim->setDirection(ANIMATION_BACKWARD);
      anim->setStartOpacity(0.1f);
    }

    Application::getInstance()->getAnimationDispatcher()->addAnimation(anim);
  }

  void paintImage(Painter* painter, const IntPoint& pos, const Image& im, const String& name);

  Image i[2];
  double _subx;
  double _suby;
  double _rotate;
  double _shearX;
  double _shearY;
  double _scale;
  int _spread;

  Widget* _popup;
  int _mcount;
  MyModalWindow* _modalwindow;


  List<Widget*> _buttons;

  //FlowLayout* _layout;
};

FOG_IMPLEMENT_OBJECT(MyWindow)

MyWindow::MyWindow(uint32_t createFlags) :
  Window(createFlags)
{
  setWindowTitle(Ascii8("Testing..."));

  i[0].readFile(Ascii8("babelfish.png"));
  i[1].readFile(Ascii8("kweather.png"));

  i[0].convert(PIXEL_FORMAT_ARGB32);
  i[1].convert(PIXEL_FORMAT_ARGB32);

  _mcount = 0;

  _subx = 0.0;
  _suby = 0.0;
  _rotate = 0.0;
  _shearX = 0.0;
  _shearY = 0.0;
  _scale = 1.0;
  _spread = SPREAD_REPEAT;

  testGridLayout();

  setContentRightMargin(0);
}

MyWindow::~MyWindow()
{
  //Yeah I know no child widget destructurs... 
}

void MyWindow::onPaint(PaintEvent* e)
{
  uint32_t vis = getVisibility();

  if(vis == WIDGET_VISIBLE) {
    setWindowTitle(Ascii8("STATE: VISIBLE"));
  } else if(vis == WIDGET_VISIBLE_MAXIMIZED){
    setWindowTitle(Ascii8("STATE: MAXIMIZED"));
  } else if(vis == WIDGET_VISIBLE_MINIMIZED){
    setWindowTitle(Ascii8("STATE: MINIMIZED"));
  } else if(vis == WIDGET_HIDDEN){
    setWindowTitle(Ascii8("STATE: HIDDEN"));
  } else if(vis == WIDGET_VISIBLE_RESTORE){
    setWindowTitle(Ascii8("STATE: RESTORED"));
  }

  TimeTicks ticks = TimeTicks::highResNow();
  Painter* p = e->getPainter();

  p->save();
  p->setOperator(OPERATOR_SRC);

  // Clear everything to white.
  p->setSource(Argb(0xFFFFFFFF));
  p->clear();
}

// ============================================================================
// [MAIN]
// ============================================================================

FOG_GUI_MAIN()
{
  Application app(Ascii8("Gui"));

  BOOL b = UxTheme::EnableTheming(TRUE);
  BOOL ret = UxTheme::isAppThemed();

  MyWindow window(WINDOW_TYPE_DEFAULT);
  window.setSize(IntSize(500, 400));
  window.show();

  app.addListener(EVENT_LAST_WINDOW_CLOSED, &app, &Application::quit);

    

  return app.run();
}
