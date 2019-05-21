#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {

    case SYS_yield: {

      _yield();

      // QUESTION: what's the point to set GPRx? 
      // since we can just pass the result automatically by c compiler
      // it's in $EAX
      // or it's just a manner.
      c->GPRx = 0;
      break;
    }

    case SYS_exit: {
      /*for (int i = 0; i < 4; i ++) {
        printf("gpr: %d\n", a[i]);
      }*/
      _halt(a[1]);
      break;
    }

    case SYS_write: {
      int fd = a[1];
      char *buf = (char *) a[2];
      int count = a[3];
      // a[1] = fd
      // a[2] = buf[]
      // a[3] = count
      if (fd == 1 || fd == 2) {
        for (int i = 0; i < count; i++) {
          _putc(buf[i]);
        }
      }
      c->GPRx = count;
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c->GPRx;
}
