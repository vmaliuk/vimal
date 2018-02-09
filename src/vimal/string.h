#pragma once
#include <sstream>
#include <string_view>

namespace vma {

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace string_utils {

template<typename ...Args>
inline std::string to_string(Args&& ...args);

} // namespace string_utils
} // namespace vma

#include "string.hpp"
