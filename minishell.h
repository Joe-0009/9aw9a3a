#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>
# include <stdarg.h>

extern int	g_last_exit_status;

/**
 * Parser state for handling quotes
 */
typedef enum e_state
{
	STATE_NORMAL,
	STATE_IN_SINGLE_QUOTE,
	STATE_IN_DOUBLE_QUOTE
}	t_state;

/**
 * Token types for parsing
 */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_IN,
	TOKEN_APPEND,
	TOKEN_HEREDOC
}	t_token_type;

/**
 * Token structure for lexical analysis
 */
typedef struct s_token
{
	char					*content;
	t_token_type			type;
	struct s_token			*next;
}	t_token;

/**
 * Redirection structure for I/O operations
 */
typedef struct s_redirections
{
	t_token_type			type;
	char					*file;
	int						heredoc_fd;
	struct s_redirections	*next;
}	t_redirections;

/**
 * Command structure representing a single command or pipeline element
 */
typedef struct s_command
{
	char					**args;
	int						args_count;
	t_redirections			*redirections;
	struct s_command		*next;
}	t_command;

/**
 * Environment variable linked list structure
 */
typedef struct s_env
{
	char					*key;
	char					*value;
	struct s_env			*next;
}	t_env;

/* ===================== MEMORY MANAGEMENT ===================== */
void						free_command(t_command *cmd);
void						free_command_list(t_command *cmd_list);
void						safe_free(void **ptr);
void						safe_close(int *fd);
void						safe_doube_star_free(char **str);
void						free_env_list(t_env **env_list);

/* ===================== STRING UTILS ===================== */
int							is_number(char *str);
int							ft_isspace(char c);
char						*ft_strndup(const char *s, size_t n);
int							ft_strcmp(const char *s1, const char *s2);
int							ft_fprintf_fd(int fd, const char *format, ...);
char						*strip_quotes(const char *value);
int							is_content_quoted(char *content);

/* ===================== TOKENIZER ===================== */
typedef struct s_tokenizer
{
	t_token					**tokens;     /* List of tokens being built */
	char					*input;       /* Input string being tokenized */
	int						i;            /* Current position in input */
	int						start;        /* Start position of current token */
	t_state					state;        /* Current quote state */
}	t_tokenizer;

typedef struct s_quote_ctx
{
	char					c;            /* Current character being processed */
	size_t					*i;           /* Current position pointer */
	int						*in_quotes;   /* Whether we're in quotes */
	int						*quoted;      /* Whether string has quotes */
	char					*quote_type;  /* Current quote type (' or ") */
}	t_quote_ctx;

t_token						*tokenize_input(char *input);
void						assign_token_types(t_token *tokens);
int							is_operator(char c);
char						*extract_operator_token(char *str, int *pos);
char						*extract_word_token(char *str, int start, int end);
int							process_normal_char(t_tokenizer *t);
int							process_operator(t_tokenizer *t);
int							process_whitespace(t_tokenizer *t);
void						skip_whitespace(t_tokenizer *t);
int							process_end_of_input(t_tokenizer *t);
t_token						*clean_tokens_return_null(t_token **tokens);
void						handle_quotes(t_tokenizer *t);
int							add_token(t_token **tokens, char *content);

/* ===================== TOKEN LIST ===================== */
void						ft_token_add_back(t_token **lst, t_token *new);
void						ft_token_delone(t_token *lst, void (*del)(void *));
void						ft_token_clear(t_token **lst, void (*del)(void *));
t_token						*ft_token_new(char *content);

/* ===================== COMMAND CREATION & EXECUTION ===================== */
t_command					*create_cmds(t_token **tokens);
int							execute_command_list(t_command *cmd_list,
								t_env **env_list);
char						*find_executable_path(char *cmd, char **envp);
int							setup_redirections(t_command *cmd);
int							setup_heredoc(char *delimiter, char **envp);
char						*process_heredoc_delimiter(const char *delimiter);
void						execute_single_command(t_command *current,
								t_env *env_list);
t_command					*create_command_type_word(t_token **tokens);
t_command					*command_init(void);
int							setup_redirect_in(char *file_path);
int							setup_redirect_out(char *file_path,
								int append_mode);
int							handle_redirect_token(t_token **current,
								t_command **first_cmd, t_command **current_cmd);
void						add_redirection(t_command *cmd, t_token_type redirect_type, char *file);

/* ===================== BUILTINS ===================== */
typedef struct s_env_setup
{
	char					**env_array;       /* Array of environment variables */
	char					**temp_env;        /* Temporary environment array */
	int						env_count;         /* Count of environment variables */
	int						extra_vars;        /* Count of extra variables */
	int						i;                 /* Loop counter */
	int						j;                 /* Loop counter */
}	t_env_setup;

int							is_builtin_command(char *cmd);
int							execute_builtin(t_command *cmd, t_env **env_list);
int							builtin_cd(t_command *cmd, t_env **env);
int							builtin_echo(t_command *cmd);
int							builtin_pwd(t_env **env);
int							builtin_export(t_command *cmd, t_env **env_list);
int							builtin_unset(t_command *cmd, t_env **env_list);
int							builtin_env(t_command *cmd, t_env *env_list);
int							builtin_exit(t_command *cmd, t_env **env_list);
void						export_one_arg(char *arg, t_env **env_list);
void						unset_one_arg(char *arg, t_env **env_list);
void						print_environment(t_env *env_list, t_command *cmd);

/* ===================== ENV LINKED LIST HELPERS ===================== */
t_env						*find_env_node(t_env *env_list, const char *key);
void						add_or_update_env(t_env **env_list,
								const char *key, const char *value);
void						print_export_list(t_env *env_list);
t_env						*envp_to_env_list(char **envp);
char						**env_list_to_envp(t_env *env_list);
int							is_valid_identifier(char *str);

/* ===================== SIGNALS ===================== */
void						setup_signals(void);
void						setup_heredoc_signals(void);
void						setup_exec_signals(void);
void						set_sigint_default(void);
void						restore_signals(void);
int							*get_exit_status(void);
void						handle_sigint_heredoc(int sig);

/* ===================== ENV EXPANSION UTILS ===================== */
int							is_var_char(char c);
char						*extract_var_name(const char *str, int *pos);
char						*get_env_value(char *var_name, char **envp);
int							add_char_to_result(char **result, char c);
void						update_quote_state(char c, t_state *state);
char						*expand_variables(char *str, char **envp);
void						expand_command_args(t_command *cmd, char **envp);

/* ===================== ENV EXPANSION ARGS UTILS ===================== */
typedef struct s_expand_vars
{
	t_command				*cmd;          /* Command being processed */
	char					**envp;        /* Environment variables */
	int						i;             /* Current argument index */
	int						j;             /* Output argument index */
	int						added;         /* Number of args added by splitting */
	int						is_export;     /* Whether this is an export command */
	int						was_arg_quoted; /* Whether argument had quotes */
	int						had_quoted_vars; /* Whether argument had quoted variables */
}	t_expand_vars;

int							count_split_words(char **split_words);
void						copy_and_replace_args(t_command *cmd, char **new_args,
								int pos, char **split_words);
int							add_split_args_to_command(t_command *cmd, int pos,
								char **split_words);
void						expand_and_strip_arg(t_command *cmd, char **envp, int i);
int							split_and_insert_args(t_expand_vars *v);
void						compact_args(t_command *cmd, int *i, int *j);
void						expand_args_loop(t_expand_vars *v);
void						expand_redirections_loop(t_expand_vars *v);
int							has_var_in_dquotes(const char *str);
int							is_var_in_squotes(const char *str);
int							was_quoted(const char *str);

/* ===================== EXECUTOR UTILS ===================== */
int							setup_pipe(int pipe_fd[2]);
int							handle_heredoc_redir(t_redirections *redir, char **envp);
void						handle_fork_error(t_command *current,
								int prev_pipe_read, int pipe_fd[2]);

/* ===================== EXECUTOR CHILD ===================== */
void						child_process(t_command *current,
								int prev_pipe_read, int pipe_fd[2],
								t_env *env_list);
int							wait_for_children(void);
int							parent_process(int prev_pipe_read, int pipe_fd[2]);

/* ===================== EXECUTOR EXEC ===================== */
int							setup_command_pipe(t_command *current,
								int *prev_pipe_read, int pipe_fd[2]);
int							is_parent_builtin(char *cmd);
int							execute_single_parent_builtin(t_command *cmd_list, t_env **env_list);
int							print_env_vars(char **envp);
void						handle_builtin_command(t_command *current,
								t_env *env_list);
void						handle_external_command(t_command *current,
								t_env *env_list);

/* ===================== UTILITY FUNCTIONS ===================== */
int							process_command(char *input, t_env **env_list);
void						print_Cmd_list(t_command *cmd);
void						initialize_empty_env(t_env **env_list);
void						update_shlvl(t_env **env_list);

#endif