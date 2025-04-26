#include "../minishell.h"

int add_arg(t_command *cmd, char *arg_content)
{
    char **new_args;
    int i;

    new_args = malloc(sizeof(char *) * (cmd->args_count + 2));
    if (!new_args)
        return (0);
        
    for (i = 0; i < cmd->args_count; i++)
        new_args[i] = cmd->args[i];
        
    new_args[i] = ft_strdup(arg_content);
    if (!new_args[i])
    {
        free(new_args);
        return (0);
    }
    
    new_args[i + 1] = NULL;
    if (cmd->args)
        free(cmd->args);
        
    cmd->args = new_args;
    cmd->args_count++;
    return (1);
}

static int handle_word_token(t_token **current, t_command **first_cmd, t_command **current_cmd)
{
    //  we don't have a current command create a new one with create_command_type_word
    if (!*current_cmd)
    {
        *current_cmd = create_command_type_word(current);
        if (!*current_cmd)
            return (0);
        if (!*first_cmd)
            *first_cmd = *current_cmd;
        return (1);
    }
    
    //  we already have a command add this word as an argument instead of creating a new command
    if (!add_arg(*current_cmd, (*current)->content))
        return (0);
    
    *current = (*current)->next;
    return (1);
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

	first_cmd = NULL;
	current_cmd = NULL;
	current = *tokens;
	while (current)
	{
		if (current->type == TOKEN_WORD)
		{
			if (!handle_word_token(&current, &first_cmd, &current_cmd))
				return (NULL);
		}
		else if (current->type == TOKEN_PIPE)
		{
			if (!handle_pipe_token(current_cmd))
				return (NULL);
			current = current->next;
			// reset current_cmd to NULL to create a new command after the "|"
            current_cmd = NULL;
		}
		else if (current->type == TOKEN_REDIRECT_IN
			|| current->type == TOKEN_REDIRECT_OUT
			|| current->type == TOKEN_APPEND || current->type == TOKEN_HEREDOC)
		{
			if (!handle_redirect_token(&current, &first_cmd, &current_cmd))
				return (NULL);
		}
		else
			current = current->next;
	}
	return (first_cmd);
}
