/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: isallali <isallali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 17:49:31 by isallali          #+#    #+#             */
/*   Updated: 2025/05/07 17:49:32 by isallali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	token_utils_add_token(t_token **tokens, char *content)
{
	t_token	*new_token;

	new_token = ft_token_new(content);
	if (!new_token)
	{
		safe_free((void **)&content);
		return (0);
	}
	ft_token_add_back(tokens, new_token);
	return (1);
}

char	*extract_word_token(char *str, int start, int end)
{
	char	*token;

	token = ft_strndup(&str[start], (end - start));
	if (!token)
		ft_putstr_fd("minishell: memory allocation error in tokenization\n", 2);
	return (token);
}

void	token_utils_skip_whitespace(char *input, int *i, int *start)
{
	while (input[*i] && ft_isspace(input[*i]))
		(*i)++;
	*start = *i;
}

void	token_utils_handle_quotes(t_tokenizer *t)
{
	if (t->input[t->i] == '"' && t->state != STATE_IN_SINGLE_QUOTE)
	{
		if (t->state == STATE_NORMAL)
			t->state = STATE_IN_DOUBLE_QUOTE;
		else
			t->state = STATE_NORMAL;
	}
	else if (t->input[t->i] == '\'' && t->state != STATE_IN_DOUBLE_QUOTE)
	{
		if (t->state == STATE_NORMAL)
			t->state = STATE_IN_SINGLE_QUOTE;
		else
			t->state = STATE_NORMAL;
	}
	(t->i)++;
}

t_token	*token_utils_clean_tokens_return_null(t_token **tokens)
{
	ft_token_clear(tokens, free);
	return (NULL);
}
