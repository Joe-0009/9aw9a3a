#include "../minishell.h"

int	count_split_words(char **split_words)
{
	int	count;

	count = 0;
	while (split_words[count])
		count++;
	return (count);
}

void	copy_and_replace_args(t_command *cmd, char **new_args,
		int pos, char **split_words)
{
	int	i;
	int	j;

	i = -1;
	while (++i < pos)
		new_args[i] = cmd->args[i];
	j = 0;
	while (split_words[j])
		new_args[i++] = ft_strdup(split_words[j++]);
	j = pos + 1;
	while (j < cmd->args_count)
		new_args[i++] = cmd->args[j++];
	free(cmd->args[pos]);
	free(cmd->args);
	cmd->args = new_args;
	cmd->args_count = i;
}

int	add_split_args_to_command(t_command *cmd, int pos,
		char **split_words)
{
	char	**new_args;
	int		word_count;

	word_count = count_split_words(split_words);
	if (word_count <= 1)
	{
		ft_free_strs(split_words);
		return (0);
	}
	new_args = ft_calloc(cmd->args_count + word_count, sizeof(char *));
	if (!new_args)
	{
		ft_free_strs(split_words);
		return (-1);
	}
	copy_and_replace_args(cmd, new_args, pos, split_words);
	ft_free_strs(split_words);
	return (word_count - 1);
}

int	split_and_insert_args(t_command *cmd, int i, int is_export)
{
	char	**split_words;
	int		added;

	if (i > 0 && !is_export)
	{
		split_words = ft_split(cmd->args[i], ' ');
		if (split_words && split_words[0])
		{
			added = add_split_args_to_command(cmd, i, split_words);
			if (added > 0)
				return (added);
		}
	}
	return (0);
}

void	compact_args(t_command *cmd, int *i, int *j)
{
	if (cmd->args[*i] && cmd->args[*i][0] != '\0')
	{
		if (*i != *j)
		{
			cmd->args[*j] = cmd->args[*i];
			cmd->args[*i] = NULL;
		}
		(*j)++;
	}
	else if (cmd->args[*i])
	{
		free(cmd->args[*i]);
		cmd->args[*i] = NULL;
	}
}