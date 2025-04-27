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

static void	add_words_as_args(t_command *cmd, t_token **current)
{
	int			new_count;
	char		**new_args;
	int			i;
	int			j;
	t_token		*temp;

	new_count = 0;
	temp = *current;
	while (temp && temp->type == TOKEN_WORD)
	{
		new_count++;
		temp = temp->next;
	}
	if (new_count == 0)
		return ;
	new_args = malloc(sizeof(char *) * (cmd->args_count + new_count + 1));
	if (!new_args)
		return ;
	i = 0;
	while (i < cmd->args_count)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	j = 0;
	while (*current && (*current)->type == TOKEN_WORD && j < new_count)
	{
		new_args[i + j] = ft_strdup((*current)->content);
		*current = (*current)->next;
		j++;
	}
	new_args[i + j] = NULL;
	free(cmd->args);
	cmd->args = new_args;
	cmd->args_count += new_count;
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
	if (*current && (*current)->type == TOKEN_WORD)
		add_words_as_args(*current_cmd, current);
	return (1);
}
