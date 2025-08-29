#include "memalloc.h"
#include "exec.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/wait.h>
#include <fcntl.h>
#include "../include/my_shell.h"

void exec_redirect(commands *cmd)
{
    if (cmd->command_list[0].filein)
    {
        int fd = open(cmd->command_list[0].filein, O_RDONLY);
        if (fd < 0)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (cmd->command_list[0].fileout)
    {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->command_list[0].overwrite_true)
        {
            flags |= O_TRUNC;
        }
        else
        {
            flags |= O_APPEND;
        }
        int fd = open(cmd->command_list[0].fileout, flags, 0644);
        if (fd < 0)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

char *resolve_path(char *cmd_name)
{
    if (strchr(cmd_name, '/'))
    {
        return strdup(cmd_name);
    }
    char *env = getenv("PATH");
    if (!env)
    {
        return NULL;
    }
    char *path_copy = strdup(env);
    if (!path_copy)
    {
        return NULL;
    }
    char *dir = strtok(path_copy, ":");
    while (dir)
    {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd_name);
        if (access(full_path, X_OK) == 0)
        {
            free(path_copy);
            return strdup(full_path);
        }
        dir = strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
}

void exec(args arg, char *cmd, char **args, char **envp, commands *cmds)
{
    // now I can only process command count of 1, for more commands it is
    // necessary to use pipes and more advanced function to run
    if (arg.verbose)
    {
        for (int i = 0; i < cmds->command_count; i++)
        {
            fprintf(stderr, "number:%d\t", i);
            for (int j = 0; j < cmds->command_list[i].argc; j++)
                fprintf(stderr, "%dth cmd:%s\t", j,
                        cmds->command_list[i].argv[j]);
            fprintf(stderr, "\n");
            fprintf(stderr,"stdin:%s\tstdout:%s\tis_overwrite:%d\n",cmds->command_list[i].filein,cmds->command_list[i].fileout,cmds->command_list[i].overwrite_true );
        }
    }
    for (int i = 0; i < cmds->command_count; i++)
    {
        if (strcmp(cmds->command_list[i].cmd, "exit") == 0)
        {
            free_all(cmd, args, cmds);
            printf("exit\n");
            exit(0);
        }
        char *resolved_cmd = resolve_path(cmds->command_list[i].cmd);
        if (arg.verbose)
        {
            printf("running process %s, number %d in stack\n", resolved_cmd, i);
        }
        if (!resolved_cmd)
        {
            fprintf(stderr, "my shell: command not found: %s\n", cmds->command_list[i].cmd);
            continue;
        }
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            exec_redirect(cmds);
            execve(resolved_cmd, cmds->command_list[i].argv, envp);
            perror("execve");
            exit(1);
        }
        else if (pid > 0)
        {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
            free(resolved_cmd);
        }
        else
        {
            perror("fork");
            free(resolved_cmd);
        }
    }
}