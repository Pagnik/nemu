#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();

  //printf_debug("esp: %x, ebp: %x\n", cpu.esp, cpu.ebp);
  
  //printf_debug("val: %x, width: %d\n", id_dest->val, id_dest->width);
  /*
  if (id_dest->width == 1) {
    rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
  }*/

  rtl_push(&id_dest->val);
  //printf_debug("esp: %x, *esp: %x\n", cpu.esp, vaddr_read(cpu.esp, id_dest->width));
  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();


  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();

  t0 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {


  cpu.esp = cpu.ebp;
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_msb(&t0, &cpu.eax, 2);
    rtl_xori(&t0, &t0, 1);
    rtl_addi(&t0, &t0, ~0);
    
    *((uint16_t *) &cpu.edx) = t0;
    
  }
  else {
    // TODO();
    rtl_msb(&t0, &cpu.eax, 4);
    rtl_xori(&t0, &t0, 1);
    rtl_addi(&t0, &t0, ~0);
    
    cpu.edx = t0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, (id_dest->width) >> 1);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;

  rtl_andi(&t0, &id_src->val, 0xffff);
  operand_write(id_dest, &t0);

  
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}


/* 
make_EHelper(movs) {
  
}*/