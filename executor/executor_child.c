#include "../minishell.h"

void	handle_child_input(int prev_pipe_read)
{
	if (prev_pipe_read != -1)
	{
		if (dup2(prev_pipe_read, STDIN_FILENO) == -1)
		{
			perror("minishell: dup2 error on input");
			exit(1);
		}
		safe_close(&prev_pipe_read);
	}
}

void	handle_child_output(t_command *current, int pipe_fd[2])
{
	if (current->next)
	{
		safe_close(&pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("minishell: dup2 error on output");
			exit(1);
		}
		safe_close(&pipe_fd[1]);
	}
}

void	child_process(t_command *current, int prev_pipe_read, int pipe_fd[2],
		t_env *env_list)
{
	set_sigint_default();
	signal(SIGQUIT, SIG_DFL);
	handle_child_input(prev_pipe_read);
	handle_child_output(current, pipe_fd);
	if (setup_redirections(current) == -1)
		exit(1);
	execute_single_command(current, env_list);
	ft_putstr_fd("minishell: command execution failed\n", 2);
	exit(127);
}

int	wait_for_children(void)
{
	pid_t	last_pid;
	int		status;
	int		last_status;

	status = 0;
	last_status = 0;
	while ((last_pid = waitpid(-1, &status, 0)) > 0)
	{
		if (WIFSIGNALED(status))
		{
			last_status = 128 + WTERMSIG(status);
			if (WTERMSIG(status) == SIGQUIT && g_last_exit_status != 131)
				write(STDERR_FILENO, "Quit (core dumped)\n", 19);
			if (WTERMSIG(status) == SIGINT)
				write(STDOUT_FILENO, "\n", 1);
		}
		else if (WIFEXITED(status))
		{
			last_status = WEXITSTATUS(status);
		}
	}
	return (last_status);
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
