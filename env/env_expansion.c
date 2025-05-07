#include "../minishell.h"

void	expand_and_strip_arg(t_command *cmd, char **envp, int i)
{
	char	*expanded;
	char	*stripped;

	if (!cmd->args[i])
		return ;
	expanded = expand_variables(cmd->args[i], envp);
	safe_free((void **)&cmd->args[i]);
	if (!expanded)
	{
		cmd->args[i] = NULL;
		return ;
	}
	cmd->args[i] = expanded;
	stripped = strip_quotes(cmd->args[i]);
	if (stripped)
	{
		safe_free((void **)&cmd->args[i]);
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
