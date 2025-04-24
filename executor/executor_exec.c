#include "../minishell.h"

static char	*get_exec_path(t_command *current, t_env *env_list)
{
	char	**envp;
	char	*exec_path;

	envp = env_list_to_envp(env_list);
	exec_path = find_executable_path(current->args[0], envp);
	safe_doube_star_free(envp);
	if (!exec_path)
		exec_path = ft_strdup(current->args[0]);
	return (exec_path);
}

static char	*resolve_relative_path(char *exec_path)
{
	char	cwd[PATH_MAX];
	char	*temp;
	char	*temp2;
	char	*resolved_path;

	resolved_path = exec_path;
	if (exec_path[0] != '/' && (exec_path[0] == '.' && exec_path[1] == '/'))
	{
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			temp = exec_path;
			resolved_path = ft_strjoin(cwd, "/");
			temp2 = resolved_path;
			resolved_path = ft_strjoin(resolved_path, temp + 2);
			free(temp);
			free(temp2);
		}
	}
	return (resolved_path);
}

void	handle_builtin_command(t_command *current, t_env *env_list)
{
	execute_builtin(current, &env_list);
	exit(g_last_exit_status);
}

void	handle_external_command(t_command *current, t_env *env_list)
{
	char	*path;
	char	**envp;

	path = find_executable_path(current->args[0], env_list_to_envp(env_list));
	if (path)
	{
		envp = env_list_to_envp(env_list);
		if (execve(path, current->args, envp) == -1)
		{
			perror("minishell: execve");
			free(path);
			safe_doube_star_free(envp);
			exit(126);
		}
	}
	else
	{
		ft_fprintf_fd(2, "minishell: %s: command not found\n", current->args[0]);
		exit(127);
	}
}

void	execute_single_command(t_command *current, t_env *env_list)
{
	if (current->args && current->args[0]
		&& is_builtin_command(current->args[0]))
		exit(execute_builtin(current, &env_list));
	else if (current->args && current->args[0])
		handle_external_command(current, env_list);
	exit(0);
}

int	setup_command_pipe(t_command *current, int *prev_pipe_read, int pipe_fd[2])
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
