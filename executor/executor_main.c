#include "../minishell.h"

static int	setup_all_heredocs(t_command *cmd_list, char **envp)
{
	t_command		*current;
	t_redirections	*redir;
	int				result;

	current = cmd_list;
	while (current)
	{
		redir = current->redirections;
		while (redir)
		{
			if (redir->type == TOKEN_HEREDOC)
			{
				result = handle_heredoc_redir(redir, envp);
				if (result == 130)
					return (130);
				if (result == -1)
					return (-1);
			}
			redir = redir->next;
		}
		current = current->next;
	}
	return (0);
}

static int	setup_pipes_and_heredocs(t_command *cmd_list, t_env **env_list,
		int pipe_fd[2], int *prev_pipe_read)
{
	int		setup_result;
	char	**envp;

	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	*prev_pipe_read = -1;
	envp = env_list_to_envp(*env_list);
	setup_result = setup_all_heredocs(cmd_list, envp);
	safe_doube_star_free(envp);
	if (setup_result == 130)
	{
		g_last_exit_status = 130;
		setup_signals();
		return (130);
	}
	if (setup_result == -1)
	{
		setup_signals();
		return (1);
	}
	return (0);
}
static int	setup_command_pipe(t_command *current, int *prev_pipe_read,
		int pipe_fd[2])
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

// static void	execute_command_process(t_command *current, int *prev_pipe_read,
// 		int pipe_fd[2], t_env **env_list)
// {
// 	pid_t	pid;
// 	char	**envp;

// 	envp = env_list_to_envp(*env_list);
// 	if (!envp)
// 		return ;
// 	expand_command_args(current, envp);
// 	if (!setup_command_pipe(current, prev_pipe_read, pipe_fd))
// 	{
// 		safe_doube_star_free(envp);
// 		return ;
// 	}
// 	pid = fork();
// 	if (pid == -1)
// 	{
// 		handle_fork_error(current, *prev_pipe_read, pipe_fd);
// 		safe_doube_star_free(envp);
// 		return ;
// 	}
// 	if (pid == 0)
// 		child_process(current, *prev_pipe_read, pipe_fd, *env_list);
// 	safe_doube_star_free(envp);
// 	*prev_pipe_read = parent_process(*prev_pipe_read, pipe_fd);
// }

static int	check_parent_builtin(t_command *cmd_list, t_env **env_list)
{
	if (cmd_list && cmd_list->next == NULL && cmd_list->args
		&& cmd_list->args[0] && is_parent_builtin(cmd_list->args[0]))
		return (execute_single_parent_builtin(cmd_list, env_list));
	return (0);
}

static t_command	*find_last_command(t_command *cmd_list)
{
	t_command	*last_command;

	last_command = cmd_list;
	while (last_command && last_command->next)
		last_command = last_command->next;
	return (last_command);
}

static int	execute_single_pipe_command(t_command *current, int *prev_pipe_read,
		int pipe_fd[2], t_env **env_list, t_command *last_command,
		pid_t *last_pid)
{
	pid_t	pid;
	char	**envp;

	envp = env_list_to_envp(*env_list);
	if (!envp)
		return (0);
	expand_command_args(current, envp);
	if (!setup_command_pipe(current, prev_pipe_read, pipe_fd))
	{
		safe_doube_star_free(envp);
		return (0);
	}
	if ((pid = fork()) == -1)
	{
		handle_fork_error(current, *prev_pipe_read, pipe_fd);
		safe_doube_star_free(envp);
		return (0);
	}
	if (pid == 0)
		child_process(current, *prev_pipe_read, pipe_fd, *env_list);
	if (current == last_command)
		*last_pid = pid;
	safe_doube_star_free(envp);
	*prev_pipe_read = parent_process(*prev_pipe_read, pipe_fd);
	return (1);
}
static int	execute_pipeline(t_command *cmd_list, int *prev_pipe_read,
		int pipe_fd[2], t_env **env_list, pid_t *last_pid)
{
	t_command	*current;
	t_command	*last_command;

	last_command = find_last_command(cmd_list);
	current = cmd_list;
	while (current)
	{
		if (!execute_single_pipe_command(current, prev_pipe_read, pipe_fd,
				env_list, last_command, last_pid))
			return (0);
		current = current->next;
	}
	return (1);
}

int	execute_command_list(t_command *cmd_list, t_env **env_list)
{
	int pipe_fd[2];
	int prev_pipe_read;
	int status;
	pid_t last_pid;
	int result;

	setup_exec_signals();
	if ((result = check_parent_builtin(cmd_list, env_list)))
		return (result);
	if ((result = setup_pipes_and_heredocs(cmd_list, env_list, pipe_fd,
				&prev_pipe_read)))
		return (result);
	last_pid = -1;
	execute_pipeline(cmd_list, &prev_pipe_read, pipe_fd, env_list, &last_pid);
	status = wait_for_specific_pid(last_pid);
	setup_signals();
	return (status);
}
