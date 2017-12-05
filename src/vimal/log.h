#pragma once
#include "location.h"
#include "error.h"
#include <sstream>
#include <memory>
#include <mutex>

namespace vimal {

class Log
{
public:
    Log() = default;
    virtual ~Log() = default;

    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    // log message
    template <typename ...Args>
    void out(Args&& ...args);

    // log error
    template <typename ...Args>
    void error(const error::Info& error_info, Args&& ...args);
    void error(const char* location);
    void error();

protected:
    virtual void outs(const std::string& message) = 0;

private:
    template <typename Arg, typename ...Args>
    void out(std::ostringstream &os, Arg&& arg, Args&& ...args);
    void out(std::ostringstream &os);
    void out_time(std::ostringstream &os);

private:
    std::mutex _mtx;
};


class StdLog : public Log
{
public:
    StdLog() = default;
    virtual ~StdLog() = default;

protected:
    void outs(const std::string& message) override;
};


namespace global {

class Log
{
public:
    template<typename LogT, typename ...Args,
             typename = std::enable_if_t<std::is_base_of<vimal::Log, std::decay_t<LogT>>::value>>
    static inline void initialize(Args&& ...args) {
        log().reset(std::make_unique<LogT>(std::forward<Args>(args)...));
    }

    template<typename ...Args>
    static inline void out(Args&& ...args) {
        log()->out(std::forward<Args>(args)...);
    }

    template<typename ...Args>
    static inline void error(Args&& ...args) {
        log()->error(std::forward<Args>(args)...);
    }

private:
    static inline std::unique_ptr<vimal::Log>& log() {
        static std::unique_ptr<vimal::Log> global_log = std::make_unique<StdLog>();
        return global_log;
    }
};

} // namespace global


template <typename ...Args>
inline void Log::out(Args&& ...args) try
{
    std::ostringstream os;
    out_time(os);
    out(os, std::forward<Args>(args)...);
}
catch(std::exception&)
{
    throw_location(SourceLocation);
}

template <typename Arg, typename ...Args>
inline void Log::out(std::ostringstream &os, Arg&& arg, Args&& ...args)
{
    os << std::move<Arg>(arg);
    out(os, std::forward<Args>(arg)...);
}

template <typename ...Args>
void Log::error(const error::Info& error_info, Args&& ...args)
{
    out(error_info, std::forward<Args>(args)...);
}

} // namespace vimal
