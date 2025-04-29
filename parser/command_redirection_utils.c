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
