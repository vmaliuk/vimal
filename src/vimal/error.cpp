#include "error.h"
#include "string.hpp"
#include <system_error>

using namespace vimal::error;

using vimal::string_utils::to_string;

const char* Pack::what() const noexcept
{
    if (_message.empty())
    {
        int i = 0;
        std::ostringstream os;
        os << "exceptions pack"s;
        for (auto& eptr : _exceptions)
        {
            try
            {
                std::rethrow_exception(eptr);
            }
            catch (std::exception& ex)
            {
                os << " ["s << ++i << "]: "s << ex.what();
            }
        }
        _message = std::move(os.str());
    }
    return _message.c_str();
}

class Info::Builder
{
public:
    void build(Info& info, std::exception& ex)
    {
        try
        {
            std::rethrow_if_nested(ex);
        }
        catch (...)
        {
            build(info, std::current_exception());
        }
    }

    void build(Info& info, std::exception_ptr eptr)
    {
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (Pack& pack)
        {
            while (auto eptr = pack.pop())
            {
                build(info, eptr);
            }
        }
        catch (Location& err)
        {
            info._location = err.location();
            build(info, err);
        }
        catch (vimal::Error& err)
        {
            if (info._level < err.level())
            {
                info._level = err.level();
            }
            info._messages.emplace_back(err.message());
            build(info, err);
        }
        catch (std::system_error& sys_err)
        {
            info._messages.emplace_back(to_string("system error "s, sys_err.code(), " : "s, sys_err.what()));
            build(info, sys_err);
        }
        catch (std::exception& ex)
        {
            info._messages.emplace_back(ex.what());
            build(info, ex);
        }
        catch (std::nested_exception& ex)
        {
            build(info, ex.nested_ptr());
        }
    }
};

Info vimal::error::make_info(std::exception_ptr eptr)
{
    return make_info(nullptr, eptr);
}

Info vimal::error::make_info(const char* location, std::exception_ptr eptr)
{
    Info err_info;
    if (location != nullptr)
    {
        err_info._location = location;
    }
    Info::Builder info_builder;
    info_builder.build(err_info, eptr);
    return err_info;
}

std::ostream& vimal::error::operator<<(std::ostream& os, const Info& err_info)
{
    bool nested = false;
    if (err_info._out_level)
    {
        switch (err_info._level)
        {
        case Level::Warning:
            os << "Warning"s;
            break;
        case Level::Critical:
            os << "Critical"s;
            break;
        default:
            os << "Error"s;
        }
        nested = true;
    }
    for (auto& message : err_info._messages)
    {
        if (nested)
        {
            os << " | "s;
        }
        os << message;
        nested = true;
    }
    if (err_info._location != nullptr)
    {
        if (nested)
        {
            os << " | "s;
        }
        os << err_info._location;
    }
    return os;
}

[[noreturn]] void vimal::rethrow(const char* location)
{
    std::exception_ptr eptr = std::current_exception();
    if (eptr)
    {
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (vimal::Error&)
        {
            throw;
        }
        catch (...)
        {
            vimal::throw_location(location);
        }
    }
    else
    {
        vimal::throw_location(location);
    }
}

[[noreturn]] void vimal::throw_location(const char* location)
{
    throw_error(Location(location));
}

[[noreturn]] void vimal::throw_error(std::string&& message)
{
    throw_error(vimal::Error(std::move(message)));
}
