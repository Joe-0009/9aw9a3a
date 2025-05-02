#include "../minishell.h"

t_command	*command_init(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->args_count = 0;
	cmd->redirections = NULL;
	cmd->next = NULL;
	return (cmd);
}

static int	allocate_args_array(t_command *cmd, t_token **tokens)
{
	int		arg_count;
	t_token	*current;

	arg_count = 0;
	current = *tokens;
	while (current && current->type == TOKEN_WORD)
	{
		arg_count++;
		current = current->next;
	}
	cmd->args = ft_calloc(arg_count + 1, sizeof(char *));
	if (!cmd->args)
		return (0);
	cmd->args_count = arg_count;
	return (1);
}

static void	copy_args_to_command(t_command *cmd, t_token **tokens)
{
	int		i;
	t_token	*current;

	i = 0;
	current = *tokens;
	while (i < cmd->args_count && current && current->type == TOKEN_WORD)
	{
		cmd->args[i] = ft_strdup(current->content);
		i++;
		current = current->next;
	}
	cmd->args[i] = NULL;
	*tokens = current;
}

static t_command	*handle_allocation_failure(t_command *cmd)
{
	free_command(cmd);
	return (NULL);
}

t_command	*create_command_type_word(t_token **tokens)
{
	t_command	*cmds;

	if (!*tokens || (*tokens)->type != TOKEN_WORD)
		return (NULL);
	cmds = command_init();
	if (!cmds)
		return (NULL);
	if (!allocate_args_array(cmds, tokens))
		return (handle_allocation_failure(cmds));
	copy_args_to_command(cmds, tokens);
	return (cmds);
}
