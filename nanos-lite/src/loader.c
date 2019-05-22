#include "proc.h"

#define DEFAULT_ENTRY 0x4000000

extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_filesz(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  
  /*size_t ramdisk_size = get_ramdisk_size();
  
  ramdisk_read(DEFAULT_ENTRY, 0, ramdisk_size);*/

  int fd = fs_open(filename, 0, 0);
  int fsize = fs_filesz(fd);
  fs_read(fd, (void *) DEFAULT_ENTRY, fsize);

  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
