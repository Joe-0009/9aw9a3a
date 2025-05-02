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

static void	process_quote_check(t_quote_ctx *ctx)
{
	if (!*(ctx->in_quotes))
	{
		*(ctx->in_quotes) = 1;
		*(ctx->quote_type) = ctx->c;
		(*(ctx->i))++;
	}
	else if (*(ctx->in_quotes) && ctx->c == *(ctx->quote_type))
	{
		*(ctx->quoted) = 1;
		*(ctx->in_quotes) = 0;
		(*(ctx->i))++;
	}
	else
		(*(ctx->i))++;
}

int	is_content_quoted(char *content)
{
	size_t	i;
	int		in_quotes;
	char	quote_type;
	int		quoted;
	t_quote_ctx ctx;

	i = 0;
	in_quotes = 0;
	quoted = 0;
	while (content[i])
	{
		if ((content[i] == '"' || content[i] == '\''))
		{
			ctx.c = content[i];
			ctx.i = &i;
			ctx.in_quotes = &in_quotes;
			ctx.quoted = &quoted;
			ctx.quote_type = &quote_type;
			process_quote_check(&ctx);
		}
		else
			i++;
	}
	return (quoted);
}
