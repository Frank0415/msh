#ifndef MY_SHELL_H
#define MY_SHELL_H

#define DEFAULT_ARGS "myshell > "
#define MAX_INPUT_LENGTH 1024
#define MAX_BUFFER 128
#define MAX_ARGS 64
#define MAX_CMD 64

typedef struct
{
    int help;
    int wrong;
    int verbose;
} args;

typedef struct
{
    char *cmd;
    char **argv;
    char *filein;
    char *fileout;
    int overwrite_true;
} command;

typedef struct
{
    command *command_list;
    int command_count;
} commands;

#endif