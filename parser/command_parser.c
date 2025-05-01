#include "../minishell.h"

static t_command	*handle_word_token(t_token **current, t_command **first_cmd,
		t_command **current_cmd)
{
	t_command	*new_cmd;

	new_cmd = create_command_type_word(current);
	if (!new_cmd)
		return (NULL);
	if (!*first_cmd)
		*first_cmd = new_cmd;
	else if (*current_cmd)
		(*current_cmd)->next = new_cmd;
	*current_cmd = new_cmd;
	return (new_cmd);
}

static int	handle_pipe_token(t_command *current_cmd)
{
	if (!current_cmd)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|\n", 2);
		return (0);
	}

	return (1);
}

t_command	*create_cmds(t_token **tokens)
{
	t_command	*first_cmd;
	t_command	*current_cmd;
	t_token		*current;
	t_command *new_cmd;

	first_cmd = NULL;
	current_cmd = NULL;
	current = *tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD)
		{
			if (!handle_word_token(&current, &first_cmd, &current_cmd))
			{
				if (first_cmd)
					free_command_list(first_cmd);
				return (NULL);
			}
		}
		else if (current->type == TOKEN_PIPE)
		{
			if (!handle_pipe_token(current_cmd))
			{
				if (first_cmd)
					free_command_list(first_cmd);
				return (NULL);
			}
			// Check if next token is a redirection and create a new command for it
			if (current->next && (current->next->type == TOKEN_REDIRECT_IN 
				|| current->next->type == TOKEN_REDIRECT_OUT
				|| current->next->type == TOKEN_APPEND 
				|| current->next->type == TOKEN_HEREDOC))
			{
				new_cmd = command_init();
				if (!new_cmd)
				{
					free_command_list(first_cmd);
					return (NULL);
				}
				current_cmd->next = new_cmd;
				current_cmd = new_cmd;
			}
			current = current->next;
		}
		else if (current->type == TOKEN_REDIRECT_IN
			|| current->type == TOKEN_REDIRECT_OUT
			|| current->type == TOKEN_APPEND || current->type == TOKEN_HEREDOC)
		{
			if (!handle_redirect_token(&current, &first_cmd, &current_cmd))
			{
				if (first_cmd)
					free_command_list(first_cmd);
				return (NULL);
			}
		}
		else
			current = current->next;
	}
	return (first_cmd);
}
