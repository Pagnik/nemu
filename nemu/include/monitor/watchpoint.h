#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next, *prior;

  /* TODO: Add more members if necessary */
  int last_val;
  char expr[100];
  int counter;    
} WP;

#endif
