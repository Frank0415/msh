#include "memalloc.h"
#include "../include/my_shell.h"
#include "stdlib.h"

char *cmdline_alloc()
{
    char *ret = malloc(sizeof(char) * MAX_INPUT_LENGTH);
    return ret;
}
char **args_list_alloc()
{
    char **ret = malloc(sizeof(char *) * (MAX_ARGS));
    //   for (int i = 0; i < MAX_ARGS; i++) {
    //     ret[i] = malloc(sizeof(char) * MAX_ARGS); // Allocate 64 bytes for each argument
    //   }
    return ret;
}

commands *commands_alloc()
{
    commands *cmd = malloc(sizeof(commands));
    cmd->command_list = malloc(sizeof(command) * MAX_CMD);
    return cmd;
}

void free_all(char *cmdline, char **args, commands *cmd)
{
    free(cmdline);
    for (int i = 1; i < MAX_ARGS; i++)
    {
        if (args[i])
        {
            free(args[i]);
        }
    }
    free(args);
    free(cmd->command_list);
    free(cmd);
}
