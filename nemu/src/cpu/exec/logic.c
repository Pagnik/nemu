#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  //TODO();
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  
  

  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_li(&t1, 0);
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  // TODO();

  //printf_debug("$eax = %x\n", cpu.eax);
  rtl_xor(&id_dest->val, &id_dest->val, &id_src->val);

  rtl_li(&t0, 0);
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  operand_write(id_dest, &id_dest->val);
  print_asm_template2(xor);
  //printf_debug("$eax = %x\n", cpu.eax);
}

make_EHelper(or) {
  // TODO();
  rtl_or(&id_dest->val, &id_dest->val, &id_src->val);
  rtl_li(&t0, 0);
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  operand_write(id_dest, &id_dest->val);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // TODO();
  // unnecessary to update CF and OF in NEMU

  rtl_subi(&t0, &id_src->val, 1);     // shift (imm - 1) to get CF
  rtl_sar(&t0, &id_dest->val, &t0);

  rtl_andi(&t1, &t0, 1);      // t1 = CF
  rtl_set_CF(&t1);

  rtl_sari(&t0, &t0, 1);        // shift for the last time
  
  // flush
  operand_write(id_dest, &t0);

  // update OF = (OF & (imm == 1))
  
  rtl_get_OF(&t0);

  rtl_setrelopi(RELOP_EQ, &t1, &id_src->val, 1);
  rtl_and(&t0, &t0, &t1);

  rtl_set_OF(&t0);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_subi(&t0, &id_src->val, 1);     // shift (imm - 1) to get CF
  rtl_shl(&t0, &id_dest->val, &t0);

  rtl_msb(&t1, &t0, id_dest->width);
  rtl_set_CF(&t1);      // CF = t1

  rtl_shli(&t0, &t0, 1); 
  operand_write(id_dest, &t0);

  rtl_msb(&t2, &t0, id_dest->width);   // t2 = msa(dest)
  
  rtl_setrelop(RELOP_EQ, &t0, &t1, &t2);      // t0 = (t1 != t2)

  rtl_setrelopi(RELOP_EQ, &t1, &id_src->val, 1);    // t1 = (imm == 1)


  rtl_get_OF(&t2);          // t2 = OF
  // update OF = t2 | (!t2 & t1 & t0)

  rtl_and(&t0, &t0, &t1);
  //rtl_neg(&t1, &t2);
  rtl_setrelopi(RELOP_EQ, &t1, &t2, 0);
  rtl_and(&t0, &t0, &t1);
  rtl_or(&t0, &t0, &t2);

  rtl_set_OF(&t0);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_subi(&t0, &id_src->val, 1);     // shift (imm - 1) to get CF
  rtl_shr(&t0, &id_dest->val, &t0);

  rtl_andi(&t1, &t0, 1);      // t1 = CF
  rtl_set_CF(&t1);

  rtl_shri(&t0, &t0, 1);        // shift for the last time
  
  // flush
  operand_write(id_dest, &t0);

/*    what is "the original operand???"
  rtl_setrelopi(RELOP_EQ, &t1, &id_src->val, 1);    // t1 = (imm == 1)

  rtl_get_OF(&t0);        // t0 = OF

  rtl_msb(&t2, );

   01
00 01
01 01
11 11
10 00
  */
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;
  printf_debug("in setcc: dest width: %d\n", id_dest->width);
  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  
  rtl_not(&id_dest->val, &id_dest->val);
  operand_write(id_dest, &id_dest->val);
  
  print_asm_template1(not);
}
