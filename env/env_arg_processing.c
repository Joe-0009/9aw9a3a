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
			safe_free((void **)&v->cmd->args[v->i]);
			v->cmd->args[v->i] = stripped;
		}
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
	if (!v->cmd || !v->cmd->args || v->cmd->args_count <= 0)
		return ;
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
}

void	expand_redirections_loop(t_expand_vars *v)
{
	t_redirections	*redir;
	char			*expanded;

	redir = v->cmd->redirections;
	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC && !redir->was_in_squotes)
		{
			expanded = expand_variables(redir->file, v->envp);
			if (expanded)
			{
				safe_free((void **)&redir->file);
				redir->file = expanded;
			}
		}
		redir = redir->next;
	}
}
