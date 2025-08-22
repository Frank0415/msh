#ifndef MY_SHELL_H
#define MY_SHELL_H

#define DEFAULT_ARGS "myshell > "
#define MAX_INPUT_LENGTH 1024
#define MAX_BUFFER 128
#define MAX_ARGS 64

typedef struct {
  int help;
  int wrong;
} args;

#endif