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
	envp = env_list_to_envp(cmd_ctx->env_list);
	setup_result = setup_all_heredocs(cmd_ctx->cmd_list, envp);
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
static int	setup_command_pipe(t_cmd_ctx *cmd_ctx)
{
	if (cmd_ctx->current->next)
	{
		if (setup_pipe(cmd_ctx->pipe_fd) == -1)
		{
			safe_close(cmd_ctx->prev_pipe_read);
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

static void	execute_command_process(t_cmd_ctx *cmd_ctx)
{
	pid_t	pid;
	char	**envp;

	envp = env_list_to_envp(cmd_ctx);
	if (!envp)
		return ;
	expand_command_args(cmd_ctx->current, envp);
	if (!setup_command_pipe(cmd_ctx))
	{
		safe_doube_star_free(envp);
		return ;
	}
	pid = fork();
	if (pid == -1)
	{
		handle_fork_error(cmd_ctx);
		safe_doube_star_free(envp);
		return ;
	}
	if (pid == 0)
		child_process(cmd_ctx);
	safe_doube_star_free(envp);
	cmd_ctx->prev_pipe_read = parent_process(cmd_ctx);
}

int	execute_command_list(t_command *cmd_list, t_env **env_list)
{
	t_cmd_ctx	*cmd_ctx;

	cmd_ctx->env_list = env_list;
	cmd_ctx->cmd_list = cmd_list;
	cmd_ctx->cmd_size = ft_lstsize(cmd_list);
	setup_exec_signals();
	if (cmd_list && cmd_list->next == NULL && cmd_list->args
		&& cmd_list->args[0] && is_parent_builtin(cmd_list->args[0]))
		return (execute_single_parent_builtin(cmd_list, env_list));
	cmd_ctx->init_result = setup_pipes_and_heredocs(cmd_ctx);
	if (cmd_ctx->init_result != 0)
		return (cmd_ctx->init_result);
	cmd_ctx->current = cmd_list;
	while (cmd_ctx->current)
	{
		execute_command_process(cmd_ctx);
		cmd_ctx->current = cmd_ctx->current->next;
	}
	cmd_ctx->status = wait_for_children();
	setup_signals();
	return (cmd_ctx->status);
}
