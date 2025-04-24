#include "../minishell.h"

int	process_operator(t_token **tokens, char *input, int *i, int *start)
{
	char	*token_content;

	if (*i > *start)
	{
		token_content = extract_word_token(input, *start, *i);
		if (!token_content || !add_token(tokens, token_content))
			return (0);
	}
	token_content = extract_operator_token(input, i);
	if (!token_content || !add_token(tokens, token_content))
		return (0);
	*start = *i;
	return (1);
}

int	process_whitespace(t_token **tokens, char *input, int *i, int *start)
{
	char	*token_content;

	if (*i > *start)
	{
		token_content = extract_word_token(input, *start, *i);
		if (!token_content || !add_token(tokens, token_content))
			return (0);
	}
	skip_whitespace(input, i, start);
	return (1);
}

int	process_normal_char(t_token **tokens, char *input, int *i, int *start)
{
	if (is_operator(input[*i]))
		return (process_operator(tokens, input, i, start));
	else if (ft_isspace(input[*i]))
		return (process_whitespace(tokens, input, i, start));
	(*i)++;
	return (1);
}

int	process_end_of_input(t_token **tokens, char *input, int i, int start,
		t_state state)
{
	char	*token_content;

	if (i > start)
	{
		token_content = extract_word_token(input, start, i);
		if (!token_content || !add_token(tokens, token_content))
			return (0);
	}
	if (state != STATE_NORMAL)
	{
		fprintf(stderr, "minishell: syntax error: unclosed quote\n");
		return (0);
	}
	return (1);
}

void	handle_quotes(char *input, int *i, t_state *state)
{
	char	quote_char;

	quote_char = input[*i];
	if (*state == STATE_NORMAL)
		*state = (quote_char == '\'') ? STATE_IN_SINGLE_QUOTE : STATE_IN_DOUBLE_QUOTE;
	else if ((*state == STATE_IN_SINGLE_QUOTE && quote_char == '\'')
		|| (*state == STATE_IN_DOUBLE_QUOTE && quote_char == '"'))
		*state = STATE_NORMAL;
	(*i)++;
}

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
