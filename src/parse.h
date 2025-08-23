#ifndef PARSE_H
#define PARSE_H

#include "../include/my_shell.h"

args parseargs(int argc, char **argv);
void eval(char *cmd, char **args);
void exec(char *cmd, char **args, char **envp);
void split(char *cmd, char **args);

#endif