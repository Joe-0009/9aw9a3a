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
		return (ft_free_strs(split_words), 0);
	new_args = ft_calloc(cmd->args_count + word_count - 1, sizeof(char *));
	if (!new_args)
		return (ft_free_strs(split_words), -1);
	copy_and_replace_args(cmd, new_args, pos, split_words);
	ft_free_strs(split_words);
	return (word_count - 1);
}

void	expand_and_strip_arg(t_command *cmd, char **envp, int i)
{
	char	*expanded;
	char	*stripped;

	expanded = expand_variables(cmd->args[i], envp);
	if (expanded)
	{
		free(cmd->args[i]);
		cmd->args[i] = expanded;
	}
	stripped = strip_quotes(cmd->args[i]);
	if (stripped)
	{
		free(cmd->args[i]);
		cmd->args[i] = stripped;
	}
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
			{
				return (added);
			}
		}
	}
	return (0);
}