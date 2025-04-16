#include "minishell.h"

char *find_executable_path(char *cmd, char **envp)
{
    char *path_env;
    int i;
    char **paths;
    char *full_path;

    if (!cmd || !*cmd)
        return (NULL);
    if ((cmd[0] == '/') || (cmd[0] == '.' && (cmd[1] == '/' || 
        (cmd[1] == '.' && cmd[2] == '/'))))
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }
    path_env = NULL;
    i = -1;
    while (envp[++i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            path_env = envp[i] + 5;
            break ;
        }
    }
    if (!path_env)
    {
        printf("PATH environment variable not found\n");
        return (NULL);
    }
    
    paths = ft_split(path_env, ':');
    if (!paths)
        return (NULL);
    full_path = NULL;
    i = 0;
    while (paths[i])
    {
        full_path = ft_strjoin(paths[i], "/");
        if (!full_path)
            break;
        char *temp = full_path;
        full_path = ft_strjoin(full_path, cmd);
        free(temp);
        if (!full_path)
            break;
        if (access(full_path, X_OK) == 0)
            break;
        free(full_path);
        full_path = NULL;
        i++;
    }
    i = 0;
    while (paths[i])
        free(paths[i++]);
    free(paths);
    return (full_path);
}

t_command *command_init(void)
{
    t_command *cmd;

    cmd = malloc(sizeof(t_command));
    if (!cmd)
        return (NULL);
    cmd->args = NULL;
    cmd->args_count = 0;
    cmd->redirections = NULL;
    cmd->next = NULL;
    return (cmd);
}

t_command *create_command_type_word(t_token **tokens)
{
    t_command *cmds;
    t_token *current;
    t_token *tmp;
    int args_count;
    int i;

    current = *tokens;
    if (!current || current->type != TOKEN_WORD)
        return (NULL);
    tmp = current;
    cmds = command_init();
    if (!cmds)
        return (NULL);
    args_count = 0;
    while (tmp && tmp->type == TOKEN_WORD)
    {
        args_count++;
        tmp = tmp->next;
    }
    cmds->args = malloc(sizeof(char*) * (args_count + 1));
    if (!cmds->args)
        return (free(cmds), NULL);
    i = 0;
    while (current && current->type == TOKEN_WORD && i < args_count)
    {
        cmds->args[i] = ft_strdup(current->content);
        current = current->next;
        i++;
    }
    cmds->args[i] = NULL; 
    cmds->args_count = i;
    *tokens = current;
    return (cmds);
}


void add_redirection(t_command *cmd, t_token_type redirect_type, char *file)
{
    t_redirections *redirection;
    t_redirections *current;
    
    redirection = malloc(sizeof(t_redirections));
    if (!redirection)
        return;
    redirection->type = redirect_type;
    redirection->file = ft_strdup(file);
    redirection->heredoc_fd = -1;
    redirection->next = NULL;
    if (!redirection->file)
        return (free(redirection));
    if (!cmd->redirections)
        cmd->redirections = redirection;
    else
    {
        current = cmd->redirections;
        while (current->next)
            current = current->next;
        current->next = redirection;
    }
}

t_command *create_cmds(t_token **tokens)
{
    t_command *first_cmd;  
    t_command *current_cmd;
    t_command *new_cmd;  
    t_token *current;
    t_token_type redirect_type;

    first_cmd = NULL;  
    current_cmd = NULL;
    new_cmd = NULL; 
    current = *tokens;
    while (current)
    {
        if (current->type == TOKEN_WORD)
        {
            new_cmd = create_command_type_word(&current); 
            if (!new_cmd)
                return NULL;
            if (!first_cmd) 
                first_cmd = new_cmd;
            else if (current_cmd)  
                current_cmd->next = new_cmd;
            current_cmd = new_cmd;
        }
        else if (current->type == TOKEN_PIPE)
        {
            if (!current_cmd)
            {
                fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
                return NULL;
            }
            current = current->next;  
        }
        else if (current->type == TOKEN_REDIRECT_IN || 
                current->type == TOKEN_REDIRECT_OUT ||
                current->type == TOKEN_APPEND ||
                current->type == TOKEN_HEREDOC)
        {
            if (!current_cmd)
            {
                new_cmd = command_init();
                if (!new_cmd)
                    return NULL;
                if (!first_cmd)
                    first_cmd = new_cmd;
                current_cmd = new_cmd;
            }
            redirect_type = current->type;
            current = current->next;
            if (!current || current->type != TOKEN_WORD)
            {
                fprintf(stderr, "minishell: syntax error near unexpected token\n");
                return NULL;
            }
            add_redirection(current_cmd, redirect_type, current->content);
            current = current->next;
        }
        else
            current = current->next;
    }
    return first_cmd;
}

