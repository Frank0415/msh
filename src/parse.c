#include "parse.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

args parseargs(int argc, char **argv) {
  args ret = {.help = 0, .wrong = 0};
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      if (ret.help == 0) {
        ret.help = 1;
      }
    } else {
      ret.wrong = 1;
    }
  }
  return ret;
}

void eval(char *cmd) {
  if (strcmp(cmd, "quit\n") == 0) {
    free(cmd);
    exit(0);
  } else {
    char *args[MAX_ARGS];
    split(cmd, args);
  }
}