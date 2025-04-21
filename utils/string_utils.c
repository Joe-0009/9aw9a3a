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
	size_t	len;

	if (!value)
		return (NULL);
	len = ft_strlen(value);
	if (len >= 2)
	{
		if ((value[0] == '"' && value[len - 1] == '"') ||
			(value[0] == '\'' && value[len - 1] == '\''))
			return (ft_substr(value, 1, len - 2));
	}
	return (ft_strdup(value));
}
