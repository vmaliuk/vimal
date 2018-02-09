#pragma once
#include "location.h"
#include <string>
#include <ostream>
#include <list>

namespace vma {
namespace error {

enum class Level
{
    Default,
    Warning,
    Critical
};

} // namespace error

template <typename E,
          typename = std::enable_if_t<std::is_base_of<std::exception, std::decay_t<E>>::value>>
[[noreturn]] inline void throw_error(E&& error);

template <typename E,
          typename = std::enable_if_t<std::is_base_of<std::exception, std::decay_t<E>>::value>>
[[noreturn]] inline void throw_error(vma::Location location, E&& error);

[[noreturn]] void throw_error(std::string&& message);
[[noreturn]] void throw_error(vma::Location location);

[[noreturn]] void rethrow(vma::Location location);

class Error : public std::exception
{
public:
    Error(Error&&) = default;
    Error(const Error&) = default;
    Error& operator=(Error&&) = default;
    Error& operator=(const Error&) = default;
    virtual ~Error() = default;

    explicit Error(std::string&& message);

    inline const std::string& message() const;

    virtual error::Level level() const;

protected:
    Error() = default;
    const char* what() const noexcept override final;

protected:
    std::string _message;
};

class Warning : public Error
{
public:
    using Error::Error;
    error::Level level() const override;
};


namespace error {

class Critical : public Error
{
public:
    using Error::Error;
    error::Level level() const override;
};

class Info
{
    class Builder;

public:
    friend Info make_info(vma::Location location, std::exception_ptr eptr);
    friend Info make_info(std::exception_ptr eptr);

    friend std::ostream& operator<<(std::ostream& os, const Info& err_info);

private:
    Info() = default;
    Level _level = Level::Default;
    const char* _location = nullptr;
    std::list<std::string> _messages;
};

Info make_info(vma::Location location, std::exception_ptr eptr);
Info make_info(std::exception_ptr eptr);

inline Info make_info(vma::Location location);
inline Info make_info();

template <typename Ex>
inline Info make_info(vma::Location location, Ex&& ex);

template <typename Ex>
inline Info make_info(Ex&& ex);

} // namespace error
} // namespace vma

#include "error.hpp"
