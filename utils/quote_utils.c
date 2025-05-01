#include "../minishell.h"

static size_t	calculate_stripped_length(const char *value)
{
	int		in_quotes;
	char	quote_type;

	size_t(i), (len);
	len = 0;
	i = -1;
	in_quotes = 0;
	while (value[++i])
	{
		if ((value[i] == '"' || value[i] == '\''))
		{
			if (!in_quotes)
			{
				in_quotes = 1;
				quote_type = value[i];
			}
			else if (value[i] == quote_type)
				in_quotes = 0;
			else
				len++;
		}
		else
			len++;
	}
	return (len);
}

static void	build_stripped_string(const char *value, char *result)
{
	int		in_quotes;
	char	quote_type;

	size_t(i), (j);
	i = 0;
	j = 0;
	in_quotes = 0;
	while (value[i])
	{
		if ((value[i] == '"' || value[i] == '\''))
		{
			if (!in_quotes)
			{
				in_quotes = 1;
				quote_type = value[i];
			}
			else if (value[i] == quote_type)
				in_quotes = 0;
			else
				result[j++] = value[i];
		}
		else
			result[j++] = value[i];
		i++;
	}
}

char	*strip_quotes(const char *value)
{
	size_t	len;
	char	*result;

	if (!value)
		return (NULL);
	len = calculate_stripped_length(value);
	if (len == ft_strlen(value))
	{
		result = ft_strdup(value);
		return (result);
	}
	result = ft_calloc(len + 1, 1);
	if (!result)
		return (NULL);
	build_stripped_string(value, result);
	return (result);
}

static void	process_quote_check(char c, size_t *i, int *in_quotes, int *quoted,
		char *quote_type)
{
	if (!*in_quotes)
	{
		*in_quotes = 1;
		*quote_type = c;
		(*i)++;
	}
	else if (*in_quotes && c == *quote_type)
	{
		*quoted = 1;
		*in_quotes = 0;
		(*i)++;
	}
	else
		(*i)++;
}

int	is_content_quoted(char *content)
{
	size_t	i;
	int		in_quotes;
	char	quote_type;
	int		quoted;

	i = 0;
	in_quotes = 0;
	quoted = 0;
	while (content[i])
	{
		if ((content[i] == '"' || content[i] == '\''))
			process_quote_check(content[i], &i, &in_quotes, &quoted,
				&quote_type);
		else
			i++;
	}
	return (quoted);
}
