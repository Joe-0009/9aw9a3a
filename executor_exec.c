#include "minishell.h"

static char *get_exec_path(t_command *current, char **envp)
{
    char *exec_path = find_executable_path(current->args[0], envp);
    if (!exec_path)
        exec_path = ft_strdup(current->args[0]);
    return exec_path;
}

void execute_single_command(t_command *current, char **envp)
{
    char *exec_path;
    if (current->args && current->args[0] && is_builtin_command(current->args[0]))
        exit(execute_builtin(current, envp));
    else if (current->args && current->args[0])
    {
        exec_path = get_exec_path(current, envp);
        if (!exec_path)
        {
            fprintf(stderr, "minishell: %s: command not found\n", current->args[0]);
            exit(127);
        }
        execve(exec_path, current->args, envp);
        perror("execve error");
        free(exec_path);
        exit(127);
    }
    exit(0);
}

int setup_command_pipe(t_command *current, int *prev_pipe_read, int pipe_fd[2])
{
    if (current->next)
    {
        if (setup_pipe(pipe_fd) == -1)
        {
            safe_close(prev_pipe_read);
            return (0);
        }
    }
    else
    {
        pipe_fd[0] = -1;
        pipe_fd[1] = -1;
    }
    return (1);
}

int is_parent_builtin(char *cmd)
{
    return (ft_strcmp(cmd, "cd") == 0 ||
            ft_strcmp(cmd, "export") == 0 ||
            ft_strcmp(cmd, "unset") == 0 ||
            ft_strcmp(cmd, "exit") == 0);
}
