

#include <stdarg.h>
#define DBG
void printf_debug(const char *format, ...)
{
    #ifdef DBG
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);
    #endif
}