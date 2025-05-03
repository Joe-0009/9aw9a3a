#include "../minishell.h"

static int	process_variable(char **result, char *str, int *i, char **envp)
{
	char	*var_name;
	char	*var_value;
	char	*temp;

	(*i)++;
	if (!str[*i] || ft_isspace(str[*i]))
		return (add_char_to_result(result, '$'));
	var_name = extract_var_name(str, i);
	if (!var_name)
		return (0);
	var_value = get_env_value(var_name, envp);
	safe_free((void **)&var_name);
	if (!var_value)
		return (1);
	temp = *result;
	*result = ft_strjoin(*result, var_value);
	if (temp)
		safe_free((void **)&temp);
	safe_free((void **)&var_value);
	return (*result != NULL);
}

static int	handle_expand_dollar(char **result, char *str, int *i, char **envp)
{
	if (str[*i] == '$' && str[*i + 1])
	{
		if (!process_variable(result, str, i, envp))
			return (0);
		return (1);
	}
	return (-1);
}

static int	handle_expand_char(char **result, char *str, int *i)
{
	if (!add_char_to_result(result, str[(*i)++]))
	{
		safe_free((void **)&(*result));
		return (0);
	}
	return (1);
}

char	*expand_variables(char *str, char **envp)
{
	int		i;
	char	*result;
	int		handle_ret;

	i = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (str[i])
	{
		handle_ret = handle_expand_dollar(&result, str, &i, envp);
		if (handle_ret == 0)
			return (NULL);
		else if (handle_ret == 1)
			continue ;
		if (!handle_expand_char(&result, str, &i))
			return (NULL);
	}
	return (result);
}
