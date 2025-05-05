int	wait_for_children(void)
{
	pid_t	wpid;
	int		status;
	int		last_status;
	int		pipefail;

	status = 0;
	last_status = 0;
	pipefail = 0;
	while ((wpid = waitpid(-1, &status, 0)) > 0)
	{
		if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGPIPE)
				pipefail = 1; // Mark that we had a broken pipe, but continue
			else
			{
				last_status = 128 + WTERMSIG(status);
				if (WTERMSIG(status) == SIGQUIT && g_last_exit_status != 131)
					write(STDERR_FILENO, "Quit: 3\n", 9);
				if (WTERMSIG(status) == SIGINT)
					write(STDOUT_FILENO, "\n", 1);
			}
		}
		else if (WIFEXITED(status))
		{
			// When not dealing with broken pipe errors, capture the exit status
			if (!pipefail || WEXITSTATUS(status) != 0)
				last_status = WEXITSTATUS(status);
		}
	}
	return (last_status);
}