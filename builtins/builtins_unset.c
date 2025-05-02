#include "../minishell.h"

int	builtin_unset(t_command *cmd, t_env **env_list)
{
	int	i;

	i = 1;
	while (i < cmd->args_count)
	{
		unset_one_arg(cmd->args[i], env_list);
		i++;
	}
	return (0);
}