#include "../minishell.h"



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
