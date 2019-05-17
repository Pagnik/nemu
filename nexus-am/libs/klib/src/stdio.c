#include "klib.h"
#include <stdarg.h>

#include <stdlib.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int d2str(char *out, int d) {
  int c = 0;
  if (d == -2147483648) {
	  strcpy(out, "-2147483648");
    return 11;
  }
  if (d < 0) {
    out[c++] = '-';
    d = (-d);
  }
  
  int a = 1000000000;
	//printf("%d\n", ud);
  while (d / a == 0 && a > 1) {
    a /= 10;
  }
  for (; a > 0; a /= 10) {
    out[c++] = (char) ('0' + (d / a));
    d = d % a;
  }

  
  return c;



}







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
            j += d2str(out + j, d);
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
