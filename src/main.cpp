#include "version.h"
#include "log.h"

using namespace vma;
using namespace std::string_literals;

int main(int /*argc*/, char* /*argv*/[]) try
{
    global::Log::info(Version::to_string());
    throw_error("vimal error"s);
    return 0;
}
catch(...)
{
    global::Log::error(SourceLocation);
    return 1;
}
