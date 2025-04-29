#include "../minishell.h"

int	builtin_export(t_command *cmd, t_env **env_list)
{
	int	i;

	if (!cmd->args[1])
	{
		print_export_list(*env_list);
		return (0);
	}
	i = 1;
	while (cmd->args[i])
	{
		export_one_arg(cmd->args[i], env_list);
		i++;
	}
	return (0);
}

void	print_export_list(t_env *env_list)
{
	t_env	*cur;

	cur = env_list;
	while (cur)
	{
		if (cur->key)
		{
			if (cur->value)
			{
				ft_putstr_fd("declare -x ", 1);
				ft_putstr_fd(cur->key, 1);
				ft_putstr_fd("=\"", 1);
				ft_putstr_fd(cur->value, 1);
				ft_putstr_fd("\"\n", 1);
			}
			else
			{
				ft_putstr_fd("declare -x ", 1);
				ft_putstr_fd(cur->key, 1);
				ft_putchar_fd('\n', 1);
			}
		}
		cur = cur->next;
	}
}
