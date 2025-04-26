#include "../minishell.h"

static void	handle_heredoc_child_process(int pipe_fd[2], char *delimiter,
		char **envp)
{
	int		quoted;
	char	*line;
	char	*processed_delimiter;
	char	*expanded;

	setup_heredoc_signals();
	safe_close(&pipe_fd[0]);
	quoted = is_content_quoted(delimiter);
	processed_delimiter = strip_quotes(delimiter);
	if (!processed_delimiter)
		exit(1);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			if (g_last_exit_status == 130)
				break ;
			ft_putstr_fd("minishell: warning: heredoc delimited by end-of-file\n", 2);
			break ;
		}
		if (ft_strcmp(line, processed_delimiter) == 0)
		{
			safe_free((void **)&line);
			break ;
		}
		if (!quoted)
		{
			expanded = expand_variables(line, envp);
			if (expanded)
			{
				ft_putstr_fd(expanded, pipe_fd[1]);
				free(expanded);
			}
			else
				ft_putstr_fd(line, pipe_fd[1]);
		}
		else
			ft_putstr_fd(line, pipe_fd[1]);
		ft_putchar_fd('\n', pipe_fd[1]);
		safe_free((void **)&line);
	}
	safe_free((void **)&processed_delimiter);
	safe_close(&pipe_fd[1]);
	exit(0);
}

int	setup_heredoc(char *delimiter, char **envp)
{
	int		pipe_fd[2];
	pid_t	pid;
	int		status;

	if (pipe(pipe_fd) == -1)
		return (ft_putstr_fd("minishell: heredoc pipe error\n", 2), -1);
	pid = fork();
	if (pid == -1)
	{
		safe_close(&pipe_fd[0]);
		safe_close(&pipe_fd[1]);
		return (ft_putstr_fd("minishell: heredoc fork error\n", 2), -1);
	}
	if (pid == 0)
		handle_heredoc_child_process(pipe_fd, delimiter, envp);
	safe_close(&pipe_fd[1]);
	waitpid(pid, &status, 0);
	setup_signals();
	if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT) || 
		status == 33280 || g_last_exit_status == 130)
	{
		safe_close(&pipe_fd[0]);
		g_last_exit_status = 130;
		return (130);
	}
	return (pipe_fd[0]);
}
