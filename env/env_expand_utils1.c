#include "../minishell.h"

int	is_var_char(char c)
{
	return (ft_isalnum(c) || c == '_' || c == '?');
}

char	*extract_var_name(const char *str, int *pos)
{
	int	start;

	start = *pos;
	while (str[*pos] && is_var_char(str[*pos]))
		(*pos)++;
	return (ft_strndup(str + start, *pos - start));
}

char	*get_env_value(char *var_name, char **envp)
{
	int		i;
	size_t	var_len;

	i = 0;
	var_len = ft_strlen(var_name);
	if (ft_strcmp(var_name, "?") == 0)
		return (ft_itoa(g_last_exit_status));
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var_name, var_len) == 0
			&& envp[i][var_len] == '=')
			return (ft_strdup(envp[i] + var_len + 1));
		i++;
	}
	return (NULL);
}

int	add_char_to_result(char **result, char c)
{
	char	*temp;
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	temp = *result;
	*result = ft_strjoin(*result, buf);
	if (temp)
		free(temp);
	return (*result != NULL);
}

void	update_quote_state(char c, t_state *state)
{
	if (c == '"' && *state != STATE_IN_SINGLE_QUOTE)
	{
		if (*state == STATE_NORMAL)
			*state = STATE_IN_DOUBLE_QUOTE;
		else
			*state = STATE_NORMAL;
	}
	else if (c == '\'' && *state != STATE_IN_DOUBLE_QUOTE)
	{
		if (*state == STATE_NORMAL)
			*state = STATE_IN_SINGLE_QUOTE;
		else
			*state = STATE_NORMAL;
	}
}

// Returns 1 if a $ is found inside single quotes, 0 otherwise
int is_var_in_squotes(const char *str)
{
    int i = 0;
    t_state state = STATE_NORMAL;
    while (str[i]) {
        update_quote_state(str[i], &state);
        if (str[i] == '$' && state == STATE_IN_SINGLE_QUOTE)
            return 1;
        i++;
    }
    return 0;
}

int was_quoted(const char *str)
{
    int i = 0;
    t_state state = STATE_NORMAL;
    int was_in_quotes = 0;

    while (str[i])
    {
        if ((str[i] == '"' || str[i] == '\'') && state == STATE_NORMAL)
            was_in_quotes = 1;
        update_quote_state(str[i], &state);
        i++;
    }
    return (was_in_quotes);
}
