#include "../minishell.h"

t_command	*cleanup_and_return_null(t_command *first_cmd)
{
	if (first_cmd)
		free_command_list(first_cmd);
	return (NULL);
}

t_command	*finish_command_parsing(t_command *first_cmd)
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
