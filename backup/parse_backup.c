#include "parse.h"
#include "memalloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/wait.h>

// Function declarations for modularity (helper functions only)
void handle_space(char *cmd, char **args, int *args_count, int *last_avail_ptr, int *cmd_last_ptr, int i);
void save_argument(char *cmd, char **args, int *args_count, int start, int end);
void finalize_arguments(char *cmd, char **args, int *args_count, int last_avail_ptr, int i, int cmd_last_ptr);
void print_debug_info(char *cmd, char **args, int args_count);
void handle_redirect(char *cmd, commands *cmds, int *i);
void split(args arg, char *cmd, char **args, commands *cmds);

args parseargs(int argc, char **argv)
{
    args ret = {.help = 0, .wrong = 0};
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

void eval(args arg, char *cmd, char **args, commands *cmds)
{
    split(arg, cmd, args, cmds);
}

// 修改后的 split 函数
void split(args arg, char *cmd, char **args, commands *cmds)
{
    // 初始化命令结构
    cmds->command_count = 1;
    cmds->command_list = malloc(sizeof(command));
    cmds->command_list[0].cmd = cmd;
    cmds->command_list[0].argv = args;
    cmds->command_list[0].filein = NULL;
    cmds->command_list[0].fileout = NULL;
    cmds->command_list[0].overwrite_true = 0;
    int args_count = 1;
    int last_avail_ptr = 0;
    int cmd_last_ptr = 0;
    args[0] = cmd;
    for (int i = 0; i < MAX_INPUT_LENGTH && cmd[i] != '\0'; i++)
    {
        if (cmd[i] == '\n' || cmd[i] == '\0')
        {
            finalize_arguments(cmd, args, &args_count, last_avail_ptr, i, cmd_last_ptr);
            args[args_count] = NULL;
            if (arg.verbose)
            {
                print_debug_info(cmd, args, args_count);
                fprintf(stderr, "redir in: %s\nredir out: %s\n",  cmds->command_list[0].fileout ? cmds->command_list[0].fileout : "NULL",cmds->command_list[0].fileout ? cmds->command_list[0].fileout : "NULL");
            }
            return;
        }
        else if (cmd[i] == ' ')
        {
            // fprintf(stderr, "%d\n", i);
            handle_space(cmd, args, &args_count, &last_avail_ptr, &cmd_last_ptr,
                             i);
        }
        else if (cmd[i] == '<' || cmd[i] == '>')
        {
            // 处理重定向，但不将其作为参数
            handle_redirect(cmd, cmds, &i);
            // 重定向处理后，last_avail_ptr 需要更新到当前位置之后
            last_avail_ptr = i + 1;
        }
    }
}

/**
 * Handles space characters: either marks the end of the command or saves an argument.
 */
void handle_space(char *cmd, char **args, int *args_count, int *last_avail_ptr, int *cmd_last_ptr, int i)
{
    if (*cmd_last_ptr == 0)
    {
        // First space: mark command end
        *cmd_last_ptr = i;
        *last_avail_ptr = i + 1;
    }
    else if (*last_avail_ptr >= 0 && *last_avail_ptr < i)
    {
        // Subsequent space: save the current argument if there's content
        save_argument(cmd, args, args_count, *last_avail_ptr, i);
        *last_avail_ptr = i + 1;
    }
    else
    {
        // Consecutive spaces: just update last_avail_ptr
        *last_avail_ptr = i + 1;
    }
}

/**
 * Saves an argument from the command string into the args array.
 */
void save_argument(char *cmd, char **args, int *args_count, int start, int end)
{
    int len = end - start;
    if (len <= 0)
    {
        return; // Don't save empty arguments
    }
    if (len > 63)
    {
        free_all(cmd, args, NULL); // Pass NULL for commands (not used yet)
        fprintf(stderr, "Sorry, currently args length longer than 64 are not supported.\n");
        exit(0);
    }
    // Allocate memory for the argument
    args[*args_count] = malloc((len + 1) * sizeof(char));
    if (!args[*args_count])
    {
        fprintf(stderr, "Memory allocation failed for args[%d].\n", *args_count);
        exit(1);
    }
    strncpy(args[*args_count], &cmd[start], len);
    args[*args_count][len] = '\0';
    // fprintf(stderr,"%d,%s\n",*args_count,args[*args_count]);
    (*args_count)++;
}

/**
 * Finalizes arguments at the end of input (newline or null).
 */
void finalize_arguments(char *cmd, char **args, int *args_count, int last_avail_ptr, int i, int cmd_last_ptr)
{
    if (last_avail_ptr >= 0 && last_avail_ptr < i && cmd_last_ptr > 0)
    {
        save_argument(cmd, args, args_count, last_avail_ptr, i);
    }
    // Null-terminate the command
    if (cmd_last_ptr > 0)
    {
        cmd[cmd_last_ptr] = '\0';
    }
    else
    {
        cmd[i] = '\0';
    }
}

/**
 * Handles IO redirects: < for input, > for output (overwrite), >> for output (append).
 */
void handle_redirect(char *cmd, commands *cmds, int *i)
{
    if (cmd[*i] == '<')
    {
        // 输入重定向
        (*i)++; // 跳过 '<'
        // 跳过空格找到文件名
        while (cmd[*i] == ' ')
            (*i)++;

        int start = *i;
        // 找到文件名结束位置
        while (cmd[*i] != ' ' && cmd[*i] != '\0' && cmd[*i] != '\n')
            (*i)++;
        int end = *i;

        // 保存文件名
        int len = end - start;
        cmds->command_list[0].filein = malloc(len + 1);
        strncpy(cmds->command_list[0].filein, &cmd[start], len);
        cmds->command_list[0].filein[len] = '\0';

        // 回退一步，让主循环继续处理
        (*i)--;
    }
    else if (cmd[*i] == '>')
    {
        // 处理输出重定向
        if (cmd[*i + 1] == '>')
        {
            // 追加模式 ">>"
            cmds->command_list[0].overwrite_true = 0;
            (*i) += 2; // 跳过 ">>"
        }
        else
        {
            // 覆盖模式 ">"
            cmds->command_list[0].overwrite_true = 1;
            (*i) += 1; // 跳过 ">"
        }

        // 跳过空格找到文件名
        while (cmd[*i] == ' ')
            (*i)++;

        int start = *i;
        // 找到文件名结束位置
        while (cmd[*i] != ' ' && cmd[*i] != '\0' && cmd[*i] != '\n')
            (*i)++;
        int end = *i;

        // 保存文件名
        int len = end - start;
        cmds->command_list[0].fileout = malloc(len + 1);
        strncpy(cmds->command_list[0].fileout, &cmd[start], len);
        cmds->command_list[0].fileout[len] = '\0';

        // 回退一步，让主循环继续处理
        (*i)--;
    }
}

/**
 * Prints debug information
 */
void print_debug_info(char *cmd, char **args, int args_count)
{
    fprintf(stderr, "Debug info:\n");
    fprintf(stderr, "Command: %s\n", cmd);
    for (int i = 0; i < args_count; i++)
    {
        fprintf(stderr, "args[%d]: %s\n", i, args[i]);
    }
}
