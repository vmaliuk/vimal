#pragma once

namespace vma {
class Location
{
public:
    constexpr Location() = default;
    constexpr Location(const Location& other) = default;
    constexpr Location& operator=(const Location& other) = default;

    constexpr explicit Location(const char* location)
    : _location(location) {
    }

    constexpr operator const char*() const {
        return _location;
    }

    constexpr operator bool() const {
        return _location != nullptr;
    }

private:
    const char* _location = nullptr;
};
} // namespace vma

#define Stringify(x) #x
#define No2String(x) Stringify(x)
#define SourceLocation vma::Location(__FILE__ ":" No2String(__LINE__))
