#pragma once

namespace vma {

template <typename E, 
          typename = std::enable_if_t<std::is_base_of<std::exception, std::decay_t<E>>::value>>
[[noreturn]] inline void throw_error(E&& error)
{
    if (std::current_exception())
    {
        std::throw_with_nested(std::forward<E>(error));
    }
    else
    {
        throw std::forward<E>(error);
    }
}

template <typename E,
          typename = std::enable_if_t<std::is_base_of<std::exception, std::decay_t<E>>::value>>
[[noreturn]] inline void throw_error(vma::Location location, E&& error)
{
    try
    {
        throw_error(std::forward<E>(error));
    }
    catch (...)
    {
        throw_error(location);
    }
}

inline const std::string& Error::message() const
{
    return _message;
}

inline const char* Error::what() const noexcept
{
    return _message.c_str();
}

inline error::Level Error::level() const
{
    return error::Level::Default;
}

inline error::Level Warning::level() const
{
    return error::Level::Warning;
}

inline error::Level error::Critical::level() const
{
    return error::Level::Critical;
}

namespace error {

template <typename Ex>
inline Info make_info(vma::Location location, Ex&& ex)
{
    return make_info(location, std::make_exception_ptr(std::forward<Ex>(ex)));
}

template <typename Ex>
inline Info make_info(Ex&& ex)
{
    return make_info(std::make_exception_ptr(std::forward<Ex>(ex)));
}

inline Info make_info(vma::Location location)
{
    return make_info(location, std::current_exception());
}

inline Info make_info()
{
    return make_info(std::current_exception());
}

std::ostream& operator<<(std::ostream& os, const Info& err_info);

} // namespace error
} // namespace vma
