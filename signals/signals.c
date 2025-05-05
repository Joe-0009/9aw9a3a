#include "../minishell.h"

void ctrl_d_handle()
{
    printf("exit\n");
    exit(0);
}

void sigint_handler(int sig)
{
    (void)sig;

    g_last_exit_status = 130;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void heredoc_sigint_handler(int sig)
{
    (void)sig;
    
    g_last_exit_status = 130;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);    
}
void	child_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(130);
}
