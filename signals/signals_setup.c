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
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
		perror("sigaction SIGPIPE ignore error");
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
	// We still want to ignore SIGPIPE in the parent process, but we'll let
	// child processes handle it normally through the default handler
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
		perror("sigaction SIGPIPE ignore error");
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
	// Make sure we reset SIGPIPE to default as well
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
		perror("sigaction SIGPIPE default error");
}
