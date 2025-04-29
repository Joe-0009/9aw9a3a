#include "../minishell.h"

static void	handle_directory_errors(char *path, char **envp)
{
	struct stat	file_stat;

	if (stat(path, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
	{
		ft_fprintf_fd(2, "minishell: %s: Is a directory\n", path);
		if (ft_strchr(path, '/'))
			free(path);
		safe_doube_star_free(envp);
		exit(126);
	}
	else if (stat(path, &file_stat) == -1 && ft_strchr(path, '/'))
	{
		ft_fprintf_fd(2, "minishell: %s: Not a directory\n", path);
		safe_doube_star_free(envp);
		exit(127);
	}
}

static void	exec_command(char *path, t_command *current, char **envp)
{
	struct stat	file_stat;

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

	envp = env_list_to_envp(env_list);
	if (current->args[0] && ft_strchr(current->args[0], '/'))
		handle_directory_errors(current->args[0], envp);
	path = find_executable_path(current->args[0], envp);
	exec_command(path, current, envp);
	safe_doube_star_free(envp);
	ft_fprintf_fd(2, "minishell: %s: command not found\n",
		current->args[0]);
	exit(127);
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
