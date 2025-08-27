#ifndef PARSE_H
#define PARSE_H

#include "my_shell.h"

// Function declarations
args parseargs(int argc, char **argv);
void eval(char *cmd, char **args);
void split(char *cmd, char **args);
void exec(char *cmd, char **args, char **envp);

#endif