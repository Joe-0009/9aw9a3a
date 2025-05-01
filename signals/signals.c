#include "../minishell.h"

int	*get_exit_status(void)
{
	return (&g_last_exit_status);
}

static void	handle_sigint_prompt(int sig)
{
	(void)sig;

	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	*get_exit_status() = 130;
}

void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	
	*get_exit_status() = 130;
	write(1, "\n", 1);
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
	close(STDIN_FILENO);
	exit(130);
}

void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_prompt;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction SIGINT interactive error");
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction SIGQUIT interactive error");
}
