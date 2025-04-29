#include "../minishell.h"

static int	is_env_var_format(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	if (i > 0 && str[i] == '=')
		return (1);
	return (0);
}

static char	**create_temp_env(t_env *env_list, t_command *cmd)
{
	char	**env_array;
	char	**temp_env;
	int		i;
	int		j;
	int		extra_vars;

	extra_vars = 0;
	i = 1;
	while (i < cmd->args_count && is_env_var_format(cmd->args[i++]))
		extra_vars++;
	env_array = env_list_to_envp(env_list);
	if (!env_array)
		return (NULL);
	i = 0;
	while (env_array[i])
		i++;
	temp_env = ft_calloc(i + extra_vars + 1, sizeof(char *));
	if (!temp_env)
	{
		safe_doube_star_free(env_array);
		return (NULL);
	}
	i = 0;
	while (env_array[i])
	{
		temp_env[i] = ft_strdup(env_array[i]);
		i++;
	}
	j = 1;
	while (j < cmd->args_count && is_env_var_format(cmd->args[j]))
		temp_env[i++] = ft_strdup(cmd->args[j++]);
	temp_env[i] = NULL;
	safe_doube_star_free(env_array);
	return (temp_env);
}

int	builtin_env(t_command *cmd, t_env *env_list)
{
	t_env	*current;
	char	**temp_env;
	int		i;
	int		first_cmd_arg;

	first_cmd_arg = 1;
	while (first_cmd_arg < cmd->args_count
		&& is_env_var_format(cmd->args[first_cmd_arg]))
		first_cmd_arg++;
	if (first_cmd_arg < cmd->args_count)
	{
		temp_env = create_temp_env(env_list, cmd);
		if (!temp_env)
			return (1);
		if (access(cmd->args[first_cmd_arg], F_OK) == -1)
		{
			ft_putstr_fd("env: ", 2);
			ft_putstr_fd(cmd->args[first_cmd_arg], 2);
			ft_putstr_fd(": No such file or directory\n", 2);
			safe_doube_star_free(temp_env);
			return (127);
		}
		safe_doube_star_free(temp_env);
		return (0);
	}
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
	i = 1;
	while (i < cmd->args_count && is_env_var_format(cmd->args[i]))
	{
		ft_putstr_fd(cmd->args[i], 1);
		ft_putchar_fd('\n', 1);
		i++;
	}
	return (0);
}
