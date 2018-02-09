#include "log.h"
#include <iostream>
#include <iomanip>
#include <mutex>
#include <chrono>
#include <ctime>

using namespace std::chrono;
using namespace vma::global;

class StdLog : public vma::Log
{
public:
    StdLog() = default;
    virtual ~StdLog() = default;

private:
    inline void out(const std::string& message) override
    {
        std::cout << message << std::endl;
        std::cout.flush();
    }
};

std::ostream& operator<<(std::ostream& os, const vma::Location& location)
{
    os << static_cast<const char*>(location);
    return os;
}

std::unique_ptr<vma::Log>& _detail::log()
{
    static std::unique_ptr<vma::Log> global_log = std::make_unique<StdLog>();
    return global_log;
}

void _detail::log_time(std::ostringstream& os) try
{
    auto now = system_clock::to_time_t(system_clock::now());
    os << std::put_time(std::localtime(&now), "%F %T ");
}
catch (...)
{
    throw_error(SourceLocation);
}

void _detail::log_out(std::ostringstream& os) try
{
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    log()->out(os.str());
}
catch (...)
{
    throw_error(SourceLocation);
}

void Log::error(const vma::Location& location)
{
    info(error::make_info(location));
}

void Log::error()
{
    info(error::make_info());
}
