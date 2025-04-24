#include "../minishell.h"

void	add_redirection(t_command *cmd, t_token_type redirect_type, char *file)
{
	t_redirections	*redirection;
	t_redirections	*current;

	redirection = malloc(sizeof(t_redirections));
	if (!redirection)
		return ;
	redirection->type = redirect_type;
	redirection->file = ft_strdup(file);
	redirection->heredoc_fd = -1;
	redirection->next = NULL;
	if (!redirection->file)
		return (free(redirection));
	if (!cmd->redirections)
		cmd->redirections = redirection;
	else
	{
		current = cmd->redirections;
		while (current->next)
			current = current->next;
		current->next = redirection;
	}
}

static int	create_cmd_if_needed(t_command **first_cmd, t_command **current_cmd)
{
	t_command	*new_cmd;

	if (!*current_cmd)
	{
		new_cmd = command_init();
		if (!new_cmd)
			return (0);
		if (!*first_cmd)
			*first_cmd = new_cmd;
		*current_cmd = new_cmd;
	}
	return (1);
}

static int	check_redirect_syntax(t_token **current)
{
	if (!*current || (*current)->type != TOKEN_WORD)
	{
		if (!*current)
			ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", 2);
		else
			ft_fprintf_fd(2, "minishell: syntax error near unexpected token %s\n", 
				(*current)->content);
		return (0);
	}
	return (1);
}

int	handle_redirect_token(t_token **current, t_command **first_cmd,
		t_command **current_cmd)
{
	t_token_type	redirect_type;

	if (!create_cmd_if_needed(first_cmd, current_cmd))
		return (0);
		
	redirect_type = (*current)->type;
	*current = (*current)->next;
	
	if (!check_redirect_syntax(current))
		return (0);
		
	add_redirection(*current_cmd, redirect_type, (*current)->content);
	*current = (*current)->next;
	return (1);
}
