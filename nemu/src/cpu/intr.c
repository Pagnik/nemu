#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  // TODO();
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  //rtl_push(cpu.eip);
  rtl_push(&ret_addr);
  // TODO: check NO < limits , then what to do?
  
  vaddr_t id_p = cpu.idtr.base + NO * sizeof(GateDesc);

  vaddr_t ofst = 0;

  printf("NO: %d\n", NO);
  ofst = (vaddr_t) vaddr_read(id_p, 2);
  ofst = ((vaddr_t) vaddr_read(id_p + sizeof(GateDesc) - 2, 2) << 16) | ofst;


  
  rtl_j(ofst);
}

void dev_raise_intr() {
}
