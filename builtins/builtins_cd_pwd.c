#include "../minishell.h"

int builtin_cd(t_command *cmd, t_env **env)
{
    int ret;
    char (*old_pwd), (*new_pwd), (*home), (*dir);

    old_pwd = getcwd(NULL, 0);
	if (cmd->args_count > 2)
    {
        if (old_pwd)
            free(old_pwd);
		return (fprintf(stderr, "minishell: cd: too many arguments\n"), 1);
    }
	if (cmd->args_count == 1)
	{
		home = find_env_node(*env, "HOME")->value;
		if (!home)
        {
            if (old_pwd)
                free(old_pwd);
			return (fprintf(stderr, "minishell: cd: HOME not set\n"), 1);
        }
		dir = home;
	}
	else
		dir = cmd->args[1];
	ret = chdir(dir);
	if (ret != 0)
    {
        if (old_pwd)
            free(old_pwd);
        return (fprintf(stderr, "minishell: cd: %s: %s\n", dir, strerror(errno)), 1);
    }
    new_pwd = getcwd(NULL, 0);
    if (new_pwd)
    {
        add_or_update_env(env, "PWD", new_pwd);
        if (old_pwd)
			add_or_update_env(env, "OLDPWD", old_pwd);
        free(new_pwd);
    }
	if (old_pwd)
		free(old_pwd);
	return (0);
}

int builtin_pwd(t_env **env)
{
    char *cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		printf("%s\n", cwd);
		free(cwd);
		return (0);
	}
    else
    {
        // when getcwd fails, i use the PWD environment variable
        t_env *pwd_node = find_env_node(*env, "PWD");
        if (pwd_node && pwd_node->value)
        {
            printf("%s\n", pwd_node->value);
            return (0);
        }
    }

    return (fprintf(stderr, "minishell: pwd: error retrieving current directory\n"), 1);
}
