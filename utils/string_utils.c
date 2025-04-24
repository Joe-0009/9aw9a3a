#include "../minishell.h"

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
	size_t	len;
	size_t	j;
	char	*result;
	int		in_quotes;
	char	quote_type;

	if (!value)
		return (NULL);
	len = 0;
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
				in_quotes = 0;
			else
				len++;
		}
		else
			len++;
		i++;
	}
	result = ft_strdup("");
	if (!result)
		return (NULL);
	if (len == ft_strlen(value))
	{
		free(result);
		return (ft_strdup(value));
	}
	free(result);
	result = ft_calloc(len + 1, sizeof(char));
	if (!result)
		return (NULL);
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
	return (result);
}

int	ft_fprintf_fd(int fd, const char *format, ...)
{
	va_list	args;
	int		i;
	int		count;
	char	*str;
	char	c;
	char	*temp;
	char	*result;

	va_start(args, format);
	i = 0;
	count = 0;
	result = ft_strdup("");
	if (!result)
		return (-1);
	
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1])
		{
			i++;
			if (format[i] == 's')
			{
				str = va_arg(args, char *);
				if (!str)
					str = "(null)";
				temp = result;
				result = ft_strjoin(result, str);
				free(temp);
				count += ft_strlen(str);
			}
			else if (format[i] == 'c')
			{
				c = (char)va_arg(args, int);
				temp = result;
				result = ft_calloc(ft_strlen(result) + 2, sizeof(char));
				if (!result)
				{
					free(temp);
					va_end(args);
					return (-1);
				}
				ft_strlcpy(result, temp, ft_strlen(temp) + 1);
				result[ft_strlen(temp)] = c;
				free(temp);
				count++;
			}
		}
		else
		{
			temp = result;
			result = ft_calloc(ft_strlen(result) + 2, sizeof(char));
			if (!result)
			{
				free(temp);
				va_end(args);
				return (-1);
			}
			ft_strlcpy(result, temp, ft_strlen(temp) + 1);
			result[ft_strlen(temp)] = format[i];
			free(temp);
			count++;
		}
		i++;
	}
	
	ft_putstr_fd(result, fd);
	free(result);
	va_end(args);
	return (count);
}
