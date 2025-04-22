#include "../minishell.h"

static int	add_split_args_to_command(t_command *cmd, int pos,
		char **split_words)
{
	char	**new_args;

	int(word_count), (i), (j);
	word_count = 0;
	while (split_words[word_count])
		word_count++;
	if (word_count <= 1)
		return (ft_free_strs(split_words), 0);
	new_args = ft_calloc(cmd->args_count + word_count - 1, sizeof(char *));
	if (!new_args)
		return (ft_free_strs(split_words), -1);
	i = -1;
	while (++i < pos)
		new_args[i] = cmd->args[i];
	j = 0;
	while (j < word_count)
		new_args[i++] = ft_strdup(split_words[j++]);
	j = pos + 1;
	while (j < cmd->args_count)
		new_args[i++] = cmd->args[j++];
	free(cmd->args[pos]);
	ft_free_strs(split_words);
	free(cmd->args);
	cmd->args = new_args;
	cmd->args_count = i;
	return (word_count - 1);
}

static void	expand_and_strip_arg(t_command *cmd, char **envp, int i)
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

static int	split_and_insert_args(t_command *cmd, int i, int is_export)
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

static void	compact_args(t_command *cmd, int *i, int *j)
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

static void	expand_args_loop(t_command *cmd, char **envp)
{
	int	i;
	int	j;
	int	added;
	int	is_export;

	i = 0;
	j = 0;
	added = 0;
	is_export = (cmd->args[0] && ft_strcmp(cmd->args[0], "export") == 0);
	while (i < cmd->args_count)
	{
		expand_and_strip_arg(cmd, envp, i);
		added = split_and_insert_args(cmd, i, is_export);
		if (added > 0)
			continue ;
		compact_args(cmd, &i, &j);
		i++;
	}
	cmd->args_count = j;
}

static void	expand_redirections_loop(t_command *cmd, char **envp)
{
	t_redirections	*redir;
	char			*expanded;

	redir = cmd->redirections;
	while (redir)
	{
		if (redir->type != TOKEN_HEREDOC)
		{
			expanded = expand_variables(redir->file, envp);
			if (expanded)
			{
				free(redir->file);
				redir->file = expanded;
			}
		}
		redir = redir->next;
	}
}

void	expand_command_args(t_command *cmd, char **envp)
{
	expand_args_loop(cmd, envp);
	expand_redirections_loop(cmd, envp);
	safe_doube_star_free(envp);
}
