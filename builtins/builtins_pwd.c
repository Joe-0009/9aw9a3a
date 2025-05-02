#include "../minishell.h"

// static int	print_pwd_from_env(t_env **env)
// {
// 	t_env	*pwd_node;

// 	pwd_node = find_env_node(*env, "PWD");
// 	if (pwd_node && pwd_node->value)
// 	{
// 		ft_fprintf_fd(1, "%s\n", pwd_node->value);
// 		return (0);
// 	}
// 	return (ft_fprintf_fd(2,
// 			"minishell: pwd: error retrieving current directory\n"), 1);
// }

int	builtin_pwd(t_env **env)
{
	char	current_dir[PATH_MAX];
	char	*pwd_env;

	if (getcwd(current_dir, PATH_MAX) != NULL)
	{
		ft_putstr_fd(current_dir, 1);
		ft_putchar_fd('\n', 1);
		return (0);
	}
	pwd_env = get_env_value("PWD", env_list_to_envp(*env));
	if (pwd_env)
	{
		ft_putstr_fd(pwd_env, 1);
		ft_putchar_fd('\n', 1);
		free(pwd_env);
		return (0);
	}
	ft_putstr_fd("minishell: pwd: error retrieving current directory\n", 2);
	return (1);
}