#include "minishell.h"

int	builtin_cd(t_command *cmd)
{
	char *dir;
	char *home;
	int ret;

	if (cmd->args_count == 1)
	{
		home = getenv("HOME");
		if (!home)
		{
			fprintf(stderr, "minishell: cd: HOME not set\n");
			return (1);
		}
		dir = home;
	}
	else
		dir = cmd->args[1];

	ret = chdir(dir);
	if (ret != 0)
	{
		fprintf(stderr, "minishell: cd: %s: %s\n", dir, strerror(errno));
		return (1);
	}
	return (0);
}

int	builtin_echo(t_command *cmd)
{
	int	i;
	int	print_newline;

	i = 1;
	print_newline = 1;
	if (cmd->args_count > 1 && ft_strcmp(cmd->args[1], "-n") == 0)
	{
		print_newline = 0;
		i = 2;
	}
	while (i < cmd->args_count )
	{
		printf("%s", cmd->args[i]);
		if (i < cmd->args_count - 1)
			printf(" ");
		i++;
	}
	if (print_newline)
		printf("\n");
	return (0);
}

int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	return (1);
}

static int	print_env_vars(char **envp)
{
	int	i;

	i = 0;
	while (envp[i] != NULL)
	{
		printf("declare -x %s\n", envp[i]);
		i++;
	}
	return (0);
}

static int	set_env_var(char *var_name, char *var_value)
{
	if (setenv(var_name, var_value, 1) != 0)
	{
		fprintf(stderr, "minishell: export: unable to set environment variable\n");
		return (1);
	}
	return (0);
}

int	builtin_export(t_command *cmd, char **envp)
{
	char	*equal_sign;
	char	*var_name;
	char	*var_value;
	int		i;

	if (cmd->args_count == 1)
		return (print_env_vars(envp));
	i = 1;
	while (i < cmd->args_count)
	{
		equal_sign = strchr(cmd->args[i], '=');
		if (equal_sign)
		{
			*equal_sign = '\0';
			var_name = cmd->args[i];
			var_value = equal_sign + 1;
			if (set_env_var(var_name, var_value) != 0)
				return (1);
		}
		else
		{
			printf("minishell: export: '%s': not a valid identifier\n", cmd->args[i]);
			return (1);
		}
		i++;
	}
	return (0);
}

int	builtin_unset(t_command *cmd)
{
	int	i;

	if (cmd->args_count == 1)
		return (0);
	i = 1;
	while (i < cmd->args_count)
	{
		if (unsetenv(cmd->args[i]) != 0)
		{
			fprintf(stderr, "minishell: unset: '%s': unable to unset var\n",
				cmd->args[i]);
			return (1);
		}
		i++;
	}
	return (0);
}

int	builtin_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i] != NULL)
	{
		printf("%s\n", envp[i]);
		i++;
	}
	return (0);
}

static int	process_exit_arg(char *arg)
{
	int	exit_status;

	if (!is_number(arg))
	{
		fprintf(stderr, "minishell: exit: %s: numeric argument required\n", arg);
		return (2);
	}
	exit_status = ft_atoi(arg);
	return (exit_status % 256);
}

int	builtin_exit(t_command *cmd)
{
	int	exit_status;

	exit_status = 0;
	if (cmd->args_count > 1)
		exit_status = process_exit_arg(cmd->args[1]);
	printf("exit\n");
	exit(exit_status);
	return (exit_status);
}
