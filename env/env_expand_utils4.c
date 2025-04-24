#include "../minishell.h"

void	compact_args(t_command *cmd, int *i, int *j)
{
	if (cmd->args[*i] && cmd->args[*i][0] != '\0')
	{
		if (*i != *j)
		{
			cmd->args[*j] = cmd->args[*i];
			cmd->args[*i] = NULL;
		}
		(*j)++;
	}
	else if (cmd->args[*i])
	{
		free(cmd->args[*i]);
		cmd->args[*i] = NULL;
	}
}

void	expand_args_loop(t_command *cmd, char **envp)
{
	int	i;
	int	j;
	int	added;
	int	is_export;

	if (!cmd || !cmd->args || cmd->args_count <= 0)
		return ;
	i = 0;
	j = 0;
	added = 0;
	is_export = (cmd->args[0] && ft_strcmp(cmd->args[0], "export") == 0);
	while (i < cmd->args_count)
	{
		if (cmd->args[i])
		{
			expand_and_strip_arg(cmd, envp, i);
			added = split_and_insert_args(cmd, i, is_export);
			if (added > 0)
				continue ;
			compact_args(cmd, &i, &j);
		}
		i++;
	}
	cmd->args_count = j;
}

void	expand_redirections_loop(t_command *cmd, char **envp)
{
	t_redirections	*redir;
	char			*expanded;

	redir = cmd->redirections;
	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC)
		{
			expanded = expand_variables(redir->file, envp);
			if (expanded)
			{
				free(redir->file);
				redir->file = expanded;
			}
		}
		redir = redir->next;
	}
}