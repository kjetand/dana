#include "dana/pencil.h"

#include <GL/glew.h>

#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include <nanovg/nanovg.h>
#include <nanovg/nanovg_gl.h>
#include <nanovg/nanovg_gl_utils.h>

#include <array>

namespace dana {

static constexpr auto convert(const LineCap line_cap) noexcept {
  switch (line_cap) {
    case LineCap::BUTT:
      return NVG_BUTT;
    case LineCap::ROUND:
      return NVG_ROUND;
    case LineCap::SQUARE:
      return NVG_SQUARE;
  }
  return NVG_SQUARE;
}

static constexpr auto convert(const LineJoin line_join) noexcept {
  switch (line_join) {
    case LineJoin::MITER:
      return NVG_MITER;
    case LineJoin::ROUND:
      return NVG_ROUND;
    case LineJoin::BEVEL:
      return NVG_BEVEL;
  }
  return NVG_ROUND;
}

static constexpr auto convert(const Solidity solidity) noexcept {
  if (solidity == Solidity::SOLID) {
    return NVG_SOLID;
  }
  return NVG_HOLE;
}

static constexpr auto convert(const Direction direction) noexcept {
  if (direction == Direction::CLOCKWISE) {
    return NVG_CW;
  }
  return NVG_CCW;
}

static constexpr auto convert(
    const CompositeOperations composite_operation) noexcept {
  switch (composite_operation) {
    case CompositeOperations::SOURCE_OVER:
      return NVG_SOURCE_OVER;
    case CompositeOperations::SOURCE_IN:
      return NVG_SOURCE_IN;
    case CompositeOperations::SOURCE_OUT:
      return NVG_SOURCE_OUT;
    case CompositeOperations::ATOP:
      return NVG_ATOP;
    case CompositeOperations::DESTINATION_OVER:
      return NVG_DESTINATION_OVER;
    case CompositeOperations::DESTINATION_IN:
      return NVG_DESTINATION_IN;
    case CompositeOperations::DESTINATION_OUT:
      return NVG_DESTINATION_OUT;
    case CompositeOperations::DESTINATION_ATOP:
      return NVG_DESTINATION_ATOP;
    case CompositeOperations::LIGHTER:
      return NVG_LIGHTER;
    case CompositeOperations::COPY:
      return NVG_COPY;
    case CompositeOperations::XOR:
    default:
      return NVG_XOR;
  }
}

static constexpr auto convert(const BlendFactors blend_factor) noexcept {
  switch (blend_factor) {
    case BlendFactors::ZERO:
      return NVG_ZERO;
    case BlendFactors::ONE:
      return NVG_ONE;
    case BlendFactors::SRC_COLOR:
      return NVG_SRC_COLOR;
    case BlendFactors::ONE_MINUS_SRC_COLOR:
      return NVG_ONE_MINUS_SRC_COLOR;
    case BlendFactors::DST_COLOR:
      return NVG_DST_COLOR;
    case BlendFactors::ONE_MINUS_DST_COLOR:
      return NVG_ONE_MINUS_DST_COLOR;
    case BlendFactors::SRC_ALPHA:
      return NVG_SRC_ALPHA;
    case BlendFactors::ONE_MINUS_SRC_ALPHA:
      return NVG_ONE_MINUS_SRC_ALPHA;
    case BlendFactors::DST_ALPHA:
      return NVG_DST_ALPHA;
    case BlendFactors::ONE_MINUS_DST_ALPHA:
      return NVG_ONE_MINUS_DST_ALPHA;
    case BlendFactors::SRC_ALPHA_SATURATE:
    default:
      return NVG_SRC_ALPHA_SATURATE;
  }
}

static auto convert(const Color& color) noexcept {
  return nvgRGBA(color.r, color.g, color.b, color.a);
}

static constexpr auto convert(const NVGcolor& nvg_color) noexcept {
  return Color{static_cast<unsigned char>(nvg_color.r),
               static_cast<unsigned char>(nvg_color.g),
               static_cast<unsigned char>(nvg_color.b),
               static_cast<unsigned char>(nvg_color.a)};
}

static auto convert(const Paint& paint) noexcept {
  NVGpaint nvg_paint;

  nvg_paint.xform[0] = paint.transform.horizontal_scaling;
  nvg_paint.xform[1] = paint.transform.horizontal_skewing;
  nvg_paint.xform[2] = paint.transform.vertical_skewing;
  nvg_paint.xform[3] = paint.transform.vertical_scaling;
  nvg_paint.xform[4] = paint.transform.horizontal_moving;
  nvg_paint.xform[5] = paint.transform.vertical_moving;

  nvg_paint.extent[0] = std::get<0>(paint.extent);
  nvg_paint.extent[1] = std::get<1>(paint.extent);

  nvg_paint.radius = paint.radius;
  nvg_paint.feather = paint.feather;
  nvg_paint.innerColor = convert(paint.inner_color);
  nvg_paint.outerColor = convert(paint.outer_color);
  nvg_paint.image = paint.image;

  return nvg_paint;
}

static auto convert(const NVGpaint& nvg_paint) noexcept {
  Paint paint;

  paint.transform.horizontal_scaling = nvg_paint.xform[0];
  paint.transform.horizontal_skewing = nvg_paint.xform[1];
  paint.transform.vertical_skewing = nvg_paint.xform[2];
  paint.transform.vertical_scaling = nvg_paint.xform[3];
  paint.transform.horizontal_moving = nvg_paint.xform[4];
  paint.transform.vertical_moving = nvg_paint.xform[5];

  paint.extent.first = nvg_paint.extent[0];
  paint.extent.second = nvg_paint.extent[1];

  paint.radius = nvg_paint.radius;
  paint.feather = nvg_paint.feather;
  paint.inner_color = convert(nvg_paint.innerColor);
  paint.outer_color = convert(nvg_paint.outerColor);
  paint.image = nvg_paint.image;

  return paint;
}

static constexpr float convertAlpha(const unsigned char alpha) noexcept {
  return alpha / 255.0f;
}

Pencil::Pencil() {
  constexpr const unsigned int nvg_flags{NVG_STENCIL_STROKES | NVG_ANTIALIAS};
  m_context = std::shared_ptr<NVGcontext>(
      nvgCreateGL3(nvg_flags), [](NVGcontext* ptr) { nvgDeleteGL3(ptr); });
}

Pencil& Pencil::beginFrame(const float width, const float height,
                           const float pixel_ratio) noexcept {
  nvgBeginFrame(m_context.get(), width, height, pixel_ratio);
  return *this;
}

Pencil& Pencil::cancelFrame() noexcept {
  nvgCancelFrame(m_context.get());
  return *this;
}

Pencil& Pencil::endFrame() noexcept {
  nvgEndFrame(m_context.get());
  return *this;
}

Pencil& Pencil::setGlobalCompositeOperation(
    const CompositeOperations composite_operation) noexcept {
  nvgGlobalCompositeOperation(m_context.get(), convert(composite_operation));
  return *this;
}

Pencil& Pencil::setGlobalCompositeBlendFunc(
    const BlendFactors src_factor, const BlendFactors dst_factor) noexcept {
  nvgGlobalCompositeBlendFunc(m_context.get(), convert(src_factor),
                              convert(dst_factor));
  return *this;
}

Pencil& Pencil::setGlobalCompositeBlendFuncSeparate(
    const BlendFactors src_rgb, const BlendFactors dst_rgb,
    const BlendFactors src_alpha, const BlendFactors dst_alpha) noexcept {
  nvgGlobalCompositeBlendFuncSeparate(m_context.get(), convert(src_rgb),
                                      convert(dst_rgb), convert(src_alpha),
                                      convert(dst_alpha));
  return *this;
}

Pencil& Pencil::save() noexcept {
  nvgSave(m_context.get());
  return *this;
}

Pencil& Pencil::restore() noexcept {
  nvgRestore(m_context.get());
  return *this;
}

Pencil& Pencil::reset() noexcept {
  nvgReset(m_context.get());
  return *this;
}

Pencil& Pencil::setAntiAlias(const bool enabled) noexcept {
  nvgShapeAntiAlias(m_context.get(), enabled ? 1 : 0);
  return *this;
}

Pencil& Pencil::setStrokeColor(const Color& color) noexcept {
  nvgStrokeColor(m_context.get(), convert(color));
  return *this;
}

Pencil& Pencil::setStrokePaint(const Paint& paint) noexcept {
  nvgStrokePaint(m_context.get(), convert(paint));
  return *this;
}

Pencil& Pencil::setFillColor(const Color& color) noexcept {
  nvgFillColor(m_context.get(), convert(color));
  return *this;
}

Pencil& Pencil::setFillPaint(const Paint& paint) noexcept {
  nvgFillPaint(m_context.get(), convert(paint));
  return *this;
}

Pencil& Pencil::setMiterLimit(const float limit) noexcept {
  nvgMiterLimit(m_context.get(), limit);
  return *this;
}

Pencil& Pencil::setStrokeWidth(const float size) noexcept {
  nvgStrokeWidth(m_context.get(), size);
  return *this;
}

Pencil& Pencil::setLineCap(const LineCap cap) noexcept {
  nvgLineCap(m_context.get(), convert(cap));
  return *this;
}

Pencil& Pencil::setLineJoin(const LineJoin join) noexcept {
  nvgLineJoin(m_context.get(), convert(join));
  return *this;
}

Pencil& Pencil::setGlobalAlpha(const unsigned char alpha) noexcept {
  nvgGlobalAlpha(m_context.get(), convertAlpha(alpha));
  return *this;
}

Pencil& Pencil::resetTransform() noexcept {
  nvgResetTransform(m_context.get());
  return *this;
}

Pencil& Pencil::transform(const float horizontal_scaling,
                          const float horizontal_skewing,
                          const float vertical_skewing,
                          const float vertical_scaling,
                          const float horizontal_moving,
                          const float vertical_moving) noexcept {
  nvgTransform(m_context.get(), horizontal_scaling, horizontal_skewing,
               vertical_skewing, vertical_scaling, horizontal_moving,
               vertical_moving);
  return *this;
}

Pencil& Pencil::transform(const TransformMatrix& matrix) noexcept {
  nvgTransform(m_context.get(), matrix.horizontal_scaling,
               matrix.horizontal_skewing, matrix.vertical_skewing,
               matrix.vertical_scaling, matrix.horizontal_moving,
               matrix.vertical_moving);
  return *this;
}

Pencil& Pencil::translate(const float x, const float y) noexcept {
  nvgTranslate(m_context.get(), x, y);
  return *this;
}

Pencil& Pencil::rotate(const float angle) noexcept {
  nvgRotate(m_context.get(), angle);
  return *this;
}

Pencil& Pencil::skewX(const float angle) noexcept {
  nvgSkewX(m_context.get(), angle);
  return *this;
}

Pencil& Pencil::skewY(const float angle) noexcept {
  nvgSkewY(m_context.get(), angle);
  return *this;
}

Pencil& Pencil::scale(const float x_percent, const float y_percent) noexcept {
  nvgScale(m_context.get(), x_percent, y_percent);
  return *this;
}

TransformMatrix Pencil::getCurrentTransform() const noexcept {
  std::array<float, 6> matrix{};
  nvgCurrentTransform(m_context.get(), matrix.data());
  return {matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]};
}

Paint Pencil::createLinearGradient(const float start_x, const float start_y,
                                   const float end_x, const float end_y,
                                   const Color& start_color,
                                   const Color& end_color) const noexcept {
  return convert(nvgLinearGradient(m_context.get(), start_x, start_y, end_x,
                                   end_y, convert(start_color),
                                   convert(end_color)));
}

Paint Pencil::createBoxGradient(const float x, const float y, const float width,
                                const float height, const float corner_radius,
                                const float feather, const Color& start_color,
                                const Color& end_color) const noexcept {
  return convert(nvgBoxGradient(m_context.get(), x, y, width, height,
                                corner_radius, feather, convert(start_color),
                                convert(end_color)));
}

Paint Pencil::createRadialGradient(const float center_x, const float center_y,
                                   const float inner_radius,
                                   const float outer_radius,
                                   const Color& start_color,
                                   const Color& end_color) const noexcept {
  return convert(nvgRadialGradient(m_context.get(), center_x, center_y,
                                   inner_radius, outer_radius,
                                   convert(start_color), convert(end_color)));
}

Pencil& Pencil::scissor(const float x, const float y, const float width,
                        const float height) noexcept {
  nvgScissor(m_context.get(), x, y, width, height);
  return *this;
}

Pencil& Pencil::intersectScissor(const float x, const float y,
                                 const float width,
                                 const float height) noexcept {
  nvgIntersectScissor(m_context.get(), x, y, width, height);
  return *this;
}

Pencil& Pencil::resetScissor() noexcept {
  nvgResetScissor(m_context.get());
  return *this;
}

Pencil& Pencil::beginPath() noexcept {
  nvgBeginPath(m_context.get());
  return *this;
}

Pencil& Pencil::moveTo(const float x, const float y) noexcept {
  nvgMoveTo(m_context.get(), x, y);
  return *this;
}

Pencil& Pencil::lineTo(const float x, const float y) noexcept {
  nvgLineTo(m_context.get(), x, y);
  return *this;
}

Pencil& Pencil::bezierTo(const float c1x, const float c1y, const float c2x,
                         const float c2y, const float x,
                         const float y) noexcept {
  nvgBezierTo(m_context.get(), c1x, c1y, c2x, c2y, x, y);
  return *this;
}

Pencil& Pencil::quadTo(const float cx, const float cy, const float x,
                       const float y) noexcept {
  nvgQuadTo(m_context.get(), cx, cy, x, y);
  return *this;
}

Pencil& Pencil::arcTo(const float x1, const float y1, const float x2,
                      const float y2, const float radius) noexcept {
  nvgArcTo(m_context.get(), x1, y1, x2, y2, radius);
  return *this;
}

Pencil& Pencil::closePath() noexcept {
  nvgClosePath(m_context.get());
  return *this;
}

Pencil& Pencil::setPathFillRule(Solidity solidity) noexcept {
  nvgPathWinding(m_context.get(), convert(solidity));
  return *this;
}

Pencil& Pencil::arc(const float center_x, const float center_y,
                    const float radius, const float start_angle,
                    const float end_angle, const Direction direction) noexcept {
  nvgArc(m_context.get(), center_x, center_y, radius, start_angle, end_angle,
         convert(direction));
  return *this;
}

Pencil& Pencil::rectangle(float x, float y, float width,
                          float height) noexcept {
  nvgRect(m_context.get(), x, y, width, height);
  return *this;
}

Pencil& Pencil::roundedRectangle(float x, float y, float width, float height,
                                 float radius) noexcept {
  nvgRoundedRect(m_context.get(), x, y, width, height, radius);
  return *this;
}

Pencil& Pencil::roundedRectangle(float x, float y, float width, float height,
                                 float radius_top_left, float radius_top_right,
                                 float radius_bottom_right,
                                 float radius_bottom_left) noexcept {
  nvgRoundedRectVarying(m_context.get(), x, y, width, height, radius_top_left,
                        radius_top_right, radius_bottom_right,
                        radius_bottom_left);
  return *this;
}

Pencil& Pencil::ellipse(const float center_x, const float center_y,
                        const float radius_x, const float radius_y) noexcept {
  nvgEllipse(m_context.get(), center_x, center_y, radius_x, radius_y);
  return *this;
}

Pencil& Pencil::circle(const float center_x, const float center_y,
                       const float radius) noexcept {
  nvgCircle(m_context.get(), center_x, center_y, radius);
  return *this;
}

Pencil& Pencil::fill() noexcept {
  nvgFill(m_context.get());
  return *this;
}

Pencil& Pencil::stroke() noexcept {
  nvgStroke(m_context.get());
  return *this;
}

Image Pencil::createImage(const std::string& filename, int image_flags) const
    noexcept {
  const auto image_handle{
      nvgCreateImage(m_context.get(), filename.c_str(), image_flags)};
  return Image(m_context, image_handle);
}

Image Pencil::createImage(unsigned char* data, const int size,
                          const int image_flags) const noexcept {
  const auto image_handle{
      nvgCreateImageMem(m_context.get(), image_flags, data, size)};
  return Image(m_context, image_handle);
}

Paint Pencil::createImagePattern(const Image& image, const float top_left_x,
                                 const float top_left_y, const float width,
                                 const float height, const float angle,
                                 const unsigned char alpha) const noexcept {
  auto image_pattern{convert(
      nvgImagePattern(m_context.get(), top_left_x, top_left_y, width, height,
                      angle, *image.getHandle(), convertAlpha(alpha)))};
  image_pattern.inner_color = {255, 255, 255, alpha};
  image_pattern.outer_color = {255, 255, 255, alpha};
  return image_pattern;
}
}  // namespace dana
