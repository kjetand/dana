#include "dana/image.h"

#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include <nanovg/nanovg.h>

namespace dana {

Image::Image(const std::shared_ptr<NVGcontext>& context,
             ImageHandle handle) noexcept
    : m_context{context}, m_handle{handle} {}

Image::Image(Image&& image) noexcept
    : m_context{image.m_context}, m_handle{image.m_handle} {
  image.m_context = nullptr;
  image.m_handle = std::nullopt;
}

Image::~Image() noexcept {
  if (isImageLoaded()) {
    nvgDeleteImage(m_context.get(), *m_handle);
  }
}

Image& Image::operator=(Image&& image) noexcept {
  m_context = image.m_context;
  m_handle = image.m_handle;
  image.m_context = nullptr;
  image.m_handle = std::nullopt;
  return *this;
}

std::optional<ImageHandle> Image::getHandle() const noexcept {
  return m_handle;
}

std::pair<int, int> Image::getSize() const noexcept {
  if (!isImageLoaded()) {
    return {0, 0};
  }
  int width{0};
  int height{0};
  nvgImageSize(m_context.get(), *m_handle, &width, &height);
  return {width, height};
}

bool Image::isImageLoaded() const noexcept {
  return nullptr != m_context && m_handle;
}
}  // namespace dana
