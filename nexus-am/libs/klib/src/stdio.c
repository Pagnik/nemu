#include "klib.h"
#include <stdarg.h>

#include <stdlib.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  

  int i = 0;
  int j = 0;
  


  while (1) {
    switch (fmt[i++]) {


      case '%': {

        switch (fmt[i++]) {
          case 'd': {
            int d = va_arg(ap, int);
            itoa(d, out + j);
            j += strlen(out + j);
            break;
          }

          case 's': {
            char *s = va_arg(ap, char *);
            strcpy(out + j, s);
            j += strlen(s);
            break;
          }
        }
        break;
      }


      case '\0': {
        out[j] = '\0';
        
        return j;
      }
    
    
    }


    i++;
  }
}

int sprintf(char *out, const char *fmt, ...) {


  va_list args;
  va_start(args, fmt);
  vsprintf(out, fmt, args);

  va_end(args);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
