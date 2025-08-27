#ifndef MEMALLOC_H
#define MEMALLOC_H

#include "../include/my_shell.h"

char *cmdline_alloc();
char **args_list_alloc();
commands *commands_alloc();

void free_all(char *cmdline, char **args,commands *cmd);

#endif