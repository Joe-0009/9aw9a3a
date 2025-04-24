#include "../minishell.h"

int	builtin_cd(t_command *cmd, t_env **env)
{
	char	*dir;
	char	*home;
	char	*oldpwd;
	char	*newpwd;
	int		ret;
	t_env	*home_node;

	if (cmd->args_count > 2)
	{
		ft_fprintf_fd(2, "minishell: cd: too many arguments\n");
		return (1);
	}
	oldpwd = getcwd(NULL, 0);
	if (cmd->args_count == 1)
	{
		home_node = find_env_node(*env, "HOME");
		if (home_node)
			home = home_node->value;
		else
			home = NULL;
		if (!home)
		{
			free(oldpwd);
			ft_fprintf_fd(2, "minishell: cd: HOME not set\n");
			return (1);
		}
		dir = home;
	}
	else
		dir = cmd->args[1];
	ret = chdir(dir);
	if (ret != 0)
	{
		free(oldpwd);
		ft_fprintf_fd(2, "minishell: cd: %s: %s\n", dir, strerror(errno));
		return (1);
	}
	if (oldpwd)
		add_or_update_env(env, "OLDPWD", oldpwd);
	newpwd = getcwd(NULL, 0);
	if (newpwd)
	{
		add_or_update_env(env, "PWD", newpwd);
		free(newpwd);
	}
	free(oldpwd);
	return (0);
}

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		ft_fprintf_fd(1, "%s\n", cwd);
		free(cwd);
		return (0);
	}
	else
	{
		ft_fprintf_fd(2, "minishell: pwd: error retrieving current directory: %s\n", strerror(errno));
		return (1);
	}
}
