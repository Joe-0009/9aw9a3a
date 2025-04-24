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
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
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
