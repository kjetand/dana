#include "dana/canvas.h"
#include "dana/pencil.h"

#include <GL/glew.h>
#include <SDL.h>

#include <dana/canvas.h>
#include <chrono>

namespace dana {

static Event convertEvent(const SDL_Event& event) noexcept;

static ScanCodes getScanCode(SDL_Scancode code) noexcept;

static KeyCodes getKeyCode(SDL_Keycode code) noexcept;

static WindowEvents getWindowEvent(uint8_t type) noexcept;

static KeyboardStates getKeyboardState(uint8_t state) noexcept;

static MouseButtonStates getMouseButtonState(uint8_t state) noexcept;

static MouseButtons getMouseButton(uint8_t button) noexcept;

static MouseClickTypes getMouseClickType(const uint8_t clicks) noexcept;

static MouseWheelDirections getMouseWheelDirection(
    const uint32_t direction) noexcept;

Canvas::Canvas(const int width, const int height, const std::string& title) {
  constexpr const Uint32 sdl_flags{SDL_INIT_VIDEO};
  constexpr const Uint32 window_flags{SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                      SDL_WINDOW_HIDDEN |
                                      SDL_WINDOW_ALLOW_HIGHDPI};

  SDL_Init(sdl_flags);

  // NOTE: For now only hard code GL-version to 3.1
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  m_window = c_unique_ptr<SDL_Window>(
      SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, width, height, window_flags),
      [](auto* ptr) { SDL_DestroyWindow(ptr); });

  if (nullptr == m_window) {
    throw std::runtime_error("Unable to initialize SDL window");
  }
  m_gl_context =
      c_unique_ptr<void>(SDL_GL_CreateContext(m_window.get()),
                         [](auto* ptr) { SDL_GL_DeleteContext(ptr); });

  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Unable to initialize Glew");
  }

  glViewport(0, 0, width, height);
}

Canvas::~Canvas() noexcept { SDL_Quit(); }

Canvas& Canvas::setClearColor(const Color& clear_color) noexcept {
  m_clear_color = clear_color;
  return *this;
}

Canvas& Canvas::onNewFrame(const DrawCallback& draw_callback) noexcept {
  m_draw_callback = draw_callback;
  return *this;
}

void Canvas::show() noexcept {
  SDL_ShowWindow(m_window.get());

  SDL_Event event;

  int d_width{0};
  int d_height{0};
  int w_width{0};
  int w_height{0};

  std::chrono::steady_clock::time_point begin_time;

  Pencil pencil;

  while (m_show) {
    begin_time = std::chrono::steady_clock::now();

    pollEvents(event);
    clearWindow();

    SDL_GL_GetDrawableSize(m_window.get(), &d_width, &d_height);
    SDL_GetWindowSize(m_window.get(), &w_width, &w_height);

    glViewport(0, 0, d_width, d_height);

    const auto pixel_ratio =
        static_cast<float>(d_width) / static_cast<float>(w_width);

    // Call user defined draw function
    pencil.beginFrame(static_cast<float>(w_width), static_cast<float>(w_height),
                      pixel_ratio);
    m_draw_callback(pencil);
    pencil.endFrame();

    SDL_GL_SwapWindow(m_window.get());

    m_performance = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - begin_time)
                        .count();

    SDL_Delay(10);
  }
}

void Canvas::clearWindow() const noexcept {
  glClearColor(m_clear_color.r / 255.0f, m_clear_color.g / 255.0f,
               m_clear_color.b / 255.0f, m_clear_color.a / 255.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Canvas::pollEvents(SDL_Event& event) noexcept {
  while (SDL_PollEvent(&event)) {
    m_event_callback(convertEvent(event));
    if (event.type == SDL_QUIT) {
      m_show = false;
    }
  }
}

Canvas& Canvas::onEvent(const EventCallback& event_callback) noexcept {
  m_event_callback = event_callback;
  return *this;
}

long Canvas::getPerformance() const noexcept { return m_performance; }

// Helper functions

static Event convertEvent(const SDL_Event& sdl_event) noexcept {
  Event event;

  switch (sdl_event.type) {
    case SDL_WINDOWEVENT:
      event = WindowEvent{getWindowEvent(sdl_event.window.event)};
      break;
    case SDL_KEYUP:
    case SDL_KEYDOWN:
      event =
          KeyboardEvent(getScanCode(sdl_event.key.keysym.scancode),
                        getKeyCode(sdl_event.key.keysym.sym),
                        getKeyboardState(sdl_event.key.state),
                        sdl_event.key.keysym.mod, sdl_event.key.repeat != 0);
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      event = MouseButtonEvent{getMouseButtonState(sdl_event.button.state),
                               getMouseClickType(sdl_event.button.clicks),
                               getMouseButton(sdl_event.button.button),
                               static_cast<float>(sdl_event.button.x),
                               static_cast<float>(sdl_event.button.y)};
      break;
    case SDL_MOUSEMOTION:
      event = MouseMotionEvent{getMouseButton(sdl_event.motion.state),
                               static_cast<float>(sdl_event.motion.x),
                               static_cast<float>(sdl_event.motion.y),
                               static_cast<float>(sdl_event.motion.xrel),
                               static_cast<float>(sdl_event.motion.yrel)};
      break;
    case SDL_MOUSEWHEEL:
      event = MouseWheelEvent{getMouseWheelDirection(sdl_event.wheel.direction),
                              static_cast<float>(sdl_event.wheel.x),
                              static_cast<float>(sdl_event.wheel.y)};
      break;
    case SDL_QUIT:
      event = QuitEvent{};
      break;
    default:
      event = EmptyEvent{};
  }
  return event;
}

static ScanCodes getScanCode(const SDL_Scancode code) noexcept {
  switch (code) {
    case SDL_SCANCODE_0:
      return ScanCodes::NUMBER_0;
    case SDL_SCANCODE_1:
      return ScanCodes::NUMBER_1;
    case SDL_SCANCODE_2:
      return ScanCodes::NUMBER_2;
    case SDL_SCANCODE_3:
      return ScanCodes::NUMBER_3;
    case SDL_SCANCODE_4:
      return ScanCodes::NUMBER_4;
    case SDL_SCANCODE_5:
      return ScanCodes::NUMBER_5;
    case SDL_SCANCODE_6:
      return ScanCodes::NUMBER_6;
    case SDL_SCANCODE_7:
      return ScanCodes::NUMBER_7;
    case SDL_SCANCODE_8:
      return ScanCodes::NUMBER_8;
    case SDL_SCANCODE_9:
      return ScanCodes::NUMBER_9;
    case SDL_SCANCODE_A:
      return ScanCodes::A;
    case SDL_SCANCODE_AC_BACK:
      return ScanCodes::AC_BACK;
    case SDL_SCANCODE_AC_BOOKMARKS:
      return ScanCodes::AC_BOOKMARKS;
    case SDL_SCANCODE_AC_FORWARD:
      return ScanCodes::AC_FORWARD;
    case SDL_SCANCODE_AC_HOME:
      return ScanCodes::AC_HOME;
    case SDL_SCANCODE_AC_REFRESH:
      return ScanCodes::AC_REFRESH;
    case SDL_SCANCODE_AC_SEARCH:
      return ScanCodes::AC_SEARCH;
    case SDL_SCANCODE_AC_STOP:
      return ScanCodes::AC_STOP;
    case SDL_SCANCODE_AGAIN:
      return ScanCodes::AGAIN;
    case SDL_SCANCODE_ALTERASE:
      return ScanCodes::ALTERASE;
    case SDL_SCANCODE_APOSTROPHE:
      return ScanCodes::APOSTROPHE;
    case SDL_SCANCODE_APPLICATION:
      return ScanCodes::APPLICATION;
    case SDL_SCANCODE_AUDIOMUTE:
      return ScanCodes::AUDIOMUTE;
    case SDL_SCANCODE_AUDIONEXT:
      return ScanCodes::AUDIONEXT;
    case SDL_SCANCODE_AUDIOPLAY:
      return ScanCodes::AUDIOPLAY;
    case SDL_SCANCODE_AUDIOPREV:
      return ScanCodes::AUDIOPREV;
    case SDL_SCANCODE_AUDIOSTOP:
      return ScanCodes::AUDIOSTOP;
    case SDL_SCANCODE_B:
      return ScanCodes::B;
    case SDL_SCANCODE_BACKSLASH:
      return ScanCodes::BACKSLASH;
    case SDL_SCANCODE_BACKSPACE:
      return ScanCodes::BACKSPACE;
    case SDL_SCANCODE_BRIGHTNESSDOWN:
      return ScanCodes::BRIGHTNESSDOWN;
    case SDL_SCANCODE_BRIGHTNESSUP:
      return ScanCodes::BRIGHTNESSUP;
    case SDL_SCANCODE_C:
      return ScanCodes::C;
    case SDL_SCANCODE_CALCULATOR:
      return ScanCodes::CALCULATOR;
    case SDL_SCANCODE_CANCEL:
      return ScanCodes::CANCEL;
    case SDL_SCANCODE_CAPSLOCK:
      return ScanCodes::CAPSLOCK;
    case SDL_SCANCODE_CLEAR:
      return ScanCodes::CLEAR;
    case SDL_SCANCODE_CLEARAGAIN:
      return ScanCodes::CLEARAGAIN;
    case SDL_SCANCODE_COMMA:
      return ScanCodes::COMMA;
    case SDL_SCANCODE_COMPUTER:
      return ScanCodes::COMPUTER;
    case SDL_SCANCODE_COPY:
      return ScanCodes::COPY;
    case SDL_SCANCODE_CRSEL:
      return ScanCodes::CRSEL;
    case SDL_SCANCODE_CURRENCYSUBUNIT:
      return ScanCodes::CURRENCYSUBUNIT;
    case SDL_SCANCODE_CURRENCYUNIT:
      return ScanCodes::CURRENCYUNIT;
    case SDL_SCANCODE_CUT:
      return ScanCodes::CUT;
    case SDL_SCANCODE_D:
      return ScanCodes::D;
    case SDL_SCANCODE_DECIMALSEPARATOR:
      return ScanCodes::DECIMALSEPARATOR;
    case SDL_SCANCODE_DELETE:
      return ScanCodes::DELETE;
    case SDL_SCANCODE_DISPLAYSWITCH:
      return ScanCodes::DISPLAYSWITCH;
    case SDL_SCANCODE_DOWN:
      return ScanCodes::DOWN;
    case SDL_SCANCODE_E:
      return ScanCodes::E;
    case SDL_SCANCODE_EJECT:
      return ScanCodes::EJECT;
    case SDL_SCANCODE_END:
      return ScanCodes::END;
    case SDL_SCANCODE_EQUALS:
      return ScanCodes::EQUALS;
    case SDL_SCANCODE_ESCAPE:
      return ScanCodes::ESCAPE;
    case SDL_SCANCODE_EXECUTE:
      return ScanCodes::EXECUTE;
    case SDL_SCANCODE_EXSEL:
      return ScanCodes::EXSEL;
    case SDL_SCANCODE_F:
      return ScanCodes::F;
    case SDL_SCANCODE_F1:
      return ScanCodes::F1;
    case SDL_SCANCODE_F10:
      return ScanCodes::F10;
    case SDL_SCANCODE_F11:
      return ScanCodes::F11;
    case SDL_SCANCODE_F12:
      return ScanCodes::F12;
    case SDL_SCANCODE_F13:
      return ScanCodes::F13;
    case SDL_SCANCODE_F14:
      return ScanCodes::F14;
    case SDL_SCANCODE_F15:
      return ScanCodes::F15;
    case SDL_SCANCODE_F16:
      return ScanCodes::F16;
    case SDL_SCANCODE_F17:
      return ScanCodes::F17;
    case SDL_SCANCODE_F18:
      return ScanCodes::F18;
    case SDL_SCANCODE_F19:
      return ScanCodes::F19;
    case SDL_SCANCODE_F2:
      return ScanCodes::F2;
    case SDL_SCANCODE_F20:
      return ScanCodes::F20;
    case SDL_SCANCODE_F21:
      return ScanCodes::F21;
    case SDL_SCANCODE_F22:
      return ScanCodes::F22;
    case SDL_SCANCODE_F23:
      return ScanCodes::F23;
    case SDL_SCANCODE_F24:
      return ScanCodes::F24;
    case SDL_SCANCODE_F3:
      return ScanCodes::F3;
    case SDL_SCANCODE_F4:
      return ScanCodes::F4;
    case SDL_SCANCODE_F5:
      return ScanCodes::F5;
    case SDL_SCANCODE_F6:
      return ScanCodes::F6;
    case SDL_SCANCODE_F7:
      return ScanCodes::F7;
    case SDL_SCANCODE_F8:
      return ScanCodes::F8;
    case SDL_SCANCODE_F9:
      return ScanCodes::F9;
    case SDL_SCANCODE_FIND:
      return ScanCodes::FIND;
    case SDL_SCANCODE_G:
      return ScanCodes::G;
    case SDL_SCANCODE_GRAVE:
      return ScanCodes::GRAVE;
    case SDL_SCANCODE_H:
      return ScanCodes::H;
    case SDL_SCANCODE_HELP:
      return ScanCodes::HELP;
    case SDL_SCANCODE_HOME:
      return ScanCodes::HOME;
    case SDL_SCANCODE_I:
      return ScanCodes::I;
    case SDL_SCANCODE_INSERT:
      return ScanCodes::INSERT;
    case SDL_SCANCODE_J:
      return ScanCodes::J;
    case SDL_SCANCODE_K:
      return ScanCodes::K;
    case SDL_SCANCODE_KBDILLUMDOWN:
      return ScanCodes::KBDILLUMDOWN;
    case SDL_SCANCODE_KBDILLUMTOGGLE:
      return ScanCodes::KBDILLUMTOGGLE;
    case SDL_SCANCODE_KBDILLUMUP:
      return ScanCodes::KBDILLUMUP;
    case SDL_SCANCODE_KP_0:
      return ScanCodes::KP_0;
    case SDL_SCANCODE_KP_00:
      return ScanCodes::KP_00;
    case SDL_SCANCODE_KP_000:
      return ScanCodes::KP_000;
    case SDL_SCANCODE_KP_1:
      return ScanCodes::KP_1;
    case SDL_SCANCODE_KP_2:
      return ScanCodes::KP_2;
    case SDL_SCANCODE_KP_3:
      return ScanCodes::KP_3;
    case SDL_SCANCODE_KP_4:
      return ScanCodes::KP_4;
    case SDL_SCANCODE_KP_5:
      return ScanCodes::KP_5;
    case SDL_SCANCODE_KP_6:
      return ScanCodes::KP_6;
    case SDL_SCANCODE_KP_7:
      return ScanCodes::KP_7;
    case SDL_SCANCODE_KP_8:
      return ScanCodes::KP_8;
    case SDL_SCANCODE_KP_9:
      return ScanCodes::KP_9;
    case SDL_SCANCODE_KP_A:
      return ScanCodes::KP_A;
    case SDL_SCANCODE_KP_AMPERSAND:
      return ScanCodes::KP_AMPERSAND;
    case SDL_SCANCODE_KP_AT:
      return ScanCodes::KP_AT;
    case SDL_SCANCODE_KP_B:
      return ScanCodes::KP_B;
    case SDL_SCANCODE_KP_BACKSPACE:
      return ScanCodes::KP_BACKSPACE;
    case SDL_SCANCODE_KP_BINARY:
      return ScanCodes::KP_BINARY;
    case SDL_SCANCODE_KP_C:
      return ScanCodes::KP_C;
    case SDL_SCANCODE_KP_CLEAR:
      return ScanCodes::KP_CLEAR;
    case SDL_SCANCODE_KP_CLEARENTRY:
      return ScanCodes::KP_CLEARENTRY;
    case SDL_SCANCODE_KP_COLON:
      return ScanCodes::KP_COLON;
    case SDL_SCANCODE_KP_COMMA:
      return ScanCodes::KP_COMMA;
    case SDL_SCANCODE_KP_D:
      return ScanCodes::KP_D;
    case SDL_SCANCODE_KP_DBLAMPERSAND:
      return ScanCodes::KP_DBLAMPERSAND;
    case SDL_SCANCODE_KP_DBLVERTICALBAR:
      return ScanCodes::KP_DBLVERTICALBAR;
    case SDL_SCANCODE_KP_DECIMAL:
      return ScanCodes::KP_DECIMAL;
    case SDL_SCANCODE_KP_DIVIDE:
      return ScanCodes::KP_DIVIDE;
    case SDL_SCANCODE_KP_E:
      return ScanCodes::KP_E;
    case SDL_SCANCODE_KP_ENTER:
      return ScanCodes::KP_ENTER;
    case SDL_SCANCODE_KP_EQUALS:
      return ScanCodes::KP_EQUALS;
    case SDL_SCANCODE_KP_EQUALSAS400:
      return ScanCodes::KP_EQUALSAS400;
    case SDL_SCANCODE_KP_EXCLAM:
      return ScanCodes::KP_EXCLAM;
    case SDL_SCANCODE_KP_F:
      return ScanCodes::KP_F;
    case SDL_SCANCODE_KP_GREATER:
      return ScanCodes::KP_GREATER;
    case SDL_SCANCODE_KP_HASH:
      return ScanCodes::KP_HASH;
    case SDL_SCANCODE_KP_HEXADECIMAL:
      return ScanCodes::KP_HEXADECIMAL;
    case SDL_SCANCODE_KP_LEFTBRACE:
      return ScanCodes::KP_LEFTBRACE;
    case SDL_SCANCODE_KP_LEFTPAREN:
      return ScanCodes::KP_LEFTPAREN;
    case SDL_SCANCODE_KP_LESS:
      return ScanCodes::KP_LESS;
    case SDL_SCANCODE_KP_MEMADD:
      return ScanCodes::KP_MEMADD;
    case SDL_SCANCODE_KP_MEMCLEAR:
      return ScanCodes::KP_MEMCLEAR;
    case SDL_SCANCODE_KP_MEMDIVIDE:
      return ScanCodes::KP_MEMDIVIDE;
    case SDL_SCANCODE_KP_MEMMULTIPLY:
      return ScanCodes::KP_MEMMULTIPLY;
    case SDL_SCANCODE_KP_MEMRECALL:
      return ScanCodes::KP_MEMRECALL;
    case SDL_SCANCODE_KP_MEMSTORE:
      return ScanCodes::KP_MEMSTORE;
    case SDL_SCANCODE_KP_MEMSUBTRACT:
      return ScanCodes::KP_MEMSUBTRACT;
    case SDL_SCANCODE_KP_MINUS:
      return ScanCodes::KP_MINUS;
    case SDL_SCANCODE_KP_MULTIPLY:
      return ScanCodes::KP_MULTIPLY;
    case SDL_SCANCODE_KP_OCTAL:
      return ScanCodes::KP_OCTAL;
    case SDL_SCANCODE_KP_PERCENT:
      return ScanCodes::KP_PERCENT;
    case SDL_SCANCODE_KP_PERIOD:
      return ScanCodes::KP_PERIOD;
    case SDL_SCANCODE_KP_PLUS:
      return ScanCodes::KP_PLUS;
    case SDL_SCANCODE_KP_PLUSMINUS:
      return ScanCodes::KP_PLUSMINUS;
    case SDL_SCANCODE_KP_POWER:
      return ScanCodes::KP_POWER;
    case SDL_SCANCODE_KP_RIGHTBRACE:
      return ScanCodes::KP_RIGHTBRACE;
    case SDL_SCANCODE_KP_RIGHTPAREN:
      return ScanCodes::KP_RIGHTPAREN;
    case SDL_SCANCODE_KP_SPACE:
      return ScanCodes::KP_SPACE;
    case SDL_SCANCODE_KP_TAB:
      return ScanCodes::KP_TAB;
    case SDL_SCANCODE_KP_VERTICALBAR:
      return ScanCodes::KP_VERTICALBAR;
    case SDL_SCANCODE_KP_XOR:
      return ScanCodes::KP_XOR;
    case SDL_SCANCODE_L:
      return ScanCodes::L;
    case SDL_SCANCODE_LALT:
      return ScanCodes::LALT;
    case SDL_SCANCODE_LCTRL:
      return ScanCodes::LCTRL;
    case SDL_SCANCODE_LEFT:
      return ScanCodes::LEFT;
    case SDL_SCANCODE_LEFTBRACKET:
      return ScanCodes::LEFTBRACKET;
    case SDL_SCANCODE_LGUI:
      return ScanCodes::LGUI;
    case SDL_SCANCODE_LSHIFT:
      return ScanCodes::LSHIFT;
    case SDL_SCANCODE_M:
      return ScanCodes::M;
    case SDL_SCANCODE_MAIL:
      return ScanCodes::MAIL;
    case SDL_SCANCODE_MEDIASELECT:
      return ScanCodes::MEDIASELECT;
    case SDL_SCANCODE_MENU:
      return ScanCodes::MENU;
    case SDL_SCANCODE_MINUS:
      return ScanCodes::MINUS;
    case SDL_SCANCODE_MODE:
      return ScanCodes::MODE;
    case SDL_SCANCODE_MUTE:
      return ScanCodes::MUTE;
    case SDL_SCANCODE_N:
      return ScanCodes::N;
    case SDL_SCANCODE_NUMLOCKCLEAR:
      return ScanCodes::NUMLOCKCLEAR;
    case SDL_SCANCODE_O:
      return ScanCodes::O;
    case SDL_SCANCODE_OPER:
      return ScanCodes::OPER;
    case SDL_SCANCODE_OUT:
      return ScanCodes::OUT;
    case SDL_SCANCODE_P:
      return ScanCodes::P;
    case SDL_SCANCODE_PAGEDOWN:
      return ScanCodes::PAGEDOWN;
    case SDL_SCANCODE_PAGEUP:
      return ScanCodes::PAGEUP;
    case SDL_SCANCODE_PASTE:
      return ScanCodes::PASTE;
    case SDL_SCANCODE_PAUSE:
      return ScanCodes::PAUSE;
    case SDL_SCANCODE_PERIOD:
      return ScanCodes::PERIOD;
    case SDL_SCANCODE_POWER:
      return ScanCodes::POWER;
    case SDL_SCANCODE_PRINTSCREEN:
      return ScanCodes::PRINTSCREEN;
    case SDL_SCANCODE_PRIOR:
      return ScanCodes::PRIOR;
    case SDL_SCANCODE_Q:
      return ScanCodes::Q;
    case SDL_SCANCODE_R:
      return ScanCodes::R;
    case SDL_SCANCODE_RALT:
      return ScanCodes::RALT;
    case SDL_SCANCODE_RCTRL:
      return ScanCodes::RCTRL;
    case SDL_SCANCODE_RETURN:
      return ScanCodes::RETURN;
    case SDL_SCANCODE_RETURN2:
      return ScanCodes::RETURN2;
    case SDL_SCANCODE_RGUI:
      return ScanCodes::RGUI;
    case SDL_SCANCODE_RIGHT:
      return ScanCodes::RIGHT;
    case SDL_SCANCODE_RIGHTBRACKET:
      return ScanCodes::RIGHTBRACKET;
    case SDL_SCANCODE_RSHIFT:
      return ScanCodes::RSHIFT;
    case SDL_SCANCODE_S:
      return ScanCodes::S;
    case SDL_SCANCODE_SCROLLLOCK:
      return ScanCodes::SCROLLLOCK;
    case SDL_SCANCODE_SELECT:
      return ScanCodes::SELECT;
    case SDL_SCANCODE_SEMICOLON:
      return ScanCodes::SEMICOLON;
    case SDL_SCANCODE_SEPARATOR:
      return ScanCodes::SEPARATOR;
    case SDL_SCANCODE_SLASH:
      return ScanCodes::SLASH;
    case SDL_SCANCODE_SLEEP:
      return ScanCodes::SLEEP;
    case SDL_SCANCODE_SPACE:
      return ScanCodes::SPACE;
    case SDL_SCANCODE_STOP:
      return ScanCodes::STOP;
    case SDL_SCANCODE_SYSREQ:
      return ScanCodes::SYSREQ;
    case SDL_SCANCODE_T:
      return ScanCodes::T;
    case SDL_SCANCODE_TAB:
      return ScanCodes::TAB;
    case SDL_SCANCODE_THOUSANDSSEPARATOR:
      return ScanCodes::THOUSANDSSEPARATOR;
    case SDL_SCANCODE_U:
      return ScanCodes::U;
    case SDL_SCANCODE_UNDO:
      return ScanCodes::UNDO;
    case SDL_SCANCODE_UNKNOWN:
      return ScanCodes::UNKNOWN;
    case SDL_SCANCODE_UP:
      return ScanCodes::UP;
    case SDL_SCANCODE_V:
      return ScanCodes::V;
    case SDL_SCANCODE_VOLUMEDOWN:
      return ScanCodes::VOLUMEDOWN;
    case SDL_SCANCODE_VOLUMEUP:
      return ScanCodes::VOLUMEUP;
    case SDL_SCANCODE_W:
      return ScanCodes::W;
    case SDL_SCANCODE_WWW:
      return ScanCodes::WWW;
    case SDL_SCANCODE_X:
      return ScanCodes::X;
    case SDL_SCANCODE_Y:
      return ScanCodes::Y;
    case SDL_SCANCODE_Z:
      return ScanCodes::Z;
    case SDL_SCANCODE_INTERNATIONAL1:
      return ScanCodes::INTERNATIONAL1;
    case SDL_SCANCODE_INTERNATIONAL2:
      return ScanCodes::INTERNATIONAL2;
    case SDL_SCANCODE_INTERNATIONAL3:
      return ScanCodes::INTERNATIONAL3;
    case SDL_SCANCODE_INTERNATIONAL4:
      return ScanCodes::INTERNATIONAL4;
    case SDL_SCANCODE_INTERNATIONAL5:
      return ScanCodes::INTERNATIONAL5;
    case SDL_SCANCODE_INTERNATIONAL6:
      return ScanCodes::INTERNATIONAL6;
    case SDL_SCANCODE_INTERNATIONAL7:
      return ScanCodes::INTERNATIONAL7;
    case SDL_SCANCODE_INTERNATIONAL8:
      return ScanCodes::INTERNATIONAL8;
    case SDL_SCANCODE_INTERNATIONAL9:
      return ScanCodes::INTERNATIONAL9;
    case SDL_SCANCODE_LANG1:
      return ScanCodes::LANG1;
    case SDL_SCANCODE_LANG2:
      return ScanCodes::LANG2;
    case SDL_SCANCODE_LANG3:
      return ScanCodes::LANG3;
    case SDL_SCANCODE_LANG4:
      return ScanCodes::LANG4;
    case SDL_SCANCODE_LANG5:
      return ScanCodes::LANG5;
    case SDL_SCANCODE_LANG6:
      return ScanCodes::LANG6;
    case SDL_SCANCODE_LANG7:
      return ScanCodes::LANG7;
    case SDL_SCANCODE_LANG8:
      return ScanCodes::LANG8;
    case SDL_SCANCODE_LANG9:
      return ScanCodes::LANG9;
    case SDL_SCANCODE_NONUSBACKSLASH:
      return ScanCodes::NONUSBACKSLASH;
    case SDL_SCANCODE_NONUSHASH:
      return ScanCodes::NONUSHASH;
    case SDL_SCANCODE_APP1:
      return ScanCodes::APP1;
    case SDL_SCANCODE_APP2:
      return ScanCodes::APP2;
    case SDL_SCANCODE_AUDIOREWIND:
      return ScanCodes::AUDIOREWIND;
    case SDL_SCANCODE_AUDIOFASTFORWARD:
      return ScanCodes::AUDIOFASTFORWARD;
    case SDL_NUM_SCANCODES:
      return ScanCodes::NONE;
  }
  return ScanCodes::NONE;
}

static KeyCodes getKeyCode(const SDL_Keycode code) noexcept {
  switch (code) {
    case SDLK_0:
      return KeyCodes::NUMBER_0;
    case SDLK_1:
      return KeyCodes::NUMBER_1;
    case SDLK_2:
      return KeyCodes::NUMBER_2;
    case SDLK_3:
      return KeyCodes::NUMBER_3;
    case SDLK_4:
      return KeyCodes::NUMBER_4;
    case SDLK_5:
      return KeyCodes::NUMBER_5;
    case SDLK_6:
      return KeyCodes::NUMBER_6;
    case SDLK_7:
      return KeyCodes::NUMBER_7;
    case SDLK_8:
      return KeyCodes::NUMBER_8;
    case SDLK_9:
      return KeyCodes::NUMBER_9;
    case SDLK_a:
      return KeyCodes::a;
    case SDLK_AC_BACK:
      return KeyCodes::AC_BACK;
    case SDLK_AC_BOOKMARKS:
      return KeyCodes::AC_BOOKMARKS;
    case SDLK_AC_FORWARD:
      return KeyCodes::AC_FORWARD;
    case SDLK_AC_HOME:
      return KeyCodes::AC_HOME;
    case SDLK_AC_REFRESH:
      return KeyCodes::AC_REFRESH;
    case SDLK_AC_SEARCH:
      return KeyCodes::AC_SEARCH;
    case SDLK_AC_STOP:
      return KeyCodes::AC_STOP;
    case SDLK_AGAIN:
      return KeyCodes::AGAIN;
    case SDLK_ALTERASE:
      return KeyCodes::ALTERASE;
    case SDLK_QUOTE:
      return KeyCodes::QUOTE;
    case SDLK_APPLICATION:
      return KeyCodes::APPLICATION;
    case SDLK_AUDIOMUTE:
      return KeyCodes::AUDIOMUTE;
    case SDLK_AUDIONEXT:
      return KeyCodes::AUDIONEXT;
    case SDLK_AUDIOPLAY:
      return KeyCodes::AUDIOPLAY;
    case SDLK_AUDIOPREV:
      return KeyCodes::AUDIOPREV;
    case SDLK_AUDIOSTOP:
      return KeyCodes::AUDIOSTOP;
    case SDLK_b:
      return KeyCodes::b;
    case SDLK_BACKSLASH:
      return KeyCodes::BACKSLASH;
    case SDLK_BACKSPACE:
      return KeyCodes::BACKSPACE;
    case SDLK_BRIGHTNESSDOWN:
      return KeyCodes::BRIGHTNESSDOWN;
    case SDLK_BRIGHTNESSUP:
      return KeyCodes::BRIGHTNESSUP;
    case SDLK_c:
      return KeyCodes::c;
    case SDLK_CALCULATOR:
      return KeyCodes::CALCULATOR;
    case SDLK_CANCEL:
      return KeyCodes::CANCEL;
    case SDLK_CAPSLOCK:
      return KeyCodes::CAPSLOCK;
    case SDLK_CLEAR:
      return KeyCodes::CLEAR;
    case SDLK_CLEARAGAIN:
      return KeyCodes::CLEARAGAIN;
    case SDLK_COMMA:
      return KeyCodes::COMMA;
    case SDLK_COMPUTER:
      return KeyCodes::COMPUTER;
    case SDLK_COPY:
      return KeyCodes::COPY;
    case SDLK_CRSEL:
      return KeyCodes::CRSEL;
    case SDLK_CURRENCYSUBUNIT:
      return KeyCodes::CURRENCYSUBUNIT;
    case SDLK_CURRENCYUNIT:
      return KeyCodes::CURRENCYUNIT;
    case SDLK_CUT:
      return KeyCodes::CUT;
    case SDLK_d:
      return KeyCodes::d;
    case SDLK_DECIMALSEPARATOR:
      return KeyCodes::DECIMALSEPARATOR;
    case SDLK_DELETE:
      return KeyCodes::DELETE;
    case SDLK_DISPLAYSWITCH:
      return KeyCodes::DISPLAYSWITCH;
    case SDLK_DOWN:
      return KeyCodes::DOWN;
    case SDLK_e:
      return KeyCodes::e;
    case SDLK_EJECT:
      return KeyCodes::EJECT;
    case SDLK_END:
      return KeyCodes::END;
    case SDLK_EQUALS:
      return KeyCodes::EQUALS;
    case SDLK_ESCAPE:
      return KeyCodes::ESCAPE;
    case SDLK_EXECUTE:
      return KeyCodes::EXECUTE;
    case SDLK_EXSEL:
      return KeyCodes::EXSEL;
    case SDLK_f:
      return KeyCodes::f;
    case SDLK_F1:
      return KeyCodes::F1;
    case SDLK_F10:
      return KeyCodes::F10;
    case SDLK_F11:
      return KeyCodes::F11;
    case SDLK_F12:
      return KeyCodes::F12;
    case SDLK_F13:
      return KeyCodes::F13;
    case SDLK_F14:
      return KeyCodes::F14;
    case SDLK_F15:
      return KeyCodes::F15;
    case SDLK_F16:
      return KeyCodes::F16;
    case SDLK_F17:
      return KeyCodes::F17;
    case SDLK_F18:
      return KeyCodes::F18;
    case SDLK_F19:
      return KeyCodes::F19;
    case SDLK_F2:
      return KeyCodes::F2;
    case SDLK_F20:
      return KeyCodes::F20;
    case SDLK_F21:
      return KeyCodes::F21;
    case SDLK_F22:
      return KeyCodes::F22;
    case SDLK_F23:
      return KeyCodes::F23;
    case SDLK_F24:
      return KeyCodes::F24;
    case SDLK_F3:
      return KeyCodes::F3;
    case SDLK_F4:
      return KeyCodes::F4;
    case SDLK_F5:
      return KeyCodes::F5;
    case SDLK_F6:
      return KeyCodes::F6;
    case SDLK_F7:
      return KeyCodes::F7;
    case SDLK_F8:
      return KeyCodes::F8;
    case SDLK_F9:
      return KeyCodes::F9;
    case SDLK_FIND:
      return KeyCodes::FIND;
    case SDLK_g:
      return KeyCodes::g;
    case SDLK_BACKQUOTE:
      return KeyCodes::BACKQUOTE;
    case SDLK_h:
      return KeyCodes::h;
    case SDLK_HELP:
      return KeyCodes::HELP;
    case SDLK_HOME:
      return KeyCodes::HOME;
    case SDLK_i:
      return KeyCodes::i;
    case SDLK_INSERT:
      return KeyCodes::INSERT;
    case SDLK_j:
      return KeyCodes::j;
    case SDLK_k:
      return KeyCodes::k;
    case SDLK_KBDILLUMDOWN:
      return KeyCodes::KBDILLUMDOWN;
    case SDLK_KBDILLUMTOGGLE:
      return KeyCodes::KBDILLUMTOGGLE;
    case SDLK_KBDILLUMUP:
      return KeyCodes::KBDILLUMUP;
    case SDLK_KP_0:
      return KeyCodes::KP_0;
    case SDLK_KP_00:
      return KeyCodes::KP_00;
    case SDLK_KP_000:
      return KeyCodes::KP_000;
    case SDLK_KP_1:
      return KeyCodes::KP_1;
    case SDLK_KP_2:
      return KeyCodes::KP_2;
    case SDLK_KP_3:
      return KeyCodes::KP_3;
    case SDLK_KP_4:
      return KeyCodes::KP_4;
    case SDLK_KP_5:
      return KeyCodes::KP_5;
    case SDLK_KP_6:
      return KeyCodes::KP_6;
    case SDLK_KP_7:
      return KeyCodes::KP_7;
    case SDLK_KP_8:
      return KeyCodes::KP_8;
    case SDLK_KP_9:
      return KeyCodes::KP_9;
    case SDLK_KP_A:
      return KeyCodes::KP_A;
    case SDLK_KP_AMPERSAND:
      return KeyCodes::KP_AMPERSAND;
    case SDLK_KP_AT:
      return KeyCodes::KP_AT;
    case SDLK_KP_B:
      return KeyCodes::KP_B;
    case SDLK_KP_BACKSPACE:
      return KeyCodes::KP_BACKSPACE;
    case SDLK_KP_BINARY:
      return KeyCodes::KP_BINARY;
    case SDLK_KP_C:
      return KeyCodes::KP_C;
    case SDLK_KP_CLEAR:
      return KeyCodes::KP_CLEAR;
    case SDLK_KP_CLEARENTRY:
      return KeyCodes::KP_CLEARENTRY;
    case SDLK_KP_COLON:
      return KeyCodes::KP_COLON;
    case SDLK_KP_COMMA:
      return KeyCodes::KP_COMMA;
    case SDLK_KP_D:
      return KeyCodes::KP_D;
    case SDLK_KP_DBLAMPERSAND:
      return KeyCodes::KP_DBLAMPERSAND;
    case SDLK_KP_DBLVERTICALBAR:
      return KeyCodes::KP_DBLVERTICALBAR;
    case SDLK_KP_DECIMAL:
      return KeyCodes::KP_DECIMAL;
    case SDLK_KP_DIVIDE:
      return KeyCodes::KP_DIVIDE;
    case SDLK_KP_E:
      return KeyCodes::KP_E;
    case SDLK_KP_ENTER:
      return KeyCodes::KP_ENTER;
    case SDLK_KP_EQUALS:
      return KeyCodes::KP_EQUALS;
    case SDLK_KP_EQUALSAS400:
      return KeyCodes::KP_EQUALSAS400;
    case SDLK_KP_EXCLAM:
      return KeyCodes::KP_EXCLAM;
    case SDLK_KP_F:
      return KeyCodes::KP_F;
    case SDLK_KP_GREATER:
      return KeyCodes::KP_GREATER;
    case SDLK_KP_HASH:
      return KeyCodes::KP_HASH;
    case SDLK_KP_HEXADECIMAL:
      return KeyCodes::KP_HEXADECIMAL;
    case SDLK_KP_LEFTBRACE:
      return KeyCodes::KP_LEFTBRACE;
    case SDLK_KP_LEFTPAREN:
      return KeyCodes::KP_LEFTPAREN;
    case SDLK_KP_LESS:
      return KeyCodes::KP_LESS;
    case SDLK_KP_MEMADD:
      return KeyCodes::KP_MEMADD;
    case SDLK_KP_MEMCLEAR:
      return KeyCodes::KP_MEMCLEAR;
    case SDLK_KP_MEMDIVIDE:
      return KeyCodes::KP_MEMDIVIDE;
    case SDLK_KP_MEMMULTIPLY:
      return KeyCodes::KP_MEMMULTIPLY;
    case SDLK_KP_MEMRECALL:
      return KeyCodes::KP_MEMRECALL;
    case SDLK_KP_MEMSTORE:
      return KeyCodes::KP_MEMSTORE;
    case SDLK_KP_MEMSUBTRACT:
      return KeyCodes::KP_MEMSUBTRACT;
    case SDLK_KP_MINUS:
      return KeyCodes::KP_MINUS;
    case SDLK_KP_MULTIPLY:
      return KeyCodes::KP_MULTIPLY;
    case SDLK_KP_OCTAL:
      return KeyCodes::KP_OCTAL;
    case SDLK_KP_PERCENT:
      return KeyCodes::KP_PERCENT;
    case SDLK_KP_PERIOD:
      return KeyCodes::KP_PERIOD;
    case SDLK_KP_PLUS:
      return KeyCodes::KP_PLUS;
    case SDLK_KP_PLUSMINUS:
      return KeyCodes::KP_PLUSMINUS;
    case SDLK_KP_POWER:
      return KeyCodes::KP_POWER;
    case SDLK_KP_RIGHTBRACE:
      return KeyCodes::KP_RIGHTBRACE;
    case SDLK_KP_RIGHTPAREN:
      return KeyCodes::KP_RIGHTPAREN;
    case SDLK_KP_SPACE:
      return KeyCodes::KP_SPACE;
    case SDLK_KP_TAB:
      return KeyCodes::KP_TAB;
    case SDLK_KP_VERTICALBAR:
      return KeyCodes::KP_VERTICALBAR;
    case SDLK_KP_XOR:
      return KeyCodes::KP_XOR;
    case SDLK_l:
      return KeyCodes::l;
    case SDLK_LALT:
      return KeyCodes::LALT;
    case SDLK_LCTRL:
      return KeyCodes::LCTRL;
    case SDLK_LEFT:
      return KeyCodes::LEFT;
    case SDLK_LEFTBRACKET:
      return KeyCodes::LEFTBRACKET;
    case SDLK_LGUI:
      return KeyCodes::LGUI;
    case SDLK_LSHIFT:
      return KeyCodes::LSHIFT;
    case SDLK_m:
      return KeyCodes::m;
    case SDLK_MAIL:
      return KeyCodes::MAIL;
    case SDLK_MEDIASELECT:
      return KeyCodes::MEDIASELECT;
    case SDLK_MENU:
      return KeyCodes::MENU;
    case SDLK_MINUS:
      return KeyCodes::MINUS;
    case SDLK_MODE:
      return KeyCodes::MODE;
    case SDLK_MUTE:
      return KeyCodes::MUTE;
    case SDLK_n:
      return KeyCodes::n;
    case SDLK_NUMLOCKCLEAR:
      return KeyCodes::NUMLOCKCLEAR;
    case SDLK_o:
      return KeyCodes::o;
    case SDLK_OPER:
      return KeyCodes::OPER;
    case SDLK_OUT:
      return KeyCodes::OUT;
    case SDLK_p:
      return KeyCodes::p;
    case SDLK_PAGEDOWN:
      return KeyCodes::PAGEDOWN;
    case SDLK_PAGEUP:
      return KeyCodes::PAGEUP;
    case SDLK_PASTE:
      return KeyCodes::PASTE;
    case SDLK_PAUSE:
      return KeyCodes::PAUSE;
    case SDLK_PERIOD:
      return KeyCodes::PERIOD;
    case SDLK_POWER:
      return KeyCodes::POWER;
    case SDLK_PRINTSCREEN:
      return KeyCodes::PRINTSCREEN;
    case SDLK_PRIOR:
      return KeyCodes::PRIOR;
    case SDLK_q:
      return KeyCodes::q;
    case SDLK_r:
      return KeyCodes::r;
    case SDLK_RALT:
      return KeyCodes::RALT;
    case SDLK_RCTRL:
      return KeyCodes::RCTRL;
    case SDLK_RETURN:
      return KeyCodes::RETURN;
    case SDLK_RETURN2:
      return KeyCodes::RETURN2;
    case SDLK_RGUI:
      return KeyCodes::RGUI;
    case SDLK_RIGHT:
      return KeyCodes::RIGHT;
    case SDLK_RIGHTBRACKET:
      return KeyCodes::RIGHTBRACKET;
    case SDLK_RSHIFT:
      return KeyCodes::RSHIFT;
    case SDLK_s:
      return KeyCodes::s;
    case SDLK_SCROLLLOCK:
      return KeyCodes::SCROLLLOCK;
    case SDLK_SELECT:
      return KeyCodes::SELECT;
    case SDLK_SEMICOLON:
      return KeyCodes::SEMICOLON;
    case SDLK_SEPARATOR:
      return KeyCodes::SEPARATOR;
    case SDLK_SLASH:
      return KeyCodes::SLASH;
    case SDLK_SLEEP:
      return KeyCodes::SLEEP;
    case SDLK_SPACE:
      return KeyCodes::SPACE;
    case SDLK_STOP:
      return KeyCodes::STOP;
    case SDLK_SYSREQ:
      return KeyCodes::SYSREQ;
    case SDLK_t:
      return KeyCodes::t;
    case SDLK_TAB:
      return KeyCodes::TAB;
    case SDLK_THOUSANDSSEPARATOR:
      return KeyCodes::THOUSANDSSEPARATOR;
    case SDLK_u:
      return KeyCodes::u;
    case SDLK_UNDO:
      return KeyCodes::UNDO;
    case SDLK_UNKNOWN:
      return KeyCodes::UNKNOWN;
    case SDLK_UP:
      return KeyCodes::UP;
    case SDLK_v:
      return KeyCodes::v;
    case SDLK_VOLUMEDOWN:
      return KeyCodes::VOLUMEDOWN;
    case SDLK_VOLUMEUP:
      return KeyCodes::VOLUMEUP;
    case SDLK_w:
      return KeyCodes::w;
    case SDLK_WWW:
      return KeyCodes::WWW;
    case SDLK_x:
      return KeyCodes::x;
    case SDLK_y:
      return KeyCodes::y;
    case SDLK_z:
      return KeyCodes::z;
    case SDLK_AMPERSAND:
      return KeyCodes::AMPERSAND;
    case SDLK_ASTERISK:
      return KeyCodes::ASTERISK;
    case SDLK_AT:
      return KeyCodes::AT;
    case SDLK_CARET:
      return KeyCodes::CARET;
    case SDLK_COLON:
      return KeyCodes::COLON;
    case SDLK_DOLLAR:
      return KeyCodes::DOLLAR;
    case SDLK_EXCLAIM:
      return KeyCodes::EXCLAIM;
    case SDLK_GREATER:
      return KeyCodes::GREATER;
    case SDLK_HASH:
      return KeyCodes::HASH;
    case SDLK_LEFTPAREN:
      return KeyCodes::LEFTPAREN;
    case SDLK_LESS:
      return KeyCodes::LESS;
    case SDLK_PERCENT:
      return KeyCodes::PERCENT;
    case SDLK_PLUS:
      return KeyCodes::PLUS;
    case SDLK_QUESTION:
      return KeyCodes::QUESTION;
    case SDLK_QUOTEDBL:
      return KeyCodes::QUOTEDBL;
    case SDLK_RIGHTPAREN:
      return KeyCodes::RIGHTPAREN;
    case SDLK_UNDERSCORE:
      return KeyCodes::UNDERSCORE;
    case SDLK_APP1:
      return KeyCodes::APP1;
    case SDLK_APP2:
      return KeyCodes::APP2;
    case SDLK_AUDIOREWIND:
      return KeyCodes::AUDIOREWIND;
    case SDLK_AUDIOFASTFORWARD:
      return KeyCodes::AUDIOFASTFORWARD;
  }
  return KeyCodes::NONE;
}

static WindowEvents getWindowEvent(const uint8_t type) noexcept {
  switch (type) {
    case SDL_WINDOWEVENT_SHOWN:
      return WindowEvents::SHOWN;
    case SDL_WINDOWEVENT_HIDDEN:
      return WindowEvents::HIDDEN;
    case SDL_WINDOWEVENT_EXPOSED:
      return WindowEvents::EXPOSED;
    case SDL_WINDOWEVENT_MOVED:
      return WindowEvents::MOVED;
    case SDL_WINDOWEVENT_RESIZED:
      return WindowEvents::RESIZED;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      return WindowEvents::SIZE_CHANGED;
    case SDL_WINDOWEVENT_MINIMIZED:
      return WindowEvents::MINIMIZED;
    case SDL_WINDOWEVENT_MAXIMIZED:
      return WindowEvents::MAXIMIZED;
    case SDL_WINDOWEVENT_RESTORED:
      return WindowEvents::RESTORED;
    case SDL_WINDOWEVENT_ENTER:
      return WindowEvents::ENTERED;
    case SDL_WINDOWEVENT_LEAVE:
      return WindowEvents::LEAVED;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      return WindowEvents::FOCUS_GAINED;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      return WindowEvents::FOCUS_LOST;
    case SDL_WINDOWEVENT_CLOSE:
      return WindowEvents::CLOSED;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
    case SDL_WINDOWEVENT_HIT_TEST:
    case SDL_WINDOWEVENT_NONE:
      return WindowEvents::NONE;
  }
  return WindowEvents::NONE;
}

static KeyboardStates getKeyboardState(const uint8_t state) noexcept {
  switch (state) {
    case SDL_PRESSED:
      return KeyboardStates::PRESSED;
    case SDL_RELEASED:
      return KeyboardStates::RELEASED;
  }
  return KeyboardStates::NONE;
}

static MouseButtonStates getMouseButtonState(const uint8_t state) noexcept {
  if (state & SDL_PRESSED) {
    return MouseButtonStates::PRESSED;
  }
  return MouseButtonStates::RELEASED;
}

static MouseButtons getMouseButton(const uint8_t button) noexcept {
  if (button & SDL_BUTTON_LEFT) {
    return MouseButtons::LEFT;
  } else if (button & SDL_BUTTON_RIGHT) {
    return MouseButtons::RIGHT;
  } else if (button & SDL_BUTTON_MIDDLE) {
    return MouseButtons::MIDDLE;
  } else if (button & SDL_BUTTON_X1) {
    return MouseButtons::X1;
  }
  return MouseButtons::X2;
}

static MouseClickTypes getMouseClickType(const uint8_t clicks) noexcept {
  if (clicks > 1) {
    return MouseClickTypes::DOUBLE;
  }
  return MouseClickTypes::SINGLE;
}

static MouseWheelDirections getMouseWheelDirection(
    const uint32_t direction) noexcept {
  if (direction & SDL_MOUSEWHEEL_NORMAL) {
    return MouseWheelDirections::NORMAL;
  }
  return MouseWheelDirections ::FLIPPED;
}

}  // namespace dana
