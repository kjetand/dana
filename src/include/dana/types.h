#pragma once

#include <utility>

namespace dana {

using ImageHandle = int;

using Extent = std::pair<float, float>;

struct TransformMatrix {
  float horizontal_scaling;
  float horizontal_skewing;
  float vertical_skewing;
  float vertical_scaling;
  float horizontal_moving;
  float vertical_moving;
};

struct Color {
  unsigned char r{0};
  unsigned char g{0};
  unsigned char b{0};
  unsigned char a{255};
};

struct Paint {
  TransformMatrix transform;
  Extent extent;
  float radius;
  float feather;
  Color inner_color;
  Color outer_color;
  ImageHandle image;
};

enum class CompositeOperations {
  SOURCE_OVER,
  SOURCE_IN,
  SOURCE_OUT,
  ATOP,
  DESTINATION_OVER,
  DESTINATION_IN,
  DESTINATION_OUT,
  DESTINATION_ATOP,
  LIGHTER,
  COPY,
  XOR
};

enum class BlendFactors {
  ZERO,
  ONE,
  SRC_COLOR,
  ONE_MINUS_SRC_COLOR,
  DST_COLOR,
  ONE_MINUS_DST_COLOR,
  SRC_ALPHA,
  ONE_MINUS_SRC_ALPHA,
  DST_ALPHA,
  ONE_MINUS_DST_ALPHA,
  SRC_ALPHA_SATURATE
};

enum class LineCap { BUTT, ROUND, SQUARE };

enum class LineJoin { MITER, ROUND, BEVEL };

enum class Solidity { SOLID, HOLE };

enum class Direction { CLOCKWISE, COUNTER_CLOCKWISE };

enum ImageFlags {
  IMAGE_GENERATE_MIPMAPS = 1 << 0,
  IMAGE_REPEATX = 1 << 1,
  IMAGE_REPEATY = 1 << 2,
  IMAGE_FLIPY = 1 << 3,
  IMAGE_PREMULTIPLIED = 1 << 4,
  IMAGE_NEAREST = 1 << 5
};
}  // namespace dana
