#include "minishell.h"

int			g_last_exit_status = 0;

static void	shell_loop(t_env **env_list)
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
			ret = process_command(input, env_list);
			if (ret == 1)
				printf("\n");
			should_exit = ret;
		}
		safe_free((void **)&input);
	}
	clear_history();
}

int	main(int argc, char **argv, char **envp)
{
	t_env	*env_list;

	(void)argc;
	(void)argv;
	setup_signals();
	env_list = envp_to_env_list(envp);
	if (!env_list)
		initialize_empty_env(&env_list);
	else
		update_shlvl(&env_list);
	shell_loop(&env_list);
	free_env_list(&env_list);
	return (0);
}
