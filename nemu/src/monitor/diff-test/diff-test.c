#include <dlfcn.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "diff-test.h"

static void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
static void (*ref_difftest_getregs)(void *c);
static void (*ref_difftest_setregs)(const void *c);
static void (*ref_difftest_exec)(uint64_t n);

static bool is_skip_ref;
static bool is_skip_dut;

void difftest_skip_ref() { is_skip_ref = true; }
void difftest_skip_dut() { is_skip_dut = true; }

void init_difftest(char *ref_so_file, long img_size) {
#ifndef DIFF_TEST
  return;
#endif

  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  //printf("error: %s\n", dlerror());
  assert(handle);

  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(void) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);

  ref_difftest_init();
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), img_size);
  ref_difftest_setregs(&cpu);
}



union gdb_regs {
  struct {
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t eip, eflags;
    uint32_t cs, ss, ds, es, fs, gs;
  };
  struct {
    uint32_t array[77];
  };
};

/*
struct {
  int idx;
  char name[8];
} gpr_table[] = {
  {0, "eax"}, {1, "ecx"}, {2, "edx"}, {3, "ebx"}, {4, "esp"},
  {5, "ebp"}, {6, "esi"}, {7, "edi"},
};
*/
static char* gpr_names[] = {
    "$eax", "$ecx", "$edx", "$ebx", "$esp", "$ebp", "$esi", "$edi",
  };

void difftest_step(uint32_t eip) {
  // CPU_state ref_r;
  union gdb_regs ref_r;
  if (is_skip_dut) {
    is_skip_dut = false;
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_getregs(&ref_r);

  
  
  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
  //TODO();

  // no need to invoke difftest_getregs here, 
  // just use the global cpu
  uint32_t *dut_r_p = (uint32_t *) &cpu;
  bool failed = false;
  for (int i = 0; i < 8; i++) {
    if (dut_r_p[i] != ref_r.array[i]) {
      printf("%s differs: %x\t%x\n", gpr_names[i], dut_r_p[i], ref_r.array[i]);
      failed = true;
    }
  }
  
  if (cpu.eip != ref_r.eip) {
    printf("eip differs: %x\t%x\n", cpu.eip, ref_r.eip);
    failed = true;
  }



  typedef struct  {
      unsigned CF: 1;
      unsigned dont_care1: 1;
      unsigned PF: 1;
      unsigned dont_care3: 1;
      unsigned AF: 1;
      unsigned dont_care5: 1;
      unsigned ZF: 1;
      unsigned SF: 1;
      unsigned dont_care8_9_10: 3;
      unsigned OF: 1;
      unsigned dont_care: 20;
  } __eflags_t;

  __eflags_t *ref_eflags_p = (__eflags_t *) &ref_r.eflags;

  //printf_debug("CF: %d-%d, OF: %d-%d\n", cpu.CF, ref_eflags_p->CF, cpu.OF, ref_eflags_p->OF);

  /*
  if (cpu.ZF != ref_eflags_p->ZF) {
    printf("ZF differs: %d\t%d\n", cpu.ZF, ref_eflags_p->ZF);
    failed = true;
  }
  if (cpu.OF != ref_eflags_p->OF) {
    printf("OF differs: %d\t%d\n", cpu.OF, ref_eflags_p->OF);
    failed = true;
  }
  if (cpu.SF != ref_eflags_p->SF) {
    printf("SF differs: %d\t%d\n", cpu.SF, ref_eflags_p->SF);
    failed = true;
  }
  if (cpu.CF != ref_eflags_p->CF) {
    printf("CF differs: %d\t%d\n", cpu.CF, ref_eflags_p->CF);
    failed = true;
  }*/

  if (failed) {
    nemu_state = NEMU_ABORT;
  }



}
