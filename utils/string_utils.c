#include "../minishell.h"

/*
 * String manipulation utility functions
 * Handles string comparison, duplication, and manipulation
 */

int	is_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

char	*ft_strndup(const char *s, size_t n)
{
	size_t	len;
	char	*result;

	len = 0;
	while (len < n && s[len])
		len++;
	result = calloc(len + 1, 1);
	if (!result)
		return (NULL);
	for (size_t i = 0; i < len; i++)
		result[i] = s[i];
	return (result);
}

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f'
		|| c == '\r');
}

char	*strip_quotes(const char *value)
{
    size_t	i;
    char	*result;
    int		in_quotes;
    char	quote_type;
    
    if (!value)
        return (NULL);
    
    // Count characters after stripping quotes
    size_t len = 0;
    i = 0;
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
            {
                in_quotes = 0;
            }
            else
                len++;
        }
        else
            len++;
        i++;
    }
    
    // Allocate exactly the right amount of memory
    result = ft_strdup("");
    if (!result)
        return (NULL);
    
    // If no quotes to strip, return a simple duplicate
    if (len == ft_strlen(value))
    {
        free(result);
        return (ft_strdup(value));
    }
    
    // Otherwise, allocate appropriate memory and strip quotes
    free(result);
    result = ft_calloc(len + 1, sizeof(char));
    if (!result)
        return (NULL);
    
    i = 0;
    size_t j = 0;
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
            {
                in_quotes = 0;
            }
            else
                result[j++] = value[i];
        }
        else
            result[j++] = value[i];
        i++;
    }
    
    return (result);
}
