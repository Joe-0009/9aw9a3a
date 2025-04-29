#include "../minishell.h"

static int	handle_cd_errors(char *dir, char *old_pwd)
{
	if (old_pwd)
		free(old_pwd);
	if (!dir)
		return (ft_putstr_fd("minishell: cd: HOME not set\n", 2), 1);
	else
		return (ft_fprintf_fd(2, "minishell: cd: %s: %s\n", dir,
				strerror(errno)), 1);
}

static int	update_pwd_vars(t_env **env, char *old_pwd)
{
	char	*new_pwd;

	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
	{
		ft_fprintf_fd(2, "minishell: cd: error retrieving current directory: "
			"getcwd: cannot access parent directories: "
			"No such file or directory\n");
		return (1);
	}
	if (new_pwd)
	{
		add_or_update_env(env, "PWD", new_pwd);
		if (old_pwd)
			add_or_update_env(env, "OLDPWD", old_pwd);
		free(new_pwd);
	}
	if (old_pwd)
		free(old_pwd);
	return (0);
}

int	builtin_cd(t_command *cmd, t_env **env)
{
	char	*old_pwd;
	char	*dir;
	int		ret;

	old_pwd = getcwd(NULL, 0);
	if (cmd->args_count > 2)
	{
		if (old_pwd)
			free(old_pwd);
		return (ft_putstr_fd("minishell: cd: too many arguments\n", 2), 1);
	}
	if (cmd->args_count == 1)
		dir = find_env_node(*env, "HOME")->value;
	else
		dir = cmd->args[1];
	if (cmd->args_count == 1 && !dir)
		return (handle_cd_errors(NULL, old_pwd));
	ret = chdir(dir);
	if (ret != 0)
		return (handle_cd_errors(dir, old_pwd));
	update_pwd_vars(env, old_pwd);
	return (0);
}

static int	print_pwd_from_env(t_env **env)
{
	t_env	*pwd_node;

	pwd_node = find_env_node(*env, "PWD");
	if (pwd_node && pwd_node->value)
	{
		ft_fprintf_fd(1, "%s\n", pwd_node->value);
		return (0);
	}
	return (ft_fprintf_fd(2,
			"minishell: pwd: error retrieving current directory\n"), 1);
}

int	builtin_pwd(t_env **env)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		ft_fprintf_fd(1, "%s\n", cwd);
		free(cwd);
		return (0);
	}
	return (print_pwd_from_env(env));
}
