#include "../minishell.h"

void	handle_child_input(t_cmd_ctx *cmd_ctx)
{
	if (cmd_ctx->prev_pipe_read != -1)
	{
		if (dup2(cmd_ctx->prev_pipe_read, STDIN_FILENO) == -1)
		{
			perror("minishell: dup2 error on input");
			exit(1);
		}
		safe_close(cmd_ctx->prev_pipe_read);
	}
}

void	handle_child_output(t_cmd_ctx *cmd_ctx)
{
	if (cmd_ctx->current->next)
	{
		safe_close(cmd_ctx->pipe_fd[0]);
		if (dup2(cmd_ctx->pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("minishell: dup2 error on output");
			exit(1);
		}
		safe_close(cmd_ctx->pipe_fd[1]);
	}
}

void	child_process(t_cmd_ctx *cmd_ctx)
{
	setup_exec_signals();
	handle_child_input(cmd_ctx);
	handle_child_output(cmd_ctx);
	if (setup_redirections(cmd_ctx->current) == -1)
		exit(1);
	execute_single_command(cmd_ctx);
	ft_fprintf_fd(2, "minishell: command execution failed\n");
	exit(127);
}

static int	handle_process_status(pid_t pid, int status, pid_t last_pid)
{
	int	last_command_status;
	int	exit_status;

	last_command_status = 0;
	exit_status = 0;
	if (pid == last_pid)
	{
		if (WIFEXITED(status))
			last_command_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_command_status = 128 + WTERMSIG(status);
	}
	if (WIFSIGNALED(status))
	{
		exit_status = 128 + WTERMSIG(status);
		if (WTERMSIG(status) == SIGQUIT && g_last_exit_status != 131)
			write(STDERR_FILENO, "Quit: 3\n", 9);
		if (WTERMSIG(status) == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
	}
	else if (WIFEXITED(status))
		exit_status = WEXITSTATUS(status);
	return (pid == last_pid ? last_command_status : exit_status);
}

int	wait_for_specific_pid(pid_t last_pid)
{
	pid_t	pid;
	int		status;
	int		last_status;
	int		last_command_status;

	status = 0;
	last_status = 0;
	last_command_status = 0;
	while ((pid = waitpid(-1, &status, 0)) > 0)
	{
		if (pid == last_pid)
			last_command_status = handle_process_status(pid, status, last_pid);
		else
			last_status = handle_process_status(pid, status, -1);
	}
	if (last_pid != -1)
		return (last_command_status);
	return (last_status);
}

int	wait_for_children(void)
{
	pid_t	pid;
	int		status;
	int		last_command_status;

	status = 0;
	last_command_status = 0;
	while ((pid = waitpid(-1, &status, 0)) > 0)
	{
		if (WIFEXITED(status))
			last_command_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			last_command_status = 128 + WTERMSIG(status);
			if (WTERMSIG(status) == SIGQUIT && g_last_exit_status != 131)
				write(STDERR_FILENO, "Quit: 3\n", 9);
			if (WTERMSIG(status) == SIGINT)
				write(STDOUT_FILENO, "\n", 1);
		}
	}
	return (last_command_status);
}

int	parent_process(int prev_pipe_read, int pipe_fd[2])
{
	if (prev_pipe_read != -1)
		safe_close(&prev_pipe_read);
	if (pipe_fd[1] != -1)
	{
		safe_close(&pipe_fd[1]);
		return (pipe_fd[0]);
	}
	if (pipe_fd[0] != -1)
		return (pipe_fd[0]);
	return (-1);
}
