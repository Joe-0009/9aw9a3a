#include "../minishell.h"

int	setup_redirect_in(char *file_path)
{
	int	fd;

	fd = open(file_path, O_RDONLY);
	if (fd == -1)
	{
		ft_fprintf_fd(2, "minishell: %s: %s\n", file_path, strerror(errno));
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		ft_fprintf_fd(2, "minishell: input redirection failed: %s\n",
			strerror(errno));
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

int	setup_redirect_out(char *file_path, int append_mode)
{
	int	flags;
	int	fd;

	flags = O_WRONLY | O_CREAT;
	if (append_mode)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(file_path, flags, 0644);
	if (fd == -1)
	{
		ft_fprintf_fd(2, "minishell: %s: %s\n", file_path, strerror(errno));
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		ft_fprintf_fd(2, "minishell: output redirection failed: %s\n",
			strerror(errno));
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	handle_heredoc_redirection(t_redirections *redir)
{
	if (dup2(redir->heredoc_fd, STDIN_FILENO) == -1)
	{
		ft_fprintf_fd(2, "minishell: heredoc redirection failed: %s\n",
			strerror(errno));
		return (-1);
	}
	safe_close(&redir->heredoc_fd);
	return (0);
}

static int	handle_single_redirection(t_redirections *redir)
{
	int	result;

	result = 0;
	if (redir->type == TOKEN_REDIRECT_IN)
		result = setup_redirect_in(redir->file);
	else if (redir->type == TOKEN_REDIRECT_OUT)
		result = setup_redirect_out(redir->file, 0);
	else if (redir->type == TOKEN_APPEND)
		result = setup_redirect_out(redir->file, 1);
	else if (redir->type == TOKEN_HEREDOC)
		result = handle_heredoc_redirection(redir);
	else
	{
		ft_putstr_fd("minishell: unknown redirection type\n", 2);
		result = -1;
	}
	return (result);
}

int	setup_redirections(t_command *cmd)
{
	t_redirections	*redir;
	int				result;

	redir = cmd->redirections;
	result = 0;
	while (redir && result == 0)
	{
		result = handle_single_redirection(redir);
		redir = redir->next;
	}
	return (result);
}
