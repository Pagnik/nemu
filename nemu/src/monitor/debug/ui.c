#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>


#include <memory/memory.h>
void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *arg) {
  int n_step = atoi(arg);
  cpu_exec(n_step);
  return 0;
}
static int cmd_info(char *args) {
  switch (*args) {
    case 'r':
      printf("eax\t0x%x\n", cpu.eax);
      printf("ecx\t0x%x\n", cpu.ecx);
      printf("edx\t0x%x\n", cpu.edx);
      printf("ebx\t0x%x\n", cpu.ebx);
      printf("esp\t0x%x\n", cpu.esp);
      printf("ebp\t0x%x\n", cpu.ebp);
      printf("esi\t0x%x\n", cpu.esi);
      printf("edi\t0x%x\n", cpu.edi);
      printf("eip\t0x%x\n", cpu.eip);
      break;

    case 'w':
      printf("Not yet implemented\n");
      break;

    default:
      printf("Unkown args\n");
      break;
  }

  return 0;
}

static int cmd_x(char *args) {
  int length;
  args = strtok(args, " ");
  length = atoi(args);
  bool s;
  int v = expr(args + strlen(args) + 1, &s);
  //printf("expr value of: %d (%x)\n", v, v);
  if (!s || (v < 0)) {
    printf("invalid expression or value\n");
    return 0;
  }

  uint32_t addr = (uint32_t) v;

  for (uint32_t i = addr; i < (addr + length * 4); i += 4) {
    //printf("addr: %x\n", i);
    printf("0x%x\n", vaddr_read(i, 4));
  }

  // actually I don't think it should always return 0
  // but there is an assert inside vaddr_read
  return 0;
}

static int cmd_p(char *args) {

  bool s;

  int v = expr(args, &s);
  printf("expr value of: %d (%x)\n", v, v);
  if (!s || (v < 0)) {
    printf("invalid expression or value\n");
    return 0;
  }
  printf("%d\t(%x)\n", v, v);
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single instruction step", cmd_si},
  { "info", "Print current information", cmd_info},
  { "p", "Print result of expressions", cmd_p},
  { "x", "Print memory", cmd_x},

  /* TODO: Add more commands */

};



#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
