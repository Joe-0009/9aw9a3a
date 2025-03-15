#include "minishell.h"


static int	is_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}


static char	*extract_var_name(const char *str, int *pos)
{
	int	start;

	start = *pos;
	while (str[*pos] && is_var_char(str[*pos]))
		(*pos)++;
	return (ft_strndup(str + start, *pos - start));
}


static char	*get_env_value(char *var_name, char **envp)
{
	int		i;
	size_t	var_len;

	i = 0;
	var_len = ft_strlen(var_name);
	if (ft_strcmp(var_name, "?") == 0)
		return (ft_strdup("0"));
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var_name, var_len) == 0 && 
			envp[i][var_len] == '=')
			return (ft_strdup(envp[i] + var_len + 1));
		i++;
	}
	return (ft_strdup(""));
}


static int	add_char_to_result(char **result, char c)
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


static void	update_quote_state(char c, t_state *state)
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
	free(var_name);
	if (!var_value)
		return (0);
	temp = *result;
	*result = ft_strjoin(*result, var_value);
	if (temp)
		free(temp);
	free(var_value);
	return (*result != NULL);
}

char	*expand_variables(char *str, char **envp)
{
	int		i;
	t_state	state;
	char	*result;

	i = 0;
	state = STATE_NORMAL;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (str[i])
	{
		update_quote_state(str[i], &state);
		if (str[i] == '$' && state != STATE_IN_SINGLE_QUOTE && str[i + 1])
		{
			if (!process_variable(&result, str, &i, envp))
			{
				free(result);
				return (NULL);
			}
		}
		else
		{
			if (!add_char_to_result(&result, str[i++]))
			{
				free(result);
				return (NULL);
			}
		}
	}
	return (result);
}

/**
 * Apply variable expansion to command args
 */
void	expand_command_args(t_command *cmd, char **envp)
{
	int				i;
	char			*expanded;
	t_redirections	*redir;

	i = 0;
	while (i < cmd->args_count)
	{
		expanded = expand_variables(cmd->args[i], envp);
		if (expanded)
		{
			free(cmd->args[i]);
			cmd->args[i] = expanded;
		}
		i++;
	}
	redir = cmd->redirections;
	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC)
		{
			expanded = expand_variables(redir->file, envp);
			if (expanded)
			{
				free(redir->file);
				redir->file = expanded;
			}
		}
		redir = redir->next;
	}
} 
