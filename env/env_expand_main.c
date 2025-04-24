#include "../minishell.h"

void	expand_command_args(t_command *cmd, char **envp)
{
	if (cmd && cmd->args)
		expand_args_loop(cmd, envp);
	if (cmd)
		expand_redirections_loop(cmd, envp);
	safe_doube_star_free(envp);
}
