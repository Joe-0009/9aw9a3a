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

t_env	*add_or_update_env(t_env **env_list, const char *key, const char *value)
{
	t_env	*node;

	node = find_env_node(*env_list, key);
	if (node)
	{
		if (node->value)
			free(node->value);
		if (value != NULL)
			node->value = ft_strdup(value);
		else
			node->value = NULL;
		return (node);
	}
	node = (t_env *)malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	if (value != NULL)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
	node->next = *env_list;
	*env_list = node;
	return (node);
}

t_env	*envp_to_env_list(char **envp)
{
	t_env	*env_list;
	char	*equal_sign;
	char	*key;
	char	*value;
	int		i;

	env_list = NULL;
	i = 0;
	while (envp[i])
	{
		equal_sign = ft_strchr(envp[i], '=');
		if (equal_sign)
		{
			key = ft_substr(envp[i], 0, equal_sign - envp[i]);
			value = ft_strdup(equal_sign + 1);
			add_or_update_env(&env_list, key, value);
			free(key);
			free(value);
		}
		i++;
	}
	return (env_list);
}

static int	count_env_entries(t_env *env_list)
{
	t_env	*cur;
	int		size;

	size = 0;
	cur = env_list;
	while (cur)
	{
		if (cur->key)
			size++;
		cur = cur->next;
	}
	return (size);
}

static char	*create_env_string(char *key, char *value)
{
	char	*result;
	char	*temp;

	result = ft_strdup(key);
	if (!result)
		return (NULL);
	if (value)
	{
		temp = result;
		result = ft_strjoin(result, "=");
		free(temp);
		if (!result)
			return (NULL);
		temp = result;
		result = ft_strjoin(result, value);
		free(temp);
	}
	return (result);
}

char	**env_list_to_envp(t_env *env_list)
{
	t_env	*cur;
	char	**envp;
	int		size;
	int		i;

	size = count_env_entries(env_list);
	envp = (char **)ft_calloc(size + 1, sizeof(char *));
	if (!envp)
		return (NULL);
	cur = env_list;
	i = 0;
	while (cur)
	{
		if (cur->key)
		{
			envp[i] = create_env_string(cur->key, cur->value);
			if (!envp[i])
				return (safe_doube_star_free(envp), NULL);
			i++;
		}
		cur = cur->next;
	}
	return (envp);
}
