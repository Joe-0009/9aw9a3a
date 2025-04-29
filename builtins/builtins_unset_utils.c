#include "../minishell.h"

static void	free_env_node(t_env *node)
{
	free(node->key);
	if (node->value)
		free(node->value);
	free(node);
}

void	unset_one_arg(char *arg, t_env **env_list)
{
	t_env	*prev;
	t_env	*cur;

	if (!is_valid_identifier(arg))
	{
		ft_fprintf_fd(2, "minishell: unset: `%s': not a valid identifier\n", arg);
		return ;
	}
	prev = NULL;
	cur = *env_list;
	while (cur)
	{
		if (ft_strcmp(cur->key, arg) == 0)
		{
			if (prev)
				prev->next = cur->next;
			else
				*env_list = cur->next;
			free_env_node(cur);
			break ;
		}
		prev = cur;
		cur = cur->next;
	}
}
