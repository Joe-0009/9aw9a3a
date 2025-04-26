#include "../minishell.h"

void	handle_builtin_command(t_command *current, t_env *env_list)
{
	execute_builtin(current, &env_list);
	exit(g_last_exit_status);
}

void	handle_external_command(t_command *current, t_env *env_list)
{
	char		*path;
	char		**envp;
	struct stat	file_stat;

	envp = env_list_to_envp(env_list);
	if (current->args[0] && ft_strchr(current->args[0], '/'))
	{
		if (stat(current->args[0], &file_stat) == 0
			&& S_ISDIR(file_stat.st_mode))
		{
			ft_fprintf_fd(2, "minishell: %s: Is a directory\n",
				current->args[0]);
			safe_doube_star_free(envp);
			exit(126);
		}
		else if (stat(current->args[0], &file_stat) == -1)
		{
			ft_fprintf_fd(2, "minishell: %s: Not a directory\n",
				current->args[0]);
			safe_doube_star_free(envp);
			exit(127);
		}
	}
	path = find_executable_path(current->args[0], envp);
	if (path)
	{
		if (stat(path, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
		{
			ft_fprintf_fd(2, "minishell: %s: Is a directory\n", path);
			free(path);
			safe_doube_star_free(envp);
			exit(126);
		}
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
		safe_doube_star_free(envp);
		ft_fprintf_fd(2, "minishell: %s: command not found\n",
			current->args[0]);
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
