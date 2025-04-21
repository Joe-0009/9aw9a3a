#include "../minishell.h"

int	builtin_env(t_env *env_list)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (current->value)
		{
			ft_putstr_fd(current->key, 1);
			ft_putchar_fd('=', 1);
			ft_putstr_fd(current->value, 1);
			ft_putchar_fd('\n', 1);
		}
		current = current->next;
	}
	return (0);
}