#include "version.h"
#include "log.h"

using namespace vimal;

int main(int /*argc*/, char* /*argv*/[]) try
{
    global::Log::out(Version::to_string());
    return 0;
}
catch(...)
{
    global::Log::error(SourceLocation);
    return 1;
}
