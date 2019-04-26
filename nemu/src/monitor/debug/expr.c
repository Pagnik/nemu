#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */


 // this implementation of token analysis make me uncomfortable, it's too dummy

#include <sys/types.h>
#include <regex.h>


#include <string.h>
#include <stdlib.h>


static uint32_t* reg_by_name(char *name);

enum {
  TK_NOTYPE = 256, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_EQ, TK_LB, TK_RB,
  TK_DEC, TK_NEG, TK_LGCAND, TK_LGCOR, TK_HEX, TK_CMPL, TK_DEREF,
  TK_REG, TK_NEQ,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\&\\&", TK_LGCAND},  // logical and
  {"\\|\\|", TK_LGCOR},     // logical or
  {"\\+", TK_PLUS},     // plus
  {"-", TK_MINUS},      // minus (or negative)
  {"\\*", TK_MUL},      // multiple (or dereference)
  {"/", TK_DIV},        // division
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"\\(", TK_LB},       // left bracket
  {"\\)", TK_RB},       // right bracket
  {"\\$[a-zA-z0-9]+", TK_REG},  // register
  {"0x[0-9]+", TK_HEX}, // hexadecimal
  {"[0-9]+", TK_DEC},   // decimal
  {"!", TK_CMPL},       // one's complement
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;


static int eval(int l, int r);


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  printf("string: %s\n", e);
  while (e[position] != '\0') {
    //printf("2\n");
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      //printf("trying rule#%d\n", i);
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        if (rules[i].token_type == TK_NOTYPE) {
          break;;
        }

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        tokens[nr_token].type = rules[i].token_type;
        switch (rules[i].token_type) {
          case TK_REG:
          case TK_DEC:
          case TK_HEX:
            if (substr_len > 32) {
              printf("too long\n");
              return false;
            }
            memcpy(tokens[nr_token].str, substr_start, substr_len);


            break;

          case TK_MINUS:
            if (!((nr_token > 0) &&
            ((tokens[nr_token - 1].type == TK_HEX)
            || (tokens[nr_token - 1].type == TK_DEC)
            || (tokens[nr_token - 1].type == TK_REG)
            || (tokens[nr_token - 1].type == TK_RB)))) {
              tokens[nr_token].type = TK_NEG;
            }
            break;
          case TK_MUL:
            if (!((nr_token > 0) &&
            ((tokens[nr_token - 1].type == TK_HEX)
            || (tokens[nr_token - 1].type == TK_DEC)
            || (tokens[nr_token - 1].type == TK_REG)
            || (tokens[nr_token - 1].type == TK_RB)))) {
              tokens[nr_token].type = TK_DEREF;
            }
            break;
          default:

            break;
        }
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  //
  *success = true;
  //return 0x100000;

  printf("nr_token: %d\n", nr_token);
  return eval(0, nr_token);
}


// check if the parentheses are legal
// return the number of parentheses to be skipped
static int check_parentheses(int l, int r) {
  int s = 0;
  int skip = 1;
  for (int i = l; i < r; i++) {
    if (tokens[i].type == TK_RB) {
      s--;
    } else {
      if (tokens[i].type == TK_LB) {
        s++;
      }
    }
    if (s < 0) {
      return -1;
    } else if (s == 0 && i != r - 1) {
      // no outmost parentheses to be skipped.
      skip = 0;
    }
  }

  if (skip) {
    int i, j;
    for (i = l, j = r - 1, skip = 0; i <= j; i++, j++) {
      if (tokens[i].type == TK_LB && tokens[j].type == TK_RB) {
        skip ++;
      }
    }
  }

  return skip;

}

static inline int check_neg(int l, int r) {
  return tokens[l].type == TK_NEG;
}

static inline int check_cmpl(int l, int r) {
  return tokens[l].type == TK_CMPL;
}

static inline int check_deref(int l, int r) {
  return tokens[l].type == TK_DEREF;
}


static int check_op(int l, int r) {
  int prio = NR_REGEX;
  int pos = -1;

  for (int i = l; i < r; i++) {
    // shouldve made a table here.
    switch (tokens[i].type) {
      case TK_MUL:
      case TK_DIV:
        if (prio <= 1) {          // all <= because of left-combination
          prio = 1;
          pos = i;
        }
        break;
      case TK_PLUS:
      case TK_MINUS:
        if (prio <= 2) {
          prio = 2;
          pos = i;
        }
        break;
      case TK_EQ:
      case TK_NEQ:
        if (prio <= 3) {
          prio = 3;
          pos = i;
        }
        break;
      case TK_LGCAND:
        printf("yes\n");
        if (prio <= 4) {
          prio = 4;
          pos = i;
        }
        break;
      case TK_LGCOR:
        if (prio <= 5) {
          prio = 5;
          pos = i;
        }
        break;

      default:
        break;
    }
  }
  return pos;
}











static int eval(int l, int r) {
  int res;
  assert(l < r);

  if (l == r - 1) {
    uint32_t *regptr = reg_by_name(tokens[l].str);
    switch (tokens[l].type) {
      case TK_DEC:
        return strtol(tokens[l].str, NULL, 10);
      case TK_HEX:
        return strtol(tokens[l].str, NULL, 16);
      case TK_REG:


        assert(regptr != NULL);
        return *regptr;
      default:
        assert(0);
    }

  }

  res = check_parentheses(l, r);
  assert(res >= 0);
  if (res > 0) {
    return eval(l + res, r - res);
  }


  res = check_neg(l, r);
  if (res == 1) {
    return -eval(l + 1, r);
  }

  res = check_cmpl(l, r);
  if (res == 1) {
    return !eval(l + 1, r);
  }

  res = check_deref(l, r);
  if (res == 1) {
    return *((int *)eval(l + 1, r));
  }


  res = check_op(l, r);
  assert(l <= res && res < r);
  switch (tokens[res].type) {
    case TK_PLUS:
      return eval(l, res) + eval(res + 1, r);
      break;
    case TK_MINUS:
      return eval(l, res) - eval(res + 1, r);
      break;
    case TK_MUL:
      return eval(l, res) * eval(res + 1, r);
      break;
    case TK_DIV:
      return eval(l, res) / eval(res + 1, r);
      break;

    case TK_EQ:
      return eval(l, res) == eval(res + 1, r);
      break;
    case TK_NEQ:
      return eval(l, res) != eval(res + 1, r);
      break;
    case TK_LGCAND:
      return eval(l, res) && eval(res + 1, r);
      break;
    case TK_LGCOR:
      return eval(l, res) || eval(res + 1, r);
    default:
      break;
  }

  panic("shouldn't reach here\n");
  return 777;
}


static uint32_t* reg_by_name(char *name) {
  static char* names[] = {
    "$eax", "$ecx", "$edx", "$ebx", "$esp", "$ebp", "$esi", "$edi",
  };
  int n_names = sizeof(names) / sizeof(char*);
  for (int i = 0; i < n_names; i++) {
    if (strcmp(names[i], name) == 0) {
      return &(cpu.gpr[i]._32);
    }
  }

  return NULL;
}
