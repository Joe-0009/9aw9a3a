#include "../minishell.h"

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

int	is_var_in_squotes(const char *str)
{
	int		i;
	t_state	state;

	i = 0;
	state = STATE_NORMAL;
	while (str[i])
	{
		update_quote_state(str[i], &state);
		if (str[i] == '$' && state == STATE_IN_SINGLE_QUOTE)
			return (1);
		i++;
	}
	return (0);
}

int	was_quoted(const char *str)
{
	int		i;
	t_state	state;
	int		was_in_quotes;

	i = 0;
	state = STATE_NORMAL;
	was_in_quotes = 0;
	while (str[i])
	{
		if ((str[i] == '"' || str[i] == '\'') && state == STATE_NORMAL)
			was_in_quotes = 1;
		update_quote_state(str[i], &state);
		i++;
	}
	return (was_in_quotes);
}

int	has_var_in_dquotes(const char *str)
{
	int		i;
	t_state	state;

	i = 0;
	state = STATE_NORMAL;
	while (str[i])
	{
		update_quote_state(str[i], &state);
		if (str[i] == '$' && state == STATE_IN_DOUBLE_QUOTE)
			return (1);
		i++;
	}
	return (0);
}