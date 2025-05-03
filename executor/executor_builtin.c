#include "../minishell.h"

int	is_builtin_command(char *cmd)
{
	char	*builtins[8];
	int		i;

	builtins[0] = "cd";
	builtins[1] = "echo";
	builtins[2] = "pwd";
	builtins[3] = "export";
	builtins[4] = "unset";
	builtins[5] = "env";
	builtins[6] = "exit";
	builtins[7] = NULL;
	i = -1;
	while (builtins[++i])
	{
		if (ft_strcmp(builtins[i], cmd) == 0)
			return (1);
	}
	return (0);
}

int	execute_builtin(t_command *cmd, t_env **env_list)
{
	char	*command;

	command = cmd->args[0];
	if (ft_strcmp(command, "cd") == 0)
		return (builtin_cd(cmd, env_list));
	else if (ft_strcmp(command, "echo") == 0)
		return (builtin_echo(cmd));
	else if (ft_strcmp(command, "pwd") == 0)
		return (builtin_pwd(env_list));
	else if (ft_strcmp(command, "export") == 0)
		return (builtin_export(cmd, env_list));
	else if (ft_strcmp(command, "unset") == 0)
		return (builtin_unset(cmd, env_list));
	else if (ft_strcmp(command, "env") == 0)
		return (builtin_env(cmd, *env_list));
	else if (ft_strcmp(command, "exit") == 0)
		return (builtin_exit(cmd, env_list));
	return (1);
}

int	is_parent_builtin(char *cmd)
{
	return (ft_strcmp(cmd, "cd") == 0 || ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "exit") == 0);
}

int	execute_single_parent_builtin(t_command *cmd_list, t_env **env_list)
{
	int		status;
	char	**envp;

	envp = env_list_to_envp(*env_list);
	if (!envp)
		return (1);
	expand_command_args(cmd_list, envp);
	safe_doube_star_free(envp);
	status = execute_builtin(cmd_list, env_list);
	setup_signals();
	return (status);
}
