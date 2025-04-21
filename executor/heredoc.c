#include "../minishell.h"

static int is_delimiter_quoted(char *delimiter)
{
	size_t i;
	size_t len;
	int in_quotes;
	char quote_type;
	int quoted;

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
	char	*line;
	char	*processed_delimiter;
	int quoted;


	if (pipe(pipe_fd) == -1)
		return (ft_putstr_fd("minishell: heredoc pipe error\n", 2), -1);
	quoted = is_delimiter_quoted(delimiter);
	processed_delimiter = process_heredoc_delimiter(delimiter);
	if (!processed_delimiter)
		return (-1);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_putstr_fd("minishell: warning: heredoc delimited by end-of-file\n", 2);
			safe_close(&pipe_fd[1]);
			safe_free((void **)&processed_delimiter);
			break ;
		}
		if (ft_strcmp(line, processed_delimiter) == 0)
		{
			safe_free((void **)&line);
			safe_free((void **)&processed_delimiter);
			safe_close(&pipe_fd[1]);
			break ;
		}
		if (!quoted)
			ft_putstr_fd(expand_variables(line, envp), pipe_fd[1]);
		else
			ft_putstr_fd(line, pipe_fd[1]);
		ft_putchar_fd('\n', pipe_fd[1]);
		safe_free((void **)&line);
	}
	safe_close(&pipe_fd[1]);
	return (pipe_fd[0]);
}


char	*process_heredoc_delimiter(const char *delimiter)
{
	size_t	len;
	char	*result;
	size_t	i, j;
	int		in_quotes;
	char	quote_type;

	if (!delimiter)
		return (NULL);
	len = ft_strlen(delimiter);
	result = (char *)ft_calloc(len + 1, sizeof(char));
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	in_quotes = 0;
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
			else if (delimiter[i] == quote_type)
			{
				in_quotes = 0;
				i++;
			}
			else
				result[j++] = delimiter[i++];
		}
		else
			result[j++] = delimiter[i++];
	}
	return (result);
}
