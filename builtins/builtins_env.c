#include "../minishell.h"

// int	builtin_env(t_env *env_list)
// {
// 	t_env	*current;

// 	current = env_list;
// 	while (current)
// 	{
// 		if (current->value)
// 		{
// 			ft_putstr_fd(current->key, 1);
// 			ft_putchar_fd('=', 1);
// 			ft_putstr_fd(current->value, 1);
// 			ft_putchar_fd('\n', 1);
// 		}
// 		current = current->next;
// 	}
// 	return (0);
// }

// Updates the env builtin to check its arguments
int	builtin_env(t_command *cmd, t_env *env_list)
{
	t_env	*current;

	// If arguments are provided (other than 'env' itself)
	if (cmd->args_count > 1)
	{
		// Check if the first argument exists as a file or command
		if (access(cmd->args[1], F_OK) == -1)
		{
			ft_putstr_fd("env: ", 2);
			ft_putstr_fd(cmd->args[1], 2);
			ft_putstr_fd(": No such file or directory\n", 2);
			return (127);
		}
	}
	
	// No arguments or file exists, print environment
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