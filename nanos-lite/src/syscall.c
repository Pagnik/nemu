#include "common.h"
#include "syscall.h"





static inline int sys_yield() {
  _yield();
  return 0;
}

static inline int sys_write(int fd, char *buf, int count) {
  
  if (fd == 1 || fd == 2) {
    for (int i = 0; i < count; i++) {
      _putc(buf[i]);
    }
  }
  return count;
}

static inline int sys_exit(int status) {
  _halt(status);
  // 
  panic("shouldn't reach here\n");
  return -1;
}

static inline int sys_brk(uintptr_t addr) {
  if (0) {  // sanity check

  } else {
    return 0;
  }
}


_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    // QUESTION: what's the point to set GPRx? 
    // since we can just pass the result automatically by c compiler
    // it's in $EAX
    // or it's just a manner.
    case SYS_yield: {
      c->GPRx = sys_yield();
      break;
    }

    case SYS_exit: {
      sys_exit(a[1]);
      break;
    }

    case SYS_write: {
      c->GPRx = sys_write(a[1], (char *) a[2], a[3]);
      break;
    }

    case SYS_brk: {
      c->GPRx = sys_brk(a[1]);
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c->GPRx;
}
