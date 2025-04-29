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
				{
					g_last_exit_status = 130;
					return (130);
				}
				if (result == -1)
					return (-1);
			}
			redir = redir->next;
		}
		current = current->next;
	}
	return (0);
}

static int	init_and_setup_heredocs(t_command *cmd_list, t_env **env_list,
		int pipe_fd[2], int *prev_pipe_read)
{
	int		setup_result;
	char	**envp;

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
		return (1);
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	*prev_pipe_read = -1;
	return (0);
}

static void	execute_command_process(t_command *current, int *prev_pipe_read,
		int pipe_fd[2], t_env **env_list)
{
	pid_t	pid;

	expand_command_args(current, env_list_to_envp(*env_list));
	if (!setup_command_pipe(current, prev_pipe_read, pipe_fd))
		return ;
	pid = fork();
	if (pid == -1)
	{
		handle_fork_error(current, *prev_pipe_read, pipe_fd);
		return ;
	}
	if (pid == 0)
		child_process(current, *prev_pipe_read, pipe_fd, *env_list);
	*prev_pipe_read = parent_process(*prev_pipe_read, pipe_fd);
}

static int	handle_executor_init(t_command *cmd_list, t_env **env_list,
		int pipe_fd[2], int *prev_pipe_read)
{
	int	init_result;

	if (cmd_list && cmd_list->next == NULL && cmd_list->args
		&& cmd_list->args[0] && is_parent_builtin(cmd_list->args[0]))
		return (execute_single_parent_builtin(cmd_list, env_list));
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	*prev_pipe_read = -1;
	init_result = init_and_setup_heredocs(cmd_list, env_list, pipe_fd,
			prev_pipe_read);
	if (init_result == 130)
	{
		setup_signals();
		return (130);
	}
	else if (init_result != 0)
	{
		setup_signals();
		return (1);
	}
	return (0);
}

int	execute_command_list(t_command *cmd_list, t_env **env_list)
{
	int			pipe_fd[2];
	int			prev_pipe_read;
	int			status;
	t_command	*current;
	int			init_result;

	setup_exec_signals();
	init_result = handle_executor_init(cmd_list, env_list, pipe_fd,
			&prev_pipe_read);
	if (init_result != 0)
		return (init_result);
	current = cmd_list;
	while (current)
	{
		execute_command_process(current, &prev_pipe_read, pipe_fd, env_list);
		current = current->next;
	}
	status = wait_for_children();
	setup_signals();
	return (status);
}
