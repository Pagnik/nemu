#ifndef __ARCH_H__
#define __ARCH_H__

#include <am.h>

#define PMEM_SIZE (128 * 1024 * 1024)
#define PGSIZE    4096    // Bytes mapped by a page

typedef uint32_t size_t;

struct _Context {

  union {
    struct {
      //uintptr_t why_esp;
      struct _Protect *prot;
      //uintptr_t esi, ebx, eax, eip, edx, err, eflags, ecx, cs, esp, edi, ebp;
      uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
      //uintptr_t dont_care1;
      int       irq;
      uintptr_t err;
      //uintptr_t dont_care2;
      uintptr_t cs;
      uintptr_t eflags;
    };
    //uintptr_t tmp[sizeof()];
  };
  
};


/*
0
1916655708
485293249
31688
31668
1445038994
16
1050824
0
129
0
*/

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif
