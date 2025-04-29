#include "../minishell.h"

static int	process_exit_arg(char *arg)
{
	int	exit_status;

	if (!is_number(arg))
	{
		ft_fprintf_fd(2, "minishell: exit: %s: numeric argument required\n", arg);
		return (2);
	}
	exit_status = ft_atoi(arg);
	return (exit_status);
}

int	builtin_exit(t_command *cmd, t_env **env_list)
{
	int	exit_status;

	ft_putstr_fd("exit\n", 1);
	if (cmd->args_count > 1)
	{
		exit_status = process_exit_arg(cmd->args[1]);
		if (exit_status == 2)
			return (exit_status);
	}
	else
		exit_status = 0;
	if (cmd->args_count > 2)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		return (1);
	}
	free_command_list(cmd);
	free_env_list(env_list);
	clear_history();
	exit(exit_status);
}
