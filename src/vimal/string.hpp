#pragma once
#include <sstream>

namespace vimal {

using namespace std::string_literals;

namespace string_utils {

template <typename Value>
inline void to_sstream(std::ostringstream& os, const Value& value)
{
    os << value;
}

template <typename Value, typename ...Values>
inline void to_sstream(std::ostringstream& os, const Value& value, Values&&... values)
{
    os << value;
    to_sstream(os, std::forward<Values>(values)...);
}

template<typename ...Values>
inline std::string to_string(Values&&... values)
{
    std::ostringstream os;
    to_sstream(os, std::forward<Values>(values)...);
    return os.str();
}

} // namespace string_utils
} // namespace vimal
