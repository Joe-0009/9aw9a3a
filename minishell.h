#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <termios.h>
# include <limits.h>
# include "get_next_line/get_next_line.h"
# include "libft/libft.h"



extern int g_last_exit_status;

/**
 * Parser state for handling quotes
 */
typedef enum e_state {
    STATE_NORMAL,           // Normal parsing state
    STATE_IN_SINGLE_QUOTE,  // Inside single quotes
    STATE_IN_DOUBLE_QUOTE   // Inside double quotes
} t_state;

/**
 * Token types for parsing
 */
typedef enum e_token_type {
    TOKEN_WORD,             
    TOKEN_PIPE,             
    TOKEN_REDIRECT_OUT,     
    TOKEN_REDIRECT_IN,      
    TOKEN_APPEND,           
    TOKEN_HEREDOC           
} t_token_type;

/**
 * Token structure for lexical analysis
 */
typedef struct s_token
{
    char *content;          // Token content
    t_token_type type;      // Token type
    struct s_token *next;   // Next token in list
} t_token;

/**
 * Redirection structure for I/O operations
 */
typedef struct s_redirections
{
    t_token_type type;
    char *file;
    int heredoc_fd;
    struct s_redirections *next; 
} t_redirections;

/**
 * Command structure representing a single command or pipeline element
 */
typedef struct s_command
{
    char **args;
    int args_count;
    t_redirections *redirections;
    struct s_command *next;
} t_command;

/* Memory management functions */
void free_command(t_command *cmd);
void free_command_list(t_command *cmd_list);
void safe_free(void **ptr);
void safe_close(int *fd);

/* String utility functions */
int is_number(char *str);
int ft_isspace(char c);
char *ft_strndup(const char *s, size_t n);
int ft_strcmp(char *s1, char *s2);

/* Tokenizer functions */
t_token		*tokenize_input(char *input);
void        expand_command_args(t_command *cmd, char **envp);
void		assign_token_types(t_token *tokens);
int			is_operator(char c);
char		*extract_operator_token(char *str, int *pos);
char		*extract_word_token(char *str, int start, int end);
int			process_normal_char(t_token **tokens, char *input, int *i, int *start);
int			process_operator(t_token **tokens, char *input, int *i, int *start);
int			process_whitespace(t_token **tokens, char *input, int *i, int *start);
void		skip_whitespace(char *input, int *i, int *start);
int			process_end_of_input(t_token **tokens, char *input, int i, 
				int start, t_state state);
t_token		*clean_tokens_return_null(t_token **tokens);
void		handle_quotes(char *input, int *i, t_state *state);
int			add_token(t_token **tokens, char *content);

/* Token list functions */
void ft_token_add_back(t_token **lst, t_token *new);
void ft_token_delone(t_token *lst, void (*del)(void *));
void ft_token_clear(t_token **lst, void (*del)(void *));
t_token *ft_token_new(char *content);

/* Command creation and execution */
t_command *create_cmds(t_token **tokens);
int execute_command_list(t_command *cmd_list, char **envp);
char *find_executable_path(char *cmd, char **envp);
int setup_redirections(t_command *cmd);
int setup_heredoc(char *delimiter);
void execute_single_command(t_command *current, char **envp);

/* Built-in commands */
int is_builtin_command(char *cmd);
int execute_builtin(t_command *cmd, char **envp);
int builtin_cd(t_command *cmd);
int builtin_echo(t_command *cmd);
int builtin_pwd(void);
int builtin_export(t_command *cmd, char **envp);
int builtin_unset(t_command *cmd);
int builtin_env(char **envp);
int builtin_exit(t_command *cmd);

/* Signal handling */
void setup_signals(void);

#endif