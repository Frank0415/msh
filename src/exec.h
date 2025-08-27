#ifndef EXEC_H
#define EXEC_H

#include "../include/my_shell.h"

void exec(args arg, char *cmd, char **args, char **envp, commands *cmds);
void exec_pipe(args arg, char *cmd, char **args, char **envp, commands *cmds);

#endif