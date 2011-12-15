// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_GLOBAL_ENUMUI_H
#define _FOG_CORE_GLOBAL_ENUMUI_H

// [Dependencies]
#include <Fog/Core/Global/EnumCore.h>
#include <Fog/Core/Global/EnumG2d.h>

namespace Fog {

//! @addtogroup Fog_Core_Global
//! @{

// NOTE: All FB_ constants are specific to the frame-buffer implementation and
// used mainly by FbEngine and FbWindow itself. The standard UI code should use
// constants without FB_ prefix unless working directly with FbEngine/FbWindow.

// ============================================================================
// [Fog::FB_DOUBLE_BUFFER]
// ============================================================================

//! @brief FbWindow double-buffer type.
enum FB_DOUBLE_BUFFER
{
  //! @brief No double-buffer yet.
  FB_DOUBLE_BUFFER_NONE = 0,

  //! @brief Double-buffer is using DIBSECTION (Windows).
  FB_DOUBLE_BUFFER_WIN_DIBSECTION = 1,

  //! @brief Double-buffer is using CGImageRef (Mac).
  FB_DOUBLE_BUFFER_MAC_CGIMAGE  = 2,

  //! @brief Double-buffer is using XShm-Pixmap (X11).
  FB_DOUBLE_BUFFER_X11_XSHM_PIXMAP = 3,

  //! @brief Double-buffer is using XShm-Image (X11).
  //!
  //! @note Used in case that it failed to create XShm-Pixmap.
  FB_DOUBLE_BUFFER_X11_XSHM_IMAGE = 4
};

// ============================================================================
// [Fog::FB_EVENT]
// ============================================================================

//! @brief FbWindow event code.
enum FB_EVENT
{
  FB_EVENT_CREATE = 0,
  FB_EVENT_DESTROY = 1,

  FB_EVENT_CLOSE = 2,

  FB_EVENT_SHOW = 3,
  FB_EVENT_HIDE = 4,
  FB_EVENT_HIDE_BY_PARENT = 5,

  FB_EVENT_GEOMETRY = 6,
  FB_EVENT_MAXIMIZE = 7,
  FB_EVENT_MINIMIZE = 8,
  FB_EVENT_RESTORE = 9,
  
  FB_EVENT_FOCUS_IN = 10,
  FB_EVENT_FOCUS_OUT = 11,

  FB_EVENT_KEY_DOWN = 12,
  FB_EVENT_KEY_UP = 13,
  FB_EVENT_KEY_PRESS = 14,

  FB_EVENT_MOUSE_IN = 15,
  FB_EVENT_MOUSE_OUT = 16,
  FB_EVENT_MOUSE_MOVE = 17,

  FB_EVENT_MOUSE_DOWN = 18,
  FB_EVENT_MOUSE_UP = 19,

  FB_EVENT_MOUSE_CLICK = 20,
  FB_EVENT_MOUSE_DBL_CLICK = 21
};

// ============================================================================
// [Fog::FB_GEOMETRY]
// ============================================================================

//! @brief FbWindow geometry event flags.
enum FB_GEOMETRY
{
  FB_GEOMETRY_WINDOW_POSITION = 0x01,
  FB_GEOMETRY_WINDOW_SIZE = 0x02,
  FB_GEOMETRY_CLIENT_POSITION = 0x04,
  FB_GEOMETRY_CLIENT_SIZE = 0x08,
  FB_GEOMETRY_ORIENTATION = 0x10
};

// ============================================================================
// [Fog::FB_MISC]
// ============================================================================

//! @brief Various constants used by @c FbEngine and @c FbWindow.
//!
//! @note This is mainly for internal purposes, but visible here so the default
//! values can be safely "modified" to fit into a particular siftware/hardware.
enum FB_MISC
{
  FB_MISC_DEFAULT_WHEEL_LINES = 3
};






















// ============================================================================
// [Fog::MARGIN_LOCATION]
// ============================================================================

//! @brief Margin location.
enum MARGIN_LOCATION
{
  MARGIN_TOP = 0,
  MARGIN_RIGHT = 1,
  MARGIN_BOTTOM = 2,
  MARGIN_LEFT = 3
};

// ============================================================================
// [Fog::LAYOUT_DEFAULTS]
// ============================================================================

// TODO: Move to theme.

const int LAYOUT_DEFAULT_SPACING = 6;
const int LAYOUT_DEFAULT_WIDGET_MARGIN = 9;
const int LAYOUT_DEFAULT_WINDOW_MARGIN = 11;

// ============================================================================
// [Fog::LAYOUT_EDGE]
// ============================================================================

//! @brief Layout edge.
//!
//! @sa @ref BorderLayout.
enum LAYOUT_EDGE
{
  LAYOUT_EDGE_X_MASK = 0x01,
  LAYOUT_EDGE_Y_MASK = 0x02,

  LAYOUT_EDGE_NORTH = 0x08 | LAYOUT_EDGE_Y_MASK,
  LAYOUT_EDGE_SOUTH = 0x10 | LAYOUT_EDGE_Y_MASK,
  LAYOUT_EDGE_WEST = 0x20 | LAYOUT_EDGE_X_MASK,
  LAYOUT_EDGE_EAST = 0x40 | LAYOUT_EDGE_X_MASK,
  LAYOUT_EDGE_CENTER = 0x80
};

//! @brief Layout sort type.
//!
//! @sa @ref BorderLayout.
enum LAYOUT_SORT
{
  LAYOUT_SORT_NONE = 0,
  LAYOUT_SORT_X = 1,
  LAYOUT_SORT_Y = 2
};

// ============================================================================
// [Fog::LAYOUT_CONSTRAINT]
// ============================================================================

//! @brief Layout constraint.
// TODO: Not implemented yet.
enum LAYOUT_CONSTRAINT
{
  LAYOUT_CONSTRAINT_DEFAULT,
  LAYOUT_CONSTRAINT_NO,
  LAYOUT_CONSTRAINT_MINIMUM_SIZE,
  LAYOUT_CONSTRAINT_FIXED_SIZE,
  LAYOUT_CONSTRAINT_MAXIMUM_SIZE,
  LAYOUT_CONSTRAINT_MINIMUM_MAXIMUM_SIZE
};

//! @brief Layout direction.
enum LAYOUT_DIRECTION
{
  LAYOUT_DIRECTION_LEFT_TO_RIGHT = 0,
  LAYOUT_DIRECTION_RIGHT_TO_LEFT = 1,
  LAYOUT_DIRECTION_TOP_TO_BOTTOM = 0,
  LAYOUT_DIRECTION_BOTTOM_TO_TOP = 1,

  LAYOUT_DIRECTION_DOWN = LAYOUT_DIRECTION_TOP_TO_BOTTOM,
  LAYOUT_DIRECTION_UP = LAYOUT_DIRECTION_BOTTOM_TO_TOP
};

// ============================================================================
// [Fog::WIDGET_LIMITS]
// ============================================================================

enum WIDGET_LIMITS
{
  WIDGET_MIN_SIZE = 0,
  WIDGET_MAX_SIZE = 0xFFFFFF
};

// ============================================================================
// [Fog::WIDGET_UPDATE_FLAGS]
// ============================================================================

enum WIDGET_UPDATE_FLAGS
{
  // --------------------------------------------------------------------------
  // [Geometry / Layout]
  // --------------------------------------------------------------------------

  //! @brief Something needs update (set if something needs update).
  WIDGET_UPDATE_SOMETHING = (1 << 0),

  //! @brief Some child needs update (has generic update flag set).
  WIDGET_UPDATE_CHILD = (1 << 1),

  //! @brief Update the widget geometry.
  WIDGET_UPDATE_GEOMETRY = (1 << 2),

  // --------------------------------------------------------------------------
  // [Paint]
  // --------------------------------------------------------------------------

  //! @brief Repaint non-client area.
  WIDGET_UPDATE_NCPAINT = (1 << 16),

  //! @brief Repaint client area.
  WIDGET_UPDATE_PAINT  = (1 << 17),

  //! @brief Repaint caret.
  WIDGET_UPDATE_CARET = (1 << 18),

  //! @brief Repaint everything, including caret.
  //!
  //! Combination of @c WIDGET_UPDATE_NCPAINT, @c WIDGET_UPDATE_PAINT and @c
  //! WIDGET_UPDATE_CARET.
  WIDGET_REPAINT_ALL = WIDGET_UPDATE_NCPAINT | WIDGET_UPDATE_PAINT | WIDGET_UPDATE_CARET,

  //! @brief Update everything for the widget and all children.
  WIDGET_UPDATE_ALL = (1 << 31)
};

// ============================================================================
// [Fog::WIDGET_PAINT_HINT]
// ============================================================================

//! @brief Paint hint that can be returned by @c Widget::getPaintMode().
enum WIDGET_PAINT_HINT
{
  //! @brief Widget paint event will repaint the whole widget (default).
  //!
  //! This is the default and most optimized mode, fast themes should use this
  //! for large controls (tables, lists, grids, etc...).
  WIDGET_PAINT_SCREEN = 0x0000,

  //! @brief Widget paint event will not repaint the whole widget, in some
  //! regions the parent content is propagated.
  //!
  //! This flag must be returned by @c Widget::getPaintHint() if widget want
  //! to base content on parent, otherwise the result of painting is undefined
  //! (there can be some garbage pixels from previous painting or just random
  //! bytes from previous data in the buffer).
  WIDGET_PAINT_PROPAGATE = 0x0001
};

// ============================================================================
// [Fog::WIDGET_STATE]
// ============================================================================

//! @brief Widget state.
enum WIDGET_STATE
{
  //! @brief Widget is disabled.
  WIDGET_DISABLED = 0,
  //! @brief Widget is enabled, but parent is disabled and this makes control disabled.
  WIDGET_DISABLED_BY_PARENT = 1,
  //! @brief Widget is enabled and it parent too.
  WIDGET_ENABLED = 2,
};

// ============================================================================
// [Fog::WIDGET_VISIBILITY]
// ============================================================================

//! @brief Widget visibility
enum WIDGET_VISIBILITY
{
  //! @brief Widget is hidden.
  WIDGET_HIDDEN = 0,
  //! @brief Widget is hidden by parent that's not visible.
  WIDGET_HIDDEN_BY_PARENT = 1,
  WIDGET_VISIBLE_MINIMIZED = 2,

  //! @brief Widget is visible.
  WIDGET_VISIBLE = 3,
  WIDGET_VISIBLE_RESTORE = 4,
  WIDGET_VISIBLE_MAXIMIZED = 5,
  WIDGET_VISIBLE_FULLSCREEN = 6
};

// ============================================================================
// [Fog::FRAME_STYLE]
// ============================================================================

enum FRAME_STYLE
{
  FRAME_NONE = 0,
  FRAME_TEXT_AREA = 1
};

// ============================================================================
// [Fog::FOCUSABLE]
// ============================================================================

enum FOCUSABLE
{
  FOCUSABLE_NEXT = 0,
  FOCUSABLE_PREVIOUS = 1
};

// ============================================================================
// [Fog::FOCUS_MASK]
// ============================================================================

//! @brief Focus masks used by @c FOCUS_POLICY
enum FOCUS_MASK
{
  FOCUS_MASK_TAB = (1 << 0),
  FOCUS_MASK_CLICK = (1 << 1),
  FOCUS_MASK_WHEEL = (1 << 2)
};

// ============================================================================
// [Fog::FOCUS_POLICY]
// ============================================================================

//! @brief Focus policy.
enum FOCUS_POLICY
{
  FOCUS_NONE = 0,
  FOCUS_TAB = FOCUS_MASK_TAB,
  FOCUS_CLICK = FOCUS_MASK_CLICK,
  FOCUS_STRONG = FOCUS_TAB | FOCUS_CLICK,
  FOCUS_WHEEL = FOCUS_STRONG | FOCUS_MASK_WHEEL
};

// ============================================================================
// [Fog::MODAL_POLICY]
// ============================================================================

//! @brief Modal policy.
enum MODAL_POLICY
{
  MODAL_NONE = 0,
  MODAL_WINDOW = 1,
  MODAL_APPLICATION = 2
};

// ============================================================================
// [Fog::LAYOUT_POLICY_FLAGS]
// ============================================================================

//! @brief Layout policy flags.
enum LAYOUT_POLICY_FLAGS
{
  LAYOUT_WIDTH_MASK = 0x0F,
  LAYOUT_WIDTH_SHIFT = 0,

  LAYOUT_HEIGHT_MASK = 0xF0,
  LAYOUT_HEIGHT_SHIFT = 4,

  LAYOUT_EXPANDING_WIDTH = (0x1 << LAYOUT_WIDTH_SHIFT),
  LAYOUT_SHRINKING_WIDTH = (0x2 << LAYOUT_WIDTH_SHIFT),
  LAYOUT_GROWING_WIDTH = (0x4 << LAYOUT_WIDTH_SHIFT),
  LAYOUT_IGNORE_WIDTH  = (0x8 << LAYOUT_WIDTH_SHIFT),

  LAYOUT_EXPANDING_HEIGHT = (0x1 << LAYOUT_HEIGHT_SHIFT),
  LAYOUT_SHRINKING_HEIGHT = (0x2 << LAYOUT_HEIGHT_SHIFT),
  LAYOUT_GROWING_HEIGHT = (0x4 << LAYOUT_HEIGHT_SHIFT),
  LAYOUT_IGNORE_HEIGHT  = (0x8 << LAYOUT_HEIGHT_SHIFT)
};

// ============================================================================
// [Fog::LAYOUT_POLICY]
// ============================================================================

//! @brief Layout policy.
enum LAYOUT_POLICY
{
  LAYOUT_POLICY_HEIGHT_FIXED = 0,
  LAYOUT_POLICY_WIDTH_FIXED = 0,

  LAYOUT_POLICY_WIDTH_MINIMUM = LAYOUT_EXPANDING_WIDTH,
  LAYOUT_POLICY_WIDTH_MAXIMUM = LAYOUT_SHRINKING_WIDTH,

  LAYOUT_POLICY_WIDTH_PREFERRED =
    LAYOUT_GROWING_WIDTH |
    LAYOUT_SHRINKING_WIDTH,

  LAYOUT_POLICY_WIDTH_MINIMUM_EXPANDING =
    LAYOUT_GROWING_WIDTH |
    LAYOUT_EXPANDING_WIDTH,

  LAYOUT_POLICY_WIDTH_EXPANDING =
    LAYOUT_GROWING_WIDTH |
    LAYOUT_EXPANDING_WIDTH |
    LAYOUT_SHRINKING_WIDTH,

  LAYOUT_POLICY_WIDTH_IGNORED =
    LAYOUT_GROWING_WIDTH |
    LAYOUT_IGNORE_WIDTH |
    LAYOUT_SHRINKING_WIDTH,

  LAYOUT_POLICY_HEIGHT_MINIMUM = LAYOUT_EXPANDING_HEIGHT,
  LAYOUT_POLICY_HEIGHT_MAXIMUM = LAYOUT_SHRINKING_HEIGHT,

  LAYOUT_POLICY_HEIGHT_PREFERRED =
    LAYOUT_GROWING_HEIGHT |
    LAYOUT_SHRINKING_HEIGHT,

  LAYOUT_POLICY_HEIGHT_MINIMUM_EXPANDING =
    LAYOUT_GROWING_HEIGHT |
    LAYOUT_EXPANDING_HEIGHT,

  LAYOUT_POLICY_HEIGHT_EXPANDING =
    LAYOUT_GROWING_HEIGHT |
    LAYOUT_EXPANDING_HEIGHT |
    LAYOUT_SHRINKING_HEIGHT,

  LAYOUT_POLICY_HEIGHT_IGNORED =
    LAYOUT_GROWING_HEIGHT |
    LAYOUT_IGNORE_HEIGHT |
    LAYOUT_SHRINKING_HEIGHT
};

// ============================================================================
// [Fog::MINMAXSIZE]
// ============================================================================

// TODO: Rename...
enum MINMAXSIZE
{
  MIN_WIDTH_IS_SET   = 1,
  MIN_HEIGHT_IS_SET = 2,

  MAX_WIDTH_IS_SET   = 1,
  MAX_HEIGHT_IS_SET = 2
};

// ============================================================================
// [Fog::WINDOW_FLAGS]
// ============================================================================

enum WIDGET_FLAGS
{
  WIDGET_FLAG_LAYOUT_USES_WINDOW_RECT = (1 << 0)
};

// ============================================================================
// [Fog::WINDOW_FLAGS]
// ============================================================================

//! @brief GuiWindow create flags (used also by @c Widget).
enum WINDOW_FLAGS
{
  // --------------------------------------------------------------------------
  // [Window Type]
  // --------------------------------------------------------------------------

  //! @brief Flag to indicate the maximum of window types
  //! (needed for fast identification of TopLevel-Windows)
  WINDOW_TYPE_MASK = 0x000000FF,

  //! @brief Create GuiWindow (this flag is used in Fog::Widget).
  WINDOW_NATIVE = (1 << 0),

  //! @brief Create dialog window instead of normal one.
  //!
  //! A dialog is a window with thin frame, which cannot be resized.
  //! Normally a dialog also has no minimize and maximize functionality
  WINDOW_DIALOG = (1 << 1),

  //! @brief Create tool window instead of normal one.
  //!
  //! A tool window is often a small window with a smaller than usual
  //! title bar and decoration
  WINDOW_TOOL = (1 << 2),

  //! @brief Create popup like window instead of normal one.
  //!
  //! Created popup window hasn't native borders and decoration, use
  //! border style to set these borders.
  WINDOW_POPUP = (1 << 3),

  //! @brief Create a window without frame and decoration
  //!
  //! The window will not have a decoration at all, this will complete
  //! overwrite the window frame flag.
  WINDOW_FRAMELESS = (1 << 4),

  //! @brief Create a window without frame and decoration which fills the whole
  //! screen
  //!
  //! This flag overwrites all other window-types
  WINDOW_FULLSCREEN = (1 << 5),

  // --------------------------------------------------------------------------
  // [Window Hints]
  // --------------------------------------------------------------------------

  //! @brief Flag to indicate the maximum of window hints
  WINDOW_HINTS_MASK = 0xFFFFFF00,

  //! @brief Marks the window as non-resizeable
  //!
  //! The window could not be resized at all. This will overwrite
  //! resize feature of window frame flag (e.g. WINDOW_NATIVE)
  WINDOW_FIXED_SIZE = (1 << 8),

  //! @brief Marks the window as non-movable
  //!
  //! The window could not be dragged using the window decoration
  WINDOW_DRAGABLE = (1 << 9),

  //! @brief Marks the window to stay TopMost of the other windows
  //!
  //! The window will be on top of the whole system
  WINDOW_ALWAYS_ON_TOP = (1 << 11),

  //! @brief Marks if the Window should have no system menu
  //!
  //! If this flag is set the system menu is completely disabled for the
  //! window
  WINDOW_SYSTEM_MENU = (1 << 12),

  //! @brief Marks if the Window should have a max button
  WINDOW_MAXIMIZE = (1 << 13),

  //! @brief Marks if the Window should have a min button
  WINDOW_MINIMIZE = (1 << 14),

  //! @brief Marks if the Window should not have a close button
  //!
  //! The close button is only visible, if the window has also a system menu!
  //! This will only disable the functionality from UI. (Close-Button & SystemMenu-Entry)
  //! But Alt+F4 will close the window!
  WINDOW_CLOSE_BUTTON = (1 << 15),

  //! @brief Marks if the Window should have a context help button
  //!
  //! The help button is only visible, if the window has also a system menu!
  //! A tool window and a popup could not have a help button
  WINDOW_CONTEXT_HELP_BUTTON = (1 << 16),

  //! @brief Marks the window to allow transparency
  WINDOW_TRANSPARENT = (1 << 17),

  //! @brief Marks that the PopUp window should be inline within the top level window
  //!
  //! A pop normally creates a system top level window. But if this
  //! flag is set, the pop acts as a child widget on top of all others in the guiWindow!
  WINDOW_INLINE_POPUP = (1 << 18),

  // --------------------------------------------------------------------------
  // [Engine Specific]
  // --------------------------------------------------------------------------

  //! @brief Create X11 window that listens only for XPropertyChange events.
  //!
  //! X11GuiEngine dependent and non-portable flag.
  WINDOW_X11_PROPERTY_ONLY = (1 << 30),

  //! @brief Override redirection from window managers under X11.
  //!
  //! X11GuiEngine dependent and non-portable flag.
  WINDOW_X11_OVERRIDE_REDIRECT = (1 << 31)
};

// TODO GUI: This is not good.
//
// Window type can't be set of flags, it must be constant like 1, 2, 3. Flags
// should be get from windowing system, because X11 can be totally different
// to Windows and sometimes it just don't allow to set these flags.
enum WINDOW_TYPES
{
  WINDOW_TYPE_DIALOG = WINDOW_DIALOG | WINDOW_SYSTEM_MENU | WINDOW_DRAGABLE | WINDOW_FIXED_SIZE | WINDOW_ALWAYS_ON_TOP | WINDOW_CLOSE_BUTTON,
  WINDOW_TYPE_DEFAULT = WINDOW_NATIVE | WINDOW_SYSTEM_MENU | WINDOW_DRAGABLE | WINDOW_MINIMIZE | WINDOW_MAXIMIZE | WINDOW_CLOSE_BUTTON,
  WINDOW_TYPE_TOOL  = WINDOW_TOOL | WINDOW_CLOSE_BUTTON | WINDOW_SYSTEM_MENU | WINDOW_DRAGABLE,
  WINDOW_TYPE_POPUP = WINDOW_POPUP | WINDOW_ALWAYS_ON_TOP | WINDOW_DRAGABLE,
  WINDOW_TYPE_FRAMELESS = WINDOW_FRAMELESS | WINDOW_DRAGABLE,
  WINDOW_TYPE_FULLSCREEN = WINDOW_FRAMELESS | WINDOW_SYSTEM_MENU | WINDOW_ALWAYS_ON_TOP
};

// ============================================================================
// [Fog::KEY_CODE]
// ============================================================================

//! @brief Key codes.
enum KEY_CODE
{
  KEY_BACKSPACE            = 8,
  KEY_TAB                  = 9,
  KEY_CLEAR                = 12,
  KEY_ENTER                = 13,
  KEY_PAUSE                = 19,
  KEY_ESC                  = 27,
  KEY_SPACE                = 32,
  KEY_EXCLAIM              = 33,
  KEY_DOUBLEQUOTE          = 34,
  KEY_HASH                 = 35,
  KEY_DOLLAR               = 36,
  KEY_PERCENT              = 37,
  KEY_AMPERSAND            = 38,
  KEY_QUOTE                = 39,
  KEY_LEFT_PAREN           = 40,
  KEY_RIGHT_PAREN          = 41,
  KEY_ASTERISK             = 42,
  KEY_PLUS                 = 43,
  KEY_COMMA                = 44,
  KEY_MINUS                = 45,
  KEY_PERIOD               = 46,
  KEY_SLASH                = 47,
  KEY_0                    = 48,
  KEY_1                    = 49,
  KEY_2                    = 50,
  KEY_3                    = 51,
  KEY_4                    = 52,
  KEY_5                    = 53,
  KEY_6                    = 54,
  KEY_7                    = 55,
  KEY_8                    = 56,
  KEY_9                    = 57,
  KEY_COLON                = 58,
  KEY_SEMICOLON            = 59,
  KEY_LESS                 = 60,
  KEY_EQUALS               = 61,
  KEY_GREATER              = 62,
  KEY_QUESTION             = 63,
  KEY_AT                   = 64,
  KEY_LEFT_BRACKET         = 91,
  KEY_BACKSLASH            = 92,
  KEY_RIGHT_BRACKET        = 93,
  KEY_CARET                = 94,
  KEY_UNDERSCORE           = 95,
  KEY_BACKQUOTE            = 96,
  KEY_A                    = 97,
  KEY_B                    = 98,
  KEY_C                    = 99,
  KEY_D                    = 100,
  KEY_E                    = 101,
  KEY_F                    = 102,
  KEY_G                    = 103,
  KEY_H                    = 104,
  KEY_I                    = 105,
  KEY_J                    = 106,
  KEY_K                    = 107,
  KEY_L                    = 108,
  KEY_M                    = 109,
  KEY_N                    = 110,
  KEY_O                    = 111,
  KEY_P                    = 112,
  KEY_Q                    = 113,
  KEY_R                    = 114,
  KEY_S                    = 115,
  KEY_T                    = 116,
  KEY_U                    = 117,
  KEY_V                    = 118,
  KEY_W                    = 119,
  KEY_X                    = 120,
  KEY_Y                    = 121,
  KEY_Z                    = 122,
  KEY_LEFT_BRACE           = 123,
  KEY_PIPE                 = 124,
  KEY_RIGHT_BRACE          = 125,
  KEY_TILDE                = 126,
  KEY_DELETE               = 127,
  KEY_KP_0                 = 256,
  KEY_KP_1                 = 257,
  KEY_KP_2                 = 258,
  KEY_KP_3                 = 259,
  KEY_KP_4                 = 260,
  KEY_KP_5                 = 261,
  KEY_KP_6                 = 262,
  KEY_KP_7                 = 263,
  KEY_KP_8                 = 264,
  KEY_KP_9                 = 265,
  KEY_KP_PERIOD            = 266,
  KEY_KP_DIVIDE            = 267,
  KEY_KP_MULTIPLY          = 268,
  KEY_KP_MINUS             = 269,
  KEY_KP_PLUS              = 270,
  KEY_KP_ENTER             = 271,
  KEY_KP_EQUALS            = 272,
  KEY_UP                   = 273,
  KEY_DOWN                 = 274,
  KEY_RIGHT                = 275,
  KEY_LEFT                 = 276,
  KEY_INSERT               = 277,
  KEY_HOME                 = 278,
  KEY_END                  = 279,
  KEY_PAGE_UP              = 280,
  KEY_PAGE_DOWN            = 281,
  KEY_F1                   = 282,
  KEY_F2                   = 283,
  KEY_F3                   = 284,
  KEY_F4                   = 285,
  KEY_F5                   = 286,
  KEY_F6                   = 287,
  KEY_F7                   = 288,
  KEY_F8                   = 289,
  KEY_F9                   = 290,
  KEY_F10                  = 291,
  KEY_F11                  = 292,
  KEY_F12                  = 293,
  KEY_F13                  = 294,
  KEY_F14                  = 295,
  KEY_F15                  = 296,
  KEY_NUM_LOCK             = 300,
  KEY_CAPS_LOCK            = 301,
  KEY_SCROLL_LOCK          = 302,
  KEY_RIGHT_SHIFT          = 303,
  KEY_LEFT_SHIFT           = 304,
  KEY_RIGHT_CTRL           = 305,
  KEY_LEFT_CTRL            = 306,
  KEY_RIGHT_ALT            = 307,
  KEY_LEFT_ALT             = 308,
  KEY_RIGHT_META           = 309,
  KEY_LEFT_META            = 310,
  KEY_LEFT_SUPER           = 311, //!< @brief Left "Windows" key.
  KEY_RIGHT_SUPER          = 312, //!< @brief Right "Windows" key.
  KEY_MODE                 = 313, //!< @brief "Alt Gr" key.
  KEY_HELP                 = 315,
  KEY_PRINT                = 316,
  KEY_SYS_REQ              = 317,
  KEY_BREAK                = 318,
  KEY_MENU                 = 319,
  KEY_POWER                = 320, //!< @brief Mac power key.
  KEY_EURO                 = 321, //!< @brief Euro key.
  KEY_ALPHA                = 322, //!< @brief Select letters on a numeric keypad.

  KEY_COMPOSE              = 400,

  KEY_COUNT                = 401,
  KEY_MASK                 = 0x1FFFFFFF,

  // Modifiers mapped to key
  KEY_SHIFT                = (1 << 29),
  KEY_CTRL                 = (1 << 30),
  KEY_ALT                  = (1 << 31)
};

// ============================================================================
// [Fog::MODIFIER_CODE]
// ============================================================================

//! @brief Key modifier codes.
enum MODIFIER_CODE
{
  //! @brief Left shift.
  MODIFIER_LEFT_SHIFT      = 0x0001,
  //! @brief Right shift.
  MODIFIER_RIGHT_SHIFT     = 0x0002,
  //! @brief Left or right shift, use if (mod & MODIFIER_SHIFT).
  MODIFIER_SHIFT           = 0x0003,

  //! @brief Left control.
  MODIFIER_LEFT_CTRL       = 0x0004,
  //! @brief Right control.
  MODIFIER_RIGHT_CTRL      = 0x0008,
  //! @brief Left or right control.
  MODIFIER_CTRL            = 0x000C,

  //! @brief Left alt.
  MODIFIER_LEFT_ALT        = 0x0010,
  //! @brief Right alt.
  MODIFIER_RIGHT_ALT       = 0x0020,
  //! @brief Left or right alt.
  MODIFIER_ALT             = 0x0030,

  //! @brief Left meta.
  MODIFIER_LEFT_META       = 0x0040,
  //! @brief Right meta.
  MODIFIER_RIGHT_META      = 0x0080,
  //! @brief Left or right meta.
  MODIFIER_META            = 0x00C0,

  //! @brief Left, Shift, Alt and Meta masks.
  MODIFIER_MASK            = 0x00FF,

  //! @brief Num lock.
  MODIFIER_NUM             = 0x1000,
  //! @brief Caps lock.
  MODIFIER_CAPS            = 0x2000,
  //! @brief Mode.
  MODIFIER_MODE            = 0x4000
};

// ============================================================================
// [Fog::BUTTON_CODE]
// ============================================================================

//! @brief Mouse button code.
//!
//! @sa @ref WHEEL_CODE.
enum BUTTON_CODE
{
  //! @brief Left button.
  BUTTON_LEFT = 0x0001,
  //! @brief Middle button.
  BUTTON_MIDDLE = 0x0002,
  //! @brief Right button.
  BUTTON_RIGHT = 0x0004,
  //! @brief Invalid button (used internally).
  BUTTON_INVALID = 0xFFFF
};

// ============================================================================
// [Fog::WHEEL_CODE]
// ============================================================================

//! @brief Mouse wheel code.
//!
//! @sa @ref BUTTON_CODE.
enum WHEEL_CODE
{
  //! @brief Wheel up button.
  WHEEL_UP = 0x0008,
  //! @brief Wheel down button.
  WHEEL_DOWN = 0x0010
};

// ============================================================================
// [Fog::ORIENTATION]
// ============================================================================

//! @brief Orientation of widget or layout.
enum ORIENTATION
{
  //! @brief Horizontal orientation.
  ORIENTATION_HORIZONTAL = 0,
  //! @brief Vertical orientation.
  ORIENTATION_VERTICAL = 1
};

// ============================================================================
// [Fog::ALIGNMENT]
// ============================================================================

enum ALIGNMENT
{
  //! @brief Aligns with the left edge.
  ALIGNMENT_LEFT = 0x001,
  //! @brief Aligns with the right edge.
  ALIGNMENT_RIGHT = 0x002,
  //! @brief Centers horizontally in the available space.
  ALIGNMENT_HCENTER = 0x004,
  //! @brief Justifies the text in the available space.
  ALIGNMENT_JUSTIFY = 0x008,

  //! @brief Aligns with the top.
  ALIGNMENT_TOP = 0x0020,
  //! @brief Aligns with the bottom.
  ALIGNMENT_BOTTOM = 0x0040,
  //! @brief Centers vertically in the available space.
  ALIGNMENT_VCENTER = 0x0080,

  //! @brief Centers in both dimensions.
  ALIGNMENT_CENTER  = ALIGNMENT_HCENTER | ALIGNMENT_VCENTER,

  // TODO: not supported yet.
  //ALIGNMENT_ABSOLUTE = 0x0010,

  //! @brief Synonym for ALIGNMENT_LEFT.
  ALIGNMENT_LEADING  = ALIGNMENT_LEFT,
  //! @brief Synonym for ALIGNMENT_RIGHT.
  ALIGNMENT_TRAILING = ALIGNMENT_RIGHT,

  ALIGNMENT_HORIZONTAL_MASK =
    ALIGNMENT_LEFT |
    ALIGNMENT_RIGHT |
    ALIGNMENT_HCENTER |
    ALIGNMENT_JUSTIFY, // | ALIGNMENT_ABSOLUTE

  ALIGNMENT_VERTICAL_MASK =
    ALIGNMENT_TOP |
    ALIGNMENT_BOTTOM |
    ALIGNMENT_VCENTER
};

// ============================================================================
// [Fog::CARET_TYPE]
// ============================================================================

enum CARET_TYPE
{
  //! @brief Normal type of caret.
  CARET_TYPE_NORMAL = 0,
  //! @brief Overwrite type of caret.
  CARET_TYPE_OVERWRITE = 1,
};

// ============================================================================
// [Fog::FOCUS_REASON]
// ============================================================================

enum FOCUS_REASON
{
  FOCUS_REASON_NONE = 0,
  FOCUS_REASON_MOUSE,
  FOCUS_REASON_WHEEL,
  FOCUS_REASON_TAB_FORWARD,
  FOCUS_REASON_TAB_BACKWARD
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_GLOBAL_ENUMUI_H
