#include "common.h"

// abstract from interuption to event!

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {

    case _EVENT_YIELD: {
      printf("this is a yield event\n");
      break;
    }
    case _EVENT_SYSCALL: {
      //printf("syscall id: %d\n", c->GPR1);
      do_syscall(c);
      break;
    }
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
