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
  GateDesc *idt = vaddr_read(cpu.idtr.base, 4);


  
  rtl_j(idt[NO].offset_31_16);
}

void dev_raise_intr() {
}
