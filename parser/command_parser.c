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
		ft_putstr_fd("minishell: syntax error near unexpected token `|", 2);
		return (0);
	}
	return (1);
}

static int	handle_pipe_with_redirection(t_token *current,
	t_command **current_cmd)
{
	t_command	*new_cmd;

	if (current->next && (current->next->type == TOKEN_REDIRECT_IN
		|| current->next->type == TOKEN_REDIRECT_OUT
		|| current->next->type == TOKEN_APPEND
		|| current->next->type == TOKEN_HEREDOC))
	{
		new_cmd = command_init();
		if (!new_cmd)
			return (0);
		(*current_cmd)->next = new_cmd;
		*current_cmd = new_cmd;
	}
	return (1);
}

static t_command	*cleanup_and_return_null(t_command *first_cmd)
{
	if (first_cmd)
		free_command_list(first_cmd);
	return (NULL);
}

static t_command	*finish_command_parsing(t_command *first_cmd)
{
	t_command	*current;

	current = first_cmd;
	while (current)
	{
		if (!current->args)
		{
			current->args = malloc(sizeof(char *));
			if (!current->args)
				return (free_command_list(first_cmd), NULL);
			current->args[0] = NULL;
			current->args_count = 0;
		}
		current = current->next;
	}
	return (first_cmd);
}

t_command	*create_cmds(t_token **tokens)
{
	t_command	*first_cmd;
	t_command	*current_cmd;
	t_token		*current;

	first_cmd = NULL;
	current_cmd = NULL;
	current = *tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD)
		{
			if (!handle_word_token(&current, &first_cmd, &current_cmd))
				return (cleanup_and_return_null(first_cmd));
		}
		else if (current->type == TOKEN_PIPE)
		{
			if (!handle_pipe_token(current_cmd) || 
				!handle_pipe_with_redirection(current, &current_cmd))
				return (cleanup_and_return_null(first_cmd));
			current = current->next;
		}
		else if (current->type == TOKEN_REDIRECT_IN || 
			current->type == TOKEN_REDIRECT_OUT ||
			current->type == TOKEN_APPEND || 
			current->type == TOKEN_HEREDOC)
		{
			if (!handle_redirect_token(&current, &first_cmd, &current_cmd))
				return (cleanup_and_return_null(first_cmd));
		}
		else
			current = current->next;
	}
	return (finish_command_parsing(first_cmd));
}
