#include "../minishell.h"

static void	process_heredoc_line(char *line, int pipe_fd, int quoted, char **envp)
{
	char	*expanded;

	if (!quoted)
	{
		expanded = expand_variables(line, envp);
		if (expanded)
		{
			ft_putstr_fd(expanded, pipe_fd);
			free(expanded);
		}
		else
			ft_putstr_fd(line, pipe_fd);
	}
	else
		ft_putstr_fd(line, pipe_fd);
	ft_putchar_fd('\n', pipe_fd);
}

static void	read_heredoc_lines(int pipe_fd, char *processed_delimiter, 
		int quoted, char **envp)
{
	char	*line;

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
		process_heredoc_line(line, pipe_fd, quoted, envp);
		safe_free((void **)&line);
	}
}

static void	handle_heredoc_child_process(int pipe_fd[2], char *delimiter,
		char **envp)
{
	int		quoted;
	char	*processed_delimiter;

	setup_heredoc_signals();
	safe_close(&pipe_fd[0]);
	quoted = is_content_quoted(delimiter);
	processed_delimiter = strip_quotes(delimiter);
	if (!processed_delimiter)
		exit(1);
	
	read_heredoc_lines(pipe_fd[1], processed_delimiter, quoted, envp);
	
	safe_free((void **)&processed_delimiter);
	safe_close(&pipe_fd[1]);
	exit(0);
}

static int	handle_heredoc_parent(int pipe_fd[2], pid_t pid)
{
	int	status;

	safe_close(&pipe_fd[1]);
	waitpid(pid, &status, 0);
	setup_signals();
	if ((WIFSIGNALED(status) && WTERMSIG(status) == SIGINT) || 
		status == 33280)
	{
		safe_close(&pipe_fd[0]);
		g_last_exit_status = 130;
		return (130);
	}
	return (pipe_fd[0]);
}

int	setup_heredoc(char *delimiter, char **envp)
{
	int		pipe_fd[2];
	pid_t	pid;

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
	return (handle_heredoc_parent(pipe_fd, pid));
}
