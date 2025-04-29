#include "minishell.h"

void	print_Cmd_list(t_command *cmd)
{
	int	i;

	i = 0;
	while (cmd)
	{
		i = 0;
		while (i < cmd->args_count)
			printf("%s ", cmd->args[i++]);
		printf("\n");
		cmd = cmd->next;
	}
}

void	free_env_list(t_env *env_list)
{
	t_env	*next;

	while (env_list)
	{
		next = env_list->next;
		safe_free((void **)&env_list->key);
		safe_free((void **)&env_list->value);
		safe_free((void **)&env_list);
		env_list = next;
	}
}

void	update_shlvl(t_env **env_list)
{
	int		shlvl;
	char	*shlvl_str;
	t_env	*node;

	shlvl = 1;
	node = find_env_node(*env_list, "SHLVL");
	if (node && node->value)
		shlvl = ft_atoi(node->value) + 1;
	shlvl_str = ft_itoa(shlvl);
	add_or_update_env(env_list, "SHLVL", shlvl_str);
	safe_free((void **)&shlvl_str);
}

void	initialize_empty_env(t_env **env_list)
{
	char	cwd[PATH_MAX];
	char	*shell_path;

	add_or_update_env(env_list, "SHLVL", "1");
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		add_or_update_env(env_list, "PWD", cwd);
	shell_path = "env";
	add_or_update_env(env_list, "_", shell_path);
}
