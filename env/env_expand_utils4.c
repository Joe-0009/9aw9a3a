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

void expand_args_loop(t_command *cmd, char **envp)
{
    int i = 0, j = 0, added = 0;
    int is_export;
    int had_quoted_vars;
    int was_arg_quoted;
    char *stripped;

    if (!cmd || !cmd->args || cmd->args_count <= 0)
        return;

    is_export = (cmd->args[0] && ft_strcmp(cmd->args[0], "export") == 0);

    while (i < cmd->args_count)
    {
        if (!cmd->args[i])
        {
            i++;
            continue;
        }
        was_arg_quoted = was_quoted(cmd->args[i]);
        had_quoted_vars = has_var_in_dquotes(cmd->args[i]);
        if (!is_var_in_squotes(cmd->args[i]))
        {
            expand_and_strip_arg(cmd, envp, i);
        }
        else
        {
            stripped = strip_quotes(cmd->args[i]);
            if (stripped)
            {
                free(cmd->args[i]);
                cmd->args[i] = stripped;
            }
        }
        if (!was_arg_quoted && !had_quoted_vars)
            added = split_and_insert_args(cmd, i, is_export);
        else
            added = 0;

        if (added > 0)
            continue;

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