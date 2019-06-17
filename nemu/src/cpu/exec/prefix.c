#include "cpu/exec.h"

make_EHelper(real);


// my current guess is:
// if the program doesn't manually instr 0x66
// then it's always on 32-bits when r32/16
make_EHelper(operand_size) {
  decoding.is_operand_size_16 = true;
  exec_real(eip);
  decoding.is_operand_size_16 = false;
}
