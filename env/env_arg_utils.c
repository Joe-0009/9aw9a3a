#include "../minishell.h"

int	count_split_words(char **split_words)
{
	int	count;

	count = 0;
	if (!split_words)
		return (0);
	while (split_words[count])
		count++;
	return (count);
}

void	copy_and_replace_args(t_command *cmd, char **new_args, int pos,
		char **split_words)
{
	int	i;
	int	j;

	i = 0;
	while (i < pos)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	j = 0;
	while (split_words[j])
	{
		new_args[i + j] = ft_strdup(split_words[j]);
		j++;
	}
	safe_free((void **)&cmd->args[pos]);
	safe_free((void **)&cmd->args);
	i = i + j;
	while (cmd->args[pos + 1 + (i - pos - j)])
	{
		new_args[i] = cmd->args[pos + 1 + (i - pos - j)];
		i++;
	}
	new_args[i] = NULL;
}

int	add_split_args_to_command(t_command *cmd, int pos, char **split_words)
{
	int		word_count;
	char	**new_args;
	int		new_size;

	word_count = count_split_words(split_words);
	if (word_count == 0)
		return (0);
	new_size = cmd->args_count + word_count - 1;
	new_args = ft_calloc(new_size + 1, sizeof(char *));
	if (!new_args)
		return (0);
	copy_and_replace_args(cmd, new_args, pos, split_words);
	cmd->args = new_args;
	cmd->args_count = new_size;
	return (word_count);
}

int	split_and_insert_args(t_expand_vars *v)
{
	char	**split_words;
	int		added;

	if (v->i > 0 && !v->is_export)
	{
		split_words = ft_split(v->cmd->args[v->i], ' ');
		if (split_words && split_words[0])
		{
			added = add_split_args_to_command(v->cmd, v->i, split_words);
			if (added > 0)
				return (added);
		}
	}
	return (0);
}

void	compact_args(t_command *cmd, int *i, int *j)
{
	int	k;

	k = *i + 1;
	while (k < cmd->args_count)
	{
		if (cmd->args[k])
		{
			cmd->args[*j] = cmd->args[k];
			*j = *j + 1;
		}
		k++;
	}
	cmd->args[*j] = NULL;
	cmd->args_count = *j;
}
