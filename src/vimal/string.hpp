#pragma once

namespace vma {
namespace string_utils {

template<typename ...Args>
inline std::string to_string(Args&& ...args)
{
    std::ostringstream os;
    (os << ... << args);
    return os.str();
}

} // namespace string_utils
} // namespace vma
