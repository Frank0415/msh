#include "parse.h"
#include "memalloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/wait.h>

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

void eval(char *cmd, char **args) { split(cmd, args); }

/*

for command:
ls -la\n
01 2 345 6
then last_avail_ptr = 3
*/

void split(char *cmd, char **args) {
  int args_count = 0;
  int last_avail_ptr = 0;
  int cmd_last_ptr = 0;

  // First, set args[0] to be the command itself
  args[0] = cmd;
  args_count = 1;

  for (int i = 0; i < MAX_INPUT_LENGTH; i++) {

    if (cmd[i] == '\n') {
      if (last_avail_ptr > 0) {
        if (i - last_avail_ptr > 63) {
          free_all(cmd, args);
          fprintf(stderr, "Sorry, currently args length longer than 64 are not "
                          "supported.\n");
          exit(0);
        }
        // Allocate memory for args[args_count]
        args[args_count] = malloc((i - last_avail_ptr + 1) * sizeof(char));
        if (!args[args_count]) {
          fprintf(stderr, "Memory allocation failed for args[%d].\n",
                  args_count);
          exit(1);
        }
        strncpy(args[args_count], &cmd[last_avail_ptr], i - last_avail_ptr);
        args[args_count][i - last_avail_ptr] = '\0';
        args_count++;
      }
      // Null-terminate command at first space (or at newline if no spaces)
      if (cmd_last_ptr > 0) {
        cmd[cmd_last_ptr] = '\0';
      } else {
        cmd[i] = '\0'; // No spaces found, terminate at newline
      }
      // NULL-terminate the args array for execve
      args[args_count] = NULL;

// Debug info
#ifndef NDEBUG
      fprintf(stderr, "Debug info:\n");
      fprintf(stderr, "Command: %s\n", cmd);
      for (int i = 0; i < args_count; i++) {
        fprintf(stderr, "args[%d]: %s\n", i, args[i]);
      }
#endif
      return;
    }
    if (cmd_last_ptr == 0 && cmd[i] == ' ') {
      cmd_last_ptr = i;       // Mark first space position
      last_avail_ptr = i + 1; // Start of first argument
    } else if (cmd[i] == ' ' && last_avail_ptr > 0) {
      // Found space, save current argument
      if (i - last_avail_ptr > 63) {
        free_all(cmd, args);
        fprintf(
            stderr,
            "Sorry, currently args length longer than 64 are not supported.\n");
        exit(0);
      }
      // Allocate memory for args[args_count]
      args[args_count] = malloc((i - last_avail_ptr + 1) * sizeof(char));
      if (!args[args_count]) {
        fprintf(stderr, "Memory allocation failed for args[%d].\n", args_count);
        exit(1);
      }
      strncpy(args[args_count], &cmd[last_avail_ptr], i - last_avail_ptr);
      args[args_count][i - last_avail_ptr] = '\0';
      args_count++;
      last_avail_ptr = i + 1; // Start of next argument
    }
  }
}

void exec(char *cmd, char **args, char **envp) {
  if (strcmp(cmd, "exit") == 0) {
    free_all(cmd, args);
    exit(0);
  }
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    execve(cmd, args, envp);
    perror("execve");
    exit(1);
  } else if (pid > 0) {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
  } else {
    perror("fork");
  }
}