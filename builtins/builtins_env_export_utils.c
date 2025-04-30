#include "../minishell.h"

int	is_valid_identifier(char *str)
{
	int	i;

	if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
		return (0);
	i = 1;
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

t_env	*find_env_node(t_env *env_list, const char *key)
{
	while (env_list)
	{
		if (ft_strcmp(env_list->key, key) == 0)
			return (env_list);
		env_list = env_list->next;
	}
	return (NULL);
}

void add_or_update_env(t_env **env_list, const char *key, const char *value)
{
	t_env	*node;
	t_env	*last;

	if (!key)
		return ;
	node = find_env_node(*env_list, key);
	if (node)
	{
		if (node->value)
			safe_free((void **)&node->value);
		if (value != NULL)
			node->value = ft_strdup(value);
		else
			node->value = NULL;
		return ;
	}
	node = (t_env *)malloc(sizeof(t_env));
	if (!node)
		return ;
	node->key = ft_strdup(key);
	if (!node->key)
	{
		free(node);
		return ;
	}
	if (value != NULL)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
	if (value != NULL && !node->value)
	{
		free(node->key);
		free(node);
		return ;
	}
	node->next = NULL;
	if (*env_list == NULL)
	{
		*env_list = node;
		return ;
	}
	last = *env_list;
	while (last->next)
		last = last->next;
	last->next = node;
}
