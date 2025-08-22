#include "../include/my_shell.h"
#include "parse.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[]) {
  // To Do: to be moved to args func
  args in_args = parseargs(argc, argv);
  if (in_args.wrong) {
    printf("The shell has no such option(s). \n");
    return 0;
  }

  // To Do: to be moved to a function
  char *cmdline = malloc(sizeof(char) * MAX_INPUT_LENGTH);

  while (1) {
    if (1) {
      printf(DEFAULT_ARGS);
      // to be implemented: custom output
    }

    fgets(cmdline, MAX_INPUT_LENGTH, stdin);

    if (feof(stdin)) {
      free(cmdline);
      exit(0);
    }
    eval(cmdline);
  }

  free(cmdline);
}