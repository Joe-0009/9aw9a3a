#include "../minishell.h"

static int	check_var_in_single_quotes(char *arg, int *in_single_quotes)
{
	t_state	state;
	int		j;

	*in_single_quotes = 0;
	state = STATE_NORMAL;
	j = 0;
	while (arg[j])
	{
		update_quote_state(arg[j], &state);
		if (arg[j] == '$' && state == STATE_IN_SINGLE_QUOTE)
		{
			*in_single_quotes = 1;
			return (1);
		}
		j++;
	}
	return (0);
}

void	expand_and_strip_arg(t_command *cmd, char **envp, int i)
{
	char	*expanded;
	char	*stripped;
	int		in_single_quotes;

	check_var_in_single_quotes(cmd->args[i], &in_single_quotes);
	if (!in_single_quotes)
	{
		expanded = expand_variables(cmd->args[i], envp);
		if (expanded)
		{
			free(cmd->args[i]);
			cmd->args[i] = expanded;
		}
	}
	stripped = strip_quotes(cmd->args[i]);
	if (stripped)
	{
		free(cmd->args[i]);
		cmd->args[i] = stripped;
	}
}

static void	process_arg_expansion(t_command *cmd, char **envp, int i,
		int *was_quoted, int *had_quoted)
{
	char	*stripped;

	*was_quoted = was_quoted(cmd->args[i]);
	*had_quoted = has_var_in_dquotes(cmd->args[i]);
	if (!is_var_in_squotes(cmd->args[i]))
		expand_and_strip_arg(cmd, envp, i);
	else
	{
		stripped = strip_quotes(cmd->args[i]);
		if (stripped)
		{
			free(cmd->args[i]);
			cmd->args[i] = stripped;
		}
	}
}

void	expand_args_loop(t_command *cmd, char **envp)
{
	int		i;
	int		j;
	int		added;
	int		is_export;
	int		had_quoted_vars;
	int		was_arg_quoted;

	i = 0;
	j = 0;
	added = 0;
	if (!cmd || !cmd->args || cmd->args_count <= 0)
		return ;
	is_export = (cmd->args[0] && ft_strcmp(cmd->args[0], "export") == 0);
	while (i < cmd->args_count)
	{
		if (!cmd->args[i])
		{
			i++;
			continue ;
		}
		process_arg_expansion(cmd, envp, i, &was_arg_quoted, &had_quoted_vars);
		if (!was_arg_quoted && !had_quoted_vars)
			added = split_and_insert_args(cmd, i, is_export);
		else
			added = 0;
		if (added > 0)
			continue ;
		compact_args(cmd, &i, &j);
		i++;
	}
	cmd->args_count = j;
}

void	expand_redirections_loop(t_command *cmd, char **envp)
{
	t_redirections *redir;
	char *expanded;

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