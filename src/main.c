#include "../include/my_shell.h"
#include "memalloc.h"
#include "parse.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[], char *envp[])
{
    // To Do: to be moved to args func
    args in_args = parseargs(argc, argv);
    if (in_args.wrong)
    {
        printf("The shell has no such option(s). \n");
        return 0;
    }
    else if (in_args.help)
    {
        printf(
            "Welcome to use my shell!\n\nThis project is a mix of 15-213's "
            "shell lab and VE482's project 1.\n\n");
        printf("Currently the project is still largely unfinished,\nwith the "
               "following features remain to be implemented:\n\n");
        printf("2. I/O Redirection and PATH resolution\n");
        printf("3. Unix Pipes\n");
        printf("4. Background Jobs and Signals\n");
        printf("5. Built-in commands and expansion\n");
        printf("6. Error handing and potential robustness improv\n");
        printf("\n");
        printf("Currently these features are already implemented:\n\n");
        printf("0. A (kind of) functioning program that accepts 'exit'\n");
        printf("1. Execute single external commands with flags\n");
        return 0;
    }

    // To Do: to be moved to a function

    char *cmdline = cmdline_alloc();
    char **args = args_list_alloc();

    while (1)
    {
        if (1)
        {
            printf(DEFAULT_ARGS);
            // to be implemented: custom output
        }

        fgets(cmdline, MAX_INPUT_LENGTH, stdin);

        if (feof(stdin))
        {
            free_all(cmdline, args);
            exit(0);
        }
        eval(cmdline, args);
        exec(cmdline, args,
             envp); // how can I setup environ array to setup the env var?
    }

    free(cmdline);
}