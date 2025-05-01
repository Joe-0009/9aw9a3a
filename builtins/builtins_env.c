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

static int	process_env_args(t_command *cmd, int *first_cmd_arg)
{
	int	extra_vars;

	*first_cmd_arg = 1;
	extra_vars = 0;
	while (*first_cmd_arg < cmd->args_count
		&& is_env_var_format(cmd->args[*first_cmd_arg]))
	{
		(*first_cmd_arg)++;
		extra_vars++;
	}
	return (extra_vars);
}

static char	**setup_and_copy_env(t_env *env_list, t_command *cmd, 
		int *first_cmd_arg)
{
	char	**env_array;
	char	**temp_env;
	int		env_count;
	int		extra_vars;
	int		i;
	int		j;

	extra_vars = process_env_args(cmd, first_cmd_arg);
	env_array = env_list_to_envp(env_list);
	if (!env_array)
		return (NULL);
	env_count = 0;
	while (env_array[env_count])
		env_count++;
	temp_env = ft_calloc(env_count + extra_vars + 1, sizeof(char *));
	if (!temp_env)
		return (safe_doube_star_free(env_array), NULL);
	i = -1;
	while (env_array[++i])
		temp_env[i] = ft_strdup(env_array[i]);
	j = 1;
	while (j < *first_cmd_arg)
		temp_env[i++] = ft_strdup(cmd->args[j++]);
	safe_doube_star_free(env_array);
	return (temp_env);
}

static int	handle_cmd_exec(t_command *cmd, t_env *env_list, int first_cmd_arg)
{
	char	**temp_env;

	temp_env = setup_and_copy_env(env_list, cmd, &first_cmd_arg);
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

int	builtin_env(t_command *cmd, t_env *env_list)
{
	int	first_cmd_arg;

	first_cmd_arg = 1;
	while (first_cmd_arg < cmd->args_count
		&& is_env_var_format(cmd->args[first_cmd_arg]))
		first_cmd_arg++;
	if (first_cmd_arg < cmd->args_count)
		return (handle_cmd_exec(cmd, env_list, first_cmd_arg));
	print_environment(env_list, cmd);
	return (0);
}
