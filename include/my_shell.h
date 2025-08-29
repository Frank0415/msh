#ifndef MY_SHELL_H
#define MY_SHELL_H

#define DEFAULT_ARGS "myshell > "
#define MAX_INPUT_LENGTH 1024
#define MAX_BUFFER 128
#define MAX_ARGS 128
#define MAX_CMD 64

typedef struct
{
    int help;
    int wrong;
    int verbose;
} args;

/*
Basic idea: Create a very long const char** and then in command struct every ptr
only is a pointer to the address of the const char*, and | can be naturally replaced with NULL to elim the prev. command
*/

typedef struct
{
    char *cmd;
    int argc;
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