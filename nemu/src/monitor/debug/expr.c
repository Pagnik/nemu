#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */


 // this implementation of token analysis make me uncomfortable, it's too dummy

#include <sys/types.h>
#include <regex.h>


#include <string.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_EQ, TK_LB, TK_RB,
  TK_DEC, TK_NEG, TK_INC, TK_LGCAND, TK_LGCOR, TK_HEX, TK_CMPL, TK_DEREF,
  TK_REG,

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
  {"\\+=", TK_INC},     // increment
  {"\\&\\&", TK_LGCAND},    // logical and
  {"||", TK_LGCOR},     // logical or
  {"\\+", TK_PLUS},     // plus
  {"-", TK_MINUS},      // minus (or negative)
  {"\\*", TK_MUL},      // multiple (or dereference)
  {"/", TK_DIV},        // division
  {"==", TK_EQ},        // equal
  {"\\(", TK_LB},       // left bracket
  {"\\)", TK_RB},       // right bracket
  {"\\$[a-zA-z0-9]+"},  // register
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

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        tokens[nr_token].type = rules[i].token_type;
        switch (rules[i].token_type) {

          case TK_DEC:
          case TK_HEX:
            if (substr_len > 32) {
              printf("decimal too long\n");
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
  return eval(0, nr_token);
}

static int check_parentheses(int l, int r) {
  int s = 0;
  int rightmost_start = -1;    // start of rightmost sequences of ')'
  for (int i = l; i < r; i++) {
    if (tokens[i].type == TK_RB) {
      s--;
      if (rightmost_start < 0) {
        rightmost_start = i;
      }
    } else {
      rightmost_start = -1;
      if (tokens[i].type == TK_LB) {
        s++;
      }
    }
    if (s < 0) {
      return -1;
    }
  }
  if (tokens[r - 1].type == TK_RB) {
    return (r - rightmost_start);
  } else {
    return 0;
  }
}

static int check_neg(int l, int r) {
  if (tokens[l].type == TK_NEG) {
    return 1;
  } else {
    return 0;
  }
}

static int check_op(int l, int r) {
  int res = -1;
  int p = -1;
  int s = 0;
  for (int i = l; i < r; i++) {
    if ((tokens[i].type == TK_PLUS || tokens[i].type == TK_MINUS) && s == 0) {
      if (p <= 1) {
        p = 0;
        res = i;
      }
    } else if ((tokens[i].type == TK_MUL || tokens[i].type == TK_DIV) && s == 0) {
      if (p <= 0) {
        p = 1;
        res = i;
      }
    } else if (tokens[i].type == TK_LB) {
      s++;
    } else if (tokens[i].type == TK_RB) {
      s--;
    }
  }
  return res;
}

static int eval(int l, int r) {
  int res;
  assert(l < r);

  if (l == r - 1) {
    assert(tokens[l].type == TK_DEC);
    return atoi(tokens[l].str);
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
    default:
      break;
  }

  panic("shouldn't reach here\n");
  return 777;
}
