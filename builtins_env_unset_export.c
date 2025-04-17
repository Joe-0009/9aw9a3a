#include "minishell.h"

int builtin_export(t_command *cmd, char **envp)
{
    char *equal_sign;
    char *var_name;
    char *var_value;
    int i;
    int ret;

    if (cmd->args_count == 1)
        return (print_env_vars(envp));
    i = 1;
    ret = 0;
    while (i < cmd->args_count)
    {
        equal_sign = ft_strchr(cmd->args[i], '=');
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
            else if (setenv(var_name, var_value, 1) != 0)
            {
                fprintf(stderr, "minishell: export: unable to set environment variable\n");
                ret = 1;
            }
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

int builtin_unset(t_command *cmd)
{
    int i;
    int ret;

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

int builtin_env(char **envp)
{
    int i;
    
    i = 0;
    while (envp[i] != NULL)
    {
        printf("%s\n", envp[i]);
        i++;
    }
    return (0);
}