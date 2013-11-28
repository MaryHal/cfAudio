#ifndef _Log_h_
#define _Log_h_

#include <string>

#if defined(DEBUG)
    #define coreAssert(x,y) if(!(x)){ coreFailedAssert(#x, y, __FILE__, __LINE__); }
    void coreFailedAssert(const std::string& condition,
                          const std::string message,
                          const char* file,
                          int line);
#else
    #define coreAssert(x,y) {}
#endif

void logf(const char* format, ...);
void log(const std::string& str);

#endif
