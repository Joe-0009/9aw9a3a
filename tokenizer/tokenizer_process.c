#include "../minishell.h"

void	skip_whitespace(char *input, int *i, int *start)
{
	while (input[*i] && ft_isspace(input[*i]))
		(*i)++;
	*start = *i;
}

t_token	*clean_tokens_return_null(t_token **tokens)
{
	ft_token_clear(tokens, free);
	return (NULL);
}

int	add_token(t_token **tokens, char *content)
{
	t_token	*new_token;

	new_token = ft_token_new(content);
	if (!new_token)
	{
		free(content);
		return (0);
	}
	ft_token_add_back(tokens, new_token);
	return (1);
}

t_token	*tokenize_input(char *input)
{
	t_token	*tokens;
	t_state	state;
	int		i;
	int		start;

	tokens = NULL;
	state = STATE_NORMAL;
	i = 0;
	start = 0;
	while (input[i])
	{
		if ((input[i] == '"' && state != STATE_IN_SINGLE_QUOTE)
			|| (input[i] == '\'' && state != STATE_IN_DOUBLE_QUOTE))
		{
			handle_quotes(input, &i, &state);
			continue ;
		}
		if (state == STATE_NORMAL && !process_normal_char(&tokens, input, &i,
				&start))
			return (clean_tokens_return_null(&tokens));
		else if (state != STATE_NORMAL)
			i++;
	}
	if (!process_end_of_input(&tokens, input, i, start, state))
		return (clean_tokens_return_null(&tokens));
	return (tokens);
}
