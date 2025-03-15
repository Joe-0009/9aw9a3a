#include "minishell.h"

/**
 * Sets up a heredoc redirection by reading input until delimiter
 * @param delimiter String that marks the end of heredoc input
 * @return File descriptor for reading from heredoc, -1 on error
 */
int setup_heredoc(char *delimiter)
{
    int pipe_fd[2];
    char *line;
    
    if (pipe(pipe_fd) == -1)
    {
        perror("minishell: heredoc pipe error");
        return -1;
    }
    
    printf("heredoc ");
    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            fprintf(stderr, "minishell: warning: heredoc delimited by end-of-file\n");
            safe_close(&pipe_fd[1]);
            break;
        }
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            safe_close(&pipe_fd[1]);
            break;
        }
        write(pipe_fd[1], line, ft_strlen(line));
        write(pipe_fd[1], "\n", 1);
        free(line);
    }
    safe_close(&pipe_fd[1]);
    return pipe_fd[0];
}

/**
 * Sets up input redirection from a file
 * @param file_path Path to input file
 * @return 0 on success, -1 on error
 */
static int setup_redirect_in(char *file_path)
{
    int fd = open(file_path, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "minishell: %s: %s\n", file_path, strerror(errno));
        return -1;
    }
    
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "minishell: input redirection failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

/**
 * Sets up output redirection to a file (truncate or append mode)
 * @param file_path Path to output file
 * @param append_mode Whether to append to file rather than truncate
 * @return 0 on success, -1 on error
 */
static int setup_redirect_out(char *file_path, int append_mode)
{
    int flags = O_WRONLY | O_CREAT;
    flags |= (append_mode ? O_APPEND : O_TRUNC);
    
    int fd = open(file_path, flags, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "minishell: %s: %s\n", file_path, strerror(errno));
        return -1;
    }
    
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "minishell: output redirection failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int setup_redirections(t_command *cmd)
{
    t_redirections *redir;
    int result;

    redir = cmd->redirections;
    result = 0;
    while (redir && result == 0)
    {
        if (redir->type == TOKEN_REDIRECT_IN)
            result = setup_redirect_in(redir->file);
        else if (redir->type == TOKEN_REDIRECT_OUT)
            result = setup_redirect_out(redir->file, 0);
        else if (redir->type == TOKEN_APPEND)
            result = setup_redirect_out(redir->file, 1);
        else if (redir->type == TOKEN_HEREDOC)
        {
                if (dup2(redir->heredoc_fd, STDIN_FILENO) == -1)
                {
                    fprintf(stderr, "minishell: heredoc redirection failed: %s\n", strerror(errno));
                    result = -1;
                }
                safe_close(&redir->heredoc_fd);
        }
        else
        {
            fprintf(stderr, "minishell: unknown redirection type\n");
            result = -1;
        }
        redir = redir->next;
    }
    return result;
}

