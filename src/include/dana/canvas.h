#pragma once

#include "dana/events.h"
#include "dana/pencil.h"
#include "dana/types.h"
#include "dana/util.h"

#include <functional>
#include <string>

struct SDL_Window;
union SDL_Event;

namespace dana {

/// The callback type used to draw graphics onto a canvas.
using DrawCallback = std::function<void(Pencil&)>;

/// The callback type used to handle keyboard, mouse and window events.
using EventCallback = std::function<void(const Event&)>;

class Canvas {
  c_unique_ptr<SDL_Window> m_window{nullptr};
  c_unique_ptr<void> m_gl_context{nullptr};

  DrawCallback m_draw_callback{[](Pencil&) {}};
  EventCallback m_event_callback{[](const Event&) {}};

  bool m_show{true};
  Color m_clear_color;
  long m_performance{0};

 public:
  /// Constructs a canvas window with a given width and height, and a title
  /// text.
  Canvas(int width, int height, const std::string& title);

  ~Canvas() noexcept;

  /// Sets the clear color that is used to clear the screen for each frame
  /// before graphics are drawn.
  Canvas& setClearColor(const Color& clear_color) noexcept;

  /// Sets the graphics callback that is called for each frame.
  Canvas& onNewFrame(const DrawCallback& draw_callback) noexcept;

  /// Sets the event handler callback that is used to catch keyboard, mouse and
  /// window events.
  Canvas& onEvent(const EventCallback& event_callback) noexcept;

  /// Shows the canvas window on screen.
  void show() noexcept;

  /// Returns the number of milliseconds it took to render the previous frame.
  /// Can be used to measure performance of your application.
  long getPerformance() const noexcept;

 private:
  void pollEvents(SDL_Event& event) noexcept;

  void clearWindow() const noexcept;
};
}  // namespace dana
