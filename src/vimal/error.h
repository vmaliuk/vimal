#pragma once
#include <ostream>
#include <string>
#include <list>

namespace vimal {
namespace error {

enum class Level
{
    Default,
    Warning,
    Critical
};

} // namespace error

[[noreturn]] void throw_error(std::string&& message);
[[noreturn]] void throw_location(const char* location);
[[noreturn]] void rethrow(const char* location);

/*  throwing c-stile strings is disabled
      as it's reserved for source location
    use throw_error(std::string&&) instead
    use throw_location(const char*) to throw exception with source location
*/
[[noreturn]] void throw_error(const char*) = delete;

template <typename E, typename = std::enable_if_t<std::is_base_of<std::exception, std::decay_t<E>>::value>>
[[noreturn]] inline void throw_error(E&& ex)
{
    if (std::current_exception())
    {
        std::throw_with_nested(std::forward<E>(ex));
    }
    else
    {
        throw std::forward<E>(ex);
    }
}

template <typename E, typename = std::enable_if_t<std::is_base_of<std::exception, std::decay_t<E>>::value>>
[[noreturn]] inline void throw_error(const char* location, E&& ex)
{
    try
    {
        throw_error(std::forward<E>(ex));
    }
    catch (...)
    {
        throw_location(location);
    }
}

class Error : public std::exception
{
public:
    explicit Error(std::string&& message)
    : _message(std::move(message)) {
    }
    Error(Error&&) = default;
    Error(const Error&) = default;
    Error& operator=(Error&&) = default;
    Error& operator=(const Error&) = default;
    virtual ~Error() = default;

    inline const std::string& message() const {
        return _message;
    }

    virtual error::Level level() const {
        return error::Level::Default;
    }

protected:
    Error() = default;

    const char* what() const noexcept override {
        return _message.c_str();
    }

protected:
    std::string _message;
};


class Warning : public Error
{
public:
    using Error::Error;
    error::Level level() const override {
        return error::Level::Warning;
    }
};

namespace error {

class Critical : public Error
{
public:
    using Error::Error;
    error::Level level() const override {
        return error::Level::Critical;
    }
};


class Location : public Error
{
public:
    explicit Location(const char* location)
    : _location(location) {
    }
    Location(const Location& other) = default;
    Location& operator=(const Location& other) = default;
    virtual ~Location() = default;

    inline const char* location() const {
        return _location;
    }

private:
    const char* _location = nullptr;
};

class Pack : public std::exception
{
public:
    Pack() = default;
    Pack(Pack&&) = default;
    Pack(const Pack&) = default;
    Pack& operator=(Pack&&) = default;
    Pack& operator=(const Pack&) = default;
    virtual ~Pack() = default;

    inline void push() {
        _exceptions.push_back(std::current_exception());
    }

    inline std::exception_ptr pop() {
        std::exception_ptr eptr;
        if (!_exceptions.empty()) {
            eptr = _exceptions.back();
            _exceptions.pop_back();
        }
        return eptr;
    }

    inline void throw_if_not_empty() const {
        if (!_exceptions.empty()) {
            throw *this;
        }
    }

    const char* what() const noexcept override;

private:
    std::list<std::exception_ptr> _exceptions;
    mutable std::string _message;
};

class Info
{
    class Builder;

public:
    friend std::ostream& operator<<(std::ostream& os, const Info& err_info);

    friend Info make_info(const char* location, std::exception_ptr eptr);
    friend Info make_info(std::exception_ptr eptr);

    inline Level level() const {
        return _level;
    }

    const Info& level(bool out_level) const {
        _out_level = out_level;
        return *this;
    }

private:
    Info() = default;
    Level _level = Level::Default;
    const char* _location = nullptr;
    std::list<std::string> _messages;
    mutable bool _out_level = true;
};

std::ostream& operator<<(std::ostream& os, const Info& err_info);

Info make_info(const char* location, std::exception_ptr eptr);
Info make_info(std::exception_ptr eptr);

template <typename E>
inline Info make_info(const char* location, E&& e) {
    return make_info(location, std::make_exception_ptr(std::forward<E>(e)));
}

template <typename E>
inline Info make_info(E&& e) {
    return make_info(std::make_exception_ptr(std::forward<E>(e)));
}

inline Info make_info(const char* location) {
    return make_info(location, std::current_exception());
}

inline Info make_info() {
    return make_info(std::current_exception());
}

} // namespace error
} // namespace vimal
