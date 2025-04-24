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

static void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	*get_exit_status() = 130;
	write(1, "\n", 1);
	close(STDIN_FILENO);
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

void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_heredoc;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction SIGINT heredoc error");
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction SIGQUIT heredoc error");
}

void	setup_exec_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction SIGINT ignore error");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction SIGQUIT ignore error");
}

void	set_sigint_default(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction SIGINT default error");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		perror("sigaction SIGQUIT default error");
}
