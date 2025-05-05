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

static int	setup_pipes_and_heredocs(t_cmd_ctx *cmd_ctx)
{
	int		setup_result;
	char	**envp;

	cmd_ctx->pipe_fd[0] = -1;
	cmd_ctx->pipe_fd[1] = -1;
	cmd_ctx->prev_pipe_read = -1;
	envp = env_list_to_envp(*(cmd_ctx->env_list));
	setup_result = setup_all_heredocs(cmd_ctx->cmd_list, envp);
	safe_doube_star_free(envp);
	if (setup_result == 130)
	{
		g_last_exit_status = 130;
		return (130);
	}
	if (setup_result == -1)
	{
		return (1);
	}
	return (0);
}

static int	setup_command_pipe(t_cmd_ctx *cmd_ctx)
{
	if (cmd_ctx->current->next)
	{
		if (setup_pipe(cmd_ctx->pipe_fd) == -1)
		{
			safe_close(&cmd_ctx->prev_pipe_read);
			return (0);
		}
	}
	else
	{
		cmd_ctx->pipe_fd[0] = -1;
		cmd_ctx->pipe_fd[1] = -1;
	}
	return (1);
}

static pid_t	execute_command_process(t_cmd_ctx *cmd_ctx)
{
	pid_t	pid;
	char	**envp;

	envp = env_list_to_envp(*(cmd_ctx->env_list));
	if (!envp)
		return (-1);
	expand_command_args(cmd_ctx->current, envp);
	if (!setup_command_pipe(cmd_ctx))
		return (safe_doube_star_free(envp), -1);
	pid = fork();
	if (pid == -1)
		return (handle_fork_error(cmd_ctx), safe_doube_star_free(envp), -1);
	if (pid == 0)
		child_process(cmd_ctx);
	signal(SIGINT, SIG_IGN);
	safe_doube_star_free(envp);
	cmd_ctx->prev_pipe_read = parent_process(cmd_ctx->prev_pipe_read,
			cmd_ctx->pipe_fd);
	return (pid);
}

int	execute_command_list(t_command *cmd_list, t_env **env_list)
{
	t_cmd_ctx	cmd_ctx;
	int			status;
	pid_t		last_pid;
	t_command	*last_cmd;

	last_pid = -1;
	cmd_ctx.env_list = env_list;
	cmd_ctx.cmd_list = cmd_list;
	cmd_ctx.current = cmd_list;
	cmd_ctx.cmd_size = count_commands(cmd_list);
	if (cmd_ctx.cmd_list && cmd_ctx.cmd_list->next == NULL
		&& cmd_ctx.cmd_list->args && cmd_ctx.cmd_list->args[0]
		&& is_parent_builtin(cmd_ctx.cmd_list->args[0]))
		return (execute_single_parent_builtin(&cmd_ctx));
	cmd_ctx.init_result = setup_pipes_and_heredocs(&cmd_ctx);
	if (cmd_ctx.init_result != 0)
		return (cmd_ctx.init_result);
	last_cmd = cmd_list;
	while (last_cmd && last_cmd->next)
		last_cmd = last_cmd->next;
	while (cmd_ctx.current)
	{
		if (cmd_ctx.current == last_cmd)
			last_pid = execute_command_process(&cmd_ctx);
		else
			execute_command_process(&cmd_ctx);
		cmd_ctx.current = cmd_ctx.current->next;
	}
	status = wait_for_specific_pid(last_pid);
	signal(SIGINT, sigint_handler);
	cmd_ctx.status = status;
	return (status);
}
