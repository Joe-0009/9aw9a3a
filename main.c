#include "minishell.h"

int			g_last_exit_status = 0;

// static void	display_welcome(void)
// {
// 	printf("\n");
// 	printf(" ╔════════════════════════════════════╗\n");
// 	printf(" ║            MINISHELL               ║\n");
// 	printf(" ╚════════════════════════════════════╝\n");
// 	printf(" Welcome to Minishell! Type 'exit' to quit.\n\n");
// }

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

static int	process_command(char *input, t_env **env_list)
{
	t_token		*tokens;
	t_command	*cmds;

	tokens = NULL;
	cmds = NULL;
	tokens = tokenize_input(input);
	if (!tokens)
		return (0);
	assign_token_types(tokens);
	cmds = create_cmds(&tokens);
	if (cmds)
	{
		ft_token_clear(&tokens, free);
		g_last_exit_status = execute_command_list(cmds, env_list);
		// printf("Command exit status: %d\n", g_last_exit_status);
		free_command_list(cmds);
	}
	else if (tokens)
	{
		// fprintf(stderr, "minishell: syntax error in command\n");
		ft_token_clear(&tokens, free);
	}
	return (0);
}

static void	shell_loop(t_env *env_list)
{
	char	*input;
	int		should_exit;
	int		ret;

	should_exit = 0;
	while (!should_exit)
	{
		input = readline("minishell> ");
		if (input == NULL)
		{
			printf("exit\n");
			break ;
		}
		if (input[0] != '\0')
		{
			add_history(input);
			ret = process_command(input, &env_list);
			if (ret == 1)
				printf("\n");
			should_exit = ret;
		}
		free(input);
	}
	clear_history();
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

static void	update_shlvl(t_env **env_list)
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

static void	initialize_empty_env(t_env **env_list)
{
	char	cwd[PATH_MAX];
	char	*shell_path;

	add_or_update_env(env_list, "SHLVL", "1");
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		add_or_update_env(env_list, "PWD", cwd);
	shell_path = "./minishell";
	add_or_update_env(env_list, "_", shell_path);
}

int	main(int argc, char **argv, char **envp)
{
	t_env *env_list;

	(void)argc;
	(void)argv;
	setup_signals();
	env_list = envp_to_env_list(envp);
	if (!env_list)
		initialize_empty_env(&env_list);
	else
		update_shlvl(&env_list);
	shell_loop(env_list);
	free_env_list(env_list);
	return (0);
}