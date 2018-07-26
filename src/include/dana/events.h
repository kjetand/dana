#pragma once

#include <cstdint>
#include <functional>
#include <variant>

namespace dana {

struct EmptyEvent {};

enum class WindowEvents {
  NONE,
  SHOWN,
  HIDDEN,
  EXPOSED,
  MOVED,
  RESIZED,
  SIZE_CHANGED,
  MINIMIZED,
  MAXIMIZED,
  RESTORED,
  ENTERED,
  LEAVED,
  FOCUS_GAINED,
  FOCUS_LOST,
  CLOSED
};

struct WindowEvent {
  WindowEvents type{WindowEvents::NONE};
};

enum class KeyboardStates { NONE, PRESSED, RELEASED };

enum class KeyboardModifiers {
  NONE,
  SHIFT,
  LEFT_SHIFT,
  RIGHT_SHIFT,
  CTRL,
  LEFT_CTRL,
  RIGHT_CTRL,
  ALT,
  LEFT_ALT,
  RIGHT_ALT,
  GUI,
  LEFT_GUI,
  RIGHT_GUI,
  NUM_LOCK,
  CAPS_LOCK,
  ALT_GR
};

enum class ScanCodes {
  NONE,
  NUMBER_0,
  NUMBER_1,
  NUMBER_2,
  NUMBER_3,
  NUMBER_4,
  NUMBER_5,
  NUMBER_6,
  NUMBER_7,
  NUMBER_8,
  NUMBER_9,
  A,
  AC_BACK,
  AC_BOOKMARKS,
  AC_FORWARD,
  AC_HOME,
  AC_REFRESH,
  AC_SEARCH,
  AC_STOP,
  AGAIN,
  ALTERASE,
  APOSTROPHE,
  APPLICATION,
  AUDIOMUTE,
  AUDIONEXT,
  AUDIOPLAY,
  AUDIOPREV,
  AUDIOSTOP,
  B,
  BACKSLASH,
  BACKSPACE,
  BRIGHTNESSDOWN,
  BRIGHTNESSUP,
  C,
  CALCULATOR,
  CANCEL,
  CAPSLOCK,
  CLEAR,
  CLEARAGAIN,
  COMMA,
  COMPUTER,
  COPY,
  CRSEL,
  CURRENCYSUBUNIT,
  CURRENCYUNIT,
  CUT,
  D,
  DECIMALSEPARATOR,
  DELETE,
  DISPLAYSWITCH,
  DOWN,
  E,
  EJECT,
  END,
  EQUALS,
  ESCAPE,
  EXECUTE,
  EXSEL,
  F,
  F1,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F2,
  F20,
  F21,
  F22,
  F23,
  F24,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  FIND,
  G,
  GRAVE,
  H,
  HELP,
  HOME,
  I,
  INSERT,
  J,
  K,
  KBDILLUMDOWN,
  KBDILLUMTOGGLE,
  KBDILLUMUP,
  KP_0,
  KP_00,
  KP_000,
  KP_1,
  KP_2,
  KP_3,
  KP_4,
  KP_5,
  KP_6,
  KP_7,
  KP_8,
  KP_9,
  KP_A,
  KP_AMPERSAND,
  KP_AT,
  KP_B,
  KP_BACKSPACE,
  KP_BINARY,
  KP_C,
  KP_CLEAR,
  KP_CLEARENTRY,
  KP_COLON,
  KP_COMMA,
  KP_D,
  KP_DBLAMPERSAND,
  KP_DBLVERTICALBAR,
  KP_DECIMAL,
  KP_DIVIDE,
  KP_E,
  KP_ENTER,
  KP_EQUALS,
  KP_EQUALSAS400,
  KP_EXCLAM,
  KP_F,
  KP_GREATER,
  KP_HASH,
  KP_HEXADECIMAL,
  KP_LEFTBRACE,
  KP_LEFTPAREN,
  KP_LESS,
  KP_MEMADD,
  KP_MEMCLEAR,
  KP_MEMDIVIDE,
  KP_MEMMULTIPLY,
  KP_MEMRECALL,
  KP_MEMSTORE,
  KP_MEMSUBTRACT,
  KP_MINUS,
  KP_MULTIPLY,
  KP_OCTAL,
  KP_PERCENT,
  KP_PERIOD,
  KP_PLUS,
  KP_PLUSMINUS,
  KP_POWER,
  KP_RIGHTBRACE,
  KP_RIGHTPAREN,
  KP_SPACE,
  KP_TAB,
  KP_VERTICALBAR,
  KP_XOR,
  L,
  LALT,
  LCTRL,
  LEFT,
  LEFTBRACKET,
  LGUI,
  LSHIFT,
  M,
  MAIL,
  MEDIASELECT,
  MENU,
  MINUS,
  MODE,
  MUTE,
  N,
  NUMLOCKCLEAR,
  O,
  OPER,
  OUT,
  P,
  PAGEDOWN,
  PAGEUP,
  PASTE,
  PAUSE,
  PERIOD,
  POWER,
  PRINTSCREEN,
  PRIOR,
  Q,
  R,
  RALT,
  RCTRL,
  RETURN,
  RETURN2,
  RGUI,
  RIGHT,
  RIGHTBRACKET,
  RSHIFT,
  S,
  SCROLLLOCK,
  SELECT,
  SEMICOLON,
  SEPARATOR,
  SLASH,
  SLEEP,
  SPACE,
  STOP,
  SYSREQ,
  T,
  TAB,
  THOUSANDSSEPARATOR,
  U,
  UNDO,
  UNKNOWN,
  UP,
  V,
  VOLUMEDOWN,
  VOLUMEUP,
  W,
  WWW,
  X,
  Y,
  Z,
  INTERNATIONAL1,
  INTERNATIONAL2,
  INTERNATIONAL3,
  INTERNATIONAL4,
  INTERNATIONAL5,
  INTERNATIONAL6,
  INTERNATIONAL7,
  INTERNATIONAL8,
  INTERNATIONAL9,
  LANG1,
  LANG2,
  LANG3,
  LANG4,
  LANG5,
  LANG6,
  LANG7,
  LANG8,
  LANG9,
  NONUSBACKSLASH,
  NONUSHASH,
  APP1,
  APP2,
  AUDIOREWIND,
  AUDIOFASTFORWARD
};

enum class KeyCodes {
  NONE,
  NUMBER_0,
  NUMBER_1,
  NUMBER_2,
  NUMBER_3,
  NUMBER_4,
  NUMBER_5,
  NUMBER_6,
  NUMBER_7,
  NUMBER_8,
  NUMBER_9,
  a,
  AC_BACK,
  AC_BOOKMARKS,
  AC_FORWARD,
  AC_HOME,
  AC_REFRESH,
  AC_SEARCH,
  AC_STOP,
  AGAIN,
  ALTERASE,
  QUOTE,
  APPLICATION,
  AUDIOMUTE,
  AUDIONEXT,
  AUDIOPLAY,
  AUDIOPREV,
  AUDIOSTOP,
  b,
  BACKSLASH,
  BACKSPACE,
  BRIGHTNESSDOWN,
  BRIGHTNESSUP,
  c,
  CALCULATOR,
  CANCEL,
  CAPSLOCK,
  CLEAR,
  CLEARAGAIN,
  COMMA,
  COMPUTER,
  COPY,
  CRSEL,
  CURRENCYSUBUNIT,
  CURRENCYUNIT,
  CUT,
  d,
  DECIMALSEPARATOR,
  DELETE,
  DISPLAYSWITCH,
  DOWN,
  e,
  EJECT,
  END,
  EQUALS,
  ESCAPE,
  EXECUTE,
  EXSEL,
  f,
  F1,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F2,
  F20,
  F21,
  F22,
  F23,
  F24,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  FIND,
  g,
  BACKQUOTE,
  h,
  HELP,
  HOME,
  i,
  INSERT,
  j,
  k,
  KBDILLUMDOWN,
  KBDILLUMTOGGLE,
  KBDILLUMUP,
  KP_0,
  KP_00,
  KP_000,
  KP_1,
  KP_2,
  KP_3,
  KP_4,
  KP_5,
  KP_6,
  KP_7,
  KP_8,
  KP_9,
  KP_A,
  KP_AMPERSAND,
  KP_AT,
  KP_B,
  KP_BACKSPACE,
  KP_BINARY,
  KP_C,
  KP_CLEAR,
  KP_CLEARENTRY,
  KP_COLON,
  KP_COMMA,
  KP_D,
  KP_DBLAMPERSAND,
  KP_DBLVERTICALBAR,
  KP_DECIMAL,
  KP_DIVIDE,
  KP_E,
  KP_ENTER,
  KP_EQUALS,
  KP_EQUALSAS400,
  KP_EXCLAM,
  KP_F,
  KP_GREATER,
  KP_HASH,
  KP_HEXADECIMAL,
  KP_LEFTBRACE,
  KP_LEFTPAREN,
  KP_LESS,
  KP_MEMADD,
  KP_MEMCLEAR,
  KP_MEMDIVIDE,
  KP_MEMMULTIPLY,
  KP_MEMRECALL,
  KP_MEMSTORE,
  KP_MEMSUBTRACT,
  KP_MINUS,
  KP_MULTIPLY,
  KP_OCTAL,
  KP_PERCENT,
  KP_PERIOD,
  KP_PLUS,
  KP_PLUSMINUS,
  KP_POWER,
  KP_RIGHTBRACE,
  KP_RIGHTPAREN,
  KP_SPACE,
  KP_TAB,
  KP_VERTICALBAR,
  KP_XOR,
  l,
  LALT,
  LCTRL,
  LEFT,
  LEFTBRACKET,
  LGUI,
  LSHIFT,
  m,
  MAIL,
  MEDIASELECT,
  MENU,
  MINUS,
  MODE,
  MUTE,
  n,
  NUMLOCKCLEAR,
  o,
  OPER,
  OUT,
  p,
  PAGEDOWN,
  PAGEUP,
  PASTE,
  PAUSE,
  PERIOD,
  POWER,
  PRINTSCREEN,
  PRIOR,
  q,
  r,
  RALT,
  RCTRL,
  RETURN,
  RETURN2,
  RGUI,
  RIGHT,
  RIGHTBRACKET,
  RSHIFT,
  s,
  SCROLLLOCK,
  SELECT,
  SEMICOLON,
  SEPARATOR,
  SLASH,
  SLEEP,
  SPACE,
  STOP,
  SYSREQ,
  t,
  TAB,
  THOUSANDSSEPARATOR,
  u,
  UNDO,
  UNKNOWN,
  UP,
  v,
  VOLUMEDOWN,
  VOLUMEUP,
  w,
  WWW,
  x,
  y,
  z,
  AMPERSAND,
  ASTERISK,
  AT,
  CARET,
  COLON,
  DOLLAR,
  EXCLAIM,
  GREATER,
  HASH,
  LEFTPAREN,
  LESS,
  PERCENT,
  PLUS,
  QUESTION,
  QUOTEDBL,
  RIGHTPAREN,
  UNDERSCORE,
  APP1,
  APP2,
  AUDIOREWIND,
  AUDIOFASTFORWARD
};

struct KeyboardEvent {
  ScanCodes code{ScanCodes::NONE};
  KeyCodes key{KeyCodes::NONE};
  KeyboardStates state{KeyboardStates::NONE};
  uint16_t modifiers{0};
  bool repeat{false};

  KeyboardEvent(const ScanCodes _code, const KeyCodes _key,
                const KeyboardStates _state, const uint16_t _modifiers,
                const bool _repeat);

  bool hasModifier(const KeyboardModifiers modifier) const noexcept;
};

enum class MouseClickTypes { SINGLE, DOUBLE };

enum class MouseButtonStates { PRESSED, RELEASED };

enum class MouseButtons { LEFT, RIGHT, MIDDLE, X1, X2 };

struct MouseButtonEvent {
  MouseButtonStates state;
  MouseClickTypes click_type;
  MouseButtons button;
  float x;
  float y;
};

struct MouseMotionEvent {
  MouseButtons button;
  float x;
  float y;
  float rel_x;
  float rel_y;
};

enum class MouseWheelDirections { NORMAL, FLIPPED };

struct MouseWheelEvent {
  MouseWheelDirections direction;
  float x;
  float y;
};

struct QuitEvent {};

using Event =
    std::variant<EmptyEvent, WindowEvent, KeyboardEvent, MouseButtonEvent,
                 MouseMotionEvent, MouseWheelEvent, QuitEvent>;

template <typename T>
void handleEventIf(
    const Event& event,
    const std::function<void(const T&)>& event_handler) noexcept {
  if (std::holds_alternative<T>(event)) {
    event_handler(std::get<T>(event));
  }
}
}  // namespace dana