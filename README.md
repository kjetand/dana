# Dana
A small and cute 2D graphics library for modern C++ which aims to provide an easy entrance to basic 2D graphics programming. The framework is heavily based on NanoVG and uses a similar API interface as HTML5 Canvas.

## Example
```cpp
#include <dana.h>

int main() {
  dana::Canvas canvas(500, 500, "Title");
  canvas.onNewFrame([](dana::Pencil& pencil) {
    pencil.beginPath()
      .setStrokeColor({ 255, 0, 0, 255 });
      .moveTo(10, 10);
      .lineTo(100, 10);
      .stroke();
    
    pencil.beginPath();
    // Render more stuff...
  })
  .onEvent([](const dana::Event& event) {
    handleEventIf<dana::WindowEvent>(event, [](const auto& window_event) {
      // Handle window events...
    });
  })
  .show();
}
```

## Features
- [x] Paths: lines, bezier curves and arcs
- [x] Shapes: Rectangles and circles
- [x] Colors and gradients
- [x] Basic transformations
- [x] Image patterns
- [ ] Font and text rendering - _not yet implemented_

## Dependencies
Dana is currently a wrapper of the popular C/C++ libraries below:
- [NanoVG](https://github.com/memononen/nanovg/) for anti-aliased vector drawings
- [SDL2](https://www.libsdl.org/) for window management
- [Glew](http://glew.sourceforge.net/) for OpenGL
- [GTest](https://github.com/google/googletest/) for unit testing

## License
Dana is licenced under the [MIT License](LICENCE).

**Note** dependencies have the following licencing:

- [SDL2](https://www.libsdl.org/) is licenced under [zlib](https://www.zlib.net/zlib_license.html)
- [Glew](http://glew.sourceforge.net/) is licenced under the [Modified BSD License](http://glew.sourceforge.net/glew.txt), the [Mesa 3-D License](http://glew.sourceforge.net/mesa.txt) and the [Khronos License](http://glew.sourceforge.net/khronos.txt)
- [NanoVG](https://github.com/memononen/nanovg) is licenced under [zlib](https://www.zlib.net/zlib_license.html)
