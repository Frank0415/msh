#ifndef PARSE_H
#define PARSE_H

#include "../include/my_shell.h"

args parseargs(int argc, char **argv);
void eval(args arg, char *cmd, char **args, commands *cmds);

#endif