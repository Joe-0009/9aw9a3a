#include "../minishell.h"

static int	append_string(char **result, const char *str)
{
	char	*temp;
	int		len;

	if (!str)
		str = "(null)";
	len = ft_strlen(str);
	temp = *result;
	*result = ft_strjoin(*result, str);
	free(temp);
	return (len);
}

static int	append_char(char **result, char c)
{
	char	*temp;
	size_t	curr_len;

	curr_len = ft_strlen(*result);
	temp = *result;
	*result = ft_calloc(curr_len + 2, sizeof(char));
	if (!*result)
	{
		free(temp);
		return (-1);
	}
	ft_strlcpy(*result, temp, curr_len + 1);
	(*result)[curr_len] = c;
	free(temp);
	return (1);
}

static int	process_format_specifier(char **result, char specifier,
		va_list args)
{
	char	*str;
	char	c;

	if (specifier == 's')
	{
		str = va_arg(args, char *);
		return (append_string(result, str));
	}
	else if (specifier == 'c')
	{
		c = (char)va_arg(args, int);
		return (append_char(result, c));
	}
	if (append_char(result, '%') == -1 || append_char(result, specifier) == -1)
		return (-1);
	return (2);
}

static int	process_format_char(char **result, const char **format, int *i,
		va_list args)
{
	int	ret;

	if ((*format)[*i] == '%' && (*format)[*i + 1])
	{
		(*i)++;
		ret = process_format_specifier(result, (*format)[*i], args);
		if (ret == -1)
			return (-1);
		return (ret);
	}
	else
	{
		if (append_char(result, (*format)[*i]) == -1)
			return (-1);
		return (1);
	}
}

static int	cleanup_fprintf(char *result, va_list args)
{
	free(result);
	va_end(args);
	return (-1);
}

static int	process_format_string(char **result, const char *format,
		va_list args)
{
	int	i;
	int	count;
	int	ret;

	count = 0;
	i = 0;
	while (format[i])
	{
		ret = process_format_char(result, &format, &i, args);
		if (ret == -1)
			return (-1);
		count += ret;
		i++;
	}
	return (count);
}

int	ft_fprintf_fd(int fd, const char *format, ...)
{
	va_list	args;
	int		count;
	char	*result;

	va_start(args, format);
	result = ft_strdup("");
	if (!result)
		return (-1);
	count = process_format_string(&result, format, args);
	if (count == -1)
		return (cleanup_fprintf(result, args));
	ft_putstr_fd(result, fd);
	free(result);
	va_end(args);
	return (count);
}
