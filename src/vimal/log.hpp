#pragma once
#include "error.h"
#include <memory>

namespace vma {
namespace global {

class _detail
{
    static std::unique_ptr<vma::Log>& log();
    static void log_time(std::ostringstream& os);
    static void log_out(std::ostringstream& os);
    friend class global::Log;
};

template<typename LogT, typename ...Args,
         typename = std::enable_if_t<std::is_base_of<vma::Log, std::decay_t<LogT>>::value>>
inline void Log::initialize(Args&& ...args)
{
    _detail::log() = std::make_unique<LogT>(std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Log::info(Args&& ...args) try
{
    std::ostringstream os;
    _detail::log_time(os);
    (os << ... << args);
    _detail::log_out(os);
}
catch(std::exception&)
{
    throw_error(SourceLocation);
}

} // namespace global
} // namespace vma
