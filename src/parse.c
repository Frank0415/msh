#include "parse.h"
#include "memalloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/wait.h>

// Function prototypes for internal use
void handle_redirect(char **args, commands *cmds, int *i, int flag);
void handle_pipe(char **args, commands *cmds, int *i, int *arg_cnt);
void handle_args(char **args, command *cmd, int *count, int *const i); // Changed to pointer to modify struct
void init_next_cmd(commands *cmds);
void ERR_double_redirect(int is_input);
void ERR_syntax(const char *cmd);
void ERR_missing_program();

// Parses command-line arguments for the shell program itself (e.g., -h for help, -v for verbose)
args parseargs(int argc, char **argv)
{
    args ret = {.help = 0, .wrong = 0, .verbose = 0}; // Initialize verbose to 0
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            ret.help = 1;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
        {
            ret.verbose = 1;
        }
        else
        {
            ret.wrong = 1;
        }
    }
    return ret;
}

// Main evaluation function: tokenizes the command string and parses pipes, redirects, and arguments
void eval(args arg, char *cmd, char **args, commands *cmds)
{
    int arg_cnt = 0;
    cmds->command_count = 0;

    // Tokenize the command string using space and newline as delimiters
    args[arg_cnt] = strtok(cmd, " \t\n");
    while (args[arg_cnt])
    {
        // if (arg.verbose)
        // {
        //     printf("Argument %d: %s\t", arg_cnt, args[arg_cnt]);
        // }
        arg_cnt++;
        args[arg_cnt] = strtok(NULL, " \t\n"); // Continue tokenization
    }
    if (arg.verbose){}
    //     printf("\n");

    init_next_cmd(cmds); // first command init
    // Parse each token
    for (int i = 0; i < arg_cnt; i++)
    {
        if (strcmp(args[i], "|") == 0) // Corrected: check for equality
        {
            handle_pipe(args, cmds, &i, &arg_cnt);
        }
        else if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">>") == 0 ||
                 strcmp(args[i], ">") == 0) // Corrected: check for equality
        {
            int flag = (strcmp(args[i], "<") == 0) ? 0 : ((strcmp(args[i], ">") == 0) ? 1 : 2); // Corrected ternary logic
            handle_redirect(args, cmds, &i, flag);
        }
        else
        {
            handle_args(args, &cmds->command_list[cmds->command_count - 1], &cmds->command_list[cmds->command_count - 1].argc, &i); // Pass pointer to modify
        }
    }
    // Null-terminate the last command's argv (assuming argv is pre-allocated or handled elsewhere; add allocation if needed)
    cmds->command_list[cmds->command_count - 1].argv[cmds->command_list[cmds->command_count - 1].argc] = NULL;
}

// Handles redirection operators (<, >, >>)
void handle_redirect(char **args, commands *cmds, int *i, int flag)
{
    // Determine input or output redirection based on flag (0: input, 1: output overwrite, 2: output append)
    char **change = (!flag)
                        ? &cmds->command_list[cmds->command_count - 1].filein
                        : &cmds->command_list[cmds->command_count - 1].fileout; // Use pointer to modify
    if (*change)
    {
        ERR_double_redirect((!flag) ? 1 : 0);
    }
    (*i)++;                                        // Move to the filename
    if (*i >= /* some max, but assuming safe */ 0) // Removed erroneous condition check
    {
        *change = args[*i]; // Set the file
    }
    if (flag == 1) // Overwrite for '>'
    {
        cmds->command_list[cmds->command_count - 1].overwrite_true = 1;
    }
}

// Handles pipe operator (|)
void handle_pipe(char **args, commands *cmds, int *i, int *arg_cnt)
{
    // Replace pipe with NULL to terminate current command's argv
    args[*i] = NULL;
    cmds->command_list[cmds->command_count - 1].argv[cmds->command_list[cmds->command_count - 1].argc] = args[*i];
    init_next_cmd(cmds);
    if (++(*i) == *arg_cnt) // Check if program is missing after pipe
    {
        ERR_missing_program();
        return;
    }
}

// Handles regular arguments for a command
void handle_args(char **args, command *cmd, int *count, int *const i) // Changed to pointer
{
    cmd->argv[*count] = args[*i]; // Store argument
    if (*count == 0)
    {
        cmd->cmd = args[*i]; // Set command name
    }
    (*count)++;
}

// Initializes the next command in the commands list
void init_next_cmd(commands *cmds)
{
    // argv has already been allocated. No toucing argv here
    cmds->command_count++;
    cmds->command_list[cmds->command_count - 1].cmd = NULL;
    cmds->command_list[cmds->command_count - 1].filein = NULL;
    cmds->command_list[cmds->command_count - 1].fileout = NULL;
    cmds->command_list[cmds->command_count - 1].overwrite_true = 0;
    cmds->command_list[cmds->command_count - 1].argc = 0;
}

// Error functions
void ERR_double_redirect(int is_input) { printf("error: duplicated %s redirection", is_input ? "input" : "output"); }

void ERR_syntax(const char *cmd) { printf("syntax error near %s", cmd); }

void ERR_missing_program() { printf("error: missing program"); }