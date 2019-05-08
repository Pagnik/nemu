
/*
#include <stdarg.h>
#include <stdio.h>
#define DBG
void inline printf_debug(const char *format, ...) {
    #ifdef DBG

    printf("%s\\ ", __FILE__);
    printf("%s\\ ", __func__);
    printf("%d:\t", __LINE__);
    

    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);
    #endif
}



void inline QUESTION(char *str) {
    printf("%s\\ ", __FILE__);
    printf("%s\\ ", __func__);
    printf("%d:\t", __LINE__);
    printf("%s\n", str);
}
*/