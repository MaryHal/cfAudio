#include "Log.hpp"

#include <cstdio>
#include <cstdlib>
#include <stdarg.h>

namespace cfAudio
{
    void coreFailedAssert(const std::string& condition,
                          const std::string message,
                          const char* file,
                          int line)
    {
        Console::logf(" ## Failed Assertion ##");
        Console::logf("Condition: (%s)", condition.c_str());
        Console::logf("Assertion: %s", message.c_str());
        Console::logf("At: %s:%d", file, line);
        exit(1);
    }

    void Console::logf(const char *format, ...)
    {
        char sz[1024];
        va_list marker;

        va_start(marker, format);
        vsprintf(sz, format, marker);
        va_end(marker);

        std::string str = sz;

        log(str);
    }

    void Console::log(const std::string& str)
    {
        fprintf(stdout, "%s\n", str.c_str() );
    }
};
