#pragma once

#include "dana/types.h"

#include <memory>
#include <optional>
#include <string>
#include <utility>

struct NVGcontext;

namespace dana {

class Image {
  std::shared_ptr<NVGcontext> m_context{nullptr};
  std::optional<ImageHandle> m_handle{std::nullopt};

 public:
  Image() noexcept = default;

  Image(const std::shared_ptr<NVGcontext>& context,
        ImageHandle handle) noexcept;

  Image(Image&& image) noexcept;

  ~Image() noexcept;

  Image& operator=(Image&& image) noexcept;

  std::optional<ImageHandle> getHandle() const noexcept;

  std::pair<int, int> getSize() const noexcept;

 protected:
  Image(const Image&) = delete;

  Image& operator=(const Image&) = delete;

 private:
  bool isImageLoaded() const noexcept;
};
}  // namespace dana
