#include "../minishell.h"

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
