#include "../minishell.h"

int	process_command(char *input, t_env **env_list)
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
		free_command_list(cmds);
	}
	else if (tokens)
	{
		ft_token_clear(&tokens, free);
	}
	return (0);
}
