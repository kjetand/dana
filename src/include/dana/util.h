#pragma once

#include <functional>
#include <memory>

namespace dana {

template <typename T>
using c_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

}  // namespace dana
