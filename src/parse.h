#ifndef PARSE_H
#define PARSE_H

#include "../include/my_shell.h"

args parseargs(int argc, char **argv);
void eval(char *cmd);
void split(char *cmd, char *args[MAX_ARGS]);

#endif