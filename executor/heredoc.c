#include "../minishell.h"

static int	is_delimiter_quoted(char *delimiter)
{
	size_t	i;
	size_t	len;
	int		in_quotes;
	char	quote_type;
	int		quoted;

	i = 0;
	in_quotes = 0;
	quoted = 0;
	len = ft_strlen(delimiter);
	while (i < len)
	{
		if ((delimiter[i] == '"' || delimiter[i] == '\''))
		{
			if (!in_quotes)
			{
				in_quotes = 1;
				quote_type = delimiter[i];
				i++;
			}
			else if (in_quotes && delimiter[i] == quote_type)
			{
				quoted = 1;
				in_quotes = 0;
				i++;
			}
			else
				i++;
		}
		else
			i++;
	}
	return (quoted);
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
	{
		int	quoted;
		char	*line;
		char	*processed_delimiter;

		setup_heredoc_signals(); // Use heredoc-specific signal handler
		safe_close(&pipe_fd[0]);
		quoted = is_delimiter_quoted(delimiter);
		processed_delimiter = strip_quotes(delimiter);
		if (!processed_delimiter)
			exit(1);
		while (1)
		{
			line = readline("> ");
			if (!line)
			{
				// Check if this was due to SIGINT or actual EOF
				if (g_last_exit_status == 130)  // SIGINT was received
					break;
				ft_putstr_fd("minishell: warning: heredoc delimited by end-of-file\n", 2);
				break ;
			}
			if (ft_strcmp(line, processed_delimiter) == 0)
			{
				safe_free((void **)&line);
				break ;
			}
			if (!quoted)
				ft_putstr_fd(expand_variables(line, envp), pipe_fd[1]);
			else
				ft_putstr_fd(line, pipe_fd[1]);
			ft_putchar_fd('\n', pipe_fd[1]);
			safe_free((void **)&line);
		}
		safe_free((void **)&processed_delimiter);
		safe_close(&pipe_fd[1]);
		exit(0);
	}
	safe_close(&pipe_fd[1]);
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		safe_close(&pipe_fd[0]);
		g_last_exit_status = 130;
		return (130);
	}
	return (pipe_fd[0]);
}
