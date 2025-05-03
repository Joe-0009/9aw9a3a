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

static void	print_single_export(char *key, char *value)
{
	ft_fprintf_fd(1, "declare -x %s", key);
	if (value)
	{
		ft_fprintf_fd(1, "=\"%s\"", value);
	}
	ft_fprintf_fd(1, "\n");
}

void	print_export_list(t_env *env_list)
{
	t_env	*cur;

	cur = env_list;
	while (cur)
	{
		if (cur->key)
			print_single_export(cur->key, cur->value);
		cur = cur->next;
	}
}
