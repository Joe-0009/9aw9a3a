#include "../minishell.h"

static void	process_arg_expansion(t_expand_vars *v)
{
	char	*stripped;

	v->was_arg_quoted = was_quoted(v->cmd->args[v->i]);
	v->had_quoted_vars = has_var_in_dquotes(v->cmd->args[v->i]);
	if (!is_var_in_squotes(v->cmd->args[v->i]))
		expand_and_strip_arg(v->cmd, v->envp, v->i);
	else
	{
		stripped = strip_quotes(v->cmd->args[v->i]);
		if (stripped)
		{
			free(v->cmd->args[v->i]);
			v->cmd->args[v->i] = stripped;
		}
	}
}

static void	restore_command_name(t_expand_vars *v, char *orig_cmd)
{
	if (orig_cmd && v->cmd->args && v->cmd->args_count > 0)
	{
		if (!v->cmd->args[0] || strcmp(orig_cmd, v->cmd->args[0]) != 0)
		{
			free(v->cmd->args[0]);
			v->cmd->args[0] = orig_cmd;
		}
		else
			free(orig_cmd);
	}
}

static void	process_argument(t_expand_vars *v)
{
	process_arg_expansion(v);
	if (v->i > 0 || v->cmd->args_count == 1)
	{
		if (!v->was_arg_quoted && !v->had_quoted_vars)
		{
			v->added = split_and_insert_args(v);
			if (v->added > 0)
			{
				v->i += v->added - 1;
			}
		}
	}
	v->i++;
}

void	expand_args_loop(t_expand_vars *v)
{
	char	*orig_cmd;

	orig_cmd = NULL;
	if (!v->cmd || !v->cmd->args || v->cmd->args_count <= 0)
		return ;
	if (v->cmd->args[0])
		orig_cmd = ft_strdup(v->cmd->args[0]);
	v->i = 0;
	while (v->i < v->cmd->args_count)
	{
		if (!v->cmd->args[v->i])
		{
			v->i++;
			continue ;
		}
		process_argument(v);
	}
	restore_command_name(v, orig_cmd);
}

void	expand_redirections_loop(t_expand_vars *v)
{
	t_redirections	*redir;
	char			*expanded;

	redir = v->cmd->redirections;
	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC)
		{
			expanded = expand_variables(redir->file, v->envp);
			if (expanded)
			{
				free(redir->file);
				redir->file = expanded;
			}
		}
		redir = redir->next;
	}
}
