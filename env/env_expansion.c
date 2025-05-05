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
		free(cmd->args[i]);
		cmd->args[i] = expanded;
	}
	stripped = strip_quotes(cmd->args[i]);
	if (stripped)
	{
		free(cmd->args[i]);
		cmd->args[i] = stripped;
	}
}

void	expand_command_args(t_command *cmd, char **envp)
{
	t_expand_vars	v;

	v.cmd = cmd;
	v.envp = envp;
	v.i = 0;
	v.j = 0;
	v.added = 0;
	if (cmd && cmd->args && cmd->args_count > 0)
	{
		v.is_export = (cmd->args[0] && ft_strcmp(cmd->args[0], "export") == 0);
		expand_args_loop(&v);
	}
	if (cmd)
	{
		expand_redirections_loop(&v);
		clean_empty_args(cmd); 
	}
}
