#include "minishell.h"

int	builtin_cd(t_command *cmd, t_env **env)
{
	char	*dir;
	char	*home;
	int		ret;

	if (cmd->args_count > 2)
		return (fprintf(stderr, "minishell: cd: too many arguments\n"), 1);
	if (cmd->args_count == 1)
	{
		home = find_env_node(*env, "HOME")->value;
		if (!home)
			return (fprintf(stderr, "minishell: cd: HOME not set\n"), 1);
		dir = home;
	}
	else
		dir = cmd->args[1];
	ret = chdir(dir);
	if (ret != 0)
		return (fprintf(stderr, "minishell: cd: %s: %s\n", dir,
				strerror(errno)), 1);
	return (0);
}

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		printf("%s\n", cwd);
		free(cwd);
		return (0);
	}
	return (1);
}
