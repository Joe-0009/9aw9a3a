#include "../minishell.h"

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