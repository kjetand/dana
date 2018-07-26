#pragma once

#include "dana/image.h"
#include "dana/types.h"

#include <memory>
#include <string>

struct NVGcontext;

namespace dana {

class Pencil {
  std::shared_ptr<NVGcontext> m_context{nullptr};

 public:
  explicit Pencil();

  /// \brief Begin drawing a frame. Has to be called before calling other
  /// methods. The pixel ratio is for Hi-DPI devices and is typically calculated
  /// as frameBufferWidth / windowWidth.
  Pencil& beginFrame(float width, float height, float pixel_ratio) noexcept;

  /// \brief Cancels drawing the current frame.
  Pencil& cancelFrame() noexcept;

  /// \brief Ends drawing frame and flushing remainder render state.
  Pencil& endFrame() noexcept;

  /// \brief Sets the global composite operation.
  Pencil& setGlobalCompositeOperation(
      CompositeOperations composite_operation) noexcept;

  /// \brief Sets the composite operation with custom pixel arithmetic.
  Pencil& setGlobalCompositeBlendFunc(BlendFactors src_factor,
                                      BlendFactors dst_factor) noexcept;

  /// \brief Sets the composite operation with custom pixel arithmetic for RGB
  /// and alpha components separately.
  Pencil& setGlobalCompositeBlendFuncSeparate(BlendFactors src_rgb,
                                              BlendFactors dst_rgb,
                                              BlendFactors src_alpha,
                                              BlendFactors dst_alpha) noexcept;

  /// \brief Saves the current render state.
  Pencil& save() noexcept;

  /// \brief Restores the previous saved render state.
  Pencil& restore() noexcept;

  /// \brief Resets the render state to default values.
  Pencil& reset() noexcept;

  /// \brief Enable/disable anti-aliasing.
  Pencil& setAntiAlias(bool enabled) noexcept;

  /// \brief Sets the color of the pencil stroke.
  Pencil& setStrokeColor(const Color& color) noexcept;

  /// \brief Sets the paint of the pencil stroke.
  Pencil& setStrokePaint(const Paint& paint) noexcept;

  /// \brief Sets the pencil fill color.
  Pencil& setFillColor(const Color& color) noexcept;

  /// \brief Sets the pencil fill paint.
  Pencil& setFillPaint(const Paint& paint) noexcept;

  /// \brief Sets the maximum miter length. The miter length is the distance
  /// between the inner corner and the outer corner where two lines meet.
  Pencil& setMiterLimit(float limit) noexcept;

  /// \brief Sets the stroke width.
  Pencil& setStrokeWidth(float size) noexcept;

  /// \brief Sets the line cap of the pencil.
  Pencil& setLineCap(LineCap cap) noexcept;

  /// \brief Sets the line join of the pencil.
  Pencil& setLineJoin(LineJoin join) noexcept;

  /// \brief Sets the transparency applied to all rendered shapes. Already
  /// transparent paths will get proportionally more transparent as well.
  Pencil& setGlobalAlpha(unsigned char alpha) noexcept;

  /// \brief Resets transform back to default.
  Pencil& resetTransform() noexcept;

  /// \brief Transforms the current coordinate system.
  Pencil& transform(float horizontal_scaling, float horizontal_skewing,
                    float vertical_skewing, float vertical_scaling,
                    float horizontal_moving, float vertical_moving) noexcept;

  /// \brief Transforms the current coordinate system.
  Pencil& transform(const TransformMatrix& matrix) noexcept;

  /// \brief Sets current coordinate system to a new origin.
  Pencil& translate(float x, float y) noexcept;

  /// \brief Rotates the current coordinate system by a given angle in radians.
  Pencil& rotate(float angle) noexcept;

  /// \brief Skews the current coordinate system along X-axis. Angle is
  /// specified in radians.
  Pencil& skewX(float angle) noexcept;

  /// \brief Skews the current coordinate system along Y-axis. Angle is
  /// specified in radians.
  Pencil& skewY(float angle) noexcept;

  /// \brief Scales the current coordinate system. (1.0=100%, 0.5=50%, 2=200%,
  /// etc.)
  Pencil& scale(float x_percent, float y_percent) noexcept;

  /// \brief Gets the current transformation matrix.
  TransformMatrix getCurrentTransform() const noexcept;

  /// \brief Creates and returns a linear gradient.
  Paint createLinearGradient(float start_x, float start_y, float end_x,
                             float end_y, const Color& start_color,
                             const Color& end_color) const noexcept;

  /// \brief Creates and returns a box gradient.
  Paint createBoxGradient(float x, float y, float width, float height,
                          float corner_radius, float feather,
                          const Color& start_color,
                          const Color& end_color) const noexcept;

  /// \brief Creates and returns a radial gradient.
  Paint createRadialGradient(float center_x, float center_y, float inner_radius,
                             float outer_radius, const Color& start_color,
                             const Color& end_color) const noexcept;

  /// \brief Sets the current scissor rectangle. Scissoring allows you to clip
  /// the rendering into a rectangle.
  Pencil& scissor(float x, float y, float width, float height) noexcept;

  /// \brief Intersects current scissor rectangle with the specified rectangle.
  /// The scissor rectangle is transformed by the current transform.
  Pencil& intersectScissor(float x, float y, float width,
                           float height) noexcept;

  /// \brief Resets and disables scissoring.
  Pencil& resetScissor() noexcept;

  /// \brief Begins a new path by clearing the current path.
  Pencil& beginPath() noexcept;

  /// \brief Moves the pencil to a given position.
  Pencil& moveTo(float x, float y) noexcept;

  /// \brief Adds a line from the current pencil position to a given position.
  Pencil& lineTo(float x, float y) noexcept;

  /// \brief Adds a bezier from the current pencil position to a given position
  /// with two given control points.
  Pencil& bezierTo(float c1x, float c1y, float c2x, float c2y, float x,
                   float y) noexcept;

  /// \brief Adds a quadratic bezier curve from the current pencil position to a
  /// given position with one given control point.
  Pencil& quadTo(float cx, float cy, float x, float y) noexcept;

  /// \brief Creates an arc between two tangents.
  Pencil& arcTo(float x1, float y1, float x2, float y2, float radius) noexcept;

  /// \brief Creates a path from the current point back to the starting point.
  Pencil& closePath() noexcept;

  /// \brief Sets path fill rule.
  Pencil& setPathFillRule(Solidity solidity) noexcept;

  /// \brief Creates an arc that can become a full circle, or just part of a
  /// circle.
  Pencil& arc(float center_x, float center_y, float radius, float start_angle,
              float end_angle, Direction direction) noexcept;

  /// \brief Creates a rectangle shape.
  Pencil& rectangle(float x, float y, float width, float height) noexcept;

  /// \brief Creates a rounded rectangle shape with one given roundness radius
  /// for all corners.
  Pencil& roundedRectangle(float x, float y, float width, float height,
                           float radius) noexcept;

  /// \brief Creates a rounded rectangle shape with a given roundness radius for
  /// each corner.
  Pencil& roundedRectangle(float x, float y, float width, float height,
                           float radius_top_left, float radius_top_right,
                           float radius_bottom_right,
                           float radius_bottom_left) noexcept;

  /// \brief Creates an ellipse shape.
  Pencil& ellipse(float center_x, float center_y, float radius_x,
                  float radius_y) noexcept;

  /// \brief Creates a circle shape.
  Pencil& circle(float center_x, float center_y, float radius) noexcept;

  /// \brief Fills the current path with the current fill style.
  Pencil& fill() noexcept;

  /// \brief Strokes the current path with the current stroke style.
  Pencil& stroke() noexcept;

  /// \brief Creates an image from file.
  Image createImage(const std::string& filename, int image_flags) const
      noexcept;

  /// \brief Creates an image from a memory buffer.
  Image createImage(unsigned char* data, int size, int image_flags) const
      noexcept;

  Paint createImagePattern(const Image& image, float top_left_x,
                           float top_left_y, float image_width,
                           float image_height, float angle,
                           unsigned char alpha) const noexcept;
};
}  // namespace dana
