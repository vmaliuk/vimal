#include "log.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace vimal;
using namespace std::chrono;

void Log::out_time(std::ostringstream &os) try
{
    auto now = system_clock::to_time_t(system_clock::now());
    os << std::put_time(std::localtime(&now), "%F %T ");
}
catch (...)
{
    throw_location(SourceLocation);
}

void Log::out(std::ostringstream& os) try
{
    std::lock_guard<std::mutex> lock(_mtx);
    outs(os.str());
}
catch (...)
{
    throw_location(SourceLocation);
}

void Log::error(const char* location)
{
    error(error::make_info(location));
}

void Log::error()
{
    error(error::make_info());
}

void StdLog::outs(const std::string& message)
{
    std::cout << message << std::endl;
    std::cout.flush();
}
