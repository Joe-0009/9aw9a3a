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

static int	is_valid_n_flag(char *arg)
{
    int i;

    if (arg[0] != '-' || arg[1] != 'n')
        return (0);
    i = 2;
    while (arg[i])
    {
        if (arg[i] != 'n')
            return (0);
        i++;
    }
    return (1);
}

int	builtin_echo(t_command *cmd)
{
    int	i;
    int	print_newline;

    i = 1;
    print_newline = 1;
    while (i < cmd->args_count && is_valid_n_flag(cmd->args[i]))
    {
        print_newline = 0;
        i++;
    }
    while (i < cmd->args_count)
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
	char *cwd;
    
    cwd = getcwd(NULL, 0);
	if (cwd)
	{
		printf("%s\n", cwd);
		free(cwd);
		return (0);
	}
	return (1);
}

static int	compare_env_vars(const void *a, const void *b)
{
    return (ft_strcmp(*( char **)a, *( char **)b));
}

static int	print_env_vars(char **envp)
{
    int		i;
    int		env_count;
    char	**sorted_env;

    env_count = 0;
    while (envp[env_count])
        env_count++;
    sorted_env = malloc(sizeof(char *) * (env_count + 1));
    if (!sorted_env)
        return (1);
    i = 0;
    while (i < env_count)
    {
        sorted_env[i] = envp[i];
        i++;
    }
    sorted_env[i] = NULL;
    qsort(sorted_env, env_count, sizeof(char *), compare_env_vars);
    i = 0;
    while (sorted_env[i])
    {
        printf("declare -x %s\n", sorted_env[i]);
        i++;
    }
    free(sorted_env);
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

static int	is_valid_identifier(char *str)
{
    int i;

    if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
        return (0);
    i = 1;
    while (str[i])
    {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

int	builtin_export(t_command *cmd, char **envp)
{
    char	*equal_sign;
    char	*var_name;
    char	*var_value;
    int		i;
    int		ret;

    if (cmd->args_count == 1)
        return (print_env_vars(envp));
    i = 1;
    ret = 0;
    while (i < cmd->args_count)
    {
        equal_sign = strchr(cmd->args[i], '=');
        if (equal_sign)
        {
            var_name = ft_substr(cmd->args[i], 0, equal_sign - cmd->args[i]);
            var_value = ft_strdup(equal_sign + 1);
            if (!var_name || !var_value)
            {
                fprintf(stderr, "minishell: export: memory allocation error\n");
                return (1);
            }
            if (!is_valid_identifier(var_name))
            {
                fprintf(stderr, "minishell: export: '%s': not a valid identifier\n", var_name);
                ret = 1;
            }
            else if (set_env_var(var_name, var_value) != 0)
                ret = 1;
            free(var_name);
            free(var_value);
        }
        else
        {
            if (!is_valid_identifier(cmd->args[i]))
            {
                fprintf(stderr, "minishell: export: '%s': not a valid identifier\n", cmd->args[i]);
                ret = 1;
            }
        }
        i++;
    }
    return (ret);
}

int	builtin_unset(t_command *cmd)
{
    int	i;
    int	ret;

    if (cmd->args_count == 1)
        return (0);
    i = 1;
    ret = 0;
    while (i < cmd->args_count)
    {
        if (!is_valid_identifier(cmd->args[i]))
        {
            fprintf(stderr, "minishell: unset: '%s': not a valid identifier\n", cmd->args[i]);
            ret = 1;
        }
        else if (unsetenv(cmd->args[i]) != 0)
        {
            fprintf(stderr, "minishell: unset: '%s': unable to unset var\n", cmd->args[i]);
            ret = 1;
        }
        i++;
    }
    return (ret);
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

    printf("exit\n");
    if (cmd->args_count > 2)
    {
        fprintf(stderr, "minishell: exit: too many arguments\n");
        return (1);
    }
    if (cmd->args_count > 1)
        exit_status = process_exit_arg(cmd->args[1]);
    else
        exit_status = 0;
    exit(exit_status);
}
