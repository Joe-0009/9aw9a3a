#include "../minishell.h"

static int	handle_cd_errors(char *dir, char *old_pwd)
{
	if (old_pwd)
		free(old_pwd);
	if (!dir)
		return (ft_fprintf_fd(2, "minishell: cd: HOME not set\n"), 1);
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
		ft_fprintf_fd(2,
			"minishell: cd: error retrieving current directory: "
			"getcwd: cannot access parent directories: "
			"No such file or directory\n");
		if (old_pwd)
			free(old_pwd);
		return (1);
	}
	add_or_update_env(env, "PWD", new_pwd);
	if (old_pwd)
		add_or_update_env(env, "OLDPWD", old_pwd);
	free(new_pwd);
	free(old_pwd);
	return (0);
}

static char	*get_target_dir(t_command *cmd, t_env **env, char *old_pwd)
{
	t_env	*home_node;
	char	*dir;

	if (cmd->args_count == 1)
	{
		home_node = find_env_node(*env, "HOME");
		if (home_node)
			dir = home_node->value;
		else
			dir = NULL;
		if (!dir)
		{
			handle_cd_errors(NULL, old_pwd);
			return (NULL);
		}
	}
	else
		dir = cmd->args[1];
	return (dir);
}

int	builtin_cd(t_command *cmd, t_env **env)
{
	char	*old_pwd;
	char	*dir;
	int		ret;

	old_pwd = getcwd(NULL, 0);
	if (cmd->args_count > 2)
	{
		free(old_pwd);
		return (ft_fprintf_fd(2, "minishell: cd: too many arguments\n"), 1);
	}
	dir = get_target_dir(cmd, env, old_pwd);
	if (!dir)
		return (1);
	ret = chdir(dir);
	if (ret != 0)
		return (handle_cd_errors(dir, old_pwd));
	return (update_pwd_vars(env, old_pwd));
}
