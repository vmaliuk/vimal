#pragma once
#include "location.h"
#include <sstream>

namespace vma {

class Log
{
public:
    Log() = default;
    virtual ~Log() = default;

    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    virtual void out(const std::string& message) = 0;
};

namespace global {

class Log
{
public:
    template<typename LogT, typename ...Args,
             typename = std::enable_if_t<std::is_base_of<vma::Log, std::decay_t<LogT>>::value>>
    static inline void initialize(Args&& ...args);

    template<typename ...Args>
    static inline void info(Args&& ...args);

    static void error(const Location& location);
    static void error();

    friend std::ostream& operator<<(std::ostream&, const Location&);
};

} // namespace global
} // namespace vma

#include "log.hpp"
