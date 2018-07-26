#include <dana.h>

#include <iostream>
#include <variant>

using namespace dana;

void drawLines(const float x, const float y, Pencil& pencil) noexcept {
  pencil.save()
      .translate(x, y)
      .setAntiAlias(true)
      .setStrokeWidth(2)

      .beginPath()
      .setStrokeColor({255, 0, 0, 255})
      .moveTo(10, 10)
      .lineTo(100, 10)
      .stroke()

      .setStrokeWidth(4)

      .beginPath()
      .setStrokeColor({0, 255, 0, 255})
      .moveTo(10, 20)
      .lineTo(100, 20)
      .stroke()

      .beginPath()
      .setStrokeColor({0, 0, 255, 255})
      .setStrokeWidth(6)
      .moveTo(10, 30)
      .lineTo(100, 30)
      .stroke()

      .restore();
}

void drawQuadCurve(const float x, const float y, Pencil& pencil) noexcept {
  pencil.save()
      .translate(x, y)
      .beginPath()
      .moveTo(0, 10)
      .quadTo(50, 50, 100, 10)
      .stroke()

      .restore();
}

void drawBezierCurve(const float x, const float y, Pencil& pencil) noexcept {
  pencil.save()
      .translate(x, y)
      .setFillColor({0, 0, 0, 255})
      .setStrokeWidth(1.5f)

      .beginPath()
      .moveTo(0, 25)
      .bezierTo(25, 75, 75, 0, 100, 25)
      .stroke()

      .restore();
}

void drawImage(const float x, const float y, Pencil& pencil) noexcept {
  const float width{220};
  const float height{220};

  pencil.save();
  static const auto image{pencil.createImage(
      "lenna.jpg", ImageFlags::IMAGE_GENERATE_MIPMAPS |
                       ImageFlags::IMAGE_REPEATX | ImageFlags::IMAGE_REPEATY)};
  const auto image_pattern{
      pencil.createImagePattern(image, x, y, width, height, 0.3f, 255)};

  pencil.beginPath()
      .rectangle(x, y, width + 100, height + 100)
      .setFillPaint(image_pattern)
      .fill()
      .restore();
}

void drawShapes(const float x, const float y, Pencil& pencil) noexcept {
  pencil.save()
      .beginPath()
      .circle(x, y, 100)
      .setFillColor({123, 245, 35, 123})
      .setStrokeColor({25, 65, 211, 255})
      .setStrokeWidth(5)
      .fill()
      .stroke()
      .restore();
}

void drawRoundedRectangle(float x, float y, float mouse_x, float mouse_y,
                          Pencil& pencil) {
  const unsigned char color_value{
      static_cast<unsigned char>(static_cast<int>(mouse_x + mouse_y) % 255)};
  // Some random color behaviour based on mouse position
  Color color;
  if (color_value < 80) {
    color = {color_value, 0, 0, 255};
  } else if (color_value < 180) {
    color = {color_value, static_cast<unsigned char>(color_value / 2), 0, 255};
  } else {
    color = {static_cast<unsigned char>(color_value * 2),
             static_cast<unsigned char>(color_value / 4), color_value, 255};
  }
  pencil.save()
      .beginPath()
      .setFillColor(color)
      .roundedRectangle(x, y, 50, 50, 10)
      .fill()
      .restore();
}

void handleEvent(const WindowEvent& event) noexcept {
  if (event.type == WindowEvents::FOCUS_GAINED) {
    std::cout << "Focus gained!" << std::endl;
  }
  if (event.type == WindowEvents::FOCUS_LOST) {
    std::cout << "Focus lost!" << std::endl;
  }
}

void handleEvent(const KeyboardEvent& event) noexcept {
  if (event.hasModifier(KeyboardModifiers::SHIFT)) {
    std::cout << "Shift as modifier!" << std::endl;
  }
  if (event.key == KeyCodes::a && event.state == KeyboardStates::PRESSED) {
    std::cout << "The letter 'a' was pressed!" << std::endl;
  }
  if (event.state == KeyboardStates::RELEASED) {
    std::cout << "Key released!" << std::endl;
  }
}

void handleEvent(const MouseButtonEvent& event) noexcept {
  if (event.click_type == MouseClickTypes::DOUBLE &&
      event.state == MouseButtonStates::RELEASED) {
    std::cout << "Double click at (" << event.x << ", " << event.y << ")"
              << std::endl;
  }
}

int main() noexcept {
  Canvas canvas(500, 500, "Canvas");
  canvas.setClearColor({255, 255, 255, 255});

  float mouse_x{0};
  float mouse_y{0};

  canvas.onEvent([&](const Event& event) {
    handleEventIf<WindowEvent>(
        event, [](const auto& window_event) { handleEvent(window_event); });
    handleEventIf<KeyboardEvent>(
        event, [](const auto& keyboard_event) { handleEvent(keyboard_event); });
    handleEventIf<MouseButtonEvent>(event, [](const auto& mouse_button_event) {
      handleEvent(mouse_button_event);
    });
    handleEventIf<MouseMotionEvent>(event, [&](const auto& mouse_motion_event) {
      mouse_x = mouse_motion_event.x;
      mouse_y = mouse_motion_event.y;
    });
  });

  canvas.onNewFrame([&](Pencil& pencil) {
    drawLines(0, 0, pencil);
    drawQuadCurve(120, 0, pencil);
    drawBezierCurve(230, 0, pencil);
    drawImage(20, 140, pencil);
    drawShapes(300, 300, pencil);
    drawRoundedRectangle(360, 10, mouse_x, mouse_y, pencil);
  });

  canvas.show();
  return 0;
}
