#include "error.h"
#include "string.h"
#include <system_error>

using namespace vma::error;

using vma::string_utils::to_string;

namespace _error {
class Location : public std::exception
{
public:
    Location(const Location& other) = default;
    Location& operator=(const Location& other) = default;
    virtual ~Location() = default;

    explicit Location(vma::Location location)
    : _location(location)
    {
    }

    inline vma::Location location() const
    {
        return _location;
    }

private:
    vma::Location _location;
};
} // namespace _error

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
        catch (_error::Location& err)
        {
            info._location = err.location();
            build(info, err);
        }
        catch (vma::Error& err)
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

vma::Error::Error(std::string&& message)
: _message(std::move(message))
{
}

Info vma::error::make_info(std::exception_ptr eptr)
{
    return make_info(vma::Location(), eptr);
}

Info vma::error::make_info(vma::Location location, std::exception_ptr eptr)
{
    Info err_info;
    if (location)
    {
        err_info._location = location;
    }
    Info::Builder info_builder;
    info_builder.build(err_info, eptr);
    return err_info;
}

std::ostream& vma::error::operator<<(std::ostream& os, const Info& err_info)
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
    for (auto& message : err_info._messages)
    {
        os << " | "s << message;
    }
    if (err_info._location != nullptr)
    {
        os << " | "s << err_info._location;
    }
    return os;
}

[[noreturn]] void vma::rethrow(vma::Location location)
{
    std::exception_ptr eptr = std::current_exception();
    if (eptr)
    {
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (vma::Error&)
        {
            throw;
        }
        catch (...)
        {
            vma::throw_error(location);
        }
    }
    else
    {
        vma::throw_error(location);
    }
}

[[noreturn]] void vma::throw_error(vma::Location location)
{
    throw_error(_error::Location(location));
}

[[noreturn]] void vma::throw_error(std::string&& message)
{
    throw_error(vma::Error(std::move(message)));
}
